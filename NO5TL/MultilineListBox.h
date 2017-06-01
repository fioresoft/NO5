// MultilineListBox.h: interface for the CMultilineListBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTILINELISTBOX_H__EF54EE45_380A_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_MULTILINELISTBOX_H__EF54EE45_380A_11DA_A17D_000103DD18CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mystring.h"

namespace NO5TL
{

// TODO:	check that the list was created with LBS_HASSTRINGS|LBS_OWNERDRAWVARIABLE
//			iden for attach
//			hide invalid methods
//			support vertical image padding	
template <class T,class TBase = CListBox,class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CMultilineListBoxImpl : \
	public CWindowImpl< T , TBase , TWinTraits >,
	public COwnerDraw< T >
{
	CImageList m_il;
	CSize m_sz;

	enum
	{
		IMG_PADDING = 10, // space on left and right of the image
	};

public:
	struct ItemData
	{
		int m_iImage;
		int m_iSelImage;

		ItemData(int iImage = -1,int iSelImage = -1)
		{
			m_iImage = iImage;
			m_iSelImage = iSelImage;
		}
	};

public:
	DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

	CMultilineListBoxImpl():m_sz(0,0)
	{
		
	}


	BEGIN_MSG_MAP(CMultilineListBoxImpl)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_SETFONT,OnSetFont)
		CHAIN_MSG_MAP_ALT(COwnerDraw< T > , 1)
		COMMAND_CODE_HANDLER(LBN_SELCHANGE,OnSelChange)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	HIMAGELIST SetImageList(HIMAGELIST hImageList)
	{
		HIMAGELIST res = m_il.Detach();
		m_il.Attach(hImageList);
		if(!m_il.IsNull()){
			m_il.GetIconSize(m_sz);
		}
		else
			m_sz.cx = m_sz.cy = 0;
		return res;
	}

	int AddString(LPCTSTR txt,int iImage = -1,int iSelImage = -1)
	{
		ItemData *p = new ItemData(iImage,iSelImage);
		int index = TBase::AddString(txt);
		SetItemData(index,(LPARAM)p);
		return index;
	}
		
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CRect r;

		DefWindowProc(uMsg,wParam,lParam);
		GetWindowRect(&r);
		SetWindowPos(NULL,0,0,r.Width(),r.Height(),SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOMOVE| \
			SWP_NOOWNERZORDER|SWP_NOZORDER);
		return 0;
	}
	LRESULT OnSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	void DrawItem(LPDRAWITEMSTRUCT lpdis)
	{
		int index = (int) lpdis->itemID;

		if(lpdis->CtlType == ODT_LISTBOX && index >= 0){
			CDCHandle dc = lpdis->hDC;
			CString s;
			CStringBuffer buf(s,GetTextLen(index) + 1);
			CRect r = lpdis->rcItem;
			bool bSelected = lpdis->itemState & ODS_SELECTED;
			COLORREF clrBack = bSelected ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor(COLOR_WINDOW);
			COLORREF clrTxt = bSelected ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_WINDOWTEXT);
			int iMode;
					
			
			DrawTheBitmap(dc,r,index,bSelected);
			GetText(index,buf);
			
			dc.FillSolidRect(&r,clrBack);
			iMode = dc.SetBkMode(OPAQUE);
			clrBack = dc.SetBkColor(clrBack);
			clrTxt = dc.SetTextColor(clrTxt);
			dc.DrawText(buf,-1,&r,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS);
			dc.SetBkColor(clrBack);
			dc.SetTextColor(clrTxt);
			dc.SetBkMode(iMode);
		}
	}

	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
	{
		int index = (int) lpMeasureItemStruct->itemID;

		if(lpMeasureItemStruct->CtlType != ODT_MENU && index >= 0)
		{
			CClientDC dc(m_hWnd);
			CFontHandle font = GetFont();
			CString s;
			CStringBuffer buf(s,GetTextLen(index) + 1);
			CRect r(0,0,lpMeasureItemStruct->itemWidth,lpMeasureItemStruct->itemHeight);

			GetText(index,buf);
			font = dc.SelectFont(font);
			// this does not draw the text

			r.left += (m_sz.cx + 2 * IMG_PADDING);
			lpMeasureItemStruct->itemHeight =  dc.DrawText(buf,-1,&r,
				DT_CALCRECT|DT_LEFT|DT_WORDBREAK);

			if(m_sz.cy > (LONG)lpMeasureItemStruct->itemHeight)
				lpMeasureItemStruct->itemHeight = m_sz.cy;

			dc.SelectFont(font);
			
		}
		else
			lpMeasureItemStruct->itemHeight = ::GetSystemMetrics(SM_CYMENU);
	}
	void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
	{
		if(lpDeleteItemStruct->CtlID == (UINT)GetDlgCtrlID()){
			ItemData *p = (ItemData *)lpDeleteItemStruct->itemData;
			delete p;
		}
	}
protected:
	void DrawTheBitmap(HDC hdc,CRect &r,int iItem,bool bSelected)
	{
		if(!m_il.IsNull()){
			ItemData *p = (ItemData *)GetItemData(iItem);
			int iImage = bSelected ? p->m_iSelImage : p->m_iImage;
			m_il.Draw(hdc,iImage,r.left + IMG_PADDING,r.top + r.Height()/2 - m_sz.cy/2,ILD_NORMAL);
			r.left += (m_sz.cx + 2 * IMG_PADDING);
		}
	}

};

class CMultiLineListBox : public CMultilineListBoxImpl<CMultiLineListBox>
{
public:
	DECLARE_WND_SUPERCLASS(_T("NO5MLListBox"), GetWndClassName());
};

} // NO5TL

#endif // !defined(AFX_MULTILINELISTBOX_H__EF54EE45_380A_11DA_A17D_000103DD18CD__INCLUDED_)
