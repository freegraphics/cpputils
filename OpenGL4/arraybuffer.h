#pragma once

#include "utils.h"
#include "simple.h"
#include "types.h"
#include "buffer.h"
#include "location.h"

struct ArrayBuffer;

namespace detail
{
	struct ArraySubBuffer;
	struct ArraySubBuffer;

	struct ArrayDataBase
	{
	protected:
		GLsizei m_item_elements;
		GLsizei m_item_columns;
		GLsizei m_item_count;
		GLenum m_element_type;
		GLsizei m_stride; 
		GLsizei m_offset;

		Buffer* m_pbuffer;
		ArraySubBuffer* m_psubbuffer;

		GLintptr m_sub_offset;
		GLsizeiptr m_size;

		AttributeLocation m_location;
		GLenum m_err_code;

	public:
		ArrayDataBase()
			:m_pbuffer(0)
			,m_sub_offset(0)
			,m_size(0)
			,m_stride(0)
			,m_offset(0)
			,m_item_elements(0)
			,m_item_columns(0)
			,m_item_count(0)
			,m_element_type(GL_FLOAT)
			,m_psubbuffer(0)
			,m_err_code(GL_NO_ERROR)
		{
		}

		GLintptr get_sub_offset() const 
		{
			return m_sub_offset;
		}

		GLsizei get_offset() const 
		{
			return m_offset;
		}

		GLsizei get_item_count() const 
		{
			return m_item_count;
		}

		GLsizei get_item_size() const 
		{
			return 
				get_gl_type_size(m_element_type)
				* get_item_elements()
				;
		}

		GLsizei get_stride() const 
		{
			return m_stride;
		}

		GLsizei get_element_size() const 
		{
			return get_gl_type_size(m_element_type);
		}

		GLenum get_element_gl_type() const 
		{
			return m_element_type;
		}

		GLsizei get_item_elements() const 
		{
			return m_item_elements;
		}

		GLsizei get_item_columns() const 
		{
			return m_item_columns;
		}

		GLsizeiptr get_size() const
		{
			return m_size;
		}

		ArraySubBuffer* get_array_sub_buffer()
		{
			return m_psubbuffer;
		}

		Buffer& get_buffer()
		{
			VERIFY_EXIT1(NOT_NULL(m_pbuffer),Fish<Buffer>::get());
			return *m_pbuffer;
		}

		const Buffer& get_buffer() const
		{
			VERIFY_EXIT1(NOT_NULL(m_pbuffer),Fish<Buffer>::get());
			return *m_pbuffer;
		}

		bool is_already_created() const 
		{
			return NOT_NULL(m_pbuffer);
		}

		LPBYTE get_pointer(size_t _idx);

	protected:
		void prepare();

		void set_offset(GLsizei _offs)
		{
			m_offset = _offs;
		}

		void set_sub_offset(GLintptr _offs)
		{
			m_sub_offset = _offs;
		}

		void set_value(Buffer* _pbuffer,GLsizei _stride,GLsizeiptr _size,ArraySubBuffer* _psubbuffer)
		{
			m_pbuffer = _pbuffer;
			m_stride = _stride;
			m_size = _size;
			m_psubbuffer = _psubbuffer;
			//create();
		}

		friend struct ArrayBuffer;
		friend struct ArraySubBuffer;;
	};//struct DataArrayBase

	struct ArraySubBuffer
	{
		std::vector<byte> data;
		std::vector<ArrayDataBase*> arrays;
		GLsizei stride;
		GLsizei item_count;
		bool tightly_packed;
		LPVOID ptr;

		ArraySubBuffer()
			:stride(0)
			,item_count(0)
			,ptr(NULL)
			,tightly_packed(true)
		{
		}

		void prepare()
		{
			if(NOT_NULL(ptr)) return;
			data.resize(stride*item_count);
		}

		LPBYTE get_pointer(size_t _idx,GLintptr _offset,GLsizei _element_size)
		{
			int buff_size = stride*item_count;
			int buff_offset = _offset + (int)_idx*stride;
			VERIFY_EXIT1(
				(buff_offset < buff_size)
				&& (buff_offset + _element_size <= buff_size)
				,NULL
				);
			if(NOT_NULL(ptr))
				return (LPBYTE)ptr + buff_offset;
			else
				return &(data[buff_offset]);
		}

		void set_sub_offset(GLintptr _offs)
		{
			std::vector<ArrayDataBase*>::iterator 
				it = arrays.begin()
				,ite = arrays.end()
				;
			for(;it!=ite;++it)
			{
				ArrayDataBase* padb = *it;
				padb->set_sub_offset(_offs);
			}
		}
	};//struct VertexSubBuffer

	inline
	LPBYTE ArrayDataBase::get_pointer(size_t _idx)
	{
		VERIFY_EXIT1(NOT_NULL(m_psubbuffer),NULL);
		return m_psubbuffer->get_pointer(_idx,get_offset(),get_item_size());
	}

	inline
	void ArrayDataBase::prepare()
	{
		VERIFY_EXIT(NOT_NULL(m_psubbuffer));
		return m_psubbuffer->prepare();
	}
};//namespace detail

struct ArrayBuffer : protected Buffer
{
protected:
	typedef Buffer base;
	GLuint m_vertex_array_id;

protected:
	std::list<detail::ArraySubBuffer> sub_buffers;

public:
	ArrayBuffer()
		:m_vertex_array_id(0)
	{
	}

	~ArrayBuffer()
	{
		clear();
	}

	void clear()
	{
		if(is_created())
		{
			stop_using_vertex_array();
			glDeleteVertexArrays(1,&m_vertex_array_id);
		}
		m_vertex_array_id = 0;
	}

	bool is_created() const 
	{
		return EQL(GL_TRUE,glIsVertexArray(m_vertex_array_id));
	}

	bool create()
	{
		if(!base::is_created())
		{
			clear();
			if(!Buffer::create(GL_ARRAY_BUFFER)) return false;
		}
		glGenVertexArrays(1,&m_vertex_array_id);
		if(is_error(m_err_code)) return false;
		glBindVertexArray(m_vertex_array_id);
		return !is_error(m_err_code);
	}

	bool add(detail::ArrayDataBase* _parray1)
	{
		return addv(_parray1,NULL);
	}

	bool add(detail::ArrayDataBase* _parray1,detail::ArrayDataBase* _parray2)
	{
		return addv(_parray1,_parray2,NULL);
	}

	bool add(detail::ArrayDataBase* _parray1,detail::ArrayDataBase* _parray2,detail::ArrayDataBase* _parray3)
	{
		return addv(_parray1,_parray2,_parray3,NULL);
	}

	bool add(detail::ArrayDataBase* _parray1,detail::ArrayDataBase* _parray2,detail::ArrayDataBase* _parray3
		,detail::ArrayDataBase* _parray4
		)
	{
		return addv(_parray1,_parray2,_parray3,_parray4,NULL);
	}

	bool add(detail::ArrayDataBase* _parray1,detail::ArrayDataBase* _parray2,detail::ArrayDataBase* _parray3
		,detail::ArrayDataBase* _parray4,detail::ArrayDataBase* _parray5
		)
	{
		return addv(_parray1,_parray2,_parray3,_parray4,_parray5,NULL);
	}

	bool add(detail::ArrayDataBase* _parray1,detail::ArrayDataBase* _parray2,detail::ArrayDataBase* _parray3
		,detail::ArrayDataBase* _parray4,detail::ArrayDataBase* _parray5,detail::ArrayDataBase* _parray6
		)
	{
		return addv(_parray1,_parray2,_parray3,_parray4,_parray5,_parray6,NULL);
	}

	bool add(detail::ArrayDataBase* _parray1,detail::ArrayDataBase* _parray2,detail::ArrayDataBase* _parray3
		,detail::ArrayDataBase* _parray4,detail::ArrayDataBase* _parray5,detail::ArrayDataBase* _parray6
		,detail::ArrayDataBase* _parray7
		)
	{
		return addv(_parray1,_parray2,_parray3,_parray4,_parray5,_parray6,_parray7,NULL);
	}

	bool add(detail::ArrayDataBase* _parray1,detail::ArrayDataBase* _parray2,detail::ArrayDataBase* _parray3
		,detail::ArrayDataBase* _parray4,detail::ArrayDataBase* _parray5,detail::ArrayDataBase* _parray6
		,detail::ArrayDataBase* _parray7,detail::ArrayDataBase* _parray8
		)
	{
		return addv(_parray1,_parray2,_parray3,_parray4,_parray5,_parray6,_parray7,_parray8,NULL);
	}

	bool add(detail::ArrayDataBase* _parray1,detail::ArrayDataBase* _parray2,detail::ArrayDataBase* _parray3
		,detail::ArrayDataBase* _parray4,detail::ArrayDataBase* _parray5,detail::ArrayDataBase* _parray6
		,detail::ArrayDataBase* _parray7,detail::ArrayDataBase* _parray8,detail::ArrayDataBase* _parray9
		)
	{
		return addv(_parray1,_parray2,_parray3,_parray4,_parray5,_parray6,_parray7,_parray8
			,_parray9,NULL
			);
	}

	bool add(detail::ArrayDataBase* _parray1,detail::ArrayDataBase* _parray2,detail::ArrayDataBase* _parray3
		,detail::ArrayDataBase* _parray4,detail::ArrayDataBase* _parray5,detail::ArrayDataBase* _parray6
		,detail::ArrayDataBase* _parray7,detail::ArrayDataBase* _parray8,detail::ArrayDataBase* _parray9
		,detail::ArrayDataBase* _parray10
		)
	{
		return addv(_parray1,_parray2,_parray3,_parray4,_parray5,_parray6,_parray7,_parray8
			,_parray9,_parray10,NULL
			);
	}

	bool addv(detail::ArrayDataBase* _parray, ...)
	{
		detail::ArraySubBuffer sub_buffer;

		VERIFY_EXIT1(NOT_NULL(_parray),false);
		sub_buffer.arrays.push_back(_parray);

		va_list args;
		va_start(args,_parray);
		detail::ArrayDataBase* parray = NULL;
		for(;NOT_NULL(parray = va_arg(args,detail::ArrayDataBase*));)
		{
			sub_buffer.arrays.push_back(parray);
		}
		va_end(args);

		sub_buffers.push_back(sub_buffer);
		detail::ArraySubBuffer& inserted_sub_buffer = sub_buffers.back();

		GLsizei item_count = inserted_sub_buffer.arrays.front()->get_item_count();
		size_t i = 0;
		GLsizei stride = inserted_sub_buffer.arrays[0]->get_item_size();
		inserted_sub_buffer.arrays[i]->set_offset(0);
		inserted_sub_buffer.tightly_packed = true;
		for(i=1;i<inserted_sub_buffer.arrays.size();i++)
		{
			if(NEQL(item_count,inserted_sub_buffer.arrays[i]->get_item_count())) 
			{
				sub_buffers.erase(--sub_buffers.end());
				return false;
			}

			inserted_sub_buffer.arrays[i]->set_offset(stride);
			stride += inserted_sub_buffer.arrays[i]->get_item_size();

			inserted_sub_buffer.tightly_packed = false;
		}

		inserted_sub_buffer.stride = stride;
		inserted_sub_buffer.item_count = item_count;
		for(i=0;i<inserted_sub_buffer.arrays.size();i++)
		{
			inserted_sub_buffer.arrays[i]->set_value(this,stride,stride*item_count,&sub_buffers.back());
		}

		return true;
	}

	bool alloc(GLenum _usage = GL_STATIC_DRAW)
	{
		if(!prepare()) return false;
		GLsizei size = 0;
		std::list<detail::ArraySubBuffer>::iterator
			it = sub_buffers.begin()
			,ite = sub_buffers.end()
			;
		for(;it!=ite;++it)
		{
			detail::ArraySubBuffer& sub_buffer = *it;
			sub_buffer.set_sub_offset(size);
			size += sub_buffer.item_count*sub_buffer.stride;
		}
		return base::alloc_data(size,_usage);
	}

	bool set(detail::ArrayDataBase* _parray)
	{
		if(IS_NULL(_parray) || IS_NULL(_parray->get_array_sub_buffer()))
		{
			m_err_code = GL_INVALID_VALUE;
			return false;
		}
		return base::set_sub_data(
			_parray->get_sub_offset(),_parray->get_size()
			,Private::to_pointer(_parray->get_array_sub_buffer()->data)
			);
	}

	bool map(detail::ArrayDataBase* _parray,bool _flush_explicit = false,bool _synchronize = true)
	{
		if(IS_NULL(_parray) || IS_NULL(_parray->get_array_sub_buffer()))
		{
			m_err_code = GL_INVALID_VALUE;
			return false;
		}
		GLvoid* ptr = base::map_data_range(
			_parray->get_sub_offset()
			,_parray->get_size()
			,GL_MAP_WRITE_BIT
				|(_flush_explicit?GL_MAP_FLUSH_EXPLICIT_BIT:0)
				|(_synchronize?0:GL_MAP_UNSYNCHRONIZED_BIT)
			);
		if(base::get_last_error()!=GL_NO_ERROR) return false;
		detail::ArraySubBuffer* p_sub_buffer = _parray->get_array_sub_buffer();
		p_sub_buffer->ptr = ptr;
		return true;
	}

	bool unmap()
	{
		return EQL(base::unmap_data(),GL_TRUE);
	}

	bool invalidate()
	{
		return base::invalidate_data();
	}

	Buffer& get_buffer()
	{
		return *this;
	}

	const Buffer& get_buffer() const
	{
		return *this;
	}

	bool bind_vertex_array()
	{
		glBindVertexArray(m_vertex_array_id);
		return !is_error(m_err_code);
	}

	bool bind_buffer()
	{
		return base::bind();
	}

	bool bind()
	{
		if(!bind_buffer()) return false;
		return bind_vertex_array();
	}

protected:
	bool prepare()
	{
		if(!is_created()) 
		{
			clear();
			return create();
		}
		else
		{
			if(!bind_buffer()) return false;
			return bind_vertex_array();
		}
	}
};//struct ArrayBuffer

template<typename _Type>
struct ArrayData : public detail::ArrayDataBase
{
protected:
	typedef detail::ArrayDataBase base;

public:
	ArrayData(GLsizei _size = 0)
	{
		m_element_type = GLMType<_Type>::get_element_gl_type;
		m_item_elements = GLMType<_Type>::get_elements_count;
		m_item_columns = GLMType<_Type>::get_columns; // to support matrix
		VERIFY(create(_size));
	}

	bool create(GLsizei _size)
	{
		if(is_already_created()) return false;
		m_item_count = _size;
		return true;
	}

	bool set_value(size_t _idx,const _Type& _value)
	{
		if(!is_already_created()) return false;
		base::prepare();
		LPBYTE ptr = base::get_pointer(_idx);
		VERIFY_EXIT1(NOT_NULL(ptr),false);
		*reinterpret_cast<_Type*>(ptr) = _value;
		return true;
	}

	_Type& get_value(size_t _idx)
	{
		if(!is_already_created()) return Fish<_Type>::get();
		base::prepare();
		LPBYTE ptr = base::get_pointer(_idx);
		VERIFY_EXIT1(NOT_NULL(ptr),Fish<_Type>::get());
		return *reinterpret_cast<_Type*>(ptr);
	}


	/*
		_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED
			,GL_GREEN,GL_BLUE,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER
			,GL_GREEN_INTEGER,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER
			,GL_RGBA_INTEGER,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type1,glm::precision _Precision>
	bool clear(const glm::detail::tvec1<_Type1,_Precision>& _value,GLenum _format,bool _normilize = false)
	{
		if(!is_already_created()) return false;
		base::get_buffer().clear_sub_data(_value,get_sub_offset(),get_size(),_format,_normilize);
		return true;
	}

	/*
		_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED
			,GL_GREEN,GL_BLUE,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER
			,GL_GREEN_INTEGER,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER
			,GL_RGBA_INTEGER,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type1,glm::precision _Precision>
	bool clear(const glm::detail::tvec2<_Type1,_Precision>& _value,GLenum _format,bool _normilize = false)
	{
		if(!is_already_created()) return false;
		base::get_buffer().clear_sub_data(_value,get_sub_offset(),get_size(),_format,_normilize);
		return true;
	}

	/*
		_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED
			,GL_GREEN,GL_BLUE,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER
			,GL_GREEN_INTEGER,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER
			,GL_RGBA_INTEGER,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type1,glm::precision _Precision>
	bool clear(const glm::detail::tvec3<_Type1,_Precision>& _value,GLenum _format,bool _normilize = false)
	{
		if(!is_already_created()) return false;
		base::get_buffer().clear_sub_data(_value,get_sub_offset(),get_size(),_format,_normilize);
		return true;
	}

	/*
		_format = GL_STENCIL_INDEX,GL_DEPTH_COMPONENT,GL_DEPTH_STENCIL,GL_RED
			,GL_GREEN,GL_BLUE,GL_RG,GL_RGB,GL_RGBA,GL_BGR,GL_BGRA,GL_RED_INTEGER
			,GL_GREEN_INTEGER,GL_BLUE_INTEGER,GL_RG_INTEGER,GL_RGB_INTEGER
			,GL_RGBA_INTEGER,GL_BGR_INTEGER,GL_BGRA_INTEGER
	*/
	template<typename _Type1,glm::precision _Precision>
	bool clear(const glm::detail::tvec4<_Type1,_Precision>& _value,GLenum _format,bool _normilize = false)
	{
		if(!is_already_created()) return false;
		base::get_buffer().clear_sub_data(_value,get_sub_offset(),get_size(),_format,_normilize);
		return true;
	}

	bool flush()
	{
		if(!is_already_created()) return false;
		base::get_buffer().flush_mapped_data_range(get_sub_offset(),get_size());
		return true;
	}

	bool flush(size_t _idx,size_t _cnt)
	{
		if(!is_already_created()) return false;
		base::get_buffer().flush_mapped_data_range(
			get_sub_offset() + (int)(_idx*get_stride())
			,(int)(_cnt*get_stride())
			);
		return true;
	}

	bool invalidate()
	{
		if(!is_already_created()) return false;
		base::get_buffer().invalidate_sub_data(get_sub_offset(),get_size());
		return true;
	}

	bool assign(
		const AttributeLocation& _location
		,bool _normilized = false
		)
	{
		GL_STATIC_ASSERT(
			GLMType<_Type>::get_element_gl_type==GL_BYTE
			|| GLMType<_Type>::get_element_gl_type==GL_UNSIGNED_BYTE
			|| GLMType<_Type>::get_element_gl_type==GL_SHORT
			|| GLMType<_Type>::get_element_gl_type==GL_UNSIGNED_SHORT
			|| GLMType<_Type>::get_element_gl_type==GL_INT
			|| GLMType<_Type>::get_element_gl_type==GL_UNSIGNED_INT
			|| GLMType<_Type>::get_element_gl_type==GL_FLOAT
			|| GLMType<_Type>::get_element_gl_type==GL_DOUBLE
			,invalid_ArrayData_template_type_for_assign_function
			);
		if(!is_already_created()) 
		{
			if(!is_error(m_err_code))
				m_err_code = GL_INVALID_OPERATION;
			test_error(m_err_code);
			return false;
		}
		if(!get_buffer().bind_vertex_array()) 
		{
			if(!is_error(m_err_code))
				m_err_code = GL_INVALID_OPERATION;
			test_error(m_err_code);
			return false;
		}
		if(IS_NULL(get_array_sub_buffer()))
		{
			m_err_code = GL_INVALID_OPERATION;
			test_error(m_err_code);
			return false;
		}
		size_t i = 0;
		for(i=0;(GLsizei)i<get_item_columns();i++)
		{
			glVertexAttribPointer(
				_location()+(GLint)i
				,get_item_elements()/get_item_columns(),get_element_gl_type()
				,ToGLBoolean(_normilized)
				,get_array_sub_buffer()->tightly_packed?0:get_stride()
				,BUFFER_OFFSET(
					get_sub_offset()
					+ get_offset()
					+ i*get_item_elements()/get_item_columns()*get_element_size()
					)
				);
		}
		m_location = _location;
		return !is_error(m_err_code);
	}

	bool assign_integer(
		const AttributeLocation& _location
		)
	{
		GL_STATIC_ASSERT(
			GLMType<_Type>::get_element_gl_type==GL_BYTE
			|| GLMType<_Type>::get_element_gl_type==GL_UNSIGNED_BYTE
			|| GLMType<_Type>::get_element_gl_type==GL_SHORT
			|| GLMType<_Type>::get_element_gl_type==GL_UNSIGNED_SHORT
			|| GLMType<_Type>::get_element_gl_type==GL_INT
			|| GLMType<_Type>::get_element_gl_type==GL_UNSIGNED_INT
			,invalid_ArrayData_template_type_for_assign_function
			);
		if(!is_already_created()) 
		{
			if(!is_error(m_err_code))
				m_err_code = GL_INVALID_OPERATION;
			test_error(m_err_code);
			return false;
		}
		if(!get_buffer().bind_vertex_array()) 
		{
			if(!is_error(m_err_code))
				m_err_code = GL_INVALID_OPERATION;
			test_error(m_err_code);
			return false;
		}
		if(IS_NULL(get_array_sub_buffer()))
		{
			m_err_code = GL_INVALID_OPERATION;
			return false;
		}
		glVertexAttribIPointer(
			_location()
			,get_item_elements(),get_element_gl_type()
			,get_array_sub_buffer()->tightly_packed?0:get_stride()
			,BUFFER_OFFSET(get_sub_offset()+get_offset())
			);
		m_location = _location;
		return !is_error(m_err_code);
	}

	bool assign_double(
		const AttributeLocation& _location
		)
	{
		GL_STATIC_ASSERT(
			GLMType<_Type>::get_element_gl_type==GL_DOUBLE
			,invalid_ArrayData_template_type_for_assign_function
			);
		if(!is_already_created()) 
		{
			if(!is_error(m_err_code))
				m_err_code = GL_INVALID_OPERATION;
			test_error(m_err_code);
			return false;
		}
		if(!get_buffer().bind_vertex_array()) 
		{
			if(!is_error(m_err_code))
				m_err_code = GL_INVALID_OPERATION;
			test_error(m_err_code);
			return false;
		}
		if(IS_NULL(get_array_sub_buffer()))
		{
			m_err_code = GL_INVALID_OPERATION;
			return false;
		}
		size_t i = 0;
		for(i=0;i<get_item_columns();i++)
		{
			glVertexAttribLPointer(
				_location()+(GLint)i
				,get_item_elements()/get_item_columns(),get_element_gl_type()
				,get_array_sub_buffer()->tightly_packed?0:get_stride()
				,BUFFER_OFFSET(
					get_sub_offset()
					+ get_offset()
					+ i*get_item_elements()/get_item_columns()*get_element_size()
					)
				);
		}
		m_location = _location;
		return !is_error(m_err_code);
	}

	bool enable()
	{
		if(!is_already_created()) return false;
		if(!get_buffer().bind_vertex_array()) 
		{
			if(!is_error(m_err_code))
				m_err_code = GL_INVALID_OPERATION;
			test_error(m_err_code);
			return false;
		}
		size_t i = 0;
		for(i=0;(GLsizei)i<get_item_columns();i++)
		{
			glEnableVertexAttribArray(m_location()+(GLint)i);
		}
		return !is_error(m_err_code);
	}

	ArrayBuffer& get_buffer() 
	{
		return static_cast<ArrayBuffer&>(base::get_buffer());
	}

	const ArrayBuffer& get_buffer() const 
	{
		return static_cast<const ArrayBuffer&>(base::get_buffer());
	}

protected:
	template<typename _Type>
	struct IteratorBase 
	{
	protected:
		ArrayData<_Type>& m_data;
		size_t m_idx;

	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef _Type value_type;
		typedef long difference_type;
		typedef difference_type distance_type;	// retained
		typedef _Type* pointer;
		typedef _Type& reference;

		IteratorBase(ArrayData<_Type>& _data,size_t _idx)
			:m_data(_data)
			,m_idx(_idx)
		{
		}

		bool eoa() const 
		{
			return (GLsizei)m_idx>=m_data.get_item_count();
		}

		reference operator * ()
		{
			return m_data.get_value(m_idx);
		}

		pointer operator -> ()
		{
			return &(m_data.get_value(m_idx));
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
			else if(nidx>=m_data.get_item_count())
				m_idx = m_data.get_item_count();
			else
				m_idx = nidx;
			return *this;
		}

		distance_type operator - (IteratorBase _it) const 
		{
			if(&m_data!=&(_it.m_data)) return 0;
			return m_idx - _it.m_idx;
		}

		IteratorBase operator + (distance_type _diff) const
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
		return iterator(*this,get_item_count());
	}

	value_type& front()
	{
		VERIFY_EXIT1(get_item_count()>0,Fish<value_type>::get());
		return get_value(0);
	}

	value_type& back()
	{
		VERIFY_EXIT1(get_item_count()>0,Fish<value_type>::get());
		return get_value(get_item_count()-1);
	}

	_Type& operator[] (size_t _idx)
	{
		return get_value(_idx);
	}	

	size_t size() const 
	{
		return get_item_count();
	}

	bool empty() const 
	{
		return get_item_count()==0;
	}

	bool resize(size_t _sz)
	{
		return create((GLsizei)_sz);
	}

public:
	static ArrayData<_Type> Null()
	{
		return ArrayData<_Type>(0);
	}
};//template<> struct ArrayData

