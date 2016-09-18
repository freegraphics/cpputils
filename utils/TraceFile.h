#pragma once

#include "utils.h"
#include "convert.h"


struct CTraceFile
{
	/**\brief constructor
	\param _szFileName -- file name
	*/
	CTraceFile(LPCTSTR _szFileName = NULL)
		:m_hFile(INVALID_HANDLE_VALUE)
	{
		if(NOT_NULL(_szFileName)) open(_szFileName);
	}

	/**\brief destructor
	*/
	~CTraceFile()
	{
		close();
	}

	bool is_valid() const 
	{
		return !(EQL(m_hFile,INVALID_HANDLE_VALUE) || IS_NULL(m_hFile));
	}

	operator bool () const 
	{
		return is_valid();
	}

	/**\brief function to open temp file.
	It open file name at current user temp path with specified name
	\param _szFileName -- file name to open
	*/
	void open(LPCTSTR _szFileName)
	{
		VERIFY_EXIT(NOT_NULL(_szFileName));
		VERIFY_EXIT(!is_valid());
		m_hFile = ::CreateFile(
			_szFileName
			,GENERIC_WRITE
			,FILE_SHARE_READ
			,NULL
			,OPEN_ALWAYS
			,FILE_ATTRIBUTE_NORMAL
			,NULL
			);
		if(EQL(INVALID_HANDLE_VALUE,m_hFile)) return;
		SetFilePointer(m_hFile,0,NULL,FILE_END);
	}

	/**\brief function to write string into the temp file
	\param _sText -- string to write 
	\param _bExpandNewLine -- flag to replace '\n' into '\r\n'
	*/
	void write(const CString_& _sText,bool _bExpandNewLine = true)
	{
		if(!is_valid()) return;
		CString sText = _sText;
		if(_bExpandNewLine)	replace(sText,_T("\n"),_T("\r\n"));
		string_converter<TCHAR,char> textA(sText,CP_ACP);
		DWORD dwWritten = 0;
		WriteFile(m_hFile,(LPVOID)(LPCSTR)textA,sText.GetLength(),&dwWritten,NULL);
	}

	void write(const std::vector<byte>& _buf,size_t _buf_sz = 0)
	{
		if(!is_valid()) return;
		DWORD dwWritten = 0;
		WriteFile(m_hFile
			,(LPVOID)&_buf[0]
			,(DWORD)(_buf_sz!=0?_buf_sz:_buf.size())
			,&dwWritten,NULL
			);
	}

	void write(const byte* _buf,size_t _buf_sz)
	{
		if(!is_valid()) return;
		DWORD dwWritten = 0;
		WriteFile(m_hFile,(LPVOID)_buf,_buf_sz,&dwWritten,NULL);
	}

	template<typename _Type>
		CTraceFile& operator << (const _Type& _dta)
	{
		CString_ str;
		common::convert(_dta,str);
		write(str);
		return *this;
	}

	void flush()
	{
		if(!is_valid()) return;
		FlushFileBuffers(m_hFile);
	}

	void close()
	{
		if(!is_valid()) return;
		::CloseHandle(m_hFile);
		m_hFile = NULL;
	}
protected:
	HANDLE m_hFile;			///< file handle
};//struct CTraceFile 
