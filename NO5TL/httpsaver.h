// httpsaver.h: interface for the httpsaver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPSAVER_H__161FF583_6C35_11D9_A17B_82F209524E2D__INCLUDED_)
#define AFX_HTTPSAVER_H__161FF583_6C35_11D9_A17B_82F209524E2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mystring.h"		// string token
#include "httpclient.h"

namespace NO5TL
{


template <class TSocket>
class CHttpClientParser : public CHttpClient<TSocket>
{
	typedef CHttpClient<TSocket> _BaseClass;
protected:
	CSimpleArray<CString> m_header;
	LPBYTE m_pDataBegin;
	LPBYTE m_pDataEnd;
	//
	CHttpClientParser(IHttpClientEvents *pSink,DWORD dwBufSize = 1024):\
		_BaseClass(pSink,dwBufSize,true)
	{
		m_pDataBegin = NULL;
		m_pDataEnd = NULL;
	}
public:
	CSimpleArray<CString> & GetRespHeader(void)
	{
		return m_header;
	}
protected:
	virtual void OnSockConnecting()
	{
		m_pDataBegin = NULL;
		m_pDataEnd = NULL;
		_BaseClass::OnSockConnecting();
	}
	// parse the header and gets a pointer to the data
	virtual void OnSockClose(int error)
	{
		CDataBuffer *pBuf = GetRespBuf();
		CStringToken st;
		CString next;
		LPSTR q = strstr(pBuf->GetData(),"\r\n\r\n");

		if(q){
			*q = '\0';
			st.Init(pBuf->GetData(),"\r\n");
			m_header.RemoveAll();
			st.GetAll2(m_header);
			*q = '\r';	// restore
			m_pDataBegin = (LPBYTE) ( q + lstrlen("\r\n\r\n") );
			m_pDataEnd = (LPBYTE)pBuf->GetData() + pBuf->GetDataLen();
		}
		_BaseClass::OnSockClose(error);
	}
};

struct IHttpFileSaverEvents : public IHttpClientEvents
{
	virtual void OnFileSaved(BOOL res) = 0;
};

template <class TSocket>
class CHttpFileSaver : public CHttpClientParser<TSocket>
{
	typedef CHttpClientParser<TSocket> _BaseClass;
	IHttpFileSaverEvents *m_pSink;
	CString m_FileOut;
public:
	CHttpFileSaver(IHttpFileSaverEvents *pSink,DWORD dwBufSize = 1024):\
		_BaseClass((IHttpClientEvents *)pSink,dwBufSize),
		m_pSink(pSink)
	{
		//
	}
	void SetFileOut(LPCSTR file)
	{
		m_FileOut = file;
	}
private:
	virtual void OnSockClose(int error)
	{
		BOOL res = TRUE;

		_BaseClass::OnSockClose(error);

		if(NULL != m_pDataBegin && NULL != m_pDataEnd)
		{
			HANDLE hFile = ::CreateFile(m_FileOut,GENERIC_WRITE,0,
				NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

			if(hFile != INVALID_HANDLE_VALUE)
			{
				DWORD dwWritten = 0;
				res = ::WriteFile(hFile,m_pDataBegin,
					(int)(m_pDataEnd - m_pDataBegin),&dwWritten,NULL);
				ATLASSERT(dwWritten == (DWORD)(m_pDataEnd - m_pDataBegin));
				::CloseHandle(hFile);
				if(res)
				{
					res = (dwWritten == (DWORD)(m_pDataEnd - m_pDataBegin));
				}
			}
		}
		m_pSink->OnFileSaved(res);
	}
};

}	// NO5TL

#endif // !defined(AFX_HTTPSAVER_H__161FF583_6C35_11D9_A17B_82F209524E2D__INCLUDED_)
