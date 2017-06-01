// HttpClient.h: interface for the CHttpClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPCLIENT_H__6300B8E3_1F40_11D9_A17B_D91F912C6947__INCLUDED_)
#define AFX_HTTPCLIENT_H__6300B8E3_1F40_11D9_A17B_D91F912C6947__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "databuffer.h"
#include "eventsocket.h"

namespace NO5TL
{


struct IHttpClientEvents : public ISocketEvents
{
private:
	virtual void OnSockRead(int error) { ATLASSERT(0);}
public:
	virtual void OnDataRead(char *buf,int len) = 0;
};

template <class TSocket>
class CHttpClient:protected ISocketEvents
{
private:
	CString m_verb;
	CString m_file;
	CString m_host;
	CString m_accept;
	CString m_UserAgent;
	bool m_KeepAlive;
	CDataBuffer m_content;
	CDataBuffer m_response;
	LPBYTE m_pBuf;
	const DWORD m_dwBufSize;
	TSocket m_socket;
	int m_iHttpVer;		// 0,1 or -1 to not add version
	IHttpClientEvents *m_pSink;
	const bool m_bStoreResp;
	bool m_bOnClose;
	//
	void MakeHeader(CString &res)
	{
		CString version;

		res = m_verb;
		// file
		res += " ";
		res += m_file;
		res += " ";
		if(m_iHttpVer >= 0){
			version.Format("HTTP/1.%1d",m_iHttpVer);
			res += version;
		}
		res += "\r\n";

		// host
		if(!m_host.IsEmpty()){
			res += "Host: ";
			res += m_host;
			res += "\r\n";
		}
		// accept
		if(!m_accept.IsEmpty()){
			res += "Accept: ";
			res += m_accept;
			res += "\r\n";
		}
		// user-agent
		if(!m_UserAgent.IsEmpty()){
			res += "User-Agent: ";
			res += m_UserAgent;
			res += "\r\n";
		}
		// keep-alive
		if(m_KeepAlive)
			res += "Connection: Keep-Alive\r\n";
		else
			res += "Connection: close\r\n";
		if(m_content.GetDataLen() > 0){
			res += "Content-Length: ";
			res.Append(m_content.GetDataLen());
			res += "\r\n";
		}
		res += "\r\n";
	}
public:
	CHttpClient(IHttpClientEvents *pSink,
					DWORD dwBufSize = 1024,
					bool bStoreResp = true):\
		m_socket((ISocketEvents *)this),
		m_pSink(pSink),
		m_dwBufSize(dwBufSize),
		m_bStoreResp(bStoreResp)
	{
		// defaults
		m_verb = "GET";
		m_file = "/";
		m_KeepAlive = false;
		m_accept = "*/*";
		m_iHttpVer = 0;
		//m_UserAgent = "Mozilla/4.0 (compatible; MSIE 5.0; Windows 98; DigExt)";
		m_UserAgent = "Microsoft Internet Explorer";
		m_pBuf = new BYTE[m_dwBufSize];
		m_bOnClose = false;
	}
	virtual ~CHttpClient()
	{
		if(m_pBuf)
			delete []m_pBuf;
	}

	BOOL InitClient(SOCKADDR_IN &sa)
	{
		BOOL res = FALSE;

		if(m_socket.CreateSocket())
		{
			//m_content.Destroy();
			m_response.Destroy();
			res = m_socket.AsyncSelect(FD_READ|FD_WRITE|FD_CONNECT|\
				FD_CLOSE);
			if(res)
			{
				res = m_socket.Connect(&sa);
			}
		}
		return res;
	}
	BOOL InitClient(LPCSTR server,int port)
	{
		BOOL res = FALSE;

		if(m_socket.CreateSocket())
		{
			m_response.Destroy();
			m_bOnClose = false;
			res = m_socket.AsyncSelect(FD_READ|FD_WRITE|FD_CONNECT|\
				FD_CLOSE);
			if(res)
			{
				res = m_socket.Connect(server,port);
			}
		}
		return res;
	}

	void EndClient(bool bAbrupt)
	{
		if(m_socket.IsHandleValid()){
			if(m_socket.IsConnecting()){
				m_socket.AsyncSelect(0);
				m_socket.CloseSocket();
			}
			if(m_socket.IsConnected()){
				if(bAbrupt){
					m_socket.AsyncSelect(0);
					m_socket.Shutdown(SD_BOTH);
					m_socket.CloseSocket();
				}
				else{
					m_socket.Shutdown(SD_SEND);
				}
			}
			else
				m_socket.CloseSocket();
		}
	}
	const TSocket & GetSocket(void) const
	{
		return m_socket;
	}
	BOOL GetRemoteAddress(CSocketAddress &addr)
	{
		BOOL res = FALSE;

		if(m_socket.IsConnected()){
			res = m_socket.GetRemoteAddress(addr);
		}
		return res;
	}
	bool IsBusy(void) const
	{
		bool res = false;

		if(m_socket.IsHandleValid()){
			res = m_socket.IsConnecting() || m_socket.IsConnected();
		}
		return res;
	}
	void SetVerb(LPCTSTR verb)
	{
		ATLASSERT(!m_socket.IsHandleValid());
		m_verb = verb;
		m_verb.MakeUpper();
	}
	void SetFile(LPCTSTR file)
	{
		ATLASSERT(!m_socket.IsHandleValid());
		m_file = file;
	}
	void SetHost(LPCTSTR host)
	{
		ATLASSERT(!m_socket.IsHandleValid());
		m_host = host;
	}
	void SetKeepAlive(bool KeepAlive)
	{
		ATLASSERT(!m_socket.IsHandleValid());
		m_KeepAlive = KeepAlive;
	}
	void SetUserAgent(LPCSTR UserAgent)
	{
		ATLASSERT(!m_socket.IsHandleValid());
		m_UserAgent = UserAgent;
	}
	void SetHttpVer(int ver)
	{
		ATLASSERT(!m_socket.IsHandleValid());
		m_iHttpVer = ver;
	}
	void AddContent(LPVOID pData,DWORD dwLen)
	{
		ATLASSERT(!m_socket.IsHandleValid());
		m_content.Add(pData,dwLen);
	}
	void AddContent(LPCSTR s)
	{
		ATLASSERT(!m_socket.IsHandleValid());
		m_content.Add(s);
	}
	CDataBuffer * GetRespBuf(void)
	{
		return &m_response;
	}
protected:
	// ISocketEvents
	virtual void OnSockConnecting(void)
	{
		m_pSink->OnSockConnecting();
	}
	virtual void OnSockConnect(int error)
	{
		m_pSink->OnSockConnect(error);
	}
	virtual void OnSockWrite(int error)
	{
		CString header;
		CDataBuffer buf;
		int sent;

		if(!error)
		{
			MakeHeader(header);
			buf.Add((LPCSTR)header);
			if(m_content.GetDataLen())
			{
				buf.Add(m_content.GetData(),m_content.GetDataLen());
			}
			sent = m_socket.Send(buf.GetData(),buf.GetDataLen());
			ATLASSERT(sent == (int)buf.GetDataLen());
		}
		m_pSink->OnSockWrite(error);
	}
	virtual void OnSockRead(int error)
	{
		int read;

		if(!error){
			ZeroMemory(m_pBuf,m_dwBufSize);
			read = m_socket.Recv((char *)m_pBuf,m_dwBufSize,0);
			if(read == SOCKET_ERROR){
				ATLASSERT(0);
			}
			else if(read > 0){
				if(m_bStoreResp)
					m_response.Add(m_pBuf,read);
				m_pSink->OnDataRead((char *)m_pBuf,read);
			}
			else if(read == 0){
				if(m_bOnClose){
					m_socket.CloseSocket();
					m_bOnClose = false;
				}
			}
		}
	}
	virtual void OnSockClose(int error)
	{
		m_socket.AsyncSelect(0);
		// try to read more data
		ZeroMemory(m_pBuf,m_dwBufSize);
		int read;
		
		do {
			read = m_socket.Recv((char *)m_pBuf,m_dwBufSize,0);
			
			if(read > 0){
				if(m_bStoreResp)
					m_response.Add(m_pBuf,read);
				m_pSink->OnDataRead((char *)m_pBuf,read);
			}
		}while(read > 0);
		// its possible to receive an FD_CLOSE and to have FD_READ on the message queue
		// if we destroy the socket, the socket handler will not find the socket in the list
		// and it will assert. instead of removing the assert from 
		// CWinSocketHandler::_OnSocketEvent ( in the case of window sockets ) we will not
		// destroy the socket and just mark we received this message.
		//m_socket.CloseSocket();
		m_bOnClose = true;
		m_pSink->OnSockClose(error);
		m_content.Destroy();
	}
	virtual void OnSockError(int error)
	{
		m_socket.AsyncSelect(0);
		m_socket.CloseSocket();
		m_pSink->OnSockError(error);
	}
	virtual void OnSockConnectTimeout(void)
	{
		m_socket.AsyncSelect(0);
		m_socket.CloseSocket();
		m_pSink->OnSockConnectTimeout();
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

} // NO5TL

#endif // !defined(AFX_HTTPCLIENT_H__6300B8E3_1F40_11D9_A17B_D91F912C6947__INCLUDED_)
