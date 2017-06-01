// RoomDlg.h: interface for the CRoomDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROOMDLG_H__43824AF0_6A6C_4795_8A57_04F91DA5A02D__INCLUDED_)
#define AFX_ROOMDLG_H__43824AF0_6A6C_4795_8A57_04F91DA5A02D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <no5tl\atldispa.h>
//#include <msxml2did.h>			// msxml disp ids

class CRoomDlg : \
	public CDialogImpl<CRoomDlg>,
	public CDialogResize<CRoomDlg>,
	public IDispDynImpl<CRoomDlg>
{
	typedef CDialogResize<CRoomDlg> _ResizeBase;

	struct CItemData
	{
		virtual ~CItemData()
		{
			//
		}
		virtual bool IsLobby(void) = 0;
	};
	struct CCategory
	{
		CString m_name;
		ULONG m_id;
		CCategory(LPCTSTR name = NULL,ULONG id = 0):m_name(name),m_id(id)
		{
			//
		}
	};
	struct CRoom : public CItemData
	{
		CString m_type;
		CString m_name;
		ULONG m_id;
		CString m_topic;
		int m_users;	// sum of each lobbies m_user

		CRoom(LPCTSTR type = NULL,LPCTSTR name = NULL,ULONG id = 0,LPCTSTR topic = NULL):\
			m_type(type),m_name(name),m_id(id),m_topic(topic)
		{
			m_users = 0;
		}
		virtual ~CRoom()
		{
			
		}
		virtual bool IsLobby(void) 
		{
			return false;
		}
	};
	struct CLobby : public CItemData
	{
		int m_index;
		int m_users;
		int m_voices;
		int m_cams;
		CRoom *m_pRoom;

		CLobby(CRoom *pRoom,int index = 0,int users = 0,int voices = 0,int cams = 0):\
			m_index(index),m_users(users),m_voices(voices),m_cams(cams)
		{
			m_pRoom = pRoom;
		}
		virtual ~CLobby()
		{
			//
		}
		virtual bool IsLobby(void) 
		{
			return true;
		}
	};
	struct ServerData
	{
		CString m_server;
	};
public:
	// if uMsg != 0 , modeless expected
	CRoomDlg(UINT uMsg = 0);
	virtual ~CRoomDlg();

	enum { IDD = IDD_ROOMSELECTOR };

	BEGIN_MSG_MAP(CRoomDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DELETEITEM, OnDeleteItem)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_HANDLER(IDC_COMBO_SERVER,CBN_SELCHANGE,OnServerSelChange)
		NOTIFY_HANDLER(IDC_TREE1,TVN_DELETEITEM,OnCategoryDelete)
		NOTIFY_HANDLER(IDC_TREE1,TVN_SELCHANGED,OnCategorySelChanged)
		NOTIFY_HANDLER(IDC_TREE2,TVN_DELETEITEM,OnRoomDelete)
		NOTIFY_HANDLER(IDC_TREE2,TVN_SELCHANGED,OnRoomSelChanged)
		NOTIFY_HANDLER(IDC_TREE2,TVN_GETINFOTIP,OnRoomGetTip)
		CHAIN_MSG_MAP(_ResizeBase)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CRoomDlg)
		DLGRESIZE_CONTROL(IDC_COMBO_SERVER,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_STATUS,DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_TREE1,DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_TREE2,DLSZ_SIZE_X|DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_COMBO_ROOM,DLSZ_SIZE_X|DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDOK,DLSZ_MOVE_X|DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDCANCEL,DLSZ_MOVE_X|DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DISPATCH_MAP(CRoomDlg)
		DISP_METHOD0_ID(OnReadyStateChange,DISPID_VALUE,VT_EMPTY)
	END_DISPATCH_MAP()

	virtual void OnFinalMessage(HWND hWnd)
	{
		if(m_uMsg){
			// TODO
			// buggy
			// delete this;
		}
	}

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDeleteItem(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCategoryDelete(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnRoomDelete(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnCategorySelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnRoomSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	LRESULT OnServerSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRoomGetTip(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);
	//
	void __stdcall OnReadyStateChange(void);


public:
	CString m_server;	// initialy should contain the default server
	CString m_room;		// on return contains the selected room
private:
	CTreeViewCtrlEx m_tv;
	CTreeViewCtrlEx m_tv2;
	CComboBox m_cbServers;
	CComboBox m_cbRooms;
	CComPtr<IXMLHTTPRequest> m_spHttp;
	CComPtr<IXMLHTTPRequest> m_spHttp2;
	bool m_bLoadingCats;
	bool m_bLoadingRooms;
	const UINT m_uMsg;

	HRESULT LoadCatsFromFile(LPCTSTR file);
	HRESULT LoadCatsFromServer(LPCTSTR server);
	HRESULT LoadCategories(IXMLDOMDocument *pDoc);
	HRESULT AddCategories(CTreeItem &parent,IXMLDOMNodeList *pNodeList);
	HRESULT LoadRooms(LPCTSTR server,ULONG cat_id);
	HRESULT LoadRooms(IXMLDOMDocument *pDoc);
	HRESULT AddRooms(CTreeItem &parent,IXMLDOMNodeList *pNodeList); 
	HRESULT AddLobbies(CTreeItem &parent,CRoom *pRoom,IXMLDOMNodeList *pNodeList);
	HRESULT LoadServers(void);
	HRESULT AddServer(IXMLDOMNode *pNode);

	void OnCategoriesLoaded(void);
	void OnRoomsLoaded(void);
	void LoadRecentRooms(void);
};

#endif // !defined(AFX_ROOMDLG_H__43824AF0_6A6C_4795_8A57_04F91DA5A02D__INCLUDED_)
