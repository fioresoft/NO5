// YahooChat.h: interface for the CYahooChat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YAHOOCHAT_H__931F8916_3E55_11D9_A17B_96836E6F0742__INCLUDED_)
#define AFX_YAHOOCHAT_H__931F8916_3E55_11D9_A17B_96836E6F0742__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <no5tl\databuffer.h>

namespace NO5YAHOO
{
// constantes uteis

#define YCHT_PROTO_NAME	"YCHT"
#define YCHT_PROTO_VER	0x0000017D
#define YCHT_DELS		"\xc0\x80"
#define YCHT_DEF_SERVER	"jcs.chat.dcn.yahoo.com"
#define YCHT_DEF_PORT	8001
#define YCHT_DEF_ROOM	"canada:1"
#define YCHT_DEF_SIGNIN_SERVER	"login.yahoo.com"
//
#define NO5_SECOND	1000 // ms
#define NO5_MINUTE	( 60 * NO5_SECOND)

typedef enum tagPACKETS
{
	PKT_LOGIN = 		0x00000001,
	PKT_LOGOUT =	 	0x00000002,
	PKT_ROOMIN =		0x00000011,
	PKT_ROOMOUT	=		0x00000012,
	PKT_INVITE =		0x00000017,
	PKT_AWAY =			0x00000021,
	PKT_GOTO =			0x00000025,
	PKT_CONFIG =		0x00000026,
	PKT_SAY	=			0x00000041,	// 'A'
	PKT_THINK =			0x00000042,	// 'B'
	PKT_EMOTION =		0x00000043,
	PKT_ADVERTISE =		0x00000044, // 
	PKT_PM =			0x00000045,
	PKT_TIME =			0x00000046,
	PKT_DICE =			0x00000047,
	PKT_PING =			0x00000062, // 'b'
	PKT_HELP =			0x00000064,
	PKT_BUDDY =			0x00000068,
	PKT_EMAIL =			0x00000069,
	PKT_YMSG =			0x00000070, // ? confirmar
	PKT_CMD =			0x00000071, // commands like goto
	PKT_GRAFFITI =		0x00000080, // confirmar
}PACKETS;


typedef enum tagRoomStatus{
	ROOM_OK =		0x00,
	ROOM_FULL =		0x01,
	ROOM_SAME =		0x10,	// tried to join same room or goto yourself
	ROOM_SECURED =	0xDF,
	ROOM_ISNTON =	0xF5,	// user u tried to goto or to invite isnt online
	ROOM_INVALID =	0xFA,	// room doesnt exist, invalid room
}ROOMSTATUS;

enum yahoo_status {
	YAHOO_STATUS_AVAILABLE = 0,
	YAHOO_STATUS_BRB,
	YAHOO_STATUS_BUSY,
	YAHOO_STATUS_NOTATHOME,
	YAHOO_STATUS_NOTATDESK,
	YAHOO_STATUS_NOTINOFFICE,
	YAHOO_STATUS_ONPHONE,
	YAHOO_STATUS_ONVACATION,
	YAHOO_STATUS_OUTTOLUNCH,
	YAHOO_STATUS_STEPPEDOUT,
	YAHOO_STATUS_INVISIBLE = 12,
	YAHOO_STATUS_CUSTOM = 99,
	YAHOO_STATUS_IDLE = 999,
	YAHOO_STATUS_ONLINE = 0x5a55aa55,
	YAHOO_STATUS_OFFLINE = 0x5a55aa56, /* don't ask */
	YAHOO_STATUS_TYPING = 0x16,			// webcam invite too
};
#define YAHOO_STATUS_GAME	0x2 		/* Games don't fit into the regular status model */

typedef enum tagTextStatus{
	TEXT_OK			= 0x00,
	TEXT_TOOLONG	= 0xf6, // ou outras mensagens do tipo "goodwill blah"
	TEXT_GENERAL_CHAT_ERROR = 0xff,
}TEXTSTATUS;

typedef enum tagInviteStatus
{
	INVITE_OK	= 0x01,
	INVITE_BAD	= 0xf5,
}INVITESTATUS;

typedef enum tagPmStatus
{
	PM_OK = 0x1,
	PM_USEROFF = 0x02,	// checar
} PMSTATUS;

// forward declarations
struct IYahooChatEvents;
struct IYahooChat;
class  CYahooChatUser;
class  CYahooChatUsers;
struct YahooChatPacketHeader;


struct IYahooChat
{
	virtual BOOL SignIn(LPCSTR server,int port,LPCSTR host,LPCSTR name,LPCSTR pw) = 0;
	virtual BOOL SignIn(SOCKADDR_IN &addr, LPCSTR host,LPCSTR name,LPCSTR pw) = 0;
	virtual BOOL Init(SOCKADDR_IN &addr) = 0;
	virtual BOOL Init(LPCSTR server,int port) = 0;
	virtual void Term(bool bAbrupt = false) = 0;
	virtual void Login(LPCSTR name = NULL,LPCSTR cookie = NULL) = 0;
	virtual void Logout(void) = 0;
	virtual void JoinRoom(LPCSTR room) = 0;
	virtual void LeaveRoom(LPCSTR room = NULL) = 0;
	virtual void SendText(LPCSTR msg) = 0;
	virtual void SendPm(LPCSTR to,LPCSTR msg) = 0;
	virtual void SendEmote(LPCSTR msg) = 0;
	virtual void SendCommand(LPCSTR cmd,LPCSTR msg) = 0;
	virtual void Invite(LPCSTR name) = 0;
	virtual void SetStatus(yahoo_status status,LPCSTR msg) = 0;
	virtual void Think(LPCSTR msg) = 0;
	virtual void Goto(LPCSTR user) = 0;
	virtual void Ping(void) = 0;
	virtual void KeepUserList(bool bKeep) = 0;
	//
	virtual LPCTSTR GetName(void) const = 0;
	virtual LPCTSTR GetCookie(void) const = 0;
	virtual LPCTSTR GetRoom(void) const = 0;
	virtual LPCTSTR GetRoomSpace(void) const = 0;
	virtual LPCTSTR GetRoomCategory(void) const = 0;
	virtual LPCTSTR GetVoiceCookie(void) const = 0;
	virtual LPCTSTR GetCamCookie(void) const = 0;
	virtual BOOL IsVoiceEnabled(void) const = 0;
	virtual CYahooChatUsers & GetUserList(void) = 0;
	virtual void GetDesiredRoom(CString &room,CString &rmid) = 0;
	virtual void GetRemoteAddress(CString &server,int &port) = 0;
	virtual void GetLocalAddress(CString &server,int &port) = 0;
	virtual BOOL IsConnecting(void) const = 0;
	virtual BOOL IsConnected(void) const = 0;
	virtual BOOL IsLogginIn(void) const = 0;
	virtual BOOL IsLoggedIn(void) const = 0;
	virtual BOOL IsLogginOut(void) const = 0;
	virtual BOOL IsJoiningRoom(void) const = 0;
	virtual BOOL IsInRoom(void) const = 0;
};

BOOL YahooChatCreate(IYahooChat **ppYahooChat,IYahooChatEvents *pYahooChatEvents);
void YahooChatDestroy(IYahooChat **ppYahooChat);

struct IYahooChatEvents
{
	virtual void OnSignIn(LPCSTR cookie) = 0;
	virtual void OnLogin(LPCSTR name,LPCSTR badwords)=0;
	virtual void OnLogoff(int status,LPCSTR msg) = 0;
	virtual void OnRoomIn(LPCSTR room,LPCSTR slogan,CYahooChatUser &user) =0;
	virtual void OnRoomOut(LPCSTR room,LPCSTR name)=0;
	virtual void OnJoinRoom(LPCSTR room,LPCSTR slogan,LPCSTR cookies,
		CYahooChatUsers &users,LPCSTR wc_cookie) = 0;
	virtual void OnRoomFull(LPCSTR msg,LPCSTR alt_room) = 0;
	virtual void OnJoinFailed(unsigned short status,LPCSTR msg)=0;
	virtual void OnText(LPCSTR room,LPCSTR name,LPCSTR msg,
		LPCSTR extra)=0;
	virtual void OnInvited(LPCSTR msg,LPCSTR room,LPCSTR name) = 0;
	virtual void OnAway(LPCSTR name,LPCSTR room,LPCSTR number,
		LPCSTR msg) = 0;
	virtual void OnThinking(LPCSTR room,LPCSTR name,LPCSTR msg) = 0;
	virtual void OnEmotion(LPCSTR room,LPCSTR name,
		LPCSTR emote,LPCSTR extra) =0;
	virtual void OnPm(LPCSTR from,LPCSTR to,LPCSTR msg)=0;
	virtual void OnBuddy(LPCSTR msg) =0;
	virtual void OnDice(LPCSTR name,LPCSTR msg) =0;
	virtual void OnAdvertise(LPCSTR msg) =0;
	virtual void OnServerMsg(LPCSTR msg,unsigned long packet,
		unsigned long status) =0;
	virtual void OnTime(LPCSTR msg) = 0;
	virtual void OnHelp(LPCSTR msg) = 0;

	// ISocketEvents
	virtual void OnSockConnect(int iError) = 0;
	virtual void OnSockConnecting(void) = 0;
	virtual void OnSockClose(int iError) = 0;
	virtual void OnSockConnectTimeout(void) = 0;
	virtual void OnSockError(int error) = 0;
	virtual void OnSockResolvingAddress(void) = 0;
	virtual void OnSockAddressResolved(int error) = 0;
};

struct YahooChatPacketHeader
{
	u_long m_proto;		// ycht
	u_long m_version;	//
	u_long m_packet;
	u_short m_status;
	u_short m_size;
public:
	YahooChatPacketHeader(void)
	{
		::ZeroMemory(this,sizeof(*this));
	}
	u_long GetVersion(void) const
	{
		return ::ntohl(m_version);
	}
	u_long GetPacket(void) const
	{
		return ::ntohl(m_packet);
	}
	u_short GetStatus(void) const
	{
		return ::ntohs(m_status);
	}
	u_short GetSize(void) const
	{
		return ::ntohs(m_size);
	}
	void SetProto(const char *proto)
	{
		m_proto = *((u_long *)proto);
	}
	void SetVersion(u_long version)
	{
		m_version = ::htonl(version);
	}
	void SetPacket(u_long packet)
	{
		m_packet = ::htonl(packet);
	}
	void SetStatus(u_short status)
	{
		m_status = ::htons(status);
	}
	void SetSize(u_short size)
	{
		m_size = ::htons(size);
	}
};

class CYahooChatUser
{
	void CopyFrom(CYahooChatUser &user);
public:
	CString m_name;
	bool m_webcam;
	CSimpleValArray<CString> m_dunno;
public:
	CYahooChatUser()
	{
		//
	}
	CYahooChatUser(CYahooChatUser &user)
	{
		CopyFrom(user);
	}
	void Parse(LPCTSTR s);

	CYahooChatUser & operator=(CYahooChatUser &user)
	{
		if(this != &user){
			CopyFrom(user);
		}
		return *this;
	}
	bool operator == (const CYahooChatUser &user)
	{
		return m_name == user.m_name;
	}
	bool operator != (const CYahooChatUser &user)
	{
		return !(*this == user);
	}
};

class CYahooChatUsers : public CSimpleArray<CYahooChatUser>
{
public:
	// string from OnJoinRoom
	void Parse(LPCTSTR s);
	// string from OnRoomIn - returns the name of the user
	CYahooChatUser HandleOnUserEnter(LPCSTR msg);
	// string from OnRoomOut - returns the name of the user
	CYahooChatUser HandleOnUserLeave(LPCSTR msg);
};

template <class TPacketHeader>
class CPacketBufferT : public NO5TL::CDataBuffer
{
public:
	// number of bytes to read in next socket call
	int ToRead(void)
	{
		int res;

		if(GetDataLen() < (int)sizeof(TPacketHeader)){
			res = (int)sizeof(TPacketHeader) - GetDataLen();
		}
		else{
			TPacketHeader *p = (TPacketHeader *)GetData();
			int wehave = GetDataLen() - sizeof(TPacketHeader);
			int weneed = (int)p->GetSize();
			res = weneed - wehave;
			ATLASSERT(res >= 0);
		}
		return res;
	}
};



} // NO5YAHOO

#endif // !defined(AFX_YAHOOCHAT_H__931F8916_3E55_11D9_A17B_96836E6F0742__INCLUDED_)
