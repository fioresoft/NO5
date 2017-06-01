// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__F7628970_2C65_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_CHILDFRM_H__F7628970_2C65_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "usrmsgs.h"
struct IMainFrame;

template <class TView>
class CChildFrame : public CTabbedMDIChildWindowImpl<CChildFrame<TView> >
{
	typedef CTabbedMDIChildWindowImpl<CChildFrame<TView> > _BaseClass;
public:
	CChildFrame(IMainFrame *pFrame)
	{
		m_pFrame = pFrame;
		m_pView = new TView();
	}
	virtual ~CChildFrame()
	{
		ATLTRACE("~CChildFrame\n");
		delete m_pView;
	}

	DECLARE_FRAME_WND_CLASS(NULL, IDR_CHATFRAME)

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		delete this;
	}

	BEGIN_MSG_MAP(CChildFrame)
		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_MDIACTIVATE,OnMdiActivate)
		MESSAGE_HANDLER(WM_CLOSE,OnClose)
		MESSAGE_HANDLER(NO5WM_SETCHILDTEXT,OnSetChildText)
		CHAIN_CLIENT_COMMANDS()
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)


	LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPMSG pMsg = (LPMSG)lParam;

		return _BaseClass::PreTranslateMessage(pMsg);
	}

	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		m_hWndClient = m_pView->Create(m_hWnd,rcDefault);
	//	MDIMaximize(m_hWnd);
		return 0;
	}

	LRESULT OnMdiActivate(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		m_pView->SendMessage(NO5WM_MDIACTIVATE,wParam,lParam);
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		m_pView->SendMessage(NO5WM_CLOSE);
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSetChildText(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		CViewTextParams *p = (CViewTextParams *)wParam;
		if(p){
			CString title = p->room;
			ATLASSERT( p->room && p->name);

			if(p->slogan){
				title += " - ";
				title += p->slogan;
			}
			SetTitle(title);
			SetTabText(p->name);
			SetTabToolTip(title);
		}
		return 0;
	}

private:
	TView *m_pView;
	IMainFrame *m_pFrame;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__F7628970_2C65_11DA_A17D_000103DD18CD__INCLUDED_)
