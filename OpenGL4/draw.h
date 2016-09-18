#pragma once

#include "utils.h"
#include "arraybuffer.h"
#include "elements.h"
#include "simple.h"

namespace detail
{
	template<
		typename _T1,typename _T2 = GLfloat,typename _T3 = GLfloat
		,typename _T4 = GLfloat,typename _T5 = GLfloat,typename _T6 = GLfloat
		,typename _T7 = GLfloat,typename _T8 = GLfloat,typename _T9 = GLfloat
		,typename _T10 = GLfloat
	>
	struct draw_arrays
	{
		static bool call(
			GLenum _mode
			,ArrayData<_T1>& _a1
			,ArrayData<_T2>& _a2
			,ArrayData<_T3>& _a3
			,ArrayData<_T4>& _a4
			,ArrayData<_T5>& _a5
			,ArrayData<_T6>& _a6
			,ArrayData<_T7>& _a7
			,ArrayData<_T8>& _a8
			,ArrayData<_T9>& _a9
			,ArrayData<_T10>& _a10
			,size_t _idx = 0
			,size_t _count = -1
			)
		{
			if(!_a1.empty()) _a1.enable();
			if(!_a2.empty()) _a2.enable();
			if(!_a3.empty()) _a3.enable();
			if(!_a4.empty()) _a4.enable();
			if(!_a5.empty()) _a5.enable();
			if(!_a6.empty()) _a6.enable();
			if(!_a7.empty()) _a7.enable();
			if(!_a8.empty()) _a8.enable();
			if(!_a9.empty()) _a9.enable();
			if(!_a10.empty()) _a10.enable();
			GLsizei min_sz = _count;
			if(min_sz==-1)
			{
				if(!_a1.empty()) 
					min_sz = _a1.get_item_count();
				if(!_a2.empty())
					min_sz = min_sz==-1?_a2.get_item_count():min_value(min_sz,_a2.get_item_count()); 
				if(!_a3.empty())
					min_sz = min_sz==-1?_a3.get_item_count():min_value(min_sz,_a3.get_item_count()); 
				if(!_a4.empty())
					min_sz = min_sz==-1?_a4.get_item_count():min_value(min_sz,_a4.get_item_count()); 
				if(!_a5.empty())
					min_sz = min_sz==-1?_a5.get_item_count():min_value(min_sz,_a5.get_item_count()); 
				if(!_a6.empty())
					min_sz = min_sz==-1?_a6.get_item_count():min_value(min_sz,_a6.get_item_count()); 
				if(!_a7.empty())
					min_sz = min_sz==-1?_a7.get_item_count():min_value(min_sz,_a7.get_item_count()); 
				if(!_a8.empty())
					min_sz = min_sz==-1?_a8.get_item_count():min_value(min_sz,_a8.get_item_count()); 
				if(!_a9.empty())
					min_sz = min_sz==-1?_a9.get_item_count():min_value(min_sz,_a9.get_item_count()); 
				if(!_a10.empty())
					min_sz = min_sz==-1?_a10.get_item_count():min_value(min_sz,_a10.get_item_count()); 
			}
			else
			{
				min_sz += _idx;
			}
			if(min_sz==-1 || (GLsizei)_idx>=min_sz) 
			{
				return true;
			}
			min_sz -= (GLsizei)_idx;
			glDrawArrays(_mode,_idx,min_sz);
			GLenum error_code = GL_NO_ERROR;
			return !is_error(error_code);
		}
	};//template<> struct draw_arrays

	template<
		typename _T0
		,typename _T1,typename _T2 = GLfloat,typename _T3 = GLfloat
		,typename _T4 = GLfloat,typename _T5 = GLfloat,typename _T6 = GLfloat
		,typename _T7 = GLfloat,typename _T8 = GLfloat,typename _T9 = GLfloat
		,typename _T10 = GLfloat
	>
	struct draw_elements
	{
		static bool call(
			GLenum _mode
			,Elements<_T0>& _elements
			,ArrayData<_T1>& _a1
			,ArrayData<_T2>& _a2
			,ArrayData<_T3>& _a3
			,ArrayData<_T4>& _a4
			,ArrayData<_T5>& _a5
			,ArrayData<_T6>& _a6
			,ArrayData<_T7>& _a7
			,ArrayData<_T8>& _a8
			,ArrayData<_T9>& _a9
			,ArrayData<_T10>& _a10
			,size_t _idx = 0
			,size_t _count = -1
			)
		{
			if(!_a1.empty()) _a1.enable();
			if(!_a2.empty()) _a2.enable();
			if(!_a3.empty()) _a3.enable();
			if(!_a4.empty()) _a4.enable();
			if(!_a5.empty()) _a5.enable();
			if(!_a6.empty()) _a6.enable();
			if(!_a7.empty()) _a7.enable();
			if(!_a8.empty()) _a8.enable();
			if(!_a9.empty()) _a9.enable();
			if(!_a10.empty()) _a10.enable();
			return _elements.draw(_mode,_idx,_count);
		}
	};
};//namespace detail


/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T1
	>
inline 
bool draw_arrays(
	GLenum _mode
	,ArrayData<_T1>& _a1
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_arrays<_T1>
		::call(_mode,_a1
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}


/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T1,typename _T2
	>
inline 
bool draw_arrays(
	GLenum _mode
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_arrays<_T1,_T2>
		::call(_mode,_a1,_a2
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T1,typename _T2,typename _T3
	>
inline 
bool draw_arrays(
	GLenum _mode
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_arrays<_T1,_T2,_T3>
		::call(_mode,_a1,_a2,_a3
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T1,typename _T2,typename _T3,typename _T4
	>
inline 
bool draw_arrays(
	GLenum _mode
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_arrays<_T1,_T2,_T3,_T4>
		::call(_mode,_a1,_a2,_a3,_a4
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T1,typename _T2,typename _T3,typename _T4,typename _T5
	>
inline 
bool draw_arrays(
	GLenum _mode
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,ArrayData<_T5>& _a5
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_arrays<_T1,_T2,_T3,_T4,_T5>
		::call(_mode,_a1,_a2,_a3,_a4,_a5
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T1,typename _T2,typename _T3,typename _T4,typename _T5
	,typename _T6
	>
inline 
bool draw_arrays(
	GLenum _mode
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,ArrayData<_T5>& _a5
	,ArrayData<_T6>& _a6
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_arrays<_T1,_T2,_T3,_T4,_T5,_T6>
		::call(_mode,_a1,_a2,_a3,_a4,_a5,_a6
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T1,typename _T2,typename _T3,typename _T4,typename _T5
	,typename _T6,typename _T7
	>
inline 
bool draw_arrays(
	GLenum _mode
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,ArrayData<_T5>& _a5
	,ArrayData<_T6>& _a6
	,ArrayData<_T7>& _a7
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_arrays<_T1,_T2,_T3,_T4,_T5,_T6,_T7>
		::call(_mode,_a1,_a2,_a3,_a4,_a5,_a6,_a7
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T1,typename _T2,typename _T3,typename _T4,typename _T5
	,typename _T6,typename _T7,typename _T8
	>
inline 
bool draw_arrays(
	GLenum _mode
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,ArrayData<_T5>& _a5
	,ArrayData<_T6>& _a6
	,ArrayData<_T7>& _a7
	,ArrayData<_T8>& _a8
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_arrays<_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8>
		::call(_mode,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}


/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T1,typename _T2,typename _T3,typename _T4,typename _T5
	,typename _T6,typename _T7,typename _T8,typename _T9
	>
inline 
bool draw_arrays(
	GLenum _mode
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,ArrayData<_T5>& _a5
	,ArrayData<_T6>& _a6
	,ArrayData<_T7>& _a7
	,ArrayData<_T8>& _a8
	,ArrayData<_T9>& _a9
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_arrays<_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8,_T9>
		::call(_mode,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8,_a9,ArrayData<GLfloat>::Null(),_idx,_count);
}


/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T1,typename _T2,typename _T3,typename _T4,typename _T5
	,typename _T6,typename _T7,typename _T8,typename _T9,typename _T10
	>
inline 
bool draw_arrays(
	GLenum _mode
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,ArrayData<_T5>& _a5
	,ArrayData<_T6>& _a6
	,ArrayData<_T7>& _a7
	,ArrayData<_T8>& _a8
	,ArrayData<_T9>& _a9
	,ArrayData<_T10>& _a10
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_arrays<_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8,_T9,_T10>
		::call(_mode,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8,_a9,_a10,_idx,_count);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T0
	,typename _T1
	>
inline 
bool draw_elements(
	GLenum _mode
	,Elements<_T0>& _elements
	,ArrayData<_T1>& _a1
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_elements<_T0,_T1>
		::call(_mode,_elements,_a1
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T0
	,typename _T1,typename _T2
	>
inline 
bool draw_elements(
	GLenum _mode
	,Elements<_T0>& _elements
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_elements<_T0,_T1,_T2>
		::call(_mode,_elements,_a1,_a2
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T0
	,typename _T1,typename _T2,typename _T3
	>
inline 
bool draw_elements(
	GLenum _mode
	,Elements<_T0>& _elements
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_elements<_T0,_T1,_T2,_T3>
		::call(_mode,_elements,_a1,_a2,_a3
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T0
	,typename _T1,typename _T2,typename _T3,typename _T4
	>
inline 
bool draw_elements(
	GLenum _mode
	,Elements<_T0>& _elements
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_elements<_T0,_T1,_T2,_T3,_T4>
		::call(_mode,_elements,_a1,_a2,_a3,_a4
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T0
	,typename _T1,typename _T2,typename _T3,typename _T4,typename _T5
	>
inline 
bool draw_elements(
	GLenum _mode
	,Elements<_T0>& _elements
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,ArrayData<_T5>& _a5
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_elements<_T0,_T1,_T2,_T3,_T4,_T5>
		::call(_mode,_elements,_a1,_a2,_a3,_a4,_a5
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T0
	,typename _T1,typename _T2,typename _T3,typename _T4,typename _T5
	,typename _T6
	>
inline 
bool draw_elements(
	GLenum _mode
	,Elements<_T0>& _elements
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,ArrayData<_T5>& _a5
	,ArrayData<_T6>& _a6
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_elements<_T0,_T1,_T2,_T3,_T4,_T5,_T6>
		::call(_mode,_elements,_a1,_a2,_a3,_a4,_a5,_a6
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}


/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T0
	,typename _T1,typename _T2,typename _T3,typename _T4,typename _T5
	,typename _T6,typename _T7
	>
inline 
bool draw_elements(
	GLenum _mode
	,Elements<_T0>& _elements
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,ArrayData<_T5>& _a5
	,ArrayData<_T6>& _a6
	,ArrayData<_T7>& _a7
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_elements<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7>
		::call(_mode,_elements,_a1,_a2,_a3,_a4,_a5,_a6,_a7
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T0
	,typename _T1,typename _T2,typename _T3,typename _T4,typename _T5
	,typename _T6,typename _T7,typename _T8
	>
inline 
bool draw_elements(
	GLenum _mode
	,Elements<_T0>& _elements
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,ArrayData<_T5>& _a5
	,ArrayData<_T6>& _a6
	,ArrayData<_T7>& _a7
	,ArrayData<_T8>& _a8
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_elements<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8>
		::call(_mode,_elements,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8
			,ArrayData<GLfloat>::Null()
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}


/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T0
	,typename _T1,typename _T2,typename _T3,typename _T4,typename _T5
	,typename _T6,typename _T7,typename _T8,typename _T9
	>
inline 
bool draw_elements(
	GLenum _mode
	,Elements<_T0>& _elements
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,ArrayData<_T5>& _a5
	,ArrayData<_T6>& _a6
	,ArrayData<_T7>& _a7
	,ArrayData<_T8>& _a8
	,ArrayData<_T9>& _a9
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_elements<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8,_T9>
		::call(_mode,_elements,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8,_a9
			,ArrayData<GLfloat>::Null()
			,_idx,_count
			);
}

/*
	_mode = GL_POINTS,GL_LINE_STRIP,GL_LINE_LOOP,GL_LINES,GL_LINE_STRIP_ADJACENCY,
		GL_LINES_ADJACENCY,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,GL_TRIANGLES,
		GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
*/
template<
	typename _T0
	,typename _T1,typename _T2,typename _T3,typename _T4,typename _T5
	,typename _T6,typename _T7,typename _T8,typename _T9,typename _T10
	>
inline 
bool draw_elements(
	GLenum _mode
	,Elements<_T0>& _elements
	,ArrayData<_T1>& _a1
	,ArrayData<_T2>& _a2
	,ArrayData<_T3>& _a3
	,ArrayData<_T4>& _a4
	,ArrayData<_T5>& _a5
	,ArrayData<_T6>& _a6
	,ArrayData<_T7>& _a7
	,ArrayData<_T8>& _a8
	,ArrayData<_T9>& _a9
	,ArrayData<_T10>& _a10
	,size_t _idx = 0
	,size_t _count = -1
	)
{
	return detail::draw_elements<_T0,_T1,_T2,_T3,_T4,_T5,_T6,_T7,_T8,_T9,_T10>
		::call(_mode,_elements,_a1,_a2,_a3,_a4,_a5,_a6,_a7,_a8,_a9,_a10,_idx,_count);
}
