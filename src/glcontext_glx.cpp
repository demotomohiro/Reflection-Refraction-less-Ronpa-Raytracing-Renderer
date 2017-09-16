#include "glcontext.hpp"
#include <cstring>
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

	Display		*display = nullptr;
	Colormap	cmap = 0;
	Window 		win = 0;
	GLXContext	ctx = nullptr;
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

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
static bool isExtensionSupported(const char *extList, const char *extension)
{
	const char *start;
	const char *where, *terminator;

	/* Extension names should not have spaces. */
	where = strchr(extension, ' ');
	if ( where || *extension == '\0' )
		return false;

	/* It takes a bit of care to be fool-proof about parsing the
	   OpenGL extensions string. Don't be fooled by sub-strings,
	   etc. */
	for ( start = extList; ; ) {
		where = strstr( start, extension );

		if ( !where )
			break;

		terminator = where + strlen( extension );

		if ( where == start || *(where - 1) == ' ' )
			if ( *terminator == ' ' || *terminator == '\0' )
				return true;

		start = terminator;
	}

	return false;
}

static bool ctxErrorOccurred = false;
static int ctxErrorHandler( Display *dpy, XErrorEvent *ev )
{
    ctxErrorOccurred = true;
    return 0;
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
	ctx = 0;

	display = XOpenDisplay(0);

	if(!display)
	{
		cerr << "Failed to open X display" << endl;
		return false;
	}

	int glx_major, glx_minor;
	if(!glXQueryVersion(display, &glx_major, &glx_minor))
	{
		cerr << "Invaild GLX version" << endl;
		return false;
	}

	if( ((glx_major == 1) && (glx_minor < 3)) || glx_major < 1)
	{
		cerr << "Invaild GLX version" << endl;
		return false;
	}

	PFNGLXCHOOSEFBCONFIGPROC glXChooseFBConfig = 0;
	glXChooseFBConfig = (PFNGLXCHOOSEFBCONFIGPROC)glXGetProcAddressARB( (const GLubyte *) "glXChooseFBConfig" );
	if(!glXChooseFBConfig)
	{
		cerr << "glXChooseFBConfig is not available" << endl;
		return false;
	}

	PFNGLXGETVISUALFROMFBCONFIGPROC glXGetVisualFromFBConfig = 0;
	glXGetVisualFromFBConfig =
		(PFNGLXGETVISUALFROMFBCONFIGPROC)glXGetProcAddressARB( (const GLubyte *) "glXGetVisualFromFBConfig");
	if(!glXGetVisualFromFBConfig)
	{
		cerr << "glXGetVisualFromFBConfig is not available" << endl;
		return false;
	}

	PFNGLXGETFBCONFIGATTRIBPROC glXGetFBConfigAttrib = 0;
	glXGetFBConfigAttrib =
		(PFNGLXGETFBCONFIGATTRIBPROC)glXGetProcAddressARB(
			(const GLubyte *) "glXGetFBConfigAttrib"); 
	if(!glXGetFBConfigAttrib)
	{
		cerr << "glXGetFBConfigAttrib is not available" << endl;
		return false;
	}

	static int visual_attribs[] =
	{
		GLX_X_RENDERABLE	,	True			,
		GLX_DRAWABLE_TYPE	,	GLX_WINDOW_BIT	,
		GLX_RENDER_TYPE		,	GLX_RGBA_BIT	,
		GLX_X_VISUAL_TYPE	,	GLX_TRUE_COLOR	,
		GLX_RED_SIZE		,	8				,
		GLX_GREEN_SIZE		,	8				,
		GLX_BLUE_SIZE		,	8				,
		GLX_ALPHA_SIZE		,	8				,
		GLX_DEPTH_SIZE		,	24				,
		GLX_STENCIL_SIZE	,	8				,
		GLX_DOUBLEBUFFER	,	True			,
		None
	};

	int fbcount;
	GLXFBConfig *fbc =
		glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
	if(!fbc)
	{
		cerr << "Failed to retrieve a framebuffer config" << endl;
		return false;
	}

	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

	int i;
	for ( i = 0; i < fbcount; i++ )
	{
		XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
		if ( vi )
		{
			int samp_buf, samples;
			glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
			glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES       , &samples  );

			cout <<
				"Matching fbconfig " << i <<
				", visual ID 0x" << std::hex << vi ->visualid << std::dec <<
				": SAMPLE_BUFFERS = " << samp_buf <<
				", SAMPLES = " << samples << endl;

			if ( best_fbc < 0 || samp_buf && samples > best_num_samp )
				best_fbc = i, best_num_samp = samples;
			if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
				worst_fbc = i, worst_num_samp = samples;
		}
		XFree( vi );
	}

	GLXFBConfig bestFbc = fbc[ best_fbc ];
	XFree( fbc );

	// Get a visual
	XVisualInfo *vi = glXGetVisualFromFBConfig( display, bestFbc );
	cout << "Chosen visual ID = 0x" << std::hex << vi->visualid << std::dec << endl;

	XSetWindowAttributes swa;
	swa.colormap = cmap =
		XCreateColormap(
				display,
				RootWindow( display, vi->screen ), 
				vi->visual, AllocNone );
	swa.background_pixmap = None;
	swa.border_pixel      = 0;
	swa.event_mask        = StructureNotifyMask;

	win =
		XCreateWindow(
				display, RootWindow( display, vi->screen ), 
				0, 0, 100, 100, 0, vi->depth, InputOutput, 
				vi->visual, 
				CWBorderPixel|CWColormap|CWEventMask, &swa );
	if ( !win )
	{
		cerr << "Failed to create window." << endl;
		return false;
	}

	// Done with the visual info data
	XFree( vi );

	XStoreName( display, win, "GL 3.3 Window" );

	XMapWindow( display, win );

	// Get the default screen's GLX extension list
	const char *glxExts = glXQueryExtensionsString( display, DefaultScreen( display ) );

	// NOTE: It is not necessary to create or make current to a context before
	// calling glXGetProcAddressARB
	PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)
		glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

	// Install an X error handler so the application won't exit if GL 3.0
	// context allocation fails.
	//
	// Note this error handler is global.  All display connections in all threads
	// of a process use the same error handler, so be sure to guard against other
	// threads issuing X commands while this code is running.
	ctxErrorOccurred = false;
	int (*oldHandler)(Display*, XErrorEvent*) =
		XSetErrorHandler(&ctxErrorHandler);

	// Check for the GLX_ARB_create_context extension string and the function.
	// If either is not present, use GLX 1.3 context creation method.
	if ( !isExtensionSupported( glxExts, "GLX_ARB_create_context" ) ||
			!glXCreateContextAttribsARB )
	{
		cerr << "glXCreateContextAttribsARB() not found"
				" ... using old-style GLX context" << endl;
		ctx = glXCreateNewContext( display, bestFbc, GLX_RGBA_TYPE, 0, True );
	}

	// If it does, try to get a GL 3.0 context!
	else
	{
		int context_attribs[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, gl_req_major_ver,
			GLX_CONTEXT_MINOR_VERSION_ARB, gl_req_minor_ver,
			GLX_CONTEXT_FLAGS_ARB        ,
            GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | 
#ifdef NDEBUG
            0
#else
            GLX_CONTEXT_DEBUG_BIT_ARB
#endif
            ,
            GLX_CONTEXT_PROFILE_MASK_ARB , GLX_CONTEXT_CORE_PROFILE_BIT_ARB, 
			None
		};

		ctx =
			glXCreateContextAttribsARB(
				display, bestFbc, 0, True, context_attribs );

		// Sync to ensure any errors generated are processed.
		XSync( display, False );
		if ( ctxErrorOccurred || !ctx )
		{
			// Couldn't create GL 3.0 context.  Fall back to old-style 2.x context.
			// When a context version below 3.0 is requested, implementations will
			// return the newest context version compatible with OpenGL versions less
			// than version 3.0.
			// GLX_CONTEXT_MAJOR_VERSION_ARB = 1
			context_attribs[1] = 1;
			// GLX_CONTEXT_MINOR_VERSION_ARB = 0
			context_attribs[3] = 0;

			ctxErrorOccurred = false;

			cerr << "Failed to create GL 3.0 context"
					" ... using old-style GLX context" << endl;
			ctx =
				glXCreateContextAttribsARB( display, bestFbc, 0, True, context_attribs );
		}
	}

	// Sync to ensure any errors generated are processed.
	XSync( display, False );

	// Restore the original error handler
	XSetErrorHandler( oldHandler );

	if ( ctxErrorOccurred || !ctx )
	{
		cerr << "Failed to create an OpenGL context" << endl;
		return false;
	}

	// Verifying that context is a direct context
	if ( ! glXIsDirect ( display, ctx ) )
		cerr << "Indirect GLX rendering context obtained" << endl;
	else
		cout << "Direct GLX rendering context obtained" <<endl;

	glXMakeCurrent( display, win, ctx );

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cerr << "Failed to glewInit\n";
		return false;
	}

	while((err=glGetError())!=GL_NO_ERROR)
	{
		cerr << "gl error from glewInit()" << endl;
	}

	return true;
}

void glcontext::uninit()
{
}

glcontext::priv::detail::~detail()
{
    if(!display)
        return;

    if(ctx)
    {
        glXMakeCurrent( display, 0, 0 );
        glXDestroyContext( display, ctx );
    }

    if(win)
        XDestroyWindow( display, win );

    if(cmap)
        XFreeColormap( display, cmap );

	XCloseDisplay( display );
}
