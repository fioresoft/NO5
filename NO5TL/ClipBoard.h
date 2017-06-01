// ClipBoard.h: interface for the CClipBoard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIPBOARD_H__2BB37AA1_0995_11D9_A17B_E0FF6E33774E__INCLUDED_)
#define AFX_CLIPBOARD_H__2BB37AA1_0995_11D9_A17B_E0FF6E33774E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLMISC_H__
#error clipboard.h requires atlmisc because it uses CString
#endif

namespace NO5TL
{

/**  
	class useful only to open and close clipboard automatically and
	to simplify setting and get text to it
*/
	
class CClipBoard  
{
	BOOL m_open;
public:
	CClipBoard():m_open(FALSE)
	{
		//
	}
	CClipBoard(HWND hOwner):m_open(FALSE)
	{
		BOOL res = Open(hOwner);
		ATLASSERT(res);
	}
	virtual ~CClipBoard()
	{
		BOOL res = Close();
		//ATLASSERT(res);
	}
	/**
		opens the clipboard for examination and prevents other
		applications from modifying the clipboard content
		OpenClipboard fails if another window has the clipboard open
	*/
	BOOL Open(HWND hOwner = NULL)
	{
		if(m_open){
			Close();
		}
		ATLASSERT(!m_open);
		return m_open = ::OpenClipboard(hOwner);
	}
	/** enables other windows to access the clipboard */
	BOOL Close(void)
	{
		if(m_open){
			m_open = !::CloseClipboard();
		}
		return !m_open;
	}
	/** 
		frees handles to data in the clipboard. Assigns ownership 
		of the to the window that currently has the clipboard
		open
	*/
	BOOL Empty(void) const
	{
		BOOL res = FALSE;

		if(m_open){
			res = EmptyClipboard();
		}
		return res;
	}
	/** 
		After SetClipboardData is called, the system owns the
		object identified by the hMem parameter.
		if hMem = NULL the window must process the WM_RENDERFORMAT
		and WM_RENDERALLFORMATS
	*/
	HANDLE SetData(UINT uFormat,HANDLE hMem = NULL) const
	{
		HANDLE hRes = NULL;

		if(m_open){
			if(Empty())
				hRes = ::SetClipboardData(uFormat,hMem);
		}
		return hRes;
	}
	HANDLE GetData(UINT uFormat) const
	{
		HANDLE hRes = NULL;

		if(m_open){
			hRes = ::GetClipboardData(uFormat);
		}
		return hRes;
	}
	HANDLE SetText(LPCTSTR text) const
	{
		HGLOBAL hMem;
		BOOL tmp;
		
		if(text && m_open){
			hMem = ::GlobalAlloc(GHND,lstrlen(text) + sizeof(TCHAR));
			if(hMem){
				TCHAR *p = (TCHAR *)::GlobalLock(hMem);

				if(p){
					lstrcpy(p,text);
					::GlobalUnlock(hMem);
					tmp = Empty();
					ATLASSERT(tmp);
#ifdef _UNICODE
					hMem = SetData(CF_TEXT,hMem);
#else
					hMem = SetData(CF_UNICODETEXT,hMem);
#endif
				}
				else{
					::GlobalFree(hMem);
					hMem = NULL;
				}
			}
		}
		return hMem;
	}
	CString GetText(void) const
	{
		CString res;
		HGLOBAL hMem = NULL;

		if(m_open){
#ifdef _UNICODE
			if(IsFormatAvailable(CF_UNICODETEXT)){
				hMem = GetData(CF_UNICODETEXT);
			}
#else
			if(IsFormatAvailable(CF_TEXT)){
				hMem = GetData(CF_TEXT);
			}
#endif
			if(hMem){
				res = (TCHAR *)::GlobalLock(hMem);
				::GlobalUnlock(hMem);
			}
		}
		return res;
	}
	BOOL IsFormatAvailable(UINT uFormat) const
	{
		return ::IsClipboardFormatAvailable(uFormat);
	}
};

// saves the TEXT stored in the clipboard, if any.
class CClipBoardSaver
{
	CString m_text;
	HWND m_hWnd;
public:
	CClipBoardSaver(HWND hWnd):m_hWnd(hWnd)
	{
		CClipBoard cb(hWnd);
		m_text = cb.GetText();
	}
	~CClipBoardSaver()
	{
		if(!m_text.IsEmpty()){
			CClipBoard cb(m_hWnd);	
			cb.SetText(m_text);
		}
	}
};

}	// NO5TL

#endif // !defined(AFX_CLIPBOARD_H__2BB37AA1_0995_11D9_A17B_E0FF6E33774E__INCLUDED_)
