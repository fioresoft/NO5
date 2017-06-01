// CamUpDlg.cpp: implementation of the CCamUpDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "CamUpDlg.h"
#include "no5app.h"
#include "imainframe.h"
#include "usrmsgs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCamUpDlg::CCamUpDlg()
{
	m_app = 2;
	m_country = "us";
	m_server = "webcam.yahoo.com";
}

CCamUpDlg::~CCamUpDlg()
{

}

LRESULT CCamUpDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	/*
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);
	*/

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	//pLoop->AddIdleHandler(this);
	//UIAddChildWindowContainer(m_hWnd);

	DlgResize_Init(false,false,WS_CLIPCHILDREN);

	HRESULT hr = AtlAdviseSinkMap(this,true);
	ATLASSERT(SUCCEEDED(hr));

	if(SUCCEEDED(hr)){
		m_ax.Attach(GetDlgItem(IDC_UPLDLG1));
		hr = m_ax.QueryControl(&m_sp);
		ATLASSERT(SUCCEEDED(hr));
		// or use that
		//GetDlgControl(IDC_EXPLORER1,IID_IWebBrowser2,(void **)&m_sp);
		if(SUCCEEDED(hr)){
			hr = Connect();
			ATLASSERT(SUCCEEDED(hr));
		}
	}

	return TRUE;
}

LRESULT CCamUpDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	HRESULT hr = AtlAdviseSinkMap(this,false);
	ATLASSERT(SUCCEEDED(hr));
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	return 0;
}

LRESULT CCamUpDlg::OnNCDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CWindow wnd = g_app.GetMainFrame()->GetHandle();

	ATLASSERT(wnd.IsWindow());
	wnd.PostMessage(NO5WM_CAMDLGDEL,0,(LPARAM)this);
	bHandled = FALSE;
	return 0;
}

LRESULT CCamUpDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	DestroyWindow();
	return 0;
}

HRESULT CCamUpDlg::Connect(void)
{
	HRESULT hr;
	USES_CONVERSION;

	hr = m_sp->put_AppName(m_app);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->put_UserName(T2OLE(m_user));
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->put_Token(T2OLE(m_token));
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->put_CountryCode(T2OLE(m_country));
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->put_Server(T2OLE(m_server));
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->Start();
	ATLASSERT(SUCCEEDED(hr));

	return hr;
}

HRESULT __stdcall CCamUpDlg::OnTokenExpired()
{
	MessageBox("token expired");
	return S_OK;
}

HRESULT __stdcall CCamUpDlg::OnResize(LONG iNewWidth, LONG iNewHeight)
{
	CRect rcDlg;
	CRect rcCtl;
	CSize sz;

	GetWindowRect(&rcDlg);
	m_ax.GetWindowRect(&rcCtl);
	ScreenToClient(&rcCtl);
	sz.cx = iNewWidth - rcCtl.Width();
	sz.cy = iNewHeight - rcCtl.Height();

	rcDlg.right = rcDlg.right + sz.cx;
	rcDlg.bottom = rcDlg.bottom + sz.cx;
	SetWindowPos(NULL,0,0,rcDlg.Width(),rcDlg.Height(),SWP_NOMOVE|SWP_NOOWNERZORDER|\
		SWP_NOZORDER);
	return S_OK;
}

HRESULT __stdcall CCamUpDlg::OnInitFailed(LONG iErrCode, BSTR bstrError)
{
	MessageBox("init failed");
	return S_OK;
}

HRESULT __stdcall CCamUpDlg::OnUpDlgClose()
{
	MessageBox("on close");
	return S_OK;
}
