// BuddyListView.cpp: implementation of the CBuddyListView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "BuddyListView.h"
#include <no5tl\no5controls.h>
#include <yahoo\yahoochat.h>
#include "user.h"
using NO5TL::CNo5TreeItem;

CString GetAwayString(UINT i);

struct BuddyItem
{
	enum ItemType
	{
		BI_GROUP,
		BI_NAME,
		BI_NONE,
		
	};
	BuddyItem(const BuddyItem &bi)
	{
		CopyFrom(bi);
	}
	BuddyItem(CBuddy *p)
	{
		m_type = BI_NAME;
		m_p = p;
	}
	BuddyItem(CBuddyGroup *q)
	{
		m_type = BI_GROUP;
		m_q = q;
	}
	BuddyItem(void)
	{
		m_type = BI_NONE;
		m_p = NULL;
	}
	BuddyItem & operator = ( const BuddyItem &rhs )
	{
		CopyFrom(rhs);
		return *this;
	}
	bool operator == (const BuddyItem &rhs)
	{
		bool res = false;
		switch(m_type){
			case BI_GROUP:
				res = m_q->m_name.CompareNoCase(rhs.m_q->m_name) == 0;
				break;
			case BI_NAME:
				res = m_p->m_name.CompareNoCase(rhs.m_p->m_name) == 0;
				break;
			default:
				break;
		}
		return res;
	}
	ItemType m_type;
	union
	{
		CBuddy *m_p;
		CBuddyGroup *m_q;
	};
private:
	void CopyFrom(const BuddyItem &bi)
	{
		if(this != &bi){
			m_type = bi.m_type;
			switch(m_type){
				case BI_GROUP:
					m_q = bi.m_q;
					break;
				case BI_NAME:
					m_p = bi.m_p;
					break;
				default:
					ATLASSERT(0);
					break;
			}
		}
	}
};

struct CFindBuddyItem
{
	BuddyItem bi;
	HTREEITEM hItem;

	CFindBuddyItem(void)
	{
		hItem = NULL;
	}
};


CBuddyListView::CBuddyListView()
{
	m_pSink = NULL;
	m_p = NULL;
}

CBuddyListView::~CBuddyListView()
{
	
}

LRESULT CBuddyListView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CImageList il;

	m_tv.Attach(GetDlgItem(IDC_TREE1));
	m_cb.Attach(GetDlgItem(IDC_COMBO1));
	il.Create(16,16,ILC_COLORDDB|ILC_MASK,2,0);

	if(!il.IsNull()){
		UINT icons[] = {IDR_MAINFRAME,IDI_BUD_OFF};
		int i;
		HICON hIcon;

		for(i=0;i<sizeof(icons)/sizeof(icons[0]);i++){
			hIcon = (HICON)LoadImage(_Module.GetResourceInstance(),MAKEINTRESOURCE(icons[i]),
				IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
			if(hIcon){
				il.AddIcon(hIcon);
				// image list take owner ship so we can destroy
				DestroyIcon(hIcon);
			}
		}
		m_tv.SetImageList(il,TVSIL_NORMAL);
		il.Detach();
	}
	DlgResize_Init(false,false,WS_CLIPCHILDREN);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	
	return 0;
}

LRESULT CBuddyListView::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	return 0;
}

LRESULT CBuddyListView::OnGetInfotip(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	LPNMTVGETINFOTIP p = (LPNMTVGETINFOTIP)pnmh;
	BuddyItem *pbi = (BuddyItem *)(p->lParam);

	if(pbi->m_type == BuddyItem::BI_NAME){
		CBuddy *pb = pbi->m_p;
		CString s = GetAwayString(pb->m_status);

		if(!pbi->m_p->m_msg.IsEmpty()){
			s += " ( ";
			s += pb->m_msg;
			s += " ) ";
		}
		lstrcpyn(p->pszText,(LPCTSTR)s,p->cchTextMax );
	}
	else
		bHandled = FALSE;
	return 0;
}

LRESULT CBuddyListView::OnDoubleClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	CNo5TreeItem item(ItemUnderCursor(),&m_tv);

	if(item){
		BuddyItem *p = (BuddyItem *)item.GetData();
		if(m_pSink && p->m_type == BuddyItem::BI_NAME)
			m_pSink->blv_OnPm(p->m_p->m_name);
	}
	return 0;
}

LRESULT CBuddyListView::OnRightClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	CNo5TreeItem item(ItemUnderCursor(),&m_tv);
	BuddyItem *pBuddyItem = (BuddyItem *)item.GetData();

	if(m_pSink && item && pBuddyItem){
		int sub = 0;
		CBuddy *pBuddy = NULL;
		CBuddyGroup *pGroup = NULL;

		if(pBuddyItem->m_type == BuddyItem::BI_NAME){
			CNo5TreeItem parent = item.GetParent();

			sub = 0;
			pBuddy = pBuddyItem->m_p;
			if(parent){
				BuddyItem *q = (BuddyItem *)parent.GetData();
				pGroup = q->m_q;
			}
		}
		else if(pBuddyItem->m_type == BuddyItem::BI_GROUP){
			pGroup = pBuddyItem->m_q;
			sub = 1;
		}
		else {
			ATLASSERT(0);
		}
		if(sub >= 0){
			CMenu menu;

			if(menu.LoadMenu(IDR_USER)){
				if(menu = menu.GetSubMenu(sub)){
					CPoint pt;

					if(sub == 0){
						// grey some items that doesnt make sense for buddy list
						menu.EnableMenuItem(ID_USER_IGNORE,MF_GRAYED);
						menu.EnableMenuItem(ID_USER_BUDDY_ADD,MF_GRAYED);
						menu.EnableMenuItem(ID_USER_MIMIC,MF_GRAYED);
						menu.EnableMenuItem(ID_USER_FOLLOW,MF_GRAYED);
					}

					GetCursorPos(&pt);
					UINT cmd = TrackPopupMenu(menu,TPM_RIGHTALIGN|TPM_BOTTOMALIGN|TPM_RETURNCMD|\
						TPM_LEFTBUTTON,pt.x,pt.y,0,m_hWnd,NULL);

					switch(cmd){
						case ID_USER_PROFILE:
							m_pSink->blv_OnProfile(pBuddy->m_name);
							break;
						case ID_USER_PM:
							m_pSink->blv_OnPm(pBuddy->m_name);
							break;
						case ID_USER_BUDDY_REMOVE:
							m_pSink->blv_OnRemoveBuddy(pBuddy->m_name,pGroup->m_name);
							break;
						case ID_USER_CAM:
							m_pSink->blv_OnViewCam(pBuddy->m_name);
							break;
						case ID_USER_CAMINVITE:
							m_pSink->blv_OnCamInvite(pBuddy->m_name);
							break;
						case ID_USER_GOTO:
							m_pSink->blv_OnGotoUser(pBuddy->m_name);
							break;
						case ID_USER_INVITE:
							m_pSink->blv_OnInviteUser(pBuddy->m_name);
							break;
						case ID_GROUP_ADD:
							m_pSink->blv_OnAddBuddy(pGroup->m_name);
							break;
						case ID_GROUP_RENAME:
							m_pSink->blv_OnRenameGroup(pGroup->m_name);
							break;
						default:
							break;
					}
				}
			}
		}
	}
	return 0;
}

LRESULT CBuddyListView::OnDeleteItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	LPNMTREEVIEW p = (LPNMTREEVIEW)pnmh;
	BuddyItem *pbi = (BuddyItem *)(p->itemOld.lParam);

	delete pbi;
	return 0;
}

LRESULT CBuddyListView::OnComboSelChange(WORD,WORD,HWND,BOOL &)
{
	if(m_pSink){
		CString s;
		int idx = m_cb.GetCurSel();

		if(idx >= 0){
			m_cb.GetLBText(idx,s);
			m_pSink->blv_OnChangeView(s);
		}
	}
	return 0;
}


// IBuddyListView
void CBuddyListView::SetAcc(IBuddyListEvents *pSink,LPCTSTR acc,CBuddyList *p)
{
	m_pSink = pSink;
	m_p = p;

	if(pSink && m_p && acc && lstrlen(acc)){
		int idx = m_cb.FindString(-1,acc);

		if(idx < 0){
			idx = m_cb.AddString(acc);
		}
		m_cb.SetCurSel(idx);
		UpdateTree();
	}
}

void CBuddyListView::RemoveAcc(LPCTSTR acc)
{
	int idx = m_cb.FindString(-1,acc);

	if(idx >= 0){
		if(idx == m_cb.GetCurSel())
			m_tv.DeleteAllItems();
		m_cb.DeleteString(idx);
	}
}

void CBuddyListView::OnAddBuddy(LPCTSTR group,CBuddy *pBuddy)
{
	CNo5TreeItem groupItem = FindGroupItem(group);

	if(groupItem.IsNull()){
		groupItem = AddGroup(group);
	}
	if(!groupItem.IsNull()){
		CNo5TreeItem item = FindBuddyItem(groupItem,pBuddy->m_name);

		if(item.IsNull()){
			int icon;
			TVINSERTSTRUCT is = {0};
			BuddyItem *p = new BuddyItem(pBuddy);

			if(pBuddy->m_status != NO5YAHOO::YAHOO_STATUS_AVAILABLE)
				icon = 1;
			else
				icon = 0;
			is.hParent = groupItem;
			is.hInsertAfter = TVI_SORT;
			is.item.mask = TVIF_TEXT|TVIF_IMAGE |TVIF_SELECTEDIMAGE|TVIF_PARAM;
			is.item.iImage = icon;
			is.item.iSelectedImage = icon;
			is.item.pszText = const_cast<LPTSTR>(LPCTSTR(pBuddy->m_name));
			is.item.lParam = (LPARAM)p;
			item = m_tv.InsertItem(&is);

			if(item){
				item = item.GetParent();
				item.Expand();
			}
		}
	}
}

void CBuddyListView::OnBuddyGroup(CBuddyGroup *pg)
{
	CNo5TreeItem groupItem = FindGroupItem(pg->m_name);

	if(groupItem.IsNull()){
		groupItem = AddGroup(pg->m_name);
	}
	if(!groupItem.IsNull()){
		const int count = pg->m_buddies.GetSize();
		for(int i=0;i<count;i++){
			OnAddBuddy(pg->m_name,pg->m_buddies[i]);
		}
	}
}

void CBuddyListView::OnRemBuddy(LPCTSTR group,LPCTSTR name)
{
	CNo5TreeItem item = FindGroupItem(group);

	if(!item.IsNull()){
		item = FindBuddyItem(item,name);
		if(!item.IsNull())
			m_tv.DeleteItem(item);
	}
}


CNo5TreeItem CBuddyListView::AddGroup(LPCTSTR group)
{
	CNo5TreeItem item = FindGroupItem(group);
	CBuddyGroup *pg = m_p->FindGroup(group);

	if((!item) && pg){
		TVINSERTSTRUCT is = {0};
		BuddyItem *p = new BuddyItem(pg);
		CString label = (LPCTSTR)GroupLabel(group);
		is.hParent = TVI_ROOT;
		is.hInsertAfter = TVI_SORT;
		is.item.mask = TVIF_TEXT|TVIF_IMAGE |TVIF_SELECTEDIMAGE|TVIF_PARAM;
		is.item.iImage = -2 ;
		is.item.iSelectedImage = -2;
		is.item.pszText = const_cast<LPTSTR>((LPCTSTR)label);
		is.item.lParam = (LPARAM)p;

		item = m_tv.InsertItem(&is);
		if(item){
			item.Expand();
		}
	}
	return item;
}

void CBuddyListView::OnUserAway(LPCTSTR user)
{
	CNo5TreeItem itemg,itemb;
	CBuddyGroup *pg = NULL;
	const int count = m_p->GetSize();
	int i;
	CBuddy *pb;

	for(i=0;i<count;i++){
		pg = m_p->operator[](i);
		pb = pg->FindBuddy(user);
		if(pb){
			itemg = FindGroupItem(pg->m_name);
			if(itemg){
				itemb = FindBuddyItem(itemg,pb->m_name);
				if(itemb){
					int icon;

					if(pb->m_status == NO5YAHOO::YAHOO_STATUS_OFFLINE)
						icon = 1;
					else
						icon = 0;

					itemb.SetImage(icon,icon);
				}
			}
		}
	}
}


void CBuddyListView::OnRenameGroup(LPCTSTR old,LPCTSTR group)
{
	CNo5TreeItem item = FindGroupItem(old);

	if(item){
		TVITEM tvi = {0};
		CString label = (LPCTSTR)GroupLabel(group);
			
		tvi.hItem = item;
		tvi.mask = TVIF_TEXT;
		tvi.pszText = const_cast<LPTSTR>((LPCTSTR)label);
		m_tv.SetItem(&tvi);
	}
}
	

static BOOL FindItemCB(CNo5TreeItem &item,LPARAM lParam)
{
	CFindBuddyItem *p = (CFindBuddyItem *)lParam;
	BuddyItem *pbi = (BuddyItem *)item.GetData();

	BOOL res = (*pbi == p->bi) ? TRUE : FALSE;

	if(res){
		p->hItem = (HTREEITEM)item;
	}
	return res;
}

CNo5TreeItem CBuddyListView::FindItem(HTREEITEM hParent,BuddyItem &item)
{
	CNo5TreeItem parent(hParent,&m_tv);
	CFindBuddyItem fi;
	BOOL dummy;
	
	fi.bi = item;
	parent.Enum(FindItemCB,(LPARAM)&fi,TRUE,TRUE,dummy);
	return CNo5TreeItem(fi.hItem,&m_tv);
}


CNo5TreeItem CBuddyListView::FindGroupItem(LPCTSTR group)
{
	CBuddyGroup *pg = m_p->FindGroup(group);
	CNo5TreeItem res;
	if(pg){
		BuddyItem bi(pg);
		res = FindItem(m_tv.GetRootItem(),bi);
	}
	return res;
}

CNo5TreeItem CBuddyListView::FindBuddyItem(HTREEITEM hGroup,LPCTSTR buddy)
{
	CNo5TreeItem item(hGroup,&m_tv);
	CNo5TreeItem res(NULL,&m_tv);
	BuddyItem *pbi = (BuddyItem *)(item.GetData());
	ATLASSERT(pbi->m_type == BuddyItem::BI_GROUP);

	if(pbi->m_type == BuddyItem::BI_GROUP){
		CBuddyGroup *pbg = pbi->m_q;
		if(pbg){
			CBuddy *pb = m_p->FindBuddy(pbg->m_name,buddy);
			if(pb){
				BuddyItem bi(pb);
				res = FindItem(item,bi);
			}
		}
	}
	return res;
}


CString CBuddyListView::GroupLabel(LPCTSTR acc)
{
	CString label = "group: ";
	label += acc;
	return label;
}

HTREEITEM CBuddyListView::ItemUnderCursor(void)
{
	TV_HITTESTINFO h = {0};

	GetCursorPos(&h.pt);
	m_tv.ScreenToClient(&h.pt);
	return m_tv.HitTest(&h);
}

CBuddy * CBuddyListView::GetBuddy(LPCTSTR group,LPCTSTR name)
{
	CBuddy *p = NULL;

	if(m_p){
		p = m_p->FindBuddy(group,name);
	}
	return p;
}

CBuddyGroup * CBuddyListView::GetGroup(LPCTSTR group)
{
	CBuddyGroup *p = NULL;

	if(m_p)
		p = m_p->FindGroup(group);
	return p;
}
void CBuddyListView::UpdateTree(void)
{
	m_tv.DeleteAllItems();

	if(m_p){
		for(int i=0;i<m_p->GetSize();i++){
			OnBuddyGroup(m_p->operator[](i));
		}
	}
}