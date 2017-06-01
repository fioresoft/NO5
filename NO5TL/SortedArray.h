// SortedArray.h: interface for the CSortedArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SORTEDARRAY_H__65A43762_4434_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_SORTEDARRAY_H__65A43762_4434_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "no5tlbase.h"
#include <algorithm>
#include <functional>

namespace NO5TL
{

template <class T>
inline void SimpleArraySort(CSimpleArray<T> &arr)
{
	std::sort<T*>(&arr.m_aT[0],&arr.m_aT[arr.GetSize()]);
}

template <class T,class Pred>
inline void SimpleArraySort(CSimpleArray<T> &arr,Pred pr)
{
	std::sort(&arr.m_aT[0],&arr.m_aT[arr.GetSize()],pr);
}

template <class T,class Pred = std::less<T> >
class CSortedArray : public CSimpleArray<T>
{
	typedef CSimpleArray< T > baseClass;
public:
	BOOL Add(T t,Pred pred)
	{
		BOOL res = baseClass::Add(t);

		if(res)
			Sort(pred);
		return res;
	}
	void Sort(Pred pred)
	{
		SimpleArraySort(*this,pred);
	}
	
};

template <class T,class Pred = std::less<T> >
class CSimpleValSortedArray : public CSimpleValArray<T>
{
	typedef CSimpleValArray< T > baseClass;
public:
	BOOL Add(T t,Pred pred)
	{
		BOOL res = baseClass::Add(t);

		if(res)
			Sort(pred);
		return res;
	}
	void Sort(Pred pred)
	{
		SimpleArraySort(*this,pred);
	}
};

template<class _Ty>
struct pointer_less : std::binary_function<_Ty, _Ty, bool> 
{
	bool operator()(const _Ty * _X, const _Ty * _Y) const
	{
		return ((*_X) < (*_Y)); 
	}
};

template <class T,class Pred = pointer_less<T> >
class CPointerSortedArray : public CPointerArray<T>
{
	typedef CPointerArray< T > baseClass;
public:
	BOOL Add(T *pt,Pred pred)
	{
		BOOL res = baseClass::Add(pt);

		if(res)
			Sort(pred);
		return res;
	}
	BOOL Add(T t,Pred pred)
	{
		BOOL res = baseClass::Add(t);

		if(res)
			Sort(pred);
		return res;
	}
	void Sort(Pred pred)
	{
		SimpleArraySort(*this,pred);
	}
	int Find(T *pt)
	{
		return baseClass::Find(pt);
	}
	template <class EqPred>
	int Find(T *pt,EqPred pred)
	{
		int res = -1;
		const int count = GetSize();

		for(int i=0;i<count;i++){
			T *pNext = operator[](i);
			if(pred(pt,pNext)){
				res = i;
				break;
			}
		}
		return res;
	}
};


} // namespace NO5TL


#endif // !defined(AFX_SORTEDARRAY_H__65A43762_4434_11DA_A17D_000103DD18CD__INCLUDED_)
