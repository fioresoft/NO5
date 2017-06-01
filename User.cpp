// User.cpp: implementation of the CUser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "User.h"
#include <no5tl\mystring.h>	// CStringArray
#include <yahoo\yahoochat.h>

DWORD CUser::MAX_PPS = 3;
DWORD CUser::MAX_PP2S = 4;

// CUser

CUser::CUser()
{
	m_gender = USER_GENDER_UNKNOWN;
	m_cam = false;
	m_ignored = false;
	m_mimic = false;
	m_follow = false;
	m_buddy = false;
	m_chat = false;
	m_pm = false;
	m_me = false;
	//m_v = (double)0;
	//m_f1 = m_f2 = m_f3 = 0L;
	//m_posts_dt = 0L;
	//m_t = 0L;
	m_tStart = GetTickCount();
	m_posts_dt = 0;
	m_posts = 0;
	m_invites = 0;
	m_pms = 0;
	m_dwLastAction = GetTickCount();
	m_dwLinks = 0;
	m_repeated = 0;
	m_tEnter = 0;
	m_age = 0;
}

CUser::~CUser()
{
	//ATLTRACE("~CUser(%s)\n",(LPCTSTR)m_name);
}

CUser & CUser::CopyFrom(const CUser &user)
{
	m_name = user.m_name;
	m_gender = user.m_gender;
	m_nick = user.m_nick;
	m_location = user.m_location;
	m_age = (unsigned short)user.m_age;
	m_cam = user.m_cam;
	m_ignored = user.m_ignored;
	m_mimic = user.m_mimic;
	m_follow = user.m_follow;
	m_buddy = user.m_buddy;
	m_pm = user.m_pm;
	m_me = user.m_me;
	/*
	m_v = user.m_v;
	m_f1 = user.m_f1;
	m_f2 = user.m_f2;
	m_f3 = user.m_f3;
	m_posts_dt = user.m_posts_dt;
	m_t = user.m_t;
	*/
	m_tStart = user.m_tStart;
	m_posts_dt = user.m_posts_dt;
	m_posts = user.m_posts;
	m_invites = user.m_invites;
	m_pms = user.m_pms;
	m_dwLastAction = user.m_dwLastAction;
	m_dwLinks = user.m_dwLinks;
	m_sLastLine = user.m_sLastLine;
	m_repeated = user.m_repeated;
	m_tEnter = user.m_tEnter;

	SetInfoTag(const_cast<CInfoTag&>(user.m_info));
	return *this;
}

void CUser::SetInfoTag(CInfoTag &info)
{
	CString val;

	m_info = info;

	// get gender
	val = m_info.GetVal(CString("SEX"));
	if(val.IsEmpty())
		val = m_info.GetVal(CString("GENDER"));
	if(val.IsEmpty())
		val = m_info.GetVal(CString("S"));

	if(!val.IsEmpty()){
		val.MakeUpper();
		if(val[0] == 'M')
			m_gender = USER_GENDER_MALE;
		else if(val[0] == 'F')
			m_gender = USER_GENDER_FEMALE;
		else
			m_gender = USER_GENDER_UNKNOWN;
	}
}

CInfoTag & CUser::GetInfoTag(void)
{
	return m_info;
}

/*
void CUser::dt(DWORD h)
{
	m_f3 = m_f2;
	m_f2 = m_f1;
	m_f1 += m_posts_dt;
	ATLASSERT(m_f1 >= m_f2 && m_f1 >= m_f3 && m_f2 >= m_f3);
	if(m_f3)
		m_v = (double)(3.0 * m_f1 - 4.0 * m_f2 + m_f3) / (double)(2.0 * (h/1000.0) );
	m_posts_dt = 0;
	m_t += h/1000;
}
*/

// return true if user should be auto-ignored
bool CUser::OnAnyPost(void)
{
	const DWORD t_cur = GetTickCount();
	bool bIgnore = false;
	bool debug = true;

	m_posts_dt++;

	// one second and max posts reached ?
	if(m_tStart && (t_cur - m_tStart <= 1000) && (m_posts_dt >= MAX_PPS) && debug == true){
		bIgnore = true;
	}
	// 2 seconds and the equivalent max posted reached ?
	else if(m_tStart && ( t_cur - m_tStart <= 2000 ) && ( m_posts_dt >= MAX_PP2S) ){
		bIgnore = true;
	}
	else if(m_tStart && t_cur - m_tStart > 2000){
		// reset
		m_posts_dt = 1;
		m_tStart = t_cur;
	}
	m_dwLastAction = t_cur;

	if(m_me || m_buddy)
		bIgnore = false;

	return bIgnore;
}

bool CUser::OnInvite(void)
{
	m_invites++;
	return false;
}

bool CUser::OnPost(void)
{
	m_posts++;
	return false;
}

bool CUser::OnPm(void)
{
	m_pms++;
	return false;
}

bool CUser::SetLastLine(LPCTSTR s)
{
	if(!m_sLastLine.CompareNoCase(s))
		m_repeated++;
	else{
		m_sLastLine = s;
		m_repeated = 0;
	}
	return m_repeated != 0;
}

LPCTSTR CUser::GetClientName(void)
{
	LPCTSTR p = "";

	if(!m_info.IsEmpty()){
		CString id = m_info.GetVal(CString("ID"));

		if(!lstrcmpi(id,"yhlt"))
			p = "yahelite";
		else if(!lstrcmpi(id,"yzak"))
			p = "yazak";
		else if(!lstrcmpi(id,"cchat"))
			p = "cchat";
		else if(!lstrcmpi(id,"no5"))
			p = "NO5";
		else if(!lstrcmpi(id,"chuck_chat"))
			p = "chuck_chat";
		else if(!lstrcmpi(id,"ymlite"))
			p = "ymlite";
	}
	return p;
}


// CUserList

CUserList::CUserList()
{
	//
}

CUserList::~CUserList()
{
	RemoveAll();
}

/*
void CUserList::AddUsers(const CYahooChatUsers &users)
{
	int i;
	const int count = users.GetSize();

	for(i=0;i<count;i++){
		AddUser(users[i]);
	}
}

CUser * CUserList::AddUser(const CYahooChatUser &user)
{
	CUser *p = new CUser();

	p->m_name = user.m_name;
	p->m_cam = user.m_webcam;
	m_users.Add(p);
	return p;
}
*/

CUser * CUserList::AddUser(const CUser &user)
{
	CUser *p = new CUser(user);

#ifdef _DEBUG
	CUser *pFound = GetUser(user.m_name);
	if(pFound)
		ATLTRACE("CUserList::AddUser adding %s but name already there!\n",LPCTSTR(user.m_name));
#endif
	m_users.Add(p);
	return p;
}

CUser * CUserList::AddUser(LPCTSTR name)
{
	CUser *p = new CUser();

#ifdef _DEBUG
	CUser *pFound = GetUser(name);
	if(pFound)
		ATLTRACE("CUserList::AddUser adding %s but name already there!\n",name);
#endif
	p->m_name = name;
	m_users.Add(p);
	return p;
}

void CUserList::RemoveUser(LPCSTR name)
{
	CUser *p = GetUser(name);

	if(p){
		BOOL res = m_users.Remove(p);
		ATLASSERT(res);
	}
}

void CUserList::RemoveAll(void)
{
	m_users.RemoveAll();
}


int CUserList::GetSize(void) const
{
	return m_users.GetSize();
}

CUser * CUserList::GetUser(LPCSTR name)
{
	int i;
	const int count = m_users.GetSize();
	CUser *p = NULL;

	for(i=0;i<count;i++){
		p = m_users[i];
		if(!lstrcmpi(name,p->m_name))
			break;
	}
	if(i >= count)
		p = NULL;
	return p;
}

CUser * CUserList::GetUser(int i)
{
	return m_users[i];
}

NO5TL::CPointerArray<CUser> & CUserList::GetUsers(void)
{
	return m_users;
}

CUser * CUserList::GetUserInChat(LPCTSTR name)
{
	int i;
	const int count = m_users.GetSize();
	CUser *p = NULL;

	for(i=0;i<count;i++){
		p = m_users[i];
		if(!lstrcmpi(name,p->m_name) && p->m_chat)
			break;
	}
	if(i >= count)
		p = NULL;
	return p;
}

int CUserList::GetChatCount(void) const
{
	int i;
	const int count = m_users.GetSize();
	int chat = 0;

	for(i=0;i<count;i++){
		if(m_users[i]->m_chat)
			chat++;
	}
	return chat;
}

void CUserList::CleanUp(void)
{
	int i;
	int count = m_users.GetSize();
	CUser *p = NULL;
	DWORD dwNow = GetTickCount();

	for(i=0;i<count;){
		p = m_users[i];
		// we dont remove users in chat,pm,follow,ignore(?)
		if(! ( p->m_chat || p->m_pm || p->m_follow || p->IsIgnored() ) ){
			if(dwNow - p->m_dwLastAction > ( 60 * 1000 )){
				m_users.Remove(p);
				count--;
			}
			else
				i++;
		}
		else
			i++;
	}
}

void CUserList::SetChat(bool bSet)
{
	int i;
	const int count = m_users.GetSize();
	CUser *p = NULL;

	for(i=0;i<count;i++){
		m_users[i]->m_chat = bSet;
	}
}

// CBuddy
void CBuddy::Reset(void)
{
	m_name.Empty();
	m_msg.Empty();
	m_status = NO5YAHOO::YAHOO_STATUS_OFFLINE;
}

// CBuddyGroup
void CBuddyGroup::CopyFrom(const CBuddyGroup &rhs)
{
	if(this != &rhs){
		m_name = rhs.m_name;
		m_buddies.RemoveAll();
		NO5TL::CopySimpleArray(m_buddies,rhs.m_buddies);
	}
}

CBuddy * CBuddyGroup::FindBuddy(LPCTSTR name)
{
	const int count = m_buddies.GetSize();
	int i;
	CBuddy *p = NULL;

	for(i=0;i<count;i++){
		p = m_buddies[i];
		if(p->m_name.CompareNoCase(name) == 0)
			break;
		else
			p = NULL;
	}
	return p;
}

// CBuddyList
void CBuddyList::CopyFrom(const CBuddyList &rhs)
{
	if(this != &rhs){
		RemoveAll();
		NO5TL::CopySimpleArray(*this,rhs);
	}
}

CBuddyGroup * CBuddyList::FindGroup(LPCTSTR group)
{
	CBuddyGroup *p = NULL;
	const int count = GetSize();

	for(int i=0;i<count;i++){
		p = operator[](i);
		if(p->m_name.CompareNoCase(group) == 0)
			break;
		else
			p = NULL;
	}
	return p;
}

CBuddy * CBuddyList::FindBuddy(LPCTSTR group,LPCTSTR name)
{
	CBuddyGroup *p = FindGroup(group);
	CBuddy *q = NULL;

	if(p){
		q = p->FindBuddy(name);
	}
	return q;
}

CBuddy * CBuddyList::FindBuddy(LPCTSTR name)
{
	CBuddyGroup *pg = NULL;
	const int count = GetSize();
	CBuddy *pb = NULL;

	for(int i=0;i<count;i++){
		pg = operator[](i);
		pb = FindBuddy(pg->m_name,name);
		if(pb)
			break;
	}
	return pb;
}

CBuddyGroup * CBuddyList::AddBuddies(LPCTSTR group,const CSimpleArray<CString> &buddies)
{
	CBuddyGroup *pGroup = FindGroup(group);

	if(!pGroup){
		pGroup = new CBuddyGroup();
		pGroup->m_name = group;
		Add(pGroup);
	}

	for(int i=0;i<buddies.GetSize();i++){
		CBuddy *pBuddy = pGroup->FindBuddy(buddies[i]);
		if(!pBuddy){
			pBuddy = new CBuddy();
			pBuddy->m_name = buddies[i];
			pGroup->m_buddies.Add(pBuddy);
		}
	}
	return pGroup;
}

CBuddy * CBuddyList::AddBuddy(LPCTSTR group,const CBuddy &buddy)
{
	CBuddyGroup *pGroup = FindGroup(group);

	if(!pGroup){
		pGroup = new CBuddyGroup();
		pGroup->m_name = group;
		Add(pGroup);
	}
	CBuddy *pBuddy = pGroup->FindBuddy(buddy.m_name);
	if(!pBuddy){
		pBuddy = new CBuddy(buddy);
		pGroup->m_buddies.Add(pBuddy);
	}
	return pBuddy;
}

void CBuddyList::RemoveBuddy(LPCTSTR group,LPCTSTR name)
{
	CBuddyGroup *pGroup = FindGroup(group);

	if(pGroup){
		CBuddy *pBuddy = pGroup->FindBuddy(name);
		if(pBuddy){
			BOOL res = pGroup->m_buddies.Remove(pBuddy);
			ATLASSERT(res);
		}
	}
}

void CBuddyList::SetBuddyStatus(LPCTSTR name,LONG status,LPCTSTR msg)
{
	CBuddyGroup *pg;
	int count = GetSize();
	int i;
	CBuddy *pb;

	for(i=0;i<count;i++){
		pg = operator[](i);
		pb = pg->FindBuddy(name);
		if(pb){
			pb->m_status = status;
			pb->m_msg = ( msg ? msg : "" );
		}
	}
}