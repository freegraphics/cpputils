#pragma once

#include "utils.h"
#include "simple.h"
#include "types.h"
#include "shader.h"
#include "location.h"

struct UniformBlockIndex
{
protected:
	GLuint m_index;
	GLint m_size;
	GLuint m_program;

public:
	UniformBlockIndex(GLuint _index = 0,GLint _size = 0,GLuint _program = 0)
		:m_index(_index)
		,m_size(_size)
		,m_program(_program)
	{
	}

	GLuint index() const 
	{
		return m_index;
	}

	GLint size() const 
	{
		return m_size;
	}

	GLuint program() const 
	{
		return m_program;
	}

	static UniformBlockIndex get_error_index()
	{
		return UniformBlockIndex(-1,0,-1);
	}

	bool is_error_index() const 
	{
		UniformBlockIndex ei = get_error_index();
		return EQL(m_index,ei.m_index)
			&& EQL(m_program,ei.m_program)
			;
	}
};//struct UniformBlockIndex

struct ShaderInfo
{
	GLenum type;
	CString file_name;
	CString shader_source;

	ShaderInfo(
		GLenum _type = GL_NONE
		,const CString& _file_name = CString()
		,const CString& _shader_txt = CString()
		)
		:type(_type)
		,file_name(_file_name)
		,shader_source(_shader_txt)
	{
	}
};//struct ShaderInfo

struct SubroutineInfo
{
	CString uniform_var;
	CString func_name;

	SubroutineInfo(
		const CString& _uniform_var = CString()
		,const CString& _func_name = CString()
		)
		:uniform_var(_uniform_var)
		,func_name(_func_name)
	{
	}
};//struct SubroutineInfo

struct GLProgram
{
protected:
	GLuint program;
	std::vector<GLShader> shaders;
	GLenum m_err_code;

public:
	GLProgram()
		:program(0)
		,m_err_code(GL_NO_ERROR)
	{
	}

	~GLProgram()
	{
		clear();
	}

	void clear()
	{
		shaders.clear();
		if(glIsProgram(program))
		{
			glDeleteProgram(program);
		}
		program = 0;
	}

	bool create()
	{
		clear();
		program = glCreateProgram();
		return !is_error(m_err_code);
	}

	template<typename _ShaderInfoIterator>
	bool load_shaders(_ShaderInfoIterator _it,_ShaderInfoIterator _ite,CString& _err_log)
	{
		prepare();

		for(;_it!=_ite;++_it)
		{
			const ShaderInfo& si = *_it;

			GLShader shader;
			shader.create(si.type);

			CString shader_source = si.shader_source;
			if(shader_source.IsEmpty())
				FileUtils::load_file(si.file_name,shader_source);

			shader.set_source(shader_source);

			GLint compile_status = 0;
			CString err_log;
			shader.compile(compile_status,err_log);

			if(!compile_status) 
			{
				_err_log += err_log;
				shader.clear();
				return false;
			}

			shaders.push_back(GLShader());
			attach_shader(shader);
			shaders.back().move_from(shader);
		}

		GLint linked_status = 0;
		CString err_log;
		link(linked_status,err_log);

		if(!linked_status) 
		{
			_err_log += err_log;
			shaders.clear();
			return false;
		}
		return true;
	}

	bool use()
	{
		glUseProgram(program);
		return !is_error(m_err_code);
	}

	UniformLocation get_uniform_location(const CString& _var_name) 
	{
		string_converter<TCHAR,GLchar> var_name(_var_name,CP_ACP);
		UniformLocation ul(glGetUniformLocation(program,(const GLchar*)var_name),program);
		if(is_error(m_err_code))
			ul = UniformLocation::get_error_location();
		return ul;
	}

	AttributeLocation get_attribute_location(const CString& _var_name) 
	{
		string_converter<TCHAR,GLchar> var_name(_var_name,CP_ACP);
		AttributeLocation al(glGetAttribLocation(program,(const GLchar*)var_name));
		if(is_error(m_err_code))
			al = AttributeLocation::get_error_location();
		return al;
	}

	UniformBlockIndex get_uniform_block_index(const CString& _block_name)
	{
		string_converter<TCHAR,GLchar> block_name(_block_name,CP_ACP);
		GLuint idx = glGetUniformBlockIndex(program,(const GLchar*)block_name);
		if(is_error(m_err_code))
			return UniformBlockIndex::get_error_index();
		GLint sz = 0;
		glGetActiveUniformBlockiv(program,idx,GL_UNIFORM_BLOCK_DATA_SIZE,&sz);
		UniformBlockIndex ubi(idx,sz,program);
		if(is_error(m_err_code))
			ubi = UniformBlockIndex::get_error_index();
		return ubi;
	}

	template<typename _Iterator>
	bool select_subroutines(
		IN GLenum _shader_type
		,IN _Iterator _it,IN _Iterator _ite
		)
	{
		GLint uniform_locations = 0;
		glGetProgramStageiv(program,_shader_type,GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,&uniform_locations);
		std::vector<GLuint> indices;
		if(uniform_locations<=0) 
		{
			if(!is_error(m_err_code)) m_err_code = GL_INVALID_OPERATION;
			return false;
		}
		indices.resize(uniform_locations,0);
		for(;_it!=_ite;++_it)
		{
			const SubroutineInfo& si = *_it;
			string_converter<TCHAR,GLchar> subroutine_var(si.uniform_var,CP_ACP);
			GLint location = glGetSubroutineUniformLocation(program,_shader_type,(const GLchar*)subroutine_var);
			if(location<0) 
			{
				if(!is_error(m_err_code)) m_err_code = GL_INVALID_VALUE;
				return false;
			}
			string_converter<TCHAR,GLchar> subroutine_func(si.func_name,CP_ACP);
			GLuint index = glGetSubroutineIndex(program,_shader_type,(const GLchar*)subroutine_func);
			if(EQL(index,GL_INVALID_INDEX)) 
			{
				if(!is_error(m_err_code)) m_err_code = GL_INVALID_VALUE;
				return false;
			}
			indices[location] = index;
		}
		glUniformSubroutinesuiv(_shader_type, uniform_locations, &indices[0]);
		return !is_error(m_err_code);

		// need to call use() to start using set soubroutines
		// page 77 of the "Red Book"
	}


/*	
	GLint uniform_block_binding(GLuint _uniform_block_idx,GLuint _uniforma_block_binding)
	{
		return glUniformBlockBinding(program,_uniform_block_idx,_uniforma_block_binding);
	}
*/

protected:
	bool prepare()
	{
		if(!glIsProgram(program))
		{
			return create();
		}
		return true;
	}	

	void attach_shader(const GLShader& _shader)
	{
		glAttachShader(program,_shader());
	}

	void link(GLint& _status,CString& _err_log)
	{
		glLinkProgram(program);
		glGetProgramiv( program, GL_LINK_STATUS, &_status);
		if(!_status)
		{
			GLsizei len = 0;
			glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );

			std::auto_ptr<GLchar> log(new GLchar[len+1]);
			glGetProgramInfoLog( program, len, &len, log.get() ); 
			string_converter<GLchar,TCHAR> logT(log.get(),len,CP_ACP);
			_err_log = (TCHAR*)logT;
		}
	}
};//struct GLProgram
