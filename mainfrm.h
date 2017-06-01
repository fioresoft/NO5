// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__F762896B_2C65_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_MAINFRM_H__F762896B_2C65_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef NO5TL_USE_MARQUEEWND
#include <no5tl\marqueewnd.h>
#else
#include <no5tl\marqueemm.h>
#endif

#include "rightframe.h"
#include "imainframe.h"
#include "bottomframe.h"
#include "coloreditor.h"

// fwd declarations
class COptionsPropertySheet;
class CRoomDlg;
//


typedef CDotNetTabCtrl<CTabViewTabItem> MyTabCtrl;
typedef CTabbedMDIClient<MyTabCtrl> MyMDIClient;

class CMainFrame :	public CTabbedMDIFrameWindowImpl<CMainFrame,MyMDIClient>,
					public CUpdateUI<CMainFrame>,
					public CMessageFilter,
					public CIdleHandler,
					public IMainFrame
{
	typedef CTabbedMDIFrameWindowImpl<CMainFrame,MyMDIClient> _BaseClass;
	enum Timers
	{
		ID_TIMER_BASE = 500,
	};
public:
	CMainFrame();
	virtual ~CMainFrame();

	DECLARE_FRAME_WND_CLASS("NO5CLS_FRAME", IDR_MAINFRAME)

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_MARQUEE, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNCDestroy)
		MESSAGE_HANDLER(WM_TIMER,OnTimer)
		MESSAGE_HANDLER(NO5WM_COLORDLG,OnColorDlgClose)
		MESSAGE_HANDLER(NO5WM_CLRDLGDEL,OnClrDlgDel)
		MESSAGE_HANDLER(NO5WM_OPTIONSDLG,OnOptionsDlgClose)
		MESSAGE_HANDLER(NO5WM_ROOMDLG,OnRoomDlgClose)
		MESSAGE_HANDLER(NO5WM_UNIGNORE,OnUnignore)
		MESSAGE_HANDLER(NO5WM_CAMDLGDEL,OnCamDlgDel)
		MESSAGE_HANDLER(NO5WM_VOICEDLGDEL,OnVoiceDlgDel)
		MESSAGE_HANDLER(NO5WM_SCRIPTWND,OnScriptWnd)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_FORM_NEW, OnFormNew)
		COMMAND_ID_HANDLER(ID_SCRIPT_NEW, OnScriptNew)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_VIEW_MARQUEE, OnViewMarquee)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_WINDOW_CASCADE, OnWindowCascade)
		COMMAND_ID_HANDLER(ID_WINDOW_TILE_HORZ, OnWindowTile)
		COMMAND_ID_HANDLER(ID_WINDOW_ARRANGE, OnWindowArrangeIcons)
		COMMAND_ID_HANDLER(ID_EDIT_COLORS, OnEditColors)
		COMMAND_ID_HANDLER(ID_FILE_OPTIONS, OnOptions)
		COMMAND_ID_HANDLER(ID_ROOM_SELECT,OnRoomSelect)
		COMMAND_ID_HANDLER(ID_HELP_HOMEPAGE,OnHelpHomepage)
		COMMAND_HANDLER(IDC_MARQUEE1,MQN_CHANGED,OnMarqueeChange)
		CHAIN_MDI_CHILD_COMMANDS()
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()


// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,  BOOL& bHandled);
	LRESULT OnNCDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,  BOOL& bHandled);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam,  BOOL& bHandled);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnColorDlgClose(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnClrDlgDel(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnOptionsDlgClose(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnRoomDlgClose(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnUnignore(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnCamDlgDel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnScriptWnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnVoiceDlgDel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewMarquee(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnHelpHomepage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnScriptNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFormNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMarqueeChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnWindowCascade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowTile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowArrangeIcons(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditColors(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRoomSelect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	// IMainFrame
	virtual IUserListView * UserListView(void)
	{
		return m_right.GetUserListView();
	}
	virtual IQuickIgnoreView * QuickIgnoreView(void)
	{
		return m_right.GetQuickIgnoreView();
	}
	virtual IBuddyListView * BuddyListView(void)
	{
		return m_right.GetBuddyListView();
	}
	virtual void ActiveView(CViewBase *pView);
	virtual CViewBase * ActiveView(void);
	virtual IInputFrame * InputFrame(void);
	virtual HWND GetHandle(void);
	virtual void AddView(CViewBase *pView);
	virtual void RemoveView(CViewBase *pView);
	virtual CToolBarCtrl & GetChatToolbar(void)
	{
		return m_tbChat;
	}
	virtual void AddMarqueeString(LPCTSTR s);
	virtual void GetViews(CSimpleValArray<CViewBase*> &views);
	virtual void ActivateChatView(LPCTSTR name);
	virtual ULONG CreateTimer(ULONG nInterval);
	virtual void DestroyTimer(ULONG nID);
	virtual void OpenBrowser(LPCTSTR url,bool bAsk);
	
private:
	CHorSplitterWindow m_split1;
	CSplitterWindow m_split2;
	CBottomFrame m_bottom;
	CRightFrame m_right;
	CViewBase *m_pView;
	CTabbedMDICommandBarCtrl m_CmdBar;
	COptionsPropertySheet *m_pOptionsDlg;
	CSimpleValArray<CViewBase *> m_views;
	CRoomDlg *m_pRoomDlg;
	CToolBarCtrl m_tbFrame;
	CToolBarCtrl m_tbChat;
	CReBarCtrl m_rebar;
	CSimpleValArray<ULONG> m_timers;	// timers created from script
#ifdef NO5TL_USE_MARQUEEWND
	NO5TL::CMarqueeWnd m_marquee;
#else
	NO5TL::CMarqueeMM m_marquee;
#endif

	void ShowFrameBand(BOOL bShow);
	void ShowChatBand(BOOL bShow);
	void ShowMarqueeBand(BOOL bShow);
	void CreateMarquee(void);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__F762896B_2C65_11DA_A17D_000103DD18CD__INCLUDED_)
