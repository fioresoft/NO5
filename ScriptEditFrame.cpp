// ScriptEditFrame.cpp: implementation of the CScriptEditFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ScriptEditFrame.h"
#include "viksoe\rtfscripteditor.h"
#include "no5app.h"
#include <no5tl\winfile.h>
#include <no5tl\color.h>
#include "path.h"
#include "usrmsgs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScriptEditFrame::CScriptEditFrame(HWND hWndOwner,LPCTSTR file):m_wndOwner(hWndOwner)
{
	m_lang = LANG_NONE;
	if(file){
		m_file = file;
		m_lang = GetFileLang(file);
	}
	m_pEdit = new CRtfScriptEditorCtrl();
}

CScriptEditFrame::~CScriptEditFrame()
{
	delete m_pEdit;
}

STDMETHODIMP CScriptEditFrame::LookupNamedItem(LPCOLESTR pstrName,LPUNKNOWN* ppunkItem)
{
	USES_CONVERSION;
	LPCTSTR p = OLE2CT(pstrName);
	HRESULT hr = TYPE_E_ELEMENTNOTFOUND;

	if(lstrcmpi(p,"no5") == 0){
		CComObject<CNo5Obj> *pApp = g_app.GetNo5Obj();
		if(pApp){
			hr = pApp->QueryInterface(IID_INo5Obj,(void **)ppunkItem);
		}
	}
	return hr;
}

LRESULT CScriptEditFrame::OnInitDialog(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_SCRIPT), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_SCRIPT), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	m_pEdit->Create(m_hWnd,rcDefault,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|\
		ES_LEFT|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,WS_EX_CLIENTEDGE,IDC_SCRIPTEDIT);

	
	LoadKeywords();
	m_pEdit->SetCaseSensitive(FALSE);
	m_pEdit->SetSyntax(SC_COMMENT,"'");

	SYNTAXCOLOR sc;
	sc.bBold = TRUE;
	sc.bUnderline = FALSE;
	sc.clrText = RGB(0,0,160);
	m_pEdit->SetSyntaxColor(SC_KEYWORD,sc);

	sc.bBold = FALSE;
	sc.bUnderline = FALSE;
	sc.clrText = RGB(0,120,0);
	m_pEdit->SetSyntaxColor(SC_COMMENT,sc);

	sc.bBold = FALSE;
	sc.bUnderline = FALSE;
	sc.clrText = NO5TL::Colors::GREEN;
	m_pEdit->SetSyntaxColor(SC_STRING,sc);


	if(!m_file.IsEmpty()){
		CPath path(m_file,FALSE /*folder*/,FALSE /* no args*/);
		ReadFile(m_file);
		SetWindowText(path.GetFileTitle());
	}

	LoadKeywords();
	CenterWindow();

	CRect rcStatic;
	CWindow wndStatic = GetDlgItem(IDC_STATIC1);
	wndStatic.GetWindowRect(&rcStatic);
	wndStatic.ShowWindow(SW_HIDE);
	ScreenToClient(&rcStatic);
	m_pEdit->MoveWindow(rcStatic.left,rcStatic.top,rcStatic.Width(),rcStatic.Height());
	m_pEdit->SetDelayedFormat(FALSE);

	m_cbObjs = GetDlgItem(IDC_COMBO_OBJS);
	m_cbEvents = GetDlgItem(IDC_COMBO_EVENTS);
	m_btStart = GetDlgItem(ID_SCRIPT_RUN);
	m_btStop = GetDlgItem(ID_SCRIPT_STOP);

	HRESULT hr = Initiate((m_lang == LANG_JS ? "jscript" : "vbscript"));
	if(SUCCEEDED(hr)){
		hr = AddObject("no5",FALSE,TRUE);
		if(SUCCEEDED(hr)){
			FillObjsCombo();
			m_cbObjs.SetCurSel(0);
			BOOL dummy;
			OnComboObjsChange(0,0,0,dummy);
		}
	}
	DlgResize_Init(true,false);

	m_btStart.SetIcon(LoadIcon(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDI_START)));
	m_btStop.SetIcon(LoadIcon(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDI_STOP)));
	m_btStop.EnableWindow(FALSE);
	m_pEdit->SetFocus();

	return TRUE;
}

LRESULT CScriptEditFrame::OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_spEngine)
		Terminate();
	return 0;
}

LRESULT CScriptEditFrame::OnNCDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_wndOwner.PostMessage(NO5WM_SCRIPTWND,0,(LPARAM)this);
	bHandled = FALSE;
	return 0;
}

LRESULT CScriptEditFrame::OnClose(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DestroyWindow();
	return 0;
}

LRESULT CScriptEditFrame::OnScriptRun(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{	
	if(wNotifyCode == 0 || wNotifyCode == BN_CLICKED){
		if(!m_bRunning){
			HRESULT hr = StartScript();
			if(SUCCEEDED(hr)){
				int len = m_pEdit->GetWindowTextLength();
				if(len){
					char *buf = new char[len + 1];

					buf[0] = '\0';
					m_pEdit->GetWindowText(buf,len+1);

					hr = AddScript(buf);
					delete buf;
					if(SUCCEEDED(hr)){
						m_btStart.EnableWindow(FALSE);
						m_btStop.EnableWindow(TRUE);
						g_app.IncScript();
					}
				}
			}
			if(FAILED(hr)){
				BOOL dummy;
				MessageBox("Error","Error");
				OnScriptStop(0,wID,hWndCtl,dummy);
			}
		}
	}

	return 0;
}

LRESULT CScriptEditFrame::OnScriptStop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{
	if(wNotifyCode == 0 || wNotifyCode == BN_CLICKED){
		HRESULT hr = Terminate();
		if(SUCCEEDED(hr)){
			HRESULT hr = Initiate((m_lang == LANG_JS ? "jscript" : "vbscript"));
			if(SUCCEEDED(hr)){
				hr = AddObject("no5",FALSE,TRUE);
				if(SUCCEEDED(hr)){
					FillObjsCombo();
					m_cbObjs.SetCurSel(0);
					BOOL dummy;
					OnComboObjsChange(0,0,0,dummy);
					g_app.DecScript();
				}
			}
			m_btStart.EnableWindow(TRUE);
			m_btStop.EnableWindow(FALSE);
			ATLASSERT(SUCCEEDED(hr));
		}
	}

	return 0;
}

LRESULT CScriptEditFrame::OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_file.IsEmpty()){
		BOOL dummy;
		OnFileSaveAs(0,0,0,dummy);
	}
	else{
		BOOL res = WriteFile(m_file);
		ATLASSERT(res);
	}
	return 0;
}

LRESULT CScriptEditFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(\
		TRUE,			// file open
		"vbs",			// def ext
		m_file,			// file name
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
		"vbscript ( .vbs )\0*.vbs\0jscript\0*.js\0text files\0*.txt\0All files\0*.*\0",	// filter
		m_hWnd);

	CString path = g_app.GetFormsFolder();
	dlg.m_ofn.lpstrInitialDir = (LPCTSTR)path;

	if(dlg.DoModal() == IDOK){
		m_pEdit->SetWindowText("");
		BOOL res = ReadFile(dlg.m_szFileName);
		ATLASSERT(res);
		if(res){
			ScriptLang lang;
			m_file = dlg.m_szFileName;
			lang = GetFileLang(m_file);
			SetWindowText(dlg.m_szFileTitle);
			if(lang != m_lang){
				BOOL b;
				OnScriptStop(0,0,0,b);
				LoadKeywords();
			}
		}
	}
	return 0;
}

LRESULT CScriptEditFrame::OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(\
		FALSE,			// not file open
		"vbs",			// def ext
		m_file,			// file name
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		"vbscript ( .vbs )\0*.vbs\0jscript\0*.js\0text files\0*.txt\0All files\0*.*\0",	// filter
		m_hWnd);

	CString path = g_app.GetFormsFolder();
	dlg.m_ofn.lpstrInitialDir = (LPCTSTR)path;

	if(dlg.DoModal() == IDOK){
		BOOL res = WriteFile(dlg.m_szFileName);
		ATLASSERT(res);
		if(res){
			ScriptLang lang;
			m_file = dlg.m_szFileName;
			lang = GetFileLang(m_file);
			SetWindowText(dlg.m_szFileTitle);
			if(lang != m_lang){
				BOOL b;
				OnScriptStop(0,0,0,b);
				LoadKeywords();
			}
		}
	}
	return 0;
}

LRESULT CScriptEditFrame::OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CScriptEditFrame::OnComboObjsChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	FillEventsCombo();
	return 0;
}

LRESULT CScriptEditFrame::OnComboEventsChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int idx = m_cbObjs.GetCurSel();
	if(idx >= 0){
		CString objName;
		m_cbObjs.GetLBText(idx,objName);
		idx = m_cbEvents.GetCurSel();
		if(idx >= 0){
			CString event;
			m_cbEvents.GetLBText(idx,event);

			if(!objName.IsEmpty() && !event.IsEmpty()){
				CSimpleArray<CString> args;
				if(GetObjectEventParams(objName,event,args)){
					CString s;
					s = "Sub ";
					s += objName;
					s += '_';
					s += event;
					s += '(';
					for(int i=0;i<args.GetSize();i++){
						s += args[i];
						if(i != args.GetSize() - 1)
							s += ',';
					}
					s += ")\r\n' add your code here\r\nEnd Sub\r\n";
					m_pEdit->SetSel(-1,-1);
					m_pEdit->AppendText(s);
					m_pEdit->FormatAll();
				}
			}
		}
	}
	return 0;
}

BOOL CScriptEditFrame::ReadFile(LPCTSTR file)
{
   BOOL res = FALSE;
   HANDLE hFile = ::CreateFile(file,GENERIC_READ,FILE_SHARE_READ,NULL,
	   OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hFile){
		TCHAR chunk[252];
		DWORD dwRead;
		const DWORD dwToRead = sizeof(chunk)/sizeof(chunk[0]) - sizeof(TCHAR);

		 while(true){
			 ZeroMemory(chunk,sizeof(chunk)/sizeof(chunk[0]));
			 dwRead = 0;
			 res = ::ReadFile(hFile,chunk,dwToRead,&dwRead,
				 NULL);
			 if(!res || dwRead == 0)
				 break;
			 m_pEdit->AppendText(chunk);
		}
		CloseHandle(hFile);
		m_pEdit->SetSel(0,0);
	}
   return res;
 }

BOOL CScriptEditFrame::WriteFile(LPCTSTR file)
{
   int len = m_pEdit->GetWindowTextLength();
   BOOL res = FALSE;

   if(len >= 0){
	   TCHAR *buf = new TCHAR[len + 1];
	   ZeroMemory(buf,(len + 1) * sizeof(TCHAR));
	   m_pEdit->GetWindowText(buf,len + 1);

	   HANDLE hFile = CreateFile(file,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
		   FILE_ATTRIBUTE_NORMAL,NULL);

	   if(hFile){
		   DWORD dwWritten = 0;

		   res = ::WriteFile(hFile,buf,(DWORD)len,&dwWritten,NULL);
		   ATLASSERT(dwWritten == (DWORD)len);
		   CloseHandle(hFile);
	   }
   }
   return res;
}

void CScriptEditFrame::LoadKeywords(void)
{
	CString path;
	NO5TL::CWinFile wf;

	m_pEdit->ClearConstants();
	m_pEdit->ClearKeywords();
	path = g_app.GetPath();
	if(m_lang == LANG_JS){
		path += "jskeys.txt";
		m_pEdit->SetCaseSensitive(TRUE);
		m_pEdit->SetChangeCase(TRUE);
		m_pEdit->SetSyntax(SC_COMMENT,"//");
		m_pEdit->SetSyntax(SC_STRING,"\"");
	}
	else{
		path += "vbskeys.txt";
		m_pEdit->SetCaseSensitive(TRUE);
		m_pEdit->SetChangeCase(TRUE);
		m_pEdit->SetSyntax(SC_COMMENT,"\'");
		m_pEdit->SetSyntax(SC_STRING,"\"");
	}

	if(wf.Create(path,OPEN_EXISTING,GENERIC_READ)){
		CSimpleArray<CString> a;
		if(wf.ReadLines(a)){
			const int count = a.GetSize();
			for(int i=0;i<count;i++)
				m_pEdit->AddKeyword(a[i]);
		}
	}
}

void CScriptEditFrame::FillObjsCombo(void)
{
	CSimpleArray<CString> a;

	m_cbObjs.ResetContent();
	GetObjects(a);
	for(int i=0;i<a.GetSize();i++)
		m_cbObjs.AddString(a[i]);
}

void CScriptEditFrame::FillEventsCombo(void)
{
	CSimpleArray<CString> events;
	CString obj;
	int idx;

	idx = m_cbObjs.GetCurSel();
	if(idx >= 0){
		m_cbObjs.GetLBText(idx,obj);
		GetObjectEvents(obj,events);
		m_cbEvents.ResetContent();
		for(int i=0;i<events.GetSize();i++)
			m_cbEvents.AddString(events[i]);
	}
}

void CScriptEditFrame::GetObjects(CSimpleArray<CString> &objs)
{
	objs.Add(CString("no5"));
}

BOOL CScriptEditFrame::GetObjectMembers(LPCTSTR name,CSimpleArray<CString> &methods,CSimpleArray<CString> &props)
{
	USES_CONVERSION;
	CComVariant varRes;
	HRESULT hr;

	hr = m_spParser->ParseScriptText(T2OLE(name),NULL,NULL,NULL,0,0,
		SCRIPTTEXT_ISEXPRESSION,&varRes,NULL);
	if (FAILED(hr))
		return FALSE;
	if (varRes.vt != VT_DISPATCH)
		return FALSE;
	CComPtr<IDispatch> spDisp = varRes.pdispVal;
	CComPtr<ITypeInfo> spti;
	hr = spDisp->GetTypeInfo(0,0,&spti);
	if(FAILED(hr))
		return FALSE;
	TYPEATTR *pta = NULL;
	spti->GetTypeAttr(&pta);
	if(!pta)
		return FALSE;
	BOOL result = FALSE;
	int i;
	for (i=0; i<pta->cFuncs; i++)
	{
		FUNCDESC* pfd;
		spti->GetFuncDesc(i, &pfd);
		if ((pfd->wFuncFlags & (FUNCFLAG_FRESTRICTED | 
			FUNCFLAG_FHIDDEN | FUNCFLAG_FNONBROWSABLE)) == 0)
		{
			CComBSTR bstrName;
			spti->GetDocumentation(pfd->memid, &bstrName, NULL, NULL, NULL);
			CString str = OLE2T(bstrName);
			if (str[0] != '_')
			{
				if (pfd->invkind == INVOKE_FUNC)
				{
					methods.Add(str);
					result = TRUE;
				}
				else if (pfd->invkind == INVOKE_PROPERTYGET)
				{
					props.Add(str);
					result = TRUE;
				}
			}
		}
		spti->ReleaseFuncDesc(pfd);
	}
	for (i = 0; i < pta->cVars; i++)
	{
		VARDESC* pvd;
		spti->GetVarDesc(i, &pvd);
		if (pvd->varkind == VAR_DISPATCH && (pvd->wVarFlags & (VARFLAG_FHIDDEN | 
			VARFLAG_FRESTRICTED | VARFLAG_FNONBROWSABLE)) == 0   )
		{
			CComBSTR bstrName;
			spti->GetDocumentation(pvd->memid, &bstrName, NULL, NULL, NULL);
			CString str = OLE2T(bstrName);
			if (str[0] != '_')
			{
				props.Add(str);
				result = TRUE;
			}
		}
		spti->ReleaseVarDesc(pvd);
	}
	spti->ReleaseTypeAttr(pta);
	return result;
}

BOOL CScriptEditFrame::GetObjectEvents(LPCTSTR name,CSimpleArray<CString> &events)
{
	USES_CONVERSION;
	CComPtr<ITypeInfo> spti;
	HRESULT hr = GetItemInfo(T2COLE(name),SCRIPTINFO_ITYPEINFO,NULL,&spti);
	if (FAILED(hr))
		return FALSE;

	TYPEATTR *pta;
	spti->GetTypeAttr(&pta);

	CComPtr<ITypeInfo> sprti;
	if (pta->cImplTypes < 2)
		return TRUE;
	
	HREFTYPE rt;
	spti->GetRefTypeOfImplType(1,&rt);
	spti->GetRefTypeInfo(rt,&sprti);
	TYPEATTR *prta;
	sprti->GetTypeAttr(&prta);
	for (int j=0; j<prta->cFuncs; j++)
	{
		FUNCDESC* pfd;
		sprti->GetFuncDesc(j, &pfd);
		CComBSTR bstrName;
		sprti->GetDocumentation(pfd->memid, &bstrName, NULL, NULL, NULL);
		events.Add(CString(OLE2T(bstrName)));
		sprti->ReleaseFuncDesc(pfd);
	}
	sprti->ReleaseTypeAttr(prta);	
	spti->ReleaseTypeAttr(pta);
	return TRUE;
}

BOOL CScriptEditFrame::GetObjectFunctionParams(LPCTSTR objName,LPCTSTR method,CSimpleArray<CString> & args)
{
	if (!m_spEngine || !m_bInit)
		return FALSE;
	USES_CONVERSION;
	CComVariant varRes;
	HRESULT hr;

	hr = m_spParser->ParseScriptText(T2OLE(objName),NULL,NULL,NULL,0,0,
		SCRIPTTEXT_ISEXPRESSION,&varRes,NULL);
	if (FAILED(hr))
		return FALSE;
	if (varRes.vt != VT_DISPATCH)
		return FALSE;
	CComPtr<IDispatch> pDisp = varRes.pdispVal;
	CComPtr<ITypeInfo> ppti;
	pDisp->GetTypeInfo(0,0,&ppti);
	BSTR bstrNames[1];
	MEMBERID memId[1];

	BSTR bstrParams[20];
	UINT numParams;
	bstrNames[0] =T2OLE(method); 
	hr = ppti->GetIDsOfNames(bstrNames,1,memId);
	if (FAILED(hr))
		return FALSE;
	hr = ppti->GetNames(memId[0],bstrParams,20,&numParams);
	if (FAILED(hr))
		return FALSE;
	BOOL result = FALSE;
	for (UINT i = 1; i < numParams; i++)
	{
		args.Add(CString(OLE2CT(bstrParams[i])));
		SysFreeString(bstrParams[i]);
		result = TRUE;
	}
	return result;
}

BOOL CScriptEditFrame::GetObjectEventParams(LPCTSTR objName,LPCTSTR event,CSimpleArray<CString> &args)
{
	if (!m_spEngine || !m_bInit)
		return FALSE;
	USES_CONVERSION;

	BSTR bstrNames[1];
	MEMBERID memId[1];

	BSTR bstrParams[20];
	UINT numParams;

	CComPtr<ITypeInfo> spti;
	GetItemInfo(T2COLE(objName),SCRIPTINFO_ITYPEINFO,NULL,&spti);

	TYPEATTR *pta;
	spti->GetTypeAttr(&pta);

	CComPtr<ITypeInfo> sprti;
	if (pta->cImplTypes < 2)
		return FALSE;
	
	HREFTYPE rt;
	spti->GetRefTypeOfImplType(1,&rt);
	spti->GetRefTypeInfo(rt,&sprti);

	
	bstrNames[0] =T2OLE(event); 
	sprti->GetIDsOfNames(bstrNames,1,memId);
	
	sprti->GetNames(memId[0],bstrParams,20,&numParams);
	for (UINT i = 1; i < numParams; i++)
	{
		args.Add(CString(OLE2CT(bstrParams[i])));
		SysFreeString(bstrParams[i]);
	}
	spti->ReleaseTypeAttr(pta);
	return TRUE;
}

LPCTSTR GetFileExt(LPCTSTR file)
{
	LPCTSTR pRes = NULL;

	if(file){
		pRes = file + lstrlen(file);
		while(pRes != file && *pRes != '.'){
			pRes--;
		}
		if(*pRes == '.')
			pRes++;
		else
			pRes = NULL;
	}
	return pRes;
}

ScriptLang GetFileLang(LPCTSTR file)
{
	LPCTSTR p = GetFileExt(file);
	ScriptLang res = LANG_NONE;

	if(p){
		if(!lstrcmpi(p,"vbs"))
			res = LANG_VBS;
		else if(!lstrcmpi(p,"js"))
			res = LANG_JS;
	}
	return res;
}
