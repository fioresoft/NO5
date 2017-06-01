// RightFrame.h: interface for the CRightFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIGHTFRAME_H__F922AD92_304F_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_RIGHTFRAME_H__F922AD92_304F_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct IUserListView;
struct IQuickIgnoreView;
struct IBuddyListView;
class CView1;
class CBuddyListView;
class CScriptView;
class CFormList;

typedef CDotNetTabCtrl<CTabViewTabItem> RightTabCtrl;

class CRightFrame: public CTabbedChildWindow<RightTabCtrl> 
{
	typedef CTabbedChildWindow<RightTabCtrl> _BaseClass;
public:
	CRightFrame();
	virtual ~CRightFrame();

	BEGIN_MSG_MAP(CRightFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);

	//
	IUserListView * GetUserListView(void);
	IQuickIgnoreView * GetQuickIgnoreView(void);
	IBuddyListView * GetBuddyListView(void);
private:
	CView1 *m_pView1;
	CBuddyListView *m_pBuddyView;
	CScriptView *m_pScriptView;
	CFormList *m_pForms;
};

#endif // !defined(AFX_RIGHTFRAME_H__F922AD92_304F_11DA_A17D_000103DD18CD__INCLUDED_)
