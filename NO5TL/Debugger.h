// Debugger.h: interface for the CDebugger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEBUGGER_H__671218E3_7666_11D7_A17A_903B9E301B33__INCLUDED_)
#define AFX_DEBUGGER_H__671218E3_7666_11D7_A17A_903B9E301B33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLBASE_H__
#error debugger.h requires atlbase.h
#endif
#ifndef _INC_TOOLHELP32
#error debugger.h requires <tlhelp32.h>
#endif

#include "handle.h"

namespace NO5TL
{
class CProcessEnumerator
{
	struct ToolhelpSnapshot
	{
		HANDLE m_handle;
		//
		bool IsValidHandle(void) const
		{
			return m_handle != (HANDLE)-1;
		}
	public:
		ToolhelpSnapshot(DWORD flags = TH32CS_SNAPPROCESS,DWORD processId = 0)
		{
			m_handle = ::CreateToolhelp32Snapshot(flags,processId);
			ATLASSERT(IsValidHandle());
		}
		~ToolhelpSnapshot()
		{
			if(IsValidHandle())
				CloseHandle(m_handle);
		}
		bool ProcessFirst(PROCESSENTRY32 *pe)
		{
			return ::Process32First(m_handle,pe) == TRUE;
		}
		bool ProcessNext(PROCESSENTRY32 *pe)
		{
			return ::Process32Next(m_handle,pe) == TRUE;
		}
	};
public:
	struct ProcessEntry:public PROCESSENTRY32
	{
	public:
		ProcessEntry(void)
		{
			::ZeroMemory(this,sizeof(PROCESSENTRY32));
			dwSize = sizeof(PROCESSENTRY32);
		}
		PROCESSENTRY32 * operator&()
		{
			return (this);
		}
	};
	CSimpleArray<ProcessEntry> m_procs;
	int Enum(void)
	{
		ToolhelpSnapshot th;
		ProcessEntry pe;
		bool res;
		int count = 0;

		res = th.ProcessFirst(&pe);
		while(res){
			count++;
			m_procs.Add(pe);
			res = th.ProcessNext(&pe);
		}
		return count;
	}
};

class CDebugger:private CThread
{
	CSincroObject m_process;
	DWORD m_process_id;
	//
	static LPSTR GetDebugStringA(HANDLE hProcess,
		OUTPUT_DEBUG_STRING_INFO &si)
	{
		LPTSTR buf = new char[si.nDebugStringLength + 1];
		BOOL res;

		::ZeroMemory(buf,si.nDebugStringLength + 1);
		ATLASSERT(buf && si.fUnicode == 0);
		// doesnt handle UNICODE
		if(si.fUnicode == 0){
			res = ::ReadProcessMemory(hProcess,si.lpDebugStringData,
				buf,si.nDebugStringLength,NULL);
			if(!res){
				delete []buf;
				buf = NULL;
			}
		}
		return buf;
	}
	// used when si.fUnicode == 1, returns UNICODE string
	static LPWSTR GetDebugStringW(HANDLE hProcess,
		OUTPUT_DEBUG_STRING_INFO &si)
	{
		WORD chars = si.nDebugStringLength/2 + 1;
		LPWSTR buf = new WCHAR[chars];
		BOOL res;

		::ZeroMemory(buf,chars * 2 );
		ATLASSERT(buf && si.fUnicode != 0);
		// doesnt handle ANSI
		if(si.fUnicode != 0){
			res = ::ReadProcessMemory(hProcess,si.lpDebugStringData,
				buf,si.nDebugStringLength,NULL);
			if(!res){
				delete []buf;
				buf = NULL;
			}
		}
		return buf;
	}
	BOOL _init(void)
	{
		BOOL res = FALSE;
		// open process - this has to be called from the thread
		// that will wait for debug events ( i believe so )
		m_process = ::OpenProcess(PROCESS_ALL_ACCESS,FALSE,
			m_process_id);
		if(m_process.IsHandleValid()){
			// set us as debugger
			res = ::DebugActiveProcess(m_process_id);
		}
		return res;
	}
public:
	virtual DWORD ThreadProc(void)
	{
		DWORD res_wait;
		BOOL res;
		DEBUG_EVENT debug_event = {0};

		if(!_init())
			return -1;

		while(!GetEnd()){
			res_wait = m_process.Wait();
			if(res_wait == WAIT_OBJECT_0){
				break;
			}
			res = ::WaitForDebugEvent(&debug_event,0);
			if(res){
				switch(debug_event.dwDebugEventCode){
					case OUTPUT_DEBUG_STRING_EVENT:
					{
						OUTPUT_DEBUG_STRING_INFO &si = debug_event.u.DebugString;

						if(si.fUnicode == 0){
							LPSTR p = GetDebugStringA(m_process,si);
							if(p){
								OnOutputDebugString(p);
								delete []p;
							}
						}
						else{
							LPWSTR p = GetDebugStringW(m_process,si);
							if(p){
								OnOutputDebugString(p);
								delete []p;
							}
						}
						break;
					}
					default:
						break;
				}
				res = ::ContinueDebugEvent(debug_event.dwProcessId,
					debug_event.dwThreadId,0);
				ATLASSERT(res);
			}
		}
		return 0;
	}
public:
	CDebugger(void)
	{
		//
	}
	BOOL Init(DWORD process_id)
	{
		m_process_id = process_id;
		return CreateThread();
	}
protected:
	virtual void OnOutputDebugString(LPSTR p) = 0;
	virtual void OnOutputDebugString(LPWSTR p) = 0;
};


} // NO5TL

#endif // !defined(AFX_DEBUGGER_H__671218E3_7666_11D7_A17A_903B9E301B33__INCLUDED_)
