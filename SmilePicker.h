// SmilePicker.h: interface for the CSmilePicker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMILEPICKER_H__87F40413_88CA_4174_8261_EFD23959EFD0__INCLUDED_)
#define AFX_SMILEPICKER_H__87F40413_88CA_4174_8261_EFD23959EFD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <no5tl\htmledit.h>
using NO5TL::CHtmlCtrl;

typedef CWinTraits<WS_OVERLAPPEDWINDOW|WS_VISIBLE|WS_VSCROLL,WS_EX_TOOLWINDOW> CSPStyle;

class CSmilePicker : \
	public CWindowImpl<CSmilePicker,CHtmlCtrl,CSPStyle>,
	public IDispEventImpl<1,CSmilePicker,&DIID_HTMLDocumentEvents,&LIBID_MSHTML,4,0>
{
	typedef IDispEventImpl<1,CSmilePicker,&DIID_HTMLDocumentEvents,&LIBID_MSHTML,4,0> MyDisp;
public:
	DECLARE_WND_SUPERCLASS("no5smileys",CHtmlCtrl::GetWndClassName())
	CSmilePicker(HWND hWndNotify);
	virtual ~CSmilePicker();
	virtual void OnFinalMessage(HWND hWnd)
	{
		delete this;
	}

	BEGIN_MSG_MAP(CSmilePicker)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()
	
	BEGIN_SINK_MAP(CSmilePicker)
		SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONCLICK,OnClick)
	END_SINK_MAP()


// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnDestroy(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	virtual VARIANT_BOOL __stdcall OnClick(void);

	CString m_res;	// selected code or empty
private:
	int LoadSmiles(void);
	CWindow m_notify;

};

class CSmilePicker2 : \
	public CWindowImpl<CSmilePicker2,CWindow,CSPStyle>
{
	struct Data
	{
		CString code;
	};
public:
	DECLARE_WND_CLASS_EX("no5smileys2",0,COLOR_WINDOW)
	CSmilePicker2(HWND hWndNotify);
	virtual ~CSmilePicker2();
	virtual void OnFinalMessage(HWND hWnd)
	{
		delete this;
	}

	BEGIN_MSG_MAP(CSmilePicker2)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		COMMAND_ID_HANDLER(IDC_BUTTON1,OnClick)
	END_MSG_MAP()
	

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnDestroy(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnSize(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	CString m_res;	// selected code or empty
private:
	void LoadSmiles(void);
	CWindow m_notify;
	CToolBarCtrl m_tb;
	CImageList m_il;

};

#endif // !defined(AFX_SMILEPICKER_H__87F40413_88CA_4174_8261_EFD23959EFD0__INCLUDED_)
