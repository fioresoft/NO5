#if !defined(AFX_FONTCOMBO_H__20030528_EF53_EC6F_2CB8_0080AD509054__INCLUDED_)
#define AFX_FONTCOMBO_H__20030528_EF53_EC6F_2CB8_0080AD509054__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// FontComboPicker - Simple Font picker controls
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2003 Bjarke Viksoe.
//
// Add the following macro to the parent's message map:
//   REFLECT_NOTIFICATIONS()
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#ifndef __cplusplus
  #error WTL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLCTRLS_H__
  #error FontCombo.h requires atlctrls.h to be included first
#endif


/////////////////////////////////////////////////////////////////////////////
// 

// Extended Combo styles
#define FPS_EX_TYPEICON   0x00000001
#define FPS_EX_FIXEDBOLD  0x00000002
#define FPS_EX_SHOWFACE   0x00000004

// TrueType icon
__declspec(selectany) BYTE tt_icon[] = 
{
   0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x01,
   0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00,
   0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00,
   0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x80, 0x80,
   0x80, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00,
   0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88,
   0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
   0x88, 0x00, 0x80, 0x00, 0x00, 0x00, 0x08, 0x80, 0x88, 0x08, 0x80, 0x00, 0x00, 0x00, 0x08, 0x88,
   0x88, 0x88, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
   0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFC, 0x0F, 0x00, 0x00, 0xFF, 0x3F,
   0x00, 0x00, 0xFF, 0x3F, 0x00, 0x00, 0xC0, 0x3F, 0x00, 0x00, 0xF3, 0x3F, 0x00, 0x00, 0xF3, 0x37,
   0x00, 0x00, 0xF3, 0x37, 0x00, 0x00, 0xF1, 0x27, 0x00, 0x00, 0xB0, 0x07, 0x00, 0x00, 0xB3, 0x7F,
   0x00, 0x00, 0x92, 0x7F, 0x00, 0x00, 0x80, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00
};

// OpenType icon
__declspec(selectany) BYTE ot_icon[] = 
{
   0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x01,
   0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00,
   0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00,
   0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x80, 0x80,
   0x80, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00,
   0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
   0x07, 0x77, 0x67, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x76, 0x70, 0x00, 0x00, 0x00, 0x70,
   0x00, 0x00, 0x08, 0x67, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x07, 0x66, 0x00, 0x00, 0x00, 0x70,
   0x07, 0x00, 0x00, 0x66, 0x70, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x66, 0x70, 0x00, 0x00, 0x08,
   0x00, 0x00, 0x00, 0x86, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86, 0x70, 0x00, 0x00, 0x00,
   0x70, 0x00, 0x00, 0x86, 0x70, 0x00, 0x00, 0x00, 0x07, 0x08, 0x77, 0x66, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x07, 0x86, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
   0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xF8, 0xFF, 0x00, 0x00, 0xE0, 0x3F,
   0x00, 0x00, 0xC7, 0x1F, 0x00, 0x00, 0xC7, 0x8F, 0x00, 0x00, 0xC7, 0x8F, 0x00, 0x00, 0xC3, 0xC7,
   0x00, 0x00, 0xE3, 0xC7, 0x00, 0x00, 0xE3, 0xC7, 0x00, 0x00, 0xF3, 0xC7, 0x00, 0x00, 0xF1, 0xC7,
   0x00, 0x00, 0xF8, 0x0F, 0x00, 0x00, 0xFE, 0x1F, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 
};


template< class T, class TBase = CComboBox, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CFontPickerImpl : 
   public CWindowImpl< T, TBase, TWinTraits >,
   public COwnerDraw< T >
{
public:
   DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

   enum 
   {
      s_cxIndent = 3,
      TMPF_OPENTYPE = 0x80,
   };

   CSimpleArray<LOGFONT> m_aFonts;
   CSimpleArray<TEXTMETRIC> m_aMetrics;
   HICON m_hTtIcon;
   HICON m_hOtIcon;
   CFont m_fntBold;
   DWORD m_dwExtStyle;
   // EnumFontFamilies data
   DWORD m_dwFilter;
   TEXTMETRIC m_tm;

   // Operations

   BOOL SubclassWindow(HWND hWnd)
   {
      ATLASSERT(m_hWnd==NULL);
      ATLASSERT(::IsWindow(hWnd));
      BOOL bRet = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);
      if( bRet ) _Init();
      return bRet;
   }
   DWORD SetExtendedFontStyle(DWORD dwStyle)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      ATLASSERT(GetCount()==0); // Before adding items, please!
      DWORD dwOldStyle = m_dwExtStyle;
      m_dwExtStyle = dwStyle;
      Invalidate();
      return dwOldStyle;
   }

   int Dir(DWORD dwType = -1, LPCTSTR pstrFamily = NULL)
   {
      m_aFonts.RemoveAll();
      m_aMetrics.RemoveAll();
      CClientDC dc = m_hWnd;      
      dc.GetTextMetrics(&m_tm);
      m_dwFilter = dwType;
      ::EnumFontFamilies(dc, pstrFamily, _EnumFamCB, (LPARAM) this);
      for( int i = 0; i < m_aFonts.GetSize(); i++ ) {
         int iItem = AddString(m_aFonts[i].lfFaceName);
         // NOTE: Because of possible sorting of the list we store
         //       the index in the item-data.
         SetItemData(iItem, (LPARAM) i);
      }
      return GetCount() - 1;
   }
   BOOL GetLogFont(int iIndex, LOGFONT& lf) const
   {
      if( iIndex < 0 || iIndex >= GetCount() ) return FALSE;
      lf = m_aFonts[GetItemData(iIndex)];
      return TRUE;
   }
   BOOL GetTextMetrics(int iIndex, TEXTMETRIC& tm) const
   {
      if( iIndex < 0 || iIndex >= GetCount() ) return FALSE;
      tm = m_aMetrics[GetItemData(iIndex)];
      return TRUE;
   }

   // Callbacks

   static BOOL CALLBACK _EnumFamCB(CONST LOGFONT* lplf, CONST TEXTMETRIC* lptm, DWORD FontType, LPARAM pThis) 
   {
      T* pT = (T*) pThis;
      CClientDC dc = pT->m_hWnd;
      if( (FontType & pT->m_dwFilter) == 0 ) return TRUE;
      for( int i = 0; i < pT->m_aFonts.GetSize(); i++ ) {
         if( ::lstrcmp(lplf->lfFaceName, pT->m_aFonts[i].lfFaceName) == 0 ) return TRUE;
      }
      LOGFONT lf = *lplf;
      lf.lfWidth = 0;
      lf.lfHeight = pT->m_tm.tmHeight;
      pT->m_aFonts.Add(lf);
      TEXTMETRIC tm = *lptm;
      if( tm.tmPitchAndFamily & TMPF_TRUETYPE ) {
         // HACK: A little hack to determine if this is an OpenType...
         const NEWTEXTMETRIC* lpntm = (const NEWTEXTMETRIC*) lptm;
         if( lpntm->ntmFlags & 0x60000 ) tm.tmPitchAndFamily |= TMPF_OPENTYPE;
      }
      pT->m_aMetrics.Add(tm);
      return TRUE;
   }

   // Implementation

   void _Init()
   {
      ATLASSERT(::IsWindow(m_hWnd));
      // Need to set these in resource editor
      // If it's a ListBox, use LBS_OWNERDRAWVARIABLE.
      ATLASSERT(GetStyle() & CBS_OWNERDRAWVARIABLE);
      ATLASSERT(GetStyle() & CBS_HASSTRINGS);

      m_dwExtStyle = 0;

      CFontHandle font = GetFont();
	  // changed by NO5
	  if(font.IsNull())
		  font = (HFONT)::GetStockObject(SYSTEM_FONT);
	  //
      LOGFONT lf;
      font.GetLogFont(&lf);
      lf.lfWeight += FW_BOLD;
      if( !m_fntBold.IsNull() ) m_fntBold.DeleteObject();
      m_fntBold.CreateFontIndirect(&lf);

      const DWORD ICO_OFFSET = 6 + 16; // sizeof(ICONDIR) + sizeof(ICONDIRENTRY)
      m_hTtIcon = ::CreateIconFromResource(tt_icon + ICO_OFFSET, sizeof(tt_icon) - ICO_OFFSET, TRUE, 0x00030000);
      ATLASSERT(m_hTtIcon!=NULL);
      m_hOtIcon = ::CreateIconFromResource(ot_icon + ICO_OFFSET, sizeof(ot_icon) - ICO_OFFSET, TRUE, 0x00030000);
      ATLASSERT(m_hOtIcon!=NULL);
   }

   // Message map and handlers

   BEGIN_MSG_MAP(CFontPickerImpl)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      CHAIN_MSG_MAP_ALT( COwnerDraw< T >, 1 )
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();
      _Init();
      return lRes;
   }
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      ::DestroyIcon(m_hTtIcon);
      ::DestroyIcon(m_hOtIcon);
      bHandled = FALSE;
      return 0;
   }

   // COwnerDraw

   void MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
   {
	   // changed by me
	   int iIndex = lpMIS->itemID;
	   if( iIndex == -1 ) return;
	   //
	   CClientDC dc(m_hWnd);
	   TEXTMETRIC tm = m_aMetrics[GetItemData(lpMIS->itemID)]; 
	   if( m_dwExtStyle & FPS_EX_SHOWFACE ) {
		// Already initialized 'tm'...
	   }
	   else if( m_dwExtStyle & FPS_EX_FIXEDBOLD && ((tm.tmPitchAndFamily & 3) == FIXED_PITCH) ) {
		 HFONT hOldFont = dc.SelectFont(m_fntBold);
		 dc.GetTextMetrics(&tm);
		 dc.SelectFont(hOldFont);
	   }
	   else {
		 dc.GetTextMetrics(&tm);
	   }
	   lpMIS->itemHeight = tm.tmHeight + 1;
   }

   void DrawItem(LPDRAWITEMSTRUCT lpDIS)
   {
      int iIndex = lpDIS->itemID;
      if( iIndex == -1 ) return;
      LOGFONT& lf = m_aFonts[GetItemData(iIndex)];
      TEXTMETRIC& tm = m_aMetrics[GetItemData(iIndex)];

      CDCHandle dc = lpDIS->hDC;
      RECT rc = lpDIS->rcItem;
      bool bSelected = lpDIS->itemState & ODS_SELECTED;

      // Never paint selected colors for edit part
      if( lpDIS->itemState & ODS_COMBOBOXEDIT ) bSelected = false;

      // Fill background and Font item
      CBrushHandle brBack = ::GetSysColorBrush(bSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW);
      dc.FillRect(&rc, brBack);

      ::InflateRect(&rc, -s_cxIndent, 0);

      if( m_dwExtStyle & FPS_EX_TYPEICON ) 
      {
         if( tm.tmPitchAndFamily & TMPF_OPENTYPE ) {
            dc.DrawIconEx(rc.left, rc.top + 1, m_hOtIcon, 16, 16);
         }
         else if( tm.tmPitchAndFamily & TMPF_TRUETYPE ) {
            dc.DrawIconEx(rc.left, rc.top + 1, m_hTtIcon, 16, 16);
         }
         rc.left += 18;
      }

      dc.SetBkMode(TRANSPARENT);
      dc.SetTextColor(::GetSysColor(bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
      CFont font;
      HFONT hOldFont = NULL;
      if( m_dwExtStyle & FPS_EX_SHOWFACE ) 
      {
         font.CreateFontIndirect(&lf);
         hOldFont = dc.SelectFont(font);
      }
      else if( (m_dwExtStyle & FPS_EX_FIXEDBOLD) != 0 && 
               (tm.tmPitchAndFamily & TMPF_FIXED_PITCH ) == 0 ) 
      {
         hOldFont = dc.SelectFont(m_fntBold);
      }
      dc.DrawText(lf.lfFaceName, -1, &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX | DT_EDITCONTROL | DT_END_ELLIPSIS);
      if( hOldFont ) dc.SelectFont(hOldFont);
   }
};

class CFontPickerComboCtrl : public CFontPickerImpl<CFontPickerComboCtrl, CComboBox, CWinTraitsOR<CBS_OWNERDRAWVARIABLE|CBS_DROPDOWNLIST|CBS_HASSTRINGS> >
{
public:
   DECLARE_WND_SUPERCLASS(_T("WTL_FontPickerComboBox"), GetWndClassName())  
};

class CFontPickerListCtrl : public CFontPickerImpl<CFontPickerListCtrl, CListBox, CWinTraitsOR<LBS_OWNERDRAWVARIABLE|LBS_HASSTRINGS> >
{
public:
   DECLARE_WND_SUPERCLASS(_T("WTL_FontPickerListBox"), GetWndClassName())  
};


#endif // !defined(AFX_FONTCOMBO_H__20030528_EF53_EC6F_2CB8_0080AD509054__INCLUDED_)
