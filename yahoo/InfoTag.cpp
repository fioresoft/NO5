// InfoTag.cpp: implementation of the CInfoTag class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <no5tl\mystring.h>
#include "InfoTag.h"
using namespace NO5TL;
using namespace NO5YAHOO;

// array will contain INF,KEY1:VALUE1,KEY2:VALUE2,etc

bool CInfoTag::Parse(CSimpleArray<CString> &props)
{
	const int count = props.GetSize();
	int i;
	CString item;
	int pos;
	CString key,val;
	bool res;

	for(i=0;i<count;i++){
		item = props[i];
		// pos == 0 would mean a value without key, we ignore it
		if( (pos = item.Find(':',0)) > 0){
			if(pos != item.GetLength() - 1){
				StringCopyN(key,item,0,pos);
				StringCopyN(val,item,pos + 1,-1);
				res = m_map.Add(key,val) ? true : false;
			}
			//else{
				// key without value, ignore it
			//}
		}
	}
	return res;
}

// return "<FONT INF KEY1:VAL1 KEY2:VAL2 ... >
CString CInfoTag::GetCode(void) const
{
	CString res;

	res = "<FONT ";
	res += GetCode2();
	res += '>';

	return res;
}

// return "INF KEY1:VAL1 KEY2:VAL2 ..."
CString CInfoTag::GetCode2(void) const
{
	CString key,val;
	const int size = m_map.GetSize();
	int i;
	CString res;

	if(!size)
		return res;

	res = "INF";

	for(i=0;i<size;i++){
		key = m_map.GetKeyAt(i);
		val = m_map.GetValueAt(i);

		res += ' ';
		res += key;
		res += ':';
		res += val;
	}

	return res;
}