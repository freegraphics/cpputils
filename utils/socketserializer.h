#pragma once

#include <security/safeint.hpp>
#include "utils.h"
#include "errors.h"
#include "thread.h"
#include "sockets.h"

#pragma pack(push)
#pragma pack(1)

namespace Private
{
	struct CValuePre
	{
		size_t m_size;
		bool m_bTag;
		bool m_bOpen;

		CValuePre()
			:m_size(0)
			,m_bTag(false)
			,m_bOpen(false)
		{
		}
	};

	struct CValue
	{
		CValuePre pre;
		WCHAR buf[1];

		CValue()
		{
			buf[0] = 0;
		}

		static void write(const CString_& _sValue,bool _bTag,bool _bOpen,size_t& _sz,std::vector<BYTE>& _buf)
		{
			try
			{
				SafeInt<size_t> sz = 
					SafeInt<size_t>(_sValue.GetLength()+1)*sizeof(WCHAR)
					+ sizeof(CValuePre)
					;
				_sz = sz;
			}
			catch(SafeIntException&)
			{
				_sz = 0;
				_buf.resize(0);
				return;
			}
			_buf.resize(_sz);
			CValue* pdata = reinterpret_cast<CValue*>(&_buf[0]);
			pdata->pre.m_size = _sValue.GetLength();
			pdata->pre.m_bTag = _bTag;
			pdata->pre.m_bOpen = _bOpen;
			string_converter<TCHAR,WCHAR> sValueW(_sValue,CP_ACP);
			wcscpy_s((PWCHAR)(pdata->buf),_sValue.GetLength()+1,sValueW);
		}

		static void prepare(const CValuePre& _pre,size_t& _sz,std::vector<BYTE>& _buf)
		{
			try
			{
				SafeInt<size_t> sz =
					SafeInt<size_t>(_pre.m_size+1)*sizeof(WCHAR)
					;
				_sz = sz;
			}
			catch(SafeIntException&)
			{
				_sz = 0;
				_buf.resize(0);
				return;
			}
			_buf.resize(_sz);
		}

		static bool read(const CValuePre& _pre,const std::vector<BYTE>& _buf,CString_& _sValue,bool& _bTag,bool& _bOpen)
		{
			try
			{
				SafeInt<size_t>	sz = 
					SafeInt<size_t>(_pre.m_size+1)*sizeof(WCHAR)
					;
				if(sz!=_buf.size()) return false;
			}
			catch(SafeIntException&)
			{
				return false;
			}
			string_converter<WCHAR,TCHAR> sValueT(reinterpret_cast<const WCHAR*>(&_buf[0]),(long)_pre.m_size,CP_ACP);
			_sValue = (TCHAR*)sValueT;
			_bTag = _pre.m_bTag;
			_bOpen = _pre.m_bOpen;
			return true;
		}
	};//struct CValue
};//namespace Private

#pragma pack(pop)

struct StopException{};

struct CSocketOps
{
protected:
	SyncSocket& m_socket;
	HANDLE m_hstopevent;

public:
	CSocketOps(SyncSocket& _socket)
		:m_socket(_socket)
		,m_hstopevent(NULL)
	{
	}

	void write(const CString_& _sValue,bool _bTag,bool _bOpen)
	{
		size_t sz = 0;
		std::vector<BYTE> buf;
		Private::CValue::write(_sValue,_bTag,_bOpen,sz,buf);
		if(buf.empty() || sz==0)
			throw_error(_T("failed to create buffer"));
		int err = ERROR_SUCCESS;
		if(!m_socket.async_send(buf,err))
			error_throw(err);
		if(!m_socket.wait_send(m_hstopevent,INFINITE,err))
			error_throw(err);
	}

	void write_tag(const CString_& _sName,bool _bOpen)
	{
		write(_sName,true,_bOpen);
	}

	void write_value(const CString_& _sValue)
	{
		write(_sValue,false,false);
	}

	void read(CString_& _sValue,bool& _bTag,bool& _bOpen)
	{
		int err = ERROR_SUCCESS;
		std::vector<byte> pre_buf;
		pre_buf.resize(sizeof(Private::CValuePre));
		Private::CValuePre* pre = (Private::CValuePre*)(&pre_buf[0]);

		if(!m_socket.async_read(&pre_buf,err))
			error_throw(err);
		if(!m_socket.wait_read(m_hstopevent,INFINITE,err))
			error_throw(err);

		size_t sz=0;
		std::vector<byte> buf;
		Private::CValue::prepare(*pre,sz,buf);

		if(buf.empty() || sz==0)
			throw_error(_T("failed to create buffer"));

		if(!m_socket.async_read(&buf,err))
			error_throw(err);
		if(!m_socket.wait_read(m_hstopevent,INFINITE,err))
			error_throw(err);

		if(!Private::CValue::read(*pre,buf,_sValue,_bTag,_bOpen))
			throw_error(_T("failed to read value"));
	}

	void set_stophandle(IThreadState* _pstate)
	{
		if(IS_NULL(_pstate)) return;
		m_hstopevent = _pstate->get_stophandle();
	}
protected:
	void error_throw(int _err)
	{
		if(_err==ERROR_SUCCESS)
			throw StopException();
		else
			throw_win32Error(_err);
	}
};//struct CSocketOps

class CSocketLoader 
{
protected:
	CSocketOps m_socket;

	struct LastRead
	{
		CString_ m_sValue;
		bool m_bTag;
		bool m_bOpen;

		bool isOpenTag() const {return m_bTag && m_bOpen;}
		bool isCloseTag() const {return m_bTag && !m_bOpen;}
		bool isValue() const {return !m_bTag;}
		const CString_& get_name() const {return m_sValue;}
		const CString_& get_value() const {return m_sValue;}
	};

	LastRead m_top;

public:
	CSocketLoader(SyncSocket& _socket)
		:m_socket(_socket)
	{
	}

	template<typename _HolderClass>
	CSocketLoader(SyncSocket& _socket,_HolderClass* _pholder)
		:m_socket(_socket)
	{
		m_socket.set_stophandle(reinterpret_cast<IThreadState*>(_pholder));
	}

/*
---pseudo code------------

	process()
	{
		while(is_open_tag)
		{
			name = get_top();
			create_node(name);
			next();
			if(! is_value) error();
			set_node_value();
			next();
			process();
			if(!is_close_tag() || get_top()!=name) error();
			next();
		}
	}
*/

	void read()
	{
		m_socket.read(m_top.m_sValue,m_top.m_bTag,m_top.m_bOpen);
	}

	void load(IArchive& _archive,HANDLE _hStopHandle = NULL)
	{
		VERIFY_EXIT(NOT_NULL(&_archive) && NOT_NULL(_archive.get_root()));
		INode* pnode = _archive.get_root();
		read();
		utils::CErrorTag __et(
			cnamedvalue(_T("tag"),m_top.m_sValue)
			,cnamedvalue(_T("bTag"),m_top.m_bTag)
			,cnamedvalue(_T("bOpen"),m_top.m_bOpen)
			);

		if(!m_top.isOpenTag()) throw_error(_T("expected open tag"));
		pnode->set_name(m_top.get_name());
		read();

		utils::CErrorTag __et1(
			cnamedvalue(_T("value"),m_top.m_sValue)
			,cnamedvalue(_T("bTag1"),m_top.m_bTag)
			,cnamedvalue(_T("bOpen1"),m_top.m_bOpen)
			);

		if(!m_top.isValue()) throw_error(_T("expected value of tag"));
		pnode->set_value(m_top.get_value());
		read();

		utils::CErrorTag __et2(
			cnamedvalue(_T("next"),m_top.m_sValue)
			,cnamedvalue(_T("bTag2"),m_top.m_bTag)
			,cnamedvalue(_T("bOpen2"),m_top.m_bOpen)
			);

		process(pnode);
		if(!m_top.isCloseTag()
			|| m_top.get_name()!=pnode->get_name()
			) 
			throw_error(_T("expected close tag"));
	}

	void process(INode* _parent)
	{
		VERIFY_EXIT(NOT_NULL(_parent));
		while(m_top.isOpenTag())
		{
			utils::CErrorTag __et(
				cnamedvalue(_T("tag"),m_top.m_sValue)
				,cnamedvalue(_T("bTag"),m_top.m_bTag)
				,cnamedvalue(_T("bOpen"),m_top.m_bOpen)
				);

			INode* pnode = _parent->add_child(m_top.get_name());
			read();

			utils::CErrorTag __et1(
				cnamedvalue(_T("value"),m_top.m_sValue)
				,cnamedvalue(_T("bTag1"),m_top.m_bTag)
				,cnamedvalue(_T("bOpen1"),m_top.m_bOpen)
				);

			if(!m_top.isValue()) throw_error(_T("expected value of tag"));
			pnode->set_value(m_top.get_value());
			read();

			utils::CErrorTag __et2(
				cnamedvalue(_T("next"),m_top.m_sValue)
				,cnamedvalue(_T("bTag2"),m_top.m_bTag)
				,cnamedvalue(_T("bOpen2"),m_top.m_bOpen)
				);

			process(pnode);
			if(!m_top.isCloseTag()
				|| m_top.get_name()!=pnode->get_name()
				) 
				throw_error(_T("expected close tag"));
			read();
		}
	}
};//


class CSocketSaver : public ISerializeVisitor
{
protected:
	CSocketOps m_socket;

public:
	CSocketSaver(SyncSocket& _socket)
		:m_socket(_socket)
	{
	}

	template<typename _Holder>
	CSocketSaver(SyncSocket& _socket,_Holder* _pholder)
		:m_socket(_socket)
	{
		m_socket.set_stophandle(_pholder);
	}

public:
	void save(IArchive& _arch)
	{
		visit(&_arch);
	}

	virtual void visit(IArchive* _p)
	{
		VERIFY_EXIT(NOT_NULL(_p) && NOT_NULL(_p->get_root()));
		INode* pnode = _p->get_root();
		if(pnode->get_name().IsEmpty())
			pnode->set_name(_T("root"));
		pnode->visit(*this);
	}

	virtual void visit(INode* _p)
	{
		VERIFY_EXIT(NOT_NULL(_p));
		utils::CErrorTag __et(
			cnamedvalue(_T("name"),_p->get_name())
			,cnamedvalue(_T("value"),_p->get_value())
			);

		m_socket.write_tag(_p->get_name(),true);
		m_socket.write_value(_p->get_value());

		CPtrShared<INodeList> pnodelist = _p->get_allchildlist();
		pnodelist->visit(*this);

		m_socket.write_tag(_p->get_name(),false);
	}

	virtual void visit(INodeList* _p)
	{
		VERIFY_EXIT(NOT_NULL(_p));
		size_t i=0,cnt=_p->size();
		for(i=0;i<cnt;i++)
		{
			CPtrShared<INode> pnode = (*_p)[(long)i];
			VERIFY_DO(NOT_NULL(pnode),continue);
			pnode->visit(*this);
		}
	}
};

