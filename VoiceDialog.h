// VoiceDialog.h: interface for the CVoiceDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOICEDIALOG_H__0101E263_BFD8_4582_B9E1_BAA167321432__INCLUDED_)
#define AFX_VOICEDIALOG_H__0101E263_BFD8_4582_B9E1_BAA167321432__INCLUDED_

#import "C:\Arquivos de programas\Yahoo!\Messenger\yacscom.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids  
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlctrlx.h>

class CVoiceDialog : \
	public CAxDialogImpl<CVoiceDialog>,
	public CMessageFilter,
	public IDispEventImpl<1,CVoiceDialog,&DIID__IYAcsEvents,&LIBID_YACSCOMLib,1,0>,
	public CDialogResize<CVoiceDialog>
{
	typedef IDispEventImpl<1,CVoiceDialog,&DIID__IYAcsEvents,&LIBID_YACSCOMLib,1,0> YacsSource;
public:
	CVoiceDialog();
	virtual ~CVoiceDialog();

	enum { IDD = IDD_VOICE };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(CVoiceDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCDESTROY,OnNCDestroy)
		MESSAGE_HANDLER(WM_CLOSE,OnClose)
		MESSAGE_HANDLER(WM_CTLCOLORBTN,OnCtlColorBtn)
		COMMAND_HANDLER(IDC_BUTTON_CONNECT,BN_CLICKED,OnButtonConnect)
		COMMAND_HANDLER(IDC_CHECK_FREE,BN_CLICKED,OnButtonFree)
		NOTIFY_HANDLER(IDC_LIST1,LVN_ITEMCHANGED,OnLVItemChanged)
		CHAIN_MSG_MAP( CDialogResize<CVoiceDialog> )
	ALT_MSG_MAP(1)	// button talk messages
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnTalkDown)
		MESSAGE_HANDLER(WM_LBUTTONUP,OnTalkUp)
		MESSAGE_HANDLER(WM_CAPTURECHANGED,OnTalkCaptureChanged)
	END_MSG_MAP()

	BEGIN_SINK_MAP(CVoiceDialog)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x1, OnRemoteSourceOnAir)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x10, OnInputMuteChange)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x11, OnMonitorGainChange)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x12, OnInputSourceChange)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x13, OnMonitorMuteChange)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x14, OnSystemConnectFailure)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x15, OnAudioError)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x16, OnSourceMuted)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x17, OnSourceUnmuted)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x18, OnTransmitReport)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x2, OnRemoteSourceOffAir)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x3, OnConferenceReady)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x4, OnSourceEntry)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x5, OnSourceExit)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x6, OnConferenceNotReady)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x7, OnLocalOnAir)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x8, OnLocalOffAir)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0x9, OnInputLevelChange)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0xa, OnOutputLevelChange)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0xb, OnSystemConnect)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0xc, OnSystemDisconnect)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0xd, OnOutputGainChange)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0xe, OnOutputMuteChange)
		SINK_ENTRY_EX(1,DIID__IYAcsEvents, 0xf, OnInputGainChange)
	END_SINK_MAP()

	BEGIN_DLGRESIZE_MAP(CVoiceDialog)
		DLGRESIZE_CONTROL(IDC_LABEL_TALKING,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_PROGRESS_INPUT,DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_LIST1,DLSZ_SIZE_X|DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_PROGRESS_OUTPUT,DLSZ_MOVE_X|DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_CHECK_FREE,DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_BUTTON_TALK,DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_LABEL_REPORT,DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_BUTTON_CONNECT,DLSZ_MOVE_Y|DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_STATUS,DLSZ_MOVE_Y|DLSZ_SIZE_X)
	END_DLGRESIZE_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnNCDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnTalkDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnTalkUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnTalkCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnCtlColorBtn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnButtonConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnButtonFree(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnLVItemChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled);
	
public:

	HRESULT __stdcall OnRemoteSourceOnAir(LONG sourceId, BSTR sourceName);
	HRESULT __stdcall OnInputMuteChange(LONG mute);
	HRESULT __stdcall OnMonitorGainChange(SHORT gain);
	HRESULT __stdcall OnInputSourceChange(SHORT source);
	HRESULT __stdcall OnMonitorMuteChange(LONG mute);
	HRESULT __stdcall OnSystemConnectFailure(LONG code, BSTR message);
	HRESULT __stdcall OnAudioError(LONG code, BSTR message);
	HRESULT __stdcall OnSourceMuted(LONG numMuting, BSTR sourceName);
	HRESULT __stdcall OnSourceUnmuted(LONG numMuting, BSTR sourceName);
	HRESULT __stdcall OnTransmitReport(INT numReceiving, INT numTotal);
	HRESULT __stdcall OnRemoteSourceOffAir(LONG sourceId, BSTR sourceName);
	HRESULT __stdcall OnConferenceReady();
	HRESULT __stdcall OnSourceEntry(LONG sourceId, BSTR sourceName);
	HRESULT __stdcall OnSourceExit(LONG sourceId, BSTR sourceName);
	HRESULT __stdcall OnConferenceNotReady();
	HRESULT __stdcall OnLocalOnAir();
	HRESULT __stdcall OnLocalOffAir();
	HRESULT __stdcall OnInputLevelChange(SHORT level);
	HRESULT __stdcall OnOutputLevelChange(SHORT level);
	HRESULT __stdcall OnSystemConnect();
	HRESULT __stdcall OnSystemDisconnect();
	HRESULT __stdcall OnOutputGainChange(SHORT gain);
	HRESULT __stdcall OnOutputMuteChange(LONG mute);
	HRESULT __stdcall OnInputGainChange(SHORT gain);

public:
	CString m_server;
	CString m_name;
	CString m_cookie;
	CString m_room;
	CString m_rmid;
private:
	CAxWindow m_ax;
	CComPtr<IYAcs> m_sp;
	CCheckListViewCtrl m_lv;
	CWindow m_status;
	CProgressBarCtrl m_pbIn,m_pbOut;
	CSimpleMap<CString,LONG> m_users;
	CContainedWindowT<CButton> m_btTalk;
	CButton m_btConn;
	bool m_bConnected;
	CBrush m_brush;		// brush for talk button to be green
	CImageList m_ImgList;
	//
	void CreateTheImgList(void);
	void Connect(void);
	void Disconnect(void);
	void SwitchConnect(void);
	int FindLVItem(BSTR name);
};

#endif // !defined(AFX_VOICEDIALOG_H__0101E263_BFD8_4582_B9E1_BAA167321432__INCLUDED_)
