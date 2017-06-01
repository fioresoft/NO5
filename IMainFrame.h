// IMainFrame.h: interface for the IMainFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAINFRAME_H__F922AD94_304F_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_IMAINFRAME_H__F922AD94_304F_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// fwd decl
struct IUserListView;
class CViewBase;
struct IInputFrame;
struct IQuickIgnoreView;
struct IBuddyListView;

struct CInputParams
{
	bool bold :1;
	bool italic:1;
	bool underline:1;
	bool fade:1;
	bool blend:1;
	CString font;
	COLORREF color;
	int font_size;
	CString colorset;

	CInputParams()
	{
		color = 0;
		font_size = 0;
		bold = italic = underline = fade = blend = false;
	}
	CInputParams(const CInputParams &p)
	{
		Copy(p);
	}
	CInputParams & operator = (const CInputParams &p)
	{
		Copy(p);
		return *this;
	}
private:
	void Copy(const CInputParams &p)
	{
		bold = p.bold;
		italic = p.italic;
		underline = p.underline;
		fade = p.fade;
		blend = p.blend;
		font = p.font;
		font_size = p.font_size;
		color = p.color;
		colorset = p.colorset;
	}
};

struct IMainFrame  
{
	virtual IUserListView * UserListView(void) = 0;
	virtual void ActiveView(CViewBase *pView) = 0;
	virtual CViewBase * ActiveView(void) = 0;
	virtual IInputFrame * InputFrame(void) = 0;
	virtual HWND GetHandle(void) = 0;
	virtual void AddView(CViewBase *pView) = 0;
	virtual void RemoveView(CViewBase *pView) = 0;
	virtual void GetViews(CSimpleValArray<CViewBase*> &views) = 0;
	virtual CToolBarCtrl & GetChatToolbar(void) = 0;
	virtual void AddMarqueeString(LPCTSTR s) = 0;
	virtual IQuickIgnoreView * QuickIgnoreView(void) = 0;
	virtual IBuddyListView *  BuddyListView(void) = 0;
	virtual void ActivateChatView(LPCTSTR name) = 0;
	virtual ULONG CreateTimer(ULONG nInterval) = 0;
	virtual void DestroyTimer(ULONG nID) = 0;
	virtual void OpenBrowser(LPCTSTR url,bool bAsk) = 0;
};

#endif // !defined(AFX_IMAINFRAME_H__F922AD94_304F_11DA_A17D_000103DD18CD__INCLUDED_)
