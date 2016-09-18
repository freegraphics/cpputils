#pragma once

#include "utils.h"
#include "simple.h"
#include "types.h"

struct Buffer;

struct SimpleVertexArray
{
protected:
	GLuint vertex_array_id;
	GLuint buffer_id;
	size_t data_items;
	size_t data_size;
	size_t vec_item_elements;
	GLenum data_element_type;
	GLboolean int_type_flag;
	GLboolean double_type_flag;
	GLenum m_err_code;
	AttributeLocation m_location;

public:
	SimpleVertexArray()
		:vertex_array_id(0)
		,buffer_id(0)
		,data_items(0)
		,vec_item_elements(0)
		,data_element_type(0)
		,data_size(0)
		,int_type_flag(GL_FALSE)
		,double_type_flag(GL_FALSE)
		,m_err_code(GL_NO_ERROR)
	{
	}

	~SimpleVertexArray()
	{
		clear();
	}

	void clear()
	{
		if(glIsBuffer(buffer_id))
		{
			stop_using_buffer();
			glDeleteBuffers(1,&buffer_id);
		}
		buffer_id = 0;
		if(glIsVertexArray(vertex_array_id))
		{
			stop_using_vertex_array();
			glDeleteVertexArrays(1,&vertex_array_id);
		}
		vertex_array_id = 0;
		data_items = 0;
		data_size = 0;
	}

	GLenum get_last_error() const {return m_err_code;}

	bool create()
	{
		glGenVertexArrays(1,&vertex_array_id);
		if(is_error(m_err_code)) return false;
		glBindVertexArray(vertex_array_id);
		if(is_error(m_err_code)) return false;
		glGenBuffers(1,&buffer_id);
		if(is_error(m_err_code)) return false;
		glBindBuffer(GL_ARRAY_BUFFER,buffer_id);
		return !is_error(m_err_code);
	}

	/*
		_usage = GL_[STATIC|DYNAMIC|STREAM]_[DRAW|READ|COPY]
		page 96 of the "Red Book"
	*/
	template<typename _Type>
	bool alloc_data(
		size_t _items_count
		,size_t _vec_item_elements
		,GLenum _usage = GL_STATIC_DRAW
		,GLenum _type = GLType<_Type>::get
		)
	{
		if(EQL(_items_count,0))
		{
			m_err_code = GL_INVALID_VALUE;
			return false;
		}
		if(!prepare_buffer()) return false;

		data_items = _items_count;
		data_size = _items_count*_vec_item_elements*sizeof(_Type);
		glBufferData(GL_ARRAY_BUFFER
			,data_size
			,NULL
			,_usage
			);
		vec_item_elements = _vec_item_elements;
		data_element_type = _type;
		int_type_flag = GLType<_Type>::is_int;
		double_type_flag = GLType<_Type>::is_double;
		return !is_error(m_err_code);
	}

	/*
		_usage = GL_[STATIC|DYNAMIC|STREAM]_[DRAW|READ|COPY]
		page 96 of the "Red Book"
	*/
	template <typename T>
	bool set_data(
		const std::vector<T>& _array
		,GLenum _usage = GL_STATIC_DRAW
		,GLenum _type = GLType<T>::get
		)
	{
		if(_array.empty())
		{
			m_err_code = GL_INVALID_VALUE;
			return false;
		}
		if(!prepare_buffer()) return false;

		data_items = _array.size();
		data_size = _array.size()*1*sizeof(T);
		glBufferData(GL_ARRAY_BUFFER
			,data_size
			,Private::to_pointer(_array)
			,_usage
			);
		vec_item_elements = 1;
		data_element_type = _type;
		int_type_flag = GLType<T>::is_int;
		double_type_flag = GLType<T>::is_double;
		return !is_error(m_err_code);
	}

	/*
		_usage = GL_[STATIC|DYNAMIC|STREAM]_[DRAW|READ|COPY]
		page 96 of the "Red Book"
	*/
	template <typename T, glm::precision P>
	bool set_data(
		const std::vector<glm::detail::tvec1<T,P> >& _array
		,GLenum _usage = GL_STATIC_DRAW
		,GLenum _type = GLType<T>::get
		)
	{
		if(_array.empty())
		{
			m_err_code = GL_INVALID_VALUE;
			return false;
		}
		if(!prepare_buffer()) return false;

		data_items = _array.size();
		data_size = _array.size()*1*sizeof(T);
		glBufferData(GL_ARRAY_BUFFER
			,data_size
			,Private::to_pointer(_array)
			,_usage
			);
		vec_item_elements = 1;
		data_element_type = _type;
		int_type_flag = GLType<T>::is_int;
		double_type_flag = GLType<T>::is_double;
		return !is_error(m_err_code);
	}

	/*
		_usage = GL_[STATIC|DYNAMIC|STREAM]_[DRAW|READ|COPY]
		page 96 of the "Red Book"
	*/
	template <typename T, glm::precision P>
	bool set_data(
		const std::vector<glm::detail::tvec2<T,P> >& _array
		,GLenum _usage = GL_STATIC_DRAW
		,GLenum _type = GLType<T>::get
		)
	{
		if(_array.empty())
		{
			m_err_code = GL_INVALID_VALUE;
			return false;
		}
		if(!prepare_buffer()) return false;

		data_items = _array.size();
		data_size = _array.size()*2*sizeof(T);
		glBufferData(GL_ARRAY_BUFFER
			,data_size
			,Private::to_pointer(_array)
			,_usage
			);
		vec_item_elements = 2;
		data_element_type = _type;
		int_type_flag = GLType<T>::is_int;
		double_type_flag = GLType<T>::is_double;
		return !is_error(m_err_code);
	}

	/*
		_usage = GL_[STATIC|DYNAMIC|STREAM]_[DRAW|READ|COPY]
		page 96 of the "Red Book"
	*/
	template <typename T, glm::precision P>
	bool set_data(
		const std::vector<glm::detail::tvec3<T,P> >& _array
		,GLenum _usage = GL_STATIC_DRAW
		,GLenum _type = GLType<T>::get
		)
	{
		if(_array.empty())
		{
			m_err_code = GL_INVALID_VALUE;
			return false;
		}
		if(!prepare_buffer()) return false;

		data_items = _array.size();
		data_size = _array.size()*3*sizeof(T);
		glBufferData(GL_ARRAY_BUFFER
			,data_size
			,Private::to_pointer(_array)
			,_usage
			);
		vec_item_elements = 3;
		data_element_type = _type;
		int_type_flag = GLType<T>::is_int;
		double_type_flag = GLType<T>::is_double;
		return !is_error(m_err_code);
	}

	/*
		_usage = GL_[STATIC|DYNAMIC|STREAM]_[DRAW|READ|COPY]
		page 96 of the "Red Book"
	*/
	template <typename T, glm::precision P>
	bool set_data(
		const std::vector<glm::detail::tvec4<T,P> >& _array
		,GLenum _usage = GL_STATIC_DRAW
		,GLenum _type = GLType<T>::get
		)
	{
		if(_array.empty())
		{
			m_err_code = GL_INVALID_VALUE;
			return false;
		}
		if(!prepare_buffer()) return false;

		data_items = _array.size();
		data_size = _array.size()*4*sizeof(T);
		glBufferData(GL_ARRAY_BUFFER
			,data_size
			,Private::to_pointer(_array)
			,_usage
			);
		vec_item_elements = 4;
		data_element_type = _type;
		int_type_flag = GLType<T>::is_int;
		double_type_flag = GLType<T>::is_double;
		return !is_error(m_err_code);
	}

	size_t items() const 
	{
		return data_items;
	}

	size_t item_elements() const
	{
		return vec_item_elements;
	}

	size_t size() const 
	{
		return data_size;
	}

	bool draw(GLenum  _mode)
	{
		GLsizei count = data_items;
		return draw(_mode,0,count);
	}

	bool draw(GLenum  _mode,GLint _first,GLsizei _count)
	{
		glDrawArrays(_mode,_first,_count);
		return !is_error(m_err_code);
	}

	bool assign(
		const AttributeLocation& _location
		,bool _normilized = false
		,GLsizei _stride = 0,GLuint _first_offs = 0
		)
	{
		bind();
		if(int_type_flag==GL_TRUE)
		{
			glVertexAttribIPointer(
				_location()
				,vec_item_elements,data_element_type
				,_stride,BUFFER_OFFSET(_first_offs)
				);
		}
		else if(double_type_flag==GL_TRUE)
		{
			glVertexAttribLPointer(
				_location()
				,vec_item_elements,data_element_type
				,_stride,BUFFER_OFFSET(_first_offs)
				);
		}
		else
		{
			glVertexAttribPointer(
				_location()
				,vec_item_elements,data_element_type
				,ToGLBoolean(_normilized)
				,_stride,BUFFER_OFFSET(_first_offs)
				);
		}
		m_location = _location;
		return !is_error(m_err_code);
	}

	bool enable()
	{
		glBindVertexArray(vertex_array_id);
		if(is_error(m_err_code)) return false;
		glEnableVertexAttribArray(m_location());
		return !is_error(m_err_code);
	}

protected:
	bool bind()
	{
		glBindVertexArray(vertex_array_id);
		if(is_error(m_err_code)) return false;
		glBindBuffer(GL_ARRAY_BUFFER,buffer_id);
		return !is_error(m_err_code);
	}

	bool prepare_buffer()
	{
		if(!glIsVertexArray(vertex_array_id) || !glIsBuffer(buffer_id))
		{
			clear();
			return create();
		}
		else
		{
			return bind();
		}
	}

	friend struct Buffer;
};//struct VertexArray
