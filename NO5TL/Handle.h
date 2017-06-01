// Handle.h: interface for the CHandle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HANDLE_H__AFDE0701_1D90_11D9_A17B_AC9DAA311946__INCLUDED_)
#define AFX_HANDLE_H__AFDE0701_1D90_11D9_A17B_AC9DAA311946__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace NO5TL
{

/* generic handle traits 
struct HandleTraits
{
	typedef implementation_defined handle_type

	static BOOL Destroy(handle_type h)
	{
		//
	}
	static handle_type InvalidHandle(void) 
	{
		//
	}
};
*/

struct WinHandleTraits
{
	typedef HANDLE handle_type;

	static BOOL Destroy(HANDLE h)
	{
		return ::CloseHandle(h);
	}
	static HANDLE InvalidHandle(void)
	{
		return NULL;
	}
};


template <class HandleTraits,BOOL t_Owner>
class CHandleT : public HandleTraits
{
public:
	typedef HandleTraits::handle_type handle_type;
private:
	handle_type m_h;
	typedef CHandleT<HandleTraits,t_Owner> _ThisClass;
public:
	CHandleT(handle_type h =InvalidHandle()):m_h(h)
	{
		//
	}
	CHandleT(_ThisClass &h):m_h(InvalidHandle())
	{
		Assign(h);
	}
	
	~CHandleT()
	{
		BOOL res = CloseHandle();
		_ASSERTE(res);
	}
	BOOL CloseHandle(void)
	{
		BOOL res = TRUE;

		if(IsOwner() && IsHandleValid()){
			if(res = Destroy(m_h))
				m_h = InvalidHandle();
		}
		else if(!IsOwner()){
			m_h = InvalidHandle();
		}
		return res;
	}
	BOOL IsOwner(void) const
	{
		return t_Owner;
	}
	BOOL IsHandleValid(void) const
	{
		return m_h != InvalidHandle();
	}
	// this will destroy any previous handle, if its owner
	BOOL AttachHandle(handle_type h)
	{
		BOOL res = CloseHandle();

		if(res){
			m_h = h;
		}
		return res;
	}
	handle_type DetachHandle(void)
	{
		handle_type hOld = m_h;
		m_h = InvalidHandle();
		return hOld;
	}
	// takes ownership if _ThisClass owns the handle
	_ThisClass & Assign(_ThisClass & rhs)
	{
		if(GetHandle() != rhs.GetHandle()){
			BOOL res = AttachHandle(rhs.DetachHandle());
			_ASSERTE(res);
		}
		return *this;
	}
	_ThisClass & Assign(handle_type t)
	{
		BOOL res = AttachHandle(t);
		_ASSERTE(res);
		return *this;
	}
	
	operator handle_type() const
	{
		return m_h;
	}

	const handle_type & GetHandle(void) const
	{
		return m_h;
	}
	handle_type & GetHandle(void)
	{
		return m_h;
	}
	_ThisClass & operator = (_ThisClass &rhs)
	{
		return Assign(rhs);
	}
	_ThisClass & operator = ( handle_type h )
	{
		return Assign(h);
	}
	
	BOOL operator == (handle_type h) const
	{
		return m_h == h ? TRUE : FALSE;
	}
	BOOL operator != (handle_type h) const
	{
		return m_h != h ? TRUE : FALSE;
	}
};

// win32 handle resource
typedef CHandleT<WinHandleTraits,TRUE> CWinHandle;

/** objets that can be used in wait functions */
template <class HandleTraits,BOOL t_Owner>
class CSincroObjectT : public CHandleT<HandleTraits,t_Owner>
{
	typedef CSincroObjectT<HandleTraits,t_Owner> _ThisClass;
	typedef CHandleT<HandleTraits,t_Owner> _BaseClass;
public:
	CSincroObjectT()
	{
		//
	}
	CSincroObjectT(handle_type h):_BaseClass(h)
	{
		//
	}
	CSincroObjectT(_ThisClass &obj):_BaseClass(obj)
	{
		//
	}
	_ThisClass & operator = ( handle_type h )
	{
		return (_ThisClass &)Assign(h);
	}
	_ThisClass & operator = (_ThisClass &rhs)
	{
		return (_ThisClass &)Assign(rhs);
	}
	DWORD Wait(DWORD ms = INFINITE) const 
	{
		return ::WaitForSingleObject(GetHandle(),ms);
	}
	DWORD MsgWait(DWORD ms,DWORD wake = QS_ALLEVENTS,
		BOOL fWaitAll = FALSE) const
	{
		HANDLE h = GetHandle();

		if(!IsHandleValid())
			return DWORD(-1);
		return ::MsgWaitForMultipleObjects(1,&h,fWaitAll,
			ms,wake);
	}
	// returns TRUE if the event is signaled
	// only quits the message loop on error or when event signaled
	BOOL WaitWithMsgLoop(void)
	{
		BOOL res = FALSE;

		if(IsHandleValid()){
			res = AtlWaitWithMessageLoop(GetHandle());
		}
		return res;
	}
};

typedef CSincroObjectT<WinHandleTraits,TRUE> CSincroObject;

template <BOOL t_Owner>
class CEventT:public CSincroObjectT<WinHandleTraits,t_Owner>
{
	typedef CSincroObjectT<WinHandleTraits,t_Owner> _BaseClass;
	typedef CEventT<t_Owner> _ThisClass;
	typedef _BaseClass::handle_type handle_type;
public:
	CEventT(handle_type h = InvalidHandle()):_BaseClass(h)
	{
		//
	}
	CEventT(CEventT<t_Owner> &event):_BaseClass(event)
	{
		//
	}
	_ThisClass & operator = (handle_type h)
	{
		return (_ThisClass &)Assign(h);
	}
	_ThisClass & operator = (_ThisClass &event)
	{
		return (_ThisClass &)Assign(event);
	}
	BOOL CreateEvent(BOOL signaled,BOOL ManualReset,
		LPCTSTR name = NULL,LPSECURITY_ATTRIBUTES atrib = NULL)
	{
		CEventT<FALSE> hEvent = ::CreateEvent(atrib,ManualReset,
			signaled,name);
		BOOL res  = FALSE;

		if(hEvent.IsHandleValid()){
			res = AttachHandle(hEvent.DetachHandle());
		}
		return res;
	}
	// signals it
	BOOL SetEvent(void) const
	{
		_ASSERTE(IsHandleValid());
		return ::SetEvent(GetHandle());
	}
	// unsignals it
	BOOL ResetEvent(void) const
	{
		_ASSERTE(IsHandleValid());
		return ::ResetEvent(GetHandle());
	}
	BOOL PulseEvent(void) const
	{
		return ::PulseEvent(GetHandle()) != NULL;
	}
	BOOL IsEventSet(void) const
	{
		return WAIT_OBJECT_0 == Wait(0);
	}
};

typedef CEventT<FALSE> CEventHandle;
typedef CEventT<TRUE> CEvent;

// wraps the win32 thread api
template <BOOL t_bOwner>
class CThreadT:public CSincroObjectT<WinHandleTraits,t_bOwner>
{
	typedef CSincroObjectT<WinHandleTraits,t_bOwner> _BaseClass;
	typedef CThreadT<t_bOwner> _ThisClass;
	typedef _BaseClass::handle_type handle_type;
public:
	CThreadT(handle_type h = InvalidHandle()):_BaseClass(h)
	{
		//
	}
	
	/** returns previous supend count */
	DWORD Suspend(void)
	{
		DWORD dwRv = 0xFFFFFFFF;
		
		_ASSERTE(IsHandleValid());
		if(IsHandleValid()){
			dwRv = ::SuspendThread(GetHandle());
			_ASSERTE(dwRv != 0xFFFFFFFF);
		}
		return dwRv;
	}
	/** returns previous supend count */
	DWORD Resume(void) const
	{
		DWORD dwRv = 0xFFFFFFFF;
			
		_ASSERTE(IsHandleValid());
		if(IsHandleValid()){
			dwRv = ::ResumeThread(GetHandle());
			_ASSERTE(dwRv != 0xFFFFFFFF);
		}
		return dwRv;
	}
	BOOL SetPriority(int nLevel = THREAD_PRIORITY_NORMAL) const
	{
		_ASSERTE(IsHandleValid());
		return ::SetThreadPriority(GetHandle(),nLevel);
	}
	int GetPriority(void) const
	{
		int iRes;

		_ASSERTE(IsHandleValid());
		iRes = ::GetThreadPriority(GetHandle());
		_ASSERTE(iRes != THREAD_PRIORITY_ERROR_RETURN);
		return iRes;
	}
	
	BOOL GetExitCode(DWORD &dwCode) const
	{
		_ASSERTE(IsHandleValid());
		BOOL res = ::GetExitCodeThread(GetHandle(),&dwCode);
		_ASSERTE(res);
		return res;
	}
	void ExitThread(DWORD dwExitCode=0) const
	{
		_ASSERTE(IsCurrentThread());
#ifndef	NO5_USE_THREAD_C_API
		::ExitThread(dwExitCode);
#else
		_endthread();
#endif
	}
	/** dangerous */
	BOOL TerminateThread(DWORD dwExitCode=0) const
	{
		BOOL res = FALSE;
		_ASSERTE(IsHandleValid());
		if(IsHandleValid()){
			res = ::TerminateThread(GetHandle(),dwExitCode);
			_ASSERTE(res);
		}
		return res;
	}
	/**
		Determine if the thread represented by this object is still
		active
	*/
	BOOL IsThreadActive(void) const
	{
		BOOL bRes = FALSE;
		DWORD dwCode;
			
		if(IsHandleValid()){
			if(GetExitCode(dwCode)){
				if(dwCode == STILL_ACTIVE){
					bRes = TRUE;
				}
			}
		}
		return bRes;
	}
};

typedef CThreadT<TRUE> CThread;
typedef CThreadT<FALSE> CThreadHandle;

// implements a thread with the ThreadProc mechanism 
// we always own this objects
template <class T>
class CThreadImpl:public CThread
{
	typedef CThread _BaseClass;
	typedef CThreadImpl< T > _ThisClass;
	//
	DWORD m_id;
	volatile BOOL m_end;
	const BOOL m_autodel;
public:
	CThreadImpl(BOOL autodel = FALSE):m_autodel(autodel)
	{
		m_id = 0;
		m_end = FALSE;
	}
	
	BOOL CreateThread(BOOL bSuspended = FALSE,DWORD dwStackSize = 0,
		LPSECURITY_ATTRIBUTES lpThreadAttributes = NULL)
	{
		BOOL res = CloseHandle();
		handle_type hThread;
		T *pThis = static_cast<T*>(this);

		ATLASSERT(res);
		if(res){
			m_id = 0;
			m_end = FALSE; 
#ifndef NO5_USE_THREAD_C_API
			hThread = ::CreateThread(lpThreadAttributes,dwStackSize,
				_ExternalThreadProc,(LPVOID)pThis,
				bSuspended ? CREATE_SUSPENDED : 0,
				&m_id);
#else
			hThread = (HANDLE)_beginthreadex(lpThreadAttributes,
				dwStackSize,
				_ExternalThreadProc,
				(LPVOID)pThis,
				bSuspended ? CREATE_SUSPENDED : 0,
				(unsigned int *)&m_id);
#endif // NO5_USE_THREAD_C_API

			AttachHandle(hThread);
			res = IsHandleValid();
		}
		return res;
	}
	
	DWORD GetThreadID(void) const
	{
		return m_id;
	}
	
	BOOL IsCurrentThread(void) const
	{
		return (::GetCurrentThreadId() == GetThreadID());
	}
	BOOL GetAutoDel(void) const
	{
		return m_autodel ? TRUE:FALSE;
	}
	
/*protected:*/

	/* virtual */BOOL GetEnd(void) const
	{
		return m_end;
	}
	/* virtual */ void SetEnd(void)
	{
		m_end = TRUE;
	}
	/*virtual*/ DWORD ThreadWaitEnd(DWORD timeout = INFINITE)
	{
		DWORD res = WAIT_FAILED;

		_ASSERTE(IsHandleValid());
		_ASSERTE(!IsCurrentThread());
		if(IsHandleValid()){
			res = Wait(timeout);
		}
		return res;
	}
	/*virtual*/ DWORD ThreadEndAndWait(DWORD timeout = INFINITE)
	{
		T *pThis = static_cast<T*>(this);
		pThis->SetEnd();
		return pThis->ThreadWaitEnd(timeout);
	}

	// return FALSE to avoid this->ThreadProc to be called
	/*virtual*/ BOOL BeforeThreadProc(void)
	{
		return TRUE;
	}
	/*virtual*/ void AfterThreadProc(DWORD res)
	{
		//
	}
	// this function must be implemented

	/* virtual
	DWORD ThreadProc(void)
	{
		_ASSERTE(0);
		return 0;
	}
	*/
	
private:

#ifndef NO5_USE_THREAD_C_API
	static DWORD WINAPI _ExternalThreadProc(LPVOID pParam)
	{
		T *pThis = (T *)pParam;
		DWORD res = 0;

		_ASSERTE(pThis);
		if(pThis){
			if(pThis->BeforeThreadProc()){
				res = pThis->ThreadProc();
				pThis->AfterThreadProc(res);
				if(pThis->GetAutoDel()){
					delete pThis;
				}
			}
		}
		return res;
	}
#else
	static unsigned int __stdcall ExternalThreadProc(LPVOID pParam)
	{
		T *pThis = (T *)pParam;
		DWORD res = 0;

		_ASSERTE(pThis);
		if(pThis){
			if(pThis->BeforeThreadProc()){
				res = pThis->ThreadProc();
				pThis->AfterThreadProc();
				if(pThis->GetAutoDel(res)){
					delete pThis;
				}
			}
		}
		return (int)res;
	}
#endif // NO5_USE_THREAD_C_API

};

class CCriticalSection
{
	mutable CRITICAL_SECTION m_cs;
public:
	CCriticalSection(void){
		::InitializeCriticalSection(&m_cs);
	}
	~CCriticalSection(){
		::DeleteCriticalSection(&m_cs);
	}
	void Lock(void) const {
		::EnterCriticalSection(&m_cs);
	}
	//BOOL TryEnter(void);
	void Unlock(void) const {
		::LeaveCriticalSection(&m_cs);
	}
};

class CFakeCriticalSection
{
	
public:
	void Lock(void) const {
		//::EnterCriticalSection(&m_cs);
	}
	//BOOL TryEnter(void);
	void Unlock(void) const {
		//::LeaveCriticalSection(&m_cs);
	}
};

template <typename T>
class CSincroT
{
	const T &m_cs;
public:
	CSincroT(T &cs):m_cs(cs)
	{
		m_cs.Lock();
	}
	~CSincroT()
	{
		m_cs.Unlock();
	}
};
typedef CSincroT<CCriticalSection> CSincro;

struct CChangeHandleTraits
{
	typedef HANDLE handle_type;

	static BOOL Destroy(HANDLE h)
	{
		return ::FindCloseChangeNotification(h) == TRUE;
	}
	static HANDLE InvalidHandle(void)
	{
		return INVALID_HANDLE_VALUE;
	}
};

template <BOOL t_Owner>
class CChangeEventT:public CSincroObjectT<CChangeHandleTraits,t_Owner>  
{
	typedef CChangeEventT<t_Owner> _ThisClass;
	typedef CSincroObjectT<CChangeHandleTraits,t_Owner>  _BaseClass;
	typedef _BaseClass::handle_type handle_type;
public:
	CChangeEventT(handle_type h):_BaseClass(h)
	{
		//
	}
	CChangeEventT(_ThisClass &obj):_BaseClass(obj)
	{
		//
	}
	_ThisClass & operator = (handle_type h)
	{
		BOOL res = Assign(h);
		ATLASSERT(res);
		return *this;
	}
	_ThisClass & operator = (_ThisClass &rhs)
	{
		BOOL res = Assign(rhs);
		ATLASSERT(res);
		return *this;
	}

	BOOL Create(LPCTSTR path,DWORD filter,BOOL bWatchSubtree)
	{
		handle_type h = ::FindFirstChangeNotification(path,bWatchSubtree,
				filter);
		AttachHandle(h);
		return IsHandleValid();
	}
	BOOL FindNext(void)
	{
		_ASSERTE(IsHandleValid());
		return ::FindNextChangeNotification(GetHandle()) == TRUE;
	}
};

//typedef CChangeEventT<FALSE> CChangeEventHandle;
//typedef CChangeEventT<TRUE> CChangeEvent;

inline BOOL CreateProcessAndWait(LPCSTR file,LPSTR params,
								 LPCSTR CurDir = NULL)
{
	STARTUPINFO startup_info = {0};
	PROCESS_INFORMATION proc_info = {0};
	BOOL res;
	SECURITY_ATTRIBUTES sa = { sizeof(sa),NULL,TRUE };
		
	startup_info.cb = sizeof(startup_info);
	startup_info.dwFlags = STARTF_USESHOWWINDOW;
	startup_info.wShowWindow = SW_MINIMIZE;

	res = ::CreateProcess(file,params,&sa,&sa,TRUE,
		DETACHED_PROCESS,NULL,CurDir,&startup_info,&proc_info);

	if(res && proc_info.hProcess != INVALID_HANDLE_VALUE){
		CSincroObject proc = proc_info.hProcess;
		CSincroObject thread = proc_info.hThread;
		/* res */ proc.WaitWithMsgLoop();
	}
	else
		MessageBox(NULL,"Could not launch ps2.exe","Error",MB_OK);
	return res;
}

} // NO5

#endif // !defined(AFX_HANDLE_H__AFDE0701_1D90_11D9_A17B_AC9DAA311946__INCLUDED_)
