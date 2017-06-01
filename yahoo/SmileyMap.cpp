// SmileyMap.cpp: implementation of the CSmileyMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <no5tl\mystring.h>
#include <map>
#include <utility>
#include <fstream>
#include "SmileyMap.h"
using namespace std;
using namespace NO5TL;

namespace NO5YAHOO
{

class CmpStringByLen
{
public:
	bool operator () (const CString &s1,const CString &s2) const
	{
		int diff = s1.GetLength() - s2.GetLength();
		
		if(diff == 0){
			diff = s1.CompareNoCase(s2);
		}
		return diff > 0;
	}
};


class CSmileyMap:public ISmileyMap
{
	typedef std::map<CString,CString,CmpStringByLen> SmileyMap;
	typedef std::pair<CString,CString> StrPair;
	typedef SmileyMap::const_iterator citer;
	// maps smiley-code to file-name
	SmileyMap m_map;
	bool m_bLoaded;
	
public:
	CSmileyMap()
	{
		m_bLoaded = false;
	}
private:

	BOOL ParseLine(LPCSTR buf)
	{
		LPCSTR p,q;
		CString key,value;

		p = q = buf;

		while(*q && isspace(*q))
			q++;

		if(!(*q))
			return FALSE;

		while(*q){
			if(isspace(*q)){
				NO5TL::StringCopyN(key,p,0,(q - p));
				key.MakeLower();
				break;
			}
			q++;
		}
		while(*q && isspace(*q))
			q++;
		if(!(*q))
			return FALSE;
		p = q;
		while(*q && !isspace(*q))
			q++;
		if(q > p){
			NO5TL::StringCopyN(value,p,0,q  - p);
			m_map[key] = value;
			return TRUE;
		}
		return FALSE;
	}
public:
	// ISmileyMap
	virtual BOOL LoadMap(LPCSTR file)
	{
		std::ifstream in(file);
		BOOL res = FALSE;
		char buf[256] = {0};
		NO5TL::CStringToken st;

		if(in.is_open()){
			while(in.getline(buf,sizeof(buf)/sizeof(buf[0]))){
				BOOL res = ParseLine(buf);
				//ATLASSERT(res);
				lstrcpy(buf,"");
			}
			res = TRUE;
		}
		if(res){
			m_bLoaded = true;
		}
		return res;
	}
	virtual BOOL IsLoaded(void) const
	{
		return m_bLoaded ? TRUE : FALSE;
	}
	virtual BOOL IsSmiley(LPCTSTR text,CString &code)
	{
		citer it = m_map.begin();
		CString tmp = text;

		tmp.MakeLower();

		for(it=m_map.begin();it != m_map.end();it++){
			if(0 == tmp.Find(it->first,0)){
				code = it->first;
				break;
			}
		}
		return it != m_map.end() ? TRUE : FALSE;
	}
	virtual BOOL FindSmiley(LPCSTR code,CString &file)
	{
		citer it = m_map.begin();
		BOOL res = FALSE;

		while(it != m_map.end()){
			if(!it->first.CompareNoCase(code)){
				res = TRUE;
				file = it->second;
				break;
			}
			else
				it++;
		}
		return res;
	}
	virtual CString GetSmileyFile(LPCTSTR code)
	{
		CString res;
		FindSmiley(code,res);
		return res;
	}
};

void SmileyMapCreate(ISmileyMap **ppSmileyMap)
{
	ATLASSERT(ppSmileyMap && (!*ppSmileyMap));
	CSmileyMap *p = new CSmileyMap;
	ATLASSERT(p);
	*ppSmileyMap = (ISmileyMap *)p;
}

void SmileyMapDestroy(ISmileyMap **ppSmileyMap)
{
	CSmileyMap *p = (CSmileyMap *)(*ppSmileyMap);
	ATLASSERT(p);
	delete p;
	*ppSmileyMap = NULL;
}

} // NO5YAHOO
