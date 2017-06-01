// SmilePicker.cpp: implementation of the CSmilePicker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "SmilePicker.h"
#include <no5tl\winfile.h>
#include "no5app.h"
#include "usrmsgs.h"

CSmilePicker::CSmilePicker(HWND hWndNotify):m_notify(hWndNotify)
{

}

CSmilePicker::~CSmilePicker()
{
	ATLTRACE("~CSmilePicker\n");
}

LRESULT CSmilePicker::OnCreate(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	LRESULT lRes = DefWindowProc();

	HRESULT hr = GetDocument();
	if(SUCCEEDED(hr)){
		hr = GetInterfaces();
		if(SUCCEEDED(hr)){
			CComPtr<IAxWinAmbientDispatch> sp;
			hr = QueryHost(&sp);
			if(SUCCEEDED(hr))
				sp->put_AllowContextMenu(VARIANT_FALSE);
			hr = MyDisp::DispEventAdvise(m_doc.m_sp);
		}
	}
	if(SUCCEEDED(hr)){
		CenterWindow();
		try{
			int loaded = LoadSmiles();
			CString s;
			s.Format("simile picker ( %d loaded )",loaded);
			SetWindowText(s);
		}catch(...){
			CString s;
			s.Format("Error %x",GetLastError());
			::MessageBox(GetDesktopWindow(),s,"debug",MB_ICONERROR);
		}
	}
	else{
		CString s;
		::MessageBox(GetDesktopWindow(),"Error loading smiley picker, try again",
			g_app.GetName(),MB_ICONERROR);
	}
	return lRes;
}

LRESULT CSmilePicker::OnDestroy(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	if(m_doc.m_sp != NULL){
		MyDisp::DispEventUnadvise(m_doc.m_sp);
	}
	bHandled = FALSE;
	return 0;
}


int CSmilePicker::LoadSmiles(void)
{
	BOOL res = FALSE;
	CSimpleArray<CString> lines;
	int added = 0;
	

	{
		NO5TL::CWinFile wf;
		
		res = wf.Create(g_app.GetSmileysFile(),OPEN_EXISTING,GENERIC_READ);
		if(res){	
			res = wf.ReadLines(lines);
		}
	}
	if(res){
		const int count = lines.GetSize();
		int i;
		LPCTSTR p,q;
		CString code,file;
		CString old;
		CString table;

		table = "<table border=1><tr>";
		for(i=0;i<count;i++){
			p = lines[i];
			try{
				old = file;
				// remove possible spaces
				while(isspace(*p))
					p++;
				if(!*p)
					continue;
				q = p;
				// code
				while(!isspace(*q))
					q++;
				if(!*q)
					continue;
				if(q > p)
					NO5TL::StringCopyN(code,p,0,(q - p));
				else
					continue;
				while(isspace(*q))
					q++;
				if(!*q)
					break;
				p = q;

				while(!isspace(*q))
					q++;
				if(q > p)
					NO5TL::StringCopyN(file,p,0,(q - p));
				else
					continue;

				//file.Replace("gif","bmp");
				code.Replace("\"","&#34;");
				code.Replace("<","&lt");
				code.Replace(">","&gt");

				if(old.CompareNoCase(file) != 0){
					CString tag;
					CString src;

					src = g_app.GetSmileysFolder();
					src += file;
					tag.Format("<td align=center valign=center><img src=\"%s\" title=\"%s\"></td>",(LPCTSTR)src,(LPCTSTR)code);
					
					try{
						table += tag;
					}catch(...){
						
					}
					tag.Empty();
					added++;

					if(added % 10 == 0){
						table += "</tr>";
						table += "<tr>";
					}
							
				}
			}catch(...){
				::MessageBox(GetDesktopWindow(),"Error load smileys",g_app.GetName(),MB_ICONERROR);
			}	
		}
		
		if(added % 10 != 0)
			table += "</tr>";
		table += "</table>";
		AppendHtmlText(table);
	}
	else{
		CString s;
		s.Format("failed to read form %s",(LPCTSTR)(g_app.GetSmileysFile()));
		MessageBox(s,g_app.GetName(),MB_ICONERROR);
	}
	return added;
}

VARIANT_BOOL CSmilePicker::OnClick(void)
{
	CComPtr<IHTMLEventObj> sp;
	HRESULT hr = m_doc.GetEventObj(&sp);

	if(SUCCEEDED(hr) && sp){
		NO5TL::CHtmlElement src;

		hr = sp->get_srcElement(&src.m_sp);
		if(SUCCEEDED(hr) && src.m_sp){
			hr = src.GetAttribute("title",m_res);
			if(hr == S_OK && (!m_res.IsEmpty()) ){
				if(m_notify.IsWindow())
					m_notify.SendMessage(NO5WM_SMILESWND,0,(LPARAM)this);
				PostMessage(WM_CLOSE);
			}
		}
	}
	return VARIANT_TRUE;
}

// CSmilePicker2

CSmilePicker2::CSmilePicker2(HWND hWndNotify):m_notify(hWndNotify)
{

}

CSmilePicker2::~CSmilePicker2()
{
	ATLTRACE("~CSmilePicker2\n");
}

LRESULT CSmilePicker2::OnCreate(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	CenterWindow();
	SetWindowText("Smiles");

	m_tb.Create(m_hWnd,rcDefault,NULL,WS_CHILD|TBSTYLE_WRAPABLE|CCS_NODIVIDER|CCS_NOMOVEX|\
		CCS_NOMOVEY|CCS_NOPARENTALIGN|CCS_NORESIZE|TBSTYLE_FLAT);
	if(m_il.Create(18,18,ILC_COLORDDB,100,1)){
		LoadSmiles();
		m_tb.ShowWindow(SW_SHOWNORMAL);
	}
	return 0;
}

LRESULT CSmilePicker2::OnDestroy(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	const int count = m_tb.GetButtonCount();
	TBBUTTON b;
	for(int i=0;i<count;i++){
		ZeroMemory(&b,sizeof(b));
		m_tb.GetButton(i,&b);
		delete (Data *)b.dwData;
	}
	return 0;
}

LRESULT CSmilePicker2::OnSize(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	if(m_tb.IsWindow()){
		m_tb.MoveWindow(0,0,LOWORD(lParam),HIWORD(lParam));
	}
	return 0;
}


void CSmilePicker2::LoadSmiles(void)
{
	BOOL res = FALSE;
	CSimpleArray<CString> lines;
	TBBUTTON *pButtons = NULL;
	int added = 0;
	

	{
		NO5TL::CWinFile wf;
		
		res = wf.Create(g_app.GetSmileysFile(),OPEN_EXISTING,GENERIC_READ);
		if(res){	
			res = wf.ReadLines(lines);
		}
	}
	if(res){
		const int count = lines.GetSize();
		int i;
		LPCTSTR p,q;
		CString code,file;
		CString old;

		pButtons = new TBBUTTON[count];
		if(!pButtons)
			return;

		for(i=0;i<count;i++){
			p = lines[i];
			if(*p != '#'){
				old = file;
				// remove possible spaces
				while(isspace(*p))
					p++;
				if(!*p)
					continue;
				q = p;
				// code
				while(!isspace(*q))
					q++;
				if(!*q)
					continue;
				NO5TL::StringCopyN(code,p,0,(q - p));
				while(isspace(*q))
					q++;
				if(!*q)
					break;
				p = q;
				while(!isspace(*q))
					q++;
				NO5TL::StringCopyN(file,p,0,(q - p));

				file.Replace("gif","bmp");
				code.Replace("\"","\\");

				if(old.CompareNoCase(file) != 0){
					CString src;

					src = g_app.GetSmileysFolder();
					src += file;

					HBITMAP hBmp = (HBITMAP)LoadImage(NULL,src,IMAGE_BITMAP,18,18,
						LR_LOADFROMFILE|LR_SHARED|LR_LOADTRANSPARENT);

					if(hBmp){
						int idx = m_il.Add(hBmp);
						if(idx >= 0){
							Data *p = new Data();
							p->code = code;
							pButtons[added].iBitmap = idx;
							pButtons[added].idCommand = IDC_BUTTON1;
							pButtons[added].fsState = TBSTATE_ENABLED;						
							pButtons[added].fsStyle = TBSTYLE_BUTTON;
							pButtons[added].dwData = (DWORD)p;
							pButtons[added].iString = -1;
							added++;
						}
						DeleteObject(hBmp);
					}
					else
						MessageBox(NO5TL::GetErrorDesc(GetLastError()));
				}
			}
		}
	}
	if(res && added){
		m_tb.SetImageList(m_il);
		m_tb.SetButtonStructSize();
		m_tb.AddButtons(added,pButtons);
		m_tb.AutoSize();
	}
	delete []pButtons;
}

LRESULT CSmilePicker2::OnClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CPoint pt;
	GetCursorPos(&pt);
	m_tb.ScreenToClient(&pt);
	int i = m_tb.HitTest(&pt);
	if(i >= 0){
		TBBUTTON b = {0};
		m_tb.GetButton(i,&b);
		Data *p = (Data *)b.dwData;
		m_res = p->code;
		if(m_notify.IsWindow())
			m_notify.SendMessage(NO5WM_SMILESWND,0,(LPARAM)this);
		PostMessage(WM_CLOSE);
	}
	return 0;
}



