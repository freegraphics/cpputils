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


#endif //#if defined(__TRACE_H__AB700A4A_3025_49E4_BBD5_279BCD110074__INCLUDED)