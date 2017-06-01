#include "stdafx.h"
#include <time.h>
#include <no5tl\no5tlbase.h>
#include <no5tl\mystring.h>
#include <no5tl\dll.h>
#include <no5tl\winsocketwithhandler.h>
#include "yahoochat.h"
#include "yahoolib2_partial.h"	// authentication
#include "yahoomessenger.h"
//#include "convertUTF.h"
using namespace NO5TL;
using namespace NO5YAHOO;


struct YahooMsgPair
{
public:
	int m_key;
	CString m_value;
public:
	YahooMsgPair()
	{
		m_key = -1;
	}
	YahooMsgPair(const YahooMsgPair &p)
	{
		m_key = p.m_key;
		m_value = p.m_value;
	}
	YahooMsgPair & operator = (const YahooMsgPair &p)
	{
		if(this != &p){
			m_key = p.m_key;
			m_value = p.m_value;
		}
		return *this;
	}
	CString MakePair(void) const
	{
		CString res;

		res.Append(m_key);
		res += YMSG_DELS;
		if(m_value.GetLength()){
			res += m_value;
		}
		res += YMSG_DELS;
		return res;
	}
	void ParsePair(LPCSTR p)
	{
		CStringToken st;
		CString res;

		st.Init(p,YMSG_DELS);

		// key
		st.GetNext3(res);
		if(!res.IsEmpty()){
			m_key = atoi((LPCSTR)res);
		}
		else{
			m_key = -1;
			return;
		}
		// value
		st.GetNext3(m_value);
	}
	bool IsValid(void) const
	{
		return m_key >= 0;
	}
};

CString CYahooMap::MakePair(int i) const
{
	YahooMsgPair p;
	ATLASSERT(i >= 0 && i < GetSize());
	p.m_key = GetKeyAt(i);
	p.m_value = GetValueAt(i);
	return p.MakePair();
}

int CountDigits(int n,int base = 10)
{
	int q = n;
	int count = 0;

	if(q == 0)
		return 1;
	while(q != 0){
		q = q / base;
		count++;
	}

	return count;
}

struct YahooMsgStrings
{
	CString result6;
	CString result96;
};

//typedef int (WINAPI * PVENKYFUNC)(const char * id,const char * pw,
//	const char * seed,char *res1,char *res2);

BOOL GetYahooStrings(LPCSTR id,LPCSTR pw,LPCSTR seed,
							YahooMsgStrings &yms,bool ver11)
{
	char *_id = new char[lstrlen(id) + 1];
	char *_pw = new char[lstrlen(pw) + 1];
	char result6[120] = {0};
	char result96[120] = {0};

	ZeroMemory(_id,lstrlen(id) + 1);
	ZeroMemory(_pw,lstrlen(pw) + 1);
	lstrcpy(_id,id);
	lstrcpy(_pw,pw);

	if(!ver11){
		GetYahooStringsPreVer11(seed,id,_id,_pw,
			(unsigned char *)&result6[0],(unsigned char *)&result96[0]);
	}
	else{
		GetYahooStringsVer11(seed,id,_pw,&result6[0],&result96[0]);
	}
	yms.result6 = (char *)result6;
	yms.result96 = (char *)result96;

	delete []_id;
	delete []_pw;
	CString s;

	return (yms.result6.GetLength() && yms.result96.GetLength()) ? TRUE: FALSE;
}


// the yahoo messenger packet buffer
typedef CPacketBufferT<YahooMsgPacketHeader> CYmsgPacketBuffer;

// class CYahooMsgPacketOut

CYahooMsgPacketOut::CYahooMsgPacketOut(void)
{
	//
}

CYahooMsgPacketOut & CYahooMsgPacketOut::CopyFrom(CYahooMsgPacketOut &p)
{
	if(this != &p){
		m_buf.Reset();
		m_buf.Add(p.m_buf.GetData(),p.m_buf.GetDataLen());
		m_header = p.m_header;
		m_data.CopyFrom(p.m_data);
	}
	return *this;
}

CYahooMsgPacketOut::CYahooMsgPacketOut(CYahooMsgPacketOut &p)
{
	CopyFrom(p);
}

CYahooMsgPacketOut & CYahooMsgPacketOut::operator = (CYahooMsgPacketOut &p)
{
	return CopyFrom(p);
}

CYahooMsgPacketOut::~CYahooMsgPacketOut()
{
	m_buf.Destroy();
	m_data.RemoveAll();
}

u_short CYahooMsgPacketOut::CalcSize(const CYahooMap &data)
{
	const int count = data.GetSize();
	int i;
	u_short res = 0;

	for(i=0;i<count;i++){
		res += CountDigits(data.GetKeyAt(i));
		res += 2;	// '\xC0\x80'
		res += data.GetValueAt(i).GetLength();
		res += 2;	// '\xC0\x80'
	}
	return res;
}
	
CDataBuffer * CYahooMsgPacketOut::Build(u_short service,u_long status,u_long\
	session_id,CYahooMap &args)
{
	u_short size = CalcSize(args);
	const int count = args.GetSize();
	int i;

	m_buf.Reset();

	// make header
	m_header.SetProto(YMSG_PROTO_NAME);
	m_header.SetVersion(::htonl(YMSG_PROTO_VER));
	m_header.SetSize(size);
	m_header.SetService(service);
	m_header.SetStatus(status);
	m_header.SetSessionID(session_id);
	m_buf.Add(m_header);

	m_data.CopyFrom(args);

	// make body
	for(i=0;i<count;i++){
		m_buf.Add((LPCTSTR)args.MakePair(i));
	}
	return &m_buf;
}

CDataBuffer * CYahooMsgPacketOut::GetData(void)
{
	return &m_buf;
}

// class CYahooMsgPacketIn


CYahooMsgPacketIn::CYahooMsgPacketIn()
{
	//
}
	
int CYahooMsgPacketIn::ParseBody(char *p)
{
	CStringToken st;
	CString key,val;
	int iKey;
		
	m_data.RemoveAll();
	st.Init(p,YCHT_DELS);

	while(st.GetNext3(key) && !key.IsEmpty()){
		iKey = atoi(key);
		if(st.GetNext3(val)){
			// add even if val is empty
			m_data.Add(iKey,val);
		}
	}
	// there must be an even number of items
	return m_data.GetSize();
}

void CYahooMsgPacketIn::UnBuild(CDataBuffer *buf)
{
	char *p = (char *)buf->GetData();
	YahooMsgPacketHeader *pHeader = (YahooMsgPacketHeader *)p;
		
	ATLASSERT((DWORD)pHeader->GetSize() + (DWORD)sizeof(*pHeader) == \
		buf->GetDataLen());
		
	// copy header
	::CopyMemory(&m_header,pHeader,sizeof(m_header));
	// parse body filling the data map
	p = p + sizeof(m_header);
	ParseBody(p);
}

void CYahooMsgUser::CopyFrom(const CYahooMsgUser &user)
{
	if(this != &user){
		m_name = user.m_name;
		m_nick = user.m_nick;
		m_location = user.m_location;
		m_age = user.m_age;
		m_flags = user.m_flags;
	}

}

BOOL CYahooMsgUsers::RemoveUser(LPCSTR name)
{
	CYahooMsgUser user;
	const int size = GetSize();
	int i;
	BOOL res = FALSE;

	if(!name)
		return res;

	for(i=0;i<size;i++){
		user = m_aT[i];
		if(user == name){
			RemoveAt(i);
			res = TRUE;
			break;
		}
	}
	return res;
}

BOOL CYahooMsgUsers::HasUser(LPCTSTR name)
{
	CYahooMsgUser user;

	user.m_name = name;
	return Find(user) >= 0 ? TRUE : FALSE;
}

/**************** CYahooMessengerT *********************/

template <class TSock>
class CYahooMessengerT:protected ISocketEvents,public IYahooMessenger
{
public:
	CYahooMessengerT(IYahooMessengerEvents *pSink):\
		m_socket((ISocketEvents *)this),
		m_pSink(pSink)
	{
		Reset();
	}
	virtual ~CYahooMessengerT()
	{
		//
	}

	virtual BOOL Init(SOCKADDR_IN &addr,LPCTSTR name,LPCTSTR pw)
	{
		BOOL res;

		if(res = m_socket.CreateSocket()){
			if(res = m_socket.AsyncSelect(FD_READ|FD_WRITE|FD_CONNECT|\
				FD_CLOSE)){
				res = m_socket.Connect(&addr);
				if(res){
					m_name = name;
					m_pw = pw;
				}
			}
		}
		return res;
	}

	virtual BOOL Init(LPCSTR server,int port,LPCTSTR name,LPCTSTR pw)
	{
		BOOL res;

		if(res = m_socket.CreateSocket()){
			//BOOL opt = TRUE;
			//int err = setsockopt(m_socket.GetHandle(),IPPROTO_TCP,TCP_NODELAY,
			//	(const char *)&opt,sizeof(opt));
			//ATLASSERT(err != SOCKET_ERROR);
			if(res = m_socket.AsyncSelect(FD_READ|FD_WRITE|FD_CONNECT|\
				FD_CLOSE)){
				res = m_socket.Connect(server,port);
				if(res){
					m_name = name;
					m_pw = pw;
					m_id = name;
				}
			}
		}
		return res;
	}
	
	virtual void Term(bool bAbrupt)
	{
		if(bAbrupt || !m_socket.IsConnected()){
			if(m_socket.IsHandleValid()){
				m_socket.AsyncSelect(0);
				m_socket.CloseSocket();
				Reset();
			}
		}
		else {
			m_socket.Shutdown();
		}
	}

	virtual void Verify(void)
	{
		CSincro sincro(m_cs2);
		m_args.RemoveAll();
		SendThePacket(YAHOO_SERVICE_VERIFY);
	}

	virtual void Auth(yahoo_status status)
	{
		CSincro sincro(m_cs2);
		
		m_args.RemoveAll();
		// 1 has to be the id
		m_args.Add(1,m_name);
		// build and send
		SendThePacket(YAHOO_SERVICE_AUTH,status);
	}
	virtual void AuthResponse(yahoo_status status,bool ver11)
	{
		
		CSincro sincro(m_cs2);
		YahooMsgStrings yms;

		m_args.RemoveAll();
		GetYahooStrings(m_id,m_pw,(LPCSTR)m_challenge,yms,ver11);
		

		ATLASSERT(yms.result6 != yms.result96);
		// id (or name?)
		m_args.Add(0,m_name);
		// name
		m_args.Add(1,m_name);
		// name
		m_args.Add(2,m_name);
		// hash 6
		m_args.Add(6,yms.result6);
		// hash 96
		m_args.Add(96,yms.result96);
		// build
		ATLASSERT(!m_build.IsEmpty());
		m_args.Add(135,m_build.GetString2());
		
		SendThePacket(YAHOO_SERVICE_AUTHRESP,status);
	}

	virtual void SetStatus(yahoo_status status,LPCSTR msg = NULL)
	{
		CSincro sincro(m_cs2);
		char buf[5] = {0};

		u_short service = YAHOO_SERVICE_ISAWAY;

		if(status == YAHOO_STATUS_AVAILABLE){
			service = YAHOO_SERVICE_ISBACK;
		}
		if(status != YAHOO_STATUS_CUSTOM){
			msg = NULL;
		}
		m_args.RemoveAll();

		if(status > 9999)
			status = YAHOO_STATUS_AVAILABLE;

		wsprintf(buf,"%d",(int)status);
		// status code
		m_args.Add(10,buf);
		if(msg){
			if(m_utf8){
				m_args.Add(97,"1");
				m_args.Add(119,ToUTF8(msg));
			}
			else
				m_args.Add(119,msg);
			// away icon
			m_args.Add(47,"0");
		}
		SendThePacket(service);
	}

	virtual void ChatOnline(void)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		// name ?
		m_args.Add(1,m_name);
		m_args.Add(6,"abcde");
		ATLASSERT(!m_build.IsEmpty());
		m_args.Add(135,m_build.GetString3());
		m_args.Add(98,"us");
		m_args.Add(109,m_name);
		// build and send
		SendThePacket(YAHOO_SERVICE_CHATONLINE);
	}

	virtual void ChatJoin(LPCSTR room,LPCSTR rmspace,bool bCam)
	{
		CSincro sincro(m_cs2);
		time_t t;
		CString stime;

		time(&t);
		stime.Format("%10lu",(unsigned long)t);
		
		m_room = room;
		m_room_desired = room;
		m_rmid_desired = rmspace;
		m_args.RemoveAll();
		// name ?
		m_args.Add(1,m_name);
		// room
		m_args.Add(104,room);
		// rmspace
		if(rmspace && lstrlen(rmspace))
			m_args.Add(129,rmspace);

		m_args.Add(62,bCam ? "1" : "2");
		m_args.Add(24,stime);
		
		// build and send
		SendThePacket(YAHOO_SERVICE_CHATJOIN);
	}
	virtual void ChatExit(void)
	{
		CSincro sincro(m_cs2);

		if(m_room.IsEmpty())
			return;
		m_args.RemoveAll();
		// room-mode ( ?? )
		m_args.Add(62,"0");
		// name
		m_args.Add(1,m_name);
		// room
		m_args.Add(104,m_room);
		// name
		m_args.Add(109,m_name);
		SendThePacket(YAHOO_SERVICE_CHATEXIT);
	}

	virtual void ChatMessage(LPCSTR text,bool utf8,YahooTextMode mode)
	{
		CSincro sincro(m_cs2);
		
		m_args.RemoveAll();
		// active id
		m_args.Add(1,m_name);
		// utf 8
		if(utf8)
			m_args.Add(97,"1");
		// room
		m_args.Add(104,m_room);
		// msg
		if(utf8)
			m_args.Add(117,ToUTF8(text));
		else
			m_args.Add(117,text);
		// textmode
		char buf[2] = {0};
		wsprintf(buf,"%d",(int)mode);
		m_args.Add(124,buf);
		// build and send
		SendThePacket(YAHOO_SERVICE_COMMENT);

		// they dont send our text back, so simulate it here
		switch(mode){
			case YMSG_TEXT_EMOTE:
				m_pSink->OnEmote(m_name,text,utf8);
				break;
			case YMSG_TEXT_BUBBLE:
				m_pSink->OnThink(m_name,text,utf8);
				break;
			case YMSG_TEXT_NORMAL:
			default:
				m_pSink->OnText(m_name,text,utf8);
				break;
		}
	}

	virtual void SendMsg(LPCSTR from,LPCSTR to,LPCSTR msg,
		LPCSTR env = NULL)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		if(!env){
			env = ";0";
		}
		//m_args.Add(0,m_id);
		m_args.Add(1,from);
		m_args.Add(5,to);
		// private message text
		
		if(m_utf8){
			m_args.Add(97,"1");
			m_args.Add(14,ToUTF8(msg));
		}
		else
			m_args.Add(14,msg);
		m_args.Add(63,env);
		// dunno
		m_args.Add(64,"0");
		SendThePacket(YAHOO_SERVICE_MESSAGE);
	}
	virtual void SendPm(LPCSTR from,LPCSTR to,LPCSTR msg,
		LPCSTR im_env = NULL)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		m_args.Add(1,from);	// active id
		m_args.Add(5,to);	// to
		if(m_utf8){
			m_args.Add(97,"1");
			m_args.Add(14,ToUTF8(msg));
		}
		else
			m_args.Add(14,msg);
		m_args.Add(63,(im_env?im_env:";0"));
		SendThePacket(YAHOO_SERVICE_CHATMSG);
	}
	virtual void NotifyTyping(LPCSTR from,LPCSTR to,bool start)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		m_args.Add(4,from);					// from
		m_args.Add(5,to);					// to
		m_args.Add(13,(start?"1":"0"));
		// message
		m_args.Add(14," ");
		// notification
		m_args.Add(49,"TYPING");
		SendThePacket(YAHOO_SERVICE_NOTIFY,YAHOO_STATUS_TYPING);
	}
	virtual void WebcamInvite(LPCTSTR to,LPCTSTR from = NULL,LPCTSTR msg = NULL)
	{
		CSincro sincro(m_cs2);

		if(!from)
			from = GetName();
		if(!msg)
			msg = " ";
		m_args.RemoveAll();
		// notification
		m_args.Add(49,"WEBCAMINVITE");
		m_args.Add(1,from);					// from
		m_args.Add(5,to);					// to
		m_args.Add(13,"1");					// ?
		// message
		if(msg){
			if(m_utf8){
				m_args.Add(97,"1");
				m_args.Add(14,ToUTF8(msg));
			}
			else
				m_args.Add(14,msg);
		}
		SendThePacket(YAHOO_SERVICE_NOTIFY,YAHOO_STATUS_TYPING);
	}
	virtual void Ping(void)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		SendThePacket(YAHOO_SERVICE_PING);
	}
	virtual void ChatPing(void)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		m_args.Add(109,m_name);
		SendThePacket(YAHOO_SERVICE_CHATPING);
	}
	virtual void AddBuddy(LPCSTR from,LPCSTR buddy,LPCSTR group,LPCTSTR msg)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		m_args.Add(1,from);
		m_args.Add(7,buddy);
		m_args.Add(65,group);
		if(msg){
			if(m_utf8){
				m_args.Add(97,"1");
				m_args.Add(14,ToUTF8(msg));
			}
			else
				m_args.Add(14,msg);
		}
		
		SendThePacket(YAHOO_SERVICE_ADDBUDDY);
	}
	virtual void RemoveBuddy(LPCSTR from,LPCSTR buddy,LPCSTR group)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		m_args.Add(1,from);
		m_args.Add(7,buddy);
		m_args.Add(65,group);
		
		SendThePacket(YAHOO_SERVICE_REMBUDDY);
	}
	virtual void RejectBuddy(LPCSTR from,LPCSTR buddy,LPCSTR msg)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		m_args.Add(1,from);
		m_args.Add(7,buddy);
		if(m_utf8){
			m_args.Add(97,"1");
			m_args.Add(14,ToUTF8(msg));
		}
		else
			m_args.Add(14,msg);
		
		SendThePacket(YAHOO_SERVICE_REJECTCONTACT);
	}
	virtual void AcceptReq(LPCSTR from,LPCSTR buddy,bool accept)
	{
		CSincro sincro(m_cs2);
		char buf[2] = {0};


		m_args.RemoveAll();
		m_args.Add(1,from);
		m_args.Add(5,buddy);
		if(accept)
			buf[0] = '1';
		else
			buf[0] = 2;
		m_args.Add(13,buf);
		if(!accept){
			m_args.Add(14,"no thanks");
			m_args.Add(97,"0");
		}
		
		SendThePacket(YAHOO_SERVICE_ACCEPTREQ);
	}
	virtual void IgnoreUser(LPCSTR from,LPCSTR user,bool ignore)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		m_args.Add(1,from);
		m_args.Add(7,user);
		m_args.Add(13,(ignore?"1":"2"));
		
		SendThePacket(YAHOO_SERVICE_IGNORECONTACT);
	}
	virtual void RenameGroup(LPCSTR id,LPCSTR old_name,LPCSTR new_name)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		m_args.Add(1,id);
		m_args.Add(65,old_name);
		m_args.Add(67,new_name);
		
		SendThePacket(YAHOO_SERVICE_GROUPRENAME);
	}
	virtual void GotoUser(LPCSTR user)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		m_args.Add(109,user);
		m_args.Add(1,m_name);
		m_args.Add(62,"2");
		
		SendThePacket(YAHOO_SERVICE_CHATGOTO);
	}
	virtual void ChatLogout(void)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		m_args.Add(1,m_name);
		SendThePacket(YAHOO_SERVICE_CHATLOGOUT);
	}
	virtual void Boot(LPCTSTR user)
	{

	}
	virtual void Logoff(void)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		SendThePacket(YAHOO_SERVICE_LOGOFF);
	}
	virtual void ChatLeave(void)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();
		m_args.Add(0,m_name);
		m_args.Add(1,m_name);
		m_args.Add(104,m_room);
		SendThePacket(YAHOO_SERVICE_CHATLEAVE);
	}
	virtual void ChatInvite(LPCTSTR to,LPCTSTR room,LPCTSTR roomid,LPCTSTR msg)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();

		if(!room)
			room = m_room;
		if(!roomid)
			roomid = m_rmspace;
		if(!msg)
			msg = (LPCTSTR)m_topic;

		m_args.Add(1,m_name);
		m_args.Add(118,to);
		m_args.Add(104,room);
		m_args.Add(117,msg);
		m_args.Add(129,roomid);
		SendThePacket(YAHOO_SERVICE_CHATINVITE2);
	}
	// get users cam cookie and cam server
	virtual void Webcam(LPCTSTR to)
	{
		CSincro sincro(m_cs2);

		m_args.RemoveAll();

		m_args.Add(1,m_name);
		if(to && lstrlen(to))
			m_args.Add(5,to);
		SendThePacket(YAHOO_SERVICE_WEBCAM);
	}

	// accessors
	virtual LPCTSTR GetID(void) const
	{
		return m_id;
	}
	virtual LPCTSTR GetName(void) const
	{
		return m_name;
	}
	virtual LPCTSTR GetPw(void) const
	{
		return m_pw;
	}
	virtual LPCTSTR GetRoom(void) const
	{
		return m_room;
	}
	virtual LPCTSTR GetRoomSpace(void) const
	{
		return m_rmspace;
	}
	virtual LPCTSTR GetRoomCategory(void) const
	{
		return m_rmcat;
	}
	virtual LPCTSTR GetCamServer(void) const
	{
		return m_WebCamServer;
	}
	
	virtual void GetDesiredRoom(CString &room,CString &rmid)
	{
		room = m_room_desired;
		rmid = m_rmid_desired;
	}
	virtual BOOL IsConnecting(void) const
	{
		return m_socket.IsConnecting();
	}
	virtual BOOL IsConnected(void) const
	{
		return m_socket.IsConnected();
	}
	virtual bool IsJoiningRoom(void) const
	{
		return m_bJoiningRoom;
	}
	virtual bool IsInRoom(void) const
	{
		return m_bInRoom;
	}
	virtual bool IsLoggedInChat(void) const
	{
		return m_bLoggedInChat;
	}
	virtual bool IsLoggedOn(void) const
	{
		return m_bLoggedOn;
	}
	virtual CYahooMsgPacketIn & GetPacketIn(void)
	{
		return m_PacketIn;
	}
	virtual void SetUTF8(bool bSet)
	{
		m_utf8 = bSet;
	}
	virtual bool GetUTF8(void) const
	{
		return m_utf8;
	}
	virtual void GetRemoteAddress(CString &server,int &port)
	{
		if(IsConnected()){
			CSocketAddress addr;
			if(m_socket.GetRemoteAddress(addr)){
				server = addr.GetIP().ToString();
				port = addr.GetPort().Get(true);
			}
		}
	}
	virtual void GetLocalAddress(CString &server,int &port)
	{
		if(IsConnected()){
			CSocketAddress addr;
			if(m_socket.GetLocalAddress(addr)){
				server = addr.GetIP().ToString();
				port = addr.GetPort().Get(true);
			}
		}
	}
	virtual LPCTSTR GetCookie(TCHAR ch)
	{
		LPCTSTR res = NULL;
		switch(ch){
			case 'Y':
				res = (LPCTSTR) m_cookieY;
				break;
			case 'T':
				res = (LPCTSTR) m_cookieT;
				break;
			case 'C':
				res = (LPCTSTR) m_cookieC;
				break;
			default:
				break;
		}
		return res;
	}
	virtual LPCTSTR GetCookie(int i)
	{
		LPCTSTR res = NULL;
		switch(i){
			case 130:
				res = (LPCTSTR) m_cookie130;
				break;
			case 61:
				res = (LPCTSTR) m_cookie61;
				break;
			case 149:
				res = (LPCTSTR) m_cookie149;
				break;
			case 150:
				res = (LPCTSTR) m_cookie150;
				break;
			case 151:
				res = (LPCTSTR) m_cookie151;
				break;
			default:
				break;
		}
		return res;
	}
	virtual void SetBuildNumber(const CMessengerVersion &v)
	{
		m_build = v;
	}
	virtual const CMessengerVersion & GetBuildNumber(void)
	{
		return m_build;
	}

		
public:
	// ISocketEvents
	virtual void OnSockRead(int error)
	{
		//CSincro sincro(m_cs);
		static char buf[2048];
		int read;

		if(error){
			m_socket.AsyncSelect(0);
			m_socket.CloseSocket();
			ATLASSERT(error && "OnSockRead");
			return;
		}
		ZeroMemory(buf,sizeof(buf));
		read = m_socket.Recv(buf,min((sizeof(buf)),(m_buf.ToRead())));
		if(read != SOCKET_ERROR){
			m_buf.Add(buf,read);
			if(m_buf.ToRead() == 0){
				// parse m_buf
				m_PacketIn.UnBuild(&m_buf);
				// notify 
				Notify();
				m_buf.Reset();
			}
		}
		ATLASSERT(read != SOCKET_ERROR);
	}
	virtual void OnSockWrite(int error)
	{
		ATLASSERT(!error);

		if(!error){
			int count = m_out_queue.GetSize();
			bool res = true;

			while((count >0) && (m_socket.CanWrite()) && res){
				CDataBuffer *pdb;
				int sent;
				CYahooMsgPacketOut &pkt = m_out_queue[0];
				pdb = pkt.GetData();
				sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
				ATLASSERT(sent == (int)pdb->GetDataLen());
				res = ( sent == (int)pdb->GetDataLen() );
				if(res){
					m_out_queue.RemoveAt(0);
					count--;
					ATLTRACE("sending queued packet\n");
				}
			}
		}
	}
	virtual void OnSockConnect(int iError)
	{
		m_pSink->OnSockConnect(iError);
	}
	virtual void OnSockConnecting(void)
	{
		m_pSink->OnSockConnecting();
	}
	// always called in close events
	virtual void OnSockClose(int iError)
	{
		m_socket.AsyncSelect(0);
		m_socket.CloseSocket();
		m_pSink->OnSockClose(iError);
		Reset();
	}
	virtual void OnSockError(int error)
	{
		m_pSink->OnSockError(error);
	}
	virtual void OnSockConnectTimeout(void)
	{
		if(m_socket.IsHandleValid())
			m_socket.AsyncSelect(0);
		m_socket.CloseSocket();
		m_pSink->OnSockConnectTimeout();
		Reset();
	}
	virtual void OnSockResolvingAddress(void)
	{
		m_pSink->OnSockResolvingAddress();
	}
	virtual void OnSockAddressResolved(int error)
	{
		if(error){
			m_socket.AsyncSelect(0);
			m_socket.CloseSocket();
			Reset();
		}
		m_pSink->OnSockAddressResolved(error);
	}
	
private:
	virtual bool SendThePacket(u_short service,yahoo_status status = YAHOO_STATUS_AVAILABLE)
	{
		bool res = true;
		CDataBuffer *pdb;
		int sent;

		pdb = m_PacketOut.Build(\
			service,			// service
			status,				// 
			m_sessionid,		// sesion-id
			m_args);			// data

		if(m_socket.CanWrite()){
			sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
			ATLASSERT(sent == (int)pdb->GetDataLen());
			res = ( sent == (int)pdb->GetDataLen() );
		}
		else{
			m_out_queue.Add(m_PacketOut);
			ATLTRACE("queueing packet to send %d\n",m_out_queue.GetSize());
		}
		return res;
	}
	void Notify(void)
	{
		CYahooMap &data = m_PacketIn.m_data;
		u_short service = m_PacketIn.m_header.GetService();
		u_long status = m_PacketIn.m_header.GetStatus();

		ATLASSERT(m_pSink != NULL);
		
		if((long)status < 0){
			m_pSink->OnYahooError(service,status,data);
			return;
		}

		switch(service){
			case YAHOO_SERVICE_LOGON:
				_OnLogon(status,data);
				break;
			case YAHOO_SERVICE_LOGOFF:
				_OnLogoff(status,data);
				break;
			case YAHOO_SERVICE_ISAWAY:
				_OnIsAway(status,data);
				break;
			case YAHOO_SERVICE_ISBACK:
				_OnIsBack(status,data);
				break;
			case YAHOO_SERVICE_IDLE:
				_OnIDLE(status,data);
				break;
			case YAHOO_SERVICE_MESSAGE:
				_OnMessage(status,data);
				break;
			case YAHOO_SERVICE_IDACT:
				_OnIdActivate(status,data);
				break;
			case YAHOO_SERVICE_IDDEACT:
				_OnIdDeactivate(status,data);
				break;
			case YAHOO_SERVICE_MAILSTAT:
				_OnMailStat(status,data);
			case YAHOO_SERVICE_USERSTAT:
				_OnUserStat(status,data);
				break;
			case YAHOO_SERVICE_NEWMAIL:
				_OnNewMail(status,data);
				break;
			case YAHOO_SERVICE_CHATINVITE:
				_OnChatInvite(status,data);
				break;
			case YAHOO_SERVICE_CALENDAR:
				_OnCalendar(status,data);
				break;
			case YAHOO_SERVICE_NEWPERSONALMAIL:
				_OnNewPersonalMail(status,data);
				break;
			case YAHOO_SERVICE_NEWCONTACT:
				_OnNewContact(status,data);
				break;
			case YAHOO_SERVICE_ADDIDENT:
				_OnAddIdent(status,data);
				break;
			case YAHOO_SERVICE_ADDIGNORE:
				_OnAddIgnore(status,data);
				break;
			case YAHOO_SERVICE_PING:
				_OnPing(status,data);
				break;
			case YAHOO_SERVICE_GOTGROUPRENAME:
				_OnGotGroupRename(status,data);
				break;
			case YAHOO_SERVICE_SYSMESSAGE:
				_OnSysMsg(status,data);
				break;
			case YAHOO_SERVICE_PASSTHROUGH2:
				_OnPassthrough(status,data);
				break;
			case YAHOO_SERVICE_CONFINVITE:
				_OnConfInvite(status,data);
				break;
			case YAHOO_SERVICE_CONFLOGON:
				_OnConfLogon(status,data);
				break;
			case YAHOO_SERVICE_CONFDECLINE:
				_OnConfDecline(status,data);
				break;
			case YAHOO_SERVICE_CONFLOGOFF:
				_OnConfLogoff(status,data);
				break;
			case YAHOO_SERVICE_CONFADDINVITE:
				_OnConfAddInvite(status,data);
				break;
			case YAHOO_SERVICE_CONFMSG:
				_OnConfMsg(status,data);
				break;
			case YAHOO_SERVICE_CHATLOGON:
				_OnChatLogon(status,data);
				break;
			case YAHOO_SERVICE_CHATLOGOFF:
				_OnChatLogoff(status,data);
				break;
			case YAHOO_SERVICE_CHATMSG:
				_OnChatMsg(status,data);
				break;
			case YAHOO_SERVICE_GAMELOGON:
				_OnGameLogon(status,data);
				break;
			case YAHOO_SERVICE_GAMELOGOFF:
				_OnGameLogoff(status,data);
				break;
			case YAHOO_SERVICE_GAMEMSG:
				_OnGameMsg(status,data);
				break;
			case YAHOO_SERVICE_FILETRANSFER:
				_OnFileTransfer(status,data);
				break;
			case YAHOO_SERVICE_VOICECHAT:
				_OnVoiceChat(status,data);
				break;
			case YAHOO_SERVICE_NOTIFY:
				_OnNotify(status,data);
				break;
			case YAHOO_SERVICE_VERIFY:
				_OnVerify(status,data);
				break;
			case YAHOO_SERVICE_P2PFILEXFER:
				_OnP2PFileXfer(status,data);
				break;
			case YAHOO_SERVICE_PEERTOPEER:
				_OnPeer2Peer(status,data);
				break;
			case YAHOO_SERVICE_AUTHRESP:
				_OnAuthResp(status,data);
				break;
			case YAHOO_SERVICE_LIST:
				_OnList(status,data);
				break;
			case YAHOO_SERVICE_AUTH:
				_OnAuth(status,data);
				break;
			case YAHOO_SERVICE_ADDBUDDY:
				_OnAddBuddy(status,data);
				break;
			case YAHOO_SERVICE_REMBUDDY:
				_OnRemBuddy(status,data);
				break;
			case YAHOO_SERVICE_IGNORECONTACT:
				_OnIgnoreContact(status,data);
				break;
			case YAHOO_SERVICE_REJECTCONTACT:
				_OnRejectContact(status,data);
				break;
			case YAHOO_SERVICE_GROUPRENAME:
				_OnGroupRename(status,data);
				break;
			case YAHOO_SERVICE_CHATONLINE:
				_OnChatOnline(status,data);
				break;
			case YAHOO_SERVICE_CHATGOTO:
				_OnChatGoto(status,data);
				break;
			case YAHOO_SERVICE_CHATJOIN:
				_OnChatJoin(status,data);
				break;
			case YAHOO_SERVICE_CHATLEAVE:
				_OnChatLeave(status,data);
				break;
			case YAHOO_SERVICE_CHATEXIT:
				_OnChatExit(status,data);
				break;
			case YAHOO_SERVICE_CHATINVITE2:
				_OnChatInvite2(status,data);
				break;
			case YAHOO_SERVICE_CHATLOGOUT:
				_OnChatLogout(status,data);
				break;
			case YAHOO_SERVICE_CHATPING:
				_OnChatPing(status,data);
				break;
			case YAHOO_SERVICE_COMMENT:
				_OnComment(status,data);
				break;
			case YAHOO_SERVICE_ACCEPTREQ:
				_OnAcceptReq(status,data);
				break;
			case YAHOO_SERVICE_WEBCAM:
				_OnWebcam(status,data);
				break;
			default:
				_OnUnknownService(service,status,data);
				break;
		}
	}

	// returns number of users added
	static int ProcessUserList(CYahooMap &data,CYahooMsgUsers &users)
	{
		const int size = data.GetSize();
		int i;
		CYahooMsgUser user;
		bool add = false;
		int count = 0;
		int key;
		CString val;

		for(i=0;i<size;i++){
			key = data.GetKeyAt(i);
			val = data.GetValueAt(i);
			switch(key){
				case 109:	// name
					// add previous
					if(add){
						users.Add(user);
						count++;
						user.Reset();
					}
					if(!val.IsEmpty()){
						user.m_name = val;
						add = true;
					}
					else
						add = false;
					break;
				case 110:
					user.m_age = atoi(val);
					break;
				case 113:
					user.m_flags = (DWORD)atoi(val);
					break;
				case 141: // nickname
					user.m_nick = val;
					break;
				case 142:
					user.m_location = val;
					break;
				default:
					break;
			}
		}
		if(add){
			count++;
			users.Add(user);
		}
		return count;
	}

	void ProcessListPacket(CYahooMap &data)
	{
		const int size = data.GetSize();
		int i;
		int key;

		for(i=0;i<size;i++){
			key = data.GetKeyAt(i);
			switch(key){
				case 87:	// buddy list
				{
					ProcessBuddyList(data.GetValueAt(i));
					break;
				}
				case 88:	// ignore list
				case 89:	// alternate ids
				{
					ProcessSimpleList(data.GetValueAt(i),key);
					break;
				}
				case 59:
				{
					CString cookie = data.GetValueAt(i);
					if(!cookie.IsEmpty()){
						int pos = cookie.Find(';');
						if(pos > 0){
							cookie = cookie.Left(pos + 1);
						}
						else{
							// ok, cookie doesnt end with ;
						}
						switch(cookie[0]){
							case 'Y':
								m_cookieY = cookie;
								break;
							case 'T':
								m_cookieT = cookie;
								break;
							case 'C':
								m_cookieC = cookie;
								break;
							default:
								ATLTRACE("unknown cookie: %c\n",cookie[0]);
								break;
						}
					}
					break;
				}
				case 149:
					m_cookie149 = data.GetValueAt(i);
					break;
				case 150:
					m_cookie150 = data.GetValueAt(i);
					break;
				case 151:
					m_cookie151 = data.GetValueAt(i);
					break;
				default:
					break;
			}
		}

	}
	// "groupname:buddy1,buddy2,...buddyn\ngroupname2:buddy1,..,buddyk\n"
	void ProcessBuddyList(LPCSTR text)
	{
		CStringToken st,st2;
		CString next;
		CString group;
		CSimpleArray<CString> buddies;
		CString buddy;
		int colon;
		

		st.Init(text,"\x0a");

		while(true){
			next = st.GetNext();
			if(next.IsEmpty()){
				break;
			}
			colon = next.Find(':',0);
			if(colon >= 0){
				// find the ':' and get the group name
				group = next.Left(colon + 1);
				// remove the "group:" from next
				next.Delete(0,group.GetLength());
				// remove the ':' from the group name
				group.Delete(group.GetLength() - 1);
				buddies.RemoveAll();
				st2.Init(next,",");
				while(true){
					buddy = st2.GetNext();
					if(buddy.IsEmpty())
						break;
					buddies.Add(buddy);
				}
				m_pSink->OnBuddyList(group,buddies);
			}
		}
	}
	// ignore list and alternate ids list
	void ProcessSimpleList(LPCSTR text,int key)
	{
		CSimpleArray<CString> lst;
		CStringToken st;
		CString next;

		st.Init(text,",");
		while(true){
			next = st.GetNext();
			if(next.IsEmpty())
				break;
			lst.Add(next);
		}
		switch(key){
			case 88:
				m_pSink->OnIgnoreList(lst);
				break;
			case 89:
				m_pSink->OnAlternateIdList(lst);
				break;
			default:
				break;
		}
	}
	// YAHOO_SERVICE_LOGON = 1
	void _OnLogon(u_long status,CYahooMap &data)
	{
		CString sCount = data.Lookup(8);
		CSimpleArray<CBuddyInfo> buddies;
		int count = 0;
		int i;
		int key;
		CString val;
		bool bAdd = false;
		CBuddyInfo buddy;
		bool me = false;

		if(!m_bLoggedOn){
			m_bLoggedOn = true;
			me = true;
		}

		if(!sCount.IsEmpty()){
			// we may not receive key 8 for a single name ?
			count = atoi(sCount);
		}
		
		for(i=0;i<data.GetSize();i++){
			key = data.GetKeyAt(i);
			if(key == 7){
				if(bAdd){
					buddies.Add(buddy);
					buddy.Reset();
				}
				bAdd = true;
				buddy.m_name = data.GetValueAt(i);
			}
			else if(key == 10 && bAdd){
				CString s = data.GetValueAt(i);
				if(!s.IsEmpty()){
					buddy.m_status = atoi(s);
				}
			}
			else if(key == 137 && bAdd){
				CString s = data.GetValueAt(i);
				if(!s.IsEmpty()){
					buddy.m_idle = atoi(s);
				}
			}
		}
		if(bAdd)
			buddies.Add(buddy);
		m_pSink->OnUserLogon(status,buddies,me);
	}
	// YAHOO_SERVICE_LOGOFF = 2
	void _OnLogoff(u_long status,CYahooMap &data)
	{
		CSimpleArray<CBuddyInfo> buddies;
		int i;
		int key;
		CString val;
		bool bAdd = false;
		CBuddyInfo buddy;
		
		for(i=0;i<data.GetSize();i++){
			key = data.GetKeyAt(i);
			if(key == 7){
				if(bAdd){
					buddies.Add(buddy);
					buddy.Reset();
				}
				bAdd = true;
				buddy.m_name = data.GetValueAt(i);
			}
			else if(key == 10 && bAdd){
				CString s = data.GetValueAt(i);
				if(!s.IsEmpty()){
					buddy.m_status = atoi(s);
				}
			}
		}
		if(bAdd)
			buddies.Add(buddy);
		m_pSink->OnUserLogoff(status,buddies);
	}
	// YAHOO_SERVICE_ISAWAY = 3
	void _OnIsAway(u_long status,CYahooMap &data)
	{
		CString scode = data.Lookup(10);
		CString msg;
		int code;
		
		if(scode.IsEmpty())
			return;
		code = atoi((LPCSTR)scode);
		if(code == YAHOO_STATUS_CUSTOM){
			msg = data.Lookup(19);
		}
		m_pSink->OnUserAway(\
			data.Lookup(7),	// buddy name
			code,			// status code
			msg);			// status msg

	}
	// YAHOO_SERVICE_ISBACK = 4
	void _OnIsBack(u_long status,CYahooMap &data)
	{
		CString scode = data.Lookup(10);
		CString msg;
		int code;
	
		if(scode.IsEmpty())
			return;
		code = atoi((LPCSTR)scode);
		if(code == YAHOO_STATUS_CUSTOM){
			msg = data.Lookup(19);
		}
		m_pSink->OnUserAway(\
			data.Lookup(7),	// buddy name
			code,			// status code
			msg);			// status msg
	}
	// YAHOO_SERVICE_IDLE = 5
	void _OnIDLE(u_long status,CYahooMap &data)
	{
		m_pSink->OnIdle(status,data);
	}
	// YAHOO_SERVICE_MESSAGE = 6
	void _OnMessage(u_long status,CYahooMap &data)
	{
		if(status !=5 ){
			CString s = data.Lookup(87);
			bool utf8 = false;
			if(!s.IsEmpty() && s[0] == '1')
				utf8 = true;

			m_pSink->OnInstantMessage(\
				data.Lookup(4),		// from
				data.Lookup(5),		// to
				data.Lookup(14),	// msg
				utf8,
				data.Lookup(63),	// im enviroment
				data.Lookup(15));	// time
		}
		else{
			CString from;
			CString to;
			CString msg;
			CString s;
			bool utf8;
			CString t;
			bool snd = false;
			int key;
			CString ime;
			int i;

			for(i=0;i<data.GetSize();i++){
				switch(key = data.GetKeyAt(i)){
					case 4:	// from
						if(snd){	// notify previous
							m_pSink->OnInstantMessage(from,to,msg,utf8,ime,t);
						}
						else
							snd = true;
						from = data.GetValueAt(i);
						to.Empty();
						msg.Empty();
						s.Empty();
						t.Empty();
						ime.Empty();
						utf8 = false;
						break;
					case 5:	// to
						to = data.GetValueAt(i);
						break;
					case 14:	// msg
						msg = data.GetValueAt(i);
						break;
					case 97:	// utf8
						s = data.GetValueAt(i);
						if(!s.IsEmpty())
							utf8 = (s[0] == '1') ;
						break;
					case 63:	// 
						ime = data.GetValueAt(i);
						break;
					case 15:	// time
						t = data.GetValueAt(i);
						break;
					default:
						break;
				}
			}
			if(snd){	// notify previous
				m_pSink->OnInstantMessage(from,to,msg,utf8,ime,t);
			}
		}
	}
	// YAHOO_SERVICE_IDACT = 7
	void _OnIdActivate(u_long status,CYahooMap &data)
	{
		m_pSink->OnIdActivate(status,data);
	}
	// YAHOO_SERVICE_IDDEACT = 8
	void _OnIdDeactivate(u_long status,CYahooMap &data)
	{
		m_pSink->OnIdDeactivate(status,data);
	}
	// YAHOO_SERVICE_MAILSTAT = 9
	void _OnMailStat(u_long status,CYahooMap &data)
	{
		m_pSink->OnMailStat(status,data);
	}
	// YAHOO_SERVICE_USERSTAT = 0xa
	void _OnUserStat(u_long status,CYahooMap &data)
	{
		m_pSink->OnUserStat(status,data);
	}
	// YAHOO_SERVICE_NEWMAIL = 0xb
	void _OnNewMail(u_long status,CYahooMap &data)
	{
		CString sCount = data.Lookup(9);
		int count = 0;

		if(!sCount.IsEmpty())
			count = atoi(sCount);
		m_pSink->OnNewMail(count);
	}
	// YAHOO_SERVICE_CHATINVITE = 0xc
	void _OnChatInvite(u_long status,CYahooMap &data)
	{
		m_pSink->OnChatInvite(status,data);
	}
	// YAHOO_SERVICE_CALENDAR = 0xd
	void _OnCalendar(u_long status,CYahooMap &data)
	{
		m_pSink->OnCalendar(status,data);
	}
	// YAHOO_SERVICE_NEWPERSONALMAIL = 0xe
	void _OnNewPersonalMail(u_long status,CYahooMap &data)
	{
		m_pSink->OnNewPersonalMail(status,data);
	}
	// YAHOO_SERVICE_NEWCONTACT = 0xf
	void _OnNewContact(u_long status,CYahooMap &data)
	{
		m_pSink->OnNewContact(status,data.Lookup(1),data.Lookup(3),data.Lookup(14));
	}
	// YAHOO_SERVICE_ADDIDENT = 0x10
	void _OnAddIdent(u_long status,CYahooMap &data)
	{
		m_pSink->OnAddIdent(status,data);
	}
	// YAHOO_SERVICE_ADDIGNORE = 0x11
	void _OnAddIgnore(u_long status,CYahooMap &data)
	{
		m_pSink->OnAddIgnore(status,data);
	}
	// YAHOO_SERVICE_PING = 0x12
	void _OnPing(u_long status,CYahooMap &data)
	{
		m_pSink->OnPing(status);
	}
	// YAHOO_SERVICE_GOTGROUPRENAME = 0x13
	// 1, 36(old), 37(new)
	void _OnGotGroupRename(u_long status,CYahooMap &data)
	{
		m_pSink->OnGotGroupRename(data.Lookup(36),data.Lookup(37));
	}
	// YAHOO_SERVICE_SYSMESSAGE = 0x14
	void _OnSysMsg(u_long status,CYahooMap &data)
	{
		m_pSink->OnSysMessage(status,data);
	}
	// YAHOO_SERVICE_PASSTHROUGH2 = 0x16
	void _OnPassthrough(u_long status,CYahooMap &data)
	{
		m_pSink->OnPassThrough(status,data);
	}
	// YAHOO_SERVICE_CONFINVITE = 0x18,
	void _OnConfInvite(u_long status,CYahooMap &data)
	{
		
	}
	// YAHOO_SERVICE_CONFLOGON = 0x19
	void _OnConfLogon(u_long status,CYahooMap &data)
	{
		
	}
	// YAHOO_SERVICE_CONFDECLINE = 0x1a
	void _OnConfDecline(u_long status,CYahooMap &data)
	{
		
	}
	// YAHOO_SERVICE_CONFLOGOFF = 0x1b
	void _OnConfLogoff(u_long status,CYahooMap &data)
	{
		
	}
	// YAHOO_SERVICE_CONFADDINVITE = 0x1c
	void _OnConfAddInvite(u_long status,CYahooMap &data)
	{
	}
	// YAHOO_SERVICE_CONFMSG = 0x1d
	void _OnConfMsg(u_long status,CYahooMap &data)
	{
	}
	// YAHOO_SERVICE_CHATLOGON = 0x1e
	// 0,1, 6(Y=... T=...) 2's 
	void _OnChatLogon(u_long status,CYahooMap &data)
	{
		m_bLoggedInChat = true;
		m_pSink->OnChatLogon(status,data);
	}
	// YAHOO_SERVICE_CHATLOGOFF = 0x1f
	void _OnChatLogoff(u_long status,CYahooMap &data)
	{
		m_bLoggedInChat = false;
		m_pSink->OnChatLogoff(status,data);
	}
	// YAHOO_SERVICE_CHATMSG = 0x20,
	void _OnChatMsg(u_long status,CYahooMap &data)
	{
		CString s = data.Lookup(97);
		bool utf8 = false;
		if(!s.IsEmpty() && s[0] == '1')
			utf8 = true;
		m_pSink->OnPrivateMessage(\
			data.Lookup(4),	// from
			data.Lookup(5),	// to
			data.Lookup(14),// msg
			utf8);
	}
	// YAHOO_SERVICE_GAMELOGON = 0x28, 
	void _OnGameLogon(u_long status,CYahooMap &data)
	{
		m_pSink->OnGameLogon(status,data);
	}
	// YAHOO_SERVICE_GAMELOGOFF = 0x29
	void _OnGameLogoff(u_long status,CYahooMap &data)
	{
		m_pSink->OnGameLogoff(status,data);
	}
	// YAHOO_SERVICE_GAMEMSG = 0x2a,
	void _OnGameMsg(u_long status,CYahooMap &data)
	{
		m_pSink->OnGameMsg(status,data);
	}
	// YAHOO_SERVICE_FILETRANSFER = 0x46,
	void _OnFileTransfer(u_long status,CYahooMap &data)
	{
		m_pSink->OnFileTransfer(status,data);
	}
	// YAHOO_SERVICE_VOICECHAT = 0x4A,
	void _OnVoiceChat(u_long status,CYahooMap &data)
	{
		m_pSink->OnVoiceChat(status,data);
	}
	// YAHOO_SERVICE_NOTIFY = 0x4b
	void _OnNotify(u_long status,CYahooMap &data)
	{
		CString msg = data.Lookup(49);

		if(!msg.CompareNoCase("TYPING")){
			CString s = data.Lookup(13);
			int i = atoi(s);
			m_pSink->OnTyping(data.Lookup(4),i == 1);
		}
		else if(!msg.CompareNoCase("WEBCAMINVITE")){
			CString s = data.Lookup(14);
			bool invite = true;
			bool denied;

			if(!s.IsEmpty()){
				if(s[0] == ' ')
					invite = true;
				else{
					int i = atoi(s);
					if(i == 1){
						invite = false;
						denied = false;
					}
					else if(i == -1){
						invite = false;
						denied = true;
					}
					else{
						// ?
					}
				}
			}
			if(invite){
				m_pSink->OnWebcamInvite(data.Lookup(4));
			}
			else{
				m_pSink->OnWebcamInviteReply(data.Lookup(4),!denied);
			}
		}
		else{
			m_pSink->OnNotify(\
				data.Lookup(4),		// from
				data.Lookup(49),	// notify-string
				data.Lookup(13));	// status
		}
	}
	// YAHOO_SERVICE_VERIFY = 0x4c
	void _OnVerify(u_long status,CYahooMap &data)
	{
		m_pSink->OnVerify(status,data);
	}
	// YAHOO_SERVICE_P2PFILEXFER = 0x4d
	void _OnP2PFileXfer(u_long status,CYahooMap &data)
	{
		m_pSink->OnP2PFileXfer(status,data);
	}
	// YAHOO_SERVICE_PEERTOPEER = 0x4F,	/* Checks if P2P possible */
	void _OnPeer2Peer(u_long status,CYahooMap &data)
	{
		m_pSink->OnPeer2Peer(status,data);
	}
	// YAHOO_SERVICE_AUTHRESP = 0x54,
	void _OnAuthResp(u_long status,CYahooMap &data)
	{
		if(status == -1){
			m_pSink->OnAuthError(data.Lookup(66),data.Lookup(20));
			m_bLoggedOn = false;
		}
	}
	// YAHOO_SERVICE_LIST = 0x55
	void _OnList(u_long status,CYahooMap &data)
	{
		ProcessListPacket(data);
	}
	// YAHOO_SERVICE_AUTH = 0x57
	void _OnAuth(u_long status,CYahooMap &data)
	{
		CString ver11 = data.Lookup(13);
		bool bVer11 = false;

		if(!ver11.IsEmpty()){
			if(ver11[0] == '1'){
				bVer11 = true;
			}
		}
		m_challenge = data.Lookup(94);
		m_sessionid = m_PacketIn.m_header.GetSessionID();
		m_pSink->OnAuthenticate(status,(LPCSTR)m_challenge,bVer11);
	}
	// YAHOO_SERVICE_ADDBUDDY = 0x83,
	void _OnAddBuddy(u_long status,CYahooMap &data)
	{
		CBuddyInfo bi;
		CString s;

		bi.m_name = data.Lookup(7);
		s = data.Lookup(66);
		if(!s.IsEmpty()){
			bi.m_status = atoi(s);
		}
		// todo - it can actually be a list
		m_pSink->OnAddBuddy(\
			data.Lookup(1),		// me
			data.Lookup(65),	// group
			bi);				// buddy info
	}
	// YAHOO_SERVICE_REMBUDDY = 0x84
	void _OnRemBuddy(u_long status,CYahooMap &data)
	{
		m_pSink->OnRemBuddy(data.Lookup(1),data.Lookup(7),data.Lookup(65));
	}
	// YAHOO_SERVICE_IGNORECONTACT = 0x85
	// > 1, 7, 13 < 1, 66, 13, 0
	void _OnIgnoreContact(u_long status,CYahooMap &data)
	{
		LPCSTR ig = data.Lookup(13);
		bool ignore = !lstrcmp(ig,"1");

		m_pSink->OnIgnoreUser(\
			data.Lookup(0),	// who
			data.Lookup(1),	// me
			ignore,
			data.Lookup(66));	// status of the answer
	}
	// YAHOO_SERVICE_REJECTCONTACT = 0x86  = acknowledges if our REJECTCONTACT was received ?
	void _OnRejectContact(u_long status,CYahooMap &data)
	{
		m_pSink->OnRejectContact(status,data.Lookup(1),data.Lookup(7),data.Lookup(66));
	}
	// YAHOO_SERVICE_ACCEPTREQ = 0xd6  1,5,13
	void _OnAcceptReq(u_long status,CYahooMap &data)
	{
		m_pSink->OnAcceptReq(status,data);
	}
	// YAHOO_SERVICE_GROUPRENAME = 0x89
	/* > 1, 65(new), 66(0), 67(old) */
	void _OnGroupRename(u_long status,CYahooMap &data)
	{
		m_pSink->OnGroupRename(data.Lookup(1),data.Lookup(67),data.Lookup(65));
	}
	// YAHOO_SERVICE_CHATONLINE = 0x96
	/* > 109(id), 1, 6(abcde) < 0,1*/
	void _OnChatOnline(u_long status,CYahooMap &data)
	{
		m_pSink->OnChatOnline();
	}
	// YAHOO_SERVICE_CHATGOTO = 0x97
	void _OnChatGoto(u_long status,CYahooMap &data)
	{
		m_pSink->OnChatGoto(status,data);
	}
	// YAHOO_SERVICE_CHATJOIN = 0x98
	void _OnChatJoin(u_long status,CYahooMap &data)
	{
		CString s;
		bool me = false;
		//int iCount = 0;
		CYahooMsgUsers users;
		bool first = true;

		if(status == 5){
			m_bMoreUsers = TRUE;
			me = true;
			first = true;
		}
		else if(status == 1){
			if(m_bMoreUsers){
				if(m_users_toget == m_users_got){
					m_bMoreUsers = FALSE;
					me = false;
					m_users_toget = 0;
					m_users_got = 0;
				}
				else{
					me = true;
					first = false;
				}
			}
			else{
				m_users_toget = m_users_got = 0;
			}
		}
		else{
			ATLTRACE("_OnChatJoin: status=%d\r\n",status);
			return;
		}

		/*
		s = data.Lookup(112);
		if(!s.IsEmpty() && s[0] == '0'){
			me = false;
		}
		s = data.Lookup(130);
		if(!s.IsEmpty()){
			first = true;
			me = true;
		}
		*/

		s = data.Lookup(108);	// numbers of users in the list
		if(!s.IsEmpty()){
			m_users_toget = (short)atoi((LPCSTR)s);
		}
		int count = 0;
		if(data.GetSize())
			count = ProcessUserList(data,users);
		if(!count)
			return;
		else{
			me = users.HasUser(GetName()) ? true : false;
		}
		if(me)
			m_users_got += count;
		ATLTRACE("_OnChatJoin: status=%d count=%d me=%d first=%d\r\n",status,count,me,first);
		if(first && me){
			ATLASSERT(me);
			m_cookie61 = data.Lookup(61);
			m_cookie130 = data.Lookup(130);
			m_WebCamServer = data.Lookup(102);
			m_room = data.Lookup(104);
			m_topic = data.Lookup(105);
			m_rmcat = data.Lookup(128);
			m_rmspace = data.Lookup(129);
			m_bInRoom = TRUE;
			m_pSink->OnChatJoin(m_room,m_topic);
		}
		if(!me)
			m_pSink->OnUserEnters(users[0]);
		else{
			m_pSink->OnUserList(users,m_users_toget != m_users_got);
			ATLTRACE("OnChatJoin toget: %d got: %d left: %d\n",m_users_toget,m_users_got,
				m_users_toget - m_users_got);
		}
	}
	// YAHOO_SERVICE_CHATLEAVE = 0x99
	void _OnChatLeave(u_long status,CYahooMap &data)
	{
		m_pSink->OnChatLeave(status,data);
	}
	// YAHOO_SERVICE_CHATEXIT = 0x9b,
	void _OnChatExit(u_long status,CYahooMap &data)
	{
		CYahooMsgUser user;

		user.m_name = data.Lookup(109);
		user.m_nick = data.Lookup(141);
		user.m_location = data.Lookup(142);
		user.m_flags = (DWORD)atoi(data.Lookup(113));
		user.m_age = atoi(data.Lookup(110));
		m_pSink->OnUserLeaves(user);
	}
	void _OnChatInvite2(u_long status,CYahooMap &data)
	{
		m_pSink->OnChatInvite2(\
			data.Lookup(104),	// room
			data.Lookup(129),	// rmspace
			data.Lookup(119),	// from
			data.Lookup(118),	// to
			data.Lookup(117));	// msg
	}
	// YAHOO_SERVICE_CHATLOGOUT = 0xa0,
	void _OnChatLogout(u_long status,CYahooMap &data)
	{
		m_bLoggedInChat = false;
		m_pSink->OnChatLogout();
	}
	// YAHOO_SERVICE_CHATPING = 0xa1
	void _OnChatPing(u_long status,CYahooMap &data)
	{
		m_pSink->OnChatPing(status,data);
	}
	// YAHOO_SERVICE_COMMENT = 0xa8
	void _OnComment(u_long status,CYahooMap &data)
	{
		CString val = data.Lookup(97);
		bool utf8 = false;
		int mode = YMSG_TEXT_NORMAL;
		CString msg = data.Lookup(117);

		if(!val.IsEmpty()){
			utf8 = (val[0] == '1');
		}
		val = data.Lookup(124);
		if(!val.IsEmpty()){
			mode = atoi(val);
		}
		switch(mode){
			case YMSG_TEXT_EMOTE:
				m_pSink->OnEmote(data.Lookup(109),msg,utf8);
				break;
			case YMSG_TEXT_BUBBLE:
				m_pSink->OnThink(data.Lookup(109),msg,utf8);
				break;
			case YMSG_TEXT_NORMAL:
			default:
				m_pSink->OnText(data.Lookup(109),msg,utf8);
				break;
		}

	}
	void _OnWebcam(u_long status,CYahooMap &data)
	{
		if(status == 1)
			m_pSink->OnWebcam(data.Lookup(61),data.Lookup(102));
		else{
			// todo
		}
	}
	void _OnUnknownService(u_short service,u_long status,CYahooMap\
		&data)
	{
		m_pSink->OnUnknownService(service,status,data);
	}

	// reset to a default state after closing connection or in constructor
	void Reset(void)
	{
		m_bJoiningRoom = false;
		m_bInRoom = false;
		m_bLoggedInChat = false;
		m_bMoreUsers = false;
		m_bLoggedOn = false;
		m_utf8 = false;
		m_users_toget = 0;
		m_users_got = 0;
		m_challenge.Empty();
		m_sessionid = 0;
		m_id.Empty();
		m_name.Empty();
		m_pw.Empty();
		m_room.Empty();
		m_topic.Empty();
		m_WebCamServer.Empty();
		m_rmcat.Empty();
		m_rmspace.Empty();
		m_room_desired.Empty();
		m_rmid_desired.Empty();
	}
private:
	TSock m_socket;
	CYmsgPacketBuffer m_buf;
	CYahooMsgPacketOut m_PacketOut;
	CYahooMsgPacketIn m_PacketIn;
	CYahooMap m_args;
	IYahooMessengerEvents *m_pSink;
	CCriticalSection m_cs2;
	CSimpleArray<CYahooMsgPacketOut> m_out_queue;
	bool m_bJoiningRoom:1;
	bool m_bInRoom:1;
	bool m_bLoggedInChat:1;
	bool m_bMoreUsers:1;
	bool m_bLoggedOn:1;
	bool m_utf8:1;
	short m_users_toget;	// number of users announced
	short m_users_got;		// number of users we got so far
	CMessengerVersion m_build;
public:
	CString m_id;
	CString m_name;
	CString m_pw;
	CString m_room;
	CString m_topic;			// "room topic"
	CString m_WebCamServer;
	//CString m_vcauth;
	//CString m_vc;
	CString m_rmcat;
	CString m_rmspace;
	CString m_challenge;
	u_long m_sessionid;
	CString m_room_desired;
	CString m_rmid_desired;
	CString m_cookieY;
	CString m_cookieT;
	CString m_cookieC;
	CString m_cookie61;
	CString m_cookie130;
	CString m_cookie149;
	CString m_cookie150;
	CString m_cookie151;
public:
};

/**************** IYahooMessenger *******************/
BOOL NO5YAHOO::YahooMessengerCreate(IYahooMessenger **ppMsg,IYahooMessengerEvents *pMsgEvents)
{
	*ppMsg = new CYahooMessengerT<CSimpleSocket>(pMsgEvents);
	return (*ppMsg) != NULL ? TRUE : FALSE;
}

void NO5YAHOO::YahooMessengerDestroy(IYahooMessenger **ppMsg)
{
	CYahooMessengerT<CSimpleSocket> *p = (CYahooMessengerT<CSimpleSocket> *)(*ppMsg);
	delete p;
	*ppMsg = NULL;
}

/************* service and key descripiton ***********/

struct ServiceDesc
{
	u_long service;
	const char *desc;
};

struct KeyDesc
{
	int key;
	LPCSTR desc;
};

#define BEGIN_SERVICE_DESC()\
const char * NO5YAHOO::GetServiceDesc(u_long service)\
{\
	static ServiceDesc _ServMap[] = {
#define SERVICE_DESC(service,desc)\
	{service,desc},
#define END_SERVICE_DESC()\
	};\
	for(int i=0;i<sizeof(_ServMap)/sizeof(_ServMap[0]);i++){\
		if(_ServMap[i].service == service)\
			return _ServMap[i].desc;\
	}\
	return NULL;\
}

BEGIN_SERVICE_DESC()
	SERVICE_DESC(YAHOO_SERVICE_LOGON,"LOGON")
	SERVICE_DESC(YAHOO_SERVICE_LOGOFF,"LOGOFF")
	SERVICE_DESC(YAHOO_SERVICE_ISAWAY,"ISAWAY")
	SERVICE_DESC(YAHOO_SERVICE_ISBACK,"ISBACK")
	SERVICE_DESC(YAHOO_SERVICE_IDLE,"ISIDLE")
	SERVICE_DESC(YAHOO_SERVICE_MESSAGE,"MESSAGE")
	SERVICE_DESC(YAHOO_SERVICE_IDACT,"IDACT")
	SERVICE_DESC(YAHOO_SERVICE_IDDEACT,"IDDEACT")
	SERVICE_DESC(YAHOO_SERVICE_MAILSTAT,"MAILSTAT")
	SERVICE_DESC(YAHOO_SERVICE_USERSTAT,"USERSTAT")
	SERVICE_DESC(YAHOO_SERVICE_NEWMAIL,"NEWMAIL")
	SERVICE_DESC(YAHOO_SERVICE_CHATINVITE,"CHATINVITE")
	SERVICE_DESC(YAHOO_SERVICE_CALENDAR,"CALENDAR")
	SERVICE_DESC(YAHOO_SERVICE_NEWPERSONALMAIL,"NEWPERSONALMAIL")
	SERVICE_DESC(YAHOO_SERVICE_NEWCONTACT,"NEWCONTACT")
	SERVICE_DESC(YAHOO_SERVICE_ADDIDENT,"ADDIDENT")
	SERVICE_DESC(YAHOO_SERVICE_ADDIGNORE,"ADDIGNORE")
	SERVICE_DESC(YAHOO_SERVICE_PING,"PING")
	SERVICE_DESC(YAHOO_SERVICE_GOTGROUPRENAME,"GROUPRENAME")
	SERVICE_DESC(YAHOO_SERVICE_SYSMESSAGE,"SYSMESSAGE")
	SERVICE_DESC(YAHOO_SERVICE_PASSTHROUGH2,"PASSTHROUGH2")
	SERVICE_DESC(YAHOO_SERVICE_CONFINVITE,"CONFINVITE")
	SERVICE_DESC(YAHOO_SERVICE_CONFLOGON,"LOGON")
	SERVICE_DESC(YAHOO_SERVICE_CONFDECLINE,"DECLINE")
	SERVICE_DESC(YAHOO_SERVICE_CONFLOGOFF,"LOGOFF")
	SERVICE_DESC(YAHOO_SERVICE_CONFADDINVITE,"ADDINVITE")
	SERVICE_DESC(YAHOO_SERVICE_CONFMSG,"CONFMSG")
	SERVICE_DESC(YAHOO_SERVICE_CHATLOGON,"CHATLOGON")
	SERVICE_DESC(YAHOO_SERVICE_CHATLOGOFF,"CHATLOGOFF")
	SERVICE_DESC(YAHOO_SERVICE_CHATMSG,"CHATMSG")
	SERVICE_DESC(YAHOO_SERVICE_GAMELOGON,"GAMELOGON")
	SERVICE_DESC(YAHOO_SERVICE_GAMELOGOFF,"GAMELOGOFF")
	SERVICE_DESC(YAHOO_SERVICE_GAMEMSG,"GAMEMSG")
	SERVICE_DESC(YAHOO_SERVICE_FILETRANSFER,"FILETRANSFER")
	SERVICE_DESC(YAHOO_SERVICE_VOICECHAT,"VOICECHAT")
	SERVICE_DESC(YAHOO_SERVICE_NOTIFY,"NOTIFY")
	SERVICE_DESC(YAHOO_SERVICE_VERIFY,"VERIFY")
	SERVICE_DESC(YAHOO_SERVICE_P2PFILEXFER,"P2PFILEXFER")
	SERVICE_DESC(YAHOO_SERVICE_PEERTOPEER,"PEERTOPEER")
	SERVICE_DESC(YAHOO_SERVICE_AUTHRESP,"AUTHRESP")
	SERVICE_DESC(YAHOO_SERVICE_LIST,"LIST")
	SERVICE_DESC(YAHOO_SERVICE_AUTH,"AUTH")
	SERVICE_DESC(YAHOO_SERVICE_ADDBUDDY,"ADDBUDDY")
	SERVICE_DESC(YAHOO_SERVICE_REMBUDDY,"REMBUDDY")
	SERVICE_DESC(YAHOO_SERVICE_IGNORECONTACT,"IGNORECONTACT")
	SERVICE_DESC(YAHOO_SERVICE_REJECTCONTACT,"REJECTCONTACT")
	SERVICE_DESC(YAHOO_SERVICE_GROUPRENAME,"GROUPRENAME")
	SERVICE_DESC(YAHOO_SERVICE_CHATONLINE,"CHATONLINE")
	SERVICE_DESC(YAHOO_SERVICE_CHATGOTO,"CHATGOTO")
	SERVICE_DESC(YAHOO_SERVICE_CHATJOIN,"CHATJOIN")
	SERVICE_DESC(YAHOO_SERVICE_CHATLEAVE,"CHATLEAVE")
	SERVICE_DESC(YAHOO_SERVICE_CHATEXIT,"CHATEXIT")
	SERVICE_DESC(YAHOO_SERVICE_CHATINVITE2,"CHATINVITE2")
	SERVICE_DESC(YAHOO_SERVICE_CHATLOGOUT,"CHATLOGOUT")
	SERVICE_DESC(YAHOO_SERVICE_CHATPING,"CHATPING")
	SERVICE_DESC(YAHOO_SERVICE_COMMENT,"COMMENT")
	SERVICE_DESC(YAHOO_SERVICE_ACCEPTREQ,"ACCEPTREQ")
END_SERVICE_DESC()


#define BEGIN_KEY_MAP()\
const char * NO5YAHOO::GetKeyDesc(int key)\
{\
	static KeyDesc _Map[] = {
#define KEY_DESC(key,desc)\
	{key,desc},
#define END_KEY_MAP()\
	};\
	for(int i=0;i<sizeof(_Map)/sizeof(_Map[0]);i++){\
		if(_Map[i].key == key)\
			return _Map[i].desc;\
	}\
	return NULL;\
}

BEGIN_KEY_MAP()
	KEY_DESC(0,"yahooid")
	KEY_DESC(1,"activeid")
	KEY_DESC(3,"buddyname?")
	KEY_DESC(4,"pmfrom")
	KEY_DESC(5,"pmto")
	KEY_DESC(6,"abcde")
	KEY_DESC(7,"buddyname?")
	KEY_DESC(8,"buddylistcount")
	KEY_DESC(9,"mailcount")
	KEY_DESC(10,"awaystate")
	KEY_DESC(11,"buddyinfo1?")
	KEY_DESC(13,"boolean")
	KEY_DESC(14,"msg")
	KEY_DESC(15,"time")
	KEY_DESC(16,"sysmsg")
	KEY_DESC(17,"buddyinfo2?")
	KEY_DESC(18,"mailfromsubj")
	KEY_DESC(19,"textaway")
	KEY_DESC(20,"filename")
	KEY_DESC(31,"ymsgpmextra1?")
	KEY_DESC(32,"ymsgpmextra5?")
	KEY_DESC(38,"filexferid")
	KEY_DESC(41,"username")
	KEY_DESC(42,"mailfromaddr")
	KEY_DESC(43,"mailfromname")
	KEY_DESC(47,"awayicon")
	KEY_DESC(49,"notifystring")
	KEY_DESC(52,"confinvitee")
	KEY_DESC(56,"leavesconf")
	KEY_DESC(57,"taggedname?")
	KEY_DESC(58,"conftext")
	KEY_DESC(59,"cookie")
	KEY_DESC(61,"wccookie?")
	KEY_DESC(62,"roommode?")
	KEY_DESC(63,"inviroment")
	KEY_DESC(65,"buddygroup")
	KEY_DESC(67,"newgroupname")
	KEY_DESC(87,"buddylist")
	KEY_DESC(88,"ignorelist")
	KEY_DESC(89,"idlist")
	KEY_DESC(94,"challenge")
	KEY_DESC(96,"hashstring2")
	KEY_DESC(97,"utf8")
	KEY_DESC(102,"camserver")
	KEY_DESC(104,"roomname")
	KEY_DESC(105,"roomtopic")
	KEY_DESC(108,"usercount")
	KEY_DESC(109,"username")
	KEY_DESC(110,"age")
	KEY_DESC(12,"singleuser?") 
	KEY_DESC(113,"userinfo")
	KEY_DESC(114,"someerror")
	KEY_DESC(117,"msg")
	KEY_DESC(118,"inviteto")
	KEY_DESC(119,"invitefrm")
	KEY_DESC(124,"textmode?")
	KEY_DESC(126,"roomflags?")
	KEY_DESC(128,"rmcat")
	KEY_DESC(129,"rmspace")
	KEY_DESC(130,"wccookie?")
	KEY_DESC(135,"buildnumber")
	KEY_DESC(141,"nickname")
	KEY_DESC(142,"location")
	KEY_DESC(149,"somecookie?")
	KEY_DESC(150,"somecookie?")
	KEY_DESC(151,"somecookie?")
	KEY_DESC(216,"firstname")
	KEY_DESC(254,"lastname")
END_KEY_MAP()
