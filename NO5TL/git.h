// Usage:
/*
HRESULT SpawnThread()
{
    IMouse* pm = 0;
    HRESULT hr = CoCreateInstance(CLSID_Mouse, 0, CLSCTX_INPROC_SERVER, IID_IMouse, (void**)&pm);
    if( SUCCEEDED(hr) )
    {
        // DONE: Marshalled itf into GIT
        DWORD   dwCookie;
        hr = CoMarshalInterThreadInterfaceInGIT(IID_IMouse, pm, &dwCookie);
        if( SUCCEEDED(hr) )
        {
            DWORD   tid;
            HANDLE  hThread = CreateThread(0, 0, ThreadProc, (void*)dwCookie, 0, &tid);

            if( hThread ) CloseHandle(hThread);
            // DONE: If thread doesn't start, remove itf from GIT
            else hr = CoRevokeInterfaceFromGlobal(dwCookie);
        }
        pm->Release();
    }
    return hr;
}

DWORD WINAPI ThreadProc(void* pv)
{
    CoInitialize(0);

    // DONE: Pull pointer out of GIT
    DWORD   dwCookie = (DWORD)pv;
    IMouse* pm = 0;
    HRESULT hr = CoGetInterfaceAndRevokeFromGIT(dwCookie, IID_IMouse, (void**)&pm);
    if( SUCCEEDED(hr) )
    {
        // Use pm
        pm->Release();
    }

    CoUninitialize();
    return hr;
}
*/

#pragma once
namespace NO5TL
{

class CGit
{
	static CComPtr<IGlobalInterfaceTable> m_p;
	static HRESULT GetTable(void)
	{
		HRESULT hr = ::CoCreateInstance(\
			CLSID_StdGlobalInterfaceTable,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IGlobalInterfaceTable,
			(void **)&m_p);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
public:
	static HRESULT RegisterInterface(LPUNKNOWN pUnk,REFIID riid,
		DWORD &cookie)
	{
		HRESULT hr = S_OK;
		if(!m_p){
			hr = GetTable();
		}
		if(SUCCEEDED(hr)){
			hr = m_p->RegisterInterfaceInGlobal(pUnk,riid,
				&cookie);
			ATLASSERT(SUCCEEDED(hr));
		}
		return hr;
	}
	static HRESULT RevokeInterface(DWORD cookie)
	{
		HRESULT hr = m_p->RevokeInterfaceFromGlobal(cookie);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
	// returns add-refed interface
	static HRESULT GetInterface(DWORD cookie,REFIID riid,LPVOID *ppv)
	{
		HRESULT hr = m_p->GetInterfaceFromGlobal(cookie,riid,ppv);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}
};

} // NO5TL
