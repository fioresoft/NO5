
#include "stdafx.h"
#include <no5tl\databuffer.h>
#include <no5tl\mystring.h>
#include <no5tl\winsocketwithhandler.h>
#include "yahoochat.h"
#include "signin.h"
using namespace NO5TL;
using namespace NO5YAHOO;

#ifdef _DEBUG
#include <fstream>
#endif

class CYahooChatPacketOut
{
	CDataBuffer m_buf;
	//
	u_short CalcSize(CSimpleValArray<LPCSTR> &args,u_long packet)
	{
		const int count = args.GetSize();
		int i;
		u_short res = 0;

		for(i=0;i<count;i++){
			res += lstrlen(args[i]);
			if(i < (count - 1)){
				res += 1;	// '\x1'
			}
		}
		return res;
	}
public:
	CYahooChatPacketOut(void)
	{
		//
	}
	CDataBuffer * Build(u_long packet,u_short status,
		CSimpleValArray<LPCSTR> &args)
	{
		u_short size = CalcSize(args,packet);
		const int count = args.GetSize();
		int i;
		YahooChatPacketHeader header;

		m_buf.Reset();

		// make header
		header.SetProto(YCHT_PROTO_NAME);
		header.SetVersion(YCHT_PROTO_VER);
		header.SetPacket(packet);
		header.SetStatus(status);
		header.SetSize(size);
		m_buf.Add(header);

		// make body
		for(i=0;i<count;i++){
			m_buf.Add(args[i]);
			if(i < (count - 1)){
				m_buf << '\x1';
			}
		}
		return &m_buf;
	}
};

class CYahooChatPacketIn
{
public:
	YahooChatPacketHeader m_header;
	CSimpleArray<CString> m_args;
private:
	//
	int ParseBody(char *p)
	{
		CStringToken st;

		m_args.RemoveAll();
		st.Init(CString(p),CString(YCHT_DELS));
		return st.GetAll2(m_args);
	}
public:
	void UnBuild(CDataBuffer *buf)
	{
		char *p = (char *)buf->GetData();
		YahooChatPacketHeader *pHeader = (YahooChatPacketHeader *)p;
		
		ATLASSERT((DWORD)pHeader->GetSize() + (DWORD)sizeof(*pHeader) == \
			buf->GetDataLen());
		
		// copy header
		::CopyMemory(&m_header,pHeader,sizeof(m_header));
		// parse body putting args into array
		p = p + sizeof(m_header);
		ParseBody(p);
	}
};

typedef CPacketBufferT<YahooChatPacketHeader> CPacketBuffer;

template <class TSock>
class CYahooChatT: protected ISocketEvents,public IYahooChat
{
	template <class TSock>
	class CSigninSink : public IYahooChatSignInEvents
	{
		CYahooChatT<TSock> *m_pOutter;
	public:
		CSigninSink(CYahooChatT<TSock> *pOutter):m_pOutter(pOutter)
		{
			//
		}
		// 
		virtual void OnSignIn(CYahooCookies &cookies)
		{
			m_pOutter->_OnSignIn(cookies);
		}
		virtual void OnSignInFailure(BOOL BadPw,BOOL BadId)
		{
			m_pOutter->_OnSignInFailure();
		}
#ifdef _DEBUG
		virtual void OnDataRead(char *buf,int len)
		{
			std::ofstream out("login_debug.txt",std::ios::app|std::ios::out);
			out.write(buf,len);
		}
#endif
	};
	friend CSigninSink<TSock>;

public:
	CYahooChatT(IYahooChatEvents *pSink):m_socket(this),m_pSink(pSink),m_sis(this)
	{
		m_bLogginIn = false;
		m_bLoggedIn = false;
		m_bJoiningRoom = false;
		m_bInChat = false;
		m_utf8 = true;
		m_bKeepUsers = false;
		m_psi = NULL;
	}
	virtual ~CYahooChatT()
	{
		if(m_psi)
			SignInDestroy(&m_psi);
	}
	virtual BOOL SignIn(LPCSTR server,int port,LPCSTR host,LPCSTR name,LPCSTR pw)
	{
		if(!m_psi){
			SignInCreate(&m_psi,&m_sis);
		}
		m_name = name;
		m_pw = pw;
		return m_psi->SignIn(server,port,host,name,pw);
	}
	virtual BOOL SignIn(SOCKADDR_IN &addr,LPCSTR host,LPCSTR name,LPCSTR pw)
	{
		if(!m_psi){
			SignInCreate(&m_psi,&m_sis);
		}
		m_name = name;
		m_pw = pw;
		return m_psi->SignIn(addr,host,name,pw);
	}
	virtual BOOL Init(SOCKADDR_IN &addr)
	{
		BOOL res;

		if(res = m_socket.CreateSocket()){
			if(res = m_socket.AsyncSelect(FD_READ|FD_WRITE|FD_CONNECT|\
				FD_CLOSE)){
				res = m_socket.Connect(&addr);
			}
		}
		return res;
	}

	virtual BOOL Init(LPCSTR server,int port)
	{
		BOOL res;

		m_bLogginIn = false;
		m_bLoggedIn = false;
		m_bJoiningRoom = false;
		m_bInChat = false;
		m_utf8 = true;

		if(res = m_socket.CreateSocket()){
			if(res = m_socket.AsyncSelect(FD_READ|FD_WRITE|FD_CONNECT|\
				FD_CLOSE)){
				res = m_socket.Connect(server,port);
			}
		}
		return res;
	}
	
	virtual void Term(bool bAbrupt)
	{
		if(bAbrupt || !m_socket.IsConnected()){
			if(m_socket.IsHandleValid())
				m_socket.AsyncSelect(0);
			m_socket.CloseSocket();
			m_bLogginIn = false;
			m_bLoggedIn = false;
			m_bJoiningRoom = false;
			m_bInChat = false;
			m_utf8 = true;
		}
		else{
			m_socket.Shutdown();
		}
	}

	virtual void Login(LPCSTR name,LPCSTR cookie)
	{
		//CSincro sincro(m_cs2);
		CDataBuffer *pdb;
		int sent;
		CString sCookie;

		if(!name){
			ATLASSERT(!m_name.IsEmpty());
		}
		else
			m_name = name;
		if(!cookie){
			ATLASSERT(!m_cookie.IsEmpty());
		}
		else
			m_cookie = cookie;

		m_bLogginIn = true;
		m_args.RemoveAll();
		m_args.Add((LPCSTR)m_name);
		m_args.Add((LPCSTR)m_cookie);
		pdb = m_PacketOut.Build(PKT_LOGIN,0,m_args);
		sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
		ATLASSERT(sent == (int)pdb->GetDataLen());
	}

	virtual void Logout(void)
	{
		//CSincro sincro(m_cs2);
		CDataBuffer *pdb;
		int sent;

		m_bLogginOut = true;
		m_args.RemoveAll();
		pdb = m_PacketOut.Build(PKT_LOGOUT,0,m_args);
		sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
		ATLASSERT(sent == (int)pdb->GetDataLen());
	}
	virtual void JoinRoom(LPCSTR room)
	{
		//CSincro sincro(m_cs2);
		CDataBuffer *pdb;
		int sent;

		m_bJoiningRoom = true;
		m_room_desired = room;
		m_rmid_desired = "";
		m_args.RemoveAll();
		m_args.Add(room);
		pdb = m_PacketOut.Build(PKT_ROOMIN,0,m_args);
		sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
		ATLASSERT(sent == (int)pdb->GetDataLen());
	}
	virtual void LeaveRoom(LPCSTR room = NULL)
	{
		LPCSTR _room = room ? room : (LPCSTR)m_room;

		if(_room){
			CDataBuffer *pdb;
			int sent;

			m_args.RemoveAll();
			m_args.Add(_room);
			pdb = m_PacketOut.Build(PKT_ROOMOUT,0,m_args);
			sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
			ATLASSERT(sent == (int)pdb->GetDataLen());
		}
	}
		
	virtual void SendText(LPCSTR msg)
	{
		//CSincro sincro(m_cs2);
		CDataBuffer *pdb;
		int sent;
		CString tmp;

		m_args.RemoveAll();
		m_args.Add((LPCSTR)m_room);
		if(m_utf8){
			tmp = ToUTF8(msg);
			m_args.Add(tmp);
		}
		else{
			m_args.Add(msg);
		}
		pdb = m_PacketOut.Build(PKT_SAY,0,m_args);
		sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
		ATLASSERT(sent == (int)pdb->GetDataLen());
	}
	virtual void SendPm(LPCSTR to,LPCSTR msg)
	{
		//CSincro sincro(m_cs2);
		CDataBuffer *pdb;
		int sent;
		CString tmp;

		m_args.RemoveAll();
		m_args.Add((LPCSTR)m_name);
		m_args.Add(to);
		if(m_utf8){
			tmp = ToUTF8(msg);
			m_args.Add(tmp);
		}
		else{
			m_args.Add(msg);
		}
		//m_args.Add("-1:-1");
		pdb = m_PacketOut.Build(PKT_PM,0,m_args);
		sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
		ATLASSERT(sent == (int)pdb->GetDataLen());
	}
	virtual void SendEmote(LPCSTR msg)
	{
		//CSincro sincro(m_cs2);
		CDataBuffer *pdb;
		int sent;
		CString tmp;

		m_args.RemoveAll();
		m_args.Add((LPCSTR)m_room);
		if(m_utf8){
			tmp = ToUTF8(msg);
			m_args.Add(tmp);
		}
		else{
			m_args.Add(msg);
		}
		pdb = m_PacketOut.Build(PKT_EMOTION,0,m_args);
		sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
		ATLASSERT(sent == (int)pdb->GetDataLen());
	}
	virtual void SendCommand(LPCSTR cmd,LPCSTR msg)
	{
		//CSincro sincro(m_cs2);
		CDataBuffer *pdb;
		int sent;
		CString tmp;

		m_args.RemoveAll();
		m_args.Add(cmd);
		if(m_utf8){
			tmp = ToUTF8(msg);
			m_args.Add(tmp);
		}
		else{
			m_args.Add(msg);
		}
		pdb = m_PacketOut.Build(PKT_CMD,0,m_args);
		sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
		ATLASSERT(sent == (int)pdb->GetDataLen());
	}
	virtual void Invite(LPCSTR name)
	{
		//CSincro sincro(m_cs2);
		CDataBuffer *pdb;
		int sent;

		m_args.RemoveAll();
		m_args.Add(name);
		pdb = m_PacketOut.Build(PKT_INVITE,0,m_args);
		sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
		ATLASSERT(sent == (int)pdb->GetDataLen());
	}
	// this packet uses separator 0xc0 0x80
	virtual void SetStatus(yahoo_status status,LPCSTR msg)
	{
		//CSincro sincro(m_cs2);
		CDataBuffer *pdb;
		int sent;
		CString s;
		CString tmp;
		char buf[10] = {0};

		wsprintf(buf,"%d",(int)status);
		s = m_room;
		s += "\xC0\x80";
		s += buf;
		s += "\xC0\x80";

		if(!msg){
			tmp = " ";
		}
		else{
			tmp = m_utf8 ? FromUTF8(msg) : msg;
		}
		s += tmp;

		m_args.RemoveAll();
		m_args.Add(s);
		pdb = m_PacketOut.Build(PKT_AWAY,0,m_args);
		sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
		ATLASSERT(sent == (int)pdb->GetDataLen());
	}
	virtual void Think(LPCSTR msg)
	{
		//CSincro sincro(m_cs2);
		CDataBuffer *pdb;
		int sent;
		CString tmp;

		m_args.RemoveAll();
		m_args.Add((LPCSTR)m_room);
		if(m_utf8){
			tmp = ToUTF8(msg);
			m_args.Add(tmp);
		}
		else{
			m_args.Add(msg);
		}
		pdb = m_PacketOut.Build(PKT_THINK,0,m_args);
		sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
		ATLASSERT(sent == (int)pdb->GetDataLen());
	}
	virtual void Goto(LPCSTR user)
	{
		//CSincro sincro(m_cs2);
		CDataBuffer *pdb;
		int sent;
		CString tmp;

		m_args.RemoveAll();
		m_args.Add(user);
		pdb = m_PacketOut.Build(PKT_GOTO,0,m_args);
		sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
		ATLASSERT(sent == (int)pdb->GetDataLen());
	}
	virtual void Ping(void)
	{
		//CSincro sincro(m_cs2);
		CDataBuffer *pdb;
		int sent;

		m_args.RemoveAll();
		pdb = m_PacketOut.Build(PKT_PING,0,m_args);
		sent = m_socket.Send(pdb->GetData(),pdb->GetDataLen(),0);
		ATLASSERT(sent == (int)pdb->GetDataLen());
	}
	//
	virtual void KeepUserList(bool bKeep)
	{
		m_bKeepUsers = bKeep;
	}
	// accessors
	virtual LPCTSTR GetName(void) const
	{
		return m_name;
	}
	virtual LPCTSTR GetCookie(void) const
	{
		return m_cookie;
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
	virtual LPCTSTR GetVoiceCookie(void) const
	{
		return m_vcauth;
	}
	virtual LPCTSTR GetCamCookie(void) const
	{
		return m_WebCamCookie;
	}
	virtual BOOL IsVoiceEnabled(void) const
	{
		return ((!m_vc.IsEmpty()) && (m_vc[0] == '1'));
	}
	virtual CYahooChatUsers & GetUserList(void)
	{
		return m_users;
	}
	virtual void GetDesiredRoom(CString &room,CString &rmid)
	{
		room = m_room_desired;
		rmid = m_rmid_desired;
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
	virtual BOOL IsConnecting(void) const
	{
		return m_socket.IsConnecting() ? TRUE:FALSE;
	}
	virtual BOOL IsConnected(void) const
	{
		return m_socket.IsConnected() ? TRUE : FALSE;
	}
	virtual BOOL IsLogginIn(void) const
	{
		return m_bLogginIn ? TRUE:FALSE;
	}
	virtual BOOL IsLoggedIn(void) const
	{
		return m_bLoggedIn ? TRUE:FALSE;
	}
	virtual BOOL IsLogginOut(void) const
	{
		return m_bLogginOut;
	}
	virtual BOOL IsJoiningRoom(void) const
	{
		return m_bJoiningRoom ? TRUE : FALSE;
	}
	virtual BOOL IsInRoom(void) const
	{
		return m_bInChat;
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
		if(m_socket.IsHandleValid())
			m_socket.AsyncSelect(0);
		m_socket.CloseSocket();
		m_pSink->OnSockClose(iError);
	}
	virtual void OnSockError(int error)
	{
		if(m_socket.IsHandleValid())
			m_socket.AsyncSelect(0);
		m_socket.CloseSocket();
		m_pSink->OnSockError(error);
	}
	virtual void OnSockConnectTimeout(void)
	{
		if(m_socket.IsHandleValid())
			m_socket.AsyncSelect(0);
		m_socket.CloseSocket();
		m_pSink->OnSockConnectTimeout();
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
		}
		m_pSink->OnSockAddressResolved(error);
	}
private:
	void Notify(void)
	{
		CSimpleArray<CString> &v = m_PacketIn.m_args;
		u_long packet = m_PacketIn.m_header.GetPacket();
		short status = (short)m_PacketIn.m_header.GetStatus();
		
		switch(packet){

			case PKT_LOGIN:
				_OnLogin(status,v);
				break;

			case PKT_LOGOUT:
				_OnLogout(status,v);
				break;

			case PKT_ROOMIN:
				_OnChatJoin(status,v);
				break;

			case PKT_ROOMOUT:
				_OnChatLeave(status,v);
				break;

			case PKT_SAY:
				_OnChatText(status,v);
				break;

			case PKT_INVITE:
				_OnInvite(status,v);
				break;
			
			case PKT_AWAY:
				_OnAway(status,v);
				break;

			case PKT_THINK:
				_OnThink(status,v);
				break;

			case PKT_EMOTION:
				_OnEmote(status,v);
				break;

			case PKT_PM:
				_OnPm(status,v);
				break;

			case PKT_DICE:
				if(v.GetSize() >=2 )
					m_pSink->OnDice(v[0],v[1]);
				break;

			case PKT_BUDDY:
				if(v.GetSize())
					m_pSink->OnBuddy(v[0]);
				break;
			
			case PKT_TIME:
				if(v.GetSize())
					m_pSink->OnTime(v[0]);
				break;

			case PKT_HELP:
				if(v.GetSize())
					m_pSink->OnHelp(v[0]);
				break;

			default:
				if(v.GetSize()){
					m_pSink->OnServerMsg(v[0],packet,status);
				}
				break;
		}
	}

	// TODO: we receive this for every buddy or only for ourself ?
	void _OnLogin(short status,CSimpleArray<CString> &v)
	{
		m_bLogginIn = false;

		if(status == 0 && v.GetSize()){
			m_bLoggedIn = true;
			//m_name = v[0];
			m_pSink->OnLogin((LPCSTR)v[0],(LPCSTR)v[1]);
		}
		else{
			// TODO
		}
	}
	// TODO: and this ?
	void _OnLogout(short status,CSimpleArray<CString> &v)
	{
		m_bLoggedIn = false;
		if(v.GetSize()){
			m_pSink->OnLogoff(status,(LPCSTR)v[0]);
		}
	}
	void _OnChatJoin(short status,CSimpleArray<CString> &v)
	{
		u_short byte_status = (u_short)status;

		byte_status <<= 8;
		byte_status >>= 8;

		switch(byte_status){
			case ROOM_OK:
				if(v.GetSize() > 5){
					// I joined the room

					m_bInChat = true;
					m_bJoiningRoom = false;

					m_room = v[0];
					m_users.RemoveAll();
					m_users.Parse(v[4]);
					ParseJoinRoomCookies(v[2]);
					m_WebCamCookie = v[5];
					m_pSink->OnJoinRoom(\
						v[0],	// room
						v[1],	// slogan
						v[2],	// cookies
						m_users,m_WebCamCookie);
				}
				else{
					// someone else has joined
					CYahooChatUser user = m_users.HandleOnUserEnter(v[4]);
					m_pSink->OnRoomIn(v[0]/*room*/,v[1]/*slogan*/,user);
				}
				if(!m_bKeepUsers)
					m_users.RemoveAll();
				break;
						
			case ROOM_FULL:
				m_bJoiningRoom = false;
				if(v.GetSize() >= 2)
					m_pSink->OnRoomFull(v[0]/*msg*/,v[1]/*alt room*/);
				else if(v.GetSize() == 1)
					m_pSink->OnRoomFull(v[0],"");
				break;
			case ROOM_SAME:
			case ROOM_SECURED:
			case ROOM_ISNTON:
			case ROOM_INVALID:
				m_bJoiningRoom = false;
				if(v.GetSize() >=1)
					m_pSink->OnJoinFailed(byte_status,v[0]);
				break;
			default:
				m_bJoiningRoom = false;
				if(v.GetSize())
					m_pSink->OnServerMsg(v[0],PKT_ROOMIN,status);
				break;
		}
	}
	void _OnChatLeave(short status,CSimpleArray<CString> &v)
	{
		CYahooChatUser user = m_users.HandleOnUserLeave(v[1]);

		if(!user.m_name.CompareNoCase(m_name)){
			m_bInChat = false;
		}
		m_pSink->OnRoomOut(v[0],user.m_name);
	}
	void _OnChatText(short status,CSimpleArray<CString> &v)
	{
		u_short byte_status = (u_short)status;

		byte_status <<= 8;
		byte_status >>= 8;

		switch(byte_status){
			case TEXT_TOOLONG:
				m_pSink->OnServerMsg(v[0],PKT_SAY,status);
				break;
			default:
				if(v.GetSize() == 3){
					if(m_utf8){
						CString utf8 = FromUTF8(v[2]);
						m_pSink->OnText(v[0],v[1],utf8,NULL);
					}
					else
						m_pSink->OnText(v[0],v[1],v[2],NULL);
				}
				else if(v.GetSize() > 3){
					if(m_utf8){
						CString utf8 = FromUTF8(v[2]);
						m_pSink->OnText(v[0],v[1],utf8,v[3]);
					}
					else
						m_pSink->OnText(v[0],v[1],v[2],v[3]);
				}	
			break;
		}
	}
	void _OnInvite(short status,CSimpleArray<CString> &v)
	{
		switch(status){
			case INVITE_OK:
				// eu fui conviado
				//OnInvited(v.at(0),v.at(1),v.at(2));
				break;
			default:
				m_pSink->OnServerMsg(v[0],PKT_INVITE,status);
				break;
		}
	}
	void _OnAway(short status,CSimpleArray<CString> &v)
	{
		if(v.GetSize() == 3)
			m_pSink->OnAway(v[0],v[1],v[2]," ");
		else if(v.GetSize() > 3){
			if(m_utf8){
				CString tmp = FromUTF8(v[3]);
				m_pSink->OnAway(v[0],v[1],v[2],tmp);
			}
			else
				m_pSink->OnAway(v[0],v[1],v[2],v[3]);
		}
	}
	void _OnThink(short status,CSimpleArray<CString> &v)
	{
		if(m_utf8){
			CString tmp = FromUTF8(v[2]);
			m_pSink->OnThinking(v[0],v[1],tmp);
		}
		else{
			m_pSink->OnThinking(v[0],v[1],v[2]);
		}
	}
	void _OnEmote(short status,CSimpleArray<CString> &v)
	{
		CString tmp;

		ATLASSERT(v.GetSize() >= 2);
	//	if(m_utf8){
	//	tmp = FromUTF8(v[2]);
	//	}
		if(v.GetSize() > 3){
			if(m_utf8)
				m_pSink->OnEmotion(v[0],v[1],tmp,v[3]);
			else
				m_pSink->OnEmotion(v[0],v[1],v[2],v[3]);
		}
		else{
			if(m_utf8)
				m_pSink->OnEmotion(v[0],v[1],tmp,NULL);
			else
				m_pSink->OnEmotion(v[0],v[1],v[2],NULL);
		}
	}
	void _OnPm(short status,CSimpleArray<CString> &v)
	{
		if(m_utf8){
			CString tmp = FromUTF8(v[2]);
			m_pSink->OnPm(v[0],v[1],tmp);
		}
		else
			m_pSink->OnPm(v[0],v[1],v[2]);
	}
	void _OnSignIn(CYahooCookies &cookie)
	{
		m_cookie = cookie.MakeCookieString();
		m_pSink->OnSignIn(m_cookie);
	}
	void _OnSignInFailure()
	{
		m_pSink->OnSignIn(NULL);
	}

private:
	TSock m_socket;
	CPacketBuffer m_buf;
	CYahooChatPacketOut m_PacketOut;
	CYahooChatPacketIn m_PacketIn;
	CSimpleValArray<LPCSTR> m_args;
	IYahooChatEvents *m_pSink;
	IYahooChatSignIn *m_psi;
	CSigninSink<TSock> m_sis;
	//CCriticalSection m_cs;
	//CCriticalSection m_cs2;
	//CInterval m_PingTimer;

	CString m_name;
	CString m_pw;
	CString m_cookie;
	CString m_WebCamCookie;
	CString m_vcauth;
	CString m_vc;
	CString m_room;
	CString m_rmcat;
	CString m_rmspace;
	CYahooChatUsers m_users;
	CString m_room_desired;
	CString m_rmid_desired;
	
	bool m_bLogginIn:1;
	bool m_bLoggedIn:1;
	bool m_bLogginOut:1;
	bool m_bJoiningRoom:1;
	bool m_bInChat:1;
	bool m_utf8:1;
	bool m_bKeepUsers;
//
	void ParseJoinRoomCookies(LPCSTR cookies)
	{
		CStringToken st;
		CString next;
		CString key,val;
		CString copy = cookies;
		int nStart;

		// clear old ones
		m_vc.Empty();
		m_vcauth.Empty();
		m_rmcat.Empty();
		m_rmspace.Empty();

		// "/c/ad.html?.vc=1&.vcauth=cookie&.rmcat=number&.rmspace=number"
		nStart = copy.Find("?.");
		if(nStart < 0)
			return;
		nStart = copy.Delete(0,nStart + 2);
		if(nStart <= 0)
			return;
		
		st.Init(copy,"&.",true);
		while(true){
			next = st.GetNext2();
			if(next.IsEmpty())
				break;
			if(ParseKeyValue(next,key,val)){
				if(!key.CompareNoCase("vc")){
					m_vc = val;
				}
				else if(!key.CompareNoCase("vcauth")){
					m_vcauth = val;
				}
				else if(!key.CompareNoCase("rmcat")){
					m_rmcat = val;
				}
				else if(!key.CompareNoCase("rmspace")){
					m_rmspace = val;
				}
			}
		}
	}
};

/************** CYahooChatUser ****************/

void CYahooChatUser::CopyFrom(CYahooChatUser &user)
{
	int size = user.m_dunno.GetSize();
	int i;

	m_name = user.m_name;
	m_webcam = user.m_webcam;
	for(i=0;i<size;i++){
		m_dunno.Add(user.m_dunno[i]);
	}
}

void CYahooChatUser::Parse(LPCTSTR s)
{
	CStringToken st;
	CString next;

	m_name.Empty();
	m_dunno.RemoveAll();
	st.Init(CString(s),CString("\x02"));
		
	// name
	m_name = st.GetNext();
	if(m_name.IsEmpty())
		return;
	// webcam
	next = st.GetNext();
	if(next.IsEmpty())
		return;
	m_webcam = next[0] == '1' ? true : false;

	// others
	while(true){
		next = st.GetNext();
		if(next.IsEmpty())
			break;
		m_dunno.Add(next);
	}
}

/************** CYahooChatUserList *******************/


void CYahooChatUsers::Parse(LPCTSTR s)
{
	CStringToken st;
	CString next;
	CYahooChatUser user;

	st.Init(CString(s),CString("\x01"));
	while(true){
		next = st.GetNext();
		if(next.IsEmpty())
			break;
		user.m_dunno.RemoveAll();
		user.Parse(next);
		Add(user);
	}
}

CYahooChatUser CYahooChatUsers::HandleOnUserEnter(LPCSTR msg)
{
	CYahooChatUser user;
	user.Parse(msg);
	Add(user);
	return user;
}

// string from OnRoomOut - returns the name of the user
CYahooChatUser CYahooChatUsers::HandleOnUserLeave(LPCSTR msg)
{
	CYahooChatUser user;
	user.Parse(msg);
	Remove(user);
	return user;
}

/************** IYahooChat *************/
BOOL NO5YAHOO::YahooChatCreate(IYahooChat **ppYahooChat,IYahooChatEvents *pYahooChatEvents)
{
	ATLASSERT(ppYahooChat != NULL);
	ATLASSERT(*ppYahooChat == NULL);
	*ppYahooChat = (IYahooChat *) new CYahooChatT<CSimpleSocket>(pYahooChatEvents);
	ATLASSERT(*ppYahooChat != NULL);
	return (*ppYahooChat != NULL) ? TRUE : FALSE;
}

void NO5YAHOO::YahooChatDestroy(IYahooChat **ppYahooChat)
{
	CYahooChatT<CSimpleSocket> *p = (CYahooChatT<CSimpleSocket> *)(*ppYahooChat);
	delete p;
	*ppYahooChat = NULL;
}

