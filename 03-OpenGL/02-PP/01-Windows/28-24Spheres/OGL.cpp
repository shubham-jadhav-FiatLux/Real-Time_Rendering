// Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>
#include "Sphere.h"

// OpenGL Header Files
#include <GL/glew.h>		// This Must Be Before Including 'gl.h'
#include <GL/gl.h>
#include "vmath.h"
using namespace vmath;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// OpenGL Libraries
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "Sphere.lib")

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable Declarations
FILE* gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
static BOOL gbFullscreen = FALSE;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int gNumVertices;
unsigned int gNumElements;

BOOL bLight = FALSE;

// Programmable Pipeline Related Global Variables
GLuint shaderProgramObject;

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

GLuint laUniform;				// Ambient
GLuint ldUniform;				// Diffuse
GLuint lsUniform;				// Specular
GLuint lightPositionUniform;

GLuint kaUniform;
GLuint kdUniform;
GLuint ksUniform;
GLuint materialShininessUniform;

GLuint lightingEnabledUniform;

GLint KeyPressed = 0;
GLfloat AngleForXRotation = 0.0f;
GLfloat AngleForYRotation = 0.0f;
GLfloat AngleForZRotation = 0.0f;
GLfloat currentWidth = WIN_WIDTH;
GLfloat currentHeight = WIN_HEIGHT;


GLfloat LightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };


mat4 perspectiveProjectionMatrix;

// Entry-Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Declarations
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);
	void ToggleFullscreen(void);
	
	// Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	BOOL bDone = FALSE;
	int iRetVal = 0;

	// Code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation of Log file failed. Exiting ..."), TEXT("File I/O Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Successfully Opened.\n");
	}

	// Initialization Of WNDCLASSEX Structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// Register WNDCLASSEX
	RegisterClassEx(&wndclass);

	// Create The Window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("OpenGL Window"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(GetSystemMetrics(SM_CXSCREEN) / 2 - WIN_WIDTH / 2),
		(GetSystemMetrics(SM_CYSCREEN) / 2 - WIN_HEIGHT / 2),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	// initialize
	iRetVal = initialize();
	if (iRetVal == -1)
	{
		fprintf(gpFile, "CHOOSE PIXEL FORMAT FAILED.\n");
		uninitialize();
	}
	else if (iRetVal == -2)
	{
		fprintf(gpFile, "SET PIXEL FORMAT FAILED.\n");
		uninitialize();
	}
	else if (iRetVal == -3)
	{
		fprintf(gpFile, "CREATE OpenGL CONTEXT FAILED.\n");
		uninitialize();
	}
	else if (iRetVal == -4)
	{
		fprintf(gpFile, "MAKING OpenGL CONTEXT AS CURRENT CONTEXT FAILED.\n");
		uninitialize();
	}
	else if (iRetVal == -5)
	{
		fprintf(gpFile, "GLEW Initialization Failed.\n");
		uninitialize();
	}
	else
		fprintf(gpFile, "Succeed.\n");

	// Show Window
	ShowWindow(hwnd, iCmdShow);

	// Foregrounding And Focusing The Window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Fullscreen
	ToggleFullscreen();

	// Game Loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == TRUE)
			{
				// Render The Scene
				display();

				// Update The Scene
				update();
			}
		}
	}

	uninitialize();

	return((int)msg.wParam);
}

// Callback Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Function Declarations
	void ToggleFullscreen(void);
	void resize(int, int);

	// Code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;

	case WM_ERASEBKGND:
		return 0;

	case WM_CHAR:
		switch (wParam)
		{
		case 'F':
		case 'f':
			ToggleFullscreen();
			break;

		case 'L':
		case 'l':
			if (bLight == FALSE)
			{
				bLight = TRUE;
			}
			else
			{
				bLight = FALSE;
			}
			break;

		case 'X':
		case 'x':
			KeyPressed = 1;
			AngleForXRotation = 0.0f;		// Reset
			AngleForYRotation = 0.0f;
			AngleForZRotation = 0.0f;
			break;

		case 'Y':
		case 'y':
			KeyPressed = 2;
			AngleForXRotation += 0.0f;
			AngleForYRotation = 0.0f;
			AngleForZRotation = 0.0f;
			break;

		case 'Z':
		case 'z':
			KeyPressed = 3;
			AngleForXRotation += 0.0f;
			AngleForYRotation = 0.0f;
			AngleForZRotation = 0.0f;
			break;

		default:
			KeyPressed = 0;
			break;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case 27:
			DestroyWindow(hwnd);
			break;

		default:
			break;
		}
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		currentWidth = LOWORD(lParam);
		currentHeight = HIWORD(lParam);
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	// Variable Declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;

	// Code
	wp.length = sizeof(WINDOWPLACEMENT);

	if (gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);
			if (GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			ShowCursor(FALSE);
			gbFullscreen = TRUE;
		}
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullscreen = FALSE;
	}
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

	// Variable Declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	// Code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));			// MemSet((void*)&pfd, NULL, sizof(PIXELFORMATDESCRIPTOR));

	// Initialization of PIXELFORMAT
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	// GetDC
	ghdc = GetDC(ghwnd);

	// Choose Pixel Format
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		return -1;
	}

	// Set The Choosen Pixel Format
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
		return -2;

	// Create OpenGl Rendering Context
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
		return -3;

	// Make The Rendering Context As The Current Context
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
		return -4;

	// GLEW Initialization
	if (glewInit() != GLEW_OK)
		return -5;

	// Print OpenGL Info
	printGLInfo();

	// Vertex Shader
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec3 a_normal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform vec4 u_lightPosition;" \
		"uniform int u_lightingEnabled;" \
		"out vec3 transformedNormals;" \
		"out vec3 lightDirection;" \
		"out vec3 viewerVector;" \
		"void main(void)" \
		"{" \
		"if (u_lightingEnabled == 1)" \
		"{" \
		"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" \
		"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
		"transformedNormals = normalMatrix * a_normal;" \
		"lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;" \
		"viewerVector = -eyeCoordinates.xyz;" \
		"}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;" \
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
		"in vec3 transformedNormals;" \
		"in vec3 lightDirection;" \
		"in vec3 viewerVector;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
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
		"vec3 ambient = u_la * u_ka;" \
		"vec3 normalized_tranformed_normals = normalize(transformedNormals);" \
		"vec3 normalized_lightDirection = normalize(lightDirection);" \
		"vec3 diffuse = u_ld * u_kd * max(dot(normalized_lightDirection, normalized_tranformed_normals), 0.0);" \
		"vec3 reflectionVector = reflect(-normalized_lightDirection, normalized_lightDirection);" \
		"vec3 normalized_viewerVector = normalize(viewerVector);" \
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector, normalized_viewerVector), 0.0), u_materialShininess);" \
		"phong_ads_light = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = vec3(1.0, 1.0, 1.0);" \
		"}" \
		"FragColor = vec4(phong_ads_light, 1.0);" \
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
	glBindAttribLocation(shaderProgramObject, SBJ_ATTRIBUTE_NORMAL, "a_normal");
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
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
	laUniform = glGetUniformLocation(shaderProgramObject, "u_la");
	ldUniform = glGetUniformLocation(shaderProgramObject, "u_ld");
	lsUniform = glGetUniformLocation(shaderProgramObject, "u_ls");
	lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_lightPosition");
	kaUniform = glGetUniformLocation(shaderProgramObject, "u_ka");
	kdUniform = glGetUniformLocation(shaderProgramObject, "u_kd");
	ksUniform = glGetUniformLocation(shaderProgramObject, "u_ks");
	materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_materialShininess");
	lightingEnabledUniform = glGetUniformLocation(shaderProgramObject, "u_lightingEnabled");

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

	// Here Starts OpenGL Code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// glShadeModel(),glHint() Deprecated In PP 

	// Clear The Screen Using Black Color
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

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
	// Code
	if (height == 0)
		height = 1;					// To avoid 'Divide By 0' in future

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// Function Declaration
	void Draw24Spheres(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use The Shader Program Object
	glUseProgram(shaderProgramObject);

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
	if (bLight == TRUE)
	{
		glUniform1i(lightingEnabledUniform, 1);

		/*glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);
		*/
	}
	else
	{
		glUniform1i(lightingEnabledUniform, 0);
	}

	Draw24Spheres();
	
	// Unuse The Shader Program Object
	glUseProgram(0);
	
	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	if (KeyPressed == 1)
	{
		AngleForXRotation += 2.0f;
		AngleForYRotation = 0.0f;
		AngleForZRotation = 0.0f;
		//if (AngleForXRotation >= 360.0f)
			//AngleForXRotation -= 360.0f;
	}
	else if (KeyPressed == 2)
	{

		AngleForYRotation += 2.0f;
		AngleForXRotation += 2.0f;
		AngleForZRotation = 0.0f;
		//if (AngleForYRotation >= 360.0f)
			//AngleForYRotation -= 360.0f;
	}
	else if (KeyPressed == 3)
	{
		AngleForZRotation += 2.0f;
		AngleForXRotation += 2.0f;
		AngleForYRotation = 0.0f;
		//if (AngleForZRotation >= 360.0f)
			//AngleForZRotation -= 360.0f;
	}


	if (KeyPressed == 1)								// X Rotation
	{
		LightPosition[1] = 10 * sin(AngleForXRotation * M_PI / 180.0f);
		LightPosition[2] = 10 * cos(AngleForXRotation * M_PI / 180.0f);
	}
	else if (KeyPressed == 2)							// Y Rotation
	{
		LightPosition[0] = 10 * sin(AngleForYRotation * M_PI / 180.0f);
		LightPosition[2] = 10 * cos(-AngleForYRotation * M_PI / 180.0f);
	}
	else if (KeyPressed == 3)							// Z Rotation
	{
		LightPosition[0] = 10 * -sin(AngleForZRotation * M_PI / 180.0f);
		LightPosition[1] = 10 * cos(AngleForZRotation * M_PI / 180.0f);
	}
	else
	{
		LightPosition[0] = 0.0f;
		LightPosition[1] = 0.0f;
		LightPosition[2] = 0.0f;
	}
}

void Draw24Spheres(void)
{
	// Variable Declarations
	GLfloat MaterialAmbient[4];
	GLfloat MaterialDiffuse[4];
	GLfloat MaterialSpecular[4];
	GLfloat MaterialShininess;

	GLfloat viewportWidth = (GLsizei)currentWidth / 4;
	GLfloat viewportHeight = (GLsizei)currentHeight / 6;

	mat4 translationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

	GLfloat zVal = -2.0f;

	// Code
	// ***** 1st sphere on 1st column, emerald *****
	// ambient material
	MaterialAmbient[0] = 0.0215; // r
	MaterialAmbient[1] = 0.1745; // g
	MaterialAmbient[2] = 0.0215; // b
	MaterialAmbient[3] = 1.0f;   // a

	// diffuse material
	MaterialDiffuse[0] = 0.07568; // r
	MaterialDiffuse[1] = 0.61424; // g
	MaterialDiffuse[2] = 0.07568; // b
	MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.633;    // r
	MaterialSpecular[1] = 0.727811; // g
	MaterialSpecular[2] = 0.633;    // b
	MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.6 * 128;

	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(0, 5 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************
	
	// ***** 2nd sphere on 1st column, jade *****
	// ambient material
	MaterialAmbient[0] = 0.135;  // r
	MaterialAmbient[1] = 0.2225; // g
	MaterialAmbient[2] = 0.1575; // b
	MaterialAmbient[3] = 1.0f;   // a

	// diffuse material
	MaterialDiffuse[0] = 0.54; // r
	MaterialDiffuse[1] = 0.89; // g
	MaterialDiffuse[2] = 0.63; // b
	MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.316228; // r
	MaterialSpecular[1] = 0.316228; // g
	MaterialSpecular[2] = 0.316228; // b
	MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.1 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(0, 4 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************
	
	// ***** 3rd sphere on 1st column, obsidian *****
	// ambient material
	MaterialAmbient[0] = 0.05375; // r
	MaterialAmbient[1] = 0.05;    // g
	MaterialAmbient[2] = 0.06625; // b
	MaterialAmbient[3] = 1.0f;    // a

	// diffuse material
	MaterialDiffuse[0] = 0.18275; // r
	MaterialDiffuse[1] = 0.17;    // g
	MaterialDiffuse[2] = 0.22525; // b
	MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.332741; // r
	MaterialSpecular[1] = 0.328634; // g
	MaterialSpecular[2] = 0.346435; // b
	MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.3 * 128;

	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(0, 3 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 4th sphere on 1st column, pearl *****
	// ambient material
	MaterialAmbient[0] = 0.25;    // r
	MaterialAmbient[1] = 0.20725; // g
	MaterialAmbient[2] = 0.20725; // b
	MaterialAmbient[3] = 1.0f;    // a

	// diffuse material
	MaterialDiffuse[0] = 1.0;   // r
	MaterialDiffuse[1] = 0.829; // g
	MaterialDiffuse[2] = 0.829; // b
	MaterialDiffuse[3] = 1.0f;  // a

	// specular material
	MaterialSpecular[0] = 0.296648; // r
	MaterialSpecular[1] = 0.296648; // g
	MaterialSpecular[2] = 0.296648; // b
	MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.088 * 128;

	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(0, 2 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 5th sphere on 1st column, ruby *****
	// ambient material
	MaterialAmbient[0] = 0.1745;  // r
	MaterialAmbient[1] = 0.01175; // g
	MaterialAmbient[2] = 0.01175; // b
	MaterialAmbient[3] = 1.0f;    // a

	// diffuse material
	MaterialDiffuse[0] = 0.61424; // r
	MaterialDiffuse[1] = 0.04136; // g
	MaterialDiffuse[2] = 0.04136; // b
	MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.727811; // r
	MaterialSpecular[1] = 0.626959; // g
	MaterialSpecular[2] = 0.626959; // b
	MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.6 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(0, 1 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 6th sphere on 1st column, turquoise *****
	// ambient material
	MaterialAmbient[0] = 0.1;     // r
	MaterialAmbient[1] = 0.18725; // g
	MaterialAmbient[2] = 0.1745;  // b
	MaterialAmbient[3] = 1.0f;    // a

	// diffuse material
	MaterialDiffuse[0] = 0.396;   // r
	MaterialDiffuse[1] = 0.74151; // g
	MaterialDiffuse[2] = 0.69102; // b
	MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.297254; // r
	MaterialSpecular[1] = 0.30829;  // g
	MaterialSpecular[2] = 0.306678; // b
	MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.1 * 128;

	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(0, 0 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************
	// *******************************************************
	// *******************************************************

	// ***** 1st sphere on 2nd column, brass *****
	// ambient material
	MaterialAmbient[0] = 0.329412; // r
	MaterialAmbient[1] = 0.223529; // g
	MaterialAmbient[2] = 0.027451; // b
	MaterialAmbient[3] = 1.0f;     // a

	// diffuse material
	MaterialDiffuse[0] = 0.780392; // r
	MaterialDiffuse[1] = 0.568627; // g
	MaterialDiffuse[2] = 0.113725; // b
	MaterialDiffuse[3] = 1.0f;     // a

	// specular material
	MaterialSpecular[0] = 0.992157; // r
	MaterialSpecular[1] = 0.941176; // g
	MaterialSpecular[2] = 0.807843; // b
	MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.21794872 * 128;

	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(1 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 2nd sphere on 2nd column, bronze *****
	// ambient material
	MaterialAmbient[0] = 0.2125; // r
	MaterialAmbient[1] = 0.1275; // g
	MaterialAmbient[2] = 0.054;  // b
	MaterialAmbient[3] = 1.0f;   // a

	// diffuse material
	MaterialDiffuse[0] = 0.714;   // r
	MaterialDiffuse[1] = 0.4284;  // g
	MaterialDiffuse[2] = 0.18144; // b
	MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.393548; // r
	MaterialSpecular[1] = 0.271906; // g
	MaterialSpecular[2] = 0.166721; // b
	MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.2 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(1 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 3rd sphere on 2nd column, chrome *****
	// ambient material
	MaterialAmbient[0] = 0.25; // r
	MaterialAmbient[1] = 0.25; // g
	MaterialAmbient[2] = 0.25; // b
	MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.4;  // r
	MaterialDiffuse[1] = 0.4;  // g
	MaterialDiffuse[2] = 0.4;  // b
	MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.774597; // r
	MaterialSpecular[1] = 0.774597; // g
	MaterialSpecular[2] = 0.774597; // b
	MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.6 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(1 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 4th sphere on 2nd column, copper *****
	// ambient material
	MaterialAmbient[0] = 0.19125; // r
	MaterialAmbient[1] = 0.0735;  // g
	MaterialAmbient[2] = 0.0225;  // b
	MaterialAmbient[3] = 1.0f;    // a

	// diffuse material
	MaterialDiffuse[0] = 0.7038;  // r
	MaterialDiffuse[1] = 0.27048; // g
	MaterialDiffuse[2] = 0.0828;  // b
	MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.256777; // r
	MaterialSpecular[1] = 0.137622; // g
	MaterialSpecular[2] = 0.086014; // b
	MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.1 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(1 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 5th sphere on 2nd column, gold *****
	// ambient material
	MaterialAmbient[0] = 0.24725; // r
	MaterialAmbient[1] = 0.1995;  // g
	MaterialAmbient[2] = 0.0745;  // b
	MaterialAmbient[3] = 1.0f;    // a

	// diffuse material
	MaterialDiffuse[0] = 0.75164; // r
	MaterialDiffuse[1] = 0.60648; // g
	MaterialDiffuse[2] = 0.22648; // b
	MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.628281; // r
	MaterialSpecular[1] = 0.555802; // g
	MaterialSpecular[2] = 0.366065; // b
	MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.4 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(1 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 6th sphere on 2nd column, silver *****
	// ambient material
	MaterialAmbient[0] = 0.19225; // r
	MaterialAmbient[1] = 0.19225; // g
	MaterialAmbient[2] = 0.19225; // b
	MaterialAmbient[3] = 1.0f;    // a
	
	// diffuse material
	MaterialDiffuse[0] = 0.50754; // r
	MaterialDiffuse[1] = 0.50754; // g
	MaterialDiffuse[2] = 0.50754; // b
	MaterialDiffuse[3] = 1.0f;    // a
	
	// specular material
	MaterialSpecular[0] = 0.508273; // r
	MaterialSpecular[1] = 0.508273; // g
	MaterialSpecular[2] = 0.508273; // b
	MaterialSpecular[3] = 1.0f;     // a
	
	// shininess
	MaterialShininess = 0.4 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(1 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************
	// *******************************************************
	// *******************************************************

	// ***** 1st sphere on 3rd column, black *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	
	// diffuse material
	MaterialDiffuse[0] = 0.01; // r
	MaterialDiffuse[1] = 0.01; // g
	MaterialDiffuse[2] = 0.01; // b
	MaterialDiffuse[3] = 1.0f; // a
	
	// specular material
	MaterialSpecular[0] = 0.50; // r
	MaterialSpecular[1] = 0.50; // g
	MaterialSpecular[2] = 0.50; // b
	MaterialSpecular[3] = 1.0f; // a
	
	// shininess
	MaterialShininess = 0.25 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(2 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 2nd sphere on 3rd column, cyan *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.1;  // g
	MaterialAmbient[2] = 0.06; // b
	MaterialAmbient[3] = 1.0f; // a
	
	// diffuse material
	MaterialDiffuse[0] = 0.0;        // r
	MaterialDiffuse[1] = 0.50980392; // g
	MaterialDiffuse[2] = 0.50980392; // b
	MaterialDiffuse[3] = 1.0f;       // a
	
	// specular material
	MaterialSpecular[0] = 0.50196078; // r
	MaterialSpecular[1] = 0.50196078; // g
	MaterialSpecular[2] = 0.50196078; // b
	MaterialSpecular[3] = 1.0f;       // a
	
	// shininess
	MaterialShininess = 0.25 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(2 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 3rd sphere on 2nd column, green *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.1;  // r
	MaterialDiffuse[1] = 0.35; // g
	MaterialDiffuse[2] = 0.1;  // b
	MaterialDiffuse[3] = 1.0f; // a
	
	// specular material
	MaterialSpecular[0] = 0.45; // r
	MaterialSpecular[1] = 0.55; // g
	MaterialSpecular[2] = 0.45; // b
	MaterialSpecular[3] = 1.0f; // a
	
	// shininess
	MaterialShininess = 0.25 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(2 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 4th sphere on 3rd column, red *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	
	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.0;  // g
	MaterialDiffuse[2] = 0.0;  // b
	MaterialDiffuse[3] = 1.0f; // a
	
	// specular material
	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.6;  // g
	MaterialSpecular[2] = 0.6;  // b
	MaterialSpecular[3] = 1.0f; // a
	
	// shininess
	MaterialShininess = 0.25 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(2 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 5th sphere on 3rd column, white *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	
	// diffuse material
	MaterialDiffuse[0] = 0.55; // r
	MaterialDiffuse[1] = 0.55; // g
	MaterialDiffuse[2] = 0.55; // b
	MaterialDiffuse[3] = 1.0f; // a
	
	// specular material
	MaterialSpecular[0] = 0.70; // r
	MaterialSpecular[1] = 0.70; // g
	MaterialSpecular[2] = 0.70; // b
	MaterialSpecular[3] = 1.0f; // a
	
	// shininess
	MaterialShininess = 0.25 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(2 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 6th sphere on 3rd column, yellow plastic *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	
	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.0;  // b
	MaterialDiffuse[3] = 1.0f; // a
	
	// specular material
	MaterialSpecular[0] = 0.60; // r
	MaterialSpecular[1] = 0.60; // g
	MaterialSpecular[2] = 0.50; // b
	MaterialSpecular[3] = 1.0f; // a
	
	// shininess
	MaterialShininess = 0.25 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(2 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************
	// *******************************************************
	// *******************************************************

	// ***** 1st sphere on 4th column, black *****
	// ambient material
	MaterialAmbient[0] = 0.02; // r
	MaterialAmbient[1] = 0.02; // g
	MaterialAmbient[2] = 0.02; // b
	MaterialAmbient[3] = 1.0f; // a
	
	// diffuse material
	MaterialDiffuse[0] = 0.01; // r
	MaterialDiffuse[1] = 0.01; // g
	MaterialDiffuse[2] = 0.01; // b
	MaterialDiffuse[3] = 1.0f; // a
	
	// specular material
	MaterialSpecular[0] = 0.4;  // r
	MaterialSpecular[1] = 0.4;  // g
	MaterialSpecular[2] = 0.4;  // b
	MaterialSpecular[3] = 1.0f; // a
	
	// shininess
	MaterialShininess = 0.078125 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(3 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 2nd sphere on 4th column, cyan *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.05; // b
	MaterialAmbient[3] = 1.0f; // a
	
	// diffuse material
	MaterialDiffuse[0] = 0.4;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.5;  // b
	MaterialDiffuse[3] = 1.0f; // a
	
	// specular material
	MaterialSpecular[0] = 0.04; // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.7;  // b
	MaterialSpecular[3] = 1.0f; // a
	
	// shininess
	MaterialShininess = 0.078125 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(3 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 3rd sphere on 4th column, green *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	
	// diffuse material
	MaterialDiffuse[0] = 0.4;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.4;  // b
	MaterialDiffuse[3] = 1.0f; // a
	
	// specular material
	MaterialSpecular[0] = 0.04; // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.04; // b
	MaterialSpecular[3] = 1.0f; // a
	
	// shininess
	MaterialShininess = 0.078125 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(3 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 4th sphere on 4th column, red *****
	// ambient material
	MaterialAmbient[0] = 0.05; // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.4;  // g
	MaterialDiffuse[2] = 0.4;  // b
	MaterialDiffuse[3] = 1.0f; // a
	
	// specular material
	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.04; // g
	MaterialSpecular[2] = 0.04; // b
	MaterialSpecular[3] = 1.0f; // a
	
	// shininess
	MaterialShininess = 0.078125 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(3 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 5th sphere on 4th column, white *****
	// ambient material
	MaterialAmbient[0] = 0.05; // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.05; // b
	MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.5;  // b
	MaterialDiffuse[3] = 1.0f; // a
	
	// specular material
	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.7;  // b
	MaterialSpecular[3] = 1.0f; // a
	
	// shininess
	MaterialShininess = 0.078125 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(3 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************

	// ***** 6th sphere on 4th column, yellow rubber *****
	// ambient material
	MaterialAmbient[0] = 0.05; // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	
	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.4;  // b
	MaterialDiffuse[3] = 1.0f; // a
	
	// specular material
	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.04; // b
	MaterialSpecular[3] = 1.0f; // a
	
	// shininess
	MaterialShininess = 0.078125 * 128;
	
	if (bLight == TRUE)
	{
		glUniform3fv(laUniform, 1, LightAmbient);
		glUniform3fv(ldUniform, 1, LightDiffuse);
		glUniform3fv(lsUniform, 1, LightSpecular);
		glUniform4fv(lightPositionUniform, 1, LightPosition);

		glUniform3fv(kaUniform, 1, MaterialAmbient);
		glUniform3fv(kdUniform, 1, MaterialDiffuse);
		glUniform3fv(ksUniform, 1, MaterialSpecular);
		glUniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glViewport(3 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
	glBindVertexArray(vao_Sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	// *******************************************************
	// *******************************************************
	// *******************************************************
}

void uninitialize(void)
{
	// Function Declarations
	void ToggleFullscreen(void);

	// Code
	if (gbFullscreen)
	{
		ToggleFullscreen();
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

	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log File Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
