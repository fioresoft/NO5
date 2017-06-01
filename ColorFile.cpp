// ColorFile.cpp: implementation of the CColorFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ColorFile.h"
#include <no5tl\inifile.h>

LPCTSTR GLOBAL_SECTION = _T("@global");
LPCTSTR COLOR_SECTION = _T("colors");

class CColorFile : public IColorFile
{
	NO5TL::CPrivateIniFile m_ini;
	typedef CSimpleValArray<int> CIntList;
	typedef CSimpleArray<CString> CStringList;
public:
	virtual BOOL SetFileName(LPCTSTR pFullPath)
	{
		return m_ini.Create(pFullPath);
	}
	virtual BOOL AddColorSet(LPCTSTR name,CColorList &colors)
	{
		CIntList icolors;
		BOOL res;

		TransformList(icolors,colors);
		res = m_ini.WriteIntList(COLOR_SECTION,name,icolors);
		if(res){
			AddName(name);
		}
		return res;
	}
	virtual BOOL GetColorSet(LPCTSTR name,CColorList &colors)
	{
		CIntList icolors;
		int res;
		

		res = m_ini.GetIntList(COLOR_SECTION,name,icolors);
		if(res >= 0){
			colors.RemoveAll();
			if(res > 0)
				TransformList(colors,icolors);
		}
		return res >= 0;
	}
	virtual BOOL DeleteColorSet(LPCTSTR name)
	{
		BOOL res =  m_ini.DeleteKey(COLOR_SECTION,name);

		if(res){
			RemoveName(name);
		}
		return res;
	}
	virtual BOOL GetNames(CSimpleArray<CString> &names)
	{
		int count = m_ini.GetStringList(GLOBAL_SECTION,"names",names);
		return count >= 0;
	}
private:
	void TransformList(CIntList &to,const CColorList &from)
	{
		const int count = from.GetSize();
		int i;
		CYahooColor color;

		to.RemoveAll();
		for(i=0;i<count;i++){
			color = from[i];
			to.Add((int)(COLORREF)color);
		}
	}
	void TransformList(CColorList &to,const CIntList &from)
	{
		const int count = from.GetSize();
		int i;
		CYahooColor color;

		to.RemoveAll();
		for(i=0;i<count;i++){
			color = (COLORREF)(from[i] & 0x00FFFFFF);
			to.Add(color);
		}
	}
	void AddName(LPCTSTR name)
	{
		CSimpleArray<CString> names;
		
		m_ini.GetStringList(GLOBAL_SECTION,"names",names);
		if(NO5TL::StringArray_FindNoCase(names,name) < 0){
			names.Add(CString(name));
			m_ini.WriteStringList("@global","names",names);
		}
	}
	void RemoveName(LPCTSTR name)
	{
		CSimpleArray<CString> names;
		int index;

		m_ini.GetStringList(GLOBAL_SECTION,"names",names);
		index = NO5TL::StringArray_FindNoCase(names,name);
		if(index >=0){
			names.RemoveAt(index);
			m_ini.WriteStringList("@global","names",names);
		}
	}

};

BOOL ColorFileCreate(IColorFile **pp)
{
	BOOL res = FALSE;
	CColorFile *pObj = new CColorFile();

	if(pObj){
		*pp = (IColorFile *)pObj;
		res = TRUE;
	}
	return res;
}

void ColorFileDestroy(IColorFile **pp)
{
	CColorFile *pObj = (CColorFile *)(*pp);

	if(pObj){
		delete pObj;
		*pp = NULL;
	}
}
