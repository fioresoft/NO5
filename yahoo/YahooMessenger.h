// YahooMessenger.h: interface for the CYahooMessenger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YAHOOMESSENGER_H__859FCE24_4959_11D9_A17B_BD378004423E__INCLUDED_)
#define AFX_YAHOOMESSENGER_H__859FCE24_4959_11D9_A17B_BD378004423E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLBASE_H__
#error yahoomessenger.h requires atlbase.h
#endif

#include <no5tl\no5tlbase.h>
#include <no5tl\databuffer.h>
#include "yahoochat.h"

#define YMSG_PROTO_NAME	"YMSG"
#define YMSG_PROTO_VER	0x00000E00
#define YMSG_DELS		"\xc0\x80"
#define YMSG_DEF_SERVER	"scs.msg.yahoo.com"
#define YMSG_DEF_PORT	5050
#define YMSG_DEF_ROOM	"canada:1"

/*
			  dec               binario		 octal
----   ---  -----   -------------------		------
nada		01024	0 00000100 00000000		002000
male		33792	0 10000100 00000000		102000
fem		 	66560	1 00000100 00000000		202000
nada	cam	01040	0 00000100 00010000		002020
fem		cam 66576	1 00000100 00010000		202020
male	cam	33808	0 10000100 00010000		102020

				  10  FEDCBA98 76543210	
				  
masks:	  dec		    hex	   oct
-----	-----		-------	------
nada:	01024		0x00400	002000
male:	32768		0x08000	100000
fem:	65536		0x10000	200000
cam:	00016		0x00010	000020

*/


namespace NO5YAHOO
{
using NO5TL::CNo5SimpleMap;
using NO5TL::CDataBuffer;

enum yahoo_service { /* these are easier to see in hex */
	YAHOO_SERVICE_LOGON = 1,
	YAHOO_SERVICE_LOGOFF,
	YAHOO_SERVICE_ISAWAY,
	YAHOO_SERVICE_ISBACK,
	YAHOO_SERVICE_IDLE, /* 5 (placemarker) */
	YAHOO_SERVICE_MESSAGE,	// pm from ymsg user  ( while YAHOO_SERVICE_CHATMSG is from ycht?)
	YAHOO_SERVICE_IDACT,
	YAHOO_SERVICE_IDDEACT,
	YAHOO_SERVICE_MAILSTAT,
	YAHOO_SERVICE_USERSTAT, /* 0xa */
	YAHOO_SERVICE_NEWMAIL,
	YAHOO_SERVICE_CHATINVITE,
	YAHOO_SERVICE_CALENDAR,
	YAHOO_SERVICE_NEWPERSONALMAIL,
	YAHOO_SERVICE_NEWCONTACT,		// in: 1,3,14
	YAHOO_SERVICE_ADDIDENT, /* 0x10 */
	YAHOO_SERVICE_ADDIGNORE,
	YAHOO_SERVICE_PING,
	YAHOO_SERVICE_GOTGROUPRENAME, /* < 1, 36(old), 37(new) */
	YAHOO_SERVICE_SYSMESSAGE = 0x14,
	YAHOO_SERVICE_PASSTHROUGH2 = 0x16,
	// YAHOO_SERVICE_CHATINVITEERROR,	conferir. 114 = err number	
	YAHOO_SERVICE_CONFINVITE = 0x18,
	YAHOO_SERVICE_CONFLOGON,
	YAHOO_SERVICE_CONFDECLINE,
	YAHOO_SERVICE_CONFLOGOFF,
	YAHOO_SERVICE_CONFADDINVITE,
	YAHOO_SERVICE_CONFMSG,
	YAHOO_SERVICE_CHATLOGON,/* 0,1, 6(Y=... T=...) 2's */
	YAHOO_SERVICE_CHATLOGOFF,
	YAHOO_SERVICE_CHATMSG = 0x20,		// pm from ycht user ?
	YAHOO_SERVICE_GAMELOGON = 0x28, 
	YAHOO_SERVICE_GAMELOGOFF,
	YAHOO_SERVICE_GAMEMSG = 0x2a,
	YAHOO_SERVICE_FILETRANSFER = 0x46,
	YAHOO_SERVICE_VOICECHAT = 0x4A,
	YAHOO_SERVICE_NOTIFY,
	YAHOO_SERVICE_VERIFY,
	YAHOO_SERVICE_P2PFILEXFER,
	YAHOO_SERVICE_PEERTOPEER = 0x4F,	/* Checks if P2P possible */
	YAHOO_SERVICE_WEBCAM,
	YAHOO_SERVICE_AUTHRESP = 0x54,
	YAHOO_SERVICE_LIST,
	YAHOO_SERVICE_AUTH = 0x57,
	YAHOO_SERVICE_ADDBUDDY = 0x83,
	YAHOO_SERVICE_REMBUDDY,
	YAHOO_SERVICE_IGNORECONTACT,	/* > 1, 7, 13 < 1, 66, 13, 0*/
	YAHOO_SERVICE_REJECTCONTACT,
	YAHOO_SERVICE_GROUPRENAME = 0x89, /* > 1, 65(new), 66(0), 67(old) */
	YAHOO_SERVICE_CHATONLINE = 0x96, /* > 109(id), 1, 6(abcde) < 0,1*/
	YAHOO_SERVICE_CHATGOTO,
	YAHOO_SERVICE_CHATJOIN,	/* > 1 104-room 129-1600326591 62-2 */
	YAHOO_SERVICE_CHATLEAVE,
	YAHOO_SERVICE_CHATEXIT = 0x9b,
	YAHOO_SERVICE_CHATINVITE2 = 0x9d,	// >104,129,126,117,118,119
	YAHOO_SERVICE_CHATLOGOUT = 0xa0,
	YAHOO_SERVICE_CHATPING,				// 109
	YAHOO_SERVICE_COMMENT = 0xa8,
	YAHOO_SERVICE_ACCEPTREQ = 0xd6,	// 1(from) 5(to) 13(1) ou 4(from),5,13
};

enum YahooTextMode
{
	YMSG_TEXT_NORMAL = 1,
	YMSG_TEXT_EMOTE,
	YMSG_TEXT_BUBBLE,
};

enum YahooUserFlags
{
	USER_NOTHING =	0x00400,
	USER_FEMALE =	0x10000,
	USER_MALE =		0x08000,
	USER_CAM =		0x00010,
};




const char * GetServiceDesc(u_long service);
const char * GetKeyDesc(int key);

// forward declarations
struct  CYahooMsgUser;
struct	IYahooMessenger;
struct  IYahooMessengerEvents;
struct	YahooMsgPacketHeader;
class	CYahooMsgUsers;
class   CYahooMap;
class	CYahooMsgPacketIn;
class	CYahooMsgPacketOut;
class	CBuddyInfo;
class	CMessengerVersion;

struct IYahooMessenger
{
	virtual BOOL Init(SOCKADDR_IN &addr,LPCTSTR name,LPCTSTR pw) = 0;
	virtual BOOL Init(LPCSTR server,int port,LPCTSTR name,LPCTSTR pw) = 0;
	virtual void Term(bool bAbrupt) = 0;
	virtual void Verify(void) = 0;
	virtual void Auth(yahoo_status status) = 0;
	virtual void AuthResponse(yahoo_status status,bool ver11) = 0;
	virtual void SetStatus(yahoo_status status,LPCSTR msg = NULL) = 0;
	virtual void ChatOnline(void) = 0;
	virtual void ChatJoin(LPCSTR room,LPCSTR rmspace,bool bCam = false) = 0;
	virtual void ChatExit(void) = 0;
	virtual void ChatMessage(LPCSTR text,bool utf8,YahooTextMode mode) = 0;
	virtual void ChatLeave(void) = 0;
	virtual void ChatInvite(LPCTSTR to,LPCTSTR room = NULL,LPCTSTR roomid = NULL,
		LPCTSTR msg = NULL) = 0;
	virtual void SendMsg(LPCSTR from,LPCSTR to,LPCSTR msg,LPCSTR env = NULL) = 0;
	virtual void SendPm(LPCSTR from,LPCSTR to,LPCSTR msg,LPCSTR im_env = NULL) = 0;
	virtual void NotifyTyping(LPCSTR from,LPCSTR to,bool start) = 0;
	virtual void WebcamInvite(LPCTSTR to,LPCTSTR from = NULL,LPCTSTR msg = NULL) = 0;
	virtual void Ping(void) = 0;
	virtual void ChatPing(void) = 0;
	virtual void AddBuddy(LPCSTR from,LPCSTR buddy,LPCSTR group,LPCTSTR msg = NULL) = 0;
	virtual void RemoveBuddy(LPCSTR from,LPCSTR buddy,LPCSTR group) = 0;
	virtual void RejectBuddy(LPCSTR from,LPCSTR buddy,LPCSTR msg) = 0;
	virtual void AcceptReq(LPCTSTR from,LPCTSTR buddy,bool accept) = 0;
	virtual void IgnoreUser(LPCSTR from,LPCSTR user,bool ignore) = 0;
	virtual void RenameGroup(LPCSTR id,LPCSTR old_name,LPCSTR new_name) = 0;
	virtual void GotoUser(LPCSTR user) = 0;
	virtual void ChatLogout(void) = 0;
	virtual void Boot(LPCTSTR user) = 0;
	virtual void Logoff(void) = 0;
	virtual void Webcam(LPCTSTR to) = 0;
	//
	virtual void SetUTF8(bool bSet) = 0;
	virtual bool GetUTF8(void) const = 0;
	//
	virtual void SetBuildNumber(const CMessengerVersion &v) = 0;
	virtual const CMessengerVersion & GetBuildNumber(void) = 0;

	// accessors
	virtual LPCTSTR GetID(void) const = 0;
	virtual LPCTSTR GetName(void) const = 0;
	virtual LPCTSTR GetPw(void) const = 0;
	virtual LPCTSTR GetRoom(void) const = 0;
	virtual LPCTSTR GetRoomSpace(void) const = 0;
	virtual LPCTSTR GetRoomCategory(void) const = 0;
	virtual LPCTSTR GetCamServer(void) const = 0;
	virtual void GetDesiredRoom(CString &room,CString &rmid) = 0;
	virtual BOOL IsConnecting(void) const = 0;
	virtual BOOL IsConnected(void) const = 0;
	virtual bool IsJoiningRoom(void) const = 0;
	virtual bool IsInRoom(void) const = 0;
	virtual bool IsLoggedInChat(void) const = 0;
	virtual bool IsLoggedOn(void) const = 0;
	virtual void GetRemoteAddress(CString &server,int &port) = 0;
	virtual void GetLocalAddress(CString &server,int &port) = 0;
	virtual LPCTSTR GetCookie(TCHAR ch) = 0;
	virtual LPCTSTR GetCookie(int  i) = 0;

	virtual CYahooMsgPacketIn & GetPacketIn(void) = 0;
};

BOOL YahooMessengerCreate(IYahooMessenger **ppMsg,IYahooMessengerEvents *pMsgEvents);
void YahooMessengerDestroy(IYahooMessenger **ppMsg);

struct IYahooMessengerEvents
{
	virtual void OnUserLogon(u_long status,CSimpleArray<CBuddyInfo> &buddies,bool bMe) = 0;
	virtual void OnUserLogoff(u_long status,CSimpleArray<CBuddyInfo> &buddies) = 0;
	virtual void OnAuthenticate(u_long status,LPCSTR challenge,
		bool ver11)=0;
	virtual void OnAuthRespError(void) = 0;
	virtual void OnChatOnline(void) = 0;
	virtual void OnChatJoin(LPCSTR room,LPCSTR topic) = 0;
	virtual void OnText(LPCSTR user,LPCSTR msg,bool utf8) = 0;
	virtual void OnEmote(LPCSTR user,LPCSTR msg,bool utf8) = 0;
	virtual void OnThink(LPCSTR user,LPCSTR msg,bool utf8) = 0;
	virtual void OnUserLeaves(CYahooMsgUser &user) = 0;
	virtual void OnUserEnters(CYahooMsgUser &user) = 0;
	virtual void OnInstantMessage(LPCSTR from,LPCSTR to,LPCSTR msg,
		bool bUTF8,LPCSTR im_env,LPCSTR t) = 0;
	virtual void OnPrivateMessage(LPCSTR from,LPCSTR to,LPCSTR msg,bool utf8) = 0;
	virtual void OnTyping(LPCTSTR from,bool bStart) = 0;
	virtual void OnWebcamInvite(LPCTSTR from) = 0;
	virtual void OnWebcamInviteReply(LPCTSTR from,bool bAccepted) = 0;
	virtual void OnNotify(LPCSTR from,LPCSTR notify_string,LPCSTR status) = 0;
	virtual void OnAddBuddy(LPCSTR me,LPCSTR group,const CBuddyInfo &bi) = 0;
	virtual void OnRemBuddy(LPCTSTR me,LPCTSTR buddy,LPCTSTR group) = 0;
	virtual void OnRejectContact(u_long status,LPCTSTR me,LPCTSTR buddy,LPCTSTR msg) = 0;
	virtual void OnGroupRename(LPCTSTR me,LPCTSTR old,LPCTSTR name) = 0;
	virtual void OnGotGroupRename(LPCTSTR old,LPCTSTR name) = 0;
	virtual void OnIgnoreUser(LPCSTR who,LPCSTR me,bool ignore,LPCSTR answer) = 0;
	virtual void OnBuddyList(LPCTSTR group,CSimpleArray<CString> &buddies) = 0;
	virtual void OnIgnoreList(CSimpleArray<CString> &lst) = 0;
	virtual void OnAlternateIdList(CSimpleArray<CString> &lst) = 0;
	virtual void OnChatInvite2(LPCSTR room,LPCSTR rmspace,LPCSTR \
		from,LPCSTR to,LPCSTR msg) = 0;
	virtual void OnUserAway(LPCSTR user,int code,LPCSTR msg) = 0;
	virtual void OnUserBack(LPCSTR user,int code,LPCSTR msg) = 0;
	virtual void OnAuthError(LPCSTR msg,LPCSTR url) = 0;
	virtual void OnYahooError(u_long service,u_long status,
		CYahooMap &data) = 0;
	virtual void OnNewContact(u_long status,LPCTSTR to,LPCTSTR buddy,LPCTSTR msg) = 0;
	virtual void OnPing(u_long status) = 0;
	virtual void OnNewMail(int count) = 0;
	virtual void OnChatLogout(void) = 0;
	virtual void OnUserList(CYahooMsgUsers &users,bool bMore) = 0;
	virtual void OnWebcam(LPCTSTR cookie,LPCTSTR server) = 0;
	// unknown events
	virtual void OnChatLogon(u_long status,CYahooMap &map) = 0;
	virtual void OnChatLogoff(u_long status,CYahooMap &map) = 0;
	virtual void OnSysMessage(u_long status,CYahooMap &map) = 0;
	virtual void OnIdle(u_long status,CYahooMap &map) = 0;
	virtual void OnIdActivate(u_long status,CYahooMap &map) = 0;
	virtual void OnIdDeactivate(u_long status,CYahooMap &map) = 0;
	virtual void OnMailStat(u_long status,CYahooMap &map) = 0;
	virtual void OnUserStat(u_long status,CYahooMap &map) = 0;
	virtual void OnChatInvite(u_long status,CYahooMap &map) = 0;
	virtual void OnCalendar(u_long status,CYahooMap &map) = 0;
	virtual void OnNewPersonalMail(u_long status,CYahooMap &map) = 0;
	virtual void OnAddIdent(u_long status,CYahooMap &map) = 0;
	virtual void OnAddIgnore(u_long status,CYahooMap &map) = 0;
	virtual void OnPassThrough(u_long status,CYahooMap &map) = 0;
	virtual void OnGameLogon(u_long status,CYahooMap &map) = 0;
	virtual void OnGameLogoff(u_long status,CYahooMap &map) = 0;
	virtual void OnGameMsg(u_long status,CYahooMap &map) = 0;
	virtual void OnFileTransfer(u_long status,CYahooMap &map) = 0;
	virtual void OnVoiceChat(u_long status,CYahooMap &map) = 0;
	virtual void OnVerify(u_long status,CYahooMap &map) = 0;
	virtual void OnP2PFileXfer(u_long status,CYahooMap &map) = 0;
	virtual void OnPeer2Peer(u_long status,CYahooMap &map) = 0;
	virtual void OnChatGoto(u_long status,CYahooMap &map) = 0;
	virtual void OnChatLeave(u_long status,CYahooMap &map) = 0;
	virtual void OnChatPing(u_long status,CYahooMap &map) = 0;
	virtual void OnAcceptReq(u_long status,CYahooMap &map) = 0;

	//
	virtual void OnUnknownService(u_short service,u_long status,
		CYahooMap &data) = 0;

	// ISocketEvents
	virtual void OnSockConnect(int iError) = 0;
	virtual void OnSockConnecting(void) = 0;
	virtual void OnSockClose(int iError) = 0;
	virtual void OnSockConnectTimeout(void) = 0;
	virtual void OnSockError(int error) = 0;
	virtual void OnSockResolvingAddress(void) = 0;
	virtual void OnSockAddressResolved(int error) = 0;

};

class CYahooMap : public CNo5SimpleMap<int,CString>
{
	typedef CNo5SimpleMap<int,CString> _BaseClass;
public:
	CYahooMap(void):_BaseClass(true /*multimap*/)
	{
		//
	}
	CString MakePair(int i) const;

	BOOL Add(int key,LPCSTR val)
	{
		return _BaseClass::Add(key,CString(val));
	}
	BOOL Add(int key,int val)
	{
		char buf[20] = {0};
		wsprintf(buf,"%d",val);
		return Add(key,buf);
	}
	CString Lookup(int key) const
	{
		int nIndex = FindKey(key);
		if(nIndex == -1)
			return CString();   
		return GetValueAt(nIndex);
	}
	void CopyFrom(CYahooMap &m)
	{
		_BaseClass::CopyFrom(m);
	}
};

struct YahooMsgPacketHeader
{
	u_long	m_proto;		// ycht
	u_long	m_version;	//
	u_short m_size;
	u_short m_service;
	u_long	m_status;
	u_long	m_sessionid;
public:
	YahooMsgPacketHeader(void)
	{
		::ZeroMemory(this,sizeof(*this));
	}
	u_long GetVersion(void) const
	{
		return ::ntohl(m_version);
	}
	u_short GetSize(void) const
	{
		return ::ntohs(m_size);
	}
	u_short GetService(void) const
	{
		return ::ntohs(m_service);
	}
	u_long GetStatus(void) const
	{
		return ::ntohl(m_status);
	}
	u_long GetSessionID(void) const
	{
		return ::ntohl(m_sessionid);
	}
	
	void SetProto(const char *proto)
	{
		m_proto = *((u_long *)proto);
	}
	void SetVersion(u_long version)
	{
		m_version = ::htonl(version);
	}
	void SetSize(u_short size)
	{
		m_size = ::htons(size);
	}
	void SetService(u_short service)
	{
		m_service = ::htons(service);
	}
	void SetStatus(u_long status)
	{
		m_status = ::htonl(status);
	}
	void SetSessionID(u_long sessionid)
	{
		m_sessionid = ::htonl(sessionid);
	}
	
};


struct CYahooMsgUser
{
private:
	void CopyFrom(const CYahooMsgUser &user);
public:
	CString m_name;
	CString m_nick;
	CString m_location;
	DWORD	m_flags;
	short	m_age;
	//
	CYahooMsgUser()
	{
		m_age = -1;
		m_flags = 0;
	}
	CYahooMsgUser(const CYahooMsgUser &user)
	{
		CopyFrom(user);
	}
	void Reset(void)
	{
		m_name = "";
		m_nick = "";
		m_location = "";
		m_flags = 0;
		m_age = 0;
	}
	CYahooMsgUser & operator=(const CYahooMsgUser &user)
	{
		CopyFrom(user);
		return *this;
	}
	bool operator==(const CYahooMsgUser &user)
	{
		return !m_name.CompareNoCase(user.m_name);
	}
	bool operator==(LPCTSTR name)
	{
		return !m_name.CompareNoCase(name);
	}
	bool operator != (const CYahooMsgUser &user)
	{
		return !(*this == user);
	}
	bool operator != (LPCTSTR name)
	{
		return !(*this == name);
	}
	bool Male(void) const
	{
		return (m_flags & USER_MALE) != 0;
	}
	bool Female(void) const
	{
		return (m_flags & USER_FEMALE) != 0;
	}
	bool Cam(void) const
	{
		return (m_flags & USER_CAM) != 0;
	}

};

class CYahooMsgUsers : public CSimpleArray<CYahooMsgUser>
{
public:
	BOOL RemoveUser(LPCSTR name);
	BOOL HasUser(LPCTSTR name);
};

class CYahooMsgPacketIn
{
public:
	CYahooMsgPacketIn();
	YahooMsgPacketHeader m_header;
	CYahooMap m_data;
private:
	//
	int ParseBody(char *p);
public:
	void UnBuild(CDataBuffer *buf);
};

class CYahooMsgPacketOut
{
private:
	CYahooMsgPacketOut & CopyFrom(CYahooMsgPacketOut &p);
public:
	CYahooMsgPacketOut(void);
	CYahooMsgPacketOut(CYahooMsgPacketOut &p);
	CYahooMsgPacketOut & operator = ( CYahooMsgPacketOut &p);
	~CYahooMsgPacketOut();
	CDataBuffer * Build(u_short service,u_long status,u_long session_id,CYahooMap &args);
	//
	CDataBuffer m_buf;
	YahooMsgPacketHeader m_header;
	CYahooMap m_data;
	//
	static u_short CalcSize(const CYahooMap &data);
	CDataBuffer * GetData(void);
};

// when we or a buddy logs on , we receive info of the buddy
// there are much more info passed, but i dont know the meaning
class CBuddyInfo
{
public:
	CBuddyInfo(void)
	{
		Reset();
	}
	CBuddyInfo(const CBuddyInfo &bi)
	{
		Reset();
		CopyFrom(bi);
	}
	CBuddyInfo & operator = ( const CBuddyInfo &bi )
	{
		if(this != &bi){
			Reset();
			CopyFrom(bi);
		}
		return *this;
	}
	void Reset(void)
	{
		m_name = "";
		m_status = YAHOO_STATUS_OFFLINE;
		m_idle = 0;
	}
	CString m_name;
	int m_status;
	int m_idle;
private:
	void CopyFrom(const CBuddyInfo &bi)
	{
		m_name = bi.m_name;
		m_status = bi.m_status;
		m_idle = bi.m_idle;
	}
};

class CMessengerVersion
{
	unsigned short m_v[4];
	CMessengerVersion & CopyFrom(const CMessengerVersion &v)
	{
		if(this != &v)
			for(int i=0; i < sizeof(m_v)/sizeof(m_v[0]); i++)
				m_v[i] = v.m_v[i];
		return *this;
	}
public:
	CMessengerVersion()
	{
		//
	}
	CMessengerVersion(const CMessengerVersion &v)
	{
		CopyFrom(v);
	}
	CMessengerVersion(unsigned short version[])
	{
		int i;
		for(i=0;i<sizeof(m_v)/sizeof(m_v[0]);i++)
			m_v[i] = version[i];
	}
	CMessengerVersion & operator = ( const CMessengerVersion &v)
	{
		return CopyFrom(v);
	}
	bool IsEmpty() const
	{
		return m_v[0] == 0;
	}

	// "1, 2, 3, 4"
	CString GetString1(void)
	{
		CString s;
		s.Format("%hu, %hu, %hu, %hu",m_v[0],m_v[1],m_v[2],m_v[3]);
		return s;
	}
	// "1.2.3.4"
	CString GetString2(void)
	{
		CString s;
		s.Format("%hu.%hu.%hu.%hu",m_v[0],m_v[1],m_v[2],m_v[3]);
		return s;
	}
	// "ym1.2.3.4"
	CString GetString3(void)
	{
		CString s;
		s.Format("ym%hu.%hu.%hu.%hu",m_v[0],m_v[1],m_v[2],m_v[3]);
		return s;
	}
};
 

} // NO5YAHOO

#endif // !defined(AFX_YAHOOMESSENGER_H__859FCE24_4959_11D9_A17B_BD378004423E__INCLUDED_)
