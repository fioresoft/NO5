#ifndef NO5TL_WINTHREAD_H
#define NO5TL_WINTHREAD_H


#ifndef __ATLAPP_H__
	#error winthread.h requires atlapp.h
#endif //__ATLAPP_H__
#include "handle.h"

namespace NO5TL
{

template <class T>
class CWinThreadT : public CThreadT<TRUE>,public CMessageLoop
{
	typedef CThreadT<TRUE> _BaseClass;
	typedef CWinThreadT<T> _ThisClass;
		//
	DWORD m_id;
	const BOOL m_autodel;
public:
	CWinThreadT(BOOL autodel = FALSE):m_autodel(autodel)
	{
		m_id = 0;
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
	BOOL PostThreadMessage(UINT msg,WPARAM wParam = 0,LPARAM lParam = 0)
	{
		return ::PostThreadMessage(m_id,msg,wParam,lParam);
	}
	BOOL SetEnd(void)
	{
		return PostThreadMessage(WM_QUIT);
	}

	// CAREFUL : THIS WONT PICK MESSAGES
	/*virtual */DWORD ThreadWaitEnd(DWORD timeout = INFINITE)
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
	
		
/*protected:*/

	virtual BOOL OnIdle(int nIdleCount)
	{
		BOOL res;
		T *pT = static_cast<T*>(this);

		if(res = pT->DoSomething()){
			CMessageLoop::OnIdle(nIdleCount);
		}
		return res;
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
	BOOL DoSomething(void)
	{
		_ASSERTE(0);
		return TRUE;
	}
	*/
private:

#ifndef NO5_USE_THREAD_C_API
	static DWORD WINAPI _ExternalThreadProc(LPVOID pParam)
#else
	static unsigned int __stdcall ExternalThreadProc(LPVOID pParam)
#endif // NO5_USE_THREAD_C_API
	{
		T *pThis = (T *)pParam;
		int res = 0;
	
		ATLASSERT(pThis);
		if(pThis){
			if(pThis->BeforeThreadProc()){
				res = pThis->Run();
				pThis->AfterThreadProc(res);
				if(pThis->GetAutoDel()){
					delete pThis;
				}
			}
		}
		return res;
	}
};

} // NO5TL

#endif //NO5TL_WINTHREAD_H
