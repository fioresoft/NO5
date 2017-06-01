// WinFile.h: interface for the CWinFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINFILE_H__943F7A41_9DD2_11D7_A17A_9E4CE971EE36__INCLUDED_)
#define AFX_WINFILE_H__943F7A41_9DD2_11D7_A17A_9E4CE971EE36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLMISC_H__
	#error winfile.h requires atlmisc.h
#endif //__ATLMISC_H__

#ifndef NO5TL_WINFILE_H_
#define NO5TL_WINFILE_H_

#include "databuffer.h"
#include "handle.h"
#include "no5tl\mystring.h"

namespace NO5TL
{

struct FileHandleTraits
{
	typedef HANDLE handle_type;

	static BOOL Destroy(HANDLE h)
	{
		return ::CloseHandle(h);
	}
	static HANDLE InvalidHandle(void)
	{
		return INVALID_HANDLE_VALUE;
	}
};

template <bool bOwner>
class CWinFileT : public CHandleT<FileHandleTraits,bOwner>
{
	typedef CHandleT<FileHandleTraits,bOwner> _BaseClass;
	typedef CWinFileT<bOwner> _ThisClass;
public:
	CWinFileT(HANDLE hFile = InvalidHandle()):\
		_BaseClass(hFile)
	{
		//
	}
	CWinFileT(_ThisClass &rhs):_BaseClass((_BaseClass &)rhs)
	{
		//
	}

	_ThisClass & operator = (_ThisClass &rhs)
	{
		return Assign(rhs);
	}
	_ThisClass & operator = ( handle_type h )
	{
		return Assign(h);
	}
	
	BOOL Create(LPCTSTR name,DWORD dwCreateFlags,
		DWORD dwAccess = GENERIC_READ|GENERIC_WRITE,
		DWORD dwShareMode = 0,
		DWORD dwAttribute = FILE_ATTRIBUTE_NORMAL,
		DWORD dwFlags = 0,
		HANDLE hTemplate = NULL,
		LPSECURITY_ATTRIBUTES psa = NULL)
	{		
		HANDLE h = ::CreateFile(name,dwAccess,dwShareMode,
			psa,dwCreateFlags,dwAttribute|dwFlags,hTemplate);
		BOOL res = FALSE;

		if(h != InvalidHandle())
			res = AttachHandle(h);
		return res;
	}
	
	DWORD GetSize(LPDWORD pFileSizeHigh = NULL)
	{
		DWORD res = (DWORD)-1;

		ATLASSERT(IsHandleValid());
		if(IsHandleValid()){
			res = ::GetFileSize(GetHandle(),pFileSizeHigh);
			if(res == (DWORD)-1){
				if(pFileSizeHigh == NULL){
					ATLASSERT(0);
				}
				else{
					DWORD LastError = ::GetLastError();

					if(LastError != NO_ERROR){
						ATLASSERT(0);
					}
				}
			}
		}
		return res;
	}
	BOOL Read(LPVOID pBuf,DWORD dwToRead,DWORD *pdwRead)
	{
		BOOL res = FALSE;

		ATLASSERT(IsHandleValid());

		if(IsHandleValid()){
			res = ::ReadFile(GetHandle(),pBuf,dwToRead,pdwRead,NULL);
		}
		return res;
	}

	BOOL Write(LPVOID pBuf,DWORD dwWrite,DWORD *pdwWritten)
	{
		BOOL res = FALSE;

		ATLASSERT(IsHandleValid());

		if(IsHandleValid()){
			res = ::WriteFile(GetHandle(),pBuf,dwWrite,pdwWritten,NULL);
		}
		return res;
	}

	BOOL Write(CDataBuffer &db)
	{
		BOOL res = FALSE;
		DWORD written = 0;

		ATLASSERT(IsHandleValid());

		if(IsHandleValid()){
			res = Write(db.GetData(),db.GetDataLen(),&written);
			if(res){
				res = (int)written == db.GetDataLen() ? TRUE : FALSE;
			}
		}
		return res;
	}

	BOOL WriteString(LPCTSTR p,DWORD *pdwWritten = NULL)
	{
		DWORD dummy = 0;
		BOOL res;

		res = Write((LPVOID)(const_cast<LPTSTR>(p)),lstrlen(p),&dummy);

		if(res && pdwWritten){
			*pdwWritten = dummy;
		}
		return res;
	}

	BOOL ReadAll(CDataBuffer &db,int ChunkSize = 512)
	{
		BOOL res = FALSE;

		ATLASSERT(IsHandleValid());
		if(IsHandleValid()){
			char *buf = new char[ChunkSize];
			ATLASSERT(buf);
			if(buf){
				DWORD dwRead = 0;
				DWORD dwTotal = 0;
				DWORD dwSize = GetSize();
				
				do{
					::ZeroMemory(buf,ChunkSize);
					res = Read(buf,sizeof(buf),&dwRead);
					ATLASSERT(res);
					dwTotal += dwRead;
					db.Add(buf,dwRead);
				}while(dwRead == sizeof(buf) && res);

				if(res){
					ATLASSERT(dwTotal == dwSize);
					res = dwTotal == dwSize;
				}
				delete []buf;
			}
		}
		return res;
	}
	BOOL ReadLines(CSimpleArray<CString> &lines)
	{
		CDataBuffer db;
		BOOL res = ReadAll(db);

		if(res){
			NO5TL::CStringToken st;
			db.AddNull();
			st.Init(db.GetData(),"\r\n");
			st.GetAll(lines);
		}
		return res;
	}
};

typedef CWinFileT<true> CWinFile;
typedef CWinFileT<false> CWinFileHandle;

#endif // NO5TL_WINFILE_H_

} // namespace NO5TL

#endif // !defined(AFX_WINFILE_H__943F7A41_9DD2_11D7_A17A_9E4CE971EE36__INCLUDED_)
