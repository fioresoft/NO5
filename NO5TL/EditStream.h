// EditStream.h: interface for the CEditStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITSTREAM_H__21621BE1_3E94_11D9_A17B_C6D6ABB76442__INCLUDED_)
#define AFX_EDITSTREAM_H__21621BE1_3E94_11D9_A17B_C6D6ABB76442__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLCTRLS_H__
#error editstream.h requires atlctrls.h
#endif

// #include <sstream>		// basic_stingbuf
// #include <ostream>		// basic_ostream

namespace NO5TL
{
using std::basic_stringbuf;
using std::basic_ostream;
using std::char_traits;

template <class E,class T = char_traits<E> >
class basic_editbuf : public basic_stringbuf<E,T>
{
	CEdit &m_edit;
public:
	basic_editbuf(CEdit &edit):m_edit(edit)
	{
		//
	}
protected:
	virtual int sync()
	{
		// write to the external destination the elements
		// in [pbase,pptr[
		if(pbase() < pptr()){
			//T::assign(*pptr(),E());

			m_edit.AppendText(\
				str().c_str(),
				FALSE,	// no scroll
				TRUE);	// can undo
			// adjust the put pointers to the beginning
			setp(pbase() /*m_buf */,pbase(),epptr());
		}
		return 0;
	}
};

template <class E,class T = char_traits<E> >
class basic_oeditstream : public basic_ostream<E,T>
{
	typedef basic_ostream<E,T> _BaseClass;
	typedef basic_editbuf<E,T> _BufClass;
public:
	basic_oeditstream(CEdit &edit):_BaseClass( new _BufClass(edit) )
	{
		//
	}
	virtual ~basic_oeditstream()
	{
		delete rdbuf();
	}
};
typedef basic_oeditstream<char> oeditstream;

template <class E,class T = char_traits<E> >
class basic_richeditbuf : public basic_stringbuf<E,T>
{
	typedef basic_stringbuf<E,T> _BaseClass;
	typedef basic_richeditbuf<E,T> _ThisClass;
	CRichEditCtrl &m_edit;
public:
	basic_richeditbuf(CRichEditCtrl &edit):m_edit(edit)
	{
		//
	}
protected:
	virtual int sync()
	{
		// write to the external destination the elements
		// in [pbase,pptr[
		if(pbase() < pptr()){
			//T::assign(*pptr(),E());
			EDITSTREAM es = {0};

			es.dwCookie = (DWORD)this;
			es.pfnCallback = StreamInCallback;
			m_edit.StreamIn(SF_TEXT|SFF_SELECTION,es);
			// adjust the put pointers to the beginning
			//setp(pbase() /*m_buf */,pbase(),epptr());
			str(std::string());
		}
		return 0;
	}
private:
	static DWORD CALLBACK StreamInCallback(DWORD dwCookie,LPBYTE \
		pBuf,LONG cb,LONG *pcb)
	{
		DWORD res = -1;
		_ThisClass *pbuf = (_ThisClass *)dwCookie;

		if(pbuf){
			// devemos copiar para o buf tudo o que estiver
			// entre pbase e pptr, mas nao podemos copiar mais
			// do que cb
			*pcb = (LONG)pbuf->xsgetn((E *)pBuf,(streamsize)cb);
			return *pcb ? 0 : -1;
		}
		return res;
	}

};

template <class E,class T = char_traits<E> >
class basic_oricheditstream : public basic_ostream<E,T>
{
	typedef basic_oricheditstream<E,T> _thisClass;
	typedef basic_ostream<E,T> _BaseClass;
	typedef basic_richeditbuf<E,T> _BufClass;
public:
	basic_oricheditstream(CRichEditCtrl &edit):_BaseClass( new _BufClass(edit) )
	{
		//
	}
	virtual ~basic_oricheditstream()
	{
		delete rdbuf();
	}
};

inline std::ostream & endl2( std::ostream &os)
{
	os << "\r\n";
	os.flush();
	return os;
}

typedef basic_oricheditstream<char> oricheditstream;


} // NO5TL

#endif // !defined(AFX_EDITSTREAM_H__21621BE1_3E94_11D9_A17B_C6D6ABB76442__INCLUDED_)
