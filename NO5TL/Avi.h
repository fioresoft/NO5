// Avi.h: interface for the CAvi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVI_H__D05A271C_1B58_4596_B5C4_03EB481B77B3__INCLUDED_)
#define AFX_AVI_H__D05A271C_1B58_4596_B5C4_03EB481B77B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vfw.h>

namespace NO5TL
{
template <class T>
inline T Module(T t)
{
	return t < 0 ? -t : t;
}


class CAvi
{
public:
	PAVIFILE m_pf;
	PAVISTREAM m_ps;
	PAVISTREAM m_pcs;
	LONG m_lPos;
	const DWORD m_dwRate;
	AVICOMPRESSOPTIONS m_co;
	DWORD m_dwWidth;
	DWORD m_dwHeight;
public:
	CAvi(DWORD dwRate = 1):m_dwRate(dwRate)
	{
		m_pf = NULL;
		m_ps = NULL;
		m_pcs = NULL;
		m_lPos = 0;
		ZeroMemory(&m_co,sizeof(m_co));
		m_dwWidth = m_dwHeight = 0;
		::AVIFileInit();
	}
	virtual ~CAvi()
	{
		Destroy();
		::AVIFileExit();
	}
	LONG GetPos(void) const
	{
		return m_lPos;
	}
	HRESULT FileOpen(LPCTSTR name)
	{
		HRESULT hr;

		ATLASSERT(NULL == m_pf);

		hr = ::AVIFileOpen(&m_pf,name,OF_CREATE|OF_WRITE,NULL);
		return hr;
	}
	HRESULT AddFrame(LPVOID pData,DWORD dwSize)
	{
		HRESULT hr = S_OK;

		if(!m_ps){
			ATLASSERT(m_lPos == 0);
			hr = CreateStream(m_dwWidth,m_dwHeight);
		}
		if(SUCCEEDED(hr)){
			hr = Write(pData,dwSize);
		}
		return hr;
	}
	HRESULT MakeCompressedStream(void)
	{
		HRESULT hr = E_POINTER;

		if(m_ps && !m_pcs){
			hr = ::AVIMakeCompressedStream(&m_pcs,m_ps,&m_co,NULL);
		}
		return hr;
	}
	HRESULT CreateStream(DWORD dwWidth,DWORD dwHeight)
	{
		AVISTREAMINFO si = {0};
		HRESULT hr = E_POINTER;

		if(m_pf){
			ATLASSERT(!m_ps && m_lPos == 0);

			m_dwWidth = dwWidth;
			m_dwHeight = dwHeight;
			si.fccType = streamtypeVIDEO;
			//si.fccHandler = mmioFOURCC('C','V','I','D');
			si.dwScale = 1;
			si.dwRate = m_dwRate;
			si.dwQuality = -1;		// default
			si.rcFrame.left = 0;
			si.rcFrame.top = 0;
			si.rcFrame.right = dwWidth;
			si.rcFrame.bottom = dwHeight;
			hr = ::AVIFileCreateStream(m_pf,&m_ps,&si);
		}
		return hr;
	}

	HRESULT Write(LPVOID pData,DWORD dwSize)
	{
		HRESULT hr = E_POINTER;
		PAVISTREAM ps = m_pcs ? m_pcs : m_ps;

		if(ps){
			LONG lSamples = 0,lWritten = 0;

			hr = ::AVIStreamWrite(ps,
				m_lPos++,
				1,				// samples
				pData,			// for video stream this is bitmapinfo
				dwSize,			
				AVIIF_KEYFRAME,
				&lSamples,		// samples written
				&lWritten);		// bytes written

			ATLASSERT(lSamples == 1/* && (DWORD)lWritten == dwSize*/);
		}
		return hr;
	}

	void Destroy(void)
	{
		if(m_pcs){
			::AVIStreamRelease(m_pcs);
			m_pcs = NULL;
		}
		if(m_ps){
			::AVIStreamRelease(m_ps);
			m_ps = NULL;
		}
		if(m_pf){
			::AVIFileRelease(m_pf);
			m_pf = NULL;
		}
		ZeroMemory(&m_co,sizeof(m_co));
		m_lPos = 0;
	}

	BOOL SaveOptionsDlg(HWND hWndParent,AVICOMPRESSOPTIONS &opt)
	{
		BOOL res = FALSE;

		if(m_ps){
			LPAVICOMPRESSOPTIONS pOpt = &opt;

			res = ::AVISaveOptions(hWndParent,0,1,&m_ps,&pOpt);
		}
		return res;
	}

	BOOL ChooseCompressorDlg(HWND hWndParent,LPBITMAPINFO pbi)
	{
		BOOL res;
		COMPVARS cv = {sizeof(cv)};

		res = ICCompressorChoose(hWndParent,
			ICMF_CHOOSE_DATARATE|ICMF_CHOOSE_KEYFRAME,
			pbi,NULL,&cv,NULL);
		if(res){
			m_co.fccType = streamtypeVIDEO;
			m_co.fccHandler = cv.fccHandler;
			m_co.dwQuality = cv.lQ;
		}
		return res;
	}
	HRESULT StreamSetFormat(PAVISTREAM ps,LPBITMAPINFOHEADER pbi)
	{
		DWORD dwNumColors;

		if(pbi->biBitCount < 16)
			dwNumColors = 1 << pbi->biBitCount;
		else
			dwNumColors = 0;
		return ::AVIStreamSetFormat(\
			ps,
			m_lPos,		// lPos
			pbi,
			pbi->biSize + \
			dwNumColors * sizeof(RGBQUAD));
	}
};

} // NO5TL;


#endif // !defined(AFX_AVI_H__D05A271C_1B58_4596_B5C4_03EB481B77B3__INCLUDED_)
