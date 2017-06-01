// TextStream.h: interface for the CTextStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTSTREAM_H__F26C05AB_40EE_11D9_A17B_9ACFB300C043__INCLUDED_)
#define AFX_TEXTSTREAM_H__F26C05AB_40EE_11D9_A17B_9ACFB300C043__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLBASE_H__
#error textstream.h requires atlbase.h
#endif
#ifndef __ATLMISC_H__
#error textstream.h requires atlmisc.h
#endif

#include <no5tl\no5tlbase.h>	// copy array, no5simplemap
#include <no5tl\color.h>	
#include <no5tl\mystring.h>		// FontAll::FromString
#include "yahoocolor.h"
#include "infotag.h"

using NO5TL::CopySimpleArray;

/*
	parsing text from chat:

	scan each char, if char is the beginning of a tag, that is,
	ch == '\x1b' || ch == '<'  then send any plain text
	to the stream and parse the text, sending the result to the stream

	Why do we need to abstract the stream insteand of sending directly
	to the destination ( rich edit control, for example ) ?
	Because the later alogirithm is:
	if(plain text)
		if(alt == true)
			send to text to dest using saved colors. ok
		else if(fade == true)
			*problem*
			calculate the fade colors based on the lenght of the text
			send text using calculated colors
		else
			send text
	else if(tag != alt,fade)
		activates the desired effect in the dest
	else if(tag == fade,alt)
		save colors, turn on flags

	the problem is the fade colours are calculated on the next
	text send and not on the whole text. if there are tags in the middle
	of the text, the result will be wrong
	exemple:
	<fade ...>abc<i>def</fade>
	the fade should be calculated based on the lenght "abcdef"
	but in the previous algorithm it will calculate based on abc
	then send it, then calculate again for def

	other problem: we want to use the parsing code to parse the 
	text that we will SEND to the chat. and in previous code
	is not usefull for that

	the text stream algorithm:

	add text with codes to the stream
	scan each char, if ch == tag, add previous text to stream
	and then parse the tag and add result , as an internal code,
	to the stream
	translate the resulting stream to desired code format
*/

namespace NO5YAHOO
{

enum TextAtom
{
	ATOM_TEXT,	// plain text
	ATOM_CHAR,	// a single char
	ATOM_BOLD,
	ATOM_ITALIC,
	ATOM_UNDER,
	ATOM_COLOR,
	ATOM_FONT,	// font face and/or size
	ATOM_FADE,
	ATOM_ALT,
	ATOM_BREAK,	// line break
	ATOM_SMILEY,
	ATOM_LINK,
	ATOM_CHARSET,
	ATOM_BKCOLOR,
	ATOM_HTML,		//html text. meaning: do not parse
	ATOM_BADTAG,	// badly formatted tags, like color tags
	ATOM_TATTOO,
	ATOM_NONE,
};

// represents a text effect that we want to turn on or off , or
// a plain text
class CTextAtom
{
public:
	struct FontInfo
	{
		CString m_face;
		int m_size;
		CInfoTag m_inf;
		COLORREF m_color;

		FontInfo(void)
		{
			m_size = 0;
			m_color = CLR_INVALID;
		}
		FontInfo(LPCTSTR face):m_face(face),m_size(0),
			m_color(CLR_INVALID)
		{
			//
		}
		FontInfo(int size):m_size(size),m_color(CLR_INVALID)
		{
			//
		}
		FontInfo(CInfoTag &inf):m_size(0),m_inf(inf),
			m_color(CLR_INVALID)
		{
			//
		}
		bool HasInfo(void) const
		{
			return m_inf.GetMap().GetSize() != 0;
		}
		bool HasFace(void) const
		{
			return !m_face.IsEmpty();
		}
		bool HasSize(void) const
		{
			return m_size > 0;
		}
		bool HasColor(void) const
		{
			return m_color != CLR_INVALID;
		}
	};

	int m_type;	// one of the TextAtom values
	bool m_bSet;
	
	union{
		TCHAR m_ch;				// a single character
		COLORREF m_color;
		FontInfo *m_pFont;
		CSimpleArray<CYahooColor> *m_pColors;
		CString *m_pText;
		BYTE m_charset;
		LPHANDLE m_nothing;
		UINT m_badtype;
	};

	void Zero(void)
	{
		m_bSet  = false;
		m_nothing = NULL;	// zero the union
		m_type = ATOM_NONE;
	}
	CTextAtom & CopyFrom(const CTextAtom &from);
public:
	CTextAtom(void)
	{
		Zero();
	}
	CTextAtom(const CTextAtom &atom)
	{
		Zero();
		CopyFrom(atom);
	}

	CTextAtom(LPCTSTR text)
	{
		Zero();
		AtomText(text);
	}
	CTextAtom(TCHAR c)
	{
		Zero();
		m_ch = c;
		m_type = ATOM_CHAR;
	}
	CTextAtom(COLORREF cr)
	{
		Zero();
		m_color = cr;
		m_type = ATOM_COLOR;
	}
	CTextAtom(LPCTSTR face,int size)
	{
		Zero();
		AtomFont(face,size);
	}
	virtual ~CTextAtom()
	{
		Destroy();
	}
	// destroy and zero the atom
	void Destroy(void);

	CTextAtom & operator = (const CTextAtom &atom)
	{
		return CopyFrom(atom);
	}

	int GetType(void) const
	{
		return m_type;
	}
	const CTextAtom & AtomText(LPCTSTR p);
	const CTextAtom & AtomChar(TCHAR c)
	{
		if(m_type != ATOM_CHAR){
			Destroy();
			m_type = ATOM_CHAR;
		}			
		m_ch = c;
		return *this;
	}
	const CTextAtom & AtomBreak(void)
	{
		if(m_type != ATOM_BREAK){
			Destroy();
			m_type = ATOM_BREAK;
		}
		return *this;
	}
	const CTextAtom & AtomColor(COLORREF cr)
	{
		if(m_type != ATOM_COLOR){
			Destroy();
			m_type = ATOM_COLOR;
		}			
		m_color = cr;
		return *this;
	}
	const CTextAtom & AtomBkColor(COLORREF cr)
	{
		if(m_type != ATOM_BKCOLOR){
			Destroy();
			m_type = ATOM_BKCOLOR;
		}			
		m_color = cr;
		return *this;
	}

	const CTextAtom & AtomFont(LPCTSTR face,int size,CInfoTag\
		*pinf = NULL,COLORREF cr = CLR_INVALID)
	{
		if(m_type != ATOM_FONT){
			Destroy();
			m_type = ATOM_FONT;
		}
		if(!m_pFont)
			m_pFont = new FontInfo();
		if(face && lstrlen(face))
			m_pFont->m_face = face;
		if(size > 0)
			m_pFont->m_size = size;
		if(cr != CLR_INVALID)
			m_pFont->m_color = cr;
		if(pinf != NULL)
			m_pFont->m_inf = *pinf;
		
		m_bSet = true;
		return *this;
	}

	const CTextAtom & AtomFontOff(void)
	{
		Destroy();
		m_type = ATOM_FONT;
		m_bSet = false;
		return *this;
	}
	const CTextAtom & AtomTattoo(LPCTSTR face,int size,COLORREF cr = CLR_INVALID)
	{
		if(m_type != ATOM_TATTOO){
			Destroy();
			m_type = ATOM_TATTOO;
		}
		if(!m_pFont)
			m_pFont = new FontInfo();
		if(face && lstrlen(face))
			m_pFont->m_face = face;
		if(size > 0)
			m_pFont->m_size = size;
		if(cr != CLR_INVALID)
			m_pFont->m_color = cr;
		m_bSet = true;
		return *this;
	}
	const CTextAtom & AtomBold(BOOL set)
	{
		Destroy();
		m_bSet = set ? true:false;
		m_type = ATOM_BOLD;
		return *this;
	}
	const CTextAtom & AtomItalic(BOOL set)
	{
		Destroy();
		m_bSet = set ? true:false;
		m_type = ATOM_ITALIC;
		return *this;
	}
	const CTextAtom & AtomUnder(BOOL set)
	{
		Destroy();
		m_bSet = set ? true:false;
		m_type = ATOM_UNDER;
		return *this;
	}
	const CTextAtom & AtomLink(BOOL set)
	{
		Destroy();
		m_bSet = set ? true:false;
		m_type = ATOM_LINK;
		return *this;
	}
	const CTextAtom & AtomLink(LPCTSTR p)
	{
		Destroy();
		m_type = ATOM_LINK;
		if(!m_pText)
			m_pText = new CString;
		if(p){
			*m_pText = p;
		}
		m_bSet = true;
		return *this;
	}
	const CTextAtom & AtomAlt(CSimpleArray<CYahooColor> &colors)
	{
		if(m_type != ATOM_ALT){
			Destroy();
			m_type = ATOM_ALT;
		}
		if(!m_pColors)
			m_pColors = new CSimpleArray<CYahooColor>;
		NO5TL::CopySimpleArray(*m_pColors,colors);
		m_bSet = true;
		return *this;
	}
	const CTextAtom & AtomAltOff(void)
	{
		Destroy();
		m_bSet = false;
		m_type = ATOM_ALT;
		return *this;
	}
	const CTextAtom & AtomFade(CSimpleArray<CYahooColor> &colors)
	{
		if(m_type != ATOM_FADE){
			Destroy();
			m_type = ATOM_FADE;
		}
		if(!m_pColors)
			m_pColors = new CSimpleArray<CYahooColor>;
		NO5TL::CopySimpleArray(*m_pColors,colors);
		m_bSet = true;
		return *this;
	}
	const CTextAtom & AtomFadeOff(void)
	{
		Destroy();
		m_bSet = false;
		m_type = ATOM_FADE;
		return *this;
	}
	const CTextAtom & AtomSmiley(LPCTSTR smiley)
	{
		if(m_type != ATOM_SMILEY){
			Destroy();
			m_type = ATOM_SMILEY;
		}
		if(!m_pText){
			m_pText = new CString();
		}
		*m_pText = smiley;
		return *this;
	}
	const CTextAtom & AtomCharset(BYTE bCharset)
	{
		Destroy();
		m_bSet = true;
		m_type = ATOM_CHARSET;
		m_charset = bCharset;
		return *this;
	}
	const CTextAtom & AtomHtml(LPCTSTR p)
	{
		if(m_type != ATOM_HTML){
			Destroy();
			m_type = ATOM_HTML;
		}
		if(!m_pText)
			m_pText = new CString;
		if(p){
			*m_pText = p;
		}
		return *this;
	}
	const CTextAtom & AtomBadtag(TextAtom type)
	{
		Destroy();
		m_type = ATOM_BADTAG;
		m_badtype = (UINT)type;
		return *this;
	}
	
	// parsing
	bool ParseYahooTag(LPCTSTR p);
	bool ParseInlineTag(LPCTSTR tag);
	// coding
	CString GetYahooCode(void) const;
	CString GetInlineCode(void) const;
private:
	CString MakeFadeAltTag(void) const;
	CString MakeFontTag(void) const;
	bool HandleInfoTag(void) const;
};

class CTextStream
{
	// used to parse tags in EnumTags
	struct TagParser
	{
		CTextStream *pOutter;

		TagParser(CTextStream *pThis):pOutter(pThis)
		{
			//
		}
		bool operator () (LPCTSTR tag)
		{
			CTextAtom atom;
			bool res = false;

			if(tag[0] == '\x1b')
				res = atom.ParseYahooTag(tag);
			if(tag[0] == '<')
 				res = atom.ParseInlineTag(tag);

			if(res){
				pOutter->AddAtom(atom);
			}
			return res;
		}
	};
	friend TagParser;

	enum StreaMode
	{
		MODE_YAHOO,		// parse any tag that yahoo might send
		MODE_INLINE,	// parse tags <> from input line
		MODE_TEXT,		// plain text mode. parse break lines and smileys ?
	};

	CTextStream(const CTextStream &)
	{
		ATLASSERT(0 && "todo");
	}
	CTextStream & operator = (const CTextStream &)
	{
		ATLASSERT(0 && "todo");
		return *this;
	}
	// add text [p,q[ looking for smileys
	void AddTextParsingSmileys(LPCTSTR p,LPCTSTR q);
	// this method parses the plain-text enumerated, scanning
	// for line breaks
	void ParsePlainText(LPCTSTR p,LPCTSTR q);

protected:
	CString m_FontFace;			// default font face
	int m_FontSize;				// default font size
	CYahooColor m_FontColor;	// default font color
	bool m_fade,m_alt;
	CSimpleArray<CTextAtom> m_atoms;
		// save the fade/alt because we may want to "clone" it
	CSimpleArray<CYahooColor> m_colors;	
		// stores the info tag
	CInfoTag m_inf;
		
	ULONG m_breaks;		// number of line breaks added
	ULONG m_links;		// number of links added
	ULONG m_smileys;	// number of smileys added
	ULONG m_badtags;	// bad tags counter
	short m_FadeFactor;	// in percent
	int m_mode;
	BYTE m_charset;	// last charset added
public:
	CTextStream();
	virtual ~CTextStream();
	void SetDefaults(LPCTSTR FontFace,int FontSize,COLORREF cr)
	{
		m_FontFace = FontFace;
		m_FontSize = FontSize;
		m_FontColor = cr;
	}
	void AddDefaults(void)
	{
		CTextAtom atom;

		AddAtom(atom.AtomCharset(DEFAULT_CHARSET));
		if(m_FontSize != 0 && !m_FontFace.IsEmpty())
			AddAtom(atom.AtomFont(m_FontFace,m_FontSize));
		if(CLR_INVALID != m_FontColor )
			AddAtom(atom.AtomColor(m_FontColor));
	}
	// reset to default values
	void Reset(void)
	{
		m_fade = m_alt = false;
		m_atoms.RemoveAll();
		m_inf.Reset();
		m_colors.RemoveAll();
		m_breaks = 0;
		m_links = 0;
		m_smileys = 0;
		m_badtags = 0;
		m_FadeFactor = 100;
		m_mode = MODE_YAHOO;
		m_charset = DEFAULT_CHARSET;
		m_FontSize = 0;
		m_FontFace.Empty();
		m_FontColor = CLR_INVALID;
	}

	void AddYahooText(LPCTSTR text);
	void AddPlainText(LPCTSTR text);
	void AddInlineText(LPCTSTR text);
	
	// called by EnumTags with text that is not between tags
	bool operator () (LPCTSTR p)
	{
		AddPlainText(p);
		return true;
	}
	

	CInfoTag & GetInfoTag(void)
	{
		return m_inf;
	}
	CTextStream & AddAtom(const CTextAtom &atom);

	// encoding
		// [begin,end[
	CString GetPlainText(int AtomBegin = 0,int AtomEnd = -1) const;
	// return plain text including smileys
	CString GetPlainText2(int AtomBegin = 0,int AtomEnd = -1) const;
	CString GetYahooText(int AtomBegin = 0,int AtomEnd = -1) const;
	CString GetInlineText(int AtomBegin = 0,int AtomEnd = -1) const;

	// send stream to destination, for example, rich edit
	virtual void SendToDest(void) = 0;
	virtual void ScrollText(void) = 0;
	virtual bool IsSmiley(LPCTSTR p,CString &smiley) = 0;
	virtual bool IsLink(LPCTSTR p,CString &link);

	// operators
	CTextStream & operator << ( CColor color)
	{
		CTextAtom atom;
		return AddAtom(atom.AtomColor(color));
	}
	CTextStream & operator << (LPCTSTR text)
	{
		switch(m_mode){
			case MODE_YAHOO:
				AddYahooText(text);
				break;
			case MODE_INLINE:
				AddInlineText(text);
				break;
			case MODE_TEXT:
				AddPlainText(text);
				break;
			default:
				ATLASSERT(0);
				break;
		}
		return *this;
	}
	CTextStream & operator << (const CString &text)
	{
		return ((*this) << (LPCTSTR)text);
	}
	CTextStream & operator << (CTextStream & (*f)(CTextStream &ts))
	{
		return (*f)(*this);
	}

	friend CTextStream & Flush(CTextStream &ts)
	{
		ts.SendToDest();
		return ts;
	}
	friend CTextStream & Endl(CTextStream &ts)
	{
		return ts << "\r\n" << Flush;
	}
	friend CTextStream & YahooMode(CTextStream &ts)
	{
		ts.m_mode = MODE_YAHOO;
		return ts;
	}
	friend CTextStream & InlineMode(CTextStream &ts)
	{
		ts.m_mode = MODE_INLINE;
		return ts;
	}
	friend CTextStream & TextMode(CTextStream &ts)
	{
		ts.m_mode = MODE_TEXT;
		return ts;
	}
	CTextStream & AddHtmlAtom(LPCTSTR text)
	{
		CTextAtom atom;
		return AddAtom(atom.AtomHtml(text));
	}
	UINT GetLinkCount(void) const
	{
		return m_links;
	}
	UINT GetSmileyCount(void) const
	{
		return m_smileys;
	}
	UINT GetBreakCount(void) const
	{
		return m_breaks;
	}
	UINT GetBadtagCount(void) const
	{
		return m_badtags;
	}
	CTextStream & Append(CTextStream &ts);

protected:
	// [start,end[ 
	int GetPlainTextLength(int AtomStart = 0,int AtomEnd = -1) const;
	// atom start must be an ATOM_FADE with m_fade == true
	int GetFadeLen(int AtomStart) const;
	// returns the index of the first occurent of atom.m_type == m_type
	// searchs the range [AtomStart,AtomEnd[
	int FindAtom(int type,int AtomStart,int AtomEnd = -1) const;
};

template <class T>
inline CTextStream & operator << (CTextStream &ts,T &f)
{
	return f(ts);
}


// functors

struct Bold
{
	const BOOL m_on;
	Bold(BOOL on):m_on(on)
	{
		//
	}
	Bold(bool on):m_on( on ? TRUE : FALSE)
	{
		//
	}
	CTextStream & operator () (CTextStream &ts)
	{
		CTextAtom atom;
		return ts.AddAtom(atom.AtomBold(m_on));
	}
};
struct Italic
{
	const BOOL m_on;
	Italic(BOOL on):m_on(on)
	{
		//
	}
	Italic(bool on):m_on( on ? TRUE : FALSE)
	{
		//
	}
	CTextStream & operator () (CTextStream &ts)
	{
		CTextAtom atom;
		return ts.AddAtom(atom.AtomItalic(m_on));
	}
};
struct Under
{
	const BOOL m_on;
	Under(BOOL on):m_on(on)
	{
		//
	}
	Under(bool on):m_on( on ? TRUE : FALSE)
	{
		//
	}
	CTextStream & operator () (CTextStream &ts)
	{
		CTextAtom atom;
		return ts.AddAtom(atom.AtomUnder(m_on));
	}
};

struct Font
{
	CString m_face;
	const int m_size;
	const bool m_close;
	Font(LPCTSTR face,int size = 0):m_size(size),m_close(false)
	{
		m_face = face;
	}
	Font(int size):m_size(size),m_close(false)
	{
		//
	}
	Font(bool bClose):m_close(bClose),m_size(0)
	{
		//
	}
	CTextStream & operator () (CTextStream &ts)
	{
		CTextAtom atom;
		if(!m_close)
			return ts.AddAtom(atom.AtomFont(m_face,m_size));
		else
			return ts.AddAtom(atom.AtomFontOff());
	}
};


struct FontAll
{
	CString m_face;
	int m_size;
	COLORREF m_clr;
	COLORREF m_clrBack;	// text background color
	bool m_bold:1;
	bool m_italic:1;
	bool m_under:1;

	FontAll(void)
	{
		m_size = 0;
		m_clr = CLR_INVALID;
		m_clrBack = CLR_INVALID;
		m_bold = false;
		m_italic = false;
		m_under = false;
	}

	FontAll(LPCTSTR face,int size,COLORREF clr,COLORREF bkclr,
		bool bold,bool italic,bool under)
	{
		Set(face,size,clr,bkclr,bold,italic,under);
	}

	FontAll(const FontAll &rhs)
	{
		if(this != &rhs){
			Set(rhs.m_face,rhs.m_size,rhs.m_clr,rhs.m_clrBack,
				rhs.m_bold,rhs.m_italic,rhs.m_under);
		}
	}
	FontAll & operator = (const FontAll &rhs)
	{
		if(this != &rhs){
			Set(rhs.m_face,rhs.m_size,rhs.m_clr,rhs.m_clrBack,
				rhs.m_bold,rhs.m_italic,rhs.m_under);
		}
		return *this;
	}


	void Set(LPCTSTR face,int size,COLORREF clr,COLORREF bkclr,
		bool bold,bool italic,bool under)
	{
		m_face = face;
		m_size = size;
		m_clr = clr;
		m_clrBack = bkclr;
		m_bold = bold;
		m_italic = italic;
		m_under = under;
	}

	CTextStream & operator () (CTextStream &ts)
	{
		CTextAtom atom;
		ts.AddAtom(atom.AtomFont(m_face,m_size));
		atom.Destroy();
		ts.AddAtom(atom.AtomColor(m_clr));
		ts.AddAtom(atom.AtomBkColor(m_clrBack));
		ts.AddAtom(atom.AtomBold( m_bold ? TRUE:FALSE));
		ts.AddAtom(atom.AtomItalic( m_italic ? TRUE:FALSE));
		ts.AddAtom(atom.AtomUnder( m_under ? TRUE:FALSE));
		return ts;
	}
	// used to read and write to ini file
	CString ToString(void) const
	{
		CString s;
		CYahooColor clr = m_clr;
		CYahooColor clrBack = m_clrBack;

		s.Format(_T("%s;%02d;%s;%s;%1d;%1d;%1d"),
			(LPCTSTR)m_face,m_size,
			clr.GetString(CYahooColor::YCSF_2),
			clrBack.GetString(CYahooColor::YCSF_2),
			m_bold,m_italic,m_under);
		return s;
	}
	BOOL FromString(LPCTSTR s)
	{
		NO5TL::CStringToken st;
		CString next;
		int i = 0;
		CYahooColor color;

		st.Init(s,";",true);

		while(i <7){
			next = st.GetNext();
			if(next.IsEmpty())
				break;
			switch(i){
				case 0:
					m_face = next;
					break;
				case 1:
					m_size = atoi((LPCSTR)next);
					break;
				case 2:
					if(color.SetFromString(next))
						m_clr = color;
					break;
				case 3:
					if(color.SetFromString(next))
						m_clrBack = color;
					break;
				case 4:
					m_bold = atoi((LPCSTR)next) != 0 ? true:false;
					break;
				case 5:
					m_italic = atoi((LPCSTR)next) != 0 ? true : false;
					break;
				case 6:
					m_under = atoi((LPCSTR)next) != 0 ? true : false;
					break;
				default:
					break;
			}
			i++;
		}
		return i == 7 ? TRUE : FALSE;
	}
};

}	// NO5YAHOO


#endif // !defined(AFX_TEXTSTREAM_H__F26C05AB_40EE_11D9_A17B_9ACFB300C043__INCLUDED_)
