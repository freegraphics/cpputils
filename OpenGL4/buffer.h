#pragma once

#include "utils.h"
#include "types.h"
#include "simple.h"
#include "SimpleVertexArray.h"
#include "uniformbuffer.h"

struct Buffer
{
protected:
	GLuint m_buffer_id;
	GLenum m_target;
	size_t m_size;
	GLenum m_err_code;

public:
	Buffer()
		:m_buffer_id(0)
		,m_target(0)
		,m_size(0)
		,m_err_code(GL_NO_ERROR)
	{
	}

	Buffer(const SimpleVertexArray& _vertex_array)
		:m_buffer_id(0)
		,m_target(0)
		,m_size(0)
		,m_err_code(GL_NO_ERROR)
	{
		m_buffer_id = _vertex_array.buffer_id;
		m_target = GL_ARRAY_BUFFER;
		m_size = _vertex_array.size();
	}

	Buffer(const UniformBuffer& _uniform_buffer)
		:m_buffer_id(0)
		,m_target(0)
		,m_size(0)
		,m_err_code(GL_NO_ERROR)
	{
		m_buffer_id = _uniform_buffer.m_uniform_buffer_id;
		m_target = GL_UNIFORM_BUFFER;
		m_size = _uniform_buffer.m_size;
	}

	~Buffer()
	{
		clear();
	}

	GLenum get_target() const 
	{
		return m_target;
	}

	/*
	(OpenGL 3.0)
	_target = GL_ARRAY_BUFFER,GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER
	,GL_ELEMENT_ARRAY_BUFFER,GL_PIXEL_PACK_BUFFER,GL_PIXEL_UNPACK_BUFFER
	,GL_TEXTURE_BUFFER,GL_TRANSFORM_FEEDBACK_BUFFER,GL_UNIFORM_BUFFER
	(OpenGL 4.5)
	,GL_ATOMIC_COUNTER_BUFFER,GL_DISPATCH_INDIRECT_BUFFER,GL_DRAW_INDIRECT_BUFFER
	,GL_QUERY_BUFFER,GL_SHADER_STORAGE_BUFFER
	*/
	bool create(GLenum _target)
	{
		clear();
		m_target = _target;
		glGenBuffers(1,&m_buffer_id);
		if(is_error(m_err_code)) return false;
		return bind();
	}

	bool is_created() const 
	{
		return EQL(glIsBuffer(m_buffer_id) ,GL_TRUE);
	}

	void clear()
	{
		if(glIsBuffer(m_buffer_id))
		{
			glDeleteBuffers(1,&m_buffer_id);
		}
		m_buffer_id = 0;
		m_size = 0;
	}

	size_t size() const
	{
		return m_size;
	}

	GLenum get_last_error() const 
	{
		return m_err_code;
	}

	bool alloc_data(size_t _size,GLenum _usage = GL_STATIC_DRAW)
	{
		if(!prepare()) return false;
		m_size = _size;
		glBufferData(m_target,m_size,NULL,_usage);
		return !is_error(m_err_code);
	}

	bool set_data(size_t _size,const GLvoid* _data,GLenum _usage = GL_STATIC_DRAW)
	{
		if(!prepare()) return false;
		m_size = _size;
		glBufferData(m_target,m_size,_data,_usage);
		return !is_error(m_err_code);
	}

	bool set_sub_data(size_t _offset,size_t _size,LPVOID _data)
	{
		if(!prepare()) return false;
		glBufferSubData(m_target,_offset,_size,_data);
		return !is_error(m_err_code);
	}

	
	/*
	_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED,GL_GREEN,GL_BLUE
		,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER,GL_GREEN_INTEGER
		,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER,GL_RGBA_INTEGER
		,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type>
	bool clear_data(const _Type& _item,GLenum _format,bool _normilize = false)
	{
		if(!prepare()) return false;
		glClearBufferData(
			m_target
			,_normilize?GLInternalType<_Type,1>::get_normilized:GLInternalType<_Type,1>::get
			,_format,GLType<_Type>::get
			,Private::to_pointer(_item)
			);
		return !is_error(m_err_code);
	}

	/*
	_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED,GL_GREEN,GL_BLUE
		,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER,GL_GREEN_INTEGER
		,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER,GL_RGBA_INTEGER
		,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type,glm::precision _Precision>
	bool clear_data(const glm::detail::tvec1<_Type,_Precision>& _item,GLenum _format,bool _normilize = false)
	{
		if(!prepare()) return false;
		glClearBufferData(
			m_target
			,_normilize?GLInternalType<_Type,1>::get_normilized:GLInternalType<_Type,1>::get
			,_format,GLType<_Type>::get
			,Private::to_pointer(_item)
			);
		return !is_error(m_err_code);
	}

	/*
	_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED,GL_GREEN,GL_BLUE
		,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER,GL_GREEN_INTEGER
		,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER,GL_RGBA_INTEGER
		,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type,glm::precision _Precision>
	bool clear_data(const glm::detail::tvec2<_Type,_Precision>& _item,GLenum _format,bool _normilize = false)
	{
		if(!prepare()) return false;
		glClearBufferData(
			m_target
			,_normilize?GLInternalType<_Type,2>::get_normilized:GLInternalType<_Type,2>::get
			,_format,GLType<_Type>::get
			,Private::to_pointer(_item)
			);
		return !is_error(m_err_code);
	}

	/*
	_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED,GL_GREEN,GL_BLUE
		,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER,GL_GREEN_INTEGER
		,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER,GL_RGBA_INTEGER
		,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type,glm::precision _Precision>
	bool clear_data(const glm::detail::tvec3<_Type,_Precision>& _item,GLenum _format,bool _normilize = false)
	{
		if(!prepare()) return false;
		glClearBufferData(
			m_target
			,_normilize?GLInternalType<_Type,3>::get_normilized:GLInternalType<_Type,3>::get
			,_format,GLType<_Type>::get
			,Private::to_pointer(_item)
			);
		return !is_error(m_err_code);
	}

	/*
	_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED,GL_GREEN,GL_BLUE
		,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER,GL_GREEN_INTEGER
		,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER,GL_RGBA_INTEGER
		,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type,glm::precision _Precision>
	bool clear_data(const glm::detail::tvec4<_Type,_Precision>& _item,GLenum _format,bool _normilize = false)
	{
		if(!prepare()) return false;
		glClearBufferData(
			m_target
			,_normilize?GLInternalType<_Type,4>::get_normilized:GLInternalType<_Type,4>::get
			,_format,GLType<_Type>::get
			,Private::to_pointer(_item)
			);
		return !is_error(m_err_code);
	}

	/*
	_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED,GL_GREEN,GL_BLUE
		,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER,GL_GREEN_INTEGER
		,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER,GL_RGBA_INTEGER
		,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type>
	bool clear_sub_data(const _Type& _item,GLintptr _offs,GLsizeiptr _size,GLenum _format,bool _normilize = false)
	{
		if(!prepare()) return false;
		glClearBufferSubData(
			m_target
			,_normilize?GLInternalType<_Type,1>::get_normilized:GLInternalType<_Type,1>::get
			,_offs,_size
			,_format,GLType<_Type>::get
			,Private::to_pointer(_item)
			);
		return !is_error(m_err_code);
	}

	/*
	_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED,GL_GREEN,GL_BLUE
		,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER,GL_GREEN_INTEGER
		,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER,GL_RGBA_INTEGER
		,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type,glm::precision _Precision>
	bool clear_sub_data(const glm::detail::tvec1<_Type,_Precision>& _item,GLintptr _offs,GLsizeiptr _size,GLenum _format,bool _normilize = false)
	{
		if(!prepare()) return false;
		glClearBufferSubData(
			m_target
			,_normilize?GLInternalType<_Type,1>::get_normilized:GLInternalType<_Type,1>::get
			,_offs,_size
			,_format,GLType<_Type>::get
			,Private::to_pointer(_item)
			);
		return !is_error(m_err_code);
	}

	/*
	_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED,GL_GREEN,GL_BLUE
		,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER,GL_GREEN_INTEGER
		,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER,GL_RGBA_INTEGER
		,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type,glm::precision _Precision>
	bool clear_sub_data(const glm::detail::tvec2<_Type,_Precision>& _item,GLintptr _offs,GLsizeiptr _size,GLenum _format,bool _normilize = false)
	{
		if(!prepare()) return false;
		glClearBufferSubData(
			m_target
			,_normilize?GLInternalType<_Type,2>::get_normilized:GLInternalType<_Type,2>::get
			,_offs,_size
			,_format,GLType<_Type>::get
			,Private::to_pointer(_item)
			);
		return !is_error(m_err_code);
	}

	/*
	_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED,GL_GREEN,GL_BLUE
		,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER,GL_GREEN_INTEGER
		,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER,GL_RGBA_INTEGER
		,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type,glm::precision _Precision>
	bool clear_sub_data(const glm::detail::tvec3<_Type,_Precision>& _item,GLintptr _offs,GLsizeiptr _size,GLenum _format,bool _normilize = false)
	{
		if(!prepare()) return false;
		glClearBufferSubData(
			m_target
			,_normilize?GLInternalType<_Type,3>::get_normilized:GLInternalType<_Type,3>::get
			,_offs,_size
			,_format,GLType<_Type>::get
			,Private::to_pointer(_item)
			);
		return !is_error(m_err_code);
	}

	/*
	_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED,GL_GREEN,GL_BLUE
		,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER,GL_GREEN_INTEGER
		,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER,GL_RGBA_INTEGER
		,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type,glm::precision _Precision>
	bool clear_sub_data(const glm::detail::tvec4<_Type,_Precision>& _item,GLintptr _offs,GLsizeiptr _size,GLenum _format,bool _normilize = false)
	{
		if(!prepare()) return false;
		glClearBufferSubData(
			m_target
			,_normilize?GLInternalType<_Type,4>::get_normilized:GLInternalType<_Type,4>::get
			,_offs,_size
			,_format,GLType<_Type>::get
			,Private::to_pointer(_item)
			);
		return !is_error(m_err_code);
	}

	bool copy_data_from(const Buffer& _buffer)
	{
		if(_buffer.size()!=size())
		{
			set_error(GL_INVALID_VALUE);
			return false;
		}
		return copy_data_from(_buffer,0,0,size());
	}

	bool copy_data_from(const Buffer& _buffer,GLintptr _readoffs,GLintptr _writeoffs,GLsizeiptr _size)
	{
		glBindBuffer(GL_COPY_READ_BUFFER,_buffer.m_buffer_id);
		if(is_error(m_err_code)) return false;
		glBindBuffer(GL_COPY_WRITE_BUFFER,m_buffer_id);
		if(is_error(m_err_code)) return false;
		glCopyBufferSubData(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,_readoffs,_writeoffs,_size);
		return !is_error(m_err_code);
	}

	bool get_sub_data(GLintptr _offs,GLsizeiptr _size,GLvoid* _data)
	{
		if(!prepare()) return false;
		glGetBufferSubData(m_target,_offs,_size,_data);
		return !is_error(m_err_code);
	}

	/*
	_access = GL_READ_ONLY,GL_WRITE_ONLY,GL_READ_WRITE
	*/
	GLvoid* map_data(GLenum _access)
	{
		if(!prepare()) return NULL;
		GLvoid* ptr = glMapBuffer(m_target,_access);
		is_error(m_err_code);
		return ptr;
	}

	bool unmap_data()
	{
		if(!prepare()) return false;
		GLboolean ret = glUnmapBuffer(m_target);
		is_error(m_err_code);
		return EQL(GL_TRUE,ret);
	}

	/*
	_access = GL_MAP_READ_BIT,GL_MAP_WRITE_BIT
		,GL_MAP_INVALIDATE_RANGE_BIT,GL_MAP_INVALIDATE_BUFFER_BIT
		,GL_MAP_FLUSH_EXPLICIT_BIT,GL_MAP_UNSYNCHRONIZED_BIT
	page 104 of the "Red Book"
	*/
	GLvoid* map_data_range(GLintptr _offset,GLsizeiptr _size, GLbitfield _access)
	{
		if(!prepare()) return NULL;
		GLvoid* ptr = glMapBufferRange(m_target,_offset,_size,_access);
		is_error(m_err_code);
		return ptr;
	}

	bool flush_mapped_data_range(GLintptr _offset,GLsizeiptr _size)
	{
		if(!prepare()) return NULL;
		glFlushMappedBufferRange(m_target,_offset,_size);
		return !is_error(m_err_code);
	}

	/* 
	to discard data in the buffer. 
	to free allocated buffer in the GPU so it can be used 
	by someone else.
	*/
	bool invalidate_data()
	{
		glInvalidateBufferData(m_buffer_id);
		return !is_error(m_err_code);
	}

	/*
	to discard some piece of the buffer.
	to free allocated buffer in the GPU so it can be used 
	by someone else.
	*/
	bool invalidate_sub_data(GLintptr _offset,GLsizeiptr _size)
	{
		glInvalidateBufferSubData(m_buffer_id,_offset,_size);
		return !is_error(m_err_code);
	}

	bool bind()
	{
		glBindBuffer(m_target,m_buffer_id);
		return !is_error(m_err_code);
	}

protected:
	void set_error(GLenum _error_code)
	{
		m_err_code = _error_code;
		test_error(m_err_code);
	}

	bool prepare()
	{
		if(!glIsBuffer(m_buffer_id)) 
		{
			if(m_target==0) 
			{
				set_error(GL_INVALID_OPERATION);
				return false;
			}
			clear();
			return create(m_target);
		}
		else
		{
			return bind();
		}
	}
};//struct Buffer
