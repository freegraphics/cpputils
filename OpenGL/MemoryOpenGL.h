#if !defined(__OPENGLMEMORY_H__B2732AF3_83A6_4ec7_9319_A251B7F49530__)
#define __OPENGLMEMORY_H__B2732AF3_83A6_4ec7_9319_A251B7F49530__

#pragma once

#include <Images/MemoryDC.h>
#include <gl/gl.h>
#include <gl/glu.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib") 

#include <utils/utils.h>

struct MemoryOpenGL
{
protected:
	MemoryDC m_memdc;
	GLenum m_LastGLError;
	HGLRC m_hRC;
public:
	MemoryOpenGL()
		:m_LastGLError(GL_NO_ERROR)
		,m_hRC(NULL)
	{
	}

	~MemoryOpenGL()
	{
		free();
	}

	bool create(const CSize& _sz,LPPIXELFORMATDESCRIPTOR _ppfd = NULL,int _bitcnt = 24,bool _bMonohrome = false)
	{
		m_memdc.Create(_sz,_bitcnt,_bMonohrome);
		PIXELFORMATDESCRIPTOR pfd = 
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_BITMAP |						// Format Must Support Window
				PFD_SUPPORT_OPENGL |					// Format Must Support OpenGL
				PFD_SUPPORT_GDI,						// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			_bitcnt,									// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			16,											// 16Bit Z-Buffer (Depth Buffer)  
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};
		if(NOT_NULL(_ppfd)) pfd = *_ppfd;

		HDC hdc = m_memdc.getDC();
		GLuint PixelFormat = ChoosePixelFormat(hdc,&pfd);
		if(!PixelFormat) return false;

		if(!SetPixelFormat(hdc,PixelFormat,&pfd)) return false;
		m_hRC=wglCreateContext(hdc);
		if(IS_NULL(m_hRC)) return false;
		if(!wglMakeCurrent(hdc,m_hRC)) return false;
		resize_gl_scene(_sz);
		if(!init_gl()) return false;
		return true;
	}

	void free()
	{
		if(NOT_NULL(m_hRC))
		{
			wglMakeCurrent(NULL,NULL);
			wglDeleteContext(m_hRC);
			m_hRC = NULL;
		}
		m_memdc.free();
	}

	template<typename _Type>
	void draw(void (_Type::*_pfunc)())
	{
		(static_cast<_Type&>(*this).*_pfunc)();
		SwapBuffers(m_memdc.getDC());
	}

	template<typename _Type>
	void draw(_Type& _obj,void (_Type::*_pfunc)())
	{
		(_obj.*_pfunc)();
		SwapBuffers(m_memdc.getDC());
	}

	void save(const CString& _sFN)
	{
		m_memdc.Save(_sFN);
	}

	bool check_gl_error()
	{
		m_LastGLError = glGetError();
		ASSERT(m_LastGLError==GL_NO_ERROR);
		return m_LastGLError==GL_NO_ERROR;
	}

protected:
	virtual void resize_gl_scene(const CSize& _sz)
	{
		GLint width = _sz.cx,height = _sz.cy;
		if(height==0)										// Prevent A Divide By Zero By
		{
			height=1;										// Making Height Equal One
		}

		glViewport(0,0,width,height);						// Reset The Current Viewport
		check_gl_error();

		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix
		check_gl_error();

		// Calculate The Aspect Ratio Of The Window
		gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
		check_gl_error();

		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		check_gl_error();
		glLoadIdentity();									// Reset The Modelview Matrix
		check_gl_error();
	}

	virtual bool init_gl()
	{
		glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
		check_gl_error();
		glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
		check_gl_error();
		glClearColor(0.0, 0.0, 0.0, 1.0f);				// Black Background
		check_gl_error();
		glClearDepth(1.0f);									// Depth Buffer Setup
		check_gl_error();
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		check_gl_error();
		glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
		check_gl_error();
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
		check_gl_error();
		glEnable( GL_LINE_SMOOTH );
		check_gl_error();
		glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
		check_gl_error();
		return true;
	}
};//struct MemoryOpenGL




#endif //#if !defined(__OPENGLMEMORY_H__B2732AF3_83A6_4ec7_9319_A251B7F49530__)