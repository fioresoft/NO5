// IInputFrame.h: interface for the IInputFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IINPUTFRAME_H__C9013EA4_3C16_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_IINPUTFRAME_H__C9013EA4_3C16_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// interfaces to communicate with the bottom frame

#include <no5tl\mystring.h>	// CStringArray
struct CInputParams;

struct IInputFrameEvents
{
	virtual void OnTextInput(LPCTSTR text) = 0;
	virtual void OnColorsetChange(LPCTSTR name) = 0;
	// pszPartial contains the word that must be searched. 
	// arr contains the names that begings with pszPartial
	virtual BOOL OnAutoComplete(LPCTSTR pszPartial,NO5TL::CStringArray &arr) = 0;
};

struct IInputFrame  
{
	virtual void SetParams(CInputParams &p) = 0;
	virtual void GetParams(CInputParams &p) = 0;
	virtual CToolBarCtrl & GetFontToolbar(void) = 0;
	virtual void SetColorsetName(LPCTSTR name,BOOL bNotifySink) = 0;
	virtual CString GetColorsetName(void) = 0;
	virtual void SetSink(IInputFrameEvents *pSink) = 0;
};

#endif // !defined(AFX_IINPUTFRAME_H__C9013EA4_3C16_11DA_A17D_000103DD18CD__INCLUDED_)
