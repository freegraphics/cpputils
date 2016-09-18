#pragma once 

#include "utils.h"
#include "types.h"
#include "simple.h"

struct Texture
{
protected:
	GLuint m_texture_id;
	GLenum m_error_code;
	GLenum m_target;

public:
	/*
	_target = GL_TEXTURE_1D,
	GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_1D_ARRAY,
	GL_TEXTURE_2D_ARRAY, GL_TEXTURE_RECTANGLE,
	GL_TEXTURE_BUFFER, GL_TEXTURE_CUBE_MAP,
	GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_2D_MULTISAMPLE,
	GL_TEXTURE_2D_MULTISAMPLE_ARRAY
	*/
	Texture(GLenum _target = 0)
		:m_error_code(GL_NO_ERROR)
		,m_target(GL_TEXTURE_2D)
	{
		if(NEQL(0,_target)) 
		{
			VERIFY(create(_target));
		}
	}

	~Texture()
	{
		clear();
	}

	GLenum get_last_error() const 
	{
		return m_error_code;
	}

	GLenum get_target() const 
	{
		return m_target;
	}

	GLuint operator() () const 
	{
		return m_texture_id;
	}

	/*
	_target = GL_TEXTURE_1D,
	GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_1D_ARRAY,
	GL_TEXTURE_2D_ARRAY, GL_TEXTURE_RECTANGLE,
	GL_TEXTURE_BUFFER, GL_TEXTURE_CUBE_MAP,
	GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_2D_MULTISAMPLE,
	GL_TEXTURE_2D_MULTISAMPLE_ARRAY
	*/
	bool create(GLenum _target = GL_TEXTURE_2D)
	{
		clear();
		glGenTextures(1,&m_texture_id);
		if(is_error(m_error_code)) return false;
		m_target = _target;
		glBindTexture(m_target,m_texture_id);
		return !is_error(m_error_code);
	}

	bool bind()
	{
		if(!glIsTexture(m_texture_id)) 
		{
			m_error_code = GL_INVALID_OPERATION;
			return !test_error(m_error_code);
		}
		glBindTexture(m_target,m_texture_id);
		return !is_error(m_error_code);
	}

	void clear()
	{
		if(glIsTexture(m_texture_id))
		{
			glDeleteTextures(1,&m_texture_id);
		}
		m_texture_id = 0;
	}

	bool unbind_from_texture_unit()
	{
		glBindTexture(m_target,0);
		return !is_error(m_error_code);
	}

	bool create_storage_1d(GLsizei _levels,GLenum _internalFormat, GLsizei _width)
	{
		if(!bind()) return false;
		glTexStorage1D(m_target,_levels,_internalFormat,_width);
		return !is_error(m_error_code);
	}

	bool create_storage_2d(GLsizei _levels,GLenum _internalFormat
		,GLsizei _width,GLsizei _height
		)
	{
		if(!bind()) return false;
		glTexStorage2D(m_target,_levels,_internalFormat,_width,_height);
		return !is_error(m_error_code);
	}

	bool create_storage_3d(GLsizei _levels,GLenum _internalFormat
		,GLsizei _width,GLsizei _height,GLsizei _depth
		)
	{
		if(!bind()) return false;
		glTexStorage3D(m_target,_levels,_internalFormat,_width,_height,_depth);
		return !is_error(m_error_code);
	}

	bool create_storage_2d_multisample(GLsizei _samples,GLenum _internalFormat
		,GLsizei _width,GLsizei _height,bool _fixedsamplelocations
		)
	{
		if(!bind()) return false;
		glTexStorage2DMultisample(m_target,_samples,_internalFormat
			,_width,_height
			,ToGLBoolean(_fixedsamplelocations)
			);
		return !is_error(m_error_code);
	}

	bool create_storage_3d_mulisample(GLsizei _samples,GLenum _internalFormat
		,GLsizei _width,GLsizei _height,GLsizei _depth,bool _fixedsamplelocations
		)
	{
		if(!bind()) return false;
		glTexStorage3DMultisample(m_target,_samples,_internalFormat
			,_width,_height,_depth
			,ToGLBoolean(_fixedsamplelocations)
			);
		return !is_error(m_error_code);
	}

	template<typename _Iterator>
	bool create_mutable_storage_1d(
		GLint _level,GLint _internalFormat, GLsizei _width,GLint _border
		,_Iterator _it,_Iterator _ite
		,GLenum _format
		,GLenum _type = GLMType<typename _Iterator::value_type>::get_element_gl_type
		)
	{
		if(!bind()) return false;
		if(NEQL(_it+0,_ite) && NEQL((GLsizei)(_ite-_it),_width))
		{
			m_error_code = GL_INVALID_VALUE;
			return !test_error(m_error_code);
		}
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
		if(is_error(m_error_code)) return false;
		glTexImage1D(m_target,_level,_internalFormat,_width,_border
			,_format,_type
			,NEQL(_it,_ite)?(const GLvoid*)Private::to_pointer(*_it):NULL
			);
		return !is_error(m_error_code);
	}

	bool create_mutable_storage_1d(
		GLint _level,GLint _internalFormat, GLsizei _width,GLint _border
		,Buffer& _pixel_unpack_buffer
		,GLsizei _offset
		,GLenum _format
		,GLenum _type
		)
	{
		if(!bind()) return false;
		if(NEQL(_pixel_unpack_buffer.get_target(),GL_PIXEL_UNPACK_BUFFER))
		{
			m_error_code = GL_INVALID_VALUE;
			return !test_error(m_error_code);
		}
		if(is_error(m_error_code)) return false;
		_pixel_unpack_buffer.bind();
		glTexImage1D(m_target,_level,_internalFormat,_width,_border
			,_format,_type,BUFFER_OFFSET(_offset)
			);
		return !is_error(m_error_code);
	}

	template<typename _Iterator>
	bool create_mutable_storage_2d(
		GLint _level,GLint _internalFormat, GLsizei _width,GLsizei _height,GLint _border
		,_Iterator _it,_Iterator _ite
		,GLenum _format
		,GLenum _type = GLMType<typename _Iterator::value_type>::get_element_gl_type
		)
	{
		if(!bind()) return false;
		if(NEQL(_it+0,_ite) && NEQL((GLsizei)(_ite-_it),_width*_height))
		{
			m_error_code = GL_INVALID_VALUE;
			return !test_error(m_error_code);
		}
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
		if(is_error(m_error_code)) return false;
		glTexImage2D(m_target,_level,_internalFormat,_width,_height,_border
			,_format,_type
			,NEQL(_it,_ite)?(const GLvoid*)Private::to_pointer(*_it):NULL
			);
		return !is_error(m_error_code);
	}

	bool create_mutable_storage_2d(
		GLint _level,GLint _internalFormat, GLsizei _width,GLsizei _height,GLint _border
		,Buffer& _pixel_unpack_buffer
		,GLsizei _offset
		,GLenum _format
		,GLenum _type
		)
	{
		if(!bind()) return false;
		if(NEQL(_pixel_unpack_buffer.get_target(),GL_PIXEL_UNPACK_BUFFER))
		{
			m_error_code = GL_INVALID_VALUE;
			return !test_error(m_error_code);
		}
		if(is_error(m_error_code)) return false;
		_pixel_unpack_buffer.bind();
		glTexImage2D(m_target,_level,_internalFormat,_width,_height,_border
			,_format,_type,BUFFER_OFFSET(_offset)
			);
		return !is_error(m_error_code);
	}

	template<typename _Iterator>
	bool create_mutable_storage_2d(
		GLint _level,GLint _internalFormat
		,GLsizei _width,GLsizei _height,GLsizei _depth,GLint _border
		,_Iterator _it,_Iterator _ite
		,GLenum _format
		,GLenum _type = GLMType<typename _Iterator::value_type>::get_element_gl_type
		)
	{
		if(!bind()) return false;
		if(NEQL(_it+0,_ite) && NEQL((GLsizei)(_ite-_it),_width*_height*_depth))
		{
			m_error_code = GL_INVALID_VALUE;
			return !test_error(m_error_code);
		}
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
		if(is_error(m_error_code)) return false;
		glTexImage3D(m_target,_level,_internalFormat,_width,_height,_depth,_border
			,_format,_type
			,NEQL(_it,_ite)?(const GLvoid*)Private::to_pointer(*_it):NULL
			);
		return !is_error(m_error_code);
	}

	bool create_mutable_storage_3d(
		GLint _level,GLint _internalFormat
		,GLsizei _width,GLsizei _height,GLsizei _depth,GLint _border
		,Buffer& _pixel_unpack_buffer
		,GLsizei _offset
		,GLenum _format
		,GLenum _type
		)
	{
		if(!bind()) return false;
		if(NEQL(_pixel_unpack_buffer.get_target(),GL_PIXEL_UNPACK_BUFFER))
		{
			m_error_code = GL_INVALID_VALUE;
			return !test_error(m_error_code);
		}
		if(is_error(m_error_code)) return false;
		_pixel_unpack_buffer.bind();
		glTexImage3D(m_target,_level,_internalFormat,_width,_height,_depth,_border
			,_format,_type,BUFFER_OFFSET(_offset)
			);
		return !is_error(m_error_code);
	}

	bool create_mutable_storage_2d_mulisample(
		GLenum _samples
		,GLint _internalFormat
		,GLsizei _width, GLsizei _height
		,bool _fixedsamplelocations
		)
	{
		if(!bind()) return false;
		glTexImage2DMultisample(m_target
			,_samples
			,_internalFormat
			,_width,_height
			,ToGLBoolean(_fixedsamplelocations)
			);
		return !is_error(m_error_code);
	}

	bool create_mutable_storage_3d_mulisample(
		GLenum _samples
		,GLint _internalFormat
		,GLsizei _width, GLsizei _height,GLsizei _depth
		,bool _fixedsamplelocations
		)
	{
		if(!bind()) return false;
		glTexImage3DMultisample(m_target
			,_samples
			,_internalFormat
			,_width,_height,_depth
			,ToGLBoolean(_fixedsamplelocations)
			);
		return !is_error(m_error_code);
	}



	/*
	_texture = GL_TEXTUREi (GL_TEXTURE0 + i)
	*/
	static void set_active_texture_unit(GLenum _texture)
	{
		glActiveTexture(_texture);
	}
};