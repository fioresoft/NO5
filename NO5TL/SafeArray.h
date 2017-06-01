// SafeArray.h: interface for the CSafeArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAFEARRAY_H__6300B8E1_1F40_11D9_A17B_D91F912C6947__INCLUDED_)
#define AFX_SAFEARRAY_H__6300B8E1_1F40_11D9_A17B_D91F912C6947__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef SAFEARRAY_H_
#define SAFEARRAY_H_

namespace NO5TL
{

template <class T,int vt>
class CSafeArray
{
	SAFEARRAY *m_psa;
	//
	HRESULT GetLBound(long &lBound)
	{
		HRESULT hr = E_POINTER;

		if(m_psa){
			hr = ::SafeArrayGetLBound(m_psa,1,&lBound);
		}
		return hr;
	}
	HRESULT GetUBound(long &uBound)
	{
		HRESULT hr = E_POINTER;

		if(m_psa){
			hr = ::SafeArrayGetUBound(m_psa,1,&uBound);
		}
		return hr;
	}
public:
	class CLock
	{
		SAFEARRAY *m_psa;
	public:
		CLock(SAFEARRAY *psa):m_psa(psa)
		{
			HRESULT hr = ::SafeArrayLock(m_psa);
			ATLASSERT(SUCCEEDED(hr));
		}
		~CLock()
		{
			HRESULT hr = ::SafeArrayUnlock(m_psa);
			ATLASSERT(SUCCEEDED(hr));
		}
	};
public:
	CSafeArray()
	{
		m_psa = NULL;
	}
	explicit CSafeArray(long count):m_psa(NULL)
	{
		HRESULT hr = Create(count);
		ATLASSERT(SUCCEEDED(hr));
	}
	explicit CSafeArray(CSafeArray<T,vt> &sa):m_psa(NULL)
	{
		CopyFrom(sa);
	}
	~CSafeArray()
	{
		HRESULT hr = Destroy();
		ATLASSERT(SUCCEEDED(hr));
	}
	HRESULT Create(long count)
	{
		SAFEARRAYBOUND bound = {count,0};
		HRESULT hr = Destroy();

		if(SUCCEEDED(hr)){
			m_psa = ::SafeArrayCreate(vt,1,&bound);
			if(m_psa){
				CLock lock(m_psa);
				::ZeroMemory(m_psa->pvData,count * sizeof(T));
				hr = S_OK;
			}
		}
		return hr;
	}
	HRESULT Destroy(void)
	{
		HRESULT hr = S_OK;

		if(m_psa){
			hr = ::SafeArrayDestroy(m_psa);
			if(SUCCEEDED(hr)){
				m_psa = NULL;
			}
		}
		return hr;
	}
	HRESULT Attach(SAFEARRAY *psa)
	{
		HRESULT hr = S_OK;

		if(m_psa){
			hr = Destroy();
		}
		if(SUCCEEDED(hr)){
			m_psa = psa;
		}
		return hr;
	}
	HRESULT DetachTo(SAFEARRAY **ppsa)
	{
		HRESULT hr = E_POINTER;

		if(ppsa){
			if(*ppsa){
				hr = ::SafeArrayDestroy(*ppsa);
			}
			else
				hr = S_OK;
			if(SUCCEEDED(hr)){
				*ppsa = m_psa;
				m_psa = NULL;
			}
		}
		return hr;
	}
	SAFEARRAY * Detach(void)
	{
		SAFEARRAY *psa = m_psa;
		m_psa = NULL;
		return psa;
	}
	HRESULT PutElement(long index,T &t)
	{
		HRESULT hr = E_POINTER;

		if(m_psa){
			T *p;
//
//			Pointer to the data to assign to the array. The variant
//			types VT_DISPATCH, VT_UNKNOWN, and VT_BSTR are pointers,
//			and do not require another level of indirection. 

			switch(vt){
				case VT_BSTR:
				case VT_UNKNOWN:
				case VT_DISPATCH:
					p = reinterpret_cast<T*>(t);
					break;
				default:
					p = &t;
					break;
			}
			hr = ::SafeArrayPutElement(m_psa,&index,(void *)p);
		}
		return hr;
	}
	HRESULT GetElement(long index,T &t)
	{
		HRESULT hr = E_POINTER;

		if(m_psa){
			hr = ::SafeArrayGetElement(m_psa,&index,(void *)&t);
		}
		return hr;
	}
	HRESULT GetCount(long &count)
	{
		long lbound,ubound;
		HRESULT hr;

		hr = GetLBound(lbound);
		if(SUCCEEDED(hr)){
			hr = GetUBound(ubound);
			if(SUCCEEDED(hr)){
				count = ubound - lbound + 1;
			}
		}
		return hr;
	}
	UINT GetDim(void){
		UINT res = 0;

		if(m_psa){
			res = ::SafeArrayGetDim(m_psa);
		}
		return res;
	}
	HRESULT CopyTo(SAFEARRAY **ppsa)
	{
		HRESULT hr = E_POINTER;

		if(ppsa && m_psa){
			if(*ppsa){
				hr = ::SafeArrayDestroy(*ppsa);
				ATLASSERT(SUCCEEDED(hr));
				if(SUCCEEDED(hr)){
					*ppsa = NULL;
				}
			}
			hr = ::SafeArrayCopy(m_psa,ppsa);
		}
		return hr;
	}

	HRESULT CopyTo(CSafeArray<T,vt> &rhs)
	{
		HRESULT hr = E_POINTER;

		if(m_psa){
			if(rhs.m_psa != NULL){
				hr = rhs.Destroy();
				ATLASSERT(SUCCEEDED(hr));
			}
			hr = ::SafeArrayCopy(m_psa,&rhs.m_psa);
		}
		return hr;
	}

	/*
	Copies the source array to the target array after releasing
	any resources in the target array. This is similar to
	SafeArrayCopy, except that the target array has to be set
	up by the caller. The target is not allocated or reallocated.
	*/
	
	// assuming vt == to.vt
	HRESULT CopyDataTo(SAFEARRAY *psa)
	{
		HRESULT hr = E_POINTER;

		if(psa && m_psa){
			// the documentation is  wrong, its psa, not &psa
			hr = ::SafeArrayCopyData(m_psa,psa);
		}
		return hr;
	}
	// assuming vt == to.vt
	HRESULT CopyDataTo(CSafeArray<T,vt> &to)
	{
		HRESULT hr = E_POINTER;

		if(m_psa && to.m_psa){
			hr = ::SafeArrayCopyData(m_psa,to.m_psa);
		}
		return hr;
	}
	// assuming vt == to.vt
	HRESULT CopyFrom(SAFEARRAY *psa)
	{
		Destroy();
		return ::SafeArrayCopy(psa,&m_psa);
	}
	// assuming vt == to.vt
	HRESULT CopyFrom(CSafeArray<T,vt> &from)
	{
		return CopyFrom((SAFEARRAY *)from);
	}
	// assuming vt == to.vt
	HRESULT CopyDataFrom(SAFEARRAY *psa)
	{
		return (::SafeArrayCopyData(psa,m_psa));
	}
	// assuming vt == to.vt
	HRESULT CopyDataFrom(CSafeArray<T,vt> &from)
	{
		return CopyDataFrom((SAFEARRAY *)from);
	}
	HRESULT ReDim(long count)
	{
		HRESULT hr = E_POINTER;
		SAFEARRAYBOUND bound = {count,0};

		if(m_psa){
			hr = ::SafeArrayRedim(m_psa,&bound);
		}
		return hr;
	}
	T * GetRawDataUnsafely(void)
	{
		return (T *)m_psa->pvData;
	}
	operator SAFEARRAY * ()
	{
		return m_psa;
	}
	operator SAFEARRAY ** ()
	{
		return &m_psa;
	}
	CSafeArray<T,vt> & operator=(CSafeArray<T,vt> &rhs)
	{
		if(this != &rhs){
			HRESULT hr =  CopyFrom(rhs);
			ATLASSERT(SUCCEEDED(hr));
		}
		return *this;
	}
	CSafeArray<T,vt> & operator=(SAFEARRAY *psa)
	{
		if(m_psa != psa){
			HRESULT hr = CopyFrom(psa);
			ATLASSERT(SUCCEEDED(hr));
		}
		return this;
	}
};


template <class T,int vt>
class CSafeArrayData
{
	CSafeArray<T,vt> &m_sa;
	T * m_pData;
public:
	CSafeArrayData(CSafeArray<T,vt> &sa):m_sa(sa)
	{
		m_pData = NULL;
		HRESULT hr = ::SafeArrayAccessData((SAFEARRAY *)m_sa,
			(void **)&m_pData);
		ATLASSERT(SUCCEEDED(hr));
	}
	~CSafeArrayData()
	{
		HRESULT hr = ::SafeArrayUnaccessData((SAFEARRAY *)m_sa);
		ATLASSERT(SUCCEEDED(hr));
	}
	T & operator[](long index)
	{
		return m_pData[index];
	}
	const T & operator[](long index) const
	{
		return m_pData[index];
	}
	T * begin(void)
	{
		return m_pData;
	}
	T * end()
	{
		long count;
		HRESULT hr = m_sa.GetCount(count);
		ATLASSERT(SUCCEEDED(hr));
		return &m_pData[count];
	}
};

} // NO5TL

# endif // SAFEARRAY_H_

#endif // !defined(AFX_SAFEARRAY_H__6300B8E1_1F40_11D9_A17B_D91F912C6947__INCLUDED_)
