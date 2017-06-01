// Color.h: interface for the CColor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLOR_H__BFC6FE21_9028_11D7_A17A_A28FC2B1B532__INCLUDED_)
#define AFX_COLOR_H__BFC6FE21_9028_11D7_A17A_A28FC2B1B532__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace NO5TL
{
	class CColor
	{
		union 
		{
			struct
			{
				BYTE m_red;
				BYTE m_green;
				BYTE m_blue;
			};
			COLORREF m_cr;
			BYTE m_a[3];
		};
	public:
		CColor(COLORREF cr = 0x000000)
		{
			m_cr = cr;
		}
		CColor(BYTE red,BYTE green,BYTE blue)
		{
			Set(red,green,blue);
		}
		CColor(BYTE a[])
		{
			Set(a);
		}
		CColor(const CColor &color)
		{
			m_cr = color.m_cr;
		}
		//virtual ~CColor(){}

		// accessors - modifiers
		void Set(COLORREF cr)
		{
			m_cr = cr;
		}
		void Set(BYTE red,BYTE green,BYTE blue)
		{
			m_cr = 0;
			m_red = red;
			m_green = green;
			m_blue = blue;
		}
		void Set(BYTE a[])
		{
			int i;
			
			m_cr = 0;
			for(i=0;i<3;++i){
				m_a[i] = a[i];
			}
		}
		COLORREF Get(void) const 
		{
			return m_cr;
		}
		void Red(BYTE red){
			m_red = red;
		}
		void Green(BYTE green){
			m_green = green;
		}
		void Blue(BYTE blue){
			m_blue = blue;
		}
		BYTE Red(void) const {
			return m_red;
		}
		BYTE Green(void) const {
			return m_green;
		}
		BYTE Blue(void) const {
			return m_blue;
		}
		// operators
		CColor& operator=(const CColor &color){
			if(this != &color){
				Set(color.m_cr);
			}
			return *this;
		}
		CColor& operator=(COLORREF cr){
			Set(cr);
			return *this;
		}
		BYTE& operator[](int iIndex)
		{
			ATLASSERT(iIndex >= 0 && iIndex < 3);
			return m_a[iIndex];
		}
		BYTE operator[](int iIndex) const
		{
			ATLASSERT(iIndex >= 0 && iIndex < 3);
			return m_a[iIndex];
		}
		operator COLORREF() const {
			return m_cr;
		}
		CColor  operator+(const CColor &color) const 
		{
			unsigned int a[3],b[3];
			int i;

			for(i=0;i<3;++i){
				a[i] = m_a[i];
				b[i] = color.m_a[i];
			}
			for(i=0;i<3;++i){
				a[i] = min(a[i] + b[i],255);
			}
			return CColor((BYTE *)a);
		}
		CColor  operator-(const CColor &color) const 
		{
			int a[3],b[3];
			int i;

			for(i=0;i<3;++i){
				a[i] = m_a[i];
				b[i] = color.m_a[i];
			}
			for(i=0;i<3;++i){
				a[i] = max(a[i] - b[i],0);
			}
			return CColor((BYTE *)a);
		}
		CColor& operator+=(const CColor &color) 
		{
			unsigned int a[3],b[3];
			int i;

			for(i=0;i<3;++i){
				a[i] = m_a[i];
				b[i] = color.m_a[i];
			}
			for(i=0;i<3;++i){
				m_a[i] = min(a[i] + b[i],255);
			}
			return *this;
		}
		CColor& operator-=(const CColor &color) 
		{
			int a[3],b[3];
			int i;

			for(i=0;i<3;++i){
				a[i] = m_a[i];
				b[i] = color.m_a[i];
			}
			for(i=0;i<3;++i){
				m_a[i] = max(a[i] - b[i],0);
			}
			return *this;
		}
		bool operator==(const CColor &color) const 
		{
			int i;
			
			for(i=0;i<3;++i){
				if(m_a[i] != color.m_a[i])
					break;
			}
			return (i == 3);
		}
		bool operator!=(const CColor &color) const 
		{
			return (!(*this == color));
		}
		CColor& Rotate(BOOL bLeft = TRUE) 
		{
			CColor color = *this;

			if(bLeft)
				Set(color.Green(),color.Blue(),color.Red());
			else
				Set(color.Blue(),color.Red(),color.Green());
			
			return *this;
		}
		// call srand() before calling this
		CColor& Random(void){
			Set(rand()%256,rand()%256,rand()%256);
			return *this;
		}
	};

#ifndef NO5TL_NOBASICCOLORS
namespace Colors
{
	using NO5TL::CColor;						//	//ANSI	//

	const CColor BLACK(0,0,0);						// 30
	const CColor RED(0xff,0,0);						// 31
	const CColor GREEN(00,80,00);					// 32
	const CColor YELLOW(0xff,0xff,0);				// 33
	const CColor BLUE(0,0,0xff);					// 34
	const CColor FUCHSIA(0xff,0,0xff);	// magneta	// 35
	const CColor AQUA(0,0xff,0xff);		// cyan		// 36
	const CColor WHITE(0xff,0xff,0xff);				// 37
	const CColor LIME(0,0xff,0);
	const CColor SILVER(0xc0,0xc0,0xc0);
	const CColor GREY(0x80,0x80,0x80);	// gray
	const CColor MARRON(0x80,0,0);
	const CColor PURPLE(0x80,0,0x80);
	const CColor OLIVE(0x80,0x80,0);
	const CColor NAVY(0,0,0x80);
	const CColor TEAL(0,0x80,0x80);
	const CColor PINK(0xff,0xc0,0xcb);
	const CColor ORANGE(0xFF,0xA5,0x00);		// html orange
	
// apparently gray is with 'a' in american and with 'e' 
// in english, so ...
#ifndef GRAY
#define GRAY GREY
#endif

} // NO5TL::Colors
#endif

}

#endif // !defined(AFX_COLOR_H__BFC6FE21_9028_11D7_A17A_A28FC2B1B532__INCLUDED_)
