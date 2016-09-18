#pragma once

#include "typelist.h"
#include "serialize.h"
#include "socketserializer.h"
#include "memserializer.h"
#include "thread.h" 
#include "sockets.h"
#include "tracealloc.h"

namespace Private
{
	template<class _TOwner,class _TMessagesTypeLst>
	struct ProcessMessageT
	{
		typedef typename _TMessagesTypeLst::Item Item;
		typedef typename _TMessagesTypeLst::Tail Tail;

		static void process_message(_TOwner& _obj,IArchive& _p)
		{
			CPtrShared<INode> pnode = _p.get_root();
			VERIFY_EXIT(NOT_NULL(pnode));
			pnode = pnode->get_child(get_structname<Item>());
			if(NOT_NULL(pnode))
			{
				TRACE(_T("process_message (%s)\n"),get_structname<Item>());
				Item item;
				load(_p,get_structname<Item>(),item,get_structlayout<Item>());
				_obj.process_message(item);
				return;
			}
			ProcessMessageT<_TOwner,Tail>::process_message(_obj,_p);
		}
	};//template<> struct ProcessMessageT

	template<class _TOwner>
	struct ProcessMessageT<_TOwner,NullType>
	{
		static void process_message(_TOwner& _obj,IArchive& _p)
		{
			CPtrShared<INode> pnode = _p.get_root();
			VERIFY_EXIT(NOT_NULL(pnode));
			CPtrShared<INodeList> pnodes = pnode->get_allchildlist();
			CString_ name;
			if(NOT_NULL(pnodes)) 
			{
				pnode = (*pnodes)[0];
				if(NOT_NULL(pnode))
					name = pnode->get_name();
			}
			_obj.unknown_message(name);
		}
	};//template<> struct ProcessMessageT
};//namespace Private 

template<class _TOwner,class _TMessagesTypeLst>
struct ServerMessageReader 
	: public CWorkedThreadImpl<ServerMessageReader<_TOwner,_TMessagesTypeLst> > 
	,public SyncSocket
{
	typedef CWorkedThreadImpl<ServerMessageReader<_TOwner,_TMessagesTypeLst> >  Thread;
	_TOwner* m_powner;

	ServerMessageReader()
		:m_powner(NULL)
	{
		m_bAutoCleanup = false;
	}

	DWORD thread_main()
	{
		if(IS_NULL(m_powner))
		{
			exit_reader_thread();
			return 0;
		}
		try
		{
			sync_access();
		}
		catch(utils::IUtilitiesException EXC_ACCESS_TYPE _pexc)
		{
			ExceptionPtr<utils::IUtilitiesException> pexc(_pexc);
			bool bexit = false;
			if(NOT_NULL(m_powner))
				m_powner->on_server_error(*pexc,bexit);
			if(bexit)
			{
				exit_reader_thread();
				return 0;
			}
		}
		
		for(;!is_aborted();)
		{
			try
			{
				CSocketLoader loader(*this,this);
				CMemoryArchive arch;
				loader.load(arch);

				Private::ProcessMessageT<_TOwner,_TMessagesTypeLst>::process_message(*m_powner,arch); 
			}
			catch(utils::IUtilitiesException EXC_ACCESS_TYPE _pexc)
			{
				ExceptionPtr<utils::IUtilitiesException> pexc(_pexc);
				bool bexit = false;
				if(NOT_NULL(m_powner))
					m_powner->on_server_error(*pexc,bexit);
				if(bexit) break;
			}
			catch(std::exception& _exc)
			{
				string_converter<char,TCHAR> msgT(_exc.what(),-1,CP_ACP);
				utils::CErrorTag __et(cnamedvalue(_T("what"),msgT));
				utils::CUtilitiesExceptionBase exc;
				exc.initialize_exception();

				bool bexit = false;
				if(NOT_NULL(m_powner))
					m_powner->on_server_error(exc,bexit);
				if(bexit) break;
			}
			catch(StopException& )
			{
				break;
			}
			catch(...)
			{
				break;
			}
		}
		exit_reader_thread();
		return 0;
	}

	virtual void OnClose(int _err)
	{
		abort();
	}

protected:
	void exit_reader_thread();
	void sync_access()
	{
		int err = ERROR_SUCCESS;
		std::vector<byte> buf;
		buf.resize(1,'U');	// horseshoe for success
		if(!async_send(buf,err))
			throw_win32Error(err);
		if(!wait_send(get_stophandle(),INFINITE,err))
			throw_win32Error(err);
	}
};//template<> struct ServerMessageReader

template<class _TOwner,class _TMessagesTypeLst>
struct SocketMessageServer : public AsyncSocket
{
protected:
	typedef std::vector<ServerMessageReader<_TOwner,_TMessagesTypeLst>*> Readers;

protected:
	CEvent closed_readers_evnt;
	CCriticalSection m_critsect;
	Readers readers;
public:

	SocketMessageServer()
		:closed_readers_evnt(TRUE,TRUE)
	{
#if defined(TRACE_SOCKETS_OPERATIONS)
		m_socket.set_name(CString(_T("SocketMessageServer_"))+get_structname<_TOwner>());
#endif
	}

	~SocketMessageServer()
	{
		predestruct();
	}

	void predestruct()
	{
		stop_server();
		wait_readers_close();
	}

	virtual void OnAccept()
	{
		CAutoLock __al(m_critsect);
		ServerMessageReader<_TOwner,_TMessagesTypeLst>* reader = trace_alloc((new ServerMessageReader<_TOwner,_TMessagesTypeLst>()));
		reader->m_powner = static_cast<_TOwner*>(this);
		int err = 0;
#if defined(TRACE_SOCKETS_OPERATIONS)
		reader->set_name(CString(_T("ServerMessageReader_")) + get_structname<_TOwner>());
#endif
		if(!reader->accept(*this,err))
		{
			OnError(err);
			delete trace_free(reader);
			reader = NULL;
			return;
		}
		closed_readers_evnt.ResetEvent();
		readers.push_back(reader);
		reader->start();
	}

	virtual void OnError(int _err,int _msg = 0)
	{
		utils::CWin32Exception exc(CREATE_SOURCEINFO(),_err,NULL);
		bool bexit = false;
		on_server_error(exc,bexit);
	}

	virtual void OnClose(int _err)
	{
		stop_server();
	}

	void wait_readers_close(DWORD _millisec = INFINITE)
	{
		wait4event(closed_readers_evnt,_millisec);
	}

	template<typename _Type>
	void process_message(const _Type&)
	{
	}

	void unknown_message(const CString_& _name)
	{
	} 

	void on_server_error(utils::IUtilitiesException& _exc,bool& _bexit)
	{
	}

	void stop_server()
	{
		CAutoLock __al(m_critsect);
		Readers::iterator
			it = readers.begin()
			,ite = readers.end()
			;
		for(;it!=ite;++it)
		{
			(**it).abort();
		}
	}

protected:
	void remove_reader(ServerMessageReader<_TOwner,_TMessagesTypeLst>* _preader)
	{
		CAutoLock __al(m_critsect);
		Readers::iterator it = std::find(readers.begin(),readers.end(),_preader);
		if(it!=readers.end())
			readers.erase(it);
		if(readers.empty())
			closed_readers_evnt.SetEvent();
	}

	template<class _TOwner,class _TMessagesTypeLst>
	friend struct ServerMessageReader;
};//template<> struct SocketMessageServer 

template<class _TOwner,class _TMessagesTypeLst> inline
void ServerMessageReader<_TOwner,_TMessagesTypeLst>::exit_reader_thread()
{
	if(NOT_NULL(m_powner))
	{
		static_cast<SocketMessageServer<_TOwner,_TMessagesTypeLst>* >(m_powner)->remove_reader(this);
	}
	m_bAutoDelete = true;
}

struct SocketMessageClient : public SyncSocket
{
	void sync_access()
	{
		int err = ERROR_SUCCESS;
		std::vector<byte> buf;
		buf.resize(1,0);
		if(!async_read(&buf,err))
			throw_win32Error(err);
		if(!wait_read(NULL,INFINITE,err))
			throw_win32Error(err);
	}

	template<typename _Message>
	void send(const _Message& _msg)
	{
		CMemoryArchive arch;
		::save(arch,get_structname<_Message>(),_msg,get_structlayout<_Message>());
		CSocketSaver saver(*this);
		saver.visit(&arch);
	}

	template<typename _Message>
	void send(const _Message* _pmsg)
	{
		VERIFY_EXIT(NOT_NULL(_pmsg))
		CMemoryArchive arch;
		::save(arch,get_structname<_Message>(),*_pmsg,get_structlayout<_Message>());
		CSocketSaver saver(*this);
		saver.visit(&arch);
	}
};//struct SocketMessageClient

