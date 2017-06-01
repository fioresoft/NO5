// No5Obj.h: Definition of the CNo5Obj class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NO5OBJ_H__63BD3F7C_C105_4813_9AA0_2524B0253479__INCLUDED_)
#define AFX_NO5OBJ_H__63BD3F7C_C105_4813_9AA0_2524B0253479__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include "no5_3.h"			// CLSID's
#include "no5_3CP.h"

class CYahooChatViewBase;
class CUserList;
class CUser;
class CYahooMsgViewBase;
class CChatViewBase;


/////////////////////////////////////////////////////////////////////////////
// CNo5Obj

class CNo5Obj : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<INo5Obj, &IID_INo5Obj>, 
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CNo5Obj>,
	public IProvideClassInfo2Impl<&CLSID_No5Obj,&DIID__INo5ObjEvents>,
	public CProxy_INo5ObjEvents< CNo5Obj >
{
public:
	CNo5Obj();
	virtual ~CNo5Obj();

	DECLARE_NOT_AGGREGATABLE(CNo5Obj) 

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CNo5Obj)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(INo5Obj)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo2)
		COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CNo5Obj)
		CONNECTION_POINT_ENTRY(DIID__INo5ObjEvents)
	END_CONNECTION_POINT_MAP()

// DECLARE_REGISTRY(CNo5Obj, _T("Ycht02.No5Obj.1"), _T("Ycht02.No5Obj"), IDS_NO5OBJ_DESC, THREADFLAGS_BOTH)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// INo5Obj
public:
	STDMETHOD(get_ActiveChat)(/*[out, retval]*/ IDispatch* *pVal);
	STDMETHOD(AddMarqueeString)(/*[in]*/ BSTR msg);
	STDMETHOD(GetChatByName)(/*[in]*/ BSTR name, /*[out,retval]*/ IDispatch **ppDisp);
	STDMETHOD(get_ActiveView)(/*[out, retval]*/ IDispatch* *pVal);
	STDMETHOD(DestroyTimer)(/*[in]*/ long timer);
	STDMETHOD(CreateTimer)(/*[in]*/ long nInterval, /*[out,retval]*/ long *pTimer);
	STDMETHOD(get_IgnoreCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(Output)(/*[in]*/ BSTR s);
	STDMETHOD(get_Chats)(/*[out, retval]*/ IDispatch* *pVal);
	STDMETHOD(About)();
	//
	HRESULT PrivateInit(void)
	{
		return S_OK;
	}
private:
};


/////////////////////////////////////////////////////////////////////////////
// CYchtObj

class CChatObj : 
	public IDispatchImpl<IChatObj, &IID_IChatObj>, 
	public ISupportErrorInfo,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CProxy_INo5ObjEvents< CChatObj >,
	public IConnectionPointContainerImpl<CChatObj>
{
public:
	CChatObj()
	{
		m_pymsg = NULL;
		m_ycht = false;
	}
	
	BEGIN_COM_MAP(CChatObj)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IChatObj)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
	END_COM_MAP()

	DECLARE_NOT_AGGREGATABLE(CChatObj) 

	//
	HRESULT Init(CYahooChatViewBase *pView)
	{
		m_pycht = pView;
		m_ycht = true;
		return S_OK;
	}
	HRESULT Init(CYahooMsgViewBase *pView)
	{
		m_pymsg = pView;
		m_ycht = false;
		return S_OK;
	}

//DECLARE_REGISTRY(CYchtObj, _T("Ycht02.YchtObj.1"), _T("Ycht02.YchtObj"), IDS_YCHTOBJ_DESC, THREADFLAGS_BOTH)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IYchtObj
public:
	STDMETHOD(get_Room)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_MyName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(JoinRoom)(/*[in]*/ BSTR room);
	STDMETHOD(SendMsg)(/*[in]*/ BSTR msg);
private:
	union{
		CYahooChatViewBase *m_pycht;
		CYahooMsgViewBase *m_pymsg;
	};
	bool m_ycht;
public :
	STDMETHOD(get_VoiceCookie)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_RoomSpace)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(RequestCamCookie)(/*[in]*/ BSTR name);
	STDMETHOD(IsYcht)(/*[out,retval]*/ VARIANT_BOOL *pRes);
	STDMETHOD(get_LocalAddress)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_RemoteAddress)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(Ignore)(/*[in]*/ BSTR name, /*[in]*/ VARIANT_BOOL bIgnore);
	STDMETHOD(SendEmote)(/*[in]*/ BSTR msg);
	STDMETHOD(Think)(/*[in]*/ BSTR msg);
	STDMETHOD(SendPrivateMsg)(/*[in]*/ BSTR name, /*[in]*/ BSTR msg);
	STDMETHOD(get_Users)(/*[out, retval]*/ IDispatch* *pVal);

BEGIN_CONNECTION_POINT_MAP(CChatObj)
	CONNECTION_POINT_ENTRY(DIID__INo5ObjEvents)
END_CONNECTION_POINT_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CChatsObj

class CChatsObj : 
	public IDispatchImpl<IChatsObj, &IID_IChatsObj>, 
	public ISupportErrorInfo,
	public CComObjectRootEx<CComSingleThreadModel>
{
public:
	CChatsObj() {}
	BEGIN_COM_MAP(CChatsObj)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IChatsObj)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
	END_COM_MAP()
	DECLARE_NOT_AGGREGATABLE(CChatsObj) 

	HRESULT Init(CSimpleValArray<CChatViewBase*> &yc);

	//DECLARE_REGISTRY(CChatsObj, _T("Ycht02.ChatsObj.1"), _T("Ycht02.ChatsObj"), IDS_CHATSOBJ_DESC, THREADFLAGS_BOTH)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IChatsObj
public:
	STDMETHOD(get_Item)(/*[in]*/ long n, /*[out, retval]*/ IDispatch* *pVal);
	STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
private:
	CSimpleValArray<CChatViewBase*> m_chats;
};

/////////////////////////////////////////////////////////////////////////////
// CUsersObj

class CUsersObj : 
	public IDispatchImpl<IUsersObj, &IID_IUsersObj>, 
	public ISupportErrorInfo,
	public CComObjectRootEx<CComSingleThreadModel>
{
public:
	CUsersObj();
	virtual ~CUsersObj();
BEGIN_COM_MAP(CUsersObj)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IUsersObj)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CUsersObj) 

	void Init(CUserList &users);

//DECLARE_REGISTRY(CUsersObj, _T("Ycht02.UsersObj.1"), _T("Ycht02.UsersObj"), IDS_USERSOBJ_DESC, THREADFLAGS_BOTH)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IUsersObj
public:
	STDMETHOD(get__NewEnum)(/*[out, retval]*/ IUnknown* *pVal);
	STDMETHOD(get_Item)(/*[in]*/ long n, /*[out, retval]*/ IDispatch* *pVal);
	STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
private:
	CUserList *m_pUsers;
};

/////////////////////////////////////////////////////////////////////////////
// CUserObj

class CUserObj : 
	public IDispatchImpl<IUserObj, &IID_IUserObj>, 
	public ISupportErrorInfo,
	public CComObjectRootEx<CComSingleThreadModel>
{
public:
	CUserObj();
	virtual ~CUserObj();
BEGIN_COM_MAP(CUserObj)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IUserObj)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CUserObj) 

	void Init(CUser &user);

//DECLARE_REGISTRY(CUserObj, _T("Ycht02.UserObj.1"), _T("Ycht02.UserObj"), IDS_USEROBJ_DESC, THREADFLAGS_BOTH)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IUserObj
public:
	STDMETHOD(get_Location)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Nickname)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Age)(/*[out, retval]*/ short *pVal);
	STDMETHOD(get_Gender)(/*[out, retval]*/ short *pVal);
	STDMETHOD(get_Client)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Cam)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
private:
	CUser *m_pUser;
};


#endif // !defined(AFX_NO5OBJ_H__63BD3F7C_C105_4813_9AA0_2524B0253479__INCLUDED_)
