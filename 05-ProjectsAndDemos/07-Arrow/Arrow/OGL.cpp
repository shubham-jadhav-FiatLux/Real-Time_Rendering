// Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>

// OpenGL Header Files
#include <GL/glew.h>		// This Must Be Before Including 'gl.h'
#include <GL/gl.h>
#include "vmath.h"
#include"OGLCylinder.h"

using namespace vmath;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#define GL_GENERATE_MIPMAP                0x8191

// OpenGL Libraries
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "OGLCylinder.lib")

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable Declarations
FILE* gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
static BOOL gbFullscreen = FALSE;

// Programmable Pipeline Related Global Variables
GLuint shaderProgramObject;

enum
{
	SBJ_ATTRIBUTE_POSITION = 0,
	SBJ_ATTRIBUTE_COLOR,
	SBJ_ATTRIBUTE_NORMAL,
	SBJ_ATTRIBUTE_TEXTURE0
};

GLuint vao_pyramid_arrow;
GLuint vbo_pyramid_position_arrow;
GLuint vbo_pyramid_texcoord_arrow;
GLuint vbo_pyramid_normal_arrow;

GLuint vao_pyramid_arrow_tail;
GLuint vbo_pyramid_position_arrow_tail;
GLuint vbo_pyramid_texcoord_arrow_tail;
GLuint vbo_pyramid_normal_arrow_tail;

GLuint vao_cylinder_arrow;
GLuint vbo_cylinder_position_arrow;
GLuint vbo_cylinder_normal_arrow;
GLuint vbo_cylinder_texcoord_arrow;				// Vertex Buffer Object for pyramid colors
GLuint vbo_cylinder_element_arrow;

float* cylinder_vertices = NULL;
float* cylinder_normals = NULL;
float* cylinder_textures = NULL;
unsigned short* cylinder_elements = NULL;

GLuint gNumVertices;
GLuint gNumElements;

const float cylinderHeight = 2.0f;
const float cylinderBaseRadius = 1.0f;
const float cylinderTopRadius = 1.0f;

const unsigned int cylinderSlices = 20;
const unsigned int cylinderStacks = 20;

GLuint vao_cube;
GLuint vbo_cube_position;
GLuint vbo_cube_normal;
GLuint vbo_cube_texcoord;

GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionMatrixUniform;

BOOL bLight = FALSE;
GLfloat lightAngleZero = 0.0f;
GLfloat xAxis = 0.0f;
GLuint laUniform;				// Ambient
GLuint ldUniform;				// Diffuse
GLuint lsUniform;				// Specular
GLuint lightPositionUniform;
GLuint lightingEnabledUniform;

GLfloat lightAmbientOne[] = { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat lightDiffuseOne[] = { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat lightSpecularOne[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPositionOne[] = { 10.0f, 0.0f, 5.0f, 1.0f };

GLfloat lightAmbientTwo[] = { 0.450f, 0.317f, 0.235f, 1.0f };
GLfloat lightDiffuseTwo[] = { 0.450f, 0.317f, 0.235f, 1.0f };
GLfloat lightSpecularTwo[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPositionTwo[] = { 0.0f, 0.0f, 5.0f, 1.0f };

GLfloat lightAmbientThree[] = { 0.450f, 0.317f, 0.235f, 1.0f };
GLfloat lightDiffuseThree[] = { 0.450f, 0.317f, 0.235f, 1.0f };
GLfloat lightSpecularThree[] = { 0.450f, 0.317f, 0.235f, 1.0f };
GLfloat lightPositionThree[] = { -10.0f, 0.0f, 5.0f, 1.0f };

// Cubemap
TCHAR* facesCubemap[] =
{
	MAKEINTRESOURCE(LEFT1), MAKEINTRESOURCE(RIGHT1), MAKEINTRESOURCE(TOP1), MAKEINTRESOURCE(BOTTOM1), MAKEINTRESOURCE(FRONT1),MAKEINTRESOURCE(BACK1)
};
GLuint cubemapTexture;

GLuint texture_arrow_tip;
GLuint texture_arrow_tail;
GLuint texture_arrow_tail_rev;
GLuint texture_arrow_stick;
GLuint textureSamplerUniform;

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

		default:
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
	BOOL LoadGLTexture(GLuint*, TCHAR[]);
	bool LoadGLCubemap(void);

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
		"in vec2 a_texcoord;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform vec4 u_lightPosition;" \
		"uniform int u_lightingEnabled;" \
		"out vec3 transformedNormals;" \
		"out vec3 lightDirection;" \
		"out vec3 viewerVector;" \
		"out vec2 a_texcoord_out;" \
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
		"in vec3 transformedNormals;" \
		"in vec3 lightDirection;" \
		"in vec3 viewerVector;" \
		"uniform sampler2D u_textureSampler;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform int u_lightingEnabled;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec3 phong_ads_light;" \
		"if (u_lightingEnabled == 1)" \
		"{" \
		"vec3 ambient = u_la;" \
		"vec3 normalized_tranformed_normals = normalize(transformedNormals);" \
		"vec3 normalized_lightDirection = normalize(lightDirection);" \
		"vec3 diffuse = u_ld * max(dot(normalized_lightDirection, normalized_tranformed_normals), 0.0);" \
		"vec3 reflectionVector = reflect(-normalized_lightDirection, normalized_tranformed_normals);" \
		"vec3 normalized_viewerVector = normalize(viewerVector);" \
		"vec3 specular = u_ls * pow(max(dot(reflectionVector, normalized_viewerVector), 0.0), 0.0);" \
		"phong_ads_light = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = vec3(1.0, 1.0, 1.0);" \
		"}" \
		"FragColor = vec4(phong_ads_light, 1.0) * texture(u_textureSampler, a_texcoord_out);" \
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
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_textureSampler");
	laUniform = glGetUniformLocation(shaderProgramObject, "u_la");
	ldUniform = glGetUniformLocation(shaderProgramObject, "u_ld");
	lsUniform = glGetUniformLocation(shaderProgramObject, "u_ls");
	lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_lightPosition");
	lightingEnabledUniform = glGetUniformLocation(shaderProgramObject, "u_lightingEnabled");

	// Declaration Of Vertex Data Arrays
	const GLfloat pyramidPosition_arrow[] =	{
											// front
											0.0f, 1.0f, 0.0f,
											-1.0f, -1.0f, 1.0f,
											1.0f, -1.0f, 1.0f,

											// right
											0.0f, 1.0f, 0.0f,
											1.0f, -1.0f, 1.0f,
											1.0f, -1.0f, -1.0f,

											// back
											0.0f, 1.0f, 0.0f,
											1.0f, -1.0f, -1.0f,
											-1.0f, -1.0f, -1.0f,

											// left
											0.0f, 1.0f, 0.0f,
											-1.0f, -1.0f, -1.0f,
											-1.0f, -1.0f, 1.0f

										};

	const GLfloat pyramidPosition_arrow_tail[] = {
										// front
										-1.0f, 1.0f, 0.0f,
										-1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, 1.0f,

										// right
										-1.0f, 1.0f, 0.0f,
										1.0f, -1.0f, 1.0f,
										1.0f, -1.0f, -1.0f,

										// back
										-1.0f, 1.0f, 0.0f,
										1.0f, -1.0f, -1.0f,
										-1.0f, -1.0f, -1.0f,

										// left
										-1.0f, 1.0f, 0.0f,
										-1.0f, -1.0f, -1.0f,
										-1.0f, -1.0f, 1.0f

									};


	const GLfloat pyramidNormals_arrow[] = {
											0.0f, 0.447214f, 0.894427f,// front-top
											0.0f, 0.447214f, 0.894427f,// front-left
											0.0f, 0.447214f, 0.894427f,// front-right

											0.894427f, 0.447214f, 0.0f, // right-top
											0.894427f, 0.447214f, 0.0f, // right-left
											0.894427f, 0.447214f, 0.0f, // right-right

											0.0f, 0.447214f, -0.894427f, // back-top
											0.0f, 0.447214f, -0.894427f, // back-left
											0.0f, 0.447214f, -0.894427f, // back-right

											-0.894427f, 0.447214f, 0.0f, // left-top
											-0.894427f, 0.447214f, 0.0f, // left-left
											-0.894427f, 0.447214f, 0.0f // left-right

	};

	const GLfloat pyramidTexcoord_arrow[] = {
										
											0.5, 1.0, // front-top
											0.0, 0.0, // front-left
											1.0, 0.0, // front-right

											0.5, 1.0, // right-top
											1.0, 0.0, // right-left
											0.0, 0.0, // right-right

											0.5, 1.0, // back-top
											1.0, 0.0, // back-left
											0.0, 0.0, // back-right

											0.5, 1.0, // left-top
											0.0, 0.0, // left-left
											1.0, 0.0, // left-right
											

	};


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



	const GLfloat cubeNormals[] = {
		// top surface
		0.0f, 1.0f, 0.0f,  // top-right of top
		0.0f, 1.0f, 0.0f,  // top-left of top
		0.0f, 1.0f, 0.0f,  // bottom-left of top
		0.0f, 1.0f, 0.0f,  // bottom-right of top

		// bottom surface
		0.0f, -1.0f, 0.0f,  // top-right of bottom
		0.0f, -1.0f, 0.0f,  // top-left of bottom
		0.0f, -1.0f, 0.0f,  // bottom-left of bottom
		0.0f, -1.0f, 0.0f,  // bottom-right of bottom

		// front surface
		0.0f, 0.0f, 1.0f,  // top-right of front
		0.0f, 0.0f, 1.0f,  // top-left of front
		0.0f, 0.0f, 1.0f,  // bottom-left of front
		0.0f, 0.0f, 1.0f,  // bottom-right of front

		// back surface
		0.0f, 0.0f, -1.0f,  // top-right of back
		0.0f, 0.0f, -1.0f,  // top-left of back
		0.0f, 0.0f, -1.0f,  // bottom-left of back
		0.0f, 0.0f, -1.0f,  // bottom-right of back

		// left surface
		-1.0f, 0.0f, 0.0f,  // top-right of left
		-1.0f, 0.0f, 0.0f,  // top-left of left
		-1.0f, 0.0f, 0.0f,  // bottom-left of left
		-1.0f, 0.0f, 0.0f,  // bottom-right of left

		// right surface
		1.0f, 0.0f, 0.0f,  // top-right of right
		1.0f, 0.0f, 0.0f,  // top-left of right
		1.0f, 0.0f, 0.0f,  // bottom-left of right
		1.0f, 0.0f, 0.0f   // bottom-right of right
	};

	const GLfloat cubeTexcoord[] = {
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



	size_t sizeOfCylinderVertices;
	size_t sizeOfCylinderNormals;
	size_t sizeOfCylinderTexCoords;
	size_t sizeOfCylinderElements;

	gNumVertices = getCylinderVerticesCount(cylinderSlices, cylinderStacks);
	gNumElements = getCylinderElementsCount(cylinderSlices, cylinderStacks);

	sizeOfCylinderVertices = gNumVertices * 3 * sizeof(float);
	sizeOfCylinderNormals = gNumVertices * 3 * sizeof(float);
	sizeOfCylinderTexCoords = gNumVertices * 2 * sizeof(float);
	sizeOfCylinderElements = gNumElements * sizeof(unsigned short);	// Don't need to multiply by 3 here as getCylinderElementsCount() will give exact count of elements.

	cylinder_vertices = (float*)malloc(sizeof(float) * gNumVertices * 3);	// Because each vertex position will have three co-ordinates, x, y and z.
	cylinder_normals = (float*)malloc(sizeof(float) * gNumVertices * 3);	// Because each vertex normal will have three co-ordinates, x, y and z.
	cylinder_textures = (float*)malloc(sizeof(float) * gNumVertices * 2);	// Because each vertex texture will have two co-ordinates, x and y.
	cylinder_elements = (unsigned short*)malloc(sizeof(unsigned short) * gNumElements * 3);	// Because each element will be triangle and each triangle has three vertices.

	getCylinderVerticesData(cylinder_vertices, cylinder_normals, cylinder_textures, cylinder_elements, cylinderBaseRadius, cylinderTopRadius, cylinderSlices, cylinderStacks);

	// gVao_cylinder
	glGenVertexArrays(1, &vao_cylinder_arrow);
	glBindVertexArray(vao_cylinder_arrow);

	// position vbo
	glGenBuffers(1, &vbo_cylinder_position_arrow);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cylinder_position_arrow);
	glBufferData(GL_ARRAY_BUFFER, sizeOfCylinderVertices, cylinder_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &vbo_cylinder_normal_arrow);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cylinder_normal_arrow);
	glBufferData(GL_ARRAY_BUFFER, sizeOfCylinderNormals, cylinder_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(SBJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SBJ_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// tex co-ord vbo
	glGenBuffers(1, &vbo_cylinder_texcoord_arrow);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cylinder_texcoord_arrow);
	glBufferData(GL_ARRAY_BUFFER, sizeOfCylinderTexCoords, cylinder_textures, GL_STATIC_DRAW);
	glVertexAttribPointer(SBJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SBJ_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &vbo_cylinder_element_arrow);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_cylinder_element_arrow);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeOfCylinderElements, cylinder_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind gVao_cylinder
	glBindVertexArray(0);

	// vao And vbo Related Code
	// Pyramid
	glGenVertexArrays(1, &vao_pyramid_arrow);
	glBindVertexArray(vao_pyramid_arrow);

		// vbo For Position
		glGenBuffers(1, &vbo_pyramid_position_arrow);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_position_arrow);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPosition_arrow), pyramidPosition_arrow, GL_STATIC_DRAW);
		glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vbo For Normal
		glGenBuffers(1, &vbo_pyramid_normal_arrow);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_normal_arrow);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals_arrow), pyramidNormals_arrow, GL_STATIC_DRAW);
		glVertexAttribPointer(SBJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SBJ_ATTRIBUTE_NORMAL);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vbo For Texcoord
		glGenBuffers(1, &vbo_pyramid_texcoord_arrow);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_texcoord_arrow);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexcoord_arrow), pyramidTexcoord_arrow, GL_STATIC_DRAW);
		glVertexAttribPointer(SBJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SBJ_ATTRIBUTE_TEXTURE0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	// Pyramid_Tail
	glGenVertexArrays(1, &vao_pyramid_arrow_tail);
	glBindVertexArray(vao_pyramid_arrow_tail);

	// vbo For Position
	glGenBuffers(1, &vbo_pyramid_position_arrow_tail);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_position_arrow_tail);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPosition_arrow_tail), pyramidPosition_arrow_tail, GL_STATIC_DRAW);
	glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vbo For Normal
	glGenBuffers(1, &vbo_pyramid_normal_arrow_tail);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_normal_arrow_tail);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals_arrow), pyramidNormals_arrow, GL_STATIC_DRAW);
	glVertexAttribPointer(SBJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SBJ_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vbo For Texcoord
	glGenBuffers(1, &vbo_pyramid_texcoord_arrow_tail);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_texcoord_arrow_tail);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexcoord_arrow), pyramidTexcoord_arrow, GL_STATIC_DRAW);
	glVertexAttribPointer(SBJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SBJ_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


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


		// vbo For Normal
		glGenBuffers(1, &vbo_cube_normal);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_normal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
		glVertexAttribPointer(SBJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SBJ_ATTRIBUTE_NORMAL);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vbo For Texcoord
		glGenBuffers(1, &vbo_cube_texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoord);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoord), cubeTexcoord, GL_STATIC_DRAW);
		glVertexAttribPointer(SBJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SBJ_ATTRIBUTE_TEXTURE0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	// Cubemap
	glGenVertexArrays(1, &vao_cubemap);
	glBindVertexArray(vao_cubemap);

	// vbo For Position
	glGenBuffers(1, &vbo_cubemap_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cubemap_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapPosition), cubemapPosition, GL_STATIC_DRAW);
	glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*
	// vbo For Normal
	glGenBuffers(1, &vbo_cubemap_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(SBJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SBJ_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	*/

	/*
	// vbo For Texcoord
	glGenBuffers(1, &vbo_cubemap_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cubemap_texcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapTexcoord), cubemapTexcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(SBJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SBJ_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	*/

	glBindVertexArray(0);

	// Here Starts OpenGL Code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// glShadeModel(),glHint() Deprecated In PP 

	// Clear The Screen Using Blue Color
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);


	if (LoadGLTexture(&texture_arrow_tip, MAKEINTRESOURCE(IDBITMAP_ARROW_TIP)) == FALSE)
		return -6;

	if (LoadGLTexture(&texture_arrow_tail, MAKEINTRESOURCE(IDBITMAP_ARROW_TAIL)) == FALSE)
		return -7;

	if (LoadGLTexture(&texture_arrow_stick, MAKEINTRESOURCE(IDBITMAP_ARROW_STICK)) == FALSE)
		return -8;

	if (LoadGLTexture(&texture_arrow_tail_rev, MAKEINTRESOURCE(IDBITMAP_ARROW_TAIL_REV)) == FALSE)
		return -9;

	// Enabling The Texture
	glEnable(GL_TEXTURE_2D);

	bool result = LoadGLCubemap();
	if (result == false)
	{
		fprintf(gpFile, "failed in LoadGLCubemap() function\n");
		DestroyWindow(ghwnd);
	}
	glEnable(GL_TEXTURE_CUBE_MAP);

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
	//functions declarations
	void vglPushMatrix(vmath::mat4 inMatrix);
	vmath::mat4 vglPopMatrix(void);
	void cube(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use The Shader Program Object
	glUseProgram(shaderProgramObject);

	mat4 translationMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -35.0f);
	rotationMatrix = vmath::rotate(lightAngleZero, 1.0f, 0.0f, 0.0f);
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;							// Here Order Is Very Important
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	vglPushMatrix(modelMatrix);

	// Pyramid
	// Transformations
	translationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	//modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	

	translationMatrix = vmath::translate(6.5f, 0.0f, -35.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 0.5f);
	rotationMatrix = vmath::rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;							// Here Order Is Very Important
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);


	// Sending Light Related Uniforms
	if (bLight == TRUE)
	{
		glUniform1i(lightingEnabledUniform, 1);

		glUniform3fv(laUniform, 1, lightAmbientOne);
		glUniform3fv(ldUniform, 1, lightDiffuseOne);
		glUniform3fv(lsUniform, 1, lightSpecularOne);
		glUniform4fv(lightPositionUniform, 1, lightPositionOne);

	}
	else
	{
		glUniform1i(lightingEnabledUniform, 0);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_arrow_tip);
	glUniform1i(textureSamplerUniform, 0);

	glBindVertexArray(vao_pyramid_arrow);
	
	glDrawArrays(GL_TRIANGLES, 0, 12);

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Cube Head
	// Transformations
	translationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	translationMatrix = vmath::translate(4.5f, 0.0f, -35.0f);
	scaleMatrix = vmath::scale(0.5f, 0.7f, 0.9f);
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;							// Here Order Is Very Important
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_arrow_tip);
	glUniform1i(textureSamplerUniform, 0);

	glBindVertexArray(vao_cube);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);

	//glBindTexture(GL_TEXTURE_2D, 0);

	// Cube Tail
	// Transformations
	translationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	translationMatrix = vmath::translate(-15.0f, 0.0f, -35.0f);
	scaleMatrix = vmath::scale(1.0f, 1.5f, 0.5f);
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;							// Here Order Is Very Important
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	// Sending Light Related Uniforms
	if (bLight == TRUE)
	{
		glUniform1i(lightingEnabledUniform, 1);

		glUniform3fv(laUniform, 1, lightAmbientTwo);
		glUniform3fv(ldUniform, 1, lightDiffuseTwo);
		glUniform3fv(lsUniform, 1, lightSpecularTwo);
		glUniform4fv(lightPositionUniform, 1, lightPositionTwo);

	}
	else
	{
		glUniform1i(lightingEnabledUniform, 0);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_arrow_tail);
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

	// Pyramid
	// Transformations
	translationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();


	translationMatrix = vmath::translate(-12.5f, 0.0f, -35.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 0.5f);
	rotationMatrix = vmath::rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;							// Here Order Is Very Important
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_arrow_tail_rev);
	glUniform1i(textureSamplerUniform, 0);

	glBindVertexArray(vao_pyramid_arrow);

	glDrawArrays(GL_TRIANGLES, 0, 12);

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Tail Pyramids
	// Transformations
	translationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();


	translationMatrix = vmath::translate(-17.5f, 0.0f, -35.0f);
	scaleMatrix = vmath::scale(1.5f, 1.5f, 0.5f);
	rotationMatrix = vmath::rotate(90.0f, 0.0f, 0.0f, 1.0f);
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;							// Here Order Is Very Important
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_arrow_tail_rev);
	glUniform1i(textureSamplerUniform, 0);

	glBindVertexArray(vao_pyramid_arrow_tail);

	glDrawArrays(GL_TRIANGLES, 0, 12);

	glBindVertexArray(0);



	rotationMatrix = vmath::rotate(180.0f, 0.0f, 0.0f, 1.0f);
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glBindVertexArray(vao_pyramid_arrow_tail);

	glDrawArrays(GL_TRIANGLES, 0, 12);

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Cylinder
	// Transformations
	translationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	translationMatrix = vmath::translate(4.5f, 0.0f, -35.0f);
	scaleMatrix = vmath::scale(18.5f, 0.5f, 0.5f);
	rotationMatrix = vmath::rotate(-90.0f, 0.0f, 1.0f, 0.0f);
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;							// Here Order Is Very Important
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	// Sending Light Related Uniforms
	if (bLight == TRUE)
	{
		glUniform1i(lightingEnabledUniform, 1);

		glUniform3fv(laUniform, 1, lightAmbientTwo);
		glUniform3fv(ldUniform, 1, lightDiffuseTwo);
		glUniform3fv(lsUniform, 1, lightSpecularTwo);
		glUniform4fv(lightPositionUniform, 1, lightPositionTwo);

	}
	else
	{
		glUniform1i(lightingEnabledUniform, 0);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_arrow_stick);
	glUniform1i(textureSamplerUniform, 0);

	glBindVertexArray(vao_cylinder_arrow);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_cylinder_element_arrow);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// *** unbind vao ***
	glBindVertexArray(0);




	//cubemap
	// Transformations
	translationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	translationMatrix = vmath::translate(-15.0f, 0.0f, -35.0f);
	scaleMatrix = vmath::scale(1.0f, 1.5f, 0.5f);
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;							// Here Order Is Very Important
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	// Sending Light Related Uniforms
	if (bLight == TRUE)
	{
		glUniform1i(lightingEnabledUniform, 1);

		glUniform3fv(laUniform, 1, lightAmbientTwo);
		glUniform3fv(ldUniform, 1, lightDiffuseTwo);
		glUniform3fv(lsUniform, 1, lightSpecularTwo);
		glUniform4fv(lightPositionUniform, 1, lightPositionTwo);

	}
	else
	{
		glUniform1i(lightingEnabledUniform, 0);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_arrow_tail);
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

	cube();

	modelMatrix = vglPopMatrix();

	// Unuse The Shader Program Object
	glUseProgram(0);
	
	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	lightAngleZero += 5.0f;

	//lightPosition[1] = 10 * sin(lightAngleZero / 180.0f * 3.14f);
	//lightPosition[2] = 10 * cos(lightAngleZero / 180.0f * 3.14f);
}


void cube(void)
{
	// Local Variable Declarations
	GLfloat xCoordinate = 100.0f;
	GLfloat yCoordinate = 100.0f;
	GLfloat zCoordinate = 100.0f;

	// Code

	// Front
	glBegin(GL_QUADS);
	//glColor3f(1.0f, 0.0f, 0.0f);
	glTexCoord3f(xCoordinate, yCoordinate, zCoordinate);
	glVertex3f(xCoordinate, yCoordinate, zCoordinate);
	glTexCoord3f(-xCoordinate, yCoordinate, zCoordinate);
	glVertex3f(-xCoordinate, yCoordinate, zCoordinate);
	glTexCoord3f(-xCoordinate, -yCoordinate, zCoordinate);
	glVertex3f(-xCoordinate, -yCoordinate, zCoordinate);
	glTexCoord3f(xCoordinate, -yCoordinate, zCoordinate);
	glVertex3f(xCoordinate, -yCoordinate, zCoordinate);
	glEnd();

	// Right
	glBegin(GL_QUADS);
	//glColor3f(0.0f, 1.0f, 0.0f);
	glTexCoord3f(xCoordinate, yCoordinate, -zCoordinate);
	glVertex3f(xCoordinate, yCoordinate, -zCoordinate);
	glTexCoord3f(xCoordinate, yCoordinate, zCoordinate);
	glVertex3f(xCoordinate, yCoordinate, zCoordinate);
	glTexCoord3f(xCoordinate, -yCoordinate, zCoordinate);
	glVertex3f(xCoordinate, -yCoordinate, zCoordinate);
	glTexCoord3f(xCoordinate, -yCoordinate, -zCoordinate);
	glVertex3f(xCoordinate, -yCoordinate, -zCoordinate);
	glEnd();

	// Back
	glBegin(GL_QUADS);
	//glColor3f(0.0f, 0.0f, 1.0f);
	glTexCoord3f(-xCoordinate, yCoordinate, -zCoordinate);
	glVertex3f(-xCoordinate, yCoordinate, -zCoordinate);
	glTexCoord3f(xCoordinate, yCoordinate, -zCoordinate);
	glVertex3f(xCoordinate, yCoordinate, -zCoordinate);
	glTexCoord3f(xCoordinate, -yCoordinate, -zCoordinate);
	glVertex3f(xCoordinate, -yCoordinate, -zCoordinate);
	glTexCoord3f(-xCoordinate, -yCoordinate, -zCoordinate);
	glVertex3f(-xCoordinate, -yCoordinate, -zCoordinate);
	glEnd();

	// Left
	glBegin(GL_QUADS);
	//glColor3f(1.0f, 1.0f, 0.0f);
	glTexCoord3f(-xCoordinate, yCoordinate, zCoordinate);
	glVertex3f(-xCoordinate, yCoordinate, zCoordinate);
	glTexCoord3f(-xCoordinate, yCoordinate, -zCoordinate);
	glVertex3f(-xCoordinate, yCoordinate, -zCoordinate);
	glTexCoord3f(-xCoordinate, -yCoordinate, -zCoordinate);
	glVertex3f(-xCoordinate, -yCoordinate, -zCoordinate);
	glTexCoord3f(-xCoordinate, -yCoordinate, zCoordinate);
	glVertex3f(-xCoordinate, -yCoordinate, zCoordinate);
	glEnd();

	// Top
	glBegin(GL_QUADS);
	//glColor3f(1.0f, 0.0f,1.0f);
	glTexCoord3f(xCoordinate, yCoordinate, -zCoordinate);
	glVertex3f(xCoordinate, yCoordinate, -zCoordinate);
	glTexCoord3f(-xCoordinate, yCoordinate, -zCoordinate);
	glVertex3f(-xCoordinate, yCoordinate, -zCoordinate);
	glTexCoord3f(-xCoordinate, yCoordinate, zCoordinate);
	glVertex3f(-xCoordinate, yCoordinate, zCoordinate);
	glTexCoord3f(xCoordinate, yCoordinate, zCoordinate);
	glVertex3f(xCoordinate, yCoordinate, zCoordinate);
	glEnd();

	// Bottom
	glBegin(GL_QUADS);
	//glColor3f(0.0f, 1.0f, 1.0f);
	glTexCoord3f(xCoordinate, -yCoordinate, -zCoordinate);
	glVertex3f(xCoordinate, -yCoordinate, -zCoordinate);
	glTexCoord3f(-xCoordinate, -yCoordinate, -zCoordinate);
	glVertex3f(-xCoordinate, -yCoordinate, -zCoordinate);
	glTexCoord3f(-xCoordinate, -yCoordinate, zCoordinate);
	glVertex3f(-xCoordinate, -yCoordinate, zCoordinate);
	glTexCoord3f(xCoordinate, -yCoordinate, zCoordinate);
	glVertex3f(xCoordinate, -yCoordinate, zCoordinate);
	glEnd();

}

bool LoadGLCubemap()
{
	// variable declarations
	bool bResult = false;

	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for (int i = 0; i < 6; i++)
	{
		HBITMAP hBitmap = NULL;
		BITMAP bmp;

		// code
		//hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), testing[i], IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), facesCubemap[i], IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		if (hBitmap)
		{
			bResult = true;
			// os function to get image information
			GetObject(hBitmap, sizeof(BITMAP), &bmp);

			fprintf(gpFile, "bitmap  %d  is loaded\n", i);
			gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				GL_RGB,
				bmp.bmWidth,
				bmp.bmHeight,
				GL_BGR_EXT,
				GL_UNSIGNED_BYTE,
				bmp.bmBits);

			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		}
		else
		{
			fprintf(gpFile, "bitmap  %d  is not loaded\n", i);
			DestroyWindow(ghwnd);
		}
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	fprintf(gpFile, "End of %s \n", __FUNCTION__);
	return bResult;
}

BOOL LoadGLTexture(GLuint* Texture, TCHAR ImageResourceID[])
{
	// Local Variable Declarations
	HBITMAP hbitmap = NULL;
	BITMAP bmp;
	BOOL bResult = FALSE;

	// Code
	hbitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		ImageResourceID,
		IMAGE_BITMAP,
		0,
		0,
		LR_CREATEDIBSECTION);

	if (hbitmap)
	{
		bResult = TRUE;

		// Get Image Data
		GetObject(hbitmap, sizeof(BITMAP), &bmp);

		// Decide Pixel Packing Or Unpacking Of Image
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);			// 1 Instead Of 4 For Better Performance 

		// Generate Texture Object
		glGenTextures(1, Texture);

		// Bind That Texture Object
		glBindTexture(GL_TEXTURE_2D, *Texture);

		// Set Texture Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		// Create The Texture
		glTexImage2D(GL_TEXTURE_2D,
			0,							// Mipmap Level
			GL_RGB,
			bmp.bmWidth,
			bmp.bmHeight,
			0,							// Border Width
			GL_BGR,
			GL_UNSIGNED_BYTE,
			bmp.bmBits);

		glGenerateMipmap(GL_TEXTURE_2D);

		// Unbind Texture Object
		glBindTexture(GL_TEXTURE_2D, 0);

		// Delete Object
		DeleteObject(hbitmap);
	}

	return bResult;
}


/************************ PUSH-POP Related Code**************************************/

typedef struct __NODE
{
	//int id;
	vmath::mat4 matData;
	struct __NODE* next;
}Node;

//variables
Node* pushPopStackHead = NULL;
int top = -1;


/********************************************************************************/
/*																				*/
/*							Stack Functions (Push Pop)							*/
/*																				*/
/********************************************************************************/
//glPushMatrix()
void vglPushMatrix(vmath::mat4 inMatrix)
{
	//variables
	Node* newNode = NULL;

	//code
	newNode = (Node*)malloc(sizeof(Node));
	if (newNode == NULL)
	{
		//fprintf(gpFile, "MALLOC FAILED!!!\n");
		return;
	}
	newNode->matData = inMatrix;
	newNode->next = NULL;

	if (top == -1 || pushPopStackHead == NULL)
	{
		pushPopStackHead = newNode;
		top += 1;
		//fprintf(gpFile, "++(PushMat) [top] : %d\n", top);
	}
	else
	{
		newNode->next = pushPopStackHead;
		pushPopStackHead = newNode;
		top += 1;
		//fprintf(gpFile, "++(PushMat) [top] : %d\n", top);
	}
}

//glPopMatrix()
vmath::mat4 vglPopMatrix(void)
{
	//variables
	vmath::mat4 item;
	Node* temp = NULL;

	//code
	if (pushPopStackHead == NULL)
	{
		//fprintf(gpFile, "[ERROR] Stack Undeflow!!!\n");
	}
	else
	{
		item = pushPopStackHead->matData;
		temp = pushPopStackHead;
		pushPopStackHead = pushPopStackHead->next;
		free(temp);
		top -= 1;
		//fprintf(gpFile, "--(PopMat) [top] : %d\n", top);
	}

	return item;
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

	if (texture_arrow_tail)
	{
		glDeleteTextures(1, &texture_arrow_tail);
		texture_arrow_tail = 0;
	}

	if (texture_arrow_tip)
	{
		glDeleteTextures(1, &texture_arrow_tip);
		texture_arrow_tip = 0;
	}

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

	if (vbo_pyramid_texcoord_arrow)
	{
		glDeleteBuffers(1, &vbo_pyramid_texcoord_arrow);
		vbo_pyramid_texcoord_arrow = 0;
	}


	if (vbo_pyramid_position_arrow)
	{
		glDeleteBuffers(1, &vbo_pyramid_position_arrow);
		vbo_pyramid_position_arrow = 0;
	}

	if (vbo_cylinder_normal_arrow)
	{
		glDeleteBuffers(1, &vbo_cylinder_normal_arrow);
		vbo_cylinder_normal_arrow = 0;
	}

	// Deletion And Uninitialization of vao
	if (vao_cube)
	{
		glDeleteVertexArrays(1, &vao_cube);
		vao_cube = 0;
	}

	if (vao_pyramid_arrow)
	{
		glDeleteVertexArrays(1, &vao_pyramid_arrow);
		vao_pyramid_arrow = 0;
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
