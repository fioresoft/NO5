// YahooColor.cpp: implementation of the CYahooColor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "YahooColor.h"

namespace NO5YAHOO
{

/**
	Sets the color from a string in the yahoo format, that is
	"<#RRGGBB> or "#RRGGBB" or only "RRGGBB" where the numbers
	are in hexa Returns false if the format of the string is any other
*/
bool CYahooColor::SetFromString(LPCTSTR s)
{
	TCHAR szBuf[12] = {0};
	int r,g,b;
	int iRes = 0;
	bool bRes = false;

	lstrcpyn(szBuf,s,min(lstrlen(s) + 1,
		sizeof(szBuf)/sizeof(szBuf[0])));
	if(szBuf[0] == TEXT('<')){
		if(szBuf[1] == '#')
			iRes = _stscanf(szBuf,TEXT("<#%2x%2x%2x>"),&r,&g,&b);
		else 
			bRes = SetFromInlineName(s);
	}
	else if(szBuf[0] == TEXT('#')){
		iRes = _stscanf(szBuf,TEXT("#%2x%2x%2x"),&r,&g,&b);
	}
	else if(szBuf[0] == TEXT('\x1b')){
		bRes = SetFromYahoo(s);
	}
	else{
		iRes = _stscanf(szBuf,TEXT("%2x%2x%2x"),&r,&g,&b);
	}
	if(iRes == 3 && !bRes){
		if(r >= 0 && g >= 0 && b >= 0){
			if(r < 256 && g < 256 && b < 256){
				Set((BYTE)r,(BYTE)g,(BYTE)b);
				bRes = true;
			}
		}
	}
	return bRes;
};

CString  CYahooColor::GetString(UINT uFormat) const
{
	TCHAR szBuf[15] = {0};

	switch(uFormat){
		case YCSF_1:
			wsprintf(szBuf,TEXT("<#%02x%02x%02x>"),Red(),Green(),
				Blue());
			break;
		case YCSF_2:
			wsprintf(szBuf,TEXT("#%02x%02x%02x"),Red(),Green(),
				Blue());
			break;
		case YCSF_3:
			wsprintf(szBuf,TEXT("%02x%02x%02x"),Red(),Green(),
				Blue());
			break;
		case YCSF_4:
			wsprintf(szBuf,TEXT("\x1b[#%02x%02x%02xm"),Red(),
				Green(),Blue());
			break;
	}
	return CString(szBuf);
};

bool CYahooColor::SetFromYahoo(LPCTSTR s)
{
	unsigned int cores[10] = {0x000000,0xff0000,0xffff00,0x808080,
		0x008000,0xff80ff,0x800080,0x0080ff,0x0000ff,0x00ffff};
	unsigned int aColor[3];
	TCHAR szBuf[12] = {0};
	char *p;
	int i;
		
	lstrcpyn(szBuf,s,
		min( (lstrlen(s) + 1),(sizeof(szBuf)/sizeof(szBuf[0])) ) );
	p = &szBuf[0];

	if(*p != (char)0x1b)
		return false;
	p++;
	if(*p != '[')
		return false;
	p++;
	if(*p == '#'){// cor no formato rrggbb
		i = _stscanf(szBuf,"\x1b[#%2x%2x%2xm",&aColor[0],&aColor[1],
			&aColor[2]);
		if(i != 3)
			return false;
		Set((BYTE)aColor[0],(BYTE)aColor[1],(BYTE)aColor[2]);
		return true;
	}
	else if(isdigit(*p)){
		i = atoi(p);
		switch(i){
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
				Set((unsigned int)cores[i - 30]);
				return true;
			default:
				return false;
		}
	}
	return false;
}

struct ColorMap
{
	LPCTSTR name;
	COLORREF color;
};

using namespace NO5TL;

// "<name>"
bool CYahooColor::SetFromInlineName(LPCTSTR name)
{
	bool res = false;

	ColorMap cp[] = {\
		"<BLACK>",		Colors::BLACK,
		"<RED>",		Colors::RED,
		"<GREEN>",		Colors::GREEN,
		"<YELLOW>",		Colors::YELLOW,
		"<BLUE>",		Colors::BLUE,
		"<MAGENTA>",	Colors::FUCHSIA, // igual
		"<CYAN>",		Colors::AQUA,
		"<WHITE>",		Colors::WHITE,
		"<GRAY>",		Colors::GREY,
		"<GREY>",		Colors::GREY,
		"<PURPLE>",		Colors::PURPLE,
		"<FUCHSIA>",	Colors::FUCHSIA,
		"<PINK>",		Colors::PINK,
		"<ORANGE>",		Colors::ORANGE,
	};
	int i;
	for(i=0;i<sizeof(cp)/sizeof(cp[0]);i++){
		if(!lstrcmpi(cp[i].name,name)){
			Set(cp[i].color);
			res = true;
		}
	}
	return res;
}

} // NO5YAHOO
