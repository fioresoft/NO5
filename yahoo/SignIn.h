// SignIn.h: interface for the CSignIn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIGNIN_H__931F8913_3E55_11D9_A17B_96836E6F0742__INCLUDED_)
#define AFX_SIGNIN_H__931F8913_3E55_11D9_A17B_96836E6F0742__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __ATLMISC_H__
	#error signin.h requires atlmisc.h
#endif //__ATLMISC_H__


namespace NO5YAHOO
{

struct CYahooCookies
{
	CString sY;	// cookie Y
	CString sT;	// cookie T
	// retorna a string Y=v=...; T=z=...
	CString MakeCookieString(void);
	CString MakeDhtmlString(void);
};

struct IYahooChatSignInEvents 
{
	virtual void OnSignIn(CYahooCookies &cookies) = 0;
	virtual void OnSignInFailure(BOOL BadPw,BOOL BadId) = 0;
	// IHttpClientEvents
	virtual void OnDataRead(char *buf,int len){}
	// IEventSocket
	virtual void OnSockConnecting(void){}
	virtual void OnSockConnect(int error){}
	virtual void OnSockClose(int error){}
	virtual void OnSockError(int error){}
	virtual void OnSockConnectTimeout(void){}
	virtual void OnSockResolvingAddress(void){}
	virtual void OnSockAddressResolved(int error){}
};

struct IYahooChatSignIn
{
	// host = login.yahoo.com ( por exemplo )
	virtual BOOL SignIn(SOCKADDR_IN &addr,LPCSTR host,LPCSTR name,
		LPCSTR pw) = 0;
	virtual BOOL SignIn(LPCSTR server,int port,LPCSTR host,LPCSTR name,LPCSTR pw) = 0;
	virtual BOOL IsBadID(void) const = 0;
	virtual BOOL IsBadPW(void) const = 0;
};

BOOL SignInCreate(IYahooChatSignIn **ppSignIn,IYahooChatSignInEvents *pSink);
void SignInDestroy(IYahooChatSignIn **ppSignIn);

} // NO5YAHOO

#endif // !defined(AFX_SIGNIN_H__931F8913_3E55_11D9_A17B_96836E6F0742__INCLUDED_)
