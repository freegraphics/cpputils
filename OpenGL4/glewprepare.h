#pragma once 

#include "utils.h"
#include "simple.h"

//#include <afxglobals.h>


struct GLEWPrepare
{
protected:
	bool m_initialed_flag;

protected:
	GLEWPrepare()
		:m_initialed_flag(false)
	{
		m_initialed_flag = init();
	}

	bool init()
	{
		const LPCTSTR simple_opengl_calss_name = _T("SimpleOpenGLClass");
		CString class_name = AfxRegisterWndClass(CS_DBLCLKS|CS_OWNDC,::LoadCursor(NULL,IDC_ARROW));
		HWND hWndFake = CreateWindow(
			class_name, _T("FAKE")
			,WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN
			,0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL
			,NULL, GetModuleHandle(NULL), NULL
			);
		if(IS_NULL(hWndFake))
			return false;

		HDC hDC = GetDC(hWndFake);
		if(IS_NULL(hDC))
		{
			DestroyWindow(hWndFake); 
			false;
		}

		// First, choose false pixel format

		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize= sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion   = 1;
		pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 16;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int iPixelFormat = ChoosePixelFormat(hDC, &pfd);
		if (iPixelFormat == 0)
		{
			DestroyWindow(hWndFake); 
			false;
		}

		if(!SetPixelFormat(hDC, iPixelFormat, &pfd))
		{
			DestroyWindow(hWndFake); 
			return false;
		}

		// Create the false, old style context (OpenGL 2.1 and before)

		HGLRC hRCFake = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRCFake);

		bool bResult = true;

		if(glewInit() != GLEW_OK)
			bResult = false;

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hRCFake);
		DestroyWindow(hWndFake); 

		return bResult;
	}

public:
	static bool make()
	{
		static GLEWPrepare _;
		return _.m_initialed_flag;
	}
};