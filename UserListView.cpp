// UserListView.cpp: implementation of the CUserListView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
// #include <no5tl\atldispa.h>
#include <no5tl\htmllistbox.h>
#include <no5tl\htmledit.h>
#include <no5tl\mystring.h>
#include <no5tl\winfile.h>
#include <yahoo\yahoocolor.h>
#include "UserListView.h"
#include "user.h"
#include "resource.h"
#include "no5app.h"

using namespace NO5TL;
using NO5YAHOO::CYahooColor;


// CUserListBox

int CUserListBox::AddUser(CUser *pUser)
{
	UserData *pData = new UserData;
	CString html;
		
	pData->m_pUser = pUser;
	pData->m_bExpanded = false;
	FormatText(false,pData,html);
		
	return AddItem(html,pData);
}

int CUserListBox::FindUser(CUser *pUser)
{
	const int count = GetCount();
	int index = -1;
	UserData *p = NULL;

	for(int i=0;i<count;i++){
		p = (UserData *)GetItemData(i);
		if(p->m_pUser == pUser){
			index = i;
			break;
		}
	}
	return index;
}

int CUserListBox::FindUser(LPCTSTR name)
{
	const int count = GetCount();
	int index = -1;
	UserData *p = NULL;

	for(int i=0;i<count;i++){
		p = (UserData *)GetItemData(i);
		const CString &s = p->m_pUser->m_name;
		if(!s.CompareNoCase(name)){
			index = i;
			break;
		}
	}
	return index;
}

void CUserListBox::UpdateUser(CUser *pUser)
{
	int index = FindUser(pUser);
	
	if(index >= 0){
		CString html;
		bool bSel = (index == GetCurSel());
		UserData *pData = (UserData *)GetItemData(index);
		FormatText(bSel,pData,html);
		
		BOOL res = SetItemText(index,html);
	}
}

LRESULT CUserListBox::OnSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMHTMLLISTBOX *p = (NMHTMLLISTBOX *)pnmh;

	// todo
	// changing the html text hre is preventing the click event to bubble, generating
	// double click events

	// item losing selection
	if(p->item1.index >= 0){
		UserData *pData = (UserData *)GetItemData(p->item1.index);
		if(pData){
			CString html;

			FormatText(false,pData,html);
			SetItemText(p->item1.index,html);
		}
	}
	// item being selected
	if(p->item2.index >= 0){
		UserData *pData = (UserData *)GetItemData(p->item2.index);
		if(pData){
			CString html;

			FormatText(true,pData,html);
			SetItemText(p->item2.index,html);
			
		}
	}
	return 0;
}

LRESULT CUserListBox::OnCompareItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMHTMLLISTBOX p = (LPNMHTMLLISTBOX)pnmh;
	int res = 0;
		
	if(p->item1.pData && p->item2.pData){
		UserData *pData1 = (UserData *)(p->item1.pData);
		UserData *pData2 = (UserData *)(p->item2.pData);
		CUser *pUser1 = pData1->m_pUser;
		CUser *pUser2 = pData2->m_pUser;
		const CString & name1 = pUser1->m_name;
		const CString & name2 = pUser2->m_name;

		res = name1.CompareNoCase(name2);
	}
	return res;
}

LRESULT CUserListBox::OnDeleteItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMHTMLLISTBOX p = (LPNMHTMLLISTBOX)pnmh;

	if(p->item1.index >= 0){
		UserData *pData = (UserData *)p->item1.pData;
		if(pData){
			delete pData;
			SetItemData(p->item1.index,NULL);
		}
	}
	return 0;
}

LRESULT CUserListBox::OnItemClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMHTMLLISTBOX p = (LPNMHTMLLISTBOX)pnmh;
	BOOL res = FALSE;


	if(p->item1.index >= 0){
		UserData *pData = (UserData *)p->item1.pData;
		if(pData){
			CHtmlElement el;
			HRESULT hr = E_POINTER;

			if(el.m_sp = p->item1.pElem){
				CString tag;
				CString id;

				hr = el.GetTagName(tag);
				if(hr == S_OK){
					if(!tag.CompareNoCase("img")){
						res = OnImageClicked(el,p->item1);
					}
					else if(!tag.CompareNoCase("input")){
						res = OnInputClicked(el,p->item1);
					}
					else {
						res = TRUE;
					}
				}
			}
		}
	}
	return (LRESULT)res;
}

LRESULT CUserListBox::OnItemDoubleClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMHTMLLISTBOX p = (LPNMHTMLLISTBOX)pnmh;
	BOOL res = FALSE;

	if(p->item1.index >= 0){
		UserData *pData = (UserData *)p->item1.pData;

		if(pData){
			m_pOutter->m_pSink->OnUserList_Pm(pData->m_pUser->m_name);
		}
	}
	return (LRESULT)res;
}

void CUserListBox::FormatText(bool bsel,UserData *p,CString &out)
{
	CString html;
	CString img;
	CUser *pUser = p->m_pUser;
	const CString path = g_app.GetPath();
	LPCTSTR pUserClass;		// uses different TD class based on gender

	if(bsel)
		pUserClass = _T("selected");
	else if(pUser->IsMale())
		pUserClass = _T("male");
	else if(pUser->IsFemale())
		pUserClass = _T("female");
	else
		pUserClass = _T("unk");

	if(pUser->m_cam)
		img = m_ImgCam;
	else
		img = m_ImgUser;

	if(!p->m_bExpanded){
		CString img_down;		// each client will have a different "down" image
		CInfoTag &it = pUser->GetInfoTag();
		CString client;			// name of the chat client
		
		
		client = it.GetVal(CString("ID"));
		if(!client.IsEmpty()){
			client.MakeUpper();
			img_down = m_client_images.Lookup(client);

		}
		if(img_down.IsEmpty())
			img_down = "down.bmp";

		img_down = path + img_down;

		html.Format("<table title=\"%s\"><tr><td><img src=\"%s\" name=\"plus\" /></td><td><img src=\"%s\" /></td><td class=\"%s\"><spam>%s</spam></td></tr></table>",
			(LPCTSTR)pUser->m_name,
			(LPCTSTR)img_down,
			(LPCTSTR)img,
			pUserClass,
			(LPCTSTR)pUser->m_name);
	}
	else{
		CString more;
		CString more2;
		LPCTSTR button_value = _T("ignore");
		
		FormatInfotagString(pUser,more);
		FormatYmsgInfo(pUser,more2);
		html.Format(\
		"<table title=\"%s\"><tr><td><img src=\"%s\" name=\"plus\" /></td>" \
		"<td><img src=\"%s\" /></td><td class=\"%s\">%s</td></tr>",
		(LPCTSTR)pUser->m_name,
		(LPCTSTR)m_ImgUp,
		(LPCTSTR)img,
		pUserClass,
		(LPCTSTR)pUser->m_name);
		html += "<tr><td></td><td></td><td><table align=right><tr><td><input type=button value=\"";
		html += button_value;
		html += "\" id=btIgnore /></td></tr></table></tr>";

		if(!more2.IsEmpty())
			html += more2;

		if(!more.IsEmpty()){
			html += "<tr><td></td><td></td><td>";
			html += more;
			html += "</td></tr>";
			html += "</table>";
		}
	}
	out = html;
}

// return TRUE to allow item selection
BOOL CUserListBox::OnImageClicked(CHtmlElement &el,HLBITEM &item)
{
	CString id;
	HRESULT hr = el.GetAttribute("name",id);
	BOOL res = TRUE;

	
	if(hr == S_OK && !id.CompareNoCase("plus")){
		CString html;
		UserData *pData = (UserData *)item.pData;
		pData->m_bExpanded = !pData->m_bExpanded;
		FormatText(item.bSelected,(UserData *)item.pData,html);
		SetItemText(item.index,html);
		res = FALSE;	// dont select it
	}
	return res;
}

BOOL CUserListBox::OnInputClicked(CHtmlElement &el,HLBITEM &item)
{
	CString id;
	HRESULT hr = el.GetAttribute("id",id);
	BOOL res = TRUE;

	
	if(hr == S_OK && !id.CompareNoCase("btIgnore")){
		UserData *pData = (UserData *)item.pData;
		m_pOutter->m_pSink->OnUserList_Ignored(pData->m_pUser->m_name);
		res = TRUE;	// dont select it
	}
	return FALSE; // dont select
}

void CUserListBox::FormatInfotagString(CUser *pUser,CString &out)
{
	NO5YAHOO::CInfoTag &it = pUser->GetInfoTag();
	int count = it.GetMap().GetSize();
	CString key,val;

	if(!it.IsEmpty()){
		out += "<table border=1  class=\"info\">";
		out += "<caption>info data</caption><tr><th>key</th><th>value</th></tr>";
		for(int i=0;i<count;i++){
			key = it.GetMap().GetKeyAt(i);
			val = it.GetMap().GetValueAt(i);
			out += "<tr><td>";
			out += key;
			out += "</td><td>";
			out += val;
			out += "</td></tr>";
		}
		out += "</table>";
	}
}

void CUserListBox::FormatYmsgInfo(CUser *pUser,CString &more)
{
	CString location;
	CString gender;

	if(!pUser->m_nick.IsEmpty()){
		CString s;
		s.Format("<tr><td></td><td>nick:</td><td>%s</td></tr>",
			(LPCTSTR)(pUser->m_nick));
		more += s;
	}
	if(!pUser->m_location.IsEmpty()){
		CString s;
		s.Format("<tr><td></td><td>location:</td><td>%s</td></tr>",
			(LPCTSTR)pUser->m_location);
		more += s;
	}
	if(pUser->m_age > 0){
		CString s;
		s.Format("<tr><td></td><td>age:</td><td>%d</td></tr>",
			(LPCTSTR)(pUser->m_age));
		more += s;
	}
}

STDMETHODIMP CUserListBox::ShowContextMenu(\
            /* [in] */ DWORD dwID,
            /* [in] */ DWORD x,
            /* [in] */ DWORD y,
            /* [in] */ IUnknown *pcmdtReserved,
            /* [in] */ IDispatch  *pdispReserved,
            /* [retval][out] */ HRESULT  *dwRetVal)
{
	CHtmlElement el;
	
	if(el.m_sp = pdispReserved){
		CHtmlElement el2;
		HRESULT hr = GetDivItemFromChildElement(el.m_sp,&el2.m_sp);

		if(hr == S_OK && el2.m_sp){
			ULONG cookie = GetDivCookie(el2.m_sp);
			
			if(cookie >= 0){
				int index = FindItemByCookie(cookie);
				if(index >= 0){
					UserData *p = (UserData *)GetItemData(index);
					POINT pt = { x, y };
					m_pOutter->OnContextMenu(p->m_pUser,pt);
				}
			}
		}
		*dwRetVal = S_OK;
	}
	return S_OK;
}

LRESULT CUserListBox::OnCreate(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	LRESULT res = _BaseClass::OnCreate(uMsg,wParam,lParam,bHandled);

	IDocHostUIHandlerDispatch *p = NULL;
	HRESULT hr = QueryInterface(IID_IDocHostUIHandlerDispatch,(LPVOID *)&p);

	if(SUCCEEDED(hr)){
		hr = SetExternalUIHandler(p);
	}
	ATLASSERT(SUCCEEDED(hr));
	/*
	CComPtr<IAxWinAmbientDispatch> spAmb;
	hr = QueryHost(&spAmb);
	if(SUCCEEDED(hr)){
		// set whatever
	}
	*/

	BOOL dummy = LoadClientImageNames();
	ATLASSERT(dummy);
	return 0;
}

LRESULT CUserListBox::OnDestroy(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	ATLTRACE("CUserListBox::OnDestroy\n");
	bHandled = FALSE;
	return 0;
}

BOOL CUserListBox::LoadClientImageNames(void)
{
	CWinFile wf;
	CString path = g_app.GetPath();
	BOOL res;

	path += "client_image.txt";

	res = wf.Create(path,OPEN_EXISTING,GENERIC_READ);

	if(res){
		CDataBuffer buf;
		res = wf.ReadAll(buf);
		if(res){
			CStringToken st;
			CString key,val;

			buf.AddNull();
			st.Init(buf.GetData(),_T(" \t\r\n"));
			
			while(res){
				key = st.GetNext();
				if(key.IsEmpty())
					break;
				val = st.GetNext();
				if(val.IsEmpty())
					break;
				res = m_client_images.Add(key,val);
			}
		}
	}
	return res;
}
	

// CuserListView

CUserListView::CUserListView():m_lb(this)
{
	m_pSink = NULL;
	m_lb.m_ImgUser = g_app.GetSmileysFolder() + "1.gif";
	m_lb.m_ImgUserSel = g_app.GetSmileysFolder() + "2.gif";
	m_lb.m_ImgUp = g_app.GetPath() + "up.bmp";
	m_lb.m_ImgCam = g_app.GetSmileysFolder() + "5.gif";
}

CUserListView::~CUserListView()
{

}

LRESULT CUserListView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	NO5TL::CModuleFileString s;
	CString title;

	title.Format("res://%s/%d",(LPCTSTR)s,IDR_HTML1);

	m_lb.Create(m_hWnd,rcDefault,title,WS_CHILD|WS_VISIBLE|WS_VSCROLL);
	return 0;
}

LRESULT CUserListView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(m_lb.IsWindow())
		m_lb.MoveWindow(0,0,LOWORD(lParam),HIWORD(lParam));
	return 0;
}

void CUserListView::AddUser(CUser *p)
{
	int index = m_lb.FindUser(p->m_name);

	if(index < 0){
		index = m_lb.AddUser(p);
		ATLASSERT(index >= 0);
	}
	else{
		ATLTRACE(_T("CUserListView::AddUser - user %s already in list!\n"),(LPCTSTR)p->m_name);
		UpdateUser(p);
	}
}

void CUserListView::RemoveUser(LPCTSTR user)
{
	int index = m_lb.FindUser(user);

	if(index >= 0){
		m_lb.DeleteItem(index);
	}
	else{
		ATLTRACE("CUserListView::Remove - %s not found!\n",user);
	}
}


void CUserListView::RemoveAll(void)
{
	m_lb.ResetContent();
}

void CUserListView::AddUsers(CUserList &users)
{
	SendMessage(WM_SETREDRAW,(WPARAM)FALSE);
	m_lb.SendMessage(WM_SETREDRAW,(WPARAM)FALSE);
	for(int i=0;i<users.GetSize();i++){
		CUser *pUser = users.GetUser(i);
		if(pUser->m_chat && !pUser->IsIgnored()){
			AddUser(pUser);
		}
	}
	SendMessage(WM_SETREDRAW,(WPARAM)TRUE);
	m_lb.SendMessage(WM_SETREDRAW,(WPARAM)TRUE);
	InvalidateRect(NULL);
}

void CUserListView::SetSink(IUserListViewEvents *pSink)
{
	m_pSink = pSink;
}


void CUserListView::UpdateUser(CUser *pUser)
{
	int index = m_lb.FindUser(pUser);

	if(index >= 0){
		m_lb.UpdateUser(pUser);
	}
	else{
		ATLTRACE("CUserListView::UpdateUser - %s not found!\n",(LPCTSTR)pUser->m_name);
	}
}

void CUserListView::OnContextMenu(CUser *pUser,POINT &pt)
{
	CMenu menu;

	if(pUser && menu.LoadMenu(IDR_USER)){
		CMenu menu2 = menu.GetSubMenu(0);

		if(menu2.IsMenu()){
			CString name;
			name = pUser->m_name;
			//MENUITEMINFO mi = { sizeof(MENUITEMINFO) };

			ATLASSERT(m_pSink);

			// add a menu item with the name of the user clicked
			/*
			mi.fMask = MIIM_ID | MIIM_STATE | MIIM_TYPE;
			mi.fType = MFT_STRING;
			mi.fState = MFS_DISABLED;
			mi.wID = ID_USER_NAME;
			mi.dwItemData = const_cast<LPTSTR>((LPCTSTR)(pUser->m_name));
			mi.cch = (pUser->m_name).GetLength();
			*/

			menu2.InsertMenu(0,MF_BYPOSITION,ID_USER_NAME,pUser->m_name);

			// set check marks according to user atrributes
			if(pUser->m_ignored)
				menu2.CheckMenuItem(ID_USER_IGNORE,MF_CHECKED);
			if(pUser->m_mimic)
				menu2.CheckMenuItem(ID_USER_MIMIC,MF_CHECKED);
			if(pUser->m_follow)
				menu2.CheckMenuItem(ID_USER_FOLLOW,MF_CHECKED);
			//if(!pUser->m_cam)
			//	menu2.EnableMenuItem(ID_USER_CAM,MF_GRAYED);

			UINT cmd = menu2.TrackPopupMenu(TPM_RIGHTALIGN|TPM_BOTTOMALIGN|TPM_NONOTIFY|\
					TPM_RETURNCMD|TPM_LEFTBUTTON,pt.x,pt.y,m_hWnd,NULL);

			switch(cmd)
			{
			case ID_USER_IGNORE:
				m_pSink->OnUserList_Ignored(name);
				break;
			case ID_USER_PROFILE:
				m_pSink->OnUserList_Profile(name);
				break;
			case ID_USER_PM:
				m_pSink->OnUserList_Pm(name);
				break;
			case ID_USER_MIMIC:
				m_pSink->OnUserList_Mimic(name);
				break;
			case ID_USER_FOLLOW:
				m_pSink->OnUserList_Follow(name);
				break;
			case ID_USER_CAM:
				m_pSink->OnUserList_Cam(name);
				break;
			case ID_USER_CAMINVITE:
				m_pSink->OnUserList_CamInvite(name);
				break;
			default:
				break;
			}
			menu2.DestroyMenu();
		}
		menu.DestroyMenu();
	}
}

LRESULT CUserListView::OnDestroy(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	ATLTRACE("CUserListView::OnDestroy\n");
	bHandled = FALSE;
	return 0;
}


