// YahooMsgView.cpp: implementation of the CYahooMsgView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <mshtmcid.h>
#include <no5tl\socket.h>
#include <no5tl\mystring.h>
#include "YahooMsgView.h"
#include "no5options.h"
#include "iuserlistview.h"
#include "imainframe.h"
#include "commandparser.h"
#include "no5app.h"
#include "colorfile.h"
#include "privatebox.h"
#include "resource.h"
#include "recentrooms.h"
#include "ibuddylistview.h"
#include "simpleinputdlg.h"
#include "camviewdlg.h"
#include "camupdlg.h"
#include "voicedialog.h"
#ifdef _DEBUG
#include <no5tl\clipboard.h>
#endif
#include <no5tl\winfile.h>

// fuck for some reason even using this some objects need to be declared with NO5TL
// like NO5TL::CHtmlElement
using namespace NO5TL;
using namespace NO5TL::Colors;
using namespace NO5YAHOO;

CString GetAwayString(UINT i);


// CYahooMsgViewBase

CYahooMsgViewBase::CYahooMsgViewBase(CTextStream &ts):CChatViewBase(ts)
{
	m_pChat = NULL;
	NO5YAHOO::YahooMessengerCreate(&m_pChat,this);
	HRESULT hr = CreateChatObj(this,&m_spChat);
	ATLASSERT(SUCCEEDED(hr));
	unsigned short ymsg_version[] = {8,1,0,413};
	CMessengerVersion v(ymsg_version);
	m_pChat->SetBuildNumber(v);
}

CYahooMsgViewBase::~CYahooMsgViewBase()
{
	ATLTRACE("~CYahooMsgViewBase\n");
	NO5YAHOO::YahooMessengerDestroy(&m_pChat);
}

// CYahooMsgViewBase::ISocketEvents
void CYahooMsgViewBase::OnSockConnect(int iError)
{
	InitTS init(m_ts);

	m_ts << m_pvo->TxtNo5();
	m_ts << "connected";
	m_pChat->Auth(YAHOO_STATUS_AVAILABLE);
}

void CYahooMsgViewBase::OnSockConnecting(void)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "connecting...";
}

void CYahooMsgViewBase::OnSockClose(int iError)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "connection closed";
}

void CYahooMsgViewBase::OnSockError(int error)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "socket error: " << m_pvo->TxtError() << NO5TL::CSocket::GetErrorDesc(error);
}

void CYahooMsgViewBase::OnSockConnectTimeout(void)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "timed  out";
}

void CYahooMsgViewBase::OnSockResolvingAddress(void)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "resolving server address";
}

void CYahooMsgViewBase::OnSockAddressResolved(int error)
{
	if(!error){
		InitTS init(m_ts);
		m_ts << m_pvo->TxtNo5();
		m_ts << "server address resolved";
	}
}


CUser * CYahooMsgViewBase::AddUser(CYahooMsgUser &user)
{
	CUser *pUser;
	
	pUser = m_users.GetUser(user.m_name);
	if(!pUser){
		pUser = AddUser(user.m_name);
	}
	CopyUserInfo(pUser,user);
	pUser->m_chat = true;
	
	// well this isnt the time the user entered coz the user was already in room
	pUser->m_tEnter = GetTickCount();

	return pUser;
}

CUser * CYahooMsgViewBase::AddUser(LPCTSTR name)
{
#ifdef _DEBUG
	CUser *pFound = m_users.GetUser(name);
	if(pFound){
		ATLTRACE("CYahooMsgViewBase::AddUser adding %s but user already there!\n",name);
	}
#endif
	CUser *pUser = m_users.AddUser(name);

	pUser->m_ignored = m_pIgnoreList->find(name);
	pUser->m_me = IsMe(name);
	pUser->m_buddy = IsBuddy(name);

	return pUser;
}

void CYahooMsgViewBase::AddUsers(CYahooMsgUsers &users)
{
	int i;
	const int count = users.GetSize();

	for(i=0;i<count;i++){
		AddUser(users[i]);
	}
}

void CYahooMsgViewBase::CopyUserInfo(CUser *pUser,CYahooMsgUser &from)
{
	ATLASSERT(pUser);
	pUser->m_cam = from.Cam();
	pUser->m_nick = from.m_nick;
	pUser->m_location = from.m_location;
	pUser->m_age = from.m_age < 0 ? 0 : from.m_age;
	if(from.Male())
		pUser->m_gender = CUser::USER_GENDER_MALE;
	else if(from.Female())
		pUser->m_gender = CUser::USER_GENDER_FEMALE;
	else
		pUser->m_gender = CUser::USER_GENDER_UNKNOWN;
}


void CYahooMsgViewBase::OnTextInput(LPCTSTR text)
{
	CInputStream ts;
	CCommandParser cp;
	NO5YAHOO::CTextAtom atom;
	CString smiley;
	CInputParams &p = m_input_params;
	ISmileyMap *psm = g_app.GetSmileyMap();

	m_pFrame->InputFrame()->GetParams(p);

	if(p.colorset.CompareNoCase("random") == 0)
		LoadColorSet();
	

	if(NULL != text && lstrlen(text)){
		if(ParseCmd(text) && DispatchCmd()){
			// it was a command
		}
		else if(text[0] == ':' && m_pChat->IsConnected() && !(psm->IsSmiley(text,smiley))){
			m_pChat->ChatMessage(&(text[1]),false,YMSG_TEXT_EMOTE);
		}
		else if(m_pChat->IsConnected()){
			// add info tag
			if(!m_it.IsEmpty()){
				//ts << InlineMode << (LPCSTR)m_it.GetCode();
				// in input we can avoid all the parsing
				ts.GetInfoTag() = m_it;
			}
				
			ts << Bold(p.bold) << Italic(p.italic) << Under(p.underline);
			if(! (p.fade || p.blend) ){
				CColor color = p.color;
				ts << color;
			}
			if(!p.font.IsEmpty()){
				if(!p.font_size)
					p.font_size = 10;
				ts << Font(p.font,p.font_size);
			}
			if(p.fade){
				atom.AtomFade(m_colors);
				ts.AddAtom(atom);
			}
			else if(p.blend){
				atom.AtomAlt(m_colors);
				ts.AddAtom(atom);
			}
			ts << InlineMode << text;
			if(p.fade){
				atom.AtomFadeOff();
				ts.AddAtom(atom);
			}
			else if(p.blend){
				atom.AtomAltOff();
				ts.AddAtom(atom);
			}
			if(!p.font.IsEmpty())
				ts << Font(true /* close */);

			m_pChat->ChatMessage(ts.GetYahooText(),false,YMSG_TEXT_NORMAL);
		}
	}
}


BOOL CYahooMsgViewBase::OnCmdJoin()
{
	if(m_pChat->IsConnected()){
		CString room;

		if(GetArgs1(room)){
			CString name,id;
			ParseRoomString(room,name,id);
			m_pChat->ChatJoin(name,id);
		}
	}
	return TRUE;
}

BOOL CYahooMsgViewBase::OnCmdGoto()
{
	if(m_pChat->IsConnected()){
		CString name;
		if(GetArgs1(name))
			m_pChat->GotoUser(name);
	}
	return TRUE;
}

BOOL CYahooMsgViewBase::OnCmdPm()
{
	if(m_pChat->IsConnected()){
		CString name,msg;
		if(GetArgs2(name,msg))
			m_pChat->SendMsg(m_pChat->GetName(),name,msg);
	}
	return TRUE;
}

BOOL CYahooMsgViewBase::OnCmdAway()
{
	if(m_pChat->IsConnected()){
		CString msg;
		if(GetArgs1(msg))
			m_pChat->SetStatus(YAHOO_STATUS_CUSTOM,msg);
		else
			m_pChat->SetStatus(YAHOO_STATUS_BRB);
	}
	return TRUE;
}

BOOL CYahooMsgViewBase::OnCmdBack()
{
	if(m_pChat->IsConnected())
		m_pChat->SetStatus(YAHOO_STATUS_AVAILABLE);
	return TRUE;
}

BOOL CYahooMsgViewBase::OnCmdLeave()
{
	if(m_pChat->IsConnected()){
		m_pChat->ChatExit();
	}
	return TRUE;
}

BOOL CYahooMsgViewBase::OnCmdThink()
{
	BOOL res = FALSE;

	if(m_pChat->IsConnected()){
		CString msg;
		if(GetArgs1(msg)){
			CString s = ". o O ( ";
			s += msg;
			s += " )";
			m_pChat->ChatMessage(s,false,YMSG_TEXT_BUBBLE);
		}
	}
	return TRUE;
}

BOOL CYahooMsgViewBase::OnCmdEmote()
{
	BOOL res = FALSE;

	if(m_pChat->IsConnected()){
		CString msg;
		if(GetArgs1(msg))
			m_pChat->ChatMessage(msg,false,YMSG_TEXT_EMOTE);
	}
	return TRUE;
}

BOOL CYahooMsgViewBase::OnCmdBud()
{
	BOOL res = FALSE;

	if( m_pChat->IsConnected()){
		CString name,group;
		if(GetArgs2(name,group))
			m_pChat->AddBuddy(m_pChat->GetName(),name,group);
		else if(GetArgs1(name))
			m_pChat->AddBuddy(m_pChat->GetName(),name,"no5");
	}
	return TRUE;
}

BOOL CYahooMsgViewBase::OnCmdInvite()
{
	BOOL res = FALSE;

	if(m_pChat->IsConnected()){
		CString name;
		if(GetArgs1(name))
			m_pChat->ChatInvite(name,m_pChat->GetRoom(),m_pChat->GetRoomSpace());
	}
	return TRUE;
}

BOOL CYahooMsgViewBase::OnCmdViewcam()
{
	if(m_pChat->IsConnected()){
		CString name;
		if(GetArgs1(name))
			OnUserList_Cam(name);
	}
	return TRUE;
}


BOOL CYahooMsgViewBase::OnCmdUnknown()
{
	InitTS init(m_ts);
	BOOL res = TRUE;

	if(!m_cmd.CompareNoCase("room"))
		m_ts << m_pChat->GetRoom();
	else if(!m_cmd.CompareNoCase("rmspace"))
		m_ts << m_pChat->GetRoomSpace();
	else if(!m_cmd.CompareNoCase("rmcat"))
		m_ts << m_pChat->GetRoomCategory();
	//else if(!m_cmd.CompareNoCase("vc"))
	//	m_ts << (m_pChat->IsVoiceEnabled() ? "true" : "false") ;
	else if(!m_cmd.CompareNoCase("camserver"))
		m_ts << m_pChat->GetCamServer();
	else if(!m_cmd.CompareNoCase("accept")){
		if(m_args.GetSize())
			m_pChat->AcceptReq(m_pChat->GetName(),m_args[0],true);
	}
	else if(!m_cmd.CompareNoCase("noaccept")){
		if(m_args.GetSize())
			m_pChat->AcceptReq(m_pChat->GetName(),m_args[0],false);
	}
	else if(!m_cmd.CompareNoCase("reject")){
		if(m_args.GetSize())
			m_pChat->RejectBuddy(m_pChat->GetName(),m_args[0],"blah");
	}
	else if(!m_cmd.CompareNoCase("add")){
		if(m_args.GetSize())
			m_pChat->AddBuddy(m_pChat->GetName(),m_args[0],"no5"," can i add you? ");
	}
	else if(!m_cmd.CompareNoCase("deny")){
		if(m_args.GetSize())
			m_pChat->RejectBuddy(m_pChat->GetName(),m_args[0],"");
	}
	else if(!m_cmd.CompareNoCase("rejoin"))
		m_pChat->ChatJoin(m_pChat->GetRoom(),m_pChat->GetRoomSpace(),true);
	else if(!m_cmd.CompareNoCase("localserver")){
		CString server;
		int port;
		m_pChat->GetLocalAddress(server,port);
		server += ':';
		server.Append(port);
		m_ts << (LPCTSTR)server;
	}
	else if(!m_cmd.CompareNoCase("server")){
		CString server;
		int port;
		m_pChat->GetRemoteAddress(server,port);
		server += ':';
		server.Append(port);
		m_ts << (LPCTSTR)server;
	}
	else if(!m_cmd.CompareNoCase("cookies")){
		CString s;
		LPCTSTR p;
		p = m_pChat->GetCookie('Y');
		if(p)
			m_ts << p << "\r\n";
		p = m_pChat->GetCookie('T');
		if(p)
			m_ts << p << "\r\n";
		p = m_pChat->GetCookie('C');
		if(p)
			m_ts << p << "\r\n";
		p = m_pChat->GetCookie(61);
		if(p)
			m_ts << p << "\r\n";
		p = m_pChat->GetCookie(130);
		if(p)
			m_ts << p << "\r\n";
		p = m_pChat->GetCookie(149);
		if(p)
			m_ts << p << "\r\n";
		p = m_pChat->GetCookie(150);
		if(p)
			m_ts << p << "\r\n";
		p = m_pChat->GetCookie(151);
		if(p)
			m_ts << p << "\r\n";
	}
	else
		res = FALSE;
	return res;
}

void CYahooMsgViewBase::ChangeRoom(LPCTSTR room)
{
	if(m_pChat->IsConnected()){
		CString name,id;
		ParseRoomString(room,name,id);
		m_pChat->ChatJoin(name,id);
	}
}


// message handlers
LRESULT CYahooMsgViewBase::OnCreate(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	CString name;

	name = m_pgo->GetLastAccountName();

	ATLASSERT(!m_pao);
	m_pao = IAccountOptions::CreateMe();
	BOOL res = m_pao->SetFile(g_app.GetOptionsFile(),name);
	ATLASSERT(res);	

	res = LoadInputParams();
	ATLASSERT(res);

	if(m_bActive){
		// disable room history buttons
		m_pFrame->GetChatToolbar().EnableButton(ID_ROOM_PREVIOUS,FALSE);
		m_pFrame->GetChatToolbar().EnableButton(ID_ROOM_NEXT,FALSE);
	}
	m_it.Add("ID","NO5");

	return 0;
}

LRESULT CYahooMsgViewBase::OnClose(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	CString name = m_pChat->GetName();

	if(m_pChat->IsInRoom())
		m_pChat->ChatLogout();

	m_pChat->Term(true);

	if(m_bActive){
		m_pFrame->InputFrame()->GetParams(m_input_params);
		m_pFrame->UserListView()->SetSink(NULL);
		m_pFrame->UserListView()->RemoveAll();
		m_pFrame->ActiveView(NULL);
		m_pFrame->InputFrame()->SetSink(NULL);

		BOOL res = SaveInputParams();
		ATLASSERT(res);
	}

	// tell all pm boxes to destroy themselves
	m_pms.DestroyBoxes();
	m_pms.RemoveAll();

	// remove our id from the buddy list view
	m_pFrame->BuddyListView()->RemoveAcc(name);

	return 0;
}

LRESULT CYahooMsgViewBase::OnChatConnect(WORD,WORD,HWND,BOOL &bHandled)
{

	if(m_pChat->IsConnected()){
		if(m_pChat->IsInRoom())
			m_pChat->ChatLogout();
		m_pChat->Term(false);
	}
	else{
		BOOL res = m_pChat->Init(m_pgo->GetServerYmsg(),m_pgo->GetPortYmsg(),
			m_pao->GetName(),m_pao->GetPassword());
		ATLASSERT(res);
	}

	return 0;
}

LRESULT CYahooMsgViewBase::_OnChatLeave(WORD,WORD,HWND,BOOL &bHandled)
{
	if(m_pChat->IsConnected())
		m_pChat->ChatLogout();
	return 0;
}

LRESULT CYahooMsgViewBase::OnRoomPrevious(WORD,WORD,HWND,BOOL &bHandled)
{
	if(m_RoomHistory.HasPrevious() && m_pChat->IsConnected()){
		m_RoomHistory.Previous();
		m_RoomHistory.m_bAdd = false;
		LPCTSTR room = m_RoomHistory.GetRoom();
		CString name,id;

		ParseRoomString(room,name,id);
		m_pChat->ChatJoin(name,id);
	}
	return 0;
}

LRESULT CYahooMsgViewBase::OnRoomNext(WORD,WORD,HWND,BOOL &bHandled)
{
	if(m_RoomHistory.HasNext() && m_pChat->IsConnected()){
		m_RoomHistory.Next();
		m_RoomHistory.m_bAdd = false;
		LPCTSTR room = m_RoomHistory.GetRoom();
		CString name,id;

		ParseRoomString(room,name,id);
		m_pChat->ChatJoin(name,id);
	}
	return 0;
}

LRESULT CYahooMsgViewBase::OnChatCam(WORD,WORD,HWND,BOOL &bHandled)
{
	if(m_pChat->IsConnected()){
		m_sCamTarget = "!@#$";
		m_pChat->Webcam(m_pChat->GetName());
	}
	return 0;
}
LRESULT CYahooMsgViewBase::OnChatVoice(WORD,WORD,HWND,BOOL &bHandled)
{
	CVoiceDialog *p = new CVoiceDialog();
	p->m_name = m_pChat->GetName();
	p->m_server = "v12.vc.scd.yahoo.com";
	p->m_room = m_pChat->GetRoom();
	p->m_rmid = m_pChat->GetRoomSpace();
	p->m_cookie = m_pChat->GetCookie(130);
	p->Create(g_app.GetMainFrame()->GetHandle());
	return 0;
}
#ifdef _DEBUG
LRESULT CYahooMsgViewBase::OnDebugAuthenticate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	m_pChat->Auth(YAHOO_STATUS_AVAILABLE);
	return 0;
}

LRESULT CYahooMsgViewBase::OnDebugAuthResp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	m_pChat->AuthResponse(YAHOO_STATUS_AVAILABLE,true);
	return 0;
}

LRESULT CYahooMsgViewBase::OnDebugChatOnline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	m_pChat->ChatOnline();
	return 0;
}

LRESULT CYahooMsgViewBase::OnDebugJoinRoom(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	m_pChat->ChatJoin("soccer:1","1600326632");
	return 0;
}

LRESULT CYahooMsgViewBase::OnDebugLogoff(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	m_pChat->Logoff();
	return 0;
}

LRESULT CYahooMsgViewBase::OnDebugChatLogoff(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	m_pChat->ChatLogout();
	return 0;
}

LRESULT CYahooMsgViewBase::OnDebugChatLeave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	m_pChat->ChatLeave();
	return 0;
}
#endif // _DEBUG

void CYahooMsgViewBase::OnUserLogon(u_long status,CSimpleArray<CBuddyInfo> &buddies,bool bMe)
{
	const int count = buddies.GetSize();

	if(!bMe){
		for(int i=0;i<count;i++){
			m_buddies.SetBuddyStatus(buddies[i].m_name,buddies[i].m_status);
			if(m_bActive){
				InitTS is(m_ts);
				m_pFrame->BuddyListView()->OnUserAway(buddies[i].m_name);
				if(buddies[i].m_status != YAHOO_STATUS_OFFLINE){
					m_ts << m_pvo->NameBuddy() << (LPCTSTR)buddies[i].m_name << \
						" is online - "  << (LPCTSTR) GetAwayString(buddies[i].m_status);
				}
			}
		}
	}
	if(bMe)
		m_pChat->ChatOnline();
}
void CYahooMsgViewBase::OnUserLogoff(u_long status,CSimpleArray<CBuddyInfo> &buddies)
{	
	const int count = buddies.GetSize();

	for(int i=0;i<count;i++){
		m_buddies.SetBuddyStatus(buddies[i].m_name,YAHOO_STATUS_OFFLINE);
		if(m_bActive){
			InitTS is(m_ts);
			m_pFrame->BuddyListView()->OnUserAway(buddies[i].m_name);
				m_ts << m_pvo->NameBuddy() << (LPCTSTR)buddies[i].m_name << " is offline";
		}
	}
}
void CYahooMsgViewBase::OnAuthenticate(u_long status,LPCSTR challenge,
		bool ver11)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "authenticating...";
	m_pChat->AuthResponse(YAHOO_STATUS_OFFLINE,true);
}
void CYahooMsgViewBase::OnAuthRespError(void)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "authentication has failed";
}

void CYahooMsgViewBase::OnChatOnline(void)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "chat online" << "\r\n";
	m_ts << "sending request to join room: ";

	CString s = m_pao->GetLastRoom();
	m_ts << LPCTSTR(s) << "\r\n";
	CString name,id;
	ParseRoomString(s,name,id);
	m_pChat->SetStatus(YAHOO_STATUS_AVAILABLE);
	m_pChat->ChatJoin(name,id);
}
void CYahooMsgViewBase::OnChatJoin(LPCSTR room,LPCSTR topic)
{
	_OnChatJoin(room,m_pChat->GetRoomSpace(),topic);
}

void CYahooMsgViewBase::OnUserList(CYahooMsgUsers &users,bool bMore)
{
	// add new users to our internal list. this will also mark them as in chat
	AddUsers(users);

	if(!bMore && m_bActive){
		FillUserListView();
		FillQuickIgnoreView();
	}
	ATLTRACE("OnUserList - more = %d\n",(bMore ? 1: 0));
}

void CYahooMsgViewBase::OnText(LPCSTR user,LPCSTR msg,bool utf8)
{
	CUser *pUser = _OnChatText(user,msg,utf8);

	if(!pUser->IsIgnored()){
		// mimic
		if(( pUser->m_mimic || m_bMimicAll ) && !pUser->m_me){
			m_pChat->ChatMessage(msg,utf8,YMSG_TEXT_NORMAL);
		}
	}
	if(g_app.GetIgnoreOptions().m_auto && g_app.GetIgnoreOptions().m_ybots){
		if(! ( pUser->IsIgnored() || pUser->m_me  || pUser->m_buddy ) ){
			u_long version = m_pChat->GetPacketIn().m_header.GetVersion();

			if(version == 0x6572726F){
				DoAutoIgnoreUser(pUser->m_name,true,IDS_IG_YBOT);
			}
		}
	}
	// alice bot
	if(m_spBot && !pUser->m_me){
		CComBSTR bname = pUser->m_name;
		HRESULT hr = m_spBot->put_UserName(bname);
		if(SUCCEEDED(hr)){
			CInputStream is;
			is << YahooMode << msg;
			CString s = is.GetPlainText2();
			if(m_BotTrigger.IsEmpty() || s.Find(m_BotTrigger,0) >= 0){
				s.Replace(m_BotTrigger,"");
				CComBSTR bstrIn = s;
				CComBSTR bstrOut;
				hr = m_spBot->respond(bstrIn,&bstrOut);
				if(SUCCEEDED(hr)){
					bstrIn.Empty();
					hr = m_spBot->stripurl(bstrOut,&bstrIn);
					USES_CONVERSION;
					CString s = OLE2CT(bstrIn);
					s.Replace("<br>","");
					bstrIn = s;
					m_pChat->ChatMessage(OLE2CT(bstrIn),false,YMSG_TEXT_NORMAL);
				}
			}
		}
	}
}

void CYahooMsgViewBase::OnEmote(LPCSTR user,LPCSTR msg,bool utf8)
{
	CUser *pUser = _OnEmote(user,msg,utf8);

	if(!pUser->IsIgnored()){
		// mimic
		if((pUser->m_mimic || m_bMimicAll) && !pUser->m_me ){
			m_pChat->ChatMessage(msg,utf8,YMSG_TEXT_EMOTE);
		}
	}
}

void CYahooMsgViewBase::OnThink(LPCSTR user,LPCSTR msg,bool utf8)
{
	CUser *pUser = _OnThink(user,msg,utf8);

	if(!pUser->IsIgnored()){
		// mimic
		if((pUser->m_mimic || m_bMimicAll) && !pUser->m_me ){
			m_pChat->ChatMessage(msg,utf8,YMSG_TEXT_BUBBLE);
		}
	}
}

void CYahooMsgViewBase::OnUserLeaves(NO5YAHOO::CYahooMsgUser &user)
{
	CUser *pUser = _OnUserLeave(user.m_name);

	if(pUser->m_follow)
		m_pChat->GotoUser(pUser->m_name);
	if(IsMe(pUser->m_name)){
		m_pChat->ChatPing();
		m_pFrame->AddMarqueeString("*chatping*");
	}
}
void CYahooMsgViewBase::OnUserEnters(NO5YAHOO::CYahooMsgUser &user)
{
	CUser *pUser = m_users.GetUser(user.m_name);
	
	if(!pUser){
		pUser = AddUser(user); // this also mark the user as in chat
	}
	else{ 
		CopyUserInfo(pUser,user);
		pUser->m_chat = true;
	}

	_OnUserEnter(pUser);
}
void CYahooMsgViewBase::OnInstantMessage(LPCSTR from,LPCSTR to,LPCSTR msg,
	bool bUTF8,LPCSTR im_env,LPCSTR t)
{
	CYahooMsgPacketIn &pi = m_pChat->GetPacketIn();
	ULONG status;

	switch(status = pi.m_header.GetStatus()){
		case PM_OK:
			if(from && lstrlen(from)){
				_OnPrivateMsg(from,to,msg,bUTF8,t);
			}
			break;
		case PM_USEROFF:
			//
			break;
		default:
			ATLTRACE("unknown pm status: %ld\n",status);
			break;
	}
	
}
void CYahooMsgViewBase::OnPrivateMessage(LPCSTR from,LPCSTR to,LPCSTR msg,bool utf8)
{
	CYahooMsgPacketIn &pi = m_pChat->GetPacketIn();
	ULONG status;

	switch(status = pi.m_header.GetStatus()){
		case PM_OK:
			if(from && lstrlen(from))
				_OnPrivateMsg(from,to,msg,utf8);
			break;
		case PM_USEROFF:
			//
			break;
		default:
			ATLTRACE("unknown pm status: %ld\n",status);
			break;
	}
}

void CYahooMsgViewBase::OnTyping(LPCTSTR from,bool bStart)
{
	CUser *pUser = m_users.GetUser(from);

	if(!pUser){
		pUser = AddUser(from);
	}
	CString s;
	if(bStart)
		s.Format(" %s is typing ",from);
	else
		s.Format(" %s stopped typing ",from);
	m_pFrame->AddMarqueeString(s);
	// notify pmbox
	CPrivateBox *pBox = m_pms.Find(from);
	if(pBox)
		pBox->Typing(bStart);
	
	// script
	if(m_spChat && pUser && !pUser->IsIgnored() && g_app.RunningScripts())
	{
		CComPtr<IDispatch> spUser;
		HRESULT hr = CreateUserObj(pUser,&spUser);
		
		if(SUCCEEDED(hr)){
			g_app.GetNo5Obj()->Fire_OnTypingNotify(m_spChat,spUser,bStart ? VARIANT_TRUE : VARIANT_FALSE);
		}
	}
}

void CYahooMsgViewBase::OnWebcamInvite(LPCTSTR from)
{
	CUser *pUser = m_users.GetUser(from);

	if(!pUser){
		pUser = AddUser(from);
	}
	if(!pUser->IsIgnored()){
		if(!pUser->OnAnyPost()){
			InitTS init(m_ts);
			CTextAtom atom;
			CString cmd("/viewcam ");
			cmd += from;
			m_ts.AddAtom(atom.AtomLink(cmd));
			m_ts.AddAtom(atom.AtomLink(FALSE));
		}
	}
	// script
	if(m_spChat && pUser && !pUser->IsIgnored() && g_app.RunningScripts())
	{
		CComPtr<IDispatch> spUser;
		HRESULT hr = CreateUserObj(pUser,&spUser);

		if(SUCCEEDED(hr)){
			g_app.GetNo5Obj()->Fire_OnCamInvitation(m_spChat,spUser);
		}
	}
}

void CYahooMsgViewBase::OnWebcamInviteReply(LPCTSTR from,bool bAccepted)
{
	CUser *pUser = m_users.GetUser(from);

	if(!pUser){
		pUser = AddUser(from);
	}
	if(!pUser->IsIgnored()){
		if(!pUser->OnAnyPost()){
			InitTS init(m_ts);
			m_ts << m_pvo->TxtNo5();
			m_ts << from << " : ";
			if(bAccepted)
				m_ts << "accepted to view your cam";
			else
				m_ts << "denied the request to view your cam";
		}
	}
	// script
	if(m_spChat && pUser && !pUser->IsIgnored() && g_app.RunningScripts())
	{
		CComPtr<IDispatch> spUser;
		HRESULT hr = CreateUserObj(pUser,&spUser);

		if(SUCCEEDED(hr)){
			g_app.GetNo5Obj()->Fire_OnWebcamInviteReply(m_spChat,spUser,
				bAccepted ? VARIANT_TRUE : VARIANT_FALSE);
		}
	}
}

void CYahooMsgViewBase::OnNotify(LPCSTR from,LPCSTR notify_string,LPCSTR status)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnAddBuddy(LPCSTR me,LPCSTR group,const CBuddyInfo &bi)
{
	DisplayUnknownCallback();
	CBuddy b;
	b.m_name = bi.m_name;
	b.m_status = bi.m_status;
	CBuddy *pb = m_buddies.AddBuddy(group,b);
	if(m_bActive)
		m_pFrame->BuddyListView()->OnAddBuddy(group,pb);
	CUser *pUser = m_users.GetUser(bi.m_name);
	if(pUser)
		pUser->m_buddy = true;
}

void CYahooMsgViewBase::OnRemBuddy(LPCSTR me,LPCSTR buddy,LPCSTR group)
{
	DisplayUnknownCallback();
	if(m_bActive)
		m_pFrame->BuddyListView()->OnRemBuddy(group,buddy);
	m_buddies.RemoveBuddy(group,buddy);

	CUser *pUser = m_users.GetUser(buddy);
	if(pUser)
		pUser->m_buddy = false;
}

void CYahooMsgViewBase::OnIgnoreUser(LPCSTR who,LPCSTR me,bool ignore,LPCSTR answer)
{
	DisplayUnknownCallback();
}
void CYahooMsgViewBase::OnBuddyList(LPCTSTR group,CSimpleArray<CString> &buddies)
{
	CUser *pUser;
	const int count = buddies.GetSize();
	int i;

	for(i=0;i<count;i++){
		pUser = m_users.GetUser(buddies[i]);
		if(pUser)
			pUser->m_buddy = true;
	}
	CBuddyGroup *pg = m_buddies.AddBuddies(group,buddies);
	if(m_bActive){
		m_pFrame->BuddyListView()->SetAcc(this,m_pChat->GetName(),&m_buddies);
		m_pFrame->BuddyListView()->OnBuddyGroup(pg);
	}
}
void CYahooMsgViewBase::OnIgnoreList(CSimpleArray<CString> &lst)
{
	
}
void CYahooMsgViewBase::OnAlternateIdList(CSimpleArray<CString> &lst)
{
	// todo
}
void CYahooMsgViewBase::OnChatLogout(void)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "you have logged out of chat";
}

void CYahooMsgViewBase::OnChatInvite2(LPCSTR room,LPCSTR rmspace,LPCSTR \
	from,LPCSTR to,LPCSTR msg)
{
	if( (!from) || (!lstrlen(from)) )
		return;
	CUser *pUser = _OnInvited(from,room,rmspace,msg);
}

void CYahooMsgViewBase::OnUserAway(LPCSTR user,int code,LPCSTR msg)
{
	InitTS init(m_ts);
	CString s = GetAwayString(code);

	m_ts << m_pvo->NameBuddy();
	m_ts << user << (LPCTSTR)s;
	if(msg && lstrlen(msg))
		m_ts << "( " << msg << " )";

	m_buddies.SetBuddyStatus(user,(long)code,msg);
	if(m_bActive)
		m_pFrame->BuddyListView()->OnUserAway(user);

}
void CYahooMsgViewBase::OnUserBack(LPCSTR user,int code,LPCSTR msg)
{
	DisplayUnknownCallback();
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << user << " is back ";
	if(msg && lstrlen(msg))
		m_ts << "( " << msg << " )";

	m_buddies.SetBuddyStatus(user,(long)code,msg);
	if(m_bActive)
		m_pFrame->BuddyListView()->OnUserAway(user);
}
void CYahooMsgViewBase::OnAuthError(LPCSTR msg,LPCSTR url)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "Authentication has failed!";
}
void CYahooMsgViewBase::OnYahooError(u_long service,u_long status,
	NO5YAHOO::CYahooMap &data)
{
	InitTS init(m_ts);
	DisplayUnknownCallback();

	CString value = data.Lookup(114);
	int code = 0;

	if(!value.IsEmpty()){
		// the key 114 contains the error code. the error code is followed by a \n
		// i dont know if its possible to get a list of error. so far only one code got
		CStringToken st;

		st.Init(value,"\n");
		value = st.GetNext();
		if(!value.IsEmpty())
			code = atoi(value);
	}

	switch(service)
	{
	case YAHOO_SERVICE_CHATJOIN:
		{
			bool noknock = true;
			switch(code)
			{
				case -35:
					{
						CString room,id,alt;
						m_pChat->GetDesiredRoom(room,id);
						int index = room.ReverseFind(':');
						if(index > 0){ // soccer:10
							alt = room.Left(index);
						}
						else
							alt = room;
						_OnRoomFull(alt,room,id);
						noknock = false;
					}
					break;
				case 16:
					m_ts << "you are already in this room";
					break;
				case -6:
					m_ts << "no such room";
					break;
				case -15: // not in chat ?
					m_ts << "you are not logged in chat";
				default:
					break;
			}
			if(noknock){
				OnCmdNoKnock();
			}
		}
		break;
	case YAHOO_SERVICE_CHATGOTO:
		switch(code)
		{
			case -11:
				m_ts << "no such user";
				break;
			default:
				break;
		}
		break;
	case YAHOO_SERVICE_CHATLOGOUT:
		switch(code)
		{
			case -1:
				m_ts << "failed to logout of chat ( not logged in ? )";
				break;
			default:
				break;
		}
		break;
	case YAHOO_SERVICE_COMMENT:
		switch(code){
			case -1:
				m_ts << "you are not logged in chat";
				break;
			default:
				break;
		}
		break;

	default:
		break;
	}
}

void CYahooMsgViewBase::OnPing(u_long status)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "yahoo ping";
	m_pChat->Ping();
}

void CYahooMsgViewBase::OnNewContact(u_long status,LPCTSTR to,LPCTSTR buddy,LPCTSTR msg)
{
	DisplayUnknownCallback();
	InitTS is(m_ts);

	if(status == 3){
		CString cmd;
		CTextAtom atom;

		m_ts << buddy << " has added you to his/her buddy list";
		if(msg && lstrlen(msg))
			m_ts << " ( " << msg << " ) ";

		cmd = "/add ";
		cmd += buddy;
		atom.AtomLink(cmd);
		m_ts.AddAtom(atom);
		atom.AtomLink(FALSE);
		m_ts.AddAtom(atom);
		m_ts << " or ";
		cmd = "/deny ";
		cmd += buddy;
		atom.AtomLink(cmd);
		m_ts.AddAtom(atom);
		atom.AtomLink(FALSE);
		m_ts.AddAtom(atom);
	}
	else if(status == 7){
		m_ts << buddy << " has rejected to add you to his/her buddy list";
		if(msg && lstrlen(msg))
			m_ts << " ( " << msg << " ) ";
	}
	else if(status == 1){
		// ok
	}
}

void CYahooMsgViewBase::OnRejectContact(u_long status,LPCTSTR me,LPCTSTR buddy,LPCTSTR msg)
{
	DisplayUnknownCallback();
	InitTS its(m_ts);
	CString s;
	if(!msg)
		msg = "";
	s.Format("you rejected contact from %s - %s",buddy,msg);
}


void CYahooMsgViewBase::OnNewMail(int count)
{
	if(count > 0){
		InitTS init(m_ts);
		CString msg;

		m_ts << m_pvo->TxtNo5();

		msg.Format("you have %d mail messages",count);
		m_ts << (LPCTSTR)msg;
	}
}

void CYahooMsgViewBase::OnGroupRename(LPCTSTR me,LPCTSTR old,LPCTSTR group)
{
	CBuddyGroup *pg = m_buddies.FindGroup(old);
	if(pg){
		pg->m_name = group;
		if(m_bActive)
			m_pFrame->BuddyListView()->OnRenameGroup(old,group);
	}
}

void CYahooMsgViewBase::OnGotGroupRename(LPCTSTR old,LPCTSTR group)
{
	// we receive this after GROUPRENAME, is it superfulous ?
	// m_pFrame->BuddyListView()->RenameGroup(m_pChat->GetName(),old,group);
}

// unknown
void CYahooMsgViewBase::OnChatLogon(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnChatLogoff(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnSysMessage(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}



void CYahooMsgViewBase::OnIdle(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnIdActivate(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnIdDeactivate(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnMailStat(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnUserStat(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}


void CYahooMsgViewBase::OnChatInvite(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnCalendar(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnNewPersonalMail(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnAddIdent(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnAddIgnore(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnPassThrough(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnGameLogon(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnGameLogoff(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnGameMsg(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnFileTransfer(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnVoiceChat(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnVerify(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnP2PFileXfer(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnPeer2Peer(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnChatGoto(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnChatLeave(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnChatPing(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

void CYahooMsgViewBase::OnAcceptReq(u_long status,CYahooMap &map)
{
	DisplayUnknownCallback();
}

//
void CYahooMsgViewBase::OnUnknownService(u_short service,u_long status,
	NO5YAHOO::CYahooMap &data)
{
	DisplayUnknownCallback();
	{
		InitTS init(m_ts);
		m_ts << m_pvo->TxtNo5();
		m_ts << "unknown service";
	}
}

void CYahooMsgViewBase::pm_OnSend(CPrivateBox *pBox,LPCTSTR msg)
{
	if(m_pChat->IsConnected()){
		m_pChat->SendMsg(m_pChat->GetName(),pBox->m_pUser->m_name,msg);
		pBox->AddMessage(pBox->m_me,msg);
	}
}

void CYahooMsgViewBase::pm_OnTyping(CPrivateBox *pBox,BOOL bStart)
{
	if(m_pChat->IsConnected()){
		m_pChat->NotifyTyping(m_pChat->GetName(),pBox->m_pUser->m_name,bStart ? true : false);
	}
}

void CYahooMsgViewBase::blv_OnAddBuddy(LPCTSTR group)
{
	if(m_pChat->IsConnected()){
		CSimpleInputDlg dlg;

		dlg.Init("Buddy name","name:",256,"");
		if(IDOK == dlg.DoModal()){
			m_pChat->AddBuddy(m_pChat->GetName(),dlg.m_text,group," may i add you ? " );
		}
	}
}

void CYahooMsgViewBase::blv_OnRemoveBuddy(LPCTSTR name,LPCTSTR group)
{
	if(m_pChat->IsConnected())
		m_pChat->RemoveBuddy(m_pChat->GetName(),name,group);
}


void CYahooMsgViewBase::blv_OnRenameGroup(LPCTSTR group)
{
	if(m_pChat->IsConnected())
		m_pChat->RenameGroup(m_pChat->GetName(),group,"friends"); // todo
}

void CYahooMsgViewBase::blv_OnViewCam(LPCTSTR name)
{
	if(m_pChat->IsConnected()){
		m_sCamTarget = name;
		m_pChat->Webcam(m_pChat->GetName());
	}
}
void CYahooMsgViewBase::blv_OnCamInvite(LPCTSTR name)
{
	if(m_pChat->IsConnected()){
		m_pChat->WebcamInvite(name,NULL,"blah");
	}
}
void CYahooMsgViewBase::blv_OnGotoUser(LPCTSTR name)
{
	if(m_pChat->IsConnected()){
		m_pChat->GotoUser(name);
	}
}
void CYahooMsgViewBase::blv_OnInviteUser(LPCTSTR name)
{
	if(m_pChat->IsConnected()){
		m_pChat->ChatInvite(name);
	}
}

void CYahooMsgViewBase::DisplayUnknownCallback(void)
{
#ifdef _DEBUG
	CYahooMsgPacketIn &pi = m_pChat->GetPacketIn();
	long status = pi.m_header.GetStatus();
	CYahooMap & map = pi.m_data;
	u_long service = pi.m_header.GetService();
	LPCTSTR desc = GetServiceDesc(service);

	if(!desc)
		desc = "";

	{
		InitTS init(m_ts);
		const int count = map.GetSize();
		LPCTSTR keyd;
		CString s;

		m_ts << m_pvo->TxtNo5();
		s.Format("service 0x%x %s\r\nstatus: %ld\r\n",(long)service,desc,status);

		for(int i=0;i<count;i++){

			s.Append(map.GetKeyAt(i));
			keyd = GetKeyDesc(map.GetKeyAt(i));
			s += "( ";
			if(keyd){
				s += keyd;
			}
			else
				s += "no description";
			s += " )";
			s += " = ";
			s += map.GetValueAt(i);
			s += "\r\n";
		}
		m_ts << LPCTSTR(s);
	}
#endif
}

void CYahooMsgViewBase::OnUserList_Cam(LPCTSTR name)
{
	if(m_pChat->IsConnected()){
		m_sCamTarget = name;
		m_pChat->Webcam(m_pChat->GetName());
	}
}

void CYahooMsgViewBase::OnUserList_CamInvite(LPCTSTR name)
{
	if(m_pChat->IsConnected()){
		m_pChat->WebcamInvite(name);
	}
}

void CYahooMsgViewBase::OnUserList_AddBuddy(LPCTSTR name)
{
	if(m_pChat->IsConnected()){
		CString sApp;
		sApp.LoadString(IDR_MAINFRAME);
		m_pChat->AddBuddy(m_pChat->GetName(),name,sApp);
	}
}

void CYahooMsgViewBase::OnUserList_RemoveBuddy(LPCTSTR name)
{
	if(m_pChat->IsConnected()){
		CBuddyGroup *pg;
		int count = m_buddies.GetSize();
		int i;

		for(i=0;i<count;i++){
			pg = m_buddies[i];
			if(pg->FindBuddy(name)){
				m_pChat->RemoveBuddy(m_pChat->GetName(),name,pg->m_name);
			}
		}
	}
}

// CYahooMsgViewImpl
template <class T>
CYahooMsgViewImpl<T>::CYahooMsgViewImpl(CTextStream &ts):CYahooMsgViewBase(ts)
{
	//
}


template <class T>
LRESULT CYahooMsgViewImpl<T>::OnMdiActivate(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	if((HWND)lParam == GetParent()){
		m_bActive = true;
		m_pFrame->UserListView()->RemoveAll();
		m_pFrame->UserListView()->SetSink(this);
		m_pFrame->QuickIgnoreView()->SetSink(this);
		FillUserListView();
		FillQuickIgnoreView();
		m_pFrame->ActiveView(this);
		m_pFrame->InputFrame()->SetSink(this);
		m_pFrame->InputFrame()->SetParams(m_input_params);
		m_pFrame->BuddyListView()->SetAcc(this,m_pChat->GetName(),&m_buddies);
		LoadColorSet();
	}
	else
		m_bActive = false;

	if((HWND)wParam == GetParent()){
		m_pFrame->ActiveView(NULL);
		m_pFrame->UserListView()->SetSink(NULL);
		m_pFrame->QuickIgnoreView()->SetSink(NULL);
		m_pFrame->QuickIgnoreView()->RemoveAll();
		m_colors.RemoveAll();
		m_pFrame->InputFrame()->GetParams(m_input_params);
		m_pFrame->InputFrame()->SetSink(NULL);
		m_pFrame->BuddyListView()->SetAcc(NULL,NULL,NULL);
	}
	return 0;
}

template <class T>
LRESULT CYahooMsgViewImpl<T>::OnCreate(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	_BaseClass::OnCreate(WM_CREATE,wParam,lParam,bHandled);
	bHandled = TRUE;
	SetTimer(TIMER_CLEANUP, 60 * 1000);
	SetTimer(TIMER_PING, 10 * ( 60 * 1000 ));
	return 0;
}

template <class T>
LRESULT CYahooMsgViewImpl<T>::OnDestroy(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	KillTimer(TIMER_CLEANUP);
	KillTimer(TIMER_PING);
	bHandled = FALSE;
	return 0;
}

template <class T>
LRESULT CYahooMsgViewImpl<T>::OnTimer(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	switch(wParam)
	{
		case static_cast<WPARAM>(TIMER_CLEANUP):
			m_users.CleanUp();
			break;
		case static_cast<WPARAM>(TIMER_PING):
			if(m_pChat->IsConnected()){
				m_pChat->Ping();
				m_pChat->ChatPing();
				m_pFrame->AddMarqueeString("*ping*");
				m_pFrame->AddMarqueeString("*chatping*");
			}
			break;
		case static_cast<WPARAM>(TIMER_KNOCK):
			if(m_pChat->IsConnected()){
				CString room,id;
				m_pChat->GetDesiredRoom(room,id);

				m_pChat->ChatJoin(room,id);
				BOOL res = KillTimer(TIMER_KNOCK);
				ATLASSERT(res);
				if(res){
					m_knocks++;
					res = SetTimer(TIMER_KNOCK,(m_knocks + 1) * 1000);
					ATLASSERT(res);
				}
				CString msg;
				msg.Format("* knock %d *",m_knocks);
				m_pFrame->AddMarqueeString(msg);
			}
		default:
			break;
	}
	return 0;
}

// its now actually send to the main frame
template <class T>
LRESULT CYahooMsgViewImpl<T>::OnCamDlgDel(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	CCamViewDlg *p = (CCamViewDlg *)lParam;
	delete p;
	return 0;
}

template <class T>
CPrivateBox * CYahooMsgViewImpl<T>::GetPrivateBox(LPCTSTR name)
{
	// first try to find an existing one for this user
	CPrivateBox *pBox = m_pms.Find(name);
	CUser *pUser = m_users.GetUser(name);

	if(!pUser){
		pUser = AddUser(name);
	}
	ATLASSERT(pUser);
	if(!pUser)
		return NULL;
	pUser->m_pm = true;

	// there was no pm for this user open, so we create one
	if(NULL == pBox){
		static const UINT cx = 380;
		static const UINT cy = 200;
		CRect rcBox;
		CRect rcWnd;
		
		::GetWindowRect(GetDesktopWindow(),&rcWnd);
		rcBox.left = rcWnd.left + 50;
		rcBox.top = rcWnd.top + 50;
		rcBox.right = rcBox.left + cx;
		rcBox.bottom = rcBox.top + cy;

		CPrivateBox::m_pvo = m_pvo;
		pBox = new CPrivateBox(this);
		ATLASSERT(pBox);
		pBox->m_pUser = pUser;
		pBox->m_me = m_pChat->GetName();

		m_pms.AddBox(pBox);
		pBox->CreateEx(GetHandle(),rcBox);
		ATLASSERT(pBox->IsWindow());
		if(m_pPmOptions->m_topmost){
			pBox->SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0,SWP_NOSIZE | SWP_NOMOVE |\
				SWP_NOACTIVATE);
		}
	}
	return pBox;
}


template <class T>
void CYahooMsgViewImpl<T>::OnWebcam(LPCTSTR cookie,LPCTSTR server)
{
	if(m_sCamTarget.Compare("!@#$") != 0){
		CCamViewDlg *pDlg = new CCamViewDlg();

		pDlg->m_user = m_pChat->GetName();
		pDlg->m_target = m_sCamTarget;
		pDlg->m_token = cookie;
		pDlg->m_server = server;
		pDlg->m_isp = m_pgo->GetConnectionType();
		pDlg->Create(m_pFrame->GetHandle());
	}
	else{
		CCamUpDlg *pDlg = new CCamUpDlg();

		pDlg->m_user = m_pChat->GetName();
		pDlg->m_token = cookie;
		pDlg->m_server = server;
		pDlg->Create(m_pFrame->GetHandle());
	}
	m_sCamTarget.Empty();
	if(m_spChat && g_app.RunningScripts()){
		USES_CONVERSION;
		g_app.GetNo5Obj()->Fire_OnCamCookie(m_spChat,T2OLE(const_cast<LPTSTR>(cookie)),
			T2OLE(const_cast<LPTSTR>(server)));
	}
}


// CYahooMsgRichView

CYahooMsgRichView::CYahooMsgRichView(void):_BaseClass(m_ts)
{
	m_pEdit = new NO5TL::CNo5RichEdit();
	m_ts.SetEdit(m_pEdit);
}

CYahooMsgRichView::~CYahooMsgRichView()
{
	delete m_pEdit;
}

// message handlers

LRESULT CYahooMsgRichView::OnCreate(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	LRESULT lRes = _BaseClass::OnCreate(0,wParam,lParam,bHandled);
	bHandled = TRUE;

	m_pEdit->Create(m_hWnd,rcDefault,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_BORDER|\
		ES_MULTILINE|ES_AUTOVSCROLL,WS_EX_CLIENTEDGE);
	BOOL res = m_pEdit->SetDefaultOleCallback();
	ATLASSERT(res);
	m_pEdit->SetBackgroundColor(m_pvo->ClrBack());
	m_pEdit->SetEventMask(ENM_LINK|ENM_MOUSEEVENTS);
	m_pEdit->SetUndoLimit(0);	// disables undo saving memory
								// sets the max a user can type or paste
	m_pEdit->LimitText(0);		// maximum since paste operations count as one
	//m_pEdit->SetLimitText(0);	// maximum bytes the control can contain - zero == max

	res = m_pChat->Init(m_pgo->GetServerYmsg(),m_pgo->GetPortYmsg(),m_pao->GetName(),
		m_pao->GetPassword());

	ATLASSERT(res);

	
	return lRes;
}

LRESULT CYahooMsgRichView::OnSize(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	if(m_pEdit->IsWindow()){
		m_pEdit->MoveWindow(0,0,LOWORD(lParam),HIWORD(lParam));
	}
	return 0;
}

LRESULT CYahooMsgRichView::OnEditMsgFilter(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	MSGFILTER *p = (MSGFILTER *)pnmh;

	bHandled = FALSE;
	switch(p->msg)
	{
	case WM_RBUTTONUP:
		{
			int line;
			int len = 0;
			int pos;
			CPoint pt;

			GetCursorPos(&pt);
			m_pEdit->ScreenToClient(&pt);
			pos = m_pEdit->CharFromPos(pt);
			line = m_pEdit->LineFromChar(pos);
			if(line >= 0){
				len = m_pEdit->LineLength(line);
			}
			if(len > 0){
				CString s;
				CStringBuffer buf(s,len + 1);
				CString name;

				m_pEdit->GetLine(line,buf,len);
				
				buf[len] = '\0';
				LPCTSTR p = (LPCTSTR)buf;
				LPCTSTR q = p;
				while(*q && !isspace(*q))
					q++;
				if(q > p){
					StringCopyN(name,p,0,(int)(q-p));

					if(!name.IsEmpty()){
						m_pEdit->ClientToScreen(&pt);
						DisplayUserMenu(name,pt);	// todo: we dont know if its a user name
					}
				}
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

LRESULT CYahooMsgRichView::OnEditLink(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	ENLINK *p = (ENLINK *)pnmh;

	bHandled = FALSE;
	switch(p->msg)
	{
	case WM_LBUTTONDOWN:
		{
			CString link = m_pEdit->GetTextRangeString(p->chrg);
			CCommandParser cp;
			
			if(ParseCmd(link) && DispatchCmd()){
							//
			}
			else
				OpenBrowser(link);
		}
		break;
	default:
		break;
	}
	return 0; // let the control to handle the message
}

LRESULT CYahooMsgRichView::OnErrSpace(WORD,WORD,HWND,BOOL &bHandled)
{
	m_pEdit->ClearAll();
	return 0;
}

LRESULT CYahooMsgRichView::OnMaxText(WORD,WORD,HWND,BOOL &bHandled)
{
	m_pEdit->ClearAll();
	return 0;
}

LRESULT CYahooMsgRichView::OnVScroll(WORD,WORD,HWND,BOOL &bHandled)
{
	// todo: disable auto scroll ?
	return 0;
}

LRESULT CYahooMsgRichView::OnFileSaveAs(WORD,WORD,HWND,BOOL &bHandled)
{
	CFileDialog dlg(\
		FALSE,		// file open
		"ddf",		// default extension
		NULL,		// file name
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,		// flags
		"rtf files\0*.rtf\0text files\0*.txt\0All files\0*.*\0",	// filter
		m_hWnd);	// parent

	if(IDOK == dlg.DoModal()){
		// dlg.m_szFileName
		NO5TL::CWinFile wf;
		BOOL res;

		if(res = wf.Create(dlg.m_szFileName,CREATE_ALWAYS,GENERIC_WRITE)){
			res = m_pEdit->StreamOutFile(wf);
			ATLASSERT(res);
		}
	}

	return 0;
}

void CYahooMsgRichView::OnOptionsChanged(void)
{
	CChatViewBase::OnOptionsChanged();

	// todo: we dont receive information about which option has changed
	// so we will change the background colour, even if it has not changed
	m_pEdit->SetBackgroundColor(m_pvo->ClrBack());
}



// CYahooMsgHtmlView 

CYahooMsgHtmlView::CYahooMsgHtmlView(void):_BaseClass(m_ts)
{
	m_ts.SetEdit(&m_edit);
	m_pUIHandler = new CMyDocHostUIHandler((IDispatch *)this);
}

CYahooMsgHtmlView::~CYahooMsgHtmlView()
{
	delete m_pUIHandler;
}


LRESULT CYahooMsgHtmlView::OnCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CString title;
	LRESULT lRes = _BaseClass::OnCreate(WM_CREATE,wParam,lParam,bHandled);
	bHandled = TRUE;

	title.Format("res://%s/%u",g_app.GetModuleFileName(),static_cast<UINT>(IDR_HTML_CHAT));
	m_edit.Create(m_hWnd,rcDefault,title,WS_CHILD|WS_VISIBLE|WS_VSCROLL,
		WS_EX_CLIENTEDGE);
	if(m_edit.IsWindow()){
		HRESULT hr;

		hr = m_edit.GetBrowser();
		if(FAILED(hr)){
			hr = m_edit.GetDocument();
			if(SUCCEEDED(hr)){
				hr = DispEventHtml::DispEventAdvise(m_edit.m_doc.m_sp);
				if(SUCCEEDED(hr))
					hr = m_edit.GetInterfaces();
			}
		}
		else{
			hr = DispEventBrowser::DispEventAdvise(m_edit.m_spBrowser);
			if(SUCCEEDED(hr)){
				m_edit.m_spBrowser->put_Silent(VARIANT_TRUE);
				m_edit.m_spBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);
			}
		}
		if(SUCCEEDED(hr)){
			hr = m_edit.SetExternalUIHandler(static_cast<IDocHostUIHandlerDispatch*>(m_pUIHandler));
			//hr = m_edit.SetExternalDispatch(static_cast<IDispatch *>(this));
		}
		ATLASSERT(SUCCEEDED(hr));
	}

	return 0;
}

LRESULT CYahooMsgHtmlView::OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HRESULT hr = S_OK;

	if(m_edit.m_doc.m_sp){
		//hr = DispEventHtml::DispEventUnadvise(m_edit.m_doc.m_sp);
		//ATLASSERT(SUCCEEDED(hr));
	}
	if(m_edit.m_spBrowser){
		if(DispEventBrowser::m_dwEventCookie != 0xFEFEFEFE){
			hr = DispEventBrowser::DispEventUnadvise(m_edit.m_spBrowser);
			ATLASSERT(SUCCEEDED(hr));
		}
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CYahooMsgHtmlView::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_edit.MoveWindow(0,0,LOWORD(lParam),HIWORD(lParam));
	return 0;
}

LRESULT CYahooMsgHtmlView::OnFileSaveAs(WORD,WORD,HWND,BOOL &)
{
	if(m_edit.m_spCmd){
		CString path;
		path.Replace("\\","\\\\");
		HRESULT hr = m_edit.ExecCmd(IDM_SAVEAS,path);
		ATLASSERT(SUCCEEDED(hr));
	}
	return 0;
}

void CYahooMsgHtmlView::OnDocumentComplete(IDispatch *pDisp,VARIANT *pUrl)
{
	HRESULT hr = m_edit.GetDocument();

	if(SUCCEEDED(hr)){
		//hr = DispEventHtml::DispEventAdvise(m_edit.m_doc.m_sp);
		//if(SUCCEEDED(hr)){
			hr = m_edit.GetInterfaces();
			if(SUCCEEDED(hr)){
				UpdateVisualOptions();
			}
	//	}
	}
	ATLASSERT(SUCCEEDED(hr));

	if(!m_pChat->IsConnected()){
		BOOL res;
		
		res = m_pChat->Init(m_pgo->GetServerYmsg(),m_pgo->GetPortYmsg(),m_pao->GetName(),
			m_pao->GetPassword());
		if(res){
			ATLTRACE("password: %s\n",m_pao->GetPassword());
		}
		ATLASSERT(res);
	}
}

void __stdcall CYahooMsgHtmlView::OnLinkClick(BSTR name)
{
	USES_CONVERSION;
	LPCTSTR s = OLE2CT(name);
	CCommandParser cp;
	
	if(ParseCmd(s) && DispatchCmd()){
			//
	}
	else
		OpenBrowser(s);
}


void __stdcall CYahooMsgHtmlView::OnUserClick(BSTR s)
{
	USES_CONVERSION;
	LPCTSTR user = OLE2CT(s);
	CPrivateBox *pBox = m_pms.Find(user);

	if(!pBox){
		CUser *pUser = m_users.GetUser(user);
		if(pUser){
			pBox = GetPrivateBox(user);
		}
	}
	if(pBox){
		pBox->ShowPm();
	}
}

void __stdcall CYahooMsgHtmlView::OnUserRClick(BSTR s,long x,long y)
{
	USES_CONVERSION;
	LPCTSTR user = OLE2CT(s);
	CPoint pt(x,y);

	DisplayUserMenu(user,pt);
}

void __stdcall CYahooMsgHtmlView::OnDivClick(BSTR s)
{
	
}
void __stdcall CYahooMsgHtmlView::OnDivDblClk(BSTR s)
{
	
}
void __stdcall CYahooMsgHtmlView::OnDivRClick(BSTR s)
{
	
}

void __stdcall CYahooMsgHtmlView::OnUserDblClk(BSTR s)
{
	
}

void __stdcall CYahooMsgHtmlView::OnAnswerPm(BSTR s)
{
	USES_CONVERSION;
	LPCTSTR name = OLE2CT(s);
	if(name && lstrlen(name))
		OnUserList_Pm(name);
}


void CYahooMsgHtmlView::DisplayUserName(CUser *pUser)
{
	FontAll f;
	CTextAtom atom;
	CString html;
	CYahooColor color;

	if(pUser->m_me)
		f = m_pvo->NameMe();
	else if(pUser->m_buddy)
		f = m_pvo->NameBuddy();
	else
		f = m_pvo->NameUser();

	m_ts << f;
	html.Format("<span id=\"user\" name=\"%s\" class=\"user\">%s</span>",
		(LPCTSTR)pUser->m_name,
		(LPCTSTR)pUser->m_name);
	m_ts.AddHtmlAtom(html);
}

void CYahooMsgHtmlView::DisplayChatText(CUser *pUser,CTextStream &ts)
{
	CString msg;
	CString s;
	CString t;
	LPCTSTR cls;	// class name
	LPCTSTR divfn;
	LPCTSTR spfn;

	m_ts.Reset();
	m_ts.SetDefaults("tahoma",10,0);

	divfn =  "onmouseover=\"OnDivOver(this)\" onmouseout=\"OnDivOut(this)\" " \
		" onclick=\"OnDivClick(this)\" ondblclk=\"OnDivDblClk(this)\" "\
		" oncontextmenu=\"OnDivRClick(this)\" ";

	spfn =  "onmouseover=\"OnUserOver(this)\" onmouseout=\"OnUserOut(this)\" " \
		" onclick=\"OnUserClick(this)\" ondblclk=\"OnUserDblClk(this)\" "\
		" oncontextmenu=\"OnUserRClick(this)\" ";

	if(pUser->m_me)
		cls = "me";
	else if(pUser->m_buddy)
		cls = "buddy";
	else
		cls = "user";

	s.Format("<div class=\"%s\" id=\"divUser\" user=\"%s\" %s >",
		cls,(LPCTSTR)pUser->m_name,divfn);
	t.Format("<span class=\"%s\" id=\"spanUser\" user=\"%s\" %s >",
		cls,(LPCTSTR)pUser->m_name,spfn);
	t += pUser->m_name;
	t += "</span>";
	s += t;
	m_ts.AddHtmlAtom(s);
	m_ts.AddHtmlAtom("&nbsp;:&nbsp;");
	m_ts.Append(ts);
	m_ts.AddHtmlAtom("</div>");
	m_ts << Flush;
	m_ts.ScrollText();
}

void CYahooMsgHtmlView::DisplayEmote(CUser *pUser,CTextStream &ts)
{
	CString msg;
	CString s;
	CString t;
	LPCTSTR cls;	// class name
	LPCTSTR divfn;
	LPCTSTR spfn;

	m_ts.Reset();
	m_ts.SetDefaults("tahoma",10,0);

	divfn =  "onmouseover=\"OnDivOver(this)\" onmouseout=\"OnDivOut(this)\" " \
		" onclick=\"OnDivClick(this)\" ondblclk=\"OnDivDblClk(this)\" "\
		" oncontextmenu=\"OnDivRClick(this)\" ";

	spfn =  "onmouseover=\"OnUserOver(this)\" onmouseout=\"OnUserOut(this)\" " \
		" onclick=\"OnUserClick(this)\" ondblclk=\"OnUserDblClk(this)\" "\
		" oncontextmenu=\"OnUserRClick(this)\" ";

	if(pUser->m_me)
		cls = "me";
	else if(pUser->m_buddy)
		cls = "buddy";
	else
		cls = "user";

	s.Format("<div class=\"%s\" id=\"divUser\" user=\"%s\" %s >",
		cls,(LPCTSTR)pUser->m_name,divfn);
	t.Format("<span class=\"%s\" id=\"spanUser\" user=\"%s\" %s >",
		cls,(LPCTSTR)pUser->m_name,spfn);
	t += pUser->m_name;
	t += "</span>";
	s += t;
	m_ts.AddHtmlAtom(s);
	m_ts.AddHtmlAtom("&nbsp;:&nbsp;");
	m_ts << m_pvo->TxtEmote();
	m_ts.Append(ts);
	m_ts.AddHtmlAtom("</div>");
	m_ts << Flush;
	m_ts.ScrollText();

}
void CYahooMsgHtmlView::DisplayThink(CUser *pUser,CTextStream &ts)
{
	CString msg;
	CString s;
	CString t;
	LPCTSTR cls;	// class name
	LPCTSTR divfn;
	LPCTSTR spfn;

	m_ts.Reset();
	m_ts.SetDefaults("tahoma",12,0);
	//m_ts.AddDefaults();

	divfn =  "onmouseover=\"OnDivOver(this)\" onmouseout=\"OnDivOut(this)\" " \
		" onclick=\"OnDivClick(this)\" ondblclk=\"OnDivDblClk(this)\" "\
		" oncontextmenu=\"OnDivRClick(this)\" ";

	spfn =  "onmouseover=\"OnUserOver(this)\" onmouseout=\"OnUserOut(this)\" " \
		" onclick=\"OnUserClick(this)\" ondblclk=\"OnUserDblClk(this)\" "\
		" oncontextmenu=\"OnUserRClick(this)\" ";

	if(pUser->m_me)
		cls = "me";
	else if(pUser->m_buddy)
		cls = "buddy";
	else
		cls = "user";

	s.Format("<div class=\"%s\" id=\"divUser\" user=\"%s\" %s >",
		cls,(LPCTSTR)pUser->m_name,divfn);
	ATLTRACE("%s\n",(LPCTSTR)s);
	t.Format("<span class=\"%s\" id=\"spanUser\" user=\"%s\" %s >",
		cls,(LPCTSTR)pUser->m_name,spfn);
	t += pUser->m_name;
	t += "</span>";
	s += t;
	m_ts.AddHtmlAtom(s);
	m_ts.AddHtmlAtom("&nbsp;:&nbsp;");
	m_ts << m_pvo->TxtThought();
	m_ts.Append(ts);
	m_ts.AddHtmlAtom("</div>");
#ifdef _DEBUG
	{
		HANDLE hFile = CreateFile("debug_bubble.txt",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,
			NULL);

		if(hFile){
			CString tmp;
			m_ts.GetHtml(tmp);
			LPCTSTR p = tmp;
			DWORD dummy;
			WriteFile(hFile,(LPCVOID)p,lstrlen(p),&dummy,NULL);
			CloseHandle(hFile);
		}
	}
#endif
	m_ts << Flush;
	m_ts.ScrollText();
}

void CYahooMsgHtmlView::DisplayPm(LPCTSTR to,CUser *pUser,CTextStream &ts,LPCTSTR strtime)
{
	CString msg;
	CString s;
	CString t;
	LPCTSTR cls;	// class name
	LPCTSTR divfn;
	LPCTSTR spfn;

	m_ts.Reset();
	m_ts.SetDefaults("tahoma",10,0);

	divfn =  "onmouseover=\"OnDivOver(this)\" onmouseout=\"OnDivOut(this)\" " \
		" onclick=\"OnDivClick(this)\" ondblclk=\"OnDivDblClk(this)\" "\
		" oncontextmenu=\"OnDivRClick(this)\" ";

	spfn =  "onmouseover=\"OnUserOver(this)\" onmouseout=\"OnUserOut(this)\" " \
		" onclick=\"OnUserClick(this)\" ondblclk=\"OnUserDblClk(this)\" "\
		" oncontextmenu=\"OnUserRClick(this)\" ";

	if(pUser->m_me)
		cls = "me";
	else if(pUser->m_buddy)
		cls = "buddy";
	else
		cls = "user";

	s.Format("<div class=\"%s\" id=\"divUser\" user=\"%s\" %s >",
		"pm",(LPCTSTR)pUser->m_name,divfn);
	ATLTRACE("%s\n",(LPCTSTR)s);
	t.Format("<span class=\"%s\" id=\"spanUser\" user=\"%s\" %s >",
		cls,(LPCTSTR)pUser->m_name,spfn);
	t += pUser->m_name;
	t += "</span>";
	s += t;
	m_ts.AddHtmlAtom(s);
	if(strtime && lstrlen(strtime)){
		m_ts << InlineMode << " [" << strtime << "] ";
	}
	m_ts << " tells you in pm: ";
	m_ts.Append(ts);
	
	// add button to answer
	t.Empty();
	t.Format("&nbsp;&nbsp;<input align=\"right\" type=\"button\" value=\"answer\" user=\"%s\" onclick=\"OnAnswerPm(this)\" />",
		(LPCTSTR)pUser->m_name);
	t += "</div>";
	m_ts.AddHtmlAtom(t);
	m_ts << Flush;
	m_ts.ScrollText();
}
/*
void CYahooMsgHtmlView::DisplayInvited(CUser *pUser,LPCTSTR room,LPCTSTR rmid,LPCTSTR msg)
{
	//
}
void CYahooMsgHtmlView::DisplayRoomFull(LPCTSTR alt_room,LPCTSTR room,LPCTSTR rmid)
{
	//
}
*/


void CYahooMsgHtmlView::OnOptionsChanged(void)
{
	CChatViewBase::OnOptionsChanged();

	// todo: we dont receive information about which option has changed
	// so we will change all options
	UpdateVisualOptions();
}

void CYahooMsgHtmlView::UpdateVisualOptions(void)
{
	HRESULT hr;
	FontAll f;
	CComVariant v;
	CString s;
	const int high = 64;

	// background color
	hr = m_edit.PutBgColor(m_pvo->ClrBack());
	ATLASSERT(SUCCEEDED(hr));
	// name user
	f = m_pvo->NameUser();
	hr = m_edit.m_doc.ChangeStyleFont("span.user",f.m_face);
	hr = m_edit.m_doc.ChangeStyleFont("span.userOver",f.m_face);
	s.Empty();
	s.Format("%dpt",f.m_size);
	v = (LPCTSTR)s;
	hr = m_edit.m_doc.ChangeStyleFontSize("span.user",v);
	hr = m_edit.m_doc.ChangeStyleFontSize("span.userOver",v);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBold("span.user",f.m_bold ? "bold" : "normal");
	hr = m_edit.m_doc.ChangeStyleBold("span.userOver",f.m_bold ? "bold" : "normal");
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBkColor("span.user",f.m_clrBack);
	hr = m_edit.m_doc.ChangeStyleBkColor("span.userOver",f.m_clrBack);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleColor("span.user",f.m_clr);
	hr = m_edit.m_doc.ChangeStyleColor("span.userOver",HighlightColor(f.m_clr,high));
	ATLASSERT(SUCCEEDED(hr));
	// todo: italic and underline
	// name buddy
	f = m_pvo->NameBuddy();
	hr = m_edit.m_doc.ChangeStyleFont("span.buddy",f.m_face);
	hr = m_edit.m_doc.ChangeStyleFont("span.buddyOver",f.m_face);
	s.Empty();
	s.Format("%dpt",f.m_size);
	v = (LPCTSTR)s;
	hr = m_edit.m_doc.ChangeStyleFontSize("span.buddyOver",v);
	hr = m_edit.m_doc.ChangeStyleFontSize("span.buddyOver",v);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBold("span.buddy",f.m_bold ? "bold" : "normal");
	hr = m_edit.m_doc.ChangeStyleBold("span.buddyOver",f.m_bold ? "bold" : "normal");
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBkColor("span.buddy",f.m_clrBack);
	hr = m_edit.m_doc.ChangeStyleBkColor("span.buddyOver",f.m_clrBack);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleColor("span.buddy",f.m_clr);
	hr = m_edit.m_doc.ChangeStyleColor("span.buddyOver",HighlightColor(f.m_clr,high));
	// name me
	f = m_pvo->NameMe();
	hr = m_edit.m_doc.ChangeStyleFont("span.me",f.m_face);
	hr = m_edit.m_doc.ChangeStyleFont("span.meOver",f.m_face);
	s.Empty();
	s.Format("%dpt",f.m_size);
	v = (LPCTSTR)s;
	hr = m_edit.m_doc.ChangeStyleFontSize("span.me",v);
	hr = m_edit.m_doc.ChangeStyleFontSize("span.meOver",v);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBold("span.me",f.m_bold ? "bold" : "normal");
	hr = m_edit.m_doc.ChangeStyleBold("span.meOver",f.m_bold ? "bold" : "normal");
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBkColor("span.me",f.m_clrBack);
	hr = m_edit.m_doc.ChangeStyleBkColor("span.meOver",f.m_clrBack);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleColor("span.me",f.m_clr);
	hr = m_edit.m_doc.ChangeStyleColor("span.meOver",HighlightColor(f.m_clr,high));
	ATLASSERT(SUCCEEDED(hr));
	// text user
	f = m_pvo->TxtUser();
	hr = m_edit.m_doc.ChangeStyleFont("div.user",f.m_face);
	hr = m_edit.m_doc.ChangeStyleFont("div.userOver",f.m_face);
	s.Empty();
	s.Format("%dpt",f.m_size);
	v = (LPCTSTR)s;
	hr = m_edit.m_doc.ChangeStyleFontSize("div.user",v);
	hr = m_edit.m_doc.ChangeStyleFontSize("div.userOver",v);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBold("div.user",f.m_bold ? "bold" : "normal");
	hr = m_edit.m_doc.ChangeStyleBold("div.userOver",f.m_bold ? "bold" : "normal");
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBkColor("div.user",f.m_clrBack);
	hr = m_edit.m_doc.ChangeStyleBkColor("div.userOver",f.m_clrBack);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleColor("div.user",f.m_clr);
	hr = m_edit.m_doc.ChangeStyleColor("div.userOver",f.m_clr);
	ATLASSERT(SUCCEEDED(hr));
	// todo: italic and underline
	// text buddy
	f = m_pvo->TxtBuddy();
	hr = m_edit.m_doc.ChangeStyleFont("div.buddy",f.m_face);
	hr = m_edit.m_doc.ChangeStyleFont("div.buddyOver",f.m_face);
	s.Empty();
	s.Format("%dpt",f.m_size);
	v = (LPCTSTR)s;
	hr = m_edit.m_doc.ChangeStyleFontSize("div.buddy",v);
	hr = m_edit.m_doc.ChangeStyleFontSize("div.buddyOver",v);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBold("div.buddy",f.m_bold ? "bold" : "normal");
	hr = m_edit.m_doc.ChangeStyleBold("div.buddyOver",f.m_bold ? "bold" : "normal");
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBkColor("div.buddy",f.m_clrBack);
	hr = m_edit.m_doc.ChangeStyleBkColor("div.buddyOver",f.m_clrBack);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleColor("div.buddy",f.m_clr);
	hr = m_edit.m_doc.ChangeStyleColor("div.buddyOver",f.m_clr);
	ATLASSERT(SUCCEEDED(hr));
	// todo: italic and underline
	// text me
	f = m_pvo->TxtMe();
	hr = m_edit.m_doc.ChangeStyleFont("div.me",f.m_face);
	hr = m_edit.m_doc.ChangeStyleFont("div.meOver",f.m_face);
	s.Empty();
	s.Format("%dpt",f.m_size);
	v = (LPCTSTR)s;
	hr = m_edit.m_doc.ChangeStyleFontSize("div.me",v);
	hr = m_edit.m_doc.ChangeStyleFontSize("div.meOver",v);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBold("div.me",f.m_bold ? "bold" : "normal");
	hr = m_edit.m_doc.ChangeStyleBold("div.meOver",f.m_bold ? "bold" : "normal");
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBkColor("div.me",f.m_clrBack);
	hr = m_edit.m_doc.ChangeStyleBkColor("div.meOver",f.m_clrBack);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleColor("div.me",f.m_clr);
	hr = m_edit.m_doc.ChangeStyleColor("div.meOver",f.m_clr);
	ATLASSERT(SUCCEEDED(hr));
	// todo: italic and underline
	// text pm
	f = m_pvo->TxtPvt();
	hr = m_edit.m_doc.ChangeStyleFont("div.pm",f.m_face);
	hr = m_edit.m_doc.ChangeStyleFont("div.pmOver",f.m_face);
	s.Empty();
	s.Format("%dpt",f.m_size);
	v = (LPCTSTR)s;
	hr = m_edit.m_doc.ChangeStyleFontSize("div.pm",v);
	hr = m_edit.m_doc.ChangeStyleFontSize("div.pmOver",v);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBold("div.pm",f.m_bold ? "bold" : "normal");
	hr = m_edit.m_doc.ChangeStyleBold("div.pmOver",f.m_bold ? "bold" : "normal");
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleBkColor("div.pm",f.m_clrBack);
	hr = m_edit.m_doc.ChangeStyleBkColor("div.pmOver",f.m_clrBack);
	ATLASSERT(SUCCEEDED(hr));
	hr = m_edit.m_doc.ChangeStyleColor("div.pm",f.m_clr);
	hr = m_edit.m_doc.ChangeStyleColor("div.pmOver",f.m_clr);
	ATLASSERT(SUCCEEDED(hr));
}


HRESULT CMyDocHostUIHandler::ShowContextMenu(/* [in] */ DWORD dwID,
            /* [in] */ DWORD x,
            /* [in] */ DWORD y,
            /* [in] */ IUnknown *pcmdtReserved,
            /* [in] */ IDispatch  *pdispReserved,
            /* [retval][out] */ HRESULT  *dwRetVal)
{
	switch(dwID){
		case CONTEXT_MENU_TEXTSELECT:
			*dwRetVal = S_FALSE;
			break;
		default:
			*dwRetVal = S_OK;	// show our own
			break;
	}
	return S_OK;
}


// CYahooMsgEditView 

CYahooMsgEditView::CYahooMsgEditView(void):_BaseClass(m_ts)
{
	
}

CYahooMsgEditView::~CYahooMsgEditView()
{
	
}


LRESULT CYahooMsgEditView::OnCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes;

	lRes = _BaseClass::OnCreate(WM_CREATE,wParam,lParam,bHandled);
	bHandled = TRUE;

	m_edit.Create(\
		m_hWnd,
		rcDefault,
		NULL,
		WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_BORDER|ES_AUTOVSCROLL|ES_MULTILINE,
		WS_EX_CLIENTEDGE);

	ATLASSERT(m_edit.IsWindow());
	m_edit.SetFont(AtlGetDefaultGuiFont());
	m_ts.SetEdit(m_edit);


	BOOL res = m_pChat->Init(m_pgo->GetServerYmsg(),m_pgo->GetPortYmsg(),m_pao->GetName(),
		m_pao->GetPassword());
	ATLASSERT(res);

	return lRes;
}

LRESULT CYahooMsgEditView::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_edit.MoveWindow(0,0,LOWORD(lParam),HIWORD(lParam));
	return 0;
}

LRESULT CYahooMsgEditView::OnFileSaveAs(WORD,WORD,HWND,BOOL &bHandled)
{
	CFileDialog dlg(\
		FALSE,		// file open
		"ddf",		// default extension
		NULL,		// file name
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,		// flags
		"text files\0*.txt\0All files\0*.*\0",	// filter
		m_hWnd);	// parent

	if(IDOK == dlg.DoModal()){
		NO5TL::CWinFile wf;
		BOOL res;

		if(res = wf.Create(dlg.m_szFileName,CREATE_ALWAYS,GENERIC_WRITE)){
			int len = m_edit.GetWindowTextLength();

			if(len > 0){
				DWORD dwWritten;
				char *p = new char[len + 1];
				ZeroMemory(p,len + 1);
				m_edit.GetWindowText(p,len+1);
				res = wf.Write(p,len,&dwWritten);
				delete []p;
			}
		}
	}

	return 0;
}

void CYahooMsgEditView::DisplayUserName(CUser *pUser)
{
	m_ts << LPCTSTR(pUser->m_name);
}

void CYahooMsgEditView::DisplayChatText(CUser *pUser,CTextStream &ts)
{
	InitTS its(m_ts);

	DisplayUserName(pUser);
	m_ts.Append(ts);
}

CString GetAwayString(UINT i)
{
	const UINT first = IDS_AWAY_AVAILABLE;
	UINT id;
	bool ok = true;
	CString s;

	switch(i){
		case YAHOO_STATUS_AVAILABLE:
		case YAHOO_STATUS_BRB:
		case YAHOO_STATUS_BUSY:
		case YAHOO_STATUS_NOTATHOME:
		case YAHOO_STATUS_NOTATDESK:
		case YAHOO_STATUS_NOTINOFFICE:
		case YAHOO_STATUS_ONPHONE:
		case YAHOO_STATUS_ONVACATION:
		case YAHOO_STATUS_OUTTOLUNCH:
		case YAHOO_STATUS_STEPPEDOUT:
			id = first + ( i - YAHOO_STATUS_AVAILABLE);
			break;
		case YAHOO_STATUS_INVISIBLE:
			id = IDS_AWAY_INVISIBLE;
			break;
		case YAHOO_STATUS_CUSTOM:
			id = IDS_AWAY_CUSTOM;
			break;
		case YAHOO_STATUS_IDLE:
			id = IDS_AWAY_IDLE;
			break;
		case YAHOO_STATUS_ONLINE:
			id = IDS_AWAY_ON;
			break;
		case YAHOO_STATUS_OFFLINE:
			id = IDS_AWAY_OFF;
			break;
		case YAHOO_STATUS_TYPING:
			id = IDS_AWAY_TYPE;
			break;
		default:
			ok = false;
			break;
	}
	if(ok){
		s.LoadString(id);
	}
	return s;
}
