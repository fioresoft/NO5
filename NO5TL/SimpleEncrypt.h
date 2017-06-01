// SimpleEncrypt.h: interface for the CSimpleEncrypt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLEENCRYPT_H__051AEB97_9A43_4A31_8CA2_2C4128E1F97F__INCLUDED_)
#define AFX_SIMPLEENCRYPT_H__051AEB97_9A43_4A31_8CA2_2C4128E1F97F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace NO5TL
{

struct SimpleEncryptedData
{
	char *m_buf;
	int m_len;
	SimpleEncryptedData()
	{
		m_buf = NULL;
		m_len = 0;
	}
	SimpleEncryptedData(char *buf,int len)
	{
		m_buf = buf;
		m_len = len;
	}
	~SimpleEncryptedData()
	{
		Reset();
	}
	void Reset(void)
	{
		if(m_buf){
			delete []m_buf;
			m_buf = NULL;
		}
		m_len = 0;
	}
};

inline void Encrypt(LPCSTR p,SimpleEncryptedData &data)
{
	int i,j;
	char key;
	
	ATLASSERT(data.m_buf == NULL);
	data.m_len = 2 * lstrlen(p);
	data.m_buf = new char[data.m_len];

	srand(GetTickCount());

	for(i=0,j=0;i<lstrlen(p);i++,j+=2){
		key = ((unsigned char)(rand() % 127) + ( unsigned char )(GetTickCount() % 127)) % 255;
		key = ( key == 0 ) ? 0xab : key;
		data.m_buf[j] = key;
		data.m_buf[j+1] = key ^ p[i];
	}
}

// p must have half of the size of data.len + 2
inline void Decrypt(const SimpleEncryptedData &data,char *p)
{
	int i,j;
	char key;
	char val;

	for(i=0,j=0;j<data.m_len;i++,j+=2){
		key = data.m_buf[j];
		val = data.m_buf[j+1];
		val = val ^ key;
		p[i] = val;
	}
	p[i] = '\0';
}

} // NO5TL


#endif // !defined(AFX_SIMPLEENCRYPT_H__051AEB97_9A43_4A31_8CA2_2C4128E1F97F__INCLUDED_)
