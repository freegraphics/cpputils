#pragma once

// use
// #define TRACE_SOCKETS_OPERATIONS
// to trace sockets operations to files (one file for one socket) and to the debug console

#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

#include "utils.h"
#include "thread.h"
#include "cyclicbuffer.h"
#include "queue.h"
#include "TraceFile.h"


template<typename _Handler,typename _Class>
struct Handles
{
protected:
	typedef std::map<_Handler,_Class*> Handle2ObjMap;
	Handle2ObjMap handle2obj;
	CCriticalSection m_critsec;
public:
	bool add(_Handler _handle,_Class* _pobj)
	{
		CAutoLock __al(m_critsec);
		if(handle2obj.find(_handle)!=handle2obj.end()) return false;
		handle2obj.insert(Handle2ObjMap::value_type(_handle,_pobj));
		return true;
	}

	bool find_obj(_Handler _handle,_Class*& _pobj) const
	{
		CAutoLock __al(m_critsec);
		_pobj = NULL;
		Handle2ObjMap::const_iterator it = handle2obj.find(_handle);
		if(it==handle2obj.end()) return false;
		_pobj = const_cast<_Class*>(it->second);
		return true;
	}

	bool find(const _Class* _pobj,_Handler& _handle) const
	{
		CAutoLock __al(m_critsec);
		Handle2ObjMap::const_iterator
			it = handle2obj.begin()
			,ite = handle2obj.end()
			;
		for(;it!=ite;++it)
		{
			if(it->second==_pobj)
			{
				_handle = it->first;
				return true;
			}
		}
		_handle = NULL;
		return false;
	}

	bool remove(_Handler _handle)
	{
		CAutoLock __al(m_critsec);
		Handle2ObjMap::iterator it = handle2obj.find(_handle);
		if(it==handle2obj.end()) return false;
		handle2obj.erase(it);
		return true;
	}
};//template<> struct Handles

template<typename _Handler>
struct NotifyMessageWindowImpl 
	: public CWorkedThreadImpl<NotifyMessageWindowImpl<_Handler> >
{
protected:
	typedef CWorkedThreadImpl<NotifyMessageWindowImpl<_Handler> > Thread;
	HINSTANCE m_hInstance;
	static singleton<Handles<HWND,NotifyMessageWindowImpl<_Handler> > > g_handles;
	CEvent evnt;
public:

	NotifyMessageWindowImpl(HINSTANCE _hInstance = GetModuleHandle(NULL))
		:m_hInstance(_hInstance)
		,evnt(FALSE,FALSE)
	{
		m_bAutoCleanup = false;
		start();
		::is_signaled(evnt,INFINITE);
	}

	~NotifyMessageWindowImpl()
	{
		//stop();
		//predestruct();
	}

	void start()
	{
		Thread::start();
	}

	HWND get_hwnd() const
	{
		HWND hwnd = NULL;
		if(!g_handles.get().find(this,hwnd)) return NULL;
		return hwnd;
	}

	void stop()
	{
		HWND hwnd = NULL;
		if(g_handles.get().find(this,hwnd))
		{
			::PostMessage(hwnd,WM_QUIT,NULL,NULL);
			::is_signaled(evnt,INFINITE);
			wait(INFINITE);
		}
	}

	DWORD thread_main()
	{
		create_window(m_hInstance);
#if defined(USE_MFC)
		evnt.SetEvent();
#else
		evnt.Set();
#endif
		do_messages_loop();
		destroy_window();
		m_bAutoDelete = true;
#if defined(USE_MFC)
		evnt.SetEvent();
#else
		evnt.Set();
#endif
		return 0;
	}

	LRESULT WindowProc(HWND _hWnd,UINT _message,WPARAM _wParam,LPARAM _lParam)
	{
		return ::DefWindowProc(_hWnd,_message,_wParam,_lParam);
	}

	static LRESULT CALLBACK WinProc(HWND _hWnd,UINT _message,WPARAM _wParam,LPARAM _lParam)
	{
		NotifyMessageWindowImpl<_Handler>* pthis = NULL; 
		if(!g_handles.get().find_obj(_hWnd,pthis))
			::DefWindowProc(_hWnd,_message,_wParam,_lParam);
		return static_cast<_Handler*>(pthis)->WindowProc(_hWnd,_message,_wParam,_lParam);
	}

protected:
	bool create_window(HINSTANCE _hInstance)
	{
		WNDCLASSEX wClass;
		ZeroObj(wClass);
		wClass.cbClsExtra=NULL;
		wClass.cbSize=sizeof(WNDCLASSEX);
		wClass.cbWndExtra=NULL;
		wClass.hbrBackground=(HBRUSH)COLOR_WINDOW;
		wClass.hCursor=LoadCursor(NULL,IDC_ARROW);
		wClass.hIcon=NULL;
		wClass.hIconSm=NULL;
		wClass.hInstance=_hInstance;
		wClass.lpfnWndProc=(WNDPROC)WinProc;
		wClass.lpszClassName=window_class_name();
		wClass.lpszMenuName=NULL;
		wClass.style=CS_HREDRAW|CS_VREDRAW;

		if(!RegisterClassEx(&wClass))
			return false;

		HWND hwnd = CreateWindowEx(NULL
			,window_class_name()
			,window_class_name()
			,WS_OVERLAPPEDWINDOW
			,0,0,0,0
			,NULL
			,NULL
			,_hInstance
			,NULL
			);

		if(IS_NULL(hwnd))
			return false;

		g_handles.get_non_const().add(hwnd,this);

		::ShowWindow(hwnd,SW_HIDE);

		return true;
	}

	void do_messages_loop()
	{
		MSG msg;
		ZeroObj(msg);

		for(;GetMessage(&msg,NULL,0,0) && !is_aborted();)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void destroy_window()
	{
		HWND hwnd = NULL;
		if(g_handles.get().find(this,hwnd))
		{
			::DestroyWindow(hwnd);
			g_handles.get_non_const().remove(hwnd);
		}
	}

	static LPCTSTR window_class_name()
	{
		return _T("NotifyMessageWindowImplClass");
	}
};//template<> struct NotifyMessageWindowImpl 

struct WSASetup
{
protected:
	WSADATA wsadata;
	long counter;
	CCriticalSection critsect;
public:
	WSASetup()
		:counter(0)
	{
		ZeroObj(wsadata);
	}

	bool add_reference()
	{
		CAutoLock __al(critsect);
		VERIFY_EXIT1(counter>=0,false);
		if(counter==0)
		{
			int nResult = ::WSAStartup(MAKEWORD(2,2),&wsadata);
			if(nResult!=0) return false;
		}
		counter++;
		return true;
	}

	void release_reference()
	{
		CAutoLock __al(critsect);
		VERIFY_EXIT(counter>0);
		if(--counter==0)
		{
			::WSACleanup();
		}
	}
};//struct WSASetup

namespace Private
{
	inline
	void bytes_to_string(const byte* _buf,int _sz,CString& _str)
	{
		_str.Empty();
		if(IS_NULL(_buf)) return;
		int i = 0;
		for(i=0;i<_sz;i++)
		{
			_str += Format(_T("%02X "),_buf[i]);
		}
		_str += _T("\t");
		for(i=0;i<_sz;i++)
		{
			_str += Format(_T("%c"),_buf[i]>=0x20?_buf[i]:_T('.'));
		}
	}
};//namespace Private

struct Socket
{
	static bool is_error(int _ret)
	{
		return EQL(_ret,SOCKET_ERROR);
	}

	static bool is_error(SOCKET _socket)
	{
		return EQL(_socket,INVALID_SOCKET);
	}

	struct ReturnProcessor
	{
		int ret;

		ReturnProcessor(int _ret,int& _err)
			:ret(_ret)
		{
			_err = Socket::is_error(_ret)?WSAGetLastError():0;
		}

		operator bool () const
		{
			return !Socket::is_error(ret);
		}

		operator int () const
		{
			return ret;
		}
	};//struct ReturnProcessor

protected:
	static singleton<WSASetup> g_wsasetup;
	SOCKET m_socket;
#if defined(TRACE_SOCKETS_OPERATIONS)
	CString m_name;
#endif

public:

	Socket()
		:m_socket(NULL)
	{
		g_wsasetup.get_non_const().add_reference();
	}

	~Socket()
	{
		int err = 0;
		close(err);
		g_wsasetup.get_non_const().release_reference();
	}

	void set_name(const CString& _name)
	{
#if defined(TRACE_SOCKETS_OPERATIONS)
		m_name = _name;
#endif
	}

	operator SOCKET () const 
	{
		return m_socket;
	}

	bool operator == (SOCKET _socket) const
	{
		return m_socket==_socket;
	}

	bool is_valid() const
	{
		return NOT_NULL(m_socket) && NEQL(m_socket,INVALID_SOCKET);
	}

	bool create(int _af,int _type,int _protocol,int& _err)
	{
		_err = 0;
		VERIFY_EXIT1(IS_NULL(m_socket) || EQL(m_socket,INVALID_SOCKET),false);
		m_socket = ::socket(_af,_type,_protocol);
		if(is_error(m_socket))
		{
			_err = WSAGetLastError();
			return false;
		}
#if defined(TRACE_SOCKETS_OPERATIONS)
		TRACE(_T("socket \"%s\" (0x%08X) create()\n"),(LPCTSTR)m_name,m_socket);
#endif
		return true;
	}

	int send(const byte* _buf,int _len,int _flags,int& _err)
	{
#if defined(TRACE_SOCKETS_OPERATIONS)
		CString data;
		Private::bytes_to_string(_buf,_len,data);
		TRACE(_T("socket \"%s\" (0x%08X) send() data=\"%s\"\n"),(LPCTSTR)m_name,m_socket,(LPCTSTR)data);
#endif
		int sended = ::send(m_socket,(const char*)_buf,_len,_flags);
		return ReturnProcessor(sended,_err);
	}

	bool ioctrlsocket(long _cmd,u_long* _argp,int& _err)
	{
		int ret = ::ioctlsocket(m_socket,_cmd,_argp);
		return ReturnProcessor(ret,_err);
	}

	int recv(byte* _buf,int _len,int _flags,int& _err)
	{
		int received = ::recv(m_socket,(char*)_buf,_len,_flags);
#if defined(TRACE_SOCKETS_OPERATIONS)
		CString data;
		Private::bytes_to_string(_buf,received,data);
		TRACE(_T("socket \"%s\" (0x%08X) receive() data=\"%s\"\n"),(LPCTSTR)m_name,m_socket,(LPCTSTR)data);
#endif
		return ReturnProcessor(received,_err);
	}

	bool bind(const struct sockaddr* _name, int _namelen, int& _err)
	{
#if defined(TRACE_SOCKETS_OPERATIONS)
		TRACE(_T("socket \"%s\" (0x%08X) bind()\n"),(LPCTSTR)m_name,m_socket);
#endif
		int ret = ::bind(m_socket,_name,_namelen);
		return ReturnProcessor(ret,_err);
	}

	bool WASAsyncSelect(HWND _hWnd,unsigned int _wMsg,long _lEvent, int& _err)
	{
#if defined(TRACE_SOCKETS_OPERATIONS)
		TRACE(_T("socket \"%s\" (0x%08X) WASAsyncSelect()\n"),(LPCTSTR)m_name,m_socket);
#endif
		int ret = ::WSAAsyncSelect(m_socket,_hWnd,_wMsg,_lEvent);
		return ReturnProcessor(ret,_err);
	}

	bool close(int& _err)
	{
		if(EQL(m_socket,0)) return true;
#if defined(TRACE_SOCKETS_OPERATIONS)
		TRACE(_T("socket \"%s\" (0x%08X) close()\n"),(LPCTSTR)m_name,m_socket);
#endif
		int ret = ::closesocket(m_socket);
		m_socket = NULL;
		return ReturnProcessor(ret,_err);
	}

	bool listen(int _backlog,int& _err)
	{
#if defined(TRACE_SOCKETS_OPERATIONS)
		TRACE(_T("socket \"%s\" (0x%08X) listen()\n"),(LPCTSTR)m_name,m_socket);
#endif
		int ret = ::listen(m_socket,_backlog);
		return ReturnProcessor(ret,_err);
	}

	bool accept(const Socket& _socket,sockaddr* _sockAddrClient,int* _addrLen,int& _err)
	{
		_err = 0;
		VERIFY_EXIT1(IS_NULL(m_socket) || EQL(m_socket,INVALID_SOCKET),false);
		m_socket = ::accept(_socket,_sockAddrClient,_addrLen);
#if defined(TRACE_SOCKETS_OPERATIONS)
		TRACE(_T("socket \"%s\" (0x%08X)  from (0x%08X) accept()\n"),(LPCTSTR)m_name,m_socket,(SOCKET)_socket);
#endif
		if(is_error(m_socket))
		{
			_err = WSAGetLastError();
			return false;
		}
		return true;
	}

	bool connect(SOCKADDR* _socketAddr, int _socketAddrSize, int& _err)
	{
#if defined(TRACE_SOCKETS_OPERATIONS)
		TRACE(_T("socket \"%s\" (0x%08X) connect()\n"),(LPCTSTR)m_name,m_socket);
#endif
		int ret = ::connect(m_socket,_socketAddr,_socketAddrSize);
		return ReturnProcessor(ret,_err);
	}

	bool shutdown(int _how, int& _err)
	{
#if defined(TRACE_SOCKETS_OPERATIONS)
		TRACE(_T("socket \"%s\" (0x%08X) shutdown()\n"),(LPCTSTR)m_name,m_socket);
#endif
		int ret = ::shutdown(m_socket,_how);
		return ReturnProcessor(ret,_err);
	}
};//struct Socket


struct AsyncSocketNotifier;

interface SocketNotifyBase
{
protected:
	Socket m_socket;
public:
	SocketNotifyBase()
	{
	}

	SocketNotifyBase(const SocketNotifyBase& _obj)
		:m_socket(_obj.m_socket)
	{
	}

	bool operator == (SOCKET _socket) const
	{
		return m_socket==_socket;
	}

	void set_socket_name(const CString& _name)
	{
		m_socket.set_name(_name);
	}

	virtual void OnConnect() = 0;
	virtual void OnAccept() = 0;
	virtual void OnRead() = 0;
	virtual void OnWrite() = 0;
	virtual void OnReadBuffer() = 0;
	virtual void OnWriteBuffer() = 0;
	virtual void OnClose(int _err) = 0;
	virtual void OnError(int _err,int _msg = 0) = 0;

protected:
	friend struct AsyncSocketNotifier;
};//interface SocketNotifyBase

struct SocketNotifyBaseList : protected std::vector<SocketNotifyBase*>
{
protected:
	typedef std::vector<SocketNotifyBase*> base;
	CCriticalSection m_critsect;
public:
	SocketNotifyBaseList()
	{
	}

	~SocketNotifyBaseList()
	{
	}

	bool find(SocketNotifyBase* _psocket_data) const
	{
		CAutoLock __al(m_critsect);
		return NEQL(std::find(begin(),end(),_psocket_data),end());
	}

	const SocketNotifyBase* find(SOCKET _socket) const
	{
		CAutoLock __al(m_critsect);
		base::const_iterator
			it = begin()
			,ite = end()
			;
		for(;it!=ite;++it)
		{
			if(**it==_socket) return *it;
		}
		return NULL;
	}

	SocketNotifyBase* find(SOCKET _socket)
	{
		CAutoLock __al(m_critsect);
		base::iterator
			it = begin()
			,ite = end()
			;
		for(;it!=ite;++it)
		{
			if(**it==_socket) return *it;
		}
		return NULL;
	}

	bool add(SocketNotifyBase* _psocket_data)
	{
		CAutoLock __al(m_critsect);
		base::iterator it = std::find(begin(),end(),_psocket_data);
		VERIFY_EXIT1(EQL(it,end()),false); // can`t add second time
		insert(end(),_psocket_data);
		return true;
	}

	bool remove(SocketNotifyBase* _psocket_data)
	{
		CAutoLock __al(m_critsect);
		base::iterator it = std::find(begin(),end(),_psocket_data);
		VERIFY_EXIT1(NEQL(it,end()),false); // can`t add second time
		erase(it);
		return true;
	}
};

struct AsyncSocketNotifier 
	: public NotifyMessageWindowImpl<AsyncSocketNotifier>
{
protected:
	SocketNotifyBaseList m_sockets;
public:
	AsyncSocketNotifier()
	{
	}

	~AsyncSocketNotifier()
	{
	}

	bool create_client(SocketNotifyBase* _socket_base,const CString& _sHostByName,u_short _port,int& _err,bool _close_mgs= true)
	{
		if(!_socket_base->m_socket.create(AF_INET,SOCK_STREAM,IPPROTO_TCP,_err))
			return false;

		string_converter<TCHAR,char> HostByNameA(_sHostByName,CP_ACP);
		struct hostent* host = gethostbyname(HostByNameA);

		// Setup our socket address structure
		SOCKADDR_IN SockAddr;
		SockAddr.sin_port=htons(_port);
		SockAddr.sin_family=AF_INET;
		SockAddr.sin_addr.s_addr=*((unsigned long*)host->h_addr);

		if(!_socket_base->m_socket.WASAsyncSelect(get_hwnd(),socket_message(),(FD_CONNECT|FD_READ|FD_WRITE|(_close_mgs?FD_CLOSE:0)),_err))
		{
			_socket_base->m_socket.close(_err);
			return false;
		}

		if(!_socket_base->m_socket.connect((SOCKADDR*)(&SockAddr),sizeof(SockAddr),_err))
		{
			if(EQL(_err,WSAEWOULDBLOCK))
			{
				_err = 0;
			}
			else
			{
				_socket_base->m_socket.close(_err);
				return false;
			}
		}
		return true;
	}

	bool create_server(SocketNotifyBase* _socket_base,u_short _port,int& _err,bool _close_mgs= true)
	{
		if(!_socket_base->m_socket.create(AF_INET,SOCK_STREAM,IPPROTO_TCP,_err))
			return false;

		SOCKADDR_IN SockAddr;
		SockAddr.sin_port=htons(_port);
		SockAddr.sin_family=AF_INET;
		SockAddr.sin_addr.s_addr=htonl(INADDR_ANY);

		if(!_socket_base->m_socket.bind((LPSOCKADDR)&SockAddr,sizeof(SockAddr),_err))
		{
			_socket_base->m_socket.close(_err);
			return false;
		}

		if(!_socket_base->m_socket.WASAsyncSelect(get_hwnd(),socket_message(),(FD_ACCEPT|FD_READ|FD_WRITE|(_close_mgs?FD_CLOSE:0)),_err))
		{
			_socket_base->m_socket.close(_err);
			return false;
		}

		if(!_socket_base->m_socket.listen(SOMAXCONN,_err))
		{
			_socket_base->m_socket.close(_err);
			return false;
		}
		return true;
	}

	void destroy()
	{
		stop();
	}

	bool add_reference(SocketNotifyBase* _psocket_data) 
	{
		return m_sockets.add(_psocket_data);
	}

	bool remove_reference(SocketNotifyBase* _psocket_data)
	{
		return m_sockets.remove(_psocket_data);
	}

	LRESULT WindowProc(HWND _hWnd,UINT _message,WPARAM _wParam,LPARAM _lParam)
	{
		if(EQL(_message,socket_message()))
			OnSocketMsgHandler(_hWnd,_message,_wParam,_lParam);
		if(EQL(_message,WM_CREATE))
			OnCreateMsgHandler(_hWnd,_message,_wParam,_lParam);
		if(EQL(_message,WM_DESTROY))
			return OnDestroyMsgHandler(_hWnd,_message,_wParam,_lParam);

		return ::DefWindowProc(_hWnd,_message,_wParam,_lParam);
	}

protected:
	void OnCreateMsgHandler(HWND _hWnd,UINT _message,WPARAM _wParam,LPARAM _lParam)
	{
	}

	LRESULT OnDestroyMsgHandler(HWND _hWnd,UINT _message,WPARAM _wParam,LPARAM _lParam)
	{
		return 0;
	}

	void OnSocketMsgHandler(HWND _hWnd,UINT _message,WPARAM _wParam,LPARAM _lParam)
	{
		if(WSAGETSELECTEVENT(_lParam)!=FD_CLOSE && WSAGETSELECTERROR(_lParam)!=0)
		{
			SocketNotifyBase* phandle = get_handler((SOCKET)_wParam);
			if(NOT_NULL(phandle))
				phandle->OnError(WSAGETSELECTERROR(_lParam),WSAGETSELECTEVENT(_lParam));
			return;
		}
		switch(WSAGETSELECTEVENT(_lParam))
		{
		case FD_CONNECT:
			connect_msg_handler((SOCKET)_wParam);
			break;
		case FD_ACCEPT:
			accept_msg_handler((SOCKET)_wParam);
			break;
		case FD_READ:
			read_msg_handler((SOCKET)_wParam);
			break;
		case FD_WRITE:
			write_msg_handler((SOCKET)_wParam);
			break;
		case FD_CLOSE:
			close_msg_handler((SOCKET)_wParam,WSAGETSELECTERROR(_lParam));
			break;
		}
	}

	void connect_msg_handler(SOCKET _socket)
	{
		SocketNotifyBase* phandle = get_handler(_socket);
		if(NOT_NULL(phandle))
			phandle->OnConnect();
	}

	void accept_msg_handler(SOCKET _socket)
	{
		SocketNotifyBase* phandle = get_handler(_socket);
		if(NOT_NULL(phandle))
			phandle->OnAccept();
	}

	void read_msg_handler(SOCKET _socket)
	{
		SocketNotifyBase* phandle = get_handler(_socket);
		if(NOT_NULL(phandle))
			phandle->OnReadBuffer();
	}

	void write_msg_handler(SOCKET _socket)
	{
		SocketNotifyBase* phandle = get_handler(_socket);
		if(NOT_NULL(phandle))
			phandle->OnWriteBuffer();
	}

	void close_msg_handler(SOCKET _socket,int _err)
	{
		SocketNotifyBase* phandle = get_handler(_socket);
		if(NOT_NULL(phandle))
			phandle->OnClose(_err);
	}

	SocketNotifyBase* get_handler(SOCKET _socket)
	{
		return m_sockets.find(_socket);
	}

	static unsigned int socket_message()
	{
		return WM_USER + 0x1;
	}

};//struct AsyncSocketNotifier

struct AsyncSocketNotifierManager
{
protected:
	AsyncSocketNotifier* notifier;

	AsyncSocketNotifierManager()
	{
		notifier = trace_alloc(new AsyncSocketNotifier());
	}

	~AsyncSocketNotifierManager()
	{
		destroy();
	}
public:
	void destroy()
	{
		if(NOT_NULL(notifier))
		{
			notifier->destroy();
			notifier= NULL;
		}
	}

	static AsyncSocketNotifier& get()
	{
		static AsyncSocketNotifierManager _;
		return *(_.notifier);
	}
};

struct AsyncSocket : public SocketNotifyBase
{
protected:
	sockaddr sockAddrClient;
	Queue<byte> m_data2send;
	Queue<byte> m_readed_data;
	CCriticalSection m_critset;
public:

	AsyncSocket()
	{
		AsyncSocketNotifierManager::get().add_reference(this);
		set_queue_size(1024*16,1024*16,1024*16);
	}

	~AsyncSocket()
	{
		int err = ERROR_SUCCESS;
		close(err);
		AsyncSocketNotifierManager::get().remove_reference(this);
	}

	AsyncSocket(const AsyncSocket& _obj)
		:SocketNotifyBase(_obj)
		,sockAddrClient(_obj.sockAddrClient)
		,m_data2send(_obj.m_data2send)
		,m_readed_data(_obj.m_readed_data)
	{
		AsyncSocketNotifierManager::get().add_reference(this);
	}

	AsyncSocket& operator = (const AsyncSocket& _obj)
	{
		m_socket = _obj.m_socket;
		sockAddrClient = _obj.sockAddrClient;
		m_data2send = _obj.m_data2send;
		m_readed_data = _obj.m_readed_data;
	}

	operator bool () const 
	{
		return m_socket.is_valid();
	}

	bool create_client(const CString& _sHostByName,u_short _port,int& _err,bool _close_mgs= true)
	{
		return AsyncSocketNotifierManager::get().create_client(this,_sHostByName,_port,_err,_close_mgs);
	}

	bool create_server(u_short _port,int& _err,bool _close_mgs = true)
	{
		return AsyncSocketNotifierManager::get().create_server(this,_port,_err,_close_mgs);
	}

	void set_queue_size(
		size_t _send_data_buffer_size
		,size_t _readed_data_buffer_size
		,size_t _grow_size = 0
		)
	{
		CAutoLock __al(m_critset);
		m_data2send.resize(_send_data_buffer_size,_grow_size);
		m_readed_data.resize(_readed_data_buffer_size,_grow_size);
	}

	void send(const std::vector<byte>& _data)
	{
		bool last_state_have_data = have_data_2_send();
		push_data_2_send(_data);
		if(!last_state_have_data) 
		{
			bool bDoOnWriteCall = false;
			send_buffer(bDoOnWriteCall);
			if(bDoOnWriteCall)
				OnWrite();
		}
	}

	bool accept(AsyncSocket& _from,int& _err)
	{
		int size = sizeof(sockAddrClient);
		m_data2send.set_size(_from.m_data2send);
		m_readed_data.set_size(_from.m_readed_data);
		return m_socket.accept(_from.m_socket,&sockAddrClient,&size,_err); 
	}

	bool shutdown(int _how,int& _err)
	{
		return m_socket.shutdown(_how,_err);
	}

	bool close(int& _err)
	{
		bool bret = true;
		if(NOT_NULL((SOCKET)m_socket))
		{
			bret = m_socket.close(_err);
		}
		return bret;
	}


	virtual void OnConnect()
	{
	}

	virtual void OnAccept()
	{
	}

	virtual void OnRead()
	{
	}

	virtual void OnWrite()
	{
	}

	virtual void OnReadBuffer()
	{
		if(read_buffer())
			OnRead();
	}

	virtual void OnWriteBuffer()
	{
		bool bDoOnWriteCall = false;
		if(have_data_2_send())
			send_buffer(bDoOnWriteCall);
		else
			bDoOnWriteCall = true;
		if(bDoOnWriteCall) 
			OnWrite();
	}

	virtual void OnClose(int _err)
	{
	}

	virtual void OnError(int _err,int _msg = 0)
	{
	}

protected:
	void send_buffer(bool& _bDoOnWriteCall)
	{
		for(;have_data_2_send();)
		{
			const size_t buf_full_sz = 1024;
			byte buf[buf_full_sz];
			size_t buf_sz = get_data_2_send(buf,buf_full_sz);
			if(buf_sz==0) break;
			int err = 0;
			int sended = m_socket.send(buf,(int)buf_sz,0,err);
			if(Socket::is_error(sended))
			{
				if(err==WSAEWOULDBLOCK) 
					return;
				OnError(err);
				return;
			}
#if defined(TRACE_SOCKETS_OPERATIONS)
			trace_sended(buf,sended);
#endif
			pop_data_2_send(sended);
		}
		_bDoOnWriteCall = true;
	}

	void trace_sended(const byte* _buf,size_t _buf_sz)
	{
		CTraceFile tf;
		CString file_name,path;
		file_name.Format(_T("sndd_%X.log"),(SOCKET)m_socket);
		FileUtils::GetModulePath(NULL,path);
		FileUtils::SetLastSlash(path);
		tf.open(path+file_name);
		tf.write(_buf,_buf_sz);
		tf.flush();
	}

	bool read_buffer()
	{
		u_long amount = 0;
		int err = 0;
		if(!m_socket.ioctrlsocket(FIONREAD,&amount,err))
		{
			OnError(err);
			return false;
		}
		if(amount==0) return true;
		std::vector<byte> buf;
		buf.resize(amount);
		int received = m_socket.recv(&*buf.begin(),amount,0,err);
#if defined(TRACE_SOCKETS_OPERATIONS)
		trace_received(buf,received);
#endif
		if(Socket::is_error(received))
		{
			OnError(err);
			return false;
		}
		push_received_data(buf,received);
		return true;
	}

	void trace_received(const std::vector<byte>& _buf,int _buf_sz)
	{
		CTraceFile tf;
		CString file_name,path;
		file_name.Format(_T("rcvd_%X.log"),(SOCKET)m_socket);
		FileUtils::GetModulePath(NULL,path);
		FileUtils::SetLastSlash(path);
		tf.open(path+file_name);
		tf.write(_buf,_buf_sz);
		tf.flush();
	}

	bool have_data_2_send() const
	{
		CAutoLock __al(m_critset);
		return !m_data2send.empty();
	}

	void push_data_2_send(const std::vector<byte>& _data)
	{
		CAutoLock __al(m_critset);
		m_data2send.push(_data.begin(),_data.end());
	}

	size_t get_data_2_send(byte* _buf,size_t _buf_size)
	{
		CAutoLock __al(m_critset);
		return m_data2send.copy_to(_buf,_buf+_buf_size);
	}

	void pop_data_2_send(int _sended)
	{
		CAutoLock __al(m_critset);
		m_data2send.pop(_sended);
	}

	void push_received_data(const std::vector<byte>& _buf,int _received)
	{
		CAutoLock __al(m_critset);
		m_readed_data.push(_buf.begin(),_buf.begin()+_received);
	}

	size_t pop_received_data(std::vector<byte>& _buf)
	{
		CAutoLock __al(m_critset);
		return m_readed_data.move_to(_buf);
	}

	void clear_received_data()
	{
		CAutoLock __al(m_critset);
		m_readed_data.clear();
	}

	size_t pop_received_data(std::vector<byte>::iterator _it,std::vector<byte>::iterator _ite)
	{
		CAutoLock __al(m_critset);
		return m_readed_data.move_to(_it,_ite);
	}

};//struct AsyncSocket

struct SyncSocket : public AsyncSocket
{	
protected:
	CEvent sended_evnt;
	CEvent readed_event;
	CEvent connected_evnt;
	CEvent error_evnt;
	int error_code;

	CCriticalSection m_critsect;

	std::vector<byte>* pbuf;
	size_t readed_size;
	long sended_blocks;
public:

	SyncSocket()
		:sended_evnt(FALSE,TRUE)
		,readed_event(FALSE,FALSE)
		,connected_evnt(FALSE,TRUE)
		,error_evnt(FALSE,TRUE)
		,error_code(0)
		,pbuf(NULL)
		,readed_size(0)
		,sended_blocks(0)
	{
	}

	SyncSocket(const SyncSocket& _obj)
		:AsyncSocket(_obj)
		,error_code(_obj.error_code)
		,pbuf(_obj.pbuf)
		,readed_size(_obj.readed_size)
	{
	}

	SyncSocket& operator = (const SyncSocket& _obj)
	{
		AsyncSocket::operator = (_obj);
		error_code = _obj.error_code;
		pbuf = _obj.pbuf;
		readed_size = _obj.readed_size;
		return *this;
	}

	bool async_send(const std::vector<byte>& _data,int& _err)
	{
		if(is_error(_err)) 
			return false;

#if defined(USE_MFC)
		sended_evnt.ResetEvent();
#else
		sended_evnt.Reset();
#endif
		send(_data);
		return true;
	}

	bool wait_send(HANDLE _hstopevent,DWORD _millisec,int& _err)
	{
		return wait_event_or_error(sended_evnt,_hstopevent,_millisec,_err);
	}

	bool wait_connected(DWORD _millisec,int& _err)
	{
		return wait_event_or_error(connected_evnt,_millisec,_err);
	}

	bool async_read(std::vector<byte>* _pbuf,int& _err)
	{
		if(is_error(_err)) return false;
		set_buf(_pbuf,_err);
		pop_data();
		return true;
	}

	bool wait_read(HANDLE _hstopevent,DWORD _millisec,int& _err)
	{
		return wait_event_or_error(readed_event,_hstopevent,_millisec,_err);
	}

	virtual void OnConnect()
	{
#if defined(USE_MFC)
		connected_evnt.SetEvent();
#else
		connected_evnt.Set();
#endif
	}

	virtual void OnAccept()
	{
	}

	virtual void OnRead()
	{
		pop_data();
	}

	virtual void OnWrite()
	{
#if defined(USE_MFC)
		sended_evnt.SetEvent();
#else
		sended_evnt.Set();
#endif
	}

	virtual void OnClose(int _err)
	{
#if defined(USE_MFC)
		sended_evnt.SetEvent();
		readed_event.SetEvent();
#else
		sended_evnt.Set();
		readed_event.Set();
#endif
	}

	virtual void OnError(int _err,int _msg = 0)
	{
		CAutoLock __al(m_critsect);
		error_code = _err;
#if defined(USE_MFC)
		error_evnt.SetEvent();
#else
		error_evnt.Set();
#endif
	}

protected:
	bool is_error(int& _err)
	{
		CAutoLock __al(m_critsect);
		if(!Socket::is_error(error_code)) return false;
		_err = error_code;
		return true;
	}

	bool set_buf(std::vector<byte>* _pbuf,int& _err)
	{
		CAutoLock __al(m_critsect);
		pbuf = NULL;
		VERIFY_EXIT1(!_pbuf->empty(),(_err=ERROR_BAD_ARGUMENTS,false));
		pbuf = _pbuf;
		readed_size = 0;
		return true;
	}

	void pop_data()
	{
		CAutoLock __al(m_critsect);
		if(IS_NULL(pbuf)) return;
		readed_size += pop_received_data(pbuf->begin()+readed_size,pbuf->end());
		if(readed_size==pbuf->size()) 
		{
			pbuf = NULL;
#if defined(USE_MFC)
			readed_event.SetEvent();
#else
			readed_event.Set();
#endif
		}
	}

	bool wait_event_or_error(HANDLE _hok,DWORD _millisec,int& _err)
	{
		switch(::wait4event(_hok,error_evnt,_millisec))
		{
		case 0: return true;
		case 1:
			{
				CAutoLock __al(m_critsect);
				_err = error_code;
				return false;
			}
		default:
			_err = ERROR_INVALID_HANDLE;
			return false;
		}
	}

	bool wait_event_or_error(HANDLE _hok,HANDLE _hstopevent,DWORD _millisec,int& _err)
	{
		if(IS_NULL(_hstopevent))
			return wait_event_or_error(_hok,_millisec,_err);

		switch(::wait4event(_hok,_hstopevent,error_evnt,_millisec))
		{
		case 0: return true;
		case 1:
			_err = 0;
			return false;
		case 2:
			{
				CAutoLock __al(m_critsect);
				_err = error_code;
				return false;
			}
		default:
			_err = ERROR_INVALID_HANDLE;
			return false;
		}
	}
};//struct SyncSocket

/*
struct TestServer;

struct TestServerMessageReader : public AsyncSocket
{
	TestServer* test_server;

	TestServerMessageReader()
		:test_server(NULL)
	{
	}

	virtual void OnRead()
	{
		std::cout << "TestServerMessageReader::OnRead(" << (UINT_PTR)(SOCKET)m_socket << ")\n";
		std::vector<byte> buf;
		pop_received_data(buf);
		std::vector<byte>::const_iterator
			it = buf.begin()
			,ite = buf.end()
			;
		for(;it!=ite;++it)
		{
			std::cout << *it;
		}
		std::cout << "\n";
	}

	virtual void OnError(int _err,int _msg = 0)
	{
		std::cout << "TestServerMessageReader::OnError(" << (UINT_PTR)(SOCKET)m_socket << ") error code : " << _err << "\n";
	}

	virtual void OnClose(int _err);
};//struct TestServerMessageReader

struct TestServer : public AsyncSocket
{
	CEvent close_reader;
	long readers; 

	TestServer()
		:readers(0)
		,close_reader(FALSE,FALSE)
	{
	}

	virtual void OnAccept()
	{
		std::cout << "TestServer::OnAccept(" << (UINT_PTR)(SOCKET)m_socket << ")\n";
		TestServerMessageReader* message_reader = trace_alloc(new TestServerMessageReader());
		message_reader->test_server = this;
		readers++;
		int err = 0;
		if(!message_reader->accept(*this,err))
		{
			OnError(err);
			readers--;
			delete trace_free(message_reader);
			message_reader = NULL;
		}
		close_reader.ResetEvent();
	}

	virtual void OnError(int _err,int _msg = 0)
	{
		std::cout << "TestServer::OnError(" << (UINT_PTR)(SOCKET)m_socket << ") error code : " << _err << "\n";
	}

	void OnCloseReader()
	{
		readers--;
		if(readers==0) close_reader.SetEvent();
	}
};//struct TestServer

void TestServerMessageReader::OnClose(int _err)
{
	if(NOT_NULL(test_server))
		test_server->OnCloseReader();
	delete trace_free(this);
}


struct TestClient : public AsyncSocket
{
	typedef AsyncSocket base;

	CEvent sended_evnt;
	CEvent connected_evnt;
	CEvent error_evnt;

	TestClient()
		:sended_evnt(FALSE,TRUE)
		,connected_evnt(FALSE,TRUE)
		,error_evnt(FALSE,TRUE)
	{
	}

	void send(const std::vector<byte>& _data)
	{
		sended_evnt.ResetEvent();
		base::send(_data);
	}

	virtual void OnWrite()
	{
		sended_evnt.SetEvent();
	}

	virtual void OnConnect()
	{
		connected_evnt.SetEvent();
	}

	virtual void OnError(int _err,int _msg = 0)
	{
		connected_evnt.SetEvent();
		error_evnt.SetEvent();
		std::cout << "TestClient::OnError(" << (UINT_PTR)(SOCKET)m_socket << ") err=" << _err << "\n";
	}
};//struct TestClient
*/