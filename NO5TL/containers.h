// containers.h: interface for the containers class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTAINERS_H__1AB52C89_927C_11D7_A17A_F575EA193B2C__INCLUDED_)
#define AFX_CONTAINERS_H__1AB52C89_927C_11D7_A17A_F575EA193B2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace NO5TL
{

template <class T>
class CList
{
	struct ListItem
	{
		T m_t;
		ListItem *m_pNext;
		//
		ListItem()
		{
			m_pNext = NULL;
		}
		ListItem(const T &t)
		{
			m_t = t;
			m_pNext = NULL;
		}
		T & operator *()
		{
			return m_t;
		}
		const T & operator *() const
		{
			return m_t;
		}
		// prefix
		ListItem & operator++()
		{
			this = m_pNext;
			return *this;
		}
		// postfix
		ListItem & operator++(int)
		{
			ListItem *res = this;
			this = m_pNext;
			return *res;
		}
		ListItem * operator->()
		{
			return this;
		}
	};
	ListItem *m_pStart;
	//
	ListItem * GetNext(void)
	{
		ListItem *next = m_pStart;

		if(!next){
			next = new ListItem();
			ATLASSERT(next);
			m_pStart = next;
			return next;
		}
		while(next->m_pNext){
			next = next->m_pNext;
		}
		next->m_pNext = new ListItem();
		ATLASSERT(next->m_pNext);
		return next->m_pNext;
	}
	ListItem * GetItem(int index)
	{
		int i;
		ListItem *next = m_pStart;
		ATLASSERT(index >= 0 && index < GetSize());

		for(i=0;i<index;i++){
			next = next->m_pNext;
		}
		return next;
	}
public:
	typedef ListItem iterator;
public:
	CList(void)
	{
		m_pStart = NULL;
	}
	~CList()
	{
		RemoveAll();
	}
	int GetSize(void) const
	{
		int count = 0;
		ListItem *next = m_pStart;

		while(next){
			next = next->m_pNext;
			count++;
		}
		return count;
	}
	void Add(const T &t)
	{
		ListItem *next = GetNext();

		next->m_t = t;
	}
	void Insert(int index, const T &t)
	{
		ListItem *item;

		// index == GetSize() is ok here ( inserts at the end )
		ATLASSERT(index >= 0 && index <= GetSize());
		item = new ListItem(t);
		if(index == 0){
			item->m_pNext = m_pStart;
			m_pStart = item;
		}
		else{
			ListItem *prev = GetItem(index - 1);
			
			item->m_pNext = prev->m_pNext;
			prev->m_pNext = item;
		}
	}
	T & GetAt(int index)
	{
		ListItem *p;

		ATLASSERT(index >= 0 && index < GetSize());
		p = GetItem(index);
		return p->m_t;
	}
	void RemoveAt(int index)
	{
		ListItem *tmp;

		ATLASSERT(index >= 0 && index < GetSize());
		if(index == 0){
			tmp = m_pStart;
			m_pStart = m_pStart->m_pNext;
			delete tmp;
		}
		else{
			ListItem *prev = GetItem(index - 1);

			tmp = prev->m_pNext;
			prev->m_pNext = tmp->m_pNext;
			delete tmp;
		}
	}
	void RemoveAll(void)
	{
		int count = GetSize();
		int i;
		for(i=0;i<count;i++)
			RemoveAt(0);
	}
	int Find(const T &t)
	{
		int i = 0;
		int res = -1;
		ListItem *next = m_pStart;

		while(next){
			if(next->m_t == t){
				res = i;
				break;
			}
			else{
				next = next->m_pNext;
				i++;
			}
		}
		return res;
	}
	bool Remove(const T &t)
	{
		int index = Find(t);
		bool res = false;

		if(index >= 0){
			RemoveAt(index);
			res = true;
		}
		return res;
	}
};

template <class T1,class T2>
struct CPair
{
	T1 first;
	T2 second;
	CPair(){}
	CPair(const T1 &_first,const T2 &_second)
	{
		first = _first;
		second = _second;
	}
};

template <class Key,class T>
class CMultiMap
{
public:
	typedef CPair<Key,T> value_type;
private:
	CList<value_type> m_lst;
public:
	void Add(const Key &key,const T &t)
	{
		m_lst.Add(value_type(key,t));
	}
	value_type & GetAt(int index)
	{
		return m_lst.GetAt(index);
	}
	Key & GetKeyAt(int index)
	{
		value_type &res = m_lst.GetAt(index);
		return res.first;
	}
	T & GetValueAt(int index)
	{
		value_type &res = m_lst.GetAt(index);
		return res.second;
	}
	int GetSize(void) const
	{
		return m_lst.GetSize();
	}
	T Lookup(const Key &key)
	{
		int i;
		int size = m_lst.GetSize();
		value_type value;

		for(i=0;i<size;i++){
			value =  m_lst.GetAt(i);
			if(value.first == key)
				return value.second;
		}
		return T();
	}
	void RemoveAt(int index)
	{
		m_lst.RemoveAt(index);
	}
	void Remove(const Key &key)
	{
		int i;
		int size = m_lst.GetSize();
		value_type value;

		for(i=0;i<size;i++){
			value =  m_lst.GetAt(i);
			if(value.first == key)
				break;
		}
		if(i < size)
			m_lst.RemoveAt(i);
	}
	void RemoveAll()
	{
		m_lst.RemoveAll();
	}
};

} // NO5TL

#endif // !defined(AFX_CONTAINERS_H__1AB52C89_927C_11D7_A17A_F575EA193B2C__INCLUDED_)
