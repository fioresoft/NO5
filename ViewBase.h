// ViewBase.h: interface for the CViewBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWBASE_H__C9013EA3_3C16_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_VIEWBASE_H__C9013EA3_3C16_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "imainframe.h"
#include "iinputframe.h"
#include "commandparser.h"

// this class has common stuff to all view types, be it chat views or text editor views
// or whatever
class CViewBase : \
	public IInputFrameEvents,
	public CCommandParser
{
public:
	enum ViewType
	{
		VIEW_CHAT_YCHT_RTF = 0,
		VIEW_CHAT_YCHT_HTML,
		VIEW_CHAT_YCHT_TEXT,
		VIEW_CHAT_YMSG_RTF,
		VIEW_CHAT_YMSG_HTML,
		VIEW_CHAT_YMSG_TEXT,
		VIEW_TYPE_UNKNOWN,
	};
public:
	CViewBase(void);
	virtual ~CViewBase();
	virtual ViewType GetViewType(void) const = 0;
	virtual bool IsChatView(void) const = 0;
	virtual bool IsActive(void) const { return m_bActive; }
	virtual bool IsHandleValid(void) = 0;
	virtual bool IsYchtView(void) const = 0;
	virtual HWND GetHandle(void) = 0;
protected:
	static IMainFrame *m_pFrame;
	bool m_bActive;
};

#endif // !defined(AFX_VIEWBASE_H__C9013EA3_3C16_11DA_A17D_000103DD18CD__INCLUDED_)
