#pragma once

#include <GL/glew.h>
#ifndef USE_EGL
#ifdef _WIN32
#	include <GL/wglew.h>
#else
#	include <GL/glxew.h>
#endif
#endif

#ifdef GLEW_MX
#	error Not yet implemented.
#endif

#include "GLUtil/contextUtil.hpp"

