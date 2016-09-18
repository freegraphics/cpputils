#pragma once 

#include "utils.h"
#include "types.h"
#include "simple.h"

namespace Private
{
	struct Location
	{
	protected:
		GLint location;

	public:
		Location(GLint _location)
			:location(_location)
		{
		}

		GLint operator() () const 
		{
			return location;
		}
	};
};//namespace Private

struct AttributeLocation : public Private::Location
{
public:
	AttributeLocation(GLint _location = 0)
		:Private::Location(_location)
	{
	}

	template<typename _Type>
	bool set(const _Type _data)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			|| GLType<_Type>::get==GL_SHORT
			,illegal_input_type
			);
		if(is_error_location()) return false;
		switch(GLType<_Type>::get)
		{
		case GL_FLOAT: 
			glVertexAttrib1fv(location,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_data)));
			break;
		case GL_DOUBLE: 
			glVertexAttrib1dv(location,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_data)));
			break;
		case GL_SHORT: 
			glVertexAttrib1sv(location,detail::to_type<GL_SHORT>::convert(Private::to_pointer(_data)));
			break;
		default:
			return false;
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type>
	bool set_integer(const _Type _data)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		switch(GLType<_Type>::get)
		{
		case GL_INT: 
			glVertexAttribI1iv(location,detail::to_type<GL_INT>::convert(Private::to_pointer(_data)));
			break;
		case GL_UNSIGNED_INT: 
			glVertexAttribI1uiv(location,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_data)));
			break;
		default:
			return false;
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tvec1<_Type,_Precision>& _data)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			|| GLType<_Type>::get==GL_SHORT
			,illegal_input_type
			);
		if(is_error_location()) return false;
		switch(GLType<_Type>::get)
		{
		case GL_FLOAT: 
			glVertexAttrib1fv(location,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_data)));
			break;
		case GL_DOUBLE: 
			glVertexAttrib1dv(location,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_data)));
			break;
		case GL_SHORT: 
			glVertexAttrib1sv(location,detail::to_type<GL_SHORT>::convert(Private::to_pointer(_data)));
			break;
		default:
			return false;
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set_integer(const glm::detail::tvec1<_Type,_Precision>& _data)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		switch(GLType<_Type>::get)
		{
		case GL_INT: 
			glVertexAttribI1iv(location,detail::to_type<GL_INT>::convert(Private::to_pointer(_data)));
			break;
		case GL_UNSIGNED_INT: 
			glVertexAttribI1uiv(location,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_data)));
			break;
		default:
			return false;
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tvec2<_Type,_Precision>& _data)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			|| GLType<_Type>::get==GL_SHORT
			,illegal_input_type
			);
		if(is_error_location()) return false;
		switch(GLType<_Type>::get)
		{
		case GL_FLOAT: 
			glVertexAttrib2fv(location,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_data)));
			break;
		case GL_DOUBLE: 
			glVertexAttrib2dv(location,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_data)));
			break;
		case GL_SHORT: 
			glVertexAttrib2sv(location,detail::to_type<GL_SHORT>::convert(Private::to_pointer(_data)));
			break;
		default:
			return false;
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set_integer(const glm::detail::tvec2<_Type,_Precision>& _data)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		switch(GLType<_Type>::get)
		{
		case GL_INT: 
			glVertexAttribI2iv(location,detail::to_type<GL_INT>::convert(Private::to_pointer(_data)));
			break;
		case GL_UNSIGNED_INT: 
			glVertexAttribI2uiv(location,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_data)));
			break;
		default:
			return false;
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tvec3<_Type,_Precision>& _data)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			|| GLType<_Type>::get==GL_SHORT
			,illegal_input_type
			);
		if(is_error_location()) return false;
		switch(GLType<_Type>::get)
		{
		case GL_FLOAT: 
			glVertexAttrib3fv(location,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_data)));
			break;
		case GL_DOUBLE: 
			glVertexAttrib3dv(location,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_data)));
			break;
		case GL_SHORT: 
			glVertexAttrib3sv(location,detail::to_type<GL_SHORT>::convert(Private::to_pointer(_data)));
			break;
		default:
			return false;
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set_integer(const glm::detail::tvec3<_Type,_Precision>& _data)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		switch(GLType<_Type>::get)
		{
		case GL_INT: 
			glVertexAttribI3iv(location,detail::to_type<GL_INT>::convert(Private::to_pointer(_data)));
			break;
		case GL_UNSIGNED_INT: 
			glVertexAttribI3uiv(location,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_data)));
			break;
		default:
			return false;
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tvec4<_Type,_Precision>& _data,bool _normilize = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_BYTE 
			|| GLType<_Type>::get==GL_SHORT 
			|| GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_BYTE 
			|| GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			|| GLType<_Type>::get==GL_UNSIGNED_SHORT
			|| GLType<_Type>::get==GL_UNSIGNED_INT
			,illegal_input_type
			);
		if(is_error_location()) return false;
		switch(GLType<_Type>::get)
		{
		case GL_BYTE:
			if(_normilize)
				glVertexAttrib4Nbv(location,detail::to_type<GL_BYTE>::convert(Private::to_pointer(_data)));
			else
				glVertexAttrib4bv(location,detail::to_type<GL_BYTE>::convert(Private::to_pointer(_data)));
			break;
		case GL_SHORT: 
			if(_normilize)
				glVertexAttrib4Nsv(location,detail::to_type<GL_SHORT>::convert(Private::to_pointer(_data)));
			else
				glVertexAttrib4sv(location,detail::to_type<GL_SHORT>::convert(Private::to_pointer(_data)));
			break;
		case GL_INT:
			if(_normilize)
				glVertexAttrib4Niv(location,detail::to_type<GL_INT>::convert(Private::to_pointer(_data)));
			else
				glVertexAttrib4iv(location,detail::to_type<GL_INT>::convert(Private::to_pointer(_data)));
			break;
		case GL_FLOAT: 
			glVertexAttrib4fv(location,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_data)));
			break;
		case GL_DOUBLE: 
			glVertexAttrib4dv(location,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_data)));
			break;
		case GL_UNSIGNED_BYTE:
			if(_normilize)
				glVertexAttrib4Nubv(location,detail::to_type<GL_UNSIGNED_BYTE>::convert(Private::to_pointer(_data)));
			else
				glVertexAttrib4ubv(location,detail::to_type<GL_UNSIGNED_BYTE>::convert(Private::to_pointer(_data)));
			break;
		case GL_UNSIGNED_SHORT: 
			if(_normilize)
				glVertexAttrib4Nusv(location,detail::to_type<GL_UNSIGNED_SHORT>::convert(Private::to_pointer(_data)));
			else
				glVertexAttrib4usv(location,detail::to_type<GL_UNSIGNED_SHORT>::convert(Private::to_pointer(_data)));
			break;
		case GL_UNSIGNED_INT:
			if(_normilize)
				glVertexAttrib4Nuiv(location,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_data)));
			else
				glVertexAttrib4uiv(location,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_data)));
			break;
		default:
			return false;
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set_integer(const glm::detail::tvec4<_Type,_Precision>& _data)
	{
		GL_STATIC_ASSERT(
			|| GLType<_Type>::get==GL_BYTE 
			|| GLType<_Type>::get==GL_SHORT 
			|| GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_BYTE 
			|| GLType<_Type>::get==GL_UNSIGNED_SHORT
			|| GLType<_Type>::get==GL_UNSIGNED_INT
			,illegal_input_type
			);
		if(is_error_location()) return false;
		switch(GLType<_Type>::get)
		{
		case GL_BYTE:
			glVertexAttribI4bv(location,detail::to_type<GL_BYTE>::convert(Private::to_pointer(_data)));
			break;
		case GL_SHORT: 
			glVertexAttribI4sv(location,detail::to_type<GL_SHORT>::convert(Private::to_pointer(_data)));
			break;
		case GL_INT:
			glVertexAttribI4iv(location,detail::to_type<GL_INT>::convert(Private::to_pointer(_data)));
			break;
		case GL_UNSIGNED_BYTE:
			glVertexAttribI4ubv(location,detail::to_type<GL_UNSIGNED_BYTE>::convert(Private::to_pointer(_data)));
			break;
		case GL_UNSIGNED_SHORT: 
			glVertexAttribI4usv(location,detail::to_type<GL_UNSIGNED_SHORT>::convert(Private::to_pointer(_data)));
			break;
		case GL_UNSIGNED_INT:
			glVertexAttribI4uiv(location,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_data)));
			break;
		default:
			return false;
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	static AttributeLocation get_error_location()
	{
		return AttributeLocation(-1);
	}

	bool is_error_location() const 
	{
		AttributeLocation el = get_error_location();
		return EQL(location,el.location);
	}

	void disable_istancing()
	{
		glVertexAttribDivisor(location,0);
	}

	template<typename _Type>
	void set_divisor(GLuint _divisor,_Type)
	{
		glVertexAttribDivisor(location,_divisor);
	}

	template<typename T,glm::precision P>
	void set_divisor(GLuint _divisor,glm::detail::tmat2x2<T,P>)
	{
		GLint i = 0;
		for(i=0;i<2;i++)
		{
			glVertexAttribDivisor(location+i,_divisor);
		}
	}

	template<typename T,glm::precision P>
	void set_divisor(GLuint _divisor,glm::detail::tmat2x3<T,P>)
	{
		GLint i = 0;
		for(i=0;i<2;i++)
		{
			glVertexAttribDivisor(location+i,_divisor);
		}
	}

	template<typename T,glm::precision P>
	void set_divisor(GLuint _divisor,glm::detail::tmat2x4<T,P>)
	{
		GLint i = 0;
		for(i=0;i<2;i++)
		{
			glVertexAttribDivisor(location+i,_divisor);
		}
	}

	template<typename T,glm::precision P>
	void set_divisor(GLuint _divisor,glm::detail::tmat3x2<T,P>)
	{
		GLint i = 0;
		for(i=0;i<3;i++)
		{
			glVertexAttribDivisor(location+i,_divisor);
		}
	}

	template<typename T,glm::precision P>
	void set_divisor(GLuint _divisor,glm::detail::tmat3x3<T,P>)
	{
		GLint i = 0;
		for(i=0;i<3;i++)
		{
			glVertexAttribDivisor(location+i,_divisor);
		}
	}

	template<typename T,glm::precision P>
	void set_divisor(GLuint _divisor,glm::detail::tmat3x4<T,P>)
	{
		GLint i = 0;
		for(i=0;i<3;i++)
		{
			glVertexAttribDivisor(location+i,_divisor);
		}
	}

	template<typename T,glm::precision P>
	void set_divisor(GLuint _divisor,glm::detail::tmat4x2<T,P>)
	{
		GLint i = 0;
		for(i=0;i<4;i++)
		{
			glVertexAttribDivisor(location+i,_divisor);
		}
	}

	template<typename T,glm::precision P>
	void set_divisor(GLuint _divisor,glm::detail::tmat4x3<T,P>)
	{
		GLint i = 0;
		for(i=0;i<4;i++)
		{
			glVertexAttribDivisor(location+i,_divisor);
		}
	}

	template<typename T,glm::precision P>
	void set_divisor(GLuint _divisor,glm::detail::tmat4x4<T,P>)
	{
		GLint i = 0;
		for(i=0;i<4;i++)
		{
			glVertexAttribDivisor(location+i,_divisor);
		}
	}

	// TODO: add matrix per column set 
};//struct AttributeLocation


struct UniformLocation : public Private::Location
{
protected:
	GLuint program;

public:
	UniformLocation(GLint _location = 0,GLuint _program = -1)
		:Private::Location(_location)
		,program(_program)
	{
	}

	template<typename _Type>
	bool set(const _Type& _value)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT
			|| GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glUniform1iv(location,1,detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glUniform1uiv(location,1,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glUniform1fv(location,1,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glUniform1dv(location,1,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glProgramUniform1iv(program,location,1,detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glProgramUniform1uiv(program,location,1,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glProgramUniform1fv(program,location,1,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glProgramUniform1dv(program,location,1,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type>
	bool set(const std::vector<_Type>& _value)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT
			|| GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_value.empty()) return false;
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glUniform1iv(location,_value.size(),detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glUniform1uiv(location,_value.size(),detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glUniform1fv(location,_value.size(),detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glUniform1dv(location,_value.size(),detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glProgramUniform1iv(program,location,_value.size(),detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glProgramUniform1uiv(program,location,_value.size(),detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glProgramUniform1fv(program,location,_value.size(),detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glProgramUniform1dv(program,location,_value.size(),detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tvec1<_Type,_Precision>& _value)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT
			|| GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glUniform1iv(location,1,detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glUniform1uiv(location,1,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glUniform1fv(location,1,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glUniform1dv(location,1,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glProgramUniform1iv(program,location,1,detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glProgramUniform1uiv(program,location,1,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glProgramUniform1fv(program,location,1,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glProgramUniform1dv(program,location,1,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const std::vector<glm::detail::tvec1<_Type,_Precision> >& _value)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT
			|| GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_value.empty()) return false;
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glUniform1iv(location,_value.size(),detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glUniform1uiv(location,_value.size(),detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glUniform1fv(location,_value.size(),detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glUniform1dv(location,_value.size(),detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glProgramUniform1iv(program,location,_value.size(),detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glProgramUniform1uiv(program,location,_value.size(),detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glProgramUniform1fv(program,location,_value.size(),detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glProgramUniform1dv(program,location,_value.size(),detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tvec2<_Type,_Precision>& _value)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT
			|| GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glUniform2iv(location,1,detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glUniform2uiv(location,1,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glUniform2fv(location,1,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glUniform2dv(location,1,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glProgramUniform2iv(program,location,1,detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glProgramUniform2uiv(program,location,1,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glProgramUniform2fv(program,location,1,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glProgramUniform2dv(program,location,1,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const std::vector<glm::detail::tvec2<_Type,_Precision> >& _value)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT
			|| GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_value.empty()) return false;
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glUniform2iv(location,_value.size(),detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glUniform2uiv(location,_value.size(),detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glUniform2fv(location,_value.size(),detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glUniform2dv(location,_value.size(),detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glProgramUniform2iv(program,location,_value.size(),detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glProgramUniform2uiv(program,location,_value.size(),detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glProgramUniform2fv(program,location,_value.size(),detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glProgramUniform2dv(program,location,_value.size(),detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tvec3<_Type,_Precision>& _value)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT
			|| GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glUniform3iv(location,1,detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glUniform3uiv(location,1,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glUniform3fv(location,1,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glUniform3dv(location,1,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glProgramUniform3iv(program,location,1,detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glProgramUniform3uiv(program,location,1,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glProgramUniform3fv(program,location,1,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glProgramUniform3dv(program,location,1,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const std::vector<glm::detail::tvec3<_Type,_Precision> >& _value)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT
			|| GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_value.empty()) return false;
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glUniform3iv(location,_value.size(),detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glUniform3uiv(location,_value.size(),detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glUniform3fv(location,_value.size(),detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glUniform3dv(location,_value.size(),detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glProgramUniform3iv(program,location,_value.size(),detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glProgramUniform3uiv(program,location,_value.size(),detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glProgramUniform3fv(program,location,_value.size(),detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glProgramUniform3dv(program,location,_value.size(),detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tvec4<_Type,_Precision>& _value)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT
			|| GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glUniform4iv(location,1,detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glUniform4uiv(location,1,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glUniform4fv(location,1,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glUniform4dv(location,1,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glProgramUniform4iv(program,location,1,detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glProgramUniform4uiv(program,location,1,detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glProgramUniform4fv(program,location,1,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glProgramUniform4dv(program,location,1,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const std::vector<glm::detail::tvec4<_Type,_Precision> >& _value)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_INT 
			|| GLType<_Type>::get==GL_UNSIGNED_INT
			|| GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_value.empty()) return false;
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glUniform4iv(location,_value.size(),detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glUniform4uiv(location,_value.size(),detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glUniform4fv(location,_value.size(),detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glUniform4dv(location,_value.size(),detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_INT:
				glProgramUniform4iv(program,location,_value.size(),detail::to_type<GL_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_UNSIGNED_INT:
				glProgramUniform4uiv(program,location,_value.size(),detail::to_type<GL_UNSIGNED_INT>::convert(Private::to_pointer(_value)));
				break;
			case GL_FLOAT:
				glProgramUniform4fv(program,location,_value.size(),detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_value)));
				break;
			case GL_DOUBLE:
				glProgramUniform4dv(program,location,_value.size(),detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_value)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tmat2x2<_Type,_Precision>& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix2fv(location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix2dv(location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix2fv(program,location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix2dv(program,location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}


	template<typename _Type,glm::precision _Precision>
	bool set(const std::vector<glm::detail::tmat2x2<_Type,_Precision> >& _matrix,bool _transpose1 = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_matrix.empty()) return false;
		GLboolean transpose = ToGLBoolean(_transpose1);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix2fv(location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix2dv(location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix2fv(program,location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix2dv(program,location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tmat3x3<_Type,_Precision>& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix3fv(location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix3dv(location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix3fv(program,location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix3dv(program,location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}


	template<typename _Type,glm::precision _Precision>
	bool set(const std::vector<glm::detail::tmat3x3<_Type,_Precision> >& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_matrix.empty()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix3fv(location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix3dv(location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix3fv(program,location,_matrix.size(),detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix3dv(program,location,_matrix.size(),detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tmat4x4<_Type,_Precision>& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix4fv(location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix4dv(location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix4fv(program,location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix4dv(program,location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}


	template<typename _Type,glm::precision _Precision>
	bool set(const std::vector<glm::detail::tmat4x4<_Type,_Precision> >& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_matrix.empty()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix4fv(location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix4dv(location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix4fv(program,location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix4dv(program,location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tmat2x3<_Type,_Precision>& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix2x3fv(location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix2x3dv(location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix2x3fv(program,location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix2x3dv(program,location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}


	template<typename _Type,glm::precision _Precision>
	bool set(const std::vector<glm::detail::tmat2x3<_Type,_Precision> >& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_matrix.empty()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix2x3fv(location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix2x3dv(location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix2x3fv(program,location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix2x3dv(program,location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tmat3x2<_Type,_Precision>& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix3x2fv(location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix3x2dv(location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix3x2fv(program,location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix3x2dv(program,location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}


	template<typename _Type,glm::precision _Precision>
	bool set(const std::vector<glm::detail::tmat3x2<_Type,_Precision> >& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_matrix.empty()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix3x2fv(location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix3x2dv(location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix3x2fv(program,location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix3x2dv(program,location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tmat2x4<_Type,_Precision>& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix2x4fv(location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix2x4dv(location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix2x4fv(program,location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix2x4dv(program,location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}


	template<typename _Type,glm::precision _Precision>
	bool set(const std::vector<glm::detail::tmat2x4<_Type,_Precision> >& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_matrix.empty()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix2x4fv(location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix2x4dv(location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix2x4fv(program,location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix2x4dv(program,location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tmat4x2<_Type,_Precision>& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix4x2fv(location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix4x2dv(location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix4x2fv(program,location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix4x2dv(program,location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const std::vector<glm::detail::tmat4x2<_Type,_Precision> >& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_matrix.empty()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix4x2fv(location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix4x2dv(location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix4x2fv(program,location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix4x2dv(program,location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tmat3x4<_Type,_Precision>& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix3x4fv(location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix3x4dv(location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix3x4fv(program,location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix3x4dv(program,location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}


	template<typename _Type,glm::precision _Precision>
	bool set(const std::vector<glm::detail::tmat3x4<_Type,_Precision> >& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_matrix.empty()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix3x4fv(location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix3x4dv(location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix3x4fv(program,location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix3x4dv(program,location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const glm::detail::tmat4x3<_Type,_Precision>& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix4x3fv(location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix4x3dv(location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix4x3fv(program,location,1,transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix4x3dv(program,location,1,transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	template<typename _Type,glm::precision _Precision>
	bool set(const std::vector<glm::detail::tmat4x3<_Type,_Precision> >& _matrix,bool _transpose = false)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_FLOAT 
			|| GLType<_Type>::get==GL_DOUBLE 
			,illegal_input_type
			);
		if(is_error_location()) return false;
		if(_matrix.empty()) return false;
		GLboolean transpose = ToGLBoolean(_transpose);
		if(EQL(program,-1))
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glUniformMatrix4x3fv(location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glUniformMatrix4x3dv(location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		else
		{
			switch(GLType<_Type>::get)
			{
			case GL_FLOAT:
				glProgramUniformMatrix4x3fv(program,location,_matrix.size(),transpose,detail::to_type<GL_FLOAT>::convert(Private::to_pointer(_matrix)));
				break;
			case GL_DOUBLE:
				glProgramUniformMatrix4x3dv(program,location,_matrix.size(),transpose,detail::to_type<GL_DOUBLE>::convert(Private::to_pointer(_matrix)));
				break;
			default:
				return false;
			}
		}
		GLenum err_code = GL_NO_ERROR;
		return is_error(err_code);
	}

	static UniformLocation get_error_location()
	{
		return UniformLocation(-1,-1);
	}

	bool is_error_location() const
	{
		UniformLocation el = get_error_location();
		return EQL(location,el.location)
			&& EQL(program,el.program)
			;
	}
};//struct UniformLocation

