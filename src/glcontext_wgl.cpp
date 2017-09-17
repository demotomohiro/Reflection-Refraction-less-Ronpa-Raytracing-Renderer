#include "glcontext.hpp"

#include <tchar.h>
#include <iostream>
#include <GL/wglew.h>

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

    HWND	hWnd;
    HDC		hdc;
    HGLRC	hglrc;
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
    hWnd = 0;
    hdc = 0;
    hglrc = 0;

    hWnd =
        CreateWindowEx(
            WS_EX_APPWINDOW, _T("STATIC"),
            _T("dummy"), WS_POPUP, 0, 0,
            640, 480, NULL, NULL, GetModuleHandle(0), NULL);

    if(!hWnd)
    {
        cerr << "Failed to CreateWindowEx" << endl;
        return false;
    }

//  ShowWindow(hWnd, SW_SHOW);

    hdc = GetDC(hWnd);

    const static PIXELFORMATDESCRIPTOR pfd = { 
        sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd
        1,                     // version number
        PFD_DRAW_TO_WINDOW |   // support window
        PFD_SUPPORT_OPENGL |   // support OpenGL
        PFD_DOUBLEBUFFER,      // double buffered
        PFD_TYPE_RGBA,         // RGBA type
        32,                    // color depth
        0, 0, 0, 0, 0, 0,      // color bits ignored
        0,                     // no alpha buffer
        0,                     // shift bit ignored
        0,                     // no accumulation buffer
        0, 0, 0, 0,            // accum bits ignored
        0, //24,                    // z-buffer bits
        0,                     // stencil buffer bits
        0,                     // no auxiliary buffer  
        PFD_MAIN_PLANE,        // main layer
        0,                     // reserved
        0, 0, 0                // layer masks ignored
    };

    int  iPixelFormat = ChoosePixelFormat(hdc, &pfd);
    if(!iPixelFormat)
    {
        return false;
    }

    if(SetPixelFormat(hdc, iPixelFormat, &pfd)==FALSE)
    {
        return false;
    }

    HGLRC tmpCntxt = wglCreateContext(hdc);
    if(tmpCntxt==0)
    {
        cerr << "Failed to wglCreateContext\n";
        return false;
    }

    wglMakeCurrent(hdc, tmpCntxt);

#if 0
    const static int attribList[] =
    {
        WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        0
    };
    int pixelFormat, numFormats;
    if(
        wglChoosePixelFormatARB(
            hdc, attribList, NULL, 1, &pixelFormat, &numFormats)
        ==
        FALSE)
    {
        cerr << "Failed to wglChoosePixelFormatARB" << endl;
        return false;
    }

    if(SetPixelFormat(hdc, pixelFormat, &pfd)==FALSE)
    {
        return false;
    }
#endif

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        cerr << "Failed to glewInit\n";
        return false;
    }

    const static int attribList[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, gl_req_major_ver,
        WGL_CONTEXT_MINOR_VERSION_ARB, gl_req_minor_ver,
        WGL_CONTEXT_FLAGS_ARB,
        WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB |
#ifdef NDEBUG
        0
#else
        WGL_CONTEXT_DEBUG_BIT_ARB
#endif
        ,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    if(wglewIsSupported("WGL_ARB_create_context") == 1)
    {
        hglrc = wglCreateContextAttribsARB(hdc, 0, attribList);
        if(hglrc == 0)
        {
            cerr << "Failed to wglCreateContextAttribsARB\n";
            wglMakeCurrent(NULL,NULL);
            wglDeleteContext(tmpCntxt);
            return false;
        }

        wglMakeCurrent(NULL,NULL);
        wglDeleteContext(tmpCntxt);
        wglMakeCurrent(hdc, hglrc);
    }else
    {
        hglrc = tmpCntxt;
    }

    return true;
}

void glcontext::uninit()
{
}

glcontext::priv::detail::~detail()
{
    wglMakeCurrent(NULL, NULL);

    if(hglrc)
        wglDeleteContext(hglrc);

    if(hdc)
        ReleaseDC(hWnd, hdc);

    if(hWnd)
        DestroyWindow(hWnd);
}
