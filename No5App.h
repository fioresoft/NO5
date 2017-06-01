// No5App.h: interface for the CNo5App class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NO5APP_H__69F66361_8369_11D9_A17B_FDD37F9FE62B__INCLUDED_)
#define AFX_NO5APP_H__69F66361_8369_11D9_A17B_FDD37F9FE62B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <yahoo\smileymap.h>
#include "no5obj.h"

// using declarations
using NO5YAHOO::ISmileyMap;
// foward declarations
class CPath;
struct IMainFrame;
struct IGeneralOptions;
struct IVisualOptions;
struct CIgnoreOptions;
struct IIgnoreList;
class CRecentRooms;


class CNo5App
{
	CPath *m_pAppPath;
	ISmileyMap *m_psm;
	IMainFrame *m_pFrame;
	IGeneralOptions *m_pgo;
	IVisualOptions *m_pvo;
	CString m_name;		// name of the program
	CIgnoreOptions *m_pio;
	IIgnoreList *m_pIgnoreList;
	CRecentRooms *m_pRecentRooms;
	CComObject<CNo5Obj> *m_pNo5;
	int m_scripts;		// number of running scripts
public:
	CNo5App();
	~CNo5App();
	//
	BOOL Init(void);
	void Term(void);
	CString GetPath(void);
	CString GetModuleFileName(void);
	CString GetOptionsFile(void);
	CString GetVisualOptionsFile(void);
	CString GetSmileysFolder(void);
	CString GetSmileysFile(void);
	CString GetColorsFile(void);
	CString GetIgnoreFile(void);
	CString GetRecentRoomsFile(void);
	CString GetScriptsFolder(void);
	CString GetFormsFolder(void);
	CString GetSnapshotFolder(void);
	CString GetVideoFolder(void);
	CString GetBotsFolder(void);
	ISmileyMap * GetSmileyMap(void)
	{
		ATLASSERT(m_psm);
		return m_psm;
	}
	IMainFrame * GetMainFrame(void)
	{
		ATLASSERT(m_pFrame);
		return m_pFrame;
	}
	void SetMainFrame(IMainFrame *pFrame)
	{
		m_pFrame = pFrame;
	}
	IGeneralOptions * GetGeneralOptions(void)
	{
		return m_pgo;
	}
	void SetGeneralOptions(IGeneralOptions *pgo)
	{
		ATLASSERT(!m_pgo);
		m_pgo = pgo;
	}
	IVisualOptions * GetVisualOptions(void)
	{
		return m_pvo;
	}
	void SetVisualOptions(IVisualOptions *pvo)
	{
		ATLASSERT(!m_pvo);
		m_pvo = pvo;
	}
	const CString & GetName(void) const
	{
		return m_name;
	}
	void LoadName(UINT uResourceID);
	CIgnoreOptions & GetIgnoreOptions(void);
	void LoadIgnoreOptions(void);
	void WriteIgnoreOptions(void);
	void LoadIgnoreList(void);
	void UnloadIgnoreList(void);
	IIgnoreList * GetIgnoreList(void)
	{
		return m_pIgnoreList;
	}
	CRecentRooms * GetRecentRooms(void);
	void LoadRecentRooms(void);
	void UnloadRecentRooms(void);
	CComObject<CNo5Obj> * GetNo5Obj(void)
	{
		ATLASSERT(m_pNo5);
		return m_pNo5;
	}
	int IncScript(void)	// increments script counter, returns new value
	{
		return ++m_scripts;
	}
	int DecScript(void)
	{
		int res = --m_scripts;
		//ATLASSERT(m_scripts >= 0);
		return res;
	}
	bool RunningScripts(void) const
	{
		//return m_scripts > 0;
		return true;
	}
};

extern CNo5App g_app;

#endif // !defined(AFX_NO5APP_H__69F66361_8369_11D9_A17B_FDD37F9FE62B__INCLUDED_)
