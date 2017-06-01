// WinSocket.h: interface for the CWinSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINSOCKET_H__8C41EEA2_2D6C_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_WINSOCKET_H__8C41EEA2_2D6C_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "handle.h"
#include "asyncsocketbase.h"

namespace NO5TL
{

// forward declarations
class CAsyncResolve;
class CWinSocket;

struct IResolveEvents
{
	virtual void OnResolvingAddress(CAsyncResolve *pObj) {}
	virtual void OnResolvingService(CAsyncResolve *pObj) {}
	virtual void OnAddressResolved(CAsyncResolve *pObj,int error,
		int buflen) {}
	virtual void OnServiceResolved(CAsyncResolve *pObj,int error,
		int buflen) {}
};

// can handle multiple sockets
// obs: since we use socket handles as timer identifiers, a window
// that derives from this class has no way to use its own timer
// identifiers with the certainty that it wont conflict with ours
// ( right ? ) so it should use TimerProc callbacks
class CWinSocketHandler
{
	CSimpleMap<SOCKET,CWinSocket *> m_SockMap;
	CSimpleMap<HANDLE,CAsyncResolve *> m_ResolveMap;
public:
	static UINT WM_SOCKET_EVENT;
	static UINT WM_SOCKET_RESOLVE;
public:
	BEGIN_MSG_MAP(CWinSocketHandler)
		MESSAGE_HANDLER(WM_SOCKET_EVENT,_OnSocketEvent)
		MESSAGE_HANDLER(WM_SOCKET_RESOLVE,_OnSocketResolve)
		MESSAGE_HANDLER(WM_TIMER,OnSocketTimer)
	END_MSG_MAP()

	CWinSocketHandler()
	{
		if(!WM_SOCKET_EVENT){
			WM_SOCKET_EVENT = ::RegisterWindowMessage(_T("No5SockEvent"));
			WM_SOCKET_RESOLVE = ::RegisterWindowMessage(_T("No5SockResolve"));
		}
	}

public: // interface
	BOOL AddSocket(SOCKET s,CWinSocket *p)
	{
		CWinSocket *pp = m_SockMap.Lookup(s);
		BOOL res = FALSE;

		ATLASSERT(NULL != p);
		if(!pp && p){
			res = m_SockMap.Add(s,p);
			if(res){
				ATLTRACE("CWinSockHandler::AddSocket - socket added\n");
			}
		}
		else if(NULL !=  pp){
			ATLTRACE("Socket was already added to the handler\n");
			ATLASSERT(0);
		}
		return res;
	}
	BOOL RemoveSocket(SOCKET s)
	{
		ATLTRACE("CWinSockHandler::RemoveSocket\n");
		return m_SockMap.Remove(s);
	}
	BOOL AddAsyncEvent(HANDLE hEvent,CAsyncResolve *pObj)
	{
		BOOL res = FALSE;

		if(!m_ResolveMap.Lookup(hEvent)){
			res = m_ResolveMap.Add(hEvent,pObj);
			if(res){
				ATLTRACE("CWinSockHandler::AddAsyncEvent - event added\n");
			}
		}
		else
			ATLASSERT(0 && "resolve event already in the map");
		return res;
	}
	BOOL RemoveAsyncEvent(HANDLE hEvent)
	{
		ATLTRACE("CWinSockHandler::RemoveAsyncEvent\n");
		return m_ResolveMap.Remove(hEvent);
	}
public: // message handlers
	LRESULT _OnSocketEvent(UINT uMsg,WPARAM wParam,LPARAM lParam,
		BOOL &bHandled);
	LRESULT _OnSocketResolve(UINT uMsg,WPARAM wParam,LPARAM lParam,
		BOOL &bHandled);
	LRESULT OnSocketTimer(UINT uMsg,WPARAM wParam,LPARAM lParam,
		BOOL &bHandled);
};

__declspec(selectany) UINT CWinSocketHandler::WM_SOCKET_EVENT = 0;
__declspec(selectany) UINT CWinSocketHandler::WM_SOCKET_RESOLVE = 0;

class CWinSocket : public CAsyncSocketBase
{
	CWindow m_handler;
	DWORD m_timeout;	// miliseconds
public:
	CWinSocket(ISocketEvents *pSink):CAsyncSocketBase(pSink)
	{
		ATLASSERT(NULL != pSink);
		m_timeout = 0;
	}
	virtual ~CWinSocket()
	{
		CloseSocket();
	}
	void SetHandler(HWND hWnd)
	{
		m_handler = hWnd;
		ATLASSERT(m_handler.IsWindow());
	}
	virtual BOOL AsyncSelect(long events = FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE)
	{
		BOOL res = FALSE;

		ATLASSERT(m_handler.IsWindow());
		if(m_handler.IsWindow())
			res = ::WSAAsyncSelect(GetHandle(),m_handler,
			CWinSocketHandler::WM_SOCKET_EVENT,events) != SOCKET_ERROR;
		return res;
	}
	virtual BOOL Connect(SOCKADDR *addr,int addrlen)
	{
		int res;
		BOOL rv = TRUE;

		ATLASSERT(IsHandleValid());
		// set the timer
		if(m_timeout){
			UINT timer = m_handler.SetTimer((UINT)GetHandle(),
				m_timeout);
			ATLASSERT(timer);
		}
		m_connecting = true;
		m_pSink->OnSockConnecting();
		res = CSocket::Connect(addr,addrlen);
		if(res == SOCKET_ERROR && WSAEWOULDBLOCK != GetLastError()){
			m_connecting = false;
			rv = FALSE;
			if(m_timeout){
				m_handler.KillTimer((UINT)GetHandle());
			}
			_OnError(GetLastError());
		}
		return rv;
	}
	virtual BOOL Connect(SOCKADDR_IN *addr)
	{
		return Connect((SOCKADDR *)addr,sizeof(SOCKADDR_IN));
	}
	virtual BOOL Connect(LPCSTR server,u_short port)
	{
		CSocketAddress addr;
		BOOL res;

		_OnResolving();
		res = addr.Set(server,CPort(port));
		if(res){
			_OnResolved(0);
			res = Connect(&addr);
		}
		else{
			int error = CSocket::GetLastError();
			_OnResolved(error);
			_OnError(error);
		}
		return res;
	}

	virtual BOOL SetConnectTimeout(DWORD dwTime)
	{
		BOOL res = FALSE;

		ATLASSERT(!IsConnecting() && !IsConnected());

		if(!IsConnecting() && !IsConnected()){
			m_timeout = dwTime;
			res = TRUE;
		}
		return res;
	}

	// called by the handler, should not be called directly
	// despite being public
	void OnSocketEvent(long events,int error)
	{
		if(error){
			_OnError(error);
		}
		if(events & FD_READ)
			_OnRead(error);
		else if(events & FD_WRITE)
			_OnWrite(error);
		else if(events & FD_CONNECT)
			_OnConnect(error);
		else if(events & FD_CLOSE)
			_OnClose(error);
		else if(events & FD_ACCEPT)
			_OnAccept(error);
		else if(events & FD_OOB)
			_OnOutOfBand(error);
	}
	void _OnSocketTimeout(void)
	{
		BOOL res;

		res = m_handler.KillTimer((UINT)GetHandle());
		ATLASSERT(res);
		m_pSink->OnSockConnectTimeout();
		res = AsyncSelect(0);
		ATLASSERT(res);
		res = Shutdown() != SOCKET_ERROR;
		ATLASSERT(res);
		res = CloseSocket();
		ATLASSERT(res);
	}
protected:
	virtual void _OnConnect(int error)
	{
		m_connecting = false;
		if(!error){
			m_connected = true;
		}
		if(m_timeout){
			BOOL debug = m_handler.KillTimer((UINT)GetHandle());
			ATLASSERT(debug);
		}
		m_pSink->OnSockConnect(error);
	}
};

struct AsyncHandleTraits
{
	typedef HANDLE handle_type;

	static BOOL Destroy(handle_type h)
	{
		int res = ::WSACancelAsyncRequest(h);
		BOOL ok = FALSE;

		if(res == SOCKET_ERROR){
			res = CSocket::GetLastError();
			// in this value, it has already been canceled
			if(res == WSAEALREADY || res == WSAEINVAL)
				ok = TRUE;
			CSocket::SetLastError(res);
		}
		else
			ok = TRUE;

		return ok;
	}

	static handle_type InvalidHandle(void)
	{
		return HANDLE(NULL);
	}
};


typedef CHandleT<AsyncHandleTraits,TRUE> CAsyncHandle;

// base de CAsyncAddress e CAsyncService, gerencia o handle, o buf
// um handle de janela e chama event callbacks atraves de m_pSink
// que deve ser passado no contructor.
// Deve ser adicionado ao handler 
class CAsyncResolve
{
protected:
	CAsyncHandle m_handle;
	BYTE m_buf[MAXGETHOSTSTRUCT];
	CWindow m_handler;
	void ZeroBuf(void) {\
		::ZeroMemory(m_buf,sizeof(m_buf));
	}
	IResolveEvents *m_pSink;
public:
	CAsyncResolve(IResolveEvents *pSink):m_pSink(pSink)
	{
		ZeroBuf();
		ATLASSERT(NULL != m_pSink);
	}

	virtual BOOL CancelAsyncRequest(void)
	{
		BOOL res = TRUE;
		
		if(m_handle.IsHandleValid()){
			res = m_handle.CloseHandle();
		}
		return res;
	}
	virtual BOOL InProgress(void) const
	{
		return m_handle.IsHandleValid();
	}
	// window that will handle the messages
	void SetHandler(HWND hWnd)
	{
		ATLASSERT(::IsWindow(hWnd));
		m_handler = hWnd;
	}
	HANDLE GetHandle(void) const
	{
		return m_handle.GetHandle();
	}
	// called by CWinSocketHandler
	virtual void OnResolveEvent(int err,int buflen) = 0;
};

class CAsyncAddress : public CAsyncResolve,public CComputerAddress
{
public:
	CAsyncAddress(IResolveEvents *pSink):CAsyncResolve(pSink)
	{
		//
	}
	virtual BOOL GetHostByName(LPCSTR name)
	{
		BOOL res = FALSE;
		CIPAddress ip;

		// ve se nao eh uma string com um endereco em dotted 
		// decima notation, se for, chama gethostbyaddr
		ip.Set(name);
		if(ip.IsValid()){
			res = GetHostByAddress(ip);
		}
		else if(!m_handle.IsHandleValid()){
			ZeroBuf();
			ATLASSERT(m_handler.IsWindow());
			if(m_pSink)
				m_pSink->OnResolvingAddress(this);
			m_handle = ::WSAAsyncGetHostByName(m_handler,
				CWinSocketHandler::WM_SOCKET_RESOLVE,name,
				(char *)m_buf,sizeof(m_buf));
			res = m_handle.IsHandleValid();
		}
		return res;
	}
	virtual BOOL GetHostByAddress(const CIPAddress &ip)
	{
		BOOL res = FALSE;

		if(!m_handle.IsHandleValid()){
			int len = sizeof(IN_ADDR);
			int type = AF_INET;
			const char *addr = (const char *)((IN_ADDR *)&ip);

			ZeroBuf();
			ATLASSERT(m_handler.IsWindow());
			if(m_pSink)
				m_pSink->OnResolvingAddress(this);
			m_handle = ::WSAAsyncGetHostByAddr(m_handler,
				CWinSocketHandler::WM_SOCKET_RESOLVE,
				addr,len,type,(char *)m_buf,sizeof(m_buf));
			res = m_handle.IsHandleValid();
		}
		return res;
	}
	virtual void OnResolveEvent(int err,int buflen)
	{
		ATLASSERT(NULL != m_pSink);
		if(NULL != m_pSink){
			if(!err)
				CopyHostent((HOSTENT *)m_buf);
			
			if(m_pSink){
				m_pSink->OnAddressResolved(this,err,buflen);
			}
		}
		m_handle.CloseHandle();
	}
};

class CAsyncService : public CAsyncResolve,public CService
{
public:
	CAsyncService(IResolveEvents *pSink):CAsyncResolve(pSink)
	{
		//
	}
	virtual BOOL GetServByName(LPCSTR name,LPCSTR proto = NULL)
	{
		BOOL res = FALSE;

		if(!m_handle.IsHandleValid()){
			ZeroBuf();
			ATLASSERT(m_handler.IsWindow());
			if(m_pSink)
				m_pSink->OnResolvingService(this);
			m_handle = ::WSAAsyncGetServByName(m_handler,
				CWinSocketHandler::WM_SOCKET_RESOLVE,name,proto,
				(char *)m_buf,
				sizeof(m_buf));
			res = m_handle.IsHandleValid();
		}
		return res;
	}
	virtual BOOL GetServByPort(const CPort &port,LPCSTR proto = NULL)
	{
		BOOL res = FALSE;

		if(!m_handle.IsHandleValid()){
			ZeroBuf();
			ATLASSERT(m_handler.IsWindow());
			if(m_pSink)
				m_pSink->OnResolvingService(this);
			m_handle = ::WSAAsyncGetServByPort(m_handler,
				CWinSocketHandler::WM_SOCKET_RESOLVE,
				port.Get(false),proto,(char *)m_buf,sizeof(m_buf));
			res = m_handle.IsHandleValid();
		}
		return res;
	}
	virtual void OnResolveEvent(int err,int buflen)
	{
		ATLASSERT(m_pSink != NULL);
		if(NULL != m_pSink){
			if(!err)
				CopyServent((SERVENT *)m_buf);
			if(m_pSink){
				m_pSink->OnServiceResolved(this,err,buflen);
			}
		}
		m_handle.CloseHandle();
	}
};

inline LRESULT CWinSocketHandler::_OnSocketEvent(UINT uMsg,WPARAM wParam,
	LPARAM lParam,BOOL &bHandled)
{
	SOCKET s = static_cast<SOCKET>(wParam);
	int error = WSAGETSELECTERROR(lParam);
	long event = WSAGETSELECTEVENT(lParam);
	CWinSocket *p = m_SockMap.Lookup(s);

	ATLASSERT(NULL != p);
	if(NULL != p)
		p->OnSocketEvent(event,error);
	return 0;
}

inline LRESULT CWinSocketHandler::_OnSocketResolve(UINT uMsg,WPARAM wParam,
	LPARAM lParam,BOOL &bHandled)
{
	HANDLE hEvent = (HANDLE)wParam;
	int err = WSAGETASYNCERROR(lParam);
	int bufsize = WSAGETASYNCBUFLEN(lParam);
	CAsyncResolve *p = m_ResolveMap.Lookup(hEvent);

	ATLASSERT(uMsg == WM_SOCKET_RESOLVE);
	if(uMsg != WM_SOCKET_RESOLVE){
		bHandled = FALSE;
		return 0;
	}

	if(p){
		// tem que remover antes pois CAsyncResolve ira destruir
		// o handle
		RemoveAsyncEvent(hEvent);
		p->OnResolveEvent(err,bufsize);
	}
	else{
		ATLASSERT(0);
		bHandled = FALSE;
	}
	return 0;
}

inline LRESULT CWinSocketHandler::OnSocketTimer(UINT uMsg,WPARAM wParam,
	LPARAM lParam,BOOL &bHandled)
{
	SOCKET s = static_cast<SOCKET>(wParam);
	CWinSocket *p = m_SockMap.Lookup(s);

	ATLASSERT(NULL != p);
	if(NULL != p)
		p->_OnSocketTimeout();
	else
		bHandled = FALSE;
	
	return 0;
}

typedef CWinTraits<WS_OVERLAPPED> CInvisibleWinTraits;

class CSocketWindow : \
	public CWindowImpl<CSocketWindow,CWindow,CInvisibleWinTraits>,
	public CWinSocketHandler
{
	typedef CWindowImpl<CSocketWindow,CWindow,CInvisibleWinTraits> _BaseClass;
public:
#ifdef _DEBUG
	DECLARE_WND_CLASS(_T("NO5SocketWindow"));
#endif

	BEGIN_MSG_MAP(CSocketWindow)
		CHAIN_MSG_MAP(CWinSocketHandler)
	END_MSG_MAP()

	HWND Create(HWND hWndParent = NULL)
	{
		return _BaseClass::Create(\
			hWndParent,
			rcDefault,
			NULL,
			0,0);
	}
};


} // namespace NO5TL

#endif // !defined(AFX_WINSOCKET_H__8C41EEA2_2D6C_11DA_A17D_000103DD18CD__INCLUDED_)
