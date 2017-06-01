// IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__E268CB23_82CF_11D9_A17B_945A74D8E32D__INCLUDED_)
#define AFX_INIFILE_H__E268CB23_82CF_11D9_A17B_945A74D8E32D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mystring.h"

namespace NO5TL
{

class CPrivateIniFile
{
	CString m_path;	// full path and file name
public:
	CPrivateIniFile()
	{
		//
	}
	CPrivateIniFile(LPCTSTR pFullPath)
	{
		BOOL res = Create(pFullPath);
		ATLASSERT(res);
	}
	BOOL Create(LPCTSTR pFullPath)
	{
		HANDLE hFile = CreateFile(\
			pFullPath,
			GENERIC_READ|GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		BOOL res = FALSE;

		if(hFile != INVALID_HANDLE_VALUE){
			m_path = pFullPath;
			CloseHandle(hFile);
			res = TRUE;
		}
		return res;
	}
	BOOL WriteSection(LPCTSTR section,LPCTSTR pairs)
	{
		ATLASSERT( !m_path.IsEmpty() );
		return WritePrivateProfileSection(section,pairs,m_path);
	}
	BOOL WriteString(LPCTSTR section,LPCTSTR key,LPCTSTR value)
	{
		ATLASSERT( !m_path.IsEmpty() );
		return WritePrivateProfileString(section,key,value,m_path);
	}
	BOOL WriteStringList(LPCTSTR section,LPCTSTR key,const CSimpleArray<CString> &lst)
	{
		CString sCommaList;
		sCommaList = NO5TL::MakeCommaList(lst);
		return WriteString(section,key,sCommaList);
	}
	BOOL WriteIntList(LPCTSTR section,LPCTSTR key,const CSimpleValArray<int> &lst)
	{
		CString sCommaList;
		sCommaList = NO5TL::MakeCommaList(lst);
		return WriteString(section,key,sCommaList);
	}
	BOOL WriteInt(LPCTSTR section,LPCTSTR key,int val)
	{
		TCHAR buf[10] = {0};
		wsprintf(buf,_T("%d"),val);
		return WriteString(section,key,buf);
	}

	CString GetString(LPCTSTR section,LPCTSTR key,LPCTSTR defvalue,
		DWORD nSize)
	{
		CString value;
		ATLASSERT( !m_path.IsEmpty() );
		{
			CStringBuffer buf(value,nSize);
			DWORD res = GetPrivateProfileString(section,key,defvalue,
				buf,nSize,m_path);
		}
		return value;
	}
	UINT GetInt(LPCTSTR section,LPCTSTR key,UINT defvalue)
	{
		ATLASSERT( !m_path.IsEmpty() );
		return GetPrivateProfileInt(section,key,defvalue,m_path);
	}
	BOOL DeleteSection(LPCTSTR section)
	{
		ATLASSERT( !m_path.IsEmpty() );
		return WritePrivateProfileString(section,NULL,"",m_path);
	}
	BOOL DeleteKey(LPCTSTR section,LPCTSTR key)
	{
		ATLASSERT( !m_path.IsEmpty() );
		return WritePrivateProfileString(section,key,NULL,m_path);
	}
	// retrives a list that is internally stored as a comma separated list
	// the parameter lst is in/out , on imput it contains the default values
	int GetStringList(LPCTSTR section,LPCTSTR key,CSimpleArray<CString> &lst)
	{
		CString sDefault = NO5TL::MakeCommaList(lst);
		CString res = GetString(section,key,sDefault,1024);
		lst.RemoveAll();
		return MakeStringList(res,lst);
	}
	int GetIntList(LPCTSTR section,LPCTSTR key,CSimpleValArray<int> &lst)
	{
		CString sDefault = NO5TL::MakeCommaList(lst);
		CString res = GetString(section,key,sDefault,1024);
		lst.RemoveAll();
		return NO5TL::MakeIntList(res,lst);
	}
	BOOL WriteStruct(LPCTSTR section,LPCTSTR key,LPVOID lpStruct,UINT uStructSize)
	{
		ATLASSERT( !m_path.IsEmpty() );
		return WritePrivateProfileStruct(section,key,lpStruct,uStructSize,m_path);
	}
	BOOL GetStruct(LPCTSTR section,LPCTSTR key,LPVOID lpStruct,UINT uStructSize)
	{
		ATLASSERT( !m_path.IsEmpty() );
		return GetPrivateProfileStruct(section,key,lpStruct,uStructSize,m_path);
	}
};

} // NO5TL


#endif // !defined(AFX_INIFILE_H__E268CB23_82CF_11D9_A17B_945A74D8E32D__INCLUDED_)
