// RoomDlg.cpp: implementation of the CRoomDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <no5tl\mystring.h>
#include "resource.h"
#include "RoomDlg.h"
#include "recentrooms.h"
#include "no5app.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRoomDlg::CRoomDlg(UINT uMsg):m_uMsg(uMsg)
{
	m_bLoadingCats = false;
	m_bLoadingRooms = false;
}

CRoomDlg::~CRoomDlg()
{

}


LRESULT CRoomDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	DlgResize_Init(true,false,WS_THICKFRAME | WS_CLIPCHILDREN);
	m_tv.Attach(GetDlgItem(IDC_TREE1));
	m_tv2.Attach(GetDlgItem(IDC_TREE2));
	m_cbServers.Attach(GetDlgItem(IDC_COMBO_SERVER));
	m_cbRooms.Attach(GetDlgItem(IDC_COMBO_ROOM));

	LoadRecentRooms();


	if(m_server.IsEmpty())
		m_server = _T("http://insider.msg.yahoo.com");
	HRESULT hr = LoadServers();
	ATLASSERT(SUCCEEDED(hr));

	// select m_server in the combo
	BOOL bDummy;
	const int count = m_cbServers.GetCount();
	int i;
	ServerData *p;

	for(i=0;i<count;i++){
		p = (ServerData *)m_cbServers.GetItemDataPtr(i);
		if(p->m_server.CompareNoCase(m_server) == 0)
			break;
	}
	m_cbServers.SetCurSel(i);
	OnServerSelChange(0,0,NULL,bDummy);
	
	return TRUE;
}

LRESULT CRoomDlg::OnDeleteItem(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DELETEITEMSTRUCT *p = (DELETEITEMSTRUCT *)lParam;

	if(p->CtlID == (UINT)m_cbServers.GetDlgCtrlID()){
		if(p->itemData){
			delete (ServerData *)(p->itemData);
		}
	}
	else
		bHandled = FALSE;
	return 0;
}

LRESULT CRoomDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	NO5TL::CWindowTextString room(m_cbRooms);

	m_room = room;

	if(!m_uMsg)
		EndDialog(wID);
	else {
		CWindow wnd = GetParent();

		if(!wnd)
			wnd = GetWindow(GW_OWNER);
		ATLASSERT(wnd.IsWindow());
		if(wnd.IsWindow()){
			wnd.SendMessage(m_uMsg,(WPARAM)wID);
			DestroyWindow();
		}
	}
	return 0;
}

LRESULT CRoomDlg::OnCategoryDelete(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW p = (LPNMTREEVIEW)(pnmh);

	if(p->itemOld.lParam != 0){
		CCategory *pCat = (CCategory *)p->itemOld.lParam;
		delete pCat;
	}
	return 0;
}

LRESULT CRoomDlg::OnRoomDelete(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW p = (LPNMTREEVIEW)(pnmh);

	if(p->itemOld.lParam != 0){
		CItemData *pData = (CItemData *)p->itemOld.lParam;
		delete pData;
	}
	return 0;
}

LRESULT CRoomDlg::OnCategorySelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW p = (LPNMTREEVIEW)(pnmh);

	if(p->itemNew.lParam != 0){
		CCategory *pCat = (CCategory *)p->itemNew.lParam;
		m_tv2.DeleteAllItems();
		m_tv2.SetRedraw(FALSE);
		LoadRooms(m_server,pCat->m_id);
		m_tv2.SetRedraw(TRUE);
		m_tv2.InvalidateRect(NULL);
	}
	return 0;
}

LRESULT CRoomDlg::OnRoomSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW p = (LPNMTREEVIEW)(pnmh);

	if(p->itemNew.lParam != 0){
		CItemData *pData = (CItemData *)p->itemNew.lParam;
		CString s;
		
		if(pData->IsLobby()){
			CLobby *pLobby = (CLobby *)pData;
			s.Format("%s:%d::%lu",(LPCTSTR)(pLobby->m_pRoom->m_name),pLobby->m_index,
				pLobby->m_pRoom->m_id);

		}
		else{
			CRoom *pRoom = (CRoom *)pData;
			s.Format("%s::%lu",(LPCTSTR)pRoom->m_name,pRoom->m_id);
		}
		m_cbRooms.SetWindowText(s);
	}
	return 0;
}

LRESULT CRoomDlg::OnServerSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int sel = m_cbServers.GetCurSel();

	if(sel >= 0){
		ServerData *p = (ServerData *)m_cbServers.GetItemDataPtr(sel);
		if(p){
			m_server = p->m_server;
			HRESULT hr = LoadCatsFromServer(m_server);
			ATLASSERT(SUCCEEDED(hr));
		}
	}
	return 0;
}

LRESULT CRoomDlg::OnRoomGetTip(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMTVGETINFOTIP p = (LPNMTVGETINFOTIP)(pnmh);

	if(p->lParam){
		CItemData *pData = (CItemData *)(p->lParam);

		if(!pData->IsLobby()){
			CRoom *pRoom = (CRoom *)pData;
			lstrcpyn(p->pszText,(LPCTSTR)(pRoom->m_topic),p->cchTextMax);
		}
	}
	return 0;
}

HRESULT CRoomDlg::LoadCatsFromFile(LPCTSTR file)
{
	CComPtr<IXMLDOMDocument> spDoc;
	HRESULT hr = spDoc.CoCreateInstance(CLSID_DOMDocument30);

	if(hr == S_OK){
		hr = spDoc->put_async(VARIANT_FALSE);
		if(hr == S_OK){
			CComVariant varFile(file);
			VARIANT_BOOL bRes = VARIANT_FALSE;

			hr = spDoc->load(varFile,&bRes);
			if(hr == S_OK && bRes == VARIANT_TRUE){
				hr = LoadCategories(spDoc);
			}
		}
	}
	return hr;
}

HRESULT CRoomDlg::LoadCatsFromServer(LPCTSTR server)
{
	CComBSTR bstrUrl = server;
	CComVariant varAsync = true;
	CComVariant varNull;
	HRESULT hr;

	if(m_spHttp){
		m_spHttp->abort();
		m_spHttp.Release();
		m_bLoadingCats =  false;
	}
	m_bLoadingCats = true;


	hr = m_spHttp.CoCreateInstance(CLSID_XMLHTTP30);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_spHttp->put_onreadystatechange(static_cast<IDispatch *>(this));
	ATLASSERT(SUCCEEDED(hr));

	if(server[lstrlen(server) - 1] != '/')
		bstrUrl += OLESTR("/");
	bstrUrl += OLESTR("ycontent/?chatcat=0");
	hr = m_spHttp->open(CComBSTR("GET"),bstrUrl,varAsync,varNull,varNull);
	if(hr == S_OK){
		hr = m_spHttp->send(varNull);
		if(hr == S_OK){
			
		}
		else
			ATLASSERT(0);
	}
	return hr;
}

HRESULT CRoomDlg::LoadCategories(IXMLDOMDocument *pDoc)
{
	CComBSTR bstrName;
	CComPtr<IXMLDOMElement> spElem;
	CComPtr<IXMLDOMNode> spNode;
	CComQIPtr<IXMLDOMElement> spElem2;
	HRESULT hr;

	hr = pDoc->get_documentElement(&spElem);
	if(hr == S_OK){
		hr = spElem->get_nodeName(&bstrName);
		if(hr == S_OK && bstrName == OLESTR("content")){
			USES_CONVERSION;
			hr = spElem->get_firstChild(&spNode);
			if(hr == S_OK){
				hr = spNode->get_nodeName(&bstrName);
				if(hr == S_OK && bstrName == OLESTR("chatCategories")){
					CComPtr<IXMLDOMNodeList> spNodeList;

					hr = spNode->get_childNodes(&spNodeList);
					if(hr == S_OK){
						hr = AddCategories(m_tv.GetRootItem(),spNodeList);
					}
				}
			}
		}
	}
	return hr;
}
	

HRESULT CRoomDlg::AddCategories(CTreeItem &parent,IXMLDOMNodeList *pNodeList)
{
	long count = 0;
	HRESULT hr = pNodeList->get_length(&count);
	CComPtr<IXMLDOMNode> spNode;
	CComQIPtr<IXMLDOMElement> spElem;
	CComBSTR bstrNodeName;
	CComVariant varAttribute;
	CComBSTR bstrAttrName;
	USES_CONVERSION;
	CCategory *pCat = NULL;
	CTreeItem item;
	CComPtr<IXMLDOMNodeList> spChildNodes;
									
	for(long i=0;i<count && hr == S_OK;i++){
		spNode.Release();
		hr = pNodeList->get_item(i,&spNode);
		if(hr == S_OK && ( spElem = spNode )){
			hr = spElem->get_nodeName(&bstrNodeName);
			if(bstrNodeName == OLESTR("category")){
				// get categorie name
				bstrAttrName = OLESTR("name");
				hr = spElem->getAttribute(bstrAttrName,&varAttribute);
				if(hr == S_OK && varAttribute.vt == VT_BSTR){
					pCat = new CCategory();
					pCat->m_name = OLE2CT(varAttribute.bstrVal);
					varAttribute.Clear();
					bstrAttrName = OLESTR("id");
					hr = spElem->getAttribute(bstrAttrName,&varAttribute);
					if(hr == S_OK && varAttribute.ChangeType(VT_UI4) == S_OK ){
						pCat->m_id = varAttribute.ulVal;
						item = m_tv.InsertItem(pCat->m_name,parent,TVI_SORT);
						ATLASSERT(!item.IsNull());
						item.SetData((DWORD_PTR)pCat);

						spChildNodes.Release();
						hr = spElem->get_childNodes(&spChildNodes);
						if(hr == S_OK){
							hr = AddCategories(item,spChildNodes);
							if(hr == S_FALSE)
								hr = S_OK;
						}
					}
					else{
						delete pCat; pCat = NULL;
					}
				}
			}
		}
	}
	return hr;
}

HRESULT CRoomDlg::LoadRooms(LPCTSTR server,ULONG cat_id)
{
	CString sUrl = server;
	CComVariant varAsync = true;
	CComVariant varNull;
	HRESULT hr;

	if(m_spHttp2){
		m_spHttp2->abort();
		m_spHttp2.Release();
	}
	m_bLoadingRooms = true;
	hr = m_spHttp2.CoCreateInstance(CLSID_XMLHTTP30);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_spHttp2->put_onreadystatechange(static_cast<IDispatch *>(this));
	ATLASSERT(SUCCEEDED(hr));

	if(server[lstrlen(server) - 1] != '/')
		sUrl += '/';
	sUrl += "ycontent/?chatroom_";
	sUrl.Append(cat_id);
	hr = m_spHttp2->open(CComBSTR("GET"),CComBSTR(LPCTSTR(sUrl)),varAsync,varNull,varNull);
	if(hr == S_OK){
		hr = m_spHttp2->send(varNull);
		if(hr == S_OK){
			
		}
	}
	return hr;
}

HRESULT CRoomDlg::LoadRooms(IXMLDOMDocument *pDoc)
{
	HRESULT hr = E_POINTER;

	if(pDoc){
		CComPtr<IXMLDOMElement> spElem;
		CComPtr<IXMLDOMNode> spNode;
		CComQIPtr<IXMLDOMElement> spElem2;
		CComBSTR bstrName;

		hr = pDoc->get_documentElement(&spElem);
		if(hr == S_OK){
			hr = spElem->get_nodeName(&bstrName);
			if(hr == S_OK && bstrName == OLESTR("content")){
				hr = spElem->get_firstChild(&spNode);
				if(hr == S_OK && ( spElem2 = spNode ) ){
					hr = spElem2->get_nodeName(&bstrName);
					if(hr == S_OK && bstrName == OLESTR("chatRooms")){
						CComPtr<IXMLDOMNodeList> spChildNodes;

						hr = spElem2->get_childNodes(&spChildNodes);
						if(hr == S_OK){
							CTreeItem item = m_tv2.GetRootItem();

							hr = AddRooms(item,spChildNodes);
						}
					}
				}
			}
		}
	}
	return hr;
}

HRESULT CRoomDlg::AddRooms(CTreeItem &parent,IXMLDOMNodeList *pNodeList)
{
	long count = 0;
	HRESULT hr = pNodeList->get_length(&count);
	CComPtr<IXMLDOMNode> spNode;
	CComQIPtr<IXMLDOMElement> spElem;
	CComBSTR bstrNodeName;
	CComVariant varAttribute;
	CComBSTR bstrAttrName;
	USES_CONVERSION;
	CRoom *pRoom = NULL;
	CTreeItem item;
	CComPtr<IXMLDOMNodeList> spChildNodes;
	CString label;
									
	for(long i=0;i<count;i++){
		spNode.Release();
		hr = pNodeList->get_item(i,&spNode);
		if(hr == S_OK && ( spElem = spNode )){
			hr = spElem->get_nodeName(&bstrNodeName);
			if(bstrNodeName == OLESTR("room")){
				pRoom = new CRoom();

				// get room name
				bstrAttrName = OLESTR("name");
				hr = spElem->getAttribute(bstrAttrName,&varAttribute);
				ATLASSERT(hr == S_OK && ( varAttribute.vt == VT_BSTR ) );
				if(hr == S_OK && varAttribute.vt == VT_BSTR){
					pRoom->m_name = OLE2CT(varAttribute.bstrVal);
				}
				else{
					delete pRoom; 
					pRoom = NULL;
					break;
				}
				// get room id
				varAttribute.Clear(); varAttribute.vt = VT_EMPTY;
				bstrAttrName = OLESTR("id");
				hr = spElem->getAttribute(bstrAttrName,&varAttribute);
				ATLASSERT(hr == S_OK);
				if(hr == S_OK && varAttribute.ChangeType(VT_UI4) == S_OK ){
					pRoom->m_id = varAttribute.ulVal;
				}
				// room topic
				varAttribute.Clear(); varAttribute.vt = VT_EMPTY;
				bstrAttrName = OLESTR("topic");
				hr = spElem->getAttribute(bstrAttrName,&varAttribute);
				if(hr == S_OK && varAttribute.vt == VT_BSTR ){
					pRoom->m_topic = OLE2CT(varAttribute.bstrVal);
				}
				// room type ( always "yahoo" ? )
				varAttribute.Clear(); varAttribute.vt = VT_EMPTY;
				bstrAttrName = OLESTR("type");
				hr = spElem->getAttribute(bstrAttrName,&varAttribute);
				if(hr == S_OK && varAttribute.vt == VT_BSTR ){
					pRoom->m_type = OLE2CT(varAttribute.bstrVal);
				}
				
				item = m_tv2.InsertItem(pRoom->m_name,parent,TVI_SORT);
				ATLASSERT(!item.IsNull());
				item.SetData((DWORD_PTR)pRoom);

				spChildNodes.Release();
				hr = spElem->get_childNodes(&spChildNodes);
				if(hr == S_OK){
					pRoom->m_users = 0;
					hr = AddLobbies(item,pRoom,spChildNodes);
					if(hr == S_FALSE)
						hr = S_OK;
					// update text with number of users
					label.Format("%s ( %d )",(LPCTSTR)pRoom->m_name,pRoom->m_users);
					item.SetText(label);
				}
			}
		}
	}
	return hr;
}

HRESULT CRoomDlg::AddLobbies(CTreeItem &parent,CRoomDlg::CRoom *pRoom,IXMLDOMNodeList *pNodeList)
{
	long count = 0;
	HRESULT hr = pNodeList->get_length(&count);
	CComPtr<IXMLDOMNode> spNode;
	CComQIPtr<IXMLDOMElement> spElem;
	CComBSTR bstrNodeName;
	CComVariant varAttribute;
	CComBSTR bstrAttrName;
	USES_CONVERSION;
	CLobby *pLobby = NULL;
	CTreeItem item;
	CComPtr<IXMLDOMNodeList> spChildNodes;
	CString label;
									
	for(long i=0;i<count;i++){
		spNode.Release();
		hr = pNodeList->get_item(i,&spNode);
		if(hr == S_OK && ( spElem = spNode )){
			hr = spElem->get_nodeName(&bstrNodeName);
			if(bstrNodeName == OLESTR("lobby")){
				pLobby = new CLobby(pRoom);

				// get room index
				bstrAttrName = OLESTR("count");
				hr = spElem->getAttribute(bstrAttrName,&varAttribute);
				ATLASSERT(hr == S_OK);
				if(hr == S_OK && ( varAttribute.ChangeType(VT_I4) == S_OK ) ){
					pLobby->m_index = varAttribute.lVal;
				}
				// get number of users
				varAttribute.Clear();
				varAttribute.vt = VT_EMPTY;
				bstrAttrName = OLESTR("users");
				hr = spElem->getAttribute(bstrAttrName,&varAttribute);
				ATLASSERT(hr == S_OK);
				if(hr == S_OK && ( varAttribute.ChangeType(VT_I4) == S_OK ) ){
					pLobby->m_users = varAttribute.lVal;
					pRoom->m_users += pLobby->m_users;
				}
				// get voice users count
				varAttribute.Clear();
				varAttribute.vt = VT_EMPTY;
				bstrAttrName = OLESTR("voices");
				hr = spElem->getAttribute(bstrAttrName,&varAttribute);
				if(hr == S_OK && ( varAttribute.ChangeType(VT_I4) == S_OK ) ){
					pLobby->m_voices = varAttribute.lVal;
				}
				// webcam count
				varAttribute.Clear();
				varAttribute.vt = VT_EMPTY;
				bstrAttrName = OLESTR("webcams");
				hr = spElem->getAttribute(bstrAttrName,&varAttribute);
				if(hr == S_OK && ( varAttribute.ChangeType(VT_I4) == S_OK ) ){
					pLobby->m_cams = varAttribute.lVal;
				}
				// format item label
				label.Format("%s:%d ( %d ) voice( %d ) cam ( %d )",(LPCTSTR)(pRoom->m_name),
					pLobby->m_index,pLobby->m_users,pLobby->m_voices,pLobby->m_cams);
				
				item = m_tv2.InsertItem(label,parent,TVI_SORT);
				ATLASSERT(!item.IsNull());
				item.SetData((DWORD_PTR)pLobby);

			}
		}
	}
	return hr;
}

HRESULT CRoomDlg::LoadServers(void)
{
	LPCTSTR file = _T("roomservers.xml");
	CComPtr<IXMLDOMDocument> spDoc;
	HRESULT hr = spDoc.CoCreateInstance(CLSID_DOMDocument30);

	m_cbServers.ResetContent();

	if(hr == S_OK){
		hr = spDoc->put_async(VARIANT_FALSE);
		if(hr == S_OK){
			CComVariant varFile(file);
			VARIANT_BOOL bRes = VARIANT_FALSE;

			hr = spDoc->load(varFile,&bRes);
			if(hr == S_OK && bRes == VARIANT_TRUE){
				CComPtr<IXMLDOMElement> spElem;
				CComPtr<IXMLDOMNodeList> spNodeList;
				CComPtr<IXMLDOMNode> spNode;
				CComBSTR bstrName;
				long count = 0;

				hr = spDoc->get_documentElement(&spElem);
				if(hr == S_OK){
					hr = spElem->get_nodeName(&bstrName);
					if(hr == S_OK && bstrName == OLESTR("servers")){
						hr = spElem->get_childNodes(&spNodeList);
						if(hr == S_OK){
							hr = spNodeList->get_length(&count);
							for(long i=0; i<count && hr == S_OK; i++){
								spNode.Release();
								hr = spNodeList->get_item(i,&spNode);
								if(hr == S_OK && spNode != NULL){
									bstrName.Empty();
									hr = spNode->get_nodeName(&bstrName);
									if(SUCCEEDED(hr) && bstrName == OLESTR("server"))
										hr = AddServer(spNode);
								}
							}
						}
					}
				}
			}
		}
	}
	if(FAILED(hr)){
		bool debug = true;
	}
	return hr;
}

HRESULT CRoomDlg::AddServer(IXMLDOMNode *pNode)
{
	HRESULT hr = E_POINTER;

	if(pNode){
		CComPtr<IXMLDOMNodeList> spNodeList;
		hr = pNode->get_childNodes(&spNodeList);
		if(hr == S_OK){
			long count = 0;
			CComPtr<IXMLDOMNode> spNode;
			CComPtr<IXMLDOMNode> spNode2;
			CComBSTR bstrName;
			CComVariant var;
			USES_CONVERSION;
			CString name,country;

			hr = spNodeList->get_length(&count);
			for(long i=0; i<count && hr == S_OK && count < 50 ; i++){
				spNode.Release();
				hr = spNodeList->get_item(i,&spNode);
				if(hr == S_OK){
					bstrName.Empty();
					hr = spNode->get_nodeName(&bstrName);
					if(bstrName == OLESTR("name")){
						spNode2.Release();
						hr = spNode->get_firstChild(&spNode2);
						if(hr == S_OK){
							var.Clear();
							var.vt = VT_EMPTY;
							hr = spNode2->get_nodeValue(&var);
							ATLASSERT(SUCCEEDED(hr) && var.vt == VT_BSTR);
							name = OLE2CT(var.bstrVal);
						}
					}
					else if(bstrName == OLESTR("country")){
						spNode2.Release();
						hr = spNode->get_firstChild(&spNode2);
						if(hr == S_OK){
							var.Clear();
							var.vt = VT_EMPTY;
							hr = spNode2->get_nodeValue(&var);
							ATLASSERT(SUCCEEDED(hr) && var.vt == VT_BSTR);
							country = OLE2CT(var.bstrVal);
						}
					}
				}
			}
			if(!name.IsEmpty()){
				int index = m_cbServers.AddString(country);
				if(index >= 0){
					ServerData *pServerData = new ServerData();
					pServerData->m_server = name;
					m_cbServers.SetItemDataPtr(index,pServerData);
				}
			}
		}
	}
	return hr;
}

void CRoomDlg::OnCategoriesLoaded(void)
{
	CComPtr<IDispatch> spDisp;
	CComQIPtr<IXMLDOMDocument> spDoc;
	HRESULT hr;

	ATLASSERT(m_bLoadingCats);
	m_bLoadingCats = false;

	hr = m_spHttp->get_responseXML(&spDisp);
	m_spHttp.Release();
	if(hr == S_OK && (spDoc = spDisp) ){
		m_tv.SetRedraw(FALSE);
		m_tv2.SetRedraw(FALSE);
		m_tv.DeleteAllItems();
		m_tv2.DeleteAllItems();
		hr = LoadCategories(spDoc);
		m_tv.SetRedraw(TRUE);
		m_tv2.SetRedraw(TRUE);
		m_tv.InvalidateRect(NULL);
		m_tv2.InvalidateRect(NULL);
	}
}

void CRoomDlg::OnRoomsLoaded(void)
{
	CComPtr<IDispatch> spDisp;
	CComQIPtr<IXMLDOMDocument> spDoc;
	HRESULT hr;

	ATLASSERT(m_bLoadingRooms);
	m_bLoadingRooms = false;

	
	hr = m_spHttp2->get_responseXML(&spDisp);
	m_spHttp2.Release();
	if(hr == S_OK && (spDoc = spDisp) ){
		hr = LoadRooms(spDoc);
	}
}

void __stdcall CRoomDlg::OnReadyStateChange(void)
{
	long state = 0;
	HRESULT hr;
	CWindow wnd = GetDlgItem(IDC_STATUS);

	if(m_bLoadingCats)
		hr = m_spHttp->get_readyState(&state);
	else if(m_bLoadingRooms)
		hr = m_spHttp2->get_readyState(&state);
	else{
		ATLASSERT(0);
	}
	if(hr == S_OK){
		switch(state)
		{
			case 0:		// uninitialized - open not called yet
				wnd.SetWindowText("opening");
				break;
			case 1:		// loading - send not called yet
				wnd.SetWindowText("loading .");
				break;
			case 2:		// loaded -  status and headers not available yet
				wnd.SetWindowText("loading ..");
				break;
			case 3:		// interactive - some data received
				wnd.SetWindowText("loading ...");
				break;
			case 4:		// completed - all data received
				wnd.SetWindowText("loaded");
				if(m_bLoadingCats)
					OnCategoriesLoaded();
				else if(m_bLoadingRooms)
					OnRoomsLoaded();
				else{
					ATLASSERT(0);
				}
				break;
			default:
				ATLASSERT(0);
				break;
		}
	}
}

void CRoomDlg::LoadRecentRooms(void)
{
	CRecentRooms *p;

	if(!g_app.GetRecentRooms()){
		g_app.LoadRecentRooms();
	}
	p = g_app.GetRecentRooms();

	if(p){
		m_cbRooms.ResetContent();
		p->FillCombo(m_cbRooms);
	}
}