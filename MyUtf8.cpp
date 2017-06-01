// MyUtf8.cpp: implementation of the MyUtf8 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyUtf8.h"

/*
0000-0000-0000-007F	=> 0xxxxxxx
0000-0080-0000-07FF	=> 110xxxxx 10xxxxxx
0000-0800-0000-FFFF	=> 1110xxxx 10xxxxxx 10xxxxxx
0001-0000-0010-FFFF	=> 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/

int UTF8_GetBytes(unsigned short ch)
{
	int res = -1;

	if(ch < (unsigned short)0x0000007F)
		res = 1;
	else if(ch < (unsigned short)0x000007FF)
		res = 2;
	else if(ch < (unsigned short)0x0000FFFF)
		res = 3;
	else if(ch < (unsigned short)0x0010FFFF)
		res = 4;

	return res;
}

	

bool utf_isUnary(unsigned char ch)
{
	return ch <= (unsigned char)0x7f;
}

bool utf_isBinary(unsigned char ch)
{
	return ch <= (unsigned char)0xDF;
}
bool utf_isTrinary(unsigned char ch)
{
	return ch <= (unsigned char)0xEF;
}
bool utf_isQuaternary(unsigned char ch)
{
	return ch <= (unsigned char)0xF7;
}

bool utf_isTrailing(unsigned char ch)
{
	return ch <= (unsigned char)0xBF && ch > 0;
}

bool utf_isValidString(const char *p,int len)
{
	bool res = true;

	for(int i=0,count=1;i<len && count && res;i+=count){
		count = 0;

		unsigned  char ch = p[i];
		if(utf_isUnary(p[i])){
			count = 1;
			continue;
		}
		else if(utf_isBinary(p[i])){
			if(i + 1 > len ){
				res = false;
				continue;
			}
			if(utf_isTrailing(p[i+1])){
				count = 2;
				continue;
			}
			else{
				res = false;
				break;
			}
		}
		else if(utf_isTrinary(p[i])){
			if(i + 2 > len ){
				res = false;
				continue;
			}
			if(utf_isTrailing(p[i+1])){
				if(utf_isTrailing(p[i+2])){
					count = 3;
					continue;
				}
				else{
					res = false;
					break;
				}
			}
			else{
				res = false;
				break;
			}
		}
		else if(utf_isQuaternary(p[i])){
			if(i + 3 > len ){
				res = false;
				continue;
			}
			if(utf_isTrailing(p[i+1])){
				if(utf_isTrailing(p[i+2])){
					if(utf_isTrailing(p[i+3])){
						count = 4;
						continue;
					}
					else{
						res = false;
						break;
					}
				}
				else{
					res = false;
					break;
				}
			}
			else{
				res = false;
				break;
			}
		}
	}
	return res;
}
