// EventSocket.h: interface for the CEventSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTSOCKET_H__C9FB15C6_1DB2_11D9_A17B_AC9DAA311946__INCLUDED_)
#define AFX_EVENTSOCKET_H__C9FB15C6_1DB2_11D9_A17B_AC9DAA311946__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "eventhandler.h"
#include "wineventhandler.h"
#include "asyncsocketbase.h"

namespace NO5TL
{


/**
	implements an asyncronous socket using WSAEventSelect.
	the socket must be added to an instance of CEventHandler
*/
class CEventSocket : public CAsyncSocketBase,public IEventCallback
{
protected:
	CEvent m_event;
public:
	CEventSocket(ISocketEvents *pSink):CAsyncSocketBase(pSink)
	{
		BOOL res = m_event.CreateEvent(\
			FALSE,	// not signaled
			TRUE);	// manual event
		ATLASSERT(res);
	}
	virtual ~CEventSocket()
	{
		CloseSocket();
	}
	virtual BOOL AsyncSelect(long events = FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE)
	{
		return WSAEventSelect(GetHandle(),(HANDLE)m_event,events) == 0;
	}
	const CEvent & GetEvent(void) const
	{
		return m_event;
	}
	
public:
	// IEventCallback
	virtual void OnEventSignaled(void)
	{
		WSANETWORKEVENTS events = {0};
		int res;
		int error;

		res = ::WSAEnumNetworkEvents(GetHandle(),(HANDLE)m_event,
			&events);
		ATLASSERT(res != SOCKET_ERROR);
		if(res != SOCKET_ERROR){
			if(events.lNetworkEvents & FD_READ){
				error = events.iErrorCode[FD_READ_BIT];
				if(error)
					_OnError(error);
				_OnRead(error);
			}
			if(events.lNetworkEvents & FD_WRITE){
				error = events.iErrorCode[FD_WRITE_BIT];
				if(error)
					_OnError(error);
				_OnWrite(error);
			}
			if(events.lNetworkEvents & FD_CONNECT){
				error = events.iErrorCode[FD_CONNECT_BIT];
				if(error)
					_OnError(error);
				_OnConnect(error);
			}
			if(events.lNetworkEvents & FD_CLOSE){
				error = events.iErrorCode[FD_CLOSE_BIT];
				if(error)
					_OnError(error);
				_OnClose(error);
			}
			if(events.lNetworkEvents & FD_ACCEPT){
				error = events.iErrorCode[FD_ACCEPT_BIT];
				if(error)
					_OnError(error);
				_OnAccept(error);
			}
			if(events.lNetworkEvents & FD_OOB){
				error = events.iErrorCode[FD_OOB_BIT];
				if(error)
					_OnError(error);
				_OnOutOfBand(error);
			}
		}
	}
	virtual void OnEventAdded(void){}
	virtual void OnEventRemoved(void){}
	virtual void OnEventTimeOut(void){}
};

// this class handles adding and removing the event to a static
// event handler
// use other event handlers when you want to overide some of 
// CThreadImpl callbacs

template <class TEventHandler>
class CEventSocket2T : public CEventSocket
{
	static TEventHandler m_handler;
	static LONG m_count;
public:
	CEventSocket2T(ISocketEvents *pSink):CEventSocket(pSink)
	{
		m_count++;
	}
	virtual ~CEventSocket2T()
	{
		CloseSocket();
		LONG count = --m_count;

		ATLASSERT(count >= 0);
		if(count == 0 && m_handler.IsHandleValid()){
			if(m_handler.IsCurrentThread())
				m_handler.SetEnd();
			else
				m_handler.ThreadEndAndWait(5000);
			m_handler.CloseHandle();
		}
	}
	virtual BOOL AsyncSelect(long events = FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE)
	{
		BOOL res = GetHandler().FindEvent(GetEvent());

		if(!res){
			res = GetHandler().AddEvent(GetEvent(),this);
		}
		else{
			// ok
		}

		if(res){
			res = CEventSocket::AsyncSelect(events);
		}
		return res;
	}
	virtual BOOL CloseSocket()
	{
		if(IsHandleValid()){
			BOOL res = GetHandler().RemoveEvent(GetEvent());
			ATLASSERT(res);
		}
		return CEventSocket::CloseSocket();
	}
protected:
	static TEventHandler & GetHandler()
	{
		BOOL res;
		if(!m_handler.IsHandleValid()){
			res = m_handler.CreateThread();
			ATLASSERT(res);
		}
		return m_handler;
	}
};

typedef CEventSocket2T<CEventHandler> CEventSocket2;
typedef CEventSocket2T<CWinEventHandler> CEventSocket3;

template <class TEventHandler>
TEventHandler CEventSocket2T<TEventHandler>::m_handler;
template <class TEventHandler>
LONG CEventSocket2T<TEventHandler>::m_count = 0;

} // NO5TL

#endif // !defined(AFX_EVENTSOCKET_H__C9FB15C6_1DB2_11D9_A17B_AC9DAA311946__INCLUDED_)
