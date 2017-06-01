// ShellTree.h: interface for the CShellTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHELLTREE_H__F24F1241_1AA3_11D9_A17B_B94D3680EF49__INCLUDED_)
#define AFX_SHELLTREE_H__F24F1241_1AA3_11D9_A17B_B94D3680EF49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mystring.h"
#include "shellfolder.h"
#include "utils.h"

#ifndef __ATLCTRLX_H__
#error shelltree.h requires atlctrlx.h to be included first
#endif

namespace NO5TL
{

static BOOL DoContextMenu(HWND hWnd, LPSHELLFOLDER lpsfParent, LPITEMIDLIST lpi, POINT point);

struct TVI_DATA
{
	CPidl m_pidl_rel;
	CPidl m_pidl_abs;
	CShellFolder m_FolderParent;
	bool m_isfolder:1;
	bool m_hassubfolder:1;
	bool m_isfilesys:1;
	int m_nIcon;
	int m_nOpenIcon;
	int m_nSelIcon;
	CString m_name_rel;
	CString m_name_abs;

	TVI_DATA()
	{
		m_isfolder = false;
		m_hassubfolder = false;
		m_isfilesys = false;
		m_nIcon = -1;
		m_nOpenIcon = -1;
		m_nSelIcon = -1;
	}

	TVI_DATA(LPITEMIDLIST pidl_rel,LPITEMIDLIST pidl_abs,IShellFolder *pParent)
	{
		m_pidl_rel.Attach(pidl_rel);
		m_pidl_abs.Attach(pidl_abs);
		m_FolderParent.m_folder = pParent;
	}

	HRESULT Load(void)
	{
		DWORD flags = SFGAO_HASSUBFOLDER|SFGAO_FILESYSTEM|\
			SFGAO_FOLDER;
		HRESULT hr;

		if(!m_FolderParent.m_folder){ // desktop item
			SHFILEINFO sfi = {0};

			m_isfolder = true;
			m_isfilesys = true;
			m_hassubfolder = true;
			if(::SHGetFileInfo((LPCTSTR)(LPITEMIDLIST)m_pidl_abs,
				0,&sfi,sizeof(sfi),SHGFI_DISPLAYNAME|SHGFI_PIDL))
				m_name_rel = sfi.szDisplayName;

		}
		else { // items that have a parent folder

			hr = m_FolderParent.GetAttributesOf(m_pidl_rel,flags);
			if(FAILED(hr))
				return hr;
			m_isfolder = (flags & SFGAO_FOLDER) ? true : false;
			m_hassubfolder = (flags & SFGAO_HASSUBFOLDER) ? true:false;
			m_isfilesys = (flags & SFGAO_FILESYSTEM)?true:false;
			hr = m_FolderParent.GetDisplayNameOf(m_pidl_rel,
				m_name_rel);
			ATLASSERT(SUCCEEDED(hr));
		}

		// these do not depend on the item having a parent or not
		m_pidl_abs.GetIconIndex(m_nIcon);
		m_pidl_abs.GetIconIndex(m_nOpenIcon,SHGFI_OPENICON|SHGFI_SMALLICON);
		m_pidl_abs.GetIconIndex(m_nSelIcon,SHGFI_SELECTED|SHGFI_SMALLICON); 
		hr = m_pidl_abs.GetPath(m_name_abs);
		ATLASSERT(SUCCEEDED(hr));

		return hr;
	}

	int GetIcon(void) { return m_nIcon;}
	int GetOpenIcon(void) { return m_nOpenIcon;}
	int GetSelIcon(void) { return m_nSelIcon;}

	static TVI_DATA * CreateMe(LPITEMIDLIST pidl_rel,LPITEMIDLIST pidl_abs,IShellFolder *pParent)
	{
		TVI_DATA *p = new TVI_DATA(pidl_rel,pidl_abs,pParent);

		ATLASSERT(p);
		if(p){
			HRESULT hr = p->Load();
			ATLASSERT(SUCCEEDED(hr));
		}
		return p;
	}

};

typedef CWinTraitsOR<TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS> CTreeTraits;

template <class T,class TBase = CWindow,class TWinTraits = CControlWinTraits>
class CShellTreeT : public CWindowImpl<T,CTreeViewCtrlExT<TBase>,TWinTraits>
{
	typedef CShellTreeT<T,TBase,TWinTraits> _ThisClass;
	typedef CWindowImpl<T,CTreeViewCtrlExT<TBase>,TWinTraits> _BaseClass;
	CPidl m_pidl_root;
	HIMAGELIST m_hSysImgLst;
public:
	bool m_show_files:1;
	bool m_showhidden:1;
	bool m_contextmenu:1;
public:
	DECLARE_WND_SUPERCLASS(__T("NO5TL_ShellTree"),_BaseClass::GetWndClassName())

	BEGIN_MSG_MAP(_ThisClass)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDING,OnItemExpanding)
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDED,OnItemExpanded)
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_DELETEITEM,OnItemDelete)
		REFLECTED_NOTIFY_CODE_HANDLER(NM_RCLICK,OnRClick)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		LRESULT res = DefWindowProc(uMsg,wParam,lParam);
		AddRoot();
		return res;
	}

	LRESULT OnItemExpanding(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMTREEVIEW p = (LPNMTREEVIEW)pnmh;
		CTreeItem item(p->itemNew.hItem,this);

		if(p->action & TVE_EXPAND){ // expanding
			CMyWaitCursor wc(m_hWnd);

			HRESULT hr = AddTreeItems(item);
			ATLASSERT(SUCCEEDED(hr));
			if(SUCCEEDED(hr)){
				HelperSortChildren(item);
				TVI_DATA *pData = (TVI_DATA *)p->itemNew.lParam;
				if(pData){
					item.SetImage(pData->GetOpenIcon(),
						pData->GetOpenIcon());
				}
			}
		}
		else if(p->action & TVE_COLLAPSE){ // collapsing
		}

		return (LRESULT)FALSE; // allow the list to expand or collapse
	}
	LRESULT OnItemExpanded(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMTREEVIEW p = (LPNMTREEVIEW)pnmh;
		CTreeItem item(p->itemNew.hItem,this);

		if(p->action & TVE_EXPAND){ // expanded
			
		}
		else if(p->action & TVE_COLLAPSE){ // collapsed
			CTreeItem child;
			CMyWaitCursor wc(m_hWnd);

			do{
				child = item.GetChild();
				if(!child.IsNull())
					child.Delete();
			}while(!child.IsNull());

			TVI_DATA *pData = (TVI_DATA *)p->itemNew.lParam;
			if(pData){
				item.SetImage(pData->GetIcon(),pData->GetIcon());
			}
		}
		return 0; // ignored
	}
	LRESULT OnItemDelete(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMTREEVIEW p = (LPNMTREEVIEW)pnmh;
		TVI_DATA *data = (TVI_DATA*)p->itemOld.lParam;
		if(data)
			delete data;
		return 0;
	}

	LRESULT OnRClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		POINT pt = {0};
		POINT ptClient = {0};

		if(!m_contextmenu){
			bHandled = FALSE;
			return (LRESULT)FALSE;
		}
		if(::GetCursorPos(&pt)){
			TVHITTESTINFO hi = {0};
			ptClient = pt;
			::ScreenToClient(m_hWnd,&ptClient);
			hi.pt = ptClient;
			if(HitTest(&hi) && hi.hItem){
				CTreeItem item(hi.hItem,this);
				TVI_DATA *p = (TVI_DATA *)item.GetData();
				CShellFolder parent(p->m_FolderParent.m_folder);

				if(!parent.m_folder)
					parent.GetDesktop();

				DoContextMenu(m_hWnd,parent.m_folder,
					p->m_pidl_rel,pt);
			}
		}
		bHandled = FALSE;
		return 0;
	}

	// overides
	BOOL SubclassWindow(HWND hWnd)
	{
		BOOL res = _BaseClass::SubclassWindow(hWnd);

		if(res){
			HRESULT hr = AddRoot();
			ATLASSERT(SUCCEEDED(hr));
			res = SUCCEEDED(hr);
		}
		return res;
	}

	// public methods

	HRESULT SetRoot(int nFolder = CSIDL_DESKTOP)
	{
		ATLASSERT(!IsWindow());
		HRESULT hr = m_pidl_root.GetSpecialFolder(nFolder);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT SetRoot(LPCTSTR full_path)
	{
		ATLASSERT(!IsWindow());
		HRESULT hr;
		CShellFolder desktop;
		ULONG eaten = 0,DWORD flags = 0;
		USES_CONVERSION;

		hr = desktop.m_folder->ParseDisplayName(\
			GetParent(),NULL,T2COLE(full_path),&eaten,
			&m_pidl_root,&flags);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	void ShowFiles(bool show)
	{
		ATLASSERT(!IsWindow());
		m_show_files = show;
	}
	void ShowHiddenItems(bool show)
	{
		ATLASSERT(!IsWindow());
		m_show_hidden = show;
	}
	void ShowContextMenu(bool show)
	{
		m_contextmenu = show;
	}

	CShellTreeT()
	{
		m_show_files = true;
		m_showhidden = false;
		m_contextmenu = true;
		m_hSysImgLst = NULL;
	}
	virtual ~CShellTreeT()
	{
		//
	}
private:
	HRESULT AddRoot(void)
	{
		CPidl pidl_abs;
		CPidl pidl_rel;
		HRESULT hr = S_OK;
		CShellFolder folder;
		CPidl parent;
		TVI_DATA *pData;

		if(m_pidl_root.IsNull()){
			hr = m_pidl_root.GetSpecialFolder(CSIDL_DESKTOP,
				GetParent());
			if(FAILED(hr))
				return hr;
			hr = pidl_abs.GetSpecialFolder(CSIDL_DESKTOP,
				GetParent());
			if(FAILED(hr))
				return hr;
		}

		if(pidl_abs.IsNull())
			pidl_abs.Attach(m_pidl_root.CopyAll());

		if(!m_hSysImgLst){
			m_hSysImgLst = GetSysImgList();
			SetImageList(m_hSysImgLst,TVSIL_NORMAL);
		}

		if(pidl_abs.Count() >= 2){
			parent.Attach(pidl_abs.CopyParent());
			pidl_rel.Attach(pidl_abs.CopyTail());
			ATLASSERT(!parent.IsNull() && !pidl_rel.IsNull());
			hr = folder.Bind(parent);
		}
		else{
			// desktop has no parent folder
			folder.m_folder.Release();
		}

		if(SUCCEEDED(hr)){
			pData = TVI_DATA::CreateMe(pidl_rel.Detach(),
				pidl_abs.Detach(),folder.m_folder);
			if(pData){
				CTreeItem item = HelperAddItem(NULL,pData);
				if(item.IsNull())
					hr = E_FAIL;
			}
			else{
				hr = E_OUTOFMEMORY;
			}
		}
		return hr;
	}

	CTreeItem HelperAddItem(HTREEITEM hParent,TVI_DATA *pData)
	{
		TVINSERTSTRUCT tvis = {0};
		TVITEM &tvi = tvis.item;

		tvis.hParent = hParent;
		tvis.hInsertAfter = TVI_LAST;
		tvi.mask = TVIF_CHILDREN|TVIF_IMAGE|TVIF_PARAM|\
			TVIF_TEXT|TVIF_SELECTEDIMAGE;
		tvi.pszText = (TCHAR *)(LPCTSTR)pData->m_name_rel;
		tvi.iImage = pData->GetIcon();
		tvi.iSelectedImage = pData->GetSelIcon();
		tvi.cChildren = pData->m_isfolder;
		tvi.lParam = (LPARAM)pData;

		return InsertItem(&tvis);
	}

	HRESULT AddTreeItems(HTREEITEM hParent)
	{
		CTreeItem parent(hParent,this);
		CPidl pidl_parent;
		CShellFolder folder;
		HRESULT hr;
		CPidl pidl_rel;
		ULONG fetched;
		CPidl pidl_abs;

		if(!hParent || hParent == TVI_ROOT)
			return AddRoot();
		pidl_parent.Attach(GetItemPidl(parent));
		if(!pidl_parent.IsEmpty()){
			// if its empty, the parent is the desktop, and it 
			// cant bind to itself
			hr = folder.Bind(pidl_parent);
		}
		else
			hr = S_OK;
		if(SUCCEEDED(hr)){
			CComPtr<IEnumIDList> pEnum;
			DWORD flags = SHCONTF_FOLDERS;
			if(m_show_files)
				flags |= SHCONTF_NONFOLDERS;
			if(m_showhidden)
				flags |= SHCONTF_INCLUDEHIDDEN;

			hr = folder.m_folder->EnumObjects(GetParent(),flags,&pEnum);

			if(SUCCEEDED(hr)){
				MSG msg = {0};

				while(true){

					while(::PeekMessage(&msg,m_hWnd,0,0,PM_REMOVE)){
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);
					}
					fetched = 0;
					hr = pEnum->Next(1,&pidl_rel,&fetched);
					if(hr != S_OK)
						break;
					ATLASSERT(fetched == 1);
					// if pidl_parent is the desktop item, then it
					// is an empty list ( size == 0 ) but it is ok
					// because CPidl::Attach handles it correctly
					// that is, pidl_abs becomes pidl_rel in this case
					pidl_abs.Attach(CPidl::Append(pidl_parent,pidl_rel));
					HelperAddItem(\
						hParent,
						TVI_DATA::CreateMe(\
						pidl_rel.Detach(),
						pidl_abs.Detach(),
						folder.m_folder));
				}
			}
		}
		pidl_parent.Detach();
		return hr;
	}
	
	void HelperSortChildren(CTreeItem &parent)
	{
		CShellFolder desktop;
		TVSORTCB t;
		t.hParent = HTREEITEM(parent);
		t.lParam = (LPARAM)(TVI_DATA *)parent.GetData();
		t.lpfnCompare = CompareItemsCallback;
		BOOL res = SortChildrenCB(&t);
		ATLASSERT(res);
	}
	static HIMAGELIST GetSysImgList(void)
	{
		CPidl pidl;
		int tmp;

		pidl.GetSpecialFolder(CSIDL_DESKTOP);
		return pidl.GetIconIndex(tmp);
	}
	static LPITEMIDLIST GetItemPidl(CTreeItem &item)
	{
		TVI_DATA *p = (TVI_DATA *)item.GetData();
		return LPITEMIDLIST(p->m_pidl_abs);
	}
	static int CALLBACK CompareItemsCallback(LPARAM lp1,LPARAM lp2,LPARAM lp)
	{
		TVI_DATA *p1 = (TVI_DATA *)lp1;
		TVI_DATA *p2 = (TVI_DATA *)lp2;
		TVI_DATA *p3 = (TVI_DATA *)lp;
		CShellFolder f;

		int res;
		return res = f.CompareIDs(p1->m_pidl_abs,p2->m_pidl_abs);
	}
};

class CShellTree : public CShellTreeT<CShellTree,CWindow,CTreeTraits>
{
	//
};

BOOL DoContextMenu(HWND hWnd, LPSHELLFOLDER lpsfParent, LPITEMIDLIST lpi, POINT point)
{

    CComPtr<IContextMenu> spContextMenu;
    HRESULT hr = lpsfParent->GetUIObjectOf(hWnd, 1, (const struct _ITEMIDLIST**)&lpi, IID_IContextMenu, 0, (LPVOID*)&spContextMenu);
	if(FAILED(hr))
		return FALSE;

	HMENU hMenu = ::CreatePopupMenu();
	if(hMenu == NULL)
		return FALSE;

	// Get the context menu for the item.
	hr = spContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_EXPLORE);
	if(FAILED(hr))
		return FALSE;

	int idCmd = ::TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, point.x, point.y, 0, hWnd, NULL);

	if (idCmd != 0)
	{
		USES_CONVERSION;

		// Execute the command that was selected.
		CMINVOKECOMMANDINFO cmi = { 0 };
		cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
		cmi.fMask = 0;
		cmi.hwnd = hWnd;
		cmi.lpVerb = T2CA(MAKEINTRESOURCE(idCmd - 1));
		cmi.lpParameters = NULL;
		cmi.lpDirectory = NULL;
		cmi.nShow = SW_SHOWNORMAL;
		cmi.dwHotKey = 0;
		cmi.hIcon = NULL;
		hr = spContextMenu->InvokeCommand(&cmi);
	}

	::DestroyMenu(hMenu);

	return TRUE;
}

} // NO5TL

#endif // !defined(AFX_SHELLTREE_H__F24F1241_1AA3_11D9_A17B_B94D3680EF49__INCLUDED_)
