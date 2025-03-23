/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
/*
** WIN_GLIMP.C
**
** This file contains ALL Win32 specific stuff having to do with the
** OpenGL refresh.  When a port is being made the following functions
** must be implemented by the port:
**
** GLimp_EndFrame
** GLimp_Init
** GLimp_LogComment
** GLimp_Shutdown
**
** Note that the GLW_xxx functions are Windows specific GL-subsystem
** related functions that are relevant ONLY to win_glimp.c
*/

#include "../renderer/tr_local.h"
#include "../win32/win_local.h"

extern void WG_CheckHardwareGamma(void);
extern void WG_RestoreGamma(void);

typedef enum
{
	RSERR_OK,
	RSERR_INVALID_FULLSCREEN,
	RSERR_INVALID_MODE,
	RSERR_UNKNOWN
} rserr_t;

#define TRY_PFD_SUCCESS		0
#define TRY_PFD_FAIL_SOFT	1
#define TRY_PFD_FAIL_HARD	2

#define	WINDOW_CLASS_NAME	"Quake 3: Arena"

// function declaration
static rserr_t
GLW_SetMode(const char* drivername, int mode, int colorbits, bool cdsFullscreen);
void QGL_EnableLogging( bool enable );
bool QGL_Init( const char *dllname );
void QGL_Shutdown( void );

// variable declarations
glwstate_t glw_state;

cvar_t	*r_allowSoftwareGL;		// don't abort out if the pixelformat claims software
cvar_t	*r_maskMinidriver;		// allow a different dll name to be treated as if it were opengl32.dll

/*
** GLW_StartDriverAndSetMode
*/
static bool
GLW_StartDriverAndSetMode(const char* drivername, int mode, int colorbits, bool cdsFullscreen)
{
	rserr_t err;

	err = GLW_SetMode(drivername, r_mode->integer, colorbits, cdsFullscreen);

	switch (err)
	{
	case RSERR_INVALID_FULLSCREEN:
		ri.Printf(PRINT_ALL, "...WARNING: fullscreen unavailable in this mode\n");
		return false;
	case RSERR_INVALID_MODE:
		ri.Printf(PRINT_ALL, "...WARNING: could not set the given mode (%d)\n", mode);
		return false;
	default:
		break;
	}

	return true;
}

/*
** GLW_MakeContext
*/
static int GLW_MakeContext(int colorbits, int depthbits, int stencilbits, bool stereo)
{
	int pixelformat = 0;

	// don't putz around with pixelformat if it's already set (e.g. this is a soft
	// reset of the graphics system)
	if (!glw_state.pixelFormatSet) {

		// choose, set, and describe our desired pixel format.  If we're
		// using a minidriver then we need to bypass the GDI functions,
		// otherwise use the GDI functions.

		if (SDL_GL_SetAttribute(SDL_GL_STEREO, stereo) == false) {
			ri.Printf(PRINT_ALL, "...GLW_ChoosePFD failed\n");
			return TRY_PFD_FAIL_SOFT;
		}

		if (SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, colorbits) == false) {
			ri.Printf(PRINT_ALL, "...GLW_ChoosePFD failed\n");
			return TRY_PFD_FAIL_SOFT;
		}
		
		if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depthbits) == false) {
			ri.Printf(PRINT_ALL, "...GLW_ChoosePFD failed\n");
			return TRY_PFD_FAIL_SOFT;
		}

		if (SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencilbits) == false) {
			ri.Printf(PRINT_ALL, "...GLW_ChoosePFD failed\n");
			return TRY_PFD_FAIL_SOFT;
		}

		ri.Printf(PRINT_ALL, "...PIXELFORMAT %d selected\n", pixelformat);

		glw_state.pixelFormatSet = true;
	}

	// startup the OpenGL subsystem by creating a context and making it current
	if (!glw_state.hGLRC) {

		ri.Printf(PRINT_ALL, "...creating GL context: ");

		if ((glw_state.hGLRC = SDL_GL_CreateContext(g_wv.hWnd)) == 0) {

			ri.Printf(PRINT_ALL, "failed, %s\n", SDL_GetError());
			return TRY_PFD_FAIL_HARD;
		}

		ri.Printf(PRINT_ALL, "succeeded\n");
		ri.Printf(PRINT_ALL, "...making context current: ");

		if (!SDL_GL_MakeCurrent(g_wv.hWnd, glw_state.hGLRC)) {

			SDL_GL_DestroyContext(glw_state.hGLRC);
			glw_state.hGLRC = NULL;
			ri.Printf(PRINT_ALL, "failed\n");
			
			return TRY_PFD_FAIL_HARD;
		}

		ri.Printf(PRINT_ALL, "succeeded\n");
	}

	return TRY_PFD_SUCCESS;
}

/*
** GLW_InitDriver
**
** - get a DC if one doesn't exist
** - create an HGLRC if one doesn't exist
*/
static bool GLW_InitDriver(const char* drivername, int colorbits)
{
	int		tpfd;
	int		depthbits, stencilbits;

	ri.Printf(PRINT_ALL, "Initializing OpenGL driver\n");

	if (colorbits == 0) {
		colorbits = glw_state.desktopBitsPixel;
	}

	// implicitly assume Z-buffer depth == desktop color depth
	if (r_depthbits->integer == 0) {
		if (colorbits > 16) {
			depthbits = 24;
		} else {
			depthbits = 16;
		}
	} else {
		depthbits = r_depthbits->integer;
	}

	// do not allow stencil if Z-buffer depth likely won't contain it
	stencilbits = r_stencilbits->integer;
	if (depthbits < 24) {
		stencilbits = 0;
	}

	// make two attempts to set the PIXELFORMAT

	// first attempt: r_colorbits, depthbits, and r_stencilbits
	if (!glw_state.pixelFormatSet) {

		if ((tpfd = GLW_MakeContext(colorbits, depthbits, stencilbits, r_stereo->integer)) !=
			TRY_PFD_SUCCESS) {
			if (tpfd == TRY_PFD_FAIL_HARD) {

				ri.Printf(PRINT_WARNING, "...failed hard\n");
				return false;
			}

			// if we've already tried the desktop bit depth and no stencil bits
			if ((r_colorbits->integer == glw_state.desktopBitsPixel) && (stencilbits == 0)) {

				ri.Printf(PRINT_ALL, "...failed to find an appropriate PIXELFORMAT\n");
				return false;
			}

			// second attempt: desktop's color bits and no stencil
			if (colorbits > glw_state.desktopBitsPixel) {
				colorbits = glw_state.desktopBitsPixel;
			}

			stencilbits = 0;
			if (GLW_MakeContext(colorbits, depthbits, stencilbits, r_stereo->integer) !=
				TRY_PFD_SUCCESS) {

				ri.Printf(PRINT_ALL, "...failed to find an appropriate PIXELFORMAT\n");

				return false;
			}
		}

		// report if stereo is desired but unavailable
		int stereo = 0;
		SDL_GL_GetAttribute(SDL_GL_STEREO, &stereo);
		if (!(stereo) && (r_stereo->integer != 0)) {
			ri.Printf(PRINT_ALL, "...failed to select stereo pixel format\n");
			glConfig.stereoEnabled = false;
		}
	}

	// store PFD specifics
	glConfig.colorBits = (int)colorbits;
	glConfig.depthBits = (int)depthbits;
	glConfig.stencilBits = (int)stencilbits;

	return true;
}

/*
** SDL_CreateWindow
**
** Responsible for creating the window and initializing the OpenGL driver.
*/
static bool
GLW_CreateWindow(const char* drivername, int width, int height, int colorbits,
	bool cdsFullscreen)
{
	cvar_t* vid_xpos;
	cvar_t* vid_ypos;
	int stylebits;
	int x, y, w, h;

	// create the HWND if one does not already exist
	if (!g_wv.hWnd) {

		// compute width and height
		x = 0;
		y = 0;
		w = width;
		h = height;

		stylebits = SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL;

		if (cdsFullscreen) {
			stylebits |= SDL_WINDOW_FULLSCREEN;
		}

		if (cdsFullscreen) {
			x = 0;
			y = 0;
		} else {

			vid_xpos = ri.Cvar_Get("vid_xpos", "", 0);
			vid_ypos = ri.Cvar_Get("vid_ypos", "", 0);
			x = vid_xpos->integer;
			y = vid_ypos->integer;

			// adjust window coordinates if necessary 
			// so that the window is completely on screen
			if (x < 0)
				x = 0;
			if (y < 0)
				y = 0;

			if (w < glw_state.desktopWidth &&
				h < glw_state.desktopHeight) {

				if (x + w > glw_state.desktopWidth)
					x = (glw_state.desktopWidth - w);
				if (y + h > glw_state.desktopHeight)
					y = (glw_state.desktopHeight - h);
			}
		}

		SDL_PropertiesID props = SDL_CreateProperties();
		SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, WINDOW_CLASS_NAME);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, x);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, y);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, w);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, h);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, stylebits);
		g_wv.hWnd = SDL_CreateWindowWithProperties(props);
		SDL_DestroyProperties(props);

		if (!g_wv.hWnd) {
			ri.Error(ERR_FATAL, "GLW_CreateWindow() - Couldn't create window");
		}

		SDL_ShowWindow(g_wv.hWnd);
		ri.Printf(PRINT_ALL, "...created window@%d,%d (%dx%d)\n", x, y, w, h);
	} else {
		ri.Printf(PRINT_ALL, "...window already present, CreateWindowEx skipped\n");
	}

	if (!GLW_InitDriver(drivername, colorbits)) {

		SDL_HideWindow(g_wv.hWnd);
		SDL_DestroyWindow(g_wv.hWnd);
		g_wv.hWnd = NULL;

		return false;
	}

	SDL_RaiseWindow(g_wv.hWnd);
	SDL_StartTextInput(g_wv.hWnd);

	return true;
}

/*
** GLW_SetMode
*/
static rserr_t
GLW_SetMode(const char* drivername, int mode, int colorbits, bool cdsFullscreen)
{
	const char* win_fs[] = { "W", "FS" };

	// print out informational messages
	ri.Printf(PRINT_ALL, "...setting mode %d:", mode);

	if (!R_GetModeInfo(&glConfig.vidWidth, &glConfig.vidHeight, &glConfig.windowAspect,
		mode)) {

		ri.Printf(PRINT_ALL, " invalid mode\n");

		return RSERR_INVALID_MODE;
	}

	ri.Printf(PRINT_ALL, " %d %d %s\n", glConfig.vidWidth, glConfig.vidHeight,
		win_fs[cdsFullscreen]);

	// check our desktop attributes
	SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
	const SDL_DisplayMode* displayMode = SDL_GetDesktopDisplayMode(displayID);

	if (displayMode == NULL)
	{
		ri.Printf(PRINT_ALL, "SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());

		return RSERR_UNKNOWN;
	}

	const SDL_PixelFormatDetails* pixelFormatDetails =
		SDL_GetPixelFormatDetails(displayMode->format);
	
	if (pixelFormatDetails == NULL)
	{
		ri.Printf(PRINT_ALL, "SDL_GetPixelFormatDetails failed: %s", SDL_GetError());

		return RSERR_UNKNOWN;
	}

	glw_state.desktopBitsPixel = pixelFormatDetails->bits_per_pixel;
	glw_state.desktopWidth = displayMode->w;
	glw_state.desktopHeight = displayMode->h;

	// do a CDS if needed
	if (cdsFullscreen) {
		if (!GLW_CreateWindow(drivername, glConfig.vidWidth, glConfig.vidHeight, colorbits,
			true)) {
			return RSERR_INVALID_MODE;
		}

		glw_state.cdsFullscreen = true;
	} else {
		glw_state.cdsFullscreen = false;

		if (!GLW_CreateWindow(drivername, glConfig.vidWidth, glConfig.vidHeight, colorbits,
			false)) {
			return RSERR_INVALID_MODE;
		}
	}

	glConfig.displayFrequency = displayMode->refresh_rate;
	glConfig.isFullscreen = cdsFullscreen;

	return RSERR_OK;
}

/*
** GLW_InitExtensions
*/
static void
GLW_InitExtensions(void)
{
	if (!r_allowExtensions->integer) {
		ri.Printf(PRINT_ALL, "*** IGNORING OPENGL EXTENSIONS ***\n");
		return;
	}

	ri.Printf(PRINT_ALL, "Initializing OpenGL extensions\n");

	// GL_S3_s3tc
	glConfig.textureCompression = TC_NONE;

	if (strstr(glConfig.extensions_string, "GL_S3_s3tc")) {
		if (r_ext_compressed_textures->integer) {
			glConfig.textureCompression = TC_S3TC;
			ri.Printf(PRINT_ALL, "...using GL_S3_s3tc\n");
		} else {
			glConfig.textureCompression = TC_NONE;
			ri.Printf(PRINT_ALL, "...ignoring GL_S3_s3tc\n");
		}
	} else {
		ri.Printf(PRINT_ALL, "...GL_S3_s3tc not found\n");
	}

	// GL_EXT_texture_env_add
	glConfig.textureEnvAddAvailable = false;
	if (strstr(glConfig.extensions_string, "EXT_texture_env_add")) {
		if (r_ext_texture_env_add->integer) {
			glConfig.textureEnvAddAvailable = true;
			ri.Printf(PRINT_ALL, "...using GL_EXT_texture_env_add\n");
		} else {
			glConfig.textureEnvAddAvailable = false;
			ri.Printf(PRINT_ALL, "...ignoring GL_EXT_texture_env_add\n");
		}
	} else {
		ri.Printf(PRINT_ALL, "...GL_EXT_texture_env_add not found\n");
	}

	// GL_ARB_multitexture
	qglMultiTexCoord2fARB = NULL;
	qglActiveTextureARB = NULL;
	qglClientActiveTextureARB = NULL;

	if (strstr(glConfig.extensions_string, "GL_ARB_multitexture")) {
		if (r_ext_multitexture->integer) {

			qglMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord2fARB");
			qglActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)SDL_GL_GetProcAddress("glActiveTextureARB");
			qglClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)SDL_GL_GetProcAddress("glClientActiveTextureARB");

			if (qglActiveTextureARB) {

				qglGetIntegerv(GL_MAX_ACTIVE_TEXTURES_ARB, &glConfig.maxActiveTextures);

				if (glConfig.maxActiveTextures > 1) {
					ri.Printf(PRINT_ALL, "...using GL_ARB_multitexture\n");
				} else {
					qglMultiTexCoord2fARB = NULL;
					qglActiveTextureARB = NULL;
					qglClientActiveTextureARB = NULL;
					ri.Printf(PRINT_ALL, "...not using GL_ARB_multitexture, < 2 texture units\n");
				}
			}
		} else {
			ri.Printf(PRINT_ALL, "...ignoring GL_ARB_multitexture\n");
		}
	} else {
		ri.Printf(PRINT_ALL, "...GL_ARB_multitexture not found\n");
	}

	// GL_EXT_compiled_vertex_array
	qglLockArraysEXT = NULL;
	qglUnlockArraysEXT = NULL;

	if (strstr(glConfig.extensions_string, "GL_EXT_compiled_vertex_array"))
	{
		if (r_ext_compiled_vertex_array->integer)
		{
			ri.Printf(PRINT_ALL, "...using GL_EXT_compiled_vertex_array\n");
			qglLockArraysEXT = (void (APIENTRY*)(int, int))SDL_GL_GetProcAddress("glLockArraysEXT");
			qglUnlockArraysEXT = (void (APIENTRY*)(void))SDL_GL_GetProcAddress("glUnlockArraysEXT");
			if (!qglLockArraysEXT || !qglUnlockArraysEXT) {
				ri.Error(ERR_FATAL, "bad getprocaddress");
			}
		} else {
			ri.Printf(PRINT_ALL, "...ignoring GL_EXT_compiled_vertex_array\n");
		}
	} else {
		ri.Printf(PRINT_ALL, "...GL_EXT_compiled_vertex_array not found\n");
	}
}

/*
** GLW_LoadOpenGL
**
** GLimp_win.c internal function that attempts to load and use 
** a specific OpenGL DLL.
*/
static bool
GLW_LoadOpenGL(const char* drivername)
{
	char buffer[1024];
	bool cdsFullscreen;

	Q_strncpyz(buffer, drivername, sizeof(buffer));
	Q_strlwr(buffer);

	// determine if we're on a standalone driver
	if (strstr(buffer, "opengl32") != 0) {
		glConfig.driverType = GLDRV_ICD;
	} else {

		glConfig.driverType = GLDRV_STANDALONE;
		ri.Printf(PRINT_ALL, "...assuming '%s' is a standalone driver\n", drivername);
	}

	// load the driver and bind our function pointers to it
	if (QGL_Init(buffer)) {

		cdsFullscreen = r_fullscreen->integer;

		// create the window and set up the context
		if (GLW_StartDriverAndSetMode(drivername, r_mode->integer, r_colorbits->integer,
			cdsFullscreen) == false) {
			// if we're on a 24/32-bit desktop and we're going fullscreen on an ICD,
			// try it again but with a 16-bit desktop
			if (glConfig.driverType == GLDRV_ICD) {
				if (r_colorbits->integer != 16 || cdsFullscreen != true ||
					r_mode->integer != 3) {
					if (!GLW_StartDriverAndSetMode(drivername, 3, 16, true)) {
						goto fail;
					}
				}
			} else {
				goto fail;
			}
		}

		return true;
	}
fail:

	QGL_Shutdown();

	return false;
}

/*
** GLimp_EndFrame
*/
void GLimp_EndFrame(void)
{
	// swapinterval stuff
	if (r_swapInterval->modified) {
		r_swapInterval->modified = false;

		if (!glConfig.stereoEnabled) {	// why?
			SDL_GL_SetSwapInterval(r_swapInterval->integer);
		}
	}

	// don't flip if drawing to front buffer
	if (Q_stricmp(r_drawBuffer->string, "GL_FRONT") != 0) {
		if (glConfig.driverType > GLDRV_ICD) {
			if (!SDL_GL_SwapWindow(g_wv.hWnd)) {
				ri.Error(ERR_FATAL, "GLimp_EndFrame() - SwapBuffers() failed!\n");
			}
		} else {
			SDL_GL_SwapWindow(g_wv.hWnd);
		}
	}

	// check logging
	QGL_EnableLogging(r_logFile->integer);
}

static void GLW_StartOpenGL(void)
{
	bool attemptedOpenGL32 = false;

	// load and initialize the specific OpenGL driver
	if (!GLW_LoadOpenGL(r_glDriver->string)) {
		if (!Q_stricmp(r_glDriver->string, OPENGL_DRIVER_NAME)) {
			attemptedOpenGL32 = true;
		}

		if (!attemptedOpenGL32) {
			attemptedOpenGL32 = true;
			if (GLW_LoadOpenGL(OPENGL_DRIVER_NAME)) {
				ri.Cvar_Set("r_glDriver", OPENGL_DRIVER_NAME);
				r_glDriver->modified = false;
			} else {
				ri.Error(ERR_FATAL, "GLW_StartOpenGL() - could not load OpenGL subsystem\n");
			}
		}
	}
}

/*
** GLimp_Init
**
** This is the platform specific OpenGL initialization function. It
** is responsible for loading OpenGL, initializing it, setting
** extensions, creating a window of the appropriate size, doing
** fullscreen manipulations, etc.  Its overall responsibility is
** to make sure that a functional OpenGL subsystem is operating
** when it returns to the ref.
*/
void GLimp_Init(void)
{
	char	buf[1024];
	cvar_t* lastValidRenderer = ri.Cvar_Get("r_lastValidRenderer", "(uninitialized)",
		CVAR_ARCHIVE);

	ri.Printf(PRINT_ALL, "Initializing OpenGL subsystem\n");

	if (!SDL_WasInit(SDL_INIT_VIDEO)) {
		if (!SDL_Init(SDL_INIT_VIDEO)) {
			Com_Printf("Couldn't init SDL video: %s.\n", SDL_GetError());

			return;
		}

		int version = SDL_GetVersion();

		Com_Printf("SDL version is: %i.%i.%i\n", SDL_VERSIONNUM_MAJOR(version),
			SDL_VERSIONNUM_MINOR(version), SDL_VERSIONNUM_MICRO(version));
		Com_Printf("SDL video driver is \"%s\".\n", SDL_GetCurrentVideoDriver());
	}

	r_allowSoftwareGL = ri.Cvar_Get("r_allowSoftwareGL", "0", CVAR_LATCH);
	r_maskMinidriver = ri.Cvar_Get("r_maskMinidriver", "0", CVAR_LATCH);

	// load appropriate DLL and initialize subsystem
	GLW_StartOpenGL();

	// get our config strings
	Q_strncpyz(glConfig.vendor_string, qglGetString(GL_VENDOR), sizeof(glConfig.vendor_string));
	Q_strncpyz(glConfig.renderer_string, qglGetString(GL_RENDERER), sizeof(glConfig.renderer_string));
	Q_strncpyz(glConfig.version_string, qglGetString(GL_VERSION), sizeof(glConfig.version_string));
	Q_strncpyz(glConfig.extensions_string, qglGetString(GL_EXTENSIONS), sizeof(glConfig.extensions_string));

	// chipset specific configuration
	Q_strncpyz(buf, glConfig.renderer_string, sizeof(buf));
	Q_strlwr(buf);

	// NOTE: if changing cvars, do it within this block.  This allows them
	// to be overridden when testing driver fixes, etc. but only sets
	// them to their default state when the hardware is first installed/run.
	if (Q_stricmp(lastValidRenderer->string, glConfig.renderer_string)) {

		glConfig.hardwareType = GLHW_GENERIC;
		ri.Cvar_Set("r_textureMode", "GL_LINEAR_MIPMAP_NEAREST");
		ri.Cvar_Set("r_picmip", "1");
	}

	ri.Cvar_Set("r_lastValidRenderer", glConfig.renderer_string);

	GLW_InitExtensions();
	WG_CheckHardwareGamma();
}

/*
** GLimp_Shutdown
**
** This routine does all OS specific shutdown procedures for the OpenGL
** subsystem.
*/
void GLimp_Shutdown(void)
{
	// const char *strings[] = { "soft", "hard" };
	const char* success[] = { "failed", "success" };
	int retVal;

	ri.Printf(PRINT_ALL, "Shutting down OpenGL subsystem\n");

	// restore gamma.  We do this first because 3Dfx's extension needs a valid OGL subsystem
	WG_RestoreGamma();

	// set current context to NULL
	retVal = SDL_GL_MakeCurrent(NULL, NULL) != 0;
	ri.Printf(PRINT_ALL, "...SDL_GL_MakeCurrent( NULL, NULL ): %s\n", success[retVal]);

	// delete HGLRC
	if (glw_state.hGLRC) {
		retVal = SDL_GL_DestroyContext(glw_state.hGLRC) != 0;
		ri.Printf(PRINT_ALL, "...deleting GL context: %s\n", success[retVal]);
		glw_state.hGLRC = NULL;
	}

	// destroy window
	if (g_wv.hWnd) {
		ri.Printf(PRINT_ALL, "...destroying window\n");
		SDL_HideWindow(g_wv.hWnd);
		SDL_DestroyWindow(g_wv.hWnd);
		g_wv.hWnd = NULL;
		glw_state.pixelFormatSet = false;
	}

	// close the r_logFile
	if (glw_state.log_fp) {
		fclose(glw_state.log_fp);
		glw_state.log_fp = 0;
	}

	// reset display settings
	if (glw_state.cdsFullscreen) {
		ri.Printf(PRINT_ALL, "...resetting display\n");
		glw_state.cdsFullscreen = false;
	}

	// shutdown QGL subsystem
	QGL_Shutdown();

	Com_Memset(&glConfig, 0, sizeof(glConfig));
	Com_Memset(&glState, 0, sizeof(glState));
}

/*
** GLimp_LogComment
*/
void GLimp_LogComment(char* comment)
{
	if (glw_state.log_fp) {
		fprintf(glw_state.log_fp, "%s", comment);
	}
}

/*
===========================================================

SMP acceleration

===========================================================
*/

SDL_Condition* renderCommandsEvent = NULL;
SDL_Condition* renderCompletedEvent = NULL;
SDL_Condition* renderActiveEvent = NULL;
SDL_Mutex* renderMutex = NULL;

void (*glimpRenderThread)(void);

void GLimp_RenderThreadWrapper(void)
{
	glimpRenderThread();

	// unbind the context before we die
	SDL_GL_MakeCurrent(g_wv.hWnd, NULL);
}

/*
=======================
GLimp_SpawnRenderThread
=======================
*/
SDL_Thread* renderThreadHandle = NULL;
SDL_ThreadID renderThreadId = 0;

bool GLimp_SpawnRenderThread(void (*function)(void))
{
	renderCommandsEvent = SDL_CreateCondition();
	renderCompletedEvent = SDL_CreateCondition();
	renderActiveEvent = SDL_CreateCondition();
	renderMutex = SDL_CreateMutex();

	glimpRenderThread = function;

	renderThreadHandle = SDL_CreateThread(
		(SDL_ThreadFunction)GLimp_RenderThreadWrapper,
		"GLimp_RenderThreadWrapper",
		NULL);

	if (!renderThreadHandle) {
		return false;
	}

	renderThreadId = SDL_GetThreadID(renderThreadHandle);

	return true;
}

static void* s_smpData;
static int s_wglErrors;

void* GLimp_RendererSleep(void)
{
	void* data;

	if (!SDL_GL_MakeCurrent(g_wv.hWnd, NULL)) {
		s_wglErrors++;
	}

	SDL_BroadcastCondition(renderActiveEvent);

	// after this, the front end can exit GLimp_FrontEndSleep
	SDL_SignalCondition(renderCompletedEvent);

	SDL_LockMutex(renderMutex);
	SDL_WaitCondition(renderCommandsEvent, renderMutex);

	if (!SDL_GL_MakeCurrent(g_wv.hWnd, glw_state.hGLRC)) {
		s_wglErrors++;
	}

	SDL_BroadcastCondition(renderCompletedEvent);
	SDL_BroadcastCondition(renderCommandsEvent);
	SDL_UnlockMutex(renderMutex);

	data = s_smpData;

	// after this, the main thread can exit GLimp_WakeRenderer
	SDL_SignalCondition(renderActiveEvent);

	return data;
}

void GLimp_FrontEndSleep(void)
{
	SDL_LockMutex(renderMutex);
	SDL_WaitConditionTimeout(renderCompletedEvent, renderMutex, 5);

	if (!SDL_GL_MakeCurrent(g_wv.hWnd, glw_state.hGLRC)) {
		s_wglErrors++;
	}
	SDL_UnlockMutex(renderMutex);
}

void GLimp_WakeRenderer(void* data)
{
	SDL_LockMutex(renderMutex);
	s_smpData = data;

	if (!SDL_GL_MakeCurrent(g_wv.hWnd, NULL)) {
		s_wglErrors++;
	}

	// after this, the renderer can continue through GLimp_RendererSleep
	SDL_SignalCondition(renderCommandsEvent);

	SDL_WaitCondition(renderActiveEvent, renderMutex);
	SDL_UnlockMutex(renderMutex);
}
