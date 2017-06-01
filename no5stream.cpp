// no5stream.cpp: implementation of the no5stream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <no5tl\colorfader.h>
#include <yahoo\smileymap.h>
#include <yahoo\myutf8.h>
#include <no5tl\mystring.h>
#include "no5stream.h"
#include "no5app.h"
using namespace NO5TL;
using namespace NO5YAHOO;
typedef CSimpleArray<CYahooColor> ColorArray;

CString MakeUnicodeHtmlString(LPCWSTR p);

/******************* CInputStream ******************/
bool CInputStream::IsSmiley(LPCTSTR p,CString &smiley)
{
	ATLASSERT(g_app.GetSmileyMap()->IsLoaded());
	return g_app.GetSmileyMap()->IsSmiley(p,smiley) ? true : false;
}



/******************* CRichEditStream ******************/

// we well handle alt/fade here
void CRichEditStream::SendToDest()
{
	const int count = m_atoms.GetSize();
	register int i;
	ColorArray colors;
	int it = 0;
	CTextAtom atom;
	TCHAR ch[5] = {0};
	// estes valores nao tem a ver com this->m_fade e this->m_alt
	// os quais sao usados em AddAtom
	bool fade = false,alt = false;
	
	m_pEdit->SetSelEnd();

	//GetEdit().SendMessage(WM_SETREDRAW,(WPARAM)FALSE);
	for(i=0;i<count;i++){
		atom = m_atoms[i];

		switch(atom.GetType()){
			case ATOM_TEXT:
			{
				if(!atom.m_pText){
					ATLASSERT(0);
					break;
				}
				if(!fade && !alt){
					GetEdit().AppendText(*atom.m_pText);
				}
				else if(fade || alt){
					const int len = atom.m_pText->GetLength();
					for(int j=0;j<len;j++){
						GetEdit().SetTextColor(colors[it++]);
						if(it == colors.GetSize())
							it = 0;
						wsprintf(ch,"%c",atom.m_pText->GetAt(j));
						GetEdit().AppendText(ch);
					}
				}
				break;
			}
			case ATOM_CHAR:
				wsprintf(ch,"%c",atom.m_ch);
				if(fade || alt){
					GetEdit().SetTextColor(colors[it++]);
					if(it == colors.GetSize())
						it = 0;
				}
				GetEdit().AppendText(ch);
				break;
			case ATOM_BREAK:
				{
					GetEdit().AppendText("\r\n");
				}
				break;
			
			case ATOM_FADE:
			{
				CColorFader<CYahooColor> fader;

				colors.RemoveAll();
				if(atom.m_bSet){
					int n = GetFadeLen(i);

					if(!atom.m_pColors){
						ATLASSERT(0);
						break;
					}
					fader.Fade(*atom.m_pColors,colors,n,m_FadeFactor);
					it = 0;
					fade = true;
				}
				else{
					// retore default color
					GetEdit().SetTextColor(m_FontColor);
					fade = false;
				}
				break;
			}
			case ATOM_ALT:
			{
				colors.RemoveAll();
				if(atom.m_bSet){
					if(!atom.m_pColors){
						ATLASSERT(0);
						break;
					}
					NO5TL::CopySimpleArray(colors,*atom.m_pColors);
					it = 0;
					alt = true;
				}
				else{
					// retore default color
					GetEdit().SetTextColor(m_FontColor);
					alt = false;
				}
				break;
			}
			case ATOM_BOLD:
				GetEdit().SetBold(atom.m_bSet);
				break;
			case ATOM_ITALIC:
				GetEdit().SetItalic(atom.m_bSet);
				break;
			case ATOM_UNDER:
				GetEdit().SetUnderline(atom.m_bSet);
				break;
			case ATOM_TATTOO:
				// fall
			case ATOM_FONT:
				if(!atom.m_bSet){
					// restore default stream font
					GetEdit().SetTextFontName(m_FontFace);
					GetEdit().SetTextHeight(m_FontSize);
				}
				else{
					if(!atom.m_pFont){
						ATLASSERT(0);
						break;
					}
					if(!atom.m_pFont->HasInfo()){
						if(atom.m_pFont->HasFace()){
							GetEdit().SetTextFontName(atom.m_pFont->m_face);
						}
						if(atom.m_pFont->HasSize()){
							int size = max(6,atom.m_pFont->m_size);
							size = min(size,24);
							GetEdit().SetTextHeight(size);
						}
					}
				}
				break;

			case ATOM_COLOR:
				if(!(fade || alt)){
					GetEdit().SetTextColor(atom.m_color);
				}
				break;
			case ATOM_BKCOLOR:
				GetEdit().SetTextBkColor(atom.m_color);
				break;
			case ATOM_CHARSET:
				GetEdit().SetCharset(atom.m_charset);
				break;

			case ATOM_SMILEY:
				{
					ISmileyMap *psm = g_app.GetSmileyMap();
					CString path(g_app.GetSmileysFolder());
					CString file;
					
					if(!atom.m_pText){
						ATLASSERT(0);
						break;
					}
					if(psm->FindSmiley(*atom.m_pText,file)){
						BOOL res;
						
						file.Replace(".gif",".bmp");
						path += file;
						res = GetEdit().PasteBitmapFromFile(path,LR_SHARED|LR_LOADTRANSPARENT);
						if(!res){
							GetEdit().AppendText(*atom.m_pText);
						}
					}
					else{
						GetEdit().AppendText(*atom.m_pText);
					}
				}
				break;
			
			case ATOM_LINK:
				GetEdit().SetLink(atom.m_bSet);
				if(atom.m_bSet && atom.m_pText)
					GetEdit().AppendText(*atom.m_pText);
				break;

			default:
				break;
		} // end of switch
	} // end of loop for

	//GetEdit().SendMessage(WM_SETREDRAW,(WPARAM)TRUE);
	//GetEdit().InvalidateRect(NULL);
}

/******** CHtmlEditStream ***************/

CString CHtmlEditStream::GetFontTag(LPCTSTR name,int size)
{
	CString res;

	if(size < 0)
		size = 1;

	if(name && size > 0)
		res.Format("<font style=\"font-size: %dpt;font-family: '%s'\">",
			size,name);
	else if(name)
		res.Format("<font style=\"font-family: '%s'\">",name);
	else if(size > 0)
		res.Format("<font style=\"font-size: %dpt\">",size);
	else
		res = "</font>";
		return res;
}

CString CHtmlEditStream::GetImgTag(LPCTSTR file)
{
	CString s;

	s.Format("<img src=\"%s\">",file);
	return s;
}

CString CHtmlEditStream::GetLinkTag(BOOL bOn,LPCTSTR url,LPCTSTR text,LPCTSTR tip)
{
	CString s;

	if(bOn){
		LPCTSTR _tip = tip ? tip : url;
		LPCTSTR _text = text ? text : url;
		// the "href" property is modified when we call IHTMLElement::GetAttribute
		// for example, if it doesnt contain "http://"
		// the A tag requires an href or a name property
		s.Format("<a href=\"%s\" name=\"%s\" title=\"%s\" id=\"idlink\">%s",url,url,_tip,_text);
	}
	else
		s = "</a>";
	return s;
}
	

// we well handle alt/fade here
void CHtmlEditStream::SendToDest()
{
	CString res;

	GetHtml(res);
	if(!res.IsEmpty()){
		HRESULT hr = GetEdit().AppendHtmlText(res);
		ATLASSERT(SUCCEEDED(hr));
	}
}

void CHtmlEditStream::GetHtml(CString &res)
{
	const int count = m_atoms.GetSize();
	register int i;
	ColorArray colors;
	int it = 0;
	CTextAtom atom;
	TCHAR ch[10] = {0};
	// estes valores nao tem a ver com this->m_fade e this->m_alt
	// os quais sao usados em AddAtom
	bool fade = false,alt = false;
	CString tmp;
	bool debug = false;
	
	for(i=0;i<count;i++){
		atom = m_atoms[i];

		switch(atom.GetType()){
			case ATOM_TEXT:
			{
				if(!atom.m_pText){
					ATLASSERT(0);
					break;
				}

				if(! ( fade || alt )){
					tmp.Empty();
					CString &s = *(atom.m_pText);
					if(utf_isValidString(s,s.GetLength())){
						LPWSTR pw = NO5TL::UTF8toUNICODE(s);
						if(pw){
							tmp += MakeUnicodeHtmlString(pw);
							delete []pw;
						}
						else{
							tmp += s;
						}
					}
					else
						tmp += (s);
					tmp.Replace("<","&lt;");
					tmp.Replace(">","&gt;");
					res += tmp;
				}
				else if(fade || alt){
					const int len = atom.m_pText->GetLength();
					for(int j=0;j<len;j++){
						res += GetColorTag(colors[it++]);
						if(it == colors.GetSize())
							it = 0;
						if(atom.m_pText->GetAt(j) == '<')
							lstrcpy(ch,"&lt;");
						else if(atom.m_pText->GetAt(j) == '>')
							lstrcpy(ch,"&gt;");
						else
							wsprintf(ch,"%c",atom.m_pText->GetAt(j));
						res += ch;
					}
				}
				break;
			}
			case ATOM_CHAR:
				if(atom.m_ch == '<')
					lstrcpy(ch,"&lt;");
				else if(atom.m_ch == '>')
					lstrcpy(ch,"&gt;");
				else
					wsprintf(ch,"%c",atom.m_ch);

				if(fade || alt){
					res += GetColorTag(colors[it++]);
					if(it == colors.GetSize())
						it = 0;
				}
				res += ch;
				break;
			case ATOM_BREAK:
				res += "<br>";
				break;
			
			case ATOM_FADE:
			{
				CColorFader<CYahooColor> fader;

				colors.RemoveAll();
				if(atom.m_bSet){
					int n = GetFadeLen(i);

					if(!atom.m_pColors){
						ATLASSERT(0);
						break;
					}
					fader.Fade(*atom.m_pColors,colors,n,m_FadeFactor);
					it = 0;
					fade = true;
				}
				else{
					// retore default color
					res += GetColorTag(m_FontColor);
					fade = false;
				}
				break;
			}
			case ATOM_ALT:
			{
				colors.RemoveAll();
				if(atom.m_bSet){
					if(!atom.m_pColors){
						ATLASSERT(0);
						break;
					}
					NO5TL::CopySimpleArray(colors,*atom.m_pColors);
					it = 0;
					alt = true;
				}
				else{
					// retore default color
					res += GetColorTag(m_FontColor);
					alt = false;
				}
				break;
			}
			case ATOM_BOLD:
				res += GetBoldTag(atom.m_bSet);
				break;
			case ATOM_ITALIC:
				res += GetItalicTag(atom.m_bSet);
				break;
			case ATOM_UNDER:
				res += GetUnderlineTag(atom.m_bSet);
				break;
			case ATOM_TATTOO:
				// fall
			case ATOM_FONT:
				if(!atom.m_bSet){
					// restore default stream font
					res += GetFontTag(m_FontFace,m_FontSize);
				}
				else{
					int size = 0;

					if(!atom.m_pFont){
						ATLASSERT(0);
						break;
					}
					if(!atom.m_pFont->HasInfo()){
						if(atom.m_pFont->HasSize())
							size = min(24,max(6,atom.m_pFont->m_size));
						res += GetFontTag(atom.m_pFont->m_face,
							size);
					}
				}
				break;

			case ATOM_COLOR:
				if(! ( fade || alt )){
					res += GetColorTag(atom.m_color);
				}
				break;
			
			case ATOM_SMILEY:
				{
					ISmileyMap *psm = g_app.GetSmileyMap();
					CString path(g_app.GetSmileysFolder());
					CString file;
					
					if(!atom.m_pText){
						ATLASSERT(0);
						break;
					}
					if(psm->FindSmiley(*atom.m_pText,file)){
						path += file;
						res += GetImgTag(path);
					}
					else{
						res += *atom.m_pText;
					}
				}
				break;
			case ATOM_LINK:
				{
					if(atom.m_bSet && atom.m_pText)
						res += GetLinkTag(TRUE,*atom.m_pText,NULL,NULL);
					else if(atom.m_bSet)
						res += GetLinkTag(TRUE,"","",""); // not used so far
					else
						res += GetLinkTag(FALSE);
				}
				break;
			case ATOM_HTML:
				{
					if(atom.m_pText)
						res += *atom.m_pText;
				}
				break;

			default:
				break;
		} // end of switch
	} // end of loop for
}

/******** CEditCtlStream ********/
void CEditCtlStream::SendToDest(void)
{
	ATLASSERT(m_edit);
	m_edit.AppendText(GetPlainText2());
}

void CEditCtlStream::ScrollText(void)
{
	SCROLLINFO si = { sizeof(SCROLLINFO) };
	int nPos=0;
			
	si.fMask = SIF_RANGE|SIF_PAGE;
	::GetScrollInfo(m_edit,SB_VERT,&si);
	si.fMask = SIF_POS;
	si.nPos = si.nMax;
	::SetScrollInfo(m_edit,SB_VERT,&si,TRUE);
	if(si.nPage){
		nPos = si.nMax - si.nPage;
		m_edit.SendMessage(WM_VSCROLL,MAKEWPARAM(SB_THUMBTRACK,nPos),(LPARAM)\
			NULL);
	}
}

CString MakeUnicodeHtmlString(LPCWSTR p)
{
	int len = wcslen(p);
	int i;
	char buf[10] = {0};
	CString res;

	for(i=0;i<len;i++){
		ZeroMemory(buf,sizeof(buf)/sizeof(buf[0]));
		if(p[i] > (unsigned short)127){
			wsprintf(buf,"&#x%x;",(int)p[i]);
			res += buf;
		}
		else{
			wsprintf(buf,"%c",(char)p[i]);
			res += buf;
		}
	}
	return res;
}
