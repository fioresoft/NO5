// splitframe.h: interface for the splitframe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLITFRAME_H__61282B76_25AE_11D9_A17B_EBC8ACA76378__INCLUDED_)
#define AFX_SPLITFRAME_H__61282B76_25AE_11D9_A17B_EBC8ACA76378__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLFRAME_H__
#error splitframe.h requires atlframe.h to be included first
#endif

#ifndef __ATLSPLIT_H__
#error splitframe.h requires atlsplit.h to be included first
#endif


template <class T,bool t_bVertical = true, class TBase = CMDIWindow, class TWinTraits = CFrameWinTraits>
class ATL_NO_VTABLE CMDISplitterFrameImpl : public CMDIFrameWindowImpl<T,TBase,TWinTraits>,
	public CSplitterImpl<T,t_bVertical>
{
	typedef CMDIFrameWindowImpl<T,CMDIWindow,TWinTraits> _BaseClass;
	typedef CSplitterImpl<T,t_bVertical > _splitter;
	typedef CMDISplitterFrameImpl<T,t_bVertical,TBase,TWinTraits> _ThisClass;
public:
	BEGIN_MSG_MAP(_ThisClass)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(_splitter)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		_splitter::OnCreate(uMsg,wParam,lParam,bHandled);
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// handled, no background painting needed
		return 1;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(wParam != SIZE_MINIMIZED)
		{
			T* pT = static_cast<T*>(this);
			pT->UpdateLayout();
		}
		// message must be handled, otherwise DefFrameProc would resize the client again
		return 0;
	}

	void UpdateLayout(BOOL bResizeBars = TRUE)
	{
		RECT r = {0};

		GetClientRect(&r);
		UpdateBarsPosition(r,bResizeBars);
		SetSplitterRect(&r);
	}
};


#endif // !defined(AFX_SPLITFRAME_H__61282B76_25AE_11D9_A17B_EBC8ACA76378__INCLUDED_)
