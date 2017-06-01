// Utils.h: interface for the CUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILS_H__7D29794A_9BA8_11D7_A17A_F8A2F80EBA36__INCLUDED_)
#define AFX_UTILS_H__7D29794A_9BA8_11D7_A17A_F8A2F80EBA36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace NO5TL
{

	// CDCSaver
	// CObjSelector
	// GetCrushColor
	// CLazyFileDialog
	// CMyWaitCursor
	// CMouseTracker


class CDCSaver
{
	CDCHandle m_dc;
public:
	CDCSaver(HDC hdc):m_dc(hdc)
	{
		m_dc.SaveDC();
	}
	~CDCSaver()
	{
		m_dc.RestoreDC(-1);
	}
};

template <class T>
class CObjSelector
{
	T m_hObjOld;
	HDC m_hdc;
public:
	CObjSelector(HDC hdc):m_hdc(hdc),m_hObjOld((T)NULL)
	{
		//
	}
	~CObjSelector()
	{
		if(m_hObjOld){
			T h;
			h = (T)::SelectObject(m_hdc,(HGDIOBJ)m_hObjOld);
			ATLASSERT(h);
		}
	}
	BOOL Select(T hNewObj)
	{
		m_hObjOld = (T)::SelectObject(m_hdc,(HGDIOBJ)hNewObj);
		// TODO: para regions error is indicated by GDI_ERROR
		return NULL != m_hObjOld;
	}
};

inline COLORREF GetBrushColor(HBRUSH hBrush)
{
	CBrushHandle brush(hBrush);
	LOGBRUSH lb = {0};

	brush.GetLogBrush(&lb);
	return lb.lbColor;
}

inline void SetClassBkColor(HWND hWnd,COLORREF clr)
{
	HBRUSH hbr = ::CreateSolidBrush(clr);
	HBRUSH hbrOld = (HBRUSH)::SetClassLong(hWnd,GCL_HBRBACKGROUND,
		(LONG)hbr);

	if(hbrOld)
		::DeleteObject(hbrOld);
	::InvalidateRect(hWnd,NULL,TRUE);
}

inline COLORREF GetClassBkColor(HWND hWnd)
{
	HBRUSH hBrush = (HBRUSH)::GetClassLong(hWnd,GCL_HBRBACKGROUND);
	return NO5TL::GetBrushColor(hBrush);
}

#ifdef LAZYFILEDIALOG
// se "file" estiver vazio depois do ctor, entao usuario cancelou
// se nao, contem o full path ( path + file name )
class CLazyFileDialog : public CFileDialog
{
public:
	// file pode ser usado para inicializar o edit control
	CLazyFileDialog(CString &file,BOOL bOpen = TRUE):\
		CFileDialog(\
		bOpen,	// open file or save file dlg
		NULL,	// def extension if none is specified, without dot
		file.IsEmpty() ? NULL : (LPCTSTR)file, // name to initialize
		OFN_HIDEREADONLY | OFN_NOCHANGEDIR |OFN_FILEMUSTEXIST|\
		OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST|OFN_EXPLORER,
		"Text files ( *.txt )\0*.txt\0All files ( *.* )\0*.*\0",
		GetDesktopWindow())
	{
		file = "";
		if(IDOK == DoModal()){
			file = m_szFileName;
		}
	}
};
#endif // LAZYFILEDIALOG

template <class T>
class CMouseTracker
{
	enum {
		TRACKER_TIMERID = 1,
		TRACKER_TIMEOUT = 200,
	};
	typedef CMouseTracker<T> _ThisClass;
	bool m_bMouseOver;	// mouse is over the client window
	bool m_bTrack;		// track
public:
	BEGIN_MSG_MAP(CMouseTracker)
		MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMove)
		MESSAGE_HANDLER(WM_TIMER,OnTimer)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
	END_MSG_MAP()

public:
	CMouseTracker()
	{
		m_bTrack = false;
		m_bMouseOver = false;
	}

public:
	LRESULT OnMouseMove(UINT,WPARAM,LPARAM,BOOL &bHandled)
	{
		if(IsTracking() && !IsMouseOver()){
			UINT timer;
			T *pT = static_cast<T*>(this);

			// we detected the mouse is over the client window

			// start timer, to detect when it leaves
			timer = pT->SetTimer(TRACKER_TIMERID,TRACKER_TIMEOUT);
			ATLASSERT(timer);
			m_bMouseOver = true;
			// notifies
			pT->OnMouseEnter();
		}
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnTimer(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		UINT nIDEvent = (UINT)wParam;
		T *pT = static_cast<T*>(this);

		if(nIDEvent == TRACKER_TIMERID){
			POINT pt = {0};
			RECT rc = {0};

			ATLASSERT(IsTracking() && IsMouseOver());
			// check if mouse is still over the client area
			::GetCursorPos(&pt);
			pT->GetClientRect(&rc);

			pT->ScreenToClient(&pt);
			if( ! ::PtInRect(&rc,pt) ){

				m_bMouseOver = false;
				// nope, it has left the client area
				// kill the timer and notify
				pT->KillTimer(TRACKER_TIMERID);
				pT->OnMouseLeave();
			}
		}
		else{
			bHandled = FALSE;
		}
		return 0;
	}
	LRESULT OnDestroy(UINT,WPARAM,LPARAM,BOOL &bHandled)
	{
		if(IsTracking() && IsMouseOver()){
			// this will kill the timer
			TrackMouse(false);
		}
		bHandled = FALSE;
		return 0;
	}

	// interface
	bool IsTracking(void) const
	{
		return m_bTrack;
	}
	void TrackMouse(bool track = true)
	{
		if(IsTracking()){
			if(!track){
				T *pT = static_cast<T*>(this);
				pT->KillTimer(TRACKER_TIMERID);
				m_bTrack = false;
				m_bMouseOver = false;
			}
			else {
				// do nothing
			}
		}
		else{
			m_bTrack = track;
		}
	}
	bool IsMouseOver(void) const
	{
		return m_bMouseOver;
	}
	// overidables
	void OnMouseEnter(void)
	{
		//
	}
	void OnMouseLeave(void)
	{
		//
	}
};


} // namespace NO5TL


#endif // !defined(AFX_UTILS_H__7D29794A_9BA8_11D7_A17A_F8A2F80EBA36__INCLUDED_)
