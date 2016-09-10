#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#ifdef _WIN32
#	include <GL/wglew.h>
#else
#	include <GL/glxew.h>
#endif

#ifdef GLEW_MX
#	error Not yet implemented.
#endif

#ifdef NDEBUG
#	undef GLEW_GET_FUN
#	define GLEW_GET_FUN(x)	(set_last_gl_call(__FILE__, __LINE__, #x), x)
#	define GL_CALL(x)		(set_last_gl_call(__FILE__, __LINE__, #x), x)
#else
#	define GL_CALL(x)		x
#endif

struct gl_call_info
{
	const	char*	filename = nullptr;
			int		line     = 0;
	const	char*	funcname = nullptr;

	void set(const char* filename, int line, const char* funcname)
	{
		this->filename	= filename;
		this->line		= line;
		this->funcname	= funcname;
	}
};

inline gl_call_info& get_gl_call_info()
{
	static gl_call_info gci;

	return gci;
}

inline void set_last_gl_call(const char* filename, int line, const char* funcname)
{
	get_gl_call_info().set(filename, line, funcname);
}

