#pragma once

#include "config.h"
#include "atlmfc.h"
#include "utils.h"

template<typename _ItemType>
struct BinFile
{
protected:
	HANDLE m_hFile;
	DWORD m_file_position;
	DWORD m_file_size;
	bool read_mode_flag;

public:
	BinFile()
		:m_hFile(NULL)
		,m_file_position(0)
		,m_file_size(0)
		,read_mode_flag(true)
	{
	}

	~BinFile()
	{
		close();
	}

	void reset()
	{
		m_file_position = 0;
		if(is_valid_file())
		{
			::SetFilePointer(m_hFile,m_file_position,NULL,FILE_BEGIN);
		}
	}

	void close()
	{
		if(!is_valid_file()) return;
		::CloseHandle(m_hFile);
		m_hFile = NULL;
		m_file_position = 0;
		m_file_size = 0;
	}

	void open_to_write(const CString& _file_name)
	{
		open(_file_name,false);
	}

	void open_to_read(const CString& _file_name)
	{
		open(_file_name,true);
	}

	void open(const CString& _file_name,bool _read_mode = true)
	{
		read_mode_flag = _read_mode;
		m_hFile = ::CreateFile(
			_file_name
			,(_read_mode?0:GENERIC_WRITE)|GENERIC_READ
			,FILE_SHARE_WRITE|FILE_SHARE_READ
			,NULL
			,_read_mode?OPEN_EXISTING:OPEN_ALWAYS
			,FILE_ATTRIBUTE_NORMAL
			,NULL
			);
		if(m_hFile==INVALID_HANDLE_VALUE) 
		{
			m_hFile = NULL;
		}
		else
		{
			m_file_size = ::GetFileSize(m_hFile,NULL);
		}
		reset();
	}

	size_t size()
	{
		if(!read_mode_flag)
		{
			if(is_valid_file())
				m_file_size = ::GetFileSize(m_hFile,NULL);
			else
				m_file_size = 0;
		}
		return m_file_size/sizeof(_ItemType);
	}

	bool eof()
	{
		if(!read_mode_flag)
		{
			if(is_valid_file())
			{
				m_file_size = ::GetFileSize(m_hFile,NULL);
				m_file_position = SetFilePointer(m_hFile,0,NULL,FILE_CURRENT);
				if(EQL(INVALID_SET_FILE_POINTER,m_file_position)) m_file_position = 0;
			}
			else
			{
				m_file_size = 0;
				m_file_position = 0;
			}
		}
		return m_file_position>=m_file_size;
	}

	void move_to_end()
	{
		if(!is_valid_file()) return;
		m_file_position = (m_file_size/sizeof(_ItemType))*sizeof(_ItemType);
		::SetFilePointer(m_hFile,m_file_position,NULL,FILE_BEGIN);
	}

	_ItemType get()
	{
		_ItemType item = _ItemType();
		get(item);
		return item;
	}

	void get(_ItemType& _item)
	{
		if(!is_valid_file() || eof()) 
		{
			_item = _ItemType();
			return;
		}
		DWORD dwReaded = 0;
		if(!::ReadFile(m_hFile,&_item,sizeof(_ItemType),&dwReaded,NULL)
			|| dwReaded!=sizeof(_ItemType)
			)
		{
			_item = _ItemType();
			return;
		}
		m_file_position += sizeof(_ItemType);
	}

	template<typename _Iterator>
	size_t get(_Iterator _it,_Iterator _ite)
	{
		if(!is_valid_file() || eof()) return 0;

		size_t cnt = std::distance(_it,_ite);
		std::vector<_ItemType> data;
		data.resize(cnt);
		DWORD dwReaded = 0;
		if(!::ReadFile(m_hFile,&(data[0]),data.size()*sizeof(_ItemType),&dwReaded,NULL)) 
			dwReaded = 0;
		size_t readed = dwReaded/sizeof(_ItemType);
		size_t i = 0;
		for(i=0;_it!=_ite && i<readed;++_it,i++)
		{
			*_it = data[i];
		}
		m_file_position += readed*sizeof(_ItemType);
		return readed;
	}

	template<typename _Iterator>
	size_t put(_Iterator _it,_Iterator _ite)
	{
		if(!is_valid_file()) return 0;

		size_t cnt = std::distance(_it,_ite);
		std::vector<_ItemType> data;
		data.resize(cnt);

		size_t i = 0;
		for(i=0;i<cnt && _it!=_ite;i++,++_it)
		{
			data[i] = *_it;
		}
		DWORD dwWritten = 0;
		if(!WriteFile(m_hFile,&(data[0]),data.size()*sizeof(_ItemType),&dwWritten,NULL))
			dwWritten = 0;
		size_t written = dwWritten/sizeof(_ItemType);
		return written;
	}

	size_t put(_ItemType _data)
	{
		if(!is_valid_file()) return 0;
		DWORD dwWritten = 0;
		if(!WriteFile(m_hFile,&_data,sizeof(_data),&dwWritten,NULL))
			dwWritten = 0;
		size_t written = dwWritten/sizeof(_ItemType);
		return written;
	}

	void next(size_t _cnt = 1)
	{
		m_file_position += sizeof(_ItemType)*_cnt;
		if(is_valid_file() && !eof())
		{
			::SetFilePointer(m_hFile,m_file_position,NULL,FILE_BEGIN);
		}
	}

	bool is_valid_file() const
	{
		return NOT_NULL(m_hFile) && NEQL(m_hFile,INVALID_HANDLE_VALUE);
	}
};//template<> struct BinFile
