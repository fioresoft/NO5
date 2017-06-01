// WinEventHandler.h: interface for the CWinEventHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINEVENTHANDLER_H__A50B44A1_505D_11D9_A17B_CD5DC1F1C73C__INCLUDED_)
#define AFX_WINEVENTHANDLER_H__A50B44A1_505D_11D9_A17B_CD5DC1F1C73C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __ATLAPP_H__
	#error wineventhandler.h requires atlapp.h
#endif //__ATLAPP_H__

#include "winthread.h"
#include "eventhandler.h"

namespace NO5TL
{

template <class T>
class CWinEventHandlerT : public CWinThreadT<T>
{
	typedef CWinEventHandlerT<T> _ThisClass;
	typedef CWinThreadT<T> _BaseClass;
	CSimpleMap<HANDLE,IEventCallback*> m_map;
	CCriticalSection m_cs;
	const DWORD m_dwCoInit;
public:
	CWinEventHandlerT(DWORD dwCoInit = COINIT_APARTMENTTHREADED ,
		BOOL bAutoDel = FALSE):_BaseClass(bAutoDel),
		m_dwCoInit(dwCoInit)
	{
		//
	}
	~CWinEventHandlerT()
	{
		ATLTRACE("CEventHandler::~CEventHandler");
	}
	BOOL AddEvent(HANDLE hEvent,IEventCallback *p)
	{
		CSincro s(m_cs);
		BOOL res = m_map.Add(hEvent,p);
		if(res && p)
			p->OnEventAdded();
		return res;
	}
	BOOL RemoveEvent(HANDLE hEvent)
	{
		CSincro s(m_cs);
		BOOL res = FALSE;
		
		IEventCallback *p = m_map.Lookup(hEvent);
		res = m_map.Remove(hEvent);
		if(res && p){
			p->OnEventRemoved();
		}
		return res;
	}
	BOOL FindEvent(HANDLE hEvent)
	{
		CSincro s(m_cs);
		
		return m_map.FindKey(hEvent) >= 0;
	}
	// overiding CThreadImpl::SetEnd
	void SetEnd(void)
	{
		_BaseClass::SetEnd();
	}
	DWORD SetTimeOut(DWORD timeout)
	{
		ATLASSERT(0 && "not implemented");
		return 0;
	}
	DWORD GetTimeOut(void) const
	{
		ATLASSERT(0 && "not implemented");
		return 0;
	}

public: // public but logicaly its private

	BOOL DoSomething(void)
	{
		CheckEvents();
		return TRUE;
	}

	BOOL BeforeThreadProc(void)
	{
		HRESULT hr;
#ifdef _WIN32_DCOM
		hr = ::CoInitializeEx(NULL,m_dwCoInit);
#else
		hr = ::CoInitialize(NULL);
#endif
		ATLASSERT(SUCCEEDED(hr));
		return TRUE;
	}

	void AfterThreadProc(DWORD res)
	{
		CSincro s(m_cs);
		int size = m_map.GetSize();
		ATLTRACE("CWinEventHandler::AfterThreadProc. %d events in the map\n",
			size);
		if(size){
			m_map.RemoveAll();
		}
		::CoUninitialize();
	}
private:
	void CheckEvents(void)
	{
		CSincro s(m_cs);
		int size = m_map.GetSize();
		IEventCallback *p = NULL;

		if(size){
			DWORD res;
			
			res = ::WaitForMultipleObjects((DWORD)size,
				m_map.m_aKey,FALSE,0);
			
			if(res == WAIT_FAILED){
				ATLTRACE("WaitForMultipleObjects failed: %d",
					::GetLastError());
			}
			else if(res == WAIT_TIMEOUT){
				return;
			}
			else if(\
				( int(res - WAIT_ABANDONED_0) >= 0 ) \
				&& \
				( int(res - WAIT_ABANDONED_0) < size) \
				)
			{
				// some handle abandoned
				return;
			}
			else{
				int index = (int)res - (int)WAIT_OBJECT_0;
				ATLASSERT(index >= 0 && index < size);
				if(index >= 0 && index < size){
					p = m_map.GetValueAt(index);
					p->OnEventSignaled();
				}
			}
		}
	}
};

class CWinEventHandler : public CWinEventHandlerT<CWinEventHandler>
{
	typedef CWinEventHandlerT<CWinEventHandler> _BaseClass;
public:
	CWinEventHandler(\
		DWORD m_dwCoInit = COINIT_APARTMENTTHREADED,
		BOOL bAutoDel = FALSE):\
		_BaseClass(m_dwCoInit,bAutoDel)
	{
		//
	}
};


} // NO5TL

#endif // !defined(AFX_WINEVENTHANDLER_H__A50B44A1_505D_11D9_A17B_CD5DC1F1C73C__INCLUDED_)
