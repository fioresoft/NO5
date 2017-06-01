// YahooMsgAuth.h: interface for the CYahooMsgAuth class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YAHOOMSGAUTH_H__6D59CF63_4AA2_11D9_A17B_C9614B571C31__INCLUDED_)
#define AFX_YAHOOMSGAUTH_H__6D59CF63_4AA2_11D9_A17B_C9614B571C31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CYahooMsgAuth  
{
	CString m_user;
	CString m_pw;
	CString m_challenge;
public:
	CYahooMsgAuth();
	virtual ~CYahooMsgAuth();
};

#endif // !defined(AFX_YAHOOMSGAUTH_H__6D59CF63_4AA2_11D9_A17B_C9614B571C31__INCLUDED_)
