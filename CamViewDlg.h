// CamViewDlg.h: interface for the CCamViewDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMVIEWDLG_H__A0BDAD0A_9DC5_4F6C_803A_B19D0FAB4310__INCLUDED_)
#define AFX_CAMVIEWDLG_H__A0BDAD0A_9DC5_4F6C_803A_B19D0FAB4310__INCLUDED_

#import "C:\Arquivos de programas\Yahoo!\Messenger\ywcvwr.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids  
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <no5tl\avi.h>



class CCamViewDlg :\
	public CAxDialogImpl<CCamViewDlg>,
	public CMessageFilter,
	public IDispEventImpl<IDC_WCVIEWER1, CCamViewDlg>,
	public CDialogResize<CCamViewDlg>
{
public:
	CCamViewDlg();
	virtual ~CCamViewDlg();

	enum { IDD = IDD_CAMVIEW };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(CCamViewDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCDESTROY,OnNCDestroy)
		MESSAGE_HANDLER(WM_CLOSE,OnClose)
		COMMAND_ID_HANDLER(IDC_BUTTON1,OnButtonView)
		COMMAND_ID_HANDLER(IDC_SNAPSHOT,OnSnapshot)
		COMMAND_ID_HANDLER(IDC_RECORD,OnRecord)
		CHAIN_MSG_MAP( CDialogResize<CCamViewDlg> )
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CCamViewDlg)
		DLGRESIZE_CONTROL(IDC_STATIC1,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_WCVIEWER1,DLSZ_SIZE_X|DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_EDIT1,DLSZ_SIZE_X|DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_BUTTON1,DLSZ_SIZE_X|DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_SNAPSHOT,DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_RECORD,DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnNCDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnButtonView(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSnapshot(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRecord(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	HRESULT __stdcall OnConnectionStatusChanged(INT iStatusCode);
	HRESULT __stdcall OnFinishedReceiving(INT iErrCode);
	HRESULT __stdcall OnReceivedImage(INT iImageLen, LONG tTimeStamp);
	HRESULT __stdcall OnResize(LONG iNewWidth, LONG iNewHeight);
	HRESULT __stdcall OnRButtonDown();
	HRESULT __stdcall OnSuperModeChanged(LONG bSuper);
	HRESULT __stdcall OnHideSuper(LONG bHideSuper);
public :

BEGIN_SINK_MAP(CCamViewDlg)
	//Make sure the Event Handlers have __stdcall calling convention
	SINK_ENTRY(IDC_WCVIEWER1, 0x1, OnConnectionStatusChanged)
	SINK_ENTRY(IDC_WCVIEWER1, 0x2, OnFinishedReceiving)
	SINK_ENTRY(IDC_WCVIEWER1, 0x3, OnReceivedImage)
	SINK_ENTRY(IDC_WCVIEWER1, 0x4, OnResize)
	SINK_ENTRY(IDC_WCVIEWER1, 0x5, OnRButtonDown)
	SINK_ENTRY(IDC_WCVIEWER1, 0x6, OnSuperModeChanged)
	SINK_ENTRY(IDC_WCVIEWER1, 0x7, OnHideSuper)
END_SINK_MAP()

public:
	long m_app;
	CString m_user;
	long m_age;
	CString m_target;
	CString m_room;
	CString m_token;
	CString m_country;
	long m_isp;
	CString m_server;
private:
	CAxWindow m_ax;
	CComPtr<IWcViewer> m_sp;
	CEdit m_edit;
	CStatic m_st;
	bool m_recording;
	NO5TL::CAvi *m_pavi;

	HRESULT Connect(void);
	CString TmpShotName(void);
	CString TmpRecName(void);
	void StartRec(void);
	void StopRec(void);
	void Snapshot(void);

};

#endif // !defined(AFX_CAMVIEWDLG_H__A0BDAD0A_9DC5_4F6C_803A_B19D0FAB4310__INCLUDED_)
