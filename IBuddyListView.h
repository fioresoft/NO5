// IBuddyListView.h: interface for the IBuddyListView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBUDDYLISTVIEW_H__D121E0E8_5DFC_468C_8CD6_1C7CF1DD9600__INCLUDED_)
#define AFX_IBUDDYLISTVIEW_H__D121E0E8_5DFC_468C_8CD6_1C7CF1DD9600__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// fwd declarations
struct CBuddy;
class CBuddyGroup;
class CBuddyList;

struct IBuddyListEvents
{
	virtual void blv_OnPm(LPCTSTR to) = 0;
	virtual void blv_OnRemoveBuddy(LPCTSTR name,LPCTSTR group) = 0;
	virtual void blv_OnProfile(LPCTSTR name) = 0;
	virtual void blv_OnRenameGroup(LPCTSTR group) = 0;
	virtual void blv_OnAddBuddy(LPCTSTR group) = 0;
	virtual void blv_OnViewCam(LPCTSTR name) = 0;
	virtual void blv_OnCamInvite(LPCTSTR name) = 0;
	virtual void blv_OnGotoUser(LPCTSTR name) = 0;
	virtual void blv_OnInviteUser(LPCTSTR name) = 0;
	virtual void blv_OnChangeView(LPCTSTR name) = 0;
};

struct IBuddyListView  
{
public:
	virtual void SetAcc(IBuddyListEvents *pSink,LPCTSTR acc,CBuddyList *p) = 0;
	virtual void RemoveAcc(LPCTSTR acc) = 0;
	virtual void OnBuddyGroup(CBuddyGroup *pg) = 0;
	virtual void OnUserAway(LPCTSTR user) = 0;
	virtual void OnAddBuddy(LPCTSTR group,CBuddy *pb) = 0;
	virtual void OnRemBuddy(LPCTSTR group,LPCTSTR name) = 0;
	virtual void OnRenameGroup(LPCTSTR old,LPCTSTR group) = 0;
};

#endif // !defined(AFX_IBUDDYLISTVIEW_H__D121E0E8_5DFC_468C_8CD6_1C7CF1DD9600__INCLUDED_)
