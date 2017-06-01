// No5Window.h: interface for the CNo5Window class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NO5WINDOW_H__B959AC0A_354B_11D7_A179_FE6E78F0B835__INCLUDED_)
#define AFX_NO5WINDOW_H__B959AC0A_354B_11D7_A179_FE6E78F0B835__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLWIN_H__
#error no5window.h requires atlwin.h 
#endif

#ifndef __ATLMISC_H__
#error no5window.h requires atlmisc.h 
#endif

namespace NO5TL
{

// styles used to create rebars and toolbars in rebars
const DWORD NO5TL_DEF_BAND_STYLE = (RBBS_GRIPPERALWAYS|\
	RBBS_CHILDEDGE);

const DWORD NO5TL_DEF_TB_STYLE = (WS_CHILD|WS_VISIBLE|CCS_NORESIZE|\
	CCS_NODIVIDER|CCS_NOPARENTALIGN|TBSTYLE_FLAT);

const DWORD NO5TL_DEF_REBAR_STYLE = (WS_CHILD|WS_VISIBLE|WS_BORDER|\
	WS_CLIPCHILDREN|WS_CLIPSIBLINGS|CCS_TOP|RBS_BANDBORDERS|\
	RBS_VARHEIGHT);

const UINT NO5TL_INSERTBAND_MASK = (RBBIM_CHILD|RBBIM_SIZE|\
	RBBIM_LPARAM|RBBIM_STYLE|RBBIM_CHILDSIZE|RBBIM_ID);

// helps to construct toolbars
#define BEGIN_TOOLBAR_MAP(name) TBBUTTON name[] = {
#define BUTTON_ENTRY(iBitmap,cmd,state,style,data,iString) \
	{iBitmap,cmd,state,style,data,iString},
#define BUTTON_PUSH(iBitmap,cmd,iString) \
	BUTTON_ENTRY(iBitmap,cmd,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,iString)
#define BUTTON_SEP() BUTTON_ENTRY(-1,-1,TBSTATE_ENABLED,TBSTYLE_SEP,0,-1)
#define END_TOOLBAR_MAP() };

// returns x^n
inline int power(int x,unsigned int n)
{
	int i;
	int res = 1;

	for(i=0;i<(int)n;i++){
		res *= x;
	}
	return res;
}

inline bool myatoi(char *s,int &x)
{
	int len;
	bool res = false;
	int dig;

	ATLASSERT(s != NULL);
	if(s != NULL){
		len = lstrlen(s);
				
		if(len){
			// verify we have only digits
			for(int i=0;i<len;i++){
				if(::IsCharAlpha(s[i]))
					break;
			}
			if(i == len){
				x = 0;
				int j;
				for(i=len-1,j=0;i>=0;i--,j++){
					dig = s[i] - '0';
					x += dig * power(10,j);
				}
				res = true;
			}
		}
	}
	return res;
}

class CNo5Window : public CWindow  
{
public:
	CNo5Window(HWND hWnd = NULL)
	{
		m_hWnd = hWnd;
	}
	CNo5Window& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	CNo5Window& operator<<(LPCTSTR s)
	{
		SetWindowText(s);
		return *this;
	}
	
	const CNo5Window& operator>>(CString &s) const
	{
		int len = GetWindowTextLength();

		if(len > 0){
			GetWindowText(s.GetBuffer(len),len+1);
			s.ReleaseBuffer();
		}
		return *this;
	}

	CNo5Window & operator<<(int i)
	{
		char buf[10] = {0};
		wsprintf(buf,"%d",i);
		SetWindowText(buf);
		return *this;
	}
	const CNo5Window & operator>>(int &i) const
	{
		char buf[10] = {0};
		bool res;

		GetWindowText(buf,sizeof(buf) - 1);
		res = myatoi(buf,i);
		ATLASSERT(res);
		return *this;
	}
	BOOL IsStylePresent(DWORD style) const
	{
		return ((GetStyle() & style) == style);
	}
	BOOL IsExStylePresent(DWORD style) const
	{
		return ((GetExStyle() & style) == style);
	}
	void ToggleStyle(DWORD style) 
	{
		BOOL IsPresent = IsStylePresent(style);
		DWORD add = 0;
		DWORD remove = 0;

		if(IsPresent)
			remove = style;
		else
			add = style;
		ModifyStyle(remove,add);
	}
	void ToggleExStyle(DWORD style) 
	{
		BOOL IsPresent = IsExStylePresent(style);
		DWORD add = 0;
		DWORD remove = 0;

		if(IsPresent)
			remove = style;
		else
			add = style;
		ModifyStyleEx(remove,add);
	}
	
	BOOL SetTopmostStyle(BOOL bSet = TRUE) 
	{
		HWND hInsertAfter = bSet ? HWND_TOPMOST : HWND_NOTOPMOST;
		return SetWindowPos(hInsertAfter,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|\
			SWP_FRAMECHANGED);
	}
	BOOL ToggleTopmostStyle(void)
	{
		BOOL bIsSet = IsExStylePresent(WS_EX_TOPMOST);
		return SetTopmostStyle(!bIsSet);
	}

};

} // NO5TL

#endif // !defined(AFX_NO5WINDOW_H__B959AC0A_354B_11D7_A179_FE6E78F0B835__INCLUDED_)
