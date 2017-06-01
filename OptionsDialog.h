// OptionsDialog.h: interface for the COptionsDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTIONSDIALOG_H__E553C4B1_2762_4255_9558_BC4451970A4E__INCLUDED_)
#define AFX_OPTIONSDIALOG_H__E553C4B1_2762_4255_9558_BC4451970A4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlddx.h>
#include <no5tl\no5richedit.h>
#include "coloreditor.h"	// CColorWindow
#include "no5options.h"

// fwd declarations
struct IGeneralOptions;
struct IIgnoreList;
struct IVisualOptions;

namespace NO5YAHOO
{
	struct FontAll;
}

//

class CPmPage : \
	public CPropertyPageImpl<CPmPage>,
	public CWinDataExchange<CPmPage>
{
	typedef CPropertyPageImpl<CPmPage>  _BaseClass;
public:
	CPmPage()
	{
		m_pgo = NULL;
	}
	virtual ~CPmPage()
	{
		//
	}

	enum { IDD = IDD_PROPPAGE_PM };

	BEGIN_MSG_MAP(CPmPage)
		MESSAGE_HANDLER(WM_INITDIALOG,OnInitDialog)
		COMMAND_CODE_HANDLER(BN_CLICKED,OnModified)
		COMMAND_CODE_HANDLER(EN_CHANGE,OnModified)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CPmPage)
		DDX_CHECK(IDC_CHECK1,m_iPopup)
		DDX_CHECK(IDC_CHECK2,m_iFriendsOnly)
		DDX_CHECK(IDC_CHECK3,m_iNoLinks)
		DDX_CHECK(IDC_CHECK4,m_iTopmost)
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

	LRESULT OnModified(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		SetModified(TRUE);
		return 0;
	}

	int OnSetActive()
	{
		// 0 = allow activate
		// -1 = go back that was active
		// page ID = jump to page
		//DoDataExchange(DDX_LOAD);
		return 0;
	}
	BOOL OnKillActive()
	{
		// FALSE = allow deactivate
		// TRUE = prevent deactivation
		//DoDataExchange(DDX_SAVE);
		return FALSE;
	}
	int OnApply();
	void OnReset()
	{
		// cancel button clicked
	}
	BOOL OnQueryCancel()
	{
		// FALSE = allow cancel
		// TRUE = prevent cancel
		return FALSE;
	}

	int m_iPopup;
	int m_iFriendsOnly;
	int m_iNoLinks;
	int m_iTopmost;
	IGeneralOptions *m_pgo;

};

class CGeneralPage : \
	public CPropertyPageImpl<CGeneralPage>,
	public CWinDataExchange<CGeneralPage>
{
	typedef CPropertyPageImpl<CGeneralPage>  _BaseClass;
public:
	CGeneralPage()
	{
		m_iTopmost = BST_UNCHECKED;
		m_iShowEnter = BST_UNCHECKED;
		m_iConnType = 0;
		m_pgo = NULL;
	}
	virtual ~CGeneralPage()
	{
		//
	}

	enum { IDD = IDD_PROPPAGE_GENERAL };

	BEGIN_MSG_MAP(CGeneralPage)
		MESSAGE_HANDLER(WM_INITDIALOG,OnInitDialog)
		COMMAND_CODE_HANDLER(BN_CLICKED,OnModified)
		COMMAND_CODE_HANDLER(EN_CHANGE,OnModified)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CPmPage)
		DDX_CHECK(IDC_CHECK1,m_iTopmost)
		DDX_CHECK(IDC_CHECK2,m_iShowEnter)
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnModified(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		SetModified(TRUE);
		return 0;
	}

	int OnSetActive()
	{
		// 0 = allow activate
		// -1 = go back that was active
		// page ID = jump to page
		return 0;
	}
	BOOL OnKillActive()
	{
		// FALSE = allow deactivate
		// TRUE = prevent deactivation
		return FALSE;
	}
	int OnApply();
	void OnReset()
	{
	}
	BOOL OnQueryCancel()
	{
		// FALSE = allow cancel
		// TRUE = prevent cancel
		return FALSE;
	}
	//
	int m_iTopmost;
	int m_iShowEnter;
	int m_iConnType;
	IGeneralOptions *m_pgo;
};

class CAutoIgnorePage : \
	public CPropertyPageImpl<CAutoIgnorePage>,
	public CWinDataExchange<CAutoIgnorePage>
{
	typedef CPropertyPageImpl<CAutoIgnorePage>  _BaseClass;
public:
	CAutoIgnorePage()
	{
		m_pgo = NULL;
		m_iAuto,m_iFlood = BST_UNCHECKED;
		m_iSpam = m_iTags = m_iYBots = BST_UNCHECKED;
		m_dwSpamTime = 0;
		m_dwAutoTime = 0;
		m_dwpps = 0;
	}
	virtual ~CAutoIgnorePage()
	{
		//
	}

	enum { IDD = IDD_PROPPAGE_AUTOIGNORE };

	BEGIN_MSG_MAP(CAutoIgnorePage)
		MESSAGE_HANDLER(WM_INITDIALOG,OnInitDialog)
		COMMAND_CODE_HANDLER(BN_CLICKED,OnModified)
		COMMAND_CODE_HANDLER(EN_CHANGE,OnModified)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CAutoIgnorePage)
		DDX_CHECK(IDC_CHECK_AUTO,m_iAuto)
		DDX_CHECK(IDC_CHECK_FLOOD,m_iFlood)
		DDX_CHECK(IDC_CHECK_SPAM,m_iSpam)
		DDX_CHECK(IDC_CHECK_BADTAG,m_iTags)
		DDX_CHECK(IDC_CHECK_YBOT,m_iYBots)
		DDX_UINT(IDC_EDIT_PPS,m_dwpps)
		DDX_UINT(IDC_EDIT_STIME,m_dwSpamTime)
		DDX_INT(IDC_EDIT_ATIME,m_dwAutoTime)
		DDX_TEXT(IDC_EDIT_SWORDS,m_sWords)
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnModified(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		SetModified(TRUE);
		return 0;
	}

	int OnSetActive()
	{
		// 0 = allow activate
		// -1 = go back that was active
		// page ID = jump to page
		return 0;
	}
	BOOL OnKillActive()
	{
		// FALSE = allow deactivate
		// TRUE = prevent deactivation
		return FALSE;
	}
	int OnApply();
	void OnReset()
	{
	}
	BOOL OnQueryCancel()
	{
		// FALSE = allow cancel
		// TRUE = prevent cancel
		return FALSE;
	}
	//
	IGeneralOptions *m_pgo;
	int m_iAuto,m_iFlood;
	int m_iSpam,m_iTags,m_iYBots;
	DWORD m_dwSpamTime;
	LONG m_dwAutoTime;
	DWORD m_dwpps;
	CString m_sWords;
};

class CIgnorePage : \
	public CPropertyPageImpl<CIgnorePage>,
	public CWinDataExchange<CIgnorePage>
{
	typedef CPropertyPageImpl<CIgnorePage>  _BaseClass;
public:
	CIgnorePage(void)
	{
		m_pgo = NULL;
		m_days = 0;
		m_pIgnoreList = NULL;
	}
	virtual ~CIgnorePage()
	{
		//
	}

	enum { IDD = IDD_PROPPAGE_PIGNORE };

	BEGIN_MSG_MAP(CIgnorePage)
		MESSAGE_HANDLER(WM_INITDIALOG,OnInitDialog)
		COMMAND_ID_HANDLER(IDC_BUTTON1,OnRemoveUser)
		COMMAND_ID_HANDLER(IDC_BUTTON2,OnRemoveAll)
		COMMAND_CODE_HANDLER(BN_CLICKED,OnModified)
		COMMAND_CODE_HANDLER(EN_CHANGE,OnModified)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CIgnorePage)
		DDX_INT(IDC_EDIT1,m_days)
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnRemoveUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRemoveAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnModified(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		SetModified(TRUE);
		return 0;
	}

	int OnSetActive()
	{
		// 0 = allow activate
		// -1 = go back that was active
		// page ID = jump to page
		return 0;
	}
	BOOL OnKillActive()
	{
		// FALSE = allow deactivate
		// TRUE = prevent deactivation
		return FALSE;
	}
	int OnApply();
	void OnReset()
	{
	}
	BOOL OnQueryCancel()
	{
		// FALSE = allow cancel
		// TRUE = prevent cancel
		return FALSE;
	}
	//
	IGeneralOptions *m_pgo;
	long m_days;
	CListViewCtrl m_lv;
	IIgnoreList *m_pIgnoreList;
private:
	void LoadList(void);
	int GetCurSel(void);	// returns our list view current sel
};


class CVisualPage : \
	public CPropertyPageImpl<CVisualPage>,
	public CWinDataExchange<CVisualPage>
{
	typedef CPropertyPageImpl<CVisualPage>  _BaseClass;
public:
	CVisualPage(void);
	virtual ~CVisualPage();

	enum { IDD = IDD_PROPPAGE_VISUAL };

	BEGIN_MSG_MAP(CVisualPage)
		MESSAGE_HANDLER(WM_INITDIALOG,OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		COMMAND_CODE_HANDLER(CBN_SELCHANGE,OnComboSelChange)
		COMMAND_ID_HANDLER(IDC_BUTTON1,OnColorChoose)
		COMMAND_ID_HANDLER(IDC_BUTTON2,OnColorDefault)
		COMMAND_ID_HANDLER(IDC_BUTTON3,OnFontChoose)
		COMMAND_ID_HANDLER(IDC_BUTTON4,OnFontDefault)
		COMMAND_ID_HANDLER(IDC_BUTTON5,OnBackChoose)
		COMMAND_ID_HANDLER(IDC_BUTTON6,OnBackDefault)
		COMMAND_ID_HANDLER(IDC_BUTTON7,OnClrBkChoose)
		COMMAND_ID_HANDLER(IDC_BUTTON8,OnClrBkDefault)
		COMMAND_ID_HANDLER(IDC_BUTTON9,OnRestoreAll)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CAutoIgnorePage)
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnModified(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		SetModified(TRUE);
		return 0;
	}
	LRESULT OnComboSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRestoreAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnColorChoose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnColorDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFontChoose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFontDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBackChoose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBackDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClrBkChoose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClrBkDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);


	int OnSetActive()
	{
		// 0 = allow activate
		// -1 = go back that was active
		// page ID = jump to page
		return 0;
	}
	BOOL OnKillActive()
	{
		// FALSE = allow deactivate
		// TRUE = prevent deactivation
		return FALSE;
	}
	int OnApply();
	void OnReset()
	{
	}
	BOOL OnQueryCancel()
	{
		// FALSE = allow cancel
		// TRUE = prevent cancel
		return FALSE;
	}
private:
	NO5TL::CNo5RichEdit m_edit;
	CComboBox m_cb;
	IVisualOptions *m_pvo;
	//
	void LoadCombo(void);
	void CreatePreview(void);
	void UpdatePreview(void);
	UINT GetItemId(void);
	static void GetItemFont(UINT id,NO5YAHOO::FontAll &f,IVisualOptions *pvo);
	static void SetItemFont(UINT id,NO5YAHOO::FontAll &f,IVisualOptions *pvo);
	void GetItemFont(UINT id,NO5YAHOO::FontAll &f);
	void SetItemFont(UINT id,NO5YAHOO::FontAll &f);
	void CreateDefault(IVisualOptions **pp);
};

class COptionsPropertySheet : public CPropertySheetImpl<COptionsPropertySheet>
{
	typedef CPropertySheetImpl<COptionsPropertySheet> _BaseClass;
public:
	COptionsPropertySheet(HWND hWndParent = NULL)
		: _BaseClass((LPCTSTR)_T("Options"), 0, hWndParent)
	{
		//
	}
	virtual ~COptionsPropertySheet()
	{
		//
	}

	BEGIN_MSG_MAP(COptionsPropertySheet)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	LRESULT OnDestroy(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	void AddPages(void);

	CPmPage m_pagePm;
	CGeneralPage m_pageGeneral;
	CAutoIgnorePage m_pageAuto;
	CIgnorePage m_pageIgnore;
	CVisualPage m_pageVisual;
};

#endif // !defined(AFX_OPTIONSDIALOG_H__E553C4B1_2762_4255_9558_BC4451970A4E__INCLUDED_)
