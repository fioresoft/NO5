// YahooTxtObj.h: Definition of the CYahooTxtObj class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YAHOOTXTOBJ_H__272E6FC2_0D21_4FBF_A899_98A6854EDFA1__INCLUDED_)
#define AFX_YAHOOTXTOBJ_H__272E6FC2_0D21_4FBF_A899_98A6854EDFA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include "no5_3.h"			// CLSID's

namespace NO5YAHOO
{
	class CTextStream;
}

/////////////////////////////////////////////////////////////////////////////
// CYahooTxtObj

class CYahooTxtObj : 
	public IDispatchImpl<IYahooTxtObj, &IID_IYahooTxtObj>, 
	public ISupportErrorInfo,
	public CComObjectRootEx<CComSingleThreadModel>
{
public:
	CYahooTxtObj() {}
BEGIN_COM_MAP(CYahooTxtObj)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IYahooTxtObj)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CYahooTxtObj) 

	HRESULT Init(NO5YAHOO::CTextStream *pts);

	//DECLARE_REGISTRY(CYahooTxtObj, _T("Ycht02.YahooTxtObj.1"), _T("Ycht02.YahooTxtObj"), IDS_YAHOOTXTOBJ_DESC, THREADFLAGS_BOTH)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IYahooTxtObj
public:
	STDMETHOD(get_Plain)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_Text)(/*[out, retval]*/ BSTR *pVal);
private:
	NO5YAHOO::CTextStream *m_pts;
};

#endif // !defined(AFX_YAHOOTXTOBJ_H__272E6FC2_0D21_4FBF_A899_98A6854EDFA1__INCLUDED_)
