#pragma once

#include "utils.h"
#include "simple.h"
#include "renderbuffer.h"
#include "texture.h"

struct Framebuffer 
{
protected:
	GLuint m_frame_buffer_id;
	GLenum m_target;
	GLenum m_error_code;
	std::vector<GLenum> m_attachments;

public:
	Framebuffer(GLenum _target = 0)
		:m_frame_buffer_id(0)
		,m_target(GL_FRAMEBUFFER)
		,m_error_code(GL_NO_ERROR)
	{
		if(NEQL(_target,0))
		{
			VERIFY(create(_target));
		}
	}

	~Framebuffer()
	{
		clear();
	}

	GLenum get_last_error() const 
	{
		return m_error_code;
	}

	void clear()
	{
		if(glIsFramebuffer(m_frame_buffer_id))
		{
			glDeleteFramebuffers(1,&m_frame_buffer_id);
		}
		m_frame_buffer_id = 0;
		m_target = GL_FRAMEBUFFER;
	}

	/*
	_target = GL_DRAW_FRAMEBUFFER|GL_READ_FRAMEBUFFER|GL_FRAMEBUFFER
	*/
	bool create(GLenum _target = GL_FRAMEBUFFER)
	{
		clear();
		glGenFramebuffers(1,&m_frame_buffer_id);
		if(is_error(m_error_code)) return false;
		m_target = _target;
		glBindFramebuffer(m_target,m_frame_buffer_id);
		return !is_error(m_error_code);
	}

	bool bind()
	{
		if(!glIsFramebuffer(m_frame_buffer_id)) 
		{
			m_error_code = GL_INVALID_OPERATION;
			test_error(m_error_code);
			return false;
		}
		glBindFramebuffer(m_target,m_frame_buffer_id);
		return !is_error(m_error_code);
	}

	/*
	_pname = GL_FRAMEBUFFER_DEFAULT_WIDTH|GL_FRAMEBUFFER_DEFAULT_HEIGHT
	|GL_FRAMEBUFFER_DEFAULT_LAYERS|GL_FRAMEBUFFER_DEFAULT_SAMPLES
	|GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS 
	*/
	bool set_parameter(GLenum _pname,GLint _param)
	{
		if(!bind()) return false;
		glFramebufferParameteri(m_target,_pname,_param);
		return !is_error(m_error_code);
	}

	/*
	_attachment = GL_COLOR_ATTACHMENT0..
	|GL_DEPTH_ATTACHMENT|GL_STENCIL_ATTACHMENT|GL_DEPTH_STENCIL_ATTACHMENT
	*/
	bool set_render_buffer(GLenum _attachment,const Renderbuffer& _render_buffer)
	{
		if(!bind()) return false;
		glFramebufferRenderbuffer(m_target,_attachment,GL_RENDERBUFFER,_render_buffer());
		if(is_error(m_error_code)) return false;
		m_attachments.push_back(_attachment);
		return true;
	}

	/*
	_attachment = GL_COLOR_ATTACHMENT0..
	|GL_DEPTH_ATTACHMENT|GL_STENCIL_ATTACHMENT|GL_DEPTH_STENCIL_ATTACHMENT
	*/
	bool set_render_texture(GLenum _attachment,const Texture& _texture,GLint _level)
	{
		if(!bind()) return false;
		glFramebufferTexture2D(m_target,_attachment,_texture.get_target(),_texture(),_level);
		if(is_error(m_error_code)) return false;
		m_attachments.push_back(_attachment);
		return true;
	}

	/*
	returns = GL_FRAMEBUFFER_COMPLETE| GL_FRAMEBUFFER_UNDEFINED 
		| GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT | GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
		| GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER | GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
		| GL_FRAMEBUFFER_UNSUPPORTED | GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
	*/
	GLenum check_status()
	{
		if(!bind()) return GL_FRAMEBUFFER_UNDEFINED;
		GLenum ret = glCheckFramebufferStatus(m_target);
		test_error(m_error_code);
		return ret;
	}

	bool invalidate()
	{
		if(!bind()) return false;
		glInvalidateFramebuffer(m_target,m_attachments.size(),Private::to_pointer(m_attachments));
		return !is_error(m_error_code);
	}

	bool invalidate_sub(GLint _x, GLint _y,GLsizei _width, GLsizei _height)
	{
		if(!bind()) return false;
		glInvalidateSubFramebuffer(
			m_target,m_attachments.size(),Private::to_pointer(m_attachments)
			,_x,_y,_width,_height
			);
		return !is_error(m_error_code);
	}

	/*
	_buffer = GL_FRONT GL_FRONT_LEFT GL_NONE
	GL_BACK GL_FRONT_RIGHT GL_FRONT_AND_BACK
	GL_LEFT GL_BACK_LEFT GL_COLOR_ATTACHMENTi
	GL_RIGHT GL_BACK_RIGHT
	*/
	bool draw_buffer(GLenum _buffer)
	{
		if(!bind()) return false;
		glDrawBuffer(_buffer);
		return !is_error(m_error_code);
	}

	/*
	_buffer,... = GL_FRONT GL_FRONT_LEFT GL_NONE
	GL_BACK GL_FRONT_RIGHT GL_FRONT_AND_BACK
	GL_LEFT GL_BACK_LEFT GL_COLOR_ATTACHMENTi
	GL_RIGHT GL_BACK_RIGHT
	*/
	bool draw_buffers(GLenum _buffer, ...)
	{
		if(!bind()) return false;
		std::vector<GLenum> buffers;
		buffers.push_back(_buffer);
		va_list args;
		va_start(args,_buffer);
		GLenum buf = 0;
		for(;NEQL(0,(buf = va_arg(args,GLenum)));)
		{
			buffers.push_back(buf);
		}
		va_end(args);
		glDrawBuffers(buffers.size(),Private::to_pointer(buffers));
		return !is_error(m_error_code);
	}

	/*
	_buffer = GL_FRONT GL_FRONT_LEFT GL_NONE
	GL_BACK GL_FRONT_RIGHT GL_FRONT_AND_BACK
	GL_LEFT GL_BACK_LEFT GL_COLOR_ATTACHMENTi
	GL_RIGHT GL_BACK_RIGHT
	*/
	bool read_buffer(GLenum _buffer)
	{
		if(!bind()) return false;
		glReadBuffer(_buffer);
		return !is_error(m_error_code);
	}

/*
	TODO:
	void glEnablei(GLenum capability, GLuint index);
	void glDisablei(GLenum capability, GLuint index);
	GLboolean glIsEnabledi(GLenum capability, GLuint index);
*/


};//struct Framebuffer 

