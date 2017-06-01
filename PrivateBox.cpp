// PrivateBox.cpp: implementation of the CPrivateBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ctime>
#include <no5tl\mystring.h>
#include <no5tl\no5tlbase.h>	// CInterval;
#include <no5tl\winfile.h>
#include "resource.h"
#include "no5options.h"
#include "PrivateBox.h"
#include "no5stream.h"
#include "no5app.h"
#include "colorfile.h"
#include "user.h"
#include "imainframe.h"

using namespace NO5TL::Colors;
using namespace NO5YAHOO;

IVisualOptions * CPrivateBox::m_pvo = NULL;
IColorFile * CPrivateBox::m_pcf = NULL;
DWORD CPrivateBox::m_count = 0;

static BOOL ToolbarAddControl(HWND hToolBar,HWND hWnd,int pos,int count,BOOL bComboBox);

typedef CWinTraits<WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		WS_EX_CLIENTEDGE> CPmBottomTraits;


// bottom frame of the pm box

class CPmBottomFrame : public CFrameWindowImpl<CPmBottomFrame,CWindow,CPmBottomTraits>
{
	typedef CFrameWindowImpl<CPmBottomFrame,CWindow,CPmBottomTraits> _BaseClass;
	enum Timers
	{
		ID_TIMER_TYPE = 100,
	};
public:
	CPmBottomFrame():m_view(this,1)
	{
		m_timer = 0;
	}

	DECLARE_FRAME_WND_CLASS("NO5CLS_PMBOTTOM", IDR_PMBOTTOMFRAME)

	BEGIN_MSG_MAP(CPmBottomFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		COMMAND_ID_HANDLER(ID_EDIT_FADE,OnEditFade)
		COMMAND_ID_HANDLER(ID_EDIT_BLEND,OnEditBlend)
		COMMAND_CODE_HANDLER(CBN_SELENDOK,OnComboSelChange)
		CHAIN_MSG_MAP(_BaseClass)
		FORWARD_NOTIFICATIONS()
	ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_CHAR,OnEditChar)
	END_MSG_MAP()

	// message handlers
	LRESULT OnCreate(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		const BOOL bIniSep = TRUE;
		int x;
		CRect rcCombo(0,0,0,200);

		m_hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_PMBOTTOMFRAME, bIniSep /*separator*/,
			ATL_SIMPLE_TOOLBAR_STYLE);
		m_tb.Attach(m_hWndToolBar);

		m_hWndClient = m_view.Create(m_hWnd,rcDefault,NULL,WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|\
			ES_LEFT|WS_BORDER,0);

		// correct toolbar button styles
		TBBUTTONINFO info = { sizeof(TBBUTTONINFO),TBIF_COMMAND|TBIF_STYLE,0,0,0,TBSTYLE_CHECK};

		info.idCommand = ID_EDIT_BOLD;
		m_tb.SetButtonInfo(ID_EDIT_BOLD,&info);
		info.idCommand = ID_EDIT_ITALIC;
		m_tb.SetButtonInfo(ID_EDIT_ITALIC,&info);
		info.idCommand = ID_EDIT_UNDERLINE;
		m_tb.SetButtonInfo(ID_EDIT_UNDERLINE,&info);
		info.idCommand = ID_EDIT_FADE;
		m_tb.SetButtonInfo(ID_EDIT_FADE,&info);
		info.idCommand = ID_EDIT_BLEND;
		m_tb.SetButtonInfo(ID_EDIT_BLEND,&info);

		// create combo with font sizes
		m_cbSize.Create(m_tb,rcCombo,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_BORDER|\
			CBS_DROPDOWNLIST,0);
		m_cbSize.SetFont(AtlGetDefaultGuiFont());

		int sizes[] = { 6,8,10,12,14,16,18,20,22,24,26,28,30,32 };
		int j;
		TCHAR tmp[3] = {0};
		int index;

		for(j=0;j < sizeof(sizes)/sizeof(sizes[0]);j++){
			wsprintf(tmp,_T("%02d"),sizes[j]);
			index = m_cbSize.AddString(tmp);
			if(index >= 0){
				m_cbSize.SetItemData(index,(DWORD)sizes[j]);
			}
			else{
				ATLASSERT(0);
			}
		}
		x = bIniSep ? 8 : 7;
		ToolbarAddControl(m_tb,m_cbSize,x,2,TRUE);

		// create combo with fade names
		m_cbFade.Create(m_tb,rcCombo,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_BORDER|CBS_DROPDOWNLIST,
			0);
		m_cbFade.SetFont(AtlGetDefaultGuiFont());
		x = bIniSep ? 11 : 10;
		ToolbarAddControl(m_tb,m_cbFade,x,4,TRUE);
		m_tb.ShowWindow(SW_NORMAL);
		ReloadFadeCombo();

		//
		UpdateLayout(FALSE);
		// select a font size
		int idx = m_cbSize.FindStringExact(-1,"10");
		m_cbSize.SetCurSel(idx);

		return 0;
	}
	LRESULT OnTimer(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		if(wParam == ID_TIMER_TYPE){
			m_dt.UpdateEnd();
			const DWORD dt = m_dt.Get();
			if(m_dt.Get() > 5000){
				KillTimer(ID_TIMER_TYPE);
				m_timer = 0;
				CWindow wnd = GetParent();
				wnd.PostMessage(NO5WM_TYPING,(WPARAM)FALSE);
			}
		}
		else
			bHandled = FALSE;
		return 0;
	}

	LRESULT OnEditFade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
		if(m_tb.IsButtonChecked(ID_EDIT_FADE))
			m_tb.CheckButton(ID_EDIT_BLEND,FALSE);
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnEditBlend(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
		if(m_tb.IsButtonChecked(ID_EDIT_BLEND))
			m_tb.CheckButton(ID_EDIT_FADE,FALSE);
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnComboSelChange(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		if(hWndCtl == m_cbFade.m_hWnd){
			/*
			if(m_pSink){
				CString s;
				int index = m_cbFade.GetCurSel();
				
				if(index >= 0){
					m_cbFade.GetLBText(index,s);
					m_pSink->OnColorsetChange(s);
				}
			}
			*/
			bHandled = FALSE;	// pm frame handles this
		}
		else if(hWndCtl == m_cbSize.m_hWnd){
			//
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnEditChar(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		if(wParam == (WPARAM)'\r'){
			if(m_timer){
				KillTimer(ID_TIMER_TYPE);
				m_timer = 0;
			}
			OnEnter();
		}
		else if(wParam == (WPARAM)'\n'){
			
		}
		else{
			if(!m_timer){
				m_timer = SetTimer(ID_TIMER_TYPE,1000);
				CWindow wnd = GetParent();
				wnd.SendMessage(NO5WM_TYPING,(WPARAM)TRUE);
			}
			m_dt.UpdateStart();
			bHandled = FALSE;
		}
		return 0;
	}

	LONG GetDesiredHeight(void)
	{
		CRect r;
		LONG cy = 0;
		CClientDC dc(m_view);
		TEXTMETRIC tm = {0};

		m_tb.GetWindowRect(&r);
		cy += r.Height();
		dc.GetTextMetrics(&tm);
		cy += tm.tmHeight;
		cy += 10;
		return cy;
	}

private:
	void ReloadFadeCombo(void)
	{
		IColorFile *p = NULL;

		if(ColorFileCreate(&p)){
			CSimpleArray<CString> names;
			
			if(p->SetFileName(g_app.GetColorsFile())){
				if(p->GetNames(names)){
					m_cbFade.ResetContent();

					for(int i=0;i<names.GetSize();i++){
						m_cbFade.AddString(names[i]);
					}
					// add the random one
					m_cbFade.AddString("random");
				}
			}
			ColorFileDestroy(&p);
		}
	}
	void OnEnter(void)
	{
		NO5TL::CWindowText s(m_view);
		CWindow wnd = GetParent();

		KillTimer(ID_TIMER_TYPE);
		m_timer = 0;
		wnd.SendMessage(NO5WM_ENTER,(WPARAM)(LPCTSTR)s);
		s.Empty();
	}

	//
private:
	CContainedWindowT<CEdit> m_view;
	CComboBox m_cbSize;
	CComboBox m_cbFade;
	CToolBarCtrl m_tb;
	UINT m_timer;
	NO5TL::CInterval m_dt;

	friend CPrivateBox;
};

// CPrivateBox 

CPrivateBox::CPrivateBox(IPrivateBoxEvents *pSink)
{
	m_pBottom = NULL;
	m_pSink = pSink;
	m_pUser = NULL;
	m_bNotifyDestroy = true;
	// static data
	if(m_count == 0){
		ATLASSERT(!(m_pvo && m_pcf));
		m_pvo = g_app.GetVisualOptions();
		ColorFileCreate(&m_pcf);
		m_pcf->SetFileName(g_app.GetColorsFile());
	}
	m_count++;
}

CPrivateBox::~CPrivateBox()
{
	if(m_pBottom)
		delete m_pBottom;
	ATLASSERT(m_count > 0);
	m_count--;
	if(m_count == 0){
		// free static data
		m_pvo = NULL;
		ColorFileDestroy(&m_pcf);
	}
}


BOOL CPrivateBox::PreTranslateMessage(MSG *pMsg)
{
	CWindow wndFocus = GetFocus();
	BOOL res = FALSE;

	if(wndFocus && (wndFocus == m_top.m_hWnd) ){
		res = _BaseClass::PreTranslateMessage(pMsg);
	}
	return res;
}


LRESULT CPrivateBox::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CreateSimpleStatusBar();
	m_sb.Attach(m_hWndStatusBar);

	// append user menu
	CMenuHandle menu = GetMenu();
	CMenu menuUser;

	if(menu.IsMenu()){
		if(menuUser.LoadMenu(IDR_USER)){
			menuUser = menuUser.GetSubMenu(0);
			if(menuUser.IsMenu()){
				menu.AppendMenu(MF_POPUP|MF_STRING,UINT_PTR(HMENU(menuUser)),"User");
				DrawMenuBar();
			}
		}
	}

	// make window title
	SetWindowText(m_pUser->m_name);


	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	ATLASSERT(m_pUser);

	BOOL res;
	CRect r;

	res = CreateTopFrame();
	ATLASSERT(res);
	res = CreateBottomFrame();
	ATLASSERT(res);
	UpdateLayout(FALSE);

	UISetCheck(ID_USER_IGNORE,m_pUser->IsIgnored() ? TRUE : FALSE);
	UISetCheck(ID_USER_FOLLOW,m_pUser->m_follow ? TRUE : FALSE);
	UISetCheck(ID_USER_MIMIC,m_pUser->m_mimic ? TRUE : FALSE);

	return 0;
}

LRESULT CPrivateBox::OnDestroy(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	// unregister object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);

	if(m_bNotifyDestroy)
		m_pSink->pm_OnDestroy(m_pUser->m_name,this);
	bHandled = TRUE;
	return 0;
}

LRESULT CPrivateBox::OnEnter(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	LPCTSTR p = (LPCTSTR)wParam;

	if(p && lstrlen(p)){
		CInputStream ts;
		bool fade = false;
		bool blend = false;
		CTextAtom atom;
		int idx;

		ts << InlineMode;

		// get font size
		idx = m_pBottom->m_cbSize.GetCurSel();
		if(idx >= 0){
			CString s;
			m_pBottom->m_cbSize.GetLBText(idx,s);
			if(!s.IsEmpty())
				atom.AtomFont("arial",atoi(s));
		}

		if(m_pBottom->m_tb.IsButtonChecked(ID_EDIT_FADE) && m_colors.GetSize()){
			fade = true;
			ts.AddAtom(atom.AtomFade(m_colors));
		}
		else if(m_pBottom->m_tb.IsButtonChecked(ID_EDIT_BLEND) && m_colors.GetSize()){
			blend = true;
			ts.AddAtom(atom.AtomAlt(m_colors));
		}
		if(m_pBottom->m_tb.IsButtonChecked(ID_EDIT_BOLD))
			ts << Bold(TRUE);
		if(m_pBottom->m_tb.IsButtonChecked(ID_EDIT_ITALIC))
			ts << Italic(TRUE);
		if(m_pBottom->m_tb.IsButtonChecked(ID_EDIT_UNDERLINE))
			ts << Under(TRUE);

		ts << YahooMode << p;

		if(fade)
			ts.AddAtom(atom.AtomFadeOff());
		else if(blend)
			ts.AddAtom(atom.AtomAltOff());

		CString s = ts.GetYahooText();
		
		if(s.GetLength()){
			m_pSink->pm_OnSend(this,s);
		}
	}
	return 0;
}

LRESULT CPrivateBox::OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(\
		FALSE,		// file open
		"ddf",		// default extension
		NULL,		// file name
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,		// flags
		"rtf files\0*.rtf\0text files\0*.txt\0All files\0*.*\0",	// filter
		m_hWnd);	// parent

	if(IDOK == dlg.DoModal()){
		NO5TL::CWinFile wf;
		BOOL res;

		if(res = wf.Create(dlg.m_szFileName,CREATE_ALWAYS,GENERIC_WRITE)){
			res = m_top.StreamOutFile(wf);
			ATLASSERT(res);
		}
	}
	return 0;
}

LRESULT CPrivateBox::OnTyping(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	if(m_pSink)
		m_pSink->pm_OnTyping(this,(BOOL)wParam);
	return 0;
}

LRESULT CPrivateBox::OnUserIgnore(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_pSink->pm_OnUserIgnored(m_pUser->m_name);
	UISetCheck(ID_USER_IGNORE,m_pUser->IsIgnored() ? TRUE : FALSE);
	return 0;
}

LRESULT CPrivateBox::OnUserFollow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_pSink->pm_OnUserFollow(m_pUser->m_name);
	UISetCheck(ID_USER_FOLLOW,m_pUser->m_follow ? TRUE : FALSE);
	return 0;
}

LRESULT CPrivateBox::OnUserMimic(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_pSink->pm_OnUserMimic(m_pUser->m_name);
	UISetCheck(ID_USER_MIMIC,m_pUser->m_mimic ? TRUE : FALSE);
	return 0;
}

LRESULT CPrivateBox::OnUserProfile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_pSink->pm_OnUserProfile(m_pUser->m_name);
	return 0;
}

LRESULT CPrivateBox::OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CPrivateBox::OnComboSelChange(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(hWndCtl == m_pBottom->m_cbFade){
		CComboBox cb = m_pBottom->m_cbFade;
		CString s;
		int idx = cb.GetCurSel();
		if(idx >= 0){
			cb.GetLBText(idx,s);
			m_colors.RemoveAll();
			m_pcf->GetColorSet(s,m_colors);
		}
	}
	else if(hWndCtl == m_pBottom->m_cbSize){
			//
	}
	return 0;
}

LRESULT CPrivateBox::OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_top.PostMessage(WM_COPY);
	return 0;
}

LRESULT CPrivateBox::OnEditMsgFilter(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	/*
	MSGFILTER *p = (MSGFILTER *)pnmh;

	bHandled = FALSE;
	switch(p->msg)
	{
	case WM_RBUTTONUP:
		{
			int line;
			int len = 0;
			int pos;
			CPoint pt;

			GetCursorPos(&pt);
			m_top.ScreenToClient(&pt);
			pos = m_top.CharFromPos(pt);
			line = m_top.LineFromChar(pos);
			if(line >= 0){
				len = m_top.LineLength(line);
			}
			if(len > 0){
				CString s;
				NO5TL::CStringBuffer buf(s,len + 1);
				CString name;

				m_top.GetLine(line,buf,len);
				
				buf[len] = '\0';
				LPCTSTR p = (LPCTSTR)buf;
				LPCTSTR q = p;
				while(*q && !isspace(*q))
					q++;
				if(q > p){
					StringCopyN(name,p,0,(int)(q-p));

					if(!name.IsEmpty()){
						m_top.ClientToScreen(&pt);
						DisplayUserMenu(name,pt);	// todo: we dont know if its a user name
					}
				}
			}
		}
		break;
	default:
		break;
	}
	*/
	bHandled = FALSE;
	return 0;
}

LRESULT CPrivateBox::OnEditLink(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	ENLINK *p = (ENLINK *)pnmh;

	bHandled = FALSE;
	switch(p->msg)
	{
	case WM_LBUTTONDOWN:
		{
			CString link = m_top.GetTextRangeString(p->chrg);
			g_app.GetMainFrame()->OpenBrowser(link,true);
		}
		break;
	default:
		break;
	}
	return 0; // let the control to handle the message
}


BOOL CPrivateBox::CreateBottomFrame(void)
{
	ATLASSERT(!m_pBottom);
	m_pBottom = new CPmBottomFrame();
	m_pBottom->Create(m_hWnd,rcDefault,NULL,0,0);
	return m_pBottom->IsWindow();
}

BOOL CPrivateBox::CreateTopFrame(void)
{
	m_top.Create(m_hWnd,rcDefault,NULL,WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|\
		WS_VSCROLL,WS_EX_CLIENTEDGE);
	m_top.SetOleCallback(this);
	m_top.SetBackgroundColor(m_pvo->ClrBack());
	m_top.SetEventMask(ENM_LINK/*|ENM_MOUSEEVENTS*/);
	m_top.SetUndoLimit(0);
	return m_top.IsWindow();
}

void CPrivateBox::AddMessage(LPCTSTR from,LPCTSTR text)
{
	CRichEditStream ts;

	ts.SetEdit(&m_top);
	ATLASSERT(m_pvo);
	ts << YahooMode;
	ts.SetDefaults("tahoma",10,0);
	ts.AddDefaults();
	bool bFromMe = lstrcmpi(from,m_me) == 0;

	if(bFromMe)
		ts << m_pvo->NameMe();
	else if(m_pUser->m_buddy)
		ts << m_pvo->NameBuddy();
	else
		ts << m_pvo->NameUser(); 
	ts << from << " : ";
	if(bFromMe)
		ts << m_pvo->TxtMe();
	else if(m_pUser->m_buddy)
		ts << m_pvo->TxtBuddy();
	else
		ts << m_pvo->TxtUser();
	ts << text << Endl;
	ts.ScrollText();

	if(m_sb.IsWindow()){
		time_t t = time(NULL);
		struct tm *ptm = localtime(&t);
		CString s;

		s.Format("msg %s at %s",bFromMe ? "sent" : "received" ,asctime(ptm));
		m_sb.SetText(0/*pane*/,s,0/*type*/);
	}
}

void CPrivateBox::DestroyPm(void)
{
	m_bNotifyDestroy = false;
	DestroyWindow();
}

void CPrivateBox::ShowPm(void)
{
	ShowWindow(SW_RESTORE);
}

void CPrivateBox::UpdateUser(void)
{
	UISetCheck(ID_USER_IGNORE,m_pUser->IsIgnored());
	UISetCheck(ID_USER_FOLLOW,m_pUser->m_follow);
	UISetCheck(ID_USER_MIMIC,m_pUser->m_mimic);
}

void CPrivateBox::Typing(BOOL bStart)
{
	if(m_sb.IsWindow()){
		time_t t = time(NULL);
		struct tm *ptm = localtime(&t);
		CString s;

		s.Format("%s %s [ %s ]",(LPCTSTR)m_pUser->m_name, bStart ? "is typing" : "stopped typing",asctime(ptm));
		m_sb.SetText(0/*pane*/,s,0/*type*/);
	}
}

void CPrivateBox::UpdateLayout(BOOL bResizeBars)
{
	CRect rect;
	GetClientRect(&rect);
	LONG cyBottom = 0;

	// position bars and offset their dimensions
	UpdateBarsPosition(rect, bResizeBars);

	if(m_pBottom->IsWindow()){
		cyBottom = m_pBottom->GetDesiredHeight();
		m_pBottom->MoveWindow(rect.left,rect.TopLeft + rect.Height() - cyBottom,rect.Width(),
			cyBottom);
	}

	if(m_top.IsWindow()){
		m_top.MoveWindow(rect.left,rect.top,rect.Width(),rect.Height() - cyBottom);
	}
	
}

STDMETHODIMP CPrivateBox::GetContextMenu(WORD seltype, LPOLEOBJECT pOleObj,
							    CHARRANGE* pchrg, HMENU* phMenu)
{
	HRESULT hr = S_FALSE;

	if(seltype & (SEL_TEXT|SEL_MULTICHAR) != 0){
		CMenu menu;
		hr = S_FALSE;
		*phMenu = NULL;
		menu.LoadMenu(IDR_PMFRAME);
		if(menu.IsMenu()){
			menu = menu.GetSubMenu(1);
			if(menu.IsMenu()){
				*phMenu = menu.Detach();	// rich edit will destroy the menu
				hr = S_OK;
			}
		}
	}
	return hr;
}

/* CPrivateBoxList */

CPrivateBox * CPrivateBoxList::Find(LPCTSTR name)
{
	CPrivateBox *p = NULL;
	const int count = m_arr.GetSize();
	int i;

	for(i=0;i<count;i++){
		p = m_arr[i];
		if(!lstrcmpi(name,p->m_pUser->m_name))
			break;
		else
			p = NULL;
	}
	return p;
}

BOOL CPrivateBoxList::RemoveBox(LPCTSTR name)
{
	BOOL res = FALSE;
	CPrivateBox *p;
	const int count = m_arr.GetSize();
	int i;

	for(i=0;i<count;i++){
		p = m_arr[i];
		if(!lstrcmpi(name,p->m_pUser->m_name)){
			m_arr.RemoveAt(i);
			res = TRUE;
			break;
		}
	}
	return res;
}

// TODO: there are 2 implementations of this. remove one
BOOL ToolbarAddControl(HWND hToolBar,HWND hWnd,int pos,int count,BOOL bComboBox)
{
	CToolBarCtrl tb = hToolBar;
	CWindow wnd = hWnd;
	CRect rcButton;
	BOOL res;

	res = tb.GetItemRect(pos,&rcButton);
	if(res){
		if(bComboBox){
			CRect rcWnd;
			CComboBox cb = hWnd;

			cb.SetItemHeight(-1,rcButton.Height() - 5);
			wnd.GetWindowRect(&rcWnd);
			wnd.MoveWindow(rcButton.left,rcButton.top,count * rcButton.Width(),rcWnd.Height());
		}
		else
			wnd.MoveWindow(rcButton.left,rcButton.top,count * rcButton.Width(),rcButton.Height());
	}
	return res;
}