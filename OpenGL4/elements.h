#pragma once

#include "utils.h"
#include "simple.h"
#include "error.h"
#include "buffer.h"

template<typename _Type>
struct Elements : protected Buffer
{
protected:
	typedef Buffer base;

protected:
	const GLenum m_gl_type;
	_Type* m_ptr;	
	std::vector<_Type> m_data;

public:
	Elements()
		:m_gl_type(GLType<_Type>::get)
		,m_ptr(NULL)
	{
		GL_STATIC_ASSERT(
			GLType<_Type>::get==GL_UNSIGNED_BYTE
			|| GLType<_Type>::get==GL_UNSIGNED_SHORT
			|| GLType<_Type>::get==GL_UNSIGNED_INT
			,illegal_type_for_Elements_template
			);
		m_target = GL_ELEMENT_ARRAY_BUFFER;
	}

	GLenum get_last_error() const 
	{
		return base::get_last_error();
	}

	bool create()
	{
		return base::create(m_target);
	}

	bool alloc(size_t _size,GLenum _usage = GL_STATIC_DRAW)
	{
		return base::alloc_data(_size,_usage);
	}

	size_t size() const 
	{
		return base::size();
	}

	bool clear(const _Type& _item = _Type())
	{
		return base::clear_data(_item,GL_RED_INTEGER,false);
	}

	/*
	_access = GL_READ_ONLY,GL_WRITE_ONLY,GL_READ_WRITE
	*/
	bool map(GLenum _access)
	{
		m_ptr = reinterpret_cast<_Type*>(base::map_data(_access));
		return test_error(base::get_last_error());
	}

	bool unmap()
	{
		bool ret = base::unmap_data();
		m_ptr = NULL;
		return ret;
	}

	bool set(GLenum _usage = GL_STATIC_DRAW)
	{
		if(m_data.size()<size())
		{
			set_error(GL_INVALID_OPERATION);
			return false;
		}
		return base::set_data(size(),Private::to_pointer(m_data),_usage);
	}

	bool get(GLintptr _offs = 0,GLsizeiptr _size = size())
	{
		if(NOT_NULL(m_ptr))
		{
			set_error(GL_INVALID_OPERATION);
			return false;
		}
		if(_offs<0 || _offs>size()
			|| _size<0 || _offs+_size>size())
		{
			set_error(GL_INVALID_VALUE);
			return false;
		}
		prepare_local_buffer(); 
		if(_offs>m_data.size() || _offs+_size>m_data.size())
		{
			ASSERT(FALSE);
			set_error(GL_INVALID_VALUE);
			return false;
		}
		return base::get_sub_data(_offs,_size,&m_data[_offs]);
	}

	bool set_value(size_t _idx,const _Type& _item)
	{
		prepare_local_buffer();
		_Type* ptr = get_pointer(_idx);
		VERIFY_EXIT1(NOT_NULL(ptr),false);
		*ptr = _item;
		return true;
	}

	/*const _Type& get_value(size_t _idx) const 
	{
		prepare_local_buffer();
		const _Type* ptr = get_pointer(_idx);
		VERIFY_EXIT1(NOT_NULL(ptr),Fish<_Type>::get());
		return *ptr;
	}*/

	_Type& get_value(size_t _idx)
	{
		prepare_local_buffer();
		_Type* ptr = get_pointer(_idx);
		VERIFY_EXIT1(NOT_NULL(ptr),Fish<_Type>::get());
		return *ptr;
	}

	bool bind()
	{
		return base::bind();
	}

	GLenum get_gl_type() const 
	{
		return m_gl_type;
	}

	const GLvoid* get_buffer_offset(size_t _idx) const 
	{
		if(_idx>size()) return BUFFER_OFFSET(0);
		return BUFFER_OFFSET(GLTypeSize<GLType<_Type>::get>::get*_idx);
	}

	bool draw(GLenum _mode,size_t _idx = 0,size_t _count = -1)
	{
		if(!bind()) return false;
		GLsizei cnt = 0;
		const GLvoid* ptr = NULL;
		if(!get_draw_params(_idx,_count,OUT cnt,OUT ptr)) return false;
		glDrawElements(_mode,cnt,m_gl_type,ptr);
		return !is_error(m_err_code);
	}

	bool draw_base_vertex(GLenum _mode,GLint _basevertex,size_t _idx = 0,size_t _count = -1)
	{
		if(!bind()) return false;
		GLsizei cnt = 0;
		const GLvoid* ptr = NULL;
		if(!get_draw_params(_idx,_count,OUT cnt,OUT ptr)) return false;
		glDrawElementsBaseVertex(_mode,cnt,m_gl_type,ptr,_basevertex);
		return !is_error(m_err_code);
	}

	bool draw_range(GLenum _mode,GLuint _start,GLuint _end,size_t _idx = 0,size_t _count = -1)
	{
		if(!bind()) return false;
		GLsizei cnt = 0;
		const GLvoid* ptr = NULL;
		if(!get_draw_params(_idx,_count,OUT cnt,OUT ptr)) return false;
		glDrawRangeElements(_mode,_start,_end,cnt,m_gl_type,ptr);
		return !is_error(m_err_code);
	}

	bool draw_range_base_vertex(GLenum _mode,GLuint _start,GLuint _end,GLint _basevertex,size_t _idx = 0,size_t _count = -1)
	{
		if(!bind()) return false;
		GLsizei cnt = 0;
		const GLvoid* ptr = NULL;
		if(!get_draw_params(_idx,_count,OUT cnt,OUT ptr)) return false;
		glDrawRangeElementsBaseVertex(_mode,_start,_end,cnt,m_gl_type,ptr,_basevertex);
		return !is_error(m_err_code);
	}

protected:
	void prepare_local_buffer()
	{
		if(IS_NULL(m_ptr))
			m_data.resize(size());
	}

	bool get_draw_params(size_t _idx,size_t _count,OUT GLsizei& _cnt,OUT const GLvoid*& _offset)
	{
		if(_idx>size()) 
		{
			set_error(GL_INVALID_VALUE);
			return false;
		}
		size_t cnt = 0;
		if(_count==-1)
		{
			cnt = size() - _idx;
		}
		else
			cnt = _count;
		if(_idx+cnt>size())
		{
			set_error(GL_INVALID_VALUE);
			return false;
		}
		_cnt = (GLsizei)cnt;
		_offset = get_buffer_offset(_idx);
		return true;
	}

	_Type* get_pointer(size_t _idx)
	{
		if(NOT_NULL(m_ptr))
		{
			VERIFY_EXIT1(_idx<size(),NULL);
			return m_ptr + _idx;
		}
		else
		{
			VERIFY_EXIT1(_idx<m_data.size(),NULL);
			return &m_data[_idx];
		}
	}

	const _Type* get_pointer(size_t _idx) const 
	{
		if(NOT_NULL(m_ptr))
		{
			VERIFY_EXIT1(_idx<size(),NULL);
			return m_ptr + _idx;
		}
		else
		{
			VERIFY_EXIT1(_idx<m_data.size(),NULL);
			return &m_data[_idx];
		}
	}
protected:

	template<typename _Type>
	struct IteratorBase 
	{
	protected:
		Elements<_Type>& m_data;
		size_t m_idx;

	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef _Type value_type;
		typedef long difference_type;
		typedef difference_type distance_type;	// retained
		typedef _Type* pointer;
		typedef _Type& reference;

		IteratorBase(Elements<_Type>& _data,size_t _idx)
			:m_data(_data)
			,m_idx(_idx)
		{
		}

		bool eoa() const 
		{
			return (GLsizei)m_idx>=m_data.size();
		}

		reference operator * ()
		{
			return m_data.get_value(m_idx);
		}

		pointer operator -> ()
		{
			return &m_data.get_value(m_idx);
		}

		IteratorBase& operator ++ ()
		{
			if(!eoa()) m_idx++;
			return *this;
		}

		IteratorBase operator ++ (int)
		{
			IteratorBase it(*this);
			operator ++ ();
			return it;
		}

		IteratorBase& operator -- ()
		{
			if(m_idx>0) m_idx--;
			return *this;
		}

		IteratorBase operator -- (int)
		{
			IteratorBase it(*this);
			operator -- ();
			return it;
		}

		IteratorBase& operator += (distance_type _diff)
		{
			long nidx = m_idx+_diff;
			if(nidx<0)
				m_idx = 0;
			else if(nidx>=m_data.size())
				m_idx = m_data.size();
			else
				m_idx = nidx;
			return *this;
		}

		IteratorBase& operator + (distance_type _diff) const
		{
			IteratorBase it(*this);
			it += _diff;
			return it;
		}

		bool operator == (const IteratorBase& _it) const
		{
			if(&m_data!=&(_it.m_data)) return false;
			if(eoa() && _it.eoa()) return true;
			if(eoa() || _it.eoa()) return false;
			return m_idx==_it.m_idx;
		}

		bool operator != (const IteratorBase& _it) const
		{
			return !operator==(_it);
		}

		bool operator < (const IteratorBase& _it) const
		{
			if(m_data!=_it.m_data) return false;
			if(eoa() && _it.eoa()) return true;
			if(eoa() || _it.eoa()) return false;
			return m_idx<_it.m_idx;
		}

		bool operator > (const IteratorBase& _it) const
		{
			return _it.operator < (*this);
		}

	};//struct IteratorBase 
public:
	typedef IteratorBase<_Type> iterator;
	typedef _Type value_type;

public:
	iterator begin()
	{
		return iterator(*this,0);
	}

	iterator end()
	{
		return iterator(*this,size());
	}

	value_type& front()
	{
		VERIFY_EXIT1(size()>0,Fish<value_type>::get());
		return get_value(0);
	}

	const value_type& front() const 
	{
		VERIFY_EXIT1(size()>0,Fish<value_type>::get());
		return get_value(0);
	}

	value_type& back()
	{
		VERIFY_EXIT1(size()>0,Fish<value_type>::get());
		return get_value(size()-1);
	}

	const value_type& back() const 
	{
		VERIFY_EXIT1(size()>0,Fish<value_type>::get());
		return get_value(size()-1);
	}

	_Type& operator[] (size_t _idx)
	{
		return get_value(_idx);
	}	

	bool empty() const 
	{
		return size()==0;
	}
};//template<> struct Elements