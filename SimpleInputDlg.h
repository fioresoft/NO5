// SimpleInputDlg.h: interface for the CSimpleInputDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLEINPUTDLG_H__85DF3529_CF6E_4523_A991_D26E351E6D8F__INCLUDED_)
#define AFX_SIMPLEINPUTDLG_H__85DF3529_CF6E_4523_A991_D26E351E6D8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlddx.h>

class CSimpleInputDlg : \
	public CDialogImpl<CSimpleInputDlg>,
	public CWinDataExchange<CSimpleInputDlg>
{
public:
	enum { IDD = IDD_INPUT };

	BEGIN_MSG_MAP(CSimpleInputDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CSimpleInputDlg)
		DDX_TEXT_LEN(IDC_EDIT1,m_text,m_max)
	END_DDX_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	CSimpleInputDlg(void)
	{
		m_max = 0;
		m_bInit = false;
	}
	void Init(LPCTSTR title,LPCTSTR label,int maxchars,LPCTSTR txt = NULL)
	{
		m_text = txt;
		m_label = label;
		m_title = title;
		m_max = maxchars;
		m_bInit = true;
	}

	CString m_text;
	CString m_label;
	CString m_title;
	int m_max;	// max input chars
	bool m_bInit;
};

#endif // !defined(AFX_SIMPLEINPUTDLG_H__85DF3529_CF6E_4523_A991_D26E351E6D8F__INCLUDED_)
