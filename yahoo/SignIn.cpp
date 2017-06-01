#include "stdafx.h"
#include <no5tl\mystring.h>
#include <no5tl\winsocketwithhandler.h>
#include <no5tl\httpclient.h>
#include "signin.h"
using namespace NO5TL;
using namespace NO5YAHOO;

// retorna a string Y=v=...; T=z=...
CString CYahooCookies::MakeCookieString(void)
{
	CString res = sY;
	int pos = sT.Find('&',0);
		
	res += " ";
	//res.append(sT,0,pos);
	StringAppend(res,sT,0,pos);
	return res;
}
CString CYahooCookies::MakeDhtmlString(void)
{
	CString res = sY;
	res += " ";
	res += sT;
	return res;
}

template <class TSock>
class CSignInT: protected IHttpClientEvents,public IYahooChatSignIn
{
	CHttpClient<TSock> m_html;
	BOOL m_id;	// bad id
	BOOL m_pw;	// bad pw
	IYahooChatSignInEvents *m_pSink;
	//
	bool Parse(LPCSTR pData,CYahooCookies &cookies)
	{
		CString data(pData);
		int res,start=0,end=0;
		bool ok = true;

		while(true){
			res = data.Find("Set-Cookie:",end);
			if(res != -1){
				res = data.Find("Y=",res + lstrlen("Set-Cookie")-1);
				if(res != -1){
					start = res;
					end = data.Find(";",start + lstrlen("Y=") - 1);
					ATLASSERT(end != -1);
					StringCopyN(cookies.sY,data,start,end - start+1);
				}
				else{
					res = data.Find("T=",res + \
						lstrlen("Set-Cookie")-1);
					if(res != -1){
						start = res;
						end = data.Find(";",start + lstrlen("T=") - 1);
						ATLASSERT(end != -1);
						StringCopyN(cookies.sT,data,start,
							end - start+1);
						ok = true;
					}
					else{
						ok = false;
					}
					break;
				}
			}
			else
				break;
		}
		if(cookies.sT.IsEmpty() || cookies.sY.IsEmpty())
			ok = false;
		if(!ok){
			res = data.Find("Invalid Yahoo ID");
			if(res != -1)
				m_id = true;
			res = data.Find("Invalid Password");
			if(res != -1)
				m_pw = true;
		}
		return ok;
	}
public:
	CSignInT(IYahooChatSignInEvents *pSink):m_html(this),
		m_pSink(pSink)
	{
		m_id = m_pw = FALSE;
	}
	// host = "login.yahoo.com"
	virtual BOOL SignIn(SOCKADDR_IN &addr,LPCTSTR host,LPCTSTR name,
		LPCTSTR pw)
	{
		CString data;
		data.Format("&login=%s&passwd=%s",name,pw);
		m_html.SetHost(host);
		m_html.AddContent((LPCSTR)data);
		m_html.SetVerb("POST");
		m_html.SetFile("/config/login?");
		m_html.SetHttpVer(0);
		
		return m_html.InitClient(addr);
	}
	virtual BOOL SignIn(LPCSTR server,int port,LPCSTR host,LPCSTR name,LPCSTR pw)
	{
		CString data;
		data.Format("&login=%s&passwd=%s",name,pw);
		m_html.SetHost(host);
		m_html.AddContent((LPCSTR)data);
		m_html.SetVerb("POST");
		m_html.SetFile("/config/login?");
		m_html.SetHttpVer(0);
		
		return m_html.InitClient(server,port);
	}

	virtual BOOL IsBadID(void) const
	{
		return m_id;
	}
	virtual BOOL IsBadPW(void) const
	{
		return m_pw;
	}
protected: 
	// IHttpClientEvents
	virtual void OnDataRead(char *buf,int len)
	{
		m_pSink->OnDataRead(buf,len);
	}
	virtual void OnSockConnecting(void)
	{
		m_pSink->OnSockConnecting();
	}
	virtual void OnSockConnect(int error)
	{
		m_pSink->OnSockConnect(error);
	}
	virtual void OnSockClose(int error)
	{
		CYahooCookies cookies;
		CDataBuffer *buf = m_html.GetRespBuf();
		bool res;
		
		buf->AddNull();
		res = Parse((LPCSTR)buf->GetData(),cookies);

		if(!res){
			m_pSink->OnSignInFailure(IsBadPW(),IsBadID());
		}
		else{
			m_pSink->OnSignIn(cookies);
		}
		m_pSink->OnSockClose(error);
	}
	virtual void OnSockError(int error)
	{
		m_pSink->OnSockError(error);
	}
	virtual void OnSockResolvingAddress(void)
	{
		m_pSink->OnSockResolvingAddress();
	}
	virtual void OnSockAddressResolved(int error)
	{
		m_pSink->OnSockAddressResolved(error);
	}
};

/*********** IYahooSignIn **************/
BOOL NO5YAHOO::SignInCreate(IYahooChatSignIn **ppSignIn,
								IYahooChatSignInEvents *pSink)
{
	*ppSignIn = (IYahooChatSignIn *) new CSignInT<CSimpleSocket>(pSink);
	return *ppSignIn != NULL ? TRUE : FALSE;
}

void NO5YAHOO::SignInDestroy(IYahooChatSignIn **ppSignIn)
{
	CSignInT<CSimpleSocket> *p = (CSignInT<CSimpleSocket> *)(*ppSignIn);
	delete p;
	*ppSignIn = NULL;
}

