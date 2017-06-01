// YahooColor.h: interface for the CYahooColor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YAHOOCOLOR_H__B5791B49_3F65_11D9_A17B_A9024DBE6F42__INCLUDED_)
#define AFX_YAHOOCOLOR_H__B5791B49_3F65_11D9_A17B_A9024DBE6F42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef YAHOOCOLOR_H_
#define YAHOOCOLOR_H_
#include <no5tl\color.h>

namespace NO5YAHOO
{
using NO5TL::CColor;

class CYahooColor:public CColor
{
	bool SetFromYahoo(LPCTSTR s);
	bool SetFromInlineName(LPCTSTR s);
public:
	/** Yahoo Color String Formats */
	enum YCSF {
		YCSF_1,	// <#RRGGBB>
		YCSF_2,	// #RRGGBB
		YCSF_3, // RRGGBB
		YCSF_4, // .[#RRGGBBm
		YCSF_5,	//	<color_name>
	};
public:
	/**
		well constructors and operator= are not inherited so lets
		copy all of them
	*/
	explicit CYahooColor(COLORREF cr = 0x000000):CColor(cr)
	{
		//
	};
	CYahooColor(BYTE red,BYTE green,BYTE blue):CColor(red,green,blue)
	{
		//
	};
	CYahooColor(BYTE a[]):CColor(a)
	{
		//
	};
	CYahooColor(const CYahooColor &color):CColor(color)
	{
		//
	};
	CYahooColor(const CColor &color):CColor(color)
	{
		//
	};
	CYahooColor& operator=(const CYahooColor &color){
		if(this != &color){
			Set(color.Get());
		}
		return *this;
	};
	CYahooColor& operator=(COLORREF cr){
		Set(cr);
		return *this;
	};
	CYahooColor& operator=(const CColor color)
	{
		if(this != &color){
			Set(color.Get());
		}
		return *this;
	}
	/**
		Sets the color from a string in the yahoo format, that is
		"<#RRGGBB> or "#RRGGBB" or only "RRGGBB" or 0x1b[#rrggbbm
		where the numbersare in hexa
		Returns false if the format of the string is any other
	*/
	bool SetFromString(LPCTSTR s);
	CString  GetString(UINT uFormat = YCSF_1) const;
};

} // NO5YAHOO
#endif // YAHOOCOLOR_H_

#endif // !defined(AFX_YAHOOCOLOR_H__B5791B49_3F65_11D9_A17B_A9024DBE6F42__INCLUDED_)
