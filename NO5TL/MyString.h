// MyString.h: interface for the CMyString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYSTRING_H__4035FCE9_8A6C_11D7_A17A_C6D4728BE631__INCLUDED_)
#define AFX_MYSTRING_H__4035FCE9_8A6C_11D7_A17A_C6D4728BE631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLMISC_H__
#error mystring.h requires atlmisc.h 
#endif

namespace NO5TL
{

// copy n characters from 'from' to 'to' starting and including 'start'
inline void StringCopyN(CString &to,LPCTSTR from,int start,int n)
{
	CString _from(from);
	if(n >= 0)
		to = _from.Mid(start,n);
	else
		to = _from.Mid(start);
}

// find in (str[start],str(-1)( one of the characters in dels
// return the index in relation to the whole string
inline int StringFindOneOfEx(LPCTSTR str,LPCTSTR dels,int start)
{
	CString tmp;
	int res;

	StringCopyN(tmp,str,start,-1);
	res = tmp.FindOneOf(dels);
	if(res >= 0)
		res += start;
	return res;
}

inline CString & StringAppend(CString &to,LPCTSTR from,int start,int count)
{
	CString copy;

	StringCopyN(copy,from,start,count);
	to += copy;
	return to;
}

// TODO : muito ineficiente
inline int StringFindNoCase(LPCTSTR s1,LPCSTR psz,int nStart)
{
	CString _s1 = s1;
	CString _s2 = psz;

	_s1.MakeLower();
	_s2.MakeLower();
	
	ATLASSERT(_s1.GetLength() == lstrlen(s1));
	return _s1.Find(_s2,nStart);
}


class CStringToken
{
	CString m_str;
	CString m_dels;
	int m_start;
	int m_end;
	bool m_IncludeLast;
public:
	CStringToken(void)
	{
		m_start = m_end = 0;
		m_IncludeLast = true;
	}
	void Init(LPCTSTR str,LPCTSTR dels,bool IncludeLast = true)
	{
		m_str = str;
		m_dels = dels;
		m_IncludeLast = IncludeLast;
		// resets
		m_start = m_end = 0;
	}
	void SetDelimiters(LPCTSTR dels)
	{
		m_dels = dels;
	}

	CString GetNext(void)
	{
		CString res;
		
		while(m_end >= 0 && m_end < m_str.GetLength()){
			m_end = StringFindOneOfEx(m_str,m_dels,m_start);
			if(m_end >= 0){
				if(m_end > m_start){
					StringCopyN(res,m_str,m_start,m_end - m_start);
					m_start = ++m_end;
					break;
				}
				else{
					m_start = ++m_end;
				}
			}
		}
		if(res.IsEmpty() && m_IncludeLast){
			m_end = m_str.GetLength();
			if(m_end > m_start){
				StringCopyN(res,m_str,m_start,-1);
				m_start = ++m_end;
			}
		}
		return res;
	}
	int GetAll(CSimpleArray<CString> &v)
	{
		CString res;

		while(true){
			res = GetNext();
			if(res.IsEmpty())
				break;
			v.Add(res);
		}
		return v.GetSize();
	}

	// the whole delimiter string is a delimiter and only the whole of it
	CString GetNext2(void)
	{
		CString res;
		
		while(m_end >= 0 && m_end <= m_str.GetLength()){
			// WTL::CString::Find includes m_start in the search
			// even when it is not zero ( unlike MFC )
			m_end = m_str.Find(m_dels,m_start);
			if(m_end >= 0){
				if(m_end > m_start){
					StringCopyN(res,m_str,m_start,m_end - m_start);
					m_end += m_dels.GetLength();
					m_start = m_end;
					break;
				}
				else{
					// there was a delimiter at the beginning or
					// a delimiter following another one
					m_end += m_dels.GetLength();
					m_start = m_end;
				}
			}
		}
		if(res.IsEmpty() && m_IncludeLast){
			m_end = m_str.GetLength();
			if(m_end > m_start){
				StringCopyN(res,m_str,m_start,-1);
				m_start = ++m_end;
			}
		}
		return res;
	}

	int GetAll2(CSimpleArray<CString> &v)
	{
		CString res;

		while(true){
			res = GetNext2();
			if(res.IsEmpty())
				break;
			v.Add(res);
		}
		return v.GetSize();
	}
	// whole string is a delimiter ( like GetNext2 )
	// empty string between delimiters are returned too
	// if the string starts with a del, the first res is empty
	// returns true if it got something ( even if empty string )
	// returns false when it scanned all the string
	bool GetNext3(CString &res)
	{
		bool got = false;

		if(m_start < m_str.GetLength()){
			// WTL::CString::Find includes m_start in the search
			// even when it is not zero ( unlike MFC )
			m_end = m_str.Find(m_dels,m_start);
			if(m_end >= 0){
				if(m_end > m_start){
					StringCopyN(res,m_str,m_start,m_end - m_start);
				}
				else
					res = "";
				m_end += m_dels.GetLength();
				m_start = m_end;
				got = true;
			}
			else if(m_IncludeLast){
				m_end = m_str.GetLength();
				if(m_end > m_start){
					StringCopyN(res,m_str,m_start,-1);
					m_start = ++m_end;
					got = true;
				}
			}
		}
		return got;
	}
	int GetAll3(CSimpleArray<CString> &v)
	{
		CString next;
		
		while(GetNext3(next))
			v.Add(next);

		return v.GetSize();
	}
};

inline CString RemoveTags(LPCTSTR text,TCHAR chIni,TCHAR chEnd)
{
	const TCHAR *p = text;
	const TCHAR *pIni = p;
	CString res;
	bool bInside = false;
	TCHAR *buf = new TCHAR[lstrlen(text) + 1];

	buf[0] = '\0';
	while(*p){
		if(*p == chIni && !bInside){
			if(p - pIni > 0){
				// copy text
				lstrcpyn(buf,pIni,(p - pIni + 1));
				bInside = true;
				pIni = p++;
				res += buf;
			}
			else{
				p++;
				bInside = true;
			}
		}
		else if(*p == chEnd && bInside){
			pIni = ++p;
			bInside = false;
		}
		else
			p++;
	}
	if(p - pIni > 0){
		lstrcpyn(buf,pIni,(p - pIni + 1));
		res += buf;
	}
	delete []buf;
	return res;
}

typedef CString ( * REPLACEFUN)(LPCTSTR tag);

inline CString ReplaceTags(LPCTSTR text,TCHAR chIni,TCHAR chEnd,
					REPLACEFUN pfn)
{
	const TCHAR *p = text;
	const TCHAR *pIni = p;
	CString res;
	bool bInside = false;
	TCHAR *buf = new TCHAR[lstrlen(text) + 1];
	CString rep; // replacement

	buf[0] = '\0';
	while(*p){
		if(*p == chIni && !bInside){
			if(p - pIni > 0){
				// copy text
				lstrcpyn(buf,pIni,(p - pIni + 1));
				bInside = true;
				pIni = p++; // pIni esta em '<'
				res += buf;
			}
			else{
				p++;
				bInside = true;
			}
		}
		else if(*p == chEnd && bInside){
			p++; // esta logo depois de '>'
			lstrcpyn(buf,pIni,(p - pIni + 1));
			rep = (*pfn)(buf);
			if(!rep.IsEmpty())
				res += rep;
			pIni = p;
			bInside = false;
		}
		else
			p++;
	}
	if(p - pIni > 0){
		lstrcpyn(buf,pIni,(p - pIni + 1));
		res += buf;
	}
	delete []buf;
	return res;
}

// each beginning tag letter must have a corresponde ending letter
// exemple pszBegin = "<\#" pszEnd = ">#"
// fText and fTag are functions or functors accepting an LPCTSTR
template <class CEnumText,class CEnumTags>
void EnumTags(LPCTSTR text,LPCTSTR pszBegin,LPCTSTR pszEnd,
			   CEnumText &fText,CEnumTags &fTag)
{
	LPCTSTR p,q;
	CString tmp;
	int i;
	const int count = lstrlen(pszBegin);

	ATLASSERT(count == lstrlen(pszEnd));

	p = q = text;

	while(*q){
		for(i=0;i<count;i++){
			if(pszBegin[i] == *q){
				if( q > p ){
					// enumerate the text [p,q[
					StringCopyN(tmp,p,0,q - p);
					fText(tmp);
				}
				p = q++;
				while(*q && *q != pszEnd[i])
					q++;
				if(_T('\0') == *q)
					break;	// a tag without closing
				StringCopyN(tmp,p,0,q - p + 1);
				// enumerate tag [p,q]
				if(!fTag(tmp))
					fText(tmp);

				p = ++q;	// one past the end of the tag
				break;
			}
		}
		if(i == count)
			q++;
	}
	if( q > p ){
		// enum text [p,q[
		fText(p);
	}
}

// inner tag takes priority
template <class fText,class fTags>
void EnumTags2(LPCTSTR text,TCHAR chBegin,TCHAR chEnd,
			   fText &f1,fTags &f2)
{
	LPCTSTR p,q;
	bool bInside = false;
	CString tmp;

	p = q = text;

	while(*q){
		if(*q == chBegin){
			bInside = true;
			// enum text between [p,q[ even if this is a fake
			// begin tag - no problem
			if(q > p){
				StringCopyN(tmp,p,0,q - p);
				f1(tmp);
			}
			p = q;	// begining of the tag
		}
		else if(*q == chEnd && bInside){
			bInside = false;
			// enum the tag [p,q]
			StringCopyN(tmp,p,0,q - p + 1);
			if(!f2(tmp))
				f1(tmp);
			p = q + 1;	// after end tag
		}
		q++;
	}
	// enum the rest of the text
	if(q > p){
		f1(p);
	}
}

/*
// inner tag takes priority
template <class fText,class fTags>
void EnumTags3(LPCTSTR text,LPCTSTR pszBegin,LPCTSTR pszEnd,
			   fText &f1,fTags &f2)
{
	LPCTSTR p,q;
	bool bInside = false;
	CString tmp;
	int i;
	const int count = lstrlen(pszBegin);
	int index = 0;

	p = q = text;
	ATLASSERT(lstrlen(pszBegin) == lstrlen(pszEnd));

	if(!lstrcmpi(text,">:d<")){
		int apague = 0;
	}

	while(*q){

		if(bInside && *q == pszEnd[index]){
			bInside = false;
			index = -1;
			// enum the tag [p,q]
			StringCopyN(tmp,p,0,q - p + 1);
			if(!f2(tmp))
				f1(tmp);
			p = ++q;
			continue;
		}

		for(i = 0;i<count;i++){
			if(*q == pszBegin[i]){
				index = i;
				bInside = true;
				// enum text between [p,q[ even if this is a fake
				// begin tag - no problem
				if(q > p){
					StringCopyN(tmp,p,0,q - p);
					f1(tmp);
				}
				p = q++;	// p at begining of the tag
				break;
			}
		}
		if(i == count)
			q++;
	}
	// enum the rest of the text
	if(q > p){
		f1(p);
	}
}
*/

template <class fText,class fTags>
void EnumTags3(LPCTSTR text,LPCTSTR pszBegin,LPCTSTR pszEnd,
			   fText &f1,fTags &f2)
{
	LPCTSTR p,q;
	bool bInside = false;
	CString tmp;
	int i;
	const int count = lstrlen(pszBegin);
	int index = 0;
	LPCTSTR pBeginTag = NULL;

	p = q = text;
	ATLASSERT(lstrlen(pszBegin) == lstrlen(pszEnd));

	if(!lstrcmpi(text,">:d<")){
		int apague = 0;
	}

	while(*q){

		if(bInside && *q == pszEnd[index]){
			bInside = false;
			index = -1;
			// enum [p,pBeginTag[
			if(pBeginTag > p){
				StringCopyN(tmp,p,0,pBeginTag - p);
				f1(tmp);
			}
			// enum the tag [pBeginTag,q]
			StringCopyN(tmp,pBeginTag,0,q - pBeginTag + 1);
			if(!f2(tmp)){
				f1(tmp);
			}
			p = ++q;
			pBeginTag = NULL;
			continue;
		}

		for(i = 0;i<count;i++){
			if(*q == pszBegin[i]){
				index = i;
				bInside = true;
				pBeginTag = q;
				break;
			}
		}
		q++;
	}
	// enum the rest of the text
	if(q > p){
		f1(p);
	}
}

// if s is not involved in delimiters, a copy of it is returned
inline CString RemoveDelimiters(LPCSTR s, char chStart = '\"',char chEnd = '\"')
{
	LPCSTR p,q;
	CString res;

	p = s;
	if(*p == chStart){
		q = ++p;
		while(*q && *q != chEnd)
			q++;
		if(*q == chEnd && q > p){
			// now we want [p,q[
			StringCopyN(res,p,0,(int)(q - p));
		}
		else{
			res = s;
		}
	}
	else{
		res = s;
	}
	return res;
}

// // if s is not involved in quotes, a copy of it is returned
inline CString RemoveQuotes(LPCSTR s)
{
	return RemoveDelimiters(s);
}


// parses a string like "key=value"
// returns TRUE if both, key and value are not empty
inline BOOL ParseKeyValue(LPCTSTR s,CString &key,CString &value)
{
	CStringToken st;
	BOOL res = FALSE;

	st.Init(s,"=",true);
	key = st.GetNext();
	if(!key.IsEmpty()){
		value = st.GetNext();
		if(!value.IsEmpty())
			res = TRUE;
	}
	return res;
}

inline CString MakeRandomString(int count)
{
	CString res;
	LPTSTR buf = res.GetBuffer(count);
	int i;
	int letters = 'z' - 'a' + 1;

	for(i=0;i<count;i++){
		buf[i] = 'a' + rand() % letters;
	}
	res.ReleaseBuffer();
	return res;
}

inline CString GetErrorDesc(DWORD error)
{
	LPTSTR buf;
	int res;
	CString msg;

	res = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|\
		FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		error,
		0,				// LANGID
		(LPTSTR)&buf,
		0,				// min buf
		NULL);			// args

	if(res){
		msg = buf;
		::LocalFree((HLOCAL)buf);
	}
	return msg;
}

inline CString ToUTF8(LPCTSTR s)
{
	int res;
	LPWSTR buf = NULL;
	LPSTR buf2;
	CString out;

	// first we pass a a NULL output buffer ( length zero )
	// to get back the required length size
	res = ::MultiByteToWideChar(\
				CP_ACP,					// code page
				0,						// no flags
				s,						// in
				-1,						// in length
				NULL,					// out
				0);						// out len

	if(!res){
		// some error occurred
		return CString(s);
	}

	// the length returned is the number of wide chars required
	buf = new WCHAR[res + 1];
		
	res = ::MultiByteToWideChar(\
		CP_ACP,						// code page
		0,							// flags
		s,							// in
		-1,							// in len
		buf,						// out 
		res + 1);					// out len

	if(!res){
		// some error
		delete []buf;
		return CString(s);
	}

	// again we want the length of our buffer

	res = ::WideCharToMultiByte(\
		CP_UTF8,				// code page
		0,						// flags
		buf,					// in
		-1,						// in len
		NULL,					// out
		0,						// out len
		NULL,NULL);				// 
	
	if(!res){
		delete []buf;
		return CString(s);
	}
	
	buf2 = out.GetBuffer(res + 1);
	
	res = ::WideCharToMultiByte(\
		CP_UTF8,				// code page. We want ANSI code page
		0,						// flags
		buf,					// in ( wide char string )
		-1,						// in len
		buf2,					// out ( utf8 string )
		res + 1,				// out buf len
		NULL,NULL);

	out.ReleaseBuffer();

	if(!res){
		delete []buf;
		return CString(s);
	}

	delete []buf;
	return out;
}

inline CString FromUTF8(LPCSTR s)
{
	int res;
	LPWSTR buf = NULL;
	LPSTR buf2;
	CString out;

	// first we pass a a NULL output buffer ( length zero )
	// to get back the required length size
	res = ::MultiByteToWideChar(\
				CP_UTF8,				// code page
				MB_ERR_INVALID_CHARS,	// no flags
				s,						// in
				-1,						// in length
				NULL,					// out
				0);						// out len

	if(!res){
		// some error occurred
		return CString(s);
	}

	// the length returned is the number of wide chars required
	buf = new WCHAR[res + 1];
		
	res = ::MultiByteToWideChar(\
		CP_UTF8,					// code page
		MB_ERR_INVALID_CHARS,							// flags
		s,							// in
		-1,							// in len
		buf,						// out 
		res + 1);					// out len

	if(!res){
		// some error
		delete []buf;
		return CString(s);
	}

	// again we want the length of our buffer

	res = ::WideCharToMultiByte(\
		CP_ACP,					// code page
		0,						// flags
		buf,					// in
		-1,						// in len
		NULL,					// out
		0,						// out len
		NULL,NULL);				// 
	
	if(!res){
		delete []buf;
		return CString(s);
	}
	
	buf2 = out.GetBuffer(res + 1);
	
	res = ::WideCharToMultiByte(\
		CP_ACP,					// code page. We want ANSI code page
		0,						// flags
		buf,					// in ( wide char string )
		-1,						// in len
		buf2,					// out ( utf8 string )
		res + 1,				// out buf len
		NULL,NULL);

	out.ReleaseBuffer();

	if(!res){
		delete []buf;
		return CString(s);
	}

	delete []buf;
	return out;
}

// delete []res or NULL
inline LPWSTR UTF8toUNICODE(LPCTSTR p)
{
	int res;
	LPWSTR buf = NULL;

	// first we pass a a NULL output buffer ( length zero )
	// to get back the required length size
	res = ::MultiByteToWideChar(\
				CP_UTF8,				// code page
				MB_ERR_INVALID_CHARS,	// no flags
				p,						// in
				-1,						// in length
				NULL,					// out
				0);						// out len

	if(!res){
		// some error occurred
		return NULL;
	}

	// the length returned is the number of wide chars required
	buf = new WCHAR[res + 1];
		
	res = ::MultiByteToWideChar(\
		CP_UTF8,					// code page
		MB_ERR_INVALID_CHARS,							// flags
		p,							// in
		-1,							// in len
		buf,						// out 
		res + 1);					// out len

	if(!res){
		// some error
		delete []buf;
		return NULL;
	}
	return buf;
}

// delete the result. if NULL call GetLastError
inline WCHAR * UTF8toUNICODE(LPCSTR p,int len)
{
	// first compute the required length for the wide char string
	int count = ::MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,p,
		len,NULL,0);

	if(count == 0){
		return NULL;	// error, call get last error
	}
	LPWSTR q = new WCHAR[count + 1];
	ZeroMemory(q,sizeof(WCHAR) * (count + 1));

	// translate
	count = ::MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,p,
		len,q,count);

	if(len == 0){
		delete []q;
		return NULL;	// error, call get last error
	}
	return q;
}


inline CString & PathAddBackSlash(CString &s)
{
	if(!s.IsEmpty()){
		if(s.GetAt(s.GetLength() - 1) != '\\')
			s += '\\';
	}
	return s;
}

class CStringBuffer
{
	LPTSTR m_p;
	CString &m_s;
public:
	CStringBuffer(CString &s):m_s(s)
	{
		m_p = s.GetBuffer(0);
	}
	CStringBuffer(CString &s,int len):m_s(s)
	{
		m_p = s.GetBuffer(len);
	}
	~CStringBuffer()
	{
		m_s.ReleaseBuffer();
	}
	operator LPTSTR () 
	{
		return m_p;
	}
};

// this doesnt set window text
class  CWindowTextString : public CString
{
public:
	CWindowTextString(HWND hWnd)
	{
		const int len = GetWindowTextLength(hWnd) + 1;
		CStringBuffer buf(*this,len);
		GetWindowText(hWnd,buf,len);
	}
	const CString & operator = (LPCTSTR s)
	{
		return CString::operator = ( s );
	}
};

class  CWindowText : public CString
{
	HWND m_hWnd; // dont use CWindow to not have to include atlwin
public:
	CWindowText(HWND hWnd):m_hWnd(hWnd)
	{
		const int len = GetWindowTextLength(hWnd) + 1;
		CStringBuffer buf(*this,len);
		GetWindowText(hWnd,buf,len);
	}
	~CWindowText()
	{
		SetWindowText(m_hWnd,this -> operator LPCTSTR () );
	}
};


class CModuleFileString : public CString
{
public:
	CModuleFileString(HMODULE hModule = _Module.GetModuleInstance())
	{
		CStringBuffer buf(*this,MAX_PATH + 1);
		::GetModuleFileName(hModule,buf,MAX_PATH);
	}
};

class CModulePathString : public CModuleFileString
{
public:
	CModulePathString(HMODULE hModule = _Module.GetModuleInstance()):\
		CModuleFileString(hModule)
	{
		int pos = ReverseFind(_T('\\'));
		ATLASSERT(pos >= 0);
		if(pos >= 0)
			Delete(pos + 1,GetLength() - pos - 1);
	}
};


class CCurDirString : public CString
{
public:
	CCurDirString()
	{
		{
			CStringBuffer buf(*this,MAX_PATH + 1);
			GetCurrentDirectory(MAX_PATH,buf);
		}
		PathAddBackSlash(*this);
	}
};

class CCurDirSaver : public CCurDirString
{
	CCurDirString s;
public:
	CCurDirSaver()
	{
		//
	}
	~CCurDirSaver()
	{
		SetCurrentDirectory(s);
	}
};

// make a comma list from a string list
inline CString MakeCommaList(const CSimpleArray<CString> &lst)
{
	CString res;
	int i;

	for(i=0;i<lst.GetSize();i++){
		res += lst[i];
		if(i != lst.GetSize() - 1)
			res += ',';
	}
	return res;
}

// make a string of comma separated numbers from a list
inline CString MakeCommaList(const CSimpleValArray<int> &lst)
{
	CString res;
	int i;

	for(i=0;i<lst.GetSize();i++){
		res.Append(lst[i]);
		if(i != lst.GetSize() - 1)
			res += ',';
	}
	return res;
}

inline BOOL MakeIntList(LPCTSTR in,CSimpleValArray<int> &lst)
{
	CStringToken st;
	CString next;
	BOOL res = TRUE;

	st.Init(in,",",true);
	next = st.GetNext();

	while(!next.IsEmpty() && res){
		res = lst.Add(atoi(next));
		next = st.GetNext();
	}
	return res;
}


// make a string list from a comma list
inline int MakeStringList(LPCTSTR in,CSimpleArray<CString> &out)
{
	CStringToken st;
	st.Init(in,",",true);
	return st.GetAll(out);
}

// find a string in a string array, canse insensitive.
// return the index or -1
inline int StringArray_FindNoCase(const CSimpleArray<CString> &arr,LPCTSTR s)
{
	int i;

	for(i=0;i<arr.GetSize();i++){
		if(!arr[i].CompareNoCase(s))
			break;
	}
	if(i == arr.GetSize())
		i = -1;
	return i;
}

class CStringArray : public CSimpleArray<CString>
{
	typedef CSimpleArray<CString> _BaseClass;
public:
	int FindNoCase(LPCTSTR s)
	{
		return StringArray_FindNoCase(*this,s);
	}
	BOOL RemoveNoCase(LPCTSTR s)
	{
		int index = FindNoCase(s);
		return RemoveAt(index);
	}
	BOOL CopyFrom(const CStringArray &rhs)
	{
		BOOL res = TRUE;

		RemoveAll();
		return AppendFrom(rhs);
	}
	BOOL AppendFrom(const CStringArray &rhs)
	{
		BOOL res = TRUE;

		for(int i=0;i<rhs.GetSize() && res;i++)
			res = Add(rhs[i]);
		return res;
	}
	CString MakeCommaList(void)
	{
		return NO5TL::MakeCommaList(*this);
	}
	BOOL AppendFromCommaList(LPCTSTR s)
	{
		CStringArray out;

		NO5TL::MakeStringList(s,out);
		return AppendFrom(out);
	}
};

// array of unique strings
template <bool bNoCase>
class CStringSetT : public CStringArray
{
public:
	// returns true if string was already there or not
	BOOL Add(CString & t,BOOL *pAdded = NULL)
	{
		BOOL res = TRUE;
		int index = Find(t);

		if(index < 0){
			res = CStringArray::Add(t);
			if(res && pAdded){
				if(index < 0)
					*pAdded = FALSE;
				else 
					*pAdded = TRUE;
			}
		}
		else if(pAdded)
			*pAdded = FALSE;

		return res;
	}
	const CString & operator[] (int nIndex) const
	{
		return CStringArray::operator [] (nIndex);
	}
	int Find(LPCTSTR s)
	{
		int index = -1;

		if(bNoCase)
			index = FindNoCase(s);
		else
			index = CStringArray::Find(CString(s));

		return index;
	}
	BOOL Remove(LPCTSTR t)
	{
		int nIndex = Find(t);
		if(nIndex == -1)
			return FALSE;
		return RemoveAt(nIndex);
	}
};

// stack of unique strings - not case senstivie, with a max number of items
class CUniqueStringStack : public NO5TL::CStringSetT<true>
{
	const UINT m_max;
public:
	CUniqueStringStack(UINT uMax = 20):m_max(uMax)
	{
		ATLASSERT(m_max > 0);
	}

	BOOL Push(LPCTSTR s)
	{
		CString ss = s;
		int count = GetSize();
			
		if(static_cast<UINT>(count) == m_max && (Find(s) >= 0)){
			RemoveAt(0);
		}
		return Add(ss);
	}
	CString Top(void)
	{
		const int count = GetSize();
		int pos = count - 1;
		CString res;


		if(pos >= 0)
			res = operator[] (pos);
		return res;
	}
	CString Pop(void)
	{
		const int count = GetSize();
		int pos = count - 1;
		CString res;


		if(pos >= 0){
			res = operator[] (pos);
			RemoveAt(pos);
		}
		return res;
	}
};

} // NO5TL




#endif // !defined(AFX_MYSTRING_H__4035FCE9_8A6C_11D7_A17A_C6D4728BE631__INCLUDED_)
