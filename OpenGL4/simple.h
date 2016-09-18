#pragma once 

#include "utils.h"
#include "error.h"

inline
void get_error(GLenum& _err_code)
{
	_err_code = glGetError();
}

inline
bool is_error_func(GLenum& _err_code,LPCTSTR _source_info)
{
	get_error(_err_code);
#if defined(GL_EXCEPTION_MODE)
	if(NEQL(GL_NO_ERROR,_err_code))
		throw_gl_error(_err_code,_source_info);
#endif
	return NEQL(GL_NO_ERROR,_err_code);
}

inline 
bool test_error_func(GLenum _err_code,LPCTSTR _source_info)
{
#if defined(GL_EXCEPTION_MODE)
	if(NEQL(GL_NO_ERROR,_err_code))
		throw_gl_error(_err_code,_source_info);
#endif
	return NEQL(GL_NO_ERROR,_err_code);
}

#define is_error(_err_code) is_error_func(_err_code,CREATE_SOURCEINFO())
#define test_error(_err_code) test_error_func(_err_code,CREATE_SOURCEINFO())

inline
void stop_using_vertex_array()
{
	glBindVertexArray(0);
}

inline
void stop_using_buffer()
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

inline
void set_point_size(GLfloat _size)
{
	glPointSize(_size);
}

inline
void set_line_width(GLfloat _width)
{
	glLineWidth(_width);
}

/*
	_face = GL_FRONT_AND_BACK, (GL_FRONT, GL_BACK)
	_mode = GL_POINT, GL_LINE, GL_FILL
*/
inline
void set_polygon_mode(GLenum _face,GLenum _mode)
{
	glPolygonMode(_face,_mode);
}

/*
	_mode = GL_CCW, GL_CW
*/
inline 
void set_front_face(GLenum _mode)
{
	glFrontFace(_mode);
}

/*
	_mode = GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
*/
inline
void set_cull_face(GLenum _face)
{
	glCullFace(_face);
}

inline 
void enable_cull_face()
{
	glEnable(GL_CULL_FACE);
}

inline 
void disable_cull_face()
{
	glDisable(GL_CULL_FACE);
}

inline
void enable_depth_test()
{
	glEnable(GL_DEPTH_TEST);
}

inline
void disable_depth_test()
{
	glDisable(GL_DEPTH_TEST);
}

inline 
void set_primitive_restart_index(GLuint _index)
{
	glPrimitiveRestartIndex(_index);
}

inline
void enable_primitive_restart()
{
	glEnable(GL_PRIMITIVE_RESTART);
}

inline
void disable_primitive_restart()
{
	glDisable(GL_PRIMITIVE_RESTART);
}

inline
void set_clear_color(GLclampf _red, GLclampf _green,GLclampf _blue, GLclampf _alpha)
{
	glClearColor(_red,_green,_blue,_alpha);
}

inline 
void set_clear_color(const glm::vec4& _color)
{
	glClearColor(_color.r,_color.g,_color.b,_color.a);
}

inline 
void set_clear_color(const glm::vec3& _color)
{
	glClearColor(_color.r,_color.g,_color.b,(GLclampf)1.0);
}

inline
void set_clear_depth(GLclampd _depth)
{
	glClearDepth(_depth);
}

inline
void set_clear_depth(GLclampf _depth)
{
	glClearDepthf(_depth);
}

inline
void set_clear_stencil(GLint _s)
{
	glClearStencil(_s);
}

inline
void clear_drawing_buffers(
	bool _color_buffer
	,bool _depth_buffer = false
	,bool _stencil_buffer = false
	)
{
	glClear(
		(_color_buffer?GL_COLOR_BUFFER_BIT:0)
		| (_depth_buffer?GL_DEPTH_BUFFER_BIT:0)
		| (_stencil_buffer?GL_STENCIL_BUFFER_BIT:0)
		);
}

inline
void mask_color(bool _red,bool _green,bool _blue,bool _alpha)
{
	glColorMask(
		ToGLBoolean(_red)
		,ToGLBoolean(_green)
		,ToGLBoolean(_blue)
		,ToGLBoolean(_alpha)
		);
}

// TODO: implement
//inline
//void mask_color()
//{
//	glColorMaski()	
//}

inline
void mask_depth(bool _flag)
{
	glDepthMask(ToGLBoolean(_flag));
}

inline
void mask_stencil(bool _flag)
{
	glStencilMask(ToGLBoolean(_flag));
}

/*
	_face = GL_CCW,GL_CW
*/
inline
void mask_stencil(GLenum _face,GLuint _mask)
{
	glStencilMaskSeparate(_face,_mask);
}

inline
bool is_multisampling_supported()
{
	GLint value = 0;
	glGetIntegerv(GL_SAMPLE_BUFFERS,&value);
	return EQL(value,GL_TRUE);
}

inline
void enable_multisampling()
{
	glEnable(GL_MULTISAMPLE);
}

inline
void disable_multisampling()
{
	glDisable(GL_MULTISAMPLE);
}

inline
GLint get_samples_per_pixel()
{
	GLint value = 0;
	glGetIntegerv(GL_SAMPLES,&value);
	return value;
}

inline
glm::vec2 get_sample_position(GLuint _index)
{
	glm::vec2 position;
	glGetMultisamplefv(GL_SAMPLE_POSITION,_index,Private::to_pointer(position));
	return position;
}

inline
void enable_sample_shading()
{
	glEnable(GL_SAMPLE_SHADING);
}

inline
void disable_sample_shading()
{
	glDisable(GL_SAMPLE_SHADING);
}

inline 
void set_minimum_sample_shading(GLfloat _value)
{
	glMinSampleShading(_value);
}

inline
void enable_scissor_test()
{
	glEnable(GL_SCISSOR_TEST);
}

inline
void disable_scissor_test()
{
	glDisable(GL_SCISSOR_TEST);
}

inline
void set_scissor_box(GLint _x,GLint _y,GLint _width,GLint _height)
{
	glScissor(_x,_y,_width,_height);
}

inline 
bool is_enabled_scissor_test()
{
	return glIsEnabled(GL_SCISSOR_TEST)==GL_TRUE;
}

inline 
glm::ivec4 get_scissor_box()
{
	glm::ivec4 box;
	glGetIntegerv(GL_SCISSOR_BOX,Private::to_pointer(box));
	return box;
}

inline
void enable_use_sample_alpha_tyo_coverage()
{
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
}

inline
void disable_use_sample_alpha_tyo_coverage()
{
	glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
}

inline
void enable_set_sample_alpha_to_one()
{
	glEnable(GL_SAMPLE_ALPHA_TO_ONE);
}

inline
void disable_set_sample_alpha_to_one()
{
	glDisable(GL_SAMPLE_ALPHA_TO_ONE);
}

inline 
void enable_sample_coverage()
{
	glEnable(GL_SAMPLE_COVERAGE);
}

inline 
void disable_sample_coverage()
{
	glDisable(GL_SAMPLE_COVERAGE);
}

inline
void set_sample_coverage(GLfloat _value,bool _invert)
{
	glSampleCoverage(_value,ToGLBoolean(_invert));
}

inline 
void enable_sample_mask()
{
	glEnable(GL_SAMPLE_MASK);
}

inline 
void disable_sample_mask()
{
	glDisable(GL_SAMPLE_MASK);
}

inline 
void set_sample_mask(GLuint _index, GLbitfield _mask)
{
	glSampleMaski(_index,_mask);
}

/*
	_func = GL_NEVER, GL_ALWAYS, GL_LESS,
		GL_LEQUAL, GL_EQUAL, GL_GEQUAL, GL_GREATER, or
		GL_NOTEQUAL
*/
inline
void set_stencil_func(GLenum _func, GLint _ref, GLuint _mask)
{
	glStencilFunc(_func,_ref,_mask);
}

/*
	_face = GL_FRONT_AND_BACK, (GL_FRONT, GL_BACK)
	_func = GL_NEVER, GL_ALWAYS, GL_LESS,
		GL_LEQUAL, GL_EQUAL, GL_GEQUAL, GL_GREATER, or
		GL_NOTEQUAL
*/
inline 
void set_stencil_func_separate(GLenum _face,GLenum _func,GLint _ref,GLuint _mask)
{
	glStencilFuncSeparate(_face,_func,_ref,_mask);
}

/*
	_fail,_zfail,_zpass = GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP,
		GL_DECR, GL_DECR_WRAP, or GL_INVERT
*/
inline 
void set_stencil_op(GLenum _fail, GLenum _zfail, GLenum _zpass)
{
	glStencilOp(_fail,_zfail,_zpass);
}

/*
	_face = GL_FRONT_AND_BACK, (GL_FRONT, GL_BACK)
	_fail,_zfail,_zpass = GL_KEEP, GL_ZERO, GL_REPLACE, GL_INCR, GL_INCR_WRAP,
		GL_DECR, GL_DECR_WRAP, or GL_INVERT
*/
inline
void set_stencil_op_separate(GLenum _face,GLenum _fail, GLenum _zfail, GLenum _zpass)
{
	glStencilOpSeparate(_face,_fail,_zfail,_zpass);
}

inline
bool is_enabled_stencil_test()
{
	return EQL(glIsEnabled(GL_STENCIL_TEST),GL_TRUE);
}

/*
	_func = GL_NEVER, GL_ALWAYS, GL_LESS, GL_LEQUAL, GL_EQUAL,
		GL_GEQUAL, GL_GREATER, or GL_NOTEQUAL
*/
inline 
void set_depth_func(GLenum _func)
{
	glDepthFunc(_func);
}

inline 
void enable_polygon_offset_fill()
{
	glEnable(GL_POLYGON_OFFSET_FILL);
}

inline 
void enable_polygon_offset_line()
{
	glEnable(GL_POLYGON_OFFSET_LINE);
}

inline 
void enable_polygon_offset_point()
{
	glEnable(GL_POLYGON_OFFSET_POINT);
}

inline
void set_polygon_offset(GLfloat _factor,GLfloat _units)
{
	glPolygonOffset(_factor,_units);
}

inline
void enable_blending()
{
	glEnable(GL_BLEND);
}

inline
void disable_blending()
{
	glDisable(GL_BLEND);
}

inline 
void enable_dithering()
{
	glEnable(GL_DITHER);
}

inline 
void disable_dithering()
{
	glDisable(GL_DITHER);
}

/*
	_hint = GL_FASTEST, GL_NICEST, GL_DONT_CARE
*/
inline 
void set_line_smooth_hint(GLenum _hint)
{
	glHint(GL_LINE_SMOOTH_HINT,_hint);
}

/*
	_hint = GL_FASTEST, GL_NICEST, GL_DONT_CARE
*/
inline 
void set_polygon_smooth_hint(GLenum _hint)
{
	glHint(GL_POLYGON_SMOOTH_HINT,_hint);
}

/*
	_hint = GL_FASTEST, GL_NICEST, GL_DONT_CARE
*/
inline 
void set_texture_compresion_hint(GLenum _hint)
{
	glHint(GL_TEXTURE_COMPRESSION_HINT,_hint);
}

/*
	_hint = GL_FASTEST, GL_NICEST, GL_DONT_CARE
*/
inline
void set_fragment_shader_derivative_hint(GLenum _hint)
{
	glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,_hint);
}

inline 
void enable_line_smooth()
{
	glEnable(GL_LINE_SMOOTH);
}

inline 
void disable_line_smooth()
{
	glDisable(GL_LINE_SMOOTH);
}

inline 
void enable_polygon_smooth()
{
	glEnable(GL_POLYGON_SMOOTH);
}

inline 
void disable_polygon_smooth()
{
	glDisable(GL_POLYGON_SMOOTH);
}

inline
void set_depth_range(GLclampf _near,GLclampf _far)
{
	glDepthRangef(_near,_far); 
}

inline
void set_depth_range(GLclampd _near,GLclampd _far)
{
	glDepthRange(_near,_far); 
}