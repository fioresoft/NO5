// User.h: interface for the CUser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USER_H__3B13D7C3_3521_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_USER_H__3B13D7C3_3521_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <yahoo\infotag.h>
#include <no5tl\no5tlbase.h>	// CInterval,CPointerArray
using NO5TL::CInterval;
using NO5YAHOO::CInfoTag;

class CUser
{
	CUser & CopyFrom(const CUser &user);
public:
	enum UserGender
	{
		USER_GENDER_MALE = 0,
		USER_GENDER_FEMALE,
		USER_GENDER_UNKNOWN,
	};
public:
	CUser();
	~CUser();
	CUser(const CUser &user)
	{
		CopyFrom(user);
	}
	CUser & operator = (const CUser &user)
	{
		return CopyFrom(user);
	}
	void SetInfoTag(CInfoTag &info);
	CInfoTag & GetInfoTag(void);
	bool OnAnyPost(void);	// returns true if user should be temporarily ignored
	bool OnPost(void);		// idem
	bool OnPm(void);		// idem
	bool OnInvite(void);	// idem
	bool IsMale(void) const { return m_gender == USER_GENDER_MALE;}
	bool IsFemale(void) const { return m_gender == USER_GENDER_FEMALE;}
	bool IsIgnored(void) const	// tells if the user is ignored, by any means
	{
		return m_ignored;
	}
	bool SetLastLine(LPCTSTR s);	// true if repeated
	LPCTSTR GetClientName(void);
private:
	CInfoTag m_info;
public:
	CString m_name;
	CString m_nick;
	CString m_location;
	UserGender m_gender;
	short m_age;
	// flags
	bool m_cam:1;
	bool m_ignored:1;
	bool m_mimic:1;
	bool m_follow:1;
	bool m_buddy:1;
	bool m_chat:1;			// user is on chat
	bool m_pm:1;			// we have a pm box open with this user
	bool m_me:1;
public:
	/*
	LONG m_f1;	// f(x_i) = total posts 
	LONG m_f2;	// f(x_i-1) = total posts before that
	LONG m_f3; // f(x_i-2) = total posts before that
	double m_v;	// instantaneous velocity in posts/second
	LONG m_posts_dt;	// posts during dt
	DWORD m_t;			// total time
	void dt(DWORD h);
	*/
	// when a user posts something we set the start, then for each post we
	// we check the time, if it less the t , we increase the counter m_posts_dt
	// if the counter exceeds maximum allowed, the user can be ignored.
	// if the time is greater then t, than this is the new start, we zero m_posts_dt
	DWORD m_tStart;
	DWORD m_posts_dt;			// total of any kind of post in dt
	static DWORD MAX_PPS;
	static DWORD MAX_PP2S;
	DWORD m_posts;				// total posts in chat
	DWORD m_invites;			// total invites
	DWORD m_pms;				// total pms
	DWORD m_dwLastAction;		// last time user did anything. used to clean the list
	DWORD m_dwLinks;			// total links
	CString m_sLastLine;		// last line in chat
	DWORD m_repeated;			// repeated line counter
	DWORD m_tEnter;				// time the user entered the room
};

class CUserList
{
	NO5TL::CPointerArray<CUser> m_users;
public:
	CUserList();
	virtual ~CUserList();
	CUser * AddUser(const CUser &user);
	CUser * AddUser(LPCTSTR name);
	void RemoveUser(LPCSTR name);
	void RemoveAll(void);
	int GetSize(void) const;
	CUser * GetUser(LPCSTR name);
	CUser * GetUser(int i);
	NO5TL::CPointerArray<CUser> & GetUsers(void);
	CUser * GetUserInChat(LPCTSTR name);	// returns user name if it is in chat
	int GetChatCount(void) const;			// number of users in chat
	/*
	void dt(DWORD h)
	{
		for(int i=0;i<m_users.GetSize();i++)
			m_users[i]->dt(h);
	}
	*/
	void CleanUp(void);		// delete inative users ( and not in chat or pm )
	void SetChat(bool bSet);	// mark all users as in chat or not
};


struct CBuddy
{
	CString m_name;
	LONG	m_status;
	CString m_msg;

	CBuddy(void)
	{
		Reset();
	}
	CBuddy(const CBuddy &b)
	{
		CopyFrom(b);
	}
	CBuddy & operator = ( const CBuddy &b )
	{
		CopyFrom(b);
		return *this;
	}
	// compare by name
	bool operator == ( const CBuddy &b )
	{
		return (m_name.CompareNoCase(b.m_name) == 0);
	}
	void Reset(void);
private:
	void CopyFrom(const CBuddy &b)
	{
		if(this != &b){
			m_name = b.m_name;
			m_status = b.m_status;
			m_msg = b.m_msg;
		}
	}
};

class CBuddyGroup
{
public:
	CString m_name;
	NO5TL::CPointerArray<CBuddy> m_buddies;
	//
	CBuddyGroup() {}
	CBuddyGroup(const CBuddyGroup &rhs)
	{
		CopyFrom(rhs);
	}
	CBuddyGroup & operator = ( const CBuddyGroup &rhs )
	{
		CopyFrom(rhs);
		return *this;
	}
	bool operator == ( const CBuddyGroup &rhs )
	{
		return ( m_name.CompareNoCase(rhs.m_name) == 0 );
	}
	CBuddy * FindBuddy(LPCTSTR name);
private:
	void CopyFrom( const CBuddyGroup &rhs );
};

class CBuddyList : public NO5TL::CPointerArray<CBuddyGroup>
{
public:
	CBuddyList() {}
	CBuddyList( const CBuddyList &rhs )
	{
		CopyFrom(rhs);
	}
	CBuddyList & operator = ( const CBuddyList &rhs )
	{
		CopyFrom(rhs);
		return *this;
	}
	CBuddyGroup * FindGroup(LPCTSTR group);
	CBuddy * FindBuddy(LPCTSTR group,LPCTSTR name);
	CBuddy * FindBuddy(LPCTSTR name);
	CBuddyGroup * AddBuddies(LPCTSTR group,const CSimpleArray<CString> &buddies);
	// set the status for this buddy in all groups that he is in, if thats possible at all
	void SetBuddyStatus(LPCTSTR name,LONG status,LPCTSTR msg = NULL);
	CBuddy * AddBuddy(LPCTSTR group,const CBuddy &buddy);
	void RemoveBuddy(LPCTSTR group,LPCTSTR name);
private:
	void CopyFrom( const CBuddyList &rhs );
};


#endif // !defined(AFX_USER_H__3B13D7C3_3521_11DA_A17D_000103DD18CD__INCLUDED_)
