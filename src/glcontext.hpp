#pragma once

#include "gl_common.hpp"
#include <memory>

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

	bool	isSuccess;

private:

	bool init
	(
		GLint gl_req_major_ver,
		GLint gl_req_minor_ver
	);
	void uninit();

    class priv
    {
    public:

        priv();
        ~priv();

        bool init
        (
            GLint gl_req_major_ver,
            GLint gl_req_minor_ver
        );

    private:

        class detail;
        std::unique_ptr<detail>   pimpl;
    };

    priv impl;
};

}

