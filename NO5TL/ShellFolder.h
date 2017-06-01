// ShellFolder.h: interface for the CShellFolder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHELLFOLDER_H__97FBFAD7_1A66_11D9_A17B_F29CDA5EC14E__INCLUDED_)
#define AFX_SHELLFOLDER_H__97FBFAD7_1A66_11D9_A17B_F29CDA5EC14E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace NO5TL
{

class CPidl
{
	LPITEMIDLIST m_pidl;
	bool m_cantfree;	// used internally to catch misuses of the class
public:
	CPidl(LPITEMIDLIST pidl = NULL):m_pidl(pidl)
	{
		m_cantfree = false;
	}
	~CPidl()
	{
		Free();
	}
	void Free(void)
	{
		ATLASSERT(!m_cantfree);
		if(NULL != m_pidl){
			::CoTaskMemFree(m_pidl);
			m_pidl = NULL;
		}
	}
	operator LPITEMIDLIST ( void )
	{
		return m_pidl;
	}
	operator LPCITEMIDLIST ( void ) const
	{
		return m_pidl;
	}

	LPITEMIDLIST * operator & (void)
	{
		ATLASSERT(IsNull());
		return &m_pidl;
	}
	// compare pointers
	BOOL operator == (LPCITEMIDLIST pidl) const
	{
		return m_pidl == pidl ? TRUE : FALSE;
	}
	BOOL operator != (LPCITEMIDLIST pidl) const
	{
		return m_pidl != pidl ? TRUE : FALSE;
	}
	LPITEMIDLIST Attach(LPITEMIDLIST pidl)
	{
		// we assume the pointer we receive can be freed
		LPITEMIDLIST pidlOld = m_pidl;
		m_pidl = pidl;
		m_cantfree = false;
		return pidlOld;
	}
	LPITEMIDLIST Detach(void)
	{
		LPITEMIDLIST pidl = m_pidl;
		m_pidl = NULL;
		m_cantfree = false;
		return pidl;
	}
	USHORT HeadSize(LPCITEMIDLIST pidl = NULL) const
	{
		USHORT cb = 0;
		LPCITEMIDLIST p = pidl ? pidl : m_pidl;

		if(p){
			cb = *((USHORT *)p);
		}
		return cb;
	}
	// doesnt copy - doesnt return NULL but End() at the end
	LPITEMIDLIST Next(LPCITEMIDLIST pidl = NULL) const
	{
		LPBYTE p = (LPBYTE)(pidl ? pidl : m_pidl);

		if(p){
			p += HeadSize((LPCITEMIDLIST)p);
		}
		return (LPITEMIDLIST)p;
	}
	// doesnt copy, changes the value of the internal pointer
	// so it should not be freed
	CPidl & Advance(void)
	{
		if(m_pidl){
			Attach(Next());
			// we changed the pointer, cant free it anymore
			m_cantfree = true;
		}
		return *this;
	}

	BOOL IsNull(void) const
	{
		return NULL == m_pidl;
	}

	BOOL IsEmpty(void) const
	{
		BOOL res = TRUE;
		if(!IsNull())
			res = HeadSize() == 0 ? TRUE : FALSE;
		return res;
	}

	// returns a pointer to the terminating zero
	LPCITEMIDLIST End(void) const
	{
		LPBYTE p = (LPBYTE)m_pidl;
		if(p){
			p += TotalSize();
			ATLASSERT(0 == HeadSize((LPCITEMIDLIST)p));
		}
		return LPCITEMIDLIST(p);
	}

	USHORT TotalSize(void) const
	{
		USHORT sum = 0;
		CPidl pidl(m_pidl);

		while(!pidl.IsNull() && pidl.HeadSize()){
			sum += pidl.HeadSize();
			pidl.Attach(pidl.Next());
		}
		pidl.Detach();
		return sum;
	}

	ULONG Count(void) const
	{
		ULONG count = 0;
		CPidl pidl(m_pidl);

		while(!pidl.IsNull() && pidl.HeadSize()){
			count++;
			pidl.Advance();
		}
		pidl.Detach();
		return count;
	}

	// returns true if the list contains only one item
	// if it contains only the terminating zero, or if the list
	// is NULL, it returns FALSE
	BOOL IsSingle(void) const
	{
		BOOL res = FALSE;

		if(m_pidl && HeadSize()){
			CPidl pidl(Next());
			res = (pidl.HeadSize() == 0) ? TRUE : FALSE;
			pidl.Detach();
		}
		return res;
	}
	// this doesnt make a copy
	LPITEMIDLIST GetAt(int index) const
	{
		int i;
		CPidl nxt;

		nxt.Attach(m_pidl);
		i = 0;
		while(!nxt.IsNull() && nxt != End() && i < index){
			i++;
			nxt.Advance();
		}
		ATLASSERT(i == index);
		if(i >  index){
			nxt.Attach(NULL);
		}
		return nxt.Detach();
	}
	/******** copying methods *******************/

	// makes a new list composed by [start,end[
	LPITEMIDLIST CopyItems(int start, int end) const
	{
		LPCITEMIDLIST p,q;
		LPBYTE buf = NULL;

		ATLASSERT(start >= 0);
		ATLASSERT(end >= start);
		p = GetAt(start);
		q = GetAt(end);

		if((p && q) && (q - p > 0)){
			const int bufsize = (char *)q - (char *)p + sizeof(USHORT);
			buf = (LPBYTE)::CoTaskMemAlloc(bufsize);
			ATLASSERT(buf);
			if(buf){
				ZeroMemory(buf,bufsize);
				CopyMemory(buf,p,((char*)q - (char *)p));
			}
		}
		return (LPITEMIDLIST)buf;
	}
	LPITEMIDLIST CopyHead(void) const
	{
		LPITEMIDLIST p = NULL;

		if(!IsNull() && !IsEmpty())
			p = CopyItems(0,1);
	}
	LPITEMIDLIST CopyTail(void) const
	{
		ULONG count = Count();
		LPITEMIDLIST p = NULL;

		if(!IsNull() && !IsEmpty()){
			p = CopyItems(count - 1,count);
		}
		return p;
	}
	// gets a full copy
	LPITEMIDLIST CopyAll(void) const
	{
		return CopyItems(0,Count());
	}
	// returns a new, allocated pidl
	static LPITEMIDLIST Append(const CPidl &pidl1,const CPidl &pidl2)
	{
		LPBYTE buf = NULL;
		USHORT cb = 0;
		USHORT cb1 = pidl1.TotalSize();
		USHORT cb2 = pidl2.TotalSize();

		cb = cb1 + cb2;
		buf = (LPBYTE)::CoTaskMemAlloc(cb + sizeof(USHORT));
		ATLASSERT(buf);
		if(buf){
			ZeroMemory(buf,cb + sizeof(USHORT));
			CopyMemory(buf,(LPCITEMIDLIST)pidl1,cb1);
			CopyMemory(buf + cb1,(LPCITEMIDLIST)pidl2,cb2);
		}
		return (LPITEMIDLIST)buf;
	}

	CPidl & CopyFrom(LPITEMIDLIST pidl) 
	{
		CPidl tmp(pidl);

		Free();
		m_pidl = tmp.CopyAll();
		tmp.Detach();
		return *this;
	}

	// copies all but the last one. this will hopefully get the parent
	LPITEMIDLIST CopyParent(void) const
	{
		LPITEMIDLIST p = NULL;
		if(!IsNull()){
			const count = Count();

			if(count >= 2){
				// copies [0,count-1[
				p = CopyItems(0,count - 1);
			}
		}
		return p;
	}


	/***** other methods **********/

	HRESULT GetSpecialFolder(int nFolder,HWND hWnd = ::GetDesktopWindow())
	{
		HRESULT hr;

		ATLASSERT(IsNull());
		hr = ::SHGetSpecialFolderLocation(hWnd,nFolder,&m_pidl);
		ATLASSERT(SUCCEEDED(hr));
		return hr;
	}

#ifdef _WTL_USE_CSTRING

	BOOL GetPath(CString &path)
	{
		LPTSTR buf = path.GetBuffer(MAX_PATH * sizeof(TCHAR));
		BOOL res = ::SHGetPathFromIDList(m_pidl,buf);
		path.ReleaseBuffer();
		return res;
	}
#endif

	HIMAGELIST GetIconIndex(int &nIcon,int flags = SHGFI_SMALLICON)
	{
		SHFILEINFO sfi = {0};

		HIMAGELIST hImgLst = (HIMAGELIST)::SHGetFileInfo(\
			(LPCTSTR)m_pidl,
			0,
			&sfi,
			sizeof sfi,
			flags | SHGFI_PIDL |SHGFI_SYSICONINDEX );

		ATLASSERT(hImgLst);
		if(hImgLst){
			nIcon = sfi.iIcon;
		}
		return hImgLst;
	}
};

class CShellFolder
{
public:
	CComPtr<IShellFolder> m_folder;
public:
	CShellFolder(IShellFolder *pFolder = NULL):m_folder(pFolder)
	{
		if(!m_folder){
			HRESULT hr = ::SHGetDesktopFolder(&m_folder);
			ATLASSERT(SUCCEEDED(hr));
		}
	}

	HRESULT GetDesktop(void)
	{
		m_folder.Release();
		return ::SHGetDesktopFolder(&m_folder);
	}

	HRESULT Bind(LPCITEMIDLIST pidl)
	{
		IShellFolder *pFolder = NULL;
		HRESULT hr = m_folder->BindToObject(pidl,NULL,
			IID_IShellFolder,(LPVOID *)&pFolder);
		if(SUCCEEDED(hr)){
			m_folder.Release();
			// the ref count was already incremented
			m_folder.Attach(pFolder);
		}
		return hr;
	}

	HRESULT GetAttributesOf(LPITEMIDLIST pidl,ULONG &inout) const
	{
		CPidl tmp(pidl);
		HRESULT hr = E_INVALIDARG;

		// the docs say it should be a list with a single item...
		ATLASSERT(tmp.IsSingle());
		if(tmp.IsSingle()){
			hr = m_folder->GetAttributesOf(1,(LPCITEMIDLIST *)&pidl,&inout);
			ATLASSERT(SUCCEEDED(hr));
		}
		tmp.Detach();
		return hr;
	}
	BOOL Item_IsFolder(LPITEMIDLIST pidl) const
	{
		BOOL res = FALSE;
		ULONG mask = SFGAO_FOLDER;
		HRESULT hr = GetAttributesOf(pidl,mask);
		ATLASSERT(SUCCEEDED(hr));
		if(SUCCEEDED(hr))
			res = (mask & SFGAO_FOLDER) ? TRUE : FALSE;
		return res;
	}
	BOOL Item_IsFileSys(LPITEMIDLIST pidl) const
	{
		BOOL res = FALSE;
		ULONG mask = SFGAO_FILESYSTEM;
		HRESULT hr = GetAttributesOf(pidl,mask);
		ATLASSERT(SUCCEEDED(hr));
		if(SUCCEEDED(hr))
			res = (mask & SFGAO_FILESYSTEM ) ? TRUE : FALSE;
		return res;
	}
	BOOL Item_HasSubFolders(LPITEMIDLIST pidl) const
	{
		BOOL res = FALSE;
		ULONG mask = SFGAO_HASSUBFOLDER ;
		HRESULT hr = GetAttributesOf(pidl,mask);
		ATLASSERT(SUCCEEDED(hr));
		if(SUCCEEDED(hr))
			res = (mask & SFGAO_HASSUBFOLDER) ? TRUE : FALSE;
		return res;
	}
	int CompareIDs(LPCITEMIDLIST pidl1,LPCITEMIDLIST pidl2,
		LPARAM lParam = 0)
	{
		HRESULT hr = m_folder->CompareIDs(lParam,pidl1,pidl2);
		ATLASSERT(SUCCEEDED(hr));
		if(FAILED(hr))
			throw hr;
		return (int)(short)HRESULT_CODE(hr);
	}
	HRESULT GetDisplayNameOf(LPCITEMIDLIST pidl,CString &name,
		DWORD flags = SHGDN_INFOLDER | SHGDN_INCLUDE_NONFILESYS)
	{
		STRRET ret = {0};
		HRESULT hr = m_folder->GetDisplayNameOf(pidl,flags,&ret);
		ATLASSERT(SUCCEEDED(hr));
		if(SUCCEEDED(hr)){
			switch(ret.uType){
				case STRRET_WSTR:
					name = ret.pOleStr;
					::CoTaskMemFree(ret.pOleStr);
					break;
				case STRRET_OFFSET:
					name = (TCHAR *)((LPBYTE)pidl + ret.uOffset);
					break;
				case STRRET_CSTR:
					name = ret.cStr;
					break;
			}
		}
		return hr;
	}
};

} // NO5TL


#endif // !defined(AFX_SHELLFOLDER_H__97FBFAD7_1A66_11D9_A17B_F29CDA5EC14E__INCLUDED_)
