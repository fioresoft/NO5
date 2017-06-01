// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "aboutdlg.h"
#include "yahoochatview.h"
#include "yahoomsgview.h"
#include "ChildFrm.h"
#include "coloreditor.h"
#include "optionsdialog.h"
#include "MainFrm.h"
#include "logindlg.h"
#include "no5app.h"
#include "no5options.h"
#include "roomdlg.h"
#include "scripteditframe.h"
#include "camviewdlg.h"
#include "formframe.h"
#include "voicedialog.h"

static BOOL CALLBACK EnumChildProc(HWND hWnd,LPARAM lParam);

CMainFrame::CMainFrame():m_bottom(this)
{
	m_pView = NULL;
	m_pOptionsDlg = NULL;
	m_pRoomDlg = NULL;
}

CMainFrame::~CMainFrame()
{
	if(m_pOptionsDlg)
		delete m_pOptionsDlg;
	if(m_pRoomDlg)
		delete m_pRoomDlg;
	ATLASSERT(m_views.GetSize() == 0);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CMDIFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	if(m_pOptionsDlg && m_pOptionsDlg->IsWindow() && m_pOptionsDlg->IsDialogMessage(pMsg))
		return TRUE;

	HWND hWnd = MDIGetActive();
	if(hWnd != NULL)
		return (BOOL)::SendMessage(hWnd, WM_FORWARDMSG, 0, (LPARAM)pMsg);

	return FALSE;
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	m_tbFrame = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE,
		ATL_SIMPLE_TOOLBAR_PANE_STYLE,IDC_TOOLBAR_FRAME);

	m_tbChat = CreateSimpleToolBarCtrl(m_hWnd, IDR_CHATFRAME, FALSE,
		ATL_SIMPLE_TOOLBAR_PANE_STYLE,IDC_TOOLBAR_CHAT);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	m_rebar = m_hWndToolBar;

	// add rebar bands
		// cmd bar
	AddSimpleReBarBandCtrl(m_rebar,hWndCmdBar,IDC_BAND_CMDBAR,
		NULL,	// title
		FALSE,	// new row
		0,		// cx
		TRUE);	// full width


		// frame toolbar
	AddSimpleReBarBandCtrl(m_rebar,m_tbFrame, IDC_BAND_FRAME,
		NULL,	// title
		TRUE,	// new row
		0,		// cx
		TRUE);	// full width always

	// chat toolbar
	AddSimpleReBarBandCtrl(m_rebar,m_tbChat, IDC_BAND_CHAT,
		NULL,	// title
		FALSE,	// new row
		0,		// cx
		TRUE);	// full width always
		
	// marquee
	CreateMarquee();

	// chat toolbar
	AddSimpleReBarBandCtrl(m_rebar,m_marquee, IDC_BAND_MARQUEE,
		NULL,	// title
		TRUE,	// new row
		0,		// cx
		TRUE);	// full width always


	CreateSimpleStatusBar();

	CreateMDIClient();
	m_CmdBar.SetMDIClient(m_hWndMDIClient);

	UIAddToolBar(m_tbFrame);
	UIAddToolBar(m_tbChat);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);
	UISetCheck(ID_VIEW_MARQUEE, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);


	// create horizontal splitter
	//m_split1.SetSplitterExtendedStyle(0,SPLIT_PROPORTIONAL);
	m_split1.Create(m_hWnd,rcDefault);
	// create panes of spliter1
	m_split2.Create(m_split1,rcDefault);
	m_bottom.Create(m_split1,rcDefault,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		WS_EX_CLIENTEDGE);
	// create panes of splitter2
	m_right.Create(m_split2,rcDefault,NULL);

	CRect r;
	m_hWndClient = m_split1;
	UpdateLayout(FALSE);
	m_split1.SetSplitterPanes(m_split2,m_bottom);
	m_split1.GetClientRect(&r);
	m_split1.SetSplitterPos(r.Height() - m_bottom.GetDesiredHeight()  );
	m_split2.SetSplitterPanes(m_hWndMDIClient,m_right);
	m_split1.GetSplitterPaneRect(SPLIT_PANE_TOP,&r);
	m_split2.SetSplitterPos(3 * r.Width() / 4 );

	g_app.SetMainFrame(this);
	if(g_app.GetGeneralOptions()->GetTopmost())
		ModifyStyleEx(0,WS_EX_TOPMOST);


	ShowChatBand(FALSE);

	CString s;
	s.Format("welcome to %s",LPCTSTR(g_app.GetName()));
	AddMarqueeString(s);

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,  BOOL& bHandled)
{
	for(int i=0;i<m_timers.GetSize();i++)
		KillTimer(m_timers[i]);
	bHandled = FALSE;
	return 0;
}

LRESULT CMainFrame::OnNCDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,  BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CMainFrame::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	EnumChildWindows(m_tabbedClient,EnumChildProc,(LPARAM)this);
	bHandled = FALSE;

	return 0;
}

LRESULT CMainFrame::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam,  BOOL& bHandled)
{
	if(m_timers.Find((ULONG &)wParam) >= 0){
		g_app.GetNo5Obj()->Fire_OnTimer(wParam);
	}
	return 0;
}

LRESULT CMainFrame::OnColorDlgClose(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CColorEditorDlg *pDlg = (CColorEditorDlg *)lParam;

	if(wParam == IDOK){
		InputFrame()->SetColorsetName(pDlg->m_ColorSet,TRUE);
	}
	return 0;
}

LRESULT CMainFrame::OnClrDlgDel(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CColorEditorDlg *pDlg = (CColorEditorDlg *)lParam;
	delete pDlg;
	return 0;
}

LRESULT CMainFrame::OnOptionsDlgClose(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	ATLASSERT(m_pOptionsDlg);

	// notify views, if needed
	for(int i=0;i<m_views.GetSize();i++){
		CViewBase *p = m_views[i];
		if(p->IsChatView()){
			CChatViewBase *pChatView = (CChatViewBase *)p;
			pChatView->OnOptionsChanged();
		}
	}
	if(g_app.GetGeneralOptions()->GetTopmost())
		SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0,SWP_NOSIZE | SWP_NOMOVE |  SWP_NOACTIVATE);
	else
		SetWindowPos(HWND_NOTOPMOST, 0, 0, 0, 0,SWP_NOSIZE | SWP_NOMOVE |  SWP_NOACTIVATE);

	delete m_pOptionsDlg;
	m_pOptionsDlg = NULL;

	return 0;
}

LRESULT CMainFrame::OnRoomDlgClose(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	ATLASSERT(m_pRoomDlg);

	if(wParam == IDOK){
		if(m_pView && m_pView->IsChatView()){
			g_app.GetGeneralOptions()->SetRoomServer(m_pRoomDlg->m_server);
			((CChatViewBase *)m_pView)->ChangeRoom(m_pRoomDlg->m_room);
		}
	}
	//m_pRoomDlg = NULL;
	return 0;
}

LRESULT CMainFrame::OnUnignore(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	// notify views, if needed
	for(int i=0;i<m_views.GetSize();i++){
		CViewBase *p = m_views[i];
		if(p->IsChatView()){
			CChatViewBase *pChatView = (CChatViewBase *)p;
			pChatView->OnUnignore((BOOL)wParam,(LPCTSTR)lParam);
		}
	}
	return 0;
}

LRESULT CMainFrame::OnCamDlgDel(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CCamViewDlg *p = (CCamViewDlg *)lParam;
	delete p;

	return 0;
}

LRESULT CMainFrame::OnVoiceDlgDel(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CVoiceDialog *p = (CVoiceDialog *)lParam;
	delete p;

	return 0;
}

LRESULT CMainFrame::OnScriptWnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CScriptEditFrame *p = (CScriptEditFrame *)lParam;
	delete p;
	return 0;
}


LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CLoginDlg dlg;

	if(IDOK == dlg.DoModal()){
		if(dlg.m_iProto == CLoginDlg::PROTO_YCHT){
			switch(dlg.m_iView){
				case CLoginDlg::ViewType::VIEW_RTF:
					{
					CChildFrame<CYahooChatRichView> * pChild = new CChildFrame<CYahooChatRichView>(this);
					pChild->CreateEx(m_hWndMDIClient);
					break;
					}
				case CLoginDlg::ViewType::VIEW_HTML:
					{
					CChildFrame<CYahooChatHtmlView> * pChild = new CChildFrame<CYahooChatHtmlView>(this);
					pChild->CreateEx(m_hWndMDIClient);
					break;
					}
				case CLoginDlg::ViewType::VIEW_TXT:
					{
					CChildFrame<CYahooChatEditView> * pChild = new CChildFrame<CYahooChatEditView>(this);
					pChild->CreateEx(m_hWndMDIClient);
					break;
					}
				default:
					ATLASSERT(0);
			}
		}
		else if(dlg.m_iProto == CLoginDlg::PROTO_YMSG){
			switch(dlg.m_iView){
				case CLoginDlg::ViewType::VIEW_RTF:
					{
					CChildFrame<CYahooMsgRichView> * pChild = new CChildFrame<CYahooMsgRichView>(this);
					pChild->CreateEx(m_hWndMDIClient);
					break;
					}
				case CLoginDlg::ViewType::VIEW_HTML:
					{
					CChildFrame<CYahooMsgHtmlView> * pChild = new CChildFrame<CYahooMsgHtmlView>(this);
					pChild->CreateEx(m_hWndMDIClient);
					break;
					}
				case CLoginDlg::ViewType::VIEW_TXT:
					{
					CChildFrame<CYahooMsgEditView> * pChild = new CChildFrame<CYahooMsgEditView>(this);
					pChild->CreateEx(m_hWndMDIClient);
					break;
					}
				default:
					ATLASSERT(0);
			}
		}
	}

	return 0;
}

LRESULT CMainFrame::OnScriptNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CScriptEditFrame *pFrame = new CScriptEditFrame(m_hWnd,NULL);
	pFrame->Create(m_hWnd);
	return 0;
}

LRESULT CMainFrame::OnFormNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CRect r(0,0,CW_USEDEFAULT,CW_USEDEFAULT);
	CFormFrame *pFrame = new CFormFrame();
	pFrame->CreateEx(m_hWnd,r,NULL);
	return 0;
}



LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible

	bVisible = !bVisible;

	// frame band
	ShowFrameBand(bVisible);
	ShowChatBand(bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewMarquee(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible

	bVisible = !bVisible;

	// frame band
	ShowMarqueeBand(bVisible);
	UISetCheck(ID_VIEW_MARQUEE, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnWindowCascade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MDICascade();
	return 0;
}

LRESULT CMainFrame::OnWindowTile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MDITile();
	return 0;
}

LRESULT CMainFrame::OnWindowArrangeIcons(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MDIIconArrange();
	return 0;
}


LRESULT CMainFrame::OnEditColors(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CColorEditorDlg *pDlg = new CColorEditorDlg();

	pDlg->m_ColorSet = m_bottom.GetColorsetName();
	pDlg->m_ColorFilePath = g_app.GetColorsFile();
	pDlg->Create(m_hWnd);
	ATLASSERT(pDlg->IsWindow());

	return 0;
}

LRESULT CMainFrame::OnOptions(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_pOptionsDlg && !m_pOptionsDlg->IsWindow()){
		delete m_pOptionsDlg;
		m_pOptionsDlg = NULL;
	}
	if(!m_pOptionsDlg){
		m_pOptionsDlg = new COptionsPropertySheet(m_hWnd);

		m_pOptionsDlg->AddPages();
		m_pOptionsDlg->Create(m_hWnd);
		ATLASSERT(m_pOptionsDlg->IsWindow());

		CWindow tc = m_pOptionsDlg->GetTabControl();
		tc.ModifyStyle(TCS_MULTILINE,0,SWP_NOSIZE /*dummy to call SetWindowPos */);
	}
	else
		m_pOptionsDlg->ShowWindow(SW_SHOWNORMAL);

	return 0;
}

LRESULT CMainFrame::OnRoomSelect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_pRoomDlg){
		if(m_pRoomDlg->IsWindow()){
			m_pRoomDlg->ShowWindow(SW_SHOWNORMAL);
		}
		else{
			delete m_pRoomDlg;
			m_pRoomDlg = NULL;
		}
	}
	if(!m_pRoomDlg){
		m_pRoomDlg = new CRoomDlg(NO5WM_ROOMDLG);
		m_pRoomDlg->m_server = g_app.GetGeneralOptions()->GetRoomServer();
		m_pRoomDlg->Create(m_hWnd);
		ATLASSERT(m_pRoomDlg->IsWindow());
	}
	return 0;
}

LRESULT CMainFrame::OnMarqueeChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CMarqueeOptions o;
	o.m_clrBack = m_marquee.GetBackColor();
	o.m_clrTxt = m_marquee.GetTextColor();
	o.m_elapse = m_marquee.GetElapse();
	BOOL res = g_app.GetGeneralOptions()->SetMarqueeOptions(o);
	ATLASSERT(res);
	return 0;
}


BOOL CALLBACK EnumChildProc(HWND hWnd,LPARAM lParam)
{
	CWindow wnd = hWnd;

	wnd.SendMessage(WM_CLOSE);
	return TRUE;
}


void CMainFrame::ActiveView(CViewBase *pView)
{
	m_pView = pView;
	
	if(m_pView && m_pView->IsChatView()){
		ShowChatBand(TRUE);
	}
	else{
		ShowChatBand(FALSE);
	}
}

CViewBase * CMainFrame::ActiveView(void)
{
	return m_pView;
}

IInputFrame * CMainFrame::InputFrame(void)
{
	return &m_bottom;
}

HWND CMainFrame::GetHandle(void)
{
	ATLASSERT(IsWindow());
	return m_hWnd;
}

void CMainFrame::AddView(CViewBase *pView)
{
	BOOL bFound = (m_views.Find(pView) >= 0);

	ATLASSERT(!bFound);
	if(!bFound){
		m_views.Add(pView);
	}
}

void CMainFrame::RemoveView(CViewBase *pView)
{
	BOOL bFound = (m_views.Find(pView) >= 0);

	ATLASSERT(bFound);
	m_views.Remove(pView);
}

void CMainFrame::ShowFrameBand(BOOL bShow)
{
	int idx = m_rebar.IdToIndex(IDC_BAND_FRAME);

	if(idx >= 0){
		m_rebar.ShowBand(idx,bShow);
	}
}

void CMainFrame::ShowChatBand(BOOL bShow)
{
	int idx = m_rebar.IdToIndex(IDC_BAND_CHAT);

	if(idx >= 0){
		m_rebar.ShowBand(idx,bShow);
	}
}

void CMainFrame::ShowMarqueeBand(BOOL bShow)
{
	int idx = m_rebar.IdToIndex(IDC_BAND_MARQUEE);

	if(idx >= 0){
		if(bShow){
			if(m_marquee.IsPaused())
				m_marquee.Start();
		}
		else
			m_marquee.Stop();
		BOOL res = m_rebar.ShowBand(idx,bShow);
		ATLASSERT(res);
	}
}

void CMainFrame::CreateMarquee(void)
{
	CRect r;
	CMarqueeOptions o;
	{
		CClientDC dc = m_hWnd;
		HFONT hFont;
		TEXTMETRIC tm;
		CFont font;

		//font = (HFONT)GetStockObject(ANSI_FIXED_FONT);
		font.CreatePointFont(100,"courier");
		hFont = dc.SelectFont(font);

		dc.GetTextMetrics(&tm);
		dc.SelectFont(hFont);
		GetClientRect(&r);
		r.top = 0;
		r.bottom = 3 * tm.tmHeight / 2;
	}
	g_app.GetGeneralOptions()->GetMarqueeOptions(o);
	m_marquee.SetBackColor(o.m_clrBack);
	m_marquee.SetTextColor(o.m_clrTxt);
	m_marquee.SetLoop(false);
	m_marquee.SetElapse(o.m_elapse);
	m_marquee.Create(m_hWnd,r,NULL,0,0,IDC_MARQUEE1);
	m_marquee.SetInc(m_marquee.GetCharWidth()/8);
	m_marquee.SetSpace( 2 * m_marquee.GetCharWidth() );
	m_marquee.Start();
}

void CMainFrame::AddMarqueeString(LPCTSTR s)
{
	m_marquee.AddItem(s);
}

void CMainFrame::GetViews(CSimpleValArray<CViewBase*> &views)
{
	NO5TL::CopySimpleArray<CViewBase *>(views,m_views);
}

void CMainFrame::ActivateChatView(LPCTSTR name)
{
	const int count = m_views.GetSize();
	int i;
	CViewBase *pView;

	for(i=0;i<count;i++){
		pView = m_views[i];
		if(pView->IsChatView()){
			LPCTSTR s = ((CChatViewBase *)(pView))->GetChatName();
			if(s && !lstrcmpi(s,name)){
				CWindow wnd = pView->GetHandle();
				if(wnd.IsWindow()){
					MDIActivate(wnd.GetParent());
				}
			}
		}
	}
}

ULONG CMainFrame::CreateTimer(ULONG nInterval)
{
	int i;
	ULONG id = 0;

	// find an unused id
	for(i=0;i<100;i++){
		ULONG dummy = ID_TIMER_BASE + (ULONG)i;
		if(m_timers.Find( dummy ) < 0){
			id = ID_TIMER_BASE + (ULONG)i;
		}
	}
	if(id){
		SetTimer(id,nInterval);
		m_timers.Add(id);
	}
	return id;
}

void CMainFrame::DestroyTimer(ULONG nID)
{
	KillTimer(nID);
	m_timers.Remove(nID);
}

void CMainFrame::OpenBrowser(LPCTSTR url,bool bAsk)
{
	CString msg;
	int res = IDYES;

	if(bAsk){
		msg.Format("Do you want to open \r\n%s",url);
		res = MessageBox(msg,g_app.GetName(),MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON1|MB_APPLMODAL|\
			MB_TOPMOST);
	}
	if(res == IDYES)
		::ShellExecute(m_hWnd,_T("OPEN"),url,NULL,NULL,SW_SHOWNORMAL);
}

LRESULT CMainFrame::OnHelpHomepage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString s;
	
	s.LoadString(IDS_HOMEPAGE);
	OpenBrowser(s,true);
	return 0;
}