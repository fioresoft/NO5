// LoginDlg.h: interface for the CLoginDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGINDLG_H__BEE1A201_83CE_11D9_A17B_FDD37F9FE62B__INCLUDED_)
#define AFX_LOGINDLG_H__BEE1A201_83CE_11D9_A17B_FDD37F9FE62B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlddx.h>

struct IGeneralOptions; // fwd decl
struct IAccountOptions;

class CLoginDlg  :	public CDialogImpl<CLoginDlg>
{
public:
	CLoginDlg(void);
	virtual ~CLoginDlg();

	enum { IDD = IDD_LOGINDLG };

	enum Protocol
	{
		PROTO_YCHT,
		PROTO_YMSG,
		PROTO_NONE,	// just an end of list marker
	};
	enum ViewType
	{
		VIEW_HTML,
		VIEW_RTF,
		VIEW_TXT,
		VIEW_NONE,	// just an end of list marker
	};

	BEGIN_MSG_MAP(CLoginDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_COMBO_IDS, CBN_EDITCHANGE, OnComboEditChange)
		COMMAND_HANDLER(IDC_COMBO_IDS, CBN_EDITUPDATE, OnComboEditUpdate)
		COMMAND_HANDLER(IDC_COMBO_IDS, CBN_SELCHANGE, OnAccountSelChange)
		COMMAND_HANDLER(IDC_COMBO_PORT, CBN_SELCHANGE, OnPortsSelChange)
		COMMAND_HANDLER(IDC_COMBO_SERVER, CBN_CLOSEUP, OnServersSelChange)
		COMMAND_HANDLER(IDC_COMBO_PROTO,CBN_SELCHANGE,OnProtoSelChange)
		COMMAND_HANDLER(IDC_COMBO_VIEW,CBN_SELCHANGE,OnViewSelChange)
		COMMAND_HANDLER(IDC_ID_DELETE, BN_CLICKED, OnAccountDelete)
		COMMAND_HANDLER(IDC_ROOM_SELECT, BN_CLICKED, OnRoomSelect)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnCancel)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnOK)
	END_MSG_MAP()


	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnComboEditChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnComboEditUpdate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAccountSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPortsSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnServersSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnProtoSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnViewSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAccountDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnRoomSelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
public:
	CString m_acc;
	CString m_pw;
	CString m_room;
	CString m_server;
	int m_iView;
	int m_iProto;
	int m_port;
	CComboBox m_cbAccs;
	CComboBox m_cbServers;
	CComboBox m_cbPorts;
	CComboBox m_cbProtos;
	CComboBox m_cbViews;
	CComboBox m_cbRoom;
private:
	IGeneralOptions *m_po;
	IAccountOptions *m_pa;
	void FillComboBox(UINT nID,const CSimpleArray<CString> &lst,bool bReset = true);
	void FillComboBox(UINT nID,const CSimpleValArray<int> &lst,bool bReset = true);
	static LPCTSTR GetProtoName(int proto);
	static LPCTSTR GetViewName(int view);
	void SaveServers(void);
	void SavePorts(void);
	void LoadProtoCombo(void);
	void LoadViewCombo(void);
	void LoadRoomCombo(void);
	void ReadRoom(void);	// reads the room name into m_room
	void ReadPw(void);		// read pw name into m_pw
	// load account names from ini file
	virtual void LoadAccounts(void);
	virtual void OnProtoChange(void);
	virtual void OnAccountChange(void);
};


#endif // !defined(AFX_LOGINDLG_H__BEE1A201_83CE_11D9_A17B_FDD37F9FE62B__INCLUDED_)
