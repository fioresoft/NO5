// ScriptEditFrame.h: interface for the CScriptEditFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTEDITFRAME_H__5FE964AD_5C9B_4406_90B3_54F504223059__INCLUDED_)
#define AFX_SCRIPTEDITFRAME_H__5FE964AD_5C9B_4406_90B3_54F504223059__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <no5tl\scriptsite.h>
class CRtfScriptEditorCtrl;

enum ScriptLang
{
	LANG_VBS,
	LANG_JS,
	LANG_NONE,
};

LPCTSTR GetFileExt(LPCTSTR file);
ScriptLang GetFileLang(LPCTSTR file);


class CScriptSiteWindow : \
	public CWindow,
	public IActiveScriptSiteImpl,
	public IActiveScriptSiteWindowImpl
	
{
public:

	STDMETHOD(QueryInterface)(REFIID iid,LPVOID *ppv)
	{
		HRESULT hr = E_POINTER;

		if(ppv){
			*ppv = NULL;
			if(InlineIsEqualUnknown(iid))
				*ppv = (IUnknown *)(IActiveScriptSite *)(this);
			else if(InlineIsEqualGUID(iid,IID_IActiveScriptSite))
				*ppv = static_cast<IActiveScriptSite*>(this);
			else if(InlineIsEqualGUID(iid,IID_IActiveScriptSiteWindow))
				*ppv = static_cast<IActiveScriptSiteWindow*>(this);
			else
				hr = E_NOINTERFACE;

			if(*ppv){
				(IUnknown *)(IActiveScriptSite *)(this)->AddRef();
				hr = S_OK;
			}
		}
		return hr;
	}
	STDMETHOD_(ULONG,AddRef)(void)
	{
		return 1;
	}
	STDMETHOD_(ULONG,Release)(void)
	{
		return 1;
	}
	STDMETHOD(GetWindow)(HWND *phWnd)
	{
		if(phWnd==NULL)
			return E_POINTER;
		*phWnd = m_hWnd;
		return NOERROR;
	};

	// former engine destroyed, previous interfaces released
	// vbscript resets variables but retains coded added to ParseScriptText with SCRIPTTEXT_ISPERSISTENT 
	// new engine created
	// cannont run code. 
	virtual HRESULT Initiate(LPCTSTR pszLanguage)
	{
		USES_CONVERSION;
		HRESULT hr;

		Terminate();
		m_bInit = false;
		m_bRunning = false;
		
		hr = m_spEngine.CoCreateInstance(T2COLE(pszLanguage));
		if(SUCCEEDED(hr)){
			m_spParser = m_spEngine;
			if(m_spParser){
				hr = m_spEngine->SetScriptSite(static_cast<IActiveScriptSite*>(this));
				if(SUCCEEDED(hr)){
					hr = m_spParser->InitNew();
				}
			}
			else{
				hr = E_NOINTERFACE;
			}
		}
		if(FAILED(hr)){
			m_bInit = false;
			Terminate();
		}
		else{
			m_bInit = true;
		}
		return hr;
	}

	STDMETHOD(StartScript)()
	{
		HRESULT hr = E_FAIL;
		
		if(m_bInit){
			m_bRunning = true;
			hr = m_spEngine->SetScriptState(SCRIPTSTATE_CONNECTED);
			if(SUCCEEDED(hr)){
			}
			else
				m_bRunning = false;
		}
		return hr;
	}
	STDMETHOD(StopScript)()
	{
		HRESULT hr = S_FALSE;

		if(m_bRunning){
			hr = m_spEngine->SetScriptState(SCRIPTSTATE_DISCONNECTED);
			hr = m_spEngine->InterruptScriptThread(SCRIPTTHREADID_ALL,NULL,0);
			hr = m_spEngine->SetScriptState(SCRIPTSTATE_INITIALIZED);
			m_bRunning = false;
		}
		return hr;
	}

	STDMETHOD(Terminate)()
	{
		if(m_spEngine) {
			// Disconnect the host application from the engine. This will prevent
			// the further firing of events. Event sinks that are in progress will
			// be completed before the state changes.
			m_spEngine->SetScriptState(SCRIPTSTATE_DISCONNECTED);

			// Call to InterruptScriptThread to abandon any running scripts and
			// force cleanup of all script elements.
			m_spEngine->InterruptScriptThread(SCRIPTTHREADID_ALL,NULL,0);
			m_spEngine->Close();
			if(m_spParser)
				m_spParser.Release();
			m_spEngine.Release();

			m_bRunning = false;
			m_bInit = false;
		}
		return NOERROR;
	}

	STDMETHOD(AddScript)(LPCTSTR pszScript,LPCTSTR pszContext=NULL)
	{
		if(!m_bInit) return E_FAIL;

		USES_CONVERSION;
		const DWORD dwFlags = SCRIPTTEXT_ISVISIBLE | SCRIPTTEXT_ISPERSISTENT ;
		EXCEPINFO einfo;
		return m_spParser->ParseScriptText(T2COLE(pszScript),pszContext!=NULL ? T2COLE(pszContext) : OLESTR(""),NULL,NULL,0,0,dwFlags,NULL,&einfo);
	}

	STDMETHOD(AddObject)(LPCTSTR pszName,BOOL bGlobalCollection=FALSE,BOOL bIsSource = TRUE)
	{
		if(!m_bInit) return E_FAIL;

		DWORD dwFlags = SCRIPTITEM_ISVISIBLE;
		if(bGlobalCollection)
			dwFlags |= SCRIPTITEM_GLOBALMEMBERS;
		if(bIsSource)
			dwFlags |= SCRIPTITEM_ISSOURCE;

		USES_CONVERSION;
		return m_spEngine->AddNamedItem(T2COLE(pszName),dwFlags);
	}

	STDMETHOD(RunScriptSub)(LPCTSTR name)
	{
		HRESULT  hr = E_FAIL;
		
		if(m_bInit){
			CComDispatchDriver spDisp;

			hr = m_spEngine->GetScriptDispatch(NULL,&spDisp);
			if(SUCCEEDED(hr)){
				USES_CONVERSION;
				hr = spDisp.Invoke0(T2COLE(name));
			}
		}
		return hr;
	}
	CScriptSiteWindow()
	{
		m_bRunning = false;
		m_bInit = false;
	}
	virtual ~CScriptSiteWindow()
	{
		if(!!m_spEngine)
			Terminate();

		if(!!m_spEngine) {
			// Always call prior to release
			m_spEngine->Close();
			m_spEngine.Release();
		}
	}
protected:
	bool m_bInit;
	bool	m_bRunning;
	CComPtr<IActiveScript>	m_spEngine;
	CComQIPtr<IActiveScriptParse> m_spParser;
};

class CScriptEditFrame : \
	public CDialogImpl<CScriptEditFrame,CScriptSiteWindow>,
	public CDialogResize<CScriptEditFrame>
{
	typedef CDialogImpl<CScriptEditFrame,CScriptSiteWindow>  baseClass;
public:
	CScriptEditFrame(HWND hWndOwner,LPCTSTR file = NULL);
	virtual ~CScriptEditFrame();

	enum { IDD = IDD_SCRIPTFRAME };

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		//delete this;
	}

	STDMETHOD(LookupNamedItem)(LPCOLESTR pstrName,LPUNKNOWN* ppunkItem);
	// This method is called when the script engine terminates
	STDMETHOD(OnScriptTerminate)(const VARIANT* /*pvarResult*/,const EXCEPINFO* /*pexcepinfo*/)
	{
		return S_OK;
	};
	// This method is called when (before) the script engine starts executing the script/event handler
	STDMETHOD(OnEnterScript)(void)
	{
		return S_OK;
	};

	// This method is called when (after) the script engine finishes executing the script/event handler
	STDMETHOD(OnLeaveScript)(void)
	{
		return S_OK;
	};

	// This method is called when the script engine's state is changed
	STDMETHOD(OnStateChange)(SCRIPTSTATE ss)
	{
		/*
		if(true){
			LPCTSTR msg;

			switch(ss){
				case SCRIPTSTATE_UNINITIALIZED:
					msg = "uninitialized";
					break;
				case SCRIPTSTATE_INITIALIZED:
					msg = "initialized";
					break;
				case SCRIPTSTATE_STARTED:
					msg = "started";
					break;
				case SCRIPTSTATE_CONNECTED:
					msg = "connected";
					break;
				case SCRIPTSTATE_DISCONNECTED:
					msg = "disconnected";
					break;
				case SCRIPTSTATE_CLOSED:
					msg = "closed";
					break;
				default:
					msg = "unknown";
					break;
			}
		}
		*/
		return NOERROR;
	};
	

	BEGIN_MSG_MAP(CScriptEditFrame)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCDESTROY,OnNCDestroy)
		MESSAGE_HANDLER(WM_CLOSE,OnClose)
		COMMAND_ID_HANDLER(ID_SCRIPT_RUN,OnScriptRun)
		COMMAND_ID_HANDLER(ID_SCRIPT_STOP,OnScriptStop)
		COMMAND_ID_HANDLER(ID_FILE_CLOSE,OnFileClose)
		COMMAND_ID_HANDLER(ID_FILE_OPEN,OnFileOpen)
		COMMAND_ID_HANDLER(ID_FILE_SAVE,OnFileSave)
		COMMAND_ID_HANDLER(ID_FILE_SAVE_AS,OnFileSaveAs)
		COMMAND_HANDLER(IDC_COMBO_OBJS,CBN_SELCHANGE,OnComboObjsChange)
		COMMAND_HANDLER(IDC_COMBO_EVENTS,CBN_SELCHANGE,OnComboEventsChange)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP( CDialogResize<CScriptEditFrame> )
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CScriptEditFrame)
		DLGRESIZE_CONTROL(IDC_COMBO_OBJS,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_COMBO_EVENTS,DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(ID_SCRIPT_RUN,DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(ID_SCRIPT_STOP,DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_SCRIPTEDIT,DLSZ_SIZE_X|DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNCDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnScriptRun(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/);
	LRESULT OnScriptStop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/);
	LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnComboObjsChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnComboEventsChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
private:
	CRtfScriptEditorCtrl *m_pEdit;
	CComboBox m_cbObjs;
	CComboBox m_cbEvents;
	CButton	m_btStart;
	CButton m_btStop;
	CString m_file;
	ScriptLang m_lang;
	CWindow m_wndOwner;
private:
	BOOL ReadFile(LPCTSTR file);
	BOOL WriteFile(LPCTSTR file);
	void LoadKeywords(void);
	void FillObjsCombo(void);
	void FillEventsCombo(void);
	void GetObjects(CSimpleArray<CString> &objs);
	BOOL GetObjectMembers(LPCTSTR name,CSimpleArray<CString> &methods,CSimpleArray<CString> &props);
	BOOL GetObjectEvents(LPCTSTR name,CSimpleArray<CString> &events);
	BOOL GetObjectFunctionParams(LPCTSTR objName,LPCTSTR method,CSimpleArray<CString> & args);
	BOOL GetObjectEventParams(LPCTSTR objName,LPCTSTR event,CSimpleArray<CString> &args);

};

#endif // !defined(AFX_SCRIPTEDITFRAME_H__5FE964AD_5C9B_4406_90B3_54F504223059__INCLUDED_)
