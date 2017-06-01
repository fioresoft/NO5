// YahooChatView.h: interface for the CYahooChatRichView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YAHOOCHATVIEW_H__8C41EEA1_2D6C_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_YAHOOCHATVIEW_H__8C41EEA1_2D6C_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <no5tl\mystring.h>
#include <no5tl\no5richedit.h>
#include <no5tl\htmledit.h>
#include <yahoo\yahoochat.h>
#include <yahoo\yahoocolor.h>
#include "no5stream.h"
#include "usrmsgs.h"
#include "user.h"
#include "viewbase.h"
#include "imainframe.h"
#include "iuserlistview.h"
#include "privatebox.h"
#include "ignorelist.h"
#include "ibuddylistview.h"
#include <no5tl\atldispa.h>

#import "C:\downloads\AimlBot1_0\AimlBot1.1\bin\Debug\aimlbot.dll" no_namespace raw_interfaces_only named_guids

// using declarations
//using NO5TL::CHtmlEditImpl;
using NO5TL::CHtmlCtrl;
using NO5TL::MSHTML_LIBVER_MAJ;
using NO5TL::MSHTML_LIBVER_MIN;
using NO5TL::WBC_LIBVER_MAJ;
using NO5TL::WBC_LIBVER_MIN;

// fwd declarations
struct IVisualOptions;
struct IGeneralOptions;
struct IAccountOptions;
class CCommandParser;
struct CPmOptions;
class CRecentRooms;
class CYahooMsgViewBase;

//

COLORREF HighlightColor(COLORREF clrOld,int inc);

struct InitTS
{
	CTextStream &m_ts;
	const bool m_bEndl;

	InitTS(CTextStream &ts,bool bEndl = true);
	~InitTS();
};

class CRoomHistory
{
	struct CRoomHistoryEntry
	{
		CRoomHistoryEntry *m_previous;
		CRoomHistoryEntry *m_next;
		CString m_room;
		//
		CRoomHistoryEntry(LPCTSTR room):m_room(room)
		{
			m_previous = m_next = NULL;
		}
	};
	CRoomHistoryEntry *m_cur;
public:
	bool m_bAdd; // next room should be added
public:
	CRoomHistory(void)
	{
		m_cur = NULL;
		m_bAdd = true;
	}
	~CRoomHistory(void)
	{
		Purge();
	}
	void Add(LPCTSTR room)
	{
		CRoomHistoryEntry *p = new CRoomHistoryEntry(room);

		if(!m_cur){
			m_cur = p;
		}
		else{
			m_cur->m_next = p;
			p->m_previous = m_cur;
			m_cur = p;
		}
	}
	LPCTSTR GetRoom(void)
	{
		LPCTSTR res = NULL;

		if(m_cur)
			res = (LPCTSTR)(m_cur->m_room);
		return res;
	}
	BOOL HasNext(void) const
	{
		return m_cur && m_cur->m_next;
	}
	BOOL HasPrevious(void) const
	{
		return m_cur && m_cur->m_previous;
	}
	void Next(void)
	{
		ATLASSERT(HasNext());
		if(HasNext())
			m_cur = m_cur->m_next;
	}
	void Previous(void)
	{
		ATLASSERT(HasPrevious());
		if(HasPrevious()){
			m_cur = m_cur->m_previous;
		}
	}
	void Purge(void)
	{
		CRoomHistoryEntry *p = NULL;

		// move to begin
		while(HasPrevious()){
			Previous();
		}
		p = m_cur;
		while(m_cur){
			p = m_cur;
			m_cur = m_cur->m_next;
			delete p;
		}
	}
};



//

struct ICommandDispatcher
{
	virtual BOOL OnCmdJoin() = 0;
	virtual BOOL OnCmdGoto() = 0;
	virtual BOOL OnCmdPm() = 0;
	virtual BOOL OnCmdAway() = 0;
	virtual BOOL OnCmdBack() = 0;
	virtual BOOL OnCmdLeave() = 0;
	virtual BOOL OnCmdThink() = 0;
	virtual BOOL OnCmdEmote() = 0;
	virtual BOOL OnCmdMimic() = 0;
	virtual BOOL OnCmdIgnore() = 0;
	virtual BOOL OnCmdFollow() = 0;
	virtual BOOL OnCmdInvite() = 0;
	virtual BOOL OnCmdKnock() = 0;
	virtual BOOL OnCmdNoKnock() = 0;
	virtual BOOL OnCmdBud() = 0;
	virtual BOOL OnCmdViewcam() = 0;
	virtual BOOL OnCmdBot() = 0;
	virtual BOOL OnCmdUnknown() = 0;
};

// things that should be in all chat views, be it ycht,ymsg,rtf,html etc
class CChatViewBase : \
	protected ICommandDispatcher ,
	public CViewBase ,
	public IUserListViewEvents,
	public IPrivateBoxEvents,
	public IQuickIgnoreViewEvents,
	public IBuddyListEvents
{
public:
	CChatViewBase(CTextStream &ts);
	virtual ~CChatViewBase();
		
	// from CViewBase
	virtual bool IsChatView(void) const
	{
		return true;
	}
	virtual LPCTSTR GetChatName(void) const = 0;
	
	// CViewBase::IInputFrameEvents
	virtual void OnColorsetChange(LPCTSTR name);
	virtual BOOL OnAutoComplete(LPCTSTR pszPartial,NO5TL::CStringArray &arr);
	//
	// called by the frame
	virtual void OnOptionsChanged(void);
	virtual void ChangeRoom(LPCTSTR room) = 0;
	virtual void OnUnignore(BOOL bAll,LPCTSTR user);
	//
	virtual void DoIgnoreUser(LPCTSTR name,bool bIgnore);
	virtual bool IsMe(LPCTSTR name) const = 0;
	virtual bool IsBuddy(LPCTSTR name);
	//
	CUserList m_users;			// users in chat and others.
	CBuddyList m_buddies;	
	CComPtr<IDispatch> m_spChat;
protected:
	enum MyTimers
	{
		TIMER_CLEANUP,	// used to cleanup the user list
		TIMER_PING,
		TIMER_KNOCK,
	};	
	CTextStream &m_ts;

	IAccountOptions *m_pao;
	CPrivateBoxList m_pms;
	NO5YAHOO::CInfoTag m_it;
	CRoomHistory m_RoomHistory;
	bool m_bKnock;
	int m_knocks;
	static IIgnoreList *m_pIgnoreList;
	static IGeneralOptions *m_pgo;
	static IVisualOptions *m_pvo;
	static CPmOptions *m_pPmOptions;
	static int m_count;
	// each account will keep different settings for bold,italic,font, etc
	// so we need to keep that data, which will be loaded on window creation
	// and saved on destruction. also this is passed to bottom frame when we get active
	CInputParams m_input_params;
	// we keep the colors so we dont have to load them every time
	CSimpleArray<NO5YAHOO::CYahooColor> m_colors;
	bool m_bMimicAll;	// if enabled, mimic all
	CComPtr<IAimlBot> m_spBot;
	CString m_BotTrigger;
	//
	BOOL DispatchCmd();
	// fills our color list based on the name in the m_input_params;
	BOOL LoadColorSet(void);
	// save some input options
	BOOL SaveInputParams(void);
	// read some input options
	BOOL LoadInputParams(void);
	BOOL LoadBot(LPCTSTR name);
	// this method either finds a pointer to an existing private box or create one 
	virtual CPrivateBox * GetPrivateBox(LPCTSTR name) = 0;
	virtual void DoMimicUser(LPCTSTR name,bool bMimic);
	virtual void DoFollowUser(LPCTSTR name,bool bFollow);
	virtual void DoAutoIgnoreUser(LPCTSTR name,bool bIgnore,LPCTSTR reason);
	virtual void DoAutoIgnoreUser(LPCTSTR name,bool bIgnore,UINT uRes);
	virtual void DisplayIgnoreMsg(LPCTSTR name,bool bIgnore);
	virtual void DisplayMimicMsg(LPCTSTR name,bool bMimic);
	virtual void DisplayFollowMsg(LPCTSTR name,bool bFollow);
	virtual void DisplayAutoIgnoreMsg(LPCTSTR name,bool bIgnore,LPCTSTR reason);
	virtual void DisplayAutoIgnoreMsg(LPCTSTR name,bool bIgnore,UINT uRes);
	virtual void OpenBrowser(LPCTSTR url,bool bAsk = true);
	// display the user name as a link with the correct color
	virtual void DisplayUserName(CUser *pUser);
	virtual void DisplayChatText(CUser *pUser,CTextStream &ts);
	virtual void DisplayEmote(CUser *pUser,CTextStream &ts);
	virtual void DisplayThink(CUser *pUser,CTextStream &ts);
	virtual void DisplayPm(LPCTSTR to,CUser *pUser,CTextStream &ts,LPCTSTR strtime);
	virtual void DisplayInvited(CUser *pUser,LPCTSTR room,LPCTSTR rmid,LPCTSTR msg);
	virtual void DisplayRoomFull(LPCTSTR alt_room,LPCTSTR room,LPCTSTR rmid);
	virtual void DisplayUserMenu(LPCTSTR user,const CPoint &pt);
	// ICommandDispatcher
	virtual BOOL OnCmdMimic();
	virtual BOOL OnCmdFollow();
	virtual BOOL OnCmdIgnore();
	virtual BOOL OnCmdKnock();
	virtual BOOL OnCmdNoKnock();
	virtual BOOL OnCmdBot();
	// IUserListViewEvents
	virtual void OnUserList_Ignored(LPCTSTR name);
	virtual void OnUserList_Profile(LPCTSTR name);
	virtual void OnUserList_Mimic(LPCTSTR name);
	virtual void OnUserList_Pm(LPCTSTR name);
	virtual void OnUserList_Follow(LPCTSTR name);
	// IPrivateBoxList
	virtual void pm_OnUserIgnored(LPCTSTR name);
	virtual void pm_OnUserMimic(LPCTSTR name);
	virtual void pm_OnUserFollow(LPCTSTR name);
	virtual void pm_OnUserProfile(LPCTSTR name);
	virtual void pm_OnDestroy(LPCTSTR name,CPrivateBox *pm);
	virtual void pm_OnTyping(CPrivateBox *pBox,BOOL bStart) {}
	// IAutoIgnoreViewEvents
	virtual void OnQuick_RemoveUser(LPCTSTR name);
	// IBuddyListEvents
	virtual void blv_OnPm(LPCTSTR to);
	virtual void blv_OnProfile(LPCTSTR name);
	virtual void blv_OnChangeView(LPCTSTR name);
	//
	virtual CUser * AddUser(LPCTSTR name) = 0;
	virtual bool TestAutoIgnore(CUser *pUser,CTextStream &ts,ULONG &uRes);
	virtual void FillQuickIgnoreView(void);
	virtual void FillUserListView(void);
	// parse room::id into room and id
	virtual void ParseRoomString(LPCTSTR s,CString &room,CString &id);
	virtual CUser * _OnChatText(LPCTSTR name,LPCTSTR msg,bool utf8);
	virtual void _OnUserEnter(CUser *pUser);
	virtual CUser * _OnUserLeave(LPCTSTR name);
	virtual CUser * _OnPrivateMsg(LPCTSTR from,LPCTSTR to,LPCTSTR msg,bool utf8,LPCTSTR t = NULL);
	virtual CUser * _OnEmote(LPCTSTR name,LPCTSTR msg,bool utf8 = false);
	virtual CUser * _OnThink(LPCTSTR name,LPCTSTR msg,bool utf8 = false);
	virtual CUser * _OnInvited(LPCTSTR from,LPCTSTR room,LPCTSTR rmid,LPCTSTR msg);
	virtual void _OnRoomFull(LPCTSTR alt_room,LPCTSTR room,LPCTSTR id);
	virtual void _OnChatJoin(LPCTSTR room,LPCTSTR rmid,LPCTSTR topic);
};

// things common to all ycht chat views and that does not need a window handle
class CYahooChatViewBase : \
	public CChatViewBase,
	public NO5YAHOO::IYahooChatEvents
{
public:
	NO5YAHOO::IYahooChat *m_pChat;
public:
	BEGIN_MSG_MAP(CYahooChatViewBase)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(NO5WM_CLOSE,OnClose)
		COMMAND_ID_HANDLER(ID_CHAT_CONNECT,OnChatConnect)
		COMMAND_ID_HANDLER(ID_CHAT_LEAVE,OnChatLeave)
		COMMAND_ID_HANDLER(ID_CHAT_VOICE,OnChatVoice)
		COMMAND_ID_HANDLER(ID_ROOM_PREVIOUS,OnRoomPrevious)
		COMMAND_ID_HANDLER(ID_ROOM_NEXT,OnRoomNext)
	END_MSG_MAP()

	// message handlers
	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnClose(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnChatConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnChatLeave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnChatVoice(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnRoomPrevious(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnRoomNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);

	// from CChatViewBase - public section
	virtual void ChangeRoom(LPCTSTR room);
	virtual bool IsYchtView(void) const
	{
		return true;
	}
	virtual bool IsMe(LPCTSTR name) const
	{
		return !lstrcmpi(name,m_pChat->GetName());
	}
	virtual LPCTSTR GetChatName(void) const
	{
		LPCTSTR p = NULL;
		if(m_pChat)
			p = m_pChat->GetName();
		return p;
	}
	//

protected:
	//
	CYahooChatViewBase(CTextStream &ts);
	virtual ~CYahooChatViewBase();

	// overide from base class CViewBase
	virtual void OnTextInput(LPCTSTR text);
	virtual BOOL OnCmdJoin();
	virtual BOOL OnCmdGoto();
	virtual BOOL OnCmdPm();
	virtual BOOL OnCmdAway();
	virtual BOOL OnCmdBack();
	virtual BOOL OnCmdLeave();
	virtual BOOL OnCmdThink();
	virtual BOOL OnCmdEmote();
	virtual BOOL OnCmdInvite();
	virtual BOOL OnCmdBud();
	virtual BOOL OnCmdViewcam();
	virtual BOOL OnCmdUnknown();
	

	// IYahooChatEvents
	virtual void OnSignIn(LPCSTR cookie);
	virtual void OnLogin(LPCSTR name,LPCSTR badwords);
	virtual void OnLogoff(int status,LPCSTR msg);
	virtual void OnRoomIn(LPCSTR room,LPCSTR slogan,NO5YAHOO::CYahooChatUser &user);
	virtual void OnRoomOut(LPCSTR room,LPCSTR name);
	virtual void OnJoinRoom(LPCSTR room,LPCSTR slogan,LPCSTR cookies,
		NO5YAHOO::CYahooChatUsers &users,LPCSTR wc_cookie);
	virtual void OnRoomFull(LPCSTR msg,LPCSTR alt_room);
	virtual void OnJoinFailed(unsigned short status,LPCSTR msg);
	virtual void OnText(LPCSTR room,LPCSTR name,LPCSTR msg,
		LPCSTR extra);
	virtual void OnInvited(LPCSTR msg,LPCSTR room,LPCSTR name);
	virtual void OnAway(LPCSTR name,LPCSTR room,LPCSTR number,
		LPCSTR msg);
	virtual void OnThinking(LPCSTR room,LPCSTR name,LPCSTR msg);
	virtual void OnEmotion(LPCSTR room,LPCSTR name,
		LPCSTR emote,LPCSTR extra);
	virtual void OnPm(LPCSTR from,LPCSTR to,LPCSTR msg);
	virtual void OnBuddy(LPCSTR msg);
	virtual void OnDice(LPCSTR name,LPCSTR msg);
	virtual void OnAdvertise(LPCSTR msg);
	virtual void OnServerMsg(LPCSTR msg,unsigned long packet,
		unsigned long status);
	virtual void OnTime(LPCSTR msg);
	virtual void OnHelp(LPCSTR msg);

	// ISocketEvents
	virtual void OnSockConnect(int iError);
	virtual void OnSockConnecting(void);
	virtual void OnSockClose(int iError);
	virtual void OnSockConnectTimeout(void);
	virtual void OnSockError(int error);
	virtual void OnSockResolvingAddress(void);
	virtual void OnSockAddressResolved(int error);

	// IBuddyListEvents
	virtual void blv_OnRemoveBuddy(LPCTSTR name,LPCTSTR group);
	virtual void blv_OnRenameGroup(LPCTSTR name);
	virtual void blv_OnAddBuddy(LPCTSTR group);
	virtual void blv_OnViewCam(LPCTSTR name);
	virtual void blv_OnCamInvite(LPCTSTR name);
	virtual void blv_OnGotoUser(LPCTSTR name);
	virtual void blv_OnInviteUser(LPCTSTR name);
	// IUserListViewEvents
	virtual void OnUserList_Cam(LPCTSTR name);
	virtual void OnUserList_CamInvite(LPCTSTR name);
	virtual void OnUserList_AddBuddy(LPCTSTR name);
	virtual void OnUserList_RemoveBuddy(LPCTSTR name);

	virtual CUser * AddUser(NO5YAHOO::CYahooChatUser &user);
	virtual CUser * AddUser(LPCTSTR name);
	void AddUsers(NO5YAHOO::CYahooChatUsers &users);
	virtual void pm_OnSend(CPrivateBox *pBox,LPCTSTR msg);

};

// things that should be in all ycht chat views and depend on window handle
template <class T >
class CYahooChatViewImpl : \
	public CWindowImpl<T>,
	public CYahooChatViewBase
{
	typedef CYahooChatViewImpl<T> _ThisClass;
	typedef CYahooChatViewBase _BaseClass;

public:
	CYahooChatViewImpl(CTextStream &ts);
	virtual ~CYahooChatViewImpl(){};

	BEGIN_MSG_MAP(CYahooChatViewImpl)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		MESSAGE_HANDLER(WM_TIMER,OnTimer)
		MESSAGE_HANDLER(NO5WM_MDIACTIVATE,OnMdiActivate)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	// msg handlers
	LRESULT OnMdiActivate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnTimer(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);

protected:
	// overide from CYahooChatViewBase
	virtual void OnJoinRoom(LPCSTR room,LPCSTR slogan,LPCSTR cookies,
		NO5YAHOO::CYahooChatUsers &users,LPCSTR wc_cookie);
	virtual bool IsHandleValid(void);
	virtual HWND GetHandle(void)
	{
		return m_hWnd;
	}
	virtual CPrivateBox * GetPrivateBox(LPCTSTR name);
};

class CYahooChatRichView : public CYahooChatViewImpl<CYahooChatRichView>
{
	typedef CYahooChatViewImpl<CYahooChatRichView>  _BaseClass;
public:
	CYahooChatRichView(void);
	virtual ~CYahooChatRichView();

	BEGIN_MSG_MAP(CYahooChatRichView)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		COMMAND_CODE_HANDLER(EN_ERRSPACE,OnErrSpace)
		COMMAND_CODE_HANDLER(EN_MAXTEXT,OnMaxText)
		COMMAND_CODE_HANDLER(EN_VSCROLL,OnVScroll)
		NOTIFY_CODE_HANDLER(EN_LINK,OnEditLink)
		NOTIFY_CODE_HANDLER(EN_MSGFILTER,OnEditMsgFilter)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	// messange handlers
	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnSize(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnEditMsgFilter(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnEditLink(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnErrSpace(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnMaxText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnVScroll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	// overides from CViewBase
	virtual ViewType GetViewType(void) const
	{
		return VIEW_CHAT_YCHT_RTF;
	}
	// called by the frame
	virtual void OnOptionsChanged(void);

private:
	NO5TL::CNo5RichEdit *m_pEdit;
	CRichEditStream m_ts;
};

class IDocHostUIHandlerImpl : \
	public IDispatchImpl<IDocHostUIHandlerDispatch, &IID_IDocHostUIHandlerDispatch, &LIBID_ATLLib>
{
public:
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
	// IDocHostUIHandlerDispatch

	STDMETHOD(ShowContextMenu)( 
            /* [in] */ DWORD dwID,
            /* [in] */ DWORD x,
            /* [in] */ DWORD y,
            /* [in] */ IUnknown *pcmdtReserved,
            /* [in] */ IDispatch  *pdispReserved,
            /* [retval][out] */ HRESULT  *dwRetVal)
	{
		return S_FALSE;
	}

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
};

class CMyDocHostUIHandler : public IDocHostUIHandlerImpl
{
public:
	CMyDocHostUIHandler(IDispatch *pDisp)
	{
		m_pDisp = pDisp;
	}
	STDMETHOD(ShowContextMenu)( 
            /* [in] */ DWORD dwID,
            /* [in] */ DWORD x,
            /* [in] */ DWORD y,
            /* [in] */ IUnknown *pcmdtReserved,
            /* [in] */ IDispatch  *pdispReserved,
            /* [retval][out] */ HRESULT  *dwRetVal);
	STDMETHOD(GetExternal)( 
           /* [out] */ IDispatch  **ppDispatch)
	{
		*ppDispatch = NULL;
		HRESULT hr = S_FALSE;

		if(m_pDisp){
			*ppDispatch = m_pDisp;
			hr = S_OK;
		}
		return hr;
	}
private:
	IDispatch *m_pDisp;
};

class CYahooChatHtmlView : \
	public CYahooChatViewImpl<CYahooChatHtmlView>,
	public IDispEventImpl<1,CYahooChatHtmlView,&DIID_HTMLDocumentEvents,
		&LIBID_MSHTML,MSHTML_LIBVER_MAJ,MSHTML_LIBVER_MIN>,
	public IDispEventImpl<2,CYahooChatHtmlView,&DIID_DWebBrowserEvents2,&LIBID_SHDocVw,
		WBC_LIBVER_MAJ,WBC_LIBVER_MIN>,
	public IDispDynImpl<CYahooChatHtmlView>
{
	typedef IDispEventImpl<1,CYahooChatHtmlView,&DIID_HTMLDocumentEvents,
		&LIBID_MSHTML,MSHTML_LIBVER_MAJ,MSHTML_LIBVER_MIN> DispEventHtml;
	typedef IDispEventImpl<2,CYahooChatHtmlView,&DIID_DWebBrowserEvents2,&LIBID_SHDocVw,
		WBC_LIBVER_MAJ,WBC_LIBVER_MIN> DispEventBrowser;
	typedef CYahooChatViewImpl<CYahooChatHtmlView>  _BaseClass;
public:
	CYahooChatHtmlView(void);
	virtual ~CYahooChatHtmlView();


	BEGIN_MSG_MAP(CYahooChatHtmlView)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	BEGIN_SINK_MAP(CYahooChatHtmlView)
		//SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONCLICK,OnClick)
		//SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONDBLCLICK,OnDblClk)
		//SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEOUT,OnMouseOut)
		//SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEOVER,OnMouseOver)
		// web browser
		SINK_ENTRY_EX(2,DIID_DWebBrowserEvents2,DISPID_DOCUMENTCOMPLETE,OnDocumentComplete)
	END_SINK_MAP()

	BEGIN_DISPATCH_MAP(CYahooChatHtmlView)
		DISP_METHOD1_ID(OnDivClick,3,VT_EMPTY,VT_BSTR)
		DISP_METHOD1_ID(OnDivDblClk,4,VT_EMPTY,VT_BSTR)
		DISP_METHOD1_ID(OnDivRClick,5,VT_EMPTY,VT_BSTR)
		DISP_METHOD1_ID(OnUserClick,6,VT_EMPTY,VT_BSTR)
		DISP_METHOD1_ID(OnUserDblClk,7,VT_EMPTY,VT_BSTR)
		DISP_METHOD_ID(OnUserRClick,8,VT_EMPTY,3,VTS_BSTR VTS_I4 VTS_I4)
		DISP_METHOD1_ID(OnLinkClick,9,VT_EMPTY,VT_BSTR)
		DISP_METHOD1_ID(OnAnswerPm,10,VT_EMPTY,VT_BSTR)
	END_DISPATCH_MAP()

	// message handlers
	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnSize(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	
	// DWebBrowserEvents2
	virtual void __stdcall OnDocumentComplete(IDispatch *pDisp,VARIANT *pUrl);
	// IDispatch
	void __stdcall OnDivClick(BSTR s);
	void __stdcall OnDivDblClk(BSTR s);
	void __stdcall OnDivRClick(BSTR s);
	void __stdcall OnUserClick(BSTR s);
	void __stdcall OnUserDblClk(BSTR s);
	void __stdcall OnUserRClick(BSTR s,long x,long y);
	void __stdcall OnLinkClick(BSTR s);
	void __stdcall OnAnswerPm(BSTR s);

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


	// overides from CViewBase
	virtual ViewType GetViewType(void) const
	{
		return VIEW_CHAT_YCHT_HTML;
	}
	// from CChatViewBase
	void DisplayUserName(CUser *pUser);
	virtual void DisplayChatText(CUser *pUser,CTextStream &ts);
	virtual void DisplayEmote(CUser *pUser,CTextStream &ts);
	virtual void DisplayThink(CUser *pUser,CTextStream &ts);
	virtual void DisplayPm(LPCTSTR to,CUser *pUser,CTextStream &ts,LPCTSTR strtime);
	//virtual void DisplayInvited(CUser *pUser,LPCTSTR room,LPCTSTR rmid,LPCTSTR msg);
	//virtual void DisplayRoomFull(LPCTSTR alt_room,LPCTSTR room,LPCTSTR rmid);
	// called by the frame
	virtual void OnOptionsChanged(void);
private:
	CHtmlEditStream m_ts;
	CHtmlCtrl m_edit;
	CMyDocHostUIHandler *m_pUIHandler;
	void UpdateVisualOptions(void);
};

class CYahooChatEditView : public CYahooChatViewImpl<CYahooChatEditView>
{
	typedef CYahooChatViewImpl<CYahooChatEditView>  _BaseClass;
public:
	CYahooChatEditView(void);
	virtual ~CYahooChatEditView();


	BEGIN_MSG_MAP(CYahooChatEditView)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()



	// message handlers
	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnSize(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);

	// overides from CViewBase
	virtual ViewType GetViewType(void) const
	{
		return VIEW_CHAT_YCHT_TEXT;
	}
	// from CChatViewBase
	void DisplayUserName(CUser *pUser);
	virtual void DisplayChatText(CUser *pUser,CTextStream &ts);
private:
	CEdit m_edit;
	CEditCtlStream m_ts;
};

struct CViewTextParams
{
	LPCSTR room;
	LPCSTR slogan;
	LPCSTR name;
};

HRESULT CreateUserObj(CUser *pUser,IDispatch **ppObj);
HRESULT CreateChatObj(CChatViewBase *pView,IDispatch **ppObj);


#endif // !defined(AFX_YAHOOCHATVIEW_H__8C41EEA1_2D6C_11DA_A17D_000103DD18CD__INCLUDED_)
