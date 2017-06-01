// WinSocket2.h: interface for the CWinSocket2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINSOCKET2_H__8C41EEA3_2D6C_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_WINSOCKET2_H__8C41EEA3_2D6C_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace NO5TL
{

class CAsyncBase 
{
	struct LocalStorage
	{
		CSocketWindow handler;
		UINT count; // objects in this thread using the handler

		LocalStorage()
		{
			count = 0;
		}
	};

	static DWORD m_dwIndex;	// local storage index
	static UINT m_counter;	// objects from all threads using this
							// class
	static CCriticalSection m_cs;
	CSocketWindow *m_pHandler;
protected:
	CAsyncBase(void)
	{
		m_pHandler = NULL;
		BOOL res;

		res = GetLocalStorage();
		if(res){
			res = OpenHandler(&m_pHandler);
		}
		ATLASSERT(res);
	}
	virtual ~CAsyncBase()
	{
		BOOL res = CloseHandler();

		if(res){
			res = ReleaseLocalStorage();
		}
		ATLASSERT(res);
	}
protected:
	// allocates a TLS index, if the object conter for this
	// class is zero
	// increments the counter of objects of this class
	static BOOL GetLocalStorage(void)
	{
		CSincro sincro(m_cs);
		BOOL res = FALSE;

		if(m_counter == 0){
			ATLASSERT(m_dwIndex == 0xFFFFFFFF);
			m_dwIndex = ::TlsAlloc();
			if(m_dwIndex != 0xFFFFFFFF){
				res = ::TlsSetValue(m_dwIndex,NULL);
				if(res)
					m_counter++;
			}
		}
		else{
			ATLASSERT(m_dwIndex != 0xFFFFFFFF);
			m_counter++;
			res = TRUE;
		}
		return res;
	}
	// decrements the count of objects of this class
	// if it reaches zero, releases the TLS index
	static BOOL ReleaseLocalStorage(void)
	{
		CSincro sincro(m_cs);
		BOOL res;

		if(0 == --m_counter){
			ATLASSERT(m_dwIndex != 0xFFFFFFFF);
			res = ::TlsFree(m_dwIndex);
			if(res)
				m_dwIndex = 0xFFFFFFFF;
		}
		return res;
	}

	// create handler window for this thread, if it was not yet
	// created
	BOOL OpenHandler(CSocketWindow **ppHandler)
	{
		BOOL res = FALSE;
		LocalStorage *pls = (LocalStorage *)::TlsGetValue(m_dwIndex);

		if(!pls && ::GetLastError() == NO_ERROR){
			pls = new LocalStorage;
			if(NULL != pls){
				res = ::TlsSetValue(m_dwIndex,pls);
			}
		}
		else if(!pls && GetLastError() != NO_ERROR){
			res = FALSE;
		}
		else
			res = TRUE;

		if(res){
			if(pls->count == 0){
				pls->handler.Create();
				res = pls->handler.IsWindow();
				ATLTRACE("creating socket handler in thread 0x%08X result = %d\n",::GetCurrentThreadId(),(int)res);
			}
			if(res){
				pls->count++;
				*ppHandler = &pls->handler;
			}
		}
		return res && ((*ppHandler)->IsWindow());
	}

	// decreases reference count for the thread and if it reaches
	// zero, destroy the handler window
	BOOL CloseHandler(void)
	{
		BOOL res = FALSE;

		if(m_dwIndex != 0xFFFFFFFF){
			LocalStorage *pls = (LocalStorage *)::TlsGetValue(m_dwIndex);

			if(pls){
				pls->count--;
				if(0 == pls->count){
					if(pls->handler.IsWindow()){
						pls->handler.DestroyWindow();
						res = !(pls->handler.IsWindow());
						ATLTRACE("destroying handler in thread 0x%08X res = %d\n",
							::GetCurrentThreadId(),res);
						ATLASSERT(res);
					}
					else
						res = TRUE;
					if(res){
						delete pls;
						res = ::TlsSetValue(m_dwIndex,NULL);
					}
				}
				else
					res = TRUE;
			}
		}
		return res;
	}
	
	CSocketWindow & GetHandler(void)
	{
		ATLASSERT(m_pHandler && m_pHandler->IsWindow());
		return *m_pHandler;
	}
};

__declspec(selectany) DWORD CAsyncBase::m_dwIndex = 0xFFFFFFFF;
__declspec(selectany) UINT CAsyncBase::m_counter = 0;
#ifndef NO5TL_ASYNCBASE_INIT
#define NO5TL_ASYNCBASE_INIT() NO5TL::CCriticalSection NO5TL::CAsyncBase::m_cs;
#endif

// internally handles AddSocket,RemoveSocket,SetHandler of
// CAsyncSocket, making it easier to use
class CWinSocket2 : public CWinSocket,public CAsyncBase
{
public:
	CWinSocket2(ISocketEvents *pSink):CAsyncSocket(pSink)
	{
		
	}
	virtual ~CWinSocket2()
	{
		//CloseSocket();
	}
	virtual BOOL CreateSocket(int type = SOCK_STREAM,int proto = 0)
	{
		BOOL res = CAsyncSocket::CreateSocket(type,proto);

		if(res){
			SetHandler(GetHandler().m_hWnd);
			res = GetHandler().AddSocket(GetHandle(),this);
		}
		return res;
	}
	virtual BOOL CloseSocket()
	{
		BOOL res = TRUE;

		if(IsHandleValid()){
			res = GetHandler().RemoveSocket(GetHandle());
			ATLASSERT(res);
		}
		CAsyncSocket::CloseSocket();
		return res;
	}
	virtual BOOL AttachAcceptedSocket(SOCKET s)
	{
		ATLASSERT(INVALID_SOCKET == GetHandle());
		BOOL res = CAsyncSocket::AttachAcceptedSocket(s);

		if(res){
			SetHandler(GetHandler().m_hWnd);
			res = GetHandler().AddSocket(GetHandle(),this);
		}
		return res;
	}
};

// internally handles AddAyncEvent,RemoveAsyncEvent,SetHandler of
// CAsyncAddress, making it easier to use
class CAsyncAddress2 : public CAsyncAddress,protected CAsyncBase
{
public:
	CAsyncAddress2(IResolveEvents *pSink):CAsyncAddress(pSink)
	{
		//
	}
	virtual BOOL GetHostByName(LPCTSTR name)
	{
		BOOL res;
		SetHandler(GetHandler().m_hWnd);
		res = CAsyncAddress::GetHostByName(name);
		if(res){
			res = GetHandler().AddAsyncEvent(GetHandle(),this);
		}
		return res;
	}
	virtual BOOL GetHostByAddr(const CIPAddress &ip)
	{
		BOOL res;
		SetHandler(GetHandler().m_hWnd);
		res = CAsyncAddress::GetHostByAddr(ip);
		if(res){
			res = GetHandler().AddAsyncEvent(GetHandle(),this);
		}
		return res;
	}
};

} // namespace NO5TL


#endif // !defined(AFX_WINSOCKET2_H__8C41EEA3_2D6C_11DA_A17D_000103DD18CD__INCLUDED_)
