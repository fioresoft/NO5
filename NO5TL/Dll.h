// Dll.h: interface for the Dll class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLL_H__A2C6000E_8F5B_11D7_A17A_91DAE8359932__INCLUDED_)
#define AFX_DLL_H__A2C6000E_8F5B_11D7_A17A_91DAE8359932__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLMISC_H__
#error dll.h requires atlmisc.h 
#endif

namespace NO5TL
{

	/** wrapper for the HMODULE handle */
	class CModule  
	{
		HMODULE m_hModule;
	public:
		/**
			This will only work if the the file ( module ) has been
			mapped into the adress space of the calling process.
			That is different from LoadLibrary
		*/
		CModule(LPCTSTR name = NULL)
		{
			m_hModule = ::GetModuleHandle(name);
			ATLASSERT(m_hModule != NULL);
		}
		explicit CModule(HMODULE hModule):m_hModule(hModule)
		{
			//
		}
		virtual ~CModule()
		{
			//
		}
		/** returns full path and file name */
		CString GetFileName(void) const
		{
			CString res;
			TCHAR *p = res.GetBuffer(MAX_PATH + 1);
			
			BOOL b = ::GetModuleFileName(m_hModule,p,
				MAX_PATH + 1);
			ATLASSERT(b);
			res.ReleaseBuffer();
			return res;
		}
		/** If it fails it returns NULL */
		FARPROC GetProcAddress(LPCSTR name) const
		{
			ATLASSERT(m_hModule != NULL);
			return ::GetProcAddress(m_hModule,name);
		}
		/** Gets the procedure by its ordinal value */
		FARPROC GetProcAddress(UINT uIndex) const
		{
			ATLASSERT(m_hModule != NULL);
			USES_CONVERSION;
			LPCSTR p = T2CA(MAKEINTRESOURCE(uIndex));
			return ::GetProcAddress(m_hModule,p);
		}
		// handle 
		operator HMODULE(void) const
		{
			return m_hModule;
		}
		void SetHandle(HMODULE hModule)
		{
			m_hModule = hModule;
		}
		HMODULE GetHandle(void) const
		{
			return m_hModule;
		}
	};

	class CLibrary:public CModule  
	{
	public:
		CLibrary(HINSTANCE hInst = NULL):\
			CModule((HMODULE)hInst)
		{
			//
		}
		virtual ~CLibrary()
		{
			BOOL res = FreeLibrary();
			ATLASSERT(res);
		}
		/** Returns FALSE if fails */
		BOOL LoadLibrary(LPCTSTR name)
		{
			BOOL bRes = FALSE;
			HMODULE hRes = ::LoadLibrary(name);

			if(hRes){
				SetHandle(hRes);
				bRes = TRUE;
			}
			return bRes;
		}
		BOOL FreeLibrary(void)
		{
			BOOL res = TRUE;
			if(GetHandle()){
				res = ::FreeLibrary(GetHandle());
				if(res)
					SetHandle(NULL);
			}
			return res;
		}
	};

	class CDll:public CLibrary 
	{
	public:
		explicit CDll(HINSTANCE hInst = NULL):CLibrary(hInst)
		{
			//
		}
		/** returns FALSE if the DLL does not export the function
			"DllGetVersion
		*/
		BOOL GetVersionInfo(DLLVERSIONINFO &vi) const
		{
			DLLGETVERSIONPROC pfn;
			BOOL bRes = FALSE;
			HRESULT hr;

			pfn = (DLLGETVERSIONPROC)GetProcAddress("DllGetVersion");
			if(pfn){
				bRes = TRUE;
				hr = (*pfn)(&vi);
				if(FAILED(hr)){
					bRes = FALSE;
				}
			}
			return bRes;
		}
	};
}

#endif // !defined(AFX_DLL_H__A2C6000E_8F5B_11D7_A17A_91DAE8359932__INCLUDED_)
