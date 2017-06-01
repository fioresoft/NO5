// HtmlListBox.h: interface for the CHtmlListBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLLISTBOX_H__1E0C55ED_CB95_4137_A169_5F00318FE6AB__INCLUDED_)
#define AFX_HTMLLISTBOX_H__1E0C55ED_CB95_4137_A169_5F00318FE6AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <no5tl\htmledit.h>
#include <no5tl\sortedarray.h>

#define HLBN_SELCHANGING	1
#define HLBN_SELCHANGED		2
#define HLBN_COMPAREITEM	3
#define HLBN_DELETEITEM		4
#define HLBN_CLICK			5
#define HLBN_DBLCLK			6

namespace NO5TL
{

typedef struct tagHLBITEM
{
	LPCTSTR			text;
	LPVOID			pData;
	int				index;
	bool			bSelected;
	IHTMLElement	*pElem;
	IHTMLElement	*pDiv;
	IHTMLEventObj	*pEvent;

} HLBITEM, *LPHLBITEM;

typedef struct tagNMHTMLLISTBOX {
    NMHDR       hdr;
	HLBITEM		item1;
	HLBITEM		item2;
	bool		bubble;
} NMHTMLLISTBOX, *LPNMHTMLLISTBOX;


template <class T, class TBase = CAxWindow, class TWinTraits = CControlWinTraits>
class CHtmlListBoxImpl : \
	public CWindowImpl<T,TBase,TWinTraits>,
	//public CMessageFilter,
	public IDispEventImpl<1,T,&DIID_HTMLDocumentEvents,&LIBID_MSHTML,4,0>,
	public IDispEventImpl<2,T,&DIID_DWebBrowserEvents2,&LIBID_SHDocVw,1,0>
{
	typedef CHtmlListBoxImpl<T,TBase,TWinTraits> _ThisClass;
	typedef IDispEventImpl<1,T,&DIID_HTMLDocumentEvents,&LIBID_MSHTML,4,0> DocumentSink;
	typedef IDispEventImpl<2,T,&DIID_DWebBrowserEvents2,&LIBID_SHDocVw,1,0> BrowserSink;

	struct ItemData
	{
		LPVOID m_pUserData;
		CString m_text;
		ULONG m_cookie;
		//
		ItemData(LPCTSTR text,LPVOID pUserData,ULONG cookie)
		{
			m_text = text;
			m_pUserData = pUserData;
			m_cookie = cookie;
		}
	};

	struct CItemLess
	{
		_ThisClass *m_pOutter;
		//
		CItemLess(_ThisClass *pOutter):m_pOutter(pOutter)
		{
			//
		}
		bool operator()(ItemData *p1,ItemData *p2)
		{
			return m_pOutter->CompareItems(p1,p2) < 0;
		}
	};
	struct CItemEqual
	{
		_ThisClass *m_pOutter;
		//
		CItemEqual(_ThisClass *pOutter):m_pOutter(pOutter)
		{
			//
		}
		bool operator()(ItemData *p1,ItemData *p2)
		{
			return m_pOutter->CompareItems(p1,p2) == 0;
		}
	};
	friend CItemLess;
	friend CItemEqual;

	typedef CPointerSortedArray<ItemData,CItemLess> CContainer;

public:
	DECLARE_WND_SUPERCLASS(_T("No5HtmlListBox"),TBase::GetWndClassName())

	/*
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		   (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
			return FALSE;

		// give HTML page a chance to translate this message
		return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
	}
	*/

	int CompareItems( ItemData *p1,ItemData *p2)
	{
		CWindow wnd = GetParent();
		HLBITEM item1,item2;
		NMHTMLLISTBOX nm;

		item1.bSelected = false;
		item1.index = -1;
		item1.pData = p1->m_pUserData;
		item1.text = p1->m_text;
		//
		item2.bSelected = false;
		item2.index = -1;
		item2.pData = p2->m_pUserData;
		item2.text = p2->m_text;
		//
		nm.hdr.code = HLBN_COMPAREITEM;
		nm.item1 = item1;
		nm.item2 = item2;

		return (int)wnd.SendMessage(WM_NOTIFY,(WPARAM)GetDlgCtrlID(),(LPARAM)&nm);
	}

	BEGIN_MSG_MAP(CHtmlListBoxImpl)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
	END_MSG_MAP()

	BEGIN_SINK_MAP(T)
		SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONCLICK,OnClick)
		SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONDBLCLICK,OnDblClk)		
		SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONKEYDOWN,OnKeyDown)
		//SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONKEYPRESS,OnKeyPress)
		//SINK_ENTRY_EX(1,DIID_HTMLDocumentEvents,DISPID_HTMLDOCUMENTEVENTS_ONKEYUP,OnKeyUp)

		// web browser
		SINK_ENTRY_EX(2,DIID_DWebBrowserEvents2,DISPID_DOCUMENTCOMPLETE,OnDocumentComplete)
	END_SINK_MAP()

	LRESULT OnCreate(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		RECT r = {0};
		LRESULT res = DefWindowProc();
		
		HRESULT hr = GetBrowser();
		if(SUCCEEDED(hr)){
			hr = BrowserSink::DispEventAdvise(m_spBrowser);
		}
		else{
			hr = GetDocument();
			if(SUCCEEDED(hr)){
				hr = DocumentSink::DispEventAdvise(m_doc.m_sp);
			}
		}
		if(SUCCEEDED(hr)){
		//	CMessageLoop *pLoop = _Module.GetMessageLoop();
		//	pLoop->AddMessageFilter(this);
		}
		ATLASSERT(SUCCEEDED(hr));
		return 0;
	}

	LRESULT OnDestroy(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL &bHandled)
	{
		if(m_spBrowser){
			DocumentSink::DispEventUnadvise(m_doc.m_sp);
			BrowserSink::DispEventUnadvise(m_spBrowser);

			m_spBrowser.Release();
			m_doc.m_sp.Release();
			//CMessageLoop *pLoop = _Module.GetMessageLoop();
		//	pLoop->RemoveMessageFilter(this);
		}
		return 0;
	}

	// IHTMLDocumentEvents
	virtual VARIANT_BOOL __stdcall OnClick(void)
	{	
		CComPtr<IHTMLElement> spSrc;
		HRESULT hr = S_OK;
		
		if(m_doc.m_sp){
			CComPtr<IHTMLEventObj> spEvent;
			hr = m_doc.GetEventObj(&spEvent);
			if(hr == S_OK){
				hr = spEvent->get_srcElement(&spSrc);

				if(hr == S_OK){
					CComPtr<IHTMLElement> spItem;
					
					hr = GetDivItemFromChildElement(spSrc,&spItem);
					if(hr == S_OK){
						ULONG cookie = GetDivCookie(spItem);
						if(cookie >= 0){
							int index = FindItemByCookie(cookie);
							if(index >= 0){
								if(TRUE == (BOOL)NotifyParent2(HLBN_CLICK,index,spSrc,spItem,spEvent)){
									SetCurSel(index,true);
								}
							}
						}
					}
				}
			}
		}
		return VARIANT_FALSE; // allow bubbling
	}
	virtual VARIANT_BOOL __stdcall OnDblClk(void)
	{
		CComPtr<IHTMLElement> spSrc;
		HRESULT hr = S_OK;
		
		if(m_doc.m_sp){
			CComPtr<IHTMLEventObj> spEvent;
			hr = m_doc.GetEventObj(&spEvent);
			if(hr == S_OK){
				hr = spEvent->get_srcElement(&spSrc);

				if(hr == S_OK){
					CComPtr<IHTMLElement> spItem;
					
					hr = GetDivItemFromChildElement(spSrc,&spItem);
					if(hr == S_OK){
						ULONG cookie = GetDivCookie(spItem);
						if(cookie >= 0){
							int index = FindItemByCookie(cookie);
							if(index >= 0){
								if(TRUE == (BOOL)NotifyParent2(HLBN_DBLCLK,index,spSrc,spItem,spEvent)){
									SetCurSel(index,true);
								}
							}
						}
					}
				}
			}
		}
		return VARIANT_FALSE; // allow bubbling
	}
	virtual void __stdcall OnKeyDown(void)
	{
		CComPtr<IHTMLEventObj> spEvent;
		HRESULT hr;

		if(m_doc.m_sp){
			hr = m_doc.GetEventObj(&spEvent);
			if(hr == S_OK){
				long keycode = 0;
				hr = spEvent->get_keyCode(&keycode);
				if(hr == S_OK && keycode != 0){
					switch(keycode)
					{
					case VK_DOWN:
						{
							int cur = GetCurSel();
							if(cur >= 0 && (cur + 1 < GetCount()) )
								SetCurSel(cur+1,true);
						}
						break;
					case VK_UP:
						{
							const int cur = GetCurSel();
							if(cur > 0){
								SetCurSel(cur-1,true);
							}
						}
						break;
					default:
						break;
					}
				}
			}
		}
	}
	virtual VARIANT_BOOL __stdcall OnKeyPress(void)
	{
		CComPtr<IHTMLEventObj> spEvent;
		HRESULT hr;

		if(m_doc.m_sp){
			hr = m_doc.GetEventObj(&spEvent);
			if(hr == S_OK){
				long kc = 0;

				hr = spEvent->get_keyCode(&kc);
			}
		}
		return VARIANT_FALSE;
	}
	// DWebBrowserEvents2
	virtual void __stdcall OnDocumentComplete(IDispatch *pDisp,VARIANT *pUrl)
	{
		HRESULT hr = GetDocument();

		if(SUCCEEDED(hr)){
			hr = DocumentSink::DispEventAdvise(m_doc.m_sp);
		}
		ATLASSERT(SUCCEEDED(hr));
	}
private:
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
public:
	CHtmlListBoxImpl()
	{
		m_cookie = 0;
		m_iCurSel = -1;
	}
	virtual ~CHtmlListBoxImpl()
	{
		
	}
	int AddItem(LPCTSTR text,LPVOID pData)
	{
		ItemData *pItem = new ItemData(text,pData,++m_cookie);
		int index = -1;
		ItemData *pItemSel = NULL;
		int iOldSel = GetCurSel();
		
		if(pItem){
			if(iOldSel >= 0){
				pItemSel = m_items[iOldSel];
			}
			SetCurSel(-1);

			BOOL bAdded = m_items.Add(pItem,CItemLess(this));
			if(bAdded){
				index = m_items.Find(pItem,CItemEqual(this));
				if(index >= 0){
					HRESULT hr = InsertDivItem(index,text,pItem->m_cookie);
					ATLASSERT(hr == S_OK);
					if(hr == S_OK && pItemSel){
						int iNewSel = m_items.Find(pItemSel,CItemEqual(this));
						if(iNewSel >= 0)
							SetCurSel(iNewSel,false);
					}
				}
			}
		}
		return index;
	}
	BOOL DeleteItem(int index)
	{
		BOOL res = FALSE;

		ATLASSERT(index >= 0 && index < GetCount());
		if(index >= 0 && index < GetCount()){
			HRESULT hr;
			CComPtr<IHTMLElementCollection> spCol;
			int iNewSel = -1;

			if(index == m_iCurSel)
				iNewSel = -1;
			else if(index < m_iCurSel)
				iNewSel = m_iCurSel - 1;
			else
				iNewSel = m_iCurSel;
			SetCurSel(-1);

			// let our parent to know we are deleting this item
			NotifyParent(HLBN_DELETEITEM,index,-1);

		
			// remove item from our container
			res = m_items.RemoveAt(index);
			ATLASSERT(res);

			// remove the div item

			hr = GetDivItems(&spCol);
			if(hr == S_OK){
				long count;

				hr = spCol->get_length(&count);
				if(hr == S_OK){
					CHtmlElement el;
					CComVariant id(OLESTR("lbitem"));
					CComVariant pos;
					CString total;
					CString tmp;
					CComPtr<IDispatch> spDisp;

					for(long i=0,j=0;i<count;i++){
						pos = i;
						hr = spCol->item(id,pos,&spDisp);
						if(hr == S_OK && (el.m_sp = spDisp) && IsListItem(el.m_sp) ){
							if(j != index && ( S_OK == el.GetOuterHtml(tmp)) )
								total += tmp;
							j++;
						}
						spDisp.Release();
					}
					el.m_sp.Release();
					if(S_OK == GetInnerDiv(&el.m_sp))
						hr = el.PutInnerHtml(total);
				}
			}
			if(iNewSel >= 0)
				SetCurSel(iNewSel,false);
		}
		return res;
	}
	int GetCurSel(void) const
	{
		return m_iCurSel;
	}
	// return the previous selection index
	int SetCurSel(int index,bool bNotify = false)
	{
		int iOld = GetCurSel();
		CHtmlElement el;

		if(bNotify){
			NotifyParent(HLBN_SELCHANGING,iOld,index);
		}
		if(iOld >= 0){
			/*
			hr = GetDivByIndex(iOld,el);
			if(hr == S_OK)
				hr = el.SetClassName("normal");
			ATLASSERT(SUCCEEDED(hr));
			*/
		}
		if(index >= 0){
			/*
			el.m_sp.Release();
			hr = GetDivByIndex(index,el);
			if(hr == S_OK)
				hr = el.SetClassName("selected");
			ATLASSERT(SUCCEEDED(hr));
			*/
		}
		m_iCurSel = index;
		if(bNotify){
			NotifyParent(HLBN_SELCHANGED,iOld,index);
		}
		return iOld;
	}

	void ResetContent(void)
	{
		HRESULT hr;
		CHtmlElement el;
		int i;
		const int count = GetCount();

		for(i=0;i<count;i++){
			// this notifies the parent
			DeleteItem(0);
		}
		hr = GetInnerDiv(&el.m_sp);
		if(hr == S_OK)
			hr = el.PutInnerHtml("");
		m_iCurSel = -1;
	}

	int FindItem(LPCTSTR text)
	{
		ItemData item(text,NULL,-1);
		return m_items.Find(&item,CItemEqual(this));
	}
	int FindByUserData(LPVOID *pData)
	{
		ItemData item(NULL,pData,-1);
		return m_items.Find(&item,CItemEqual(this));
	}

	BOOL SetItemData(int index,LPVOID pUserData)
	{
		BOOL res = FALSE;

		if(index >=0 && index < GetCount()){
			ItemData *p = m_items[index];
			p->m_pUserData = pUserData;
		}
		return res;
	}

	LPVOID GetItemData(int index)
	{
		LPVOID p = NULL;

		ATLASSERT(index >= 0 && index < GetCount());
		if(index >= 0 && index < GetCount()){
			ItemData *pData = m_items[index];
			if(pData)
				p = pData->m_pUserData;
		}
		return p;
	}

	BOOL SetItemText(int index,LPCTSTR text)
	{
		BOOL res = FALSE;

		if(index >=0 && index < GetCount()){
			ItemData *p = m_items[index];
			p->m_text = text;
			HRESULT hr = UpdateDivItem(index);
			if(hr == S_OK)
				res = TRUE;
		}
		return res;
	}

	CString GetItemText(int index)
	{
		CString res;

		if(index >=0 && index < GetCount()){
			ItemData *p = m_items[index];
			if(p){
				res = p->m_text;
			}
		}
		return res;
	}


	int GetCount(void) const
	{
		return m_items.GetSize();
	}

protected:
	// finds an item in the list box, by his cookie,  and returns its index 
	int FindItemByCookie(ULONG cookie)
	{
		const int count = GetCount();
		int i;
		ItemData *p;
		int res = -1;

		for(i=0;i<count;i++){
			p = (ItemData *)m_items[i];
			if(cookie == p->m_cookie){
				res = i;
				break;
			}
		}
		return res;
	}

	HRESULT InsertDivItem(int index,LPCTSTR text,LONG cookie)
	{
		CHtmlElement el;
		HRESULT hr = E_FAIL;

		ATLASSERT(index >= 0 && index <= GetCount());
		if(index >= 0 && index <= GetCount()){
			CComBSTR where;

			if(index == 0){
				hr = GetInnerDiv(&el.m_sp);
				where = OLESTR("AfterBegin");
			}
			else{
				hr = GetDivByIndex(index - 1,&el.m_sp);
				where = OLESTR("AfterEnd");
			}
			if(hr == S_OK && el.m_sp){
				CString tag;
				// build our tag
				tag.Format("<div class=\"normal\" id=\"lbitem\" cookie=%d>%s</div>",
					cookie,text);
				hr = el.m_sp->insertAdjacentHTML(where,CComBSTR((LPCTSTR)tag));
			}
		}
		return hr;
	}
	HRESULT GetInnerDiv(IHTMLElement **ppElem)
	{
		CComQIPtr<IHTMLElementCollection> spCol;
		HRESULT hr = E_POINTER;

		if(m_doc.m_sp && ppElem != NULL && *ppElem == NULL){
			hr = m_doc.m_sp->get_all(&spCol);
			if(SUCCEEDED(hr)){
				CComPtr<IDispatch> spDisp;
				CComVariant varName(OLESTR("inner"));
				CComVariant varIndex = 0;

				hr = spCol->item(varName,varIndex,&spDisp);
				if(hr == S_OK){
					CComQIPtr<IHTMLElement> spElem = spDisp;
					if(!spElem)
						hr = E_FAIL;
					else
						*ppElem = spElem.Detach();
				}
			}
		}
		return hr;
	}
	// it should get collection of items whose id="lbitem" but i dont know how
	// so it gets a collection of all div elements
	HRESULT GetDivItems(IHTMLElementCollection **pp)
	{
		HRESULT hr = E_POINTER;

		if(m_doc.m_sp){
			CComQIPtr<IHTMLElementCollection> spCol;

			hr = m_doc.m_sp->get_all(&spCol);
			if(SUCCEEDED(hr)){
				CComVariant tag(OLESTR("div"));
				CComPtr<IDispatch> spDisp;

				hr = spCol->tags(tag,&spDisp);
				if(hr == S_OK){
					spCol.Release();
					if(spCol = spDisp){
						*pp = spCol.Detach();
						hr = S_OK;
					}
					else
						hr = S_FALSE;
				}
			}
		}
		return hr;
	}

	HRESULT GetDivByCookie(ULONG cookie,IHTMLElement **ppOut)
	{
		HRESULT hr = E_POINTER;

		ATLASSERT(ppOut && *ppOut == NULL);
		if(m_doc.m_sp && ppOut && *ppOut == NULL){
			CComQIPtr<IHTMLElementCollection> spCol;

			hr = GetDivItems(&spCol);
			if(hr == S_OK){
				CComPtr<IDispatch> spDisp;
				CComVariant varID(OLESTR("lbitem"));
				CComVariant varIndex = 0;
				CHtmlElement el;
				long count = 0;
				ULONG cur_cookie = -1;

				hr = spCol->get_length(&count);
				if(hr == S_OK){
					hr = S_FALSE;
					for(long i=0;i<count;i++){
						varIndex = i;
						hr = spCol->item(varID,varIndex,&spDisp);
						if(hr == S_OK){
							if(el.m_sp = spDisp){
								cur_cookie = GetDivCookie(el.m_sp);
								if(cur_cookie == cookie){
									*ppOut = el.m_sp.Detach();
									hr = S_OK;
									break;
								}
								else{
									spDisp.Release();
									el.m_sp.Release();
									hr = S_FALSE;
								}
							}
						}
					}
				}
			}
		}
		return hr;
	}

	HRESULT GetDivByIndex(int index,IHTMLElement **ppElem)
	{
		HRESULT hr = E_POINTER;

		ATLASSERT(ppElem && *ppElem == NULL);
		if(ppElem && *ppElem == NULL){
			ItemData *p = (ItemData *)m_items[index];

			if(p)
				hr = GetDivByCookie(p->m_cookie,ppElem);
		}
		return hr;
	}

	ULONG GetDivCookie(IHTMLElement *pElem)
	{
		long res = 0;
		CHtmlElement el;
		HRESULT hr;
		
		el.m_sp = pElem;
		hr = el.GetAttribute("cookie",&res);
		ATLASSERT(SUCCEEDED(hr));
		return (ULONG)res;
	}

	// check if the element is a list element ( which are div elements and have  cookie attribute )
	BOOL IsListItem(IHTMLElement *pElem)
	{
		BOOL res = FALSE;
		CString tag;
		CHtmlElement el;
		HRESULT hr;

		el.m_sp = pElem;
		hr = el.GetTagName(tag);
		
		if(SUCCEEDED(hr) && (!tag.CompareNoCase("div")) ){
			long cookie;
			hr = el.GetAttribute("cookie",&cookie);
			if(hr == S_OK && cookie > 0)
				res = TRUE;
		}
		return res;
	}
	// the user may have clicked in an element inside the div item, so we look upwards
	// until we find a div element that is a list item
	HRESULT GetDivItemFromChildElement(IHTMLElement *p,IHTMLElement **ppOut)
	{
		HRESULT hr = S_FALSE;
		CComPtr<IHTMLElement> spNext;
		CComPtr<IHTMLElement> spParent;

		spNext = p;
		*ppOut = NULL;

		while(spNext){
			if(IsListItem(spNext)){
				*ppOut = spNext.Detach();
				hr = S_OK;
				break;
			}
			else{
				spParent.Release();
				hr = spNext->get_parentElement(&spParent);
				if(SUCCEEDED(hr) && spParent){
					spNext = spParent;
				}
				else{
					hr = S_FALSE;
					break;
				}
			}
		}
		return hr;
	}
	LRESULT NotifyParent(UINT code,int iIndex1,int iIndex2 = -1)
	{
		CWindow w = GetParent();
		NMHTMLLISTBOX nm;
		HLBITEM item1 = {0},item2 = {0};
		ItemData *pItemData;

		nm.hdr.code = code;
		nm.hdr.hwndFrom = m_hWnd;
		nm.hdr.idFrom = GetDlgCtrlID();

		item1.index = iIndex1;
		item2.index = iIndex2;

		if(iIndex1 >= 0){
			pItemData = m_items[iIndex1];
			item1.text = pItemData->m_text;
			item1.pData = pItemData->m_pUserData;
			item1.bSelected = (iIndex1 == GetCurSel());
			nm.item1 = item1;
		}
		if(iIndex2 >= 0){
			pItemData = m_items[iIndex2];
			item2.text = pItemData->m_text;
			item2.pData = pItemData->m_pUserData;
			item2.bSelected = (iIndex2 == GetCurSel());
			nm.item2 = item2;
		}
		return w.SendMessage(WM_NOTIFY,(WPARAM)GetDlgCtrlID(),(LPARAM)&nm);
	}

	LRESULT NotifyParent2(UINT code,int iIndex1,IHTMLElement *pElem,IHTMLElement *pDiv = NULL,
		IHTMLEventObj *pEvent = NULL)
	{
		CWindow w = GetParent();
		NMHTMLLISTBOX nm;
		ItemData *pItemData;

		nm.hdr.code = code;
		nm.hdr.hwndFrom = m_hWnd;
		nm.hdr.idFrom = GetDlgCtrlID();

		nm.item1.index = iIndex1;
		nm.item2.index = -1;

		if(iIndex1 >= 0){
			pItemData = m_items[iIndex1];
			nm.item1.text = pItemData->m_text;
			nm.item1.pData = pItemData->m_pUserData;
			nm.item1.bSelected = (iIndex1 == GetCurSel());
			if(pElem)
				nm.item1.pElem = pElem;
			if(pDiv)
				nm.item1.pDiv = pDiv;
			if(pEvent)
				nm.item1.pEvent = pEvent;
		}
		return w.SendMessage(WM_NOTIFY,(WPARAM)GetDlgCtrlID(),(LPARAM)&nm);
	}

	// update the div item inner html based on the text stored
	HRESULT UpdateDivItem(int index)
	{
		ItemData *pItem = m_items[index];
		HRESULT hr = E_FAIL;

		if(pItem){
			CHtmlElement el;
			hr = GetDivByIndex(index,&el.m_sp);
			if(hr == S_OK){
				hr = el.PutInnerHtml(pItem->m_text);
			}
		}
		return hr;
	}

protected:
	CContainer m_items;
	int m_iCurSel;
	ULONG m_cookie;
	CHtmlDocument m_doc;
	CComQIPtr<IWebBrowser2> m_spBrowser;
};

class CHtmlListBox : public CHtmlListBoxImpl<CHtmlListBox>
{
public:
};

} // namespace NO5TL

#endif // !defined(AFX_HTMLLISTBOX_H__1E0C55ED_CB95_4137_A169_5F00318FE6AB__INCLUDED_)
