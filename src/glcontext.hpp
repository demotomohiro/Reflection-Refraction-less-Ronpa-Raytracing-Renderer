#pragma once

#include "gl_common.hpp"

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

#ifdef _WIN32
	HWND	hWnd;
	HDC		hdc;
	HGLRC	hglrc;
#else
	Display		*display;
	Colormap	cmap;
	Window 		win;
	GLXContext	ctx;
#endif
};

}

