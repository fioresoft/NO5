// HtmlListBox.h: interface for the CHtmlListBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLLISTBOX_H__65A43772_4434_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_HTMLLISTBOX_H__65A43772_4434_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sortedarray.h"
#include "htmledit.h"

namespace NO5TL
{

template <class T,class TCont>
class CHtmlListBoxImpl : public CHtmlEditImpl< T >
{
	typedef CHtmlEditImpl< T > _BaseClass;
	typedef CHtmlListBoxImpl<T,TCont> _ThisClass;
protected:
	typedef TCont cont_type;
	typedef TCont::item_type item_type;
	cont_type m_items;
	item_type *m_pItem;			// currenctly selected item
	int m_iCurSel;				// index of the current sel
	long m_cookies;				// counter used to uniquely indentify each item in html	
public:
	CHtmlListBoxImpl(void)
	{
		m_pItem = NULL;
		m_iCurSel = -1;
		m_cookies = 0;
	}
	
	BEGIN_MSG_MAP(CHtmlListBoxImpl)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()

	// methods

	// return index
	int AddItem(const item_type &item)
	{
		int index = -1;
		item_type *p = new item_type(item);
		
		if(p){
			p->m_cookie = ++m_cookies;
			if(m_items.Add(p)){
				index = m_items.Find(p);
				if(index >= 0){
					InsertDivItem(index,p);
					// restore the correct current selection
					if(m_pItem)
						SetCurSel(m_items.Find(m_pItem));
				}
			}
		}
		return index;
	}
	BOOL RemoveItem(int index)
	{
		BOOL res = FALSE;
		item_type *p = m_items[index];
		
		RemoveDivItem(p->m_cookie);

		if(res = m_items.RemoveAt(index)){
			if(index < m_iCurSel){
				m_iCurSel--;
			}
			else if(index == m_iCurSel){
				if(!m_items.GetSize())	// current sel was zero and we only had one item
					m_iCurSel = -1;
				else if(index == m_items.GetSize()) // we removed the last item
					m_iCurSel--;
				else{
					// we removed some item in the middle, the m_iCurSel index can remain the
					// same
				}
			}
			else{
				// we removed an item above current sel, so let it alone
			}
			if(m_iCurSel >= 0)
				m_pItem = m_items[m_iCurSel];
			else
				m_pItem = NULL;

		}
		return res;
	}
	void SetCurSel(int index)
	{
		int iNewSel;

		if(index < 0 || index >= m_items.GetSize()){
			iNewSel = -1;
		}
		else
			iNewSel = index;

		if(iNewSel < 0){
			if(m_pItem){
				DisplayNormal(m_pItem);
				m_pItem = NULL;
			}
		}
		else{
			item_type *pNewSel = m_items[iNewSel];

			if(m_pItem)
				DisplayNormal(m_pItem);
			if(pNewSel)
				DisplaySelected(pNewSel);
			m_pItem = pNewSel;
		}
		m_iCurSel = iNewSel;
	}
	int GetCurSel(void) const
	{
		return m_iCurSel;
	}
	void ResetContent(void)
	{
		m_items.RemoveAll();
		RemoveAllDivItems();
		m_cookies = 0;
		m_iCurSel = -1;
		m_pItem = NULL;
	}
	int GetCount(void)
	{
		return m_items.GetSize();
	}
	void UpdateItem(int index)
	{
		ATLASSERT( index >=0 && index < GetCount());
		if(index >=0 && index < GetCount()){
			item_type *pItem = m_items[index];
			ATLASSERT(pItem);
			if(pItem){
				BOOL bSelected = (index == GetCurSel()) ? TRUE : FALSE;
				
				if(bSelected)
					DisplaySelected(pItem);
				else
					DisplayNormal(pItem);
			}
		}
	}
	// overdiable

	void FormatItemHtml(item_type *pItem,BOOL bSelected,CString &out)
	{
		out.Empty();
		out = pItem->m_text;
	}
	BOOL OnItemClicked(int index,CHtmlElement &src)
	{
		return TRUE; // allow selection change
	}
	void OnItemDoubleClicked(int index, CHtmlElement &src)
	{
		//
	}
	void OnContextMenu(int index,CHtmlElement &src,POINT &pt)
	{
		//
	}

	// implementation
protected:
	// helper class, the item must have already been added to internal list and cookie
	// must be set
	HRESULT InsertDivItem(int index,item_type *p)
	{
		CHtmlElement el;
		HRESULT hr;
		CComBSTR bstrDiv;
		CString sDiv;
		CString sHtml;
		T *pT = static_cast<T*>(this);

		ATLASSERT(p->m_cookie  > 0);
		pT->FormatItemHtml(p,FALSE,sHtml);
		sDiv.Format("<div class=\"normal\" id=\"item\" cookie=\"%d\" isitem=1>%s</div>",
				p->m_cookie,(LPCTSTR)sHtml);
		bstrDiv = sDiv;
			
		ATLASSERT(index >= 0 && index < m_items.GetSize());

		if(index == 0 || (index == m_items.GetSize() - 1)){
			hr = GetInnerDiv(&el.m_sp);
			if(hr == S_OK){
				if(index == 0){
					hr = el.m_sp->insertAdjacentHTML( CComBSTR(OLESTR("AfterBegin")) ,bstrDiv);
				}
				else{
					hr = el.m_sp->insertAdjacentHTML( CComBSTR(OLESTR("BeforeEnd")) ,bstrDiv);
				}
			}
		}
		else{
			ATLASSERT(index > 0);
			hr = GetDivByIndex(&el.m_sp,index - 1);
			if(hr == S_OK){
				hr = el.m_sp->insertAdjacentHTML( CComBSTR(OLESTR("AfterEnd")) ,bstrDiv);
			}
		}
		return hr;
	}
	void RemoveDivItem(long cookie)
	{
		CHtmlElement elInner;
		HRESULT hr;

		ATLASSERT(cookie > 0);

		hr = GetInnerDiv(&elInner.m_sp);
		if(hr == S_OK){
			CComPtr<IHTMLElementCollection> spCol;

			hr = GetItemCollection(&spCol);
			if(hr == S_OK){
				long count = 0;
				hr = spCol->get_length(&count);
				if(hr == S_OK && count){
					CComVariant id(OLESTR("item"));
					CComVariant number;
					CComPtr<IDispatch> spDisp;
					CHtmlElement el;
					long cur_cookie;
					CString sText;
					CString sOuter;

					for(long i=0;i<count;i++){
						number = i;
						spDisp.Release();
						hr = spCol->item(id,number,&spDisp);
						if(hr == S_OK && (el.m_sp = spDisp) && IsListItem(el.m_sp)){
							cur_cookie = GetDivCookie(el.m_sp);
							if(cur_cookie != cookie){
								sOuter.Empty();
								hr = el.GetOuterHtml(sOuter);
								if(hr == S_OK){
									sText += sOuter;
								}
							}
						}
					}
					hr = elInner.PutInnerHtml(sText);
				}
			}
		}
	}

			
	HRESULT GetInnerDiv(IHTMLElement **pp)
	{
		CComQIPtr<IHTMLElementCollection> spCol;
		HRESULT hr = E_POINTER;

		if(m_doc.m_sp){
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
						*pp = spElem.Detach();
				}
			}
		}
		return hr;
	}
	// it should get collection of items whose id="item" but i dont know how
	// so it gets a collection of all div elements
	HRESULT GetItemCollection(IHTMLElementCollection **pp)
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

	HRESULT GetDivByCookie(IHTMLElement **pp,long cookie)
	{
		HRESULT hr = E_POINTER;

		if(m_doc.m_sp){
			CComQIPtr<IHTMLElementCollection> spCol;

			hr = GetItemCollection(&spCol);
			if(hr == S_OK){
				CComPtr<IDispatch> spDisp;
				CComVariant varID(OLESTR("item"));
				CComVariant varIndex = 0;
				CHtmlElement el;
				long count = 0;
				long cur_cookie = -1;

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
									*pp = el.m_sp.Detach();
									hr = S_OK;
									break;
								}
								else{
									spDisp.Release();
									el.m_sp.Release();
								}
							}
						}
					}
				}
			}
		}
		return hr;
	}
	// hopefully the position in the html is the same as in our list
	HRESULT GetDivByIndex(IHTMLElement **pp,long index)
	{
		HRESULT hr = E_POINTER;

		if(m_doc.m_sp){
			CComQIPtr<IHTMLElementCollection> spCol;

			hr = GetItemCollection(&spCol);
			if(hr == S_OK){
				CComPtr<IDispatch> spDisp;
				CComVariant varID(OLESTR("item"));
				CComVariant varIndex = 0;
				CHtmlElement el;
				long count = 0;
				long i,j;

				hr = spCol->get_length(&count);
				if(hr == S_OK){
					for(i=0,j=0;i<count;i++){
						varIndex = i;
						spDisp.Detach();
						el.m_sp.Detach();
						hr = spCol->item(varID,varIndex,&spDisp);
						if(hr == S_OK && (el.m_sp = spDisp) && IsListItem(el.m_sp)){
							if(j == index){
								*pp = el.m_sp.Detach();
								break;
							}
							else
								j++;
						}
					}
					if(j != index)
						hr = S_FALSE;
				}
			}
		}
		return hr;
	}
	long GetDivCookie(IHTMLElement *p)
	{
		CHtmlElement elem;
		long res = -1;
		
		if(elem.m_sp = p){
			HRESULT hr = elem.GetAttribute("cookie",&res);
			ATLASSERT(SUCCEEDED(hr));
		}
		return res;
	}

	// check if the element is a list element ( which are div elements and have isitem attribute )
	BOOL IsListItem(IHTMLElement *p)
	{
		CHtmlElement elem;
		BOOL res = FALSE;

		if(elem.m_sp = p){
			CString tag;
			HRESULT hr = elem.GetTagName(tag);
			if(SUCCEEDED(hr) && (!tag.CompareNoCase("div")) ){
				long isitem = 0;
				hr = elem.GetAttribute("isitem",&isitem);
				if(hr == S_OK && isitem == 1)
					res = TRUE;
			}
		}
		return res;
	}
	// the user may have clicked in an element inside the div item, so we look upwards
	// until we find a div element that is a list item
	HRESULT GetDivFromChild(IHTMLElement *p,IHTMLElement **ppOut)
	{
		HRESULT hr = S_FALSE;
		CComPtr<IHTMLElement> spNext;
		CComPtr<IHTMLElement> spParent;

		spNext = p;

		while(spNext){
			if(IsListItem(spNext)){
				*ppOut = spNext.Detach();
				hr = S_OK;
				break;
			}
			else{
				hr = spNext->get_parentElement(&spParent);
				if(SUCCEEDED(hr)){
					spNext = spParent;
					spParent.Release();
				}
				else
					break;
			}
		}
		return hr;
	}

	HRESULT DisplaySelected(item_type *pItem)
	{
		CHtmlElement elem;
		HRESULT hr;


		hr = GetDivByCookie(&elem.m_sp,pItem->m_cookie);
		if(SUCCEEDED(hr)){
			hr = elem.SetClassName("selected");
			if(SUCCEEDED(hr)){
				CString s;
				T *pT = static_cast<T*>(this);

				pT->FormatItemHtml(pItem,TRUE,s);
				hr = elem.PutInnerHtml(s);
			}
		}
		return hr;
	}

	HRESULT DisplayNormal(item_type *pItem)
	{
		CHtmlElement elem;
		HRESULT hr;


		hr = GetDivByCookie(&elem.m_sp,pItem->m_cookie);
		if(SUCCEEDED(hr)){
			hr = elem.SetClassName("normal");
			if(SUCCEEDED(hr)){
				CString s;
				T *pT = static_cast<T*>(this);

				pT->FormatItemHtml(pItem,FALSE,s);
				hr = elem.PutInnerHtml(s);
			}
		}
		return hr;
	}
	HRESULT RemoveAllDivItems(void)
	{
		CHtmlElement elem;
		HRESULT hr;

		hr = GetInnerDiv(&elem.m_sp);
		if(hr == S_OK){
			hr = elem.PutInnerHtml("");
		}
		return hr;
	}

	virtual VARIANT_BOOL __stdcall OnClick(void)
	{
		CHtmlElement elSrc,elItem;
		HRESULT hr;

		if(m_doc.m_sp){
			hr = m_doc.GetEventSource(&elSrc.m_sp);
			if(hr == S_OK){
				hr = GetDivFromChild(elSrc.m_sp,&elItem.m_sp);
				if(hr == S_OK){
					long cookie = GetDivCookie(elItem.m_sp);
					int index = m_items.Find(cookie);
					if(index >= 0){
						T *pT = static_cast<T*>(this);
						if(pT->OnItemClicked(index,elSrc))
							SetCurSel(index);
					}
				}
			}
		}
		return VARIANT_FALSE;
	}
	virtual VARIANT_BOOL __stdcall OnDblClk(void)
	{
		CHtmlElement elSrc,elItem;
		HRESULT hr;

		if(m_doc.m_sp){
			hr = m_doc.GetEventSource(&elSrc.m_sp);
			if(hr == S_OK){
				hr = GetDivFromChild(elSrc.m_sp,&elItem.m_sp);
				if(hr == S_OK){
					long cookie = GetDivCookie(elItem.m_sp);
					int index = m_items.Find(cookie);
					if(index >= 0){
						T *pT = static_cast<T*>(this);
						pT->OnItemDoubleClicked(index,elSrc);
					}
				}
			}
		}
		return VARIANT_FALSE;
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
							if(m_iCurSel >= 0 && (m_iCurSel < m_items.GetSize() - 1))
								SetCurSel(m_iCurSel+1);
						}
						break;
					case VK_UP:
						{
							if(m_iCurSel > 0)
								SetCurSel(m_iCurSel-1);
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
				if(hr == S_OK){
					char buf[2] = {0};
					int index;

					buf[0] = (char)kc;
					index = m_items.FindPartial(buf);
					if(index >= 0)
						SetCurSel(index);
				}
			}
		}
		return VARIANT_FALSE;
	}

	STDMETHOD(ShowContextMenu)( 
            /* [in] */ DWORD dwID,
            /* [in] */ DWORD x,
            /* [in] */ DWORD y,
            /* [in] */ IUnknown *pcmdtReserved,
            /* [in] */ IDispatch  *pdispReserved,
            /* [retval][out] */ HRESULT  *dwRetVal)
	{
		T *pT = static_cast<T*>(this);
		CHtmlElement elSrc,elItem;

		if(elSrc.m_sp = pdispReserved){
			HRESULT hr = GetDivFromChild(elSrc.m_sp,&elItem.m_sp);
			if(hr == S_OK){
				long cookie = GetDivCookie(elItem.m_sp);
				int index = m_items.Find(cookie);
				if(index >= 0){
					T *pT = static_cast<T*>(this);
					POINT pt = {x,y};
					pT->OnContextMenu(index,elSrc,pt);
				}
			}
		}
		*dwRetVal = S_OK;
		return S_OK;
	}
};

template <class TItem,class TPred>
class CListBoxItemList : public NO5TL::CPointerSortedArray<TItem,TPred>
{
	typedef NO5TL::CPointerSortedArray<TItem,TPred> _BaseClass;
public:
	typedef TItem item_type;
public:
	int Find(TItem *p)
	{
		return _BaseClass::Find(p);
	}
	// searching methods. return index
	int Find(LPCTSTR text1)
	{
		TItem *it;
		int i;
		int index = -1;

		for(i=0;i<GetSize();i++){
			it = operator[](i);
			if(!it->m_text.CompareNoCase(text1)){
				index = i;
				break;
			}
		}
		return index;
	}
	int Find(LONG cookie)
	{
		TItem *it;
		int i;
		int index = -1;

		for(i=0;i<GetSize();i++){
			it = operator[](i);
			if(cookie == it->m_cookie){
				index = i;
				break;
			}
		}
		return index;
	}
	int FindPartial(LPCTSTR text1)
	{
		TItem *it;
		int i;
		int index = -1;

		for(i=0;i<GetSize();i++){
			it = it = operator[](i);
			if(it->m_text.Find(text1,0) == 0){
				index = i;
				break;
			}
		}
		return index;
	}
};

class CListBoxItem
{
public:
	LONG m_cookie;		// required
	CString m_text;		// required
public:
	CListBoxItem(void)
	{
		m_cookie = -1;
	}
	CListBoxItem(const CListBoxItem &item)
	{
		m_cookie = item.m_cookie;
		m_text = item.m_text;
	}
};


template <class TItem>
struct ItemLess
{
	bool operator () ( TItem *l,TItem *r ) const
	{
		return l->m_text.CompareNoCase(r->m_text) < 0;
	}
};

template <class TItem>
struct NotSorted
{
	bool operator () ( TItem *l,TItem *r ) const
	{
		return false;
	}
};

// basic html list boxes

typedef CListBoxItemList<CListBoxItem,NotSorted<CListBoxItem> > Container1;
typedef CListBoxItemList<CListBoxItem,ItemLess<CListBoxItem> > SortedContainer1;

class CBasicHtmlListBox : public CHtmlListBoxImpl<CBasicHtmlListBox,Container1 >
{
public:
};

class CBasicSortedHtmlListBox : public CHtmlListBoxImpl<CBasicSortedHtmlListBox,SortedContainer1>
{
	//
};

// example of sel expanding html list boxes


class CListBoxItem2 : public CListBoxItem
{
public:
	CString m_SelText;
	CString m_img;
	CString m_SelImg;
public:
	CListBoxItem2(void)
	{
		
	}
	CListBoxItem2(const CListBoxItem2 &item):CListBoxItem(item)
	{
		m_SelText = item.m_SelText;
		m_img = item.m_img;
		m_SelImg = item.m_SelImg;
	}
};

typedef CListBoxItemList<CListBoxItem2,NotSorted<CListBoxItem2> > Container2;
typedef CListBoxItemList<CListBoxItem2,ItemLess<CListBoxItem2> > SortedContainer2;

class CHtmlListBox2 : public CHtmlListBoxImpl<CHtmlListBox2,Container2>
{
public:
	void FormatItemHtml(CListBoxItem2 *pItem,BOOL bSelected,CString &out)
	{
		out.Empty();
		
		if(!bSelected){
			LPCTSTR buf = "<table><tbody><tr><td><img src=\"%s\"></td><td>%s</td></tr></tbody></table>";
			out.Format(buf,(LPCTSTR)pItem->m_img,(LPCTSTR)pItem->m_text);
		}
		else{
			LPCTSTR buf = "<table><tbody><tr><td rowSpan=2><img src=\"%s\"></td><td>%s</td></tr><tr><td>%s</td></tr></tbody></table>";

			out.Format(buf,(LPCTSTR)pItem->m_SelImg,(LPCTSTR)pItem->m_text,(LPCTSTR)pItem->m_SelText);
		}
	}
};

class CSortedHtmlListBox2 : public CHtmlListBoxImpl<CSortedHtmlListBox2,SortedContainer2>
{
public:
	void FormatItemHtml(CListBoxItem2 *pItem,BOOL bSelected,CString &out)
	{
		out.Empty();
		
		if(!bSelected){
			LPCTSTR buf = "<table><tbody><tr><td><img src=\"%s\"></td><td>%s</td></tr></tbody></table>";
			out.Format(buf,(LPCTSTR)pItem->m_img,(LPCTSTR)pItem->m_text);
		}
		else{
			LPCTSTR buf = "<table><tbody><tr><td rowSpan=2><img src=\"%s\"></td><td>%s</td></tr><tr><td>%s</td></tr></tbody></table>";

			out.Format(buf,(LPCTSTR)pItem->m_SelImg,(LPCTSTR)pItem->m_text,(LPCTSTR)pItem->m_SelText);
		}
	}
};

} // NO5TL


#endif // !defined(AFX_HTMLLISTBOX_H__65A43772_4434_11DA_A17D_000103DD18CD__INCLUDED_)
