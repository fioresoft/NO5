// BuddyListView.h: interface for the CBuddyListView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUDDYLISTVIEW_H__84E8C1C8_9D50_4843_A714_426127E2C018__INCLUDED_)
#define AFX_BUDDYLISTVIEW_H__84E8C1C8_9D50_4843_A714_426127E2C018__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <no5tl\mystring.h>
#include <no5tl\no5controls.h>
#include "ibuddylistview.h"

struct BuddyItem;

class CBuddyListView : \
	public CDialogImpl<CBuddyListView>,
	public IBuddyListView,
	public CDialogResize<CBuddyListView>,
	public CMessageFilter
{
	typedef CDialogImpl<CBuddyListView> baseClass;
	NO5TL::CNo5TreeCtrl m_tv;
	IBuddyListEvents *m_pSink;
	CBuddyList *m_p;
	CComboBox m_cb;
public:
	CBuddyListView();
	virtual ~CBuddyListView();

	enum { IDD = IDD_BUDDIES };

	BEGIN_MSG_MAP(CBuddyListView)
		MESSAGE_HANDLER(WM_INITDIALOG,OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		NOTIFY_CODE_HANDLER(TVN_GETINFOTIP,OnGetInfotip)
		NOTIFY_CODE_HANDLER(TVN_DELETEITEM,OnDeleteItem)
		NOTIFY_CODE_HANDLER(NM_DBLCLK,OnDoubleClick)
		NOTIFY_CODE_HANDLER(NM_RCLICK,OnRightClick)
		COMMAND_CODE_HANDLER(CBN_SELCHANGE,OnComboSelChange)
		CHAIN_MSG_MAP( CDialogResize<CBuddyListView> )
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CBuddyListView)
		//DLGRESIZE_CONTROL(IDC_STATIC1,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_COMBO1,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_TREE1,DLSZ_SIZE_X|DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetInfotip(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnDoubleClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnRightClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnDeleteItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnComboSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}

	// IBuddyListView
	virtual void SetAcc(IBuddyListEvents *pSink,LPCTSTR acc,CBuddyList *p);
	virtual void RemoveAcc(LPCTSTR acc);
	virtual void OnBuddyGroup(CBuddyGroup *pg);
	virtual void OnUserAway(LPCTSTR user);
	virtual void OnAddBuddy(LPCTSTR group,CBuddy *pb);
	virtual void OnRemBuddy(LPCTSTR group,LPCTSTR name);
	virtual void OnRenameGroup(LPCTSTR old,LPCTSTR group);
private:
	NO5TL::CNo5TreeItem FindItem(HTREEITEM hParent,BuddyItem &item);
	NO5TL::CNo5TreeItem FindGroupItem(LPCTSTR group);
	NO5TL::CNo5TreeItem FindBuddyItem(HTREEITEM hGroup,LPCTSTR buddy);
	static CString AccLabel(LPCTSTR acc);
	static CString GroupLabel(LPCTSTR group);
	HTREEITEM ItemUnderCursor(void);
	CBuddy * GetBuddy(LPCTSTR group,LPCTSTR name);
	CBuddyGroup * GetGroup(LPCTSTR group);
	BOOL GetAcc(CString &s);
	NO5TL::CNo5TreeItem AddGroup(LPCTSTR name);
	void UpdateTree(void);

};

#endif // !defined(AFX_BUDDYLISTVIEW_H__84E8C1C8_9D50_4843_A714_426127E2C018__INCLUDED_)
