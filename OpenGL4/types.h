#pragma once

#include "utils.h"

template<typename T>
struct GLType
{
	enum {get = 0,is_int = 0,is_double = 0};
};
template<GLenum _GLType>
struct GLenumToType
{
	typedef void type;
};


#define DEFINE_GLTYPE(_Type,_GL_TYPE_VALUE,_IsIntType,_IsDouble)				\
template<> struct GLType<_Type>													\
{																				\
	enum {get = _GL_TYPE_VALUE,is_int = _IsIntType,is_double = _IsDouble};		\
};																				\
template<> struct GLenumToType<_GL_TYPE_VALUE>									\
{																				\
	typedef _Type type;															\
};																				\
/***/

DEFINE_GLTYPE(GLbyte,GL_BYTE,GL_TRUE,GL_FALSE);
DEFINE_GLTYPE(GLubyte,GL_UNSIGNED_BYTE,GL_TRUE,GL_FALSE);
DEFINE_GLTYPE(GLshort,GL_SHORT,GL_TRUE,GL_FALSE);
DEFINE_GLTYPE(GLushort,GL_UNSIGNED_SHORT,GL_TRUE,GL_FALSE);
DEFINE_GLTYPE(GLint,GL_INT,GL_TRUE,GL_FALSE);
DEFINE_GLTYPE(GLuint,GL_UNSIGNED_INT,GL_TRUE,GL_FALSE);
//DEFINE_GLTYPE(GLfixed,GL_FIXED,GL_FALSE,GL_FALSE);
DEFINE_GLTYPE(GLfloat,GL_FLOAT,GL_FALSE,GL_FALSE);
//DEFINE_GLTYPE(GLhalf,GL_HALF_FLOAT,GL_FALSE,GL_FALSE);
DEFINE_GLTYPE(GLdouble,GL_DOUBLE,GL_FALSE,GL_TRUE);

template<typename _Type,size_t _Components>
struct GLInternalType
{
	enum {get = 0,get_normilized = 0};
};

#define DEFINE_GLINTERNALTYPE(_Type,_Components,_GL_INTERNAL_TYPE_VALUE,_GL_INTERNAL_TYPE_NORMILIZED_VALUE)	\
template<>																									\
struct GLInternalType<_Type,_Components>																	\
{																											\
	enum {get = _GL_INTERNAL_TYPE_VALUE,get_normilized = _GL_INTERNAL_TYPE_NORMILIZED_VALUE};				\
};																											\
/***/

DEFINE_GLINTERNALTYPE(GLubyte,1,GL_R8UI,GL_R8);
DEFINE_GLINTERNALTYPE(GLushort,1,GL_R16UI,GL_R16);
//DEFINE_GLINTERNALTYPE(GLhalf,1,GL_R16F,GL_R16F);
DEFINE_GLINTERNALTYPE(GLfloat,1,GL_R32F,GL_R32F);
DEFINE_GLINTERNALTYPE(GLbyte,1,GL_R8I,GL_R8I);
DEFINE_GLINTERNALTYPE(GLshort,1,GL_R16I,GL_R16I);
DEFINE_GLINTERNALTYPE(GLint,1,GL_R32I,GL_R32I);
DEFINE_GLINTERNALTYPE(GLuint,1,GL_R32UI,GL_R32UI);
DEFINE_GLINTERNALTYPE(GLubyte,2,GL_RG8UI,GL_RG8);
DEFINE_GLINTERNALTYPE(GLushort,2,GL_RG16UI,GL_RG16);
//DEFINE_GLINTERNALTYPE(GLhalf,2,GL_RG16F,GL_RG16F);
DEFINE_GLINTERNALTYPE(GLfloat,2,GL_RG32F,GL_RG32F);
DEFINE_GLINTERNALTYPE(GLbyte,2,GL_RG8I,GL_RG8I)
DEFINE_GLINTERNALTYPE(GLshort,2,GL_RG16I,GL_RG16I);
DEFINE_GLINTERNALTYPE(GLint,2,GL_RG32I,GL_RG32I);
DEFINE_GLINTERNALTYPE(GLuint,2,GL_RG32UI,GL_RG32UI);
DEFINE_GLINTERNALTYPE(GLfloat,3,GL_RGB32F,GL_RGB32F);
DEFINE_GLINTERNALTYPE(GLint,3,GL_RGB32I,GL_RGB32I);
DEFINE_GLINTERNALTYPE(GLuint,3,GL_RGB32UI,GL_RGB32UI);
DEFINE_GLINTERNALTYPE(GLuint,4,GL_RGBA32UI,GL_RGBA8);
DEFINE_GLINTERNALTYPE(GLshort,4,GL_RGBA16I,GL_RGBA16);
//DEFINE_GLINTERNALTYPE(GLhalf,4,GL_RGBA16F,GL_RGBA16F);
DEFINE_GLINTERNALTYPE(GLfloat,4,GL_RGBA32F,GL_RGBA32F);
DEFINE_GLINTERNALTYPE(GLbyte,4,GL_RGBA8I,GL_RGBA8I);
DEFINE_GLINTERNALTYPE(GLint,4,GL_RGBA32I,GL_RGBA32I);
DEFINE_GLINTERNALTYPE(GLubyte,4,GL_RGBA8UI,GL_RGBA8UI);
DEFINE_GLINTERNALTYPE(GLushort,4,GL_RGBA16UI,GL_RGBA16UI);

template<GLenum _GLType>
struct GLTypeSize
{
	enum {get = 0};
};

#define DEFINE_GLTYPESIZE(_GLType,_Count,_Type)					\
template<>														\
struct GLTypeSize<_GLType>										\
{																\
	enum {get = (_Count)*sizeof(_Type)};						\
};																\
/***/

DEFINE_GLTYPESIZE(GL_FLOAT, 1, GLfloat);
DEFINE_GLTYPESIZE(GL_FLOAT_VEC2, 2, GLfloat);
DEFINE_GLTYPESIZE(GL_FLOAT_VEC3, 3, GLfloat);
DEFINE_GLTYPESIZE(GL_FLOAT_VEC4, 4, GLfloat);
DEFINE_GLTYPESIZE(GL_DOUBLE, 1, GLdouble);
DEFINE_GLTYPESIZE(GL_DOUBLE_VEC2, 2, GLdouble);
DEFINE_GLTYPESIZE(GL_DOUBLE_VEC3, 3, GLdouble);
DEFINE_GLTYPESIZE(GL_DOUBLE_VEC4, 4, GLdouble);
DEFINE_GLTYPESIZE(GL_INT, 1, GLint);
DEFINE_GLTYPESIZE(GL_INT_VEC2, 2, GLint);
DEFINE_GLTYPESIZE(GL_INT_VEC3, 3, GLint);
DEFINE_GLTYPESIZE(GL_INT_VEC4, 4, GLint);
DEFINE_GLTYPESIZE(GL_UNSIGNED_INT, 1, GLuint);
DEFINE_GLTYPESIZE(GL_UNSIGNED_INT_VEC2, 2, GLuint);
DEFINE_GLTYPESIZE(GL_UNSIGNED_INT_VEC3, 3, GLuint);
DEFINE_GLTYPESIZE(GL_UNSIGNED_INT_VEC4, 4, GLuint);
DEFINE_GLTYPESIZE(GL_BOOL, 1, GLboolean);
DEFINE_GLTYPESIZE(GL_BOOL_VEC2, 2, GLboolean);
DEFINE_GLTYPESIZE(GL_BOOL_VEC3, 3, GLboolean);
DEFINE_GLTYPESIZE(GL_BOOL_VEC4, 4, GLboolean);
DEFINE_GLTYPESIZE(GL_FLOAT_MAT2, (2*2), GLfloat);
DEFINE_GLTYPESIZE(GL_FLOAT_MAT2x3, (2*3), GLfloat);
DEFINE_GLTYPESIZE(GL_FLOAT_MAT2x4, (2*4), GLfloat);
DEFINE_GLTYPESIZE(GL_FLOAT_MAT3, (3*3), GLfloat);
DEFINE_GLTYPESIZE(GL_FLOAT_MAT3x2, (3*2), GLfloat);
DEFINE_GLTYPESIZE(GL_FLOAT_MAT3x4, (3*4), GLfloat);
DEFINE_GLTYPESIZE(GL_FLOAT_MAT4, (4*4), GLfloat);
DEFINE_GLTYPESIZE(GL_FLOAT_MAT4x2, (4*2), GLfloat);
DEFINE_GLTYPESIZE(GL_FLOAT_MAT4x3, (4*3), GLfloat);
DEFINE_GLTYPESIZE(GL_DOUBLE_MAT2, (2*2), GLdouble);
DEFINE_GLTYPESIZE(GL_DOUBLE_MAT2x3, (2*3), GLdouble);
DEFINE_GLTYPESIZE(GL_DOUBLE_MAT2x4, (2*4), GLdouble);
DEFINE_GLTYPESIZE(GL_DOUBLE_MAT3, (3*3), GLdouble);
DEFINE_GLTYPESIZE(GL_DOUBLE_MAT3x2, (3*2), GLdouble);
DEFINE_GLTYPESIZE(GL_DOUBLE_MAT3x4, (3*4), GLdouble);
DEFINE_GLTYPESIZE(GL_DOUBLE_MAT4, (4*4), GLdouble);
DEFINE_GLTYPESIZE(GL_DOUBLE_MAT4x2, (4*2), GLdouble);
DEFINE_GLTYPESIZE(GL_DOUBLE_MAT4x3, (4*3), GLdouble);


inline
size_t get_gl_type_size(GLenum _type)
{
	size_t size = 0;

#define GLTYPESIZECASE(Enum, Count, Type) \
	case Enum: size = Count * sizeof(Type); break	\
	/***/

	switch(_type)
	{
		GLTYPESIZECASE(GL_FLOAT, 1, GLfloat);
		GLTYPESIZECASE(GL_FLOAT_VEC2, 2, GLfloat);
		GLTYPESIZECASE(GL_FLOAT_VEC3, 3, GLfloat);
		GLTYPESIZECASE(GL_FLOAT_VEC4, 4, GLfloat);
		GLTYPESIZECASE(GL_DOUBLE, 1, GLdouble);
		GLTYPESIZECASE(GL_DOUBLE_VEC2, 2, GLdouble);
		GLTYPESIZECASE(GL_DOUBLE_VEC3, 3, GLdouble);
		GLTYPESIZECASE(GL_DOUBLE_VEC4, 4, GLdouble);
		GLTYPESIZECASE(GL_INT, 1, GLint);
		GLTYPESIZECASE(GL_INT_VEC2, 2, GLint);
		GLTYPESIZECASE(GL_INT_VEC3, 3, GLint);
		GLTYPESIZECASE(GL_INT_VEC4, 4, GLint);
		GLTYPESIZECASE(GL_UNSIGNED_INT, 1, GLuint);
		GLTYPESIZECASE(GL_UNSIGNED_INT_VEC2, 2, GLuint);
		GLTYPESIZECASE(GL_UNSIGNED_INT_VEC3, 3, GLuint);
		GLTYPESIZECASE(GL_UNSIGNED_INT_VEC4, 4, GLuint);
		GLTYPESIZECASE(GL_BOOL, 1, GLboolean);
		GLTYPESIZECASE(GL_BOOL_VEC2, 2, GLboolean);
		GLTYPESIZECASE(GL_BOOL_VEC3, 3, GLboolean);
		GLTYPESIZECASE(GL_BOOL_VEC4, 4, GLboolean);
		GLTYPESIZECASE(GL_FLOAT_MAT2, 4, GLfloat);
		GLTYPESIZECASE(GL_FLOAT_MAT2x3, 6, GLfloat);
		GLTYPESIZECASE(GL_FLOAT_MAT2x4, 8, GLfloat);
		GLTYPESIZECASE(GL_FLOAT_MAT3, 9, GLfloat);
		GLTYPESIZECASE(GL_FLOAT_MAT3x2, 6, GLfloat);
		GLTYPESIZECASE(GL_FLOAT_MAT3x4, 12, GLfloat);
		GLTYPESIZECASE(GL_FLOAT_MAT4, 16, GLfloat);
		GLTYPESIZECASE(GL_FLOAT_MAT4x2, 8, GLfloat);
		GLTYPESIZECASE(GL_FLOAT_MAT4x3, 12, GLfloat);
		GLTYPESIZECASE(GL_DOUBLE_MAT2, 4, GLdouble);
		GLTYPESIZECASE(GL_DOUBLE_MAT2x3, 6, GLdouble);
		GLTYPESIZECASE(GL_DOUBLE_MAT2x4, 8, GLdouble);
		GLTYPESIZECASE(GL_DOUBLE_MAT3, 9, GLdouble);
		GLTYPESIZECASE(GL_DOUBLE_MAT3x2, 6, GLdouble);
		GLTYPESIZECASE(GL_DOUBLE_MAT3x4, 12, GLdouble);
		GLTYPESIZECASE(GL_DOUBLE_MAT4, 16, GLdouble);
		GLTYPESIZECASE(GL_DOUBLE_MAT4x2, 8, GLdouble);
		GLTYPESIZECASE(GL_DOUBLE_MAT4x3, 12, GLdouble);

#undef GLTYPESIZECASE

	default:
		return 0;
		break;
	}
	return size;
}

namespace detail
{
	template<GLenum _ToType>
	struct to_type
	{
		template<typename _Type>
		static const typename GLenumToType<_ToType>::type* convert(const _Type* _ptr)
		{
			return reinterpret_cast<const GLenumToType<_ToType>::type*>(_ptr);
		}
	};//template<> struct to_type
};//namespace detail

template<typename _Type>
struct GLMType
{
	typedef _Type element_type;
	enum {get_element_gl_type = GL_FLOAT};
	enum {get_elements_count = 1};
	enum {get_columns = 1};
};

#define DEFINE_GLMTYPE(_Container,_ItemCount,_Columns)		\
template<typename T,glm::precision P>						\
struct GLMType<_Container<T,P> >							\
{															\
	typedef T element_type;									\
	enum {get_element_gl_type = GLType<T>::get};			\
	enum {get_elements_count = (_ItemCount)};				\
	enum {get_columns = (_Columns)};						\
};															\
/***/

DEFINE_GLMTYPE(glm::detail::tvec1,1,1);
DEFINE_GLMTYPE(glm::detail::tvec2,2,1);
DEFINE_GLMTYPE(glm::detail::tvec3,3,1);
DEFINE_GLMTYPE(glm::detail::tvec4,4,1);
DEFINE_GLMTYPE(glm::detail::tmat2x2,2*2,2);
DEFINE_GLMTYPE(glm::detail::tmat2x3,2*3,2);
DEFINE_GLMTYPE(glm::detail::tmat2x4,2*4,2);
DEFINE_GLMTYPE(glm::detail::tmat3x2,3*2,3);
DEFINE_GLMTYPE(glm::detail::tmat3x3,3*3,3);
DEFINE_GLMTYPE(glm::detail::tmat3x4,3*4,3);
DEFINE_GLMTYPE(glm::detail::tmat4x2,4*2,4);
DEFINE_GLMTYPE(glm::detail::tmat4x3,4*3,4);
DEFINE_GLMTYPE(glm::detail::tmat4x4,4*4,4);

namespace detail
{
	// items
	template<typename _Type> inline
	GLenum get_element_gl_type(_Type)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type> inline
	GLsizei get_elements_count(_Type)
	{
		return 1;
	}

	// glm::vecs
	template<typename _Type,glm::precision _Precision> inline
	GLenum get_element_gl_type(glm::detail::tvec1<_Type,_Precision>)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_element_gl_type(glm::detail::tvec2<_Type,_Precision>)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_element_gl_type(glm::detail::tvec3<_Type,_Precision>)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_element_gl_type(glm::detail::tvec4<_Type,_Precision>)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_elements_count(glm::detail::tvec1<_Type,_Precision>)
	{
		return 1;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_elements_count(glm::detail::tvec2<_Type,_Precision>)
	{
		return 2;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_elements_count(glm::detail::tvec3<_Type,_Precision>)
	{
		return 3;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_elements_count(glm::detail::tvec4<_Type,_Precision>)
	{
		return 4;
	}

	// glm::matrix
	template<typename _Type,glm::precision _Precision> inline
	GLenum get_element_gl_type(glm::detail::tmat2x2<_Type,_Precision>)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_elements_count(glm::detail::tmat2x2<_Type,_Precision>)
	{
		return 2*2;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_element_gl_type(glm::detail::tmat2x3<_Type,_Precision>)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_elements_count(glm::detail::tmat2x3<_Type,_Precision>)
	{
		return 2*3;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_element_gl_type(glm::detail::tmat2x4<_Type,_Precision>)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_elements_count(glm::detail::tmat2x4<_Type,_Precision>)
	{
		return 2*4;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_element_gl_type(glm::detail::tmat3x2<_Type,_Precision>)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_elements_count(glm::detail::tmat3x2<_Type,_Precision>)
	{
		return 3*2;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_element_gl_type(glm::detail::tmat4x2<_Type,_Precision>)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_elements_count(glm::detail::tmat4x2<_Type,_Precision>)
	{
		return 4*2;
	}


	template<typename _Type,glm::precision _Precision> inline
	GLenum get_element_gl_type(glm::detail::tmat3x3<_Type,_Precision>)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_elements_count(glm::detail::tmat3x3<_Type,_Precision>)
	{
		return 3*3;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_element_gl_type(glm::detail::tmat3x4<_Type,_Precision>)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_elements_count(glm::detail::tmat3x4<_Type,_Precision>)
	{
		return 3*4;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_element_gl_type(glm::detail::tmat4x3<_Type,_Precision>)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_elements_count(glm::detail::tmat4x3<_Type,_Precision>)
	{
		return 4*3;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_element_gl_type(glm::detail::tmat4x4<_Type,_Precision>)
	{
		return GLType<_Type>::get;
	}

	template<typename _Type,glm::precision _Precision> inline
	GLenum get_elements_count(glm::detail::tmat4x4<_Type,_Precision>)
	{
		return 4*4;
	}

};//namespace detail