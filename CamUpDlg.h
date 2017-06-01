// CamUpDlg.h: interface for the CCamUpDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMUPDLG_H__ECD94C8B_E12D_4800_9B41_6F006FF65EE1__INCLUDED_)
#define AFX_CAMUPDLG_H__ECD94C8B_E12D_4800_9B41_6F006FF65EE1__INCLUDED_

#import "C:\Documents and Settings\usuario\Meus documentos\YCABDLL\yuplapp.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids  
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// yuplapp.dll

class CCamUpDlg : \
	public CAxDialogImpl<CCamUpDlg>,
	public CMessageFilter,
	public CDialogResize<CCamUpDlg>,
	public IDispEventImpl<IDC_UPLDLG1, CCamUpDlg>
{
public:
	CCamUpDlg();
	virtual ~CCamUpDlg();

	enum { IDD = IDD_CAMUP };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(CCamUpDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCDESTROY,OnNCDestroy)
		MESSAGE_HANDLER(WM_CLOSE,OnClose)
		CHAIN_MSG_MAP( CDialogResize<CCamUpDlg> )
	END_MSG_MAP()

	BEGIN_SINK_MAP(CCamUpDlg)
		//Make sure the Event Handlers have __stdcall calling convention
		SINK_ENTRY(IDC_UPLDLG1, 0x1, OnTokenExpired)
		SINK_ENTRY(IDC_UPLDLG1, 0x2, OnResize)
		SINK_ENTRY(IDC_UPLDLG1, 0x3, OnInitFailed)
		SINK_ENTRY(IDC_UPLDLG1, 0x4, OnUpDlgClose)
	END_SINK_MAP()

	BEGIN_DLGRESIZE_MAP(CCamUpDlg)
		DLGRESIZE_CONTROL(IDC_UPLDLG1,DLSZ_SIZE_X|DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnNCDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

public:
	long m_app;
	CString m_user;
	CString m_token;
	CString m_country;
	CString m_server;
private:
	CAxWindow m_ax;
	CComPtr<IUplDlg> m_sp;

	HRESULT __stdcall OnTokenExpired();
	HRESULT __stdcall OnResize(LONG iNewWidth, LONG iNewHeight);
	HRESULT __stdcall OnInitFailed(LONG iErrCode, BSTR bstrError);
	HRESULT __stdcall OnUpDlgClose();

	HRESULT Connect(void);

};

#endif // !defined(AFX_CAMUPDLG_H__ECD94C8B_E12D_4800_9B41_6F006FF65EE1__INCLUDED_)
