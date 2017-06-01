// YahooTxtObj.cpp : Implementation of CYcht02App and DLL registration.

#include "stdafx.h"
#include "no5_3.h"
#include "YahooTxtObj.h"
#include <yahoo\textstream.h>

/////////////////////////////////////////////////////////////////////////////
//

STDMETHODIMP CYahooTxtObj::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IYahooTxtObj,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

HRESULT CYahooTxtObj::Init(NO5YAHOO::CTextStream *pts)
{
	m_pts = pts;
	return S_OK;
}

STDMETHODIMP CYahooTxtObj::get_Text(BSTR *pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal){
		CString s = m_pts->GetYahooText();
		CComBSTR bs = s;
		*pVal = bs.Detach();
	}
	return S_OK;
}

STDMETHODIMP CYahooTxtObj::get_Plain(BSTR *pVal)
{
	HRESULT hr = E_POINTER;

	if(pVal){
		CString s = m_pts->GetPlainText2();
		CComBSTR bs = s;
		*pVal = bs.Detach();
	}
	return S_OK;
}
