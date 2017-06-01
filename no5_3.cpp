// ycht02.cpp : main source file for ycht02.exe
//

#include "stdafx.h"
#include "initguid.h"
#include "resource.h"
#include "no5_3.h"
#include "no5_3_i.c"
#include "aboutdlg.h"
#include "yahoochatview.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "no5app.h"
#include <no5tl\winsocketwithhandler.h>
#include <no5tl\no5richedit.h>
#include "No5Obj.h"
#include "YahooTxtObj.h"

NO5TL::CSocketWindow NO5TL::CWinSocketData::m_wnd;
RegisterTabbedMDIMessages g_RegisterTabbedMDIMessages;


CAppModule _Module;
CNo5App g_app;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet;
	try{
		nRet = theLoop.Run();
	}catch(...){
		int debug = 0;
	}

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	NO5TL::CWinSockDLL sdll;
	NO5TL::CRichEditLib richlib;
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES |ICC_TREEVIEW_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(ObjectMap, hInstance,&LIBID_NO5Lib);
	ATLASSERT(SUCCEEDED(hRes));
	g_app.Init();
	AtlAxWinInit();
	ATLTRACE("before Run\n");
	int nRet = Run(lpstrCmdLine, SW_MAXIMIZE);
	ATLTRACE("after Run\n");

	g_app.Term();
	_Module.Term();
	::CoUninitialize();

	return nRet;
}
