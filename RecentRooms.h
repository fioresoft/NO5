// RecentRooms.h: interface for the CRecentRooms class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECENTROOMS_H__2E8EC809_472E_4A55_AB35_95F382112709__INCLUDED_)
#define AFX_RECENTROOMS_H__2E8EC809_472E_4A55_AB35_95F382112709__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <no5tl\mystring.h>


class CRecentRooms  
{
public:
	NO5TL::CUniqueStringStack *m_pRooms;
	bool m_bDirty;
public:
	CRecentRooms();
	~CRecentRooms();
	BOOL Read(LPCTSTR file);
	BOOL Write(LPCTSTR file);
	void Add(LPCTSTR room);
	bool IsDirty(void) const { return m_bDirty; }
	void FillCombo(HWND hWndCombo);
};

#endif // !defined(AFX_RECENTROOMS_H__2E8EC809_472E_4A55_AB35_95F382112709__INCLUDED_)
