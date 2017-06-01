// SimpleInputDlg.cpp: implementation of the CSimpleInputDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "SimpleInputDlg.h"

LRESULT CSimpleInputDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	ATLASSERT(m_bInit);
	DoDataExchange(DDX_LOAD);
	SetWindowText(m_title);

	CWindow wnd = GetDlgItem(IDC_LABEL1);
	wnd.SetWindowText(m_label);
	return 0;
}

LRESULT CSimpleInputDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE);
	EndDialog(IDOK);
	return 0;
}

LRESULT CSimpleInputDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return 0;
}

