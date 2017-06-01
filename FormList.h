// FormList.h: interface for the CFormList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMLIST_H__F179C288_4506_41D3_ACA2_545BB393CDEA__INCLUDED_)
#define AFX_FORMLIST_H__F179C288_4506_41D3_ACA2_545BB393CDEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFormList : \
	public CDialogImpl<CFormList>,
	public CMessageFilter,
	public CDialogResize<CFormList>
{
public:
	CFormList();
	virtual ~CFormList();

	enum { IDD = IDD_FORMLIST };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(CFormList)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(NO5WM_FORMVIEW, OnDeleteViewer)
		COMMAND_HANDLER(IDC_LIST1,LBN_DBLCLK,OnList1DClick)
		COMMAND_ID_HANDLER(ID_FORM_EDIT,OnBtEdit)
		COMMAND_ID_HANDLER(ID_FORM_RUN,OnBtRun)
		COMMAND_ID_HANDLER(ID_FORM_NEW,OnBtNew)
		COMMAND_ID_HANDLER(ID_FORM_RELOAD,OnBtReload)
		CHAIN_MSG_MAP( CDialogResize<CFormList> )
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CFormList)
		DLGRESIZE_CONTROL(IDC_LIST1,DLSZ_SIZE_X|DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(ID_FORM_EDIT,DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_FORM_RUN,DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_FORM_NEW,DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_FORM_RELOAD,DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnDeleteViewer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnList1DClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtRun(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtReload(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtNew(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	CListBox m_lb1;
	void ReloadList(void);
	bool GetCurSel(CString &s);
};

#endif // !defined(AFX_FORMLIST_H__F179C288_4506_41D3_ACA2_545BB393CDEA__INCLUDED_)
