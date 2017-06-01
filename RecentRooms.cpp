// RecentRooms.cpp: implementation of the CRecentRooms class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <no5tl\winfile.h>
#include <no5tl\mystring.h>
#include "RecentRooms.h"
using namespace NO5TL;

//

CRecentRooms::CRecentRooms()
{
	m_bDirty = false;
	m_pRooms = new CUniqueStringStack(20);
}

CRecentRooms::~CRecentRooms()
{
	delete m_pRooms;
}

BOOL CRecentRooms::Read(LPCTSTR file)
{
	CWinFile wf;
	BOOL res;

	res = wf.Create(file,OPEN_EXISTING,GENERIC_READ);
	if(res){
		CDataBuffer db;

		res = wf.ReadAll(db);
		if(res){
			CStringToken st;
			CString next;
			
			db.AddNull();
			st.Init(db.GetData(),"\r\n",true);
			next = st.GetNext();
			while(!next.IsEmpty()){
				m_pRooms->Push(next);
				next = st.GetNext();
			}
		}
	}
	else{
		DWORD error = GetLastError();
		res = TRUE;
	}
	return res;
}

BOOL CRecentRooms::Write(LPCTSTR file)
{
	CWinFile wf;
	BOOL res;

	res = wf.Create(file,CREATE_ALWAYS,GENERIC_WRITE);
	if(res){
		const int count = m_pRooms->GetSize();
		int i;
		DWORD dummy;
		CString sAll;

		for(i=0;i<count && res && count < 20 ;i++){
			const CString &s = (*m_pRooms)[i];
			sAll += s;
			sAll += "\r\n";
		}
		res = wf.Write((void *)((LPCTSTR)sAll),sAll.GetLength(),&dummy);
		if(res)
			m_bDirty = false;
	}
	return res;
}

void CRecentRooms::Add(LPCTSTR room)
{
	m_pRooms->Push(room);
	m_bDirty = true;
}

void CRecentRooms::FillCombo(HWND hWndCombo)
{
	const int count = m_pRooms->GetSize();
	int i;
	CComboBox cb = hWndCombo;

	for(i=0;i<count;i++){
		cb.AddString(m_pRooms->operator[](i));
	}
}
