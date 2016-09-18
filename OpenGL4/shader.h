#pragma once

#include "utils.h"
#include "simple.h"
#include "types.h"

struct GLShader
{
protected:
	GLuint shader;

public:
	GLShader()
		:shader(0)
	{
	}

	~GLShader()
	{
		clear();
	}

	GLuint operator() () const 
	{
		return shader;
	}

	void create(GLenum _type)
	{
		clear();
		shader = glCreateShader(_type);
	}

	void move_from(GLShader& _shader)
	{
		shader = _shader.shader;
		_shader.shader = 0;
	}

	void set_source(const CString& _shader_source)
	{
		string_converter<TCHAR,GLchar> shader_source(_shader_source,CP_ACP);
		glShaderSource(shader,1,&(&shader_source),0);
	}

	void compile(GLint& _status,CString& _log)
	{
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &_status);
		if(!_status)
		{
			GLsizei len = 0;
			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );
			std::auto_ptr<GLchar> log(new GLchar[len+1]);
			glGetShaderInfoLog( shader, len, &len, log.get()); 
			string_converter<GLchar,TCHAR> logT(log.get(),len,CP_ACP);
			_log = (TCHAR*)logT;
		}
	}

	void clear()
	{
		if(glIsShader(shader))
		{
			glDeleteShader(shader);
		}
		shader = 0;
	}
};//struct GLShader

