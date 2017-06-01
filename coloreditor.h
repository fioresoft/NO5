#if !defined(WTL_COLOREDITOR_H_CA7858B3_3981_11DA_A17D000103DD18CD_DEFINED)
#define WTL_COLOREDITOR_H_CA7858B3_3981_11DA_A17D000103DD18CD_DEFINED

#include <no5tl\gradrect.h>
#include <yahoo\yahoocolor.h>
#include <no5tl\no5controls.h>
#include <no5tl\no5richedit.h>
#include "viksoe\colorcombo.h"
#include "usrmsgs.h"

using NO5YAHOO::CYahooColor;
using NO5TL::CFadeWindowImpl;
using NO5TL::CColor;
using NO5TL::CNo5RichEdit;

// forward decl
struct IColorFile;

class CColorWindow : \
	public CWindowImpl<CColorWindow,CStatic>,
	public COwnerDraw<CColorWindow>
{
	CColor m_color;
public:
	DECLARE_WND_SUPERCLASS(NULL,CStatic::GetWndClassName())

	BEGIN_MSG_MAP(CColorWindow)
		CHAIN_MSG_MAP_ALT( COwnerDraw<CColorWindow> , 1 )
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	void SetColor(COLORREF clr)
	{
		m_color = clr;
		InvalidateRect(NULL);
	}

	void DrawItem(LPDRAWITEMSTRUCT p)
	{
		CRect r = p->rcItem;
		CDCHandle dc = p->hDC;

		dc.FillSolidRect(&r,m_color);

	}
};

class CMyFadeWindow : public CFadeWindowImpl<CMyFadeWindow,true>
{
	typedef CFadeWindowImpl<CMyFadeWindow,true> _BaseClass;
public:
	BEGIN_MSG_MAP(CMyFadeWindow)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	void OnFadeWndClick(COLORREF color)
	{
		CWindow wnd = GetParent();
		wnd.PostMessage(NO5WM_FADECLICK,(WPARAM)color);
	}
};

class CColorEditorDlg: public CDialogImpl<CColorEditorDlg>
{
public:
	enum {IDD = IDD_COLORS};

	BEGIN_MSG_MAP(CColorEditorDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNCDestroy)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(NO5WM_FADECLICK,OnFadeClick)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(ID_COLOR_ADD, OnColorAdd)
		COMMAND_ID_HANDLER(ID_COLOR_REMOVE, OnColorRemove)
		COMMAND_ID_HANDLER(ID_COLOR_CLEAR, OnColorClear)
		COMMAND_ID_HANDLER(ID_COLORSET_SAVE, OnColorSetSave)
		COMMAND_ID_HANDLER(ID_COLORSET_DELETE, OnColorSetDelete)
		COMMAND_CODE_HANDLER(CBN_SELENDOK,OnSelEndOk)
		REFLECT_NOTIFICATIONS()
	ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_KEYUP,OnKeyUp)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNCDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam,BOOL& bHandled);
	LRESULT OnFadeClick(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam,BOOL& bHandled);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnColorAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnColorRemove(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnColorClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnColorSetSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnColorSetDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSelEndOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled);
	// rich edit message handlers
	LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void OnFinalMessage(HWND)
	{
		//delete this;
	}

private:

	// the current color can be changed with the scroll bars or clicking in the
	// fade strip. here we update anything when a color changes
	void SetColor(COLORREF clr);
	void CreatePreview(void);
	void UpdatePreview(void);
	BOOL FillColorSetCombo(void);
	BOOL LoadColorSet(void);
	void FillColorListBox(void);

public:
	CColorEditorDlg(void);
	~CColorEditorDlg();
	//
	CString m_ColorSet;	// name of the color set
	CSimpleArray<CYahooColor> m_colors;
	CString m_ColorFilePath;	// full path to color file
private:
	CScrollBar m_bars[3];
	CMyFadeWindow m_grads[3];
	CYahooColor m_color;
	CColorWindow m_wndColor;
	CColorPickerListCtrl m_lbColors;
	CContainedWindowT<CNo5RichEdit> m_preview;
	IColorFile *m_pcf;
	NO5TL::CNo5ComboBox m_cbNames;

};

#endif // !defined(WTL_COLOREDITOR_H_CA7858B3_3981_11DA_A17D000103DD18CD_DEFINED)
