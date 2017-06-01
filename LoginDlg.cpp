// LoginDlg.cpp: implementation of the CLoginDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <no5tl\no5tlbase.h>		// array find
#include <no5tl\mystring.h>
#include "resource.h"
#include "LoginDlg.h"
#include "no5options.h"
#include "no5app.h"
#include "roomdlg.h"
#include "recentrooms.h"

using NO5TL::CStringArray;
typedef CSimpleValArray<int> CIntArray;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoginDlg::CLoginDlg(void)
{
	m_iView = 0;
	m_iProto = 0;
	m_port = 0;
	m_po = g_app.GetGeneralOptions();
	m_pa = IAccountOptions::CreateMe();
}

CLoginDlg::~CLoginDlg()
{
	IAccountOptions::DestroyMe(&m_pa);
}


LRESULT CLoginDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow(GetParent());

	m_cbAccs =		GetDlgItem(IDC_COMBO_IDS);
	m_cbServers =	GetDlgItem(IDC_COMBO_SERVER);
	m_cbPorts =		GetDlgItem(IDC_COMBO_PORT);
	m_cbProtos =	GetDlgItem(IDC_COMBO_PROTO);
	m_cbViews =		GetDlgItem(IDC_COMBO_VIEW);
	m_cbRoom =		GetDlgItem(IDC_COMBO_ROOM);
	
	LoadAccounts();
	LoadProtoCombo();
	LoadViewCombo();
	LoadRoomCombo();
	// last account
	m_acc = m_po->GetLastAccountName();
	if(!m_acc.IsEmpty()){
		int idx = m_cbAccs.FindStringExact(-1,m_acc);
		if(idx >= 0){
			m_pa->SetFile(g_app.GetOptionsFile(),m_acc);

			m_cbAccs.SetCurSel(idx);
			// pw
			m_pw = m_pa->GetPassword();
			::SetWindowText(GetDlgItem(IDC_EDIT_PASSWORD),m_pw);
			// last room
			m_room = m_pa->GetLastRoom();
			m_cbRoom.SetWindowText(m_room);
		}
	}
	// last protocol
	CString tmp;
	tmp = m_po->GetLastProto();
	tmp.MakeLower();
	if(tmp == "ycht"){
		m_iProto = PROTO_YCHT;
	}
	else if(tmp == "ymsg"){
		m_iProto = PROTO_YMSG;
	}
	else
		m_iProto = PROTO_YMSG;

	// select it
	m_cbProtos.SetCurSel(m_cbProtos.FindString(-1,GetProtoName(m_iProto)));

	// load servers and ports
	OnProtoChange();
	// last view
	tmp = m_po->GetLastView();
	tmp.MakeLower();
	if(tmp == "html")
		m_iView = VIEW_HTML;
	else if(tmp == "rtf")
		m_iView = VIEW_RTF;
	else if(tmp == "text")
		m_iView = VIEW_TXT;
	else
		m_iView = VIEW_HTML;

	// select it
	m_cbViews.SetCurSel(m_cbViews.FindString(-1,GetViewName(m_iView)));

	return 0;
}

LRESULT CLoginDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return 0;
}
LRESULT CLoginDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL res;
	NO5TL::CWindowTextString acc(m_cbAccs);
	NO5TL::CWindowTextString server(m_cbServers);
	NO5TL::CWindowTextString port(m_cbPorts);
	NO5TL::CWindowTextString room(m_cbRoom);
	NO5TL::CWindowTextString pw(GetDlgItem(IDC_EDIT_PASSWORD));

	m_pw = pw;
	m_room = room;

	CStringArray accs;
	m_po->GetAccountNames(accs);
	// get the account name in the combo
	m_acc = acc;
	if(NO5TL::StringArray_FindNoCase(accs,m_acc) < 0){
		// new account
		accs.Add(m_acc);
		res = m_po->SetAccountNames(accs);
		ATLASSERT(res);
		m_pa->SetFile(g_app.GetOptionsFile(),m_acc);
	}
	// save pw
	m_pa->SetPassword(m_pw);
	// room
	m_pa->SetLastRoom(m_room);
	// proto
	res = m_po->SetLastProto(GetProtoName(m_iProto));
	// view
	res = m_po->SetLastView(GetViewName(m_iView));
	// port and server
	m_server = server;
	m_port = atoi(port);
	switch(m_iProto){
		case PROTO_YCHT:
			m_po->SetServerYcht(m_server);
			m_po->SetPortYcht(m_port);
			break;
		case PROTO_YMSG:
			m_po->SetServerYmsg(m_server);
			m_po->SetPortYmsg(m_port);
			break;
	}
	SaveServers();
	SavePorts();
	// last account
	m_po->SetLastAccountName(m_acc);

	EndDialog(IDOK);
	return 0;
}

LRESULT CLoginDlg::OnComboEditChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO : Add Code for control notification handler.
	return 0;
}
LRESULT CLoginDlg::OnComboEditUpdate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO : Add Code for control notification handler.
	return 0;
}
LRESULT CLoginDlg::OnAccountSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	OnAccountChange();
	return 0;
}
LRESULT CLoginDlg::OnPortsSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO : Add Code for control notification handler.
	return 0;
}
LRESULT CLoginDlg::OnServersSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO : Add Code for control notification handler.
	return 0;
}
LRESULT CLoginDlg::OnProtoSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int idx = m_cbProtos.GetCurSel();
	if(idx >= 0){
		m_iProto = (int)m_cbProtos.GetItemData(idx);
		OnProtoChange();
	}
	return 0;
}
LRESULT CLoginDlg::OnViewSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int idx = m_cbViews.GetCurSel();
	if(idx >= 0){
		m_iView = (int)m_cbViews.GetItemData(idx);
		//OnProtoChange();
	}
	return 0;
}

LRESULT CLoginDlg::OnAccountDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	NO5TL::CWindowTextString acc(m_cbAccs);
	BOOL res;

	res = m_po->DeleteAccount(acc);
	ATLASSERT(res);
	int idx = m_cbAccs.FindStringExact(-1,acc);
	if(idx >= 0){
		m_cbAccs.DeleteString(idx);
		OnAccountChange();
	}
	return 0;
}

LRESULT CLoginDlg::OnRoomSelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CRoomDlg dlg(0 /*modal */);

	dlg.m_room = m_room;
	dlg.m_server = m_po->GetRoomServer();
	if(IDOK == dlg.DoModal(m_hWnd)){
		m_room = dlg.m_room;
		m_po->SetRoomServer(dlg.m_server);
		m_cbRoom.SetWindowText(m_room);
	}
	return 0;
}

void CLoginDlg::LoadAccounts(void)
{
	CStringArray accs;

	if(m_po->GetAccountNames(accs) > 0){
		FillComboBox(IDC_COMBO_IDS,accs);
	}
}

void CLoginDlg::OnProtoChange(void)
{
	CStringArray servers;
	CIntArray ports;
	int idx;

	// get servers,ports,last server,last port
	switch(m_iProto){
		case PROTO_YCHT:
			m_po->GetServersYcht(servers);
			m_po->GetPortsYcht(ports);
			m_server = m_po->GetServerYcht();
			m_port = m_po->GetPortYcht();
			break;
		case PROTO_YMSG:
			m_po->GetServersYmsg(servers);
			m_po->GetPortsYmsg(ports);
			m_server = m_po->GetServerYmsg();
			m_port = m_po->GetPortYmsg();
			break;
	}
	// fill combox
	FillComboBox(IDC_COMBO_SERVER,servers);
	FillComboBox(IDC_COMBO_PORT,ports);
	// select last server
	idx = m_cbServers.FindStringExact(-1,m_server);
	if(idx >= 0)
		m_cbServers.SetCurSel(idx);
	// select last port
	char port[10] = {0};
	wsprintf(port,"%d",m_port);
	idx = m_cbPorts.FindStringExact(-1,port);
	if(idx >=  0)
		m_cbPorts.SetCurSel(idx);
}
void CLoginDlg::OnAccountChange(void)
{
	int idx;

	idx = m_cbAccs.GetCurSel();
	if(idx >= 0){
		m_cbAccs.GetLBText(idx,m_acc);
		if(!m_acc.IsEmpty()){
			if(m_pa->SetFile(g_app.GetOptionsFile(),m_acc)){
				m_pw = m_pa->GetPassword();
				m_room = m_pa->GetLastRoom();
				::SetWindowText(GetDlgItem(IDC_EDIT_PASSWORD),m_pw);
				m_cbRoom.SetWindowText(m_room);
			}
		}
	}
}

void CLoginDlg::FillComboBox(UINT nID,const CSimpleArray<CString> &lst,
							 bool bReset)
{
	CComboBox cb = GetDlgItem(nID);

	if(bReset)
		cb.ResetContent();

	for(int i=0;i<lst.GetSize();i++){
		cb.AddString(lst[i]);
	}
}

void CLoginDlg::FillComboBox(UINT nID,const CSimpleValArray<int> &lst,
							 bool bReset)
{
	CComboBox cb = GetDlgItem(nID);
	CString s;

	if(bReset)
		cb.ResetContent();

	for(int i=0;i<lst.GetSize();i++){
		s.Empty();
		s.Append(lst[i]);
		cb.AddString(s);
	}
}

LPCTSTR CLoginDlg::GetProtoName(int proto)
{
	switch(proto){
	case PROTO_YCHT:
		return "ycht";
	case PROTO_YMSG:
		return "ymsg";
	}
	return "";
}

LPCTSTR CLoginDlg::GetViewName(int view)
{
	switch(view){
	case VIEW_HTML:
		return "html";
	case VIEW_RTF:
		return "rtf";
	case VIEW_TXT:
		return "text";
	}
	return "";
}

void CLoginDlg::SaveServers(void)
{
	CStringArray servers;

	if(m_server.IsEmpty())
		return;

	switch(m_iProto){
		case PROTO_YCHT:
			m_po->GetServersYcht(servers);
			break;
		case PROTO_YMSG:
			m_po->GetServersYmsg(servers);
			break;
	}
	if(NO5TL::StringArray_FindNoCase(servers,m_server) < 0){
		servers.Add(m_server);
		//
		switch(m_iProto){
			case PROTO_YCHT:
				m_po->SetServersYcht(servers);
				break;
			case PROTO_YMSG:
				m_po->SetServersYmsg(servers);
				break;
		}
	}
}

void CLoginDlg::SavePorts(void)
{
	CIntArray ports;

	if(m_port <= 0)
		return;

	switch(m_iProto){
		case PROTO_YCHT:
			m_po->GetPortsYcht(ports);
			break;
		case PROTO_YMSG:
			m_po->GetPortsYmsg(ports);
			break;
	}
	if(ports.Find(m_port) < 0){
		ports.Add(m_port);
		//
		switch(m_iProto){
			case PROTO_YCHT:
				m_po->SetPortsYcht(ports);
				break;
			case PROTO_YMSG:
				m_po->SetPortsYmsg(ports);
				break;
		}
	}
}

void CLoginDlg::LoadProtoCombo(void)
{
	int idx;
	UINT i;

	m_cbProtos.ResetContent();

	for(i=0;i<PROTO_NONE;i++){
		idx = m_cbProtos.AddString(GetProtoName(i));
		m_cbProtos.SetItemData(idx,i);
	}
}

void CLoginDlg::LoadViewCombo(void)
{
	int idx;
	UINT i;

	m_cbViews.ResetContent();

	for(i=0;i<VIEW_NONE;i++){
		idx = m_cbViews.AddString(GetViewName(i));
		m_cbViews.SetItemData(idx,i);
	}
}

void CLoginDlg::LoadRoomCombo(void)
{
	CRecentRooms *p = g_app.GetRecentRooms();
	
	if(!p){
		g_app.LoadRecentRooms();
		p = g_app.GetRecentRooms();
	}
	if(p){
		p->FillCombo(m_cbRoom);
	}
}
	
