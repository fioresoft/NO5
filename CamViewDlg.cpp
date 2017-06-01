// CamViewDlg.cpp: implementation of the CCamViewDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "CamViewDlg.h"
#include "usrmsgs.h"
#include <ctime>
#include <no5tl\mystring.h>
#include <no5tl\dib.h>
#include "no5app.h"
#include "imainframe.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCamViewDlg::CCamViewDlg()
{
	m_app = 2;
	m_country = "us";
	m_isp = 0;
	m_age = 25;
	m_server = "webcam.yahoo.com";
	m_room = "";
	m_recording = false;
	m_pavi = NULL;
}

CCamViewDlg::~CCamViewDlg()
{
	ATLTRACE("~CCamViewDlg\n");
	if(m_pavi)
		delete m_pavi;
}

LRESULT CCamViewDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
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

	m_edit.Attach(GetDlgItem(IDC_EDIT1));
	m_edit.SetWindowText(m_target);
	m_st.Attach(GetDlgItem(IDC_STATIC1));

	ModifyStyleEx(0,WS_EX_TOPMOST,SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	DlgResize_Init(false,false,WS_CLIPCHILDREN);

	HRESULT hr = AtlAdviseSinkMap(this,true);
	ATLASSERT(SUCCEEDED(hr));

	if(SUCCEEDED(hr)){
		
		m_ax.Attach(GetDlgItem(IDC_WCVIEWER1));
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

LRESULT CCamViewDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	HRESULT hr = AtlAdviseSinkMap(this,false);
	ATLASSERT(SUCCEEDED(hr));
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	return 0;
}

LRESULT CCamViewDlg::OnNCDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CWindow wnd = g_app.GetMainFrame()->GetHandle();

	ATLASSERT(wnd.IsWindow());
	wnd.PostMessage(NO5WM_CAMDLGDEL,0,(LPARAM)this);
	bHandled = FALSE;
	return 0;
}

LRESULT CCamViewDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if(m_sp){
		m_sp->Abort();
	}
	DestroyWindow();
	return 0;
}

LRESULT CCamViewDlg::OnButtonView(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	HRESULT hr = m_sp->IsReceiving();
	CButton bt = GetDlgItem(IDC_BUTTON1);
	
	if(hr == S_OK){
		hr = m_sp->Abort();
		bt.SetWindowText("connect");
	}
	else{
		bt.SetWindowText("disconnect");
		hr = Connect();
	}
	ATLASSERT(SUCCEEDED(hr));
	return 0;
}

LRESULT CCamViewDlg::OnSnapshot(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Snapshot();
	return 0;
}

LRESULT CCamViewDlg::OnRecord(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(!m_recording){
		StartRec();
	}
	else{
		StopRec();
	}
	return 0;
}

HRESULT __stdcall CCamViewDlg::OnConnectionStatusChanged(INT iStatusCode)
{
	CString s;

	switch(iStatusCode){
		case 1: // disconnected ?
			s = "disconnected";
			break;
		case 2:	// connecting
			s = "connecting";
			break;
		case 3: // aborting ?
			s = "disconnecting";
			break;
		case 4:	// requesting access ?
			s = "requesting access";
			break;
		case 6:	// receiving
			s = "receiving images";
			break;
		case 8:	// paused
			s = "paused";
			break;
		default:
			s.Format("status %d",iStatusCode);
			ATLTRACE("cam status: %d\n",iStatusCode);
			break;
	}
	
	m_st.SetWindowText(s);

	HRESULT hr = m_sp->IsReceiving();
	if(hr == S_OK){
		CWindow bt = GetDlgItem(IDC_BUTTON1);
		bt.SetWindowText("disconnect");
	}
	else{
		CWindow bt = GetDlgItem(IDC_BUTTON1);
		bt.SetWindowText("connect");
	}

	return S_OK;
}

HRESULT __stdcall CCamViewDlg::OnFinishedReceiving(INT iErrCode)
{
	CString s;

	switch(iErrCode){
		case 1:		// disconnected
		case 2:		// disconnected after calling abort ?
			s = "disconnected";
			break;
		case 6:		// user doesnt exit or dont have a cam ?
			s = "not connected";
			break;
		case 11:
			s = "invalid token";
			break;
		case 15:	// disconnected by the target ?
			s = "refused";
			break;
		default:
			s.Format("finished: %d",iErrCode);
			ATLTRACE("cam finished: %d\n",iErrCode);
			break;
	}
	HRESULT hr = m_sp->IsReceiving();
	if(hr == S_OK){
		CWindow bt = GetDlgItem(IDC_BUTTON1);
		bt.SetWindowText("disconnect");
	}
	else{
		CWindow bt = GetDlgItem(IDC_BUTTON1);
		bt.SetWindowText("connect");
		if(m_recording)
			StopRec();
	}

	m_st.SetWindowText(s);
	return S_OK;
}

HRESULT __stdcall CCamViewDlg::OnReceivedImage(INT iImageLen, LONG tTimeStamp)
{
	struct tm *ptm = localtime((time_t *)&tTimeStamp);
	CString s = asctime(ptm);

	m_st.SetWindowText(s);
	if(m_recording){
		NO5TL::CDib dib;
		BOOL res;
		CRect rc;
		
		m_ax.GetClientRect(&rc);
		res = dib.FromClientRect(m_ax,rc);
		if(res){
			HRESULT hr = S_OK;
			if(!m_pavi->GetPos()){	// setup
				hr = m_pavi->ChooseCompressorDlg(m_hWnd,dib.GetBitmapInfo());
				if(SUCCEEDED(hr)){
					hr = m_pavi->CreateStream(dib.Width(),dib.Height());
					if(SUCCEEDED(hr)){
						hr = m_pavi->MakeCompressedStream();
						if(SUCCEEDED(hr)){
							hr = m_pavi->StreamSetFormat(m_pavi->m_pcs,
								dib.GetBitmapInfoHeader());
						}
					}
				}
			}
			if(SUCCEEDED(hr)){
				if(SUCCEEDED(hr)){
					hr = m_pavi->Write(dib.GetImageData(),dib.GetImageLen());
				}
			}
			ATLASSERT(SUCCEEDED(hr));
		}
		ATLASSERT(res);
	}
	return S_OK;
}

HRESULT __stdcall CCamViewDlg::OnResize(LONG iNewWidth, LONG iNewHeight)
{
	CString s;
	CRect rcCtl;
	CRect rcDlg;
	CSize sz;
	bool rec = false;

	if(m_recording){
		StopRec();
		rec = true;
	}

	GetWindowRect(&rcDlg);
	m_ax.GetWindowRect(&rcCtl);
	ScreenToClient(&rcCtl);
	sz.cx = iNewWidth - rcCtl.Width();
	sz.cy = iNewHeight - rcCtl.Height();

	rcDlg.right = rcDlg.right + sz.cx;
	rcDlg.bottom = rcDlg.bottom + sz.cx;
	SetWindowPos(NULL,0,0,rcDlg.Width(),rcDlg.Height(),SWP_NOMOVE|SWP_NOOWNERZORDER|\
		SWP_NOZORDER);
	if(rec)
		StartRec();
	return S_OK;
}

HRESULT __stdcall CCamViewDlg::OnRButtonDown()
{
	return S_OK;
}

HRESULT __stdcall CCamViewDlg::OnSuperModeChanged(LONG bSuper)
{
	CString s;

	s.Format("super: %ld",bSuper);
	m_st.SetWindowText(s);
	return S_OK;
}

HRESULT __stdcall CCamViewDlg::OnHideSuper(LONG bHideSuper)
{
	CString s;

	s.Format("hide super: %ld",bHideSuper);
	m_st.SetWindowText(s);
	return S_OK;
}

HRESULT CCamViewDlg::Connect(void)
{
	HRESULT hr;
	USES_CONVERSION;
	NO5TL::CWindowTextString target(m_edit);

	hr = m_sp->put_AppName(m_app);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->put_UserName(T2OLE(m_user));
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->put_Age(m_age);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->put_TargetName(T2OLE(target));
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->put_RoomName(T2OLE(m_room));
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->put_Token(T2OLE(m_token));
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->put_CountryCode(T2OLE(m_country));
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->put_ISP(m_isp);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->put_Server(T2OLE(m_server));
	ATLASSERT(SUCCEEDED(hr));
	hr = m_sp->Receive();
	ATLASSERT(SUCCEEDED(hr));

	return hr;
}

CString CCamViewDlg::TmpShotName(void)
{
	static char buf[100] = {0};
	time_t t = time(0);
	struct tm *ptm = localtime(&t);
	CString s;

	ZeroMemory(buf,sizeof(buf));
	strftime(buf,sizeof(buf)/sizeof(buf[0]) - sizeof(buf[0]),"%m_%d_%y_%H_%M_%S",ptm);
	s.Format("%s_%s",(LPCTSTR)m_target,buf);
	return s;
}

CString CCamViewDlg::TmpRecName(void)
{
	static char buf[100] = {0};
	time_t t = time(0);
	struct tm *ptm = localtime(&t);
	CString s;

	ZeroMemory(buf,sizeof(buf));
	strftime(buf,sizeof(buf)/sizeof(buf[0]) - sizeof(buf[0]),"%m_%d_%y_%H_%M_%S",ptm);
	s.Format("%s_%s",(LPCTSTR)m_target,buf);
	return s;
}

void CCamViewDlg::Snapshot(void)
{
	BOOL res = FALSE;
	if(m_sp){
		NO5TL::CDib dib;
		CRect rc;
		long lw=0,lh=0;
		
		m_sp->get_Width(&lw);
		m_sp->get_Height(&lh);
		if(lw && lh){
			m_ax.GetClientRect(&rc);
			res = dib.FromClientRect(m_ax,rc);
			if(res){
				CString file;
				file.Format("%s%s.bmp",(LPCTSTR)g_app.GetSnapshotFolder(),
					(LPCTSTR)TmpShotName());
				res = dib.SaveToFile(file);
			}
		}
	}
	ATLASSERT(res);
}

void CCamViewDlg::StartRec(void)
{
	BOOL res = FALSE;

	if(m_sp && !m_recording && !m_pavi){
		CString file;
		float fps = 0;
		
		//m_sp->get_FrameRate(&fps);
		m_pavi = new NO5TL::CAvi((long)1);
		file.Format("%s%s.avi",(LPCTSTR)g_app.GetVideoFolder(),(LPCTSTR)TmpRecName());

		HRESULT hr = m_pavi->FileOpen(file);
		if(SUCCEEDED(hr)){
			CWindow wnd = GetDlgItem(IDC_RECORD);
			m_recording = true;
			wnd.SetWindowText("Stop");
			res = TRUE;
		}
	}
	ATLASSERT(res);
}

void CCamViewDlg::StopRec(void)
{	
	if(m_recording){
		CWindow wnd = GetDlgItem(IDC_RECORD);

		m_recording = false;
		m_pavi->Destroy();
		wnd.SetWindowText("Record");
		delete m_pavi;
		m_pavi = NULL;
	}
}