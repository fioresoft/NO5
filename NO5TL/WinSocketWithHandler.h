// WinSocketWithHandler.h: interface for the CWinSocketWithHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINSOCKETWITHHANDLER_H__8C41EEA4_2D6C_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_WINSOCKETWITHHANDLER_H__8C41EEA4_2D6C_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "winsocket.h"

namespace NO5TL
{

struct CWinSocketData
{
protected:
	static CSocketWindow m_wnd;
	static int m_count;
};

__declspec(selectany) int CWinSocketData::m_count = 0;

// async socket using window messages, with a static window for all instances
// not aware of threads
class CWinSocketWithHandler : public CWinSocket,public CWinSocketData
{
	
public:
	CWinSocketWithHandler(ISocketEvents *pSink):CWinSocket(pSink)
	{
		//
	}
	virtual BOOL CreateSocket(int type = SOCK_STREAM,int proto = 0)
	{
		BOOL res = TRUE;

		ATLASSERT(m_count >= 0);
		if(!m_count){
			ATLASSERT(!m_wnd.IsWindow());
			ATLTRACE("creating socket window\n");
			m_wnd.Create();
			res = m_wnd.IsWindow();
		}
		if(res){
			m_count++;
			SetHandler(m_wnd);
			res = CWinSocket::CreateSocket(type,proto);
			if(res){
				res = m_wnd.AddSocket(GetHandle(),this);
			}
		}
		return res;
	}
	virtual BOOL CloseSocket(void)
	{
		BOOL res = TRUE;

		if(IsHandleValid()){
			res = m_wnd.RemoveSocket(GetHandle());
			ATLASSERT(res);
			res = CWinSocket::CloseSocket();
			if(res){
				m_count--;
				if(!m_count){
					ATLTRACE("destroying socket window\n");
					m_wnd.DestroyWindow();
				}
			}
		}
		return res;
	}
	virtual BOOL AttachAcceptedSocket(SOCKET s)
	{
		ATLASSERT(!IsHandleValid());
		ATLASSERT(m_count >= 0);
		BOOL res = CWinSocket::AttachAcceptedSocket(s);

		if(res && !m_count){
			ATLASSERT(!m_wnd.IsWindow());
			m_wnd.Create();
			res = m_wnd.IsWindow();
		}
		if(res){
			res = m_wnd.AddSocket(GetHandle(),this);
		}
		return res;
	}
};

// internally handles AddAyncEvent,RemoveAsyncEvent,SetHandler of
// CAsyncAddress, making it easier to use
class CAsyncAddressWithHandler : public CAsyncAddress,public CWinSocketData
{
public:
	CAsyncAddressWithHandler(IResolveEvents *pSink):CAsyncAddress(pSink)
	{
		//
	}
	virtual BOOL GetHostByName(LPCTSTR name)
	{
		BOOL res;
		SetHandler(m_wnd);
		res = CAsyncAddress::GetHostByName(name);
		if(res){
			res = m_wnd.AddAsyncEvent(GetHandle(),this);
		}
		return res;
	}
	virtual BOOL GetHostByAddr(const CIPAddress &ip)
	{
		BOOL res;
		SetHandler(m_wnd);
		res = CAsyncAddress::GetHostByAddr(ip);
		if(res){
			res = m_wnd.AddAsyncEvent(GetHandle(),this);
		}
		return res;
	}
	virtual BOOL CancelAsyncRequest(void)
	{
		BOOL res = m_wnd.RemoveAsyncEvent(this);
		if(res){
			res = CAsyncAddress::CancelAsyncRequest();
		}
		return res;
	}
};

class CSimpleSocket : \
	public CWinSocketWithHandler,
	public IResolveEvents
{
protected:
	CAsyncAddressWithHandler m_addr;
	u_short m_port;
public:
	CSimpleSocket(ISocketEvents *pSink):m_addr((IResolveEvents *)this),CWinSocketWithHandler(pSink)
	{
		
	}
	virtual BOOL Connect(LPCSTR server,u_short port)
	{
		m_port = port;
		return m_addr.GetHostByName(server);
	}
	virtual BOOL CloseSocket(void)
	{
		if(m_addr.InProgress()){
			m_addr.CancelAsyncRequest();
		}
		if(IsHandleValid())
			AsyncSelect(0);
		return CWinSocketWithHandler::CloseSocket();
	}
	virtual BOOL Connect(SOCKADDR *addr,int addrlen)
	{
		return CWinSocketWithHandler::Connect(addr,addrlen);
	}
	virtual BOOL Connect(SOCKADDR_IN *addr)
	{
		return CWinSocketWithHandler::Connect(addr);
	}

	// IResolveEvents
	virtual void OnResolvingAddress(CAsyncResolve *pObj)
	{
		_OnResolving();
	}
	virtual void OnAddressResolved(CAsyncResolve *pObj,int error,
		int buflen)
	{
		if(&m_addr == pObj && !error){
			CSocketAddress addr;
			CIPAddress ip;
			CPort port;
			BOOL res;

			_OnResolved(error);

			port.Set(m_port,true);
			ip = m_addr.GetIP();
			addr.Set(ip,port);

			res = Connect(&addr);
			ATLASSERT(res);
			if(!res){
				m_pSink->OnSockError(CSocket::GetLastError());
			}

		}
		if(error){
			_OnResolved(error);
			_OnError(error);
		}
	}
};

} // namespace NO5TL

#endif // !defined(AFX_WINSOCKETWITHHANDLER_H__8C41EEA4_2D6C_11DA_A17D_000103DD18CD__INCLUDED_)
