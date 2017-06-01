// WinGrid.h: interface for the CWinGrid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINGRID_H__4FBD6D81_8D2F_4B88_9A08_00D8EBDA0C17__INCLUDED_)
#define AFX_WINGRID_H__4FBD6D81_8D2F_4B88_9A08_00D8EBDA0C17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WG_BEGIN(name) static CWinCell * GetGrid(void) { static CWinCell name[] = { 
#define WG_CELL(type,val,id)		CWinCell(type,val,id),
#define WG_BEGINROWS(type,val)		CWinCell((type|CWinCell::WC_GROUP_ROW),val,0),
#define WG_BEGINCOLS(type,val)		CWinCell((type|CWinCell::WC_GROUP_COL),val,0),
#define WG_GROUPEND()				CWinCell((CWinCell::WC_GROUP_END),0,0),
#define WG_END(name)				}; return name; }

namespace NO5TL
{

class CWinCell 
{
public:
	enum
	{
		WC_TYPEMASK =	0x0F,
		WC_REST =		0x01,
		WC_FIXED =		0x02,
		WC_PERCENT =	0x03,
		WC_TOFIT =		0x04,
		WC_SPACE =		0x05,
		WC_GROUPMASK =	0xF0,
		WC_GROUP_COL =	0x10,
		WC_GROUP_ROW =	0x20,
		WC_GROUP_END =	0x30,
	};	
	LONG m_cxyDesired;	// desired value
	LONG m_cxyMin;
	LONG m_cxyMax;
	CSize m_szAvail;	// actual value after calculation
	int m_type;
	UINT m_id;
	CWinCell *m_pNext;
	CWinCell *m_pPrev;
	CWinCell *m_pChild;
	CWinCell *m_pParent;
	static UINT m_uMsg;
public:
	CWinCell(int type,LONG val,UINT id)
	{
		m_type = type;
		m_cxyDesired = val;
		m_id = id;
		m_cxyMin = 0;
		m_cxyMax = 0;
		m_szAvail.cx = m_szAvail.cy = 0;
		m_pNext = NULL;
		m_pChild = NULL;
		m_pParent = NULL;
		m_pPrev = NULL;
	}
	~CWinCell()
	{
		//
	}
	bool IsGroupBegin(void) const
	{
		const DWORD type = (m_type & WC_GROUPMASK);
		return (type == WC_GROUP_ROW || type == WC_GROUP_COL);
	}
	bool IsGroupEnd(void) const
	{
		return (m_type & WC_GROUPMASK) == WC_GROUP_END;
	}
	bool IsRowGroup(void) const
	{
		const DWORD type = (m_type & WC_GROUPMASK);
		return (type == WC_GROUP_ROW);
	}
	bool IsColGroup(void) const
	{
		const DWORD type = (m_type & WC_GROUPMASK);
		return (type == WC_GROUP_COL);
	}
	DWORD GetCellType(void) const
	{
		return (m_type & WC_TYPEMASK);
	}
	
	static CWinCell * MakeList(CWinCell *c /* flat vector starting with group begin */)
	{
		CWinCell *next;
		CWinCell *parent = c;

		ATLASSERT(c->IsGroupBegin());
		c++;
		if(!c->IsGroupEnd())
			parent->m_pChild = c;

		while(!c->IsGroupEnd()){
			c->m_pParent = parent;
			if(c->IsGroupBegin()){
				next = MakeList(c);
			}
			else if(c->IsGroupEnd()){
				ATLASSERT(0);
			}
			else
				next = c + 1;
			if(! next->IsGroupEnd()){
				c->m_pNext = next;
				c->m_pNext->m_pPrev = c;
			}
			c = next;
		}
		return c + 1;	// returns end of group + 1
	}
};	
	

template <class T>
class CWinGrid
{
public:
	BEGIN_MSG_MAP(CWinGrid)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
	END_MSG_MAP()

	CWinGrid()
	{
		//
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CRect r(0,0,LOWORD(lParam),HIWORD(lParam));
		T *pT = static_cast<T*>(this);

		CWinCell *pTop = pT->GetGrid();
		pTop->m_szAvail.cx = r.Width();
		pTop->m_szAvail.cy = r.Height();
		CalcGroup(pTop);
		MoveGroup(CPoint(0,0),pTop);
		return 0;
	}

	/*static */ void CalcGroup(CWinCell *pGroup)
	{
		LONG cxyRest;
		LONG cxyTotal;
		LONG cxy;
		bool isrow;
		CWinCell *c;
		CWinCell *pRest = NULL;

		ATLASSERT(pGroup->IsGroupBegin());
		if(!pGroup->IsGroupBegin())
			return;

		isrow = pGroup->IsRowGroup();
		cxyRest = isrow ? pGroup->m_szAvail.cy : pGroup->m_szAvail.cx;
		cxyTotal = cxyRest;
		

		c = pGroup->m_pChild;
		while(c){
			cxy = 0;

			switch(c->GetCellType()){
				case CWinCell::WC_REST:
					ATLASSERT(pRest == NULL);
					pRest = c;
					break;
				case CWinCell::WC_FIXED:
					cxy = c->m_cxyDesired;
					break;
				case CWinCell::WC_PERCENT:
					cxy =(c->m_cxyDesired * cxyTotal) / 100;
					break;
				case CWinCell::WC_SPACE:
					cxy = c->m_cxyDesired;
					break;
				case CWinCell::WC_TOFIT:
					{
						ATLASSERT(c->m_id > 0);
						T *pT = static_cast<T*>(this);
						CWindow wnd = pT->GetDlgItem(c->m_id);
						ATLASSERT(wnd.IsWindow());
						cxy = wnd.SendMessage(CWinCell::m_uMsg);
					}
					break;
				default:
					break;
			}
			if(cxy){
				cxy = min(cxy,cxyRest);
				cxyRest -= cxy;
				if(isrow){
					c->m_szAvail.cy = cxy;
					c->m_szAvail.cx = pGroup->m_szAvail.cx;
				}
				else{
					c->m_szAvail.cx = cxy;
					c->m_szAvail.cy = pGroup->m_szAvail.cy;
				}
			}
			c = c->m_pNext;
		}
		if(pRest){
			if(isrow){
				pRest->m_szAvail.cx = pGroup->m_szAvail.cx;
				pRest->m_szAvail.cy = cxyRest;
			}
			else{
				pRest->m_szAvail.cx = cxyRest;
				pRest->m_szAvail.cy = pGroup->m_szAvail.cy;
			}

		}
		// calcula os filhos dos grupos
		c = pGroup->m_pChild;
		while(c){
			if(c->IsGroupBegin())
				CalcGroup(c);
			c = c->m_pNext;
		}
	}
	void MoveGroup(CPoint &pt,CWinCell *pGroup)
	{
		CWinCell *c;
		LONG nTopLeft;
		T *pT = static_cast<T*>(this);
		CWindow wnd;

		ATLASSERT(pGroup->IsGroupBegin());
		if(!pGroup->IsGroupBegin())
			return;

		nTopLeft = pGroup->IsRowGroup() ? pt.y : pt.x;
		c = pGroup->m_pChild;
		while(c){
			if(pGroup->IsRowGroup()){
				if(!c->IsGroupBegin()){
					if( c->GetCellType() != CWinCell::WC_SPACE ){
						ATLASSERT(c->m_id > 0);
						wnd = pT->GetDlgItem(c->m_id);
						wnd.MoveWindow(pt.x,nTopLeft,c->m_szAvail.cx,c->m_szAvail.cy);
					}
				}
				else{
					CPoint pt2;
					pt2.x = pt.x;
					pt2.y = nTopLeft;
					MoveGroup(pt2,c);
				}
			}
			else{
				if(!c->IsGroupBegin()){
					if( c->GetCellType() != CWinCell::WC_SPACE ){
						ATLASSERT(c->m_id > 0);
						wnd = pT->GetDlgItem(c->m_id);
						wnd.MoveWindow(nTopLeft,pt.y,c->m_szAvail.cx,c->m_szAvail.cy);
					}
				}
				else{
					CPoint pt2;
					pt2.x = nTopLeft;
					pt2.y = pt.y;
					MoveGroup(pt2,c);
				}
			}
			nTopLeft += (pGroup->IsRowGroup() ? c->m_szAvail.cy : c->m_szAvail.cx );
			c = c->m_pNext;
		}
	}
	CWinCell * FindCell(CWinCell *c,UINT id)
	{
		CWinCell *res = NULL;

		while(c){
			if(c->m_id == id){
				res = c;
				break;
			}
			if(c->m_pChild){
				res = FindCell(c,id);
				if(res)
					break;
			}
			c = c->m_pNext;
		}
		return res;
	}
};

} // NO5TL

#endif // !defined(AFX_WINGRID_H__4FBD6D81_8D2F_4B88_9A08_00D8EBDA0C17__INCLUDED_)
