#pragma once

#include "utils.h"
#include "glewprepare.h"

//#include <afxglobals.h>


template<typename _Type>
struct GLDrawingToWindow
{
protected:
	MemoryDC m_memdc;
	GLenum m_error_code;
	HGLRC m_hRC;
	int m_OpenGLVersion[2];
	bool m_double_buffering_mode;
	CWnd m_draw_wnd;
	bool m_inner_wnd;

public:
	GLDrawingToWindow()
		:m_error_code(GL_NO_ERROR)
		,m_hRC(NULL)
		,m_double_buffering_mode(false)
		,m_inner_wnd(true)
	{
		ZeroObj(m_OpenGLVersion);
	}

	~GLDrawingToWindow()
	{
		free();
	}

	CWnd& get_wnd() 
	{
		return m_draw_wnd;
	}

	const CWnd& get_wnd() const
	{
		return m_draw_wnd;
	}

	bool create(const CSize& _sz,LPPIXELFORMATDESCRIPTOR _ppfd = NULL,int _bitcnt = 24,bool _bMonohrome = false,HWND _hwnd = NULL)
	{
		free();
		m_memdc.Create(_sz,_bitcnt,_bMonohrome);

		if(!GLEWPrepare::make()) return false;

		const LPCTSTR opengl_draw_class_name = _T("OpenGL30DrawClass");
		CString class_name = AfxRegisterWndClass(CS_DBLCLKS|CS_OWNDC,::LoadCursor(NULL,IDC_ARROW));
		m_inner_wnd = false;
		if(IS_NULL(_hwnd) || !IsWindow(_hwnd))
		{
			_hwnd = ::CreateWindow(
				class_name, _T("OpenGL30DrawWindow")
				,WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN
				,0, 0, _sz.cx, _sz.cy
				,NULL
				,NULL
				,AfxGetInstanceHandle()
				,NULL
				);
			m_inner_wnd = true;
		}
		m_draw_wnd.Attach(_hwnd);
		m_draw_wnd.ShowWindow(SW_HIDE);
		HDC hdc = GetDC(_hwnd);


		PIXELFORMATDESCRIPTOR pfd = 
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
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


		if(NEQL(GL_TRUE,wglewIsSupported("WGL_ARB_pixel_format")) || IS_NULL(wglChoosePixelFormatARB))
		{
			free();
			return false;
		}

		float fAttributes[] = {0,0};
		int iAttributes[] =
		{
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB
			,WGL_DOUBLE_BUFFER_ARB, GL_FALSE
			,WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB
			,WGL_DRAW_TO_WINDOW_ARB, GL_TRUE
			,WGL_SUPPORT_OPENGL_ARB, GL_TRUE
			,WGL_COLOR_BITS_ARB, pfd.cColorBits
			,WGL_ALPHA_BITS_ARB, (pfd.cColorBits==32?8:0)
			,WGL_DEPTH_BITS_ARB, pfd.cDepthBits
			,0, 0
		};

		m_double_buffering_mode = 
			EQL(GL_TRUE
				,get_attr_value(
					&iAttributes[0],&iAttributes[_countof(iAttributes)]
					,WGL_DOUBLE_BUFFER_ARB,GL_FALSE
					)
				);

		int pixelFormat = 0;
		UINT numFormats = 0;

		if(!wglChoosePixelFormatARB(hdc,iAttributes,fAttributes,1,&pixelFormat,&numFormats) || numFormats==0)
		{
			set_attr_value(&iAttributes[0],&iAttributes[_countof(iAttributes)],WGL_ACCELERATION_ARB,WGL_NO_ACCELERATION_ARB);
			if(!wglChoosePixelFormatARB(hdc,iAttributes,fAttributes,1,&pixelFormat,&numFormats) || numFormats==0)
			{
				free();
				return false;
			}
		}

		::SetPixelFormat(hdc,pixelFormat,&pfd);

		if(NEQL(GL_TRUE,wglewIsSupported("WGL_ARB_create_context")) || IS_NULL(wglCreateContextAttribsARB))
		{
			free();
			return false;
		}

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3
			,WGL_CONTEXT_MINOR_VERSION_ARB, 1
			,WGL_CONTEXT_FLAGS_ARB, 0
			,0
		};
		m_hRC = wglCreateContextAttribsARB(hdc, 0, attribs);
		wglMakeCurrent(NULL,NULL);
		if(IS_NULL(m_hRC)) 
		{
			free();
			return false;
		}

		wglMakeCurrent(hdc, m_hRC);

		glGetIntegerv(GL_MAJOR_VERSION, &m_OpenGLVersion[0]);
		glGetIntegerv(GL_MINOR_VERSION, &m_OpenGLVersion[1]);

		static_cast<_Type&>(*this).resize_scene(_sz);
		if(!static_cast<_Type&>(*this).init_scene()) 
		{
			free();
			return false;
		}
		return true;
	}

	int get_major_version() const 
	{
		return m_OpenGLVersion[0];
	}

	int get_minor_version() const 
	{
		return m_OpenGLVersion[1];
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

		HWND hwnd_old = m_draw_wnd.Detach();
		if(m_inner_wnd && NOT_NULL(hwnd_old))
		{
			::DestroyWindow(hwnd_old);
			hwnd_old = NULL;
		}
	}

	void call(void (_Type::*_pfunc)())
	{
		(static_cast<_Type&>(*this).*_pfunc)();
		glFlush();
		if(m_double_buffering_mode)
			SwapBuffers(m_memdc.getDC());
	}

	template<typename _Type1>
	void call(_Type1& _obj,void (_Type1::*_pfunc)())
	{
		(_obj.*_pfunc)();
		glFlush();
		if(m_double_buffering_mode)
			SwapBuffers(m_memdc.getDC());
	}

	void save(const CString& _sFN)
	{
		m_memdc.Save(_sFN);
	}

	bool test_gl_error()
	{
		return is_error(m_error_code);
	}

	GLenum get_last_error() const 
	{
		return m_error_code;
	}

	void read_pixels()
	{
		m_memdc.fill(0);
		std::vector<byte> buf;
		buf.resize(labs(m_memdc.getWidth())*labs(m_memdc.getHeight())*3);
		LPVOID pbuf = &buf[0];

		glFinish();
		glReadPixels(0,0,labs(m_memdc.getWidth()),labs(m_memdc.getHeight()),GL_BGR,GL_UNSIGNED_BYTE,pbuf);

		size_t x = 0, y = 0;
		size_t offs = 0;
		if(m_memdc.getBMI()->bmiHeader.biBitCount==24)
		{
			LPBYTE pdta = (LPBYTE)m_memdc.getBits();
			size_t offs1 = m_memdc.getWidthBytes()*labs(m_memdc.getHeight());
			for(y=0;y<(size_t)labs(m_memdc.getHeight());y++)
			{
				offs1 -= m_memdc.getWidthBytes();
				size_t sz = (size_t)labs(m_memdc.getWidth())*3;
				memcpy(pdta+offs1,&buf[offs],sz);
				offs += sz;
			}
		}
		else
		{
			for(y=0;y<(size_t)labs(m_memdc.getHeight());y++)
			{
				for(x=0;x<(size_t)labs(m_memdc.getWidth());x++,offs+=3)
				{
					byte b = buf[offs + 0];
					byte g = buf[offs + 1];
					byte r = buf[offs + 2];
					m_memdc.SetRGBA(x,y,r,g,b,255);
				}
			}
		}
	}

protected:
public:
	void resize_scene(const CSize& _sz)
	{
		GLint width = _sz.cx,height = _sz.cy;
		if(height==0)										// Prevent A Divide By Zero By
		{
			height=1;										// Making Height Equal One
		}

		glViewport(0,0,width,height);						// Reset The Current Viewport
		test_gl_error();



/*		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix
		test_gl_error();

		// Calculate The Aspect Ratio Of The Window
		gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
		test_gl_error();

		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		test_gl_error();
		glLoadIdentity();									// Reset The Modelview Matrix
		test_gl_error();
*/
	}

	bool init_scene()
	{
/*		glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
		test_gl_error();
		glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
		test_gl_error();
		glClearColor(0.0, 0.0, 0.0, 1.0f);				// Black Background
		test_gl_error();
		glClearDepth(1.0f);									// Depth Buffer Setup
		test_gl_error();
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		test_gl_error();
		glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
		test_gl_error();
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
		test_gl_error();
		glEnable( GL_LINE_SMOOTH );
		test_gl_error();
		glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
		test_gl_error();
*/
		return true;
	}
private:
	template<typename _It>
	void set_attr_value(_It _it,_It _ite,GLenum _field,int _new_value)
	{
		for(;_it!=_ite;)
		{
			if(*_it==_field)
			{
				if(++_it==_ite) break;
				*_it = _new_value;
				++_it;
			}
			else
			{
				if(++_it==_ite) break;
				++_it;
			}
		}
	}

	template<typename _It>
	int get_attr_value(_It _it,_It _ite,GLenum _field,int _def_value)
	{
		for(;_it!=_ite;)
		{
			if(*_it==_field)
			{
				if(++_it==_ite) break;
				return *_it;
			}
			else
			{
				if(++_it==_ite) break;
				++_it;
			}
		}
		return _def_value;
	}
};
