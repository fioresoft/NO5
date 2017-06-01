// ColorFader.h: interface for the CColorFader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORFADER_H__01F55D61_40C6_11D9_A17B_834B2DB54E42__INCLUDED_)
#define AFX_COLORFADER_H__01F55D61_40C6_11D9_A17B_834B2DB54E42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __ATLBASE_H__
#error colorfader.h requires __ATLBASE_H__
#endif

#ifndef COLORFADER_H_
#define COLORFADER_H_

#include "no5tlbase.h"

namespace NO5TL
{
	template <typename ColorType>
	class CColorFader  
	{
		typedef CSimpleArray<ColorType> ColorArray;

		// factor in percent
		void SimpleFade(const ColorType &first,const ColorType &last,
			ColorArray &out,int n,int factor);
	public:
		// adds n colors to "out"
		void Fade(const ColorArray &in,ColorArray &out,int n,
			int factor);
		//void FadeAssymetric(const list<ColorType> &in,list<ColorType>\
		//	&out,int n,int factor,const vector<int> &parts);
	};

	template <typename ColorType>
	void CColorFader<ColorType>::SimpleFade(const ColorType &first,
		const ColorType &last,ColorArray &out,int n,int factor)
	{
		int r[3] = {0}; // razao da pa para red,green e blue
		ColorType temp;
		int i;
		int j;

		if(!n)
			return;
		else if(n == 1){
			out.Add((ColorType &)first);
			return;
		}
		// optimization for n == 2
		else if(n == 2){
			out.Add((ColorType &)first);
			out.Add((ColorType &)last);
			return;
		}
		for(i=0;i<3;i++){
			r[i] = (last[i] - first[i])/(n - 1);
			if((last[i] - first[i]) % (n - 1)){
				if(r[i] >= 0)
					r[i]++;
				else
					r[i]--;
			}
			r[i] = (factor/100) * r[i];
		}
		for(i=0;i<n;i++){
			for(j=0;j<3;j++){
				temp[j] = min(255,max(0,first[j] + i * r[j]));
			}
			out.Add(temp);
		}
	}

	template <typename ColorType>
	void CColorFader<ColorType>::Fade(const ColorArray &in,
		ColorArray &out,int n,int factor)
	{
		const int count = in.GetSize();
		const int segs = count - 1;		// numbers of segments
		int segLen;						// length of each segment
		int rest;						
		int it = 0; // = in.begin();
		ColorArray tmp;
		ColorType first,last;
		int i;
		int realLen;						// real length of a segment

		if(!n)
			return;

		if(count == 0)
			return;
		else if(count == 1 || n == 1){
			SimpleArrayAddElements(out,in[0],n);
			return;
		}
		else if(count == 2){
			first = in[it++];
			last = in[it];
			SimpleFade(first,last,out,n,factor);
			return;
		}
		else{
			segLen = n/segs;
			rest = n % segs;
			for(i=0;i<segs;i++){
				if(i == 0)
					first = in[it++]; // *it++;
				last = in[it++]; // *it++;
				realLen = segLen;
				// share the rest among the segments
				if(rest){
					realLen += 1;
					rest--;
				}
				if(i)
					realLen++;
				ATLASSERT(realLen);
				SimpleFade(first,last,tmp,realLen,factor);
				ATLASSERT(tmp.GetSize() == realLen);
				if(tmp.GetSize() == 0){
					int apague = 0;
				}
				first = tmp[tmp.GetSize() - 1];	// = tmp.back()
				if(i)
					tmp.RemoveAt(0);	// tmp.pop_front();
				//out.splice(out.end(),tmp);
				AppendSimpleArray(out,tmp);
				tmp.RemoveAll();
			}
		}
	}

	/*
	template <typename ColorType>
	void CColorFader<ColorType>::FadeAssymetric(const list<ColorType> &in,
		list<ColorType> &out,int n,int factor,const vector<int> &parts)

	{
		const int count = in.size();
		const int segs = count - 1;		// numbers of segments					
		list<ColorType>::const_iterator it = in.begin();
		list<ColorType> tmp;
		CColor first,last;
		int i;						

		if(count == 0)
			return;
		else if(count == 1)
			out = in;
		else if(count == 2){
			first = *it++;
			last = *it;
			SimpleFade(first,last,out,n,factor);
			return;
		}
		else{
			for(i=0;i<segs;i++){
				if(i == 0)
					first = *it++;
				last = *it++;
				SimpleFade(first,last,tmp,parts[i],factor);
				first = tmp.back();
				if(i)
					tmp.pop_front();
				out.splice(out.end(),tmp);
			}
		}
	}
	*/

}	// NO5TL

#endif // COLORFADER_H_

#endif // !defined(AFX_COLORFADER_H__01F55D61_40C6_11D9_A17B_834B2DB54E42__INCLUDED_)
