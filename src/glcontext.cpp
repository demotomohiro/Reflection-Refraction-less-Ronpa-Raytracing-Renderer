#include <iostream>
#include "glcontext.hpp"

using namespace std;
using namespace gl_util;

namespace
{
bool check_gl_ver
(
	GLint gl_req_major_ver,
	GLint gl_req_minor_ver
)
{
	GLint major_ver;
	GLint minor_ver;
	glGetIntegerv(GL_MAJOR_VERSION, &major_ver);
	glGetIntegerv(GL_MINOR_VERSION, &minor_ver);

	if(major_ver < gl_req_major_ver || (major_ver==gl_req_major_ver && minor_ver<gl_req_minor_ver))
	{
		return false;
	}

	return true;
}
}

glcontext::glcontext
(
	GLint gl_req_major_ver,
	GLint gl_req_minor_ver
):
	isSuccess(false)
{
	if(!init(gl_req_major_ver, gl_req_minor_ver))
	{
		return;
	}

	if(!check_gl_ver(gl_req_major_ver, gl_req_minor_ver))
	{
		cerr << "OpenGL " << gl_req_major_ver << "." << gl_req_minor_ver << " or heigher is required\n";
        return;
	}

	isSuccess = true;
}

glcontext::~glcontext()
{
	uninit();
}

