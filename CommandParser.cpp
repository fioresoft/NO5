// CommandParser.cpp: implementation of the CCommandParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommandParser.h"
#include <no5tl\mystring.h>

BOOL CCommandParser::ParseCmd(LPCTSTR text)
{
	BOOL res = FALSE;
	CString tmp = text;

	if(text && lstrlen(text) && text[0] == '/' && text[1] != '/'){
		NO5TL::CStringToken st;
		m_line = text;
		tmp.Delete(0);
		st.Init(tmp," \t\r\n");
		m_cmd = st.GetNext();
		if(!m_cmd.IsEmpty()){
			m_args.RemoveAll();
			st.GetAll(m_args);
			res = TRUE;
		}
	}
	return res;
}

BOOL CCommandParser::GetArgs1(CString &arg1)
{
	BOOL res = FALSE;

	if(m_args.GetSize() >= 1){
		for(int i=0;i<m_args.GetSize();i++){
			arg1 += m_args[i];
			if(i != m_args.GetSize() - 1)
				arg1 += ' ';
		}
		res = TRUE;
	}
	return res;
}

BOOL CCommandParser::GetArgs2(CString &arg1,CString &arg2)
{
	BOOL res = FALSE;

	if(m_args.GetSize() >= 2){
		arg1 = m_args[0];
		for(int i=1;i<m_args.GetSize();i++){
			arg2 += m_args[i];
			if(i != m_args.GetSize() - 1)
				arg2 += ' ';
		}
		res = TRUE;
	}
	return res;
}