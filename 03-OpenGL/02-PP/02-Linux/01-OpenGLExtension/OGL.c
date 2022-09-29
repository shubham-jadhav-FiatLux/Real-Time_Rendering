// Standard Header Files
#include <stdio.h>			// For Standard I/O
#include <stdlib.h>			// For exit()
#include <memory.h>			// For memset()

// X11 Headers
#include <X11/Xlib.h>
#include <X11/Xutil.h>			// XVisualInfo
#include <X11/XKBlib.h>		// For Keyboard

// OpenGL Header Files
#include <GL/glew.h>
#include <GL/gl.h>			// For OpenGL Functionality
#include <GL/glx.h>			// For Bridging API's

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global Variables
Display *display = NULL;
XVisualInfo *visualInfo = NULL;	// Pointer
Colormap colormap;
Window window;
Bool fullscreen = False;
Bool bActiveWindow = False;
FILE* gpFile = NULL;

// OpenGL Related Variables
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int *);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig glxFBConfig;
GLXContext glxContext;

// Entry-Point Function
int main(void)
{
	// Function Declarations
	void toggleFullscreen(void);
	int initialise(void);
	void resize(int, int);
	void draw(void);
	void uninitialise(void);

	// Local Variables
	int defaultScreen;
	int defaultDepth;
	GLXFBConfig *glxFBConfigs = NULL;
	GLXFBConfig bestGLXFBConfig;
	XVisualInfo *tempXVisualInfo = NULL;
	int numFBConfigs;
	XSetWindowAttributes windowAttributes;
	int styleMask;
	Atom wm_delete_window_atom;
	XEvent event;
	KeySym keySym;
	int screenWidth;
	int screenHeight;
	char keys[26];
	static int winWidth, winHeight; 
	int iRetVal = 0; 
	
	static int frameBufferAttributes[] = 
		{ GLX_DOUBLEBUFFER, True,
		  GLX_X_RENDERABLE, True,
		  GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		  GLX_RENDER_TYPE, GLX_RGBA_BIT,
		  GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		  GLX_RED_SIZE, 8,
		  GLX_GREEN_SIZE, 8,
		  GLX_BLUE_SIZE, 8,
		  GLX_ALPHA_SIZE, 8,
		  GLX_STENCIL_SIZE, 8,
		  GLX_DEPTH_SIZE, 24,
		  None };				// Static For Style
	Bool bDone = False;

	// Code
	gpFile = fopen("Log.txt", "w");
	if (gpFile == NULL)
	{
		printf("Creation of Log file failed. Exiting ...\n");
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Successfully Opened.\n");
	}

	// Open The Display
	display = XOpenDisplay(NULL);
	if (display == NULL)
	{
		fprintf(gpFile,"ERROR: XOpenDisplay() Failed...\n");
		uninitialise();
		exit(1);
	}

	// Get Default Screen By Using Display
	defaultScreen = XDefaultScreen(display);
	// Get Default Depth
	defaultDepth = XDefaultDepth(display, defaultScreen);

	glxFBConfigs = glXChooseFBConfig(display, defaultScreen, frameBufferAttributes, &numFBConfigs);
	if (glxFBConfigs == NULL)
	{
		fprintf(gpFile, "glXChooseFBConfig() Failed.\n");
		uninitialise();
		exit(1);
	}
	fprintf(gpFile, "Found Framebuffer Configurations Are %d.\n", numFBConfigs);

	// Find Best Framebuffer Config From Array 
	int bestFramebufferConfig = -1;
	int worstFramebufferConfig = -1;
	int bestNumberOfSamples = -1;
	int worstNumberOfSamples = 999;

	for (int i = 0; i < numFBConfigs; i++)
	{
		tempXVisualInfo = glXGetVisualFromFBConfig(display, glxFBConfigs[i]);
		if (tempXVisualInfo != NULL)
		{
			int samples, sampleBuffers;

			glXGetFBConfigAttrib(display, glxFBConfigs[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
			glXGetFBConfigAttrib(display, glxFBConfigs[i], GLX_SAMPLES, &samples);
			fprintf(gpFile, "VisualInfo = 0x%lu Found sampleBuffers = %d And samples = %d\n", tempXVisualInfo->visualid, sampleBuffers, samples);

			if (bestFramebufferConfig < 0 || sampleBuffers && samples > bestNumberOfSamples)
			{
				bestFramebufferConfig = i;
				bestNumberOfSamples = samples;
			}
			if (worstFramebufferConfig < 0 || !sampleBuffers || samples < worstNumberOfSamples)
			{
				worstFramebufferConfig = i;
				worstNumberOfSamples = samples;
			}
		}
		XFree(tempXVisualInfo);
		tempXVisualInfo = NULL;
	}

	bestGLXFBConfig = glxFBConfigs[bestFramebufferConfig];
	glxFBConfig = bestGLXFBConfig;

	XFree(glxFBConfigs);
	glxFBConfigs = NULL;

	visualInfo = glXGetVisualFromFBConfig(display, bestGLXFBConfig);
	fprintf(gpFile, "VisualId Of Best VisualInfo Is 0x%lu\n", visualInfo->visualid);

	memset(&windowAttributes, 0, sizeof(XSetWindowAttributes));
	windowAttributes.border_pixel = 0;
	windowAttributes.background_pixel = XBlackPixel(display, defaultScreen);
	windowAttributes.background_pixmap = 0;
	windowAttributes.colormap = XCreateColormap(display,
						     RootWindow(display, visualInfo->screen),
						     visualInfo->visual,
						     AllocNone);
	windowAttributes.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask | FocusChangeMask;

	colormap = windowAttributes.colormap;

	styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;
	
	window = XCreateWindow(display,
						RootWindow(display, visualInfo->screen),
						0,
						0,
						WIN_WIDTH,
						WIN_HEIGHT,
						0,
						visualInfo->depth,
						InputOutput,
						visualInfo->visual,
						styleMask,
						&windowAttributes);
	if (!window)
	{
		printf("ERROR: XCreateWindow() Failed...\n");
		uninitialise();
		exit(1);
	}

	XStoreName(display, window, "SBJ:OpenGL");

	wm_delete_window_atom = XInternAtom(display, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(display, window, &wm_delete_window_atom, 1);

	XMapWindow(display, window);

	// Centering Of Window
	screenWidth = XWidthOfScreen(XScreenOfDisplay(display, defaultScreen));
	screenHeight = XHeightOfScreen(XScreenOfDisplay(display, defaultScreen));
	XMoveWindow(display, window, (screenWidth / 2 - WIN_WIDTH / 2), (screenHeight / 2 - WIN_HEIGHT / 2));

	// initialise
	iRetVal = initialise();
	if (iRetVal == -1)
	{
		fprintf(gpFile, "glXGetProcAddressARB() Failed.\n");
		uninitialise();
	}
	else if (iRetVal == -2)
	{
		fprintf(gpFile, "GLEW Initialization Failed.\n");
		uninitialise();
	}
	else
		fprintf(gpFile, "Succeed.\n");

	// Message Loop
	while (bDone == False)
	{
		while (XPending(display))								
		{
			XNextEvent(display, &event);								
			switch (event.type)
			{
			case MapNotify:
				break;

			case FocusIn:
				bActiveWindow = True;
				break;
				
			case FocusOut:
				bActiveWindow = False;
				break;
				
			case KeyPress:
				keySym = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);			// like WM_KEYDOWN
				switch (keySym)
				{
				case XK_Escape:
					bDone = True;
					break;
				}

				XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);				// Like WM_CHAR
				switch (keys[0])
				{
				case 'F':
				case 'f':
					if (fullscreen == False)
					{
						toggleFullscreen();
						fullscreen = True;
					}

					else
					{
						toggleFullscreen();
						fullscreen = False;
					}
					break;
				}
				break;
				
			case ConfigureNotify:
				winWidth = event.xconfigure.width;
				winHeight = event.xconfigure.height;
				resize(winWidth, winHeight);
				break;

			case 33:
				bDone = True;
				break;

			default:
				break;
			}
		}
		
		if (bActiveWindow == True)
		{
			// Update
		
			draw();
		}
	}
	
	uninitialise();

	return 0;
}

void toggleFullscreen(void)
{
	// Local Variables
	Atom wm_current_state_atom;
	Atom wm_state_fullscreen_atom;
	XEvent event;

	// Code
	wm_current_state_atom = XInternAtom(display, "_NET_WM_STATE", False);
	wm_state_fullscreen_atom = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

	memset(&event, 0, sizeof(XEvent));
	event.type = ClientMessage;
	event.xclient.window = window;
	event.xclient.message_type = wm_current_state_atom;
	event.xclient.format = 32;
	event.xclient.data.l[0] = fullscreen ? 0 : 1;
	event.xclient.data.l[1] = wm_state_fullscreen_atom;

	XSendEvent(display,																			// Like SendMessage And PostMessage
		       RootWindow(display, visualInfo->screen),
		       False,
		       SubstructureNotifyMask,
		       &event);
}

int initialise(void)
{
	// Variable Declarations
	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	// Function Declarations
	void resize(int, int);
	void printGLInfo(void);
	void uninitialise(void);

	// Code
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB");
	if (glXCreateContextAttribsARB == NULL)
		return -1;
	
	GLint contextAttributes[] = {
									GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
									GLX_CONTEXT_MINOR_VERSION_ARB, 6,
									GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
									None
								};

	glxContext = glXCreateContextAttribsARB(display, glxFBConfig, NULL, True, contextAttributes);
	if (!glxContext)
	{
		GLint contextAttributes[] = {
										GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
										GLX_CONTEXT_MINOR_VERSION_ARB, 0,
										None
									};
		glxContext = glXCreateContextAttribsARB(display, glxFBConfig, NULL, True, contextAttributes);

		fprintf(gpFile, "Cannot Support 4.6 And Hence Falling Back To Default PP Supported Version.\n");
	}
	else
	{
		fprintf(gpFile, "Found Support To 4.6 Version.\n");
	}
	
	// Checking Whether H/W Rendering or S/W Rendering
	if (!glXIsDirect(display, glxContext))
	{
		fprintf(gpFile, "Direct Rendering i.e. H/W Rendering Is Not Supported.\n");
	}
	else
	{
		fprintf(gpFile, "Direct Rendering i.e. H/W Rendering Is Supported.\n");
	}

	glXMakeCurrent(display, window, glxContext);

	// GLEW Initialization
	if (glewInit() != GLEW_OK)
		return -2;

	// Print OpenGL Info
	printGLInfo();
	
	// Depth Related Changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glShadeModel(),glHint() Deprecated In PP

	// Here Starts OpenGL Code
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

	// Warmup resize() Call
	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void printGLInfo(void)
{
	// Local Variable Declarations 
	GLint numExtentions;

	// Code
	fprintf(gpFile, "OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version: %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtentions);

	fprintf(gpFile, "Number Of Supported Extentions: %d\n", numExtentions);
	for (int i = 0; i < numExtentions; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}
}

void resize(int width, int height)
{
	if (height == 0)
		height = 1;
		
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void draw(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glXSwapBuffers(display, window);
}

void uninitialise(void)
{
	// Local Variable
	GLXContext currentContext;
	
	// Code
	if (fullscreen)
	{
		toggleFullscreen();
	}

	currentContext = glXGetCurrentContext();
	if (currentContext && currentContext == glxContext)
	{
		glXMakeCurrent(display, 0, 0);
	}
	
	if (glxContext)
	{
		glXDestroyContext(display, glxContext);
		glxContext = NULL;
	}
	
	if (visualInfo)
	{
		free(visualInfo);
		visualInfo = NULL;
	}
	
	if (window)
	{
		XDestroyWindow(display, window);
	}

	if (colormap)
	{
		XFreeColormap(display, colormap);
	}

	if (display)
	{
		XCloseDisplay(display);
		display = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log File Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
