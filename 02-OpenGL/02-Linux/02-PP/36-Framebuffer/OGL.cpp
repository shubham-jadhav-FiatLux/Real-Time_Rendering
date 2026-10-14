// Standard Header Files
#include <stdio.h>			// For Standard I/O
#include <stdlib.h>			// For exit()
#include <memory.h>			// For memset()
#include "Sphere.h"

// X11 Headers
#include <X11/Xlib.h>
#include <X11/Xutil.h>			// XVisualInfo
#include <X11/XKBlib.h>		// For Keyboard

// OpenGL Header Files
#include <GL/glew.h>
#include <GL/gl.h>			// For OpenGL Functionality
#include <GL/glx.h>			// For Bridging API's
#include "vmath.h"
using namespace vmath;

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define FBO_WIDTH 512
#define FBO_HEIGHT 512

// Global Variables
Display *display = NULL;
XVisualInfo *visualInfo = NULL;	// Pointer
Colormap colormap;
Window window;
Bool fullscreen = False;
Bool bActiveWindow = False;
FILE* gpFile = NULL;

int winWidth;
int winHeight;

// OpenGL Related Variables
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int *);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig glxFBConfig;
GLXContext glxContext;

// Programmable Pipeline Related Global Variables
GLuint shaderProgramObject;

enum
{
	SBJ_ATTRIBUTE_POSITION = 0,
	SBJ_ATTRIBUTE_COLOR,
	SBJ_ATTRIBUTE_NORMAL,
	SBJ_ATTRIBUTE_TEXTURE0
};

GLuint vao_cube;
GLuint vbo_cube_position;
GLuint vbo_cube_texcoord;

GLfloat angleCube = 0.0f;

GLuint mvpMatrixUniform;
GLuint textureSamplerUniform;

mat4 perspectiveProjectionMatrix;

// FBO Related Global Variables
GLuint fbo;
GLuint rbo;
GLuint fbo_texture;
bool bFBOResult = false;

// Texture Scene Global Vaiables
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int gNumVertices_sphere;
unsigned int gNumElements_sphere;

Bool bLight = False;
GLfloat lightAngleZero_sphere = 0.0f;
GLfloat lightAngleOne_sphere = 0.0f;
GLfloat lightAngleTwo_sphere = 0.0f;

GLuint shaderProgramObject_sphere;

GLuint vao_Sphere;
GLuint vbo_Sphere_Position;
GLuint vbo_Sphere_Normals;
GLuint vbo_Sphere_Elements;

GLuint modelMatrixUniform_sphere;
GLuint viewMatrixUniform_sphere;
GLuint projectionMatrixUniform_sphere;

GLuint laUniform_sphere[3];				// Ambient
GLuint ldUniform_sphere[3];				// Diffuse
GLuint lsUniform_sphere[3];				// Specular
GLuint lightPositionUniform_sphere[3];

struct Light
{
	vec4 lightAmbient_sphere;
	vec4 lightDiffuse_sphere;
	vec4 lightSpecular_sphere;
	vec4 lightPosition_sphere;
};
Light lights_sphere[3];

GLuint kaUniform_sphere;
GLuint kdUniform_sphere;
GLuint ksUniform_sphere;
GLuint materialShininessUniform_sphere;

GLfloat materialAmbient_sphere[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse_sphere[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular_sphere[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess_sphere = 50.0f;

GLuint lightingEnabledUniform_sphere;

mat4 perspectiveProjectionMatrix_sphere;


// Entry-Point Function
int main(void)
{
	// Function Declarations
	void toggleFullscreen(void);
	int initialize(void);
	void resize(int, int);
	void draw(void);
	void update(void);
	void uninitialize(void);

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
	//static int winWidth, winHeight; 
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
		uninitialize();
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
		uninitialize();
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
		uninitialize();
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
	iRetVal = initialize();
	if (iRetVal == -1)
	{
		fprintf(gpFile, "glXGetProcAddressARB() Failed.\n");
		uninitialize();
	}
	else if (iRetVal == -2)
	{
		fprintf(gpFile, "GLEW Initialization Failed.\n");
		uninitialize();
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
					if (bLight == False)
					{
						bLight = True;
					}
					else
					{
						bLight = False;
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
			draw();

			update();
		}
	}
	
	uninitialize();

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

int initialize(void)
{
	// Variable Declarations
	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	// Function Declarations
	void resize(int, int);
	void printGLInfo(void);
	void uninitialize(void);
	bool createFBO(GLint, GLint);
	int initialize_sphere(int, int);

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

	// Vertex Shader
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec2 a_texcoord;" \
		"uniform mat4 u_mvpMatrix;" \
		"out vec2 a_texcoord_out;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvpMatrix * a_position;" \
		"a_texcoord_out = a_texcoord;" \
		"}";

	// Creating Shader Object
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	// Giving Shader Code To Shader Object 
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
	// Compile The Shader
	glCompileShader(vertexShaderObject);
	// Error Checking
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "Vertex Shader Compilation Log: %s\n",  log);
				free(log);
				uninitialize();
			}
		}
	}

	// Fragment Shader
	const GLchar* fragmentShaderSourceCode = 
		"#version 460 core" \
		"\n" \
		"in vec2 a_texcoord_out;" \
		"uniform sampler2D u_textureSampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = texture(u_textureSampler, a_texcoord_out);" \
		"}";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(fragmentShaderObject);
	status = 0;
	infoLogLength = 0;
	log = NULL;
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "Fragment Shader Compilation Log: %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}
	
	// Shader Program Object
	shaderProgramObject = glCreateProgram();
	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);
	glBindAttribLocation(shaderProgramObject, SBJ_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(shaderProgramObject, SBJ_ATTRIBUTE_TEXTURE0, "a_texcoord");
	glLinkProgram(shaderProgramObject);
	status = 0;
	infoLogLength = 0;
	log = NULL;
	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, log);
				fprintf(gpFile, "Shader Program Link Log: %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");
	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_textureSampler");

	// Declaration Of Vertex Data Arrays

	const GLfloat cubePosition[] =		{
											// top
											1.0f, 1.0f, -1.0f,
											-1.0f, 1.0f, -1.0f,
											-1.0f, 1.0f, 1.0f,
											1.0f, 1.0f, 1.0f,

											// bottom
											1.0f, -1.0f, -1.0f,
											-1.0f, -1.0f, -1.0f,
											-1.0f, -1.0f,  1.0f,
											 1.0f, -1.0f,  1.0f,

											 // front
											 1.0f, 1.0f, 1.0f,
											-1.0f, 1.0f, 1.0f,
											-1.0f, -1.0f, 1.0f,
											 1.0f, -1.0f, 1.0f,

											 // back
											 1.0f, 1.0f, -1.0f,
											-1.0f, 1.0f, -1.0f,
											-1.0f, -1.0f, -1.0f,
											 1.0f, -1.0f, -1.0f,

											 // right
											 1.0f, 1.0f, -1.0f,
											 1.0f, 1.0f, 1.0f,
											 1.0f, -1.0f, 1.0f,
											 1.0f, -1.0f, -1.0f,

											 // left
											-1.0f, 1.0f, 1.0f,
											-1.0f, 1.0f, -1.0f,
											-1.0f, -1.0f, -1.0f,
											-1.0f, -1.0f, 1.0f,

										};

	const GLfloat cubeTexcoord[] =		{
											0.0f, 0.0f,
											1.0f, 0.0f,
											1.0f, 1.0f,
											0.0f, 1.0f,

											0.0f, 0.0f,
											1.0f, 0.0f,
											1.0f, 1.0f,
											0.0f, 1.0f,

											0.0f, 0.0f,
											1.0f, 0.0f,
											1.0f, 1.0f,
											0.0f, 1.0f,

											0.0f, 0.0f,
											1.0f, 0.0f,
											1.0f, 1.0f,
											0.0f, 1.0f,

											0.0f, 0.0f,
											1.0f, 0.0f,
											1.0f, 1.0f,
											0.0f, 1.0f,

											0.0f, 0.0f,
											1.0f, 0.0f,
											1.0f, 1.0f,
											0.0f, 1.0f,
										};

	// vao And vbo Related Code
	// Cube
	glGenVertexArrays(1, &vao_cube);
	glBindVertexArray(vao_cube);

		// vbo For Position
		glGenBuffers(1, &vbo_cube_position);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubePosition), cubePosition, GL_STATIC_DRAW);
		glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vbo For Color
		glGenBuffers(1, &vbo_cube_texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoord);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoord), cubeTexcoord, GL_STATIC_DRAW);
		glVertexAttribPointer(SBJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SBJ_ATTRIBUTE_TEXTURE0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// Here Starts OpenGL Code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);							// glShadeModel(),glHint() Deprecated In PP 

	// Clear The Screen Using Blue Color
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Enabling The Texture
	glEnable(GL_TEXTURE_2D);

	perspectiveProjectionMatrix = mat4::identity();

	// Warmup resize() Call
	resize(WIN_WIDTH, WIN_HEIGHT);

	// FBO Code
	bFBOResult = createFBO(FBO_WIDTH, FBO_HEIGHT);
	int iRetVal;
	if (bFBOResult == true)
	{
		iRetVal = initialize_sphere(FBO_WIDTH, FBO_HEIGHT);

		// Here There Should Be Error Checking Of FBO
	}
	else
	{
		fprintf(gpFile, "Create FBO Failed...\n");
		return -6;
	}
	
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

bool createFBO(GLint textureWidth, GLint textureHeight)
{
	// Local Variable Declarations
	int maxRenderbufferSize;

	// Code
	// Check Available Renderbuffer Size
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);
	if (maxRenderbufferSize < textureWidth || maxRenderbufferSize < textureHeight)
	{
		fprintf(gpFile, "Insufficient Render Buffer Size \n");
		return false;
	}

	// Create Framebuffer Object
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Create Renderbuffer Object
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	// Storage And Format Of The Renderbuffer
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, textureWidth, textureHeight);

	// Create Empty Texture For Upcoming Target Scene
	glGenTextures(1, &fbo_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGB,
			     textureWidth,
				 textureHeight,
				 0,
				 GL_RGB,
				 GL_UNSIGNED_SHORT_5_6_5,
				 NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);

	// Give Renderbuffer To FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check Whether The Framebuffeer Created Successful or Not
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(gpFile, "Frame Buffer Is Not Complete \n");
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

int initialize_sphere(int width, int height)
{
	// Variable Declarations
	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	// Function Declarations
	void resize_sphere(int, int);
	void uninitialize_sphere(void);

	// Vertex Shader PerFragment
	const GLchar* vertexShaderSourceCode_pf =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec3 a_normal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform vec4 u_lightPosition[3];" \
		"uniform int u_lightingEnabled;" \
		"out vec3 transformedNormals;" \
		"out vec3 lightDirection[3];" \
		"out vec3 viewerVector;" \
		"void main(void)" \
		"{" \
		"if (u_lightingEnabled == 1)" \
		"{" \
		"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" \
		"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
		"transformedNormals = normalMatrix * a_normal;" \
		"viewerVector = -eyeCoordinates.xyz;" \
		"for (int i = 0; i < 3; i++)" \
		"{" \
		"lightDirection[i] = vec3(u_lightPosition[i]) - eyeCoordinates.xyz;" \
		"}" \
		"}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;" \
		"}";

	// Creating Shader Object
	GLuint vertexShaderObject_pf = glCreateShader(GL_VERTEX_SHADER);
	// Giving Shader Code To Shader Object 
	glShaderSource(vertexShaderObject_pf, 1, (const GLchar**)&vertexShaderSourceCode_pf, NULL);
	// Compile The Shader
	glCompileShader(vertexShaderObject_pf);
	// Error Checking
	glGetShaderiv(vertexShaderObject_pf, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject_pf, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject_pf, infoLogLength, &written, log);
				fprintf(gpFile, "Vertex Shader Compilation Log: %s\n", log);
				free(log);
				uninitialize_sphere();
			}
		}
	}


	// Fragment Shader PerFragment
	const GLchar* fragmentShaderSourceCode_pf =
		"#version 460 core" \
		"\n" \
		"in vec3 transformedNormals;" \
		"in vec3 lightDirection[3];" \
		"in vec3 viewerVector;" \
		"uniform vec3 u_la[3];" \
		"uniform vec3 u_ld[3];" \
		"uniform vec3 u_ls[3];" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_materialShininess;" \
		"uniform int u_lightingEnabled;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec3 phong_ads_light;" \
		"if (u_lightingEnabled == 1)" \
		"{" \
		"vec3 normalized_tranformedNormals = normalize(transformedNormals);" \
		"vec3 normalized_viewerVector = normalize(viewerVector);" \
		"vec3 ambient[3];" \
		"vec3 normalized_lightDirection[3];" \
		"vec3 diffuse[3];" \
		"vec3 reflectionVector[3];" \
		"vec3 specular[3];" \
		"for (int i = 0; i < 3; i++)" \
		"{" \
		"ambient[i] = u_la[i] * u_ka;" \
		"normalized_lightDirection[i] = normalize(lightDirection[i]);" \
		"diffuse[i] = u_ld[i] * u_kd * max(dot(normalized_lightDirection[i], normalized_tranformedNormals), 0.0);" \
		"reflectionVector[i] = reflect(-normalized_lightDirection[i], normalized_tranformedNormals);" \
		"specular[i] = u_ls[i] * u_ks * pow(max(dot(reflectionVector[i], normalized_viewerVector), 0.0), u_materialShininess);" \
		"phong_ads_light += ambient[i] + diffuse[i] + specular[i];" \
		"}" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = vec3(1.0, 1.0, 1.0);" \
		"}" \
		"FragColor = vec4(phong_ads_light, 1.0);" \
		"}";

	GLuint fragmentShaderObject_pf = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject_pf, 1, (const GLchar**)&fragmentShaderSourceCode_pf, NULL);
	glCompileShader(fragmentShaderObject_pf);
	status = 0;
	infoLogLength = 0;
	log = NULL;
	glGetShaderiv(fragmentShaderObject_pf, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject_pf, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject_pf, infoLogLength, &written, log);
				fprintf(gpFile, "Fragment Shader Compilation Log: %s\n", log);
				free(log);
				uninitialize_sphere();
			}
		}
	}


	// Shader Program Object PerFragment

	shaderProgramObject_sphere = glCreateProgram();
	glAttachShader(shaderProgramObject_sphere, vertexShaderObject_pf);
	glAttachShader(shaderProgramObject_sphere, fragmentShaderObject_pf);
	glBindAttribLocation(shaderProgramObject_sphere, SBJ_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(shaderProgramObject_sphere, SBJ_ATTRIBUTE_NORMAL, "a_normal");
	glLinkProgram(shaderProgramObject_sphere);

	status = 0;
	infoLogLength = 0;
	log = NULL;
	glGetProgramiv(shaderProgramObject_sphere, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_sphere, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_sphere, infoLogLength, &written, log);
				fprintf(gpFile, "Shader Program Link Log: %s\n", log);
				free(log);
				uninitialize_sphere();
			}
		}
	}
	modelMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_modelMatrix");
	viewMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_viewMatrix");
	projectionMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_projectionMatrix");
	laUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_sphere, "u_la[0]");
	laUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_sphere, "u_la[1]");
	laUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_sphere, "u_la[2]");
	ldUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_sphere, "u_ld[0]");
	ldUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_sphere, "u_ld[1]");
	ldUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_sphere, "u_ld[2]");
	lsUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_sphere, "u_ls[0]");
	lsUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_sphere, "u_ls[1]");
	lsUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_sphere, "u_ls[2]");
	lightPositionUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_sphere, "u_lightPosition[0]");
	lightPositionUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_sphere, "u_lightPosition[1]");
	lightPositionUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_sphere, "u_lightPosition[2]");
	kaUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_ka");
	kdUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_kd");
	ksUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_ks");
	materialShininessUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_materialShininess");
	lightingEnabledUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_lightingEnabled");



	// Declaration Of Vertex Data Arrays
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices_sphere = getNumberOfSphereVertices();
	gNumElements_sphere = getNumberOfSphereElements();

	// vao And vbo Related Code
	glGenVertexArrays(1, &vao_Sphere);
	glBindVertexArray(vao_Sphere);

	glGenBuffers(1, &vbo_Sphere_Position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_Sphere_Normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(SBJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SBJ_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_Sphere_Elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// Here Starts OpenGL Code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// glShadeModel(),glHint() Deprecated In PP 

	// Clear The Screen Using Black Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	lights_sphere[0].lightAmbient_sphere = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lights_sphere[0].lightDiffuse_sphere = vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	lights_sphere[0].lightSpecular_sphere = vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	lights_sphere[0].lightPosition_sphere = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	lights_sphere[1].lightAmbient_sphere = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lights_sphere[1].lightDiffuse_sphere = vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	lights_sphere[1].lightSpecular_sphere = vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	lights_sphere[1].lightPosition_sphere = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	lights_sphere[2].lightAmbient_sphere = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lights_sphere[2].lightDiffuse_sphere = vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	lights_sphere[2].lightSpecular_sphere = vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	lights_sphere[2].lightPosition_sphere = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	// Warmup resize() Call
	resize_sphere(FBO_WIDTH, FBO_HEIGHT);

	return 0;
}

void resize(int width, int height)
{
	if (height == 0)
		height = 1;
		
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void resize_sphere(int width, int height)
{
	// Code
	if (height == 0)
		height = 1;					// To avoid 'Divide By 0' in future

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix_sphere = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void draw(void)
{
	// Function Declaration
	void display_sphere(GLint, GLint);
	void update_sphere(void);

	// Code
	if (bFBOResult == true)
	{
		display_sphere(FBO_WIDTH, FBO_HEIGHT);
		update_sphere();
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	resize(winWidth, winHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use The Shader Program Object
	glUseProgram(shaderProgramObject);

	// Cube
	// Transformations
	mat4 translationMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	mat4 rotationMatrix_x = mat4::identity();
	mat4 rotationMatrix_y = mat4::identity();
	mat4 rotationMatrix_z = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	scaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);
	rotationMatrix_x = vmath::rotate(angleCube, 1.0f, 0.0f, 0.0f);
	rotationMatrix_y = vmath::rotate(angleCube, 0.0f, 1.0f, 0.0f);
	rotationMatrix_z = vmath::rotate(angleCube, 0.0f, 0.0f, 1.0f);
	rotationMatrix = rotationMatrix_x * rotationMatrix_y * rotationMatrix_z;
	modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;				// Here Order Is Very Important
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);							// 80 Textures Can Be Activated
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glUniform1i(textureSamplerUniform, 0);

	glBindVertexArray(vao_cube);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Unuse The Shader Program Object
	glUseProgram(0);
	
	glXSwapBuffers(display, window);
}

void update(void)
{
	// Code
	angleCube = angleCube + 0.5f;
	if (angleCube >= 360.0f)
		angleCube = angleCube - 360.0f;
}

void display_sphere(GLint textureWidth, GLint textureHeight)
{
	// Code
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	resize_sphere(textureWidth, textureHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use The Shader Program Object
	glUseProgram(shaderProgramObject_sphere);

	// Transformations
	mat4 translationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform_sphere, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_sphere, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_sphere, 1, GL_FALSE, perspectiveProjectionMatrix_sphere);

	// Sending Light Related Uniforms
	if (bLight == True)
	{
		glUniform1i(lightingEnabledUniform_sphere, 1);
		for (int i = 0; i < 3; i++)
		{
			glUniform3fv(laUniform_sphere[i], 1, lights_sphere[i].lightAmbient_sphere);
			glUniform3fv(ldUniform_sphere[i], 1, lights_sphere[i].lightDiffuse_sphere);
			glUniform3fv(lsUniform_sphere[i], 1, lights_sphere[i].lightSpecular_sphere);
			glUniform4fv(lightPositionUniform_sphere[i], 1, lights_sphere[i].lightPosition_sphere);
		}

		glUniform3fv(kaUniform_sphere, 1, materialAmbient_sphere);
		glUniform3fv(kdUniform_sphere, 1, materialDiffuse_sphere);
		glUniform3fv(ksUniform_sphere, 1, materialSpecular_sphere);
		glUniform1f(materialShininessUniform_sphere, materialShininess_sphere);
	}
	else
	{
		glUniform1i(lightingEnabledUniform_sphere, 0);
	}

	glBindVertexArray(vao_Sphere);

	// Draw, either by 'glDrawTriangles()' or 'glDrawArrays()' or 'glDrawElements()'
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements_sphere, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	// Unuse The Shader Program Object
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void update_sphere(void)
{
	// Code

	lightAngleZero_sphere += 2.0f;
	lightAngleOne_sphere += 2.0f;
	lightAngleTwo_sphere += 2.0f;

	lights_sphere[0].lightPosition_sphere[1] = 10 * sin(lightAngleZero_sphere / 180.0f * 3.14f);
	lights_sphere[0].lightPosition_sphere[2] = 10 * cos(lightAngleZero_sphere / 180.0f * 3.14f);

	lights_sphere[1].lightPosition_sphere[0] = 10 * sin(lightAngleZero_sphere * 3.14f / 180.0f);
	lights_sphere[1].lightPosition_sphere[2] = 10 * cos(lightAngleZero_sphere * 3.14f / 180.0f);

	lights_sphere[2].lightPosition_sphere[0] = 10 * -sin(lightAngleZero_sphere * 3.14f / 180.0f);
	lights_sphere[2].lightPosition_sphere[1] = 10 * cos(lightAngleZero_sphere * 3.14f / 180.0f);

}

void uninitialize(void)
{
	// Function Declarations
	void uninitialize_sphere(void);

	// Local Variable
	GLXContext currentContext;
	
	// Code
	if (fullscreen)
	{
		toggleFullscreen();
	}

	uninitialize_sphere();

	//  Deletion And Uninitialization of vbo
	if (vbo_cube_texcoord)
	{
		glDeleteBuffers(1, &vbo_cube_texcoord);
		vbo_cube_texcoord = 0;
	}

	if (vbo_cube_position)
	{
		glDeleteBuffers(1, &vbo_cube_position);
		vbo_cube_position = 0;
	}

	// Deletion And Uninitialization of vao
	if (vao_cube)
	{
		glDeleteVertexArrays(1, &vao_cube);
		vao_cube = 0;
	}
	
	// Shader uninitialization
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
		GLsizei numAttachedShaders;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);
		GLuint* shaderObjects = NULL;
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
		//if (shaderObject)
			// Error Checking
		glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);
		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}
		free(shaderObjects);
		shaderObjects = NULL;
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
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

void uninitialize_sphere(void)
{

	// Code
	//  Deletion And Uninitialization of vbo
	if (vbo_Sphere_Elements)
	{
		glDeleteBuffers(1, &vbo_Sphere_Elements);
		vbo_Sphere_Elements = 0;
	}

	if (vbo_Sphere_Normals)
	{
		glDeleteBuffers(1, &vbo_Sphere_Normals);
		vbo_Sphere_Normals = 0;
	}

	if (vbo_Sphere_Position)
	{
		glDeleteBuffers(1, &vbo_Sphere_Position);
		vbo_Sphere_Position = 0;
	}

	// Deletion And Uninitialization of vao
	if (vao_Sphere)
	{
		glDeleteVertexArrays(1, &vao_Sphere);
		vao_Sphere = 0;
	}

	// Shader uninitialization
	if (shaderProgramObject_sphere)
	{
		glUseProgram(shaderProgramObject_sphere);
		GLsizei numAttachedShaders;
		glGetProgramiv(shaderProgramObject_sphere, GL_ATTACHED_SHADERS, &numAttachedShaders);
		GLuint* shaderObjects = NULL;
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
		//if (shaderObject)
			// Error Checking
		glGetAttachedShaders(shaderProgramObject_sphere, numAttachedShaders, &numAttachedShaders, shaderObjects);
		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject_sphere, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}
		free(shaderObjects);
		shaderObjects = NULL;
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_sphere);
		shaderProgramObject_sphere = 0;
	}
}
