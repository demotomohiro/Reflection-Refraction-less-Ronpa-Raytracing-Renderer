#pragma once

#include "gl_common.hpp"
#include <memory>

namespace gl_util
{

struct glcontext
{
	glcontext
	(
		GLint gl_req_major_ver=3,
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

        //Constructor and destructor of unique_ptr cannot be compiled if class detail is incomplete type.
        //So priv() and ~priv() must be defined in the cpp file where class detail is defined.
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

