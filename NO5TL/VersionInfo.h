// VersionInfo.h: interface for the CVersionInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERSIONINFO_H__2D807202_3DB4_4E7A_9B30_A753338412CA__INCLUDED_)
#define AFX_VERSIONINFO_H__2D807202_3DB4_4E7A_9B30_A753338412CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace NO5TL
{
class CVersionInfo
{
	LPVOID m_p;
public:
	CVersionInfo(void)
	{
		m_p = NULL;
	}
	~CVersionInfo(void)
	{
		if(m_p)
			delete []m_p;
	}
	BOOL Init(LPCTSTR file)
	{
		DWORD dummy = 0;
		BOOL res = FALSE;
		if(m_p){
			delete []m_p;
			m_p = NULL;
		}
		ULONG len = ::GetFileVersionInfoSize(const_cast<LPTSTR>(file),&dummy);
		if(len > 0){
			m_p = new BYTE[len];
			ZeroMemory(m_p,len);
			res = GetFileVersionInfo(const_cast<LPTSTR>(file),0,len,m_p);
		}
		return res;
	}
	// \StringFileInfo\lang-codepage\name
	BOOL GetString(int cp,LPCTSTR name,CString &out)
	{
		BOOL res = FALSE;

		if(m_p){
			UINT len = 0;
			LPVOID buf = NULL;
			LPTSTR p;
			CString s;
			s.Format("\\StringFileInfo\\%08x\\%s",cp,name);
			p = s.GetBuffer(0);
			res = VerQueryValue(m_p,p,&buf,&len);
			s.ReleaseBuffer();
			if(res){
				out = (LPCTSTR)buf;
			}
		}
		return res;
	}
};
} // NO5TL


#endif // !defined(AFX_VERSIONINFO_H__2D807202_3DB4_4E7A_9B30_A753338412CA__INCLUDED_)
