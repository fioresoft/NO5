// FormFrame.cpp: implementation of the CFormFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "usrmsgs.h"
#include "FormFrame.h"
#include "no5app.h"


CFormFrame::CFormFrame(LPCTSTR file):\
		m_FES_Busy(FALSE),
		m_FES_Modified(FALSE),
		m_FES_CanPaste(FALSE),
		m_FES_CanUndo(FALSE),
		m_FES_CanRedo(FALSE),
		m_FES_ItemCount(0),
		m_FES_SelectedItemCount(0),
		m_bModified(FALSE),
		m_file(file)
{
	g_app.IncScript();
}

CFormFrame::~CFormFrame()
{
	g_app.DecScript();
}

HRESULT CFormFrame::OnStateChanged(VARIANT_BOOL Busy, VARIANT_BOOL Modified, VARIANT_BOOL CanPaste, VARIANT_BOOL CanUndo, VARIANT_BOOL CanRedo, LONG ItemCount, LONG SelectedItemCount)
{
	// save the form editor state
	m_FES_Busy=Busy;
	m_FES_Modified=Modified;
	m_FES_CanPaste=CanPaste;
	m_FES_CanUndo=CanUndo;
	m_FES_CanRedo=CanRedo;
	m_FES_ItemCount=ItemCount;
	m_FES_SelectedItemCount=SelectedItemCount;

	// set the modified flag
	if(Modified)
	{
		SetModified(TRUE);
	}
	BOOL bSel = SelectedItemCount > 0 ? TRUE : FALSE;
	UIEnable(ID_PASTE, CanPaste == VARIANT_TRUE ? TRUE : FALSE );
	UIEnable(ID_UNDO, CanUndo == VARIANT_TRUE ? TRUE : FALSE );
	UIEnable(ID_REDO, CanRedo == VARIANT_TRUE ? TRUE : FALSE );
	UIEnable(ID_CUT,bSel);
	UIEnable(ID_COPY,bSel);
	UIEnable(ID_DELETE,bSel);
	UIEnable(ID_SELECT_ALL,ItemCount > 0 ? TRUE : FALSE);
	UIEnable(ID_UNSELECT_ALL,bSel);
	// etc ...

	return S_OK;
}

HRESULT CFormFrame::OnEditScriptError(BSTR Source, long Number, BSTR description)
{
	USES_CONVERSION;
	LPCTSTR src = OLE2CT(Source);
	LPCTSTR desc = OLE2CT(description);
	CString s;

	s.Format("Error source: %s\r\nError number: %X\r\nDescription: %s",
		src,Number,desc);
	MessageBox(s,"Error",MB_ICONINFORMATION);
	return S_OK;
}

// CFormFrame
BOOL CFormFrame::PreTranslateMessage(MSG* pMsg)
{
	if(baseClass::PreTranslateMessage(pMsg))
		return TRUE;

	if(pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST)
		return FALSE;

	CWindow wnd = pMsg->hwnd;

	// determine which component the target window belongs to
	CComPtr<IUnknown> spUnknown;
	if(m_axFormEdit.IsWindow() && m_spFormEditor)          // form editor
	{
		if(wnd==m_axFormEdit || m_axFormEdit.IsChild(wnd))
		{
			spUnknown= m_spFormEditor;
		}
	}
	if(m_axScriptEdit.IsWindow() && m_spScriptEditor)  // script editor
	{
		if(wnd==m_axScriptEdit || m_axScriptEdit.IsChild(wnd))
		{
			spUnknown=m_spScriptEditor;
		}
	}
	if(m_axFormView.IsWindow() && m_spFormViewer)          // form viewer
	{
		if(wnd==m_axFormView || m_axFormView.IsChild(wnd))
		{
			spUnknown=m_spFormViewer;
		}
	}
	// if we need to handle the message
	if(spUnknown!=NULL)
	{
		// give the component chance to translate accelerators
		CComQIPtr<IOleInPlaceActiveObject> spOleInPlaceActiveObject(spUnknown);
		if(spOleInPlaceActiveObject!=NULL &&
			spOleInPlaceActiveObject->TranslateAccelerator(pMsg)!=S_FALSE)
		{
			return TRUE;    // message handled
		}
		// if required give the host chance to translate accelerators, for example
	}
	return FALSE;
}

LRESULT CFormFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT res = baseClass::OnCreate(uMsg,wParam,lParam,bHandled);
	bHandled = TRUE;

	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_FORMS), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_FORMS), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_FORMFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_FORMFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	UIAddToolBar(hWndToolBar);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_TOOLBAR2, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	UIAddToolBar(hWndToolBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);
	hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_TOOLBAR3, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	UIAddToolBar(hWndToolBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	CreateSimpleStatusBar();

	UIAddToolBar(hWndToolBar);

	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);
	UIEnable(ID_CUT,FALSE);
	UIEnable(ID_CUT,FALSE);
	UIEnable(ID_CUT,FALSE);
	UIEnable(ID_DELETE,FALSE);
	UIEnable(ID_SELECT_ALL,FALSE);
	UIEnable(ID_UNSELECT_ALL,FALSE);
	// etc ...

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);

	HRESULT hr;
	USES_CONVERSION;
	LPOLESTR p = NULL;

	hr = StringFromCLSID(DDFORMSLib::CLSID_FormEditor,&p);
	if(SUCCEEDED(hr) && p){
		m_axFormEdit.Create(m_hWnd,NULL,OLE2T(p),WS_CHILD|WS_VISIBLE,0,(HMENU)IDC_FORMEDITOR1);
		CoTaskMemFree(p);
		p = NULL;
	}
	hr = StringFromCLSID(DDFORMSLib::CLSID_SimpleScriptEditor,&p);
	if(SUCCEEDED(hr) && p){
		m_axScriptEdit.Create(m_hWnd,NULL,OLE2T(p),WS_CHILD|WS_VISIBLE,0,(HMENU)IDC_SIMPLESCRIPTEDITOR1);
		CoTaskMemFree(p);
		p = NULL;
	}
	hr = StringFromCLSID(DDFORMSLib::CLSID_FormViewer,&p);
	if(SUCCEEDED(hr) && p){
		m_axFormView.Create(m_hWnd,NULL,OLE2T(p),WS_CHILD|WS_VISIBLE,0,(HMENU)IDC_FORMVIEWER1);
		CoTaskMemFree(p);
		p = NULL;
	}
	if(SUCCEEDED(hr)){
		AtlAdviseSinkMap(this,true);
	}

	AddTab(m_axFormEdit,"form editor");
	AddTab(m_axScriptEdit,"script editor");
	AddTab(m_axFormView,"form view");

	
	hr = m_axFormEdit.QueryControl(&m_spFormEditor);
	if(SUCCEEDED(hr)){
		hr = m_axFormView.QueryControl(&m_spFormViewer);
		if(SUCCEEDED(hr)){
			hr = m_axScriptEdit.QueryControl(&m_spScriptEditor);
			if(SUCCEEDED(hr)){
				CComPtr<DDFORMSLib::IScript2> spScript;
				hr = m_spFormEditor->get_Script(&spScript);
				if(SUCCEEDED(hr)){
					CComPtr<IUnknown> sp;
					m_spScriptEditor.QueryInterface(&sp);
					hr = spScript->put_Editor(sp);
						
					if(SUCCEEDED(hr)){
						hr = m_spFormEditor->DDUnlock(0x355EFB54,0x55385022,0x019954C1,0x4ED0972B);
						if(SUCCEEDED(hr)){
							CComQIPtr<IDispatch> sp = (INo5Obj *)g_app.GetNo5Obj();
							ATLASSERT(!!sp);
							hr = m_spFormEditor->Expose(OLESTR("no5"),sp);
							if(SUCCEEDED(hr)){
								hr = m_spFormViewer->Expose(OLESTR("no5"),sp);
								if(SUCCEEDED(hr)){
									if(m_file.IsEmpty())
										FileNew();
									else
										FileOpen(m_file);
								}
							}
						}
					}
				}
			}
		}
	}
	// put font of script editor
	if(SUCCEEDED(hr) && m_spScriptEditor != NULL){
		CComPtr<IFontDisp> spFont;
		FONTDESC fd = { sizeof(fd),OLESTR("Courier"),FONTSIZE( 10 ),FW_NORMAL,ANSI_CHARSET,FALSE,FALSE,
			FALSE };
		HRESULT hr;

		hr = ::OleCreateFontIndirect(&fd,IID_IFontDisp,(void **)&spFont);
		if(SUCCEEDED(hr)){
			hr = m_spScriptEditor->put_Font(spFont);
		}
		ATLASSERT(SUCCEEDED(hr));
	}

	ATLASSERT(SUCCEEDED(hr));
	return 0;
}

LRESULT CFormFrame::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	baseClass::OnDestroy(uMsg,wParam,lParam,bHandled);	// DestroyTabWindow();
	AtlAdviseSinkMap(this,false);
	bHandled = TRUE;
	return 0;
}

LRESULT CFormFrame::OnNCDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CFormFrame::OnFileNew(WORD,WORD,HWND,BOOL &)
{
	FileNew();
	return 0;
}

LRESULT CFormFrame::OnFileOpen(WORD,WORD,HWND,BOOL &)
{
	CFileDialog dlg(\
		TRUE,		// file open
		"ddl",		// default extension
		NULL,		// file name
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,	// flags
		"ddf files\0*.ddf\0All files\0*.*\0",	// filter
		m_hWnd);	// parent

	CString path = g_app.GetFormsFolder();
	dlg.m_ofn.lpstrInitialDir = (LPCTSTR)path;
	if(IDOK == dlg.DoModal()){
		FileOpen(dlg.m_szFileName);
	}
	return 0;
}

LRESULT CFormFrame::OnFileSave(WORD,WORD,HWND,BOOL &)
{
	if(m_file.IsEmpty()){
		BOOL dummy;
		OnFileSaveAs(0,0,NULL,dummy);
	}
	else
		FileSave(m_file);
	return 0;
}

LRESULT CFormFrame::OnFileSaveAs(WORD,WORD,HWND,BOOL &)
{
	CFileDialog dlg(\
		FALSE,		// file open
		"ddf",		// default extension
		NULL,		// file name
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,	// flags
		"ddf files\0*.ddf\0All files\0*.*\0",	// filter
		m_hWnd);	// parent

	CString path = g_app.GetFormsFolder();
	dlg.m_ofn.lpstrInitialDir = (LPCTSTR)path;
	if(IDOK == dlg.DoModal()){
		FileSave(dlg.m_szFileName);
	}
	return 0;
}

LRESULT CFormFrame::OnAddFrame(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("DDControlPack.DDLabel"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
		OLE_COLOR clrBack;
		OLE_COLOR clrFore;
		CComVariant v;

		hr = m_spFormEditor->get_FormBackColor(&clrBack);
		hr = m_spFormEditor->get_FormForeColor(&clrFore);
		v = (long)3;
		spDispatch.PutProperty(DISPID_BORDERSTYLE,&v/*ddcpBorderStyleEtched*/);
		v = (long)clrFore;
		spDispatch.PutProperty(DISPID_FORECOLOR,&v);
		v = (long)clrBack;
		spDispatch.PutProperty(DISPID_BACKCOLOR,&v);
	}
	return 0;
}
LRESULT CFormFrame::OnAddRect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("DDControlPack.DDLabel"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
		OLE_COLOR clrBack;
		OLE_COLOR clrFore;
		CComVariant v;

		hr = m_spFormEditor->get_FormBackColor(&clrBack);
		hr = m_spFormEditor->get_FormForeColor(&clrFore);
		v = (long)3;
		spDispatch.PutProperty(DISPID_BORDERSTYLE,&v/*ddcpBorderStyleEtched*/);
		v = (long)clrFore;
		spDispatch.PutProperty(DISPID_FORECOLOR,&v);
		v = (long)clrBack;
		spDispatch.PutProperty(DISPID_BACKCOLOR,&v);
	}
	return 0;
}
LRESULT CFormFrame::OnAddPicture(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("DDControlPack.DDPicture"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
	}
	return 0;
}
LRESULT CFormFrame::OnAddLabel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("DDControlPack.DDLabel"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
		
	}
	return 0;
}
LRESULT CFormFrame::OnAddButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("DDControlPack.DDButton"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
	}
	return 0;
}
LRESULT CFormFrame::OnAddCheck(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("DDControlPack.DDCheckBox"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
	}
	return 0;
}
LRESULT CFormFrame::OnAddRadio(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("DDControlPack.DDRadioButton"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
	}
	return 0;
}

LRESULT CFormFrame::OnAddText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("DDControlPack.DDTextBox"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
	}
	return 0;
}
LRESULT CFormFrame::OnAddCombo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("DDControlPack.DDComboBox"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
	}
	return 0;
}
LRESULT CFormFrame::OnAddList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("DDControlPack.DDListBox"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
	}
	return 0;
}
LRESULT CFormFrame::OnAddHScroll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("DDControlPack.DDHorzScrollBar"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
	}
	return 0;
}
LRESULT CFormFrame::OnAddVScroll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("DDControlPack.DDVertScrollBar"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
	}
	return 0;
}
LRESULT CFormFrame::OnAddSpin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("MSComCtl2.UpDown"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
	}
	return 0;
}
LRESULT CFormFrame::OnAddSlider(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("MSComCtl2.Slider"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
	}
	return 0;
}
LRESULT CFormFrame::OnAddProgress(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComDispatchDriver spDispatch;
	HRESULT hr = m_spFormEditor->Insert(OLESTR("MSComCtlLib.ProgCtrl"),&spDispatch);
	if(SUCCEEDED(hr)){
		// manipulate the new item
	}
	return 0;
}
LRESULT CFormFrame::OnAddActivex(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HRESULT hr;
	CComBSTR progid;

	hr = m_spFormEditor->ShowInsertDialog(&progid);
	if(SUCCEEDED(hr) && progid.Length()){
		CComDispatchDriver spDispatch;
		hr = m_spFormEditor->Insert(progid,&spDispatch);
		if(SUCCEEDED(hr)){
			// manipulate the new item
		}
	}
	return 0;
}
LRESULT CFormFrame::OnDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_spFormEditor->Delete();
	return 0;
}
LRESULT CFormFrame::OnGrid(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	VARIANT_BOOL b;
	HRESULT hr = m_spFormEditor->get_GridVisible(&b);
	if(SUCCEEDED(hr)){
		m_spFormEditor->put_GridVisible(b == VARIANT_TRUE ? VARIANT_FALSE : VARIANT_TRUE);
	}
	return 0;
}
LRESULT CFormFrame::OnGridSettings(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}
LRESULT CFormFrame::OnTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_spFormEditor->SetTabOrder();
	return 0;
}
LRESULT CFormFrame::OnSelectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_spFormEditor->SelectAll();
	return 0;
}
LRESULT CFormFrame::OnUnselectAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_spFormEditor->SelectNone();
	return 0;
}
LRESULT CFormFrame::OnProperties(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_spFormEditor->Properties();
	return 0;
}

LRESULT CFormFrame::OnMoveCtl(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	switch(wID){
		case ID_LEFT:
			m_spFormEditor->MoveLeft();
			break;
		case ID_RIGHT:
			m_spFormEditor->MoveRight();
			break;
		case ID_UP:
			m_spFormEditor->MoveUp();
			break;
		case ID_DOWN:
			m_spFormEditor->MoveDown();
			break;
		default:
			break;
	}
	return 0;
}
LRESULT CFormFrame::OnSendBack(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_spFormEditor->SendToBack();
	return 0;
}
LRESULT CFormFrame::OnSendFront(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_spFormEditor->BringToFront();
	return 0;
}
LRESULT CFormFrame::OnAlignCtl(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	switch(wID){
		case ID_ALIGN_LEFT:
			m_spFormEditor->AlignLeft();
			break;
		case ID_ALIGN_RIGHT:
			m_spFormEditor->AlignRight();
			break;
		case ID_ALIGN_TOP:
			m_spFormEditor->AlignTop();
			break;
		case ID_ALIGN_BOTTOM:
			m_spFormEditor->AlignBottom();
			break;
		default:
			break;
	}
	return 0;
}
LRESULT CFormFrame::OnSpaceCtl(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	switch(wID){
		case ID_SPACE_HORZ:
			m_spFormEditor->SpaceHorizontally();
			break;
		case ID_SPACE_VERT:
			m_spFormEditor->SpaceVertically();
			break;
		default:
			break;
	}
	return 0;
}
LRESULT CFormFrame::OnSizeCtl(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	switch(wID){
		case ID_SIZE_LARGEST_WIDTH:
			m_spFormEditor->SizeWidthToLargest();
			break;
		case ID_SIZE_LARGEST_HEIGHT:
			m_spFormEditor->SizeHeightToLargest();
			break;
		case ID_SIZE_LARGEST:
			m_spFormEditor->SizeToLargest();
			break;
		case ID_SIZE_SMALLEST_WIDTH:
			m_spFormEditor->SizeWidthToSmallest();
			break;
		case ID_SIZE_SMALLEST_HEIGHT:
			m_spFormEditor->SizeHeightToSmallest();
			break;
		case ID_SIZE_SMALLEST:
			m_spFormEditor->SizeToSmallest();
			break;
		default:
			break;
	}
	return 0;
}
LRESULT CFormFrame::OnCenterCtl(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	switch(wID){
		case ID_CENTER_HORZ:
			m_spFormEditor->FormAlignHCenter();
			break;
		case ID_CENTER_VERT:
			m_spFormEditor->FormAlignVCenter();
			break;
		case ID_CENTER:
			m_spFormEditor->FormAlignBoth();
			break;
		default:
			break;
	}
	return 0;
}
LRESULT CFormFrame::OnValidateScript(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HRESULT hr;
	VARIANT_BOOL res;
	hr = m_spFormEditor->ValidateScript(&res);
	if(SUCCEEDED(hr)){
		if(res == VARIANT_TRUE)
			MessageBox("no errors");
	}
	return 0;
}
LRESULT CFormFrame::OnPreviewAuto(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	VARIANT_BOOL bValid;
	HRESULT hr;

	hr = m_spFormEditor->ValidateScript(&bValid);
	if(SUCCEEDED(hr) && bValid == VARIANT_TRUE){
		// create the preview dialog
		CPreviewDlg *pDlg = new CPreviewDlg(m_hWnd,TRUE);
		USES_CONVERSION;
		pDlg->m_AutoSizing=TRUE;
		pDlg->m_FileName=MakeTempName();

		// expose objects - see OnInitialUpdate() for more details
		// class1
		CExposedObjectInfo ExposedObjectInfo;
		CComQIPtr<IDispatch> sp = (INo5Obj *)(g_app.GetNo5Obj());
		ExposedObjectInfo.m_Name=_T("no5");
		ExposedObjectInfo.m_spDispatch= sp;
		pDlg->m_ExposedObjectInfoArray.Add(ExposedObjectInfo);

		// save the form
		hr = m_spFormEditor->SaveToFile(T2OLE(pDlg->m_FileName));
		if(SUCCEEDED(hr)){
			// show the preview
			pDlg->Create(m_hWnd);
			pDlg->ShowWindow(SW_NORMAL);
		}
	}
	return 0;
}

LRESULT CFormFrame::OnPreview(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	VARIANT_BOOL bValid;
	HRESULT hr;

	hr = m_spFormEditor->ValidateScript(&bValid);
	if(SUCCEEDED(hr) && bValid == VARIANT_TRUE){
		// create the preview dialog
		CPreviewDlg *pDlg = new CPreviewDlg(m_hWnd,TRUE);
		USES_CONVERSION;
		pDlg->m_AutoSizing=FALSE;
		pDlg->m_FileName=MakeTempName();
	
		// expose objects - see OnInitialUpdate() for more details
		CExposedObjectInfo ExposedObjectInfo;
		CComQIPtr<IDispatch> sp = (INo5Obj *)(g_app.GetNo5Obj());
		ExposedObjectInfo.m_Name=_T("no5");
		ExposedObjectInfo.m_spDispatch= sp;
		pDlg->m_ExposedObjectInfoArray.Add(ExposedObjectInfo);

		// save the form
		hr = m_spFormEditor->SaveToFile(T2OLE(pDlg->m_FileName));
		if(SUCCEEDED(hr)){
			// show the preview
			pDlg->Create(m_hWnd);
			pDlg->ShowWindow(SW_NORMAL);
		}
	}
	return 0;
}

LRESULT CFormFrame::OnEditCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HRESULT hr = S_OK;

	switch(wID){
		case ID_CUT:
			hr = m_spFormEditor->Cut();
			break;
		case ID_COPY:
			hr = m_spFormEditor->Copy();
			break;
		case ID_PASTE:
			hr = m_spFormEditor->Paste();
			break;
		case ID_UNDO:
			hr = m_spFormEditor->Undo();
			break;
		case ID_REDO:
			hr = m_spFormEditor->Redo();
			break;
		default:
			break;
	}
	ATLASSERT(SUCCEEDED(hr));
	return 0;
}

LRESULT CFormFrame::OnTabSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	LRESULT lRes = baseClass::OnSelChange(idCtrl,pnmh,bHandled);
	bHandled = TRUE;

	int idx = m_TabCtrl.GetCurSel();

	if(idx == 2){
		USES_CONVERSION;
		HRESULT hr = m_spFormEditor->SaveToFile(T2OLE(MakeTempName()));

		if(SUCCEEDED(hr)){
			hr = m_spFormViewer->LoadFromFile(T2OLE(MakeTempName()));
		}
		ATLASSERT(SUCCEEDED(hr));
	}
	return lRes;
}

LRESULT CFormFrame::OnTabSelChanging(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	LRESULT lRes = baseClass::OnSelChanging(idCtrl,pnmh,bHandled);
	bHandled = TRUE;

	int idx = m_TabCtrl.GetCurSel();

	if(idx == 2){
		// force unload of form viewer
		m_spFormViewer->Unload();
	}
	return lRes;
}

void CFormFrame::FileNew(void)
{
	HRESULT hr = m_spFormEditor->New();
	hr = m_spScriptEditor->put_Modified(VARIANT_FALSE);
	if(SUCCEEDED(hr)){
		SetModified(FALSE);
		m_file.Empty();
		CComPtr<DDFORMSLib::IScript2> sp;
		hr = m_spFormEditor->get_Script(&sp);
		if(SUCCEEDED(hr)){
			sp->put_Text(OLESTR(""));
		}
	}
}

void CFormFrame::FileOpen(LPCTSTR file)
{
	USES_CONVERSION;
	HRESULT hr = m_spFormEditor->LoadFromFile(T2OLE(file));
	if(SUCCEEDED(hr)){
		hr = m_spScriptEditor->put_Modified(VARIANT_FALSE);
		if(SUCCEEDED(hr)){
			SetModified(FALSE);
			m_file = file;
		}
	}
}

void CFormFrame::FileSave(LPCTSTR file)
{
	USES_CONVERSION;
	HRESULT hr = m_spFormEditor->SaveToFile(T2OLE(file));
	if(SUCCEEDED(hr)){
		hr = m_spScriptEditor->put_Modified(VARIANT_FALSE);
		if(SUCCEEDED(hr)){
			SetModified(FALSE);
			m_file = file;
		}
	}
}

CString CFormFrame::MakeTempName(void)
{
	CString s("temp.ddf");
	return s;
}

// CPreviewDlg

CPreviewDlg::CPreviewDlg(HWND hWndOwner,BOOL bModeless):\
		m_bModeless(bModeless),
		m_wndOwner(hWndOwner)
{
	m_AutoSizing = FALSE;
	g_app.IncScript();
}

CPreviewDlg::~CPreviewDlg()
{
	g_app.DecScript();
}

LRESULT CPreviewDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HRESULT hr = S_OK;
	CMessageLoop *p = _Module.GetMessageLoop();
	p->AddMessageFilter(this);

	// create host window and control
	{
		USES_CONVERSION;
		LPOLESTR p = NULL;
		hr = StringFromCLSID(DDFORMSLib::CLSID_FormViewer,&p);
		if(SUCCEEDED(hr) && p){
			m_ax.Create(m_hWnd,NULL,OLE2T(p),WS_CHILD|WS_VISIBLE,0,(HMENU)IDC_FORMVIEWER1);
			CoTaskMemFree(p);
			p = NULL;

			hr = m_ax.QueryControl(&m_spFormViewer);
			ATLASSERT(SUCCEEDED(hr));
			if(FAILED(hr))
				return 0;
		}
	}

	// set the caption
	CString Caption;
	int len = GetWindowTextLength();
	GetWindowText(Caption.GetBuffer(len + 1),len + 1);
	Caption.ReleaseBuffer();
	Caption+=m_AutoSizing ? _T(" (Auto Sizing)") : _T(" (None Auto Sizing)");
	SetWindowText(Caption);
	USES_CONVERSION;

	// expose objects
	for(long l=0; l<m_ExposedObjectInfoArray.GetSize(); l++)
	{
		
		m_spFormViewer->Expose(T2OLE(m_ExposedObjectInfoArray[l].m_Name),
			m_ExposedObjectInfoArray[l].m_spDispatch);
	}

	// load the form
	m_spFormViewer->LoadFromFile(T2OLE(m_FileName));

	// set the form viewer background color (optional but looks better)
	//m_FormViewer.SetBackColor(m_FormViewer.GetForm().GetBackColor());

	// size the dialog
	if(m_AutoSizing)
	{
		// calculate the none client area
		CRect WindowRect;
		GetWindowRect(WindowRect);
		CRect ClientRect;
		GetClientRect(ClientRect);
		CComPtr<DDFORMSLib::IViewableForm> sp;
		LONG FormWidth,FormHeight;

		m_spFormViewer->get_Form(&sp);
		sp->get_Width(&FormWidth);
		sp->get_Height(&FormHeight);

		long NCWidth=WindowRect.Width()-ClientRect.Width();
		long NCHeight=WindowRect.Height()-ClientRect.Height();

		// calculate the required dialog dimensions
		long Width=NCWidth+FormWidth;
		long Height=NCHeight+FormHeight;

		//Width += GetSystemMetrics(SM_CXVSCROLL);
		//Height += GetSystemMetrics(SM_CYHSCROLL);

		// prevent screen overrun
		Width=min(Width,GetSystemMetrics(SM_CXSCREEN));
		Height=min(Height,GetSystemMetrics(SM_CYSCREEN));

		// size and position the dialog
		(void)SetWindowPos(NULL,0,0,Width,Height,SWP_NOZORDER);
		CenterWindow();
	}

	// size the form viewer
	CRect ClientRect(0,0,0,0);
	GetClientRect(ClientRect);
	ATLASSERT(m_ax.IsWindow());
	(void)m_ax.SetWindowPos(NULL,0,0,
		ClientRect.Width(),ClientRect.Height(),SWP_NOZORDER);

	// give focus to the form viewer
	(void)m_ax.SetFocus();
	//DlgResize_Init((m_AutoSizing ? false: true),false);

	return FALSE; // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPreviewDlg::OnNCDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_bModeless){
		m_wndOwner.PostMessage(NO5WM_FORMVIEW,0,(LPARAM)this);
	}
	bHandled = FALSE;
	return 0;
}

BOOL CPreviewDlg::PreTranslateMessage(MSG* pMsg)
{
	if(IsDialogMessage(pMsg))
		return TRUE;

	if(pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST)
		return FALSE;

	CWindow wnd = pMsg->hwnd;

	// determine which component the target window belongs to
	CComPtr<IUnknown> spUnknown;
	
	if(m_ax.IsWindow() && m_spFormViewer)          // form viewer
	{
		if(wnd==m_ax || m_ax.IsChild(wnd))
		{
			spUnknown=m_spFormViewer;
		}
	}
	// if we need to handle the message
	if(spUnknown!=NULL)
	{
		// give the component chance to translate accelerators
		CComQIPtr<IOleInPlaceActiveObject> spOleInPlaceActiveObject(spUnknown);
		if(spOleInPlaceActiveObject!=NULL &&
			spOleInPlaceActiveObject->TranslateAccelerator(pMsg)!=S_FALSE)
		{
			return TRUE;    // message handled
		}
		// if required give the host chance to translate accelerators, for example
	}
	return FALSE;
}
