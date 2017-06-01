// InfoTag.h: interface for the CInfoTag class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFOTAG_H__3B4465CB_0881_40D8_8423_F8A2CFB71F63__INCLUDED_)
#define AFX_INFOTAG_H__3B4465CB_0881_40D8_8423_F8A2CFB71F63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <no5tl\no5tlbase.h>

namespace NO5YAHOO
{

typedef NO5TL::CNo5SimpleMap<CString,CString> CInfoTagMap;

class CInfoTag
{
	CInfoTagMap m_map;
public:
	CInfoTag():m_map(false /* not multi map */)
	{
		//
	}
	CInfoTag(CInfoTag &inf):m_map(false)
	{
		CopyFrom(inf);
	}
	CInfoTag & operator = (CInfoTag & rhs)
	{
		return CopyFrom(rhs);
	}
	const CInfoTagMap & GetMap(void) const
	{
		return m_map;
	}

	bool Parse(CSimpleArray<CString> &props);

	CString GetVal(CString &key)
	{
		return m_map.Lookup(key);
	}
	// false if it already existed this key, or some error
	BOOL Add(LPCTSTR key,LPCTSTR val)
	{
		return m_map.Add(CString(key),CString(val));
	}
	// false on error
	BOOL ReplaceOrAdd(LPCTSTR key,LPCTSTR val)
	{
		BOOL res;

		if(!(res = Add(key,val)) ){
			res = m_map.SetAt(CString(key),CString(val));
		}
		return res;
	}
	
	// returns string <font INF key1:val key2:val2........>
	CString GetCode(void) const;
	// returns string INF key1:val1 key2:val2
	CString GetCode2(void) const;

	void Append(CInfoTagMap &map)
	{
		m_map.AppendFrom(map);
	}
	void Reset(void)
	{
		m_map.RemoveAll();
	}
	BOOL IsEmpty() const
	{
		return m_map.GetSize() == 0;
	}
private:
	CInfoTag & CopyFrom(CInfoTag &from)
	{
		if(this != &from){
			m_map.CopyFrom(from.m_map);
		}
		return *this;
	}

};

} // NO5YAHOO

#endif // !defined(AFX_INFOTAG_H__3B4465CB_0881_40D8_8423_F8A2CFB71F63__INCLUDED_)
