#if !defined(AFX_ATLDATAOBJ_H__20040706_4F49_248D_7C5E_0080AD509054__INCLUDED_)
#define AFX_ATLDATAOBJ_H__20040706_4F49_248D_7C5E_0080AD509054__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// IDataObject and OLE clipboard wrappers
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2004 Bjarke Viksoe.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//


#ifndef __cplusplus
  #error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLCOM_H__
  #error atldispa.h requires atlcom.h to be included first
#endif


/////////////////////////////////////////////////////////////////////////
// Misc clipboard functions

#ifdef __ATLCONV_H__

inline BOOL AtlSetClipboardText(HWND hWnd, LPCTSTR pstrText)
{
   if( !::OpenClipboard(hWnd) ) return FALSE;
   int cchLen = lstrlen(pstrText) + 1;
   HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (SIZE_T) cchLen);
   if( hGlobal == NULL ) return FALSE;
   USES_CONVERSION;
   memcpy(GlobalLock(hGlobal), T2CA(pstrText), (size_t) cchLen);
   GlobalUnlock(hGlobal);
   ::EmptyClipboard();
   BOOL bRes = ::SetClipboardData(CF_TEXT, hGlobal) != NULL;
   if( !bRes ) GlobalFree(hGlobal);
   ::CloseClipboard();
   return bRes;
}

#endif // __ATLCONV_H__


/////////////////////////////////////////////////////////////////////////
// Standard Clipboard

#if 0

class CClipboard
{
public:
   BOOL m_bOpened;

   CClipboard() : m_bOpened(FALSE)
   {
   }
   CClipboard(HWND hWnd) : m_bOpened(FALSE)
   {
      Open(hWnd);
   }
   ~CClipboard()
   {
      Close();
   }
   BOOL Open(HWND hWnd)
   {
      ATLASSERT(!m_bOpened);
      ATLASSERT(hWnd==NULL || ::IsWindow(hWnd));  // See Q92530
      BOOL bRes = ::OpenClipboard(hWnd);
      m_bOpened = bRes;
      return bRes;
   }
   void Close()
   {
      if( !m_bOpened ) return;
      ::CloseClipboard();
      m_bOpened = FALSE;
   }
   BOOL Empty()
   {
      ATLASSERT(m_bOpened);
      return ::EmptyClipboard();
   }
   int GetFormatCount() const
   {
      return ::CountClipboardFormats();
   }
   UINT EnumFormats(UINT uFormat)
   {
      ATLASSERT(m_bOpened);
      return ::EnumClipboardFormats(uFormat);
   }
   void EnumFormats(CSimpleValArray<UINT>& aFormats)
   {
      ATLASSERT(m_bOpened);
      UINT uFormat = 0;
      while( true ) {
         uFormat = ::EnumClipboardFormats(uFormat);
         if( uFormat == 0 ) break;
         aFormats.Add(uFormat);
      }
   }
   BOOL IsFormatAvailable(UINT uFormat) const
   {
      return ::IsClipboardFormatAvailable(uFormat);
   }
   int GetFormatName(UINT uFormat, LPTSTR pstrName, int cchMax) const
   {
      // NOTE: Doesn't return names of predefined cf!
      return ::GetClipboardFormatName(uFormat, pstrName, cchMax);
   }
   HANDLE GetData(UINT uFormat) const
   {
      ATLASSERT(m_bOpened);
      return ::GetClipboardData(uFormat);
   }
#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
   BOOL GetTextData(_CSTRING_NS::CString& sText) const
   {
      ATLASSERT(m_bOpened);
      // Look for UNICODE version first because there's a better
      // chance to convert to the correct locale.
      HGLOBAL hMem = ::GetClipboardData(CF_UNICODETEXT);
      if( hMem != NULL ) {
         sText = (LPCWSTR) GlobalLock(hMem);
         return GlobalUnlock(hMem);
      }
      hMem = ::GetClipboardData(CF_TEXT);
      if( hMem != NULL ) {
         sText = (LPCSTR) GlobalLock(hMem);
         return GlobalUnlock(hMem);
      }
      return FALSE;
   }
   BOOL GetFormatName(UINT uFormat, _CSTRING_NS::CString& sName) const
   {
      for( int nSize = 256; ; nSize *= 2 ) {
         int nLen = ::GetClipboardFormatName(uFormat, sName.GetBufferSetLength(nSize), nSize);
         sName.ReleaseBuffer();
         if( nLen < nSize - 1 ) return TRUE;
      }
      return TRUE;  // Hmm, unreachable!
   }
#endif
   HANDLE SetData(UINT uFormat, HANDLE hMem)
   {
      ATLASSERT(m_bOpened);
      ATLASSERT(hMem!=NULL);   // No support for WM_RENDERFORMAT here! 
                               // Enjoy the ASSERT for NULL!
      return ::SetClipboardData(uFormat, hMem);
   }
   HANDLE SetData(UINT uFormat, LPCVOID pData, int iSize)
   {
      HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, (SIZE_T) iSize);
      if( hGlobal == NULL ) return NULL;
      memcpy(GlobalLock(hGlobal), pData, (size_t) iSize);
      GlobalUnlock(hGlobal);
      HANDLE hHandle = ::SetClipboardData(uFormat, hGlobal);
      if( hHandle == NULL ) GlobalFree(hGlobal);
      return hHandle;
   }
   BOOL SetTextData(LPCSTR pstrText)
   {
      return SetData(CF_TEXT, pstrText, lstrlenA(pstrText) + 1) != NULL;
   }
   BOOL SetUnicodeTextData(LPCWSTR pstrText)
   {
      return SetData(CF_UNICODETEXT, pstrText, (lstrlenW(pstrText) + 1) * sizeof(WCHAR)) != NULL;
   }
   static HWND GetOwner()
   {
      return ::GetClipboardOwner();
   }
#if (WINVER >= 0x0500)
   static DWORD GetSequenceNumber()
   {
      return ::GetClipboardSequenceNumber();
   }
#endif
   static UINT RegisterFormat(LPCTSTR pstrFormat)
   {
      return ::RegisterClipboardFormat(pstrFormat);
   }
};

#endif // 0

/////////////////////////////////////////////////////////////////////////
// CRawDropSource

class CRawDropSource : public IDropSource
{
public:
   // IUnknown

   STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
   {
      if( riid == __uuidof(IDropSource) || riid == IID_IUnknown ) {
         *ppvObject = this;
         return S_OK;
      }
      return E_NOINTERFACE;
   }
   ULONG STDMETHODCALLTYPE AddRef()
   {
      return 1;
   }
   ULONG STDMETHODCALLTYPE Release()
   {
      return 1;
   }
   
   // IDropSource
   
   STDMETHOD(QueryContinueDrag)(BOOL bEsc, DWORD dwKeyState)
   {
      if( bEsc ) return ResultFromScode(DRAGDROP_S_CANCEL);
      if( (dwKeyState & MK_LBUTTON) == 0 ) return ResultFromScode(DRAGDROP_S_DROP);
      return S_OK;
   }
   STDMETHOD(GiveFeedback)(DWORD)
   {
      return ResultFromScode(DRAGDROP_S_USEDEFAULTCURSORS);
   }
};


#ifdef __ATLCOM_H__

//////////////////////////////////////////////////////////////////////////////
// CEnumFORMATETC

class ATL_NO_VTABLE CEnumFORMATETC : 
   public CComObjectRootEx<CComSingleThreadModel>,
   public IEnumFORMATETC
{
public:
   CSimpleValArray<FORMATETC> m_aFmtEtc;
   ULONG m_iCur;

   BEGIN_COM_MAP(CEnumFORMATETC)
      COM_INTERFACE_ENTRY(IEnumFORMATETC)
   END_COM_MAP()

public:
   CEnumFORMATETC() : m_iCur(0)
   {
   }

   void Add(FORMATETC& fmtc)
   {
      m_aFmtEtc.Add(fmtc);
   }

   // IEnumFORMATETC

   STDMETHOD(Next)(ULONG celt, LPFORMATETC lpFormatEtc, ULONG* pceltFetched)
   {
      if( pceltFetched != NULL) *pceltFetched = 0;
      if( lpFormatEtc == NULL ) return E_INVALIDARG;
      if( celt <= 0 || m_iCur >= (ULONG) m_aFmtEtc.GetSize() ) return S_FALSE;
      if( pceltFetched == NULL && celt != 1 ) return S_FALSE;
      ULONG nCount = 0;
      while( m_iCur < (ULONG) m_aFmtEtc.GetSize() && celt > 0 ) {
         *lpFormatEtc++ = m_aFmtEtc[m_iCur++];
         --celt;
         ++nCount;
      }
      if( pceltFetched != NULL ) *pceltFetched = nCount;
      return celt == 0 ? S_OK : S_FALSE;
   }
   STDMETHOD(Skip)(ULONG celt)
   {
      if( m_iCur + celt >= (ULONG) m_aFmtEtc.GetSize() ) return S_FALSE;
      m_iCur += celt;
      return S_OK;
   }
   STDMETHOD(Reset)(void)
   {
      m_iCur = 0;
      return S_OK;
   }
   STDMETHOD(Clone)(IEnumFORMATETC**)
   {
      ATLTRACENOTIMPL(_T("CEnumFORMATETC::Clone"));
   }
};


//////////////////////////////////////////////////////////////////////////////
// CSimpleDataObj

template< class T >
class ATL_NO_VTABLE ISimpleDataObjImpl : public IDataObject
{
public:
   typedef struct tagDATAOBJ
   {
      FORMATETC FmtEtc;
      STGMEDIUM StgMed;
      BOOL bRelease;
   } DATAOBJ;
   CSimpleArray<DATAOBJ> m_aObjects;

   ~ISimpleDataObjImpl()
   {
      for( int i = 0; i < m_aObjects.GetSize(); i++ ) ::ReleaseStgMedium(&m_aObjects[i].StgMed);
   }

   STDMETHOD(GetData)(FORMATETC* pformatetcIn, STGMEDIUM* pmedium)
   {
      ATLTRACE2(atlTraceControls,2,_T("ISimpleDataObjImpl::GetData\n"));
      T* pT = (T*) this;
      return pT->IDataObject_GetData(pformatetcIn, pmedium);
   }
   STDMETHOD(GetDataHere)(FORMATETC* /*pformatetc*/, STGMEDIUM* /*pmedium*/)
   {
      ATLTRACENOTIMPL(_T("ISimpleDataObjImpl::GetDataHere"));
   }
   STDMETHOD(QueryGetData)(FORMATETC* pformatetc)
   {
      ATLASSERT(pformatetc);
      int iIndex = _FindFormat(pformatetc);
      if( iIndex < 0 ) return DV_E_FORMATETC;
      // BUG: Only supports one media pr format!
      if( m_aObjects[iIndex].FmtEtc.lindex != -1 ) return DV_E_LINDEX;
      if( m_aObjects[iIndex].FmtEtc.tymed != pformatetc->tymed ) return DV_E_TYMED;
      return S_OK;
   }
   STDMETHOD(GetCanonicalFormatEtc)(FORMATETC* pformatectIn, FORMATETC* /* pformatetcOut */)
   {
      pformatectIn->ptd = NULL;
      ATLTRACENOTIMPL(_T("ISimpleDataObjImpl::GetCanonicalFormatEtc"));
   }
   STDMETHOD(SetData)(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease)
   {
      ATLASSERT(pformatetc);
      ATLASSERT(pmedium);
      if( pformatetc == NULL ) return E_POINTER;
      if( pmedium == NULL ) return E_POINTER;
      DATAOBJ obj;
      obj.FmtEtc = *pformatetc;
      obj.StgMed = *pmedium;
      obj.bRelease = fRelease;
      if( !fRelease ) _CopyStgMedium(&obj.StgMed, pmedium, pformatetc);
      int iIndex = _FindFormat(pformatetc);
      if( iIndex < 0 ) m_aObjects.Add(obj); else m_aObjects.SetAtIndex(iIndex, obj);
      return S_OK;
   }
   STDMETHOD(EnumFormatEtc)(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc)
   {
      ATLASSERT(ppenumFormatEtc);
      if( ppenumFormatEtc == NULL ) return E_POINTER;
      *ppenumFormatEtc = NULL;
      if( dwDirection != DATADIR_GET ) return E_NOTIMPL;
      CComObject<CEnumFORMATETC>* pEnum = NULL;
      if( FAILED( CComObject<CEnumFORMATETC>::CreateInstance(&pEnum) ) ) return E_FAIL;
      for( int i = 0; i < m_aObjects.GetSize(); i++ ) pEnum->Add(m_aObjects[i].FmtEtc);
      return pEnum->QueryInterface(ppenumFormatEtc);
   }
   STDMETHOD(DAdvise)(FORMATETC* /*pformatetc*/, DWORD /*advf*/, IAdviseSink* /*pAdvSink*/, DWORD* /*pdwConnection*/)
   {
      ATLTRACENOTIMPL(_T("ISimpleDataObjImpl::DAdvise"));
   }
   STDMETHOD(DUnadvise)(DWORD /*dwConnection*/)
   {
      ATLTRACENOTIMPL(_T("ISimpleDataObjImpl::DUnadvise"));
   }
   STDMETHOD(EnumDAdvise)(IEnumSTATDATA** /*ppenumAdvise*/)
   {
      ATLTRACENOTIMPL(_T("ISimpleDataObjImpl::EnumDAdvise"));
   }

   // Helper functions

   int _FindFormat(FORMATETC* pformatetc) const
   {
      for( int i = 0; i < m_aObjects.GetSize(); i++ ) {
         if( m_aObjects[i].FmtEtc.cfFormat == pformatetc->cfFormat ) return i;
      }
      return -1;
   }
   HRESULT _CopyStgMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc)
   {
      switch( pMedSrc->tymed ) {
      case TYMED_GDI:
      case TYMED_FILE:
      case TYMED_ENHMF:
      case TYMED_MFPICT:
      case TYMED_HGLOBAL:
         pMedDest->hGlobal = (HGLOBAL) ::OleDuplicateData(pMedSrc->hGlobal, pFmtSrc->cfFormat, NULL);
         break;
      case TYMED_ISTREAM:
         {
            pMedDest->pstm = NULL;
            if( FAILED( ::CreateStreamOnHGlobal(NULL, TRUE, &pMedDest->pstm) ) ) return E_OUTOFMEMORY;
            ULARGE_INTEGER alot = { 0, INT_MAX };
            if( FAILED( pMedSrc->pstm->CopyTo(pMedDest->pstm, alot, NULL, NULL) ) ) return E_FAIL;
         }
         break;
      case TYMED_ISTORAGE:
         {
            pMedDest->pstg = NULL;
            if( FAILED( ::StgCreateDocfile(NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DELETEONRELEASE | STGM_CREATE, 0, &pMedDest->pstg) ) ) return E_OUTOFMEMORY;
            if( FAILED( pMedSrc->pstg->CopyTo(0, NULL, NULL, pMedDest->pstg) ) ) return E_FAIL;
         }
         break;
      default:
         ATLASSERT(false);
         return DV_E_TYMED;
      }
      pMedDest->tymed = pMedSrc->tymed;
      pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;
      return S_OK;
   }
};

class ATL_NO_VTABLE CSimpleDataObj : 
   public CComObjectRootEx<CComSingleThreadModel>,
   public ISimpleDataObjImpl<CSimpleDataObj>
{
public:
   BEGIN_COM_MAP(CSimpleDataObj)
      COM_INTERFACE_ENTRY(IDataObject)
   END_COM_MAP()

   HRESULT IDataObject_GetData(FORMATETC* pformatetc, STGMEDIUM* pmedium)
   {
      ATLASSERT(pmedium);
      ATLASSERT(pformatetc);
      int iIndex = _FindFormat(pformatetc);
      if( iIndex < 0 ) return DV_E_FORMATETC;
      if( pformatetc->lindex != -1 ) return DV_E_LINDEX;
      if( (m_aObjects[iIndex].FmtEtc.tymed & pformatetc->tymed) == 0 ) return DV_E_LINDEX;
      return _CopyStgMedium(pmedium, &m_aObjects[iIndex].StgMed, &m_aObjects[iIndex].FmtEtc);
   }

   // Operations

   HRESULT SetGlobalData(CLIPFORMAT cf, LPCVOID pData, DWORD dwSize)
   {
      FORMATETC fmtetc = { cf, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
      STGMEDIUM stgmed = { TYMED_HGLOBAL, { 0 }, 0 };
      stgmed.hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, (SIZE_T) dwSize);
      if( stgmed.hGlobal == NULL ) return E_OUTOFMEMORY;
      memcpy(GlobalLock(stgmed.hGlobal), pData, (size_t) dwSize);
      GlobalUnlock(stgmed.hGlobal);
      return SetData(&fmtetc, &stgmed, TRUE);
   }
   HRESULT SetTextData(LPCSTR pstrData)
   {
      return SetGlobalData(CF_TEXT, pstrData, ::lstrlenA(pstrData) + 1);
   }
   HRESULT SetUnicodeTextData(LPCWSTR pstrData)
   {
      return SetGlobalData(CF_UNICODETEXT, pstrData, (::lstrlenW(pstrData) + 1) * sizeof(WCHAR));
   }
   HRESULT SetHDropData(LPCSTR pstrFilename)
   {
      FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
      STGMEDIUM stgmed = { TYMED_HGLOBAL, { 0 }, 0 };
      DWORD dwSize = sizeof(DROPFILES) + ::lstrlenA(pstrFilename) + 2;
      stgmed.hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
      if( stgmed.hGlobal == NULL ) return E_OUTOFMEMORY;
      DROPFILES* pDest = (DROPFILES*) GlobalLock(stgmed.hGlobal);
      pDest->pFiles = sizeof(DROPFILES);
      pDest->fWide = FALSE;
      ::lstrcpyA( ((LPSTR) pDest) + sizeof(DROPFILES), pstrFilename );
      GlobalUnlock(stgmed.hGlobal);
      return SetData(&fmtetc, &stgmed, TRUE);
   }
};

#endif // __ATLCOM_H__



#endif // !defined(AFX_ATLDATAOBJ_H__20040706_4F49_248D_7C5E_0080AD509054__INCLUDED_)

