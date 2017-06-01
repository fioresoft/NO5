// IgnoreList.cpp: implementation of the CIgnoreList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IgnoreList.h"
#include <iostream>
#include <fstream>
#include <set>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <no5tl\mystring.h>
using namespace std;

class CIgnoreList : public IIgnoreList
{
private:
	struct Data
	{
		//
		CString m_name;
		time_t m_time;
		//
		Data()
		{
			m_time = 0;
		}
		Data(LPCSTR name)
		{
			m_name = name;
			m_time = time(NULL);
		}

		bool operator < ( const Data &data) const
		{
			return  m_name.CompareNoCase(data.m_name) < 0;
		}

		friend ostream & operator << ( ostream & out,const Data &data)
		{
			if(!data.m_name.IsEmpty()){
				out << data.m_name.GetLength() << endl;
				out << (LPCSTR) data.m_name << endl;
				out << (int)data.m_time;
			}
			return out;
		}
		friend istream & operator >> ( istream &in,Data &data)
		{
			int len;

			// read string length
			in >> ws >> len;
			if(len > 0){
				data.m_name.Empty();
				data.m_time = 0;
				{
					NO5TL::CStringBuffer buf(data.m_name,len);

					// skip white spaces
					in >> ws;
					// read name
					in.read(buf,len);
					// read time as int
					in >> ws >> len;
					// assign time
					data.m_time = (time_t)len;
				}
			}
			return in;
		}

	};
	
	typedef set<Data> ListType;
	typedef ListType::iterator iter;
	typedef ListType::const_iterator citer;

	ListType m_list;
	bool m_dirty;

public:
	CIgnoreList()
	{
		m_dirty = false;
	}
	virtual ~CIgnoreList()
	{
	}
	
	virtual bool read(LPCTSTR file)
	{
		ifstream in(file);
		int count = 0;
		bool res = false;

		// if the file doesnt exist it will fail, coz we are using ifstream. but thats ok
		if(in.is_open()){
			int len = 0;
			Data data;
			int i;
			time_t cur = time(NULL);
			long days;

			// read number of items
			in >> count;
			m_list.clear();
			m_dirty = false;
			for(i=0;i<count;i++){
				if( in >> data ){
					days = long(data.m_time - cur)/( 24 * 60 * 60 );
					if(days > 0)
						m_list.insert(data);
				}
			}
			res = true;
		}
		return res;
	}
	virtual bool write(LPCTSTR file)
	{
		ofstream out(file);
		bool res = false;
		
		if(out){
			time_t cur = time(NULL);
			long days;

			// write number of items
			out << m_list.size() << endl;
			res = true;
			//sort();
			for(citer it = m_list.begin();res &&  it != m_list.end(); it++){
				// dont add if time expired
				days = long(it->m_time - cur)/( 24 * 60 * 60 );
				if(days > 0){
					if(!(out << (*it) << endl)){
						res = false;
					}
				}
			}
		}
		if(res)
			m_dirty = false;

		return res;
	}
	
	// add at a sorted position
	virtual void add(LPCTSTR name)
	{
		Data d;
		iter it;

		d.m_name = name;
		d.m_time = time(NULL);
		m_list.insert(d);
		m_dirty = true;
	}
	virtual void add(LPCTSTR name,long t)
	{
		Data d;
		iter it;

		d.m_name = name;
		d.m_time = static_cast<time_t>(t);
		m_list.insert(d);
		m_dirty = true;
	}
	virtual void remove(LPCTSTR name)
	{
		Data d;

		d.m_name = name;
		d.m_time = 0;
		m_list.erase(d);
		m_dirty = true;
	}
	bool find(LPCTSTR name,Data *p)
	{
		Data d(name);
		citer it = m_list.find(d);
		if(p && it != m_list.end())
			*p = *it;
		return it != m_list.end();
	}
	virtual bool find(LPCTSTR name)
	{
		return find(name,NULL);
	}
	virtual bool find(LPCTSTR name,long &t)
	{
		Data d;
		bool res = find(name,&d);
		if(res)
			t = static_cast<long>(d.m_time);
		return res;
	}
	virtual void clear(void)
	{
		m_list.clear();
		m_dirty = true;
	}
	virtual int size(void) const
	{
		return (int)m_list.size();
	}
	virtual bool is_dirty(void) const
	{
		return m_dirty;
	}
	virtual bool getat(int i,CString &name,long &t)
	{
		citer it = m_list.begin();
		bool res = false;
		int j = 0;

		while(it != m_list.end() && !res){
			if(j == i){
				name = it->m_name;
				t = static_cast<long>(it->m_time);
				res = true;
			}
			else{
				j++;
				it++;
			}
		}
		return res;
	}
};

void IIgnoreList::CreateMe(IIgnoreList **pp)
{
	ATLASSERT(pp && ( *pp == NULL ) );
	*pp = (IIgnoreList *) new CIgnoreList();
}
void IIgnoreList::DestroyMe(IIgnoreList **pp)
{
	ATLASSERT(pp && (*pp != NULL));
	if(*pp){
		delete (CIgnoreList *)(*pp);
		*pp = NULL;
	}
}
