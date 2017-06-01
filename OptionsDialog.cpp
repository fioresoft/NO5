// OptionsDialog.cpp: implementation of the COptionsDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ctime>
#include <no5tl\mystring.h>
#include "resource.h"
#include "no5options.h"
#include "no5app.h"
#include "OptionsDialog.h"
#include "usrmsgs.h"
#include "ignorelist.h"
#include "no5stream.h"
#include "imainframe.h"

// bool to button state
#ifndef BOOL2BST
#define BOOL2BST(iState,bSet) (iState =( bSet ? BST_CHECKED : BST_UNCHECKED ) )
#endif

#ifndef BST2BOOL
#define BST2BOOL(iState,bSet) ( bSet = ( iState == BST_CHECKED ) )
#endif

/** CPmPage **/

LRESULT CPmPage::OnInitDialog(UINT,WPARAM,LPARAM,BOOL &bHandled)
{
	CPmOptions p;
	BOOL res;

	m_pgo = g_app.GetGeneralOptions();
	ATLASSERT(m_pgo);
	res = m_pgo->GetPmOptions(p);
	if(res){
		BOOL2BST(m_iPopup,p.m_popup);
		BOOL2BST(m_iFriendsOnly,p.m_friends);
		BOOL2BST(m_iNoLinks,p.m_nolinks);
		BOOL2BST(m_iTopmost,p.m_topmost);
	}
	else{
		ATLASSERT(0 && "todo");
	}
	DoDataExchange(DDX_LOAD);
	bHandled = FALSE;
	return 0;
}	

int CPmPage::OnApply(void)
{
	BOOL res;

	// PSNRET_NOERROR = apply OK
	// PSNRET_INVALID = apply not OK, return to this page
	// PSNRET_INVALID_NOCHANGEPAGE = apply not OK, don't change focus
	DoDataExchange(DDX_SAVE);

	CPmOptions p;

	BST2BOOL(m_iPopup,p.m_popup);
	BST2BOOL(m_iFriendsOnly,p.m_friends);
	BST2BOOL(m_iNoLinks,p.m_nolinks);
	BST2BOOL(m_iTopmost,p.m_topmost);

	res = m_pgo->SetPmOptions(p);
	ATLASSERT(res);

	return PSNRET_NOERROR;
}

/** CGeneralPage **/

LRESULT CGeneralPage::OnInitDialog(UINT,WPARAM,LPARAM,BOOL &bHandled)
{
	m_pgo = g_app.GetGeneralOptions();
	ATLASSERT(m_pgo);
	BOOL2BST(m_iTopmost,m_pgo->GetTopmost());
	BOOL2BST(m_iShowEnter,m_pgo->ShowEnter());

	m_iConnType = m_pgo->GetConnectionType();
	if(m_iConnType < 0 || m_iConnType > 2)
		m_iConnType = 1;
	UINT id = IDC_RADIO1 + (UINT)m_iConnType;
	CButton bt = GetDlgItem(id);
	bt.SetCheck(BST_CHECKED);

	DoDataExchange(DDX_LOAD);
	bHandled = FALSE;
	return 0;
}

int CGeneralPage::OnApply(void)
{
	DoDataExchange(DDX_SAVE);

	m_pgo->SetTopmost(m_iTopmost == BST_CHECKED);
	m_pgo->ShowEnter(m_iShowEnter == BST_CHECKED);

	for(UINT i=0;i<3;i++){
		UINT id = IDC_RADIO1 + i;
		CButton bt = GetDlgItem(id);
		if(bt.GetCheck() == BST_CHECKED){
			m_pgo->SetConnectionType(i);
			break;
		}
	}
	
	return PSNRET_NOERROR;
}

/** CAutoIgnorePage **/

LRESULT CAutoIgnorePage::OnInitDialog(UINT,WPARAM,LPARAM,BOOL &bHandled)
{
	CIgnoreOptions &io = g_app.GetIgnoreOptions();
	m_pgo = g_app.GetGeneralOptions();
	ATLASSERT(m_pgo);

	BOOL2BST(m_iAuto,io.m_auto);
	BOOL2BST(m_iFlood,io.m_flood);
	BOOL2BST(m_iSpam,io.m_spambots);
	BOOL2BST(m_iTags,io.m_badtags);
	BOOL2BST(m_iYBots,io.m_ybots);

	m_dwSpamTime = io.m_tJoin;
	m_dwAutoTime = io.m_days;
	m_dwpps = io.m_pps;

	if(io.m_pWords)
		m_sWords = io.m_pWords->MakeCommaList();

	DoDataExchange(DDX_LOAD);
	m_sWords.Empty();	// we dont really need this variable
	bHandled = FALSE;
	return 0;
}

int CAutoIgnorePage::OnApply(void)
{
	CIgnoreOptions &io = g_app.GetIgnoreOptions();

	DoDataExchange(DDX_SAVE);

	BST2BOOL(m_iAuto,io.m_auto);
	BST2BOOL(m_iFlood,io.m_flood);
	BST2BOOL(m_iSpam,io.m_spambots);
	BST2BOOL(m_iTags,io.m_badtags);
	BST2BOOL(m_iYBots,io.m_ybots);

	io.m_tJoin = m_dwSpamTime;
	io.m_days = m_dwAutoTime;
	io.m_pps = m_dwpps;

	if(!io.m_pWords)
		io.m_pWords = new NO5TL::CStringArray();
	else
		io.m_pWords->RemoveAll();
	io.m_pWords->AppendFromCommaList(m_sWords);

	g_app.WriteIgnoreOptions();

	return PSNRET_NOERROR;
}

/** CPermIgnorePage **/

LRESULT CIgnorePage::OnInitDialog(UINT,WPARAM,LPARAM,BOOL &bHandled)
{
	LVCOLUMN lvcol;
	LPCTSTR cols[] = { "users","expire ( days )"};
	CRect r;

	m_lv.Attach(GetDlgItem(IDC_LIST1));
	m_pgo = g_app.GetGeneralOptions();
	ATLASSERT(m_pgo);
	m_pIgnoreList = g_app.GetIgnoreList();
	ATLASSERT(m_pIgnoreList);

	m_days = g_app.GetIgnoreOptions().m_days_perm;
	DoDataExchange(DDX_LOAD);

	m_lv.GetClientRect(&r);
	// insert list view columns
	lvcol.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	lvcol.fmt = LVCFMT_LEFT;

	for(int i=0;i<sizeof(cols)/sizeof(cols[0]);i++){
		lvcol.cx = r.Width()/2;
		lvcol.pszText = const_cast<LPTSTR>(cols[i]);
		lvcol.iSubItem = i;
		m_lv.InsertColumn(i,&lvcol);
	}
	LoadList();

	bHandled = FALSE;
	return 0;
}

LRESULT CIgnorePage::OnRemoveUser(WORD,WORD,HWND,BOOL &)
{
	int index = GetCurSel();

	if(index >= 0){
		CString user;

		m_lv.GetItemText(index,0,user);
		m_lv.DeleteItem(index);
		if(!user.IsEmpty()){
			// there is no come-back
			m_pIgnoreList->remove(user);
			CWindow wnd = g_app.GetMainFrame()->GetHandle();
			// notify the frame which will notify any views
			wnd.SendMessage(NO5WM_UNIGNORE,(WPARAM)FALSE,(LPARAM)(LPCTSTR)user);
		}
	}
	return 0;
}

LRESULT CIgnorePage::OnRemoveAll(WORD,WORD,HWND,BOOL &)
{
	m_lv.DeleteAllItems();
	m_pIgnoreList->clear();
	CWindow wnd = g_app.GetMainFrame()->GetHandle();
	// notify the frame which will notify any views
	wnd.SendMessage(NO5WM_UNIGNORE,(WPARAM)TRUE);
	return 0;
}

int CIgnorePage::OnApply(void)
{
	DoDataExchange(DDX_SAVE);
	g_app.GetIgnoreOptions().m_days_perm = m_days;
	g_app.WriteIgnoreOptions();

	return PSNRET_NOERROR;
}

void CIgnorePage::LoadList(void)
{
	int count;
	int i;
	long t;
	bool res;
	LVITEM item;
	CString s;
	LPCTSTR pszText;
	time_t cur = time(NULL);
	long days;


	count = m_pIgnoreList->size();
	m_lv.DeleteAllItems();
	m_lv.SetItemCount(count);

	for(i=0;i<count;i++){
		// get name and expiration date
		s.Empty();
		res = m_pIgnoreList->getat(i,s,t);
		ATLASSERT(res);
		// add name to list view
		ZeroMemory(&item,sizeof(item));
		item.mask = LVIF_TEXT;
		item.iItem = i;
		pszText = LPCTSTR(s);
		item.pszText = const_cast<LPTSTR>(pszText);
		m_lv.InsertItem(&item);
		// add date
		if(t > 0){
			days = long(t - cur)/( 24 * 60 * 60 );
		}
		else
			days = t;
		s.Format("%d",days);
		m_lv.SetItemText(i,1,s);
	}
}

int CIgnorePage::GetCurSel(void)
{
	return m_lv.GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);
}

/** CPermIgnorePage **/

CVisualPage::CVisualPage()
{
	m_pvo = NULL;
}

CVisualPage::~CVisualPage()
{
	ATLASSERT(m_pvo == NULL);
}

LRESULT CVisualPage::OnInitDialog(UINT,WPARAM,LPARAM,BOOL &bHandled)
{
	// create a local IVisualOptions instance. this will be copied to the app's instance
	// in case the user chooses to apply the changes
	m_pvo = IVisualOptions::CreateMe();
	ATLASSERT(m_pvo);
	m_pvo->SetFile(g_app.GetVisualOptionsFile());
	m_pvo->CopyFrom(g_app.GetVisualOptions());

	m_cb.Attach(GetDlgItem(IDC_COMBO1));

	DoDataExchange(DDX_LOAD);
	LoadCombo();
	CreatePreview();
	m_cb.SetCurSel(0);
	BOOL bDummy; OnComboSelChange(0,0,0,bDummy);
	UpdatePreview();

	return 0;
}

LRESULT CVisualPage::OnDestroy(UINT,WPARAM,LPARAM,BOOL &bHandled)
{
	// do not write this
	m_pvo->Dirty(false);
	IVisualOptions::DestroyMe(&m_pvo);
	return 0;
}



LRESULT CVisualPage::OnComboSelChange(WORD,WORD,HWND,BOOL &)
{
	UpdatePreview();
	return 0;
}

LRESULT  CVisualPage::OnRestoreAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	IVisualOptions *p = NULL;

	CreateDefault(&p);
	m_pvo->CopyFrom(p);
	p->Dirty(false);
	IVisualOptions::DestroyMe(&p);
	UpdatePreview();
	SetModified(TRUE);
	return 0;
}

LRESULT  CVisualPage::OnColorChoose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	UINT id = GetItemId();

	if(id){
		FontAll f;
		GetItemFont(id,f);
		{
			CColorDialog dlg(f.m_clr);

			if(IDOK == dlg.DoModal()){
				f.m_clr = dlg.GetColor();
				SetItemFont(id,f);
				UpdatePreview();
				SetModified(TRUE);
			}
		}
	}

	return 0;
}

LRESULT  CVisualPage::OnColorDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	IVisualOptions *p = NULL;
	FontAll f,f2;
	const UINT id = GetItemId();

	CreateDefault(&p);
	// get the default values
	GetItemFont(id,f,p);
	// get the current values
	GetItemFont(id,f2);
	// keep the current values changing the colour
	f2.m_clr = f.m_clr;
	// set the new values
	SetItemFont(id,f2);
	p->Dirty(false);
	IVisualOptions::DestroyMe(&p);
	UpdatePreview();
	SetModified(TRUE);
	return 0;
}

LRESULT  CVisualPage::OnFontChoose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	UINT id = GetItemId();

	if(id){
		FontAll f;
		GetItemFont(id,f);
		LOGFONT lf = {0};

		{
			CClientDC dc(m_hWnd);
			lf.lfHeight = -MulDiv(f.m_size, GetDeviceCaps(dc, LOGPIXELSY), 72);
		}
		if(f.m_bold)
			lf.lfWeight = FW_BOLD;
		lf.lfItalic = f.m_italic ? TRUE : FALSE;
		lf.lfUnderline = f.m_under ? TRUE : FALSE;
		lstrcpyn(lf.lfFaceName,(LPCTSTR)(f.m_face),sizeof(lf.lfFaceName));

		CFontDialog dlg(&lf);

		if(IDOK == dlg.DoModal()){
			f.m_size = dlg.GetSize() / 10;
			f.m_bold = dlg.IsBold() ? true : false;
			f.m_under = dlg.IsUnderline() ? true : false;
			f.m_italic = dlg.IsItalic() ? true : false;

			SetItemFont(id,f);
			UpdatePreview();
			SetModified(TRUE);
		}
	}
	return 0;

}

LRESULT  CVisualPage::OnFontDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	IVisualOptions *p = NULL;
	FontAll f,f2;
	const UINT id = GetItemId();

	CreateDefault(&p);
	// get the default values
	GetItemFont(id,f,p);
	// get the current values
	GetItemFont(id,f2);
	// keep the current values changing font attributes only
	f2.m_bold = f.m_bold;
	f2.m_italic = f.m_italic;
	f2.m_under = f.m_under;
	f2.m_size = f.m_size;
	f2.m_face = f.m_face;
	// set the new values
	SetItemFont(id,f2);
	p->Dirty(false);
	IVisualOptions::DestroyMe(&p);
	UpdatePreview();
	SetModified(TRUE);
	return 0;
}

LRESULT  CVisualPage::OnBackChoose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CColorDialog dlg(m_pvo->ClrBack());

	if(IDOK == dlg.DoModal()){
		m_pvo->ClrBack(dlg.GetColor());
		UpdatePreview();
		SetModified(TRUE);
	}
	return 0;
}

LRESULT  CVisualPage::OnBackDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	IVisualOptions *p = NULL;

	CreateDefault(&p);
	m_pvo->ClrBack(p->ClrBack());
	p->Dirty(false);
	IVisualOptions::DestroyMe(&p);
	UpdatePreview();
	SetModified(TRUE);
	return 0;
}

LRESULT  CVisualPage::OnClrBkChoose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	UINT id = GetItemId();

	if(id){
		FontAll f;
		GetItemFont(id,f);
		{
			CColorDialog dlg(f.m_clrBack);

			if(IDOK == dlg.DoModal()){
				f.m_clrBack = dlg.GetColor();
				SetItemFont(id,f);
				UpdatePreview();
				SetModified(TRUE);
			}
		}
	}
	return 0;
}

LRESULT  CVisualPage::OnClrBkDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	IVisualOptions *p = NULL;
	FontAll f,f2;
	const UINT id = GetItemId();

	CreateDefault(&p);
	// get the default values
	GetItemFont(id,f,p);
	// get the current values
	GetItemFont(id,f2);
	// keep the current values changing the text back colour only
	f2.m_clrBack = f.m_clrBack;
	// set the new values
	SetItemFont(id,f2);
	p->Dirty(false);
	IVisualOptions::DestroyMe(&p);
	UpdatePreview();
	SetModified(TRUE);
	return 0;
}


void CVisualPage::LoadCombo(void)
{
	UINT uFirst = IDS_NAME_USR;
	UINT uLast = IDS_LEAVE_BUD;
	CString s;
	UINT i;
	UINT id;
	int index;

	m_cb.ResetContent();
	for(i=0,id=uFirst;id <= uLast;i++,id++){
		s.Empty();
		if(s.LoadString(id)){
			index = m_cb.AddString(s);
			if(index >= 0)
				m_cb.SetItemData(index,(DWORD)id);
		}
	}
}

void CVisualPage::CreatePreview(void)
{
	CWindow wnd = GetDlgItem(IDC_STATIC_PREVIEW);
	CRect r;

	m_edit.Create(m_hWnd,rcDefault,NULL,WS_CHILD|ES_AUTOHSCROLL|WS_BORDER|ES_READONLY);
	//m_edit.SetBackgroundColor(0);
	//m_edit.SetTextColor(0xFFFFFF,SCF_ALL);
	//m_edit.LimitText(100);

	// hide static control and move the rich there
	wnd.GetWindowRect(&r);
	ScreenToClient(&r);
	wnd.ShowWindow(SW_HIDE);
	m_edit.MoveWindow(&r);
	m_edit.ShowWindow(SW_SHOWNORMAL);
}

int CVisualPage::OnApply(void)
{
	DoDataExchange(DDX_SAVE);
	IVisualOptions *p = g_app.GetVisualOptions();
	ATLASSERT(p);
	if(p){
		p->CopyFrom(m_pvo);
	}
	return PSNRET_NOERROR;
}

void CVisualPage::UpdatePreview(void)
{
	CRichEditStream ts(&m_edit);
	

	m_edit.ClearAll();
	m_edit.SetBackgroundColor(m_pvo->ClrBack());

	switch(GetItemId()){
		case IDS_NAME_USR:
		case IDS_TXT_USR:
			ts << m_pvo->NameUser() << "user: " << m_pvo->TxtUser() << "hello world";
			break;
		case IDS_NAME_BUD:
		case IDS_TXT_BUD:
			ts << m_pvo->NameBuddy() << "user: " << m_pvo->TxtBuddy() << "hello world";
			break;
		case IDS_NAME_ME:
		case IDS_TXT_ME:
			ts << m_pvo->NameMe() << "user: " << m_pvo->TxtMe() << "hello world";
			break;
		case IDS_TXT_EMOTE:
			ts << m_pvo->NameUser() << "user " << m_pvo->TxtEmote() << " is bored";
			break;
		case IDS_ENTER_USR:
			ts << m_pvo->TxtEnterUsr() << "user enters.";
			break;
		case IDS_ENTER_BUD:
			ts << m_pvo->TxtEnterBuddy() << "buddy enters.";
			break;
		case IDS_LEAVE_USR:
			ts << m_pvo->TxtLeaveUsr() << "user leaves.";
			break;
		case IDS_LEAVE_BUD:
			ts << m_pvo->TxtLeaveBuddy() << "buddy leaves.";
			break;
		default:
			break;
	}
	ts.SendToDest();
}

UINT CVisualPage::GetItemId(void)
{
	UINT id = 0;
	int i = m_cb.GetCurSel();
	if(i >= 0){
		id = (UINT)m_cb.GetItemData(i);
	}
	return id;
}

// get the FontAll for the requested id , from the pvo options
void CVisualPage::GetItemFont(UINT id,FontAll &f,IVisualOptions *pvo)
{
	switch(id){
		case IDS_NAME_USR:
			f = pvo->NameUser();
			break;
		case IDS_NAME_BUD:
			f = pvo->NameBuddy();
			break;
		case IDS_NAME_ME:
			f = pvo->NameMe();
			break;
		case IDS_TXT_USR:
			f = pvo->TxtUser();
			break;
		case IDS_TXT_BUD:
			f = pvo->TxtBuddy();
			break;
		case IDS_TXT_ME:
			f = pvo->TxtMe();
			break;
		case IDS_TXT_EMOTE:
			f = pvo->TxtEmote();
			break;
		case IDS_ENTER_USR:
			f = pvo->TxtEnterUsr();
			break;
		case IDS_ENTER_BUD:
			f = pvo->TxtEnterBuddy();
			break;
		case IDS_LEAVE_USR:
			f = pvo->TxtLeaveUsr();
			break;
		case IDS_LEAVE_BUD:
			f = pvo->TxtLeaveBuddy();
			break;
		default:
			break;
	}
}

// reverse
void CVisualPage::SetItemFont(UINT id,FontAll &f,IVisualOptions *pvo)
{
	switch(id){
		case IDS_NAME_USR:
			pvo->NameUser(f);
			break;
		case IDS_NAME_BUD:
			pvo->NameBuddy(f);
			break;
		case IDS_NAME_ME:
			pvo->NameMe(f);
			break;
		case IDS_TXT_USR:
			pvo->TxtUser(f);
			break;
		case IDS_TXT_BUD:
			pvo->TxtBuddy(f);
			break;
		case IDS_TXT_ME:
			pvo->TxtMe(f);
			break;
		case IDS_TXT_EMOTE:
			pvo->TxtEmote(f);
			break;
		case IDS_ENTER_USR:
			pvo->TxtEnterUsr(f);
			break;
		case IDS_ENTER_BUD:
			pvo->TxtEnterBuddy(f);
			break;
		case IDS_LEAVE_USR:
			pvo->TxtLeaveUsr(f);
			break;
		case IDS_LEAVE_BUD:
			pvo->TxtLeaveBuddy(f);
			break;
		default:
			break;
	}
}

void CVisualPage::GetItemFont(UINT id,FontAll &f)
{
	GetItemFont(id,f,m_pvo);
}

void CVisualPage::SetItemFont(UINT id,FontAll &f)
{
	SetItemFont(id,f,m_pvo);
}


void CVisualPage::CreateDefault(IVisualOptions **pp)
{
	ATLASSERT(pp && !(*pp));
	*pp = IVisualOptions::CreateMe();
	(*pp)->LoadDefaults();
}

/** COptionsPropertySheet **/


LRESULT COptionsPropertySheet::OnDestroy(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)

{
	CWindow wnd = GetParent();

	wnd.PostMessage(NO5WM_OPTIONSDLG);
	return 0;
}

void COptionsPropertySheet::AddPages(void)
{
	AddPage(&(m_pageGeneral.m_psp));
	AddPage(&(m_pagePm.m_psp));
	AddPage(&(m_pageAuto.m_psp));
	AddPage(&(m_pageIgnore.m_psp));
	AddPage(&(m_pageVisual.m_psp));
}