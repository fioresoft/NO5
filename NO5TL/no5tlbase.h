// no5tlbase.h: interface for the no5tlbase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NO5TLBASE_H__374E3C01_509A_11D9_A17B_CD5DC1F1C73C__INCLUDED_)
#define AFX_NO5TLBASE_H__374E3C01_509A_11D9_A17B_CD5DC1F1C73C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __ATLBASE_H__
	#error no5tlbase.h requires atlbase.h
#endif //__ATLBASE_H__

namespace NO5TL
{
	// this file should contain functions and classes that depend
	// only on atlbase.h

	// classes / functions in this file:
	// CGetCharSet
	// CInterval
	// CNo5SimpleMap
	// CoInit
	// CopySimpleArray,AppendSimpleArray,SimpleArrayAddElements


class CGetCharSet
{
public:
	typedef CSimpleValArray<BYTE> Charsets;
private:
	static int CALLBACK EnumFontsProc(const LOGFONT *plf,const\
		TEXTMETRIC *ptm,DWORD type,LPARAM lParam)
	{
		Charsets *pArr = (Charsets *)lParam;
		if(pArr){
			pArr->Add(plf->lfCharSet);
		}
		// stops enumerating
		return 0;
	}
public:

	// gets the list of charsets associated to some font name
	// returns the number of charsets added to the list
	static int GetFontCharSet(LPCTSTR FontName,Charsets &arr)
	{
		HDC hdc = ::CreateIC(TEXT("DISPLAY"),NULL,NULL,NULL);
		BYTE res = 0;

		if(hdc){
			::EnumFonts(hdc,FontName,EnumFontsProc,(LPARAM)&arr);
			::DeleteDC(hdc);
			res = arr.GetSize();
		}
		return res;
	}

	static BOOL IsSymbolCharSet(LPCTSTR FontName)
	{
		Charsets arr;
		int res = GetFontCharSet(FontName,arr);
		BYTE b = SYMBOL_CHARSET;
		return arr.Find(b) >= 0;
	}
};

class CInterval
{
	DWORD m_dwStart;
	DWORD m_dwEnd;
public:
	CInterval(void)
	{
		UpdateStart();
	}
	void UpdateStart(void)
	{
		m_dwStart = ::GetTickCount();
		m_dwEnd = m_dwStart;
	}
	/** returns the interval in milliseconds */
	DWORD UpdateEnd(void)
	{
		m_dwEnd = ::GetTickCount();
		return Get();
	};
	/** returns the interval in milliseconds */
	DWORD Get(void) const
	{
		return m_dwEnd - m_dwStart;
	}
};

template <class T>
void CopySimpleArray(CSimpleArray<T> &to,const CSimpleArray<T> &from)
{
	const int count = from.GetSize();
	int i;

	to.RemoveAll();
	for(i=0;i<count;i++){
		to.Add(from[i]);
	}
}	

// add n copies of t to arr
template <class T>
void SimpleArrayAddElements(CSimpleArray<T> &arr,T &t,int n)
{
	for(int i=0;i<n;i++){
		arr.Add(t);
	}
}

template <class T>
void AppendSimpleArray(CSimpleArray<T> &to,const CSimpleArray<T> &from)
{
	const int count = from.GetSize();
	int i;

	for(i=0;i<count;i++){
		to.Add(from[i]);
	}
}


// intended for small number of simple types or pointers
template <class TKey, class TVal>
class CNo5SimpleMap
{
public:
	CSimpleArray<TKey> m_keys;
	CSimpleArray<TVal> m_vals;
	const bool m_bMulti;

// Construction/destruction
	CNo5SimpleMap(bool bMultiMap) : m_bMulti(bMultiMap)
	{
		//
	}

	~CNo5SimpleMap()
	{
		RemoveAll();
	}

// Operations
	int GetSize() const
	{
		ATLASSERT(m_keys.GetSize() == m_vals.GetSize());
		return m_keys.GetSize();
	}
	
	BOOL Add(TKey &key,TVal &val)
	{
		BOOL res = FALSE;

		if(m_bMulti || (!m_bMulti && (FindKey(key) < 0) )){
			if(res = m_keys.Add(key))
				res = m_vals.Add(val);
		}
		return res;
	}
	// returns false if it didnt find key or some other error
	BOOL Remove(TKey &key)
	{
		BOOL res = FALSE;
		int index = FindKey(key);

		if(index >= 0){
			res = m_keys.Remove(index);
			if(res){
				res = m_vals.Remove(index);
			}
		}
		return res;
	}
	void RemoveAll()
	{
		m_keys.RemoveAll();
		m_vals.RemoveAll();
	}
	BOOL SetAt(TKey &key,TVal &val)
	{
		int nIndex = FindKey(key);
		if(nIndex == -1)
			return FALSE;
		SetAtIndex(nIndex, key, val);
		return TRUE;
	}
	TVal Lookup(TKey &key) const
	{
		int nIndex = FindKey(key);
		if(nIndex == -1)
			return TVal();    // must be able to convert
		return GetValueAt(nIndex);
	}
	TKey  ReverseLookup(TVal &val) const
	{
		int nIndex = FindVal(val);
		if(nIndex == -1)
			return TKey();    // must be able to convert
		return GetKeyAt(nIndex);
	}
	const TKey& GetKeyAt(int nIndex) const
	{
		ATLASSERT(nIndex >= 0 && nIndex < GetSize());
		return m_keys[nIndex];
	}
	const TVal& GetValueAt(int nIndex) const
	{
		ATLASSERT(nIndex >= 0 && nIndex < GetSize());
		return m_vals[nIndex];
	}
	TKey& GetKeyAt(int nIndex)
	{
		ATLASSERT(nIndex >= 0 && nIndex < GetSize());
		return m_keys[nIndex];
	}
	TVal& GetValueAt(int nIndex)
	{
		ATLASSERT(nIndex >= 0 && nIndex < GetSize());
		return m_vals[nIndex];
	}

// Implementation

	void SetAtIndex(int nIndex, TKey& key, TVal& val)
	{
		ATLASSERT(nIndex >= 0 && nIndex < GetSize());
		m_keys[nIndex] = key;
		m_vals[nIndex] = val;
	}
	int FindKey(TKey& key) const
	{
		return m_keys.Find(key);
	}
	int FindVal(TVal& val) const
	{
		return m_vals.Find(val);
	}
	// calls Add for each pair in rhs, therefore it takes into account if it is
	// a multimap or not
	BOOL CopyFrom(CNo5SimpleMap<TKey,TVal> &rhs)
	{
		RemoveAll();
		return AppendFrom(rhs);
	}
	// calls Add for each pair in rhs, therefore it takes into account if it is
	// a multimap or not
	BOOL AppendFrom(CNo5SimpleMap<TKey,TVal> &rhs)
	{
		BOOL res = TRUE;

		for(int i=0;i<rhs.GetSize() && res;i++){
			TKey &key = rhs.GetKeyAt(i);
			TVal &val = rhs.GetValueAt(i);
			res = Add(key,val);
		}
		return res;
	}
};

struct CoInit
{
	HRESULT hr;

#ifdef _WIN32_DCOM
	CoInit(DWORD dwCoInit = COINIT_APARTMENTTHREADED)
	{
		hr = ::CoInitializeEx(NULL,dwCoInit);
		ATLASSERT(SUCCEEDED(hr));
	}
#else
	CoInit()
	{
		hr = ::CoInitialize(NULL);
		ATLASSERT(SUCCEEDED(hr));
	}
#endif
	~CoInit()
	{
		if(SUCCEEDED(hr)){
			::CoUninitialize();
		}
	}
};

// array owns the objects pointed by each item
template <class T>
class CPointerArray : public CSimpleValArray< T* >
{
	typedef CSimpleValArray< T* > baseClass;
public:
	CPointerArray() { }
	~CPointerArray()
	{
		RemoveAll();
	}

	BOOL Add(T *t)
	{
		return baseClass::Add(t);
	}

	BOOL Add(const T &t)
	{
		T *p = new T(t);
		return Add(p);
	}

	BOOL Remove(T *t)
	{
		BOOL res = baseClass::Remove(t);

		if(res)
			delete t;
		return res;
	}

	BOOL RemoveAt(int nIndex)
	{
		T *pT = operator[](nIndex);
		BOOL res = FALSE;
		
		if(pT){
			res = baseClass::RemoveAt(nIndex);
			if(res)
				delete pT;
		}
		return res;
	}

	void RemoveAll()
	{
		BOOL res;

		while(GetSize()){
			res = RemoveAt(0);
			ATLASSERT(res);
		}
		baseClass::RemoveAll();
	}
};

} //	NO5TL

#endif // !defined(AFX_NO5TLBASE_H__374E3C01_509A_11D9_A17B_CD5DC1F1C73C__INCLUDED_)
