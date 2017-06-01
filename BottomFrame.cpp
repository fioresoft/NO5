// BottomFrame.cpp: implementation of the CBottomFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "usrmsgs.h"
#include <no5tl\color.h>
#include <no5tl\no5richedit.h>
#include <no5tl\mystring.h>
#include "resource.h"
#include "imainframe.h"
#include "BottomFrame.h"
#include "viksoe\fontcombo.h"
#include "colorfile.h"
#include "no5app.h"
#include "smilepicker.h"

static BOOL ToolbarAddControl(HWND hToolBar,HWND hWnd,int pos,int count,BOOL bComboBox);


class CAutoCompleteDlg : \
	public CDialogImpl<CAutoCompleteDlg>,
	public CMessageFilter
{
public:
	enum { IDD = IDD_COMPLETE };

	BEGIN_MSG_MAP(CAutoCompleteDlg)
		MESSAGE_HANDLER(WM_INITDIALOG,OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		MESSAGE_HANDLER(WM_CLOSE,OnClose)
		//MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown)
		COMMAND_CODE_HANDLER(LBN_DBLCLK,OnDoubleClick)
	ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown)
		MESSAGE_HANDLER(WM_GETDLGCODE,OnGetDlgCode)
	END_MSG_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}

	LRESULT OnInitDialog(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		CRect rc;
		GetClientRect(&rc);
		
		m_lb.SubclassWindow(GetDlgItem(IDC_LIST1));
		
		for(int i=0;i<m_arr.GetSize();i++){
			m_lb.AddString(m_arr[i]);
		}

		CRect rcWindow;
	
		GetWindowRect(&rcWindow);
		
		MoveWindow(m_ptCaret.x ,m_ptCaret.y - rcWindow.Height(),rcWindow.Width(),
			rcWindow.Height());

		//m_lb.SetFocus();
		m_lb.SetCurSel(0);

		CMessageLoop *pLoop = _Module.GetMessageLoop();
		pLoop->AddMessageFilter(this);
		
		return 0;
	}
	LRESULT OnDestroy(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		CMessageLoop *pLoop = _Module.GetMessageLoop();
		pLoop->RemoveMessageFilter(this);
		return 0;
	}
	LRESULT OnGetDlgCode(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		return DLGC_WANTALLKEYS;
	}
		
	LRESULT OnDoubleClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		int sel = m_lb.GetCurSel();
		int res = IDCANCEL;

		if(sel >= 0){
			m_lb.GetText(sel,m_res);
			res = IDOK;
		}
		EndDialog(res);
		return 0;
	}
	LRESULT OnClose(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		EndDialog(IDCANCEL);
		return 0;
	}
	LRESULT OnKeyDown(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		bHandled = TRUE;
		if(wParam == VK_ESCAPE)
			PostMessage(WM_CLOSE);
		else if(wParam == VK_RETURN){
			BOOL b;
			OnDoubleClick(0,0,NULL,b);
		}
		else
			bHandled = FALSE;
		return 0;
	}

	CAutoCompleteDlg(NO5TL::CStringArray &arr,CString &res,CPoint &ptCaret):\
		m_arr(arr),m_res(res),m_ptCaret(ptCaret),
		m_lb(this,1)
	{
		//
	}
	~CAutoCompleteDlg()
	{
		//
	}

private:
	CContainedWindowT<CListBox> m_lb;
	NO5TL::CStringArray &m_arr;
	CString &m_res;
	CPoint m_ptCaret;
};


class CInputView1 : public CWindowImpl<CInputView1>
{
public:
	CInputView1(CBottomFrame &BottomFrame):\
		m_input(this,1),
		m_BottomFrame(BottomFrame)
	{
		
	}
	virtual ~CInputView1()
	{
		//
	}
	BEGIN_MSG_MAP(CBottomFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		COMMAND_ID_HANDLER(ID_EDIT_COMPLETE,OnEditComplete)
		COMMAND_ID_HANDLER(IDC_BUTTON_SEND,OnButtonSend)
	ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_CHAR,OnEditChar)
		MESSAGE_HANDLER(WM_KEYDOWN,OnEditKeyDown)
		MESSAGE_HANDLER(WM_KEYUP,OnEditKeyUp)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		m_input.Create(m_hWnd,rcDefault,NULL,WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_LEFT);
		m_btSend.Create(m_hWnd,rcDefault,_T("Send"),WS_CHILD|WS_VISIBLE|WS_BORDER|BS_PUSHBUTTON,
			0,IDC_BUTTON_SEND);
		m_font.CreatePointFont(160,_T("Arial"));
		m_input.SetFont(m_font);
		m_fontButton.CreatePointFont(120,_T("Arial"));
		m_btSend.SetFont(m_fontButton);
		return 0;
	}
	LRESULT OnSize(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		if(m_input.IsWindow() && m_btSend.IsWindow()){
			CRect rcClient;
			CRect r;
			int cx,cy;

			GetClientRect(&rcClient);
			CalcButtonRect(cx,cy);
			cx += 8;	// add pixels to border and space

			// move combo box
			r.left = rcClient.left;
			r.top = rcClient.top;
			r.right = rcClient.right - cx;
			r.bottom = rcClient.bottom;
			m_input.MoveWindow(r.left,r.top,r.Width(),r.Height());
			// move button
			r.left = rcClient.right - cx;
			r.top = rcClient.top;
			r.right = rcClient.right;
			r.bottom = rcClient.bottom;
			m_btSend.MoveWindow(r.left,r.top,r.Width(),r.Height());
		}
		return 0;
	}
	LRESULT OnEditChar(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		if(wParam == (WPARAM)'\r'){
			//
		}
		else if(wParam == (WPARAM)'\n'){
			
		}
		else{
			bHandled = FALSE;
		}
		return 0;
	}
	LRESULT OnEditKeyDown(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		if(wParam == VK_RETURN){
			OnEnter();
		}
		else
			bHandled = FALSE;
		return 0;
	}
	LRESULT OnEditKeyUp(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		if(wParam != VK_RETURN)
			bHandled = FALSE;
		return 0;
	}
	LRESULT OnSetFocus(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		if(m_input.IsWindow())
			m_input.SetFocus();
		return 0;
	}
	
	//
	int GetDesiredHeight(void)
	{
		int cx,cy;

		CalcButtonRect(cx,cy);

		return cy + 2;
	}
	// a smile was selected in tb. add the code to the edit
	void AddSmile(LPCTSTR s)
	{
		m_input.AppendText(s);
	}
	LRESULT OnButtonSend(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		OnEnter();
		return 0;
	}

	LRESULT OnEditComplete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(m_BottomFrame.m_pSink && GetFocus() == HWND(m_input)){
			CHARRANGE cr = {0};

			m_input.GetSel(cr);
			if(cr.cpMin == cr.cpMax){
				TEXTRANGE tr;

				cr.cpMin = m_input.FindWordBreak(WB_MOVEWORDLEFT,cr.cpMin);
				tr.chrg = cr;
				if(cr.cpMax > cr.cpMin){
					CString sPartial;
					NO5TL::CStringArray arr;

					{
						NO5TL::CStringBuffer buf(sPartial,cr.cpMax - cr.cpMin + 1);
						tr.lpstrText = LPTSTR(buf);
						m_input.GetTextRange(&tr);
					}

					if(m_BottomFrame.m_pSink->OnAutoComplete(sPartial,arr)){
						CString name;

						if(arr.GetSize() == 1){ // single match. append it
							name = arr[0];
						}
						else{
							CPoint ptCaret;
							CWindow frame;
							CRect rcEdit;
							HWND hWndFocus;

							GetCaretPos(&ptCaret);
							m_input.GetWindowRect(rcEdit);
							frame = g_app.GetMainFrame()->GetHandle();
							m_input.ClientToScreen(&ptCaret);
							frame.ScreenToClient(&ptCaret);
							ptCaret.y = rcEdit.top;

							CAutoCompleteDlg dlg(arr,name,ptCaret);

							hWndFocus = GetFocus();
							if(dlg.DoModal(g_app.GetMainFrame()->GetHandle()) == IDOK){
								
							}
							else
								name.Empty();
							::SetFocus(hWndFocus);

						}
						if(name.GetLength()){
							const int count = name.GetLength() - sPartial.GetLength();
							if(count > 0){
								CString rest = name.Right(count);
								// problem: part may be in caps part not. whatever
								m_input.AppendText(rest);
							}
						}
					}
				}
			}
		}
		return 0;
	}
	
private:
	CContainedWindowT<CRichEditCtrl> m_input;
	CBottomFrame &m_BottomFrame;
	CFont m_font;
	CButton m_btSend;
	CFont m_fontButton;


	void OnEnter(void)
	{
		CInputParams p;
		NO5TL::CWindowText txt(m_input);

		if(m_BottomFrame.m_pSink){
			m_BottomFrame.m_pSink->OnTextInput(txt);
		}
		txt.Empty();
	}
	static void CalcControlRect(HWND hWnd,HFONT hFont,int &cx,int &cy,LPCTSTR text = NULL)
	{
		NO5TL::CWindowTextString s(hWnd);
		CClientDC dc(hWnd);
		CRect r(0,0,0,0);
		HFONT hFontOld = dc.SelectFont(hFont);

		if(text)
			s = text;
		dc.DrawText(s,s.GetLength(),&r,DT_CALCRECT);
		dc.SelectFont(hFontOld);
		ATLASSERT(r.Height());
		cx = r.Width();
		cy = r.Height();
	}
	void CalcButtonRect(int &cx,int &cy)
	{
		CalcControlRect(m_btSend,m_fontButton,cx,cy);
	}
};

//////////////////////////////////////////////////////////////////////////////

CBottomFrame::CBottomFrame(IMainFrame *pFrame):m_pFrame(pFrame)
{
	ATLASSERT(NULL != m_pFrame);
	m_pSink = NULL;
	m_view1 = new CInputView1(*this);
}

CBottomFrame::~CBottomFrame()
{
	delete m_view1;
}

LRESULT CBottomFrame::OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	//_BaseClass::OnCreate(WM_CREATE,wParam,lParam,bHandled);
	bHandled = TRUE;


	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_BOTTOMFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	m_tb.Attach(hWndToolBar);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_STYLE);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);
	m_hWndClient = m_view1->Create(m_hWnd,rcDefault,NULL,WS_CHILD|WS_VISIBLE);
	//AddTab(m_view1->m_hWnd,"input");
	
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

	// create color combo box
	CRect rcCombo(0,0,0,150);	// we need a big cy so the droplist is visible
	m_cbColors.Create(m_tb,rcCombo,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_BORDER|\
		CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED,0);

	// add some colours
	m_cbColors.AddColor(0,NO5TL::Colors::BLACK);
	m_cbColors.AddColor(0,NO5TL::Colors::RED);
	m_cbColors.AddColor(0,NO5TL::Colors::GREEN);
	m_cbColors.AddColor(0,NO5TL::Colors::YELLOW);
	m_cbColors.AddColor(0,NO5TL::Colors::BLUE);
	m_cbColors.AddColor(0,NO5TL::Colors::FUCHSIA);
	m_cbColors.AddColor(0,NO5TL::Colors::AQUA);
	m_cbColors.AddColor(0,NO5TL::Colors::GREY);
	m_cbColors.AddColor(0,NO5TL::Colors::LIME);
	m_cbColors.AddColor(0,NO5TL::Colors::MARRON);
	m_cbColors.AddColor(0,NO5TL::Colors::NAVY);
	m_cbColors.AddColor(0,NO5TL::Colors::OLIVE);
	m_cbColors.AddColor(0,NO5TL::Colors::PINK);
	m_cbColors.AddColor(0,NO5TL::Colors::PURPLE);
	m_cbColors.AddColor(0,NO5TL::Colors::SILVER);
	m_cbColors.AddColor(0,NO5TL::Colors::TEAL);

	ToolbarAddControl(m_tb,m_cbColors,7,2,TRUE);
	UpdateLayout(FALSE);

	// create font combobox
	m_cbFonts.Create(m_tb,rcCombo,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_BORDER|\
		CBS_DROPDOWNLIST|CBS_SORT|CBS_OWNERDRAWVARIABLE|CBS_HASSTRINGS,0);
	m_cbFonts.SetExtendedFontStyle(FPS_EX_TYPEICON|FPS_EX_SHOWFACE);
	m_cbFonts.Dir();	// add items

	ToolbarAddControl(m_tb,m_cbFonts,10,7,TRUE);
	UpdateLayout(FALSE);

	// create combo with font sizes
	m_cbSize.Create(m_tb,rcCombo,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_BORDER|\
		CBS_DROPDOWNLIST,0);
	m_cbSize.SetFont(AtlGetDefaultGuiFont());

	int sizes[] = { 6,8,10,12,14,16,18,20,22,24};
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
	ToolbarAddControl(m_tb,m_cbSize,18,2,TRUE);

	// create combo with fade names
	m_cbFade.Create(m_tb,rcCombo,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_BORDER|CBS_DROPDOWNLIST,
		0);
	m_cbFade.SetFont(AtlGetDefaultGuiFont());
	ToolbarAddControl(m_tb,m_cbFade,21,4,TRUE);
	ReloadFadeCombo();

	//
	UpdateLayout(FALSE);

	// accelerator
	m_hAccel = ::LoadAccelerators(_Module.GetResourceInstance(), MAKEINTRESOURCE(GetWndClassInfo().m_uCommonResourceID));
	ATLASSERT(m_hAccel != NULL);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	//pLoop->AddIdleHandler(this);

	return 0;
}

LRESULT CBottomFrame::OnDestroy(UINT , WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	_BaseClass::OnDestroy(WM_DESTROY,wParam,lParam,bHandled);
	//DestroyTabWindow();
	bHandled = TRUE;
	return 0;
}

LRESULT CBottomFrame::OnSmileWndClose(UINT , WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_view1){
#ifdef NO5_SMILEWND2
		CSmilePicker2 *p = (CSmilePicker2 *)lParam;
#else
		CSmilePicker *p = (CSmilePicker *)lParam;
#endif
		if(!p->m_res.IsEmpty()){
			m_view1->AddSmile(p->m_res);
			m_view1->SetFocus();
		}
	}
	return 0;
}

LRESULT CBottomFrame::OnEditFade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	if(m_tb.IsButtonChecked(ID_EDIT_FADE))
		m_tb.CheckButton(ID_EDIT_BLEND,FALSE);
	bHandled = FALSE;
	return 0;
}

LRESULT CBottomFrame::OnEditBlend(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	if(m_tb.IsButtonChecked(ID_EDIT_BLEND))
		m_tb.CheckButton(ID_EDIT_FADE,FALSE);
	bHandled = FALSE;
	return 0;
}

LRESULT CBottomFrame::OnSmiles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	CRect r(0,0,400,250);
#ifdef NO5_SMILEWND2
	CSmilePicker2 *p = new CSmilePicker2(m_hWnd);
	p->Create(m_hWnd,r,NULL);
#else
	CSmilePicker *p = new CSmilePicker(m_hWnd);
	p->Create(m_hWnd,r,"mshtml:<html><body></body></html>");
#endif
	
	return 0;
}

LRESULT CBottomFrame::OnComboSelChange(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(hWndCtl == m_cbFade.m_hWnd){
		if(m_pSink){
			CString s;
			int index = m_cbFade.GetCurSel();
			
			if(index >= 0){
				m_cbFade.GetLBText(index,s);
				m_pSink->OnColorsetChange(s);
			}
		}
	}
	else if(hWndCtl == m_cbFonts.m_hWnd){
		//
	}
	else if(hWndCtl == m_cbColors.m_hWnd){
		//
	}
	else if(hWndCtl == m_cbSize.m_hWnd){
		//
	}
	return 0;
}


void CBottomFrame::ReloadFadeCombo(void)
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

void CBottomFrame::SetParams(CInputParams &p)
{
	m_tb.CheckButton(ID_EDIT_BOLD,p.bold);
	m_tb.CheckButton(ID_EDIT_ITALIC,p.italic);
	m_tb.CheckButton(ID_EDIT_UNDERLINE,p.underline);
	m_tb.CheckButton(ID_EDIT_FADE,p.fade);
	m_tb.CheckButton(ID_EDIT_BLEND,p.blend);

	if(!p.font.IsEmpty()){
		int index = m_cbFonts.FindStringExact(-1,p.font);
		if(index >= 0)
			m_cbFonts.SetCurSel(index);
	}
	if(!p.colorset.IsEmpty()){
		int index = m_cbFade.FindStringExact(-1,p.colorset);
		if(index >= 0)
			m_cbFade.SetCurSel(index);
	}
	if(p.font_size > 0){
		TCHAR buf[3] = {0};
		int index;

		wsprintf(buf,_T("%02d"),p.font_size);
		index = m_cbSize.FindStringExact(-1,buf);
		if(index >= 0)
			m_cbSize.SetCurSel(index);
	}
	m_cbColors.SelectColor(p.color);
}

void CBottomFrame::GetParams(CInputParams &p)
{
	// get bold,italic,under,fade,alt state
	p.bold = m_tb.IsButtonChecked(ID_EDIT_BOLD) ? true : false;
	p.italic = m_tb.IsButtonChecked(ID_EDIT_ITALIC)  ? true : false;
	p.underline = m_tb.IsButtonChecked(ID_EDIT_UNDERLINE)  ? true : false;
	p.fade = m_tb.IsButtonChecked(ID_EDIT_FADE)  ? true : false;
	p.blend = m_tb.IsButtonChecked(ID_EDIT_BLEND)  ? true : false;
	// get selected color
	p.color = m_cbColors.GetSelectedColor();
	// get selected font
	int index = m_cbFonts.GetCurSel();
	if(index >= 0){
		m_cbFonts.GetLBText(index,p.font);
	}
	// get selected font size
	index = m_cbSize.GetCurSel();
	if(index >= 0){
		p.font_size = (int) m_cbSize.GetItemData(index);
	}
	// get selected color set name
	index = m_cbFade.GetCurSel();
	if(index >= 0){
		m_cbFade.GetLBText(index,p.colorset);
	}
}

CToolBarCtrl & CBottomFrame::GetFontToolbar(void)
{
	return m_tb;
}

void CBottomFrame::SetColorsetName(LPCTSTR name,BOOL bNotifySink)
{
	int index = m_cbFade.FindStringExact(-1,name);

	if(index >= 0)
		m_cbFade.SetCurSel(index);
	else{
		index = m_cbFade.AddString(name);
		m_cbFade.SetCurSel(index);
	}
	if(bNotifySink && m_pSink)
		m_pSink->OnColorsetChange(name);

}

CString CBottomFrame::GetColorsetName(void)
{
	int index = m_cbFade.GetCurSel();
	CString s;

	if(index >= 0){
		m_cbFade.GetLBText(index,s);
	}
	return s;
}

void CBottomFrame::SetSink(IInputFrameEvents *pSink)
{
	m_pSink = pSink;
}

int CBottomFrame::GetDesiredHeight(void)
{
	int cx = ::SendMessage(m_hWndToolBar,RB_GETBARHEIGHT,0,0);

	if(m_view1)
		cx += m_view1->GetDesiredHeight();
	else
		cx += 50;

	//cx += GetTabAreaHeight();

	return cx;
}

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