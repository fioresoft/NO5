// VirtualKeyboard.h: interface for the CVirtualKeyboard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIRTUALKEYBOARD_H__2BB37AA5_0995_11D9_A17B_E0FF6E33774E__INCLUDED_)
#define AFX_VIRTUALKEYBOARD_H__2BB37AA5_0995_11D9_A17B_E0FF6E33774E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "clipboard.h"

namespace NO5TL
{

class CAttachThreadInput
{
	const DWORD m_dwThis;
	const DWORD m_dwThat;
public:
	CAttachThreadInput(HWND hWnd):\
		m_dwThis(GetCurrentThreadId()),
		m_dwThat(GetWindowThreadProcessId(hWnd,NULL)) 
	{
		BOOL res = ::AttachThreadInput(m_dwThis,m_dwThat,TRUE);
		ATLASSERT(res);
	}
	~CAttachThreadInput(){
		::AttachThreadInput(m_dwThis,m_dwThat,FALSE);
	}
};

class CVirtualKeyboard
{
	struct KeyState
	{
		bool m_shift:1;
		bool m_ctrl:1;
		bool m_alt:1;
		//
		KeyState(void)
		{
			Reset();
		}
		void Reset(void)
		{
			m_shift = false;
			m_ctrl = false;
			m_alt = false;
		}
	};
	HWND m_hWnd;
	//
	void PressVirtualKey(int iVirKey,BOOL bPress = TRUE) const
	{
		DWORD dwFlags = (bPress)?0:KEYEVENTF_KEYUP;

		::keybd_event(iVirKey,0,dwFlags,0);
	}
	int GetVirtualKey(TCHAR ch,KeyState *pks) const
	{
		SHORT rv;
		BYTE vk;
		BYTE flags;
		BOOL bRv = FALSE;

		// gets the virtual key code of ch
		rv = ::VkKeyScan(ch);
		vk = LOBYTE(rv);
		flags = HIBYTE(rv);

		if(vk != (BYTE)-1 && flags != (BYTE)-1){

			if(pks){

				pks->Reset();

				// SHIFT
				if(flags & 1)
					pks->m_shift = true;
				//  CTRL
				if(flags & 2)
					pks->m_ctrl = true;
				//  ALT
				if(flags & 4)
					pks->m_alt = true;
			}
		}
		return vk;
	}
public:
	CVirtualKeyboard(HWND hwndTarget = NULL):m_hWnd(hwndTarget)
	{
		//
	}
	HWND SetTargetWindow(HWND hWnd)
	{
		HWND hTmp = m_hWnd;
		m_hWnd = hWnd;
		return hTmp;
	}
	HWND GetTargetWindow(void) const 
	{
		return m_hWnd;
	}
	void SendText(LPCSTR s,DWORD dwDelay = 200) const
	{
		CAttachThreadInput at(m_hWnd);
		int i;
		int iVirKey;
		KeyState hks;

		SetForegroundWindow(m_hWnd);
		for(i=0;s[i];i++){
			hks.Reset();
			iVirKey = GetVirtualKey(s[i],&hks);
			ATLASSERT(iVirKey >= 0);

			// SHIFT
			if(hks.m_shift){
				PressVirtualKey(VK_SHIFT);
			}
			// CTRL
			if(hks.m_ctrl){
				PressVirtualKey(VK_CONTROL);
			}
			// ALT
			if(hks.m_alt){
				PressVirtualKey(VK_MENU);
			}
			// the letter
			PressVirtualKey(iVirKey);
			PressVirtualKey(iVirKey,FALSE);
				
			// SHIFT
			if(hks.m_shift){
				PressVirtualKey(VK_SHIFT,FALSE);
			}
			// CTRL
			if(hks.m_ctrl){
				PressVirtualKey(VK_CONTROL,FALSE);
			}
			// ALT
			if(hks.m_alt){
				PressVirtualKey(VK_MENU,FALSE);
			}
			if(dwDelay)
				::Sleep(dwDelay);
		}
	}
	void SendText2(LPCSTR s) const
	{
		CClipBoard cb(GetTargetWindow());

		cb.SetText(s);
		SendPaste();
	}
	void SendEnter(void) const
	{
		CAttachThreadInput at(m_hWnd);

		SetForegroundWindow(m_hWnd);
		PressVirtualKey(VK_RETURN);
		PressVirtualKey(VK_RETURN,FALSE);
	}
	// sends a CTRL-V to the target window
	void SendPaste(void) const
	{
		CAttachThreadInput at(m_hWnd);

		SetForegroundWindow(m_hWnd);
		PressVirtualKey(VK_CONTROL);
		PressVirtualKey('V');
		PressVirtualKey('V',FALSE);
		PressVirtualKey(VK_CONTROL,FALSE);
	}
	void SendChar(char ch) const
	{
		int vk;
		KeyState state;
		CAttachThreadInput at(m_hWnd);

		SetForegroundWindow(m_hWnd);
		vk = GetVirtualKey(ch,&state);
		ATLASSERT(vk >= 0);

		// press

		if(state.m_shift)
			PressVirtualKey(VK_SHIFT);
		if(state.m_ctrl)
			PressVirtualKey(VK_CONTROL);
		if(state.m_alt)
			PressVirtualKey(VK_MENU);

		PressVirtualKey(vk);

		// release

		PressVirtualKey(vk,FALSE);

		if(state.m_shift)
			PressVirtualKey(VK_SHIFT);
		if(state.m_ctrl)
			PressVirtualKey(VK_CONTROL);
		if(state.m_alt)
			PressVirtualKey(VK_MENU);
	}
};

}	// NO5TL

#endif // !defined(AFX_VIRTUALKEYBOARD_H__2BB37AA5_0995_11D9_A17B_E0FF6E33774E__INCLUDED_)
