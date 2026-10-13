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
GLchar choosenShader = 'v';

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

GLuint laUniform;				// Ambient
GLuint ldUniform;				// Diffuse
GLuint lsUniform;				// Specular
GLuint lightPositionUniform;

GLuint kaUniform;
GLuint kdUniform;
GLuint ksUniform;
GLuint materialShininessUniform;

GLuint lightingEnabledUniform;

/*
GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 50.0f;
*/

GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 0.5f, 0.2f, 0.7f, 1.0f };
GLfloat materialSpecular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat materialShininess = 128.0f;

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
		case 'Q':
		case 'q':
			DestroyWindow(hwnd);
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

	case WM_KEYDOWN:
		switch (wParam)
		{
		case 27:
			ToggleFullscreen();
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

	// Vertex Shader PerVertex
	const GLchar* vertexShaderSourceCode_pv =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec3 a_normal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform vec4 u_lightPosition;" \
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
		"vec3 ambient = u_la * u_ka;" \
		"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" \
		"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
		"vec3 transformedNormals = normalize(normalMatrix * a_normal);" \
		"vec3 lightDirection = normalize(vec3(u_lightPosition) - eyeCoordinates.xyz);" \
		"vec3 diffuse = u_ld * u_kd * max(dot(lightDirection, transformedNormals), 0.0);" \
		"vec3 reflectionVector = reflect(-lightDirection, transformedNormals);" \
		"vec3 viewerVector = normalize(-eyeCoordinates.xyz);" \
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector, viewerVector), 0.0), u_materialShininess);" \
		"phong_ads_light = ambient + diffuse + specular;" \
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
	/*if (choosenShader == 'v')
	{
		shaderProgramObject_pv = glCreateProgram();
		glAttachShader(shaderProgramObject_pv, vertexShaderObject);
		glAttachShader(shaderProgramObject_pv, fragmentShaderObject);
		glBindAttribLocation(shaderProgramObject_pv, SBJ_ATTRIBUTE_POSITION, "a_position");
		glBindAttribLocation(shaderProgramObject_pv, SBJ_ATTRIBUTE_NORMAL, "a_normal");
		glLinkProgram(shaderProgramObject_pv);
	}*/

	shaderProgramObject_pv = glCreateProgram();
	//shaderProgramObject_pf = glCreateProgram();
	glAttachShader(shaderProgramObject_pv, vertexShaderObject_pv);
	glAttachShader(shaderProgramObject_pv, fragmentShaderObject_pv);
	//glAttachShader(shaderProgramObject_pf, vertexShaderObject_pf);
	//glAttachShader(shaderProgramObject_pf, fragmentShaderObject_pf);
	glBindAttribLocation(shaderProgramObject_pv, SBJ_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(shaderProgramObject_pv, SBJ_ATTRIBUTE_NORMAL, "a_normal");
	//glBindAttribLocation(shaderProgramObject_pf, SBJ_ATTRIBUTE_POSITION, "a_position");
	//glBindAttribLocation(shaderProgramObject_pf, SBJ_ATTRIBUTE_NORMAL, "a_normal");
	glLinkProgram(shaderProgramObject_pv);
	//glLinkProgram(shaderProgramObject_pf);

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
	laUniform = glGetUniformLocation(shaderProgramObject_pv, "u_la");
	ldUniform = glGetUniformLocation(shaderProgramObject_pv, "u_ld");
	lsUniform = glGetUniformLocation(shaderProgramObject_pv, "u_ls");
	lightPositionUniform = glGetUniformLocation(shaderProgramObject_pv, "u_lightPosition");
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
	laUniform = glGetUniformLocation(shaderProgramObject_pf, "u_la");
	ldUniform = glGetUniformLocation(shaderProgramObject_pf, "u_ld");
	lsUniform = glGetUniformLocation(shaderProgramObject_pf, "u_ls");
	lightPositionUniform = glGetUniformLocation(shaderProgramObject_pf, "u_lightPosition");
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

	// Here Starts OpenGL Code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// glShadeModel(),glHint() Deprecated In PP 

	// Clear The Screen Using Black Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use The Shader Program Object
	if (choosenShader == 'v')
		glUseProgram(shaderProgramObject_pv);
	else
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
	if (bLight == TRUE)
	{
		glUniform1i(lightingEnabledUniform, 1);

		glUniform3fv(laUniform, 1, lightAmbient);
		glUniform3fv(ldUniform, 1, lightDiffuse);
		glUniform3fv(lsUniform, 1, lightSpecular);
		glUniform4fv(lightPositionUniform, 1, lightPosition);

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

	SwapBuffers(ghdc);
}

void update(void)
{
	// Code

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
