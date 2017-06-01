// RightFrame.cpp: implementation of the CRightFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atlctrlx.h>
#include "RightFrame.h"
#include "iuserlistview.h"
#include "userlistview.h"
#include "resource.h"
#include "usrmsgs.h"
#include <no5tl\mystring.h>
#include "buddylistview.h"
#include "scriptview.h"
#include "formlist.h"


// users that are in chat and in the ignore list at the same time

class CQuickIgnoreView : \
	public CWindowImpl<CQuickIgnoreView>,
	public IQuickIgnoreView
{
public:
	BEGIN_MSG_MAP(CQuickIgnoreView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_CODE_HANDLER(LBN_DBLCLK,OnDoubleClick)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		m_lb.Create(m_hWnd,rcDefault,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|\
			LBS_SORT|LBS_NOTIFY|LBS_NOINTEGRALHEIGHT);
		m_lb.SetFont(AtlGetDefaultGuiFont());
		return 0;
	}
	LRESULT OnSize(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		if(m_lb.IsWindow()){
			m_lb.MoveWindow(0,0,LOWORD(lParam),HIWORD(lParam));
		}
		return 0;
	}
	LRESULT OnDoubleClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(m_pSink){
			int index = m_lb.GetCurSel();
			if(index >= 0){
				CString s;

				index = m_lb.GetText(index,s);
				if(index != LB_ERR){
					m_pSink->OnQuick_RemoveUser(s);
				}
			}
		}
		return 0;
	}
public:
	CQuickIgnoreView(void)
	{
		m_pSink = NULL;
	}
	virtual void SetSink(IQuickIgnoreViewEvents *pSink)
	{
		m_pSink = pSink;
	}
	virtual void AddUser(LPCTSTR name)
	{
		m_lb.AddString(name);
	}
	virtual void RemoveUser(LPCTSTR name)
	{
		int index = m_lb.FindStringExact(-1,name);
		if(index >= 0)
			m_lb.DeleteString(index);
	}
	virtual void RemoveAll(void)
	{
		m_lb.ResetContent();
	}
private:
	CListBox m_lb;
	IQuickIgnoreViewEvents *m_pSink;
};

// CView1

// view that contains the userlist view and ignored-users view
class CView1 : public CWindowImpl<CView1>
{
public:
	BEGIN_MSG_MAP(CRightFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_ID_HANDLER(ID_PANE_CLOSE,OnPaneClose)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		m_split1.Create(m_hWnd,rcDefault,NULL);
		m_pane2.Create(m_split1,_T("Ignored"));

		m_pUserView->Create(m_split1,rcDefault,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,WS_EX_CLIENTEDGE);
		m_pIgnoreView->Create(m_pane2,rcDefault,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,WS_EX_CLIENTEDGE);
		m_split1.SetSplitterPanes(m_pUserView->m_hWnd,m_pane2);
		m_pane2.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON,PANECNT_NOCLOSEBUTTON);
		m_pane2.SetClient(m_pIgnoreView->m_hWnd);

		return 0;
	}
	LRESULT OnSize(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		if(m_split1.IsWindow()){
			m_split1.MoveWindow(0,0,LOWORD(lParam),HIWORD(lParam));
		}
		if(m_bFirstSize && HIWORD(lParam)){
			m_bFirstSize = false;
			m_split1.SetSplitterPos(4 * HIWORD(lParam) / 5);
		}
		return 0;
	}
	LRESULT OnPaneClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& /*bHandled*/)
	{
		if(hWndCtl == m_pane2.m_hWnd){
			m_split1.SetSinglePaneMode(SPLIT_PANE_TOP);
		}
		return 0;
	}
	//
	CView1()
	{
		m_pUserView = new CUserListView();
		m_pIgnoreView = new CQuickIgnoreView();
		m_bFirstSize = true;
	}
	~CView1()
	{
		delete m_pUserView;
		delete m_pIgnoreView;
	}
public:
	CUserListView *m_pUserView;
	CQuickIgnoreView *m_pIgnoreView;
	CHorSplitterWindow m_split1;
	CPaneContainer m_pane2;
private:
	bool m_bFirstSize;
};

// CRightFrame

CRightFrame::CRightFrame()
{
	m_pView1 = new CView1();
	m_pBuddyView = new CBuddyListView();
	m_pScriptView = new CScriptView();
	m_pForms = new CFormList();
}

CRightFrame::~CRightFrame()
{
	delete m_pView1;
	delete m_pBuddyView;
	delete m_pScriptView;
	delete m_pForms;
}

LRESULT CRightFrame::OnCreate(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	_BaseClass::OnCreate(WM_CREATE,wParam,lParam,bHandled);

	m_pView1->Create(m_hWnd,rcDefault,NULL);
	AddTab(m_pView1->m_hWnd,_T("users"));

	m_pBuddyView->Create(m_hWnd,rcDefault,NULL);
	AddTab(m_pBuddyView->m_hWnd,_T("friends"));

	m_pScriptView->Create(m_hWnd,rcDefault,NULL);
	AddTabWithIcon(m_pScriptView->m_hWnd,_T("scripts"),IDI_SCRIPT);

	m_pForms->Create(m_hWnd,rcDefault,NULL);
	AddTabWithIcon(m_pForms->m_hWnd,_T("forms"),IDI_FORMS);
	
	return 0;
}

IUserListView * CRightFrame::GetUserListView(void)
{
	return m_pView1->m_pUserView;
}

IQuickIgnoreView * CRightFrame::GetQuickIgnoreView(void)
{
	return m_pView1->m_pIgnoreView;
}

IBuddyListView * CRightFrame::GetBuddyListView(void)
{
	return m_pBuddyView;
}