// Standard Header Files
#include <stdio.h>			// For Standard I/O
#include <stdlib.h>			// For exit()
#include <memory.h>			// For memset()

// X11 Headers
#include <X11/Xlib.h>
#include <X11/Xutil.h>			// XVisualInfo
#include <X11/XKBlib.h>		// For Keyboard

// OpenGL Header Files
#include <GL/gl.h>			// For OpenGL Functionality
#include <GL/glx.h>			// For Bridging API's
#include <GL/glu.h>			// For glu Functions

// Texture Library Headers
#include <SOIL/SOIL.h>

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
GLXContext glxContext;

float anglePyramid = 0.0f;
Bool gbLight = False;

GLfloat LightAmbientZero[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat LightDiffuseZero[] = {1.0f, 0.0f, 0.0f, 1.0f};			// Red
GLfloat LightSpecularZero[] = {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat LightPositionZero[] = {-2.0f, 0.0f, 0.0f, 1.0f};

GLfloat LightAmbientOne[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightDiffuseOne[] = { 0.0f, 0.0f, 1.0f, 1.0f };			// Blue
GLfloat LightSpecularOne[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat LightPositionOne[] = { 2.0f, 0.0f, 0.0f, 1.0f };

GLfloat MaterialAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat MaterialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat MaterialShininess = 50.0f;

// Entry-Point Function
int main(void)
{
	// Function Declarations
	void toggleFullscreen(void);
	int initialise(void);
	void resize(int, int);
	void draw(void);
	void update(void);
	void uninitialise(void);

	// Local Variables
	int defaultScreen;
	int defaultDepth;
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
		  GLX_RGBA,
		  GLX_RED_SIZE, 8,
		  GLX_GREEN_SIZE, 8,
		  GLX_BLUE_SIZE, 8,
		  GLX_ALPHA_SIZE, 8,
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
		printf("ERROR: XOpenDisplay() Failed...\n");
		uninitialise();
		exit(1);
	}

	// Get Default Screen By Using Display
	defaultScreen = XDefaultScreen(display);
	// Get Default Depth
	defaultDepth = XDefaultDepth(display, defaultScreen);
	// Get XVisualInfo
	visualInfo = glXChooseVisual(display, defaultScreen, frameBufferAttributes);
	if (visualInfo == NULL)
	{
		printf("ERROR: glxChooseVisualInfo() Failed...\n");
		uninitialise();
		exit(1);
	}

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
	if (iRetVal == -5)
	{
		fprintf(gpFile, "LoadGLTexture() FOR SMILEY TEXTURE FAILED.\n");
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

				case 'L':
				case 'l':
					if (gbLight == False)
					{
						glEnable(GL_LIGHTING);
						gbLight = True;
					}
					else
					{
						glDisable(GL_LIGHTING);
						gbLight = False;
					}
					break;

				default:
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
			update();
		
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
	// Function Declarations
	void resize(int, int);
	
	// Code
	glxContext = glXCreateContext(display, visualInfo, NULL, True);
	glXMakeCurrent(display, window, glxContext);
	
	// Here Starts OpenGL Code
	// Clear The Screen Using Black Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Depth Related Changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbientZero);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuseZero);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecularZero);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPositionZero);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbientOne);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuseOne);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecularOne);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPositionOne);
	glEnable(GL_LIGHT1);

	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// Warmup resize() Call
	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void resize(int width, int height)
{
	// Code
	if (height == 0)
		height = 1;					// To avoid 'Divide By 0' in future

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void draw(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -6.0f);
	glRotatef(anglePyramid, 0.0f, 1.0f, 0.0f);			// Spin
	
	glBegin(GL_TRIANGLES);

	// Front Face
	glNormal3f(0.0f, 0.447214f, 0.894428f);
	glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Right Face
	glNormal3f(0.894428f, 0.447214f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Back Face
	glNormal3f(0.0f, 0.447214f, -0.894428f);
	glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	// Left Face
	glNormal3f(-0.894428f, 0.447214f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	
	glEnd();
	
	glXSwapBuffers(display, window);
}

void update(void)
{
	// Code
	anglePyramid = anglePyramid + 1.0f;
	if (anglePyramid >= 360.0f)
		anglePyramid = anglePyramid - 360.0f;
}

void uninitialise(void)
{
	// Local Variable
	GLXContext currentContext;
	
	// Code
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
