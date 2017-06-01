// AsyncSocket.h: interface for the CAsyncSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASYNCSOCKET_H__BF2569E1_2882_11D9_A17B_C890EEE07877__INCLUDED_)
#define AFX_ASYNCSOCKET_H__BF2569E1_2882_11D9_A17B_C890EEE07877__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "socket.h"

namespace NO5TL
{

struct ISocketEvents
{
	virtual void OnSockError(int error){}
	virtual void OnSockRead(int error){}
	virtual void OnSockWrite(int error){}
	virtual void OnSockConnect(int error){}
	virtual void OnSockClose(int error){}
	virtual void OnSockAccept(int error){}
	virtual void OnSockOutOfBand(int error){}
	virtual void OnSockConnecting(void){}
	virtual void OnSockConnectTimeout(void){}
	virtual void OnSockResolvingAddress(void){}
	virtual void OnSockAddressResolved(int error){}
};

/** 
	base class used for non-blocking sockets. the actual event
	mechanism is implemented in derived classes
*/

class CAsyncSocketBase : public CSocket
{
protected:
	volatile bool m_canwrite;
	volatile bool m_canread;
	volatile bool m_connected;
	volatile bool m_canaccept;
	volatile bool m_connecting;
	volatile bool m_resolving;
	volatile bool m_resolved;
	ISocketEvents *m_pSink;
public:
	CAsyncSocketBase(ISocketEvents *pSink):m_pSink(pSink)
	{
		m_canwrite = false;
		m_canread = false;
		m_connected = false;
		m_canaccept = false;
		m_connecting = false;
		m_resolving = false;
		m_resolved = false;
		ATLASSERT(m_pSink);
	}
	virtual ~CAsyncSocketBase()
	{
		// virtual functions dont work in destructor so the base
		// class destructor would call CSocket::CloseSocket
		// and not CAsyncSocket::CloseSocket. So we explicitly
		// close it here
		BOOL res = CloseSocket();
		ATLASSERT(res);
	}
	virtual BOOL AsyncSelect(long events = FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE) = 0;
	
	BOOL CanRead(void) const
	{
		return m_canread ? TRUE : FALSE;
	}
	BOOL CanWrite(void) const
	{
		return m_canwrite ? TRUE : FALSE;
	}
	BOOL IsConnected(void) const
	{
		return m_connected ? TRUE : FALSE;
	}
	BOOL IsConnecting(void) const
	{
		return m_connecting ? TRUE : FALSE;
	}
	BOOL CanAccept(void) const
	{
		return m_canaccept ? TRUE : FALSE;
	}
	BOOL IsResolving(void) const
	{
		return m_resolving ? TRUE : FALSE;
	}
	BOOL IsResolved(void) const
	{
		return m_resolved ? TRUE : FALSE;
	}

	virtual int Recv(char *buf,int len,int flags = 0)
	{
		int res;
		
		m_canread = false;
		res = CSocket::Recv(buf,len,flags);

		/*
		if(res == SOCKET_ERROR){
			// Any call to the re-enabling routine, even one that
			// fails, results in re-enabling of recording and
			// signaling for the relevant network event and event
			// object.
			//m_canread = true;
		}
		*/

		return res;
	}
	virtual int Send(char *buf,int len,int flags = 0)
	{
		int res;

		m_canwrite = false;
		res = CSocket::Send(buf,len,flags);
		if(res != SOCKET_ERROR){
			// send will only send more notifications when send
			// fails with error WSAEWOULDBLOCK, so we set the flag
			// here
			m_canwrite = true;
		}
		else if(WSAEWOULDBLOCK != GetLastError()){
			m_canwrite = true;
			// since we do not always check the return value for this
			// error code ...
			ATLASSERT(0);
		}
		else {
			// an FD_WRITE event will be sent when we can write again,
			// for now, we cant
		}
		return res;
	}
	virtual BOOL Connect(SOCKADDR *addr,int addrlen)
	{
		int res;
		BOOL rv = TRUE;

		ATLASSERT(IsHandleValid());
		m_connecting = true;
		m_pSink->OnSockConnecting();
		res = CSocket::Connect(addr,addrlen);
		if(res == SOCKET_ERROR && WSAEWOULDBLOCK != GetLastError()){
			m_connecting = false;
			rv = FALSE;
			_OnError(GetLastError());
		}
		ATLASSERT(res != 0);
		return rv;
	}
	virtual BOOL Connect(SOCKADDR_IN *addr)
	{
		return Connect((SOCKADDR *)addr,sizeof(SOCKADDR_IN));
	}
	virtual BOOL Connect(LPCSTR server,u_short port) = 0;

	virtual BOOL AttachAcceptedSocket(SOCKET s)
	{
		BOOL res = CSocket::AttachAcceptedSocket(s);
		if(res){
			m_connected = true;
		}
		return res;
	}
	virtual BOOL CloseSocket(void)
	{
		BOOL res = CSocket::CloseSocket();

		if(res){
			m_connecting = false;
			m_connected = false;
			m_canread = false;
			m_canwrite = false;
			m_canaccept = false;
			m_resolving = false;
			m_resolved = false;
		}
		return res;
	}
	virtual BOOL SetConnectTimeout(DWORD dwTime)
	{
		return FALSE;
	}
protected:
	virtual void _OnError(int error)
	{
		m_pSink->OnSockError(error);
	}
	virtual void _OnRead(int error)
	{
		if(!error)
			m_canread = true;
		m_pSink->OnSockRead(error);
	}
	virtual void _OnWrite(int error)
	{
		if(!error)
			m_canwrite = true;
		m_pSink->OnSockWrite(error);
	}
	virtual void _OnConnect(int error)
	{
		m_connecting = false;
		if(!error){
			m_connected = true;
		}
		m_pSink->OnSockConnect(error);
	}
	virtual void _OnAccept(int error)
	{
		if(!error)
			m_canaccept = true;
		m_pSink->OnSockAccept(error);
	}
	virtual void _OnOutOfBand(int error)
	{
		m_pSink->OnSockOutOfBand(error);
	}
	virtual void _OnClose(int error)
	{
		m_connected = false;
		m_connecting = false;
		m_pSink->OnSockClose(error);
	}
	virtual void _OnResolving(void)
	{
		m_resolving = true;
		m_resolved = false;
		m_pSink->OnSockResolvingAddress();
	}
	virtual void _OnResolved(int error)
	{
		m_resolved = true;
		m_resolving = false;
		m_pSink->OnSockAddressResolved(error);
	}
};


} // NO5TL


#endif // !defined(AFX_ASYNCSOCKET_H__BF2569E1_2882_11D9_A17B_C890EEE07877__INCLUDED_)
