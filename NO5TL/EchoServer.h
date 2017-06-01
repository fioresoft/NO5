// EchoServer.h: interface for the CEchoServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECHOSERVER_H__53EE6064_20DE_11D9_A17B_A72596420244__INCLUDED_)
#define AFX_ECHOSERVER_H__53EE6064_20DE_11D9_A17B_A72596420244__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace NO5TL
{
namespace Net
{

using namespace NO5TL;

template <class TSocket>
struct IEchoServerEvents
{
	virtual void OnConnectionAccepted(TSocket *pSocket,
		const CSocketAddress &addr) = 0;
	virtual void OnConnectionClosed(TSocket *pSocket,int error) = 0;
	virtual void OnDataRead(TSocket *pSocket,char *buf,int read) = 0;
	virtual void OnDataSent(TSocket *pSocket,char *buf,int sent) = 0;
	virtual void OnSockError(TSocket *pSocket,int error) = 0;
};

// used to notify the server socket when a connection is being
// closed and therefore deleted
template <class TSocket>
struct IEchoServerNotify
{
	virtual void ClosingAcceptedSocket(TSocket *pSocket) = 0;
};

template <class TSocket>
class CAcceptedSocket : public TSocket, protected ISocketEvents
{
	enum{
		BUFSIZE = 512,
	};
	// diable this
	virtual BOOL CreateSocket(int type = SOCK_STREAM,int proto = 0)
	{
		ATLASSERT(0);
		return FALSE;
	}
	char m_buf[BUFSIZE + 1];
	typedef IEchoServerNotify<TSocket>  Notify;
	typedef IEchoServerEvents<TSocket>	ISink;
	Notify *m_pServer;
	ISink *m_pSink;
public:
	CAcceptedSocket(ISink *pSink):TSocket((ISocketEvents *)this),
		m_pSink(pSink)
	{
		ZeroMemory(m_buf,BUFSIZE + 1);
		m_pServer = NULL;
	}
	void SetServerNotify(Notify *pServer)
	{
		m_pServer = pServer;
	}
protected:
	virtual void OnSockRead(int error)
	{
		if(!error){
			int res;
			
			ZeroMemory(m_buf,BUFSIZE + 1);
			res = Recv(m_buf,BUFSIZE);
			if(res && res != SOCKET_ERROR){
				m_pSink->OnDataRead(this,m_buf,res);
				res = Send(m_buf,lstrlen(m_buf));
				if(res != SOCKET_ERROR)
					m_pSink->OnDataSent(this,m_buf,res);
			}
			if(res == SOCKET_ERROR){
				m_pSink->OnSockError(this,CSocket::GetLastError());
			}
		}
	}
	virtual void OnSockClose(int error)
	{
		BOOL res;

		res = Shutdown();
		ATLASSERT(res);
		res = AsyncSelect(0);
		ATLASSERT(res);
		res = CloseSocket();
		ATLASSERT(res);

		if(m_pServer)
			m_pServer->ClosingAcceptedSocket(this);
		m_pSink->OnConnectionClosed(this,error);
		delete this;
	}
	virtual void OnSockConnect(int error)
	{
		ATLASSERT(0);
	}
};

template <class TSocket, class TAcceptedSock = CAcceptedSocket>
class CEchoServer : public TSocket, protected ISocketEvents ,
	public IEchoServerNotify<TSocket>
{
	typedef IEchoServerEvents<TSocket> ISink;
	CSimpleValArray<TSocket *> m_connections;
	ISink *m_pSink;
public:
	CEchoServer(ISink *pSink):TSocket(this),m_pSink(pSink)
	{
		//
	}
	virtual ~CEchoServer()
	{
		//
	}
	BOOL StartServer(void)
	{
		CService service;
		BOOL res;

		res = service.GetByName("echo","tcp");
		if(res){
			res = CreateSocket();
			if(res){
				res = BindToAny(service.GetPort());
				if(res){
					res = AsyncSelect(FD_ACCEPT);
					if(res){
						res = Listen();
					}
				}
			}
		}
		return res;
	}
	BOOL EndServer(void)
	{
		BOOL res = AsyncSelect(0);

		if(res){
			// close open connections
			for(int i=0;i<m_connections.GetSize();i++){
				m_connections[i]->AsyncSelect(0);
				m_connections[i]->CloseSocket();
				delete m_connections[i];
			}
			res = CloseSocket();
		}
		return res;
	}
	// IEchoServerNotify
	virtual void ClosingAcceptedSocket(TSocket *p)
	{
		m_connections.Remove(p);
	}
protected:
	virtual void OnSockAccept(int error)
	{
		if(!error){
			CSocketAddress addr;
			SOCKET s = Accept(addr);
			if(s != INVALID_SOCKET){
				TAcceptedSock *p = new TAcceptedSock(m_pSink);
				ATLASSERT(p);
				if(p && p->AttachAcceptedSocket(s)){
					if(p->AsyncSelect(FD_READ|FD_WRITE|FD_CLOSE)){
						p->SetServerNotify(this);
						m_connections.Add(p);
						m_pSink->OnConnectionAccepted(p,addr);
					}
				}
			}
		}
	}
	virtual void OnSockConnect(int error)
	{
		ATLASSERT(0);
	}
};

} // NO5TL::Net

} // NO5TL

#endif // !defined(AFX_ECHOSERVER_H__53EE6064_20DE_11D9_A17B_A72596420244__INCLUDED_)
