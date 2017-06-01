// no5stream.h: interface for the no5stream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NO5STREAM_H__EB493264_48BA_11D9_A17B_AE718FA8393E__INCLUDED_)
#define AFX_NO5STREAM_H__EB493264_48BA_11D9_A17B_AE718FA8393E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <no5tl\mystring.h>
#include <no5tl\no5richedit.h>
#include <no5tl\htmledit.h>
#include <yahoo\yahoocolor.h>
#include <yahoo\textstream.h>
#include <yahoo\smileymap.h>

using NO5TL::CNo5RichEdit;
using NO5TL::CHtmlCtrl;
using NO5YAHOO::CTextStream;
using NO5YAHOO::CYahooColor;

class CInputStream : public CTextStream
{
public:
	virtual void SendToDest(void)
	{
		ATLASSERT(0);
	}
	virtual void ScrollText(void)
	{
		ATLASSERT(0);
	}
	virtual bool IsSmiley(LPCTSTR p,CString &smiley);
};


class CRichEditStream : public CInputStream
{
	CNo5RichEdit *m_pEdit;
	CNo5RichEdit & GetEdit(void)
	{
		ATLASSERT(NULL != m_pEdit);
		return *m_pEdit;
	}
public:
	CRichEditStream(CNo5RichEdit *pEdit = NULL):m_pEdit(pEdit)
	{
	}
	void SetEdit(CNo5RichEdit *pEdit)
	{
		m_pEdit = pEdit;
	}
	virtual void SendToDest(void);
	virtual void ScrollText(void)
	{
		GetEdit().ScrollText();
	}
};

class CHtmlEditStream : public CInputStream
{
	CHtmlCtrl *m_pEdit;
	//
	static CString GetColorTag(COLORREF cr)
	{
		CString res;
		CYahooColor color = cr;

		res.Format("<font color=\"%s\">",color.GetString(CYahooColor::YCSF_2));
		return res;
	}
	static CString GetBoldTag(BOOL bOn)
	{
		if(bOn)
			return CString("<b>");
		else
			return CString("</b>");
	}
	static CString GetItalicTag(BOOL bOn)
	{
		if(bOn)
			return CString("<i>");
		else
			return CString("</i>");
	}
	static CString GetUnderlineTag(BOOL bOn)
	{
		if(bOn)
			return CString("<u>");
		else
			return CString("</u>");
	}
	static CString GetFontTag(LPCTSTR name,int size);
	static CString GetImgTag(LPCTSTR file);
	static CString GetLinkTag(BOOL bOn,LPCTSTR url = NULL,LPCTSTR text = NULL,LPCTSTR tip= NULL);
	

	CHtmlCtrl & GetEdit(void)
	{
		return * m_pEdit;
	}
public:
	CHtmlEditStream(CHtmlCtrl *pEdit = NULL):m_pEdit(pEdit)
	{
		
	}
	void SetEdit(CHtmlCtrl *pEdit)
	{
		m_pEdit = pEdit;
	}
	virtual void SendToDest(void);
	virtual void ScrollText(void)
	{
		HRESULT hr = GetEdit().ScrollText();
		ATLASSERT(SUCCEEDED(hr));
	}
	void GetHtml(CString &res);
};

class CEditCtlStream : public CInputStream
{
	CEdit m_edit;
public:
	CEditCtlStream(void)
	{
	}
	void SetEdit(const CEdit &edit)
	{
		m_edit = edit;
	}
	virtual void SendToDest(void);
	virtual void ScrollText(void);
};


#endif // !defined(AFX_NO5STREAM_H__EB493264_48BA_11D9_A17B_AE718FA8393E__INCLUDED_)
