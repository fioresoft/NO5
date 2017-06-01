// ColorFile.h: interface for the CColorFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORFILE_H__CA7858B6_3981_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_COLORFILE_H__CA7858B6_3981_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <yahoo\yahoocolor.h>
using NO5YAHOO::CYahooColor;

struct IColorFile
{
	typedef CSimpleArray<CYahooColor> CColorList;
	//
	virtual BOOL SetFileName(LPCTSTR pFullPath) = 0;
	virtual BOOL AddColorSet(LPCTSTR name,CColorList &colors) = 0;
	virtual BOOL GetColorSet(LPCTSTR name,CColorList &colors) = 0;
	virtual BOOL DeleteColorSet(LPCTSTR name) = 0;
	virtual BOOL GetNames(CSimpleArray<CString> &names) = 0;
};

BOOL ColorFileCreate(IColorFile **pp);
void ColorFileDestroy(IColorFile **pp);


#endif // !defined(AFX_COLORFILE_H__CA7858B6_3981_11DA_A17D_000103DD18CD__INCLUDED_)
