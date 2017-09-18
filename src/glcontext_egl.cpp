#include "glcontext.hpp"
#include <iostream>

using namespace std;
using namespace gl_util;

class glcontext::priv::detail
{
public:

    bool init
    (
        GLint gl_req_major_ver,
        GLint gl_req_minor_ver
    );

    ~detail();

private:

    EGLDisplay display;
};

glcontext::priv::priv():
    pimpl(make_unique<detail>())
{
}

glcontext::priv::~priv()
{
}

bool glcontext::priv::init
(
    GLint gl_req_major_ver,
    GLint gl_req_minor_ver
)
{
    return pimpl->init(gl_req_major_ver, gl_req_minor_ver);
}

namespace
{
    constexpr EGLint attrib_list[] =
    {
        EGL_SURFACE_TYPE,       EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE,    EGL_OPENGL_BIT,
        EGL_NONE
    };
}

bool glcontext::init
(
    GLint gl_req_major_ver,
    GLint gl_req_minor_ver
)
{
    return true;
}

bool glcontext::priv::detail::init
(
	GLint gl_req_major_ver,
	GLint gl_req_minor_ver
)
{
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(display == EGL_NO_DISPLAY)
    {
        cerr << "Failed to eglGetDisplay(EGL_DEFAULT_DISPLAY)" << endl;
        return false;
    }

    EGLint egl_major_ver;
    EGLint egl_minor_ver;
    if(eglInitialize(display, &egl_major_ver, &egl_minor_ver) == EGL_FALSE)
    {
        cerr << "Failed to eglInitialize" << endl;
        EGLint error =  eglGetError();
        switch(error)
        {
        case EGL_BAD_DISPLAY:
            cerr << "display is not an EGL display connection" << endl;
            break;
        case EGL_NOT_INITIALIZED:
            cerr << "display cannot be initialized" << endl;
            break;
        default:
            break;
        }
        return false;
    }
    cout << "EGL version: " << egl_major_ver << "." << egl_minor_ver << endl;

    char const * client_apis = eglQueryString(display, EGL_CLIENT_APIS);
    if(!client_apis)
    {
        cerr << "Failed to eglQueryString(display, EGL_CLIENT_APIS)" << endl;
        return false;
    }
    cout << "Supported client rendering APIs: " << client_apis << endl;

    EGLConfig config;
    EGLint    num_config;
    if(eglChooseConfig(display, attrib_list, &config, 1, &num_config) == EGL_FALSE)
    {
        cerr << "Failed to eglChooseConfig" << endl;
        return false;
    }
    if(num_config < 1)
    {
        cerr << "No matching EGL frame buffer configuration" << endl;
        return false;
    }

    if(eglBindAPI(EGL_OPENGL_API) == EGL_FALSE)
    {
        cerr << "Failed to eglBindAPI(EGL_OPENGL_API)" << endl;
        return false;
    }

    const EGLint context_attrib[] =
    {
        EGL_CONTEXT_MAJOR_VERSION_KHR,  gl_req_major_ver,
        EGL_CONTEXT_MINOR_VERSION_KHR,  gl_req_minor_ver,
        EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR,    EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
        EGL_CONTEXT_FLAGS_KHR,          EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR
#ifndef NDEBUG
        | EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR
#endif
        ,
        EGL_NONE
    };

    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attrib);
    if(context == EGL_NO_CONTEXT)
    {
        cerr << "Failed to eglCreateContext" << endl;
        EGLint error =  eglGetError();
        switch(error)
        {
        case EGL_BAD_CONFIG:
            cerr << "config is not an EGL frame buffer configuration, or does not support the current rendering API" << endl;
            break;
        case EGL_BAD_ATTRIBUTE:
            cerr << "attrib_list contains an invalid context attribute or if an attribute is not recognized or out of range" << endl;
            break;
        default:
            cerr << "Unknown error" << endl;
            break;
        }
        return false;
    }

    if(eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, context) == EGL_FALSE)
    {
        cerr << "Failed to eglMakeCurrent" << endl;
        return false;
    }

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        cerr << "Failed to glewInit\n";
        return false;
    }

    if(EGL_SUCCESS != eglGetError())
    {
        return false;
    }

    return true;
}

void glcontext::uninit()
{
}

glcontext::priv::detail::~detail()
{
    if(eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) == EGL_FALSE)
    {
        cerr << "Failed to eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)" << endl;
    }

    if(eglTerminate(display) == EGL_FALSE)
    {
        cerr << "Failed to eglTerminate" << endl;
    }
}
