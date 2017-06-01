// IUserListView.h: interface for the IUserListView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IUSERLISTVIEW_H__F922AD95_304F_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_IUSERLISTVIEW_H__F922AD95_304F_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUser;
class CUserList;

struct IUserListViewEvents
{
	virtual void OnUserList_Ignored(LPCTSTR name) = 0;
	virtual void OnUserList_Profile(LPCTSTR name) = 0;
	virtual void OnUserList_Mimic(LPCTSTR name) = 0;
	virtual void OnUserList_Pm(LPCTSTR name) = 0;
	virtual void OnUserList_Follow(LPCTSTR name) = 0;
	virtual void OnUserList_Cam(LPCTSTR name) = 0;
	virtual void OnUserList_CamInvite(LPCTSTR name) = 0;
	virtual void OnUserList_AddBuddy(LPCTSTR name) = 0;
	virtual void OnUserList_RemoveBuddy(LPCTSTR name) = 0;
};

struct IUserListView  
{
	virtual void SetSink(IUserListViewEvents *pSink) = 0;
	virtual void AddUser(CUser *p) = 0;
	virtual void RemoveUser(LPCTSTR name) = 0;
	virtual void RemoveAll(void) = 0;
	virtual void AddUsers(CUserList &users) = 0;
	// the user structure pointed by p may have new values, the pointer is the same
	virtual void UpdateUser(CUser *p) = 0;
};

struct IQuickIgnoreViewEvents
{
	virtual void OnQuick_RemoveUser(LPCTSTR name) = 0;
};

struct IQuickIgnoreView
{
	virtual void SetSink(IQuickIgnoreViewEvents *pSink) = 0;
	virtual void AddUser(LPCTSTR name) = 0;
	virtual void RemoveUser(LPCTSTR name) = 0;
	virtual void RemoveAll(void) = 0;
};


#endif // !defined(AFX_IUSERLISTVIEW_H__F922AD95_304F_11DA_A17D_000103DD18CD__INCLUDED_)
