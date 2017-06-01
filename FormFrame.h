// FormFrame.h: interface for the CFormFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMFRAME_H__5EB2390C_5045_4995_A4DE_EC2BE9BEE3AE__INCLUDED_)
#define AFX_FORMFRAME_H__5EB2390C_5045_4995_A4DE_EC2BE9BEE3AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "C:\downloads\script engine\Redistributables\Redistributables\Ansi\DDForms.dll" raw_interfaces_only, raw_native_types, named_guids  


class CExposedObjectInfo
{
public:
	// name
	CString m_Name;
	// object
	CComPtr<IDispatch> m_spDispatch;
};

class CPreviewDlg : \
	public CAxDialogImpl<CPreviewDlg>,
	public CMessageFilter
{
public:
	enum { IDD = IDD_FORMVIEW2 };

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CPreviewDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNCDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CLOSE,OnClose)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(!m_bModeless)
			EndDialog(IDOK);
		else
			DestroyWindow();
		return 0;
	}
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CMessageLoop *p = _Module.GetMessageLoop();
		p->RemoveMessageFilter(this);
		return 0;
	}
	LRESULT OnNCDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(m_ax.IsWindow()){
			m_ax.MoveWindow(0,0,LOWORD(lParam),HIWORD(lParam));
		}
		return 0;
	}
	CPreviewDlg(HWND hWndOwner,BOOL bModeless = TRUE);
	virtual ~CPreviewDlg();
	// this will be set TRUE if the preview should be auto sizing
	BOOL m_AutoSizing;
	// form file name
	CString m_FileName;
	// information about the objects which require exposing to the form viewer
	CSimpleArray<CExposedObjectInfo> m_ExposedObjectInfoArray;
	CComPtr<DDFORMSLib::IFormViewer2> m_spFormViewer;
	CAxWindow m_ax;
	const BOOL m_bModeless;
	CWindow m_wndOwner;
};


typedef CWinTraits<WS_OVERLAPPEDWINDOW | WS_VISIBLE,0> CFFTraits;
typedef CDotNetTabCtrl<CTabViewTabItem> CFormTabCtrl;

class CFormFrame : \
	public CTabbedFrameImpl<CFormFrame,CFormTabCtrl,CFrameWindowImpl<CFormFrame,CWindow,CFFTraits> >,
	public CUpdateUI<CFormFrame>,
	public CMessageFilter,
	public IDispEventImpl<IDC_FORMEDITOR1, CFormFrame>
{
	typedef CTabbedFrameImpl<CFormFrame,CFormTabCtrl,CFrameWindowImpl<CFormFrame,CWindow,CFFTraits> > baseClass;
public:
	DECLARE_FRAME_WND_CLASS("NO5CLSFORM", IDR_FORMFRAME)

	CFormFrame(LPCTSTR file = NULL);
	virtual ~CFormFrame();

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		delete this;
	}
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_UPDATE_UI_MAP(CFormFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_CUT, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_COPY, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_PASTE, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_UNDO, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_REDO, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_DELETE, UPDUI_MENUPOPUP|UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_SELECT_ALL, UPDUI_MENUPOPUP|UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_UNSELECT_ALL, UPDUI_MENUPOPUP|UPDUI_TOOLBAR)
		// etc ...
	END_UPDATE_UI_MAP()
	
	BEGIN_SINK_MAP(CFormFrame)
		//Make sure the Event Handlers have __stdcall calling convention
		SINK_ENTRY(IDC_FORMEDITOR1, 0x1, OnStateChanged)
		SINK_ENTRY(IDC_FORMEDITOR1, 0x4, OnEditScriptError)
	END_SINK_MAP()

	HRESULT __stdcall OnStateChanged(VARIANT_BOOL Busy, VARIANT_BOOL Modified, VARIANT_BOOL CanPaste, VARIANT_BOOL CanUndo, VARIANT_BOOL CanRedo, LONG ItemCount, LONG SelectedItemCount);
	HRESULT __stdcall OnEditScriptError(BSTR Source, long Number, BSTR description);

	BEGIN_MSG_MAP(CFormFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNCDestroy)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER( (WM_USER + 1),OnModified)
		MESSAGE_HANDLER( NO5WM_FORMVIEW,OnDeleteViewer)
		COMMAND_ID_HANDLER(ID_FILE_NEW,OnFileNew)
		COMMAND_ID_HANDLER(ID_FILE_SAVE,OnFileSave)
		COMMAND_ID_HANDLER(ID_FILE_SAVE_AS,OnFileSaveAs)
		COMMAND_ID_HANDLER(ID_FILE_OPEN,OnFileOpen)
		COMMAND_ID_HANDLER(ID_FRAME,OnAddFrame)
		COMMAND_ID_HANDLER(ID_RECTANGLE,OnAddRect)
		COMMAND_ID_HANDLER(ID_PICTURE,OnAddPicture)
		COMMAND_ID_HANDLER(ID_LABEL,OnAddLabel)
		COMMAND_ID_HANDLER(ID_BUTTON,OnAddButton)
		COMMAND_ID_HANDLER(ID_CHECK,OnAddCheck)
		COMMAND_ID_HANDLER(ID_RADIO,OnAddRadio)
		COMMAND_ID_HANDLER(ID_TEXT,OnAddText)
		COMMAND_ID_HANDLER(ID_COMBO,OnAddCombo)
		COMMAND_ID_HANDLER(ID_LIST,OnAddList)
		COMMAND_ID_HANDLER(ID_HSCROLL,OnAddHScroll)
		COMMAND_ID_HANDLER(ID_VSCROLL,OnAddVScroll)
		COMMAND_ID_HANDLER(ID_SPIN,OnAddSpin)
		COMMAND_ID_HANDLER(ID_SLIDER,OnAddSlider)
		COMMAND_ID_HANDLER(ID_PROGRESS,OnAddProgress)
		COMMAND_ID_HANDLER(ID_ACTIVEX,OnAddActivex)
		COMMAND_ID_HANDLER(ID_DELETE,OnDelete)
		COMMAND_ID_HANDLER(ID_GRID,OnGrid)
		COMMAND_ID_HANDLER(ID_GRID_SETTINGS,OnGridSettings)
		COMMAND_ID_HANDLER(ID_TAB,OnTab)
		COMMAND_ID_HANDLER(ID_SELECT_ALL,OnSelectAll)
		COMMAND_ID_HANDLER(ID_UNSELECT_ALL,OnUnselectAll)
		COMMAND_ID_HANDLER(ID_PROPERTIES,OnProperties)
		//
		COMMAND_RANGE_HANDLER(ID_LEFT,ID_DOWN,OnMoveCtl)
		COMMAND_ID_HANDLER(ID_LAYOUT_SENDTOBACK,OnSendBack)
		COMMAND_ID_HANDLER(ID_LAYOUT_BRINGTOFRONT,OnSendFront)
		COMMAND_RANGE_HANDLER(ID_ALIGN_LEFT,ID_ALIGN_BOTTOM,OnAlignCtl)
		COMMAND_RANGE_HANDLER(ID_SPACE_HORZ,ID_SPACE_VERT,OnSpaceCtl)
		COMMAND_RANGE_HANDLER(ID_SIZE_LARGEST_WIDTH,ID_SIZE_SMALLEST,OnSizeCtl)
		COMMAND_RANGE_HANDLER(ID_CENTER_HORZ,ID_CENTER,OnCenterCtl)
		COMMAND_ID_HANDLER(ID_VALIDATE_SCRIPT,OnValidateScript)
		COMMAND_ID_HANDLER(ID_PREVIEW_AUTO_SIZE,OnPreviewAuto)
		COMMAND_ID_HANDLER(ID_PREVIEW,OnPreview)
		//
		COMMAND_RANGE_HANDLER(ID_CUT,ID_REDO,OnEditCmd)
		NOTIFY_CODE_HANDLER(CTCN_SELCHANGE, OnTabSelChange)
		NOTIFY_CODE_HANDLER(CTCN_SELCHANGING, OnTabSelChanging)
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()


// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNCDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnModified(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetModified(TRUE);
		return 0;
	}
	LRESULT OnDeleteViewer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CPreviewDlg *p = (CPreviewDlg *)lParam;
		delete p;
		return 0;
	}
	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//
	LRESULT OnAddFrame(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddRect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddPicture(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddLabel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddCheck(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddRadio(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddCombo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddHScroll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddVScroll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddSpin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddSlider(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddProgress(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAddActivex(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnGrid(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnGridSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnUnselectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnProperties(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//
	LRESULT OnMoveCtl(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSendBack(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSendFront(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAlignCtl(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSpaceCtl(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSizeCtl(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCenterCtl(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnValidateScript(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPreviewAuto(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPreview(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//
	LRESULT OnEditCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnTabSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnTabSelChanging(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	//
	void SetModified(BOOL bSet)
	{
		m_bModified = bSet;
	}

	CCommandBarCtrl m_CmdBar;
	CAxWindow m_axFormEdit;
	CAxWindow m_axScriptEdit;
	CAxWindow m_axFormView;
	CComPtr<DDFORMSLib::IFormEditor2> m_spFormEditor;
	CComPtr<DDFORMSLib::IFormViewer2> m_spFormViewer;
	CComPtr<DDFORMSLib::ISimpleScriptEditor> m_spScriptEditor;
	BOOL m_bModified;
	CString m_file;
private:
	// form editor state
	BOOL m_FES_Busy;
	BOOL m_FES_Modified;
	BOOL m_FES_CanPaste;
	BOOL m_FES_CanUndo;
	BOOL m_FES_CanRedo;
	long m_FES_ItemCount;
	long m_FES_SelectedItemCount;
	//
	void FileNew(void);
	void FileSave(LPCTSTR file);
	void FileOpen(LPCTSTR file);
	CString MakeTempName(void);
};


#endif // !defined(AFX_FORMFRAME_H__5EB2390C_5045_4995_A4DE_EC2BE9BEE3AE__INCLUDED_)
