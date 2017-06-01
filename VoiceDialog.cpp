// VoiceDialog.cpp: implementation of the CVoiceDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "VoiceDialog.h"
#include "imainframe.h"
#include "no5app.h"
#include "usrmsgs.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVoiceDialog::CVoiceDialog():m_btTalk(this,1)
{
	m_bConnected = false;
}

CVoiceDialog::~CVoiceDialog()
{
	m_ImgList.Destroy();
}

LRESULT CVoiceDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	USES_CONVERSION;
	// center the dialog on the screen
	CenterWindow();

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	//pLoop->AddIdleHandler(this);
	//UIAddChildWindowContainer(m_hWnd);

	m_lv.SubclassWindow(GetDlgItem(IDC_LIST1));
	m_status = GetDlgItem(IDC_STATUS);
	m_pbIn = GetDlgItem(IDC_PROGRESS_INPUT);
	m_pbOut = GetDlgItem(IDC_PROGRESS_OUTPUT);
	m_btConn = GetDlgItem(IDC_BUTTON_CONNECT);
	m_btTalk.SubclassWindow(GetDlgItem(IDC_BUTTON_TALK));

	CreateTheImgList();

	// setup list view
	if(!m_ImgList.IsNull()){
		m_lv.SetImageList(m_ImgList,LVSIL_SMALL);
	}

	LPCTSTR cols[] = { _T("users"),_T("muted")};
	int width[] = {0,0};
	const int count = sizeof(cols)/sizeof(cols[0]);
	int i;
	CRect rc;
	m_lv.GetClientRect(&rc);
	width[1] = m_lv.GetStringWidth("  muted  ");
	width[0] = rc.Width() - width[1];

	for(i=0;i<count;i++){
		m_lv.InsertColumn(i,cols[i],LVCFMT_LEFT,width[i],i);
	}
	// setup progress bars
	m_pbIn.SetRange(0,100);
	m_pbOut.SetRange(0,100);
	m_pbIn.SetStep(1);
	m_pbOut.SetStep(1);
	m_pbIn.SetPos(0);
	m_pbOut.SetPos(0);
	// setup button
	m_brush.CreateSolidBrush(0x00FF00);

	DlgResize_Init(true,false,WS_THICKFRAME | WS_CLIPCHILDREN);

	CComBSTR clsid(CLSID_YAcs);
	LPCTSTR pclsid = OLE2CT(clsid);
	m_ax.Create(m_hWnd,rcDefault,pclsid,WS_CHILD);
	HRESULT hr = m_ax.QueryControl(&m_sp);

	if(SUCCEEDED(hr)){
		hr = YacsSource::DispEventAdvise(m_sp);
		ATLASSERT(SUCCEEDED(hr));
		// or use that
		//GetDlgControl(IDC_EXPLORER1,IID_IWebBrowser2,(void **)&m_sp);
		if(SUCCEEDED(hr)){
			//hr = m_sp->leaveConference();
			CComBSTR bs;
			CString s;

			bs = (LPCTSTR)m_name;
			hr = m_sp->put_userName(bs);
			ATLASSERT(SUCCEEDED(hr));

			bs = (LPCTSTR)m_server;
		//	hr = m_sp->put_hostName(bs);
		//	ATLASSERT(SUCCEEDED(hr));
			s.Empty();
			s.Format("ch/%s::%s",(LPCTSTR)m_room,(LPCTSTR)m_rmid);
			bs = (LPCTSTR)s;
			hr = m_sp->put_confName(bs);
			s.Empty();
			s.Format("mc(7,0,0,437)&u=%s&ia=us",(LPCTSTR)m_name);
			bs = (LPCTSTR)s;
			hr = m_sp->put_appInfo(bs);
			ATLASSERT(SUCCEEDED(hr));
			
			bs = (LPCTSTR)m_cookie;
			hr = m_sp->put_confKey(bs);
			ATLASSERT(SUCCEEDED(hr));
			//bs = (LPCTSTR)m_name;
		//	hr = m_sp->loadSound(bs);
		//	ATLASSERT(SUCCEEDED(hr));
			
			ATLASSERT(SUCCEEDED(hr));
			//m_sp->put_inputGain(100);
			//m_sp->put_outputGain(100);
			//m_sp->put_inputAGC(100);
			//m_sp->put_inputSource(100);
			Connect();
		}
		
	}
	return TRUE;
}

LRESULT CVoiceDialog::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	HRESULT hr = YacsSource::DispEventUnadvise(m_sp);
	ATLASSERT(SUCCEEDED(hr));
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	return 0;
}

LRESULT CVoiceDialog::OnNCDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CWindow wnd = g_app.GetMainFrame()->GetHandle();

	ATLASSERT(wnd.IsWindow());
	wnd.PostMessage(NO5WM_VOICEDLGDEL,0,(LPARAM)this);
	bHandled = FALSE;
	return 0;
}

LRESULT CVoiceDialog::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if(m_bConnected)
		Disconnect();
	DestroyWindow();
	return 0;
}

LRESULT CVoiceDialog::OnButtonConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SwitchConnect();
	return 0;
}

LRESULT CVoiceDialog::OnCtlColorBtn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hWnd = HWND(lParam);
	LRESULT res = 0;

	if(hWnd == m_btTalk.m_hWnd){
		//CDCHandle dc((HDC)wParam);
		//dc.SetBkColor(0x00ff00);
		//dc.SelectBrush(m_brush);
		res = LRESULT(HBRUSH(m_brush));
	}
	else{
		bHandled = FALSE;
	}
	return res;
}

LRESULT CVoiceDialog::OnTalkDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	HRESULT hr = m_sp->startTransmit();
	ATLASSERT(SUCCEEDED(hr));
	SetCapture();
	bHandled = FALSE;
	return 0;
}

LRESULT CVoiceDialog::OnTalkUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ReleaseCapture();
	bHandled = FALSE;
	return 0;
}

LRESULT CVoiceDialog::OnTalkCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	HRESULT hr = m_sp->stopTransmit();
	ATLASSERT(SUCCEEDED(hr));
	return 0;
}

LRESULT CVoiceDialog::OnButtonFree(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CButton bt = GetDlgItem(IDC_CHECK_FREE);
	HRESULT hr = S_OK;
	UINT uDebug;

	switch(uDebug = bt.GetCheck()){
		case BST_CHECKED:
			hr = m_sp->startTransmit();
			break;
		case BST_UNCHECKED:
			hr = m_sp->stopTransmit();
			break;
		default:
			break;
	}
	ATLASSERT(SUCCEEDED(hr));
	return 0;
}

LRESULT CVoiceDialog::OnLVItemChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
   LPNMLISTVIEW pnmv = (LPNMLISTVIEW)pnmh;
   UINT uOld = pnmv->uOldState & LVIS_STATEIMAGEMASK;

   if (uOld != INDEXTOSTATEIMAGEMASK(0))
   {
	   UINT uNew = pnmv->uNewState & LVIS_STATEIMAGEMASK;

	   if(uOld != uNew){
		   CString s;

		   m_lv.GetItemText(pnmv->iItem,0,s);
		   if(!s.IsEmpty()){
			   HRESULT hr = S_OK;
			   CComBSTR bname = s;
			   LONG key = m_users.Lookup(s);

			   if(uNew == INDEXTOSTATEIMAGEMASK(2))
			   {
				   hr = m_sp->muteSource(key,bname);
			   }
			   else if (uNew == INDEXTOSTATEIMAGEMASK(1))
			   {
				   hr = m_sp->unmuteSource(key,bname);
			   }
			   ATLASSERT(SUCCEEDED(hr));
		   }
	   }
   }
   return 0;
}


HRESULT __stdcall CVoiceDialog::OnSourceEntry(LONG sourceId, BSTR sourceName)
{
	USES_CONVERSION;
	CString name = OLE2CT(sourceName);
	m_lv.InsertItem(0,name,0);
	m_users.Add(name,sourceId);
	return S_OK;
}

HRESULT __stdcall CVoiceDialog::OnSourceExit(LONG sourceId, BSTR sourceName)
{
	int idx = FindLVItem(sourceName);

	if(idx >= 0){
		USES_CONVERSION;
		CString s = OLE2CT(sourceName);
		m_lv.DeleteItem(idx);
		m_users.Remove(s);
	}
	return S_OK;
}

HRESULT __stdcall CVoiceDialog::OnRemoteSourceOnAir(LONG sourceId, BSTR sourceName)
{
	int idx = FindLVItem(sourceName);
	if(idx >= 0){
		m_lv.SetItem(idx,0,LVIF_IMAGE,NULL,1,0,0,0);
		m_lv.EnsureVisible(idx,FALSE);

		USES_CONVERSION;
		CWindow wnd = GetDlgItem(IDC_LABEL_TALKING);
		CString name = sourceName;
		CString s;
		s.Format("%s is talking",(LPCTSTR)name);
		wnd.SetWindowText(s);
	}
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnRemoteSourceOffAir(LONG sourceId, BSTR sourceName)
{
	int idx = FindLVItem(sourceName);
	if(idx >= 0){
		m_lv.SetItem(idx,0,LVIF_IMAGE,NULL,0,0,0,0);

		//USES_CONVERSION;
		CWindow wnd = GetDlgItem(IDC_LABEL_TALKING);
		//CString name = sourceName;
		//CString s;
		wnd.SetWindowText("");
	}
	return S_OK;
}

HRESULT __stdcall CVoiceDialog::OnLocalOnAir()
{
	CComBSTR name = m_name;
	int idx = FindLVItem(name);
	if(idx >= 0){
		m_lv.SetItem(idx,0,LVIF_IMAGE,NULL,1,0,0,0);

		CWindow wnd = GetDlgItem(IDC_LABEL_TALKING);
		//CString s;
		//s.Format("%s is talking");
		wnd.SetWindowText("you are talking");
	}
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnLocalOffAir()
{
	CComBSTR name = m_name;
	int idx = FindLVItem(name);
	if(idx >= 0){
		m_lv.SetItem(idx,0,LVIF_IMAGE,NULL,0,0,0,0);

		CWindow wnd = GetDlgItem(IDC_LABEL_TALKING);
		//CString s;
		//s.Format("%s is talking");
		wnd.SetWindowText("");
		wnd = GetDlgItem(IDC_LABEL_REPORT);
		wnd.SetWindowText("");
	}
	return S_OK;
}

HRESULT __stdcall CVoiceDialog::OnInputMuteChange(LONG mute)
{
#ifdef _DEBUG
	CString s;
	s.Format("OnInputMuteChange: %ld",mute);
	m_status.SetWindowText(s);
#endif
	
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnMonitorGainChange(SHORT gain)
{
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnInputSourceChange(SHORT source)
{
#ifdef _DEBUG
	CString s;
	s.Format("input source change: %d",(int)source);
	m_status.SetWindowText(s);
#endif
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnMonitorMuteChange(LONG mute)
{
#ifdef _DEBUG
	CString s;
	s.Format("monitor mute change: %ld",mute);
	m_status.SetWindowText(s);
#endif
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnSystemConnectFailure(LONG code, BSTR message)
{
	CString msg;
	USES_CONVERSION;
	msg.Format("Error: %d - %s",code,OLE2CT(message));
	m_status.SetWindowText(msg);
	Disconnect();
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnAudioError(LONG code, BSTR message)
{
	CString msg;
	USES_CONVERSION;
	msg.Format("Audio Error: %d - %s",code,OLE2CT(message));
	m_status.SetWindowText(msg);
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnSourceMuted(LONG numMuting, BSTR sourceName)
{
	int idx = FindLVItem(sourceName);
	if(idx >= 0){
		CString s;
		s.Format("%d",(int)numMuting);
		m_lv.SetItemText(idx,1,s);
	}
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnSourceUnmuted(LONG numMuting, BSTR sourceName)
{
	int idx = FindLVItem(sourceName);
	if(idx >= 0){
		CString s;
		s.Format("%d",(int)numMuting);
		m_lv.SetItemText(idx,1,s);
	}
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnTransmitReport(INT numReceiving, INT numTotal)
{
	CString s;
	CWindow wnd = GetDlgItem(IDC_LABEL_REPORT);
	s.Format("%d/%d",numReceiving,numTotal);
	wnd.SetWindowText(s);
	return S_OK;
}

HRESULT __stdcall CVoiceDialog::OnConferenceReady()
{
	m_status.SetWindowText("conference ready!");
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnConferenceNotReady()
{
	m_status.SetWindowText("conference not ready");
	return S_OK;
}

HRESULT __stdcall CVoiceDialog::OnInputLevelChange(SHORT level)
{
	m_pbIn.SetPos(100 + level);
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnOutputLevelChange(SHORT level)
{
	m_pbOut.SetPos(100 + level);
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnSystemConnect()
{
	m_status.SetWindowText("connected");
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnSystemDisconnect()
{
	m_status.SetWindowText("disconnected");
	m_bConnected = false;
	m_sp->leaveConference();
	m_lv.DeleteAllItems();
	m_btConn.SetWindowText("connect");
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnOutputGainChange(SHORT gain)
{
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnOutputMuteChange(LONG mute)
{
#ifdef _DEBUG
	CString s;
	s.Format("output mute change %ld",mute);
	m_status.SetWindowText(s);
#endif
	return S_OK;
}
HRESULT __stdcall CVoiceDialog::OnInputGainChange(SHORT gain)
{
	return S_OK;
}

void CVoiceDialog::CreateTheImgList(void)
{
	m_ImgList.Create(16,16,ILC_MASK | ILC_COLOR,3,0);
	ATLASSERT(!m_ImgList.IsNull());
	if(!m_ImgList.IsNull()){
		HICON hIcon;

		hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDR_MAINFRAME),IMAGE_ICON,
			16,16,LR_DEFAULTCOLOR);
		if(hIcon){
			m_ImgList.AddIcon(hIcon);
			DestroyIcon(hIcon);
			hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDI_TALKING),IMAGE_ICON,
				16,16,LR_DEFAULTCOLOR);
			if(hIcon){
				m_ImgList.AddIcon(hIcon);
				DestroyIcon(hIcon);

				hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDI_BUD_OFF),IMAGE_ICON,
				16,16,LR_DEFAULTCOLOR);
				if(hIcon){
					m_ImgList.AddIcon(hIcon);
					DestroyIcon(hIcon);
				}
			}
		}
		ATLASSERT(m_ImgList.GetImageCount() == 3);
	}
}
void CVoiceDialog::SwitchConnect(void)
{
	if(!m_bConnected){
		Connect();
	}
	else{
		Disconnect();
	}
}

void CVoiceDialog::Connect(void)
{
	HRESULT hr;
	
	hr = m_sp->createAndJoinConference();
	if(SUCCEEDED(hr)){
		m_btConn.SetWindowText("Disconnect");
		m_bConnected = true;
	}
	ATLASSERT(SUCCEEDED(hr));
}

void CVoiceDialog::Disconnect(void)
{
	HRESULT hr;

	hr = m_sp->leaveConference();
	if(SUCCEEDED(hr)){
		m_lv.DeleteAllItems();
		m_btConn.SetWindowText("Connect");
		m_bConnected = false;
	}
	ATLASSERT(SUCCEEDED(hr));
}

int CVoiceDialog::FindLVItem(BSTR name)
{
	USES_CONVERSION;
	LPCTSTR p = OLE2CT(name);
	LV_FINDINFO fi = {0};

	fi.flags = LVFI_STRING;
	fi.psz = p;

	return m_lv.FindItem(&fi,-1);
}
