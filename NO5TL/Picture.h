// Picture.h: interface for the CPicture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICTURE_H__6DEF7443_618E_11D9_A17B_B6C6500CF12C__INCLUDED_)
#define AFX_PICTURE_H__6DEF7443_618E_11D9_A17B_B6C6500CF12C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLWIN_H__
	#error picture.h requires atlwin.h to be included first
#endif

#ifndef __ATLGDI_H__
	#error picture.h requires atlgdi.h to be included first
#endif

namespace NO5TL
{

inline HRESULT GetStreamSize(LPSTREAM pStm,DWORD &dwSize)
{
	HRESULT hr = E_POINTER;
	STATSTG stat = {0};
	DWORD flags = STATFLAG_NONAME;

	if(pStm){
		hr = pStm->Stat(&stat,flags);
		if(SUCCEEDED(hr)){
			dwSize = stat.cbSize.LowPart;
		}
	}
	return hr;
}

// reads the contents of a stream into a buffer that is internally
// allocated and must be released with delete
inline HRESULT ReadStream(LPSTREAM pStm,LPBYTE *ppByte,DWORD &dwSize)
{
	HRESULT hr = E_POINTER;

	if(pStm && ppByte){
		hr = GetStreamSize(pStm,dwSize);
		if(SUCCEEDED(hr) && dwSize){
			LPBYTE buf = new BYTE[dwSize];
			DWORD dwRead;

			ZeroMemory(buf,dwSize);
			hr = pStm->Read(buf,dwSize,&dwRead);
			if(SUCCEEDED(hr)){
				ATLASSERT(dwSize == dwRead);
				*ppByte = buf;
			}
		}
	}
	return hr;
}

inline HRESULT SeekStreamToBegin(LPSTREAM pStm)
{
	LARGE_INTEGER pos = {0};
	HRESULT hr = E_POINTER;

	if(pStm){
		hr = pStm->Seek(pos,STREAM_SEEK_SET,NULL);
	}
	return hr;
}

class CPicture  
{
public:
	CComPtr<IPicture> m_p;
public:
	CPicture(){};
	virtual ~CPicture(){};

	// size in himetric
	HRESULT GetImageSizeHimetric(SIZE &hmSize)
	{
		HRESULT hr = E_POINTER;
		OLE_YSIZE_HIMETRIC hmHeight;
		OLE_XSIZE_HIMETRIC hmWidth;

		if(m_p){
			hr = m_p->get_Width(&hmWidth);
			hr = m_p->get_Height(&hmHeight);
			if(SUCCEEDED(hr)){
				hmSize.cx = hmWidth;
				hmSize.cy = hmHeight;
			}
		}
		return hr;
	}
	HRESULT GetImageSizePixels(HDC hdc,SIZE &sz)
	{
		SIZE szhm = {0};
		HRESULT hr = GetImageSizeHimetric(szhm);

		if(SUCCEEDED(hr)){
			CDCHandle dc(hdc);

			sz = szhm;
			dc.HIMETRICtoDP(&sz);
		}
		return hr;
	}

	HRESULT GetImageSizeLogical(HDC hdc,SIZE &sz)
	{
		SIZE szhm = {0};
		HRESULT hr = GetImageSizeHimetric(szhm);

		if(SUCCEEDED(hr)){
			CDCHandle dc(hdc);

			sz = szhm;
			dc.HIMETRICtoLP(&sz);
		}
		return hr;
	}


	HRESULT CreateFromHBITMAP(HBITMAP hBmp,HPALETTE hPal,BOOL fOwn)
	{
		HRESULT hr = E_POINTER;

		if(hBmp){
			PICTDESC pd = { sizeof(pd) };
			pd.picType = PICTYPE_BITMAP;
			pd.bmp.hbitmap = hBmp;
			pd.bmp.hpal = hPal;

			m_p = NULL;

			hr = ::OleCreatePictureIndirect(&pd,IID_IPicture,fOwn,
				(LPVOID *)&m_p);
		}
		return hr;
	}
	// para urls so parece funcionar se a imagem ja esta no nosso
	// cache 
	HRESULT LoadFromPathOrUrl(LPCTSTR path)
	{
		HRESULT hr = E_POINTER;

		if(path){
			USES_CONVERSION;
			CComPtr<IPicture> sp;
			hr = ::OleLoadPicturePath(T2OLE(path),NULL,0,CLR_INVALID,
				IID_IPicture,(LPVOID *)&sp);
			if(SUCCEEDED(hr)){
				m_p = sp;
			}
		}
		return hr;
	}
	HRESULT CreateFromStream(LPSTREAM pStm)
	{
		HRESULT hr = E_POINTER;

		if(pStm){
			CComPtr<IPicture> sp;
			DWORD dwSize = 0;

			hr = GetStreamSize(pStm,dwSize);
			if(SUCCEEDED(hr)){
				hr = OleLoadPicture(pStm,dwSize,FALSE,
					IID_IPicture,(LPVOID *)&sp);
				if(SUCCEEDED(hr)){
					m_p = sp;
				}
			}
		}
		return hr;
	}
	// memory will be owned by the stream
	// hMem must be moveable and non-discardable
	HRESULT CreateFromMemory(HGLOBAL hMem)
	{
		HRESULT hr = E_POINTER;

		if(hMem){
			CComPtr<IStream> pStm;
			hr = CreateStreamOnHGlobal(hMem,TRUE,&pStm);
			if(SUCCEEDED(hr)){
				hr = CreateFromStream(pStm);
			}
		}
		return hr;
	}
	HRESULT CreateFromData(LPVOID pData,DWORD dwSize)
	{
		HRESULT hr = E_POINTER;

		if(pData){
			HGLOBAL hMem = ::GlobalAlloc(GHND,dwSize);
			if(hMem){
				LPVOID p = ::GlobalLock(hMem);
				ATLASSERT(p);
				::CopyMemory(p,pData,dwSize);
				::GlobalUnlock(hMem);
				hr = CreateFromMemory(hMem);
			}
		}
		return hr;
	}
	// rcDest = (l,t,r,b) in logical units ( or always in pixels? )
	HRESULT Render(HDC hdc,const RECT &rcDst = RECT())
	{
		HRESULT hr = E_POINTER;

		if(m_p){
			CDCHandle dc(hdc);
			RECT _rcDst = rcDst;
			SIZE hmSize = {0};
			
			GetImageSizeHimetric(hmSize);
			

			if(::IsRectEmpty(&_rcDst)){	// copies to 0,0,cx,cy
				SIZE sz = {0};
				
				GetImageSizeLogical(dc,sz);
				_rcDst.left = 0;
				_rcDst.top = 0;
				_rcDst.right = sz.cx;
				_rcDst.bottom = sz.cy;
			}
			hr = m_p->Render(dc,
				_rcDst.left,_rcDst.top,
				_rcDst.right - _rcDst.left,
				_rcDst.bottom - _rcDst.top,
				0,
				hmSize.cy,
				hmSize.cx,
				-hmSize.cy,
				NULL);

		}
		return hr;
	}

	// x,y in logical coordinates
	HRESULT RenderFitWindow(HDC hdc,HWND hWnd)
	{
		HRESULT hr = E_POINTER;

		if(m_p){
			RECT rcClient = {0};
			GetClientRect(hWnd,&rcClient);
			hr = Render(hdc,rcClient);
		}
		return hr;
	}
	/*
	HRESULT SaveAsFile(LPCTSTR pszFile,BOOL fSaveMemCopy)
	{
		HRESULT hr = E_POINTER;

		if(m_p){
			CComPtr<IStream> pStm;

			hr = ::CreateStreamOnHGlobal(NULL,TRUE,&pStm);
			if(SUCCEEDED(hr)){
				hr = m_p->SaveAsFile(pStm,fSaveMemCopy,NULL);
				if(SUCCEEDED(hr)){
					HANDLE hFile = ::CreateFile(pszFile,
						GENERIC_WRITE,
						0,				// share
						NULL,	
						CREATE_ALWAYS,	// overwrites any previous
						FILE_ATTRIBUTE_NORMAL,
						NULL);

					if(NULL != hFile){
						DWORD dwSize = 0;
						BOOL res;
						LPBYTE pData = NULL;
						DWORD dwWritten = 0;
						
						// move stream pointer to the beg
						hr = SeekStreamToBegin(pStm);
						hr = ReadStream(pStm,&pData,dwSize);
						
						if(SUCCEEDED(hr)){
							res = ::WriteFile(hFile,pData,dwSize,&dwWritten,
								NULL);
							delete []pData;
							ATLASSERT(res);
							if(!res)
								hr = E_FAIL;
						}
						::CloseHandle(hFile);
					}
				}
			}
		}
		return hr;
	}
	*/

	/*
	fSaveMemCopy [in] Flag indicating whether or not to save a copy
	of the picture in memory. 
	*/
	HRESULT SaveAsFile2(LPCTSTR pszFile,BOOL fSaveMemCopy = TRUE)
	{
		HRESULT hr = E_POINTER;

		if(m_p){
			CComPtr<IStream> pStm;
			LONG dwSize = 0;

			// TRUE for freeing the memory when obj released
			hr = ::CreateStreamOnHGlobal(NULL,TRUE,&pStm);
			if(SUCCEEDED(hr)){
				hr = m_p->SaveAsFile(pStm,fSaveMemCopy,&dwSize);
				if(SUCCEEDED(hr)){
					HANDLE hFile = ::CreateFile(pszFile,
						GENERIC_WRITE,
						0,				// share
						NULL,	
						CREATE_ALWAYS,	// overwrites any previous
						FILE_ATTRIBUTE_NORMAL,
						NULL);

					if(NULL != hFile){
						HGLOBAL hMem = NULL;

						// do we have to free this handle ?
						hr = ::GetHGlobalFromStream(pStm,&hMem);
						
						if(SUCCEEDED(hr)){
							LPBYTE pData = (LPBYTE)::GlobalLock(hMem);

							if(pData){
								LONG dwWritten = 0;
								BOOL res;
								res = ::WriteFile(hFile,pData,
									(DWORD)dwSize,(DWORD *)&dwWritten,
									NULL);
								::GlobalUnlock(hMem);
								ATLASSERT(res && dwSize== dwWritten);
								if(!res || (dwSize != dwWritten))
									hr = E_FAIL;
							}
						}
						::CloseHandle(hFile);
					}
				}
			}
		}
		return hr;
	}
	// call delete[]
	HRESULT GetDataAsFile(LPBYTE *ppData,DWORD *pdwSize)
	{
		HRESULT hr = E_POINTER;
		CComPtr<IStream> pStm;
		DWORD dwSize = 0;

		// TRUE to delete the HGLOBAL when stream is released
		hr = ::CreateStreamOnHGlobal(NULL,TRUE,&pStm);
		if(SUCCEEDED(hr)){
			hr = m_p->SaveAsFile(pStm,TRUE,(LONG*)&dwSize);
			if(SUCCEEDED(hr)){
				dwSize = 0;
				hr = SeekStreamToBegin(pStm);
				if(SUCCEEDED(hr)){
					hr = ReadStream(pStm,ppData,dwSize);
					if(pdwSize)
						*pdwSize = dwSize;
				}
			}
		}
		return hr;
	}
	// if IPicture owns the object, then the handle should
	// not be deleted ( almost sure )
	HBITMAP GetBitmapHandle(void)
	{
		HBITMAP hBmp = NULL;

		if(m_p != NULL){
			OLE_HANDLE h = NULL;
			HRESULT hr = m_p->get_Handle(&h);
			if(SUCCEEDED(hr)){
				hBmp = (HBITMAP)h;
			}
		}
		return hBmp;
	}
	BOOL GetBitmapObject(BITMAP &bm)
	{
		BOOL res = FALSE;
		HBITMAP hBmp = GetBitmapHandle();

		if(hBmp){
			res = ::GetObject(hBmp,sizeof(BITMAP),&bm) != 0;
		}
		return res;
	}
};



template <class T,class TBase = CWindow,class TWinTraits = CControlWinTraits>
class CPictureWindowImpl : public CWindowImpl< T >
{
public:
	CPictureWindowImpl()
	{
		m_bFit = false;
	}

	BEGIN_MSG_MAP(CPictureWindowImpl)
		MESSAGE_HANDLER(WM_PAINT,OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgnd)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);

		if(m_pict.m_p){
			HRESULT hr;

			if(m_bFit){
				hr = m_pict.RenderFitWindow(dc,m_hWnd);
			}
			else{
				hr = m_pict.Render(dc,dc.m_ps.rcPaint);
			}
			ATLASSERT(SUCCEEDED(hr));
		}
		return 0;
	}
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(!m_pict.m_p){
			RECT r;
			CDCHandle dc((HDC)wParam);
			GetClientRect(&r);
			dc.FillRect(&r,AtlGetStockBrush(BLACK_BRUSH));
			
			//bHandled = FALSE;
			return 0;
		}
		if(!m_bFit){
			CDCHandle dc((HDC)wParam);
			CRgn rgnClip;
			CRect rcClient; 
			CRect rcPict(0,0,0,0);
			CSize szPict;

			GetClientRect(&rcClient);
			m_pict.GetImageSizeLogical(dc,szPict);
			rcPict.right = szPict.cx;
			rcPict.bottom = szPict.cy;
			rgnClip.CreateRectRgn(0,0,rcClient.right,rcClient.bottom);
			dc.SelectClipRgn(rgnClip);
			dc.ExcludeClipRect(&rcPict);
			// fill rect doesnt paint in the clipped region
			dc.FillRect(&rcClient,
				(HBRUSH)GetClassLong(m_hWnd,GCL_HBRBACKGROUND));
		}
		return 1;
	}
public:	// interface
	CPicture & GetPicture(void) { return m_pict; }
	HRESULT SetPicture(LPCTSTR pszPathOrUrl)
	{
		HRESULT hr = m_pict.LoadFromPathOrUrl(pszPathOrUrl);
		if(SUCCEEDED(hr))
			InvalidateRect(NULL);
		return hr;
	}
	void FitClient(bool bFit,bool bRedraw = true) 
	{ 
		InvalidateRect(NULL);
		m_bFit = bFit; 
	}
protected:
	CPicture m_pict;
	bool m_bFit;
};

class CPictureWindow : public CPictureWindowImpl<CPictureWindow>
{
public:
	DECLARE_WND_CLASS(_T("NO5PictWndClass"))
	BEGIN_MSG_MAP(CPictureWindow)
		CHAIN_MSG_MAP(CPictureWindowImpl<CPictureWindow>)
	END_MSG_MAP()
};

template <class T>
inline T Module(T t)
{
	return t < 0 ? -t : t;
}

inline DWORD CalcImageSize(LONG width,LONG height,WORD bpp)
{
	DWORD dwLineLen = (width * bpp +31) /32 * 4;
	DWORD dwSize = Module(height) * dwLineLen;
	return dwSize;
}

inline DWORD CalcImageSize(const LPBITMAPINFOHEADER pbi)
{
	return CalcImageSize(pbi->biWidth,pbi->biHeight,pbi->biBitCount);
}

inline DWORD GetColorTableLen(const BITMAPINFOHEADER *pbi)
{
	DWORD res =  (pbi->biBitCount <= 8) ? (1 << pbi->biBitCount) : 0;
	return res * sizeof(RGBQUAD);
}

inline LPBYTE GetDibBits(BITMAPINFOHEADER *pbi)
{
	return (LPBYTE)pbi + pbi->biSize + GetColorTableLen(pbi);
}


// gets the data as file and a pointer to the bitmapinfoheader
// call must delete[] the data
inline HRESULT PictureGetInfo(CPicture &p,LPBYTE *ppData,
					   LPBITMAPINFOHEADER *ppbi)
{
	DWORD dwFileSize = 0;
	HRESULT hr = p.GetDataAsFile(ppData,&dwFileSize);
	if(SUCCEEDED(hr)){
		LPBITMAPFILEHEADER pbf = (LPBITMAPFILEHEADER)*ppData;
		LPBITMAPINFOHEADER pbi = (LPBITMAPINFOHEADER)(pbf + 1);
		*ppbi = pbi;
	}
	return hr;
}

} // NO5TL

#endif // !defined(AFX_PICTURE_H__6DEF7443_618E_11D9_A17B_B6C6500CF12C__INCLUDED_)
