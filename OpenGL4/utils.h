#pragma once

#include <glm/glm.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include <gl/glew.h>
#include <gl/wglew.h>


#if !defined(GLEW_STATIC)
#pragma comment(lib, "glew32.lib")
#endif

#pragma comment(lib, "opengl32.lib")



#define BUFFER_OFFSET(x)  (reinterpret_cast<const GLvoid*>((x)))

#define GL_STATIC_ASSERT(_expr,_msg)						\
{															\
	struct ERROR_##_msg {};									\
	ERROR_##_msg d1;										\
	(void)sizeof(CompileTimeChecker<(_expr)!=0>( d1 ) );	\
}															\
/***/

inline
GLboolean ToGLBoolean(bool _value)
{
	return _value?GL_TRUE:GL_FALSE;
}

namespace Private
{
	template<typename _Type> inline
	const _Type* to_pointer(const _Type& _data)
	{
		return &_data;
	}

	template<typename _Type> inline
	_Type* to_pointer(_Type& _data)
	{
		return &_data;
	}

	template<typename _Type> inline
		const _Type* to_pointer(const std::vector<_Type>& _data)
	{
		return &_data[0];
	}

	template<typename _Type> inline
	_Type* to_pointer(std::vector<_Type>& _data)
	{
		return &_data[0];
	}

	template<typename _Type> inline
	size_t get_size(const _Type& _data)
	{
		return sizeof(_Type);
	}

	template<typename _Type> inline
	size_t get_size(const std::vector<_Type>& _data)
	{
		return sizeof(_Type)*_data.size();
	}

	//////////////////////////////////////////////////////////////////////////
	// vec

	template <typename T, glm::precision P>
	inline const T* to_pointer(const glm::detail::tvec1<T,P>& _data)
	{
		return glm::value_ptr(_data);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const glm::detail::tvec2<T,P>& _data)
	{
		return glm::value_ptr(_data);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const glm::detail::tvec3<T,P>& _data)
	{
		return glm::value_ptr(_data);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const glm::detail::tvec4<T,P>& _data)
	{
		return glm::value_ptr(_data);
	}

	// vec
	template <typename T, glm::precision P>
	inline T* to_pointer(glm::detail::tvec1<T,P>& _data)
	{
		return glm::value_ptr(_data);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(glm::detail::tvec2<T,P>& _data)
	{
		return glm::value_ptr(_data);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(glm::detail::tvec3<T,P>& _data)
	{
		return glm::value_ptr(_data);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(glm::detail::tvec4<T,P>& _data)
	{
		return glm::value_ptr(_data);
	}

	//////////////////////////////////////////////////////////////////////////
	// vector vec
	template <typename T, glm::precision P>
	inline const T* to_pointer(const std::vector<glm::detail::tvec1<T,P> >& _data)
	{
		return glm::value_ptr(_data[0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const std::vector<glm::detail::tvec2<T,P> >& _data)
	{
		return glm::value_ptr(_data[0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const std::vector<glm::detail::tvec3<T,P> >& _data)
	{
		return glm::value_ptr(_data[0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const std::vector<glm::detail::tvec4<T,P> >& _data)
	{
		return glm::value_ptr(_data[0]);
	}

	// vector vec
	template <typename T, glm::precision P>
	inline T* to_pointer(std::vector<glm::detail::tvec1<T,P> >& _data)
	{
		return glm::value_ptr(_data[0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(std::vector<glm::detail::tvec2<T,P> >& _data)
	{
		return glm::value_ptr(_data[0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(std::vector<glm::detail::tvec3<T,P> >& _data)
	{
		return glm::value_ptr(_data[0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(std::vector<glm::detail::tvec4<T,P> >& _data)
	{
		return glm::value_ptr(_data[0]);
	}

	//////////////////////////////////////////////////////////////////////////
	// matrix
	template <typename T, glm::precision P>
	inline const T* to_pointer(const glm::detail::tmat2x2<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const  glm::detail::tmat3x3<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const glm::detail::tmat4x4<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const glm::detail::tmat2x3<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const glm::detail::tmat3x2<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const glm::detail::tmat2x4<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const glm::detail::tmat4x2<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const glm::detail::tmat3x4<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const glm::detail::tmat4x3<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	// matrix
	template <typename T, glm::precision P>
	inline T* to_pointer(glm::detail::tmat2x2<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(glm::detail::tmat3x3<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(glm::detail::tmat4x4<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(glm::detail::tmat2x3<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(glm::detail::tmat3x2<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(glm::detail::tmat2x4<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(glm::detail::tmat4x2<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(glm::detail::tmat3x4<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(glm::detail::tmat4x3<T,P>& _matrix)
	{
		return &(_matrix[0][0]);
	}

	//////////////////////////////////////////////////////////////////////////
	// matrix vector
	template <typename T, glm::precision P>
	inline const T* to_pointer(const std::vector<glm::detail::tmat2x2<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const  std::vector<glm::detail::tmat3x3<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const std::vector<glm::detail::tmat4x4<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const std::vector<glm::detail::tmat2x3<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const std::vector<glm::detail::tmat3x2<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const std::vector<glm::detail::tmat2x4<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const std::vector<glm::detail::tmat4x2<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const std::vector<glm::detail::tmat3x4<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline const T* to_pointer(const std::vector<glm::detail::tmat4x3<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}
	// matrix vector
	template <typename T, glm::precision P>
	inline T* to_pointer(std::vector<glm::detail::tmat2x2<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(std::vector<glm::detail::tmat3x3<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(std::vector<glm::detail::tmat4x4<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(std::vector<glm::detail::tmat2x3<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(std::vector<glm::detail::tmat3x2<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(std::vector<glm::detail::tmat2x4<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(std::vector<glm::detail::tmat4x2<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(std::vector<glm::detail::tmat3x4<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}

	template <typename T, glm::precision P>
	inline T* to_pointer(std::vector<glm::detail::tmat4x3<T,P> >& _matrix)
	{
		return &(_matrix[0][0][0]);
	}
};//namespace Private