// CommandParser.h: interface for the CCommandParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDPARSER_H__54D8F144_31BB_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_COMMANDPARSER_H__54D8F144_31BB_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCommandParser
{
public:
	CString m_cmd;
	CString m_line;
	CSimpleArray<CString> m_args;
	// returns TRUE if its a command
	// exemplo: /pm to "some text"
	// cmd = pm, arg1 = "to" arg2 = "some text"
	BOOL ParseCmd(LPCTSTR text);
	BOOL GetArgs1(CString &arg1);
	BOOL GetArgs2(CString &arg1,CString &arg2);
};


#endif // !defined(AFX_COMMANDPARSER_H__54D8F144_31BB_11DA_A17D_000103DD18CD__INCLUDED_)
