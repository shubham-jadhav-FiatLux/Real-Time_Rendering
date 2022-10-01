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

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int gNumVertices;
unsigned int gNumElements;

Bool bLight = False;
GLchar choosenShader = 'v';
GLfloat lightAngleZero = 0.0f;
GLfloat lightAngleOne = 0.0f;
GLfloat lightAngleTwo = 0.0f;

// Programmable Pipeline Related Global Variables
GLuint shaderProgramObject_pv;
GLuint shaderProgramObject_pf;

enum
{
	SBJ_ATTRIBUTE_POSITION = 0,
	SBJ_ATTRIBUTE_COLOR,
	SBJ_ATTRIBUTE_NORMAL,
	SBJ_ATTRIBUTE_TEXTURE0
};

GLuint vao_Sphere;
GLuint vbo_Sphere_Position;
GLuint vbo_Sphere_Normals;
GLuint vbo_Sphere_Elements;

GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionMatrixUniform;

GLuint laUniform[3];				// Ambient
GLuint ldUniform[3];				// Diffuse
GLuint lsUniform[3];				// Specular
GLuint lightPositionUniform[3];

struct Light
{
	vec4 lightAmbient;
	vec4 lightDiffuse;
	vec4 lightSpecular;
	vec4 lightPosition;
};
Light lights[3];

GLuint kaUniform;
GLuint kdUniform;
GLuint ksUniform;
GLuint materialShininessUniform;

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 50.0f;

GLuint lightingEnabledUniform;

mat4 perspectiveProjectionMatrix;

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
	static int winWidth, winHeight; 
	int iRetVal = 0; 
	
	static int frameBufferAttributes[] = 
		{ 
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
		  GLX_DOUBLEBUFFER, True,
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

				XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);				// Like WM_CHAR
				switch (keys[0])
				{
				
				case 'Q':
				case 'q':
					bDone = True;
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

				case 'V':
				case 'v':
					choosenShader = 'v';
					break;

				case 'F':
				case 'f':
					choosenShader = 'f';
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
	
	// Vertex Shader PerVertex
	const GLchar* vertexShaderSourceCode_pv =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec3 a_normal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform vec3 u_la[3];" \
		"uniform vec3 u_ld[3];" \
		"uniform vec3 u_ls[3];" \
		"uniform vec4 u_lightPosition[3];" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_materialShininess;" \
		"uniform int u_lightingEnabled;" \
		"out vec3 phong_ads_light;" \
		"void main(void)" \
		"{" \
		"if (u_lightingEnabled == 1)" \
		"{" \
		"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" \
		"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
		"vec3 transformedNormals = normalize(normalMatrix * a_normal);" \
		"vec3 viewerVector = normalize(-eyeCoordinates.xyz);" \
		"vec3 ambient[3];" \
		"vec3 lightDirection[3];" \
		"vec3 diffuse[3];" \
		"vec3 reflectionVector[3];" \
		"vec3 specular[3];" \
		"for (int i = 0; i < 3; i++)" \
		"{" \
		"ambient[i] = u_la[i] * u_ka;" \
		"lightDirection[i] = normalize(vec3(u_lightPosition[i]) - eyeCoordinates.xyz);" \
		"diffuse[i] = u_ld[i] * u_kd * max(dot(lightDirection[i], transformedNormals), 0.0);" \
		"reflectionVector[i] = reflect(-lightDirection[i], transformedNormals);" \
		"specular[i] = u_ls[i] * u_ks * pow(max(dot(reflectionVector[i], viewerVector), 0.0), u_materialShininess);" \
		"phong_ads_light = phong_ads_light + ambient[i] + diffuse[i] + specular[i];" \
		"}" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = vec3(1.0, 1.0, 1.0);" \
		"}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;" \
		"}";

	// Creating Shader Object
	GLuint vertexShaderObject_pv = glCreateShader(GL_VERTEX_SHADER);
	// Giving Shader Code To Shader Object 
	glShaderSource(vertexShaderObject_pv, 1, (const GLchar**)&vertexShaderSourceCode_pv, NULL);
	// Compile The Shader
	glCompileShader(vertexShaderObject_pv);
	// Error Checking
	glGetShaderiv(vertexShaderObject_pv, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject_pv, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject_pv, infoLogLength, &written, log);
				fprintf(gpFile, "Vertex Shader Compilation Log: %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}
	
	
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
				uninitialize();
			}
		}
	}
	
	
	// Fragment Shader PerVertex
	const GLchar* fragmentShaderSourceCode_pv =
		"#version 460 core" \
		"\n" \
		"in vec3 phong_ads_light;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(phong_ads_light, 1.0);" \
		"}";

	GLuint fragmentShaderObject_pv = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject_pv, 1, (const GLchar**)&fragmentShaderSourceCode_pv, NULL);
	glCompileShader(fragmentShaderObject_pv);
	status = 0;
	infoLogLength = 0;
	log = NULL;
	glGetShaderiv(fragmentShaderObject_pv, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject_pv, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject_pv, infoLogLength, &written, log);
				fprintf(gpFile, "Fragment Shader Compilation Log: %s\n", log);
				free(log);
				uninitialize();
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
				uninitialize();
			}
		}
	}
	
	
	// Shader Program Object PerVertex
	shaderProgramObject_pv = glCreateProgram();
	glAttachShader(shaderProgramObject_pv, vertexShaderObject_pv);
	glAttachShader(shaderProgramObject_pv, fragmentShaderObject_pv);
	glBindAttribLocation(shaderProgramObject_pv, SBJ_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(shaderProgramObject_pv, SBJ_ATTRIBUTE_NORMAL, "a_normal");
	glLinkProgram(shaderProgramObject_pv);
	status = 0;
	infoLogLength = 0;
	log = NULL;
	glGetProgramiv(shaderProgramObject_pv, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_pv, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_pv, infoLogLength, &written, log);
				fprintf(gpFile, "Shader Program Link Log: %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject_pv, "u_modelMatrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject_pv, "u_viewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject_pv, "u_projectionMatrix");
	laUniform[0] = glGetUniformLocation(shaderProgramObject_pv, "u_la[0]");
	laUniform[1] = glGetUniformLocation(shaderProgramObject_pv, "u_la[1]");
	laUniform[2] = glGetUniformLocation(shaderProgramObject_pv, "u_la[2]");
	ldUniform[0] = glGetUniformLocation(shaderProgramObject_pv, "u_ld[0]");
	ldUniform[1] = glGetUniformLocation(shaderProgramObject_pv, "u_ld[1]");
	ldUniform[2] = glGetUniformLocation(shaderProgramObject_pv, "u_ld[2]");
	lsUniform[0] = glGetUniformLocation(shaderProgramObject_pv, "u_ls[0]");
	lsUniform[1] = glGetUniformLocation(shaderProgramObject_pv, "u_ls[1]");
	lsUniform[2] = glGetUniformLocation(shaderProgramObject_pv, "u_ls[2]");
	lightPositionUniform[0] = glGetUniformLocation(shaderProgramObject_pv, "u_lightPosition[0]");
	lightPositionUniform[1] = glGetUniformLocation(shaderProgramObject_pv, "u_lightPosition[1]");
	lightPositionUniform[2] = glGetUniformLocation(shaderProgramObject_pv, "u_lightPosition[2]");
	kaUniform = glGetUniformLocation(shaderProgramObject_pv, "u_ka");
	kdUniform = glGetUniformLocation(shaderProgramObject_pv, "u_kd");
	ksUniform = glGetUniformLocation(shaderProgramObject_pv, "u_ks");
	materialShininessUniform = glGetUniformLocation(shaderProgramObject_pv, "u_materialShininess");
	lightingEnabledUniform = glGetUniformLocation(shaderProgramObject_pv, "u_lightingEnabled");
	
	
	// Shader Program Object PerFragment
	shaderProgramObject_pf = glCreateProgram();
	glAttachShader(shaderProgramObject_pf, vertexShaderObject_pf);
	glAttachShader(shaderProgramObject_pf, fragmentShaderObject_pf);
	glBindAttribLocation(shaderProgramObject_pf, SBJ_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(shaderProgramObject_pf, SBJ_ATTRIBUTE_NORMAL, "a_normal");
	glLinkProgram(shaderProgramObject_pf);
	status = 0;
	infoLogLength = 0;
	log = NULL;
	glGetProgramiv(shaderProgramObject_pf, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_pf, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_pf, infoLogLength, &written, log);
				fprintf(gpFile, "Shader Program Link Log: %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject_pf, "u_modelMatrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject_pf, "u_viewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject_pf, "u_projectionMatrix");
	laUniform[0] = glGetUniformLocation(shaderProgramObject_pf, "u_la[0]");
	laUniform[1] = glGetUniformLocation(shaderProgramObject_pf, "u_la[1]");
	laUniform[2] = glGetUniformLocation(shaderProgramObject_pf, "u_la[2]");
	ldUniform[0] = glGetUniformLocation(shaderProgramObject_pf, "u_ld[0]");
	ldUniform[1] = glGetUniformLocation(shaderProgramObject_pf, "u_ld[1]");
	ldUniform[2] = glGetUniformLocation(shaderProgramObject_pf, "u_ld[2]");
	lsUniform[0] = glGetUniformLocation(shaderProgramObject_pf, "u_ls[0]");
	lsUniform[1] = glGetUniformLocation(shaderProgramObject_pf, "u_ls[1]");
	lsUniform[2] = glGetUniformLocation(shaderProgramObject_pf, "u_ls[2]");
	lightPositionUniform[0] = glGetUniformLocation(shaderProgramObject_pf, "u_lightPosition[0]");
	lightPositionUniform[1] = glGetUniformLocation(shaderProgramObject_pf, "u_lightPosition[1]");
	lightPositionUniform[2] = glGetUniformLocation(shaderProgramObject_pf, "u_lightPosition[2]");
	kaUniform = glGetUniformLocation(shaderProgramObject_pf, "u_ka");
	kdUniform = glGetUniformLocation(shaderProgramObject_pf, "u_kd");
	ksUniform = glGetUniformLocation(shaderProgramObject_pf, "u_ks");
	materialShininessUniform = glGetUniformLocation(shaderProgramObject_pf, "u_materialShininess");
	lightingEnabledUniform = glGetUniformLocation(shaderProgramObject_pf, "u_lightingEnabled");
	
	

	// Declaration Of Vertex Data Arrays
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

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
	
	// Depth Related Changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glShadeModel(),glHint() Deprecated In PP

	// Here Starts OpenGL Code
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	
	lights[0].lightAmbient = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lights[0].lightDiffuse = vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	lights[0].lightSpecular = vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	lights[0].lightPosition = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	lights[1].lightAmbient = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lights[1].lightDiffuse = vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	lights[1].lightSpecular = vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	lights[1].lightPosition = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	lights[2].lightAmbient = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lights[2].lightDiffuse = vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	lights[2].lightSpecular = vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	lights[2].lightPosition = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

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

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void draw(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use The Shader Program Object
	if (choosenShader == 'v')
		glUseProgram(shaderProgramObject_pv);
	else if (choosenShader == 'f')
		glUseProgram(shaderProgramObject_pf);

	// Transformations
	mat4 translationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	// Sending Light Related Uniforms
	if (bLight == True)
	{
		glUniform1i(lightingEnabledUniform, 1);
		for (int i = 0; i < 3; i++)
		{
			glUniform3fv(laUniform[i], 1, lights[i].lightAmbient);
			glUniform3fv(ldUniform[i], 1, lights[i].lightDiffuse);
			glUniform3fv(lsUniform[i], 1, lights[i].lightSpecular);
			glUniform4fv(lightPositionUniform[i], 1, lights[i].lightPosition);
		}

		glUniform3fv(kaUniform, 1, materialAmbient);
		glUniform3fv(kdUniform, 1, materialDiffuse);
		glUniform3fv(ksUniform, 1, materialSpecular);
		glUniform1f(materialShininessUniform, materialShininess);
	}
	else
	{
		glUniform1i(lightingEnabledUniform, 0);
	}

	glBindVertexArray(vao_Sphere);

	// Draw, either by 'glDrawTriangles()' or 'glDrawArrays()' or 'glDrawElements()'
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	// Unuse The Shader Program Object
	glUseProgram(0);
	
	glXSwapBuffers(display, window);
}

void update(void)
{
	// Code
	lightAngleZero += 2.0f;
	lightAngleOne += 2.0f;
	lightAngleTwo += 2.0f;

	lights[0].lightPosition[1] = 10 * sin(lightAngleZero / 180.0f * 3.14f );
	lights[0].lightPosition[2] = 10 * cos(lightAngleZero / 180.0f * 3.14f );

	lights[1].lightPosition[0] = 10 * sin(lightAngleZero * 3.14f / 180.0f);
	lights[1].lightPosition[2] = 10 * cos(lightAngleZero * 3.14f / 180.0f);

	lights[2].lightPosition[0] = 10 * -sin(lightAngleZero * 3.14f / 180.0f);
	lights[2].lightPosition[1] = 10 * cos(lightAngleZero * 3.14f / 180.0f);
}

void uninitialize(void)
{
	// Local Variable
	GLXContext currentContext;
	
	// Code
	if (fullscreen)
	{
		toggleFullscreen();
	}

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
	if (shaderProgramObject_pf)
	{
		glUseProgram(shaderProgramObject_pf);
		GLsizei numAttachedShaders;
		glGetProgramiv(shaderProgramObject_pf, GL_ATTACHED_SHADERS, &numAttachedShaders);
		GLuint* shaderObjects = NULL;
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
		//if (shaderObject)
			// Error Checking
		glGetAttachedShaders(shaderProgramObject_pf, numAttachedShaders, &numAttachedShaders, shaderObjects);
		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject_pf, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}
		free(shaderObjects);
		shaderObjects = NULL;
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_pf);
		shaderProgramObject_pf = 0;
	}
	
	if (shaderProgramObject_pv)
	{
		glUseProgram(shaderProgramObject_pv);
		GLsizei numAttachedShaders;
		glGetProgramiv(shaderProgramObject_pv, GL_ATTACHED_SHADERS, &numAttachedShaders);
		GLuint* shaderObjects = NULL;
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
		//if (shaderObject)
			// Error Checking
		glGetAttachedShaders(shaderProgramObject_pv, numAttachedShaders, &numAttachedShaders, shaderObjects);
		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject_pv, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}
		free(shaderObjects);
		shaderObjects = NULL;
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_pv);
		shaderProgramObject_pv = 0;
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
