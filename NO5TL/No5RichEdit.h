// No5RichEdit.h: interface for the CNo5RichEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NO5RICHEDIT_H__21621BE2_3E94_11D9_A17B_C6D6ABB76442__INCLUDED_)
#define AFX_NO5RICHEDIT_H__21621BE2_3E94_11D9_A17B_C6D6ABB76442__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <no5tl\dll.h>	// for CLibrary

#ifndef __ATLCTRLS_H__
#error no5richedit requires atlcrls.h to be included first
#endif

#ifndef __ATLMISC_H__
#error no5richedit requires atlmisc.h to be included first
#endif

#include "clipboard.h"
#include "dll.h"

namespace NO5TL
{

class CRichEditOleCallback : public IRichEditOleCallback
{
public:

	STDMETHODIMP_(ULONG) AddRef(void)
	{
		return 1;
	}
	STDMETHODIMP_(ULONG) Release(void)
	{
		return 1;
	}
	STDMETHODIMP QueryInterface(REFIID riid,LPVOID *ppv)
	{
		HRESULT hr;

		if(riid == IID_IUnknown || riid == IID_IRichEditOleCallback){
			*ppv = (IUnknown *)this;
			AddRef();
			hr = S_OK;
		}
		else
			hr = E_NOINTERFACE;
		return hr;
	}

	// IRichEditOleCallback Interface Members
	STDMETHODIMP GetNewStorage(LPSTORAGE* ppStg);
	STDMETHODIMP GetInPlaceContext(LPOLEINPLACEFRAME* ppFrame,
								   LPOLEINPLACEUIWINDOW* ppDoc,
								   LPOLEINPLACEFRAMEINFO pFrameInfo);
	STDMETHODIMP ShowContainerUI(BOOL fShow);
	STDMETHODIMP QueryInsertObject(LPCLSID pclsid, LPSTORAGE pStg, LONG cp);
	STDMETHODIMP DeleteObject(LPOLEOBJECT pOleObj);
	STDMETHODIMP QueryAcceptData(LPDATAOBJECT pDataObj, CLIPFORMAT* pcfFormat,
								 DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
	STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode);
	STDMETHODIMP GetClipboardData(CHARRANGE* pchrg, DWORD reco, 
								  LPDATAOBJECT* ppDataObject);
	STDMETHODIMP GetDragDropEffect(BOOL fDrag, DWORD grfKeyState,
								   LPDWORD pdwEffect);
	STDMETHODIMP GetContextMenu(WORD seltype, LPOLEOBJECT pOleObj,
							    CHARRANGE* pchrg, HMENU* phMenu);

private:
	static HRESULT _GetNewStorage(LPSTORAGE* ppStg);

};


/////////////////////////////////////////////////////////////////////////////
// IRichEditOleCallback Interface Members
//

//
//  FUNCTION:   CRichEditOleCallback::GetNewStorage
//
//  PURPOSE:    Gets a storage for a new object.
//
//  PARAMETERS:
//      ppStg - Where to return the storage.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//
//  COMMENTS:
//      Implemented in the document object.
//

inline HRESULT CRichEditOleCallback::GetNewStorage(LPSTORAGE* ppStg)
{
	HRESULT hr;
	hr = _GetNewStorage(ppStg);
	return hr;
}


//
//  FUNCTION:   CRichEditOleCallback::GetInPlaceContext
//
//  PURPOSE:    Gets the context information for an in place object.
//
//  PARAMETERS:
//      ppFrame    - Pointer to the frame window object.
//		ppDoc      - Pointer to the document window object.
//		pFrameInfo - Pointer to the frame window information.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//
//

inline 
HRESULT CRichEditOleCallback::GetInPlaceContext(\
										LPOLEINPLACEFRAME* ppFrame,
										LPOLEINPLACEUIWINDOW* ppDoc,
										LPOLEINPLACEFRAMEINFO pFrameInfo)
{
	return E_NOTIMPL;
}


//
//  FUNCTION:	CRichEditOleCallback::ShowContainerUI
//
//  PURPOSE:	Handles the showing or hiding of the container UI.
//
//  PARAMETERS:
//      fShow - TRUE if the function should display the UI, FALSE to hide it.
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//

inline 
HRESULT CRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
	return E_NOTIMPL;
}


//
//  FUNCTION:	CRichEditOleCallback::QueryInsertObject
//
//  PURPOSE:    Called to ask whether an object should be inserted.  We're
//				going to allow everything to be inserted.
//

inline 
HRESULT CRichEditOleCallback::QueryInsertObject(\
												LPCLSID /* pclsid */,
												LPSTORAGE /* pStg */,
												LONG /* cp */)
{
	return S_OK;
}


//
//  FUNCTION:   CRichEditOleCallback::DeleteObject
//
//  PURPOSE:    Notification that an object is going to be deleted.
//

inline 
HRESULT CRichEditOleCallback::DeleteObject(\
										   LPOLEOBJECT /* pOleObj */)
{
	return E_NOTIMPL;
}


//
//  FUNCTION:   CRichEditOleCallback::QueryAcceptData
//
//  PURPOSE:    Called to ask whether the data that is being pasted or
//				dragged should be accepted.  We accept everything.
//

inline 
HRESULT CRichEditOleCallback::QueryAcceptData(LPDATAOBJECT /* pDataObj */,
											  CLIPFORMAT* /* pcfFormat */,
											  DWORD /* reco */,
											  BOOL /* fReally */,
											  HGLOBAL /* hMetaPict */)
{
	return S_OK;
}


//
//  FUNCTION:   CRichEditOleCallback::ContextSensitiveHelp
//
//  PURPOSE:    Tells the application that it should transition into or out of
//				context sensitive help mode.  We don't implement help at all
//				so blow this off.
//

inline 
HRESULT CRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}


//
//  FUNCTION:   CRichEditOleCallback::GetClipboardData
//
//  PURPOSE:    Called when the rich edit needs to provide clipboard data.
//				We'll let the rich edit control handle this one.
//

inline 
HRESULT CRichEditOleCallback::GetClipboardData(CHARRANGE* /* pchrg */,
											   DWORD /* reco */,
											   LPDATAOBJECT* ppDataObject)

{
   	*ppDataObject = NULL;
	return E_NOTIMPL;
}


//
//  FUNCTION:   CRichEditOleCallback::GetDragDropEffect
//
//  PURPOSE:    Allows us to determine the drag cursor effect when the user
//				is dragging something over us.  We'll let the rich edit control
//				handle this one as well.
//
inline 
HRESULT CRichEditOleCallback::GetDragDropEffect(BOOL /* fDrag */,
												DWORD /* grfKeyState */,
												LPDWORD /* pdwEffect */)
{
	return E_NOTIMPL;
}


//
//  FUNCTION:   CRichEditOleCallback::GetContextMenu
//
//  PURPOSE:    Creates the context menu for alternate mouse clicks in the
//				RichEdit control.
//
//  PARAMETERS:
//      seltype - Selection type
//		pOleObj - IOleObject interface of the selected object, if any
//		pchrg   - Selection range
//		phMenu  - Place to return the constructed menu
//
//  RETURN VALUE:
//      Returns an HRESULT signifying success or failure.
//

inline 
HRESULT CRichEditOleCallback::GetContextMenu(WORD seltype,
											 LPOLEOBJECT pOleObj,
											 CHARRANGE* pchrg,
											 HMENU* phMenu)
{
	return E_NOTIMPL;
}


	//
//  FUNCTION:   CDocument::GetNewStorage
//
//  PURPOSE:    Retrieves a new storage within the document's storage.
//
//  PARAMETERS:
//      ppStg - Pointer to an IStorage interface where the fucntion should
//              return the new storage.
//
//  RETURN VALUE:
//      Returns an HRESULT signfiying success or failure.
//

inline 
HRESULT CRichEditOleCallback::_GetNewStorage(LPSTORAGE* ppStg)
{
    if (!ppStg)
        return E_INVALIDARG;

    *ppStg = NULL;

    //
    // We need to create a new storage for an object to occupy.  We're going
    // to do this the easy way and just create a storage on an HGLOBAL and let
    // OLE do the management.  When it comes to saving things we'll just let
    // the RichEdit control do the work.  Keep in mind this is not efficient, 
    // but this program is just for demonstration.
    //

    LPLOCKBYTES pLockBytes;
    HRESULT hr = CreateILockBytesOnHGlobal(NULL, TRUE, &pLockBytes);
    if (FAILED(hr))
        return hr;

    hr = StgCreateDocfileOnILockBytes(pLockBytes, 
                                      STGM_SHARE_EXCLUSIVE | STGM_CREATE |
                                          STGM_READWRITE,
                                      0,
                                      ppStg);
    pLockBytes->Release();
    return (hr);
}


class CRichEditLib : public CLibrary
{
	BOOL m_loaded;
public:
	CRichEditLib():CLibrary(NULL)
	{
		BOOL res = LoadLibrary(CRichEditCtrl::GetLibraryName());
		ATLASSERT(res);
	}
	BOOL IsLoaded(void) const
	{
		return m_loaded;
	}
};
template <class TBase = CWindow>
class CNo5RichEditT : public CRichEditCtrlT<TBase>
{
	typedef CRichEditCtrlT<TBase> _BaseClass;
	typedef CNo5RichEditT<TBase> _ThisClass;

	// helper classes used to stream in/out from/to a buffer

	// our buffer is filled by the edit
	class CStreamOut
	{
		char *m_buf;
		LONG m_pos;
	public:
		CStreamOut(char *buf)
		{
			m_buf = buf;
			m_pos = 0;
		}
		LONG Write(LPVOID pBuf,LONG cb)
		{
			memcpy(&m_buf[m_pos],pBuf,cb);
			m_pos += cb;
			return cb;
		}
		LONG GetPos(void) const
		{
			return m_pos;
		}
	};

	// the control's buffer is filled by our buffer
	class CStreamIn
	{
		const char *m_buf;
		LONG m_pos;
		LONG m_BufLen;
	public:
		CStreamIn(const char *buf,LONG BufLen)
		{
			m_buf = buf;
			m_pos = 0;
			m_BufLen = BufLen;
		}
		LONG Read(LPVOID pBuf,LONG cb)
		{
			LONG copy = m_BufLen - m_pos;

			copy = min(cb,copy);
			memcpy(pBuf,&m_buf[m_pos],copy);
			m_pos += copy;
			return copy;
		}
		LONG GetPos(void) const
		{
			return m_pos;
		}
	};
protected:
	DWORD GetCharFormat(CHARFORMAT &cf,BOOL bSel) const
	{
		if(bSel)
			return GetSelectionCharFormat(cf);
		else
			return GetDefaultCharFormat(cf);
	}
	/** 
		Turns the effect on or off. For example, to turn on the
		bold in the current selection we would call:
		SetEffect(CFE_BOLD,CFM_BOLD,SCF_SELECTION). Now to turn it
		off SetEffect(0,CFM_BOLD,SCF_SELECTION)
	*/
	BOOL SetEffect(DWORD dwEffect,DWORD dwMask,UINT uFlags)
	{
		CHARFORMAT2 cf;
		BOOL res;
		
		cf.cbSize = sizeof(cf);
		cf.dwEffects = dwEffect;
		cf.dwMask = dwMask;
		res = SetCharFormat(cf,(WORD)uFlags);
		// since we dont always check this return value
		ATLASSERT(res);
		return res;
	};
	DWORD GetEffect(DWORD dwMask,BOOL bSel) const
	{
		CHARFORMAT2 cf;

		cf.cbSize = sizeof(cf);
		cf.dwMask = dwMask;
		GetCharFormat(cf,bSel);
		return cf.dwEffects;
	};
public:
	CNo5RichEditT(HWND hWnd = NULL):_BaseClass(hWnd)
	{
		//
	}
	_ThisClass & operator = (HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}
	//
	BOOL SetTextColor(COLORREF cr,UINT uFlags = SCF_SELECTION)
	{
		CHARFORMAT cf = {0};	// size is set in SetCharFormat

		cf.dwMask = CFM_COLOR;
		cf.dwEffects = 0;
		cf.crTextColor = cr;
		return SetCharFormat(cf,uFlags);
	}
	COLORREF GetTextColor(BOOL bSel = TRUE) const
	{
		CHARFORMAT cf = {0};

		cf.dwMask = CFM_COLOR;
		GetCharFormat(cf,bSel);
		return cf.crTextColor;
	}
	/** Turns auto-color on or off */
	void SetAutoColor(BOOL bSet,UINT uFlags = SCF_SELECTION) 
	{
		DWORD dwEffect = bSet?CFE_AUTOCOLOR:0;
		SetEffect(dwEffect,CFM_COLOR,uFlags);
	}
	/** Checks if the auto-color is turned on or off */
	BOOL GetAutoColor(BOOL bSel) const
	{
		DWORD dwEffect = GetEffect(CFM_COLOR,bSel);
		return (dwEffect & CFE_AUTOCOLOR)?TRUE:FALSE;
	}
	BOOL SetTextBkColor(COLORREF cr,UINT uFlags = SCF_SELECTION)
	{
		CHARFORMAT2 cf;	

		ZeroMemory(&cf,sizeof(cf));
		cf.cbSize = sizeof(cf);
		cf.dwMask = CFM_BACKCOLOR;
		cf.dwEffects = 0;
		cf.crBackColor = cr;
		return SetCharFormat(cf,uFlags);
	}
	COLORREF GetTextBkColor(BOOL bSel = TRUE) const
	{
		CHARFORMAT cf2;

		ZeroMemory(&cf,sizeof(cf));
		cf.cbSize = sizeof(cf);
		cf.dwMask = CFM_BACKCOLOR;
		GetCharFormat(cf,bSel);
		return cf.crTextColor;
	}
	void SetBold(BOOL bSet,UINT uFlags = SCF_SELECTION)
	{
		DWORD dwEffect = bSet?CFE_BOLD:0;
		SetEffect(dwEffect,CFM_BOLD,uFlags);
	}
	BOOL GetBold(BOOL bSel) const
	{
		DWORD dwEffect = GetEffect(CFM_BOLD,bSel);
		return (dwEffect & CFE_BOLD)?TRUE:FALSE;
	}
	void SetItalic(BOOL bSet,UINT uFlags = SCF_SELECTION)
	{
		DWORD dwEffect = bSet?CFE_ITALIC:0;
		SetEffect(dwEffect,CFM_ITALIC,uFlags);
	}
	BOOL GetItalic(BOOL bSel) const
	{
		DWORD dwEffect = GetEffect(CFM_ITALIC,bSel);
		return (dwEffect & CFE_ITALIC)?TRUE:FALSE;
	}
	void SetUnderline(BOOL bSet,UINT uFlags = SCF_SELECTION)
	{
		DWORD dwEffect = bSet?CFE_UNDERLINE:0;
		SetEffect(dwEffect,CFM_UNDERLINE,uFlags);
	}
	BOOL GetUnderline(BOOL bSel) const
	{
		DWORD dwEffect = GetEffect(CFM_UNDERLINE,bSel);
		return (dwEffect & CFE_UNDERLINE)?TRUE:FALSE;
	}
	void SetStrike(BOOL bSet,UINT uFlags = SCF_SELECTION)
	{
		DWORD dwEffect = bSet?CFE_STRIKEOUT:0;
		SetEffect(dwEffect,CFM_STRIKEOUT,uFlags);
	}
	BOOL GetStrike(BOOL bSel) const
	{
		DWORD dwEffect = GetEffect(CFM_STRIKEOUT,bSel);
		return (dwEffect & CFE_STRIKEOUT)?TRUE:FALSE;
	}
	void SetProtected(BOOL bSet,UINT uFlags = SCF_SELECTION) 
	{
		DWORD dwEffect = bSet?CFE_PROTECTED:0;
		SetEffect(dwEffect,CFM_PROTECTED,uFlags);
	}
	BOOL GetProtected(BOOL bSel) const
	{
		DWORD dwEffect = GetEffect(CFM_PROTECTED,bSel);
		return (dwEffect & CFE_PROTECTED)?TRUE:FALSE;
	}
	void SetLink(BOOL bSet,UINT uFlags = SCF_SELECTION) 
	{
		DWORD dwEffect = bSet?CFE_LINK:0;
		SetEffect(dwEffect,CFM_LINK,uFlags);
	}
	BOOL GetLink(BOOL bSel) const
	{
		DWORD dwEffect = GetEffect(CFM_LINK,bSel);
		return (dwEffect & CFE_LINK)?TRUE:FALSE;
	}
	void SwitchBold(BOOL bSel = TRUE) 
	{
		UINT uFlags = bSel?SCF_SELECTION:0;
		SetBold(!GetBold(bSel),uFlags);
	}
	void SwitchItalic(BOOL bSel = TRUE) 
	{
		UINT uFlags = bSel?SCF_SELECTION:0;
		SetItalic(!GetItalic(bSel),uFlags);
	}
	void SwitchUnderline(BOOL bSel = TRUE) 
	{
		UINT uFlags = bSel?SCF_SELECTION:0;
		SetUnderline(!GetUnderline(bSel),uFlags);
	}
	void SwitchStrike(BOOL bSel = TRUE) 
	{
		UINT uFlags = bSel?SCF_SELECTION:0;
		SetStrike(!GetStrike(bSel),uFlags);
	}
	void SetTextHeight(LONG lPoints,UINT uFlags = SCF_SELECTION)
	{
		CHARFORMAT cf = {0};

		cf.dwMask = CFM_SIZE;
		cf.yHeight = lPoints * 20;
		SetCharFormat(cf,uFlags);
	}
	/** return in 'printer points' */
	LONG GetTextHeight(BOOL bSel) const
	{
		CHARFORMAT cf = {0};

		cf.dwMask = CFM_SIZE;
		GetCharFormat(cf,bSel);
		return (cf.yHeight/20);
	}
	void SetTextFontName(LPCTSTR name,UINT uFlags =\
		SCF_SELECTION) 
	{
		CHARFORMAT cf = {0};

		cf.dwMask = CFM_FACE;
		lstrcpyn(cf.szFaceName,name,LF_FACESIZE);
		SetCharFormat(cf,uFlags);
	}
	CString GetTextFontName(BOOL bSel) const
	{
		CHARFORMAT cf = {0};
			
		cf.dwMask = CFM_FACE;
		GetCharFormat(cf,bSel);
		return CString(cf.szFaceName);
	}
	void SetCharset(BYTE bCharset,UINT uFlags = SCF_SELECTION)
	{
		CHARFORMAT cf = {0};

		cf.dwMask = CFM_CHARSET;
		cf.bCharSet = bCharset;
		SetCharFormat(cf,uFlags);
	}
	BYTE GetCharset(BOOL bSel) const
	{
		CHARFORMAT cf = {0};
			
		cf.dwMask = CFM_CHARSET;
		GetCharFormat(cf,bSel);
		return cf.bCharSet;
	}
	/** in twips */
	void SetStartIndent(LONG dx)
	{
		PARAFORMAT pf = {0};

		pf.dwMask = PFM_STARTINDENT;
		pf.dxStartIndent = dx;
		SetParaFormat(pf);
	}
	LONG GetStartIndent(void) const
	{
		PARAFORMAT pf = {0};

		pf.dwMask = PFM_STARTINDENT;
		GetParaFormat(pf);
		return pf.dxStartIndent;
	}
	void SetIndent(LONG dx)
	{
		PARAFORMAT pf = {0};

		pf.dwMask = PFM_OFFSET;
		pf.dxOffset = dx;
		SetParaFormat(pf);
	}
	LONG GetIndent(void) const
	{
		PARAFORMAT pf = {0};

		pf.dwMask = PFM_OFFSET;
		GetParaFormat(pf);
		return pf.dxOffset;
	}
	/** in twips */
	void SetRelativeStartIndent(LONG dx)
	{
		PARAFORMAT pf = {0};

		pf.dwMask = PFM_STARTINDENT;
		pf.dxStartIndent = dx;
		SetParaFormat(pf);
	}
	//
	void SetRightIndent(LONG dx)
	{
		PARAFORMAT pf = {0};

		pf.dwMask = PFM_RIGHTINDENT;
		pf.dxRightIndent = dx;
		SetParaFormat(pf);
	}
	LONG GetRightIndent(void) const
	{
		PARAFORMAT pf = {0};

		pf.dwMask = PFM_RIGHTINDENT;
		GetParaFormat(pf);
		return pf.dxRightIndent;
	}
	void SetAlignment(WORD wAlign = PFA_LEFT)
	{
		PARAFORMAT pf = {0};

		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = wAlign;
		SetParaFormat(pf);
	}
	WORD GetAlignment(void) const
	{
		PARAFORMAT pf = {0};

		pf.dwMask = PFM_ALIGNMENT;
		GetParaFormat(pf);
		return pf.wAlignment;
	}

	void SetSelEnd(void)
	{
		SetSel(-1,-1);
	}

	// streaming methods

	ULONG GetRtfTextLength(BOOL bSel = FALSE)
	{
		ULONG len=0;
		EDITSTREAM es;
		ULONG uFormat = SF_RTF;

		if(bSel)
			uFormat |= SFF_SELECTION;

		es.dwCookie = (DWORD)&len;
		es.dwError = 0;
		es.pfnCallback = GetLengthCallback;

		StreamOut(uFormat,es);
		return len;
	}


	/**
		Returns the number of bytes read
	*/
	ULONG ReadRtfText(LPTSTR pBuf,BOOL bSel = FALSE)
	{
		CStreamOut out(pBuf);
		EDITSTREAM es;
		ULONG uFormat = SF_RTF;

		if(bSel)
			uFormat |= SFF_SELECTION;

		es.dwCookie = (DWORD)&out;
		es.dwError = 0;
		es.pfnCallback = StreamOutCallback;

		SendMessage(EM_STREAMOUT,(WPARAM)uFormat,(LPARAM)&es);
		return out.GetPos();
	}
	/**
		Returns the number of bytes written
	*/
	ULONG WriteRtfText(LPCTSTR pBuf,LONG BufLen,BOOL bSel =\
		FALSE)
	{
		CStreamIn in(pBuf,BufLen);
		EDITSTREAM es;
		ULONG uFormat = SF_RTF;

		if(bSel)
			uFormat |= SFF_SELECTION;

		es.dwCookie = (DWORD)&in;
		es.dwError = 0;
		es.pfnCallback = StreamInCallback;

		StreamIn(uFormat,es);
		return in.GetPos();
	}

	ULONG StreamOutFile(HANDLE hFile,UINT uFormat = SF_RTF)
	{
		EDITSTREAM es = {0};

		es.dwCookie = (DWORD)hFile;
		es.pfnCallback = StreamOutFileCB;
		return StreamOut(uFormat,es);
	}

	ULONG StreamInFile(HANDLE hFile,UINT uFormat = SF_RTF)
	{
		EDITSTREAM es = {0};
		es.dwCookie = (DWORD)hFile;
		es.pfnCallback = StreamInFileCB;
		return StreamIn(uFormat,es);
	}

	// scrolling related methods

	void ScrollText(void) 
	{
		SCROLLINFO si = { sizeof(SCROLLINFO) };
		int nPos=0;
			
		si.fMask = SIF_RANGE|SIF_PAGE;
		::GetScrollInfo(m_hWnd,SB_VERT,&si);
		si.fMask = SIF_POS;
		si.nPos = si.nMax;
		::SetScrollInfo(m_hWnd,SB_VERT,&si,TRUE);
		if(si.nPage){
			nPos = si.nMax - si.nPage;
			SendMessage(WM_VSCROLL,MAKEWPARAM(SB_THUMBTRACK,nPos),(LPARAM)\
				NULL);
		}
	}
	void ClearAll(void) 
	{
		SCROLLINFO si;
		LPCTSTR pszBuf = TEXT("{\\rtf1 }");

		SetWindowText(TEXT(""));
		WriteRtfText(pszBuf,lstrlen(pszBuf)); // precisa ?
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE|SIF_POS|SIF_PAGE;
		si.nMin = si.nMax = si.nPos = si.nPage = 0;
		::SetScrollInfo(m_hWnd,SB_VERT,&si,TRUE);
	}

	//
	_ThisClass & operator<<( _ThisClass & (*f)(_ThisClass &) )
	{
		return (*f)(*this);
	}
	friend _ThisClass & bold(_ThisClass &edit)
	{
		edit.SetBold(TRUE);
		return edit;
	}

	BOOL SetDefaultOleCallback(void)
	{
		static CRichEditOleCallback *pObj = NULL;

		if(!pObj){
			pObj = new CRichEditOleCallback();
		}
		return SetOleCallback((IRichEditOleCallback *)pObj);
	}
		
	// clipboard

	// the clipboard owns the handle passed to it.
	// If bPasteACopy is true, then a copy of hBmp will be used
	// otherwise hBmp cant be used anymore
	BOOL PasteBitmap(HBITMAP hBmp,BOOL bPasteACopy) 
	{
		CClipBoardSaver saver(m_hWnd);
		HBITMAP hCopy;
		BOOL res = FALSE;

		if(bPasteACopy){
			hCopy = (HBITMAP)::CopyImage(hBmp,IMAGE_BITMAP,0,0,
				LR_COPYRETURNORG);	// ignore cx,cy copying exactly
		}
		else
			hCopy = hBmp;

		ATLASSERT(NULL != hCopy);
		if(hCopy){
			{
				CClipBoard cb(m_hWnd);
				// this empties the cb and set the data
				res = cb.SetData(CF_BITMAP,hCopy) != NULL;
			}
			if(res)
				// the cb has to be closed to paste, i guess
				Paste();
		}
		return res;
	}
	// loads a bitmap from a file, puts it into the clipboard
	// and then calls PasteBitamp
	BOOL PasteBitmapFromFile(LPCTSTR filename,UINT flags = LR_LOADTRANSPARENT)
	{
		CClipBoardSaver saver(m_hWnd);
		HBITMAP hBmp;
		BOOL res = FALSE;

		if(filename){
			hBmp = (HBITMAP)::LoadImage(NULL,filename,IMAGE_BITMAP,
				0,0,LR_LOADFROMFILE|flags);
			if(hBmp){
				CClipBoard cb(m_hWnd);
				res = cb.SetData(CF_BITMAP,hBmp) != NULL;
			}
			if(res)
				Paste();
		}
		return res;
	}

	CString GetTextRangeString(const CHARRANGE &chrg)
	{
		CString s;
		TEXTRANGE tr = {0};
		
		tr.chrg = chrg;
		tr.lpstrText = (LPTSTR)s.GetBuffer(chrg.cpMax - chrg.cpMin + 2);
		GetTextRange(&tr);
		s.ReleaseBuffer();
		return s;
	}


private:

	// used by GetRtfTextLength
	static DWORD CALLBACK GetLengthCallback(DWORD dwCookie,LPBYTE pBuf,
		LONG cb,LONG *pcb)
	{
		ULONG *pLen = (ULONG *)dwCookie;

		*pLen += cb;
		*pcb = cb;
		return 0;
	}
	// used to read from the control to a buffer
	static DWORD CALLBACK StreamOutCallback(DWORD dwCookie,LPBYTE pBuf,
		LONG cb,LONG *pcb)
	{
		CStreamOut *pOut = (CStreamOut *)dwCookie;

		*pcb = pOut->Write(pBuf,cb);
		return 0;
	}
	// used to write from a buffer to the control
	static DWORD CALLBACK StreamInCallback(DWORD dwCookie,LPBYTE pBuf,
		LONG cb,LONG *pcb)
	{
		CStreamIn *pIn = (CStreamIn *)dwCookie;
		
		*pcb = pIn->Read(pBuf,cb);
		return 0;
	}
	// rich edit control -> file
	static DWORD CALLBACK StreamOutFileCB(DWORD dwCookie,
		LPBYTE pBuf,LONG cb,LONG *pcb)
	{
		HANDLE hFile = (HANDLE)dwCookie;
		BOOL res;

		*pcb = 0;
		res = ::WriteFile(hFile,pBuf,cb,(ULONG *)pcb,NULL);
		return res ? 0 : -1;
	}
	// file  -> rich edit
	static DWORD CALLBACK StreamInFileCB(DWORD dwCookie,LPBYTE \
		pBuf,LONG cb,LONG *pcb)
	{
		HANDLE hFile = (HANDLE)dwCookie;
		BOOL res;

		*pcb = 0;
		res = ::ReadFile(hFile,pBuf,cb,(ULONG *)pcb,NULL);
		return res ? 0 : -1;
	}
};

typedef CNo5RichEditT<CWindow> CNo5RichEdit;

} // NO5TL

#endif // !defined(AFX_NO5RICHEDIT_H__21621BE2_3E94_11D9_A17B_C6D6ABB76442__INCLUDED_)
