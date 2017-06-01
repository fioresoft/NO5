// Internet.h: interface for the CInternet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERNET_H__74BD4863_58BF_11D9_A17B_D22FC881553D__INCLUDED_)
#define AFX_INTERNET_H__74BD4863_58BF_11D9_A17B_D22FC881553D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wininet.h>
#include "handle.h"
#include "winfile.h"
#include "mystring.h"

namespace NO5TL
{

struct InternetHandleTraits
{
	typedef HINTERNET handle_type;

	static BOOL Destroy(HINTERNET h)
	{
		return ::InternetCloseHandle(h);
	}
	static HANDLE InvalidHandle(void)
	{
		return NULL;
	}
};

typedef CHandleT<InternetHandleTraits,TRUE>		CInternet;
typedef CHandleT<InternetHandleTraits,FALSE>	CInternetHandle;


/*
struct IInternetEvents
{
	virtual void OnResolvingName(LPCTSTR name){}
	virtual void OnNameResolved(LPCTSTR name) {}
	virtual void OnConnecting(SOCKADDR *addr,DWORD dwAddrLen) {}
	virtual void OnConnected(SOCKADDR *addr,DWORD dwAddrLen) {}
	virtual void OnClosingConnection(void) {}
	virtual void OnConnectionClosed(void) {}
	virtual void OnHandleCreated(HINTERNET hInet) {}
	virtual void OnHandleClosing(HINTERNET hInet) {}
	virtual void OnReceivingResponse(void) {}
	virtual void OnResponseReceived(DWORD dwLen){}
	virtual void OnSendingRequest(void){}
	virtual void OnRequestSent(DWORD dwLen){}
	virtual void OnRequestComplete(DWORD dwResult,DWORD dwError){}
};
*/

template <class T>
class CHttpFileReaderT
{
	CInternet  m_session;
	CInternet  m_service;
	CInternet  m_http;
	bool       m_bNotify;
	DWORD m_dwBufLen;
public:
	CHttpFileReaderT()
	{
		m_bNotify = true;
		m_dwBufLen = 512;
	}
	void Notify(bool bNotify)
	{
		m_bNotify = bNotify;
	}
	bool Notify(void) const
	{
		return m_bNotify;
	}
	void SetReadBufLen(DWORD dwLen)
	{
		m_dwBufLen = dwLen;
	}
	DWORD GetReadBufLen(void) const
	{
		return m_dwBufLen;
	}

	BOOL InternetOpen(LPCTSTR pszAgent = NULL,DWORD dwAccess = INTERNET_OPEN_TYPE_DIRECT)
	{
		// close any previous ones, in the correct order
		m_http = NULL;
		m_service = NULL;
		m_session = NULL;

		m_session = ::InternetOpen(pszAgent,dwAccess,
			NULL,	// proxy name
			NULL,	// proxy bypass
			0);		// flags - not async in this case

		if(Notify())
			SetCallback();

		return m_session.IsHandleValid();
	}
	BOOL InternetConnect(LPCTSTR server,INTERNET_PORT port = 80)
	{
		BOOL res = FALSE;

		if(m_session.IsHandleValid()){
			m_http = NULL;
			T *pT = static_cast< T* >(this);

			m_service = ::InternetConnect(\
				m_session,
				server,
                port,
				NULL,					// user name
				NULL,					// pw
				INTERNET_SERVICE_HTTP,  // service
				0,                      // flags,
				(DWORD)pT);				// context

			res = m_service.IsHandleValid();
		}
		return res;
	}
	BOOL HttpOpenGet(LPCSTR file,LPCSTR *ppAccept,DWORD dwFlags = INTERNET_FLAG_CACHE_IF_NET_FAIL )
	{
		BOOL res = FALSE;

		if(m_service.IsHandleValid()){
			T *pT = static_cast< T* >(this);

			m_http = ::HttpOpenRequest(m_service,
				NULL,           // verb
				file,           // object name
				NULL,           // http version
				NULL,           // referer
				ppAccept,       // accept type list
				dwFlags,        // flags
				(DWORD)pT);		// context

			res = m_http.IsHandleValid();
		}
		return res;
	}
	BOOL HttpSendReq(void)
	{
		BOOL res = FALSE;

		if(m_http){
			res = ::HttpSendRequest(m_http,
				NULL,	// headers
				0,		// headers len
				NULL,	// optional data
				0);		// opt data len
		}
		return res;
	}
	BOOL HttpQuerySetCookies(CSimpleArray<CString> &cookies)
	{
		BOOL res = FALSE;

		if(m_http){
			char *buf = new char[256];
			DWORD dwBufSize = 255;
			DWORD dwIndex = 0;
			bool bContinue = true;

			ZeroMemory(buf,dwBufSize + 1);

			while(bContinue){
				res = ::HttpQueryInfo(m_http,HTTP_QUERY_SET_COOKIE,
					buf,&dwBufSize,&dwIndex);
				if(res){
					cookies.Add(CString(buf));
					lstrcpy(buf,"");
					if(dwIndex == ERROR_HTTP_HEADER_NOT_FOUND)
						bContinue = false;
				}
				else{
					DWORD dwError = ::GetLastError();

					if(ERROR_INSUFFICIENT_BUFFER == dwError){
						delete []buf;
						buf = new char[dwBufSize + 1];
						ZeroMemory(buf,dwBufSize + 1);
						res = TRUE;
					}
					else{
						bContinue = false;
					}
				}
			}
			delete []buf;
		}
		return res;
	}
	BOOL HttpQueryStatusCode(DWORD &dwCode)
	{
		BOOL res = FALSE;
		
		if(m_http){
			DWORD dwBufLen = sizeof(dwCode);
			DWORD dwIndex = 0;

			res = ::HttpQueryInfo(m_http,HTTP_QUERY_STATUS_CODE|\
				HTTP_QUERY_FLAG_NUMBER,&dwCode,&dwBufLen,&dwIndex);
		}
		return res;
	}
	BOOL HttpQueryContentLen(DWORD &dwCode)
	{
		BOOL res = FALSE;
		
		if(m_http){
			DWORD dwBufLen = sizeof(dwCode);
			DWORD dwIndex = 0;

			res = ::HttpQueryInfo(m_http,HTTP_QUERY_CONTENT_LENGTH|\
				HTTP_QUERY_FLAG_NUMBER,&dwCode,&dwBufLen,&dwIndex);
		}
		return res;
	}

	BOOL InternetRead()
	{
		BOOL res = FALSE;
		T *pT = static_cast<T*>(this);

		if(m_http){
			char *buf = NULL;
			const DWORD dwToRead = GetReadBufLen();
			DWORD dwRead = 0;
			res = TRUE;
			DWORD dwTotalRead = 0;
			DWORD dwTotalToRead = 0;

			res = HttpQueryContentLen(dwTotalToRead);
			ATLASSERT(res);
			res = TRUE;

			buf = new char[dwToRead + 1];
			
			while(res){
				ZeroMemory(buf,dwToRead + 1);
				dwRead = 0;
				res = ::InternetReadFile(m_http,buf,dwToRead,
					&dwRead);
				if(res && dwRead){
					pT->OnDataRead(buf,dwRead,dwTotalToRead);
					dwTotalRead += dwRead;
				}
				if(dwRead == 0)
					break;
			}
			pT->OnTotalDataRead(dwTotalRead,dwTotalToRead);
			delete []buf;
		}
		return res;
	}

	BOOL Download(LPCSTR server,LPCSTR file)
	{
		BOOL res;
		CString copy = server;

		res = (::InternetAttemptConnect(0) == ERROR_SUCCESS);

		if(res && ( res = InternetOpen() ) )
		{
			if(res = InternetConnect(server))
			{
				LPCSTR ppAccept[] = {"*/*",NULL};

				if(res = HttpOpenGet(file,ppAccept,0))
				{
					res = InternetGoOnline(copy.GetBuffer(0),NULL,0);
					copy.ReleaseBuffer();
					if(res && (res = HttpSendReq()))
					{
						res = InternetRead();
					}
				}
			}
		}
		return res;
	}
	static CString GetLastErrorDesc(DWORD dwError = 0)
	{
		CString s;

		if(dwError == 0)
			dwError = ::GetLastError();

		if(dwError != 0)
			s = NO5TL::GetErrorDesc(dwError);
		if(s.IsEmpty() || dwError == ERROR_INTERNET_EXTENDED_ERROR){
			DWORD dwBufLen = 256;
			LPSTR buf = s.GetBuffer(256);
			::InternetGetLastResponseInfo(&dwError,buf,&dwBufLen);
			s.ReleaseBuffer();
		}			
		return s;
	}
	
	// overidable
public:
	void OnDataRead(char *buf,DWORD dwRead,DWORD dwContentLen)
	{
		if(Notify()){
			T *pT = static_cast<T*>(this);
			CString s;
			s.Format("%d bytes read",dwRead);
			pT->OnStatusText(s);
		}

	}
	void OnTotalDataRead(DWORD dwTotalRead,DWORD dwContentLen)
	{
		if(Notify()){
			T *pT = static_cast<T*>(this);
			CString s;
			s.Format("download finished: %d bytes read",dwTotalRead);
			pT->OnStatusText(s);
		}
	}

	void OnStatusText(LPCTSTR text)
	{
		//
	}
	
	// wininet callbacks

	void OnResolvingName(LPCTSTR name)
	{
		T *pT = static_cast<T*>(this);
		CString s("resolving address: ");
		s += name;
		pT->OnStatusText(s);
	}
	void OnNameResolved(LPCTSTR name)
	{
		T *pT = static_cast<T*>(this);
		CString s("address resolved: ");
		s += name;
		pT->OnStatusText(s);
	}
	void OnConnecting(SOCKADDR *addr,DWORD dwAddrLen)
	{
		T *pT = static_cast<T*>(this);
		pT->OnStatusText("connecting...");
	}
	void OnConnected(LPCSTR ip)
	{
		T *pT = static_cast<T*>(this);
		CString s("connected to: ");
		s += ip;
		pT->OnStatusText(s);
	}
	void OnClosingConnection(void)
	{
		T *pT = static_cast<T*>(this);
		pT->OnStatusText("closing connection...");
	}
	void OnConnectionClosed(void)
	{
		T *pT = static_cast<T*>(this);
		pT->OnStatusText("connection closed");
	}
	void OnHandleCreated(HINTERNET hInet) {}
	void OnHandleClosing(HINTERNET hInet) {}
	void OnReceivingResponse(void)
	{
		T *pT = static_cast<T*>(this);
		pT->OnStatusText("receiving response...");
	}
	void OnResponseReceived(DWORD dwLen)
	{
		T *pT = static_cast<T*>(this);
		CString s;
		s.Format("response received: %d bytes",dwLen);
		pT->OnStatusText(s);
	}
	void OnSendingRequest(void)
	{
		T *pT = static_cast<T*>(this);
		pT->OnStatusText("sending request...");
	}
	void OnRequestSent(DWORD dwLen)
	{
		T *pT = static_cast<T*>(this);
		CString s;
		s.Format("request sent: %d bytes",dwLen);
		pT->OnStatusText(s);
	}
	void OnRequestComplete(DWORD dwResult,DWORD dwError){}
private:
	BOOL SetCallback(void)
	{
		BOOL res = FALSE;

		if(m_session){
			::InternetSetStatusCallback(m_session,InetStatusCallback);
			res = TRUE;
		}
		return res;
	}
	static void CALLBACK InetStatusCallback(HINTERNET hInternet,
		DWORD dwContext,DWORD dwStatus,LPVOID pStatusInfo,
		DWORD dwInfoLen)
	{
		T *pT = reinterpret_cast<T*>(dwContext);

		if(!pT)
			return;
		if(!pT->Notify())
			return;

		switch(dwStatus){
			case INTERNET_STATUS_CLOSING_CONNECTION :
				pT->OnClosingConnection();
				break;
			case INTERNET_STATUS_CONNECTED_TO_SERVER :
				pT->OnConnected((char *)pStatusInfo);
				break;
			case INTERNET_STATUS_CONNECTING_TO_SERVER:
				pT->OnConnecting((SOCKADDR *)pStatusInfo,dwInfoLen);
				break;
			case INTERNET_STATUS_CONNECTION_CLOSED:
				pT->OnConnectionClosed();
				break;
			case INTERNET_STATUS_HANDLE_CLOSING:
				pT->OnHandleClosing(hInternet);
				break;
			case INTERNET_STATUS_HANDLE_CREATED:
				pT->OnHandleCreated(hInternet);
				break;
			case INTERNET_STATUS_RESOLVING_NAME:
				pT->OnResolvingName((LPCTSTR)pStatusInfo);
				break;
			case INTERNET_STATUS_NAME_RESOLVED:
				pT->OnNameResolved((LPCTSTR)pStatusInfo);
				break;
			case INTERNET_STATUS_RECEIVING_RESPONSE:
				pT->OnReceivingResponse();
				break;
			case INTERNET_STATUS_RESPONSE_RECEIVED:
				pT->OnResponseReceived(*(DWORD *)pStatusInfo);
				break;
			case INTERNET_STATUS_SENDING_REQUEST:
				pT->OnSendingRequest();
				break;
			case INTERNET_STATUS_REQUEST_SENT:
				pT->OnRequestSent(*(DWORD *)pStatusInfo);
				break;
			case INTERNET_STATUS_REQUEST_COMPLETE:
				pT->OnRequestComplete(((INTERNET_ASYNC_RESULT *)pStatusInfo)->dwResult,
					((INTERNET_ASYNC_RESULT *)pStatusInfo)->dwError);
				break;
			default:
				break;
		}
	}
};

template <class T>
class CHttpFileSaverT : public CHttpFileReaderT<T>
{
	typedef CHttpFileReaderT<T> _BaseClass;

	CWinFile m_file;
	CWindow m_wndStatus;
public:
	BOOL Download(LPCSTR server,LPCSTR file,LPCSTR pSaveAs,HWND hStatus)
	{
		BOOL res;

		res = m_file.Create(pSaveAs,CREATE_ALWAYS,GENERIC_WRITE);
		if(res){
			m_wndStatus = hStatus;
			res = _BaseClass::Download(server,file);
			m_file.CloseHandle();
		}
		return res;
	}
	// overide
	void OnDataRead(char *buf,DWORD dwRead,DWORD dwContentLen)
	{
		DWORD written = 0;
		BOOL res;
		
		_BaseClass::OnDataRead(buf,dwRead,dwContentLen);
		res = m_file.Write(buf,dwRead,&written);
		ATLASSERT(res && written == dwRead);
	}
	void OnStatusText(LPCTSTR text)
	{
		if(m_wndStatus.IsWindow())
			m_wndStatus.SetWindowText(text);
	}
};

class CHttpFileSaver : public CHttpFileSaverT<CHttpFileSaver>
{
	//
};

} // NO5TL

#endif // !defined(AFX_INTERNET_H__74BD4863_58BF_11D9_A17B_D22FC881553D__INCLUDED_)
