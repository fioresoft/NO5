// EventHandler.h: interface for the CEventHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTHANDLER_H__AFDE0704_1D90_11D9_A17B_AC9DAA311946__INCLUDED_)
#define AFX_EVENTHANDLER_H__AFDE0704_1D90_11D9_A17B_AC9DAA311946__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLBASE_H__
#error eventhandler.h requires atlbase to be included first
#endif

#include "utils.h"
#include "handle.h"


namespace NO5TL
{

template <typename TKey,typename TVal>
BOOL CopySimpleMap(CSimpleMap<TKey,TVal> &to,const CSimpleMap<TKey,TVal> &from)
{
	const int size = from.GetSize();
	int i;
	BOOL res = TRUE;

	to.RemoveAll();
	for(i=0;i<size && res;i++){
		res = to.Add(from.GetKeyAt(i),from.GetValueAt(i));
		ATLASSERT(res);
	}
	return res;
}

struct IEventCallback
{
	virtual void OnEventSignaled(void) = 0;
	virtual void OnEventTimeOut(void) = 0;
	virtual void OnEventAdded(void) = 0;
	virtual void OnEventRemoved(void) = 0;
};

template <class T>
class CEventPulserT
{
	T &m_event;
public:
	CEventPulserT(T &event):m_event(event)
	{
		m_event.SetEvent();
	}
	~CEventPulserT()
	{
		m_event.ResetEvent();
	}
};
typedef CEventPulserT<CEvent> Pulser;

template <class T>
class CEventHandlerT : public CThreadImpl<T>
{
	typedef CEventHandlerT<T> _ThisClass;
	typedef CThreadImpl<T> _BaseClass;
	CSimpleMap<HANDLE,IEventCallback*> m_map;
	CEvent m_WakeEvent;
	CCriticalSection m_cs;
	DWORD m_timeout;
public:
	CEventHandlerT(BOOL bAutoDel = FALSE):_BaseClass(bAutoDel)
	{
		BOOL res = m_WakeEvent.CreateEvent(\
			FALSE,	// signaled
			TRUE,	// manual-reset
			"WakeEvent");
		if(res){
			CSincro s(m_cs);
			res = m_map.Add((HANDLE)m_WakeEvent,NULL);
			if(res){
				HANDLE h = HANDLE(m_WakeEvent);
				int index = m_map.FindKey(h);
				res = (index == 0);
			}
		}
		m_timeout = INFINITE;
		ATLASSERT(res);
	}
	~CEventHandlerT()
	{
		ATLTRACE("CEventHandler::~CEventHandler");
	}
	BOOL AddEvent(HANDLE hEvent,IEventCallback *p)
	{
		Pulser pulser(m_WakeEvent);
		CSincro s(m_cs);
		BOOL res = m_map.Add(hEvent,p);
		if(res && p)
			p->OnEventAdded();
		return res;
	}
	BOOL RemoveEvent(HANDLE hEvent)
	{
		Pulser pulser(m_WakeEvent);
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
		Pulser pulser(m_WakeEvent);
		CSincro s(m_cs);
		
		return m_map.FindKey(hEvent) >= 0;
	}
	// overiding CThreadImpl::SetEnd
	void SetEnd(void)
	{
		m_WakeEvent.SetEvent();
		_BaseClass::SetEnd();
	}
	DWORD SetTimeOut(DWORD timeout)
	{
		Pulser pulser(m_WakeEvent);
		CSincro s(m_cs);
		DWORD res = m_timeout;
		m_timeout = timeout;
		return res;
	}
	DWORD GetTimeOut(void) const
	{
		return m_timeout;
	}

public: // public but logicaly its private

	DWORD ThreadProc(void)
	{
		while(!GetEnd()){
			WaitEvent();
		}
		return 0;
	}
	void AfterThreadProc(DWORD res)
	{
		CSincro s(m_cs);
		int size = m_map.GetSize();
		ATLTRACE("CEventHandler::AfterThreadProc. %d events in the map\n",
			size);
		if(size){
			m_map.RemoveAll();
		}
	}
private:
	void WaitEvent(void)
	{
		CSincro s(m_cs);
		int size = m_map.GetSize();
		IEventCallback *p = NULL;

		if(size){
			DWORD res;
			
			res = ::WaitForMultipleObjects((DWORD)size,
				m_map.m_aKey,FALSE,m_timeout);
			
			if(res == WAIT_FAILED){
				ATLTRACE("WaitForMultipleObjects failed: %d",
					::GetLastError());
			}
			else if(res == WAIT_TIMEOUT){
				CSimpleMap<HANDLE,IEventCallback*> tmp;
				BOOL tmpres = CopySimpleMap(tmp,m_map);
				
				ATLASSERT(tmpres);

				if(tmpres){
					const int tmpsize = tmp.GetSize();
					int i;

					for(i=1;i<tmpsize;i++){
						p = tmp.GetValueAt(i);
						if(p){
							p->OnEventTimeOut();
						}
					}
				}
			}
			else{
				int index = (int)res - (int)WAIT_OBJECT_0;
				ATLASSERT(index >= 0 && index < size);
				if(index >= 1){ // index 0 is the "wake-up" event
					p = m_map.GetValueAt(index);
					p->OnEventSignaled();
				}
			}
		}
	}
};

// this class can be used as it is
class CEventHandler : public CEventHandlerT<CEventHandler>
{
	typedef CEventHandlerT<CEventHandler> _BaseClass;
public:
	CEventHandler(BOOL bAutoDel = FALSE):_BaseClass(bAutoDel)
	{
		//
	}
};


} // NO5TL

#endif // !defined(AFX_EVENTHANDLER_H__AFDE0704_1D90_11D9_A17B_AC9DAA311946__INCLUDED_)
