#pragma once

#ifndef __ATLBASE_H__
#error socket.h requires atlmisc.h 
#endif

#ifndef __ATLMISC_H__
#error socket.h requires atlmisc.h 
#endif

#ifndef GRADRECT_H_
#define GRADRECT_H_

#include "no5tlbase.h"
#include "color.h"


namespace NO5TL
{

class CColorVertex:public TRIVERTEX
{
public:
	static CColor ToColor(COLOR16 r,COLOR16 g,COLOR16 b)
	{
		CColor res(r >> 8, g >> 8, b >> 8);
		return res;
	}
	static COLOR16 GetRed(const CColor &color)
	{
		return COLOR16(color.Red()) << 8;
	}
	static COLOR16 GetGreen(const CColor &color)
	{
		return COLOR16(color.Green()) << 8;
	}
	static COLOR16 GetBlue(const CColor &color)
	{
		return COLOR16(color.Blue()) << 8;
	}
public:
	CColorVertex(void)
	{
		::ZeroMemory(this,sizeof(TRIVERTEX));
	}
	CColorVertex(LONG xx,LONG yy,COLORREF cr)
	{
		CColorVertex();
		Set(xx,yy,cr);

	}
	void Set(LONG xx,LONG yy,COLORREF cr)
	{
		CColor color(cr);
		x = xx;
		y = yy;
		Red = GetRed(color);
		Green = GetGreen(color);
		Blue = GetBlue(color);
	}
};

template <bool t_bHorz>
void GradRectAsymetric(HDC hdc,CRect &r,CSimpleValArray<LONG> &parts,
					   CSimpleArray<CColor> &colors)
{
	CDCHandle dc = hdc;
	const int count = colors.GetSize();
	const int nRects = (count - 1);		// number of rectangles
	int i,j;
	CSimpleArray<CColorVertex> vertices;
	CSimpleArray<GRADIENT_RECT> grs;
	LONG cxy;
	LONG cxySum = 0;
	CColorVertex cv;
	GRADIENT_RECT gr = {0};

	if(count == 0){
		return;
	}
	else if(count == 1){
		dc.FillSolidRect(&r,colors[0]);
		return;
	}
	ATLASSERT(parts.GetSize() == nRects);
	
		
	// fill each trivertex structure
	for(i=0,j=0; i < 2 * nRects; i += 2,j++){
		cxy = parts[j];
		// the last width can be a negative value meaning
		// anything that has left
		if(cxy < 0){
			ATLASSERT(j == nRects - 1);
			cxy = t_bHorz ? r.Width() : r.Height();
			cxy -= cxySum;
		}
		if(cxy == 0){
			continue;
		}

		if(t_bHorz){
			// upper left
			cv.Set(r.left + cxySum,r.top,colors[j]);
			vertices.Add(cv);
			// lower right
			cv.Set(r.left + cxySum + cxy,r.bottom,colors[j+1]);
			vertices.Add(cv);
		}
		else{
			// upper left
			cv.Set(r.left,r.top + cxySum,colors[j]);
			vertices.Add(cv);
			// lower right
			cv.Set(r.right,r.top + cxySum + cxy,colors[j+1]);
			vertices.Add(cv);
		}
		cxySum += cxy;
		// penultimo adicionado eh o vertices superior esquerdo
		gr.UpperLeft = vertices.GetSize() - 2;
		// o ultimo eh o vertice inferior direito
		gr.LowerRight = vertices.GetSize() - 1;
		grs.Add(gr);
	}

	dc.GradientFill((PTRIVERTEX)vertices.GetData(),nRects * 2,
		grs.GetData(),nRects,
		t_bHorz ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V);
}


template <bool t_bHorz>
void GradRect(HDC hdc,CRect &r,CSimpleArray<CColor> &colors)
{
	const int nRects = colors.GetSize() - 1;	// numb of rects
	// width of each rectangle
	LONG cxy = (t_bHorz ? r.Width() : r.Height()) / nRects;	
	CSimpleValArray<LONG> parts;

	SimpleArrayAddElements<LONG>((CSimpleArray<LONG> &)parts,cxy,
		nRects);
	GradRectAsymetric<t_bHorz>(hdc,r,parts,colors);
}

template <class T,bool t_bHorz>
class CGradientViewImpl
{
public:
	BEGIN_MSG_MAP(CGradientViewImpl)
		MESSAGE_HANDLER(WM_PAINT,OnPaint)
	END_MSG_MAP()

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam,
		BOOL& bHandled)
	{
		T *pT = static_cast<T*>(this);
		CPaintDC dc(pT->m_hWnd);
		CRect r;
		
		pT->GetClientRect(&r);
		if(m_parts.GetSize())
			GradRectAsymetric<t_bHorz>(dc,r,m_parts,m_colors);
		else
			GradRect<t_bHorz>(dc,r,m_colors);
		return 0;
	}
	//
	CSimpleArray<CColor> m_colors;
	CSimpleValArray<LONG> m_parts;
private:
};

template <class T,bool t_bHorz,class TBase = CWindow,class TWinTraits = CControlWinTraits>
class CGradientWindowImpl : \
	public CWindowImpl<T,TBase,TWinTraits>,
	public CGradientViewImpl<T,t_bHorz>
{
	typedef CGradientViewImpl<T,t_bHorz> _BaseClass;
public:
	BEGIN_MSG_MAP(CGradientWindowImpl)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()
};

template <bool t_bHorz>
class CGradientWindowT : public CGradientWindowImpl<CGradientWindowT<t_bHorz>,t_bHorz>
{
	typedef CGradientWindowImpl<CGradientWindowT<t_bHorz>,t_bHorz> _BaseClass;
public:
	DECLARE_WND_CLASS(_T("NO5_GradientClass"));
	BEGIN_MSG_MAP(CGradientWindowT)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()
};
typedef CGradientWindowT<true> CGradientWindow;

template <class T,bool t_bHorz>
class CFadeWndImplBase:public CGradientViewImpl<T,t_bHorz>
{
	typedef CGradientViewImpl<T,t_bHorz> _BaseClass;
public:
	BEGIN_MSG_MAP(CFadeWindowImplBase)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()
	//
	
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam,
		BOOL& bHandled)
	{
		T *pT = static_cast<T*>(this);
		CClientDC dc(pT->m_hWnd);
		CColor color( dc.GetPixel(LOWORD(lParam),HIWORD(lParam)) );
		pT->OnFadeWndClick(color);
		return 0;
	}
		

	// interface
	COLORREF SetColor(COLORREF color,bool update = true)
	{
		COLORREF res = COLORREF(m_color);
		T *pT = static_cast<T*>(this);

		m_color = color;
		if(update){
			UpdatePartsAndColors();
			pT->InvalidateRect(NULL);
		}
		return res;
	}
	void SetColorBar(int index)
	{
		m_bar = index;
	}
	// overideable
	void OnFadeWndClick(COLORREF color)
	{
		//
	}
private:
	CColor m_color;
	int m_bar;

	void UpdatePartsAndColors(void)
	{
		CRect r;
		LONG cx;
		CColor color;
		T *pT = static_cast<T*>(this);

		pT->GetClientRect(&r);

		m_parts.RemoveAll();
		m_colors.RemoveAll();

		// for example, being bar == 0 ( red bar )
		// first color == 0,g,b
		// color at a distance of cx == r,g,b
		// last color == 255,g,b
		cx = (m_color[m_bar] * r.Width()) / 255;
	

		// rect1 = [0,cx]  rect2 = [cx,r.width]
		m_parts.Add(cx);
		m_parts.Add(-1);
		
		color = m_color;
		color[m_bar] = 0;
		m_colors.Add(color);
		m_colors.Add(m_color);
		color[m_bar] = 255;
		m_colors.Add(color);
	}
};

template <class T,bool t_bHorz>
class CFadeWindowImpl : public CWindowImpl<T>,
	public CFadeWndImplBase<T,t_bHorz>
{
	typedef CFadeWndImplBase<T,t_bHorz> _FadeImpl;
public:
	BEGIN_MSG_MAP(CFadeWindowImpl)
		CHAIN_MSG_MAP(_FadeImpl)
	END_MSG_MAP()
};

template <bool t_bHorz>
class CFadeWindowT : \
	public CFadeWindowImpl<CFadeWindowT<t_bHorz>,t_bHorz>
{
	typedef CFadeWindowImpl<CFadeWindowT<t_bHorz>,t_bHorz> _BaseClass;
public:
	BEGIN_MSG_MAP(CFadeWindowT)
		CHAIN_MSG_MAP(_BaseClass)
	END_MSG_MAP()
};

typedef CFadeWindowT<true> CFadeWindow;

}	// NO5TL

#endif	// GRADRECT_H_