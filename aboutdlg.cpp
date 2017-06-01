// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "aboutdlg.h"
#include <no5tl\versioninfo.h>
#include "no5app.h"

LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	CWindow wnd = GetDlgItem(IDC_STATIC1);
	NO5TL::CVersionInfo vi;

	if(vi.Init(g_app.GetModuleFileName())){
		CString s,tmp;
		const int cp = 0x40904b0;
		if(vi.GetString(cp,"ProductName",tmp))
			s += tmp;
		if(vi.GetString(cp,"FileDescription",tmp)){
			s += "\r\n\r\n";
			s += tmp;
		}
		if(vi.GetString(cp,"ProductVersion",tmp)){
			s += "\r\n\r\n";
			s += tmp;
		}
		wnd.SetWindowText(s);
	}
	CString sPage,sEmail,s;
	sPage.LoadString(IDS_HOMEPAGE);
	sEmail.LoadString(IDS_EMAIL);

	m_hlPage.SubclassWindow(GetDlgItem(IDC_STATIC2));
	m_hlEmail.SubclassWindow(GetDlgItem(IDC_STATIC3));
	m_hlThanks.SubclassWindow(GetDlgItem(IDC_STATIC4));

	s.Format("mailto:%s",(LPCTSTR)sEmail);
	m_hlPage.SetLabel(sPage);
	m_hlPage.SetHyperLink(sPage);
	m_hlEmail.SetLabel(sEmail);
	m_hlEmail.SetHyperLink(s);

	sPage += "thanks.php";
	m_hlThanks.SetLabel("thanks");
	m_hlThanks.SetHyperLink(sPage);

	return TRUE;
}

LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}
