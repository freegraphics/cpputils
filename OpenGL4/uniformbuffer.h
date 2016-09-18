#pragma once

#include "utils.h"
#include <utils/strconv.h>
#include "glprogram.h"

namespace detail
{
	template<class _DataStruct>
	interface IUniformBlockLayout
	{
		virtual void get_names(std::vector<const GLchar*>& _names) const = 0;
		virtual void get_ptrs(const _DataStruct& _data,std::vector<const void*>& _ptrs) const = 0;
		virtual void get_sizes(const _DataStruct& _data,std::vector<size_t>& _sizes) const = 0;
	};
};//namespace detail

template<class _DataStruct>
struct UniformBlockLayoutBase : public detail::IUniformBlockLayout<_DataStruct>
{
protected:
	interface IItemInfo
	{
		virtual ~IItemInfo() {}
		virtual const GLchar* get_name() const = 0;
		virtual const void* get_pointer(const _DataStruct& _data) const = 0;
		virtual size_t get_size(const _DataStruct& _data) const = 0;
	};

	template<typename _VarType>
	struct ItemInfo : public IItemInfo
	{
		std::auto_ptr<GLchar> m_name;
		_VarType _DataStruct::* m_member_ptr; 

		ItemInfo(const CString& _name,_VarType _DataStruct::* _member_ptr)
			:m_member_ptr(_member_ptr)
		{
			string_converter<TCHAR,GLchar> name(_name,CP_ACP);
			m_name = std::auto_ptr<GLchar>(new GLchar[name.get_length()+1]);
			size_t i = 0;
			for(i=0;i<(size_t)name.get_length();i++)
			{
				m_name.get()[i] = ((GLchar*)name)[i];
			}
			m_name.get()[i] = 0;
		}

		virtual const GLchar* get_name() const
		{
			return m_name.get();
		}

		virtual const void* get_pointer(const _DataStruct& _data) const
		{
			return Private::to_pointer(_data.*m_member_ptr);
		}

		virtual size_t get_size(const _DataStruct& _data) const
		{
			return Private::get_size(_data.*m_member_ptr);
		}
	};

protected:
	ptrvector<IItemInfo> items;

public:
	UniformBlockLayoutBase()
	{
	}

	template<typename _VarType>
	void add(const CString& _name,_VarType _DataStruct::* _member_ptr)
	{
		items.push_back(new ItemInfo<_VarType>(_name,_member_ptr));
	}

	virtual void get_names(std::vector<const GLchar*>& _names) const
	{
		ptrvector<IItemInfo>::const_iterator
			it = items.begin()
			,ite = items.end()
			;
		for(;it!=ite;++it)
		{
			const IItemInfo* ii = *it;
			_names.push_back(ii->get_name());
		}
	}

	virtual void get_ptrs(const _DataStruct& _data,std::vector<const void*>& _ptrs) const 
	{
		ptrvector<IItemInfo>::const_iterator
			it = items.begin()
			,ite = items.end()
			;
		for(;it!=ite;++it)
		{
			const IItemInfo* ii = *it;
			_ptrs.push_back(ii->get_pointer(_data));
		}
	}

	virtual void get_sizes(const _DataStruct& _data,std::vector<size_t>& _sizes) const
	{
		ptrvector<IItemInfo>::const_iterator
			it = items.begin()
			,ite = items.end()
			;
		for(;it!=ite;++it)
		{
			const IItemInfo* ii = *it;
			_sizes.push_back(ii->get_size(_data));
		}
	}

};// template<> struct UniformBlockLayoutBase

namespace Private
{
	template<typename _UniformBlockStruct> inline
	const detail::IUniformBlockLayout<_UniformBlockStruct>& get_uniform_block_layout()
	{
		typedef typename _UniformBlockStruct::UniformBlockLayout DataUniformBlockLayout; 
		static DataUniformBlockLayout _;
		return _;
	}

	template<class _UniformBlockStruct> inline
	bool save_data_to_buffer(
		const _UniformBlockStruct& _data
		,std::vector<byte>& _buf
		,const UniformBlockIndex& _block_index
		)
	{
		VERIFY_EXIT1(!_block_index.is_error_index(),false);
		const detail::IUniformBlockLayout<_UniformBlockStruct>& layout = get_uniform_block_layout<_UniformBlockStruct>();
		std::vector<const GLchar*> names;
		std::vector<const void*> ptrs;
		std::vector<size_t> sizes;
		layout.get_names(names);
		layout.get_ptrs(_data,ptrs);
		layout.get_sizes(_data,sizes);
		return save_data_to_buffer(names,ptrs,sizes,_buf,_block_index);
	}

	inline
	bool save_data_to_buffer(
		const std::vector<const GLchar*>& _names
		,const std::vector<const void*>& _ptrs
		,const std::vector<size_t>& _sizes
		,std::vector<byte>& _buf
		,const UniformBlockIndex& _block_index
		)
	{
		VERIFY_EXIT1(!_block_index.is_error_index(),false);
		_buf.resize(_block_index.size());
		std::fill(_buf.begin(),_buf.end(),0);
		VERIFY_EXIT1(_names.size()==_ptrs.size(),false);
		VERIFY_EXIT1(_names.size()==_sizes.size(),false);
		std::vector<GLuint> indices;
		std::vector<GLint> size;
		std::vector<GLint> offset;
		std::vector<GLint> type;
		indices.resize(_names.size());
		size.resize(_names.size());
		offset.resize(_names.size());
		type.resize(_names.size());
		glGetUniformIndices(_block_index.program(),_names.size(),&_names[0],&indices[0]);
		GLenum err_code = GL_NO_ERROR;
		if(is_error(err_code)) return false;
		glGetActiveUniformsiv(_block_index.program(),_names.size(),&indices[0]
			,GL_UNIFORM_OFFSET, &offset[0]
			);
		if(is_error(err_code)) return false;
		glGetActiveUniformsiv(_block_index.program(),_names.size(),&indices[0]
			,GL_UNIFORM_SIZE, &size[0]
			);
		if(is_error(err_code)) return false;
		glGetActiveUniformsiv(_block_index.program(),_names.size(),&indices[0]
			,GL_UNIFORM_TYPE, &type[0]
			);
		if(is_error(err_code)) return false;
		size_t i = 0;
		for(i=0;i<_names.size();i++)
		{
			size_t sz = size[i]*get_gl_type_size(type[i]);
			if(sz!=_sizes[i]) return false;
			if(offset[i]>=(int)_buf.size() || offset[i]+sz>=_buf.size()) return false;
			memcpy(&_buf[offset[i]],_ptrs[i],sz);
		}
		return true;
	}
};//namespace Private

struct Buffer;

struct UniformBuffer
{
protected:
	GLuint m_uniform_buffer_id;
	GLenum m_err_code;
	size_t m_size;

public:
	UniformBuffer()
		:m_uniform_buffer_id(0)
		,m_err_code(GL_NO_ERROR)
		,m_size(0)
	{
	}

	~UniformBuffer()
	{
		clear();
	}

	GLenum get_last_error() const {return m_err_code;}

	bool create()
	{
		clear();
		glGenBuffers(1,&m_uniform_buffer_id);
		if(is_error(m_err_code)) return false;
		return bind();
	}

	void clear()
	{
		if(glIsBuffer(m_uniform_buffer_id))
		{
			stop_using_buffer();
			glDeleteBuffers(1,&m_uniform_buffer_id);
		}
		m_uniform_buffer_id = 0;
		m_size = 0;
	}

	template<typename _UniformBlockStruct>
	bool set_data(
		IN GLProgram& _program
		,IN const CString& _block_name,IN const _UniformBlockStruct& _data
		,IN GLenum _usage = GL_STATIC_DRAW
		)
	{
		if(!prepare_buffer()) return false;
		std::vector<byte> buf;
		UniformBlockIndex uniform_block = _program.get_uniform_block_index(_block_name);
		if(uniform_block.is_error_index()) return false;
		if(!Private::save_data_to_buffer(_data,buf,uniform_block))
		{
			if(!is_error(m_err_code)) 
				m_err_code = GL_INVALID_VALUE;
			test_error(m_err_code);
			return false;
		}
		glBufferData(GL_UNIFORM_BUFFER,uniform_block.size(),&buf[0],_usage);
		if(is_error(m_err_code)) return false;
		m_size = uniform_block.size();
		glBindBufferBase(GL_UNIFORM_BUFFER,uniform_block.index(),m_uniform_buffer_id);
		return !is_error(m_err_code);
	}

	bool bind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER,m_uniform_buffer_id);
		return !is_error(m_err_code);
	}	

protected:
	bool prepare_buffer()
	{
		if(!glIsBuffer(m_uniform_buffer_id))
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
};//struct UniformBuffer
