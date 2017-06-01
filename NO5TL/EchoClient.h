// EchoClient.h: interface for the CEchoClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECHOCLIENT_H__53EE6061_20DE_11D9_A17B_A72596420244__INCLUDED_)
#define AFX_ECHOCLIENT_H__53EE6061_20DE_11D9_A17B_A72596420244__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace NO5TL
{
namespace Net
{

struct IEchoClientEvents : public ISocketEvents
{
	virtual void OnEchoReply(LPCSTR msg) = 0;
};

template <class TSocket>
class CEchoClientT :protected ISocketEvents
{
	enum {
		BUFSIZE = 512,
	};
	char m_buf[BUFSIZE + 1];
	IEchoClientEvents *m_pSink;
protected:
	TSocket m_socket;
public:
	CEchoClientT(IEchoClientEvents *pSink):m_socket(this),m_pSink(pSink)
	{
		ZeroMemory(m_buf,BUFSIZE + 1);
		ATLASSERT(m_pSink);
	}
	virtual ~CEchoClientT()
	{
		
	}
	BOOL InitClient(CSocketAddress &addr)
	{
		BOOL res;

		ATLASSERT(!m_socket.IsHandleValid());
		res = m_socket.CreateSocket();
		if(res){
			res = m_socket.AsyncSelect(FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE);
			if(res){
				res = m_socket.Connect(&addr);
			}
		}
		return res;
	}
	BOOL EndClient(void)
	{
		BOOL res = TRUE;

		if(m_socket.IsConnecting()){
			res = m_socket.AsyncSelect(0);
			ATLASSERT(res);
			// we close it even if previous failed
			res = m_socket.CloseSocket();
		}
		if(m_socket.IsConnected()){
			res = m_socket.Shutdown(SD_SEND);
		}
		return res;
	}

	BOOL SendLine(LPCSTR msg)
	{
		BOOL res = FALSE;

		ATLASSERT(m_socket.IsHandleValid());
		ATLASSERT(m_socket.IsConnected() && m_socket.CanWrite());
		if(m_socket.IsHandleValid() && m_socket.IsConnected() && m_socket.CanWrite()){
			res = ( m_socket.SendString(msg) == lstrlen(msg));
		}
		return res;
	}
	BOOL IsConnected(void) const
	{
		return m_socket.IsConnected() ? TRUE : FALSE;
	}
	BOOL IsConnecting(void) const
	{
		return m_socket.IsConnecting() ? TRUE : FALSE;
	}
protected:
	virtual void OnSockRead(int error)
	{
		m_pSink->OnSockRead(error);
		if(!error){
			int rd;

			ZeroMemory(m_buf,BUFSIZE + 1);
			rd = m_socket.Recv(m_buf,BUFSIZE);
			if(rd && rd != SOCKET_ERROR)
				m_pSink->OnEchoReply(m_buf);
			else if(rd == SOCKET_ERROR)
				OnSockError(m_socket.GetLastError());
		}
	}
	virtual void OnSockClose(int error)
	{
		m_pSink->OnSockClose(error);
		m_socket.AsyncSelect(0);
		m_socket.CloseSocket();
	}
	virtual void OnSockConnecting(void)
	{
		m_pSink->OnSockConnecting();
	}
	virtual void OnSockConnect(int error)
	{
		m_pSink->OnSockConnect(error);
		if(error){
			m_socket.AsyncSelect(0);
			m_socket.CloseSocket();
		}
	}
	virtual void OnSockError(int error)
	{
		m_pSink->OnSockError(error);
	}
};

} // NO5TL::Net
} // NO5TL

#endif // !defined(AFX_ECHOCLIENT_H__53EE6061_20DE_11D9_A17B_A72596420244__INCLUDED_)
