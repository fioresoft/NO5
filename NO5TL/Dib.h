// Dib.h: interface for the CDib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIB_H__86E82066_B637_4CA4_ACC6_86449C32D9F2__INCLUDED_)
#define AFX_DIB_H__86E82066_B637_4CA4_ACC6_86449C32D9F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#define PALVERSION   0x300

namespace NO5TL
{
typedef HGLOBAL HDIB;

HBITMAP CaptureClientRect(HWND hWnd,CRect &r);
WORD NumColors(LPBITMAPINFOHEADER pbi);
DWORD PaletteSize(LPBITMAPINFOHEADER pbi);

class CDib  
{
	HDIB m_hDib;
	LPBITMAPINFOHEADER m_pbi;
public:
	CDib()
	{
		m_hDib = NULL;
		m_pbi = NULL;
	}
	~CDib()
	{
		DeleteObject();
	}
	void DeleteObject(void)
	{
		if(m_hDib){
			if(m_pbi){
				GlobalUnlock(m_hDib);
				m_pbi = NULL;
			}
			GlobalFree(m_hDib);
			m_hDib = NULL;
		}
	}
	BOOL IsNull(void) const
	{
		return m_hDib == NULL;
	}
	BOOL operator()(void) const
	{
		return !IsNull();
	}
	operator HDIB() const { return m_hDib; }

	BOOL Create(DWORD dwWidth,DWORD dwHeight,WORD wBitCount)
	{
		BITMAPINFOHEADER bi = {0};
		DWORD dwBytesPerLine;
		DWORD dwLen;
		HDIB hDib;
		BOOL res = FALSE;

		// Make sure bits per pixel is valid
	    if (wBitCount <= 1)
		  wBitCount = 1;
	    else if (wBitCount <= 4)
		  wBitCount = 4;
	    else if (wBitCount <= 8)
		  wBitCount = 8;
	    else if (wBitCount <= 24)
		  wBitCount = 24;
	    else
		  wBitCount = 4;  // set default value to 4 if parameter is bogus

		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = dwWidth;
		bi.biHeight = dwHeight;
		bi.biPlanes = 1;
		bi.biBitCount = wBitCount;
		bi.biCompression = BI_RGB;
		bi.biSizeImage = 0;
		
		dwBytesPerLine =  WIDTHBYTES(bi.biBitCount * dwWidth);
		dwLen = bi.biSize + PaletteSize(&bi) + dwBytesPerLine * dwHeight;
		hDib = GlobalAlloc(GHND,dwLen);
		if(hDib){
			LPBITMAPINFOHEADER pbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
			if(pbi){
				*pbi = bi;
				if(!IsNull())
					DeleteObject();
				m_pbi = pbi;
				m_hDib = hDib;
			}
			else{
				GlobalFree(hDib);
			}
		}
		return res;
	}
	DWORD Width(void) const
	{
		DWORD res = 0;

		ATLASSERT(!IsNull() && m_pbi);
		if(!IsNull() && m_pbi){
			res = m_pbi->biWidth;
		}
		return res;
	}
	DWORD Height(void) const
	{
		DWORD res = 0;

		ATLASSERT(!IsNull() && m_pbi);
		if(!IsNull() && m_pbi){
			res = m_pbi->biHeight;
		}
		return res;
	}
	BOOL FromBitmapHandle(HDC hdc,HBITMAP hBmp,HPALETTE hPal = NULL)
	{
		BITMAP bm = {0};
		BOOL res = FALSE;

		if(GetObject(hBmp,sizeof(bm),&bm)){
			HDIB hDib = NULL;
			BITMAPINFOHEADER bi = {0};
			BITMAPINFOHEADER *pbi = NULL;
			CDCHandle dc = hdc;
			DWORD dwLen;
			DWORD dwTableSize;

			bi.biSize = sizeof(BITMAPINFOHEADER);
			bi.biWidth = bm.bmWidth;
			bi.biHeight = bm.bmHeight;
			bi.biPlanes = 1;
			bi.biBitCount = bm.bmBitsPixel > 24 ? 24 : bm.bmBitsPixel;
			//bi.biBitCount = 0;
			bi.biCompression = BI_RGB;
			bi.biSizeImage = 0;
			bi.biXPelsPerMeter = 0;
			bi.biYPelsPerMeter = 0;
			bi.biClrUsed = 0;
			bi.biClrImportant = 0;

			if(!hPal)
				hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
			hPal = dc.SelectPalette(hPal,FALSE);
			dc.RealizePalette();

			dwTableSize = PaletteSize(&bi);
			dwLen = sizeof(BITMAPINFOHEADER) + dwTableSize;
			hDib = GlobalAlloc(GHND,dwLen);
			if(!hDib){
				dc.SelectPalette(hPal,FALSE);
				dc.RealizePalette();	// precisa ?
				return FALSE;
			}
			pbi = (BITMAPINFOHEADER *)GlobalLock(hDib);
			*pbi = bi;
			// the pointer is null, we just want to get biSizeImage
			// will it get the color table too ?
			if(GetDIBits(hdc,hBmp,0,bm.bmHeight,NULL,(BITMAPINFO*)pbi,DIB_RGB_COLORS)){
				HDIB hDib2;

				bi = *pbi;
				if(bi.biSizeImage == 0)
					bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * bi.biBitCount) * bm.bmHeight;
				dwLen =  bi.biSize + dwTableSize + bi.biSizeImage;
				GlobalUnlock(hDib);
				hDib2 = GlobalReAlloc(hDib,dwLen,GMEM_ZEROINIT);
				if(hDib2){
					//GlobalFree(hDib);	hDib = NULL;
					hDib = hDib2;
					hDib2 = NULL;
					pbi = (BITMAPINFOHEADER *)GlobalLock(hDib);
					if(pbi){
						*pbi = bi;
						int lines = GetDIBits(dc,hBmp,0,bm.bmHeight,((LPBYTE)pbi + bi.biSize + \
							dwTableSize),(BITMAPINFO *)pbi,DIB_RGB_COLORS);
						if(!lines){
							//GlobalFree(hDib);
							GlobalUnlock(hDib2);
							GlobalFree(hDib2);
							hDib = NULL;
						}
						else{
							if(m_hDib)
								DeleteObject();
							m_hDib = hDib;
							m_pbi = pbi;
							hDib = NULL;
							res = TRUE;
						}
					}
					else{
						//GlobalFree(hDib);
						//hDib = NULL;
						GlobalFree(hDib2);
					}
				}
			}
			dc.SelectPalette(hPal,FALSE);
			dc.RealizePalette();
		}
		return res;
	}
	HBITMAP ToBitmapHandle(HDC hdc,HPALETTE hPal = NULL)
	{
		HBITMAP hbmp = NULL;

		if(!IsNull() && m_pbi){
			LPBITMAPINFOHEADER pbi = m_pbi;
			if(pbi){
				CDCHandle dc = hdc;
				HPALETTE hPalOld = NULL;

				if(hPal)
					hPalOld = dc.SelectPalette(hPal,FALSE);
				dc.RealizePalette();

				LPVOID pData = (LPVOID)(((LPBYTE *)pbi) + pbi->biSize + PaletteSize(pbi));
				hbmp = CreateDIBitmap(hdc,pbi,CBM_INIT,pData,(BITMAPINFO *)pbi,DIB_RGB_COLORS);
				dc.SelectPalette(hPalOld,FALSE);
				dc.RealizePalette();
			}
		}
		return hbmp;
	}
	BOOL FromClientRect(HWND hWnd,CRect &rc)
	{
		CBitmap bmp;
		BOOL res = FALSE;

		bmp.Attach(CaptureClientRect(hWnd,rc));
		if(!bmp.IsNull()){
			CClientDC dc(hWnd);
			res = FromBitmapHandle(dc,bmp);
		}
		return res;
	}
	BOOL SaveToFile(LPCTSTR file)
	{
		BOOL res = FALSE;

		ATLASSERT(!IsNull() && m_pbi);
		if(IsNull() || m_pbi == NULL)
			return FALSE;

		if(m_pbi){
			BITMAPFILEHEADER bfh = {0};
			DWORD dwSize,dwTableSize;

			dwTableSize = PaletteSize(m_pbi);
			dwSize = m_pbi->biSize + dwTableSize + m_pbi->biSizeImage;

			bfh.bfType = (WORD('M' << 8 ) | WORD('B'));
			bfh.bfSize = sizeof(bfh) + dwSize;
			bfh.bfOffBits = sizeof(bfh) + m_pbi->biSize + dwTableSize;

			HANDLE hFile = CreateFile(file,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,
					NULL);
			DWORD dwWritten;

			if(hFile){
				if(WriteFile(hFile,&bfh,sizeof(bfh),&dwWritten,NULL)){
					if(WriteFile(hFile,m_pbi,m_pbi->biSize,&dwWritten,NULL)){
						if(WriteFile(hFile,(LPBYTE)m_pbi + m_pbi->biSize + dwTableSize,
							m_pbi->biSizeImage,&dwWritten,NULL)){
							res = TRUE;
						}
					}
				}
				CloseHandle(hFile);
			}
		}
		return res;
	}
	BOOL ReadFromFile(LPCTSTR file)
	{
		BOOL res;
		BITMAPFILEHEADER bfh = {0};
		BITMAPINFOHEADER bi = {0};
		HDIB hDib = NULL;
		DWORD dwRead;
		HANDLE hFile;
		DWORD dwNumColors;
		DWORD dwSizeImage;

		hFile = CreateFile(file,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
		if(!hFile)
			return FALSE;
		// read BITMAPFILEHEADER
		res = ReadFile(hFile,&bfh,sizeof(bfh),&dwRead,NULL);
		if(!res || dwRead != sizeof(bfh)){
			CloseHandle(hFile);
			return FALSE;
		}
		if(bfh.bfType != 0x4d42){ // "BM"
			CloseHandle(hFile);
			return FALSE;
		}
		// read BITMAPINFOHEADER
		res = ReadFile(hFile,&bi,sizeof(bi),&dwRead,NULL);
		if(!res || dwRead != sizeof(bi) || bi.biSize != sizeof(BITMAPINFOHEADER)){
			CloseHandle(hFile);
			return FALSE;
		}
		dwNumColors = bi.biClrUsed;
		if(dwNumColors == 0){
			if(bi.biBitCount < 16)
				dwNumColors = 1 << bi.biBitCount;
			else
				dwNumColors = 0;
		}
		bi.biClrUsed = dwNumColors;
		dwSizeImage = bi.biSizeImage;
		if(dwSizeImage == 0)
			dwSizeImage = WIDTHBYTES(bi.biBitCount * bi.biWidth) * bi.biHeight;
		bi.biSizeImage = dwSizeImage;
		hDib = GlobalAlloc(GHND,sizeof(BITMAPINFOHEADER) + dwNumColors * sizeof(RGBQUAD) + \
			bi.biSizeImage);
		if(!hDib){
			CloseHandle(hFile);
			return FALSE;
		}
		LPBITMAPINFOHEADER pbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
		if(!pbi){
			GlobalFree(hDib);
			CloseHandle(hFile);
			return FALSE;
		}
		// copy BITMAPINFOHEADER
		*pbi = bi;
		// read color table
		if(dwNumColors){
			res = ReadFile(hFile,(LPBYTE)pbi + sizeof(BITMAPINFOHEADER),dwNumColors * \
				sizeof(RGBQUAD),&dwRead,NULL);
			if(!res || dwRead != dwNumColors * sizeof(RGBQUAD)){
				GlobalUnlock(pbi);
				GlobalFree(hDib);
				CloseHandle(hFile);
				return FALSE;
			}
		}
		// seek to image data if necessary
		if(bfh.bfOffBits != 0){
			SetFilePointer(hFile,bfh.bfOffBits,NULL,FILE_BEGIN);
		}
		// read image
		res = ReadFile(hFile,(LPBYTE)pbi + sizeof(BITMAPINFOHEADER) + dwNumColors * \
			sizeof(RGBQUAD),pbi->biSizeImage,&dwRead,NULL);

		if(!res || dwRead != pbi->biSizeImage){
			GlobalUnlock(pbi);
			GlobalFree(hDib);
			CloseHandle(hFile);
			return FALSE;
		}
		if(!IsNull())
			DeleteObject();
		m_pbi = pbi;
		m_hDib = hDib;
		CloseHandle(hFile);
		return TRUE;
	}
		
	HPALETTE CreatePalette(void)
	{
		HPALETTE hPal = NULL;

		ATLASSERT(!IsNull());
		if(!IsNull()){
			LPBITMAPINFOHEADER pbi = (LPBITMAPINFOHEADER)GlobalLock(m_hDib);
			
			if(pbi){
				WORD dwNumColors = (WORD)NumColors(pbi);
				if(dwNumColors){
					DWORD dwLen = sizeof(LOGPALETTE) + dwNumColors * sizeof(PALETTEENTRY);
					HGLOBAL hMemPal = GlobalAlloc(GHND,dwLen);
					if(hMemPal){
						LPLOGPALETTE plp = (LPLOGPALETTE)GlobalLock(hMemPal);
						if(plp){
							LPBITMAPINFO pi = (LPBITMAPINFO)pbi;
							plp->palVersion = PALVERSION;
							plp->palNumEntries = dwNumColors;
							for(WORD i=0;i<dwNumColors;i++){
								plp->palPalEntry[i].peRed = pi->bmiColors[i].rgbRed;
								plp->palPalEntry[i].peGreen = pi->bmiColors[i].rgbGreen;
								plp->palPalEntry[i].peBlue = pi->bmiColors[i].rgbBlue;
								plp->palPalEntry[i].peFlags = 0;
							}
							hPal = ::CreatePalette(plp);
							GlobalUnlock(plp);
						}
						GlobalFree(hMemPal);
					}
				}
				GlobalUnlock(pbi);
			}
		}
		return hPal;
	}
	BOOL Paint(HDC hdc,HPALETTE hPal)
	{
		CDCHandle dc = hdc;
		if(IsNull() || m_pbi == NULL)
			return FALSE;
		if(hPal){
			hPal = dc.SelectPalette(hPal,FALSE);
			dc.RealizePalette();
		}
		int lines = dc.SetDIBitsToDevice(0,0,Width(),Height(),0,0,0,Height(),(LPBYTE)m_pbi + \
			m_pbi->biSize + PaletteSize(m_pbi),(LPBITMAPINFO)m_pbi,DIB_RGB_COLORS);
		return lines != 0 ? TRUE : FALSE;
	}
	LPBYTE GetImageData(void)
	{
		LPBYTE p = NULL;

		if(!IsNull() && m_pbi != NULL){
			p = (LPBYTE)m_pbi + m_pbi->biSize + PaletteSize(m_pbi);
		}
		return p;
	}
	LPBITMAPINFO GetBitmapInfo(void)
	{
		return (LPBITMAPINFO)m_pbi;
	}
	LPBITMAPINFOHEADER GetBitmapInfoHeader(void)
	{
		return m_pbi;
	}
	void GetSize(CSize &sz)
	{
		sz.cx = Width();
		sz.cy = Height();
	}
	DWORD GetImageLen(void)
	{
		DWORD res = 0;

		if(!IsNull() && m_pbi)
			res = m_pbi->biSizeImage;
		return res;
	}
};

inline HBITMAP CaptureClientRect(HWND hWnd,CRect &rc)
{
	CClientDC dc(hWnd);
	CDC dcMem;
	CBitmap bmp;
	CBitmapHandle bmpOld;

	dcMem.CreateCompatibleDC(dc);
	bmp.CreateCompatibleBitmap(dc,rc.Width(),rc.Height());
	if(!bmp.IsNull()){
		bmpOld = dcMem.SelectBitmap(bmp);
		dcMem.BitBlt(0,0,rc.Width(),rc.Height(),dc,0,0,SRCCOPY);
		dcMem.SelectBitmap(bmpOld);
	}
	return bmp.Detach();
}

inline WORD NumColors(LPBITMAPINFOHEADER pbi)
{
	WORD res = 0;

	if(pbi){
		if(pbi->biBitCount <= 8)
			res = 1 << pbi->biBitCount;
		else
			res = 0;
	}
	return res;
}
inline DWORD PaletteSize(LPBITMAPINFOHEADER pbi)
{
	return NumColors(pbi) * sizeof(RGBQUAD);
}

} // NO5TL

#endif // !defined(AFX_DIB_H__86E82066_B637_4CA4_ACC6_86449C32D9F2__INCLUDED_)
