// CVector<T,t_n>.h: interface for the CVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR_H__154D1A64_0EF2_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_VECTOR_H__154D1A64_0EF2_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <memory.h>
#include <math.h>

namespace NO5TL
{

template <class T,unsigned short t_n>
class CVector  
{
	CVector<T,t_n> & copy(const CVector<T,t_n> &v)
	{
		memcpy(m_v,v.m_v, t_n * sizeof(T));
		return *this;
	}
public:
	union
	{
		struct 
		{
			T x,y;
		};
		T m_v[t_n];
	};
public:
	CVector()
	{
		memset(m_v,0,t_n * sizeof(T));
	}
	CVector(const CVector<T,t_n> &v)
	{
		copy(v);
	}
	CVector<T,t_n> & operator = (const CVector<T,t_n> &rhs)
	{
		if(this != &rhs)
			copy(rhs);
		return *this;
	}
	bool operator == (const CVector<T,t_n> &v ) const
	{
		bool res = true;

		for(int i=0;i<t_n;i++)
			if(m_v[i] != v.m_v[i])
				res = false;
		return res;

	}
	bool operator != (const CVector<T,t_n> &v) const
	{
		return !( (*this) == v );
	}
	// mult por escalar
	CVector<T,t_n> & operator *= ( T t )
	{
		for(int i=0;i<t_n;i++)
			m_v[i] *= t;
		return *this;
	}
	friend CVector<T,t_n> operator * ( const CVector<T,t_n> &v,T t )
	{
		CVector<T,t_n> r;

		for(int i=0;i<t_n;i++)
			r.m_v[i] = v.m_v[i] * t;
		return r;
	}
	friend CVector<T,t_n> operator * ( T t,const CVector<T,t_n> &v)
	{
		CVector<T,t_n> r;

		for(int i=0;i<t_n;i++)
			r.m_v[i] = v.m_v[i] * t;
		return r;
	}
	// soma 
	CVector<T,t_n> & operator += (const CVector<T,t_n> &rhs )
	{
		for(int i=0;i<t_n;i++)
			m_v[i] += rhs.m_v[i];
		return *this;
	}

	friend CVector<T,t_n> operator + ( const CVector<T,t_n> &lhs, const CVector<T,t_n> &rhs)
	{
		CVector<T,t_n> r;
		for(int i=0;i<t_n;i++)
			r.m_v[i] = lhs.m_v[i] + rhs.m_v[i];
		return r;
	}
	// sub 
	CVector<T,t_n> & operator -= (const CVector<T,t_n> &rhs)
	{
		for(int i=0;i<t_n;i++)
			m_v[i] -= rhs.m_v[i];
		return *this;
	}
	friend CVector<T,t_n> operator - ( const CVector<T,t_n> &lhs, const CVector<T,t_n> &rhs)
	{
		CVector<T,t_n> r;
		for(int i=0;i<t_n;i++)
			r.m_v[i] = lhs.m_v[i] - rhs.m_v[i];
		return r;
	}
	// prod escalar
	friend T operator * (const CVector<T,t_n> &lhs,const CVector<T,t_n> &rhs )
	{
		T res = T(0);

		for(int i=0;i<t_n;i++)
			res += lhs.m_v[i] * rhs.m_v[i];
		return res;
	}
	// modulo
	T modulo ( void ) const
	{
		return T(sqrt((double)((*this)*(*this))));
	}
	// projection of this in the direction of u
	CVector<T,t_n> proj(const CVector<T,t_n> &u ) const
	{
		return ( ( (*this) * u ) / ( u * u ) ) * u;
	}
};

} // namespace NO5TL

#endif // !defined(AFX_VECTOR_H__154D1A64_0EF2_11DA_A17D_000103DD18CD__INCLUDED_)
