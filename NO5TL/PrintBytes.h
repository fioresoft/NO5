// PrintBytes.h: interface for the CPrintBytes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRINTBYTES_H__32675025_9CEE_11D7_A17A_A1D058BCE831__INCLUDED_)
#define AFX_PRINTBYTES_H__32675025_9CEE_11D7_A17A_A1D058BCE831__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>

namespace NO5TL
{

inline std::ostream & PrintBytes(unsigned char *p,unsigned long count,
	std::ostream &out = std::cout,unsigned long linelen = 12,
	unsigned long sep = 4)
{
	unsigned long lines;
	unsigned long i,j,k,tmp;

	lines = count / linelen;
	if(lines == 0)
		lines = 1;
	out << std::hex << std::uppercase;
	for(i=0,k=0;i<lines && k < count;i++){
		tmp = k;
		for(j=0;j<linelen && k < count;j++,k++){
			out.width(2);
			out.fill('0');
			out << int(p[k]) << " ";
			if(((j + 1) % sep) == 0)
				out << "| ";
		}
		for(j=0;j<linelen && tmp < count;j++,tmp++){
			if(isprint(int(p[tmp])))
				out << char(p[tmp]);
			else
				out << '.';
		}

		// tava dando problema no edit control :(
		// out << std::endl;
		out << "\r\n";
	}
	return out;
}
}

#endif // !defined(AFX_PRINTBYTES_H__32675025_9CEE_11D7_A17A_A1D058BCE831__INCLUDED_)
