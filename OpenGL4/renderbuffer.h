#pragma once 

#include "utils.h"
#include "simple.h"

struct Renderbuffer
{
protected:
	GLuint m_reneder_buffer_id;
	GLenum m_error_code;

public:
	Renderbuffer()
		:m_reneder_buffer_id(0)
		,m_error_code(GL_NO_ERROR)
	{
	}

	~Renderbuffer()
	{
		clear();
	}

	void clear()
	{
		if(glIsRenderbuffer(m_reneder_buffer_id))
		{
			glDeleteRenderbuffers(1,&m_reneder_buffer_id);
		}
		m_reneder_buffer_id = 0;
	}

	GLuint operator() () const 
	{
		return m_reneder_buffer_id;
	}

	GLenum get_last_error() const 
	{
		return m_error_code;
	}

	bool create()
	{
		clear();
		glGenRenderbuffers(1,&m_reneder_buffer_id);
		if(is_error(m_error_code)) return false;
		glBindRenderbuffer(GL_RENDERBUFFER,m_reneder_buffer_id);
		return !is_error(m_error_code);
	}

	bool bind()
	{
		if(!glIsRenderbuffer(m_reneder_buffer_id)) 
		{
			m_error_code = GL_INVALID_OPERATION;
			test_error(m_error_code);
			return false;
		}
		glBindRenderbuffer(GL_RENDERBUFFER,m_reneder_buffer_id);
		return !is_error(m_error_code);
	}

	/*
	_internal_format = 
	(color-renderable)------------------------------
	GL_RED GL_R8 GL_R16
	GL_RG GL_RG8 GL_RG16
	GL_RGB GL_R3_G3_B2 GL_RGB4
	GL_RGB5 GL_RGB8 GL_RGB10
	GL_RGB12 GL_RGB16 GL_RGBA
	GL_RGBA2 GL_RGBA4 GL_RGB5_A1
	GL_RGBA8 GL_RGB10_A2 GL_RGBA12
	GL_RGBA16 GL_SRGB GL_SRGB8
	GL_SRGB_ALPHA GL_SRGB8_ALPHA8 GL_R16F
	GL_R32F GL_RG16F GL_RG32F
	GL_RGB16F GL_RGB32F GL_RGBA16F
	GL_RGBA32F GL_R11F_G11F_B10F GL_RGB9_E5
	GL_R8I GL_R8UI GL_R16I
	GL_R16UI GL_R32I GL_R32UI
	GL_RG8I GL_RG8UI GL_RG16I
	GL_RG16UI GL_RG32I GL_RG32UI
	GL_RGB8I GL_RGB8UI GL_RGB16I
	GL_RGB16UI GL_RGB32I GL_RGB32UI
	GL_RGBA8I GL_RGBA8UI GL_RGBA16I
	GL_RGBA16UI GL_RGBA32I GL_R8_SNORM
	GL_R16_SNORM GL_RG8_SNORM GL_RG16_SNORM
	GL_RGB8_SNORM GL_RGB16_SNORM GL_RGBA8_SNORM
	GL_RGBA16_SNORM
	(depth-renderable)-------------------------------
	GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,
	GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT32, or
	GL_DEPTH_COMPONENT32F
	(stencil buffer)---------------------------------
	GL_STENCIL_INDEX, GL_STENCIL_INDEX1,
	GL_STENCIL_INDEX4, GL_STENCIL_INDEX8, or
	GL_STENCIL_INDEX16
	(packed depth-stencil storage)-------------------
	GL_DEPTH_STENCIL
	*/
	bool create_storage_multisample(GLsizei _samples,GLenum _internal_format,GLsizei _width,GLsizei _height)
	{
		if(!bind()) return false;
		glRenderbufferStorageMultisample(GL_RENDERBUFFER,_samples,_internal_format,_width,_height);
		return !is_error(m_error_code);
	}

	/*
	_internal_format = 
	(color-renderable)------------------------------
	GL_RED GL_R8 GL_R16
	GL_RG GL_RG8 GL_RG16
	GL_RGB GL_R3_G3_B2 GL_RGB4
	GL_RGB5 GL_RGB8 GL_RGB10
	GL_RGB12 GL_RGB16 GL_RGBA
	GL_RGBA2 GL_RGBA4 GL_RGB5_A1
	GL_RGBA8 GL_RGB10_A2 GL_RGBA12
	GL_RGBA16 GL_SRGB GL_SRGB8
	GL_SRGB_ALPHA GL_SRGB8_ALPHA8 GL_R16F
	GL_R32F GL_RG16F GL_RG32F
	GL_RGB16F GL_RGB32F GL_RGBA16F
	GL_RGBA32F GL_R11F_G11F_B10F GL_RGB9_E5
	GL_R8I GL_R8UI GL_R16I
	GL_R16UI GL_R32I GL_R32UI
	GL_RG8I GL_RG8UI GL_RG16I
	GL_RG16UI GL_RG32I GL_RG32UI
	GL_RGB8I GL_RGB8UI GL_RGB16I
	GL_RGB16UI GL_RGB32I GL_RGB32UI
	GL_RGBA8I GL_RGBA8UI GL_RGBA16I
	GL_RGBA16UI GL_RGBA32I GL_R8_SNORM
	GL_R16_SNORM GL_RG8_SNORM GL_RG16_SNORM
	GL_RGB8_SNORM GL_RGB16_SNORM GL_RGBA8_SNORM
	GL_RGBA16_SNORM
	(depth-renderable)-------------------------------
	GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,
	GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT32, or
	GL_DEPTH_COMPONENT32F
	(stencil buffer)---------------------------------
	GL_STENCIL_INDEX, GL_STENCIL_INDEX1,
	GL_STENCIL_INDEX4, GL_STENCIL_INDEX8, or
	GL_STENCIL_INDEX16
	(packed depth-stencil storage)-------------------
	GL_DEPTH_STENCIL
	*/
	bool create_storage(GLenum _internal_format,GLsizei _width,GLsizei _height)
	{
		if(!bind()) return false;
		glRenderbufferStorage(GL_RENDERBUFFER,_internal_format,_width,_height);
		return !is_error(m_error_code);
	}
};//struct Renderbuffer