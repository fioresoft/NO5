// No5Controls.h: interface for the No5Controls class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NO5CONTROLS_H__B959AC0B_354B_11D7_A179_FE6E78F0B835__INCLUDED_)
#define AFX_NO5CONTROLS_H__B959AC0B_354B_11D7_A179_FE6E78F0B835__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __ATLCTRLS_H__
  #error no5controls.h requires atlctrls.h to be included first
#endif

#ifndef __ATLFRAME_H__
  #error no5controls.h requires atlframe.h to be included first
#endif

#include "no5window.h"
#include "utils.h"
#include "color.h"

namespace NO5TL
{

template <class T>
class CNo5ListBoxT:public CListBoxT<T>
{
	typedef CListBoxT<CNo5Window> _BaseClass;
public:
	CNo5ListBoxT(HWND hWnd = NULL):CListBoxT<T>(hWnd)
	{
		//
	}
	CNo5ListBoxT<T> & operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	CNo5ListBoxT<T> & operator<<(LPCTSTR s)
	{
		AddString(s);
		return *this;
	}
	const CNo5ListBoxT<T> & operator>>(CString &s) const
	{
		int index = GetCurSel();

		if(index >= 0){
			GetText(index,s);
		}
		return *this;
	}

	// returns the number of characters copied
	int GetText(int index,LPTSTR buf) const
	{
		return _BaseClass::GetText(index,buf);
	}
	// returns the number of characters copied
	int GetText(int index,CString &s) const
	{
		const int len = GetTextLen(index);
		int res = 0;

		if(len >= 0){
			s.Empty();
			LPTSTR buf = s.GetBuffer(len);
			res = GetText(index,buf);
			s.ReleaseBuffer();
		}
		return res;
	}

	int DeleteString(UINT index)
	{
		return CListBoxT<CNo5Window>::DeleteString(index);
	}
	int DeleteString(LPCTSTR s)
	{
		int index = FindStringExact(-1,s);

		if(index >= 0){
			index = DeleteString((UINT)index);
		}
		return index;
	}
	int AddItem(LPCTSTR s,LPARAM lParam)
	{
		int res = AddString(s);

		if(res >= 0){
			SetItemData(res,lParam);
		}
		return res;
	}
	int InsertItem(int index,LPCTSTR s,LPARAM param)
	{
		int res = InsertString(index,s);

		if(res >= 0){
			SetItemData(res,param);
		}
		return res;
	}
	BOOL GetItem(int index,CString &s,LPARAM  &param) const
	{
		BOOL res = FALSE;
		if(index >= 0 && index < GetCount()){
			GetText(index,s);
			param = (LPARAM)GetItemData(index);
			res = TRUE;
		}
		return res;
	}
	BOOL MoveItem(int to,int from)
	{
		BOOL res;
		CString s;
		LPARAM param = 0;
		
		if((res = (to >= 0)) && to < GetCount()){
			if(res = GetItem(from,s,param)){
				DeleteString((UINT)from);
				res = InsertItem(to,s,param) >= 0;
			}
		}
		return res;
	}
	int FindItemByParam(LPARAM lParam) const
	{
		const int count = GetCount();
		int i;

		for(i=0;i<count;i++){
			if(lParam == (LPARAM)GetItemData(i))
				break;
		}
		if(i == count)
			i = -1;
		return i;
	}
};

typedef CNo5ListBoxT<CNo5Window> CNo5ListBox;

// handles "page" size
// create a spin in a window/dlg and in its msg-map add 
// CHAIN_MSG_MAP_MEMBER(/* the spin control here */)
class CSpin:public CUpDownCtrl
{
	WORD m_page;
public:
	BEGIN_MSG_MAP(CSpin)
		NOTIFY_CODE_HANDLER(UDN_DELTAPOS,OnDeltaPos)
	END_MSG_MAP()

	LRESULT OnDeltaPos(int id, LPNMHDR pnmh,BOOL& bHandled)
	{
		if(id == GetDlgCtrlID()){
			LPNMUPDOWN p = (LPNMUPDOWN)pnmh;

			p->iDelta = (p->iDelta >= 0)? m_page:(-m_page);
		}
		else{
			bHandled = FALSE;
		}
		return 0;
	}
public:
	CSpin(WORD page = 1):m_page(page)
	{
		//
	}
	void SetPage(WORD page)
	{
		m_page = page;
	}
	WORD GetPage(void) const
	{
		return m_page;
	}
};

template <class TBase>
class CNo5TabCtrlT:public CTabCtrlT<TBase>
{
	typedef CTabCtrlT<TBase> _BaseClass;
public:
	// the WTL implementation has a bug since it casts the result
	// to a BOOL, but the result is actually the index of the item
	// inserted or -1 for error, so a result of 0 is not an error
	int InsertItem(int nItem, LPTCITEM pTabCtrlItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int res = ::SendMessage(m_hWnd, TCM_INSERTITEM, nItem, (LPARAM)pTabCtrlItem);
		ATLASSERT(res >= 0);
		return res;
	}
	int InsertItem(int nItem,LPCTSTR label,int iImage,LPARAM lParam)
	{
		TCITEM item = {0};

		item.mask = TCIF_PARAM;
		item.lParam = lParam;

		if(label != NULL){
			item.mask |= TCIF_TEXT;
			item.pszText = const_cast<LPTSTR>(label);
		}
		if(iImage >= 0){
			item.mask |= TCIF_IMAGE;
			item.iImage = iImage;
		}
		return InsertItem(nItem,&item);
	}
	int AddItem(LPTCITEM pItem)
	{
		return InsertItem(GetItemCount(),pItem);
	}
	int AddItem(LPCTSTR label,int iImage,LPARAM lParam)
	{
		const int count = GetItemCount();
		return InsertItem(count,label,iImage,lParam);
	}
	CString GetItemLabel(int nItem) const
	{
		TCITEM item = {0};
		CString s;
		BOOL res;

		item.mask = TCIF_TEXT;
		item.pszText = s.GetBuffer(256);
		item.cchTextMax = 255;
		res = GetItem(nItem,&item);
		ATLASSERT(res);
		s.ReleaseBuffer();
		return s;
	}
	LPARAM GetItemParam(int nItem) const
	{
		TCITEM item = {0};
		BOOL res;

		item.mask = TCIF_PARAM;
		res = GetItem(nItem,&item);
		ATLASSERT(res);
		return item.lParam;
	}
	int FindItem(LPCTSTR label) const
	{
		const int count = GetItemCount();
		int i;
		CString s;

		for(i=0;i<count;i++){
			s = GetItemLabel(i);
			s.CompareNoCase(label);
			if(s == i)
				break;
		}
		return ((i == count) ? -1 : i);
	}
	int FindItem(LPARAM lParam) const
	{
		const int count = GetItemCount();
		int i;
		LPARAM res;

		for(i=0;i<count;i++){
			res = GetItemParam(i);
			if(res == lParam)
				break;
		}
		return ((i == count) ? -1 : i);
	}
	int SetCurSel(int nItem,BOOL bNotify)
	{
		NMHDR nm = {0};
		CWindow parent = GetParent();
		int res;
		BOOL bRes = FALSE;

		if(bNotify){
			nm.code = TCN_SELCHANGING;
			nm.hwndFrom = m_hWnd;
			nm.idFrom = GetDlgCtrlID();
			
			bRes = (BOOL)parent.SendMessage(WM_NOTIFY,0,(LPARAM)\
				&nm);
		}
		if(!bRes){
			res = _BaseClass::SetCurSel(nItem);

			if(bNotify){
				nm.code = TCN_SELCHANGE;
				parent.SendMessage(WM_NOTIFY,0,(LPARAM)&nm);
			}
		}
		return res;
	}
};

typedef CNo5TabCtrlT<CNo5Window> CNo5TabCtrl;

// add REFLECT_NOTIFICATIONS to the parents window msg map

template <class T,class TTabControl = CNo5TabCtrl,class TWinTraits = CControlWinTraits>
class CTabbedPanesT:public CWindowImpl<T,TTabControl,TWinTraits>
{
	struct Param
	{
		LPARAM	m_lParam;
		HWND	m_hWnd;
		//
		Param():m_hWnd(0),m_lParam(0)
		{
			//
		}
		Param(HWND hWnd,LPARAM lParam):m_hWnd(hWnd),m_lParam(lParam)
		{
			//
		}
	};
	typedef CWindowImpl<T,TTabControl,TWinTraits> _BaseClass;
	typedef CTabbedPanesT<T,TTabControl,TWinTraits> _ThisClass;
public:
	DECLARE_WND_SUPERCLASS(NULL,_BaseClass::GetWndClassName());

	BEGIN_MSG_MAP(_ThisClass)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		REFLECTED_NOTIFY_CODE_HANDLER(TCN_SELCHANGING,OnTabSelChanging)
		REFLECTED_NOTIFY_CODE_HANDLER(TCN_SELCHANGE,OnTabSelChange)
		DEFAULT_REFLECTION_HANDLER()
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam,
		BOOL& bHandled)
	{
		const int index = GetCurSel();
		
		// moves the window to fit the "display area" of the tab
		if(index >= 0){
			MovePaneToDisplayArea(index);
		}
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnTabSelChanging(int idCtrl, LPNMHDR pnmh, BOOL&\
		bHandled)
	{
		const int index = GetCurSel();
		CWindow wnd;

		if(index >= 0){
			// hides the window at this index
			wnd = GetWindowHandle(index);
			ATLASSERT(wnd.IsWindow());
			wnd.ShowWindow(SW_HIDE);
		}
		bHandled = FALSE;
		return (LRESULT)FALSE; // allow sel change
	}

	LRESULT OnTabSelChange(int idCtrl, LPNMHDR pnmh, BOOL&\
		bHandled)
	{
		const int index = GetCurSel();
		CWindow wnd;

		if(index >= 0){
			// shows the window at this index
			wnd = GetWindowHandle(index);
			ATLASSERT(wnd.IsWindow());
			MovePaneToDisplayArea(index);
			wnd.ShowWindow(SW_SHOW);
		}
		return 0;
	}
private:
	void MovePaneToDisplayArea(int index)
	{
		CRect r;
		CWindow wnd = GetWindowHandle(index);
		CWindow wndParent = wnd.GetParent();

		ATLASSERT(wnd.IsWindow());
		GetWindowRect(&r);
		if(r.Width() && r.Height()){
			AdjustRect(FALSE,&r);
			wndParent.ScreenToClient(&r);
			if(!wnd.MoveWindow(r.left,r.top,r.Width(),r.Height(),TRUE))
				ATLASSERT(0);
		}
	}
	int InternalAddItem(HWND hWnd,LPCTSTR label,int image,LPARAM lParam)
	{
		TCITEM item = {0};
		Param *pParam = new Param(hWnd,lParam);

		item.mask = TCIF_PARAM;
		item.lParam = (LPARAM)pParam;

		if(label != NULL){
			item.mask |= TCIF_TEXT;
			item.pszText = const_cast<LPTSTR>(label);
		}
		if(image >= 0){
			item.mask |= TCIF_IMAGE;
			item.iImage = image;
		}

		return AddItem(&item);
	}
public:
	// this overide extracts the lParam of the internal structure
	// Param which is the actual param passed
	LPARAM GetItemParam(int nItem)
	{
		Param *p = (Param *)_BaseClass::GetItemParam(nItem);
		LPARAM lParam = 0;

		ATLASSERT(p);
		if(p){
			lParam = p->m_lParam;
		}
		return lParam;
	}
	// this overide deletes the pointer to the Param structure
	// before deleting the item
	BOOL DeleteItem(int nItem)
	{
		Param *p = (Param *)_BaseClass::GetItemParam(nItem);

		if(p){
			OnBeforeDeleteItem(nItem);
			delete p;
		}
		return _BaseClass::DeleteItem(nItem);
	};
	// this overide deletes all items param before calling the 
	// base class method, also calls the virtual function before
	// deleting each item. and hides the selected window if any
	BOOL DeleteAllItems(void)
	{
		const int count = GetItemCount();
		int i;
		Param *p;
		CWindow wnd = GetWindowHandle(GetCurSel());

		if(wnd.IsWindow())
			wnd.ShowWindow(SW_HIDE);

		for(i=0;i<count;i++){
			p = (Param *)_BaseClass::GetItemParam(i);
			if(p){
				OnBeforeDeleteItem(i);
				delete p;
			}
		}
		return _BaseClass::DeleteAllItems();
	}

	// returns the tab index
	int AddWindow(HWND hWnd,LPCSTR label = NULL,int iImage = -1,
		LPARAM lParam = 0)
	{
		CWindow wnd(hWnd);
		int index;
		
		ATLASSERT(m_hWnd);
		wnd.SetParent(m_hWnd);
		// with the WS_CLIPSIBLINGS style the window doesnt appear
		wnd.ModifyStyle(WS_CLIPSIBLINGS,0);
		wnd.ShowWindow(SW_HIDE);
		index = InternalAddItem(hWnd,label,iImage,lParam);
		if(index >= 0)
			SetCurSel(index,TRUE);
		return index;
	}
	BOOL RemoveWindow(HWND hWnd)
	{
		int index = FindWindow(hWnd);
		BOOL res = FALSE;
		CWindow wnd(hWnd);
		const int cur_sel = GetCurSel();
		
		if(index >= 0){
			if(cur_sel == index)
				wnd.ShowWindow(SW_HIDE);
			res = DeleteItem(index);
			if(res){
				// select another item, if this was selected
				const int count = GetItemCount();
				if(count){
					if(index == count)
						index -= 1;
					SetCurSel(index,TRUE);
				}
			}
		}
		return res;
	}
	int FindWindow(HWND hWnd)
	{
		const int count = GetItemCount();
		int i;
		HWND hNext;

		for(i=0;i<count;i++){
			hNext = GetWindowHandle(i);
			if(hNext == hWnd)
				break;
		}
		if(i == count)
			i = -1;
		return i;
	}
	HWND GetWindowHandle(int index)
	{
		Param *p = (Param *)_BaseClass::GetItemParam(index);
		HWND hWnd = NULL;

		if(p){
			hWnd = p->m_hWnd;
		}
		return hWnd;
	}
	virtual void OnBeforeDeleteItem(int index)
	{
		//
	}
};

class CTabbedPanes : public CTabbedPanesT<CTabbedPanes>
{
	typedef CTabbedPanesT<CTabbedPanes> _BaseClass;
public:
	BEGIN_MSG_MAP(CTabbedPanes)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()
};

typedef CWinTraitsOR<WS_VSCROLL|LBS_DISABLENOSCROLL|\
		LBS_OWNERDRAWFIXED|LBS_NOTIFY,0,CControlWinTraits> \
		CColorListBoxTraits;

// how to use: in normal window, just use CColorListBox::Create
// in dialog boxes, call CColorListBox::SubclassWindow
// No need to use CContainedWindow 
// In both cases, add REFLECT_NOTIFICATIONS() to the list box's
// parent window message map

template <class T,class TBase = CNo5ListBox,class TTraits = CColorListBoxTraits>
class CColorListBoxT:public CWindowImpl<T,TBase,TTraits>,
	public COwnerDraw<T>
{
	typedef CColorListBoxT<T,TBase,TTraits> _ThisClass;
	typedef COwnerDraw<T> _OwnerDrawClass;
	typedef CWindowImpl<T,TBase,TTraits> _BaseClass;
public:

	DECLARE_WND_SUPERCLASS(NULL,_BaseClass::GetWndClassName())
	
	BEGIN_MSG_MAP(_ThisClass)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		CHAIN_MSG_MAP_ALT(_OwnerDrawClass,1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	CColorListBoxT(HWND hWnd = NULL)
	{
		m_hWnd = hWnd;
	}

	LRESULT OnCreate(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		DWORD style = GetStyle();
		ATLASSERT(style & LBS_OWNERDRAWFIXED != 0);
		bHandled = FALSE;
		return 0;
	}
	
	void DrawItem(LPDRAWITEMSTRUCT p)
	{
		int index;
		CColor color;

		ATLASSERT(p->CtlType == ODT_LISTBOX);
		ATLASSERT(p->CtlID == (UINT)GetDlgCtrlID());
		ATLASSERT(p->hwndItem == m_hWnd);

		index = (int)p->itemID;
		color = (COLORREF)p->itemData;

		if(index >= 0){
			CDC dc(p->hDC);
			CDCSaver saver(dc);
			CRect r(p->rcItem);

			r.InflateRect(-2,-2);
			//r.OffsetRect(2,2);
			dc.FillSolidRect(&r,(COLORREF)color);
		}
		SetMsgHandled(FALSE);
	}
	//
	int AddColor(const CColor &color)
	{
		return AddItem(TEXT(""),(LPARAM)(COLORREF)color);
	}
	CColor GetColor(int index) const
	{
		ATLASSERT(index >= 0 && index < GetCount());
		return CColor((COLORREF)GetItemData(index));
	}
	int FindColor(const CColor &color) const
	{
		return FindItemByParam((LPARAM)(COLORREF)color);
	}
	int SelectColor(const CColor &color)
	{
		int index = FindColor(color);

		if(index >= 0){
			index = SetCurSel(index);
		}
		return index;
	}

	T & operator >> (CColor &color) const
	{
		int i = GetCurSel();
		if(i >= 0){
			color = GetColor(i);
		}
		return (T &)*this;
	}

	T & operator << (CColor &color)
	{
		int index = AddColor(color);
		ATLASSERT(index >= 0);
		return (T &)*this;
	}

};

class CColorListBox : public CColorListBoxT<CColorListBox>
{
	typedef CColorListBoxT<CColorListBox> _BaseClass;
public:
	DECLARE_WND_SUPERCLASS(_T("NO5TL_ColorListBox"),_BaseClass::GetWndClassName());

	CColorListBox(HWND hWnd = NULL):_BaseClass(hWnd)
	{
		//
	}
	CColorListBox operator = (HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}
};

template <class TBase>
class CNo5ComboBoxT : public CComboBoxT<TBase>
{
	typedef CComboBoxT<TBase> _BaseClass;
public:
	int SetCurSel(int index)
	{
		return _BaseClass::SetCurSel(index);
	}
	int SetCurSel(LPCTSTR label)
	{
		int index = FindStringExact(-1,label);
		if(index >= 0){
			index = SetCurSel(index);
		}
		return index;
	}
};

typedef CNo5ComboBoxT<CNo5Window> CNo5ComboBox;

/*********** CNo5TreeItem *****************************/

class CNo5TreeItem;

typedef BOOL (* PENUMTREEITEMS)(CNo5TreeItem &item,LPARAM lParam);

class CNo5TreeItem:public CTreeItem
{
public:
	CNo5TreeItem(HTREEITEM hItem = NULL,CTreeViewCtrlEx *pTree = NULL):\
		CTreeItem(hItem,pTree)
	{
		//
	}
	CNo5TreeItem(const CTreeItem &item):CTreeItem(item)
	{
		//
	}
	CNo5TreeItem(const CNo5TreeItem &item):CTreeItem(item)
	{
		//
	}
	CNo5TreeItem & operator =(const CNo5TreeItem & itemSrc)
	{
		m_hTreeItem = itemSrc.m_hTreeItem;
		m_pTreeView = itemSrc.m_pTreeView;
		return *this;
	}
	CNo5TreeItem & operator =(const CTreeItem & itemSrc)
	{
		m_hTreeItem = itemSrc.m_hTreeItem;
		m_pTreeView = itemSrc.m_pTreeView;
		return *this;
	}
	int EnumSiblings(PENUMTREEITEMS pfn,LPARAM lParam,BOOL bDown,
		BOOL bMe)
	{
		BOOL stop = FALSE;
		int count = 0;
		CNo5TreeItem item;

		if(bMe && !IsNull()){
			stop = (*pfn)(*this,lParam);
			count++;
		}
		if(!stop){
			if(bDown)
				item = GetNextSibling();
			else
				item = GetPrevSibling();
			while((!stop) && (!item.IsNull())){
				stop = (*pfn)(item,lParam);
				count++;
				if(bDown)
					item = item.GetNextSibling();
				else
					item = item.GetPrevSibling();
			}
		}
		return count;
	}

	int Enum(PENUMTREEITEMS pfn,LPARAM lParam,BOOL bMe,BOOL bDeep,BOOL &bStop)
	{
		CNo5TreeItem item;
		CNo5TreeItem child;
		BOOL stop = FALSE;
		int count = 0;

		if(bMe && !IsNull()){
			stop = (*pfn)(*this,lParam);
			count++;
			if((!stop) && bDeep){
				child = GetChild();
				if(!child.IsNull()){
					count += child.Enum(pfn,lParam,TRUE,bDeep,stop);
				}
			}
		}
		item = GetNextSibling();
		while((!stop) && (!item.IsNull())){
			stop = (*pfn)(item,lParam);
			count++;
			if((!stop) && bDeep){
				child = item.GetChild();
				if(!child.IsNull()){
					count += child.Enum(pfn,lParam,TRUE,bDeep,stop);
				}
			}
			item = item.GetNextSibling();
		}
		return count;
	}

	CNo5TreeItem FindItem(LPCTSTR label,BOOL bCaseSensitive,BOOL bDeep = TRUE)
	{
		CFindItemParam param;
		BOOL dummy;

		param.label = label;
		param.flags = SF_LABEL;
		if(bCaseSensitive)
			param.flags |= SF_CASE;
		Enum(FindItemCallback,(LPARAM)&param,TRUE,bDeep,dummy);
		return CNo5TreeItem(param.item,GetTreeView());
	}
	CNo5TreeItem FindItem(LPCTSTR label,LPARAM lParam,BOOL bCaseSensitive,BOOL bDeep = TRUE)
	{
		CFindItemParam param;
		BOOL dummy;

		param.label = label;
		param.lParam = lParam;
		param.flags = SF_LABEL|SF_PARAM;
		if(bCaseSensitive)
			param.flags |= SF_CASE;
		Enum(FindItemCallback,(LPARAM)&param,TRUE,bDeep,dummy);
		return CNo5TreeItem(param.item,GetTreeView());
	}
	CNo5TreeItem FindItem(LPARAM lParam,BOOL bDeep = TRUE)
	{
		CFindItemParam param;
		BOOL dummy;

		param.lParam = lParam;
		param.flags = SF_PARAM;
		Enum(FindItemCallback,(LPARAM)&param,TRUE,bDeep,dummy);
		return CNo5TreeItem(param.item,GetTreeView());
	}
	CNo5TreeItem FindSibling(LPCTSTR label,BOOL bCaseSensitive,BOOL bDown,BOOL bMe)
	{
		CFindItemParam param;

		param.label = label;
		param.flags = SF_LABEL;
		if(bCaseSensitive)
			param.flags |= SF_CASE;
		EnumSiblings(FindItemCallback,(LPARAM)&param,bDown,bMe);
		return CNo5TreeItem(param.item,GetTreeView());
	}
	CNo5TreeItem FindSibling(LPCTSTR label,LPARAM lParam,BOOL bCaseSensitive,BOOL bDown,BOOL bMe)
	{
		CFindItemParam param;

		param.label = label;
		param.lParam = lParam;
		param.flags = SF_LABEL|SF_PARAM;
		if(bCaseSensitive)
			param.flags |= SF_CASE;
		EnumSiblings(FindItemCallback,(LPARAM)&param,bDown,bMe);
		return CNo5TreeItem(param.item,GetTreeView());
	}
	CNo5TreeItem FindSibling(LPARAM lParam,BOOL bDown,BOOL bMe)
	{
		CFindItemParam param;

		param.lParam = lParam;
		param.flags = SF_PARAM;
		EnumSiblings(FindItemCallback,(LPARAM)&param,bDown,bMe);
		return CNo5TreeItem(param.item,GetTreeView());
	}
private:
	enum SearchFlags
	{
		SF_LABEL = 1,
		SF_PARAM = 2,
		SF_CASE = 4,
	};
	struct CFindItemParam
	{
		LPCTSTR label;
		LPARAM lParam;
		HTREEITEM item;
		UINT flags;
		CFindItemParam():label(NULL),item(NULL),lParam(0),flags(SF_LABEL)
		{
			//
		}
			
	};
	friend CFindItemParam;

	static BOOL FindItemCallback(CNo5TreeItem &item,LPARAM lParam)
	{
		CFindItemParam *p = (CFindItemParam *)lParam;
		bool label_match = false;
		bool param_match = false;
		bool match = false;
		
		if(p->flags & SF_LABEL){
			CString label;
			// get item label
			item.GetText(label);
			// check if the label is equal
			if(p->flags & SF_CASE){
				label_match = (label == p->label);
			}
			else{
				label_match = (label.CompareNoCase(p->label) == 0);
			}
		}
		if(p->flags & SF_PARAM){
			// check if the param is equal
			param_match = (p->lParam == (LPARAM)item.GetData());
		}
		// check if we have a full match
		if( ( p->flags & ( SF_LABEL | SF_PARAM) ) == (SF_LABEL | SF_PARAM))
			match = label_match && param_match;
		else if(p->flags & (SF_LABEL))
			match = label_match;
		else if(p->flags & SF_PARAM)
			match = param_match;

		if(match){
			p->item = (HTREEITEM)item;
		}
		return match ? TRUE:FALSE;
	}
};

template <class TBase>
class CNo5TreeCtrlT:public CTreeViewCtrlExT<TBase>
{
	typedef CTreeViewCtrlExT<TBase> _BaseClass;
public:
// Constructors
	CNo5TreeCtrlT(HWND hWnd = NULL) : CTreeViewCtrlExT< TBase >(hWnd) { }

	CNo5TreeCtrlT< TBase >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}
	CNo5TreeItem FindItem(LPCTSTR label,BOOL bCaseSensitive,BOOL bDeep)
	{
		CNo5TreeItem root = GetRootItem();
		return root.FindItem(label,bCaseSensitive,bDeep);
	}
	CNo5TreeItem FindItem(LPCTSTR label,LPARAM lParam,BOOL bCaseSensitive,BOOL bDeep)
	{
		CNo5TreeItem root = GetRootItem();
		return root.FindItem(label,lParam,bCaseSensitive,bDeep);
	}
	CNo5TreeItem FindItem(LPARAM lParam,BOOL bDeep)
	{
		CNo5TreeItem root = GetRootItem();
		return root.FindItem(lParam,bDeep);
	}
	BOOL DeleteItem(HTREEITEM hItem)
	{
		return _BaseClass::DeleteItem(hItem);
	}
	BOOL DeleteItem(LPCSTR label,BOOL bCaseSensitive,BOOL bDeep)
	{
		CNo5TreeItem item = FindItem(label,bCaseSensitive,bDeep);
		BOOL res = FALSE;

		if(item){
			res = DeleteItem(item);
		}
		return res;
	}
	BOOL DeleteItem(LPARAM lParam,BOOL bDeep)
	{
		CNo5TreeItem item = FindItem(lParam,bDeep);
		BOOL res = FALSE;

		if(item){
			res = DeleteItem(item);
		}
		return res;
	}
};

// cant use CNo5TreeCtrlT<CNo5Window>
typedef CNo5TreeCtrlT<CWindow> CNo5TreeCtrl;

/*******************************************************/
template <class TList,class TSink>
class CDragHandlerT :
	public CDragListNotifyImpl< CDragHandlerT<TList,TSink> >
{
	typedef CDragHandlerT<TList,TSink> _ThisClass;
	typedef CDragListNotifyImpl<_ThisClass> _DragListBoxHandler;
	TList &m_list;
	TSink &m_sink;
public:
	CDragHandlerT(TList &ListBox,TSink &sink):m_list(ListBox),
		m_sink(sink)
	{
		m_StartingDragIndex = -1;
	}
	BEGIN_MSG_MAP(_ThisClass)
		CHAIN_MSG_MAP(_DragListBoxHandler)
	END_MSG_MAP()

	// Overrideables
	BOOL OnBeginDrag(int /*nCtlID*/, HWND /*hWndDragList*/,
		POINT ptCursor)
	{
		m_list.DrawInsert(-1);
		m_StartingDragIndex = m_list.LBItemFromPt(ptCursor);
		return TRUE;	// allow dragging
	}
	void OnCancelDrag(int /*nCtlID*/, HWND /*hWndDragList*/, POINT /*ptCursor*/)
	{
		// nothing to do
		m_list.DrawInsert(-1);
		m_StartingDragIndex = -1;
	}
	int OnDragging(int /*nCtlID*/, HWND /*hWndDragList*/,
		POINT ptCursor)
	{
		m_list.DrawInsert(m_list.LBItemFromPt(ptCursor));
		return 0;
	}
	void OnDropped(int /*nCtlID*/, HWND /*hWndDragList*/,
		POINT ptCursor)
	{
		int index = m_list.LBItemFromPt(ptCursor);

		if(m_StartingDragIndex >= 0 && index >= 0){
			m_list.MoveItem(index,m_StartingDragIndex);
			m_sink.OnItemsMoved(index,m_StartingDragIndex);
		}
		m_list.DrawInsert(-1);
	}
private:
	int m_StartingDragIndex;
};




} // NO5TL

#endif // !defined(AFX_NO5CONTROLS_H__B959AC0B_354B_11D7_A179_FE6E78F0B835__INCLUDED_)
