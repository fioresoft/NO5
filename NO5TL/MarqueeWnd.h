// MarqueeWnd.h: interface for the CMarqueeWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MARQUEEWND_H__6FC6D3D9_A434_40DC_A340_D7254E753FA2__INCLUDED_)
#define AFX_MARQUEEWND_H__6FC6D3D9_A434_40DC_A340_D7254E753FA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLGDIX_H__
#error include atlgdix.h first
#endif

#include <no5tl\no5tlbase.h>	// CPointerArray
#include <no5tl\utils.h>		// CDCSaver

// MarQuee Notifications
#ifndef MQN_CLICKED
#define MQN_CLICKED		1
#define MQN_RCLICKED	2
#define MQN_DBLCLK		3
#define MQN_CHANGED		4
#endif


namespace NO5TL
{

class CMarqueeWnd  : \
	public CWindowImpl<CMarqueeWnd>,
	public COffscreenDraw<CMarqueeWnd>
{
	enum MyTimers
	{
		ID_TIMER = 1,
	};
	enum menu_ids
	{
		ID_MARQUEE_FASTER = 1000,
		ID_MARQUEE_SLOWER,
		ID_MARQUEE_BKCLR,
		ID_MARQUEE_TXTCLR,
		ID_MARQUEE_FONT,
		ID_MARQUEE_EMPTY,
		ID_MARQUEE_PAUSE,
		ID_MARQUEE_DEF,
	};
	struct Item
	{
		CString s;
		int pos;
		bool bDelete;
		CSize sz;		// size of the string with the selected font, in pixels

		Item(void)
		{
			pos = 0;
			bDelete = false;
			sz.cx = 0;
			sz.cy = 0;
		}
	};
public:
	DECLARE_WND_CLASS_EX(NULL,CS_VREDRAW|CS_HREDRAW|CS_DBLCLKS,COLOR_WINDOW)

	BEGIN_MSG_MAP(CMarqueeWnd)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		MESSAGE_HANDLER(WM_TIMER,OnTimer)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN,OnRButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK,OnDoubleClick)
		MESSAGE_HANDLER(WM_SETFONT,OnSetFont)
		MESSAGE_HANDLER(WM_GETFONT,OnGetFont)
#ifdef MARQUEE_DEBUG
		MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMove)
#endif // MARQUEE_DEBUG
		CHAIN_MSG_MAP(COffscreenDraw<CMarqueeWnd>)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetFont((HFONT)GetStockObject(ANSI_FIXED_FONT));
		return 0;
	}
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Stop();
		return 0;
	}
	
	void DoPaint(CDCHandle dc)
	{
		NO5TL::CDCSaver dcs(dc);
		CRect rcClient;
		LPCTSTR p;
		CPoint pt;
		int i;
		const int count = m_items.GetSize();
		Item *pItem;
		bool bPaint = true;
		Item *pOld = NULL;
		CRect rcText;

		PrepareDC(dc);
		dc.SelectFont(m_font);
		dc.SetBkColor(m_clrBack);
		dc.SetTextColor(m_clrTxt);

		GetClientRect(&rcClient);
		dc.DPtoLP(&rcClient);
		dc.FillSolidRect(&rcClient,m_clrBack);
		dc.SetTextAlign(TA_TOP);

		for(i=0;i<count && bPaint;i++){
			pItem = m_items[i];
			bPaint = pItem->pos > -pItem->sz.cx;
			if(bPaint){
				p = (LPCTSTR)pItem->s;
				pt.x = pItem->pos;
				pt.y = rcClient.top + rcClient.Height()/2 - pItem->sz.cy / 2;
				dc.TextOut(pt.x,pt.y,p,lstrlen(p));
			}
		}
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MovePos();
		return 0;
	}
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(m_timer){
			Stop();
		}
		else{
			Start();
		}
		NotifyParent(MQN_CLICKED);
		return 0;
	}
#ifdef MARQUEE_DEBUG
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CWindow wnd = GetTopLevelParent();
		CPoint pt(LOWORD(lParam),HIWORD(lParam));
		CClientDC dc = m_hWnd;
		CString s;

		PrepareDC(dc);
		dc.DPtoLP(&pt);
		s.Format("(%d,%d)",pt.x,pt.y);
		wnd.SetWindowText(s);

		
		return 0;
	}
#endif // MARQUEE_DEBUG
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#ifdef MARQUEE_DEBUG
		MovePos();
#endif
		if(NotifyParent(MQN_RCLICKED))
			return 0;

		CMenu menu;
		CPoint pt;
		const DWORD def = MF_ENABLED|MF_STRING;
		DWORD flags = def;
		bool changed = true;

		menu.CreatePopupMenu();
		if(GetElapse() <= 2 )
			flags = def | MF_GRAYED;
		else
			flags = def;
		menu.AppendMenu(flags,ID_MARQUEE_FASTER,"Faster");
		
		if(GetElapse() >=1024)
			flags = def | MF_GRAYED;
		else
			flags = def;
		menu.AppendMenu(flags,ID_MARQUEE_SLOWER,"Slower");
		
		menu.AppendMenu(MF_ENABLED|MF_SEPARATOR,-1);
		menu.AppendMenu(MF_ENABLED|MF_STRING,ID_MARQUEE_BKCLR,"Background color");
		menu.AppendMenu(MF_ENABLED|MF_STRING,ID_MARQUEE_TXTCLR,"Font color");
		//menu.AppendMenu(MF_ENABLED|MF_STRING,ID_MARQUEE_FONT,"Font");
		menu.AppendMenu(MF_ENABLED|MF_SEPARATOR,-1);
		menu.AppendMenu(MF_ENABLED|MF_STRING,ID_MARQUEE_EMPTY,"Empty");
		menu.AppendMenu(MF_ENABLED|MF_STRING,ID_MARQUEE_PAUSE,IsPaused() ? "continue" : "pause");
		menu.AppendMenu(MF_ENABLED|MF_SEPARATOR,-1);
		menu.AppendMenu(MF_ENABLED|MF_STRING,ID_MARQUEE_DEF,"Default values");
		GetCursorPos(&pt);
		int cmd = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_NONOTIFY|TPM_RETURNCMD|\
			TPM_LEFTBUTTON,pt.x,pt.y,m_hWnd);
		if(cmd){
			switch(cmd){
				case ID_MARQUEE_FASTER:
					if(m_nElapse > 2)
						SetElapse(GetElapse() >> 1);
					else
						changed = false;
					break;
				case ID_MARQUEE_SLOWER:
					if(m_nElapse < 0xffff)
						SetElapse(GetElapse() << 1);
					else
						changed = false;
					break;
				case ID_MARQUEE_BKCLR:
					{
						CColorDialog dlg(GetBackColor());
						if(IDOK == dlg.DoModal())
							SetBackColor(dlg.GetColor());
						else
							changed = false;
					}
					break;
				case ID_MARQUEE_TXTCLR:
					{
						CColorDialog dlg(GetTextColor());
						if(IDOK == dlg.DoModal())
							SetTextColor(dlg.GetColor());
						else
							changed = false;
					}
					break;
				case ID_MARQUEE_FONT:
				{
					LOGFONT lf = {0};
					m_font.GetLogFont(&lf);
					CFontDialog dlg(&lf);
					if(IDOK == dlg.DoModal()){
						CFont font;
						font.CreatePointFont(dlg.GetSize(),dlg.GetFaceName());
						SetFont(font);
						font.Detach();
					}
					else
						changed = false;
				}
				case ID_MARQUEE_EMPTY:
					Empty();
					changed = false;
					break;
				case ID_MARQUEE_PAUSE:
					if(IsPaused())
						Start();
					else
						Stop();
					changed = false;
					break;
				case ID_MARQUEE_DEF:
					SetTextColor(0xffffff);
					SetBackColor(0x008000);
					SetElapse(4);
					changed = true;
					break;
				default:
					changed = false;
					break;
			}
			if(changed)
				NotifyParent(MQN_CHANGED);
		}

		return 0;
	}
	LRESULT OnDoubleClick(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		NotifyParent(MQN_DBLCLK);
		return 0;
	}
	LRESULT OnSetFont(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HFONT hFont = HFONT(wParam);
		BOOL bRedraw = LOWORD(lParam) ? TRUE : FALSE;

		Empty();
		m_font = hFont;
		if(bRedraw)
			InvalidateRect(NULL);
		return 0;
	}
	LRESULT OnGetFont(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return LRESULT(HFONT(m_font));
	}

	CMarqueeWnd( bool bToLeft = true):m_bToLeft(bToLeft)
	{
		m_timer = 0;
		m_nElapse = 128;
		m_clrBack = 0;
		m_clrTxt = 0xFFFFFF;
		m_bLoop = true;
		m_bpi = 1;
		m_space = 0;
	}
	virtual ~CMarqueeWnd()
	{
		//
	}
public:
	BOOL AddItem(LPCTSTR s)
	{
		ATLASSERT(IsWindow());

		Item *p = new Item();
		p->s = s;
		p->bDelete = false;
		{
			CSize sz;
			CClientDC dc = m_hWnd;
			HFONT hFont = dc.SelectFont(m_font);
			dc.GetTextExtent(s,lstrlen(s),&p->sz);
			dc.SelectFont(hFont);
		}
		if(!m_bToLeft)
			p->pos = -(p->sz.cx);
		else
			p->pos = 0;

		return m_items.Add(p);
	}
	void SetBackColor(COLORREF clr)
	{
		m_clrBack = clr;
		if(IsWindow())
			InvalidateRect(NULL);
	}
	void SetTextColor(COLORREF clr)
	{
		m_clrTxt = clr;
		if(IsWindow())
			InvalidateRect(NULL);
	}
	COLORREF GetBackColor(void) const
	{
		return m_clrBack;
	}
	COLORREF GetTextColor(void) const
	{
		return m_clrTxt;
	}
	void Stop(void)
	{
		ATLASSERT(IsWindow());
		if(m_timer){
			KillTimer(ID_TIMER);
			m_timer = 0;
		}
	}
	void Start(void)
	{
		ATLASSERT(IsWindow());
		if(!m_timer){
			m_timer = SetTimer(ID_TIMER,m_nElapse);
		}
	}
	void SetLoop(bool bLoop)
	{
		m_bLoop = bLoop;
	}
	void SetElapse(UINT nElapse)
	{
		m_nElapse = nElapse;
		if(m_timer){
			Stop();
			Start();
		}
	}
	UINT GetElapse(void) const
	{
		return m_nElapse;
	}
	void SetInc(LONG bpi)
	{
		m_bpi = bpi;
		if(IsWindow())
			InvalidateRect(NULL);
	}
	LONG GetInc(void) const
	{
		return m_bpi;
	}
	void SetSpace(LONG nSpace)
	{
		m_space = nSpace;
		if(IsWindow())
			InvalidateRect(NULL);
	}
	// average char width
	LONG GetCharWidth(void)
	{
		CClientDC dc = m_hWnd;
		TEXTMETRIC tm;
		HFONT hFont = dc.SelectFont(m_font);
		int iMode = dc.SetMapMode(MM_TEXT);

		dc.GetTextMetrics(&tm);
		dc.SelectFont(hFont);
		dc.SetMapMode(iMode);
		return tm.tmAveCharWidth;
	}
	void Empty(void)
	{
		m_items.RemoveAll();
		if(IsWindow())
			InvalidateRect(NULL);
	}
	bool IsPaused(void)
	{
		return m_timer == 0;
	}
private:
	UINT		m_timer;
	UINT		m_nElapse;
	CFontHandle	m_font;
	COLORREF	m_clrBack;
	COLORREF	m_clrTxt;
	NO5TL::CPointerArray<Item> m_items;
	bool		m_bLoop;
	const bool	m_bToLeft;
	LONG		m_bpi;				// bits per increment
	LONG		m_space;			// space between items, in pixels

	void MovePos(void)
	{
		int count = m_items.GetSize();
		int i;
		bool bMove = true;
		Item *pItem;
		bool bDroped;

		for(i=0;i<count && bMove;i++){
			pItem = m_items[i];
			pItem->pos += m_bpi;

			// check if item has moved outside of client area
			bDroped = IsItemDropped(pItem);
			if(bDroped){ 
				if(m_bLoop){
					AddItem(pItem->s);
				}
				pItem->bDelete = true;
			}
			// flag marking if the next item can be moved
			bMove = CanMoveNext(pItem);
			// invalidate area so item show up correctly
			InvalidateItemRect(pItem);
		}
		for(i=0;i<count;i++){
			pItem = m_items[i];
			if(pItem->bDelete){
				m_items.RemoveAt(i);
				i--;
				count--;
			}
		}
	}
	bool IsItemDropped(Item *pItem)
	{
		CRect rcClient;
		bool bRes = false;

		if(pItem->pos > 0){
			GetClientRect(&rcClient);
			if(!m_bToLeft){
				if(pItem->pos > rcClient.right)
					bRes = true;
			}
			else{
				if(pItem->pos > (rcClient.right + pItem->sz.cx ))
					bRes = true;
			}
		}
		return bRes;
	}
	bool CanMoveNext(Item *pItem)
	{
		bool res = false;

		if(!m_bToLeft){
			res = pItem->pos > m_space;
		}
		else{
			res = pItem->pos > ( pItem->sz.cx + m_space);
		}
		return res;
	}
	void InvalidateItemRect(Item *pItem)
	{
		CRect r;
		CClientDC dc = m_hWnd;

		PrepareDC(dc);
		GetClientRect(&r);

		r.top = r.top + r.Height() / 2 - pItem->sz.cy - 1;
		r.bottom = r.top + r.Height() / 2 + pItem->sz.cy + 1;

		dc.DPtoLP(&r);
		if(!m_bToLeft){
			r.left = pItem->pos - m_bpi;
			r.right = pItem->pos + pItem->sz.cx;
		}
		else{
			r.left = pItem->pos;
			r.right = pItem->pos - pItem->sz.cx - m_bpi;
		}
		if(r.left < 0)
			r.left = 0;
		if(r.right < 0)
			r.right = 0;
		
		dc.LPtoDP(&r);
		InvalidateRect(&r,FALSE);

	}

	void PrepareDC(HDC hdc)
	{
		CDCHandle dc = hdc;
		CRect rcClient;

		GetClientRect(&rcClient);
		dc.SetMapMode(MM_ANISOTROPIC);
		dc.SetWindowExt(1,1);
		if(m_bToLeft){
			dc.SetViewportExt(-1,1);
			dc.SetViewportOrg(rcClient.right,0);
		}
		else{
			dc.SetViewportExt(1,1);
		}
	}
	LRESULT NotifyParent(UINT uCode)
	{
		CWindow wnd = GetParent();
		return wnd.SendMessage(WM_COMMAND,MAKEWPARAM(WORD(GetDlgCtrlID()),WORD(uCode)),LPARAM(m_hWnd));
	}
};
} // NO5TL

#endif // !defined(AFX_MARQUEEWND_H__6FC6D3D9_A434_40DC_A340_D7254E753FA2__INCLUDED_)
