// DataBuffer.h: interface for the CDataBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATABUFFER_H__6300B8E2_1F40_11D9_A17B_D91F912C6947__INCLUDED_)
#define AFX_DATABUFFER_H__6300B8E2_1F40_11D9_A17B_D91F912C6947__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "safearray.h"

namespace NO5TL
{

class CDataBuffer  
{
	char *m_pData;
	DWORD m_DataLen;
	DWORD m_BufLen;
	const DWORD m_IncLen;
	//
	void Realloc(void)
	{
		char *p = new char[m_BufLen + m_IncLen];
		ATLASSERT(p);
		if(p){
			::ZeroMemory(p,m_BufLen + m_IncLen);
			if(m_DataLen != 0)
				::MoveMemory(p,m_pData,m_DataLen);
			if(m_pData){
				delete [] m_pData;
			}
			m_pData = p;
			p = NULL;
			m_BufLen += m_IncLen;
		}
	}
	void EnsureBuffer(DWORD add)
	{
		while(m_DataLen + add > m_BufLen){
			Realloc();
		}
	}
	
public:
	CDataBuffer(DWORD incr = 1024):m_IncLen(incr)
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_BufLen = 0;
	}
	virtual ~CDataBuffer()
	{
		Destroy();
	}
	void Destroy(void)
	{
		if(m_pData){
			delete []m_pData;
			m_pData = NULL;
			m_DataLen = m_BufLen = 0;
		}
	}
	template <class T>
	void Add(T t)
	{
		char *pos;

		EnsureBuffer(sizeof(T));
		pos = m_pData + m_DataLen;
		::CopyMemory(pos,(void *)&t,sizeof(T));
		m_DataLen += sizeof(T);
	}
	template <>
	void Add(LPCSTR p)
	{
		char *pos;
		int len = lstrlen(p);

		EnsureBuffer(len);
		pos = m_pData + m_DataLen;
		::CopyMemory(pos,p,len);
		m_DataLen += len;
	}
	template <>
	void Add(BSTR s)
	{
		USES_CONVERSION;
		LPCSTR p = OLE2CA(s);
		Add(s);
	}
	void Add(LPVOID p,DWORD len)
	{
		char *pos;

		EnsureBuffer(len);
		pos = m_pData + m_DataLen;
		::CopyMemory(pos,p,len);
		m_DataLen += len;
	}
	template <class T>
	CDataBuffer & operator << (T t)
	{
		Add(t);
		return (*this);
	}
	char * GetData(void)
	{
		return m_pData;
	}
	DWORD GetDataLen(void) const
	{
		return m_DataLen;
	}
	void Reset(bool ZeroBuf = true)
	{
		if(ZeroBuf){
			::ZeroMemory(m_pData,m_BufLen);
		}
		m_DataLen = 0;
	}
	bool GetAsSafeArray(CSafeArray<u_char,VT_UI1> &sa)
	{
		HRESULT hr = sa.Create(m_DataLen);
		bool res = false;

		if(SUCCEEDED(hr)){
			CSafeArrayData<u_char,VT_UI1> data(sa);
			u_char *pData = data.begin();
			::CopyMemory(pData,m_pData,m_DataLen);
			res = true;
		}
		return res;
	}
	bool GetAsVariant(VARIANT *pv)
	{
		CComVariant v;
		CSafeArray<u_char,VT_UI1> sa;
		bool res = GetAsSafeArray(sa);

		if(res){
			v.vt = VT_UI1|VT_ARRAY;
			v.parray = (SAFEARRAY *)sa;
			sa.Detach();
			HRESULT hr = v.Detach(pv);
			res = SUCCEEDED(hr)?true:false;
		}
		return res;
	}
	// to transform into a null ending string
	void AddNull(void)
	{
		EnsureBuffer(1);
		Add('\0');
	}
};

}

#endif // !defined(AFX_DATABUFFER_H__6300B8E2_1F40_11D9_A17B_D91F912C6947__INCLUDED_)
