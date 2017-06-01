// YahooMsgView.h: interface for the CYahooMsgView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YAHOOMSGVIEW_H__6EDE6A0D_6B99_4F72_8A35_FF04094B8E3A__INCLUDED_)
#define AFX_YAHOOMSGVIEW_H__6EDE6A0D_6B99_4F72_8A35_FF04094B8E3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "yahoochatview.h"
#include <yahoo\yahoomessenger.h>
#include <no5tl\atldispa.h>

// things common to all ymsg chat views and that does not need a window handle
class CYahooMsgViewBase : \
	public CChatViewBase,
	public NO5YAHOO::IYahooMessengerEvents
{
public:
	NO5YAHOO::IYahooMessenger *m_pChat;
public:
	BEGIN_MSG_MAP(CYahooMsgViewBase)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(NO5WM_CLOSE,OnClose)
		COMMAND_ID_HANDLER(ID_CHAT_CONNECT,OnChatConnect)
		COMMAND_ID_HANDLER(ID_CHAT_LEAVE,_OnChatLeave)
		COMMAND_ID_HANDLER(ID_ROOM_PREVIOUS,OnRoomPrevious)
		COMMAND_ID_HANDLER(ID_ROOM_NEXT,OnRoomNext)
		COMMAND_ID_HANDLER(ID_CHAT_CAM,OnChatCam)
		COMMAND_ID_HANDLER(ID_CHAT_VOICE,OnChatVoice)
#ifdef _DEBUG
		/*
		COMMAND_ID_HANDLER(ID_DEBUG_AUTHENTICATE,OnDebugAuthenticate)
		COMMAND_ID_HANDLER(ID_DEBUG_AUTHRESP,OnDebugAuthResp)
		COMMAND_ID_HANDLER(ID_DEBUG_CHATONLINE,OnDebugChatOnline)
		COMMAND_ID_HANDLER(ID_DEBUG_JOINROOM,OnDebugJoinRoom)
		COMMAND_ID_HANDLER(ID_DEBUG_LOGOFF,OnDebugLogoff)
		COMMAND_ID_HANDLER(ID_DEBUG_CHATLOGOFF,OnDebugChatLogoff)
		COMMAND_ID_HANDLER(ID_DEBUG_CHATLEAVE,OnDebugChatLeave)
		*/
#endif
	END_MSG_MAP()

	// message handlers
	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnClose(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnChatConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT _OnChatLeave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnRoomPrevious(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnRoomNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnChatCam(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnChatVoice(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
#ifdef _DEBUG
	LRESULT OnDebugAuthenticate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnDebugAuthResp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnDebugChatOnline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnDebugJoinRoom(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnDebugLogoff(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnDebugChatLogoff(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnDebugChatLeave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
#endif

	// from CChatViewBase - public section
	virtual void ChangeRoom(LPCTSTR room);
	virtual bool IsYchtView(void) const
	{
		return false;
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
	CYahooMsgViewBase(CTextStream &ts);
	virtual ~CYahooMsgViewBase();

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
	

	// IYahooMessengerEvents
	virtual void OnUserLogon(u_long status,CSimpleArray<NO5YAHOO::CBuddyInfo> &buddies,bool bMe);
	virtual void OnUserLogoff(u_long status,CSimpleArray<NO5YAHOO::CBuddyInfo> &buddies);
	virtual void OnAuthenticate(u_long status,LPCSTR challenge,
		bool ver11);
	virtual void OnAuthRespError(void);
	virtual void OnChatOnline(void);
	virtual void OnChatJoin(LPCSTR room,LPCSTR topic);
	virtual void OnText(LPCSTR user,LPCSTR msg,bool utf8);
	virtual void OnEmote(LPCSTR user,LPCSTR msg,bool utf8);
	virtual void OnThink(LPCSTR user,LPCSTR msg,bool utf8);
	virtual void OnUserLeaves(NO5YAHOO::CYahooMsgUser &user);
	virtual void OnUserEnters(NO5YAHOO::CYahooMsgUser &user);
	virtual void OnInstantMessage(LPCSTR from,LPCSTR to,LPCSTR msg,
		bool bUTF8,LPCSTR im_env,LPCSTR time);
	virtual void OnPrivateMessage(LPCSTR from,LPCSTR to,LPCSTR msg,bool utf8);
	virtual void OnTyping(LPCTSTR from,bool bStart);
	virtual void OnWebcamInvite(LPCTSTR from);
	virtual void OnWebcamInviteReply(LPCTSTR from,bool bAccepted);
	virtual void OnNotify(LPCSTR from,LPCSTR notify_string,LPCSTR status);
	virtual void OnAddBuddy(LPCSTR me,LPCSTR group,
		const NO5YAHOO::CBuddyInfo &bi);
	virtual void OnRemBuddy(LPCTSTR me,LPCTSTR buddy,LPCTSTR group);
	virtual void OnRejectContact(u_long status,LPCTSTR me,LPCTSTR buddy,LPCTSTR msg);
	virtual void OnGotGroupRename(LPCTSTR old,LPCTSTR group);
	virtual void OnGroupRename(LPCTSTR me,LPCTSTR old,LPCTSTR group);
	virtual void OnIgnoreUser(LPCSTR who,LPCSTR me,bool ignore,LPCSTR answer);
	virtual void OnBuddyList(LPCTSTR group,CSimpleArray<CString> &buddies);
	virtual void OnIgnoreList(CSimpleArray<CString> &lst);
	virtual void OnAlternateIdList(CSimpleArray<CString> &lst);
	virtual void OnChatLogout(void);
	virtual void OnChatInvite2(LPCSTR room,LPCSTR rmspace,LPCSTR \
		from,LPCSTR to,LPCSTR msg);
	virtual void OnUserAway(LPCSTR user,int code,LPCSTR msg);
	virtual void OnUserBack(LPCSTR user,int code,LPCSTR msg);
	virtual void OnAuthError(LPCSTR msg,LPCSTR url);
	virtual void OnYahooError(u_long service,u_long status,
		NO5YAHOO::CYahooMap &data);
	virtual void OnPing(u_long status);
	virtual void OnNewContact(u_long status,LPCTSTR to,LPCTSTR buddy,LPCTSTR msg);
	virtual void OnNewMail(int count);
	virtual void OnUserList(NO5YAHOO::CYahooMsgUsers &users,bool bMore);
	// unknown
	virtual void OnChatLogon(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnChatLogoff(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnSysMessage(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnIdle(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnIdActivate(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnIdDeactivate(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnMailStat(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnUserStat(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnChatInvite(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnCalendar(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnNewPersonalMail(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnAddIdent(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnAddIgnore(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnPassThrough(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnGameLogon(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnGameLogoff(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnGameMsg(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnFileTransfer(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnVoiceChat(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnVerify(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnP2PFileXfer(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnPeer2Peer(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnChatGoto(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnChatLeave(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnChatPing(u_long status,NO5YAHOO::CYahooMap &map);
	virtual void OnAcceptReq(u_long status,NO5YAHOO::CYahooMap &map);
	//
	virtual void OnUnknownService(u_short service,u_long status,
		NO5YAHOO::CYahooMap &data);

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
	//
	virtual CUser * AddUser(NO5YAHOO::CYahooMsgUser &user);
	virtual CUser * AddUser(LPCTSTR name);
	void AddUsers(NO5YAHOO::CYahooMsgUsers &users);
	virtual void pm_OnSend(CPrivateBox *pBox,LPCTSTR msg);
	virtual void pm_OnTyping(CPrivateBox *pBox,BOOL bStart);
	void DisplayUnknownCallback(void);
	void CopyUserInfo(CUser *pUser,NO5YAHOO::CYahooMsgUser &from);
	// IUserListViewEvents
	virtual void OnUserList_Cam(LPCTSTR name);
	virtual void OnUserList_CamInvite(LPCTSTR name);
	virtual void OnUserList_AddBuddy(LPCTSTR name);
	virtual void OnUserList_RemoveBuddy(LPCTSTR name);
	//
	CString m_sCamTarget;
private:
};

// things that should be in all ycht chat views and depend on window handle
template <class T>
class CYahooMsgViewImpl : \
	public CWindowImpl<T>,
	public CYahooMsgViewBase
{
	typedef CYahooMsgViewImpl<T> _ThisClass;
	typedef CYahooMsgViewBase _BaseClass;

	enum MyTimers
	{
		TIMER_CLEANUP,	// used to cleanup the user list
		TIMER_PING,
	};

public:
	CYahooMsgViewImpl(CTextStream &ts);
	virtual ~CYahooMsgViewImpl(){};

	BEGIN_MSG_MAP(CYahooMsgViewImpl)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		MESSAGE_HANDLER(WM_TIMER,OnTimer)
		MESSAGE_HANDLER(NO5WM_MDIACTIVATE,OnMdiActivate)
		MESSAGE_HANDLER(NO5WM_CAMDLGDEL,OnCamDlgDel)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	// msg handlers
	LRESULT OnMdiActivate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnTimer(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnCamDlgDel(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);

protected:
	// overide from CYahooMsgViewBase
	virtual bool IsHandleValid(void)
	{
		return IsWindow() ? true : false;
	}
	virtual CPrivateBox * GetPrivateBox(LPCTSTR name);
	virtual HWND GetHandle(void)
	{
		return m_hWnd;
	}
	virtual void OnWebcam(LPCTSTR cookie,LPCTSTR server);
};



class CYahooMsgRichView : public CYahooMsgViewImpl<CYahooMsgRichView>
{
	typedef CYahooMsgViewImpl<CYahooMsgRichView>  _BaseClass;
public:
	CYahooMsgRichView(void);
	virtual ~CYahooMsgRichView();

	BEGIN_MSG_MAP(CYahooMsgRichView)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		COMMAND_CODE_HANDLER(EN_ERRSPACE,OnErrSpace)
		COMMAND_CODE_HANDLER(EN_MAXTEXT,OnMaxText)
		COMMAND_CODE_HANDLER(EN_VSCROLL,OnVScroll)
		COMMAND_ID_HANDLER(ID_FILE_SAVE_AS,OnFileSaveAs)
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
	LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	// overides from CViewBase
	virtual ViewType GetViewType(void) const
	{
		return VIEW_CHAT_YMSG_RTF;
	}
	// called by the frame
	virtual void OnOptionsChanged(void);

private:
	NO5TL::CNo5RichEdit *m_pEdit;
	CRichEditStream m_ts;
};


class CYahooMsgHtmlView : \
	public CYahooMsgViewImpl<CYahooMsgHtmlView>,
//	public IDocHostUIHandler,
	public IDispEventImpl<1,CYahooMsgHtmlView,&DIID_HTMLDocumentEvents,
		&LIBID_MSHTML,MSHTML_LIBVER_MAJ,MSHTML_LIBVER_MIN>,
	public IDispEventImpl<2,CYahooMsgHtmlView,&DIID_DWebBrowserEvents2,&LIBID_SHDocVw,
		WBC_LIBVER_MAJ,WBC_LIBVER_MIN>,
	public IDispDynImpl<CYahooMsgHtmlView>
{
	typedef IDispEventImpl<1,CYahooMsgHtmlView,&DIID_HTMLDocumentEvents,
		&LIBID_MSHTML,MSHTML_LIBVER_MAJ,MSHTML_LIBVER_MIN> DispEventHtml;
	typedef IDispEventImpl<2,CYahooMsgHtmlView,&DIID_DWebBrowserEvents2,&LIBID_SHDocVw,
		WBC_LIBVER_MAJ,WBC_LIBVER_MIN> DispEventBrowser;
	typedef CYahooMsgViewImpl<CYahooMsgHtmlView> _BaseClass;
public:
	CYahooMsgHtmlView(void);
	virtual ~CYahooMsgHtmlView();
	

	BEGIN_MSG_MAP(CYahooMsgHtmlView)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		COMMAND_ID_HANDLER(ID_EDIT_CLEAR, OnEditClear)
		COMMAND_ID_HANDLER(ID_EDIT_CLEAR_ALL, OnEditClearAll)
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
		COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCut)
		COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditPaste)
		COMMAND_ID_HANDLER(ID_EDIT_SELECT_ALL, OnEditSelectAll)
		COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnEditUndo)
		COMMAND_ID_HANDLER(ID_FILE_SAVE_AS,OnFileSaveAs)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	BEGIN_SINK_MAP(CYahooMsgHtmlView)
	//	SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONCLICK,OnClick)
	/*
		SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONDBLCLICK,OnDblClk)
		SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEOUT,OnMouseOut)
		SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEOVER,OnMouseOver)
	*/
		// web browser
		SINK_ENTRY_EX(2,DIID_DWebBrowserEvents2,DISPID_DOCUMENTCOMPLETE,OnDocumentComplete)
	END_SINK_MAP()

	BEGIN_DISPATCH_MAP(CYahooMsgHtmlView)
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
	LRESULT OnEditClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_edit.m_doc.Clear();
		return 0;
	}
	LRESULT OnEditClearAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0;
	}
	LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_edit.m_doc.Copy();
		return 0;
	}
	LRESULT OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_edit.m_doc.Cut();
		return 0;
	}
	LRESULT OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_edit.m_doc.Paste();
		return 0;
	}
	LRESULT OnEditSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0;
	}
	LRESULT OnEditUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0;
	}
	LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);


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
		return VIEW_CHAT_YMSG_HTML;
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

class CYahooMsgEditView : public CYahooMsgViewImpl<CYahooMsgEditView>
{
	typedef CYahooMsgViewImpl<CYahooMsgEditView>  _BaseClass;
public:
	CYahooMsgEditView(void);
	virtual ~CYahooMsgEditView();


	BEGIN_MSG_MAP(CYahooMsgEditView)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		COMMAND_ID_HANDLER(ID_FILE_SAVE_AS,OnFileSaveAs)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	// message handlers
	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnSize(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);

	// overides from CViewBase
	virtual ViewType GetViewType(void) const
	{
		return VIEW_CHAT_YMSG_TEXT;
	}
	// from CChatViewBase
	void DisplayUserName(CUser *pUser);
	virtual void DisplayChatText(CUser *pUser,CTextStream &ts);
private:
	CEdit m_edit;
	CEditCtlStream m_ts;
};

#endif // !defined(AFX_YAHOOMSGVIEW_H__6EDE6A0D_6B99_4F72_8A35_FF04094B8E3A__INCLUDED_)
