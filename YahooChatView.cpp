// YahooChatView.cpp: implementation of the CYahooChatRichView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ctime>
#include <no5tl\socket.h>
#include "YahooChatView.h"
#include "no5options.h"
#include "iuserlistview.h"
#include "imainframe.h"
#include "commandparser.h"
#include "no5app.h"
#include "colorfile.h"
#include "privatebox.h"
#include "resource.h"
#include "recentrooms.h"
#include "yahootxtobj.h"
#include "voicedialog.h"
#ifdef _DEBUG
#include <no5tl\winfile.h>
#endif
//#include "MyUtf8.h"


// fuck for some reason even using this some objects need to be declared with NO5TL
// like NO5TL::CHtmlElement
using namespace NO5TL;
using namespace NO5TL::Colors;
using namespace NO5YAHOO;
class CYahooMsgViewBase;


IVisualOptions * CChatViewBase::m_pvo = NULL;
IGeneralOptions * CChatViewBase::m_pgo = NULL;
int CChatViewBase::m_count = 0;
IMainFrame * CChatViewBase::m_pFrame = NULL;
IIgnoreList * CChatViewBase::m_pIgnoreList = NULL;
CPmOptions * CChatViewBase::m_pPmOptions = NULL;

#ifdef _DEBUG
void PrintUser(CUser *pUser,CString &s)
{
	s.Format(\
		"posts dt: %lu chat posts: %lu  links: %lu dt=%lu repeat=%lu time=%lu",
		pUser->m_posts_dt,pUser->m_posts,pUser->m_dwLinks,
		(pUser->m_dwLastAction - pUser->m_tStart),
		pUser->m_repeated,
		(pUser->m_dwLastAction - pUser->m_tEnter)/1000);
}
#endif


HRESULT CreateChatObj(CChatViewBase *pView,IDispatch **ppObj)
{
	CComObject<CChatObj> *pObj = NULL;
	HRESULT hr = CComObject<CChatObj>::CreateInstance(&pObj);

	if(SUCCEEDED(hr)){
		if(pView->IsYchtView())
			hr = pObj->Init((CYahooChatViewBase *)pView);
		else
			hr = pObj->Init((CYahooMsgViewBase *)pView);
		if(SUCCEEDED(hr)){
			hr = pObj->QueryInterface(ppObj);
		}
	}
	if(FAILED(hr) && pObj)
		delete pObj;
	return hr;
}

static HRESULT CreateTxtObj(CTextStream *pts,IDispatch **ppObj)
{
	CComObject<CYahooTxtObj> *pObj = NULL;
	HRESULT hr = CComObject<CYahooTxtObj>::CreateInstance(&pObj);

	if(SUCCEEDED(hr)){
		hr = pObj->Init(pts);
		if(SUCCEEDED(hr)){
			hr = pObj->QueryInterface(ppObj);
		}
	}
	if(FAILED(hr) && pObj)
		delete pObj;
	return hr;
}


HRESULT CreateUserObj(CUser *pUser,IDispatch **ppObj)
{
	CComObject<CUserObj> *pObj = NULL;
	HRESULT hr = CComObject<CUserObj>::CreateInstance(&pObj);

	if(SUCCEEDED(hr)){
		pObj->Init(*pUser);
		hr = pObj->QueryInterface(ppObj);
	}
	if(FAILED(hr) && pObj)
		delete pObj;
	return hr;
}


// InitTS

InitTS::InitTS(CTextStream &ts,bool bEndl ):m_ts(ts),m_bEndl(bEndl)
{
	m_ts.Reset();
	m_ts << YahooMode;
	m_ts.SetDefaults("Tahoma",10,0x000000);
	m_ts.AddDefaults();
	m_ts << Bold(FALSE) << Italic(FALSE) << Under(FALSE);
}

InitTS::~InitTS()
{
	if(m_bEndl)
		m_ts << Endl;
	else
		m_ts << Flush;
	m_ts.ScrollText();
}


// CChatViewBase

CChatViewBase::CChatViewBase(CTextStream &ts):m_ts(ts)
{
	if(!m_count){
		if(!m_pIgnoreList)
			m_pIgnoreList = g_app.GetIgnoreList();
		if(!m_pgo)
			m_pgo = g_app.GetGeneralOptions();
		if(!m_pvo)
			m_pvo = g_app.GetVisualOptions();
		if(!m_pFrame)
			m_pFrame = g_app.GetMainFrame();
		if(!m_pPmOptions){
			m_pPmOptions = new CPmOptions();
			m_pgo->GetPmOptions(*m_pPmOptions);
		}
		if(!g_app.GetRecentRooms()){
			g_app.LoadRecentRooms();
		}
	}
	m_count++;
	m_bActive = false;
	m_pao = NULL;
	m_bMimicAll = false;
	m_bKnock = false;
	m_knocks = 0;
}

CChatViewBase::~CChatViewBase()
{
	--m_count;
	ATLASSERT(m_count >= 0);
	if(!m_count){
		if(m_pPmOptions){
			delete m_pPmOptions;
			m_pPmOptions = NULL;
		}
	}
	if(m_pao)
		delete m_pao;
	ATLTRACE("~CChatViewBase\n");
};

BOOL CChatViewBase::DispatchCmd()
{

	BOOL res = FALSE;

	if(!m_cmd.CompareNoCase("join")){
		res = OnCmdJoin();
	}
	else if(!m_cmd.CompareNoCase("pm")){
		res = OnCmdPm();
	}
	else if(!m_cmd.CompareNoCase("tell")){
		res = OnCmdPm();
	}
	else if(!m_cmd.CompareNoCase("away")){
		res = OnCmdAway();
	}
	else if(!m_cmd.CompareNoCase("back")){
		res = OnCmdBack();
	}
	else if(!m_cmd.CompareNoCase("leave")){
		res = OnCmdLeave();
	}
	else if(!m_cmd.CompareNoCase("goto")){
		res = OnCmdGoto();
	}
	else if(!m_cmd.CompareNoCase("think")){
		res = OnCmdThink();
	}
	else if(!m_cmd.CompareNoCase("emote")){
		res = OnCmdEmote();
	}
	else if(!m_cmd.CompareNoCase("mimic")){
		res = OnCmdMimic();
	}
	else if(!m_cmd.CompareNoCase("ignore")){
		res = OnCmdIgnore();
	}
	else if(!m_cmd.CompareNoCase("follow")){
		res = OnCmdFollow();
	}
	else if(!m_cmd.CompareNoCase("invite")){
		res = OnCmdInvite();
	}
	else if(!m_cmd.CompareNoCase("knock")){
		res = OnCmdKnock();
	}
	else if(!m_cmd.CompareNoCase("noknock")){
		res = OnCmdNoKnock();
	}
	else if(!m_cmd.CompareNoCase("bud")){
		res = OnCmdBud();
	}
	else if(!m_cmd.CompareNoCase("viewcam")){
		res = OnCmdViewcam();
	}
	else if(!m_cmd.CompareNoCase("bot")){
		res = OnCmdBot();
	}
	else {
		res = OnCmdUnknown();
	}
	return res;
}


BOOL CChatViewBase::LoadColorSet(void)
{
	CString name = m_input_params.colorset;
	BOOL res = TRUE;

	if( ( !name.IsEmpty() ) && ( name.CompareNoCase("random") != 0 )){
		IColorFile *p = NULL;
		if(res = ColorFileCreate(&p)){
			if(res = p->SetFileName(g_app.GetColorsFile())){
				m_colors.RemoveAll();
				res = p->GetColorSet(name,m_colors);
			}
			ColorFileDestroy(&p);
		}
	}
	else if(!name.CompareNoCase("random")){
		CYahooColor clr;
		int i;
		CColor color;

		srand(GetTickCount());
		m_colors.RemoveAll();
		for(i=0;i<5;i++){
			clr = color.Random();
			m_colors.Add(clr);
		}
	}
	return res;
}

BOOL CChatViewBase::LoadInputParams(void)
{
	BOOL res = FALSE;

	if(m_pao){
		// we initialize in case we dont have anything in the file
		FontAll f(_T("arial"),10,0,0xFFFFFF,true,false,false);

		m_pao->GetFontOptions(f);
		m_input_params.bold = f.m_bold;
		m_input_params.italic = f.m_italic;
		m_input_params.underline = f.m_under;
		m_input_params.font = f.m_face;
		m_input_params.color = f.m_clr;
		m_input_params.font_size = f.m_size;

		m_input_params.fade = m_pao->GetFade() ? true : false;
		m_input_params.blend = m_pao->GetAlt() ? true : false;
		m_input_params.colorset = m_pao->GetColorSetName();

		if(m_input_params.colorset.IsEmpty())
			m_input_params.colorset = _T("random");

		res = TRUE;

	}
	return res;
}

BOOL CChatViewBase::SaveInputParams(void)
{
	BOOL res = FALSE;

	if(m_pao){
		FontAll font;

		font.m_bold = m_input_params.bold;
		font.m_italic = m_input_params.italic;
		font.m_under = m_input_params.underline;
		font.m_clr = m_input_params.color;
		font.m_face = m_input_params.font;
		font.m_size = m_input_params.font_size;
		m_pao->SetFontOptions(font);
		m_pao->SetColorSetName(m_input_params.colorset);
		m_pao->SetFade(m_input_params.fade ? TRUE : FALSE);
		m_pao->SetAlt(m_input_params.blend ? TRUE: FALSE);

		res = TRUE;
	}
	return res;
}

// every view derives from IInputFrameEvents, this is one of its methods.
void CChatViewBase::OnColorsetChange(LPCTSTR name)
{
	m_input_params.colorset = name;
	BOOL res = LoadColorSet();
	ATLASSERT(res);
}

BOOL CChatViewBase::OnAutoComplete(LPCTSTR pszPartial,CStringArray &arr)
{
	int i;
	const int count = m_users.GetSize();
	CUser *p = NULL;
	int idx = -1;

	ATLTRACE("autocomplete for: %s\n",pszPartial);
	for(i=0;i<count;i++){
		p = m_users.GetUser(i);
		idx = StringFindNoCase(p->m_name,pszPartial,0);
		if(idx == 0){
			arr.Add(p->m_name);
		}
	}
	return arr.GetSize() > 0 ? TRUE : FALSE;
}

void CChatViewBase::DoIgnoreUser(LPCTSTR name,bool bIgnore)
{
	CUser *pUser;

	if(bIgnore){
		const long days = g_app.GetIgnoreOptions().m_days_perm;
		long expire;

		// calculate the expiration time based on the options
		if(days < 0)
			expire = -1;
		else{
			expire = static_cast<long>(time(NULL)) + ( days * 24 * 60 * 60 );
		}
		m_pIgnoreList->add(name,expire);
	}
	else{
		m_pIgnoreList->remove(name);
	}

	if(pUser = m_users.GetUser(name)){
		pUser->m_ignored = bIgnore;

		if(pUser->m_chat && m_bActive){
			if(bIgnore){
				m_pFrame->UserListView()->RemoveUser(name);
				m_pFrame->QuickIgnoreView()->AddUser(name);
			}
			else{
				m_pFrame->UserListView()->AddUser(pUser);
				m_pFrame->QuickIgnoreView()->RemoveUser(name);
			}
		}
	}

	CPrivateBox *pm = m_pms.Find(name);
	if(pm){
		pm->UpdateUser();
	}
	DisplayIgnoreMsg(name,bIgnore);

	// script
	if(m_spChat && pUser && g_app.RunningScripts())
	{
		CComPtr<IDispatch> spUser;
		HRESULT hr = CreateUserObj(pUser,&spUser);
		
		if(SUCCEEDED(hr)){
			g_app.GetNo5Obj()->Fire_OnUserIgnored(m_spChat,spUser,bIgnore ? VARIANT_TRUE : VARIANT_FALSE);
		}
	}

}

void CChatViewBase::DoAutoIgnoreUser(LPCTSTR name,bool bIgnore,LPCTSTR reason)
{
	CUser *pUser;
	CIgnoreOptions &io = g_app.GetIgnoreOptions();

	if(!io.m_auto || IsMe(name) || IsBuddy(name))
		return;

	if(bIgnore){
		const long days = g_app.GetIgnoreOptions().m_days;
		long expire;

		// calculate the expiration time based on the options
		if(days < 0)
			expire = -1;
		else{
			expire = static_cast<long>(time(NULL)) + ( days * 24 * 60 * 60 );
		}
		m_pIgnoreList->add(name,expire);
	}
	else{
		m_pIgnoreList->remove(name);
	}

	if(pUser = m_users.GetUser(name)){
		pUser->m_ignored = bIgnore;

		if(pUser->m_chat && m_bActive){
			if(bIgnore){
				m_pFrame->UserListView()->RemoveUser(name);
				m_pFrame->QuickIgnoreView()->AddUser(name);
			}
			else{
				m_pFrame->UserListView()->AddUser(pUser);
				m_pFrame->QuickIgnoreView()->RemoveUser(name);
			}
		}
	}

	CPrivateBox *pm = m_pms.Find(name);
	if(pm){
		pm->UpdateUser();
	}
	DisplayAutoIgnoreMsg(name,bIgnore,reason);
	// script
	if(m_spChat && pUser && g_app.RunningScripts())
	{
		CComPtr<IDispatch> spUser;
		HRESULT	hr = CreateUserObj(pUser,&spUser);
		if(SUCCEEDED(hr)){
			g_app.GetNo5Obj()->Fire_OnUserIgnored(m_spChat,spUser,bIgnore ? VARIANT_TRUE : VARIANT_FALSE);
		}
	}
}

void CChatViewBase::DoAutoIgnoreUser(LPCTSTR name,bool bIgnore,UINT uRes)
{
	CString s;
	s.LoadString(uRes);
	DoAutoIgnoreUser(name,bIgnore,s);
}

void CChatViewBase::DoMimicUser(LPCTSTR name,bool bMimic)
{
	CUser *pUser = m_users.GetUser(name);

	if(!pUser){
		pUser = AddUser(name);
	}
	pUser->m_mimic = bMimic;
	if(pUser->m_chat && m_bActive)
		m_pFrame->UserListView()->UpdateUser(pUser);
	CPrivateBox *pm = m_pms.Find(name);
	if(pm){
		pm->UpdateUser();
	}

	DisplayMimicMsg(name,bMimic);
}

void CChatViewBase::DoFollowUser(LPCTSTR name,bool bFollow)
{
	CUser *pUser = m_users.GetUser(name);

	if(!pUser){
		pUser = AddUser(name);
	}
	pUser->m_follow = bFollow;
	if(pUser->m_chat && m_bActive)
		m_pFrame->UserListView()->UpdateUser(pUser);
	CPrivateBox *pm = m_pms.Find(name);
	if(pm){
		pm->UpdateUser();
	}
	DisplayFollowMsg(name,bFollow);
}

	// IUserListViewEvents
void CChatViewBase::OnUserList_Ignored(LPCTSTR name)
{
	DoIgnoreUser(name,!m_pIgnoreList->find(name));
}

void CChatViewBase::OnUserList_Profile(LPCTSTR name)
{
	CString url = _T("http://profiles.yahoo.com/");
	url += name;
	OpenBrowser(url,false);
}
void CChatViewBase::OnUserList_Mimic(LPCTSTR name)
{
	CUser *pUser = m_users.GetUser(name);
	ATLASSERT(pUser);
	if(pUser)
		DoMimicUser(name,!pUser->m_mimic);
}
void CChatViewBase::OnUserList_Pm(LPCTSTR name)
{
	CPrivateBox *pBox = GetPrivateBox(name);

	if(pBox){
		pBox->ShowPm();
	}
}


void CChatViewBase::OnUserList_Follow(LPCTSTR name)
{
	CUser *pUser = m_users.GetUser(name);
	ATLASSERT(pUser);
	if(pUser)
		DoFollowUser(name,!pUser->m_follow);
}

	// IPrivateBoxEvents
void CChatViewBase::pm_OnUserIgnored(LPCTSTR name)
{
	DoIgnoreUser(name,!m_pIgnoreList->find(name));
}

void CChatViewBase::pm_OnUserMimic(LPCTSTR name)
{
	CUser *pUser = m_users.GetUser(name);
	ATLASSERT(pUser);
	
	if(pUser)
		DoMimicUser(name,!pUser->m_mimic);
}

void CChatViewBase::pm_OnUserFollow(LPCTSTR name)
{
	CUser *pUser = m_users.GetUser(name);
	ATLASSERT(pUser);
	if(pUser)
		DoFollowUser(name,!pUser->m_follow);
}

void CChatViewBase::pm_OnUserProfile(LPCTSTR name)
{
	OnUserList_Profile(name);
}

void CChatViewBase::pm_OnDestroy(LPCTSTR name,CPrivateBox *pm)
{
	BOOL res = m_pms.RemoveBox(name);
	ATLASSERT(res);

	CUser *pUser = m_users.GetUser(name);

	if(pUser)
		pUser->m_pm = false;
}


BOOL CChatViewBase::OnCmdMimic()
{
	CUser *pUser;
	CString name;

	if(m_args.GetSize() == 0){
		const int size = m_users.GetSize();
		for(int i=0;i<size;i++){
			pUser = m_users.GetUser(i);
			if(pUser && pUser->m_mimic)
				DoMimicUser(pUser->m_name,false);
		}
		m_bMimicAll = false;
	}
	else if(GetArgs1(name)){
		if(name[0] == '*')
			m_bMimicAll = true;
		else{
			pUser = m_users.GetUser(name);

			if(!pUser){
				pUser = AddUser(name);
			}
			DoMimicUser(name,!pUser->m_mimic);
		}
	}
	return TRUE;
}

BOOL CChatViewBase::OnCmdFollow()
{
	CUser *pUser;
	CString name;

	if(!m_args.GetSize()){
		const int size = m_users.GetSize();

		for(int i=0;i<size;i++){
			pUser = m_users.GetUser(i);
			if(pUser->m_follow)
				DoFollowUser(pUser->m_name,false);
		}
	}
	else if(GetArgs1(name)){
		pUser = m_users.GetUser(name);
		if(!pUser){
			pUser = AddUser(name);
		}
		DoFollowUser(name,!pUser->m_follow);
	}
	return TRUE;
}

BOOL CChatViewBase::OnCmdIgnore()
{
	CString name;
	if(GetArgs1(name)){
		bool ignore;

		ignore = ! m_pIgnoreList->find(name);
		DoIgnoreUser(name,ignore);
	}
	return TRUE;
}

BOOL CChatViewBase::OnCmdKnock()
{
	CWindow wnd = GetHandle();
	BOOL res = TRUE;
	if(m_bKnock){
		// stop knocking
		res = wnd.KillTimer(TIMER_KNOCK);
		ATLASSERT(res);
		if(res){
			m_bKnock = FALSE;
			m_knocks = 0;
		}
	}
	if(res){
		res = wnd.SetTimer(TIMER_KNOCK,1000);
		ATLASSERT(res);
		if(res){
			m_bKnock = TRUE;
		}
	}
	return TRUE;
}

BOOL CChatViewBase::OnCmdNoKnock()
{
	CWindow wnd = GetHandle();
	BOOL res = TRUE;
	if(m_bKnock){
		// stop knocking
		res = wnd.KillTimer(TIMER_KNOCK);
		ATLASSERT(res);
		if(res){
			m_bKnock = FALSE;
			m_knocks = 0;
		}
	}
	return TRUE;
}

BOOL CChatViewBase::OnCmdBot()
{
	CString name;
	BOOL res;
	m_BotTrigger.Empty();

	res = GetArgs2(name,m_BotTrigger);
	if(!res)
		res = GetArgs1(name);
	if(res){
		res = LoadBot(name);
		if(!res){
			MessageBox(GetHandle(),"failed to load bot",g_app.GetName(),MB_ICONINFORMATION);
		}
	}
	else{
		m_spBot.Release();
	}
	return TRUE;
}

void CChatViewBase::OnOptionsChanged(void)
{
	BOOL res = m_pgo->GetPmOptions(*m_pPmOptions);
	ATLASSERT(res);

	CUser::MAX_PPS = g_app.GetIgnoreOptions().m_pps;
	CUser::MAX_PP2S = ( CUser::MAX_PPS + 1 );
}

// checks if the user should be auto ignored based on current options
bool CChatViewBase::TestAutoIgnore(CUser *pUser,CTextStream &ts,ULONG &uRes)
{
	CIgnoreOptions &io = g_app.GetIgnoreOptions();
	bool res = false;

	if(!( pUser->m_me || pUser->m_buddy) && io.m_auto){
		CString s = ts.GetPlainText2();

		// repeated lines
		if(pUser->m_repeated > 3){
			res = true;
			uRes = IDS_IG_REPEAT;
		}
		// check for certain words posted within a certain interval after joining
		else if(pUser->m_dwLastAction - pUser->m_tEnter < (io.m_tJoin * 1000)){
			if(io.m_pWords){
				const int count = io.m_pWords->GetSize();
				int pos;

				for(int i=0;i<count;i++){
					pos = StringFindNoCase(s,io.m_pWords->operator [](i),0);
					if(pos >= 0){
						res = true;
						uRes = IDS_IG_SPAM;
						break;
					}
				}
			}
		}
		else if(ts.GetBadtagCount() && io.m_badtags){
			res = true;
			uRes = IDS_IG_BADTAG;
		}
	}
	return res;
}

void CChatViewBase::OnQuick_RemoveUser(LPCTSTR name)
{
	DoIgnoreUser(name,false);
}

void CChatViewBase::blv_OnPm(LPCTSTR name)
{
	CPrivateBox *pBox = GetPrivateBox(name);

	if(pBox){
		pBox->ShowPm();
	}
}

void CChatViewBase::blv_OnProfile(LPCTSTR name)
{
	OnUserList_Profile(name);
}

void CChatViewBase::blv_OnChangeView(LPCTSTR name)
{
	m_pFrame->ActivateChatView(name);
}


void CChatViewBase::FillUserListView(void)
{
	m_pFrame->UserListView()->AddUsers(m_users);
}

void CChatViewBase::FillQuickIgnoreView(void)
{
	CUser *p;
	const int count = m_users.GetSize();
	int i;

	m_pFrame->QuickIgnoreView()->RemoveAll();
	for(i=0;i<count;i++){
		p = m_users.GetUser(i);
		if(p->IsIgnored() && p->m_chat)
			m_pFrame->QuickIgnoreView()->AddUser(p->m_name);
	}
}

void CChatViewBase::OnUnignore(BOOL bAll,LPCTSTR user)
{
	CUser *pUser = NULL;

	if(!bAll){
		if(pUser = m_users.GetUser(user)){
			DoIgnoreUser(user,false);
		}
	}
	else{
		const int count = m_users.GetSize();
		int i;

		for(i=0;i<count;i++){
			pUser = m_users.GetUser(i);
			if(pUser->m_ignored){
				if(m_bActive){
					m_pFrame->UserListView()->AddUser(pUser);
					//m_pFrame->QuickIgnoreView()->RemoveUser(name);
				}
				pUser->m_ignored = false;
			}
		}
		if(m_bActive)
			m_pFrame->QuickIgnoreView()->RemoveAll();
	}
}

void CChatViewBase::OpenBrowser(LPCTSTR url,bool bAsk)
{
	g_app.GetMainFrame()->OpenBrowser(url,bAsk);
}

bool CChatViewBase::IsBuddy(LPCTSTR name)
{
	return m_buddies.FindBuddy(name) != NULL;
}

void CChatViewBase::DisplayAutoIgnoreMsg(LPCTSTR name,bool bIgnore,LPCTSTR reason)
{
	CString s;

	if(bIgnore){
		if(reason && lstrlen(reason))
			s.Format("%s ignored ( %s )",name,reason);
		else
			s.Format("%s ignored",name);
	}
	else{
		s.Format("%s removed from ignore");
	}
	m_pFrame->AddMarqueeString(s);
}
void CChatViewBase::DisplayAutoIgnoreMsg(LPCTSTR name,bool bIgnore,UINT uRes)
{
	CString s;

	s.LoadString(uRes);
	DisplayAutoIgnoreMsg(name,bIgnore,s);
}

void CChatViewBase::DisplayIgnoreMsg(LPCTSTR name,bool bIgnore)
{
	CString s;
	s.Format("*** %s %s",name,bIgnore ? "ignored" : "removed from ignore list");
	m_pFrame->AddMarqueeString(s);
}


void CChatViewBase::DisplayMimicMsg(LPCTSTR name,bool bMimic)
{
	CString s;
	s.Format("*** %s %s",name,bMimic ? "added to mimic list" : "removed from mimic list");
	m_pFrame->AddMarqueeString(s);
}

void CChatViewBase::DisplayFollowMsg(LPCTSTR name,bool bFollow)
{
	CString s;
	
	s.Format("%s %s",bFollow ? "following" : "not following",name);
	m_pFrame->AddMarqueeString(s);
}

CUser * CChatViewBase::_OnChatText(LPCTSTR name,LPCTSTR msg,bool utf8)
{
	CUser *pUser = m_users.GetUser(name);
	CInputStream is;
	ULONG uRes;
	
	if(!pUser){		// ghost user
		pUser = AddUser(name);

		pUser->m_chat = true;
		if(m_bActive){
			if(!pUser->IsIgnored())
				m_pFrame->UserListView()->AddUser(pUser);
			else
				m_pFrame->QuickIgnoreView()->AddUser(name);
		}

	}

	if(pUser->IsIgnored())
		return pUser;

	if(pUser->OnAnyPost() || pUser->OnPost()){
		DoAutoIgnoreUser(pUser->m_name,true,IDS_IG_FLOOD);
		return pUser;
	}

	if(!lstrlen(msg))
		return pUser;

	is << msg;

	// retrieves information from it
	pUser->SetLastLine(is.GetPlainText2());
	pUser->m_dwLinks += is.GetLinkCount();
	CInfoTag &inf = is.GetInfoTag();

	// check for auto ignore
	if(TestAutoIgnore(pUser,is,uRes)){
		DoAutoIgnoreUser(pUser->m_name,true,uRes);
		return pUser;
	}

	if(!inf.IsEmpty()){
		pUser->SetInfoTag(inf);
		m_pFrame->UserListView()->UpdateUser(pUser);
	}

	// format and outputs message using the m_ts
	if(!pUser->IsIgnored()){
		DisplayChatText(pUser,is);
	}

	// script
	if(m_spChat && pUser && g_app.RunningScripts())
	{
		CComPtr<IDispatch> spUser;
		HRESULT hr = CreateUserObj(pUser,&spUser);

		if(SUCCEEDED(hr)){
			CComPtr<IDispatch> spTxt;
			hr = CreateTxtObj(&is,&spTxt);
			if(SUCCEEDED(hr)){
				g_app.GetNo5Obj()->Fire_OnChatMsg(m_spChat,spUser,spTxt);
			}
		}
	}
	return pUser;
}

void CChatViewBase::_OnUserEnter(CUser *pUser)
{
	pUser->m_tEnter = GetTickCount();

	if(m_bActive){
		if(!pUser->IsIgnored())
			m_pFrame->UserListView()->AddUser(pUser);
		else
			m_pFrame->QuickIgnoreView()->AddUser(pUser->m_name);
		
		CString s;
		s.Format("%s enters",LPCTSTR(pUser->m_name));
		m_pFrame->AddMarqueeString(s);
	}

	if(pUser->IsIgnored())
		return;

	if(pUser->OnAnyPost()){
		DoAutoIgnoreUser(pUser->m_name,true,IDS_IG_FLOOD);
		return;
	}

	if(m_pgo->ShowEnter()){
		InitTS init(m_ts);
		
		DisplayUserName(pUser);
		if(IsBuddy(pUser->m_name))
			m_ts << m_pvo->TxtEnterBuddy();
		else
			m_ts <<  m_pvo->TxtEnterUsr();
		m_ts << " joined the room.";
	}
	if(m_spChat && !pUser->IsIgnored() && g_app.RunningScripts()){
		CComPtr<IDispatch> spUser;
		HRESULT hr = CreateUserObj(pUser,&spUser);
		if(SUCCEEDED(hr)){
			g_app.GetNo5Obj()->Fire_OnUserEnter(m_spChat,spUser);
		}
	}
}

CUser * CChatViewBase::_OnUserLeave(LPCTSTR name)
{
	CUser *pUser = m_users.GetUser(name);
	
	if(!pUser){		// ghost user
		pUser = AddUser(name);
	}

	// mark user as not in chat
	pUser->m_chat = false;

	// remove user from list view or auto-ignore view
	if(m_bActive){
		if(!pUser->IsIgnored())
			m_pFrame->UserListView()->RemoveUser(name);
		else
			m_pFrame->QuickIgnoreView()->RemoveUser(name);
		CString s;
		s.Format("%s leaves",LPCTSTR(pUser->m_name));
		m_pFrame->AddMarqueeString(s);
	}
	if(!pUser->IsIgnored()){
		if(pUser->OnAnyPost()){
			DoAutoIgnoreUser(pUser->m_name,true,IDS_IG_FLOOD);
		}
		else if(m_pgo->ShowEnter()){
			InitTS init(m_ts);

			DisplayUserName(pUser);

			if(IsBuddy(pUser->m_name))
				m_ts << m_pvo->TxtLeaveBuddy();
			else
				m_ts <<  m_pvo->TxtLeaveUsr();
			m_ts << " left the room.";
		}
	}
	if(m_spChat && !pUser->IsIgnored() && g_app.RunningScripts()){
		CComPtr<IDispatch> spUser;
		HRESULT hr = CreateUserObj(pUser,&spUser);
		if(SUCCEEDED(hr)){
			g_app.GetNo5Obj()->Fire_OnUserLeave(m_spChat,spUser);
		}
	}
	
	// remove user from chat user list
	// m_users.RemoveUser(name);
	/** we dont remove anymore. our list contain not only chat users. but any user
		that has posted,pmed, done something . the list is cleaned up from time to time
	*/
	return pUser;
}

CUser * CChatViewBase::_OnPrivateMsg(LPCSTR from,LPCSTR to,LPCSTR msg,bool utf8,LPCTSTR t)
{
	CUser *pUser = m_users.GetUser(from);
	CInputStream is;
	
	if(!pUser){		
		pUser = AddUser(from);
	}
	if(!pUser)
		return NULL;

	if(pUser->IsIgnored())
		return pUser;

	if(pUser->OnAnyPost() || pUser->OnPm()){
		DoAutoIgnoreUser(pUser->m_name,true,IDS_IG_FLOOD);
		return pUser;
	}

	// parse the message using a CTextStream
	if(utf8)
		is << (LPCTSTR)FromUTF8(msg);
	else
		is << msg;

	// retrieves information from it
	pUser->SetLastLine(is.GetPlainText());
	pUser->m_dwLinks += is.GetLinkCount();

	// check for auto ignore
	ULONG uRes;
	if(TestAutoIgnore(pUser,is,uRes)){
		DoAutoIgnoreUser(pUser->m_name,true,uRes);
	}


	if(!pUser->IsIgnored()){
		CPrivateBox *pBox = m_pms.Find(pUser->m_name);
		if(m_pPmOptions->m_popup || pBox){
			if(!pBox){
				pBox = GetPrivateBox(from);
				pUser->m_pm = true;
			}
			if(pBox){
				pBox->ShowPm();
				pBox->AddMessage(pUser->m_name,msg);
			}
		}
		else
		{
			LPCTSTR p = NULL;
			if(t && lstrlen(t)){
				time_t tt = static_cast<time_t>(atoi(t));
				p = asctime(localtime(&tt));
			}
			DisplayPm(to,pUser,is,p);
		}
	}
	// script
	if(m_spChat && !pUser->IsIgnored() && g_app.RunningScripts()){
		CComPtr<IDispatch> spUser;
		HRESULT hr = CreateUserObj(pUser,&spUser);
		
		if(SUCCEEDED(hr)){
			CComPtr<IDispatch> spTxt;
			hr = CreateTxtObj(&is,&spTxt);
			if(SUCCEEDED(hr)){
				g_app.GetNo5Obj()->Fire_OnPrivateMsg(m_spChat,spUser,spTxt);
			}
		}
	}
	return pUser;
}

CUser * CChatViewBase::_OnEmote(LPCTSTR name,LPCTSTR msg,bool utf8)
{
	CUser *pUser = m_users.GetUser(name);
	CInputStream is;
	
	if(!pUser){		// ghost user
		pUser = AddUser(name);

		pUser->m_chat = true;
		if(m_bActive){
			if(!pUser->IsIgnored())
				m_pFrame->UserListView()->AddUser(pUser);
			else
				m_pFrame->QuickIgnoreView()->AddUser(name);
		}

	}

	if(pUser->IsIgnored())
		return pUser;

	if(pUser->OnAnyPost() || pUser->OnPost()){
		DoAutoIgnoreUser(pUser->m_name,true,IDS_IG_FLOOD);
		return pUser;
	}
	if(!lstrlen(msg))
		return pUser;
	// parse the message using a CTextStream
	if(utf8)
		is << (LPCTSTR)FromUTF8(msg);
	else
		is << msg;

	// retrieves information from it
	pUser->SetLastLine(is.GetPlainText2());
	pUser->m_dwLinks += is.GetLinkCount();

	// check for auto ignore
	ULONG uRes;
	if(TestAutoIgnore(pUser,is,uRes)){
		DoAutoIgnoreUser(pUser->m_name,true,uRes);
		return pUser;
	}

	if(!pUser->IsIgnored()){
		DisplayEmote(pUser,is);
	}
	// script
	if(m_spChat && !pUser->IsIgnored() && g_app.RunningScripts()){
		CComPtr<IDispatch> spUser;
		HRESULT hr = CreateUserObj(pUser,&spUser);
		
		if(SUCCEEDED(hr)){
			CComPtr<IDispatch> spTxt;
			hr = CreateTxtObj(&is,&spTxt);
			if(SUCCEEDED(hr)){
				g_app.GetNo5Obj()->Fire_OnEmote(m_spChat,spUser,spTxt);
			}
		}
	}
	return pUser;
}

CUser * CChatViewBase::_OnThink(LPCTSTR name,LPCTSTR msg,bool utf8)
{
	CUser *pUser = m_users.GetUser(name);
	CInputStream is;
	
	if(!pUser){		// ghost user
		pUser = AddUser(name);

		pUser->m_chat = true;
		if(m_bActive){
			if(!pUser->IsIgnored())
				m_pFrame->UserListView()->AddUser(pUser);
			else
				m_pFrame->QuickIgnoreView()->AddUser(name);
		}

	}

	if(pUser->IsIgnored())
		return pUser;

	if(pUser->OnAnyPost() || pUser->OnPost()){
		DoAutoIgnoreUser(pUser->m_name,true,IDS_IG_FLOOD);
		return pUser;
	}
	if(!lstrlen(msg))
		return pUser;

	// parse the message using a CTextStream
	if(utf8)
		is << (LPCTSTR)FromUTF8(msg);
	else
		is << msg;

	// retrieves information from it
	pUser->SetLastLine(is.GetPlainText2());
	pUser->m_dwLinks += is.GetLinkCount();

	// check for auto ignore
	ULONG uRes;
	if(TestAutoIgnore(pUser,is,uRes)){
		DoAutoIgnoreUser(pUser->m_name,true,uRes);
		return pUser;
	}

	if(!pUser->IsIgnored()){
		DisplayThink(pUser,is);
	}
	// script
	if(m_spChat && !pUser->IsIgnored() && g_app.RunningScripts()){
		CComPtr<IDispatch> spUser;
		HRESULT hr = CreateUserObj(pUser,&spUser);
		
		if(SUCCEEDED(hr)){
			CComPtr<IDispatch> spTxt;
			hr = CreateTxtObj(&is,&spTxt);
			if(SUCCEEDED(hr)){
				g_app.GetNo5Obj()->Fire_OnThink(m_spChat,spUser,spTxt);
			}
		}
	}
	return pUser;
}

void CChatViewBase::_OnChatJoin(LPCTSTR room,LPCTSTR rmid,LPCTSTR topic)
{
	// lets clear the user list view before deleting the CUser objects because the
	// user list view hold pointers to those objects
	if(m_bActive){
		m_pFrame->UserListView()->RemoveAll();
		m_pFrame->QuickIgnoreView()->RemoveAll();
	}

	// clear chat user list
	m_users.SetChat(false);		// mark all users as not in chat
	m_users.CleanUp();			// do some clean up

	{
		InitTS init(m_ts);

		m_ts << m_pvo->TxtNo5() << "you are in " << GREEN;
		m_ts << Bold(TRUE) << Italic(FALSE) << Under(FALSE) << room;
		m_ts << Bold(FALSE) << Italic(FALSE) << Under(FALSE) << m_pvo->TxtNo5();
		if(topic && topic[0] != ' '){
			m_ts << " - " << Bold(FALSE) << Italic(TRUE) << Under(FALSE) << NAVY << topic;
		}
	}
	
	CString room2 = room;
	if(rmid && lstrlen(rmid)){
		room2 += "::";
		room2 += rmid;
	}
	m_pao->SetLastRoom(room2);
	
	if(m_RoomHistory.m_bAdd){
		m_RoomHistory.Add(room2);
	}
	else
		m_RoomHistory.m_bAdd = true;	// reset

	if(m_bActive){
		m_pFrame->GetChatToolbar().EnableButton(ID_ROOM_PREVIOUS,m_RoomHistory.HasPrevious());
		m_pFrame->GetChatToolbar().EnableButton(ID_ROOM_NEXT,m_RoomHistory.HasNext());
	}

	g_app.GetRecentRooms()->Add(room2);
	
	// stop knocking 
	if(m_bKnock){
		CWindow wnd = GetHandle();
		BOOL res = wnd.KillTimer(TIMER_KNOCK);
		ATLASSERT(res);
		if(res){
			m_knocks = 0;
			m_bKnock = FALSE;
		}
	}
	CViewTextParams params = { room, topic,GetChatName() };
	CWindow wnd = GetHandle();

	wnd = wnd.GetParent();
	wnd.SendMessage(NO5WM_SETCHILDTEXT,(WPARAM)&params);
	// script
	if(m_spChat && g_app.RunningScripts()){
		g_app.GetNo5Obj()->Fire_OnChatJoin(m_spChat);
	}
}

void CChatViewBase::_OnRoomFull(LPCTSTR alt_room,LPCTSTR room,LPCTSTR id)
{
	if(!m_bKnock){
		DisplayRoomFull(alt_room,room,id);
	}
}

void CChatViewBase::ParseRoomString(LPCTSTR s,CString &room,CString &id)
{
	CStringToken st;
	CSimpleArray<CString> v;

	st.Init(s,"::",true);
	st.GetAll2(v);

	if(v.GetSize() == 1){
		if(v[0].Find("::",0) == 0)
			id = v[0];
		else
			room = v[0];
	}
	else if(v.GetSize() == 2){
		room = v[0];
		id = v[1];
	}
	else
		room = s;
}

void CChatViewBase::DisplayUserMenu(LPCTSTR name,const CPoint &pt)
{
	CUser *pUser = m_users.GetUser(name); // todo, it should display a menu even if not in list
	CMenu menu;

	if(pUser && menu.LoadMenu(IDR_USER)){
		CMenu menu2 = menu.GetSubMenu(0);

		if(menu2.IsMenu()){
			CString name;
			name = pUser->m_name;

			menu2.InsertMenu(0,MF_BYPOSITION,ID_USER_NAME,pUser->m_name);

			// set check marks according to user atrributes
			if(pUser->m_ignored)
				menu2.CheckMenuItem(ID_USER_IGNORE,MF_CHECKED);
			if(pUser->m_mimic)
				menu2.CheckMenuItem(ID_USER_MIMIC,MF_CHECKED);
			if(pUser->m_follow)
				menu2.CheckMenuItem(ID_USER_FOLLOW,MF_CHECKED);
			//if(!pUser->m_cam)
			//	menu2.EnableMenuItem(ID_USER_CAM,MF_GRAYED);

			UINT cmd = menu2.TrackPopupMenu(TPM_RIGHTALIGN|TPM_BOTTOMALIGN|TPM_NONOTIFY|\
					TPM_RETURNCMD|TPM_LEFTBUTTON,pt.x,pt.y,GetHandle(),NULL);

			switch(cmd)
			{
			case ID_USER_IGNORE:
				OnUserList_Ignored(name);
				break;
			case ID_USER_PROFILE:
				OnUserList_Profile(name);
				break;
			case ID_USER_PM:
				OnUserList_Pm(name);
				break;
			case ID_USER_MIMIC:
				OnUserList_Mimic(name);
				break;
			case ID_USER_FOLLOW:
				OnUserList_Follow(name);
				break;
			case ID_USER_CAM:
				OnUserList_Cam(name);
				break;
			case ID_USER_CAMINVITE:
				OnUserList_CamInvite(name);
				break;
			default:
				break;
			}
			menu2.DestroyMenu();
		}
		menu.DestroyMenu();
	}
}

void CChatViewBase::DisplayUserName(CUser *pUser)
{
	FontAll f;
	CTextAtom atom;

	if(IsMe(pUser->m_name))
		f = m_pvo->NameMe();
	else if(pUser->m_buddy)
		f = m_pvo->NameBuddy();
	else
		f = m_pvo->NameUser();

	m_ts << f << LPCTSTR(pUser->m_name) << Font(true);
}

void CChatViewBase::DisplayChatText(CUser *pUser,CTextStream &ts)
{
	InitTS its(m_ts);

	DisplayUserName(pUser);

	// print the ':'
	m_ts << Bold(FALSE) << Italic(FALSE) << Under(FALSE) << BLACK;
	m_ts << " : " ;

	// print the message with the right color
	if(pUser->m_me)
		m_ts << m_pvo->TxtMe();
	else if(pUser->m_buddy)
		m_ts << m_pvo->TxtBuddy();
	else 
		m_ts << m_pvo->TxtUser();

	m_ts.Append(ts);
}

void CChatViewBase::DisplayEmote(CUser *pUser,CTextStream &ts)
{
	InitTS init(m_ts);

	DisplayUserName(pUser);
	m_ts <<  " : " << m_pvo->TxtEmote();
	m_ts.Append(ts);
}

void CChatViewBase::DisplayThink(CUser *pUser,CTextStream &ts)
{
	InitTS init(m_ts);

	DisplayUserName(pUser);
	m_ts << " : " << m_pvo->TxtThought();
	m_ts.Append(ts);
}

void CChatViewBase::DisplayPm(LPCTSTR to,CUser *pUser,CTextStream &ts,LPCTSTR strtime)
{
	InitTS init(m_ts);

	// print the user name with the right colors
	DisplayUserName(pUser);
	m_ts << m_pvo->TxtPvt();
	if(strtime && lstrlen(strtime)){
		m_ts << " [" << strtime << "] ";
	}
	m_ts << " tells you in pm: " << m_pvo->TxtPvt();
	m_ts.Append(ts);
}

void CChatViewBase::DisplayInvited(CUser *pUser,LPCTSTR room,LPCTSTR rmid,LPCTSTR msg)
{
	InitTS init(m_ts);
	
	DisplayUserName(pUser);

	// print our message
	m_ts << m_pvo->TxtInvite() << " invited you to /join " << room <<
		" - " << m_pvo->TxtYahoo() << msg;
}

void CChatViewBase::DisplayRoomFull(LPCTSTR alt_room,LPCTSTR room,LPCTSTR rmid)
{
	InitTS init(m_ts);
	CTextAtom atom;
	CString cmd("/join ");
	m_ts << m_pvo->TxtNo5() << "the room is full, try ";
	cmd += alt_room;
	m_ts.AddAtom(atom.AtomLink(cmd));
	m_ts.AddAtom(atom.AtomLink(FALSE));
		
	m_ts << " or try to ";
	m_ts.AddAtom(atom.AtomLink("/knock"));
	m_ts.AddAtom(atom.AtomLink(FALSE));
}

BOOL CChatViewBase::LoadBot(LPCTSTR name)
{
	NO5TL::CCurDirSaver dirsaver;
	HRESULT hr;
	CString path;
	CString inifile;
	CString userfile;

	path = g_app.GetBotsFolder();
	path += name;
	path += "\\";

	SetCurrentDirectory(path);

		
	inifile = "data\\bot.ini";
	userfile = "log\\localuser.txt";

	m_spBot.Release();

	hr = m_spBot.CoCreateInstance(CLSID_AimlBot);
	if(SUCCEEDED(hr)){
		HRESULT res = S_OK;
		CComBSTR bstrIni = inifile;
		CComBSTR bstrUsr = userfile;
		hr = m_spBot->initialize2(bstrIni,bstrUsr,&res);
		if(FAILED(hr) || FAILED(res)){
			m_spBot.Release();
			if(FAILED(res))
				hr = res;
		}
	}
	return SUCCEEDED(hr);
}

// CYahooChatViewBase

CYahooChatViewBase::CYahooChatViewBase(CTextStream &ts):CChatViewBase(ts)
{
	m_pChat = NULL;
	NO5YAHOO::YahooChatCreate(&m_pChat,this);
	HRESULT hr = CreateChatObj(this,&m_spChat);
	ATLASSERT(SUCCEEDED(hr));
}

CYahooChatViewBase::~CYahooChatViewBase()
{
	NO5YAHOO::YahooChatDestroy(&m_pChat);
}

// CYahooChatViewBase::IYahooChatEvents

void CYahooChatViewBase::OnSignIn(LPCSTR cookie)
{
	{
		InitTS init(m_ts);

		if(NULL != cookie){
			m_ts << "signed in!";
		}
		else{
			m_ts << "sign in failed";
		}
	}
	// its better to empty the stream first, otherwise these calls below will send
	// a message which might be trigger before this method returns
	if(!cookie)
		m_pChat->Term(true);
	else
		m_pChat->Init(m_pgo->GetServerYcht(),m_pgo->GetPortYcht());
}

void CYahooChatViewBase::OnLogin(LPCSTR name,LPCSTR badwords)
{
	InitTS init(m_ts);

	if(IsMe(name)){
		m_ts << m_pvo->NameMe();
		m_pChat->JoinRoom(m_pao->GetLastRoom());
	}
	else if(IsBuddy(name))
		m_ts << m_pvo->NameBuddy();
	else
		m_ts << m_pvo->NameUser();

	m_ts << name << m_pvo->TxtNo5() << " logged in";
}

void CYahooChatViewBase::OnLogoff(int status,LPCSTR msg)
{
	{
		InitTS init(m_ts);
		m_ts << m_pvo->TxtNo5() << "logged off";
	}
	if(m_bActive){
		m_pFrame->UserListView()->RemoveAll();
		m_pFrame->QuickIgnoreView()->RemoveAll();
	}
	m_users.RemoveAll();
}

void CYahooChatViewBase::OnRoomIn(LPCSTR room,LPCSTR slogan,CYahooChatUser &user)
{
	CUser *pUser = m_users.GetUser(user.m_name);
	
	if(!pUser){
		pUser = AddUser(user); // this also mark the user as in chat
	}
	else{ 
		pUser->m_cam = user.m_webcam;
		pUser->m_chat = true;
	}

	_OnUserEnter(pUser);
}

void CYahooChatViewBase::OnRoomOut(LPCSTR room,LPCSTR name)
{
	CUser *pUser = _OnUserLeave(name);
	
	if(pUser->m_follow)
		m_pChat->Goto(name);
}

void CYahooChatViewBase::OnJoinRoom(LPCSTR room,LPCSTR slogan,
		LPCSTR cookies,CYahooChatUsers &users,LPCSTR cam_cookie)
{
	_OnChatJoin(room,m_pChat->GetRoomSpace(),slogan);

	// add new users to our internal list. this will also mark them as in chat
	AddUsers(users);

	if(m_bActive){
		FillUserListView();
		FillQuickIgnoreView();
	}
}


void CYahooChatViewBase::OnRoomFull(LPCSTR msg,LPCSTR alt_room)
{
	CString room,id;
	m_pChat->GetDesiredRoom(room,id);
	_OnRoomFull(alt_room,room,id);
}

void CYahooChatViewBase::OnJoinFailed(unsigned short status,LPCSTR msg)
{
	InitTS init(m_ts);
	bool stop_knock = true;

	switch(status){
		case ROOM_FULL:
			stop_knock = false;
			break;
	}
	m_ts << m_pvo->TxtNo5() << "failed to to join room - ";
	m_ts << m_pvo->TxtYahoo() << msg;

	if(m_bKnock && stop_knock){
		OnCmdNoKnock();
	}
}

void CYahooChatViewBase::OnText(LPCSTR room,LPCSTR name,LPCSTR msg,
	LPCSTR extra)
{
	CUser *pUser = _OnChatText(name,msg,false);

	if(pUser->IsIgnored())
		return;
	// mimic
	if((pUser->m_mimic || m_bMimicAll) && !pUser->m_me){
		m_pChat->SendText(msg);
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
					m_pChat->SendText(s);
				}
			}
		}
	}
}

CUser * CChatViewBase::_OnInvited(LPCTSTR from,LPCTSTR room,LPCTSTR rmid,LPCTSTR msg)
{
	CUser *pUser = m_users.GetUser(from);


	if(!pUser){ 
		pUser = AddUser(from);
	}

	if(pUser->IsIgnored())
		return pUser;

	if(pUser->OnAnyPost() || pUser->OnInvite()){
		DoAutoIgnoreUser(pUser->m_name,true,IDS_IG_FLOOD);
		return pUser;
	}

	{
		DisplayInvited(pUser,room,rmid,msg);
	}
	return pUser;
}

void CYahooChatViewBase::OnInvited(LPCSTR msg,LPCSTR room,
									 LPCSTR name)
{
	_OnInvited(name,room,"",msg);
}

void CYahooChatViewBase::OnAway(LPCSTR name,LPCSTR room,
								  LPCSTR number,LPCSTR msg)
{
	CUser *pUser = m_users.GetUser(name);


	if(!pUser){ 
		pUser = AddUser(name);
	}

	if(pUser->IsIgnored())
		return;

	if(pUser->OnAnyPost()){
		DoAutoIgnoreUser(pUser->m_name,true,IDS_IG_FLOOD);
		return;
	}

	{
		InitTS init(m_ts);

		DisplayUserName(pUser);
		
		// print the message
		if(IsBuddy(name))
			m_ts << m_pvo->TxtAwayBuddy();
		else
			m_ts << m_pvo->TxtAwayUsr();

		if(msg && lstrlen(msg)){
			m_ts << " ( " << msg << " )";
		}
	}
}

void CYahooChatViewBase::OnThinking(LPCSTR room,LPCSTR name,
									  LPCSTR msg)
{
	CUser *pUser = _OnThink(name,msg);

	if(!pUser->IsIgnored()){
		// mimic
		if((pUser->m_mimic || m_bMimicAll) && !pUser->m_me ){
			m_pChat->Think(msg);
		}
	}
}

void CYahooChatViewBase::OnEmotion(LPCSTR room,LPCSTR name,
	LPCSTR emote,LPCSTR extra)
{
	CUser *pUser = _OnEmote(name,emote);

	if(!pUser->IsIgnored()){
		// mimic
		if((pUser->m_mimic || m_bMimicAll) && !pUser->m_me ){
			m_pChat->SendEmote(emote);
		}
	}
}

void CYahooChatViewBase::OnPm(LPCSTR from,LPCSTR to,LPCSTR msg)
{
	_OnPrivateMsg(from,to,msg,false);
}

void CYahooChatViewBase::OnBuddy(LPCSTR msg)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtYahoo() << msg ;
}

void CYahooChatViewBase::OnDice(LPCSTR name,LPCSTR msg)
{
	CUser *pUser = m_users.GetUser(name);
	
	if(!pUser){		// ghost user
		pUser = AddUser(name);

		pUser->m_chat = true;
		if(m_bActive){
			if(!pUser->IsIgnored())
				m_pFrame->UserListView()->AddUser(pUser);
			else
				m_pFrame->QuickIgnoreView()->AddUser(name);
		}

	}

	if(pUser->IsIgnored())
		return;

	if(pUser->OnAnyPost() || pUser->OnPost()){
		DoAutoIgnoreUser(pUser->m_name,true,IDS_IG_FLOOD);
		return;
	}

	{
		InitTS init(m_ts);

		DisplayUserName(pUser);
		m_ts << " : " << m_pvo->TxtYahoo() << msg;
	}
}

void CYahooChatViewBase::OnAdvertise(LPCSTR msg)
{
	InitTS init(m_ts);
	m_ts << "Advertise: " << m_pvo->TxtYahoo() << msg;
}

void CYahooChatViewBase::OnServerMsg(LPCSTR msg,unsigned long packet,
	unsigned long status)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtYahoo() << msg ;
}

void CYahooChatViewBase::OnTime(LPCSTR msg)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtYahoo() << msg ;
}

void CYahooChatViewBase::OnHelp(LPCSTR msg)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtYahoo() << msg;
}

// CYahooChatViewBase::ISocketEvents
void CYahooChatViewBase::OnSockConnect(int iError)
{
	InitTS init(m_ts);

	m_ts << m_pvo->TxtNo5();
	m_ts << "on connect";
	m_pChat->Login();
}

void CYahooChatViewBase::OnSockConnecting(void)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "connecting...";
}

void CYahooChatViewBase::OnSockClose(int iError)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "connection closed";
}

void CYahooChatViewBase::OnSockError(int error)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "socket error: " << m_pvo->TxtError() << NO5TL::CSocket::GetErrorDesc(error);
}

void CYahooChatViewBase::OnSockConnectTimeout(void)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "timed  out";
}

void CYahooChatViewBase::OnSockResolvingAddress(void)
{
	InitTS init(m_ts);
	m_ts << m_pvo->TxtNo5();
	m_ts << "resolving server address";
}

void CYahooChatViewBase::OnSockAddressResolved(int error)
{
	if(!error){
		InitTS init(m_ts);
		m_ts << m_pvo->TxtNo5();
		m_ts << "server address resolved";
	}
}


CUser * CYahooChatViewBase::AddUser(CYahooChatUser &user)
{
	CUser *pUser;
	
	pUser = m_users.GetUser(user.m_name);
	if(!pUser){
		pUser = AddUser(user.m_name);
	}
	pUser->m_cam = user.m_webcam;
	pUser->m_chat = true;
	// well this isnt the time the user entered coz the user was already in room
	// pUser->m_tEnter = GetTickCount();
	return pUser;
}

CUser * CYahooChatViewBase::AddUser(LPCTSTR name)
{
#ifdef _DEBUG
	CUser *pFound = m_users.GetUser(name);
	if(pFound){
		ATLTRACE("CYahooChatViewBase::AddUser adding %s but user already there!\n",name);
	}
#endif
	CUser *pUser = m_users.AddUser(name);

	pUser->m_ignored = m_pIgnoreList->find(name);
	pUser->m_me = IsMe(name);
	pUser->m_buddy = IsBuddy(name);

	return pUser;
}

void CYahooChatViewBase::AddUsers(CYahooChatUsers &users)
{
	int i;
	const int count = users.GetSize();

	for(i=0;i<count;i++){
		AddUser(users[i]);
	}
}


void CYahooChatViewBase::OnTextInput(LPCTSTR text)
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
		else if(text[0] == ':' && m_pChat->IsInRoom() && !(psm->IsSmiley(text,smiley))){
			m_pChat->SendEmote(&(text[1]));
		}
		else if(m_pChat->IsInRoom()){
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

			m_pChat->SendText(ts.GetYahooText());
		}
	}
}


BOOL CYahooChatViewBase::OnCmdJoin()
{
	if(m_pChat->IsConnected()){
		CString room;
		if(GetArgs1(room))
			m_pChat->JoinRoom(room);
	}
	return TRUE;
}

BOOL CYahooChatViewBase::OnCmdGoto()
{
	if(m_pChat->IsConnected()){
		CString name;
		if(GetArgs1(name))
			m_pChat->Goto(name);
	}
	return TRUE;
}

BOOL CYahooChatViewBase::OnCmdPm()
{
	if(m_pChat->IsConnected()){
		CString name,msg;
		if(GetArgs2(name,msg))
			m_pChat->SendPm(name,msg);
	}
	return TRUE;
}

BOOL CYahooChatViewBase::OnCmdAway()
{
	if(m_pChat->IsConnected()){
		CString msg;
		if(GetArgs1(msg))
			m_pChat->SetStatus(YAHOO_STATUS_CUSTOM,msg);
		else
			m_pChat->SetStatus(YAHOO_STATUS_BRB,"");
	}
	return TRUE;
}

BOOL CYahooChatViewBase::OnCmdBack()
{
	if(m_pChat->IsConnected())
		m_pChat->SetStatus(YAHOO_STATUS_AVAILABLE,"");
	return TRUE;
}

BOOL CYahooChatViewBase::OnCmdLeave()
{
	if(m_pChat->IsInRoom()){
		m_pChat->LeaveRoom();
	}
	return TRUE;
}

BOOL CYahooChatViewBase::OnCmdThink()
{
	if(m_pChat->IsConnected()){
		CString msg;

		if(GetArgs1(msg))
			m_pChat->Think(msg);
	}
	return TRUE;
}

BOOL CYahooChatViewBase::OnCmdEmote()
{
	if(m_pChat->IsConnected()){
		CString msg;

		if(GetArgs1(msg))
			m_pChat->SendEmote(msg);
	}
	return TRUE;
}

BOOL CYahooChatViewBase::OnCmdInvite()
{
	if(m_pChat->IsConnected()){
		CString name;

		if(GetArgs1(name))
			m_pChat->Invite(name);
	}
	return TRUE;
}

BOOL CYahooChatViewBase::OnCmdBud()
{
	// todo: add buddy
	return TRUE;
}

BOOL CYahooChatViewBase::OnCmdViewcam()
{
	// todo
	return TRUE;
}


BOOL CYahooChatViewBase::OnCmdUnknown()
{
	InitTS init(m_ts);
	BOOL res = TRUE;

	if(!m_cmd.CompareNoCase("room"))
		m_ts << m_pChat->GetRoom();
	else if(!m_cmd.CompareNoCase("rmspace"))
		m_ts << m_pChat->GetRoomSpace();
	else if(!m_cmd.CompareNoCase("rmcat"))
		m_ts << m_pChat->GetRoomCategory();
	else if(!m_cmd.CompareNoCase("cookie"))
		m_ts << m_pChat->GetCookie();
	else if(!m_cmd.CompareNoCase("vcauth"))
		m_ts << m_pChat->GetVoiceCookie();
	else if(!m_cmd.CompareNoCase("wcauth"))
		m_ts << m_pChat->GetCamCookie();
	else if(!m_cmd.CompareNoCase("vc"))
		m_ts << (m_pChat->IsVoiceEnabled() ? "true" : "false") ;
	else if(!m_cmd.CompareNoCase("localserver")){
		if(m_pChat->IsConnected()){
			CString server;
			int port;
			m_pChat->GetLocalAddress(server,port);
			server += ':';
			server.Append(port);
			m_ts << (LPCTSTR)server;
		}
	}
	else if(!m_cmd.CompareNoCase("server")){
		if(m_pChat->IsConnected()){
			CString server;
			int port;
			m_pChat->GetRemoteAddress(server,port);
			server += ':';
			server.Append(port);
			m_ts << (LPCTSTR)server;
		}
	}
	else
		res = FALSE;

	return res;
}

void CYahooChatViewBase::ChangeRoom(LPCTSTR room)
{
	m_pChat->JoinRoom(room);
}

void CYahooChatViewBase::pm_OnSend(CPrivateBox *pBox,LPCTSTR msg)
{
	if(m_pChat->IsConnected())
		m_pChat->SendPm(pBox->m_me,msg);
}



// message handlers
LRESULT CYahooChatViewBase::OnCreate(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
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

LRESULT CYahooChatViewBase::OnClose(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	if(m_pChat->IsLoggedIn())
		m_pChat->Logout();
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

	return 0;
}

LRESULT CYahooChatViewBase::OnChatConnect(WORD,WORD,HWND,BOOL &bHandled)
{
	if(m_pChat->IsConnected())
		m_pChat->Term(false);
	else{
		BOOL res = m_pChat->Init(m_pgo->GetServerYcht(),m_pgo->GetPortYcht());
		ATLASSERT(res);
	}

	return 0;
}

LRESULT CYahooChatViewBase::OnChatLeave(WORD,WORD,HWND,BOOL &bHandled)
{
	if(m_pChat->IsConnected())
		m_pChat->LeaveRoom();
	return 0;
}

LRESULT CYahooChatViewBase::OnChatVoice(WORD,WORD,HWND,BOOL &bHandled)
{
	CVoiceDialog *p = new CVoiceDialog();
	p->m_name = m_pChat->GetName();
	p->m_server = "v1.vc.scd.yahoo.com";
	p->m_room = m_pChat->GetRoom();
	p->m_rmid = m_pChat->GetRoomSpace();
	p->m_cookie = m_pChat->GetVoiceCookie();
	p->Create(g_app.GetMainFrame()->GetHandle());
	return 0;
}

LRESULT CYahooChatViewBase::OnRoomPrevious(WORD,WORD,HWND,BOOL &bHandled)
{
	if(m_RoomHistory.HasPrevious() && m_pChat->IsConnected()){
		m_RoomHistory.Previous();
		m_RoomHistory.m_bAdd = false;
		m_pChat->JoinRoom(m_RoomHistory.GetRoom());
	}
	return 0;
}

LRESULT CYahooChatViewBase::OnRoomNext(WORD,WORD,HWND,BOOL &bHandled)
{
	if(m_RoomHistory.HasNext() && m_pChat->IsConnected()){
		m_RoomHistory.Next();
		m_RoomHistory.m_bAdd = false;
		m_pChat->JoinRoom(m_RoomHistory.GetRoom());
	}
	return 0;
}

void CYahooChatViewBase::blv_OnAddBuddy(LPCTSTR group)
{
	CWindow wnd = m_pFrame->GetHandle();
	CString s;
	s.LoadString(IDS_NOYCHT);
	wnd.MessageBox(s);
}

void CYahooChatViewBase::blv_OnRemoveBuddy(LPCTSTR name,LPCTSTR group)
{
	CWindow wnd = m_pFrame->GetHandle();
	CString s;
	s.LoadString(IDS_NOYCHT);
	wnd.MessageBox(s);
}


void CYahooChatViewBase::blv_OnRenameGroup(LPCTSTR group)
{
	CWindow wnd = m_pFrame->GetHandle();
	CString s;
	s.LoadString(IDS_NOYCHT);
	wnd.MessageBox(s);
}

void CYahooChatViewBase::blv_OnViewCam(LPCTSTR name)
{
	CWindow wnd = m_pFrame->GetHandle();
	CString s;
	s.LoadString(IDS_NOYCHT);
	wnd.MessageBox(s);
}
void CYahooChatViewBase::blv_OnCamInvite(LPCTSTR name)
{
	CWindow wnd = m_pFrame->GetHandle();
	CString s;
	s.LoadString(IDS_NOYCHT);
	wnd.MessageBox(s);
}
void CYahooChatViewBase::blv_OnGotoUser(LPCTSTR name)
{
	if(m_pChat->IsConnected()){
		m_pChat->Goto(name);
	}
}
void CYahooChatViewBase::blv_OnInviteUser(LPCTSTR name)
{
	if(m_pChat->IsConnected()){
		m_pChat->Invite(name);
	}
}

void CYahooChatViewBase::OnUserList_Cam(LPCTSTR name)
{
	CWindow wnd = m_pFrame->GetHandle();
	CString s;
	s.LoadString(IDS_NOYCHT);
	wnd.MessageBox(s);
}

void CYahooChatViewBase::OnUserList_CamInvite(LPCTSTR name)
{
	CWindow wnd = m_pFrame->GetHandle();
	CString s;
	s.LoadString(IDS_NOYCHT);
	wnd.MessageBox(s);
}

void CYahooChatViewBase::OnUserList_AddBuddy(LPCTSTR name)
{
	CWindow wnd = m_pFrame->GetHandle();
	CString s;
	s.LoadString(IDS_NOYCHT);
	wnd.MessageBox(s);
}

void CYahooChatViewBase::OnUserList_RemoveBuddy(LPCTSTR name)
{
	CWindow wnd = m_pFrame->GetHandle();
	CString s;
	s.LoadString(IDS_NOYCHT);
	wnd.MessageBox(s);
}



// CYahooChatViewImpl
template <class T>
CYahooChatViewImpl<T>::CYahooChatViewImpl(CTextStream &ts):CYahooChatViewBase(ts)
{
	//
}


template <class T>
LRESULT CYahooChatViewImpl<T>::OnMdiActivate(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
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
LRESULT CYahooChatViewImpl<T>::OnCreate(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	_BaseClass::OnCreate(WM_CREATE,wParam,lParam,bHandled);
	bHandled = TRUE;

	SetTimer(TIMER_CLEANUP, 60 * 1000);
	SetTimer(TIMER_PING, 10 * ( 60 * 1000 ));
	return 0;
}

template <class T>
LRESULT CYahooChatViewImpl<T>::OnDestroy(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	KillTimer(TIMER_CLEANUP);
	KillTimer(TIMER_PING);
	bHandled = FALSE;
	return 0;
}

template <class T>
LRESULT CYahooChatViewImpl<T>::OnTimer(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	switch(wParam)
	{
		case static_cast<WPARAM>(TIMER_CLEANUP):
			m_users.CleanUp();
			break;
		case static_cast<WPARAM>(TIMER_PING):
			if(m_pChat->IsConnected()){
				m_pChat->Ping();
				m_pFrame->AddMarqueeString("ping");
			}
			break;
		case static_cast<WPARAM>(TIMER_KNOCK):
			if(m_pChat->IsConnected()){
				CString room,id;
				m_pChat->GetDesiredRoom(room,id);

				if(!id.IsEmpty()){
					room += "::";
					room += id;
				}
				m_pChat->JoinRoom(room);
				BOOL res = KillTimer(TIMER_KNOCK);
				ATLASSERT(res);
				if(res){
					m_knocks++;
					res = SetTimer(TIMER_KNOCK,(m_knocks + 1) * 1000);
					ATLASSERT(res);
				}
				CString msg;
				msg.Format(" knock %d ",m_knocks);
				m_pFrame->AddMarqueeString(msg);
			}
		default:
			break;
	}
	return 0;
}


template <class T>
void CYahooChatViewImpl<T>::OnJoinRoom(LPCSTR room,LPCSTR slogan,
		LPCSTR cookies,CYahooChatUsers &users,LPCSTR cam_cookie)
{	
	CYahooChatViewBase::OnJoinRoom(room,slogan,cookies,users,cam_cookie);
}

template <class T>
bool CYahooChatViewImpl<T>::IsHandleValid(void)
{
	return IsWindow() ? true : false;
}

template <class T>
CPrivateBox * CYahooChatViewImpl<T>::GetPrivateBox(LPCTSTR name)
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



// CYahooChatRichView

CYahooChatRichView::CYahooChatRichView(void):_BaseClass(m_ts)
{
	m_pEdit = new NO5TL::CNo5RichEdit();
	m_ts.SetEdit(m_pEdit);
}

CYahooChatRichView::~CYahooChatRichView()
{
	delete m_pEdit;
}

// message handlers

LRESULT CYahooChatRichView::OnCreate(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
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

	res = m_pChat->SignIn(YCHT_DEF_SIGNIN_SERVER,80,YCHT_DEF_SIGNIN_SERVER,m_pao->GetName(),
		m_pao->GetPassword());

	ATLASSERT(res);

	
	return lRes;
}

LRESULT CYahooChatRichView::OnSize(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
{
	if(m_pEdit->IsWindow()){
		m_pEdit->MoveWindow(0,0,LOWORD(lParam),HIWORD(lParam));
	}
	return 0;
}

LRESULT CYahooChatRichView::OnEditMsgFilter(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
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
						DisplayUserMenu(name,pt);
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

LRESULT CYahooChatRichView::OnEditLink(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
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

LRESULT CYahooChatRichView::OnErrSpace(WORD,WORD,HWND,BOOL &bHandled)
{
	m_pEdit->ClearAll();
	return 0;
}

LRESULT CYahooChatRichView::OnMaxText(WORD,WORD,HWND,BOOL &bHandled)
{
	m_pEdit->ClearAll();
	return 0;
}

LRESULT CYahooChatRichView::OnVScroll(WORD,WORD,HWND,BOOL &bHandled)
{
	// todo: disable auto scroll ?
	return 0;
}


void CYahooChatRichView::OnOptionsChanged(void)
{
	CChatViewBase::OnOptionsChanged();

	// todo: we dont receive information about which option has changed
	// so we will change the background colour, even if it has not changed
	m_pEdit->SetBackgroundColor(m_pvo->ClrBack());
}



// CYahooChatHtmlView 

CYahooChatHtmlView::CYahooChatHtmlView(void):_BaseClass(m_ts)
{
	m_ts.SetEdit(&m_edit);
	m_pUIHandler = new CMyDocHostUIHandler((IDispatch *)this);
}

CYahooChatHtmlView::~CYahooChatHtmlView()
{
	delete m_pUIHandler;
}


LRESULT CYahooChatHtmlView::OnCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CYahooChatHtmlView::OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HRESULT hr = S_OK;

	if(m_edit.m_doc.m_sp){
	//	hr = DispEventHtml::DispEventUnadvise(m_edit.m_doc.m_sp);
	//	ATLASSERT(SUCCEEDED(hr));
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

LRESULT CYahooChatHtmlView::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_edit.MoveWindow(0,0,LOWORD(lParam),HIWORD(lParam));
	return 0;
}

void CYahooChatHtmlView::OnDocumentComplete(IDispatch *pDisp,VARIANT *pUrl)
{
	HRESULT hr = m_edit.GetDocument();

	if(SUCCEEDED(hr)){
		//hr = DispEventHtml::DispEventAdvise(m_edit.m_doc.m_sp);
		//ATLASSERT(SUCCEEDED(hr));
		if(SUCCEEDED(hr)){
			hr = m_edit.GetInterfaces();
			if(SUCCEEDED(hr)){
				hr = m_edit.PutBgColor(m_pvo->ClrBack());
			}
		}
	}
	ATLASSERT(SUCCEEDED(hr));

	if(!m_pChat->IsConnected()){
		BOOL bDummy;
		bDummy = m_pChat->SignIn(YCHT_DEF_SIGNIN_SERVER,80,YCHT_DEF_SIGNIN_SERVER,m_pao->GetName(),
			m_pao->GetPassword());
		ATLASSERT(bDummy);
	}
}

void __stdcall CYahooChatHtmlView::OnLinkClick(BSTR name)
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


void __stdcall CYahooChatHtmlView::OnUserClick(BSTR s)
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

void __stdcall CYahooChatHtmlView::OnUserRClick(BSTR s,long x,long y)
{
	USES_CONVERSION;
	LPCTSTR user = OLE2CT(s);
	CPoint pt(x,y);

	DisplayUserMenu(user,pt);
}

void __stdcall CYahooChatHtmlView::OnDivClick(BSTR s)
{
	
}
void __stdcall CYahooChatHtmlView::OnDivDblClk(BSTR s)
{
	
}
void __stdcall CYahooChatHtmlView::OnDivRClick(BSTR s)
{
	
}

void __stdcall CYahooChatHtmlView::OnUserDblClk(BSTR s)
{
	
}

void __stdcall CYahooChatHtmlView::OnAnswerPm(BSTR s)
{
	USES_CONVERSION;
	LPCTSTR name = OLE2CT(s);
	if(name && lstrlen(name))
		OnUserList_Pm(name);
}

void CYahooChatHtmlView::DisplayUserName(CUser *pUser)
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

void CYahooChatHtmlView::DisplayChatText(CUser *pUser,CTextStream &ts)
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

void CYahooChatHtmlView::DisplayEmote(CUser *pUser,CTextStream &ts)
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
void CYahooChatHtmlView::DisplayThink(CUser *pUser,CTextStream &ts)
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

void CYahooChatHtmlView::DisplayPm(LPCTSTR to,CUser *pUser,CTextStream &ts,LPCTSTR strtime)
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
void CYahooChatHtmlView::DisplayInvited(CUser *pUser,LPCTSTR room,LPCTSTR rmid,LPCTSTR msg)
{
	//
}
void CYahooChatHtmlView::DisplayRoomFull(LPCTSTR alt_room,LPCTSTR room,LPCTSTR rmid)
{
	//
}

*/



void CYahooChatHtmlView::OnOptionsChanged(void)
{
	CChatViewBase::OnOptionsChanged();

	// todo: we dont receive information about which option has changed
	// so we will change the background colour, even if it has not changed
	UpdateVisualOptions();
}

void CYahooChatHtmlView::UpdateVisualOptions(void)
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


// CYahooChatEditView 

CYahooChatEditView::CYahooChatEditView(void):_BaseClass(m_ts)
{
	
}

CYahooChatEditView::~CYahooChatEditView()
{
	
}


LRESULT CYahooChatEditView::OnCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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


	BOOL res = m_pChat->SignIn(YCHT_DEF_SIGNIN_SERVER,80,YCHT_DEF_SIGNIN_SERVER,m_pao->GetName(),
		m_pao->GetPassword());

	return lRes;
}

LRESULT CYahooChatEditView::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_edit.MoveWindow(0,0,LOWORD(lParam),HIWORD(lParam));
	return 0;
}

void CYahooChatEditView::DisplayUserName(CUser *pUser)
{
	m_ts << LPCTSTR(pUser->m_name);
}

void CYahooChatEditView::DisplayChatText(CUser *pUser,CTextStream &ts)
{
	InitTS its(m_ts);

	DisplayUserName(pUser);
	m_ts.Append(ts);
}

COLORREF HighlightColor(COLORREF clrOld,int inc)
{
	NO5TL::CColor color(clrOld);
	
	for(int i=0;i<3;i++){
		if(color[i]){
			int x = color[i];
			x += inc;
			if(x > 255)
				x = 255;
			color[i] = static_cast<BYTE>(x);
		}
	}
	return color;
}
