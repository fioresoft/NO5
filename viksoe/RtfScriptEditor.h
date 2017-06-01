#if !defined(AFX_RTFSCRIPTEDITOR_H__20030214_0503_2762_07DF_0080AD509054__INCLUDED_)
#define AFX_RTFSCRIPTEDITOR_H__20030214_0503_2762_07DF_0080AD509054__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// RtfScriptEditorCtrl - A syntax highlighting RTF control
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Enhancement of a control by Juraj Rojko (jrojko@twist.cz)
//
// Add the following macro to the parent's message map:
//   REFLECT_NOTIFICATIONS()
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
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
  #error RtfScriptEditor.h requires atlctrls.h to be included first
#endif


/////////////////////////////////////////////////////////////////////////////
// 

typedef enum
{
   SC_NORMAL,
   SC_KEYWORD,
   SC_COMMENT,
   SC_NUMBER,
   SC_STRING,
   SC_CONSTANT,
   //
   SC_LAST,
};

typedef struct
{
   COLORREF clrText;
   BOOL bBold;
   BOOL bUnderline;
} SYNTAXCOLOR;


template< class T, class TBase = CRichEditCtrl, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CRtfScriptEditorImpl : 
   public CWindowImpl< T, TBase, TWinTraits >
{
public:
   DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

   struct KEYWORD
   {
      CHAR szName[16];
      KEYWORD(LPCSTR pstrText)
      {
         ATLASSERT(::lstrlen(pstrText)<sizeof(szName)/sizeof(CHAR));
         ::lstrcpyA(szName, pstrText);
      }
      bool operator==(LPCSTR pstrText) const
      {
         return ::lstrcmpA(szName, pstrText) == 0;
      }
   };

   struct CProtectSelection : CHARRANGE
   {
      CRtfScriptEditorImpl* m_pT;
      CProtectSelection(CRtfScriptEditorImpl* pT) : m_pT(pT)
      {
         m_pT->GetSel(*this);
         m_pT->LockWindowUpdate();
         m_pT->HideSelection(TRUE, FALSE);
         m_pT->m_bInForcedChange = true;
      }
      ~CProtectSelection()
      {
         m_pT->SetSel(*this);
         m_pT->HideSelection(FALSE, FALSE);
         m_pT->LockWindowUpdate(NULL);
         m_pT->m_bInForcedChange = false;
      }
   };

   enum
   {
      CT_UNKNOWN,
      CT_UNDO,
      CT_REPLSEL,
      CT_DELETE,
      CT_BACK,
      CT_CUT,
      CT_PASTE,
      CT_MOVE,
      CT_SKIP,
   } m_ChangeType;

   SYNTAXCOLOR m_aColors[SC_LAST];
   CSimpleArray<KEYWORD> m_aKeywords;
   CSimpleArray<KEYWORD> m_aConstants;
   CHAR m_szCommentSL[6];
   CHAR m_szCommentMLs[6];
   CHAR m_szCommentMLe[6];
   CHAR m_chQuote;
   BOOL m_bCaseSensitive;
   BOOL m_bChangeCase;
   BOOL m_bDelayedFormat;
   //
   CHARRANGE m_crOldSel;
   bool m_bInForcedChange;

   // Operations

   BOOL SubclassWindow(HWND hWnd)
   {
      ATLASSERT(m_hWnd==NULL);
      ATLASSERT(::IsWindow(hWnd));
      BOOL bRet = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);
      if( bRet ) _Init();
      return bRet;
   }

   void SetCaseSensitive(BOOL bSensitive)
   {
      m_bCaseSensitive = bSensitive;
   }
   void SetChangeCase(BOOL bForceCase)
   {
      m_bChangeCase = bForceCase;
   }
   void SetDelayedFormat(BOOL bDelayedFormat)
   {
      m_bDelayedFormat = bDelayedFormat;
   }
   BOOL SetSyntaxColor(int iType, const SYNTAXCOLOR ic)
   {
      ATLASSERT(iType>=0 && iType<SC_LAST);
      if( iType < 0 || iType >= SC_LAST ) return FALSE;
      m_aColors[iType] = ic;
      return TRUE;
   }
   BOOL GetSyntaxColor(int iType, SYNTAXCOLOR& ic) const
   {
      ATLASSERT(iType>=0 && iType<SC_LAST);
      if( iType < 0 || iType >= SC_LAST ) return FALSE;
      ic = m_aColors[iType];
      return TRUE;
   }
   void ClearKeywords()
   {
      m_aKeywords.RemoveAll();
   }
   BOOL AddKeyword(LPCSTR pstrText)
   {
      ATLASSERT(!::IsBadStringPtr(pstrText,-1));
      KEYWORD k = pstrText;
      return m_aKeywords.Add(k);
   }
   void ClearConstants()
   {
      m_aConstants.RemoveAll();
   }
   BOOL AddConstant(LPCSTR pstrText)
   {
      ATLASSERT(!::IsBadStringPtr(pstrText,-1));
      KEYWORD k = pstrText;
      return m_aConstants.Add(k);
   }
   BOOL SetSyntax(int iType, LPCSTR pstrText)
   {
      switch( iType ) {
      case SC_COMMENT:
         ::lstrcpynA(m_szCommentSL, pstrText, 5);
         return TRUE;
      case SC_STRING:
         m_chQuote = pstrText[0];
         return TRUE;
      default:
         return FALSE;
      }
   }

   BOOL FormatAll()
   {
      ATLASSERT(::IsWindow(m_hWnd));
      _FormatTextRange(0, GetTextLength());
      Invalidate();
      return TRUE;
   }

   // Implementation

   void _Init()
   {
      ATLASSERT(::IsWindow(m_hWnd));
      ATLASSERT(::GetModuleHandle(TBase::GetLibraryName())!=NULL); // Forgot to load RTF library?

      SetFont((HFONT)::GetStockObject(ANSI_FIXED_FONT));
      
      // We want to listen to these events
      SetEventMask(ENM_CHANGE | ENM_SELCHANGE | ENM_PROTECTED);

      // Configure tab settings
      PARAFORMAT2 pf;
      pf.cbSize = sizeof(PARAFORMAT2);
      pf.dwMask = PFM_TABSTOPS ;
      pf.cTabCount = MAX_TAB_STOPS;
      for( int iTab = 0; iTab < pf.cTabCount; iTab++ ) pf.rgxTabs[iTab] = (iTab + 1) * 1440 / 5;
      SetParaFormat(pf);

      // Set default font and size
      CHARFORMAT cfDefault;
      cfDefault.cbSize = sizeof(cfDefault);
      cfDefault.dwEffects = CFE_PROTECTED; 
      cfDefault.dwMask = CFM_BOLD | CFM_FACE | CFM_SIZE | CFM_CHARSET | CFM_PROTECTED;
      cfDefault.yHeight = 200;
      cfDefault.bCharSet = EASTEUROPE_CHARSET; // 0xEE
      ::lstrcpyA(cfDefault.szFaceName, "Courier New");
      SetDefaultCharFormat(cfDefault);

      // Set default configuration
      m_ChangeType = CT_UNKNOWN;      
      ::ZeroMemory(&m_crOldSel, sizeof(m_crOldSel));
      m_bInForcedChange = false;
      //
      ::ZeroMemory(m_aColors, sizeof(m_aColors));
      ::lstrcpyA(m_szCommentSL, "//");
      ::lstrcpyA(m_szCommentMLs, "/*");
      ::lstrcpyA(m_szCommentMLe, "*/");
      m_chQuote = '\"';
      m_bCaseSensitive = TRUE;
      m_bChangeCase = FALSE;
      m_bDelayedFormat = TRUE;
   }

   int _FindKeyword(LPCSTR pstrText) const
   {
      ATLASSERT(!::IsBadStringPtr(pstrText,-1));
      for( int i = 0; i < m_aKeywords.GetSize(); i++ ) {
         if( m_bCaseSensitive ) {
            if( m_aKeywords[i] == pstrText ) return i;
         }
         else {
            if( ::lstrcmpiA(m_aKeywords[i].szName, pstrText) == 0 ) return i;
         }
      }
      return -1;
   }
   int _FindConstant(LPCSTR pstrText) const
   {
      ATLASSERT(!::IsBadStringPtr(pstrText,-1));
      for( int i = 0; i < m_aConstants.GetSize(); i++ ) {
         if( m_bCaseSensitive ) {
            if( m_aConstants[i] == pstrText ) return i;
         }
         else {
            if( ::lstrcmpiA(m_aConstants[i].szName, pstrText) == 0 ) return i;
         }
      }
      return -1;
   }
   inline bool _IsStringQuote(CHAR ch) const
   {
      return ch == m_chQuote;
   }

   BOOL _SetFormatRange(int nStart, int nEnd, const SYNTAXCOLOR& ic)
   {
      if( nStart >= nEnd ) return FALSE;

      SetSel(nStart, nEnd);

      DWORD dwEffects = 0;
      if( ic.bBold ) dwEffects |= CFE_BOLD;
      if( ic.bUnderline ) dwEffects |= CFE_UNDERLINE;

      CHARFORMAT cfm;
      cfm.cbSize = sizeof(cfm);
      GetSelectionCharFormat(cfm);   
      if( (cfm.dwMask & CFM_COLOR) && cfm.crTextColor == ic.clrText && 
          (cfm.dwMask & CFM_BOLD) && (cfm.dwEffects & CFE_BOLD) == (dwEffects & CFE_BOLD) &&
          (cfm.dwMask & CFM_UNDERLINE) && (cfm.dwEffects & CFE_BOLD) == (dwEffects & CFE_UNDERLINE) )
      {
         return FALSE;
      }
      cfm.dwEffects = dwEffects;
      cfm.crTextColor = ic.clrText;
      cfm.dwMask = CFM_UNDERLINE | CFM_BOLD | CFM_COLOR;
      return SetSelectionCharFormat(cfm);
   }

   void _ChangeCase(int nStart, int nEnd, LPCSTR pstrText)
   {
      ATLASSERT(nEnd-nStart==(int)::lstrlen(pstrText));
      if( !m_bCaseSensitive && m_bChangeCase ) {
         USES_CONVERSION;
         SetSel(nStart, nEnd);
         ReplaceSel(A2T(const_cast<LPSTR>(pstrText)));
      }
   }

   void _FormatTextRange(int nStart, int nEnd)
   {
      if( nStart >= nEnd ) return;

      CProtectSelection crOldSel(this);

      SetSel(nStart, nEnd);

      LPSTR pBuffer = NULL;
      ATLTRY(pBuffer = (LPSTR) malloc((nEnd - nStart + 1) * sizeof(CHAR)));
      if( pBuffer == NULL ) return; // Urgh, out of memory!
      long nLen = GetSelText(pBuffer);
      ATLASSERT(nLen<=nEnd-nStart);
      pBuffer[nLen] = '\0';

      LPCSTR pStart = pBuffer;
      LPSTR pPtr = pBuffer;
      LPSTR pSymbolStart = NULL;
      SYNTAXCOLOR ic = { 0 };

      while( *pPtr ) {
         CHAR ch = *pPtr;
         if( ch == m_szCommentSL[0] && 
             (m_szCommentSL[1] == '\0' || pPtr[1] == m_szCommentSL[1]) ) 
         {
            pSymbolStart = pPtr;
            do 
            {
               pPtr = ::CharNextA(pPtr);
               ch = *pPtr;
            } 
            while( ch != '\0' && ch != '\n' );
            ic = m_aColors[SC_COMMENT];
         } 
         else if( _IsStringQuote(ch) ) 
         { 
            // Process strings
            pSymbolStart = pPtr;
            CHAR chQuote = ch;
            do 
            {
               pPtr = ::CharNextA(pPtr);
               ch = *pPtr;
            } 
            while( ch != '\0' && ch != chQuote && ch != '\n' );
            if( ch == chQuote ) pPtr = ::CharNextA(pPtr);
            ic = m_aColors[SC_STRING];
         } 
         else if( ch >= '0' && ch <= '9' ) 
         { 
            // Process numbers
            pSymbolStart = pPtr;
            while( *pPtr >= '0' && *pPtr <= '9' ) pPtr = ::CharNextA(pPtr);
            ic = m_aColors[SC_NUMBER];
         } 
         else if( ::IsCharAlphaNumeric(ch) || ch == '_' ) 
         { 
            // Process keywords
            pSymbolStart = pPtr;
            do 
            {
               pPtr = ::CharNextA(pPtr);
               ch = *pPtr;
            } 
            while( ::IsCharAlphaNumeric(ch) || ch == '_' );
            *pPtr = '\0';
            int nPos = _FindKeyword(pSymbolStart);
            if( nPos >= 0 ) {
               _ChangeCase(nStart + pSymbolStart - pBuffer, nStart + pPtr - pBuffer, 
                           m_aKeywords[nPos].szName);
               /*
               if( ::lstrcmpiA(m_szCommentSL, pSymbolStart) == 0 ) {
                  *pPtr = ch;
                  *pSymbolStart = m_szCommentSL[0];
                  if( pSymbolStart[1] != '\0' && m_szCommentSL[1] != '\0' ) {
                     pSymbolStart[1] = m_szCommentSL[1];
                  }
                  pPtr = pSymbolStart;
                  pSymbolStart = NULL;
                  continue;
               }
               */
               ic = m_aColors[SC_KEYWORD];
            } 
            else 
            {
               nPos = _FindConstant(pSymbolStart);
               if( nPos >= 0 ) {
                  _ChangeCase(nStart + pSymbolStart - pBuffer, nStart + pPtr - pBuffer, 
                              m_aConstants[nPos].szName);
                  ic = m_aColors[SC_CONSTANT];
               } 
               else 
               {
                  pSymbolStart = NULL;
               }
            }
            *pPtr = ch;
         } 
         else 
         {
            pPtr = ::CharNextA(pPtr);
         }

         if( pSymbolStart ) {
            ATLASSERT(pSymbolStart<pPtr);
            _SetFormatRange(nStart + pStart - pBuffer, nStart + pSymbolStart - pBuffer, m_aColors[SC_NORMAL]);
            _SetFormatRange(nStart + pSymbolStart - pBuffer, nStart + pPtr - pBuffer, ic);
            pStart = pPtr;
            pSymbolStart = NULL;
         } 
         else if( *pPtr == '\0' ) {
            _SetFormatRange(nStart + pStart - pBuffer, nStart + pPtr - pBuffer, m_aColors[SC_NORMAL]);
         }
      }

      free(pBuffer);
   }

   void _FormatTextLines(int nLineStart, int nLineEnd)
   {
      int nStart = LineIndex(LineFromChar(nLineStart));
      int nEnd = LineIndex(LineFromChar(nLineEnd));
      nEnd += LineLength(nLineEnd);
      _FormatTextRange(nStart, nEnd);
   }
   void _FormatTextLinesPlain(int nLineStart, int nLineEnd)
   {
      int nStart = LineIndex(LineFromChar(nLineStart));
      int nEnd = LineIndex(LineFromChar(nLineEnd));
      nEnd += LineLength(nLineEnd);
      if( nStart < nEnd ) {
         CProtectSelection crOldSel(this);
         _SetFormatRange(nStart, nEnd, m_aColors[SC_NORMAL]);
      }
   }

   // Message map and handlers

   BEGIN_MSG_MAP(CRtfScriptEditorImpl)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SETTEXT, OnSetText)
      MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
      REFLECTED_COMMAND_CODE_HANDLER(EN_CHANGE, OnChange)
      REFLECTED_NOTIFY_CODE_HANDLER(EN_PROTECTED, OnProtected)
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();
      _Init();
      return lRes;
   }
   LRESULT OnSetText(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();
      FormatAll();
      return lRes;
   }
   LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      if( !m_bDelayedFormat ) {
         bHandled = FALSE;
         return 0;
      }

      CHARRANGE crCurSel;
      GetSel(crCurSel);
      LRESULT lRes = DefWindowProc();
      switch( wParam ) {
      case VK_RETURN:
      case VK_UP:
      case VK_DOWN:
      case VK_PRIOR:
      case VK_NEXT:
      case VK_HOME:
      case VK_END:
         {
            CHARRANGE crOldSel;
            GetSel(crOldSel);
            _FormatTextLines(crCurSel.cpMin, crCurSel.cpMax);
            SetSel(crOldSel);
         }
         break;
      }
      return lRes;
   }
   LRESULT OnChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      if( m_bInForcedChange ) return 0;

      CHARRANGE crCurSel;
      GetSel(crCurSel);

      if( m_ChangeType == CT_MOVE && crCurSel.cpMin == crCurSel.cpMax ) {
         // Cut was canceled, so this is paste operation
         m_ChangeType = CT_PASTE;
      }

      switch( m_ChangeType ) {
      case CT_REPLSEL: // old=(x,y) -> cur=(x+len,x+len)
         if( m_bDelayedFormat ) {
            _FormatTextLinesPlain(m_crOldSel.cpMin, crCurSel.cpMax);
         }
         else {
            _FormatTextLines(m_crOldSel.cpMin, crCurSel.cpMax);
         }
         break;
      case CT_PASTE:   // old=(x,y) -> cur=(x+len,x+len)
         _FormatTextLines(m_crOldSel.cpMin, crCurSel.cpMax);
         break;
      case CT_DELETE:  // old=(x,y) -> cur=(x,x)
      case CT_BACK:    // old=(x,y) -> cur=(x,x), newline del => old=(x,x+1) -> cur=(x-1,x-1)
         if( m_bDelayedFormat ) {
            _FormatTextLinesPlain(crCurSel.cpMin, crCurSel.cpMax);
         }
         else {
            _FormatTextLines(crCurSel.cpMin, crCurSel.cpMax);
         }
         break;
      case CT_CUT:     // old=(x,y) -> cur=(x,x)
         _FormatTextLines(crCurSel.cpMin, crCurSel.cpMax);
         break;
      case CT_UNDO:    // old=(?,?) -> cur=(x,y)
         _FormatTextLines(crCurSel.cpMin, crCurSel.cpMax);
         break;
      case CT_MOVE:    // old=(x,x+len) -> cur=(y-len,y) | cur=(y,y+len)
         _FormatTextLines(crCurSel.cpMin, crCurSel.cpMax);
         if( crCurSel.cpMin > m_crOldSel.cpMin ) { // move after
            _FormatTextLines(m_crOldSel.cpMin, m_crOldSel.cpMin);
         }
         else {        // move before
            _FormatTextLines(m_crOldSel.cpMax, m_crOldSel.cpMax);
         }
         break;
      case CT_SKIP:
         break;
      default:
         FormatAll();
         break;
      }

      // Undo action does not call OnProtected, so make it default...
      m_ChangeType = CT_UNDO;
      return 0;
   }
   LRESULT OnProtected(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
   {
      ENPROTECTED* pEP = (ENPROTECTED*) pnmh;
      // Determine type of change will occur...
      switch( pEP->msg ) {
      case WM_KEYDOWN:
         switch( pEP->wParam ) {
         case VK_DELETE:
            m_ChangeType = CT_DELETE;
            break;
         case VK_BACK:
            m_ChangeType = CT_BACK;
            break;
         default:
            m_ChangeType = CT_UNKNOWN;
            break;
         }
         break;
      case EM_REPLACESEL:
      case WM_CHAR:
         m_ChangeType = m_bDelayedFormat && pEP->wParam == VK_RETURN ? CT_SKIP : CT_REPLSEL;
         break;
      case WM_PASTE:
         m_ChangeType = m_ChangeType == CT_CUT ? CT_MOVE : CT_PASTE;
         break;
      case WM_CUT:
         m_ChangeType = CT_CUT;
         break;
      case EM_SETCHARFORMAT:
         // Ignore this
         break;
      default:
         m_ChangeType = CT_UNKNOWN;
         break;
      }

      if( pEP->msg != EM_SETCHARFORMAT && m_ChangeType != CT_MOVE ) m_crOldSel = pEP->chrg;
      return 0; // Allow operation!
   }
};

class CRtfScriptEditorCtrl : public CRtfScriptEditorImpl<CRtfScriptEditorCtrl>
{
public:
   DECLARE_WND_SUPERCLASS(_T("WTL_RtfScriptEditor"), GetWndClassName())  
};


#endif // !defined(AFX_RTFSCRIPTEDITOR_H__20030214_0503_2762_07DF_0080AD509054__INCLUDED_)

