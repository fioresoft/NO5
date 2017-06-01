// PortScanner.h: interface for the CPortScanner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PORTSCANNER_H__D9B0AD41_3CFC_11D9_A17B_86C502258B43__INCLUDED_)
#define AFX_PORTSCANNER_H__D9B0AD41_3CFC_11D9_A17B_86C502258B43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace NO5TL
{

struct IPortScannerEvents
{
	virtual void OnScanningPort(u_short port) = 0;
	virtual void OnPortOpen(u_short port) = 0;
	virtual void OnPortClosed(u_short port,int error) = 0;
	virtual void OnScanCompleted(int open,int closed,int m_timedout,
		int scanned) = 0;
	virtual void OnNoBuffer(u_short port,bool nobuf) = 0;
	virtual void OnConnectTimeout(u_short port) = 0;
	virtual void OnAbortScanning(u_short port) = 0;
};

template <class TSocket>
class CScanSocket : public TSocket,public ISocketEvents
{
	u_short m_port;
	IPortScannerEvents *m_pSink;
public:
	CScanSocket(void):TSocket(this),m_pSink(NULL)
	{
		
	}
	void SetSink(IPortScannerEvents *pSink)
	{
		m_pSink = pSink;
	}
	void SetPort(u_short port)
	{
		m_port = port;
	}
	BOOL Abort(void)
	{
		BOOL res = TRUE;
		if(IsHandleValid()){
			AsyncSelect(0);
			res = CloseSocket();
			if(res){
				m_pSink->OnAbortScanning(m_port);
			}
		}
		return res;
	}
	// ISocketEvents
	virtual void OnSockConnecting(void)
	{
		//m_pSink->OnScanningPort(m_port);
	}
	virtual void OnSockConnect(int error)
	{
		BOOL debug;

		debug = AsyncSelect(0);
		ATLASSERT(debug);
		debug = CloseSocket();
		ATLASSERT(debug);

		if(!error){
			m_pSink->OnPortOpen(m_port);
		}
		else{
			m_pSink->OnPortClosed(m_port,error);
		}
	}
	virtual void OnSockClose(int error)
	{
		ATLASSERT(0);
	}
	virtual void OnSockError(int error)
	{
		// we already handled
	}
	virtual void OnSockConnectTimeout(void)
	{
		m_pSink->OnConnectTimeout(m_port);
	}
};

template <class TSocket>
class CPortScannerT : public CThreadImplT< CPortScannerT<TSocket> >,
	public IPortScannerEvents
{
	typedef CScanSocket<TSocket> _Socket;
	u_short m_FirstPort;
	u_short m_LastPort;
	CIPAddress m_ip;
	_Socket *m_sockets;
	int m_ArraySize;
	IPortScannerEvents *m_pSink;
	volatile int m_open,m_closed,m_scanned,m_timedout;
	DWORD m_dwTimeout;
	volatile bool m_abort;
	//
	bool GetAbort(void)
	{
		return m_abort;
	}
public:
	CPortScannerT(IPortScannerEvents *pSink):m_pSink(pSink)
	{
		m_FirstPort = 0;
		m_LastPort = 0;
		m_ArraySize = 0;
		m_open = m_closed = m_scanned = m_timedout = 0;
		m_abort = false;
	}
	BOOL StartScanner(const CIPAddress &ip,u_short start,u_short end,
		int ArraySize = 50,DWORD dwTimeout = 3000)
	{
		BOOL res = FALSE;
		if(!IsThreadActive()){
			m_FirstPort = start;
			m_LastPort = end;
			m_ip = ip;
			m_ArraySize = ArraySize;
			m_dwTimeout = dwTimeout;
			m_open = m_closed = m_scanned = m_timedout = 0;
			m_abort = false;

			if(IsHandleValid())
				CloseHandle();
			res = CreateThread();
		}
		return res;
	}
	void EndScanner(BOOL abort)
	{
		if(abort)
			m_abort = true;
		else
			SetEnd();
	}
	// thread overdies
	DWORD ThreadProc(void)
	{
		BOOL res;
		CSocketAddress addr;
		u_short curport = m_FirstPort;
		int i = 0; // index of the next socket in the array
		MSG msg;
		bool nobuf = false;

		m_sockets = new _Socket[m_ArraySize];
		ATLASSERT(m_sockets);
		for(i=0;i<m_ArraySize;i++){
			m_sockets[i].SetSink(this);
			if(m_dwTimeout)
				m_sockets[i].SetConnectTimeout(m_dwTimeout);
		}
		while(!GetEnd() && curport <= m_LastPort && !GetAbort()){
			while(::PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			if(i == m_ArraySize){
				i = 0;
			}
			if(m_sockets[i].IsHandleValid()){
				i++;
				continue;
			}
			res = m_sockets[i].CreateSocket();
			if(res){
				m_sockets[i].DontLinger();
				m_sockets[i].SetOption(SO_REUSEADDR,(int)TRUE);
				res = m_sockets[i].AsyncSelect(FD_CONNECT);
				if(res){
					addr.Set(m_ip,CPort(curport));
					m_sockets[i].SetPort(curport);
					res = m_sockets[i].Connect(&addr);
				}
			}
			if(res){
				m_pSink->OnScanningPort(curport);
				if(nobuf){
					nobuf = false;
					m_pSink->OnNoBuffer(curport,false);
				}
				curport++;
			}
			else{
				int error = CSocket::GetLastError();
				BOOL debug;

				debug = m_sockets[i].AsyncSelect(0);
				ATLASSERT(debug);
				debug = m_sockets[i].CloseSocket();
				ATLASSERT(debug);
				if(error == WSAENOBUFS && !nobuf){
					nobuf = true;
					m_pSink->OnNoBuffer(curport,true);
				}
				else if(error != WSAENOBUFS){
					CString s = CSocket::GetErrorDesc(error);
					CWindow wnd(GetDesktopWindow());
					wnd.MessageBox(s);
				}
			}
			i++;
		}
		// ja iniciamos o scaneamento das portas [first,cur[
		// que nao eh necessariamente igual a [first,last + 1[
		// pois pode ter sido chamado SetEnd
		// entao esperamos ate que todos os scans iniciados, sejam
		// finalizados

		if(!GetAbort()){
			// wait the scanning to finish
			while(!GetAbort() && m_scanned < (curport - m_FirstPort)){
				while(::PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
					DispatchMessage(&msg);
				}
			}
		}
		else{
			BOOL res;

			// we dont want to wait for any pending scanning
			// to finish
			for(int i=0;i<m_ArraySize;i++){
				res = m_sockets[i].Abort();
				ATLASSERT(res);
			}
		}

		delete [] m_sockets;
		m_sockets = NULL;
		m_abort = false;

		return 0;
	}
	void AfterThreadProc(DWORD res)
	{
		m_pSink->OnScanCompleted(m_open,m_closed,m_timedout,m_scanned);
	}

protected:
	// IPortScannerEvents
	virtual void OnScanningPort(u_short port)
	{
		m_pSink->OnScanningPort(port);
	}
	virtual void OnPortOpen(u_short port)
	{
		m_open++;
		m_scanned++;
		m_pSink->OnPortOpen(port);
	}
	virtual void OnPortClosed(u_short port,int error)
	{
		m_closed++;
		m_scanned++;
		m_pSink->OnPortClosed(port,error);
	}
	virtual void OnConnectTimeout(u_short port)
	{
		m_timedout++;
		m_scanned++;
		m_pSink->OnConnectTimeout(port);
	}
	virtual void OnScanCompleted(int open,int closed,int timedout,
		int scanned)
	{
		ATLASSERT((open + closed + timedout) == scanned);
	}
	virtual void OnNoBuffer(u_short,bool)
	{
		//
	}
	virtual void OnAbortScanning(u_short port)
	{
		m_closed++;
		m_scanned++;
		m_pSink->OnAbortScanning(port);
	}
};

} // NO5TL

#endif // !defined(AFX_PORTSCANNER_H__D9B0AD41_3CFC_11D9_A17B_86C502258B43__INCLUDED_)
