// No5Obj.cpp : Implementation of CYcht02App and DLL registration.

#include "stdafx.h"
#include <no5tl\no5tlbase.h>	// copy array
#include <yahoo\yahoochat.h>
#include "no5_3.h"
#include "No5Obj.h"
#include "no5app.h"
#include "imainframe.h"
#include "aboutdlg.h"
#include "yahoochatview.h"
#include "yahoomsgview.h"
#include "user.h"

using namespace NO5YAHOO;

CNo5Obj::CNo5Obj()
{
	//
}

CNo5Obj::~CNo5Obj()
{
	int debug = 0;
}

STDMETHODIMP CNo5Obj::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_INo5Obj,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


STDMETHODIMP CNo5Obj::About()
{
	CWindow wnd = g_app.GetMainFrame()->GetHandle();
	CAboutDlg dlg;

	dlg.DoModal();

	return S_OK;
}


STDMETHODIMP CNo5Obj::get_Chats(IDispatch **pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal){
		CSimpleValArray<CViewBase *> views;
		CSimpleValArray<CChatViewBase*> chats;
		int i;
		int count;
		CViewBase *pView;

		*pVal = NULL;
		g_app.GetMainFrame()->GetViews(views);
		count = views.GetSize();
		for(i=0;i<count;i++){
			pView = views[i];
			if(pView->IsChatView()){
				chats.Add((CChatViewBase *)pView);
			}
		}
		
		CComObject<CChatsObj> *pObj = NULL;
		hr = CComObject<CChatsObj>::CreateInstance(&pObj);
		if(SUCCEEDED(hr)){
			hr = pObj->QueryInterface(pVal);
			if(SUCCEEDED(hr)){
				hr = pObj->Init(chats);
			}
		}
		if(FAILED(hr))
			delete pObj;
	}
	return hr;
}

STDMETHODIMP CNo5Obj::get_IgnoreCount(long *pVal)
{
	*pVal = (long)g_app.GetIgnoreList()->size();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
//

STDMETHODIMP CChatObj::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IChatObj,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CChatObj::SendMsg(BSTR msg)
{
	USES_CONVERSION;
	if(m_ycht && m_pycht)
		m_pycht->m_pChat->SendText(OLE2CT(msg));
	else if(m_pymsg)
		m_pymsg->m_pChat->ChatMessage(OLE2CT(msg),false,NO5YAHOO::YMSG_TEXT_NORMAL);

	return S_OK;
}

STDMETHODIMP CChatObj::Think(BSTR msg)
{
	USES_CONVERSION;
	if(m_ycht && m_pycht)
		m_pycht->m_pChat->Think(OLE2CT(msg));
	else if(m_pymsg)
		m_pymsg->m_pChat->ChatMessage(OLE2CT(msg),false,NO5YAHOO::YMSG_TEXT_BUBBLE);

	return S_OK;
}

STDMETHODIMP CChatObj::SendEmote(BSTR msg)
{
	USES_CONVERSION;
	if(m_ycht && m_pycht)
		m_pycht->m_pChat->SendEmote(OLE2CT(msg));
	else if(m_pymsg)
		m_pymsg->m_pChat->ChatMessage(OLE2CT(msg),false,NO5YAHOO::YMSG_TEXT_EMOTE);

	return S_OK;
}

STDMETHODIMP CChatObj::SendPrivateMsg(BSTR name, BSTR msg)
{
	USES_CONVERSION;
	if(m_ycht && m_pycht){
		IYahooChat *pChat = m_pycht->m_pChat;
		if(pChat){
			pChat->SendPm(OLE2CT(name),OLE2CT(msg));
		}
	}
	else if(m_pymsg){
		IYahooMessenger *pChat = m_pymsg->m_pChat;
		if(pChat){
			pChat->SendMsg(pChat->GetName(),OLE2CT(name),OLE2CT(msg));
		}
	}
	return S_OK;
}

STDMETHODIMP CChatObj::JoinRoom(BSTR room)
{
	USES_CONVERSION;
	if(m_ycht && m_pycht)
		m_pycht->m_pChat->JoinRoom(OLE2CT(room));
	else if(m_pymsg)
		m_pymsg->m_pChat->ChatJoin(OLE2CT(room),"");

	return S_OK;
}

STDMETHODIMP CChatObj::get_MyName(BSTR *pVal)
{
	HRESULT hr = E_POINTER;
	if(pVal){
		USES_CONVERSION;
		CComBSTR name;
		if(m_ycht && m_pycht){
			name = m_pycht->m_pChat->GetName();
		}
		else if(m_pymsg)
			name = m_pymsg->m_pChat->GetName();
		*pVal = name.Detach();
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CChatObj::get_Room(BSTR *pVal)
{
	HRESULT hr = E_POINTER;
	if(pVal){
		USES_CONVERSION;
		CComBSTR name;
		if(m_ycht && m_pycht){
			name = m_pycht->m_pChat->GetRoom();
		}
		else if(m_pymsg)
			name = m_pymsg->m_pChat->GetRoom();
		*pVal = name.Detach();
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CChatObj::get_VoiceCookie(BSTR *pVal)
{
	HRESULT hr = E_POINTER;
	if(pVal){
		USES_CONVERSION;
		CComBSTR name;
		if(m_ycht && m_pycht){
			hr = S_FALSE;
		}
		else if(m_pymsg){
			name = m_pymsg->m_pChat->GetCookie(130);
			*pVal = name.Detach();
			hr = S_OK;
		}
	}
	return hr;
}

STDMETHODIMP CChatObj::get_RoomSpace(BSTR *pVal)
{
	HRESULT hr = E_POINTER;
	if(pVal){
		USES_CONVERSION;
		CComBSTR name;
		if(m_ycht && m_pycht){
			name = m_pycht->m_pChat->GetRoomSpace();
		}
		else if(m_pymsg)
			name = m_pymsg->m_pChat->GetRoomSpace();
		*pVal = name.Detach();
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CChatObj::get_Users(IDispatch **pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal){
		USES_CONVERSION;
		CComObject<CUsersObj> *pObj = NULL;
		hr = CComObject<CUsersObj>::CreateInstance(&pObj);
		if(SUCCEEDED(hr)){
			if(m_ycht && m_pycht)
				pObj->Init(m_pycht->m_users);
			else if(m_pymsg)
				pObj->Init(m_pymsg->m_users);
			hr = pObj->QueryInterface(pVal);
		}
		if(FAILED(hr) && pObj)
			delete pObj;
	}
	return hr;
}

STDMETHODIMP CChatObj::Ignore(BSTR name, VARIANT_BOOL bIgnore)
{
	USES_CONVERSION;
	if(m_ycht && m_pycht)
		m_pycht->DoIgnoreUser(OLE2CT(name),bIgnore == VARIANT_TRUE);
	else if(m_pymsg)
		m_pymsg->DoIgnoreUser(OLE2CT(name),bIgnore == VARIANT_TRUE);
	return S_OK;
}

STDMETHODIMP CChatObj::get_RemoteAddress(BSTR *pVal)
{
	CString server;
	int port;
	CComBSTR bs;
	if(m_ycht && m_pycht)
		m_pycht->m_pChat->GetRemoteAddress(server,port);
	else if(m_pymsg)
		m_pymsg->m_pChat->GetRemoteAddress(server,port);
	bs = server;
	*pVal = bs.Detach();
	return S_OK;
}

STDMETHODIMP CChatObj::get_LocalAddress(BSTR *pVal)
{
	CString server;
	int port;
	CComBSTR bs;
	if(m_ycht && m_pycht)
		m_pycht->m_pChat->GetLocalAddress(server,port);
	else if(m_pymsg)
		m_pymsg->m_pChat->GetLocalAddress(server,port);
	bs = server;
	*pVal = bs.Detach();
	return S_OK;
}

STDMETHODIMP CChatObj::IsYcht(VARIANT_BOOL *pRes)
{
	HRESULT hr = E_POINTER;

	if(pRes){
		*pRes = m_ycht ? VARIANT_TRUE : VARIANT_FALSE;
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CChatObj::RequestCamCookie(BSTR name)
{
	if(!m_ycht){
		USES_CONVERSION;
		m_pymsg->m_pChat->Webcam(OLE2CT(name));
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
//

HRESULT CChatsObj::Init(CSimpleValArray<CChatViewBase*> &yc)
{
	NO5TL::CopySimpleArray(m_chats,yc);
	return S_OK;
}

STDMETHODIMP CChatsObj::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IChatsObj,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CChatsObj::get_Count(long *pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal){
		*pVal = (long) m_chats.GetSize();
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CChatsObj::get_Item(long n, IDispatch **pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal){
		*pVal = NULL;
		if( n >= 0 && n < m_chats.GetSize()){
			CChatViewBase *p = m_chats[n];
			if(p){
				CComObject<CChatObj> *pObj = NULL;
				hr = CComObject<CChatObj>::CreateInstance(&pObj);
				if(SUCCEEDED(hr)){
					hr = pObj->QueryInterface(pVal);
					if(SUCCEEDED(hr)){
						if(p->IsYchtView())
							hr = pObj->Init((CYahooChatViewBase *)p);
						else
							hr = pObj->Init((CYahooMsgViewBase *)p);
					}
				}
				if(FAILED(hr))
					delete pObj;
			}
		}
		else
			hr = E_INVALIDARG; // ok ?
	}
	return hr;
}



/////////////////////////////////////////////////////////////////////////////
//

CUsersObj::CUsersObj()
{
	m_pUsers = new CUserList();
}

CUsersObj::~CUsersObj()
{
	delete m_pUsers;
}

void CUsersObj::Init(CUserList &users)
{
	const int count = users.GetSize();
	m_pUsers->RemoveAll();
	
	for(int i=0;i<count;i++){
		CUser *pUser = users.GetUser(i);
		if(pUser && pUser->m_chat)
			m_pUsers->AddUser(*pUser);
	}
}

STDMETHODIMP CUsersObj::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IUsersObj,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CUsersObj::get_Count(long *pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal && m_pUsers){
		*pVal = (long)m_pUsers->GetSize();
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CUsersObj::get_Item(long n, IDispatch **pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal){
		*pVal = NULL;
		if( n >= 0 && n < m_pUsers->GetSize()){
			CUser *pUser = m_pUsers->GetUser((int)n);
			ATLASSERT(pUser);
			CComObject<CUserObj> *pObj = NULL;
			hr = CComObject<CUserObj>::CreateInstance(&pObj);
			if(SUCCEEDED(hr)){
				hr = pObj->QueryInterface(pVal);
				if(SUCCEEDED(hr))
					pObj->Init(*pUser);
			}
			if(FAILED(hr))
				delete pObj;
		}
		else
			hr = E_INVALIDARG; 
	}
	return hr;
}


STDMETHODIMP CUsersObj::get__NewEnum(IUnknown **pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal){
		*pVal = NULL;
		int count = m_pUsers->GetSize();
		VARIANT *arr = new VARIANT[count];
		CComVariant var;
		CComPtr<IDispatch> spDisp;

		ZeroMemory(arr,count * sizeof(VARIANT));
		for(int i=0;i<count;i++){
			CUser *pUser = m_pUsers->GetUser(i);
			ATLASSERT(pUser);
			CComObject<CUserObj> *pObj = NULL;
			hr = CComObject<CUserObj>::CreateInstance(&pObj);
			if(SUCCEEDED(hr)){
				hr = pObj->QueryInterface(&spDisp);
				if(SUCCEEDED(hr)){
					pObj->Init(*pUser);
					var = (IDispatch *)spDisp;
					arr[i].vt = VT_EMPTY;
					hr = var.Detach(&arr[i]);
					spDisp.Release();
				}
			}
			if(FAILED(hr)){
				delete pObj;
				pObj = NULL;
				break;
			}
		}
		spDisp.Release(); // not really needed
		if(SUCCEEDED(hr)){
			typedef CComEnum<IEnumVARIANT,&IID_IEnumVARIANT,VARIANT,_Copy<VARIANT>,
				CComSingleThreadModel> EnumVar;
			CComObject<EnumVar> *pObj = NULL;
			hr = CComObject<EnumVar>::CreateInstance(&pObj);
			if(SUCCEEDED(hr)){
				hr = pObj->Init(&arr[0],&arr[count],0,AtlFlagTakeOwnership);
				if(SUCCEEDED(hr)){
					hr = pObj->QueryInterface(pVal);
				}
			}
		}
		if(FAILED(hr)){
			if(arr){	// wright ? todo
				CComVariant v;
				for(int i=0;i<count;i++){
					v = arr[i];
					v.Clear();
				}
				delete []arr;
			}
		}
	}
	return hr;
}

/*
STDMETHODIMP CUsersObj::get__NewEnum(IUnknown **pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal){
		*pVal = NULL;
		int count = 10;
		long blah[10] = {0,1,2,3,4,5,6,7,8,9};
		VARIANT arr[10];
	
		for(int i=0;i<count;i++){
			arr[i].vt = VT_I4;
			arr[i].lVal = blah[i];
		}
		if(SUCCEEDED(hr)){
			typedef CComEnum<IEnumVARIANT,&IID_IEnumVARIANT,VARIANT,_Copy<VARIANT>,
				CComSingleThreadModel> EnumVar;
			CComObject<EnumVar> *pObj = NULL;
			hr = CComObject<EnumVar>::CreateInstance(&pObj);
			if(SUCCEEDED(hr)){
				hr = pObj->Init(&arr[0],&arr[count],0,AtlFlagCopy);
			}
		}
	}
	return hr;
}
*/

/////////////////////////////////////////////////////////////////////////////
//
CUserObj::CUserObj()
{
	m_pUser = new CUser();
}

CUserObj::~CUserObj()
{
	delete m_pUser;
}

void CUserObj::Init(CUser &user)
{
	*m_pUser = user;
}

STDMETHODIMP CUserObj::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IUserObj,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


STDMETHODIMP CUserObj::get_Name(BSTR *pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal && m_pUser){
		CComBSTR name = m_pUser->m_name;
		*pVal = name.Detach();
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CUserObj::get_Cam(VARIANT_BOOL *pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal && m_pUser){
		*pVal = m_pUser->m_cam ? VARIANT_TRUE : VARIANT_FALSE;
		hr = S_OK;
	}
	return hr;
}


STDMETHODIMP CUserObj::get_Client(BSTR *pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal && m_pUser){
		CComBSTR s = m_pUser->GetClientName();
		*pVal = s.Detach();
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CUserObj::get_Nickname(BSTR *pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal && m_pUser){
		CComBSTR s = m_pUser->m_nick;
		*pVal = s.Detach();
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CUserObj::get_Location(BSTR *pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal && m_pUser){
		CComBSTR s = m_pUser->m_location;
		*pVal = s.Detach();
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CUserObj::get_Gender(short *pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal && m_pUser){
		if(m_pUser->IsMale())
			*pVal = 1;
		else if(m_pUser->IsFemale())
			*pVal = 2;
		else
			*pVal = 0;
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CUserObj::get_Age(short *pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal && m_pUser){
		*pVal = m_pUser->m_age;
	}
	return hr;
}




/////////////////////////////////////////////////////////////////////////////
//


STDMETHODIMP CNo5Obj::Output(BSTR s)
{
	USES_CONVERSION;
	ATLTRACE("%s\n",OLE2CT(s));
	return S_OK;
}


STDMETHODIMP CNo5Obj::CreateTimer(/*[in]*/ long nInterval, /*[out,retval]*/ long *pTimer)
{
	*pTimer = g_app.GetMainFrame()->CreateTimer(nInterval);
	return S_OK;
}

STDMETHODIMP CNo5Obj::DestroyTimer(long timer)
{
	g_app.GetMainFrame()->DestroyTimer(timer);
	return S_OK;
}

STDMETHODIMP CNo5Obj::get_ActiveView(IDispatch **pVal)
{
	return get_ActiveChat(pVal);
}

STDMETHODIMP CNo5Obj::get_ActiveChat(IDispatch **pVal)
{
	HRESULT hr = S_FALSE;
	CViewBase *pView = g_app.GetMainFrame()->ActiveView();
	*pVal = NULL;
	if(pView && pView->IsChatView()){
		CComPtr<IDispatch> sp = ((CChatViewBase *)pView)->m_spChat;
		if(sp){
			*pVal = sp.Detach();
			hr = S_OK;
		}
	}
	else
		*pVal = NULL;
	return hr;
}

STDMETHODIMP CNo5Obj::GetChatByName(BSTR name, IDispatch **ppDisp)
{
	USES_CONVERSION;
	LPCTSTR s = OLE2CT(name);
	CSimpleValArray<CViewBase*> views;
	HRESULT hr = S_FALSE;
	g_app.GetMainFrame()->GetViews(views);

	*ppDisp = NULL;
	for(int i=0;i<views.GetSize();i++){
		if(views[i]->IsChatView()){
			 CChatViewBase *pChat = (CChatViewBase *)(views[i]);
			 if(pChat->IsMe(s)){
				 CComPtr<IDispatch> sp = pChat->m_spChat;
				 *ppDisp = sp.Detach();
				 hr = S_OK;
				 break;
			 }
		}
	}
	return hr;
}

STDMETHODIMP CNo5Obj::AddMarqueeString(BSTR msg)
{
	USES_CONVERSION;
	g_app.GetMainFrame()->AddMarqueeString(OLE2CT(msg));
	return S_OK;
}

