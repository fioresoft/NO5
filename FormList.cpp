// FormList.cpp: implementation of the CFormList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "usrmsgs.h"
#include "FormList.h"
#include "formframe.h"
#include "no5app.h"
#include "no5_3.h"
#include "imainframe.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFormList::CFormList()
{

}

CFormList::~CFormList()
{

}

LRESULT CFormList::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);

	m_lb1.Attach(GetDlgItem(IDC_LIST1));
	DlgResize_Init(false,false,WS_CLIPCHILDREN);
	ReloadList();
	return TRUE;
}

LRESULT CFormList::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	return 0;
}

LRESULT CFormList::OnDeleteViewer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPreviewDlg *p = (CPreviewDlg *)lParam;
	delete p;
	return 0;
}

LRESULT CFormList::OnList1DClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString sFile;
	if(GetCurSel(sFile)){
		// create the preview dialog
		CPreviewDlg *pDlg = new CPreviewDlg(m_hWnd,TRUE);
		USES_CONVERSION;
		pDlg->m_AutoSizing=TRUE;
		pDlg->m_FileName= g_app.GetFormsFolder();
		pDlg->m_FileName += sFile;

		// expose objects - see OnInitialUpdate() for more details
		// class1
		CExposedObjectInfo ExposedObjectInfo;
		CComQIPtr<IDispatch> sp = (INo5Obj *)(g_app.GetNo5Obj());
		ExposedObjectInfo.m_Name=_T("no5");
		ExposedObjectInfo.m_spDispatch= sp;
		pDlg->m_ExposedObjectInfoArray.Add(ExposedObjectInfo);

		// show the preview
		pDlg->Create(m_hWnd);
		pDlg->ShowWindow(SW_NORMAL);
	}
	return 0;
}


LRESULT CFormList::OnBtRun(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL b;
	OnList1DClick(0,0,0,b);
	return 0;
}

// edit ( only not running scripts )
LRESULT CFormList::OnBtEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString sFile;
	if(GetCurSel(sFile)){
		CRect r(0,0,CW_USEDEFAULT,CW_USEDEFAULT);
		CString path = g_app.GetFormsFolder();
		path += sFile;
		CFormFrame *pFrame = new CFormFrame(path);
		pFrame->CreateEx(g_app.GetMainFrame()->GetHandle(),r,NULL);
		//
	}
	return 0;
}

// new 
LRESULT CFormList::OnBtNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CRect r(0,0,CW_USEDEFAULT,CW_USEDEFAULT);
	CFormFrame *pFrame = new CFormFrame();
	pFrame->CreateEx(g_app.GetMainFrame()->GetHandle(),r,NULL);
	return 0;
}



LRESULT CFormList::OnBtReload(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ReloadList();
	return 0;
}


void CFormList::ReloadList(void)
{
	WIN32_FIND_DATA f = {0};
	HANDLE hFind = NULL;
	CString sSearch;
	LPCTSTR exts[] = { "*.ddf" };
	const int count = sizeof(exts)/sizeof(exts[0]);

	m_lb1.ResetContent();
	for(int i=0;i<count;i++){
		sSearch = g_app.GetFormsFolder() + exts[i];
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

bool CFormList::GetCurSel(CString &s)
{
	int idx = m_lb1.GetCurSel();
	bool res = false;
	if(idx >= 0)
		res = (m_lb1.GetText(idx,s) > 0);
	return res;
}