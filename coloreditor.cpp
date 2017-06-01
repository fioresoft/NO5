#include "stdafx.h"
#include "resource.h"
#include "no5stream.h"
#include "colorfile.h"
#include "coloreditor.h"
#include <no5tl\mystring.h>

using NO5TL::CWindowTextString;
using NO5YAHOO::CTextAtom;

CColorEditorDlg::CColorEditorDlg(void):m_preview(this,1)
{
	m_pcf = NULL;
	ColorFileCreate(&m_pcf);
}

CColorEditorDlg::~CColorEditorDlg()
{
	ColorFileDestroy(&m_pcf);
	ATLTRACE("~CColorEditorDlg\n");
}

LRESULT CColorEditorDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	int i;
	BOOL res;

	for(i=0;i<3;i++){
		m_bars[i].Attach(GetDlgItem(IDC_SCROLLBAR1  + i));
		m_bars[i].SetScrollRange(0,255,FALSE);
		m_bars[i].SetScrollPos(0,TRUE);
			//
		m_grads[i].SetColorBar(i);
		m_grads[i].SubclassWindow(GetDlgItem(IDC_STATIC_RED + i));
	}
	m_wndColor.SubclassWindow(GetDlgItem(IDC_STATIC_COLOR));
	m_wndColor.ModifyStyle(0,SS_OWNERDRAW);
	m_lbColors.SubclassWindow(GetDlgItem(IDC_LIST_COLORS));
	CreatePreview();
	SetColor(m_color);
	m_cbNames.Attach(GetDlgItem(IDC_COMBO_NAMES));

	m_pcf->SetFileName(m_ColorFilePath);
	res = FillColorSetCombo();
	ATLASSERT(res);
	res = LoadColorSet();
	ATLASSERT(res);
	FillColorListBox();
	UpdatePreview();

	return (LRESULT)FALSE;
		
}

LRESULT CColorEditorDlg::OnNCDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CWindow wnd = GetParent();

	ATLASSERT(wnd.IsWindow());
	wnd.PostMessage(NO5WM_CLRDLGDEL,0,(LPARAM)this);
	bHandled = FALSE;
	return 0;
}

LRESULT CColorEditorDlg::OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam,
		BOOL& bHandled)
{
	const int code = (int)(short int)LOWORD(wParam);
	const UINT id = ::GetWindowLong((HWND)lParam,GWL_ID);
	const int bar = (int)(id - IDC_SCROLLBAR1);
	int pos = (int)(short int)HIWORD(wParam);


	ATLASSERT(0 <= bar && bar < 3);
	if(!pos){
		pos = m_bars[bar].GetScrollPos();
	}

	switch(code){
		case SB_LINEDOWN:
			pos++;
			break;
		case SB_LINEUP:
			pos--;
			break;
		case SB_PAGEDOWN:
			pos += 30;
			break;
		case SB_PAGEUP:
			pos -= 30;
			break;
		case SB_THUMBPOSITION:
			break;
		case SB_THUMBTRACK:
			break;
	}
	pos = max(0,min(pos,255));
	m_color[bar] = pos;
	SetColor(m_color);
	return 0;
}

// the current color can be changed with the scroll bars or clicking in the
// fade strip. here we update anything that needs to be updated when a color changes
void CColorEditorDlg::SetColor(COLORREF clr)
{
	int i;
	CWindow wndCode = GetDlgItem(IDC_STATIC_CODE);

	m_color = clr;
	m_wndColor.SetColor(m_color);
	for(i=0;i<3;i++){
		m_grads[i].SetColor(m_color,true);
		m_bars[i].SetScrollPos(m_color[i]);
	}
	wndCode.SetWindowText(m_color.GetString(CYahooColor::YCSF_1));
}

void CColorEditorDlg::CreatePreview(void)
{
	CWindow wnd = GetDlgItem(IDC_STATIC1);
	CRect r;

	m_preview.Create(m_hWnd,rcDefault,NULL,WS_CHILD|ES_AUTOHSCROLL|WS_BORDER);
	m_preview.SetBackgroundColor(0);
	m_preview.SetTextColor(0xFFFFFF,SCF_ALL);
	m_preview.LimitText(100);

	// hide static control and move the rich there
	wnd.GetWindowRect(&r);
	ScreenToClient(&r);
	wnd.ShowWindow(SW_HIDE);
	m_preview.MoveWindow(&r);
	m_preview.ShowWindow(SW_SHOWNORMAL);
	m_preview.SetWindowText("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	UpdatePreview();
}

void CColorEditorDlg::UpdatePreview(void)
{
	if(m_preview.IsWindow()){
		CWindowTextString txt(m_preview);
		CRichEditStream ts;
		CTextAtom atom;
			
		ts.SetEdit(&m_preview);
		atom.AtomFont("arial",8,NULL,0xFFFFFF);
		ts.AddAtom(atom);
		if(m_colors.GetSize()){
			atom.AtomFade(m_colors);
			ts.AddAtom(atom);
		}
		ts.AddPlainText(txt);
		m_preview.SetWindowText("");
		ts.SendToDest();
	}
}

BOOL CColorEditorDlg::FillColorSetCombo(void)
{
	CSimpleArray<CString> names;
	BOOL res;
	
	m_cbNames.ResetContent();
	res = m_pcf->GetNames(names);
	if(res){
		for(int i=0;i<names.GetSize();i++){
			m_cbNames.AddString(names[i]);
		}
		if(!m_ColorSet.IsEmpty()){
			m_cbNames.SetCurSel(m_ColorSet);
		}
	}
	return res;
}

BOOL CColorEditorDlg::LoadColorSet(void)
{
	BOOL res = TRUE;
	
	if(!m_ColorSet.IsEmpty()){
		m_colors.RemoveAll();
		res = m_pcf->GetColorSet(m_ColorSet,m_colors);
	}
	return res;
}

void CColorEditorDlg::FillColorListBox(void)
{
	const int count = m_colors.GetSize();
	int i;

	m_lbColors.ResetContent();
	for(i=0;i<count;i++){
		m_lbColors.AddColor(m_lbColors.GetCount(),m_colors[i]);
	}
}

LRESULT CColorEditorDlg::OnFadeClick(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam,
		BOOL& bHandled)
{
	SetColor((COLORREF)wParam);
	return 0;
}

LRESULT CColorEditorDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	CWindow wnd = GetParent();
	wnd.SendMessage(NO5WM_COLORDLG,(WPARAM)wID,(LPARAM)this);
	DestroyWindow();
	return 0;	
} 

LRESULT CColorEditorDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	CWindow wnd = GetParent();
	wnd.SendMessage(NO5WM_COLORDLG,(WPARAM)wID,(LPARAM)this);
	DestroyWindow();
	return 0;
}

LRESULT CColorEditorDlg::OnColorAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	m_colors.Add(m_color);
	m_lbColors.AddColor(m_lbColors.GetCount(),m_color);
	UpdatePreview();
	return 0;
}

LRESULT CColorEditorDlg::OnColorRemove(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	int index = m_lbColors.GetCurSel();

	if(index >= 0){
		m_colors.RemoveAt(index);
		m_lbColors.DeleteString(index);
		UpdatePreview();
	}
	return 0;
}

LRESULT CColorEditorDlg::OnColorClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	m_colors.RemoveAll();
	m_lbColors.ResetContent();
	UpdatePreview();
	return 0;
}

LRESULT CColorEditorDlg::OnColorSetSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	CString name;

	m_cbNames >> name;
	if(!name.CompareNoCase("Random")){
		MessageBox("This name is reserved, choose another one");
	}
	else if(!name.IsEmpty()){
		BOOL res = m_pcf->AddColorSet(name,m_colors);
		if(res){
			m_ColorSet = name;
			// we may have added a new set or replaced an existing one
			// in all cases we get all names again and select this name
			res = FillColorSetCombo();
			if(res){
				res = LoadColorSet();
				FillColorListBox();
				m_cbNames.SetCurSel(name);
			}
		}
		ATLASSERT(res);
	}
	else
		MessageBox("please type a name and try again");
	return 0;
}

LRESULT CColorEditorDlg::OnColorSetDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	CString name;

	m_cbNames >> name;
	if(!name.IsEmpty()){
		BOOL res = m_pcf->DeleteColorSet(name);
		if(res){
			ATLASSERT(res);
			int index = m_cbNames.GetCurSel();
			if(index >= 0)
				m_cbNames.DeleteString(index);
			m_cbNames.SetCurSel(0);
			m_cbNames >> m_ColorSet;
			LoadColorSet();
			FillColorListBox();
			UpdatePreview();
		}
	}
	return 0;
}

LRESULT CColorEditorDlg::OnSelEndOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{	
	CString name;
	int index;

	index = m_cbNames.GetCurSel();
	if(index >= 0){			
		m_cbNames.GetLBText(index,name);
		if(!name.IsEmpty()){
			BOOL res;

			m_ColorSet = name;
			res = LoadColorSet();
			ATLASSERT(res);
			if(res){
				FillColorListBox();
				UpdatePreview();
			}
		}
	}
	return 0;
}
	// rich edit message handlers
LRESULT CColorEditorDlg::OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	
	UpdatePreview();
	bHandled = FALSE;
	return 0;
}
		