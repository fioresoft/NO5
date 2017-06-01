// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F7628966_2C65_11DA_A17D_000103DD18CD__INCLUDED_)
#define AFX_STDAFX_H__F7628966_2C65_11DA_A17D_000103DD18CD__INCLUDED_

// Change these values to use different versions
#define WINVER		0x0400
//#define _WIN32_WINNT	0x0400
#define _WIN32_IE	0x0400
#define _RICHEDIT_VER	0x0200
#define HTMLEDITIMPL
#define _WTL_NEW_PAGE_NOTIFY_HANDLERS
//#define _ATL_DEBUG_QI
//#define _ATL_DEBUG_INTERFACES

#include <winsock2.h>
#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;
#include <atlcom.h>
#include <atlhost.h>
#include <atlmisc.h>
#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <no5tl\atlsplit2.h>

//
#include <mshtmdid.h>
#include <exdisp.h> // header for library: SHDocVw  ( web browser control )
#include <exdispid.h> // web browser control's dispids

// tabbing framework
#include "viksoe\atlgdix.h"
#include "tabframe\customtabctrl.h"
#include "tabframe\dotnettabctrl.h"
#include "tabframe\tabbedframe.h"
#include "tabframe\tabbedmdi.h"

// msxml parser
#include <msxml2.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F7628966_2C65_11DA_A17D_000103DD18CD__INCLUDED_)
