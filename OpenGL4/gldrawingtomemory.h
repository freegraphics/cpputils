#pragma once

#include "utils.h"
#include "glewprepare.h"
#include "renderbuffer.h"
#include "framebuffer.h"
#include "texture.h"

#include <Images/MemoryDC.h>

struct GLContext
{
protected:
	CWnd m_draw_wnd;
	HGLRC m_hRC;
	int m_OpenGLVersion[2];

protected:
	GLContext()
	{
		ZeroObj(m_OpenGLVersion);
		PIXELFORMATDESCRIPTOR pfd = 
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |					// Format Must Support OpenGL
			PFD_SUPPORT_GDI,						// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			32,									// Select Our Color Depth
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
		VERIFY(create(pfd));
	}

	~GLContext()
	{
		free();
	}

	bool create(const PIXELFORMATDESCRIPTOR& _pfd)
	{
		const LPCTSTR opengl_draw_class_name = _T("OpenGL30DrawClass");
		CString class_name = AfxRegisterWndClass(CS_DBLCLKS|CS_OWNDC,::LoadCursor(NULL,IDC_ARROW));
		HWND hwnd = ::CreateWindow(
			class_name, _T("OpenGL30ContextWindow")
			,WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN
			,0, 0, 32, 32
			,NULL
			,NULL
			,AfxGetInstanceHandle()
			,NULL
			);
		m_draw_wnd.Attach(hwnd);
		m_draw_wnd.ShowWindow(SW_HIDE);
		HDC hdc = GetDC(hwnd);

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
			,WGL_COLOR_BITS_ARB, _pfd.cColorBits
			,WGL_ALPHA_BITS_ARB, (_pfd.cColorBits==32?8:0)
			,WGL_DEPTH_BITS_ARB, _pfd.cDepthBits
			//,WGL_SAMPLE_BUFFERS_ARB, 1
			//,WGL_SAMPLES_ARB, 4
			,0, 0
		};

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

		::SetPixelFormat(hdc,pixelFormat,&_pfd);

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

		HWND hwnd_old = m_draw_wnd.Detach();
		if(NOT_NULL(hwnd_old))
		{
			::DestroyWindow(hwnd_old);
			hwnd_old = NULL;
		}
	}

protected:
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

	static GLContext& get_instance()
	{
		static GLContext _;
		return _;
	}
public:
	static bool make()
	{
		return NOT_NULL(get_instance().m_hRC);
	}

	static int get_major_version() 
	{
		return get_instance().m_OpenGLVersion[0];
	}

	static int get_minor_version()
	{
		return get_instance().m_OpenGLVersion[1];
	}
};//struct GLContext


template<typename _Type>
struct GLDrawingToMemory
{
protected:
	MemoryDC m_memdc;
	GLenum m_error_code;
	//int m_OpenGLVersion[2];
	Framebuffer m_frame_buffer;
	Renderbuffer m_rgba_buffer;
	Texture m_rgba_texture;
	Renderbuffer m_depth_buffer;
	Renderbuffer m_stencil_buffer;

public:
	GLDrawingToMemory()
		:m_error_code(GL_NO_ERROR)
	{
		//ZeroObj(m_OpenGLVersion);
	}

	~GLDrawingToMemory()
	{
		free();
	}

	GLenum get_last_error() const 
	{ 
		return m_error_code;
	}

	bool create(const CSize& _sz,LPPIXELFORMATDESCRIPTOR _ppfd = NULL,int _bitcnt = 24,bool _bMonohrome = false,GLsizei _samples = 0)
	{
		free();
		m_memdc.Create(_sz,_bitcnt,_bMonohrome);

		if(!GLEWPrepare::make()) return false;
		if(!GLContext::make()) return false;

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

		create_buffers(_sz,&pfd,_samples);

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
		//return m_OpenGLVersion[0];
		return GLContext::get_major_version();
	}

	int get_minor_version() const 
	{
		//return m_OpenGLVersion[1];
		return GLContext::get_minor_version();
	}

	void free()
	{
		m_memdc.free();
	}

	void call(void (_Type::*_pfunc)())
	{
		//m_frame_buffer.draw_buffer(GL_FRONT_LEFT);
		(static_cast<_Type&>(*this).*_pfunc)();
		glFlush();
	}

	template<typename _Type1>
	void call(_Type1& _obj,void (_Type1::*_pfunc)())
	{
		//m_frame_buffer.draw_buffer(GL_FRONT_LEFT);
		(_obj.*_pfunc)();
		glFlush();
	}

	void save(const CString& _sFN)
	{
		m_memdc.Save(_sFN);
	}

	bool test_gl_error()
	{
		return is_error(m_error_code);
	}

	void read_pixels()
	{
		m_memdc.fill(0);
		std::vector<byte> buf;
		buf.resize(labs(m_memdc.getWidth())*labs(m_memdc.getHeight())*3);
		LPVOID pbuf = &buf[0];

		glFinish();
		//m_frame_buffer.read_buffer(GL_FRONT);
		
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
	bool create_buffers(const CSize& _sz,const PIXELFORMATDESCRIPTOR* _pfd,GLsizei _samples)
	{
		VERIFY_EXIT1(NOT_NULL(_pfd),false);
		m_frame_buffer.create();
		if(EQL(_samples,0))
			m_rgba_buffer.create();
		else
			m_rgba_texture.create(GL_TEXTURE_2D_MULTISAMPLE);
		GLenum internal_format = GL_RGBA8;
		switch(_pfd->cColorBits)
		{
		case 8:
		case 24:
			internal_format = GL_RGB8;
			break;
		case 32:
		default:
			internal_format = GL_RGBA8;
			break;
		};
		if(EQL(_samples,0))
		{
			m_rgba_buffer.create_storage(internal_format,_sz.cx,_sz.cy);
			m_frame_buffer.set_render_buffer(GL_COLOR_ATTACHMENT0,m_rgba_buffer);
		}
		else
		{
			m_rgba_texture.create_mutable_storage_2d_mulisample(_samples,internal_format,_sz.cx,_sz.cy,true);
			m_frame_buffer.set_render_texture(GL_COLOR_ATTACHMENT0,m_rgba_texture,0);
		}

		
		if(_pfd->cDepthBits > 0)
		{
			m_depth_buffer.create();
			switch(_pfd->cDepthBits)
			{
			case 8:
			case 16:
				internal_format = GL_DEPTH_COMPONENT16;
				break;
			case 24:
			case 32:
				internal_format = GL_DEPTH_COMPONENT32;
				break;
			};
			if(EQL(_samples,0))
				m_depth_buffer.create_storage(internal_format,_sz.cx,_sz.cy);
			else
				m_depth_buffer.create_storage_multisample(_samples,internal_format,_sz.cx,_sz.cy);

			m_frame_buffer.set_render_buffer(GL_DEPTH_ATTACHMENT,m_depth_buffer);
		}
		if(_pfd->cStencilBits > 0)
		{
			m_stencil_buffer.create();
			switch(_pfd->cStencilBits)
			{
			case 1:
				internal_format = GL_STENCIL_INDEX1;
				break;
			case 4:
				internal_format = GL_STENCIL_INDEX4;
				break;
			case 8:
				internal_format = GL_STENCIL_INDEX8;
				break;
			case 16:
				internal_format = GL_STENCIL_INDEX16;
				break;
			};
			if(EQL(_samples,0))
				m_stencil_buffer.create_storage(internal_format,_sz.cx,_sz.cy);
			else
				m_stencil_buffer.create_storage_multisample(_samples,internal_format,_sz.cx,_sz.cy);

			m_frame_buffer.set_render_buffer(GL_STENCIL_ATTACHMENT,m_stencil_buffer);
		}
		return EQL(m_frame_buffer.check_status(),GL_FRAMEBUFFER_COMPLETE);
	}

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
};//struct GLDrawingToMemory
