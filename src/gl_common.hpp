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

#include "GLUtil/contextUtil.hpp"

