// Socket.h: interface for the CSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKET_H__C9FB15C5_1DB2_11D9_A17B_AC9DAA311946__INCLUDED_)
#define AFX_SOCKET_H__C9FB15C5_1DB2_11D9_A17B_AC9DAA311946__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLMISC_H__
#error socket.h requires atlmisc.h 
#endif

#ifndef SD_BOTH
#define SD_BOTH 2
#endif

namespace NO5TL
{

class CWinSockDLL
{
	WSADATA m_data;
public:
	CWinSockDLL(BYTE major = 2,BYTE minor = 0)
	{
		ZeroMemory(&m_data,sizeof(m_data));
		if(major != 0){
			BOOL res = Load(major,minor);
			ATLASSERT(res);
		}
	}
	~CWinSockDLL()
	{
		if(IsLoaded() != 0){
			::WSACleanup();
		}
	}

	BOOL Load(BYTE major,BYTE minor)
	{
		WORD wRequested = MAKEWORD(major,minor);
		BOOL res = FALSE;

		if(::WSAStartup(wRequested,&m_data) == 0){
			if(LOBYTE(m_data.wVersion) == major && HIBYTE(m_data.wVersion) == minor){
				res = TRUE;
			}
			else{
				ZeroMemory(&m_data,sizeof(WSADATA));
			}
		}
		return res;
	}
	const WSADATA & GetData(void) const
	{
		return m_data;
	}
	BOOL IsLoaded(void) const
	{
		return m_data.wVersion != 0;
	}
};
	


// CPort ( port )	<---->	CService ( SERVENT )
// CIPAddress ( IN_ADDR )	<---->	CComputerAddress ( HOSTENT )
// CPort,CIPAddress		<---->		CSocketAddress ( SOCKADDR_IN )

class CPort
{
	u_short m_port;
public:
	CPort(void)
	{
		m_port = 0;
	}
	CPort(const CPort &port)
	{
		m_port = port.m_port;
	}
	CPort & operator = (const CPort &port)
	{
		if(this != &port){
			m_port = port.m_port;
		}
		return *this;
	}
	CPort(u_short port,bool HostOrder = true)
	{
		Set(port,HostOrder);
	}
	void Set(u_short port,bool HostOrder)
	{
		if(HostOrder)
			m_port = ::htons(port);
		else
			m_port = port;
	}
	u_short Get(bool HostOrder) const
	{
		u_short port;

		if(HostOrder)
			port = ::ntohs(m_port);
		else
			port = m_port;
		return port;
	}
	CString ToString(void) const
	{
		CString s;
		s.Format("%hu",Get(true));
		return s;
	}
};

class CService
{
	CString m_name;
	CPort m_port;
	CSimpleArray<CString> m_aliases;
	CString m_proto;

protected:
	void CopyServent(SERVENT *pse)
	{
		int i = 0;

		m_name = pse->s_name;
		m_port.Set(pse->s_port,false);
		m_aliases.RemoveAll();
		while(pse->s_aliases[i]){
			m_aliases.Add(CString(pse->s_aliases[i]));
			i++;
		}
		m_proto = pse->s_proto;
	}
public:
	BOOL GetByName(const char *name,const char *proto = NULL)
	{
		SERVENT *pse = ::getservbyname(name,proto);
		if(pse)
			CopyServent(pse);
		return NULL != pse;
	}
	BOOL GetByPort(const CPort &port,const char *proto = NULL)
	{
		SERVENT *pse = ::getservbyport(port.Get(false),proto);
		if(pse)
			CopyServent(pse);
		return NULL != pse;
	}
	const CString & GetName(void) const
	{
		return m_name;
	}
	const CPort & GetPort(void) const
	{
		return m_port;
	}
	const CSimpleArray<CString> & GetAliases(void) const
	{
		return m_aliases;
	}
	const CString & GetProtocol(void) const
	{
		return m_proto;
	}
	const CString & ToString(void) const
	{
		return GetName();
	}
};

class CIPAddress:public IN_ADDR
{
public:
	CIPAddress(void)
	{
		S_un.S_addr = INADDR_NONE;
	}
	CIPAddress(IN_ADDR ip)
	{
		Set(ip.S_un.S_addr,false);
	}
	CIPAddress(u_long ip,bool HostOrder)
	{
		Set(ip,HostOrder);
	}
	// dotted notation "a.b.c.d"
	CIPAddress(const char *ip)
	{
		Set(ip);
	}
	CIPAddress(const CIPAddress &ip)
	{
		S_un.S_addr = ip.S_un.S_addr;
	}
	CIPAddress & operator = (const CIPAddress &rhs)
	{
		if(this != &rhs){
			S_un.S_addr = rhs.S_un.S_addr;
		}
		return *this;
	}
	// it cant be virtual
	~CIPAddress()
	{
		//
	}
	void Set(u_long ip,bool HostOrder)
	{
		if(HostOrder)
			S_un.S_addr = ::htonl(ip);
		else
			S_un.S_addr = ip;
	}
	// dotted notation "a.b.c.d"
	void Set(const char *ip)
	{
		Set(::inet_addr(ip),false);
	}
	u_long Get(bool HostOrder) const
	{
		return HostOrder ? (ntohl(S_un.S_addr)) : S_un.S_addr;
	}
	CString ToString(void) const
	{
		const char *ip = ::inet_ntoa(*this);
		CString res;

		if(ip)
			res = ip;
		return res;
	}
	BOOL IsValid(void) const
	{
		return INADDR_NONE != Get(false);
	}
};

// only for AF_INET
class CComputerAddress
{
	CString m_name;
	CSimpleArray<CString> m_aliases;
	CSimpleArray<CIPAddress> m_ips;
protected:
	void CopyHostent(HOSTENT *phe)
	{
		int i = 0;

		ATLASSERT(phe->h_addrtype == AF_INET);
		ATLASSERT(phe->h_length == sizeof(IN_ADDR));
		ClearData();
		m_name = phe->h_name;
		while(phe->h_aliases[i]){
			m_aliases.Add(CString(phe->h_aliases[i]));
			i++;
		}
		i = 0;
		while(phe->h_addr_list[i]){
			u_long ul = *((u_long *)phe->h_addr_list[i]);
			CIPAddress ip(ul,false);
			i++;
			m_ips.Add(ip);
		}
	}

	void ClearData(void)
	{
		m_name = "";
		m_aliases.RemoveAll();
		m_ips.RemoveAll();
	}

public:
	virtual BOOL GetHostByName(LPCSTR name)
	{
		CIPAddress ip;
		BOOL res = FALSE;
			
		ip.Set(name);
		if(INADDR_NONE != ip.Get(false)){
			res = GetHostByAddr(ip);
		}
		else{
			HOSTENT *phe = ::gethostbyname(name);
			if(phe != NULL){
				CopyHostent(phe);
				res = TRUE;
			}
		}
		return res;
	}
	virtual BOOL GetHostByAddr(const CIPAddress &ip)
	{
		HOSTENT *phe = ::gethostbyaddr((char *)&ip.S_un.S_addr,
			sizeof(IN_ADDR),AF_INET);
		if(phe)
			CopyHostent(phe);
		return NULL != phe;
	}
	virtual const CString & GetName(void) const
	{
		return m_name;
	}
	virtual const CSimpleArray<CString> & GetAliases(void) const
	{
		return m_aliases;
	}
	virtual CIPAddress GetIP(void) const
	{
		if(m_ips.GetSize())
			return m_ips[0];
		else
			return CIPAddress();
	}
	virtual const CSimpleArray<CIPAddress> & GetIPs(void) const
	{
		return m_ips;
	}
};

class CSocketAddress:public SOCKADDR_IN
{
public:
	CSocketAddress()
	{
		::ZeroMemory(this,sizeof(SOCKADDR_IN));
	}
	CSocketAddress(LPCSTR addr,const CPort &port)
	{
		::ZeroMemory(this,sizeof(SOCKADDR_IN));
		BOOL res = Set(addr,port);
		ATLASSERT(res);
	}
	CSocketAddress(const CIPAddress &ip,const CPort &port)
	{
		::ZeroMemory(this,sizeof(SOCKADDR_IN));
		Set(ip,port);
	}
	CSocketAddress(SOCKADDR_IN *addr)
	{
		::ZeroMemory(this,sizeof(SOCKADDR_IN));
		Set(addr);
	}

	// it cant be virtual
	~CSocketAddress()
	{
		//
	}
	void Set(SOCKADDR_IN *addr)
	{
		::CopyMemory(this,addr,sizeof(SOCKADDR_IN));
	}
	void Set(const CIPAddress &ip,const CPort &port)
	{
		sin_family = AF_INET;
		sin_port = port.Get(false);
		sin_addr.S_un.S_addr = ip.Get(false);
	}
	BOOL Set(LPCSTR addr,const CPort &port)
	{
		BOOL res = FALSE;
		CComputerAddress ca;

		if(res = ca.GetHostByName(addr)){
			Set(ca.GetIP(),port);
		}
		return res;
	}
	CIPAddress GetIP(void) const
	{
		return CIPAddress(sin_addr);
	}
	CPort GetPort(void) const
	{
		return CPort(sin_port,false);
	}
	CString ToString(void) const
	{
		CString s = GetIP().ToString();
		s += ':';
		s.Append((int)(GetPort().Get(true)));
		return s;
	}
	BOOL IsValid(void) const
	{
		return GetIP().IsValid();
	}
};


struct SocketTraits
{
	typedef SOCKET handle_type;

	static BOOL Destroy(SOCKET s)
	{
		return ::closesocket(s) != SOCKET_ERROR;
	}
	static SOCKET InvalidHandle(void)
	{
		return INVALID_SOCKET;
	};
};

class CSocket
{
	struct ErrorEntry {
		int nID;
		const char* pcMessage;

		ErrorEntry(int id, const char* pc = 0) : nID(id), pcMessage(pc) 
		{ 
			//
		}
	};
	
	// should we allow those ?

	CSocket(CSocket &obj)
	{
		ATLASSERT(0);
	}
	CSocket & operator=(SOCKET s)
	{
		ATLASSERT(0);
		//Assign(s);
		return *this;
	}
	CSocket & operator = (CSocket &rhs)
	{
		ATLASSERT(0);
		//Assign(rhs);
		return *this;
	}
protected:
	SOCKET m_s;

	static SOCKET InvalidHandle(void)
	{
		return INVALID_SOCKET;
	}
public:
	CSocket(SOCKET s = InvalidHandle()):m_s(s)
	{
		
	}
	virtual ~CSocket()
	{
		CloseSocket();
	}
	BOOL IsHandleValid(void) const
	{
		return InvalidHandle() != m_s;
	}
	SOCKET GetHandle(void) const
	{
		return m_s;
	}
	// destroys any previous one
	CSocket & Assign(SOCKET s)
	{
		BOOL res = CloseSocket();

		ATLASSERT(res);
		// possible memory leak if close failed, anyway ...
		m_s = s;
		return *this;
	}

	virtual BOOL CreateSocket(int type = SOCK_STREAM,int proto = 0)
	{
		// this will destroy any previous one
		Assign(::socket(AF_INET,type,proto));
		return IsHandleValid();
	}
	virtual BOOL CloseSocket(void)
	{
		BOOL res = TRUE;

		if(IsHandleValid()){
			res = (::closesocket(m_s) != SOCKET_ERROR) ? TRUE : FALSE;
			if(res){
				m_s = InvalidHandle();
			}
		}
		return res;
	}
	BOOL Shutdown(int how = SD_BOTH) const
	{
		return !(::shutdown(GetHandle(),how) == SOCKET_ERROR);
	}
	BOOL Bind(const SOCKADDR *name,int namelen) const
	{
		return !(::bind(GetHandle(),name,namelen) == SOCKET_ERROR);
	}
	BOOL Bind(const SOCKADDR_IN *name) const
	{
		return Bind((SOCKADDR *)name,sizeof(*name));
	}
	BOOL BindToAny(const CPort &port)
	{
		CSocketAddress addr;
		
		addr.Set(CIPAddress(INADDR_ANY,false),port);
		return Bind(&addr);
	}
	virtual int Recv(char *buf,int len,int flags = 0) const
	{
		return ::recv(GetHandle(),buf,len,flags);
	}
	virtual int Send(char *buf,int len,int flags = 0) const
	{
		return ::send(GetHandle(),buf,len,flags);
	}
	virtual int SendString(const char *buf) const
	{
		return Send(const_cast<char*>(buf),lstrlen(buf));
	}
	BOOL Listen(int backlog = SOMAXCONN) const
	{
		return !(::listen(GetHandle(),backlog) == SOCKET_ERROR);
	}
	SOCKET Accept(SOCKADDR *addr = NULL,int *addrlen = NULL) const
	{
		SOCKET s = ::accept(GetHandle(),addr,addrlen);
		ATLASSERT(s != InvalidHandle());
		return s;
	}
	SOCKET Accept(CSocketAddress &addr) const
	{
		int addrlen = sizeof(SOCKADDR_IN);
		return Accept((SOCKADDR *)&addr,&addrlen);
	}
	virtual int Connect(SOCKADDR *addr,int addrlen) const
	{
		return ::connect(GetHandle(),addr,addrlen);
	}
	virtual int Connect(SOCKADDR_IN *addr) const
	{
		return Connect((SOCKADDR *)addr,sizeof(*addr));
	}
	virtual BOOL AttachAcceptedSocket(SOCKET s)
	{
		BOOL res = CloseSocket();
		if(res)
			m_s = s;
		return res;
	}
	// options manipulation
	/**
		Set options in the level SOL_SOCKET
	*/
	BOOL SetOption(int iOption,char *pVal,int iLen) const
	{
		ATLASSERT(IsHandleValid());
		int iRes = ::setsockopt(GetHandle(),SOL_SOCKET,iOption,pVal,iLen);

		ATLASSERT(iRes != SOCKET_ERROR);
		return iRes != SOCKET_ERROR;
	}
	BOOL SetOption(int iOption,int iValue) const
	{
		return SetOption(iOption,(char *)&iValue,sizeof(int));
	}
	/**
		Get options in the level SOL_SOCKET
	*/
	BOOL GetOption(int iOption,char *pVal,int *piLen) const 
	{
		ATLASSERT(IsHandleValid());
		int iRes = ::getsockopt(GetHandle(),SOL_SOCKET,iOption,pVal,
			piLen);
		ATLASSERT(iRes != SOCKET_ERROR);
		return iRes != SOCKET_ERROR;
	}
	int GetOption(int iOption) const
	{
		int iVal;
		int iLen = sizeof(int);

		GetOption(iOption,(char *)&iVal,&iLen);
		return iVal;
	}
	BOOL DontLinger(void) const
	{
		return SetOption(SO_DONTLINGER,TRUE);
	}
	BOOL Linger(u_short usTimeOut = 0 /* seconds */) const
	{
		LINGER l;

		l.l_onoff = 1;
		l.l_linger  = usTimeOut;
		return SetOption(SO_LINGER,(char *)&l,sizeof(LINGER));
	}
	BOOL GetLinger(void) const
	{
		LINGER l;
		int iLen = sizeof(LINGER);

		GetOption(SO_LINGER,(char *)&l,&iLen);
		return l.l_onoff?true:false;
	}
	int GetLingerTimeOut(void) const
	{
		LINGER l;
		int iLen = sizeof(LINGER);

		GetOption(SO_LINGER,(char *)&l,&iLen);
		return l.l_linger;
	}
	/** milliseconds,0 ms for infinite, minimum = 500 ms*/
	BOOL SetSendTimeOut(int iTime) const
	{
		return SetOption(SO_SNDTIMEO,iTime);
	}
	int GetSendTimeOut(void) const
	{
		return GetOption(SO_SNDTIMEO);
	}
	BOOL SetReceiveTimeOut(int iTime) const
	{
		return SetOption(SO_RCVTIMEO,iTime);
	}
	int GetReceiveTimeOut(void) const
	{
		return GetOption(SO_RCVTIMEO);
	}
	int GetError(void) const
	{
		return GetOption(SO_ERROR);
	}
	// io mode manipulation
	BOOL IOMode(long cmd,u_long *pArg) const
	{
		ATLASSERT(IsHandleValid());
		int iRes = ::ioctlsocket(GetHandle(),cmd,pArg);
		ATLASSERT(iRes != SOCKET_ERROR);
		return iRes != SOCKET_ERROR;
	}
	BOOL SetNonBlocking(bool bSet = true) const
	{
		u_long arg = bSet?1:0;
		return IOMode(FIONBIO,&arg);
	}
	 // NOTE: See Q192599 why this is a bad idea! ( very inneficient)
	u_long GetAvailableData(void) const
	{
		u_long arg = 0;
		IOMode(FIONREAD,&arg);
		return arg;
	}
	
	BOOL GetLocalAddress(SOCKADDR *buf,int *len) const
	{
		BOOL res = FALSE;

		ATLASSERT(IsHandleValid());
		ATLASSERT(buf != NULL && len != NULL);
		if(IsHandleValid() && buf != NULL && len != NULL){
			res = ::getsockname(GetHandle(),buf,len) == 0;
			ATLASSERT(res);
		}
		return res;
	}
	BOOL GetLocalAddress(CSocketAddress &addr) const
	{
		SOCKADDR_IN buf = {0};
		int len = sizeof(SOCKADDR_IN);
		BOOL res = GetLocalAddress((SOCKADDR *)&buf,&len);

		if(res && len == sizeof(buf)){
			addr.Set(&buf);
		}
		return res;
	}
	
	BOOL GetRemoteAddress(SOCKADDR *buf,int *len) const
	{
		BOOL res = FALSE;

		ATLASSERT(IsHandleValid());
		ATLASSERT(buf != NULL && len != NULL);
		if(IsHandleValid() && buf != NULL && len != NULL){
			res = ::getpeername(GetHandle(),buf,len) == 0;
			ATLASSERT(res);
		}
		return res;
	}

	BOOL GetRemoteAddress(CSocketAddress &addr) const
	{
		SOCKADDR_IN buf = {0};
		int len = sizeof(SOCKADDR_IN);
		BOOL res = GetRemoteAddress((SOCKADDR *)&buf,&len);

		if(res && len == sizeof(buf)){
			addr.Set(&buf);
		}
		return res;
	}

	static int GetLastError(void)
	{
		return ::WSAGetLastError();
	}
	static void SetLastError(int error) 
	{
		::WSASetLastError(error);
	}
	static LPCSTR GetErrorDesc(int error = WSAGetLastError())
	{
		/*static */ErrorEntry ErrorList[] = {
			ErrorEntry(0,                  "No error"),
			ErrorEntry(WSAEINTR,           "Interrupted system call"),
			ErrorEntry(WSAEBADF,           "Bad file number"),
			ErrorEntry(WSAEACCES,          "Permission denied"),
			ErrorEntry(WSAEFAULT,          "Bad address"),
			ErrorEntry(WSAEINVAL,          "Invalid argument"),
			ErrorEntry(WSAEMFILE,          "Too many open sockets"),
			ErrorEntry(WSAEWOULDBLOCK,     "Operation would block"),
			ErrorEntry(WSAEINPROGRESS,     "Operation now in progress"),
			ErrorEntry(WSAEALREADY,        "Operation already in progress"),
			ErrorEntry(WSAENOTSOCK,        "Socket operation on non-socket"),
			ErrorEntry(WSAEDESTADDRREQ,    "Destination address required"),
			ErrorEntry(WSAEMSGSIZE,        "Message too long"),
			ErrorEntry(WSAEPROTOTYPE,      "Protocol wrong type for socket"),
			ErrorEntry(WSAENOPROTOOPT,     "Bad protocol option"),
			ErrorEntry(WSAEPROTONOSUPPORT, "Protocol not supported"),
			ErrorEntry(WSAESOCKTNOSUPPORT, "Socket type not supported"),
			ErrorEntry(WSAEOPNOTSUPP,      "Operation not supported on socket"),
			ErrorEntry(WSAEPFNOSUPPORT,    "Protocol family not supported"),
			ErrorEntry(WSAEAFNOSUPPORT,    "Address family not supported"),
			ErrorEntry(WSAEADDRINUSE,      "Address already in use"),
			ErrorEntry(WSAEADDRNOTAVAIL,   "Can't assign requested address"),
			ErrorEntry(WSAENETDOWN,        "Network is down"),
			ErrorEntry(WSAENETUNREACH,     "Network is unreachable"),
			ErrorEntry(WSAENETRESET,       "Net connection reset"),
			ErrorEntry(WSAECONNABORTED,    "Software caused connection abort"),
			ErrorEntry(WSAECONNRESET,      "Connection reset by peer"),
			ErrorEntry(WSAENOBUFS,         "No buffer space available"),
			ErrorEntry(WSAEISCONN,         "Socket is already connected"),
			ErrorEntry(WSAENOTCONN,        "Socket is not connected"),
			ErrorEntry(WSAESHUTDOWN,       "Can't send after socket shutdown"),
			ErrorEntry(WSAETOOMANYREFS,    "Too many references, can't splice"),
			ErrorEntry(WSAETIMEDOUT,       "Connection timed out"),
			ErrorEntry(WSAECONNREFUSED,    "Connection refused"),
			ErrorEntry(WSAELOOP,           "Too many levels of symbolic links"),
			ErrorEntry(WSAENAMETOOLONG,    "File name too long"),
			ErrorEntry(WSAEHOSTDOWN,       "Host is down"),
			ErrorEntry(WSAEHOSTUNREACH,    "No route to host"),
			ErrorEntry(WSAENOTEMPTY,       "Directory not empty"),
			ErrorEntry(WSAEPROCLIM,        "Too many processes"),
			ErrorEntry(WSAEUSERS,          "Too many users"),
			ErrorEntry(WSAEDQUOT,          "Disc quota exceeded"),
			ErrorEntry(WSAESTALE,          "Stale NFS file handle"),
			ErrorEntry(WSAEREMOTE,         "Too many levels of remote in path"),
			ErrorEntry(WSASYSNOTREADY,     "Network system is unavailable"),
			ErrorEntry(WSAVERNOTSUPPORTED, "Winsock version out of range"),
			ErrorEntry(WSANOTINITIALISED,  "WSAStartup not yet called"),
			ErrorEntry(WSAEDISCON,         "Graceful shutdown in progress"),
			ErrorEntry(WSAHOST_NOT_FOUND,  "Host not found"),
			ErrorEntry(WSANO_DATA,         "No host data of that type was found")
		};
		const int count = sizeof(ErrorList) / sizeof(ErrorEntry);
		int i;

		if(error == 0){
			error = GetLastError();
		}
		for(i=0;i<count;i++){
			if(error == ErrorList[i].nID)
				return ErrorList[i].pcMessage;
		}
		return NULL;
	}
};

} // NO5TL

#endif // !defined(AFX_SOCKET_H__C9FB15C5_1DB2_11D9_A17B_AC9DAA311946__INCLUDED_)
