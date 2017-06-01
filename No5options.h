// No5Settings.h: interface for the CNo5Settings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NO5SETTINGS_H__E268CB24_82CF_11D9_A17B_945A74D8E32D__INCLUDED_)
#define AFX_NO5SETTINGS_H__E268CB24_82CF_11D9_A17B_945A74D8E32D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// forward decl 
namespace NO5TL
{
	class CColor;
	class CStringArray;
}
namespace NO5YAHOO
{
	struct FontAll;
}

using NO5YAHOO::FontAll;
using NO5TL::CColor;


struct CPmOptions
{
	bool m_popup:1;	// popup a pmbox when pm received
	bool m_friends:1;	// accept from friends only
	bool m_nolinks:1;	// ignore pms with links
	bool m_topmost:1;	// make pm boxes topmost

	CPmOptions()
	{
		m_popup = false;
		m_friends = false;
		m_nolinks = false;
		m_topmost = false;
	}
};

struct CIgnoreOptions
{
	bool m_auto:1;		// enable auto-ignore
	bool m_flood:1;		// ignore floods ( max pps )
	bool m_spambots:1;	// users that post certain words after joining room
	bool m_badtags:1;	// users that post bad tags
	bool m_ybots:1;		// users marked as bot by yahoo
	DWORD m_tJoin;		// number of seconds after joining
	DWORD m_days;		// number of days to keep users in auto ignore
	DWORD m_days_perm;	// number of days to keep users in perm ignore
	DWORD m_pps;		// max pps if m_flood == true
	NO5TL::CStringArray *m_pWords;

	CIgnoreOptions(void)
	{
		m_auto = m_flood = false;
		m_spambots = false;
		m_tJoin = m_days = m_days_perm = 0;
		m_pWords = NULL;
		m_pps = 0;
		m_badtags = m_ybots = false;
	}
};

struct CMarqueeOptions
{
	ULONG m_elapse;
	COLORREF m_clrBack;
	COLORREF m_clrTxt;
	CMarqueeOptions()
	{
		m_elapse = 4;
		m_clrBack = 0x008000;
		m_clrTxt = 0xffffff;
	}
};

// interface to get/set options that are common to all accounts

struct IGeneralOptions
{
	virtual void SetFileName(LPCTSTR pFile) = 0;
	//
	virtual CString GetServerYmsg(void) = 0;
	virtual CString GetServerYcht(void) = 0;
	virtual UINT	GetPortYmsg(void) = 0;
	virtual UINT	GetPortYcht(void) = 0;
	virtual int		GetServersYmsg(CSimpleArray<CString> &servers) = 0;
	virtual int		GetServersYcht(CSimpleArray<CString> &servers) = 0;
	virtual int		GetPortsYmsg(CSimpleValArray<int> &ports) = 0;
	virtual int		GetPortsYcht(CSimpleValArray<int> &ports) = 0;
	virtual int		GetAccountNames(CSimpleArray<CString> &names) = 0;
	virtual CString	GetLastAccountName(void) = 0;
	virtual CString	GetLastProto(void) = 0;
	virtual CString	GetLastView(void) = 0;
	virtual CString GetRoomServer(void) = 0;

	virtual BOOL	SetLastProto(LPCTSTR proto) = 0;
	virtual BOOL	SetLastView(LPCTSTR view) = 0;
	virtual BOOL	SetServerYmsg(LPCTSTR server) = 0;
	virtual BOOL	SetServerYcht(LPCTSTR server) = 0;
	virtual BOOL	SetServersYmsg(CSimpleArray<CString> &servers) = 0;
	virtual BOOL	SetServersYcht(CSimpleArray<CString> &servers) = 0;
	virtual BOOL	SetPortYmsg(UINT port) = 0;
	virtual BOOL	SetPortYcht(UINT port) = 0;
	virtual BOOL	SetPortsYmsg(CSimpleValArray<int> &ports) = 0;
	virtual BOOL	SetPortsYcht(CSimpleValArray<int> &ports) = 0;
	virtual BOOL	SetAccountNames(const CSimpleArray<CString> &names) = 0;
	virtual BOOL	SetLastAccountName(LPCTSTR name) = 0;
	virtual BOOL	SetRoomServer(LPCTSTR name) = 0;
	//
	virtual BOOL	DeleteAccount(LPCTSTR name) = 0;
	//
	virtual BOOL	SetPmOptions(CPmOptions &options) = 0;
	virtual BOOL	GetPmOptions(CPmOptions &options) = 0;
	virtual BOOL	GetTopmost(void) = 0;
	virtual BOOL	SetTopmost(BOOL bSet) = 0;
	virtual BOOL	GetIgnoreOptions(CIgnoreOptions &options) = 0;
	virtual	BOOL	SetIgnoreOptions(const CIgnoreOptions &options) = 0;
	virtual BOOL	GetInMemoryOptions(void) = 0;
	virtual BOOL	WriteInMemoryOptions(void) = 0;
	virtual BOOL	SetMarqueeOptions(CMarqueeOptions &o) = 0;
	virtual BOOL	GetMarqueeOptions(CMarqueeOptions &o) = 0;
	virtual int		GetConnectionType(void)  = 0;
	virtual void	SetConnectionType(int i) = 0;
	// in memory options
	virtual BOOL	ShowEnter(void) const = 0;
	virtual void	ShowEnter(BOOL bShow) = 0;

	static IGeneralOptions * CreateMe(void);
	static void DestroyMe(IGeneralOptions **pp);
};

struct IAccountOptions
{
	virtual BOOL SetFile(LPCTSTR file,LPCTSTR name) = 0;
	//
	virtual CString GetPassword(void) = 0;
	virtual CString GetLastRoom(void) = 0;
	virtual CString GetName(void) = 0;
	virtual void GetFontOptions(FontAll &font) = 0;
	virtual CString GetColorSetName(void) = 0;
	virtual BOOL GetFade(void) = 0;
	virtual BOOL GetAlt(void) = 0;
	//
	virtual BOOL SetPassword(LPCTSTR pw) = 0;
	virtual BOOL SetLastRoom(LPCTSTR room) = 0;
	virtual void SetFontOptions(FontAll &font) = 0;
	virtual void SetColorSetName(LPCTSTR name) = 0;
	virtual void SetFade(BOOL bSet) = 0;
	virtual void SetAlt(BOOL bSet) = 0;

	static IAccountOptions * CreateMe(void);
	static void DestroyMe(IAccountOptions **pp);
};


struct IVisualOptions
{
	virtual BOOL SetFile(LPCTSTR file) = 0;

	// getting
	virtual CColor  ClrBack(void) const = 0;
	virtual FontAll & NameUser(void) = 0;
	virtual FontAll & NameBuddy(void) = 0;
	virtual FontAll & NameMe(void) = 0;
	virtual FontAll & TxtUser(void) = 0;
	virtual FontAll & TxtBuddy(void) = 0;
	virtual FontAll & TxtMe(void) = 0;
	virtual FontAll & TxtThought(void) = 0;
	virtual FontAll & TxtEmote(void) = 0;
	virtual FontAll & TxtLink(void) = 0;
	virtual FontAll & TxtPvt(void) = 0;
	virtual FontAll & TxtInvite(void) = 0;
	virtual FontAll & TxtAwayUsr(void) = 0;
	virtual FontAll & TxtAwayBuddy(void) = 0;
	virtual FontAll & TxtBuddyOnline(void) = 0;
	virtual FontAll & TxtEnterUsr(void) = 0;
	virtual FontAll & TxtEnterBuddy(void) = 0;
	virtual FontAll & TxtLeaveUsr(void) = 0;
	virtual FontAll & TxtLeaveBuddy(void) = 0;
	virtual FontAll & TxtError(void) = 0;
	virtual FontAll & TxtNo5(void) = 0;
	virtual FontAll & TxtMail(void) = 0;
	virtual FontAll & TxtYahoo(void) = 0;
	
	// setting
	virtual void ClrBack(COLORREF clr) = 0;
	virtual void NameUser(FontAll &f) = 0;
	virtual void NameBuddy(FontAll &f) = 0;
	virtual void NameMe(FontAll &f) = 0;
	virtual void TxtUser(FontAll &f) = 0;
	virtual void TxtBuddy(FontAll &f) = 0;
	virtual void TxtMe(FontAll &f) = 0;
	virtual void TxtThought(FontAll &f) = 0;
	virtual void TxtEmote(FontAll &f) = 0;
	virtual void TxtLink(FontAll &f) = 0;
	virtual void TxtPvt(FontAll &f) = 0;
	virtual void TxtInvite(FontAll &f) = 0;
	virtual void TxtAwayUsr(FontAll &f) = 0;
	virtual void TxtAwayBuddy(FontAll &f) = 0;
	virtual void TxtBuddyOnline(FontAll &f) = 0;
	virtual void TxtEnterUsr(FontAll &f) = 0;
	virtual void TxtEnterBuddy(FontAll &f) = 0;
	virtual void TxtLeaveUsr(FontAll &f) = 0;
	virtual void TxtLeaveBuddy(FontAll &f) = 0;
	virtual void TxtError(FontAll &f) = 0;
	virtual void TxtNo5(FontAll &f) = 0;
	virtual void TxtMail(FontAll &f) = 0;
	virtual void TxtYahoo(FontAll &f) = 0;
	//
	virtual bool Dirty(void) const = 0;
	virtual void Dirty(bool bSet) = 0;
	virtual void CopyFrom(IVisualOptions *p) = 0;
	virtual void LoadDefaults(void) = 0;

	static IVisualOptions * CreateMe(void);
	static void DestroyMe(IVisualOptions **pp);
};




#endif // !defined(AFX_NO5SETTINGS_H__E268CB24_82CF_11D9_A17B_945A74D8E32D__INCLUDED_)
