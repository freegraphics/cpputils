#pragma once

#include <gl/gl.h>
#include <gl/glu.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include <utils/utils.h>
#include <math/graphics.h>


//struct PixelFormatDescription : PIXELFORMATDESCRIPTOR
//{
//};//struct PixelFormatDescription

static const PIXELFORMATDESCRIPTOR pfd_dblbuffer_rgb24_32depth = 
{
	sizeof(PIXELFORMATDESCRIPTOR)   // size of this pfd
	,1								// version number
	,PFD_DRAW_TO_WINDOW|			// support window
	PFD_SUPPORT_OPENGL|				// support OpenGL
	PFD_DOUBLEBUFFER				// double buffered
	,PFD_TYPE_RGBA					// RGBA type
	,24								// 24-bit color depth
	,0, 0, 0, 0, 0, 0				// color bits ignored
	,0								// no alpha buffer
	,0								// shift bit ignored
	,0								// no accumulation buffer
	,0, 0, 0, 0						// accum bits ignored
	,32								// 32-bit z-buffer
	,0								// no stencil buffer
	,0								// no auxiliary buffer
	,PFD_MAIN_PLANE					// main layer
	,0								// reserved
	,0, 0, 0						// layer masks ignored
};


class MFCOpenGLImplBase
{
protected:
	// OpenGL rendering context
	HGLRC m_hRC;
	double m_fProjectiveAngle;
	double m_fAspect;
	double m_fNear;
	double m_fFar;
	PIXELFORMATDESCRIPTOR m_setup_pfd;
protected:
	GLsizei m_width;
	GLsizei m_height;
public:

	MFCOpenGLImplBase()
		:m_hRC(NULL)
		,m_setup_pfd(pfd_dblbuffer_rgb24_32depth)
	{
		m_fProjectiveAngle = 45.0;
		m_fAspect = 1.0;
		m_fNear = 2;
		m_fFar = 1000;
		m_width = 1;
		m_height = 1;
	}

protected:
	int OnCreate(
		LPCREATESTRUCT _pCreateStruct
		,CWnd* _pwnd
		,const PIXELFORMATDESCRIPTOR* _lppixelformatdescription = &pfd_dblbuffer_rgb24_32depth
		)
	{
		VERIFY_EXIT1(NOT_NULL(_pwnd) && NOT_NULL(_pwnd->GetSafeHwnd()),-1);
		VERIFY_EXIT1(NOT_NULL(_lppixelformatdescription),-1);
		m_setup_pfd = *_lppixelformatdescription;
		CDC* pdc = _pwnd->GetDC();
		VERIFY_EXIT1(NOT_NULL(pdc),-1);
		CRect client_rect;
		_pwnd->GetWindowRect(&client_rect);
		CreateContext(pdc->GetSafeHdc(),client_rect);
		return 0;
	}

	void OnDestroy()
	{
		wglMakeCurrent(NULL, NULL);

		if (m_hRC)
		{
			wglDeleteContext(m_hRC);
			m_hRC = NULL;
		}
	}

	void OnSize(
		UINT _nType,const CSize& _sz
		,CWnd* _pwnd
		)
	{
		VERIFY_EXIT(NOT_NULL(_pwnd) && NOT_NULL(_pwnd->GetSafeHwnd()));
		// when resized, recreate the device context
		wglMakeCurrent(NULL, NULL);
		if(m_hRC)
		{
			wglDeleteContext(m_hRC);
			m_hRC = NULL;
		}
		CDC* pdc = _pwnd->GetDC();
		VERIFY_EXIT(NOT_NULL(pdc));
		HDC hdc = pdc->GetSafeHdc();
		CRect rc;
		_pwnd->GetClientRect(&rc);
		CreateContext(hdc,rc);
	}

	void OnDraw()
	{
		DrawPrePost dpp(*this);
		opengl_draw();
	}

private:
	// Set OpenGL pixel format for given DC
	BOOL SetupPixelFormat(HDC _hdc)
	{
		PIXELFORMATDESCRIPTOR pfd = m_setup_pfd;
		int pixelformat = 0;
		if ((pixelformat = ChoosePixelFormat(_hdc, &pfd)) == 0)
		{
			ATLASSERT(FALSE);
			return FALSE;
		}

		if (SetPixelFormat(_hdc, pixelformat, &pfd) == FALSE)
		{
			ATLASSERT(FALSE);
			return FALSE;
		}

		return TRUE;
	}

	// Create rendering context given device context and control bounds
	void CreateContext(HDC _hdc,const CRect& _rc)
	{
		//VERIFY_EXIT(NOT_NULL(m_hRC));
		PIXELFORMATDESCRIPTOR pfd;
		if(!SetupPixelFormat(_hdc))
			return;

		int n = GetPixelFormat(_hdc);
		DescribePixelFormat(_hdc, n, sizeof(pfd), &pfd);
		m_hRC = wglCreateContext(_hdc);
		wglMakeCurrent(_hdc, m_hRC);

		resize(_rc.Width(),_rc.Height());
	}

	void predraw()
	{
		//wglMakeCurrent(GetDC(), m_hRC);	
	}

	void postdraw()
	{
		SwapBuffers(wglGetCurrentDC());
	}

protected:
	// OpenGL interface functions starts here

	virtual void opengl_draw() = 0;
	virtual void resize(GLsizei _width, GLsizei _height) = 0;
	virtual void init(GLsizei _width, GLsizei _height) = 0;

	struct DrawPrePost
	{
		MFCOpenGLImplBase& m_openglwnd;

		DrawPrePost(MFCOpenGLImplBase& _openglwnd)
			:m_openglwnd(_openglwnd)
		{
			m_openglwnd.predraw();
		}
		~DrawPrePost()
		{
			m_openglwnd.postdraw();
		}
	};
};//class MFCOpenGLImplBase

namespace opengl20
{
	struct MFCOpenGL20Impl : public MFCOpenGLImplBase
	{
		MFCOpenGL20Impl()
		{
		}
	protected:
		void GetOGLPos(int _x, int _y,double _zbase,CFPoint3D& _pt)
		{
			GLint viewport[4];
			GLdouble modelview[16];
			GLdouble projection[16];
			GLfloat winX = 0, winY = 0, winZ = 0;
			GLdouble posX = 0, posY = 0, posZ = 0;

			glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
			glGetDoublev( GL_PROJECTION_MATRIX, projection );
			glGetIntegerv( GL_VIEWPORT, viewport );

			winX = (GLfloat)_x;
			winY = (GLfloat)viewport[3] - (GLfloat)_y;
			//glReadPixels( _x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
			//winZ = (GLfloat)_zbase;

			gluUnProject( winX, winY, 1.0f, modelview, projection, viewport, &posX, &posY, &posZ);

			_pt.x = posX*_zbase/m_fFar;
			_pt.y = posY*_zbase/m_fFar;
			_pt.z = 0;
		}

		virtual void resize(GLsizei _width, GLsizei _height) 
		{
			if (_height==0)		// Предотвращение деления на ноль, если окно слишком мало
				_height=1;

			glViewport(0, 0, _width, _height);
			// Сброс текущей области вывода и перспективных преобразований

			glMatrixMode(GL_PROJECTION);// Выбор матрицы проекций
			glLoadIdentity();			// Сброс матрицы проекции

			m_width = _width;
			m_height = _height;
			m_fAspect = (double)m_width/m_height;
			gluPerspective((GLfloat)m_fProjectiveAngle,(GLfloat)m_fAspect,(GLfloat)m_fNear,(GLfloat)m_fFar);
			// Вычисление соотношения геометрических размеров для окна
			glMatrixMode(GL_MODELVIEW);	// Выбор матрицы просмотра модели
		}

		virtual void init(GLsizei _width, GLsizei _height)
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			// Очистка экрана в черный цвет
			glClearDepth(1.0);		// Разрешить очистку буфера глубины
			glDepthFunc(GL_LESS);	// Тип теста глубины
			glEnable(GL_DEPTH_TEST);// разрешить тест глубины
			glShadeModel(GL_SMOOTH);// разрешить плавное цветовое сглаживание
			glMatrixMode(GL_PROJECTION);// Выбор матрицы проекции
			glLoadIdentity();		// Сброс матрицы проекции
			m_width = _width;
			m_height = _height;
			m_fAspect = (double)m_width/m_height;
			gluPerspective((GLfloat)m_fProjectiveAngle,(GLfloat)m_fAspect,(GLfloat)m_fNear,(GLfloat)m_fFar);
			// Вычислить соотношение геометрических размеров для окна
			glMatrixMode(GL_MODELVIEW);// Выбор матрицы просмотра модели
		}
	};//struct MFCOpenGL20Impl

	inline
	void vertex(const CFPoint3D& _pt)
	{
		glVertex3d(_pt.x,_pt.y,_pt.z);
	}

	inline
	void line(const CFPoint3D& _p1,const CFPoint3D& _p2)
	{
		vertex(_p1);
		vertex(_p2);
	}

};//namespace opengl20

namespace opengl40
{
	struct MFCOpenGL4Impl : public MFCOpenGLImplBase
	{
		MFCOpenGL4Impl()
		{
		}
	protected:
		void GetOGLPos(int _x, int _y,double _zbase,CFPoint3D& _pt)
		{
			VERIFY_EXIT(FALSE);// not implemented
		}

		virtual void resize(GLsizei _width, GLsizei _height) 
		{
			VERIFY_EXIT(FALSE);// not implemented
		}

		virtual void init(GLsizei _width, GLsizei _height)
		{
			VERIFY_EXIT(FALSE);// not implemented
		}
	};//struct MFCOpenGL4Impl

};//namespace opengl40
