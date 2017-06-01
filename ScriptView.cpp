// ScriptView.cpp: implementation of the CScriptView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ScriptView.h"
#include "no5app.h"
#include <no5tl\scriptsite.h>
#include <no5tl\winfile.h>
#include "scripteditframe.h"
#include "imainframe.h"

class CSimpleScriptSite :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CScriptSiteImpl
{
public:
	CSimpleScriptSite()
	{
		//
	}
	virtual ~CSimpleScriptSite()
	{
		ATLTRACE("~CSimpleScriptSite\n");
	}

DECLARE_PROTECT_FINAL_CONSTRUCT()
BEGIN_COM_MAP(CScriptSiteBasic)
	COM_INTERFACE_ENTRY(IActiveScriptSite)
	COM_INTERFACE_ENTRY(IActiveScriptSiteWindow)
END_COM_MAP()

	STDMETHOD(LookupNamedItem)(LPCOLESTR pstrName,LPUNKNOWN* ppunkItem)
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
};

CScriptView::CScriptView()
{

}

CScriptView::~CScriptView()
{
	ATLTRACE("~CScriptView\n");
}

LRESULT CScriptView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	//pLoop->AddIdleHandler(this);
	//UIAddChildWindowContainer(m_hWnd);

	m_lb1.Attach(GetDlgItem(IDC_LIST1));
	m_lb2.Attach(GetDlgItem(IDC_LIST2));
	m_bt1.Attach(GetDlgItem(IDC_BUTTON1));
	DlgResize_Init(false,false,WS_CLIPCHILDREN);
	ReloadScriptList();
	return TRUE;
}

LRESULT CScriptView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	ReleaseRunning();
	return 0;
}

LRESULT CScriptView::OnList1DClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int idx = m_lb1.GetCurSel();
	if(idx >= 0){
		CString sFile;

		if(m_lb1.GetText(idx,sFile) > 0){
			CComObject<CSimpleScriptSite> *pObj = NULL;
			HRESULT hr = CComObject<CSimpleScriptSite>::CreateInstance(&pObj);

			if(SUCCEEDED(hr)){
				CComQIPtr<IActiveScriptSite> sp;
				hr = pObj->QueryInterface(&sp);
				if(SUCCEEDED(hr)){
					sp.Detach();
					hr = pObj->Initiate("vbscript",m_hWnd);
					if(SUCCEEDED(hr)){
						hr = pObj->AddObject("no5",FALSE,TRUE);
						if(SUCCEEDED(hr)){
							hr = pObj->Run();
							if(SUCCEEDED(hr)){
								CString buf;
								BOOL res = ReadScript(sFile,buf);
								if(res){
									hr = pObj->AddScript(buf);
								}
							}
						}
					}
				}
			}
			if(FAILED(hr) && pObj != NULL){
				CString msg;
				msg.Format("Error %08x\n%s",hr,(LPCTSTR)(NO5TL::GetErrorDesc(hr)));
				MessageBox(msg);
				delete pObj;
			}
			else{
				// dont delete we may want to run it more than once
				// m_lb1.DeleteString(idx);
				idx = m_lb2.AddString(sFile);
				if(idx >= 0)
					m_lb2.SetItemDataPtr(idx,(void *)pObj);
				g_app.IncScript();
			}
		}
	}
	return 0;
}

LRESULT CScriptView::OnList2DClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int idx = m_lb2.GetCurSel();
	if(idx >= 0){
		CString sFile;

		if(m_lb2.GetText(idx,sFile) > 0){
			CComObject<CSimpleScriptSite> *pObj = (CComObject<CSimpleScriptSite> *)m_lb2.GetItemDataPtr(idx);

			if(pObj){
				HRESULT hr = pObj->Terminate();
				if(SUCCEEDED(hr)){
					ULONG x = pObj->Release();
					m_lb2.SetItemData(idx,0);
					m_lb2.DeleteString(idx);
					//idx = m_lb1.AddString(sFile);
					g_app.DecScript();
				}
			}
		}
	}
	return 0;
}


LRESULT CScriptView::OnBtRun(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL b;
	OnList1DClick(0,0,0,b);
	return 0;
}

// edit ( only not running scripts )
LRESULT CScriptView::OnBtEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int idx = m_lb1.GetCurSel();
	if(idx >= 0){
		CString sFile;

		if(m_lb1.GetText(idx,sFile) > 0){
			CScriptEditFrame *p = new CScriptEditFrame(g_app.GetMainFrame()->GetHandle(),
				g_app.GetScriptsFolder() + sFile);

			if(p){
				p->Create(g_app.GetMainFrame()->GetHandle());
			}
		}
	}
	return 0;
}

// new 
LRESULT CScriptView::OnBtNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CScriptEditFrame *p = new CScriptEditFrame(g_app.GetMainFrame()->GetHandle());

	if(p){
		p->Create(g_app.GetMainFrame()->GetHandle());
	}
	return 0;
}

// stop all
LRESULT CScriptView::OnBtStopAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int count = m_lb2.GetCount();
	BOOL b;

	while(count > 0){
		m_lb2.SetCurSel(0);
		OnList2DClick(0,0,0,b);
		count = m_lb2.GetCount();
	}

	return 0;
}

LRESULT CScriptView::OnBtStop(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL b;
	OnList2DClick(0,0,0,b);
	return 0;
}

LRESULT CScriptView::OnBtReload(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ReloadScriptList();
	return 0;
}


void CScriptView::ReloadScriptList(void)
{
	WIN32_FIND_DATA f = {0};
	HANDLE hFind = NULL;
	CString sSearch;
	LPCTSTR exts[] = { "*.vbs","*.js" };
	const int count = sizeof(exts)/sizeof(exts[0]);

	m_lb1.ResetContent();
	for(int i=0;i<count;i++){
		sSearch = g_app.GetScriptsFolder() + exts[i];
		hFind = ::FindFirstFile(sSearch,&f);
		if(hFind != INVALID_HANDLE_VALUE){
			if((f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0){
				m_lb1.AddString(f.cFileName);
				while(FindNextFile(hFind,&f)){
					if((f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0){
						m_lb1.AddString(f.cFileName);
					}
				}
			}
			FindClose(hFind);
		}
	}
}

BOOL CScriptView::ReadScript(LPCTSTR file,CString &buf)
{
	CString path = g_app.GetScriptsFolder() + file;
	NO5TL::CWinFile wf;

	BOOL res = wf.Create(path,OPEN_EXISTING,GENERIC_READ);

	if(res){
		NO5TL::CDataBuffer db;

		res = wf.ReadAll(db);
		if(res){
			db.AddNull();
			buf = (LPCTSTR)db.GetData();
		}
	}
	return res;
}

void CScriptView::ReleaseRunning(void)
{
	// release all objects in the running objects list
	const int count = m_lb2.GetCount();
	int i;
	for(i=0;i<count;i++){
		CComObject<CSimpleScriptSite> *pObj = (CComObject<CSimpleScriptSite> *)m_lb2.GetItemDataPtr(i);

		if(pObj){
			HRESULT hr = pObj->Terminate();
			pObj->Release();
			m_lb2.SetItemData(i,0);	
		}
	}
}

