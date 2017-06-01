// No5Settings.cpp: implementation of the CNo5Settings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <no5tl\color.h>
#include <no5tl\inifile.h>
#include <no5tl\mystring.h>		// CStringArray
#include <no5tl\simpleencrypt.h>
#include <yahoo\textstream.h>
#include "no5options.h"

// using
using namespace NO5TL::Colors;
using NO5YAHOO::FontAll;
using NO5YAHOO::CYahooColor;
using NO5TL::CStringArray;


const LPCTSTR NO5_DEF_FACE = _T("tahoma");
const CYahooColor DefColor(0,0,0xC0);
const int DefSize = 11;
const CYahooColor DefBkColor(250,250,255);
// todo : sections names should contain illegal yahoo name characters


class CGeneralOptions : public IGeneralOptions
{
	NO5TL::CPrivateIniFile m_ini;
	// these values are used all the time so we cant keep reading
	// from file, instead we read all at once and keep them in memory
	bool m_bShowEnter:1; // show enter/leave messages in room
public:
	CGeneralOptions()
	{
		m_bShowEnter = false;
	}
	virtual void SetFileName(LPCTSTR pFile)
	{
		BOOL res = m_ini.Create(pFile);
		ATLASSERT(res);
	}
	virtual CString GetServerYmsg(void)
	{
		return m_ini.GetString("general","server_ymsg","",
			MAX_PATH);
	}
	virtual CString GetServerYcht(void)
	{
		return m_ini.GetString("general","server_ycht","",
			MAX_PATH);
	}
	virtual UINT GetPortYmsg(void)
	{
		return m_ini.GetInt("general","port_ymsg",0);
	}
	virtual UINT GetPortYcht(void)
	{
		return m_ini.GetInt("general","port_ycht",0);
	}
	virtual int	GetAccountNames(CSimpleArray<CString> &names)
	{
		return m_ini.GetStringList("general","accounts",names);
	}

	virtual int GetServersYcht(CSimpleArray<CString> &servers)
	{
		return m_ini.GetStringList("general","servers_ycht",servers);
	}

	virtual int GetServersYmsg(CSimpleArray<CString> &servers)
	{
		return m_ini.GetStringList("general","servers_ymsg",servers);
	}

	virtual int GetPortsYcht(CSimpleValArray<int> &ports)
	{
		return m_ini.GetIntList("general","ports_ycht",ports);
	}

	virtual int GetPortsYmsg(CSimpleValArray<int> &ports)
	{
		return m_ini.GetIntList("general","ports_ymsg",ports);
	}

	virtual BOOL SetServerYmsg(LPCTSTR server)
	{
		return m_ini.WriteString("general","server_ymsg",server);
	}
	virtual BOOL SetServerYcht(LPCTSTR server)
	{
		return m_ini.WriteString("general","server_ycht",server);
	}
	virtual BOOL SetPortYmsg(UINT port)
	{
		CString s;

		s.Append(port);
		return m_ini.WriteString("general","port_ymsg",s);
	}
	virtual BOOL SetPortYcht(UINT port)
	{
		CString s;

		s.Append(port); 
		return m_ini.WriteString("general","port_ycht",s);
	}
	virtual BOOL SetAccountNames(const CSimpleArray<CString> &names)
	{
		return m_ini.WriteStringList("general","accounts",names);
	}
	virtual BOOL DeleteAccount(LPCTSTR name)
	{
		BOOL res = m_ini.DeleteSection(name);

		if(res){
			NO5TL::CStringArray accs;
			if(GetAccountNames(accs) > 0){
				int index = accs.FindNoCase(name);
				if(index >=0){
					accs.RemoveAt(index);
					res = SetAccountNames(accs);
				}
			}
		}
		return res;
	}
	virtual CString GetLastProto(void)
	{
		return m_ini.GetString("general","last_proto","ymsg",10);
	}
	virtual CString GetLastView(void)
	{
		return m_ini.GetString("general","last_view","html",10);
	}
	virtual CString GetLastAccountName(void)
	{
		return m_ini.GetString("general","last_account","",80);
	}
	virtual BOOL SetLastProto(LPCTSTR proto)
	{
		return m_ini.WriteString("general","last_proto",proto);
	}
	virtual BOOL SetLastView(LPCTSTR view)
	{
		return m_ini.WriteString("general","last_view",view);
	}
	virtual BOOL SetServersYmsg(CSimpleArray<CString> &servers)
	{
		return m_ini.WriteStringList("general","servers_ymsg",servers);
	}
	virtual BOOL SetServersYcht(CSimpleArray<CString> &servers)
	{
		return m_ini.WriteStringList("general","servers_ycht",servers);
	}
	virtual BOOL SetPortsYmsg(CSimpleValArray<int> &ports)
	{
		return m_ini.WriteIntList("general","ports_ymsg",ports);
	}
	virtual BOOL SetPortsYcht(CSimpleValArray<int> &ports)
	{
		return m_ini.WriteIntList("general","ports_ycht",ports);
	}
	virtual BOOL SetLastAccountName(LPCTSTR name)
	{
		return m_ini.WriteString("general","last_account",name);
	}
	virtual CString GetRoomServer(void)
	{
		return m_ini.GetString("general","room_server","http://insider.msg.yahoo.com",MAX_PATH);
	}
	virtual BOOL SetRoomServer(LPCTSTR name)
	{
		return m_ini.WriteString("general","room_server",name);
	}
	virtual BOOL SetPmOptions(CPmOptions &options)
	{
		BOOL res = TRUE;
		BOOL tmp;

		tmp = m_ini.WriteInt("private_messages","popup",options.m_popup ? 1 : 0);
		ATLASSERT(tmp);

		res = ( res && tmp);

		tmp = m_ini.WriteInt("private_messages","friends_only",options.m_friends ? 1 : 0);
		ATLASSERT(tmp);

		res = ( res && tmp);

		tmp = m_ini.WriteInt("private_messages","no_links",options.m_nolinks ? 1 : 0);
		ATLASSERT(tmp);

		res = ( res && tmp);

		tmp = m_ini.WriteInt("private_messages","topmost",options.m_topmost ? 1 : 0);
		ATLASSERT(tmp);

		res = ( res && tmp);

		return res;
	}
	virtual BOOL GetPmOptions(CPmOptions &options)
	{
		UINT val;

		val = m_ini.GetInt("private_messages","popup",0);
		options.m_popup = val ? true : false;

		val = m_ini.GetInt("private_messages","friends_only",0);
		options.m_friends = val ? true : false;

		val = m_ini.GetInt("private_messages","no_links",0);
		options.m_nolinks = val ? true : false;

		val = m_ini.GetInt("private_messages","topmost",0);
		options.m_topmost = val ? true : false;

		return TRUE;
	}
	virtual BOOL GetTopmost(void)
	{
		return m_ini.GetInt("general","topmost",0) != 0;
	}
	virtual BOOL SetTopmost(BOOL bSet)
	{
		UINT val = bSet ? 1 : 0;
		return m_ini.WriteInt("general","topmost",val);
	}
	virtual BOOL SetIgnoreOptions(const CIgnoreOptions &options)
	{
		BOOL res =  m_ini.WriteStruct("general","ignore",const_cast<CIgnoreOptions*>(&options),sizeof(CIgnoreOptions));

		if(res && options.m_pWords){
			res = m_ini.WriteStringList("general","spam_words",*options.m_pWords);
		}
		return res;
	}
	virtual BOOL GetIgnoreOptions(CIgnoreOptions &options)
	{
		BOOL res;
		CStringArray *pSave = options.m_pWords;
		
		res = m_ini.GetStruct("general","ignore",(LPVOID)&options,sizeof(CIgnoreOptions));
		options.m_pWords = pSave;
		if(res){
			if(!options.m_pWords){
				options.m_pWords = new CStringArray();
			}
			else
				options.m_pWords->RemoveAll();

			// add default values
			LPCTSTR words[] = {  _T("http://"),_T("www.") };
			for(int i=0;i<sizeof(words)/sizeof(words[0]);i++){
				options.m_pWords->Add(CString(words[i]));
			}
			res = m_ini.GetStringList("general","spam_words",*options.m_pWords);
		}
		return res;
	}
	virtual int GetConnectionType(void)
	{
		return m_ini.GetInt("general","conntype",1);
	}
	virtual void SetConnectionType(int i)
	{
		m_ini.WriteInt("general","conntype",i);
	}
	// read to memory the options that are used constantly
	virtual BOOL GetInMemoryOptions(void)
	{
		m_bShowEnter =  m_ini.GetInt("general","show_enter",0) != 0;
		return TRUE;
	}
	virtual BOOL WriteInMemoryOptions(void)
	{
		UINT val = m_bShowEnter ? 1 : 0;
		return m_ini.WriteInt("general","show_enter",val);
	}
	virtual BOOL ShowEnter(void) const
	{
		return m_bShowEnter == true;
	}
	virtual void ShowEnter(BOOL bSet)
	{
		m_bShowEnter = bSet ? true : false;
	}
	virtual BOOL SetMarqueeOptions(CMarqueeOptions &o)
	{
		BOOL res = TRUE;

		res = m_ini.WriteInt("general","mq_elapse",o.m_elapse);
		if(res){
			res = m_ini.WriteInt("general","mq_bkclr",o.m_clrBack);
			if(res)
				res = m_ini.WriteInt("general","mq_txtclr",o.m_clrTxt);
		}

		return res;
	}
	virtual BOOL GetMarqueeOptions(CMarqueeOptions &options)
	{
		options.m_elapse = m_ini.GetInt("general","mq_elapse",4);
		options.m_clrBack = m_ini.GetInt("general","mq_bkclr",0x008000);
		options.m_clrTxt = m_ini.GetInt("general","mq_txtclr",0xffffff);
		return TRUE;
	}

};


class CAccountOptions : public IAccountOptions
{
	NO5TL::CPrivateIniFile m_ini;
	CString m_acc;
public:
	virtual BOOL SetFile(LPCTSTR pFullPath,LPCTSTR name)
	{
		m_acc = name;
		return m_ini.Create(pFullPath);
	}
	//
	virtual CString GetPassword(void)
	{
		NO5TL::SimpleEncryptedData data;
		CString res;

		// read len
		data.m_len = m_ini.GetInt(m_acc,"pwlen",0);
		if(data.m_len){
			data.m_buf = new char[data.m_len];
			ZeroMemory(data.m_buf,data.m_len);
			if(m_ini.GetStruct(m_acc,"password",(void *)data.m_buf,data.m_len)){
				LPSTR p = res.GetBuffer(data.m_len);	// didnt need to be so big
				ZeroMemory(p,data.m_len);
				NO5TL::Decrypt(data,p);
				res.ReleaseBuffer();
			}
		}
		return res;
	}
	virtual CString GetLastRoom(void)
	{
		return m_ini.GetString(m_acc,"last_room","",80);
	}
	virtual CString GetName(void)
	{
		return m_acc;
	}
	virtual CString GetColorSetName(void)
	{
		return m_ini.GetString(m_acc,"color_set","",80);
	}
	virtual void GetFontOptions(FontAll &font)
	{
		CString s;

		s = m_ini.GetString(m_acc,"font_options","",128);
		if(!s.IsEmpty()){
			BOOL res = font.FromString(s);
			ATLASSERT(res);
		}
	}
	virtual BOOL GetFade(void)
	{
		UINT u = m_ini.GetInt(m_acc,"fade",0);
		return u != 0;
	}
	virtual BOOL GetAlt(void)
	{
		UINT u = m_ini.GetInt(m_acc,"alt",0);
		return u != 0;
	}

	//
	virtual BOOL SetPassword(LPCTSTR pw)
	{
		NO5TL::SimpleEncryptedData data;
		NO5TL::Encrypt(pw,data);
		BOOL res = m_ini.WriteInt(m_acc,"pwlen",data.m_len);
		if(res)
			res =  m_ini.WriteStruct(m_acc,"password",data.m_buf,data.m_len);	
		return res;
	}
	virtual BOOL SetLastRoom(LPCTSTR room)
	{
		return m_ini.WriteString(m_acc,"last_room",room);
	}
	virtual void SetColorSetName(LPCTSTR name)
	{
		BOOL res = m_ini.WriteString(m_acc,"color_set",name);
		ATLASSERT(res);
	}
	virtual void SetFontOptions(FontAll &font)
	{
		CString s = font.ToString();
		BOOL res = m_ini.WriteString(m_acc,"font_options",s);
		ATLASSERT(res);
	}
	virtual void SetFade(BOOL bSet)
	{
		int i = bSet ? 1 : 0;
		m_ini.WriteInt(m_acc,"fade",i);
	}
	virtual void SetAlt(BOOL bSet)
	{
		int i = bSet ? 1 : 0;
		m_ini.WriteInt(m_acc,"alt",i);
	}

};

// static 
IGeneralOptions * IGeneralOptions::CreateMe(void)
{
	return new CGeneralOptions();
}

// static
void IGeneralOptions::DestroyMe(IGeneralOptions **pp)
{
	ATLASSERT(pp);
	if(pp && NULL != *pp){
		delete (CGeneralOptions *)(*pp);
		*pp = NULL;
	}
}

// static 
IAccountOptions * IAccountOptions::CreateMe(void)
{
	return new CAccountOptions();
}

// static
void IAccountOptions::DestroyMe(IAccountOptions **pp)
{
	ATLASSERT(pp);
	if(pp && NULL != *pp){
		delete (CAccountOptions *)(*pp);
		*pp = NULL;
	}
}

// IVisualOptions - CVisualOptions

class CVisualOptions : public IVisualOptions
{
	NO5TL::CPrivateIniFile m_ini;
	bool m_dirty;
	CString m_file;

	enum
	{
		NAME_USR,
		NAME_BUD,
		NAME_ME,
		TXT_USR,
		TXT_BUD,
		TXT_ME,
		TXT_THOUGHT,
		TXT_EMOTE,
		TXT_LNK,
		TXT_PVT,
		TXT_INV,
		TXT_AWAYUSR,
		TXT_AWAYBUD,
		TXT_BUDON,
		TXT_ENTERUSR,
		TXT_ENTERBUD,
		TXT_LEAVEUSR,
		TXT_LEAVEBUD,
		TXT_ERR,
		TXT_NO5,
		TXT_MAIL,
		TXT_YAH,
		COUNT_MARK
	};

	// these values are used all the time so we cant keep reading
	// from file, instead we read all at once and keep them in memory
	CYahooColor m_clrBack;
	FontAll m_fonts[COUNT_MARK];

	BOOL SaveAll(void)
	{
		BOOL res = TRUE;
		CString key;
		BOOL tmp = TRUE;
		
		for(UINT i = 0;i<COUNT_MARK;i++){
			key.Empty();
			key.Append(i);
			tmp = m_ini.WriteString(_T("style"),key,
				m_fonts[i].ToString());
			ATLASSERT(tmp);
			if(!tmp)
				res = FALSE;
		}
		res = m_ini.WriteString(_T("style"),_T("bkcolor"),
			m_clrBack.GetString(CYahooColor::YCSF_2));
		if(res)
			m_dirty = false;

		return res;
	}

	BOOL LoadAll(void)
	{
		CString s;
		BOOL res = TRUE;
		BOOL tmp;
		CString key;

		for(UINT i = 0;i< COUNT_MARK;i++){
			key.Empty();
			key.Append(i);

			s = m_ini.GetString(_T("style"),key,"",256);
			if(!s.IsEmpty()){
				tmp = m_fonts[i].FromString(s);
				if(!tmp){
					// load a default value for this key - TODO
					res = FALSE;
				}
			}
			else
				tmp = FALSE;
		}
		// background color
		CString sColor = m_ini.GetString(_T("style"),_T("bkcolor"),
			DefBkColor.GetString(CYahooColor::YCSF_2),16);
		if(!m_clrBack.SetFromString(sColor))
			m_clrBack = DefBkColor;

		if(!tmp){
			LoadDefaults();
			m_dirty = true;
		}

		return res;
	}
	
public:
	CVisualOptions()
	{
		m_dirty = false;
	}
	virtual ~CVisualOptions()
	{
		if(m_dirty){
			BOOL res = SaveAll();
			ATLASSERT(res);
		}
	}
	virtual BOOL SetFile(LPCTSTR file)
	{
		BOOL res;

		res = m_ini.Create(file);
		if(res){
			m_file = file;
			res = LoadAll();
		}
		return res;
	}
	virtual bool Dirty(void) const
	{
		return m_dirty;
	}
	virtual void Dirty(bool bSet)
	{
		m_dirty = bSet;
	}
	virtual void LoadDefaults(void)
	{
		m_clrBack = DefBkColor;

		m_fonts[0].Set(NO5_DEF_FACE,8,CColor(0xc0,0x10,0x20),m_clrBack,true,false,false);
		m_fonts[1].Set(NO5_DEF_FACE,8,TEAL,m_clrBack,true,false,false);
		m_fonts[2].Set(NO5_DEF_FACE,8,CColor(0,0x40,0xff),m_clrBack,true,false,false);
		m_fonts[3].Set(NO5_DEF_FACE,DefSize,DefColor,m_clrBack,false,false,false);
		m_fonts[4].Set(NO5_DEF_FACE,DefSize,DefColor,m_clrBack,false,false,false);
		m_fonts[5].Set(NO5_DEF_FACE,DefSize,DefColor,m_clrBack,false,false,false);
		m_fonts[6].Set(NO5_DEF_FACE,DefSize,DefColor,m_clrBack,false,false,false);
		m_fonts[7].Set(NO5_DEF_FACE,DefSize,PURPLE,m_clrBack,false,false,false);
		m_fonts[8].Set(NO5_DEF_FACE,DefSize,BLUE,m_clrBack,false,false,true);
		m_fonts[9].Set(NO5_DEF_FACE,DefSize,FUCHSIA,m_clrBack,false,false,false);
		m_fonts[10].Set(NO5_DEF_FACE,8,FUCHSIA,m_clrBack,false,false,false);
		m_fonts[11].Set(NO5_DEF_FACE,8,GREEN,m_clrBack,false,false,false);
		m_fonts[12].Set(NO5_DEF_FACE,8,GREEN,m_clrBack,false,false,false);
		m_fonts[13].Set(NO5_DEF_FACE,8,GREEN,m_clrBack,false,false,false);
		m_fonts[14].Set(NO5_DEF_FACE,8,TEAL,m_clrBack,false,false,false);
		m_fonts[15].Set(NO5_DEF_FACE,8,TEAL,m_clrBack,false,false,false);
		m_fonts[16].Set(NO5_DEF_FACE,8,TEAL,m_clrBack,false,false,false);
		m_fonts[17].Set(NO5_DEF_FACE,8,TEAL,m_clrBack,false,false,false);
		m_fonts[18].Set(NO5_DEF_FACE,8,RED,m_clrBack,false,false,false);
		m_fonts[19].Set(NO5_DEF_FACE,8,BLACK,m_clrBack,true,false,false);
		m_fonts[20].Set(NO5_DEF_FACE,8,GREY,m_clrBack,false,false,false);
		m_fonts[21].Set(NO5_DEF_FACE,8,BLACK,m_clrBack,false,false,false);
		
	}

	// getting
	virtual CColor ClrBack(void) const
	{
		CColor res = COLORREF(m_clrBack);
		return res;
	}
	virtual FontAll & NameUser(void)
	{
		return m_fonts[NAME_USR];
	}
	virtual FontAll & NameBuddy(void)
	{
		return m_fonts[NAME_BUD];
	}
	virtual FontAll & NameMe(void)
	{
		return m_fonts[NAME_ME];
	}
	virtual FontAll & TxtUser(void)
	{
		return m_fonts[TXT_USR];
	}
	virtual FontAll & TxtBuddy(void)
	{
		return m_fonts[TXT_BUD];
	}
	virtual FontAll & TxtMe(void)
	{
		return m_fonts[TXT_ME];
	}
	virtual FontAll & TxtThought(void)
	{
		return m_fonts[TXT_THOUGHT];
	}
	virtual FontAll & TxtEmote(void)
	{
		return m_fonts[TXT_EMOTE];
	}
	virtual FontAll & TxtLink(void)
	{
		return m_fonts[TXT_LNK];
	}
	virtual FontAll & TxtPvt(void)
	{
		return m_fonts[TXT_PVT];
	}
	virtual FontAll & TxtInvite(void)
	{
		return m_fonts[TXT_INV];
	}
	virtual FontAll & TxtAwayUsr(void)
	{
		return m_fonts[TXT_AWAYUSR];
	}
	virtual FontAll & TxtAwayBuddy(void)
	{
		return m_fonts[TXT_AWAYBUD];
	}
	virtual FontAll & TxtBuddyOnline(void)
	{
		return m_fonts[TXT_BUDON];
	}
	virtual FontAll & TxtEnterUsr(void)
	{
		return m_fonts[TXT_ENTERUSR];
	}
	virtual FontAll & TxtEnterBuddy(void)
	{
		return m_fonts[TXT_ENTERBUD];
	}
	virtual FontAll & TxtLeaveUsr(void)
	{
		return m_fonts[TXT_LEAVEUSR];
	}
	virtual FontAll & TxtLeaveBuddy(void)
	{
		return m_fonts[TXT_LEAVEBUD];
	}
	virtual FontAll & TxtError(void)
	{
		return m_fonts[TXT_ERR];
	}
	virtual FontAll & TxtNo5(void)
	{
		return m_fonts[TXT_NO5];
	}
	virtual FontAll & TxtMail(void)
	{
		return m_fonts[TXT_MAIL];
	}
	virtual FontAll & TxtYahoo(void)
	{
		return m_fonts[TXT_YAH];
	}
	
	// setting
	virtual void ClrBack(COLORREF clr)
	{
		m_clrBack = clr;
	}
	virtual void NameUser(FontAll &f)
	{
		m_fonts[NAME_USR] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void NameBuddy(FontAll &f)
	{
		m_fonts[NAME_BUD] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void NameMe(FontAll &f)
	{
		m_fonts[NAME_ME] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtUser(FontAll &f)
	{
		m_fonts[TXT_USR] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtBuddy(FontAll &f)
	{
		m_fonts[TXT_BUD] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtMe(FontAll &f)
	{
		m_fonts[TXT_ME] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtThought(FontAll &f)
	{
		m_fonts[TXT_THOUGHT] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtEmote(FontAll &f)
	{
		m_fonts[TXT_EMOTE] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtLink(FontAll &f)
	{
		m_fonts[TXT_LNK] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtPvt(FontAll &f)
	{
		m_fonts[TXT_PVT] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtInvite(FontAll &f)
	{
		m_fonts[TXT_INV] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtAwayUsr(FontAll &f)
	{
		m_fonts[TXT_AWAYUSR] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtAwayBuddy(FontAll &f)
	{
		m_fonts[TXT_AWAYBUD] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtBuddyOnline(FontAll &f)
	{
		m_fonts[TXT_BUDON] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtEnterUsr(FontAll &f)
	{
		m_fonts[TXT_ENTERUSR] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtEnterBuddy(FontAll &f)
	{
		m_fonts[TXT_ENTERBUD] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtLeaveUsr(FontAll &f)
	{
		m_fonts[TXT_LEAVEUSR] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtLeaveBuddy(FontAll &f)
	{
		m_fonts[TXT_LEAVEBUD] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtError(FontAll &f)
	{
		m_fonts[TXT_ERR] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtNo5(FontAll &f)
	{
		m_fonts[TXT_NO5] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtMail(FontAll &f)
	{
		m_fonts[TXT_MAIL] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void TxtYahoo(FontAll &f)
	{
		m_fonts[TXT_YAH] = f;
		//m_dirty[NAME_USR] = true;
		m_dirty = true;
	}
	virtual void CopyFrom(IVisualOptions *p)
	{
		CVisualOptions *pFrom = (CVisualOptions *)p;

		m_clrBack = pFrom->m_clrBack;
		for(int i=0;i<COUNT_MARK;i++)
			m_fonts[i] = pFrom->m_fonts[i];
		Dirty(true);
	}
};

// static
IVisualOptions * IVisualOptions::CreateMe(void)
{
	return new CVisualOptions();
}

// static
void IVisualOptions::DestroyMe(IVisualOptions **pp)
{
	if(pp && *pp){
		delete (CVisualOptions *)(*pp);
		*pp = NULL;
	}
}

