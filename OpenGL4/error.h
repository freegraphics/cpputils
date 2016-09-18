#pragma once

#include "utils.h"
#include <utils/errors.h>

struct GLError : public utils::CUtilitiesExceptionBase
{
protected:
	GLenum error_code;

public:
	GLError(GLenum _error_code,LPCTSTR _szSourceInfo)
		:utils::CUtilitiesExceptionBase(_szSourceInfo)
		,error_code(_error_code)
	{
		utils::CErrorTag et;
		et << cnamedvalue(_T("gl_error_code"),_error_code);
		initialize_exception();
	}

	GLenum get_code() const 
	{
		return error_code;
	}

	void Format(CString& _error_message,bool _full_info = false)
	{
		if(_full_info)
			_error_message = FormatExceptionFull(this);
		else
			_error_message = FormatException(this);
	}
};//struct GLError

inline
void throw_gl_error(GLenum _error_code,LPCTSTR _szSourceInfo)
{
#if defined(USE_EXCEPTION_REFS)
	throw GLError(_error_code,_szSourceInfo);
#else
	throw trace_alloc(new GLError(_error_code,_szSourceInfo));
#endif 
}