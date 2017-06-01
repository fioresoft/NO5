// BottomFrame.h: interface for the CBottomFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOTTOMFRAME_H__F922AD96_304F_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_BOTTOMFRAME_H__F922AD96_304F_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "viksoe\colorcombo.h"
#include "viksoe\fontcombo.h"
#include "iinputframe.h"

class CInputView1;
struct IMainFrame;
struct CInputParams;

#define CHAIN_TAB_CHILD_COMMANDS()\
if(uMsg == WM_COMMAND){\
	TTabCtrl &tc = GetTabCtrl();\
	int index = tc.GetCurSel();\
	if(index >= 0){\
		TTabCtrl::TItem *p = tc.GetItem((size_t)index);\
		if(p && p->UsingTabView()){\
			CWindow wnd = p->GetTabView();\
			wnd.SendMessage(uMsg,wParam,lParam);\
		}\
	}\
}\

typedef CDotNetTabCtrl<CTabViewTabItem> BottomTabCtrl;


class CBottomFrame: \
	public CFrameWindowImpl<CBottomFrame>,
	public IInputFrame,
	public CMessageFilter
{
	typedef CFrameWindowImpl<CBottomFrame> _BaseClass;
public:
	CBottomFrame(IMainFrame *pFrame);
	virtual ~CBottomFrame();

	DECLARE_FRAME_WND_CLASS("No5BottomFrame", IDR_BOTTOMFRAME)

	BEGIN_MSG_MAP(CBottomFrame)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		MESSAGE_HANDLER(NO5WM_SMILESWND,OnSmileWndClose)
		COMMAND_ID_HANDLER(ID_EDIT_FADE,OnEditFade)
		COMMAND_ID_HANDLER(ID_EDIT_BLEND,OnEditBlend)
		COMMAND_CODE_HANDLER(CBN_SELENDOK,OnComboSelChange)
		COMMAND_ID_HANDLER(ID_BOTTOM_SMILES,OnSmiles)
		//CHAIN_TAB_CHILD_COMMANDS()
		CHAIN_CLIENT_COMMANDS()
		CHAIN_MSG_MAP(_BaseClass)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(m_hAccel != NULL && TranslateAccelerator(m_hWnd,m_hAccel,pMsg))
			return TRUE;
		return FALSE;
	}

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnSmileWndClose(UINT /*uMsg*/,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnEditFade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnEditBlend(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnSmiles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnComboSelChange(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& bHandled);

	// IInputFrame methods
	virtual void SetParams(CInputParams &p);
	virtual void GetParams(CInputParams &p);
	virtual CToolBarCtrl & GetFontToolbar(void);
	virtual void SetColorsetName(LPCTSTR name,BOOL bNotifySink);
	virtual CString GetColorsetName(void);
	virtual void SetSink(IInputFrameEvents *pSink);
	//
	int GetDesiredHeight(void);

private:
	CInputView1 *m_view1;
	IMainFrame *m_pFrame;
	CToolBarCtrl m_tb;
	CColorPickerComboCtrl m_cbColors;
	CFontPickerComboCtrl m_cbFonts;
	CComboBox m_cbSize;
	CComboBox m_cbFade;
	IInputFrameEvents *m_pSink;

	void ReloadFadeCombo(void);

	friend CInputView1;
};




#endif // !defined(AFX_BOTTOMFRAME_H__F922AD96_304F_11DA_A17D_000103DD18CD__INCLUDED_)
