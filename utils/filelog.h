#if !defined(__FILELOG_H__170E7176_FA12_4299_87AE_AE078548C88D_INCLUDE)
#define __FILELOG_H__170E7176_FA12_4299_87AE_AE078548C88D_INCLUDE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"

#include "serialize.h"
#include "memserializer.h"
#include "errors.h"
#include "fileutils.h"


template<class _Type>
struct CLogFile
{
protected:
	HANDLE m_hfile;
	HANDLE m_hidxfile;
	std::vector<size_t> m_indexes;
public:
	CLogFile()
		:m_hfile(NULL)
		,m_hidxfile(NULL)
	{
	}

	~CLogFile()
	{
		flush();
		close();
	}

	void open(const CString& _file_name,bool _append_mode=false)
	{
		m_hfile = CreateFile(
			_file_name
			,_append_mode?GENERIC_WRITE|GENERIC_READ:GENERIC_READ
			,FILE_SHARE_READ
			,NULL
			,_append_mode?OPEN_ALWAYS:OPEN_EXISTING
			,FILE_ATTRIBUTE_NORMAL
			,NULL
			);
		if(m_hfile==INVALID_HANDLE_VALUE) 
			throw_lastWin32Error();
		CString path,fname,ext;
		FileUtils::SplitePath(_file_name,path,fname,ext);
		ext.MakeLower();
		const LPCTSTR szIdxExt = _T(".idx");
		if(ext==szIdxExt)
		{
			close();
			::SetLastError(ERROR_BAD_ARGUMENTS);
			throw_lastWin32Error();
		}
		CString idx_file_name = path + fname + szIdxExt;
		m_hidxfile = ::CreateFile(
			idx_file_name
			,_append_mode?GENERIC_WRITE|GENERIC_READ:GENERIC_READ
			,FILE_SHARE_READ
			,NULL
			,_append_mode?OPEN_ALWAYS:OPEN_EXISTING
			,FILE_ATTRIBUTE_NORMAL
			,NULL
			);
		if(m_hidxfile==INVALID_HANDLE_VALUE)
		{
			DWORD err = ::GetLastError();
			close();
			throw_win32Error(err);
		}
		DWORD sz = GetFileSize(m_hidxfile,NULL);
		if(sz%sizeof(size_t)!=0)
		{
			close();
			throw_win32Error(ERROR_BAD_FILE_TYPE);
		}
		size_t items = sz/sizeof(size_t);
		m_indexes.resize(items);
		if(items!=0)
		{
			DWORD dwReaded = 0;
			if(!ReadFile(m_hidxfile,&m_indexes[0],sz,&dwReaded,NULL) || dwReaded!=sz)
			{
				DWORD err = ::GetLastError();
				close();
				throw_win32Error(err);
			}
		}
	}

	void close()
	{
		if(valid(m_hfile))
			::CloseHandle(m_hfile);
		m_hfile = NULL;
		if(valid(m_hidxfile))
			::CloseHandle(m_hidxfile);
		m_hidxfile = NULL;
	}

	void flush()
	{
		if(valid(m_hfile))
			::FlushFileBuffers(m_hfile);
		if(valid(m_hidxfile))
			::FlushFileBuffers(m_hidxfile);
	}

	size_t size() const 
	{
		return m_indexes.size();
	}

	void get(size_t _idx,_Type& _data,const IMetaClass<_Type>& _meta_class = get_structlayout<_Type>())
	{
		std::vector<byte> buf;

		get_data(_idx,buf);

		Memory::CLoader loader(buf);
		CMemoryArchive arch;
		loader.load(arch);
		::load(arch,get_structname<_Type>(),_data,_meta_class);
	}

	template<typename _Cont>
	void get(size_t _idb,size_t _ide,_Cont& _cont,const IMetaClass<_Type>& _meta_class = get_structlayout<_Type>())
	{
		size_t i=0;
		for(i=_idb;i<_ide;i++)
		{
			_Type dta;
			get(i,dta,_meta_class);
			_cont.push_back(dta);
		}
	}

	void append(const _Type& _data,const IMetaClass<_Type>& _meta_class = get_structlayout<_Type>())
	{
		Memory::CSaver saver;
		CMemoryArchive arch;
		::save(arch,get_structname<_Type>(),_data,_meta_class);
		saver.save(arch);

		append_data(saver.get_data());
	}

	template<typename _Iterator>
	void append(_Iterator _it,_Iterator _ite,const IMetaClass<_Type>& _meta_class = get_structlayout<_Type>())
	{
		for(;_it!=_ite;++_it)
		{
			append(*_it,_meta_class);
		}
	}

protected:
	static bool valid(HANDLE _hfile)
	{
		return _hfile!=NULL && _hfile!=INVALID_HANDLE_VALUE;
	}

	void get_data(size_t _idx,std::vector<byte>& _buf)
	{
		VERIFY_EXIT(_idx<m_indexes.size()); 
		size_t begin = m_indexes[_idx];
		size_t end = m_indexes.size()-1==_idx?GetFileSize(m_hfile,NULL):m_indexes[_idx+1];
		VERIFY_EXIT(end>begin);
		_buf.resize(end-begin);
		if(::SetFilePointer(m_hfile,begin,NULL,FILE_BEGIN)==INVALID_SET_FILE_POINTER)
			throw_lastWin32Error();
		DWORD dwReaded = 0;
		if(!ReadFile(m_hfile,&_buf[0],end-begin,&dwReaded,NULL)
			|| dwReaded!=end-begin
			)
			throw_lastWin32Error();
	}

	void append_data(const std::vector<byte>& _buf)
	{
		size_t pos = GetFileSize(m_hfile,NULL);
		DWORD idxpos = ::SetFilePointer(m_hidxfile,0,NULL,FILE_END);
		if(idxpos==INVALID_SET_FILE_POINTER)
			throw_lastWin32Error();
		if(::SetFilePointer(m_hfile,0,NULL,FILE_END)==INVALID_SET_FILE_POINTER)
			throw_lastWin32Error();
		DWORD dwWritten = 0;
		if(!::WriteFile(m_hidxfile,&pos,sizeof(pos),&dwWritten,NULL) || dwWritten!=sizeof(pos))
			throw_lastWin32Error();
		if(!::WriteFile(m_hfile,&_buf[0],(DWORD)_buf.size(),&dwWritten,NULL)  || dwWritten!=_buf.size())
		{
			DWORD err = ::GetLastError();
			::SetFilePointer(m_hidxfile,idxpos,NULL,FILE_BEGIN);
			::SetEndOfFile(m_hidxfile);
			throw_win32Error(err);
		}
		m_indexes.push_back(pos);
	}
};//template<> struct LogFile

template<class _ObjectType> 
struct LogObject;

namespace Private
{
	template<class _ObjectType> 
	struct LogObjectData;

	interface ILogObject
	{
		virtual void append() = 0;
	};
};//namespace Private

struct Logs
{
protected:
	typedef std::vector<Private::ILogObject*> LogsVec;
	LogsVec m_logs;
	CString m_log_path;
public:
	Logs()
	{
	}

	~Logs()
	{
		VERIFY_EXIT(m_logs.empty());
	}

	void set_path(const CString& _log_path)
	{
		m_log_path = _log_path;
	}

	template<typename _Type>
	LogObject<_Type> create_object(const CString& _name);

	template<typename _Type>
	void delete_log_object(Private::LogObjectData<_Type>* _objbase);

	void append()
	{
		LogsVec::iterator
			it = m_logs.begin()
			,ite = m_logs.end()
			;
		for(;it!=ite;++it)
		{
			(*it)->append();
		}
	}
};

namespace Private
{
	template<class _ObjectType> 
	struct LogObjectData : public ILogObject
	{
	protected:
		Logs& m_logs;
		_ObjectType m_data;
		CLogFile<_ObjectType> m_file_log; 
		long m_counter;

	protected:
		LogObjectData(Logs& _logs,const CString& _log_file_name)
			:m_logs(_logs)
			,m_counter(0)
		{
			m_file_log.open(_log_file_name,true);
		}

		~LogObjectData()
		{
			m_logs.delete_log_object(this);
		}
	public:
		_ObjectType& get_ref() {return m_data;}
		const _ObjectType& get_ref() const {return m_data;}

		void add_ref()
		{
			m_counter++;
		}

		void release_ref()
		{
			if(!--m_counter)
				delete trace_free(this);
		}

		//interface ILogObject
		//{
		virtual void append()
		{
			m_file_log.append(m_data);
		}
		//};
		friend struct Logs;
	};
};//namespace Private

template<class _ObjectType> 
struct LogObject
{
protected:
	Private::LogObjectData<_ObjectType>* m_log_data;

protected:
	LogObject(Private::LogObjectData<_ObjectType>* _log_data)
		:m_log_data(_log_data)
	{
		VERIFY_EXIT(NOT_NULL(_log_data) && NOT_NULL(m_log_data));
		m_log_data->add_ref();
	}

public:
	LogObject()
		:m_log_data(NULL)
	{
	}

	LogObject(const LogObject& _logobj)
	{
		operator=(_logobj);
	}

	LogObject& operator = (const LogObject& _logobj)
	{
		m_log_data = _logobj.m_log_data;
		m_log_data->add_ref();
		return *this;
	}

	~LogObject()
	{
		if(NOT_NULL(m_log_data))
		{
			m_log_data->release_ref();
			m_log_data = NULL;
		}
	}

	void release()
	{
		if(NOT_NULL(m_log_data))
		{
			m_log_data->release_ref();
			m_log_data = NULL;
		}
	}

	_ObjectType& get_ref() 
	{
		VERIFY_EXIT1(NOT_NULL(m_log_data),Fish<_ObjectType>::get());
		return m_log_data->get_ref();
	}

	const _ObjectType& get_ref() const 
	{
		VERIFY_EXIT1(NOT_NULL(m_log_data),Fish<_ObjectType>::get());
		return m_log_data->get_ref();
	}

	_ObjectType* get() 
	{
		VERIFY_EXIT1(NOT_NULL(m_log_data),&Fish<_ObjectType>::get());
		return &m_log_data->get_ref();
	}

	const _ObjectType* get() const 
	{
		VERIFY_EXIT1(NOT_NULL(m_log_data),&Fish<_ObjectType>::get());
		return &m_log_data->get_ref();
	}

	_ObjectType* operator->() 
	{
		return get();
	}

	const _ObjectType* operator->() const 
	{
		return get();
	}

	friend struct Logs;
};//template<> struct LogObject

template<typename _Type> inline
LogObject<_Type> Logs::create_object(const CString& _name)
{
	if(!FileUtils::IsPathExist(m_log_path))
		FileUtils::CreateDirectory(m_log_path);
	CString path,fname,ext;
	FileUtils::SplitePath(_name,path,fname,ext);
	path = m_log_path;
	FileUtils::SetLastSlash(path);
	path += fname + _T(".log");
	Private::LogObjectData<_Type>* pdata = trace_alloc(new Private::LogObjectData<_Type>(*this,path));
	m_logs.push_back(pdata);
	return LogObject<_Type>(pdata);
}

template<typename _Type> inline
void Logs::delete_log_object(Private::LogObjectData<_Type>* _objbase)
{
	LogsVec::iterator 
		it = m_logs.begin()
		,ite = m_logs.end()
		;
	for(;it!=ite;++it)
	{
		if(*it==_objbase)
		{
			m_logs.erase(it);
			return;
		}
	}
	VERIFY_EXIT(false);// object was not founded
}

/*

struct Test
{
	struct Data
	{
		STRUCTNAME(Test::Data);

		CString m_text;
		long m_id;

		Data()
			:m_id(0)
		{
		}

		struct DefaultLayout : public Layout<Data>
		{
			DefaultLayout()
			{
				add_simple(_T("text"),&Data::m_text);
				add_simple(_T("id"),&Data::m_id);
			}
		};

		void rand()
		{
			size_t sz = ::rand()%30 + 2;
			size_t i = 0;
			m_text.Empty();
			for(i=0;i<sz;i++)
				m_text += (TCHAR)(::rand()%(_T('z') - _T('a') + 1) + _T('a'));
			m_id = ::rand();
		}

		template<typename _Stream>
		void print(_Stream& _out) const 
		{
			_out << m_id << _T(" \"") << (LPCTSTR)m_text << _T("\"\n");
		}
	};

	LogFile<Data> data_log;

	Test()
	{
		data_log.open(_T("data.dta"),true);
	}

	void populate()
	{
		size_t i=0;
		for(i=data_log.size();i<20;i++)
		{
			Data dta;
			dta.rand();
			data_log.append(dta);
		}
	}

	void print()
	{
		size_t i=0;
		for(i=0;i<data_log.size();i++)
		{
			Data dta;
			data_log.get(i,dta);
			dta.print(std::tcout);
		}
	}

	void test_logs()
	{
		Logs logs;
		logs.set_path(_T("logs"));

		LogObject<Data> log_object = logs.create_object<Data>(_T("Test_Data"));

		log_object->rand();
		log_object->rand();		// will be written to log just last (this) record

		logs.append();

		//log_object.release();
	}
};//struct Test
*/


#endif //#if !defined(__FILELOG_H__170E7176_FA12_4299_87AE_AE078548C88D_INCLUDE)