#if !defined(__TRACE_H__AB700A4A_3025_49E4_BBD5_279BCD110074__INCLUDED)
#define __TRACE_H__AB700A4A_3025_49E4_BBD5_279BCD110074__INCLUDED 

#pragma once 

#include "config.h"
#include "atlmfc.h"
#include "convert.h"

#define OUT_VALUE(_out_stream,_val) \
	_out_stream << _T(#_val) << _T(" : ") << _val << _T("\n");

#define OUT_VALUE_STR(_out_stream,_val) \
	_out_stream << _T(#_val) << _T(" : ") << (LPCTSTR)_val << _T("\n");

#define OUT_VALUE_CONV(_out_stream,_val) \
	do{	\
		CString out_val_str;	\
		common::convert(_val,out_val_str);	\
		_out_stream << _T(#_val) << _T(" : ") << (LPCTSTR)out_val_str << _T("\n");	\
	}while(false);	

interface ITraceOutput
{
	virtual void message(const CString& _str) = 0;
	virtual void process(const CString& _str,real _rate,real _max_rate = (real)1.0) = 0;
	virtual void process(const CString& _str,size_t _idx,size_t _max_idx) = 0;
	virtual void process(const CString& _str,size_t _cycle) = 0;
	virtual void trace(const CString& _file_name,const CString& _str) = 0;
};

struct TraceToConsole : public ITraceOutput
{
protected:
	DWORD time_last;

	void get_current_time()
	{
		time_last = GetCurrentTime();
	}

	void update_last_time(DWORD _t)
	{
		time_last = _t - ((_t-time_last)%1000);
	}
public:
	TraceToConsole()
	{
		get_current_time();
	}

	virtual void message(const CString& _str)
	{
		std::tcout 
			<< _T("\r\t\t\t\t\t\t\t\t\t\r")
			<< _str << _T("\n")
			;
		get_current_time();
	}

	virtual void process(const CString& _str,real _rate,real _max_rate = (real)1.0)
	{
		DWORD t = GetCurrentTime();
		if(t-time_last<1000) return;
		std::tcout << _T("\r\t\t\t\t\t\t\t\t\t\r")
			<< (LPCTSTR)Format(_str,_rate,_max_rate) 
			<< _T("\r")
			;
		update_last_time(t);
	}

	virtual void process(const CString& _str,size_t _idx,size_t _max_idx)
	{
		DWORD t = GetCurrentTime();
		if(t-time_last<1000) return;
		std::tcout  << _T("\r\t\t\t\t\t\t\t\t\t\r")
			<< (LPCTSTR)Format(_str,_idx,_max_idx) 
			<< _T("\r")
			;
		update_last_time(t);
	}

	virtual void process(const CString& _str,size_t _cycle)
	{
		DWORD t = GetCurrentTime();
		if(t-time_last<1000) return;
		std::tcout  << _T("\r\t\t\t\t\t\t\t\t\t\r")
			<< (LPCTSTR)Format(_str,_cycle) 
			<< _T("\r")
			;
		update_last_time(t);
	}

	virtual void trace(const CString& _file_name,const CString& _str)
	{
		CTraceFile trace;
		trace.open(_file_name);
		trace << _str << _T("\n");
	}
};

#endif //#if defined(__TRACE_H__AB700A4A_3025_49E4_BBD5_279BCD110074__INCLUDED)