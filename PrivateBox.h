// PrivateBox.h: interface for the CPrivateBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRIVATEBOX_H__C9013EB3_3C16_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_PRIVATEBOX_H__C9013EB3_3C16_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TODO: 
// we pass information to the view when a user is ignored, etc. but the view
//	doesnt inform us when it ignores,etc a user by other means... 

#include <no5tl\no5richedit.h>
#include "colorfile.h"
#include "usrmsgs.h"
#include "resource.h"

class CPmBottomFrame;
class CPrivateBox;
struct IVisualOptions;
class CUser;
class CPrivateBox;


struct IPrivateBoxEvents
{
	virtual void pm_OnUserIgnored(LPCTSTR name) = 0;
	virtual void pm_OnUserMimic(LPCTSTR name) = 0;
	virtual void pm_OnUserFollow(LPCTSTR name) = 0;
	virtual void pm_OnUserProfile(LPCTSTR name) = 0;
	virtual void pm_OnDestroy(LPCTSTR name,CPrivateBox *pm) = 0;
	virtual void pm_OnSend(CPrivateBox *pBox,LPCTSTR msg) = 0;
	virtual void pm_OnTyping(CPrivateBox *pBox,BOOL bStart) = 0;
};

typedef CWinTraits<WS_OVERLAPPEDWINDOW | \
	WS_VISIBLE,0> CPmboxTraits;


class CPrivateBox : \
	public CFrameWindowImpl<CPrivateBox,CWindow,CPmboxTraits>,
	public CUpdateUI<CPrivateBox>,
	public CMessageFilter,
	public NO5TL::CRichEditOleCallback
{
	typedef CFrameWindowImpl<CPrivateBox,CWindow,CPmboxTraits> _BaseClass;
public:
	DECLARE_FRAME_WND_CLASS("NO5CLS_PM", IDR_PMFRAME)

	CPrivateBox(IPrivateBoxEvents *pSink);
	virtual ~CPrivateBox();

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		delete this;
	}
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CPrivateBox)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(NO5WM_ENTER,OnEnter)
		MESSAGE_HANDLER(NO5WM_TYPING,OnTyping)
		COMMAND_ID_HANDLER(ID_USER_IGNORE,OnUserIgnore)
		COMMAND_ID_HANDLER(ID_USER_FOLLOW,OnUserFollow)
		COMMAND_ID_HANDLER(ID_USER_MIMIC,OnUserMimic)
		COMMAND_ID_HANDLER(ID_USER_PROFILE,OnUserProfile)
		COMMAND_ID_HANDLER(ID_FILE_CLOSE,OnFileClose)
		COMMAND_ID_HANDLER(ID_FILE_SAVE_AS,OnFileSaveAs)
		COMMAND_ID_HANDLER(ID_EDIT_COPY,OnEditCopy)
		COMMAND_CODE_HANDLER(CBN_SELENDOK,OnComboSelChange)
		NOTIFY_CODE_HANDLER(EN_LINK,OnEditLink)
		NOTIFY_CODE_HANDLER(EN_MSGFILTER,OnEditMsgFilter)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	BEGIN_UPDATE_UI_MAP(CPrivateBox)
		UPDATE_ELEMENT(ID_USER_IGNORE, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_USER_MIMIC, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_USER_FOLLOW, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	LRESULT OnCreate(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnDestroy(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnEnter(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnTyping(UINT,WPARAM wParam,LPARAM lParam,BOOL &bHandled);
	LRESULT OnUserIgnore(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnUserFollow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnUserMimic(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnUserProfile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnComboSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnEditMsgFilter(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnEditLink(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled);

	// IRichEditOleCallback
	STDMETHODIMP GetContextMenu(WORD seltype, LPOLEOBJECT pOleObj,
							    CHARRANGE* pchrg, HMENU* phMenu);

public: // public methods
	void UpdateLayout(BOOL bResizeBars = TRUE);
	void AddMessage(LPCTSTR from,LPCTSTR text);
	void DestroyPm(void);
	void ShowPm(void);
	void UpdateUser(void);
	void Typing(BOOL bStart);
public: // public members

	// all this data must be set by the client
	CString m_me;
	CUser *m_pUser;
public:
	static IVisualOptions *m_pvo;
private:
	NO5TL::CNo5RichEdit m_top;
	CPmBottomFrame *m_pBottom;
	CStatusBarCtrl m_sb;
	IPrivateBoxEvents *m_pSink;
	IColorFile::CColorList m_colors;
	bool m_bNotifyDestroy;
	static IColorFile *m_pcf;
	static DWORD m_count;
	
	BOOL CreateTopFrame(void);
	BOOL CreateBottomFrame(void);
};

class CPrivateBoxList
{
	CSimpleValArray<CPrivateBox *> m_arr;
public:
	CPrivateBox * Find(LPCTSTR name);
	/*
	BOOL SendText(LPCTSTR name,LPCTSTR text)
	{
		CPrivateBox *p = Find(name);
		BOOL res = FALSE;

		if(p){
			p->AddMessage(text);
			res = TRUE;
		}
		return res;
	}
	*/
	void DestroyBoxes(void)
	{
		CPrivateBox *p;
		const int count = m_arr.GetSize();
		int i;

		for(i=0;i<count;i++){
			p = m_arr[i];
			p->DestroyPm();
		}
	}
	BOOL RemoveBox(LPCTSTR name);
	BOOL AddBox(CPrivateBox *pBox)
	{
		return m_arr.Add(pBox);
	}
	void RemoveAll(void)
	{
		m_arr.RemoveAll();
	}
};



#endif // !defined(AFX_PRIVATEBOX_H__C9013EB3_3C16_11DA_A17D_000103DD18CD__INCLUDED_)
