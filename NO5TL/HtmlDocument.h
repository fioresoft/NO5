// HtmlDocument.h: interface for the CHtmlDocument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLDOCUMENT_H__8669AEA3_794B_11D7_A17A_DD326E69E131__INCLUDED_)
#define AFX_HTMLDOCUMENT_H__8669AEA3_794B_11D7_A17A_DD326E69E131__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <mshtmdid.h>
#include <mshtml.h>
#include "safearray.h"



namespace NO5TL
{
#if 0	// refazer

class CHtmlDocument :  public CComQIPtr<IHTMLDocument2>
{
	typedef CComQIPtr<IHTMLDocument2> _BaseClass;
public:
	CHtmlDocument()
	{
		p=NULL;
	}
	CHtmlDocument(IHTMLDocument2 *lp):_BaseClass(lp)
	{
		
	}
	CHtmlDocument(const CHtmlDocument& lp)
	{
		if ((p = lp.p) != NULL)
			p->AddRef();
	}
	CHtmlDocument(IUnknown* lp):_BaseClass(lp)
	{
		
	}
	~CHtmlDocument()
	{
		//
	}
	IHTMLDocument2 * operator=(IHTMLDocument2* lp)
	{
		return _BaseClass::operator=(lp);
	}
	IHTMLDocument2 * operator=(const CHtmlDocument& lp)
	{
		return _BaseClass::operator=((_BaseClass &)lp);
	}
	IHTMLDocument2* operator=(IUnknown* lp)
	{
		return _BaseClass::operator=(lp);
	}
	HRESULT PutBgColor(LPCTSTR color)
	{
		CComBSTR tmp(color);
		CComVariant v = tmp.m_str;
		HRESULT hr = _BaseClass->put_bgColor(v);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT PutBgColor(COLORREF color)
	{
		CComVariant v = (long)color;
		HRESULT hr = _BaseClass->put_bgColor(v);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT GetBgColor(COLORREF &color)
	{
		CComVariant v;
		HRESULT hr = _BaseClass->get_bgColor(&v);
		if(SUCCEEDED(hr)){
			hr = GetColor(v,color);
		}
		return hr;
	}
	HRESULT PutLinkColor(LPCTSTR color)
	{
		CComBSTR tmp(color);
		CComVariant v = tmp.m_str;
		HRESULT hr = _BaseClass->put_linkColor(v);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT GetLinkColor(COLORREF &color)
	{
		CComVariant v;
		HRESULT hr = _BaseClass->get_linkColor(&v);
		if(SUCCEEDED(hr)){
			hr = GetColor(v,color);
		}
		return hr;
	}
	HRESULT PutLinkColor(COLORREF color)
	{
		CComVariant v = (long)color;
		HRESULT hr = _BaseClass->put_linkColor(v);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT PutVLinkColor(LPCTSTR color)
	{
		CComBSTR tmp(color);
		CComVariant v = tmp.m_str;
		HRESULT hr = _BaseClass->put_vlinkColor(v);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT PutVLinkColor(COLORREF color)
	{
		CComVariant v = (long)color;
		HRESULT hr = _BaseClass->put_vlinkColor(v);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT GetVLinkColor(COLORREF &color)
	{
		CComVariant v;
		HRESULT hr = _BaseClass->get_vlinkColor(&v);
		if(SUCCEEDED(hr)){
			hr = GetColor(v,color);
		}
		return hr;
	}
	HRESULT PutUrl(LPCTSTR url)
	{
		CComBSTR tmp(url);
		HRESULT hr = _BaseClass->put_URL(tmp);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT GetUrl(CComBSTR &url)
	{
		url.Empty();
		HRESULT hr = _BaseClass->get_URL(&url);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT PutCookie(LPCTSTR url)
	{
		CComBSTR tmp(url);
		HRESULT hr = _BaseClass->put_cookie(tmp);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT GetCookie(CComBSTR &cookie)
	{
		cookie.Empty();
		HRESULT hr = _BaseClass->get_cookie(&cookie);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT PutTitle(LPCTSTR title)
	{
		CComBSTR tmp(title);
		HRESULT hr = _BaseClass->put_title(tmp);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT GetTitle(CComBSTR &title)
	{
		title.Empty();
		HRESULT hr = _BaseClass->get_title(&title);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT Write(NO5TL::CSafeArray<BSTR,VT_BSTR> &text)
	{
		HRESULT hr = _BaseClass->write((SAFEARRAY *)text);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	// Closes an output stream and forces data sent to layout 
	// to display. - ?
	HRESULT Close(void)
	{
		HRESULT hr = _BaseClass->close();
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	bool QueryCmdSupported(LPCTSTR cmd)
	{
		CComBSTR _cmd(cmd);
		VARIANT_BOOL res = VARIANT_FALSE;
		HRESULT hr = _BaseClass->queryCommandSupported(_cmd,&res);

		ATLASSERT(SUCCEEDED(hr));
		return (SUCCEEDED(hr))?res == VARIANT_TRUE:false;
	}
	CComBSTR QueryCmdText(LPCTSTR cmd)
	{
		CComBSTR _cmd(cmd);
		CComBSTR text;
		HRESULT hr = _BaseClass->queryCommandText(_cmd,&text);
		ATLASSERT(SUCCEEDED(hr));
		return text;
	}
	bool ExecCmd(LPCTSTR cmd,VARIANT &value)
	{
		HRESULT hr;
		VARIANT_BOOL ret = VARIANT_FALSE;
		CComBSTR _cmd(cmd);

		hr = _BaseClass->execCommand(_cmd,VARIANT_TRUE,value,&ret);
		ATLASSERT(SUCCEEDED(hr));
		return (SUCCEEDED(hr))?(ret == VARIANT_TRUE):false;
	}
private:
	HRESULT GetColor(CComVariant v,COLORREF &color)
	{
		HRESULT hr = v.ChangeType(VT_I4);
		if(SUCCEEDED(hr)){
			color = v.lVal;
		}
		return hr;
	}
};

#endif // 0

// events

const UINT MSHTML_LIBVER_MAJ = 4;
const UINT MSHTML_LIBVER_MIN = 0;

// fazend template <class T,UINT nID> da varios problemas
// na compilacao

template <UINT nID>
class HTMLDocumentEventsImpl : \
	public IDispEventImpl<nID,HTMLDocumentEventsImpl<nID>,
		&DIID_HTMLDocumentEvents,&LIBID_MSHTML,MSHTML_LIBVER_MAJ,
		MSHTML_LIBVER_MIN>
{
	typedef HTMLDocumentEventsImpl<nID> _ThisClass;
	typedef IDispEventImpl<nID,_ThisClass,&DIID_HTMLDocumentEvents,
		&LIBID_MSHTML,4,0> _BaseClass;
public:
	BEGIN_SINK_MAP(_ThisClass)
		// these dispids were not found in the header mshtmdid
		// obs: mudar para sink_entry_ex quando utiliza-los

		//SINK_ENTRY(nID,DISPID_HTMLDOCUMENTEVENTS_ONBEFOREACTIVATE,OnBeforeActivate)
		//SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONBEFOREEDITFOCUS,OnBeforeEditFocus)
		//SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONCELLCHANGE,OnCellChange)
		//SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONCONTEXTMENU,OnContextMenu)
		//SINK_ENTRY(nID,DISPID_HTMLDOCUMENTEVENTS_ONDATAAVAILABLE,OnDataAvailable)
		//SINK_ENTRY(nID,DISPID_HTMLDOCUMENTEVENTS_ONDATASETCHANGED,OnDatasetChanged)
		//SINK_ENTRY(nID,DISPID_HTMLDOCUMENTEVENTS_ONDATASETCOMPLETE,OnDatasetComplete)
		//SINK_ENTRY(nID,DISPID_HTMLDOCUMENTEVENTS_ONFOCUSIN,OnFocusIn)
		//SINK_ENTRY(nID,DISPID_HTMLDOCUMENTEVENTS_ONFOCUSOUT,OnFocusOut)
		//SINK_ENTRY(nID,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEWHEEL,OnMouseWheel)
		//SINK_ENTRY(nID,DISPID_HTMLDOCUMENTEVENTS_ONPROPERTYCHANGE,OnPropertyChange)
		//SINK_ENTRY(nID,DISPID_HTMLDOCUMENTEVENTS_ONROWSDELETE,OnRowsDelete)
		//SINK_ENTRY(nID,DISPID_HTMLDOCUMENTEVENTS_ONROWSINSERTED,OnRowsInserted)
		//SINK_ENTRY(nID,DISPID_HTMLDOCUMENTEVENTS_ONSELECTIONCHANGE,OnSelectionChange)
		//SINK_ENTRY(nID,DISPID_HTMLDOCUMENTEVENTS_ONSTOP,OnEventsStop)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONAFTERUPDATE,OnAfterUpdate)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONBEFOREUPDATE,OnBeforeUpdate)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONCLICK,OnClick)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONDBLCLICK,OnDblClk)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONDRAGSTART,OnDragStart)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONERRORUPDATE,OnErrorUpdate)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONHELP,OnHelp)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONKEYDOWN,OnKeyDown)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONKEYPRESS,OnKeyPress)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONKEYUP,OnKeyUp)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEDOWN,OnMouseDown)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEOUT,OnMouseOut)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEOVER,OnMouseOver)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEUP,OnMouseUp)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONREADYSTATECHANGE,OnReadyStateChange)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONROWENTER,OnRowEnter)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONROWEXIT,OnRowExit)
		SINK_ENTRY_EX(nID,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONSELECTSTART,OnSelectStart)
	END_SINK_MAP()

	//virtual void __stdcall OnBeforeActivate(IHTMLEventObj *pObj){}
	//virtual void __stdcall OnCellChange(void){}
	//virtual VARIANT_BOOL __stdcall OnContextMenu(void){return VARIANT_FALSE;}
	//virtual void __stdcall OnDataAvailable(void){}
	//virtual void __stdcall OnDatasetChanged(void){}
	//virtual void __stdcall OnDatasetComplete(void){}
	//virtual void __stdcall OnFocusIn(IHTMLEventObj *pObj){}
	//virtual void __stdcall OnFocusOut(IHTMLEventObj *pObj){}
	//virtual void __stdcall OnMouseWheel(void){}
	//virtual void __stdcall OnPropertyChange(void){}
	//virtual void __stdcall OnRowsDelete(void){}
	//virtual void __stdcall OnRowsInserted(void){}
	//virtual void __stdcall OnSelectionChange(void){}

	virtual void __stdcall OnAfterUpdate(void) {}
	virtual void __stdcall OnBeforeEditFocus(void){}
	virtual void __stdcall OnBeforeUpdate(void){}
	virtual VARIANT_BOOL __stdcall OnClick(void){return VARIANT_FALSE;}
	virtual VARIANT_BOOL __stdcall OnDblClk(void){return VARIANT_FALSE;}
	virtual VARIANT_BOOL __stdcall OnDragStart(void){return VARIANT_FALSE;}
	virtual VARIANT_BOOL __stdcall OnErrorUpdate(void){return VARIANT_FALSE;}
	virtual VARIANT_BOOL __stdcall OnHelp(void){return VARIANT_FALSE;}
	virtual void __stdcall OnKeyDown(void){}
	virtual void __stdcall OnKeyPress(void){}
	virtual void __stdcall OnKeyUp(void){}
	virtual void __stdcall OnMouseDown(void){}
	virtual void __stdcall OnMouseMove(void){}
	virtual void __stdcall OnMouseUp(void){}
	virtual void __stdcall OnMouseOver(void){}
	virtual void __stdcall OnMouseOut(void){}
	virtual void __stdcall OnReadyStateChange(void){}
	virtual void __stdcall OnRowEnter(void){}
	virtual void __stdcall OnRowExit(void){}
	virtual void __stdcall OnSelectStart(void){}
	virtual VARIANT_BOOL __stdcall OnStop(void){return VARIANT_FALSE;}
};

template <class TBase = CWindow>
class CHtmlEditT : public CAxWindowT<TBase>
{
	typedef CAxWindowT<TBase> _BaseClass;

protected:
	CComQIPtr<IHTMLDocument2> m_spDoc;
private:
	CComPtr<IHTMLElement> m_spElem;
	CComQIPtr<IHTMLBodyElement> m_spBody;
	CComQIPtr<IOleCommandTarget> m_spCmd;
	CComQIPtr<IHTMLTextContainer> m_spTextCont;
	
public:
	CHtmlEditT(HWND hWnd = NULL):_BaseClass(hWnd)
	{
		//
	}
	HRESULT GetInterfaces(void)
	{
		HRESULT hr = S_OK;

		if(!m_spDoc){
			hr = QueryControl(&m_spDoc);
		}
		if(SUCCEEDED(hr)){
			m_spCmd = m_spDoc;
			if(m_spCmd){
				hr = m_spDoc->get_body(&m_spElem);
				if(SUCCEEDED(hr)){
					m_spBody = m_spElem;
					if(!m_spBody)
						hr = E_NOINTERFACE;
					else{
						m_spTextCont = m_spElem;
						if(!m_spTextCont)
							m_spTextCont = m_spBody;
						/*
						if(!m_spTextCont){
							// the program still works. ok
						*/
					}
				}
			}
			else
				hr = E_NOINTERFACE;
		}
		return hr;
	}

	void ReleaseInterfaces(void)
	{
		m_spDoc.Release();
		m_spElem.Release();
		m_spCmd.Release();
		m_spBody.Release();
		m_spTextCont.Release();
	}

	IHTMLDocument2 * GetDocument(void)
	{
		return (IHTMLDocument2 *)m_spDoc;
	}
	

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		HWND hWnd =  _BaseClass::Create( hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);

		if(IsWindow()){
			HRESULT hr = GetInterfaces();
			ATLASSERT(SUCCEEDED(hr));
		}
		return hWnd;
	}
	HWND Create(HWND hWndParent, LPRECT lpRect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			HMENU hMenu = NULL, LPVOID lpCreateParam = NULL)
	{
		HWND hWnd = _BaseClass::Create(hWndParent, lpRect, szWindowName, dwStyle, dwExStyle, hMenu, lpCreateParam);
		if(IsWindow()){
			HRESULT hr = GetInterfaces();
			ATLASSERT(SUCCEEDED(hr));
		}
		return hWnd;
	}

	// methods
	HRESULT AppendHtmlText(LPCTSTR text)
	{
		USES_CONVERSION;
		CComBSTR where = OLESTR("BeforeEnd");
		CComBSTR html = T2COLE(text);
		return m_spElem->insertAdjacentHTML(where,html);
	}
	HRESULT AppendText(LPCTSTR text)
	{
		USES_CONVERSION;
		CComBSTR where = OLESTR("BeforeEnd");
		CComBSTR html = T2COLE(text);
		return m_spElem->insertAdjacentText(where,html);
	}
	HRESULT Write(LPCTSTR text,BOOL bNewLine)
	{
		SAFEARRAY *psa;
		HRESULT hr = E_FAIL;
		SAFEARRAYBOUND bounds;

		bounds.cElements = 1;
		bounds.lLbound = 0;
		psa = ::SafeArrayCreate(VT_VARIANT,1,&bounds);
		if(psa){
			CComVariant s(text);
			long index = 0;
			hr = ::SafeArrayPutElement(psa,&index,(VARIANT *)&s);
			if(SUCCEEDED(hr)){
				if(!bNewLine)
					hr = m_spDoc->write(psa);
				else
					hr = m_spDoc->writeln(psa);
				
				if(FAILED(hr)){
					MessageBox(GetErrorDesc(hr));
				}
			}
			::SafeArrayDestroy(psa);
		}
		return hr;
	}
	HRESULT ScrollWindow(int cy = 100)
	{
		HRESULT hr;
		CComPtr<IHTMLWindow2> spWnd;

		hr = m_spDoc->get_parentWindow(&spWnd);
		if(SUCCEEDED(hr)){
			hr = spWnd->scroll(0,100);
		}
		return hr;
	}
	HRESULT ScrollWindowBy(int cy = 100)
	{
		HRESULT hr;
		CComPtr<IHTMLWindow2> spWnd;

		hr = m_spDoc->get_parentWindow(&spWnd);
		if(SUCCEEDED(hr)){
			hr = spWnd->scrollBy(0,100);
		}
		return hr;
	}
	
	HRESULT ScrollText(void)
	{
		HRESULT hr;

		if(!m_spTextCont){
			hr = ScrollWindowBy(0x00ffffff);
		}
		else{
			// well, lets just put a huge value, for the top
			// of the visible areal
			hr = m_spTextCont->put_scrollTop(0x00ffffff);
		}
		return hr;

		// topo = total - page
		/*
		long total=0,page=0;
		HRESULT hr;

		// this should get the total height according to the
		// docs, but it gets a fixed height, probably the page
		hr = m_spElem->get_offsetHeight(&total);
		if(SUCCEEDED(hr)){
			// and this should get the visible height( page )
			// but its getting increasing height
			// probably the total height
			hr = spCont->get_scrollHeight(&page);
			if(SUCCEEDED(hr)){
				// so we invert here
				hr = spCont->put_scrollTop(page - total);
			}
		}
		*/
		return hr;
	}

	HRESULT ExecCmd(DWORD nCmd)
	{
		return m_spCmd->Exec(&CGID_MSHTML,nCmd,0,NULL,NULL);
	}
	HRESULT ExecCmd(DWORD nCmd,LPCTSTR val)
	{
		CComVariant in = val;
		return m_spCmd->Exec(&CGID_MSHTML,nCmd,0,&in,NULL);
	}
	HRESULT ExecCmd(DWORD nCmd,DWORD nVal)
	{
		CComVariant in((long)nVal);
		return m_spCmd->Exec(&CGID_MSHTML,nCmd,0,&in,NULL);
	}
	HRESULT PutDesignMode(BOOL bSet)
	{
		CComBSTR mode(bSet ? OLESTR("On") : OLESTR("Off"));
		return m_spDoc->put_designMode(mode);
	}
	BOOL GetDesignMode(void) const
	{
		CComBSTR mode;
		HRESULT hr;
		BOOL res = FALSE;
		
		hr = m_spDoc->get_designMode(&mode);
		if(SUCCEEDED(hr)){
			if(mode == OLESTR("On")){
				res = TRUE;
			}
			else{
				res = FALSE;
			}
		}
		ATLASSERT(SUCCEEDED(hr));

		return res;
	}
	HRESULT PutScrollBars(BOOL bSet)
	{
		CComBSTR bars(bSet ? OLESTR("yes") : OLESTR("no"));
		return m_spBody->put_scroll(bars);
	}
	BOOL GetScrollBars(void) const
	{
		CComBSTR bars;
		HRESULT hr;
		BOOL res = FALSE;

		hr = m_spBody->get_scroll(&bars);
		if(SUCCEEDED(hr)){
			if(bars == OLESTR("yes")){
				res = TRUE;
			}
			else{
				res = FALSE;
			}
		}
		ATLASSERT(hr == S_OK);

		return res;
	}
	HRESULT PutTextWrap(BOOL bSet)
	{
		VARIANT_BOOL bNoWrap = bSet ? VARIANT_FALSE : VARIANT_TRUE;
		return m_spBody->put_noWrap(bNoWrap);
	}
	BOOL GetTextWrap(void) const
	{
		VARIANT_BOOL bNoWrap;
		BOOL res = TRUE;
		HRESULT hr = m_spBody->get_noWrap(&bNoWrap);
		if(SUCCEEDED(hr)){
			res = bNoWrap == VARIANT_TRUE ? FALSE : TRUE;
		}
		else
			ATLASSERT(0);
		return res;
	}
	HRESULT PutBgColor(LPCTSTR color)
	{
		CComVariant v = color;
		HRESULT hr = m_spBody->put_bgColor(v);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT PutBgColor(COLORREF clr)
	{
		TCHAR buf[10] = {0};
		CComVariant v;

		wsprintf(buf,"#%02x%02x%02x",GetRValue(clr),GetGValue(clr),
			GetBValue(clr));
		v = buf;
		HRESULT hr = m_spBody->put_bgColor(v);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT GetBgColor(COLORREF &color)
	{
		CComVariant v;
		HRESULT hr = m_spBody->get_bgColor(&v);
		if(SUCCEEDED(hr)){
			hr = GetColor(v,color);
		}
		return hr;
	}
private:
	HRESULT GetColor(CComVariant v,COLORREF &color)
	{
		HRESULT hr = v.ChangeType(VT_I4);
		if(SUCCEEDED(hr)){
			color = v.lVal;
		}
		return hr;
	}
};

typedef CHtmlEditT<CWindow> CHtmlEdit;

template <class TBase = CWindow>
class CMarshaledHtmlEditT : public CHtmlEditT<TBase>
{
	typedef CHtmlEditT<TBase> _BaseClass;
	CComPtr<IStream> m_spStream;

public:
	CMarshaledHtmlEditT(HWND hWnd = NULL):_BaseClass(hWnd)
	{
		//
	}
	HRESULT MarshalDocument(IHTMLDocument2 *pDoc)
	{
		HRESULT hr = E_POINTER;

		if(!m_spStream){
			hr = ::CoMarshalInterThreadInterfaceInStream(\
				__uuidof(IHTMLDocument2),(IUnknown *)pDoc,
				&m_spStream);
		}
		return hr;
	}
	HRESULT UnmarshalDocument(void)
	{
		HRESULT hr = E_POINTER;

		if(m_spStream && (!m_spDoc)){
			hr = ::CoGetInterfaceAndReleaseStream(m_spStream.Detach(),
				__uuidof(IHTMLDocument2),(LPVOID *)&m_spDoc);
		}
		return hr;
	}
};

typedef CMarshaledHtmlEditT<CWindow> CMarshaledHtmlEdit;


} // NO5TL

#endif // !defined(AFX_HTMLDOCUMENT_H__8669AEA3_794B_11D7_A17A_DD326E69E131__INCLUDED_)
