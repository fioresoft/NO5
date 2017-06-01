// HtmlEdit.h: interface for the CHtmlEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLEDIT_H__6285B954_4287_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_HTMLEDIT_H__6285B954_4287_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mshtmdid.h>
#include <exdisp.h> // header for library: SHDocVw  ( web browser control )
#include <exdispid.h> // web browser control's dispids
#include <yahoo\yahoocolor.h>

namespace NO5TL
{

// wrapper class to CComQIPtr<IHTMLDocument2>
class CHtmlDocument
{
public:
	CComQIPtr<IHTMLDocument2> m_sp;
public:

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
					hr = m_sp->write(psa);
				else
					hr = m_sp->writeln(psa);
			}
			::SafeArrayDestroy(psa);
		}
		return hr;
	}
	HRESULT PutDesignMode(BOOL bSet)
	{
		CComBSTR mode(bSet ? OLESTR("On") : OLESTR("Off"));
		return m_sp->put_designMode(mode);
	}
	BOOL GetDesignMode(void) const
	{
		CComBSTR mode;
		HRESULT hr;
		BOOL res = FALSE;
		
		hr = m_sp->get_designMode(&mode);
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
	HRESULT GetEventObj(IHTMLEventObj **pp)
	{
		HRESULT hr = E_POINTER;

		if(NULL != pp && ( *pp == NULL) ){
			CComPtr<IHTMLWindow2> spWnd;

			hr = m_sp->get_parentWindow(&spWnd);
			if(hr == S_OK){
				CComPtr<IHTMLEventObj> spEventObj;
				hr = spWnd->get_event(&spEventObj);
				if(hr == S_OK){
					*pp = spEventObj.Detach();
				}
			}
		}
		return hr;
	}
	HRESULT GetEventSource(IHTMLElement **pp)
	{
		HRESULT hr = E_POINTER;

		if(NULL != pp && ( *pp == NULL) ){
			CComPtr<IHTMLEventObj> spEventObj;

			hr = GetEventObj(&spEventObj);
			if(hr == S_OK){
				CComPtr<IHTMLElement> spElem;
				hr = spEventObj->get_srcElement(&spElem);
				if(hr == S_OK){
					*pp = spElem.Detach();
				}
			}
		}
		return hr;
	}
	BOOL QueryCmdEnabled(LPCTSTR cmd)
	{
		HRESULT hr = E_POINTER;
		BOOL res = FALSE;

		if(m_sp != NULL){
			USES_CONVERSION;
			VARIANT_BOOL vb = VARIANT_FALSE;
			hr = m_sp->queryCommandEnabled(T2OLE(cmd),&vb);
			if(SUCCEEDED(hr))
				res = (vb == VARIANT_TRUE ? TRUE : FALSE);
		}
		return res;
	}
	BOOL ExecCommand(LPCTSTR cmd)
	{
		HRESULT hr = E_POINTER;
		BOOL res = FALSE;

		if(m_sp != NULL){
			USES_CONVERSION;
			VARIANT_BOOL vb = VARIANT_FALSE;
			CComVariant v;
			hr = m_sp->execCommand(T2OLE(cmd),VARIANT_FALSE,v,&vb);
			if(SUCCEEDED(hr))
				res = (vb == VARIANT_TRUE ? TRUE : FALSE);
		}
		return res;
	}
	void Clear(void) { ExecCommand("clear");}
	void Copy(void) { ExecCommand("copy"); }
	void Cut(void) { ExecCommand("cut"); }
	void Paste(void) { ExecCommand("paste"); }
	BOOL CanCut()
	{
		return QueryCmdEnabled("Cut");
	}
	BOOL CanCopy()
	{
		return QueryCmdEnabled("Copy");
	}
	BOOL CanClear()
	{
		return QueryCmdEnabled("Cut");
	}
	BOOL CanPaste()
	{
		return QueryCmdEnabled("Paste");
	}
	// change the selector { color: #abcdef } of the first style sheet
	HRESULT ChangeStyleColor(LPCTSTR selector,COLORREF clr)
	{
		CComPtr<IHTMLRuleStyle> spStyle;
		HRESULT hr;
		NO5YAHOO::CYahooColor color(clr);
		CComVariant s(color.GetString(NO5YAHOO::CYahooColor::YCSF_2));

		hr = GetStyle(selector,&spStyle);
		if(SUCCEEDED(hr)){
			hr = spStyle->put_color(s);
		}
		return hr;
													
	}
	HRESULT ChangeStyleBkColor(LPCTSTR selector,COLORREF clr)
	{
		CComPtr<IHTMLRuleStyle> spStyle;
		HRESULT hr;
		NO5YAHOO::CYahooColor color(clr);
		CComVariant s(color.GetString(NO5YAHOO::CYahooColor::YCSF_2));

		hr = GetStyle(selector,&spStyle);
		if(SUCCEEDED(hr)){
			hr = spStyle->put_backgroundColor(s);
		}
		return hr;
													
	}
	HRESULT ChangeStyleFont(LPCTSTR selector,LPCTSTR s)
	{
		CComPtr<IHTMLRuleStyle> spStyle;
		HRESULT hr;

		hr = GetStyle(selector,&spStyle);
		if(SUCCEEDED(hr)){
			USES_CONVERSION;
			CComBSTR bs(s);
			hr = spStyle->put_fontFamily(bs);
		}
		return hr;
													
	}
	HRESULT ChangeStyleFontSize(LPCTSTR selector,CComVariant v)
	{
		CComPtr<IHTMLRuleStyle> spStyle;
		HRESULT hr;

		hr = GetStyle(selector,&spStyle);
		if(SUCCEEDED(hr)){
			hr = spStyle->put_fontSize(v);
		}
		return hr;
													
	}
	HRESULT ChangeStyleBold(LPCTSTR selector,LPCTSTR bold)
	{
		CComPtr<IHTMLRuleStyle> spStyle;
		HRESULT hr;

		hr = GetStyle(selector,&spStyle);
		if(SUCCEEDED(hr)){
			USES_CONVERSION;
			CComBSTR bs(bold);
			hr = spStyle->put_fontWeight(bs);
		}
		return hr;										
	}
	// get the selector{ color: #abcdef } of the first style sheet
	HRESULT GetStyle(LPCTSTR selector,IHTMLRuleStyle **ppStyle)
	{
		USES_CONVERSION;
		CComPtr<IHTMLStyleSheetsCollection> spSheets;
		CComQIPtr<IHTMLStyleSheet> spSheet;
		CComPtr<IHTMLStyleSheetRulesCollection> spRules;
		long len = 0;
		CComVariant varIndex(0);
		CComVariant varRes;
		HRESULT hr = E_POINTER;

		if((!m_sp) || (!ppStyle))
			return E_POINTER;
		*ppStyle = NULL;
		hr = m_sp->get_styleSheets(&spSheets);
		if(FAILED(hr))
			return hr;
		if(!spSheets)
			return S_FALSE;
				
		hr = spSheets->get_length(&len);
		if(len <= 0)
			return S_FALSE;
		
		hr = spSheets->item(&varIndex,&varRes);
		if(FAILED(hr))
			return hr;
		spSheet = varRes.pdispVal;
		if(!spSheet)
			return E_UNEXPECTED;	
		hr = spSheet->get_rules(&spRules);
		if(FAILED(hr))
			return hr;
		len = 0;
		hr = spRules->get_length(&len);
		if(FAILED(hr))
			return hr;
		for(long i=0;i<len;i++){
			CComPtr<IHTMLStyleSheetRule> spRule;
			hr = spRules->item(i,&spRule);
			if(FAILED(hr))
				break;

			CComBSTR bs;
			hr = spRule->get_selectorText(&bs);
			if(FAILED(hr))
				break;
		
			LPCTSTR p = OLE2CT(bs);
			if(!lstrcmpi(p,selector)){
				CComPtr<IHTMLRuleStyle> spStyle;
				hr = spRule->get_style(&spStyle);
				if(FAILED(hr))
					break;
				if(!spStyle){
					hr = S_FALSE;
					break;
				}
				*ppStyle = spStyle.Detach();
				break;
			}											
		}
		return hr;
	}

};

class CHtmlElement
{
public:
	CComQIPtr<IHTMLElement> m_sp;
public:
	// methods
	HRESULT AppendHtmlText(LPCTSTR text)
	{
		USES_CONVERSION;
		CComBSTR where = OLESTR("BeforeEnd");
		CComBSTR html = T2COLE(text);
		return m_sp->insertAdjacentHTML(where,html);
	}
	HRESULT AppendText(LPCTSTR text)
	{
		USES_CONVERSION;
		CComBSTR where = OLESTR("BeforeEnd");
		CComBSTR html = T2COLE(text);
		return m_sp->insertAdjacentText(where,html);
	}
	HRESULT PutInnerHtml(LPCTSTR text)
	{
		USES_CONVERSION;
		CComBSTR bstrText(text);
		HRESULT hr = m_sp->put_innerHTML(bstrText);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT GetInnerHtml(CString &out)
	{
		USES_CONVERSION;
		CComBSTR bstrHtml;
		HRESULT hr = m_sp->get_innerHTML(&bstrHtml);
		ATLASSERT(hr == S_OK);
		if(hr = S_OK){
			out = OLE2CT(bstrHtml);
		}
		return hr;
	}
	HRESULT GetOuterHtml(CString &out)
	{
		USES_CONVERSION;
		CComBSTR bstrHtml;
		HRESULT hr = m_sp->get_outerHTML(&bstrHtml);
		ATLASSERT(hr == S_OK);
		if(hr == S_OK){
			out = OLE2CT(bstrHtml);
		}
		return hr;
	}
	HRESULT GetAttribute(LPCTSTR name,CString &out)
	{
		CComVariant v;
		CComBSTR bstrName(name);
		USES_CONVERSION;

		HRESULT hr = m_sp->getAttribute(bstrName,FALSE,&v);

		if(hr == S_OK){
			if(v.vt != VT_BSTR){
				hr = v.ChangeType(VT_BSTR);
			}
			if(hr == S_OK){
				out = OLE2CT(v.bstrVal);
			}
		}
		return hr;
	}
	HRESULT GetAttribute(LPCTSTR name,long *pVal)
	{
		CComVariant v;
		CComBSTR bstrName(name);
		USES_CONVERSION;

		HRESULT hr = m_sp->getAttribute(bstrName,FALSE,&v);

		if(hr == S_OK){
			hr = v.ChangeType(VT_I4);
			if(SUCCEEDED(hr)){
				*pVal = v.lVal;
			}
		}
		return hr;
	}

	HRESULT SetAttribute(LPCTSTR name,LPCTSTR val)
	{
		CComBSTR bstrName(name);
		CComVariant varVal(val);

		return m_sp->setAttribute(bstrName,varVal,VARIANT_FALSE);
	}

	HRESULT GetTagName(CString &out)
	{
		CComBSTR bstrTag;
		HRESULT hr = m_sp->get_tagName(&bstrTag);

		if(hr == S_OK){
			USES_CONVERSION;
			out = OLE2CT(bstrTag);
		}
		return hr;
	}
	HRESULT SetClassName(LPCTSTR name)
	{
		USES_CONVERSION;
		CComBSTR bstrName(name);

		return m_sp->put_className(bstrName);
	}
	HRESULT GetClassName(CString &out)
	{
		USES_CONVERSION;
		CComBSTR bstrName;
		HRESULT hr = m_sp->get_className(&bstrName);
		if(SUCCEEDED(hr)){
			out = OLE2CT(bstrName);
		}
		return hr;
	}
};

class CBodyElement
{
public:
	CComQIPtr<IHTMLBodyElement> m_sp;
public:
	HRESULT PutScrollBars(BOOL bSet)
	{
		CComBSTR bars(bSet ? OLESTR("yes") : OLESTR("no"));
		return m_sp->put_scroll(bars);
	}
	BOOL GetScrollBars(void) const
	{
		CComBSTR bars;
		HRESULT hr;
		BOOL res = FALSE;

		hr = m_sp->get_scroll(&bars);
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
		return m_sp->put_noWrap(bNoWrap);
	}
	BOOL GetTextWrap(void) const
	{
		VARIANT_BOOL bNoWrap;
		BOOL res = TRUE;
		HRESULT hr = m_sp->get_noWrap(&bNoWrap);
		if(SUCCEEDED(hr)){
			res = ( bNoWrap == VARIANT_TRUE ? FALSE : TRUE );
		}
		else
			ATLASSERT(0);
		return res;
	}
	HRESULT PutBgColor(LPCTSTR color)
	{
		CComVariant v = color;
		HRESULT hr = m_sp->put_bgColor(v);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	HRESULT PutBgColor(COLORREF clr)
	{
		TCHAR buf[10] = {0};
		
		wsprintf(buf,"#%02x%02x%02x",GetRValue(clr),GetGValue(clr),
			GetBValue(clr));
		return PutBgColor(buf);
	}
	HRESULT GetBgColor(COLORREF &color)
	{
		CComVariant v;
		HRESULT hr = m_sp->get_bgColor(&v);
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


class CHtmlCtrl : public CAxWindow
{
public:
	CComQIPtr<IWebBrowser2> m_spBrowser;
	CHtmlDocument m_doc;
	CHtmlElement m_elem;	// body
	CBodyElement m_body;
	CComQIPtr<IOleCommandTarget> m_spCmd;

public:
	CHtmlCtrl(HWND hWnd = NULL):CAxWindow(hWnd)
	{
		//
	}

	HRESULT GetBody(void)
	{
		HRESULT hr = E_POINTER;

		m_elem.m_sp.Release();

		if(m_doc.m_sp){
			hr = m_doc.m_sp->get_body(&m_elem.m_sp);
			if(hr == S_OK && m_elem.m_sp){
				m_body.m_sp = m_elem.m_sp;
				if(!m_body.m_sp)
					hr = E_FAIL;
			}
			else
				hr = E_FAIL;
		}
		return hr;
	}

	HRESULT GetCmdTarget(void)
	{
		HRESULT hr = E_POINTER;

		m_spCmd.Release();
		if(m_doc.m_sp){
			if(m_spCmd = m_doc.m_sp)
				hr = S_OK;
		}
		return hr;
	}

	// this will fail if the control is not a web browser control, but just a mshtml control
	HRESULT GetBrowser(void)
	{
		HRESULT hr;
		CComPtr<IUnknown> sp;

		m_spBrowser.Release();
		hr = QueryControl(&sp);
		if(hr == S_OK){
			m_spBrowser = sp;
			if(!m_spBrowser)
				hr = E_FAIL;
		}
		return hr;
	}

	HRESULT GetDocument(void)
	{
		HRESULT hr;

		m_doc.m_sp.Release();

		if(m_spBrowser){
			CComPtr<IDispatch> spDisp;

			hr = m_spBrowser->get_Document(&spDisp);
			if(hr == S_OK){
				m_doc.m_sp = spDisp;
			}
		}
		else{
			CComPtr<IUnknown> sp;
			hr = QueryControl(&sp);
			if(hr == S_OK){
				m_doc.m_sp = sp;
				if(!m_doc.m_sp)
					hr = E_FAIL;
			}
		}
		return hr;
	}

	

	// get all interfaces except browser and document
	HRESULT GetInterfaces(void)
	{
		ATLASSERT( m_doc.m_sp != NULL );
		HRESULT hr = GetCmdTarget();
		ATLASSERT(SUCCEEDED(hr));
		if(hr == S_OK){
			hr = GetBody();
			ATLASSERT(SUCCEEDED(hr));
		}
		return hr;
	}

	// release all interfaces
	void ReleaseInterfaces(void)
	{
		m_spBrowser.Release();
		m_doc.m_sp.Release();
		m_elem.m_sp.Release();
		m_spCmd.Release();
		m_body.m_sp.Release();
	}

	// methods
	HRESULT AppendHtmlText(LPCTSTR text)
	{
		return m_elem.AppendHtmlText(text);
	}
	HRESULT AppendText(LPCTSTR text)
	{
		return m_elem.AppendText(text);
	}
	
	HRESULT ScrollWindow(int cy = 100)
	{
		HRESULT hr;
		CComPtr<IHTMLWindow2> spWnd;

		hr = m_doc.m_sp->get_parentWindow(&spWnd);
		if(SUCCEEDED(hr)){
			hr = spWnd->scroll(0,cy);
		}
		return hr;
	}
	HRESULT ScrollWindowBy(int cy = 100)
	{
		HRESULT hr;
		CComPtr<IHTMLWindow2> spWnd;

		hr = m_doc.m_sp->get_parentWindow(&spWnd);
		if(SUCCEEDED(hr)){
			hr = spWnd->scrollBy(0,cy);
		}
		return hr;
	}
	
	HRESULT ScrollText(void)
	{	
		return ScrollWindowBy(0x00ffffff);
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

	HRESULT PutScrollBars(BOOL bSet)
	{
		return m_body.PutScrollBars(bSet);
	}
	BOOL GetScrollBars(void) const
	{
		return m_body.GetScrollBars();
	}
	HRESULT PutTextWrap(BOOL bSet)
	{
		return m_body.PutTextWrap(bSet);
	}
	BOOL GetTextWrap(void) const
	{
		return m_body.GetTextWrap();
	}
	HRESULT PutBgColor(LPCTSTR clr)
	{
		return m_body.PutBgColor(clr);
	}
	HRESULT PutBgColor(COLORREF clr)
	{
		return m_body.PutBgColor(clr);
	}
	HRESULT GetBgColor(COLORREF &color)
	{
		return m_body.GetBgColor(color);
	}
};


// this class is not used but its good to copy and paste
#ifdef HTMLEDITIMPL

const UINT MSHTML_LIBVER_MAJ = 4;
const UINT MSHTML_LIBVER_MIN = 0;
const UINT WBC_LIBVER_MAJ = 1;
const UINT WBC_LIBVER_MIN = 0;
const UINT MSHTML_DISP = 100;
const UINT WBC_DISP = 101;


template <class T, class TBase = CHtmlCtrl , class TWinTraits = CControlWinTraits >
class CHtmlEditImpl : \
	public CWindowImpl< T , TBase, TWinTraits>,
	public IDispEventImpl<MSHTML_DISP,T,&DIID_HTMLDocumentEvents,
		&LIBID_MSHTML,MSHTML_LIBVER_MAJ,MSHTML_LIBVER_MIN>,
	public IDispEventImpl<WBC_DISP,T,&DIID_DWebBrowserEvents2,&LIBID_SHDocVw,
		WBC_LIBVER_MAJ,WBC_LIBVER_MIN>,
	public IDispatchImpl<IDocHostUIHandlerDispatch, &IID_IDocHostUIHandlerDispatch, &LIBID_ATLLib>
{
	typedef CWindowImpl< T , TBase, TWinTraits> _BaseClass;
	typedef CHtmlEditImpl<T,TBase,TWinTraits> _ThisClass;
	typedef IDispEventImpl<MSHTML_DISP,T,&DIID_HTMLDocumentEvents,
		&LIBID_MSHTML,MSHTML_LIBVER_MAJ,MSHTML_LIBVER_MIN> DispEventHtml;
	typedef IDispEventImpl<WBC_DISP,T,&DIID_DWebBrowserEvents2,&LIBID_SHDocVw,
		WBC_LIBVER_MAJ,WBC_LIBVER_MIN> DispEventBrowser;
private:
	bool m_destroyed; // why are we getting WM_DESTROY twice ?
protected:
	CHtmlEditImpl()
	{
		m_destroyed = false;
	}

	
public:
	DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		   (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
			return FALSE;

		// give HTML page a chance to translate this message
		return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
	}

	BEGIN_MSG_MAP(CHtmlEditImpl)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
	END_MSG_MAP()

	BEGIN_SINK_MAP(T)
		// these dispids were not found in the header mshtmdid
		// obs: mudar para sink_entry_ex quando utiliza-los

		//SINK_ENTRY(MSHTML_DISP,DISPID_HTMLDOCUMENTEVENTS_ONBEFOREACTIVATE,OnBeforeActivate)
		//SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONBEFOREEDITFOCUS,OnBeforeEditFocus)
		//SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONCELLCHANGE,OnCellChange)
		//SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONCONTEXTMENU,OnContextMenu)
		//SINK_ENTRY(MSHTML_DISP,DISPID_HTMLDOCUMENTEVENTS_ONDATAAVAILABLE,OnDataAvailable)
		//SINK_ENTRY(MSHTML_DISP,DISPID_HTMLDOCUMENTEVENTS_ONDATASETCHANGED,OnDatasetChanged)
		//SINK_ENTRY(MSHTML_DISP,DISPID_HTMLDOCUMENTEVENTS_ONDATASETCOMPLETE,OnDatasetComplete)
		//SINK_ENTRY(MSHTML_DISP,DISPID_HTMLDOCUMENTEVENTS_ONFOCUSIN,OnFocusIn)
		//SINK_ENTRY(MSHTML_DISP,DISPID_HTMLDOCUMENTEVENTS_ONFOCUSOUT,OnFocusOut)
		//SINK_ENTRY(MSHTML_DISP,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEWHEEL,OnMouseWheel)
		//SINK_ENTRY(MSHTML_DISP,DISPID_HTMLDOCUMENTEVENTS_ONPROPERTYCHANGE,OnPropertyChange)
		//SINK_ENTRY(MSHTML_DISP,DISPID_HTMLDOCUMENTEVENTS_ONROWSDELETE,OnRowsDelete)
		//SINK_ENTRY(MSHTML_DISP,DISPID_HTMLDOCUMENTEVENTS_ONROWSINSERTED,OnRowsInserted)
		//SINK_ENTRY(MSHTML_DISP,DISPID_HTMLDOCUMENTEVENTS_ONSELECTIONCHANGE,OnSelectionChange)
		//SINK_ENTRY(MSHTML_DISP,DISPID_HTMLDOCUMENTEVENTS_ONSTOP,OnEventsStop)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONAFTERUPDATE,OnAfterUpdate)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONBEFOREUPDATE,OnBeforeUpdate)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONCLICK,OnClick)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONDBLCLICK,OnDblClk)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONDRAGSTART,OnDragStart)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONERRORUPDATE,OnErrorUpdate)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONHELP,OnHelp)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONKEYDOWN,OnKeyDown)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONKEYPRESS,OnKeyPress)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONKEYUP,OnKeyUp)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEDOWN,OnMouseDown)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEOUT,OnMouseOut)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEOVER,OnMouseOver)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEUP,OnMouseUp)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONMOUSEMOVE,OnMouseMove)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONREADYSTATECHANGE,OnReadyStateChange)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONROWENTER,OnRowEnter)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONROWEXIT,OnRowExit)
		SINK_ENTRY_EX(MSHTML_DISP,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONSELECTSTART,OnSelectStart)

		// web browser
		SINK_ENTRY_EX(WBC_DISP,DIID_DWebBrowserEvents2,DISPID_DOCUMENTCOMPLETE,OnDocumentComplete)
	END_SINK_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CHtmlDocument doc;
		CComPtr<IUnknown> sp;

		LRESULT res = DefWindowProc(uMsg,wParam,lParam);

	
		HRESULT hr = GetBrowser();
		if(FAILED(hr)){
			hr = GetDocument();
			if(SUCCEEDED(hr)){
				hr = DispEventHtml::DispEventAdvise(m_doc.m_sp);
				ATLASSERT(SUCCEEDED(hr));
				hr = GetInterfaces();
			}
		}
		else{
			hr = DispEventBrowser::DispEventAdvise(m_spBrowser);
			ATLASSERT(hr == S_OK);
		}
		if(SUCCEEDED(hr))
			hr = SetExternalUIHandler(static_cast<IDocHostUIHandlerDispatch*>(this));
		ATLASSERT(SUCCEEDED(hr));

		return res;
		
	}
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// why is OnDestroy being called twice ?

		if(!m_destroyed){
			HRESULT hr;
			ATLASSERT(IsWindow());

			m_destroyed = true;

			if(m_doc.m_sp){
				hr = DispEventHtml::DispEventUnadvise(m_doc.m_sp);
				ATLASSERT(SUCCEEDED(hr));
			}
			if(m_spBrowser){
				if(DispEventBrowser::m_dwEventCookie != 0xFEFEFEFE){
					hr = DispEventBrowser::DispEventUnadvise(m_spBrowser);
					ATLASSERT(SUCCEEDED(hr));
				}
			}
		}
		return 0;
	}

protected:
	// IHTMLDocumentEvents

	virtual void __stdcall OnAfterUpdate(void) {}
	virtual void __stdcall OnBeforeEditFocus(void){}
	virtual void __stdcall OnBeforeUpdate(void){}
	virtual VARIANT_BOOL __stdcall OnClick(void){return VARIANT_FALSE;}
	virtual VARIANT_BOOL __stdcall OnDblClk(void){return VARIANT_FALSE;}
	virtual VARIANT_BOOL __stdcall OnDragStart(void){return VARIANT_FALSE;}
	virtual VARIANT_BOOL __stdcall OnErrorUpdate(void){return VARIANT_FALSE;}
	virtual VARIANT_BOOL __stdcall OnHelp(void){return VARIANT_FALSE;}
	virtual void __stdcall OnKeyDown(void){}
	virtual VARIANT_BOOL __stdcall OnKeyPress(void){ return VARIANT_FALSE;}
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

	// DWebBrowserEvents2
	virtual void __stdcall OnDocumentComplete(IDispatch *pDisp,VARIANT *pUrl)
	{
		HRESULT hr = GetDocument();

		if(SUCCEEDED(hr)){
			hr = DispEventHtml::DispEventAdvise(m_doc.m_sp);
			ATLASSERT(SUCCEEDED(hr));
			if(SUCCEEDED(hr))
				hr = GetInterfaces();

		}
		ATLASSERT(SUCCEEDED(hr));
	}

	// IDocHostUIHandlerDispatch

	STDMETHOD(ShowContextMenu)( 
            /* [in] */ DWORD dwID,
            /* [in] */ DWORD x,
            /* [in] */ DWORD y,
            /* [in] */ IUnknown *pcmdtReserved,
            /* [in] */ IDispatch  *pdispReserved,
            /* [retval][out] */ HRESULT  *dwRetVal)
	{
		return S_FALSE;
	}
	
	STDMETHOD(GetHostInfo)( 
            /* [out][in] */ DWORD  *pdwFlags,
            /* [out][in] */ DWORD  *pdwDoubleClick)
	{
		return S_FALSE;
	}
	
	STDMETHOD(ShowUI)( 
            /* [in] */ DWORD dwID,
            /* [in] */ IUnknown  *pActiveObject,
            /* [in] */ IUnknown  *pCommandTarget,
            /* [in] */ IUnknown  *pFrame,
            /* [in] */ IUnknown  *pDoc,
            /* [retval][out] */ HRESULT  *dwRetVal)
	{
		return S_FALSE;
	}
	
	STDMETHOD(HideUI)( void)
	{
		return S_FALSE;
	}
	
	STDMETHOD(UpdateUI)( void)
	{
		return S_FALSE;
	}
        
    STDMETHOD(EnableModeless)( 
            /* [in] */ VARIANT_BOOL fEnable)
	{
		return S_FALSE;
	}
        
    STDMETHOD(OnDocWindowActivate)( 
            /* [in] */ VARIANT_BOOL fActivate)
	{
		return S_FALSE;
	}
	
	STDMETHOD(OnFrameWindowActivate)( 
            /* [in] */ VARIANT_BOOL fActivate)
	{
		return S_FALSE;
	}
        
    STDMETHOD(ResizeBorder)( 
            /* [in] */ long left,
            /* [in] */ long top,
            /* [in] */ long right,
            /* [in] */ long bottom,
            /* [in] */ IUnknown  *pUIWindow,
            /* [in] */ VARIANT_BOOL fFrameWindow)
	{
		 return S_FALSE;
	}
	
	STDMETHOD(TranslateAccelerator)( 
            /* [in] */ DWORD hWnd,
            /* [in] */ DWORD nMessage,
            /* [in] */ DWORD wParam,
            /* [in] */ DWORD lParam,
            /* [in] */ BSTR bstrGuidCmdGroup,
            /* [in] */ DWORD nCmdID,
            /* [retval][out] */ HRESULT  *dwRetVal)
	{
		return S_FALSE;
	}
        
    STDMETHOD(GetOptionKeyPath)( 
            /* [out] */ BSTR  *pbstrKey,
            /* [in] */ DWORD dw)
	{
		return S_FALSE;
	}
        
    STDMETHOD(GetDropTarget)( 
            /* [in] */ IUnknown  *pDropTarget,
            /* [out] */ IUnknown  **ppDropTarget)
	{
		return S_FALSE;
	}
        
    STDMETHOD(GetExternal)( 
           /* [out] */ IDispatch  **ppDispatch)
	{
		*ppDispatch = NULL;
		return S_FALSE;
	}
        
    STDMETHOD(TranslateUrl)( 
            /* [in] */ DWORD dwTranslate,
            /* [in] */ BSTR bstrURLIn,
            /* [out] */ BSTR  *pbstrURLOut)
	{
		return S_FALSE;
	}
        
    STDMETHOD(FilterDataObject)( 
           /* [in] */ IUnknown  *pDO,
           /* [out] */ IUnknown  **ppDORet)
	{
		return S_FALSE;
	}
	// IUnknown
	STDMETHOD(QueryInterface)(REFIID iid,LPVOID *ppvObj)
	{
		HRESULT hr = S_OK;
		*ppvObj = NULL;
		T *pT = static_cast<T*>(this);

		if(InlineIsEqualUnknown(iid)){
			*ppvObj = static_cast<IUnknown *>(pT);
		}
		else if(InlineIsEqualGUID(iid,IID_IDispatch))
		{
			*ppvObj = static_cast<IDispatch *>(pT);
		}
		else if(InlineIsEqualGUID(iid,IID_IDocHostUIHandlerDispatch)){
			*ppvObj = static_cast<IDocHostUIHandlerDispatch *>(pT);
		}
		else
			hr = E_NOINTERFACE;

		if(SUCCEEDED(hr)){
			IUnknown *p = static_cast<IUnknown *>(pT);
			p->AddRef();
		}
		return hr;
	}
	STDMETHOD_(ULONG,AddRef)(void)
	{
		return 1;
	}
	STDMETHOD_(ULONG,Release)(void)
	{
		return 1;
	}
};

#endif // HTMLEDITIMPL

} // NO5TL

#endif // !defined(AFX_HTMLEDIT_H__6285B954_4287_11DA_A17D_000103DD18CD__INCLUDED_)


