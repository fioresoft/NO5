#ifndef SIZERBAR_H_
#define SIZERBAR_H_

// 10-11-2005 mofications by NO5 library

////////////////////////////////////////////////////////////////
// MSDN Magazine -- July 2001
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 6.0. Runs on Win 98 and probably Win 2000 too.
// Set tabsize = 3 in your editor.
//
#include "winmgr.h"


// standard sizing cursors
static HCURSOR hcSizeEW = ::LoadCursor(NULL,(LPCTSTR)IDC_SIZEWE);
static HCURSOR hcSizeNS = ::LoadCursor(NULL,(LPCTSTR)IDC_SIZENS);


typedef CWinTraits<WS_CHILD|WS_VISIBLE,WS_EX_CLIENTEDGE> CSizerBarTraits;

template < class T, class TBase = CWindow, class TWinTraits = CSizerBarTraits >
class CSizerBarImpl : public CWindowImpl< T, TBase, TWinTraits >
{
	typedef CWindowImpl< T, TBase, TWinTraits > baseClass;
	typedef CSizerBarImpl< T, TBase, TWinTraits > thisClass;

public:
	DECLARE_WND_CLASS_EX(NULL,CS_VREDRAW|CS_HREDRAW,COLOR_WINDOW)

	CSizerBarImpl(CWinMgr *pWinMgr)
	{
		m_pWinMgr = pWinMgr;
		m_bHorz = -1;			// undetermined; I will compute
		m_bDragging=FALSE;	// not dragging yet
	}

	BEGIN_MSG_MAP(CSizerBarImpl)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_SETCURSOR,OnSetCursor)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP,OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMove)
		MESSAGE_HANDLER(WM_CHAR,OnChar)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT,OnPaint)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	//////////////////
	// Set cursor to indicate sizing is possible
	//
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::SetCursor(IsHorizontal() ? hcSizeNS : hcSizeEW);
		return 0;
	}

	//////////////////
	// User pressed mouse: intialize and enter drag state
	//
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		UINT nFlags = static_cast<UINT>(wParam);
		CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));

		m_bDragging=TRUE;
		m_ptOriginal = m_ptPrevious = Rectify(pt);
		
		GetWindowRect(&m_rcBar); // bar location in screen coords
		
		DrawBar();					 // draw it
		SetCapture();				 // all mouse messages are MINE
		m_hwndPrevFocus = ::SetFocus(m_hWnd);  // set focus to me to get Escape key

		ATLASSERT(m_pWinMgr);
		CWinMgr& wm = *m_pWinMgr;

		// get WINRECTs on either side of me
		WINRECT* pwrcSizeBar = wm.FindRect(GetDlgCtrlID());
		ATLASSERT(pwrcSizeBar);
		WINRECT* prev = pwrcSizeBar->Prev();
		ATLASSERT(prev);
		WINRECT* next = pwrcSizeBar->Next();
		ATLASSERT(next);

		// get rectangles on eithr side of me
		CRect rcPrev = prev->GetRect();
		CRect rcNext = next->GetRect();

		// get parent window
		CWindow  wndParent = GetParent();
		ATLASSERT(wndParent.IsWindow());

		// Get size info for next/prev rectangles, so I know what the min/max
		// sizes are and don't violate them. Max size never tested.
		SIZEINFO szi;
		wm.OnGetSizeInfo(szi, prev, &wndParent);
		CRect rcPrevMin(rcPrev.TopLeft(),szi.szMin);
		CRect rcPrevMax(rcPrev.TopLeft(),szi.szMax);

		wm.OnGetSizeInfo(szi, next, &wndParent);
		CRect rcNextMin(rcNext.BottomRight()-szi.szMin, rcNext.BottomRight());
		CRect rcNextMax(rcNext.BottomRight()-szi.szMax, rcNext.BottomRight());

		// Initialize m_rcConstrain. This is the box the user is allowed to move
		// the sizer bar in. Can't go outside of this--would violate min/max
		// constraints of windows on either side.
		m_rcConstrain.SetRect(
			max(rcPrevMin.right, rcNextMax.left),
			max(rcPrevMin.bottom,rcNextMax.top),
			min(rcPrevMax.right, rcNextMin.left),
			min(rcPrevMax.bottom,rcNextMin.top));

		// convert to my client coords
		wndParent.ClientToScreen(&m_rcConstrain);
		ScreenToClient(&m_rcConstrain);

		// Now adjust m_rcConstrain for the fact the bar is not a pure line, but
		// has solid width -- so I have to make a little bigger/smaller according
		// to the offset of mouse coords within the sizer bar rect iteself.
		ClientToScreen(&pt);
		m_rcConstrain.SetRect(m_rcConstrain.TopLeft() + (pt - m_rcBar.TopLeft()),
			m_rcConstrain.BottomRight() - (m_rcBar.BottomRight()-pt));

		return 0;
	}

	//////////////////
	// User moved mouse: erase old bar and draw in new position. XOR makes this
	// easy. Keep track of previous point.
	//
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_bDragging) {
			CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));

			DrawBar();				// erase old bar
			pt = Constrain(pt);	// don't go outside constrained rect!
			pt = Rectify(pt);		// clip x or y depending if horizontal or vert
			CPoint ptDelta = pt-m_ptPrevious;
			m_rcBar += ptDelta;	// move bar...
			DrawBar();				// and draw
			m_ptPrevious = pt;	// remember for next mousemove
		}
		return 0;
	}

	//////////////////
	// User let go of mouse: leave size-drag mode
	//
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_bDragging) {
			CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));

			pt = Constrain(pt);					 // don't go outside constraints
			pt = Rectify(pt);						 // clip x or y
			CPoint ptDelta = pt-m_ptOriginal; // distance moved
			CancelDrag();							 // cancel drag mode
			NotifyMoved(ptDelta);				 // notify parent
		}
		else
			bHandled = FALSE;
		return 0;
	}

	//////////////////
	// Escape key cancels sizing.
	//
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (static_cast<TCHAR>(wParam)==VK_ESCAPE && m_bDragging) {
			CancelDrag();
			return 0;
		}
		return 0;
	}

	//////////////////
	// Handle WM_CANCELMODE. This is probably unnecessary.
	//
	LRESULT OnCancelMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_bDragging)
			CancelDrag();
		return 0;
	}

	LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return DefWindowProc();
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(!m_bDragging){
			CPaintDC dc(m_hWnd);
			CRect rcClient; GetClientRect(&rcClient);

			dc.FillRect(&rcClient, COLOR_3DFACE);
			// draw 3D edge
			dc.DrawEdge(&rcClient, EDGE_RAISED, IsHorizontal() ?  (BF_TOP | BF_BOTTOM) : (BF_LEFT | BF_RIGHT) );
		}
		return 0;
	}


	//////////////////
	// Determine whether I am horizontal or vertical by looking at dimensions.
	// Remember the result for speed.
	//
	BOOL IsHorizontal()
	{
		if (!m_hWnd)
			return FALSE;	 // not created yet: doesn't matter
		if (m_bHorz!=-1)
			return m_bHorz; // I already figured it out
		
		// If width is greater than height, I must be horizontal. Duh.
		CRect rc;
		GetWindowRect(&rc);
		return m_bHorz = (rc.Width() > rc.Height());
	}

	CPoint Rectify(CPoint pt) 
	{		// make point vertical/horizontal
		return IsHorizontal() ? CPoint(0, pt.y) : CPoint(pt.x, 0);
	}

	//////////////////
	// Cancel drag mode: release capture, erase bar, restore focus
	//
	void CancelDrag()
	{
		DrawBar();								// erase bar
		ReleaseCapture();						// release mouse
		::SetFocus(m_hwndPrevFocus);		// restore original focus window
		m_bDragging = FALSE;					// stop dragging
	}

	//////////////////
	// Draw sizer bar. Set up DC, then call virt fn to do it.  Draw on parent
	// window's DC to overpaint siblings. Can't use screen DC because that would
	// also paint on any WS_EX_TOPMOST windows above me--oops!
	//
	void DrawBar()
	{
		CWindow wndParent = GetParent();
		CWindowDC dc(wndParent);
		CRect rcWin;
		wndParent.GetWindowRect(&rcWin);		 // parent window's screen rect
		CRect rc = m_rcBar;						 // bar in screen coords
		rc -= rcWin.TopLeft();					 // convert parent window coords 
		OnDrawBar(dc, rc);						 // calll virtual fn
	}

	//////////////////
	// Notify parent I moved. It's up to the parent to actually move me by
	// calling CWinMgr. I just report how much to move by.
	//
	void NotifyMoved(CPoint ptDelta)
	{
		CWindow wndParent;
		NMWINMGR nmr;
		nmr.code = NMWINMGR::SIZEBAR_MOVED;		 // notification subcode
		nmr.idFrom = GetDlgCtrlID();				 // my ID
		nmr.sizebar.ptMoved = ptDelta;			 // distance moved

		wndParent = GetParent();
		wndParent.SendMessage(WM_WINMGR, nmr.idFrom, (LPARAM)&nmr);
	}

	//////////////////
	// Draw sizer bar window using XOR op (PATINVERT) so next time will erase.
	// YOu can override as long as you use an XOR op.
	//
	void OnDrawBar(CDC& dc, CRect& rc)
	{
		CBrush brush;

		brush.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
		CBrushHandle hOldBrush = dc.SelectBrush(brush);
		dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATINVERT);
		dc.SelectBrush(hOldBrush);
	}

	//////////////////
	// Helper--force point to be inside m_rcConstrain.
	// Returns constrained point.
	//
	CPoint Constrain(CPoint pt)
	{
		if (pt.x<m_rcConstrain.left)
			pt.x = m_rcConstrain.left;
		if (pt.x>m_rcConstrain.right)
			pt.x = m_rcConstrain.right;
		if (pt.y<m_rcConstrain.top)
			pt.y = m_rcConstrain.top;
		if (pt.y>m_rcConstrain.bottom)
			pt.y = m_rcConstrain.bottom;
		return pt;
	}
	
protected:
	CWinMgr*	m_pWinMgr;			// window manager
	BOOL		m_bHorz;			// horizontal bar; else vertical
	BOOL		m_bDragging;		// in drag mode?
	CRect		m_rcBar;			// bar rect in screen coords
	CPoint		m_ptOriginal;		// original mouse pos at start of drag
	CPoint		m_ptPrevious;		// previous mouse pos while dragging
	CRect		m_rcConstrain;		// constrain mouse to this rect
	HWND		m_hwndPrevFocus;	// to restore after dragging
};

class CSizerBar : public CSizerBarImpl< CSizerBar, CWindow, CControlWinTraits>
{
	typedef CSizerBarImpl< CSizerBar, CWindow, CControlWinTraits> baseClass;
public:
	CSizerBar(CWinMgr *pWinMgr):baseClass(pWinMgr)
	{
		//
	}
	BEGIN_MSG_MAP(CSizerBar)
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()
};


#endif // SIZERBAR_H_