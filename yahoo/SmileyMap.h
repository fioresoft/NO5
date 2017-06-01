// SmileyMap.h: interface for the CSmileyMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMILEYMAP_H__0A8236E3_46AE_11D9_A17B_ED59B6AD233F__INCLUDED_)
#define AFX_SMILEYMAP_H__0A8236E3_46AE_11D9_A17B_ED59B6AD233F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLMISC_H__
#error smileymap.h requires atlmisc.h
#endif

namespace NO5YAHOO
{

struct ISmileyMap
{
	virtual BOOL LoadMap(LPCSTR file) = 0;
	virtual BOOL IsLoaded(void) const = 0;
	virtual BOOL FindSmiley(LPCSTR code,CString &file) = 0;
	virtual CString GetSmileyFile(LPCTSTR code) = 0;
	virtual BOOL IsSmiley(LPCTSTR text,CString &code) = 0;
};

void SmileyMapCreate(ISmileyMap **ppSmileyMap);
void SmileyMapDestroy(ISmileyMap **ppSmileyMap);

} // NO5YAHOO




#endif // !defined(AFX_SMILEYMAP_H__0A8236E3_46AE_11D9_A17B_ED59B6AD233F__INCLUDED_)
