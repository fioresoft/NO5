// UserListView.h: interface for the CUserListView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERLISTVIEW_H__F922AD93_304F_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_USERLISTVIEW_H__F922AD93_304F_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <no5tl\htmllistbox.h>
#include <no5tl\no5tlbase.h>	// simplemap
#include "iuserlistview.h"
using namespace NO5TL;

class CUserListView;

class CUserListBox : \
	public NO5TL::CHtmlListBoxImpl<CUserListBox>,
	public IDispatchImpl<IDocHostUIHandlerDispatch, &IID_IDocHostUIHandlerDispatch, &LIBID_ATLLib>
{
	typedef NO5TL::CHtmlListBoxImpl<CUserListBox> _BaseClass;

	struct UserData
	{
		CUser *m_pUser;
		bool m_bExpanded;
	
		UserData(void)
		{
			m_pUser = NULL;
			m_bExpanded = false;
		}
	};
public:
	//DECLARE_WND_SUPERCLASS(NULL,CUserListBox::GetWndClassName())

	BEGIN_MSG_MAP(CUserListBox)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		CHAIN_MSG_MAP(_BaseClass)
	ALT_MSG_MAP(1)
		NOTIFY_CODE_HANDLER(HLBN_SELCHANGED,OnSelChanged)
		NOTIFY_CODE_HANDLER(HLBN_COMPAREITEM,OnCompareItem)
		NOTIFY_CODE_HANDLER(HLBN_DELETEITEM,OnDeleteItem)
		NOTIFY_CODE_HANDLER(HLBN_CLICK,OnItemClick)
		NOTIFY_CODE_HANDLER(HLBN_DBLCLK,OnItemDoubleClick)
	END_MSG_MAP()


	int AddUser(CUser *pUser);
	int FindUser(CUser *pUser);
	int FindUser(LPCTSTR name);
	void UpdateUser(CUser *pUser);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//
	LRESULT OnSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnCompareItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnDeleteItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnItemClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnItemDoubleClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);

	// IDocHostUIHandlerDispatch

	STDMETHOD(ShowContextMenu)( 
            /* [in] */ DWORD dwID,
            /* [in] */ DWORD x,
            /* [in] */ DWORD y,
            /* [in] */ IUnknown *pcmdtReserved,
            /* [in] */ IDispatch  *pdispReserved,
            /* [retval][out] */ HRESULT  *dwRetVal);
	
	STDMETHOD(GetHostInfo)( 
            /* [out][in] */ DWORD  *pdwFlags,
            /* [out][in] */ DWORD  *pdwDoubleClick)
	{
		return S_FALSE;
	}
	
	STDMETHOD(ShowUI)( 
            /* [in] */ DWORD dwID,
            /* [in] */ IUnknown  *pActiveObject,
            /* [in] */ IUnknown  *pCommandTarget,
            /* [in] */ IUnknown  *pFrame,
            /* [in] */ IUnknown  *pDoc,
            /* [retval][out] */ HRESULT  *dwRetVal)
	{
		return S_FALSE;
	}
	
	STDMETHOD(HideUI)( void)
	{
		return S_FALSE;
	}
	
	STDMETHOD(UpdateUI)( void)
	{
		return S_FALSE;
	}
        
    STDMETHOD(EnableModeless)( 
            /* [in] */ VARIANT_BOOL fEnable)
	{
		return S_FALSE;
	}
        
    STDMETHOD(OnDocWindowActivate)( 
            /* [in] */ VARIANT_BOOL fActivate)
	{
		return S_FALSE;
	}
	
	STDMETHOD(OnFrameWindowActivate)( 
            /* [in] */ VARIANT_BOOL fActivate)
	{
		return S_FALSE;
	}
        
    STDMETHOD(ResizeBorder)( 
            /* [in] */ long left,
            /* [in] */ long top,
            /* [in] */ long right,
            /* [in] */ long bottom,
            /* [in] */ IUnknown  *pUIWindow,
            /* [in] */ VARIANT_BOOL fFrameWindow)
	{
		 return S_FALSE;
	}
	
	STDMETHOD(TranslateAccelerator)( 
            /* [in] */ DWORD hWnd,
            /* [in] */ DWORD nMessage,
            /* [in] */ DWORD wParam,
            /* [in] */ DWORD lParam,
            /* [in] */ BSTR bstrGuidCmdGroup,
            /* [in] */ DWORD nCmdID,
            /* [retval][out] */ HRESULT  *dwRetVal)
	{
		return S_FALSE;
	}
        
    STDMETHOD(GetOptionKeyPath)( 
            /* [out] */ BSTR  *pbstrKey,
            /* [in] */ DWORD dw)
	{
		return S_FALSE;
	}
        
    STDMETHOD(GetDropTarget)( 
            /* [in] */ IUnknown  *pDropTarget,
            /* [out] */ IUnknown  **ppDropTarget)
	{
		return S_FALSE;
	}
        
    STDMETHOD(GetExternal)( 
           /* [out] */ IDispatch  **ppDispatch)
	{
		*ppDispatch = NULL;
		return S_FALSE;
	}
        
    STDMETHOD(TranslateUrl)( 
            /* [in] */ DWORD dwTranslate,
            /* [in] */ BSTR bstrURLIn,
            /* [out] */ BSTR  *pbstrURLOut)
	{
		return S_FALSE;
	}
        
    STDMETHOD(FilterDataObject)( 
           /* [in] */ IUnknown  *pDO,
           /* [out] */ IUnknown  **ppDORet)
	{
		return S_FALSE;
	}
	// IUnknown
	STDMETHOD(QueryInterface)(REFIID iid,LPVOID *ppvObj)
	{
		HRESULT hr = S_OK;
		*ppvObj = NULL;

		if(InlineIsEqualUnknown(iid)){
			*ppvObj = static_cast<IUnknown *>(this);
		}
		else if(InlineIsEqualGUID(iid,IID_IDispatch))
		{
			*ppvObj = static_cast<IDispatch *>(this);
		}
		else if(InlineIsEqualGUID(iid,IID_IDocHostUIHandlerDispatch)){
			*ppvObj = static_cast<IDocHostUIHandlerDispatch *>(this);
		}
		else
			hr = E_NOINTERFACE;

		if(SUCCEEDED(hr)){
			IUnknown *p = static_cast<IUnknown *>(this);
			p->AddRef();
		}
		return hr;
	}
	STDMETHOD_(ULONG,AddRef)(void)
	{
		return 1;
	}
	STDMETHOD_(ULONG,Release)(void)
	{
		return 1;
	}

	CUserListBox(CUserListView *pOutter):m_pOutter(pOutter),m_client_images(false)
	{
		//
	}

private:
	void FormatText(bool bsel,UserData *p,CString &out);
	void FormatInfotagString(CUser *pUser,CString &out);
	void FormatYmsgInfo(CUser *pUser,CString &out);
	BOOL OnImageClicked(NO5TL::CHtmlElement &el,NO5TL::HLBITEM &item);
	BOOL OnInputClicked(NO5TL::CHtmlElement &el,NO5TL::HLBITEM &item);
	BOOL LoadClientImageNames(void);
	//
	CString m_ImgUser;
	CString m_ImgUserSel;
	CString m_ImgUp;
	CString m_ImgCam;
	CUserListView *m_pOutter;
	NO5TL::CNo5SimpleMap<CString,CString> m_client_images;

	friend CUserListView;
};


class CUserListView : \
	public CWindowImpl<CUserListView>,
	public IUserListView
{
	typedef CWindowImpl<CUserListView> _BaseClass;
public:
	CUserListView();
	virtual ~CUserListView();

	BEGIN_MSG_MAP(CUserListView)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		CHAIN_MSG_MAP_ALT_MEMBER(m_lb,1)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);

	// IUserListView
	virtual void AddUser(CUser *p);
	virtual void RemoveUser(LPCSTR user);
	virtual void RemoveAll(void);
	virtual void AddUsers(CUserList &users);
	virtual void SetSink(IUserListViewEvents *pSink);
	virtual void UpdateUser(CUser *pUser);

private:
	IUserListViewEvents *m_pSink;
	CUserListBox m_lb;
	friend CUserListBox;

	void OnContextMenu(CUser *pUser,POINT &pt);
};

#endif // !defined(AFX_USERLISTVIEW_H__F922AD93_304F_11DA_A17D_000103DD18CD__INCLUDED_)
