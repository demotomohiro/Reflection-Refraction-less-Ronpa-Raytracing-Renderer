#pragma once

#include "gl_common.hpp"

#ifdef USE_EGL
#   define EGL_EGLEXT_PROTOTYPES
#   include <EGL/egl.h>
#   include <EGL/eglext.h>
#endif

namespace gl_util
{

struct glcontext
{
	glcontext
	(
		GLint gl_req_major_ver=4,
		GLint gl_req_minor_ver=3
	);

	~glcontext();

	bool get_is_success() const
	{
		return isSuccess;
	}

#if 0
	void draw() const
	{
		SwapBuffers(hdc);
	}
#endif

	bool	isSuccess;

private:

	bool init
	(
		GLint gl_req_major_ver,
		GLint gl_req_minor_ver
	);
	void uninit();

#ifdef USE_EGL
    EGLDisplay display;
#else
#ifdef _WIN32
	HWND	hWnd;
	HDC		hdc;
	HGLRC	hglrc;
#else
	Display		*display = nullptr;
	Colormap	cmap = 0;
	Window 		win = 0;
	GLXContext	ctx = nullptr;
#endif
#endif
};

}

