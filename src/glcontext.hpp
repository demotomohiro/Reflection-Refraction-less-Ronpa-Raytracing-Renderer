#pragma once

#include "gl_common.hpp"
#include <memory>

namespace gl_util
{

struct glcontext
{
	glcontext
	(
		GLint gl_req_major_ver=4,
		GLint gl_req_minor_ver=3
	);

	bool get_is_success() const
	{
		return isSuccess;
	}

	bool	isSuccess;

private:

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

