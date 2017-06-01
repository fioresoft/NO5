// No5App.cpp: implementation of the CNo5App class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "no5app.h"
#include "path.h"
#include "imainframe.h"
#include "no5options.h"
#include "ignorelist.h"
#include "resource.h"
#include "recentrooms.h"
#include "no5obj.h"
#include "ddforms.tlh"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNo5App::CNo5App()
{
	m_pAppPath = new CPath();
	NO5YAHOO::SmileyMapCreate(&m_psm);
	m_pFrame = NULL;
	m_pgo = NULL;
	m_pvo = NULL;
	m_pio = new CIgnoreOptions();
	m_pRecentRooms = NULL;
	m_pNo5 = NULL;
	m_scripts = 0;
}

CNo5App::~CNo5App()
{
	delete m_pAppPath;
	NO5YAHOO::SmileyMapDestroy(&m_psm);
	delete m_pio;
	if(m_pRecentRooms)
		delete m_pRecentRooms;
}

BOOL CNo5App::Init(void)
{
#ifdef _DEBUG
	m_pAppPath->SetPath(PATH_CURDIR);
#else
	CPath path(PATH_MODULE);
	m_pAppPath->SetPath(path.GetLocation());
#endif
	if(m_pAppPath->ExistLocation()){
		if(!m_psm->IsLoaded()){
			BOOL res = m_psm->LoadMap(GetSmileysFile());
			ATLASSERT(res);
		}
		g_app.LoadName(IDR_MAINFRAME);

		// create IGeneralOptions instance and load from file
		ATLASSERT(!GetGeneralOptions());
		SetGeneralOptions(IGeneralOptions::CreateMe());
		GetGeneralOptions()->SetFileName(GetOptionsFile());
		GetGeneralOptions()->GetInMemoryOptions();
		// create IVisualOptions instance and load from file
		ATLASSERT(!GetVisualOptions());
		SetVisualOptions(IVisualOptions::CreateMe());
		GetVisualOptions()->SetFile(GetVisualOptionsFile());
		// load ignore options
		LoadIgnoreOptions();
		LoadIgnoreList();
		LoadRecentRooms();

		// register type lib
		_Module.RegisterTypeLib();

		// create script object
		HRESULT hr;

		hr = CComObject<CNo5Obj>::CreateInstance(&m_pNo5);
		if(SUCCEEDED(hr)){
			CComPtr<INo5Obj> sp;
			hr = m_pNo5->QueryInterface(&sp);
			if(SUCCEEDED(hr)){
				hr = m_pNo5->PrivateInit();
				sp.Detach();
			}
		}
		if(SUCCEEDED(hr)){
			CComPtr<DDFORMSLib::IFormEditor2> sp;
			hr = sp.CoCreateInstance(DDFORMSLib::CLSID_FormEditor);
			if(FAILED(hr)){
				CString path = GetPath();
				path += "ddforms.dll";
				path += " /s";
				UINT u = (UINT)::ShellExecute(GetDesktopWindow(),"open","regsvr32",path,NULL,SW_HIDE);
				if(u > 32){
					path = GetPath();
					path += "ddcontrolpack.dll";
					path += " /s";
					u = (UINT)::ShellExecute(GetDesktopWindow(),"open","regsvr32",path,NULL,SW_HIDE);
					path = GetPath();
					path += "ddproppageall.dll";
					path += " /s";
					u = (UINT)::ShellExecute(GetDesktopWindow(),"open","regsvr32",path,NULL,SW_HIDE);
				}
				if(u < 32)
					MessageBox(GetDesktopWindow(),"form dlls not registered",GetName(),MB_ICONINFORMATION);
				else
					hr = S_OK;

			}
		}
		ATLASSERT(SUCCEEDED(hr));
	}
	
	return m_pAppPath->ExistLocation();
}

void CNo5App::Term(void)
{
	WriteIgnoreOptions();
	GetGeneralOptions()->WriteInMemoryOptions();

	IGeneralOptions *pgo = GetGeneralOptions();
	IGeneralOptions::DestroyMe(&pgo);
	IVisualOptions *pvo = GetVisualOptions();
	IVisualOptions::DestroyMe(&pvo);
	UnloadIgnoreList();
	UnloadRecentRooms();
	m_pNo5->Release();
	// unregister type lib
	_Module.UnRegisterTypeLib();
}

CString CNo5App::GetPath(void) 
{
	return m_pAppPath->GetLocation();
}

CString CNo5App::GetModuleFileName(void)
{
	CPath path(PATH_MODULE);
	return path.GetLongPath();
}

CString CNo5App::GetOptionsFile(void)
{
	return GetPath() + _T("no5.ini");
}

CString CNo5App::GetVisualOptionsFile(void)
{
	return GetOptionsFile();
}

CString CNo5App::GetScriptsFolder(void)
{
	return GetPath() + _T("scripts\\");
}

CString CNo5App::GetFormsFolder(void)
{
	return GetPath() + _T("forms\\");
}

CString CNo5App::GetSmileysFolder(void)
{
	return GetPath() + _T("smileys\\");
}

CString CNo5App::GetSmileysFile(void)
{
	return GetPath() + _T("smileys.txt");
}

CString CNo5App::GetColorsFile(void)
{
	return GetPath() + _T("colors.ini");
}

CString CNo5App::GetIgnoreFile(void)
{
	return GetPath() + _T("ignore.txt");
}

CString CNo5App::GetRecentRoomsFile(void)
{
	return GetPath() + _T("rooms.txt");
}

CString CNo5App::GetSnapshotFolder(void)
{
	return GetPath() + _T("snapshots\\");
}

CString CNo5App::GetVideoFolder(void)
{
	return GetPath() + _T("videos\\");
}

CString CNo5App::GetBotsFolder(void)
{
	return GetPath() + _T("bots\\");
}

void CNo5App::LoadName(UINT uResourceID)
{
	BOOL res = m_name.LoadString(uResourceID);
	ATLASSERT(res);
}

CIgnoreOptions & CNo5App::GetIgnoreOptions(void)
{
	return *m_pio;
}

void CNo5App::LoadIgnoreOptions(void)
{
	ATLASSERT(m_pgo);

	if(m_pgo){
		BOOL res = m_pgo->GetIgnoreOptions(*m_pio);
		ATLASSERT(res);
	}
}

void CNo5App::WriteIgnoreOptions(void)
{
	ATLASSERT(m_pgo);

	if(m_pgo){
		BOOL res = m_pgo->SetIgnoreOptions(*m_pio);
		ATLASSERT(res);
	}
}

void CNo5App::LoadIgnoreList(void)
{
	bool res;

	ATLASSERT(!m_pIgnoreList);
	IIgnoreList::CreateMe(&m_pIgnoreList);
	res = m_pIgnoreList->read(GetIgnoreFile());
	ATLASSERT(res);
}

void CNo5App::UnloadIgnoreList(void)
{
	if(m_pIgnoreList){
		if(m_pIgnoreList->is_dirty()){
			bool res = m_pIgnoreList->write(g_app.GetIgnoreFile());
			ATLASSERT(res);
		}
		IIgnoreList::DestroyMe(&m_pIgnoreList);
	}
}

CRecentRooms * CNo5App::GetRecentRooms(void)
{
	if(!m_pRecentRooms){
		LoadRecentRooms();
	}
	return m_pRecentRooms;
}

void CNo5App::LoadRecentRooms(void)
{
	BOOL res;

	if(!m_pRecentRooms){
		m_pRecentRooms = new CRecentRooms();
	}
	res = m_pRecentRooms->Read(GetRecentRoomsFile());
	ATLASSERT(res);
}

void CNo5App::UnloadRecentRooms(void)
{
	if(m_pRecentRooms){
		if(m_pRecentRooms->IsDirty()){
			BOOL res = m_pRecentRooms->Write(GetRecentRoomsFile());
			ATLASSERT(res);
		}
		delete m_pRecentRooms;
		m_pRecentRooms = NULL;
	}
}

