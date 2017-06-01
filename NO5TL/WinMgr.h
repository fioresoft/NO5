#ifndef WINMGR_H_
#define WINMGR_H_

// 10-11-2005 mofications by NO5TL library

//
// WinMgr
//
// Written by Laurent Kempé (lkempe@netcourrier.com) for Tech Head WebSite
// Copyright (c) 2001 Laurent Kempé.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Origin: July 2001 issue of MSDN Magazine
//         Paul DiLascia 
//         Windows UI: Our WinMgr Sample Makes Custom Window Sizing Simple
//
////////////////////////////////////////////////////////////////
// MSDN Magazine -- July 2001
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 6.0. Runs on Win 98 and probably Win 2000 too.
// Set tabsize = 3 in your editor.
//
// WinMgr.h -- Main header file for WinMgr library.
//
#pragma once

// add this to a cpp file
// const WM_WINMGR = RegisterWindowMessage("WM_WINMGR");

// when using screen dimensions, this is infinite
const LONG INFINITY=0x7fff; // max short

// useful size constants
#define SIZEZERO		CSize(0,0)
#define SIZEMAX		CSize(INFINITY,INFINITY)

// handy functions to take the min or max of a CSize
inline CSize minsize(CSize a, CSize b) {
	return CSize(min((UINT)a.cx,(UINT)b.cx),min((UINT)a.cy,(UINT)b.cy));
}

inline CSize maxsize(CSize a, CSize b) {
	return CSize(max((UINT)a.cx,(UINT)b.cx),max((UINT)a.cy,(UINT)b.cy));
}

//////////////////
// Size info about a rectangle/row/column
//
struct SIZEINFO {
	SIZE szAvail;		// total size avail (passed)
	SIZE szDesired;	// desired size: default=current
	SIZE szMin;			// minimum size: default=SIZEZERO
	SIZE szMax;			// maximum size: default=MAXSIZE
};

// types of rectangles:
#define	WRCT_END			0				// end of table
#define	WRCT_FIXED		0x0001		// height/width is fixed
#define	WRCT_PCT			0x0002		// height/width is percent of total
#define	WRCT_REST		0x0003		// height/width is whatever remains
#define	WRCT_TOFIT		0x0004		// height/width to fit contents
#define	WRCF_TYPEMASK	0x000F

// group flags
#define	WRCF_ROWGROUP	0x0010		// beginning of row group
#define	WRCF_COLGROUP	0x0020		// beginning of column group
#define	WRCF_ENDGROUP	0x00F0		// end of group
#define	WRCF_GROUPMASK	0x00F0

//////////////////
// This structure is used to hold a rectangle and describe its layout. Each
// WINRECT corresponds to a child rectangle/window. Each window that uses
// WinMgr provides a table (C array) of these to describe its layout.
//
class WINRECT {
protected:
	// pointers initialized by the window manager for easy traversing:
	WINRECT* next;			// next at this level
	WINRECT* prev;			// prev at this level

	// data
	CRect rc;				// current rectangle position/size
	WORD  flags;			// flags (see above)
	UINT	nID;				// window ID if this WINRECT represents a window
	LONG	param;			// arg depends on type

public:
	//////////////////
	// Construct from args
	//
	WINRECT(WORD f, UINT id, LONG p)
	{
		memset(this, 0, sizeof(WINRECT));
		flags = f;
		nID = id;
		param = p;
	}

	//////////////////
	// Initialize map: set up all the next/prev pointers. This converts the
	// linear array to a more convenient linked list. Called from END_WINDOW_MAP.
	//
	static WINRECT* InitMap(WINRECT* pWinMap, WINRECT* parent = NULL)
	{
		ATLASSERT(pWinMap);

		WINRECT* pwrc = pWinMap;  // current table entry
		WINRECT* prev = NULL;	  // previous entry starts out none

		while (!pwrc->IsEndGroup()) {
			pwrc->prev=prev;
			pwrc->next=NULL;
			if (prev)
				prev->next = pwrc;
			prev = pwrc;
			if (pwrc->IsGroup()) {
				pwrc = InitMap(pwrc+1,pwrc); // recurse! Returns end-of-grp
				ATLASSERT(pwrc->IsEndGroup());
			}
			pwrc++;
		}
		// safety checks
		ATLASSERT(pwrc->IsEndGroup());
		ATLASSERT(prev);
		ATLASSERT(prev->next==NULL);
		return parent ? pwrc : NULL;
	}

	WINRECT* Prev()			{ return prev; }
	WINRECT* Next()			{ return next; }
	WINRECT* Children()		{ return IsGroup() ? this+1 : NULL; }

	//////////////////
	// Get the parent of a given WINRECT. To find the parent, chase the prev
	// pointer to the start of the list, then take the item before that in
	// memory. 
	//
	WINRECT* Parent()
	{
		for (WINRECT* pEntry=this; pEntry->Prev(); pEntry=pEntry->Prev()) {
			; // go backwards to the end
		}
		// the entry before the first child is the group
		WINRECT *parent = pEntry-1;
		ATLASSERT(parent->IsGroup());
		return parent;
	}
	WORD GetFlags()			{ return flags; }
	WORD SetFlags(WORD f)	{ return flags=f; }
	LONG GetParam()			{ return param; }
	LONG SetParam(LONG p)	{ return param=p; }
	UINT GetID()				{ return nID; }
	UINT SetID(UINT id)		{ return nID=id; }
	CRect& GetRect()					{ return rc; }
	void SetRect(const CRect& r)	{ rc = r; }
	WORD Type() const			{ return flags & WRCF_TYPEMASK; }
	WORD GroupType() const	{ return flags & WRCF_GROUPMASK; }
	BOOL IsGroup() const		{ return GroupType() && GroupType()!=WRCF_ENDGROUP; }
	BOOL IsEndGroup() const { return flags==0 || flags==WRCF_ENDGROUP; }
	BOOL IsEnd() const		{ return flags==0; }
	BOOL IsWindow() const	{ return nID>0; }
	BOOL IsRowGroup()	const { return (flags & WRCF_GROUPMASK)==WRCF_ROWGROUP; }
	void SetHeight(LONG h)	{ rc.bottom = rc.top + h; }
	void SetWidth(LONG w)	{ rc.right = rc.left + w; }
	LONG GetHeightOrWidth(BOOL bHeight) const {
		return bHeight ? rc.Height() : rc.Width();
	}
	void SetHeightOrWidth(LONG horw, BOOL bHeight) {
		bHeight ? SetHeight(horw) : SetWidth(horw);
	}

	//////////////////
	// Get group margins
	//
	BOOL WINRECT::GetMargins(int& w, int& h)
	{
		if (IsGroup()) {
			w=(short)LOWORD(param);
			h=(short)HIWORD(param);
			return TRUE;
		}
		w=h=0;
		return FALSE;
	}

	// For TOFIT types, param is the TOFIT size, if nonzero. Used in dialogs,
	// with CWinMgr::InitToFitSizeFromCurrent.
	BOOL HasToFitSize()			{ return param != 0; }
	SIZE GetToFitSize()			{ return CSize(LOWORD(param),HIWORD(param)); }
	void SetToFitSize(SIZE sz)	{ param = MAKELONG(sz.cx,sz.cy); }

};

//////////////////
// Below are all the macros to build your window map. 
//

#define BEGIN_WINDOW_MAP2(name) static WINRECT * GetWindowMap(void) { static WINRECT name[] = { 
#define END_WINDOW_MAP2(name) WINRECT(WRCT_END,-1,0) } ; return name; }

// Begin/end window map. 'name' can be anything you want
#define BEGIN_WINDOW_MAP(name)	WINRECT name[] = {
#define END_WINDOW_MAP()		WINRECT(WRCT_END,-1,0) }; 

// Begin/end a group.
// The first entry in your map must be BEGINROWS or BEGINCOLS.
#define BEGINROWS(type,id,m)	WINRECT(WRCF_ROWGROUP|type,id,m),
#define BEGINCOLS(type,id,m)  WINRECT(WRCF_COLGROUP|type,id,m),
#define ENDGROUP()				WINRECT(WRCF_ENDGROUP,-1,0),

// This macros is used only with BEGINGROWS or BEGINCOLS to specify margins
#define RCMARGINS(w,h)			MAKELONG(w,h)

// Macros for primitive (non-group) entries.
// val applies to height for a row entry; width for a column entry.
#define RCFIXED(id,val)		WINRECT(WRCT_FIXED,id,val),
#define RCPERCENT(id,val)	WINRECT(WRCT_PCT,id,val),
#define RCREST(id)			WINRECT(WRCT_REST,id,0),
#define RCTOFIT(id)			WINRECT(WRCT_TOFIT,id,0),
#define RCSPACE(val)			RCFIXED(-1,val)

//////////////////
// Use this to iterate the entries in a group.
//
//	CWinGroupIterator it;
//	for (it=pGroup; it; it.Next()) {
//   WINRECT* wrc = it;
//   ..
// }
//
class CWinGroupIterator {
protected:
	WINRECT* pCur;	  // current entry
public:
	CWinGroupIterator() { pCur = NULL; }
	CWinGroupIterator& operator=(WINRECT* pg) {
		ATLASSERT(pg->IsGroup()); // can only iterate a group!
		pCur = pg->Children();
		return *this;
	}
	operator WINRECT*()	{ return pCur; }
	WINRECT* pWINRECT()	{ return pCur; }
	WINRECT* Next()		{ return pCur = pCur ? pCur->Next() : NULL;}
};

// Registered WinMgr message
extern const WM_WINMGR;

// Notification struct, passed as LPARAM
struct NMWINMGR : public NMHDR {
	enum {								// notification codes:
		GET_SIZEINFO = 1,				// WinMgr is requesting size info
		SIZEBAR_MOVED,					// user moved sizer bar
	};

	// each notification code has its own part of union
	union {
		SIZEINFO sizeinfo;	// used for GET_SIZEINFO
		struct {					// used for SIZEBAR_MOVED
			POINT ptMoved;		//  distance moved (x or y = zero)
		} sizebar;
	};

	// ctor: initialize to zeroes
	NMWINMGR() { memset(this,0,sizeof(NMWINMGR)); }
};

///////////////////
// Window manager. This class calculates all the sizes and positions of the
// rectangles in the window map.
//
class CWinMgr 
	/*: public CObject */
{

public:

	CWinMgr::CWinMgr(WINRECT* pWinMap) : m_map(pWinMap)
	{
		if(m_map)
			WINRECT::InitMap(m_map);
	}

	virtual ~CWinMgr()
	{
		//
	}

	//////////////////
	// Load all rectangles from current window positions.
	//
	virtual void GetWindowPositions(CWindow* pWnd)
	{
		ATLASSERT(m_map);
		ATLASSERT(pWnd);
		for (WINRECT* wrc=m_map; !wrc->IsEnd(); wrc++) {
			if (wrc->IsWindow()) {
				CWindow Child( pWnd->GetDlgItem(wrc->GetID()));
				if (Child) {
					Child.GetWindowRect(&wrc->GetRect());
					pWnd->ScreenToClient(&wrc->GetRect());
				}
			}
		}
	}

	//////////////////
	// Move all the windows. Use DeferWindowPos for speed.
	//
	virtual void SetWindowPositions(CWindow* pWnd)
	{
		int nWindows = CountWindows();
		if (m_map && pWnd && nWindows>0) {
			HDWP hdwp = ::BeginDeferWindowPos(nWindows);
			int count=0;
			for (WINRECT* wrc=m_map; !wrc->IsEnd(); wrc++) {
				if (wrc->IsWindow()) {
					ATLASSERT(count < nWindows);
					HWND hwndChild = ::GetDlgItem(pWnd->m_hWnd, wrc->GetID());
					if (hwndChild) {
						const CRect& rc = wrc->GetRect();
						::DeferWindowPos(hdwp,
							hwndChild,
							NULL,		// HWND insert after
							rc.left,rc.top,rc.Width(),rc.Height(),
							SWP_NOZORDER);
						InvalidateRect(hwndChild,NULL,TRUE); // repaint
						count++;
					}
				} else {
					// not a window: still need to repaint background
					pWnd->InvalidateRect(&wrc->GetRect());
				}
			}
			::EndDeferWindowPos(hdwp);
		}
	}

	
	//////////////////
	// get min/max/desired size of a rectangle
	// Get size information for a single entry (WINRECT). Returns size info in
	// the SIZEINFO argument. For a group, calculate size info as aggregate of
	// subentries.
	//
	virtual void OnGetSizeInfo(SIZEINFO& szi, WINRECT* wrc, CWindow* pWnd)
	{
		szi.szMin = SIZEZERO;				// default min size = zero
		szi.szMax = SIZEMAX;					// default max size = infinite
		szi.szDesired = wrc->GetRect().Size();	// default desired size = current 

		if (wrc->IsGroup()) {
			// For groups, calculate min, max, desired size as aggregate of children
			szi.szDesired = SIZEZERO;
			BOOL bRow = wrc->IsRowGroup();

			CWinGroupIterator it;
			for (it=wrc; it; it.Next()) {
				WINRECT* wrc2 = it;
				SIZEINFO szi2;
				OnGetSizeInfo(szi2, wrc2, pWnd);
				if (bRow) {
					szi.szMin.cx = max(szi.szMin.cx, szi2.szMin.cx);
					szi.szMin.cy += szi2.szMin.cy;
					szi.szMax.cx = min(szi.szMax.cx, szi2.szMax.cx);
					szi.szMax.cy = min(szi.szMax.cy + szi2.szMax.cy, INFINITY);
					szi.szDesired.cx = max(szi.szDesired.cx, szi2.szDesired.cx);
					szi.szDesired.cy += szi2.szDesired.cy;

				} else {
					szi.szMin.cx += szi2.szMin.cx;
					szi.szMin.cy = max(szi.szMin.cy, szi2.szMin.cy);
					szi.szMax.cx = min(szi.szMax.cx + szi2.szMax.cx, INFINITY);
					szi.szMax.cy = min(szi.szMax.cy, szi2.szMax.cy);
					szi.szDesired.cx += szi2.szDesired.cx;
					szi.szDesired.cy = max(szi.szDesired.cy, szi2.szDesired.cy);
				}
			}

			// Add margins. 
			int w2,h2;
			wrc->GetMargins(w2,h2);			// get margins
			w2<<=1; h2<<=1;					// double
			szi.szMin.cx += max(0,w2);		// negative margins ==> don't include in min
			szi.szMin.cy += max(0,h2);		// ditto
			szi.szDesired.cx += abs(w2);	// for desired size, use abs vallue
			szi.szDesired.cy += abs(h2);	// ditto

		} else {
			// not a group
			WINRECT* parent = wrc->Parent();
			ATLASSERT(parent);
			CRect& rcParent = parent->GetRect();
			BOOL bRow = parent->IsRowGroup();
			int hw, hwMin, hwTotal, pct;

			switch (wrc->Type()) {
			case WRCT_FIXED:
				hw = hwMin = wrc->GetParam();	 // ht/wid is parameter
				if (hw<0) {							 // if fixed val is negative:
					hw = -hw;						 // use absolute val for desired..
					hwMin = 0;						 // ..and zero for minimum
				}
				if (bRow) {
					szi.szMax.cy = szi.szDesired.cy = hw;
					szi.szMin.cy = hwMin;
				} else {
					szi.szMax.cx = szi.szDesired.cx = hw;
					szi.szMin.cx = hwMin;
				}
				break;

			case WRCT_PCT:
				pct = wrc->GetParam();
				ATLASSERT(0<pct && pct<100);
				hwTotal = bRow ? rcParent.Height() : rcParent.Width();
				hw = (hwTotal * pct) / 100;
				szi.szDesired = bRow ? CSize(rcParent.Width(), hw) :
					CSize(hw, rcParent.Height());
				break;

			case WRCT_TOFIT:
				if (wrc->HasToFitSize()) {
					szi.szDesired = wrc->GetToFitSize();
				}
				break;

			case WRCT_REST:
				break;

			default:
				ATLASSERT(FALSE);
			}

			// If the entry is a window, send message to get min/max/tofit size.
			// Only set tofit size if type is TOFIT.
			//
			if (wrc->IsWindow() && pWnd) {
				CWindow Child( pWnd->GetDlgItem(wrc->GetID()) );
				if (Child) {
					if (!Child.IsWindowVisible() && pWnd->IsWindowVisible()) {
						// parent visible but child not ==> tofit size is zero
						// important so hidden windows use no space
						szi.szDesired = SIZEZERO;
					} else {
						szi.szAvail = rcParent.Size();
						SendGetSizeInfo(szi, pWnd, wrc->GetID());
					}
				}
			}
			szi.szDesired = maxsize(minsize(szi.szDesired,szi.szMax), szi.szMin);
		}
	}


	// calc layout using client area as total area
	void CalcLayout(CWindow* pWnd) {
		ATLASSERT(pWnd);
		CRect rcClient;
		pWnd->GetClientRect(&rcClient);
		CalcLayout(rcClient, pWnd);
	}

	// calc layout using cx, cy (for OnSize)
	void CalcLayout(int cx, int cy, CWindow* pWnd=NULL) {
		CalcLayout(CRect(0,0,cx,cy), pWnd);
	}

	// calc layout using given rect as total area
	void CalcLayout(CRect rcTotal, CWindow* pWnd=NULL) {
		ATLASSERT(m_map);
		m_map->SetRect(rcTotal);
		CalcGroup(m_map, pWnd);
	}

	// Move rectangle vertically or horizontally. Used with sizer bars.
	void MoveRect(int nID, CPoint ptMove, CWindow* pParentWnd) {
		MoveRect(FindRect(nID), ptMove, pParentWnd);
	}

	//////////////////
	// Move desired rectangle by a given vector amount.
	// Call this when a sizer bar tells you it has moved.
	//
	void MoveRect(WINRECT* pwrcMove, CPoint ptMove, CWindow* pParentWnd)
	{
		ATLASSERT(pwrcMove);
		WINRECT* prev = pwrcMove->Prev();
		ATLASSERT(prev);
		WINRECT* next = pwrcMove->Next();
		ATLASSERT(next);

		BOOL bIsRow = pwrcMove->Parent()->IsRowGroup();

		CRect& rcNext = next->GetRect();
		CRect& rcPrev = prev->GetRect();
		if (bIsRow) {
			// a row can only be moved up or down
			ptMove.x = 0;
			rcPrev.bottom += ptMove.y;
			rcNext.top += ptMove.y;
		} else {
			// a column can only be moved left or right
			ptMove.y = 0;
			rcPrev.right += ptMove.x;
			rcNext.left += ptMove.x;
		}
		pwrcMove->GetRect() += ptMove;
		if (prev->IsGroup())
			CalcGroup(prev, pParentWnd);
		if (next->IsGroup())
			CalcGroup(next, pParentWnd);
	}

	CRect GetRect(UINT nID)						 { return FindRect(nID)->GetRect(); }
	void SetRect(UINT nID, const CRect& rc) { FindRect(nID)->SetRect(rc); }

	//////////////////
	// Find the entry for a given control ID
	//
	WINRECT* FindRect(UINT nID)
	{
		ATLASSERT(m_map);
		for (WINRECT* w=m_map; !w->IsEnd(); w++) {
			if (w->GetID()==nID)
				return w;
		}
		return NULL;
	}

	//////////////////
	// Get min/max info.
	//
	void GetMinMaxInfo(CWindow* pWnd, MINMAXINFO* lpMMI)
	{
		SIZEINFO szi;
		GetMinMaxInfo(pWnd, szi); // call overloaded version
		lpMMI->ptMinTrackSize = CPoint(szi.szMin);
		lpMMI->ptMaxTrackSize = CPoint(szi.szMax);
	}

	//////////////////
	// Get min/max info. 
	//
	void GetMinMaxInfo(CWindow* pWnd, SIZEINFO& szi)
	{
		OnGetSizeInfo(szi, m_map, pWnd);  // get size info
		if (!pWnd->m_hWnd)					 // window not created ==> done
			return;

		// Add extra space for frame/dialog screen junk.
		DWORD dwStyle = pWnd->GetStyle();
		DWORD dwExStyle = pWnd->GetExStyle();
		if (dwStyle & WS_VISIBLE) {
			SIZE& szMin = szi.szMin; // ref!
			if (!(dwStyle & WS_CHILD)) {
				if (dwStyle & WS_CAPTION)
					szMin.cy += GetSystemMetrics(SM_CYCAPTION);
				if (::GetMenu(pWnd->m_hWnd))
					szMin.cy += GetSystemMetrics(SM_CYMENU);
			}
			if (dwStyle & WS_THICKFRAME) {
				szMin.cx += 2*GetSystemMetrics(SM_CXSIZEFRAME);
				szMin.cy += 2*GetSystemMetrics(SM_CYSIZEFRAME);
			} else if (dwStyle & WS_BORDER) {
				szMin.cx += 2*GetSystemMetrics(SM_CXBORDER);
				szMin.cy += 2*GetSystemMetrics(SM_CYBORDER);
			}
			if (dwExStyle & WS_EX_CLIENTEDGE) {
				szMin.cx += 2*GetSystemMetrics(SM_CXEDGE);
				szMin.cy += 2*GetSystemMetrics(SM_CYEDGE);
			}
		}
	}

	//////////////////
	// Set each control's tofit (desired) size to current size. Useful for
	// dialogs, to "remember" the current sizes as desired size.
	//
	void InitToFitSizeFromCurrent(CWindow* pWnd)
	{
		ATLASSERT(pWnd);
		ATLASSERT(m_map);
		GetWindowPositions(pWnd);
		for (WINRECT* w = m_map; !w->IsEnd(); w++) {
			if (w->Type()==WRCT_TOFIT && !w->IsGroup()) {
				w->SetToFitSize(w->GetRect().Size());
			}
		}
	}

	void TRACEDump() const { TRACEDump(m_map); }

	//////////////////
	// Useful debugging function dumps CWinMgr to TRACE stream.
	//
	void CWinMgr::TRACEDump(WINRECT* pWinMap) const
	{
	#ifdef _DEBUG
		int indent=0;
		for (WINRECT* w=pWinMap; !w->IsEnd(); w++) {
			if (w->IsEndGroup())
				indent--;
			CString s(' ',indent);
			if (w->IsGroup()) {
				s+=w->IsRowGroup() ? "ROWGROUP " : "COLGROUP ";
			} else if (w->IsEndGroup()) {
				s+=_T("ENDGROUP\n");
			}
			UINT id = w->GetID();
			CString s2;
			if (w->Type()==WRCT_FIXED) {
				s2.Format(_T("FIXED=%d id=%d"), w->GetParam(), id);
			}
			if (w->Type()==WRCT_PCT) {
				s2.Format(_T("PERCENT=%d id=%d"), w->GetParam(), id);
			} else if (w->Type()==WRCT_TOFIT) {
				s2.Format(_T("TOFIT id=%d"), id);
			} else if (w->Type()==WRCT_REST) {
				s2.Format(_T("REST id=%d"), id);
			}
			if (!s2.IsEmpty()) {
				CString s3;
				CRect& rc = w->GetRect();
				s3.Format(" (%d,%d)x(%d,%d)\n",rc.left,rc.top,rc.Width(),rc.Height());
				s2 += s3;
			}
			s += s2;
			ATLTRACE((LPCTSTR)s);
			if (w->IsGroup())
				indent++;
		}
		ATLTRACE(_T("END\n"));
	#endif
	}

protected:
	WINRECT*	m_map;			// THE window map

	//////////////////
	// Count number of table entries that correspond to windows--ie,
	// that have a child window ID associated with the entry.
	//
	int CountWindows()
	{
		ATLASSERT(m_map);
		int nWin = 0;
		for (WINRECT* w=m_map; !w->IsEnd(); w++) {
			if (w->IsWindow())
				nWin++;
		}
		return nWin;
	}

	//////////////////
	// Send message to parent, then window itself, to get size info.
	//
	BOOL SendGetSizeInfo(SIZEINFO& szi, CWindow* pWnd, UINT nID)
	{
		NMWINMGR nmw;
		nmw.code = NMWINMGR::GET_SIZEINFO;	// request size info
		nmw.idFrom = nID;							// ID of child I'm computing
		nmw.sizeinfo = szi;						// copy

		if (!pWnd->SendMessage(WM_WINMGR, nID, (LPARAM)&nmw)) {
			HWND hwndChild = ::GetDlgItem(pWnd->m_hWnd, nID);
			if (!hwndChild || !::SendMessage(hwndChild,WM_WINMGR,nID,(LPARAM)&nmw))
				return FALSE;
		}
		szi = nmw.sizeinfo; // copy back to caller's struct
		return TRUE;
	}

	
	//////////////////
	// Calculate size/positions for a row or column group This is the main
	// algorithm. If a window is given, it's used to get the min/max size and
	// desired size for TOFIT types.
	// you can override to do wierd stuff or fix bugs
	//
	virtual void CalcGroup(WINRECT* pGroup, CWindow* pWnd)
	{
		// If this bombs, most likely the first entry in your map is not a group!
		ATLASSERT(pGroup && pGroup->IsGroup());
		ATLASSERT(pWnd);

		// adjust total avail by margins
		CRect rcTotal = pGroup->GetRect();
		int w,h;
		if (pGroup->GetMargins(w,h)) {
			w = min(abs(w), rcTotal.Width()/2);
			h = min(abs(h), rcTotal.Height()/2);
			rcTotal.DeflateRect(w,h);
		}
		
		BOOL bRow = pGroup->IsRowGroup();		 // Is this a row group?

		// Running height or width: start with total
		int hwRemaining = bRow ? rcTotal.Height() : rcTotal.Width();

		// First, set all rects to their minimum sizes.
		// This ensures that each rect gets its min size.
		CWinGroupIterator it;
		for (it=pGroup; it; it.Next()) {
			WINRECT* wrc = it;
			SIZEINFO szi;
			OnGetSizeInfo(szi, wrc, pWnd);
			int hwMin = bRow ? szi.szMin.cy : szi.szMin.cx;
			hwMin = min(hwMin, hwRemaining);		// truncate
			wrc->SetHeightOrWidth(hwMin, bRow);	// set
			hwRemaining -= hwMin;					// decrement remaining height/width
			ATLASSERT(hwRemaining>=0);
		}

		// Now adjust all rects upward to desired size. Save REST rect for last.
		WINRECT* pRestRect = NULL;
		for (it=pGroup; it; it.Next()) {
			WINRECT* wrc = it;
			if (wrc->Type()==WRCT_REST) {
				ATLASSERT(pRestRect==NULL);		 // can only be one REST rect!
				pRestRect = wrc;					 // remember it
			} else {
				AdjustSize(wrc, bRow, hwRemaining, pWnd);
			}
		}
		ATLASSERT(hwRemaining>=0);

		// Adjust REST rect if any
		if (pRestRect) {
			AdjustSize(pRestRect, bRow, hwRemaining, pWnd);
			ATLASSERT(hwRemaining==0);
		}

		// All the sizes of the entries have been calculated, including
		// groups (but not their children). Now move all the rects so they're
		// adjacent to one another, without altering sizes.
		PositionRects(pGroup, rcTotal, bRow);

		// Finally, descend recursively into each subgroup.
		for (it=pGroup; it; it.Next()) {
			WINRECT* wrc = it;
			if (wrc->IsGroup())
				CalcGroup(wrc, pWnd); // recurse!
		}
	}
	//////////////////
	// Adjust the size of a single entry upwards to its desired size.
	// Decrement hwRemaining by amount increased.
	//
	virtual void AdjustSize(WINRECT* wrc, BOOL bRow,
		int& hwRemaining, CWindow* pWnd)
	{
		SIZEINFO szi;
		OnGetSizeInfo(szi, wrc, pWnd);
		int hw = bRow ? szi.szDesired.cy : szi.szDesired.cx; // desired ht or wid
		if (wrc->Type() == WRCT_REST) {
			// for REST type, use all remaining space
			CRect& rc = wrc->GetRect();
			hw = hwRemaining + (bRow ? rc.Height() : rc.Width());
		}

		// Now hw is the desired height or width, and the current size of the
		// entry is the min size. So adjust the size upwards, and decrement
		// hwRemaining appropriately. This is a little confusing, but necessary so
		// each entry gets its min size.
		//
		int hwCurrent = wrc->GetHeightOrWidth(bRow); // current size
		int hwExtra = hw - hwCurrent;						// amount extra
		hwExtra = min(max(hwExtra, 0), hwRemaining);	// truncate 
		hw = hwCurrent + hwExtra;							// new height-or-width
		wrc->SetHeightOrWidth(hw, bRow);				// set...
		hwRemaining -= hwExtra;								// and adjust remaining
	}
	//////////////////
	// Position all the rects so they're as wide/high as the total and follow one
	// another; ie, are adjacent. This operation leaves the height (rows) and
	// width (columns) unaffected. For rows, set each row's width to rcTotal and
	// one below the other; for columns, set each column as tall as rcTotal and
	// each to the right of the previous.
	//
	virtual void PositionRects(WINRECT* pGroup, const CRect& rcTotal, BOOL bRow)
	{
		LONG xoryPos = bRow ? rcTotal.top : rcTotal.left;

		CWinGroupIterator it;
		for (it=pGroup; it; it.Next()) {
			WINRECT* wrc = it;
			CRect& rc = wrc->GetRect();
			if (bRow) {							 // for ROWS:
				LONG height = rc.Height();		 // height of row = total height
				rc.top    = xoryPos;				 // top = running yPos
				rc.bottom = rc.top + height;	 // ...
				rc.left   = rcTotal.left;		 // ...
				rc.right  = rcTotal.right;		 // ...
				xoryPos += height;				 // increment yPos

			} else {									 // for COLS:
				LONG width = rc.Width();		 // width = total width
				rc.left    = xoryPos;			 // left = running xPos
				rc.right   = rc.left + width;	 // ...
				rc.top     = rcTotal.top;		 // ...
				rc.bottom  = rcTotal.bottom;	 // ...
				xoryPos += width;					 // increment xPos
			}
		}
	}


private:
	CWinMgr() { ATLASSERT(FALSE); } // no default constructor
};

#endif // WINMGR_H_