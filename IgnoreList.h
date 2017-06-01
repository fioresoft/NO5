// IgnoreList.h: interface for the CIgnoreList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IGNORELIST_H__AA7513E1_41B0_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_IGNORELIST_H__AA7513E1_41B0_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct IIgnoreList
{
	virtual bool read(LPCTSTR file) = 0;
	virtual bool write(LPCTSTR file) = 0;
	virtual void add(LPCTSTR name) = 0;
	virtual void add(LPCTSTR name,long t) = 0;
	virtual void remove(LPCTSTR name) = 0;
	virtual bool find(LPCTSTR name)  = 0;
	virtual bool find(LPCTSTR name,long &t)  = 0;
	virtual bool getat(int i,CString &name,long &t) = 0;
	virtual int size(void) const = 0;
	virtual void clear(void) = 0;
	virtual bool is_dirty(void) const = 0;

	static void CreateMe(IIgnoreList **pp);
	static void DestroyMe(IIgnoreList **pp);
};

inline long diffdays(long t1,long t0)
{
	return (t1 - t0) / ( 24 * 60 * 60 );
}

#endif // !defined(AFX_IGNORELIST_H__AA7513E1_41B0_11DA_A17D_000103DD18CD__INCLUDED_)
