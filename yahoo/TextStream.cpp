// TextStream.cpp: implementation of the CTextStream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <no5tl\utils.h>
#include <no5tl\mystring.h>
#include <no5tl\colorfader.h>
#include <yahoo\myutf8.h>
#include "TextStream.h"
using namespace NO5YAHOO;
using namespace NO5TL;


static int ParseFadeTag(LPCTSTR tag,CSimpleArray<CYahooColor> &lst);
static int MakeColorList(const CSimpleArray<CString> &in,
						  CSimpleArray<CYahooColor> &out);
static void ParseKeyValuePairs(LPCTSTR text,CNo5SimpleMap<CString,CString> &pairs,
						CSimpleArray<CString> &props);
static int MaxStringLen(LPCTSTR a[],int count);


/****************** CTextAtom ************************/

void CTextAtom::Destroy(void)
{
	switch(m_type){
		case ATOM_FONT:
		case ATOM_TATTOO:
			if(m_bSet && m_pFont){
				delete m_pFont;
			}
			break;
		case ATOM_FADE:
		case ATOM_ALT:
			if(m_bSet && m_pColors){
				delete m_pColors;
			}
			break;
		case ATOM_TEXT:
		case ATOM_SMILEY:
		case ATOM_HTML:
			if(m_pText)
				delete m_pText;
			break;
		case ATOM_LINK:
			if(m_bSet && m_pText)
				delete m_pText;
			else if(!m_bSet)
				ATLASSERT(!m_pText);
			break;
		default:
			break;
	}
	Zero();
}

CTextAtom & CTextAtom::CopyFrom(const CTextAtom &from)
{
	if(this != &from){
		Destroy();
		m_type = from.m_type;
		m_bSet = from.m_bSet;
		switch(from.m_type){
			case ATOM_FONT:
			case ATOM_TATTOO:
				if(from.m_bSet){
					m_pFont = new FontInfo;
					if(from.m_pFont){
						*m_pFont = *from.m_pFont;
					}
				}
				break;
			case ATOM_ALT:
			case ATOM_FADE:
				if(from.m_bSet){
					m_pColors = new CSimpleArray<CYahooColor>;
					if(from.m_pColors){
						NO5TL::CopySimpleArray<CYahooColor>(*m_pColors,*(from.m_pColors));
					}
				}
				break;
			case ATOM_SMILEY:
			case ATOM_TEXT:
			case ATOM_HTML:
				m_pText = new CString();
				if(from.m_pText){
					*m_pText = *from.m_pText;
				}
				break;
			case ATOM_LINK:
				if(from.m_bSet){
					m_pText = new CString();
					if(from.m_pText){
						*m_pText = *from.m_pText;
					}
				}
				else{
					ATLASSERT(!from.m_pText); // debug;
				}
				break;
			default:
				m_nothing = from.m_nothing;
				break;
		}
	}
	return *this;
}

const CTextAtom & CTextAtom::AtomText(LPCTSTR p)
{
	if(m_type != ATOM_TEXT){
		Destroy();
		m_type = ATOM_TEXT;
	}
	if(!m_pText)
		m_pText = new CString;
	if(p){
		*m_pText = p;
	}
	return *this;
}

CString CTextAtom::GetYahooCode(void) const
{
	CYahooColor color;

	switch(m_type){
		case ATOM_BOLD:
			if(m_bSet)
				return CString("\x1b[1m");
			else
				return CString("\x1b[x1m");
		case ATOM_ITALIC:
			if(m_bSet)
				return CString("\x1b[2m");
			else
				return CString("\x1b[x2m");
		case ATOM_UNDER:
			if(m_bSet)
				return CString("\x1b[4m");
			else
				return CString("\x1b[x4m");
		case ATOM_COLOR:
			color = m_color;
			return color.GetString(CYahooColor::YCSF_4);
		case ATOM_FONT:
		case ATOM_TATTOO:
			return MakeFontTag();

		case ATOM_FADE:
			return MakeFadeAltTag();

		case ATOM_ALT:
			return MakeFadeAltTag();

		case ATOM_CHAR:
			return CString(1,m_ch);
		case ATOM_TEXT:
			if(m_pText)
				return *m_pText;
			else
				return CString();
		case ATOM_BREAK:
			return CString("\r\n");
		case ATOM_SMILEY:
			if(m_pText)
				return *m_pText;
			else
				return CString();
		
		case ATOM_LINK:	
			if(m_bSet && m_pText)
				return *m_pText;
		else
			return CString();

		default:
			return CString();
	}
}

CString CTextAtom::GetInlineCode(void) const
{
	CYahooColor color;

	switch(m_type){
		case ATOM_BOLD:
			if(m_bSet)
				return CString("<b>");
			else
				return CString("</b>");
		case ATOM_ITALIC:
			if(m_bSet)
				return CString("<i>");
			else
				return CString("</i>");
		case ATOM_UNDER:
			if(m_bSet)
				return CString("<u>");
			else
				return CString("</u>");
		case ATOM_COLOR:
			color = m_color;
			return color.GetString(CYahooColor::YCSF_1);
		case ATOM_FONT:
		case ATOM_TATTOO:
			return MakeFontTag();
		case ATOM_FADE:
			return MakeFadeAltTag();
		case ATOM_ALT:
			return MakeFadeAltTag();
		case ATOM_CHAR:
			return CString(1,m_ch);
		case ATOM_TEXT:
			if(m_pText)
				return *m_pText;
			else
				return CString();
		case ATOM_BREAK:
			return CString("\r\n");
		case ATOM_SMILEY:
			if(m_pText)
				return *m_pText;
			else
				return CString();
		case ATOM_LINK:	
			if(m_bSet && m_pText)
				return *m_pText;
			else
				return CString();
		default:
			return CString();
	}
}

CString CTextAtom::MakeFontTag(void) const
{
	CString res;
	char tmp[3] = {0};

	if(m_bSet){
		res += "<font";

		if(!m_pFont){
			res += '>';
			return res;
		}

		if(m_pFont->HasInfo()){
			res += ' ';
			res += m_pFont->m_inf.GetCode2();
		}
		if(!m_pFont->m_face.IsEmpty()){
			res += " face=\"";
			res += m_pFont->m_face;
			res += '\"';
		}
		if(m_pFont->m_size > 0){
			int size = m_pFont->m_size;

			res += " size=\"";
			if(size > 99){
				// we dont need size that long, and this will
				// prevent overflowing the buffer tmp[3]
				size = 99;
			}
			wsprintf(tmp,"%d\"",size);
			res += tmp;
		}
		if(m_type == ATOM_TATTOO)
			res += " tattoo ";
		// TODO: color in font tags
		res += '>';
	}
	else
		res = "</font>";

	return res;
}

CString CTextAtom::MakeFadeAltTag(void) const
{
	CString res;

	if(!m_bSet){
		if(m_type == ATOM_FADE)
			return CString("</fade>");
		else if(m_type == ATOM_ALT)
			return CString("</alt>");
		else{
			ATLASSERT(0);
		}
	}
	else{
		ATLASSERT(m_pFont != NULL);
		res += "<";
	}

	if(m_type == ATOM_FADE)
		res += "fade ";
	else
		res += "alt ";
	
	CSimpleArray<CYahooColor> &colors = *m_pColors;
	for(int i=0;i<colors.GetSize();i++){
		res += colors[i].GetString(CYahooColor::YCSF_2);
		if(i < colors.GetSize() - 1){
			res += ',';
		}
		else
			break;
	}
	res += '>';
		
	return res;
}

bool CTextAtom::ParseYahooTag(LPCTSTR p)
{
	BOOL bSet = TRUE;
	CYahooColor color;
	LPCTSTR tag = p;
	bool res = false;

	Destroy();

	if(*p != '\x1b')
		return false;
	if(*++p != '[')
		return false;
	if(*++p == 'x'){
		bSet = FALSE;
		p++;
	}
	if(*p == 'm'){
		return true;
	}
	if(*p == '#'){
		if(color.SetFromString(tag)){
			res = true;
			AtomColor(color);
		}
		else{
			AtomBadtag(ATOM_COLOR);
			res = true;
		}
		return res;
	}
	else if(*p == 'f'){
		if(!bSet){
			// i dont know if there are xf tags
			AtomFontOff();
			return true;
		}
		else{
			LPCTSTR q;

			// it has to be between double quotes
			p++;
			if(*p != '\"')
				return false;
			q = ++p;
			while(*q && *q != '\"' )
				q++;
			if(*q != '\"')
				return false;
			if(q > p){
				CString tmp;
				StringCopyN(tmp,p,0,q - p);
				AtomFont(tmp,0);
				return true;
			}
		}
	}
	else if(*p == 'l'){
		AtomLink(bSet);
		return true;
	}
	else if(isdigit(*p)){
		int i = atoi(p);

		if( i < 0)
			return false;
		res = true;

		switch(i){
			case 1:
				AtomBold(bSet);
				break;
			case 2:
				AtomItalic(bSet);
				break;
			case 3:
				//m_edit.SetStrike(bSet);
				break;
			case 4:
				AtomUnder(bSet);
				break;
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
				if(color.SetFromString(tag))
					AtomColor(color);
				else
					res = false;
				break;
			default:
				res = false;
				break;
		}
	}
	return res;
}

bool CTextAtom::ParseInlineTag(LPCTSTR tag)
{
	LPCTSTR p,q;
	CString tmp;
	bool res = false;
	BOOL bSet = TRUE;
	CYahooColor color;
	
	Destroy();
	p = q = tag;

	if(*q != '<')
		return false;

	q++;
	// skips eventual whitespaces ex: <  font
	while(::isspace(*q))
		q++;

	if(!*q)	// there was only opening and white spaces <\t\t\t
		return false;
	if(*q == '>')
		return false;	// <  >
	if(*q == '/'){
		bSet = FALSE;
		q++;
	}
	// check for a <#abcdef>
	else if(*q == '#'){
		if(color.SetFromString(tag)){
			AtomColor(color);
			return true;
		}
		else{
			AtomBadtag(ATOM_COLOR);
			return true;
		}
	}
	// check for color names like <red>
	else if(::isalpha(*q)){
		if(color.SetFromString(tag)){
			AtomColor(color);
			return true;
		}
	}

	p = q;
	// find next white space, signaling the end of the word
	while(*q && *q != '>' && !::isspace(*q))
		q++;

	// in < font size = "10" face="blah"> 
	// q would be after font and p in f
		
	if(!*q)
		return false;

	StringCopyN(tmp,p,0,q - p);

	if(!tmp.CompareNoCase("font")){	
		if(!bSet){
			AtomFontOff();
			res = true;
		}
		else{
			CNo5SimpleMap<CString,CString> pairs(false);
			CSimpleArray<CString> props;
			CString font;
			CString size;
			CString color;
			int nSize = -1;
			CYahooColor clr(CLR_INVALID);
			CInfoTag inf;
			CString tmp = q;
			
			tmp.Remove('>');
			ParseKeyValuePairs((LPCSTR)tmp,pairs,props);
			
			font = pairs.Lookup(CString("face"));
			size = pairs.Lookup(CString("size"));
			color = pairs.Lookup(CString("color"));

			if(!font.IsEmpty())
				res = true;

			if(!size.IsEmpty()){
				nSize = atoi(size);
				nSize = max(6,min(nSize,36));
				res = true;
			}
	
			if(!color.IsEmpty()){
				if(clr.SetFromString(color)){
					res = true;
				}
				else
					clr = CLR_INVALID;
			}

			// handle "properties" , that is, strings in the font
			// tag that are not key=value pairs
			if(props.GetSize()){
				if(!props[0].CompareNoCase("INF")){
					res = inf.Parse(props);
				}
				else if(!props[0].CompareNoCase("tattoo")){
					AtomTattoo(font,nSize,clr);
					return true;
				}
				else{
					// we will not add anything to the stream for
					// tags like <font blah>
					AtomBadtag(ATOM_FONT);
					return true;
				}
			}

			// finally set the font atom
			if(res){
				CInfoTag *pinf = NULL;

				if(inf.GetMap().GetSize()){
					pinf = &inf;
				}
				AtomFont(font,nSize,pinf,clr);
			}

		} // else
	} // font
	
	// we will eliminate errors like adding fade-on and alt-on
	// without closing the fade-on in the AddAtom method
	// or is better to handle it here ?
	else if(!tmp.CompareNoCase("fade")){
		res = true;
		if(bSet){
			CSimpleArray<CYahooColor> colors;
			
			if(ParseFadeTag(tag,colors)){
				m_bSet = true;
				AtomFade(colors);
			}
		}
		else{
			m_bSet = false;
			AtomFadeOff();
		}
	}
	else if(!tmp.CompareNoCase("alt")){
		res = true;
		if(bSet){
			CSimpleArray<CYahooColor> colors;

			if(ParseFadeTag(tag,colors)){
				m_bSet = true;
				AtomAlt(colors);
			}
		}
		else{
			m_bSet = false;
			AtomAltOff();
		}

	}
	else if(!tmp.CompareNoCase("b")){
		res = true;
		AtomBold(bSet);
	}
	else if(!tmp.CompareNoCase("i")){
		res = true;
		AtomItalic(bSet);
	}
	else if(!tmp.CompareNoCase("u")){
		res = true;
		AtomUnder(bSet);
	}
	/*
	else if(!tmp.CompareNoCase("n")){
		res = true;
		AtomBreak();
	}
	*/

	return res;
}

/********************* CTextStream ****************/

CTextStream::CTextStream()
{
	Reset();
}

CTextStream::~CTextStream()
{
	
}

void CTextStream::AddYahooText(LPCTSTR text)
{
	EnumTags3(text,"<\x1b",">m",*this,TagParser(this));
}

void CTextStream::AddInlineText(LPCTSTR text)
{
	EnumTags3(text,"<",">",*this,TagParser(this));
}

void CTextStream::AddPlainText(LPCTSTR text)
{
	LPCTSTR p,q;
	CTextAtom atom;
	CString tmp;
	CString smiley;
	CString s;

	p = q = text;

	while( * q ){
		// we add a break only if we find "\r\n"
		if( (*q == '\r') && (*(q + 1) == '\n') ){
			if(q > p){
				StringCopyN(tmp,p,0,q - p);
				AddAtom(atom.AtomText(tmp));
			}
			AddAtom(atom.AtomBreak());
			q += 2;
			p = q;
		}
		else if(*q == '\r' || *q == '\n'){
			if(q > p){
				StringCopyN(tmp,p,0,q - p);
				AddAtom(atom.AtomText(tmp));
			}
			p = ++q;
		}
		else if(IsSmiley(q,smiley)){
			if(q > p){
				StringCopyN(tmp,p,0,q - p);
				AddAtom(atom.AtomText(tmp));
			}
			AddAtom(atom.AtomSmiley(smiley));
			q += smiley.GetLength();
			p = q;
		}
		else if(IsLink(q,smiley)){
			if(q > p){
				StringCopyN(tmp,p,0,q - p);
				AddAtom(atom.AtomText(tmp));
			}
			p = q;
			AddAtom(atom.AtomLink(smiley));
			AddAtom(atom.AtomLink(FALSE));
			q += smiley.GetLength();
			p = q;
		}
		else
			q++;
	}
	if(q > p){
		StringCopyN(tmp,p,0,q - p);
		AddAtom(atom.AtomText(tmp));
	}
}

CTextStream & CTextStream::AddAtom(const CTextAtom &atom)
{
	bool bAdd = true;
	const int atom_type = atom.GetType();


	if(atom_type == ATOM_FADE){
		if(atom.m_bSet){
			if(m_fade || m_alt){
				// cant open without closing
				bAdd = false;
			}
			else
				m_fade = true;
		}
		else{
			// and closing without open
			if(!m_fade)
				bAdd = false;
			else
				m_fade = false;
		}
		if(bAdd && atom.m_bSet){
			// lets also save it in the stream
			if(atom.m_pColors){
				NO5TL::CopySimpleArray(m_colors,*(atom.m_pColors));
			}
		}
	}
	else if(atom_type == ATOM_ALT){
		if(atom.m_bSet){
			if(m_fade || m_alt){
				// cant open without closing
				bAdd = false;
			}
			else
				m_alt = true;
		}
		else{
			if(!m_fade)
				bAdd = false;
			else
				m_alt = false;
		}
		if(bAdd && atom.m_bSet){
			// lets also save it in the stream
			if(atom.m_pColors){
				NO5TL::CopySimpleArray(m_colors,*atom.m_pColors);
			}
		}
	}
	
	else if(atom_type == ATOM_FONT){

		if(atom.m_bSet){
			// save the info tag
			if(atom.m_pFont && atom.m_pFont->HasInfo()){
				m_inf = atom.m_pFont->m_inf;
			}
		}
		// TODO:
		// store font face and font size in the stream
		// in case we want to know it
	}
	else if(atom_type == ATOM_BREAK){
		m_breaks++;
	}
	else if(atom_type == ATOM_SMILEY){
		m_smileys++;
	}
	else if(atom_type == ATOM_LINK){
		if(atom.m_bSet)
			m_links++;
	}
	else if(atom_type == ATOM_BADTAG){
		m_badtags++;
	}

	if(bAdd){
		m_atoms.Add((CTextAtom &)atom);
		// we will add charset atoms here if needed
		if(atom.m_type == ATOM_FONT || atom.m_type == ATOM_TATTOO){
			if(atom.m_pFont && atom.m_pFont->HasFace()){
				CTextAtom atom2;
				if(CGetCharSet::IsSymbolCharSet(atom.m_pFont->m_face)){
					AddAtom(atom2.AtomCharset(SYMBOL_CHARSET));
				}
				else{
					AddAtom(atom2.AtomCharset(DEFAULT_CHARSET));
				}
			}
		}
		//m_atoms.Add((CTextAtom &)atom);
	}

	return *this;
}

CString CTextStream::GetPlainText(int AtomBegin,int AtomEnd) const
{
	int i;
	CString s;

	if(AtomEnd < 0 || AtomEnd > m_atoms.GetSize()){
		AtomEnd = m_atoms.GetSize();
	}
	ATLASSERT(AtomBegin <= AtomEnd);
	
	for(i = AtomBegin;i<AtomEnd;i++){
		switch(m_atoms[i].m_type){
			case ATOM_CHAR:
				s += m_atoms[i].m_ch;
				break;
			case ATOM_TEXT:
				if(m_atoms[i].m_pText){
					s += *(m_atoms[i].m_pText);
				}
				break;
			case ATOM_BREAK:
				s += "\r\n";
				break;
			default:
				break;
		}
	}
	return s;
}

CString CTextStream::GetPlainText2(int AtomBegin,int AtomEnd) const
{
	int i;
	CString s;

	if(AtomEnd < 0 || AtomEnd > m_atoms.GetSize()){
		AtomEnd = m_atoms.GetSize();
	}
	ATLASSERT(AtomBegin <= AtomEnd);
	
	for(i = AtomBegin;i<AtomEnd;i++){
		switch(m_atoms[i].m_type){
			case ATOM_CHAR:
				s += m_atoms[i].m_ch;
				break;
			case ATOM_TEXT:
				if(m_atoms[i].m_pText){
					s += *(m_atoms[i].m_pText);
				}
				break;
			case ATOM_BREAK:
				s += "\r\n";
				break;
			case ATOM_SMILEY:
				if(m_atoms[i].m_pText){
					s += *(m_atoms[i].m_pText);
				}
				break;
			case ATOM_LINK:
				if(m_atoms[i].m_bSet && m_atoms[i].m_pText){
					s += *(m_atoms[i].m_pText);
				}
				break;
			default:
				break;
		}
	}
	return s;
}

CString CTextStream::GetYahooText(int AtomBegin,int AtomEnd) const
{
	CString s;
	int i;

	if(AtomEnd < 0 || AtomEnd > m_atoms.GetSize()){
		AtomEnd = m_atoms.GetSize();
	}
	ATLASSERT(AtomBegin < AtomEnd);

	// add the info tag string, if we have one
	if(!m_inf.IsEmpty()){
		s += m_inf.GetCode();
	}

	for(i = AtomBegin;i<AtomEnd;i++){
		s += m_atoms[i].GetYahooCode();
	}
	return s;
}

CString CTextStream::GetInlineText(int AtomBegin,int AtomEnd) const
{
	CString s;
	int i;

	if(AtomEnd < 0 || AtomEnd > m_atoms.GetSize()){
		AtomEnd = m_atoms.GetSize();
	}
	ATLASSERT(AtomBegin < AtomEnd);

	for(i = AtomBegin;i<AtomEnd;i++){
		s += m_atoms[i].GetInlineCode();
	}
	return s;
}

int CTextStream::GetPlainTextLength(int AtomStart,int AtomEnd) const
{
	int i;
	int res = 0;

	if(AtomEnd < 0 || AtomEnd > m_atoms.GetSize()){
		AtomEnd = m_atoms.GetSize();
	}
	ATLASSERT(AtomStart < AtomEnd);

	for(i = AtomStart;i < AtomEnd;i++){
		switch(m_atoms[i].GetType()){
			case ATOM_TEXT:
				if(m_atoms[i].m_pText){
					res += m_atoms[i].m_pText->GetLength();
				}
				break;
			case ATOM_CHAR:
				res += 1;
				break;
		}
	}
	return res;
}

// used to get the length of the text between a begin-fade and
// an end-fade
// atom start must be an ATOM_FADE with m_fade == true
int CTextStream::GetFadeLen(int AtomStart) const
{
	int AtomEnd;

	ATLASSERT(m_atoms[AtomStart].GetType() == ATOM_FADE);
	ATLASSERT(m_atoms[AtomStart].m_bSet == true);
	// find the end of the fade
	AtomEnd = FindAtom(ATOM_FADE,AtomStart + 1);
	if(AtomEnd >= 0){
		ATLASSERT(m_atoms[AtomEnd].GetType() == ATOM_FADE);
		ATLASSERT(m_atoms[AtomEnd].m_bSet == false);
	}
	else
		AtomEnd = -1;
	return GetPlainTextLength(AtomStart,AtomEnd);
}
// returns the index of the first occurent of atom.m_type == m_type
// searchs the range [AtomStart,AtomEnd[
int CTextStream::FindAtom(int type,int AtomStart,int AtomEnd) const
{
	int i;

	if(AtomEnd < 0 || AtomEnd > m_atoms.GetSize()){
		AtomEnd = m_atoms.GetSize();
	}
	ATLASSERT(AtomStart < AtomEnd);

	for(i=AtomStart;i<AtomEnd;i++){
		if(m_atoms[i].GetType() == type)
			return i;
	}
	return -1;
}

// default implentation of IsLink
bool CTextStream::IsLink(LPCTSTR p,CString &link)
{
	LPCTSTR a[] = { _T("http://"),_T("www."),_T("mailto://")};
	const int count = sizeof(a)/sizeof(a[0]);
	int i;
	CString s;
	bool res = false;
	int max_len = MaxStringLen(a,count);

	StringCopyN(s,p,0,max_len);

	for(i=0; i < count && res == false ;i++){
		if(s.Find(a[i]) == 0)
			res = true;
	}

	if(res){
		LPCTSTR q = p;

		while(*q && !isspace(*q))
			q++;
		StringCopyN(link,p,0,(int)(q - p));
	}
	return res;
}

CTextStream & CTextStream::Append(CTextStream &ts)
{
	const int count = ts.m_atoms.GetSize();

	for(int i=0; i<count; i++){
		AddAtom(ts.m_atoms[i]);
	}
	return *this;
}


/************** local functions ***********************/

// return the number of colors added
int ParseFadeTag(LPCTSTR tag,CSimpleArray<CYahooColor> &lst)
{
	CStringToken st;
	CSimpleArray<CString> tmp;

	st.Init(tag,"<> ,");
	st.GetNext();
	st.GetAll(tmp);
	return MakeColorList(tmp,lst);
}

// return number of colors successfully converted and added to out
int MakeColorList(const CSimpleArray<CString> &in,CSimpleArray<CYahooColor> &out)
{
	const int count = in.GetSize();
	int i;
	CYahooColor color;
	int res = 0;

	for(i = 0;i<count;i++){
		if(color.SetFromString(in[i])){
			if(out.Add(color))
				res++;
		}
	}
	return res;
}

// parses [pStart,pEnd[
// example  key1="value 1" key2="blah bleh" key3="value 3"
// we always begin at a key or space
// there may  not be spaces between equal sign
// there has to be quotes around the value to be considered
// props (properties ) will return the strings that doenst contain a '='
// we need it because people use the font tag to other things
void ParseKeyValuePairs(LPCSTR text,CNo5SimpleMap<CString,CString> &pairs,CSimpleArray<CString> &props)
{
	LPCTSTR p,q;
	CString key,val;

	p = q = text;

	
	while(*q){
		while(::isspace(*q))
			q++;
		p = q; // begining of key
		while(*q && *q != '=' && !(::isspace(*q)))
			q++;
		if(!(*q) || ::isspace(*q)){
			// we found a something that is not a key=value pair
			StringCopyN(key,p,0,(int)(q - p));
			props.Add(key);
		}
		else if(*q == '='){ // we found a key
			// copy the key
			StringCopyN(key,p,0,(int)(q - p));
			p = ++q;	// first quote of value
			if(*q && *q == '\"'){ // value has to start with quote
				p = ++q;	// beginning of value
				while(*q && *q != '\"')
					q++;
				if(*q){
					StringCopyN(val,p,0,(int)(q - p));
					pairs.Add(key,val);
					q++;
				}
				else{
					if(*q)
						q++;
				}
			}
		}
		else {
			//
		}
	}
}



int MaxStringLen(LPCTSTR a[],int count)
{
	int res = 0;

	for(int i=0;i<count;i++){
		if(lstrlen(a[i]) > res)
			res = lstrlen(a[i]);
	}
	return res;
}





