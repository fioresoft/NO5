# pragma once

#include <exdispid.h>
#include <exdisp.h>

namespace NO5TL
{

enum EVENT_SOURCES
{
	SOURCE_BROWSER = ATL_IDW_CLIENT,
};

const UINT LIB_VER_MAJ = 1;
const UINT LIB_VER_MIN = 0;


#define MY_SINK_ENTRY(dispid,fun) \
	SINK_ENTRY_EX(SOURCE_BROWSER,DIID_DWebBrowserEvents2,dispid,fun)

struct IWebBrowserEvents
{
	void OnStatusChange(BSTR Text)
	{
		
	}
	void OnProgressChange(long Progress,long ProgressMax)
	{
		
	}
	void OnCmdStateChange(long Cmd,VARIANT_BOOL Enable)
	{
		
	}
	void OnDownloadBegin(void)
	{
		
	}
	void OnDownloadComplete(void)
	{
		
	}
	void OnTitleChange(BSTR Text)
	{
		
	}
	void OnPropertyChange(BSTR Property)
	{
		
	}
	void OnBeforeNavigate(IDispatch* pDisp,VARIANT * URL,
		VARIANT * Flags,VARIANT * TargetFrameName,VARIANT * PostData,
		VARIANT * Headers,/*[in,out]*/ VARIANT_BOOL * Cancel)
	{
		
	}
	void OnNewWindow(/*[in, out]*/ IDispatch** ppDisp,
		/*[in, out]*/ VARIANT_BOOL * Cancel)
	{
		
	}
	void OnNavigateComplete(IDispatch* pDisp,
		VARIANT * URL )
	{
		
	}
	void OnDocumentComplete(IDispatch* pDisp,
		VARIANT * URL )
	{
		
	}
	void OnQuit(void)
	{
		
	}
	void OnVisible(VARIANT_BOOL Visible)
	{
		
	}
	void OnToolBar(VARIANT_BOOL ToolBar)
	{
		
	}
	void OnMenuBar(VARIANT_BOOL MenuBar)
	{
		
	}
	void OnStatusBar(VARIANT_BOOL StautsBar)
	{
		
	}
	void OnFullScreen(VARIANT_BOOL FullScreen)
	{
		
	}
	void OnTheaterMode(VARIANT_BOOL Theater)
	{
		
	}
};

// it doesnt work doing T instead of CWebBrowserEventsT<T> 
//IDispEventImpl<SOURCE_BROWSER,T,
//		&DIID_DWebBrowserEvents,&LIBID_SHDocVw,
//		LIB_VER_MAJ,LIB_VER_MIN>

template <class T>
class CWebBrowserEventsT:public IWebBrowserEvents,
		public IDispEventImpl<SOURCE_BROWSER,CWebBrowserEventsT<T>,
		&DIID_DWebBrowserEvents2,&LIBID_SHDocVw,
		LIB_VER_MAJ,LIB_VER_MIN>
{
	typedef CWebBrowserEventsT<T> _ThisClass;
public:
	BEGIN_SINK_MAP(_ThisClass)
		MY_SINK_ENTRY(DISPID_STATUSTEXTCHANGE,_OnStatusChange)
		MY_SINK_ENTRY(DISPID_PROGRESSCHANGE,_OnProgressChange)
		MY_SINK_ENTRY(DISPID_COMMANDSTATECHANGE,_OnCmdStateChange)
		MY_SINK_ENTRY(DISPID_DOWNLOADBEGIN,_OnDownloadBegin)
		MY_SINK_ENTRY(DISPID_DOWNLOADCOMPLETE,_OnDownloadComplete)
		MY_SINK_ENTRY(DISPID_TITLECHANGE,_OnTitleChange)
		MY_SINK_ENTRY(DISPID_PROPERTYCHANGE,_OnPropertyChange)
		MY_SINK_ENTRY(DISPID_BEFORENAVIGATE2,_OnBeforeNavigate)
		MY_SINK_ENTRY(DISPID_NEWWINDOW2,_OnNewWindow)
		MY_SINK_ENTRY(DISPID_NAVIGATECOMPLETE2,_OnNavigateComplete)
		MY_SINK_ENTRY(DISPID_DOCUMENTCOMPLETE,_OnDocumentComplete)
		MY_SINK_ENTRY(DISPID_ONQUIT,_OnQuit)
		MY_SINK_ENTRY(DISPID_ONVISIBLE,_OnVisible)
		MY_SINK_ENTRY(DISPID_ONTOOLBAR,_OnToolBar)
		MY_SINK_ENTRY(DISPID_ONMENUBAR,_OnMenuBar)
		MY_SINK_ENTRY(DISPID_ONSTATUSBAR,_OnStatusBar)
		MY_SINK_ENTRY(DISPID_ONFULLSCREEN,_OnFullScreen)
		MY_SINK_ENTRY(DISPID_ONTHEATERMODE,_OnTheaterMode)
	END_SINK_MAP()

	//
	void __stdcall _OnStatusChange(BSTR Text)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnStatusChange(Text);
	}
	void __stdcall _OnProgressChange(long Progress,long ProgressMax)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnProgressChange(Progress,ProgressMax);
	}
	void __stdcall _OnCmdStateChange(long Cmd,VARIANT_BOOL Enable)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnCmdStateChange(Cmd,Enable);
	}
	void __stdcall _OnDownloadBegin(void)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnDownloadBegin();
	}
	void __stdcall _OnDownloadComplete(void)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnDownloadComplete();
	}
	void __stdcall _OnTitleChange(BSTR Text)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnTitleChange(Text);
	}
	void __stdcall _OnPropertyChange(BSTR Property)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnPropertyChange(Property);
	}
	void __stdcall _OnBeforeNavigate(IDispatch* pDisp,VARIANT * URL,
		VARIANT * Flags,VARIANT * TargetFrameName,VARIANT * PostData,
		VARIANT * Headers,/*[in,out]*/ VARIANT_BOOL * Cancel)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnBeforeNavigate(pDisp,URL,Flags,TargetFrameName,
			PostData,Headers,Cancel);
	}
	void __stdcall _OnNewWindow(/*[in, out]*/ IDispatch** ppDisp,
		/*[in, out]*/ VARIANT_BOOL * Cancel)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnNewWindow(ppDisp,Cancel);
	}
	void __stdcall _OnNavigateComplete(IDispatch* pDisp,
		VARIANT * URL )
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnNavigateComplete(pDisp,URL);
	}
	void __stdcall _OnDocumentComplete(IDispatch* pDisp,
		VARIANT * URL )
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnDocumentComplete(pDisp,URL);
	}
	void __stdcall _OnQuit(void)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnQuit();
	}
	void __stdcall _OnVisible(VARIANT_BOOL Visible)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnVisible(Visible);
	}
	void __stdcall _OnToolBar(VARIANT_BOOL ToolBar)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnToolBar(ToolBar);
	}
	void __stdcall _OnMenuBar(VARIANT_BOOL MenuBar)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnMenuBar(MenuBar);
	}
	void __stdcall _OnStatusBar(VARIANT_BOOL StatusBar)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnStatusBar(StatusBar);
	}
	void __stdcall _OnFullScreen(VARIANT_BOOL FullScreen)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnFullScreen(FullScreen);
	}
	void __stdcall _OnTheaterMode(VARIANT_BOOL Theater)
	{
		T *pThis = static_cast<T*>(this);
		pThis->OnTheaterMode(Theater);
	}
};
}