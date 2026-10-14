// Header files
#include<windows.h>
#include<stdio.h> // For file I/O functions
#include<stdlib.h> // For exit()
#include"OGL.h"
#include"vmath.h"
#include"OGLCylinder.h"

using namespace vmath;

// OpenGL header files
#include<C:/glew/include/GL/glew.h>	// THIS MUST BE ABOVE gl.h
#include<GL/gl.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// OpenGL libraries
#pragma comment(lib, "glew32.lib")	// There is no compulsion that this should be before OpenGL32.lib. We are keeping this for parity with include files.
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib,"OGLCylinder.lib")

// Enums
enum CameraPosition
{
	FRONT = 1,
	RIGHT,
	BACK,
	LEFT,
	TOP,
	BOTTOM
};

// Global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declarations
BOOL gbFullScreen = FALSE;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
FILE* gpFile = NULL;
BOOL gbActiveWindow = FALSE;

// Debugging variables
CameraPosition gCameraPosition = FRONT;
mat4 gViewMatrix = mat4::identity();
const float gfCenterToEyeDistance = 6.0f;
float gfXRotationAngle = 0.0f;
float gfYRotationAngle = 0.0f;
float gfZRotationAngle = 0.0f;

// Animation related global variables
BOOL bLight = FALSE;
BOOL bTexture = FALSE;

// Programmable pipeline related global variables
GLuint shaderProgramObject;
enum
{
	SAS_ATTRIBUTE_POSITION = 0,
	SAS_ATTRIBUTE_COLOR,
	SAS_ATTRIBUTE_NORMAL,
	SAS_ATTRIBUTE_TEXTURE0
};
GLuint gVao_cylinder;
GLuint gVbo_cylinder_position;
GLuint gVbo_cylinder_normal;
GLuint gVbo_cylinder_texcoord;				// Vertex Buffer Object for pyramid colors
GLuint gVbo_cylinder_element;
mat4 perspectiveProjectionMatrix;	// mat4 is defined in vmath

// Shader uniforms
GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint textureSamplerUniform;
GLuint projectionMatrixUniform;

GLuint laUniform;				// Light ambient uniform
GLuint ldUniform;				// Light diffuse uniform
GLuint lsUniform;				// Light specular uniform
GLuint lightPositionUniform;	// Light position uniform

GLuint kaUniform;					// Material ambient uniform
GLuint kdUniform;					// Material diffuse uniform
GLuint ksUniform;					// Material specular uniform
GLuint materialShininessUniform;	// Material shininess uniform

GLuint lightingEnabledUniform;
GLuint textureEnabledUniform;

GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffused[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffused[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 50.0f;

GLuint texture_smiley;

float *cylinder_vertices = NULL;
float *cylinder_normals = NULL;
float *cylinder_textures = NULL;
unsigned short *cylinder_elements = NULL;

GLuint gNumVertices;
GLuint gNumElements;

const float cylinderHeight = 2.0f;
const float cylinderBaseRadius = 1.0f;
const float cylinderTopRadius = 1.0f;

const unsigned int cylinderSlices = 20;
const unsigned int cylinderStacks = 20;

// Entry-point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Local function declarations
	int initialize(void);
	void display(void);
	void uninitialize(void);

	// Variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	BOOL bDone = FALSE;
	int iRetVal = 0;

	// Code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(
			NULL,
			TEXT("Creation of log file failed. Exitting..."),
			TEXT("File I/O Error"),
			MB_OK
		);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log file successfully created.\n");
	}

	// Ininitialization of WNDCLASSEX structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// Registering the WNDCLASSEX
	RegisterClassEx(&wndclass);

	int xPos = (GetSystemMetrics(SM_CXSCREEN) - WIN_WIDTH) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - WIN_HEIGHT) / 2;

	// Create the window
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		szAppName,
		TEXT("OGL Window"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		xPos,
		yPos,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ghwnd = hwnd;

	// Initialize
	iRetVal = initialize();
	if (iRetVal == -1)
	{
		fprintf(gpFile, "Choose pixel format failed.\n");
		uninitialize();
	}
	else if (iRetVal == -2)
	{
		fprintf(gpFile, "Set pixel format failed.\n");
		uninitialize();
	}
	else if (iRetVal == -3)
	{
		fprintf(gpFile, "Create OpenGL context failed.\n");
		uninitialize();
	}
	else if (iRetVal == -4)
	{
		fprintf(gpFile, "Making OpenGL context as current context failed.\n");
		uninitialize();
	}
	else if (iRetVal == -5)
	{
		fprintf(gpFile, "GLEW initialization failed.\n");
		uninitialize();
	}
	else if (iRetVal == -6)
	{
		fprintf(gpFile, "Loading of texture smiley failed.\n");
		uninitialize();
	}
	else
	{
		fprintf(gpFile, "Initialization of PIXELFORMATDESCRIPTOR structure successful.\n");
	}

	// Show window
	ShowWindow(hwnd, iCmdShow);

	// Foregroundinng and focusing the window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Game loop
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
				// Render the scene
				display();

				// Update the scene
				// update();
			}
		}
	}

	uninitialize();

	return((int)msg.wParam);
}

// Callback function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Local function declarations
	void ToggleFullscreen(void);
	void resize(int, int);

	// Variable declarations
	vec3 eyeVector;
	vec3 centerVector;
	vec3 upVector;

	// Code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		fprintf(gpFile, "Window is in focus.\n");
		gbActiveWindow = TRUE;
		break;

	case WM_KILLFOCUS:
		fprintf(gpFile, "Window is not in focus.\n");
		gbActiveWindow = FALSE;
		break;

	case WM_ERASEBKGND:
		return(0);

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case 27:
			DestroyWindow(hwnd);
			break;

		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
			gCameraPosition = (CameraPosition)(wParam - 48);
			centerVector = vec3(0.0f, 0.0f, 0.0f);

			switch (gCameraPosition)
			{
			case FRONT:
				eyeVector = vec3(0.0f, 0.0f, gfCenterToEyeDistance);
				upVector = vec3(0.0f, 1.0f, 0.0f);
				break;

			case RIGHT:
				eyeVector = vec3(gfCenterToEyeDistance, 0.0f, 0.0f);
				upVector = vec3(0.0f, 1.0f, 0.0f);
				break;

			case BACK:
				eyeVector = vec3(0.0f, 0.0f, -gfCenterToEyeDistance);
				upVector = vec3(0.0f, 1.0f, 0.0f);
				break;

			case LEFT:
				eyeVector = vec3(-gfCenterToEyeDistance, 0.0f, 0.0f);
				upVector = vec3(0.0f, 1.0f, 0.0f);
				break;

			case TOP:
				eyeVector = vec3(0.0f, gfCenterToEyeDistance, 0.0f);
				upVector = vec3(0.0f, 0.0f, -1.0f);
				break;

			case BOTTOM:
				eyeVector = vec3(0.0f, -gfCenterToEyeDistance, 0.0f);
				upVector = vec3(0.0f, 0.0f, 1.0f);
				break;

			default:
				eyeVector = vec3(0.0f, 0.0f, gfCenterToEyeDistance);
				upVector = vec3(0.0f, 1.0f, 0.0f);
				break;
			}
			gViewMatrix = vmath::lookat(eyeVector, centerVector, upVector);
			break;

		default:
			break;

		}
		break;

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

		case 'T':
		case 't':
			if (bTexture == FALSE)
			{
				bTexture = TRUE;
			}
			else
			{
				bTexture = FALSE;
			}
			break;

		case 'X':
			gfXRotationAngle = gfXRotationAngle + 0.1f;
			if (gfXRotationAngle >= 360.0f)
			{
				gfXRotationAngle = 0.0f;
			}
			break;

		case 'x':
			gfXRotationAngle = gfXRotationAngle - 0.1f;
			if (gfXRotationAngle <= 0.0f)
			{
				gfXRotationAngle = 360.0f;
			}
			break;

		case 'Y':
			gfYRotationAngle = gfYRotationAngle + 0.1f;
			if (gfYRotationAngle >= 360.0f)
			{
				gfYRotationAngle = 0.0f;
			}
			break;

		case 'y':
			gfYRotationAngle = gfYRotationAngle - 0.1f;
			if (gfYRotationAngle <= 0.0f)
			{
				gfYRotationAngle = 360.0f;
			}
			break;

		case 'Z':
			gfZRotationAngle = gfZRotationAngle + 0.1f;
			if (gfZRotationAngle >= 360.0f)
			{
				gfZRotationAngle = 0.0f;
			}
			break;

		case 'z':
			gfZRotationAngle = gfZRotationAngle - 0.1f;
			if (gfZRotationAngle <= 0.0f)
			{
				gfZRotationAngle = 360.0f;
			}
			break;

		default:
			break;
		}
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
	// Variable declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;

	// Code
	wp.length = sizeof(WINDOWPLACEMENT);

	if (gbFullScreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);

			if (
				GetWindowPlacement(ghwnd, &wp)
				&& GetMonitorInfo(
					MonitorFromWindow(
						ghwnd,
						MONITORINFOF_PRIMARY
					),
					&mi
				)
				)
			{
				SetWindowLong(
					ghwnd,
					GWL_STYLE,
					dwStyle & ~WS_OVERLAPPEDWINDOW
				);
				SetWindowPos(
					ghwnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED
				);
			}

			ShowCursor(FALSE);
			gbFullScreen = TRUE;
		}
	}
	else
	{
		SetWindowLong(
			ghwnd,
			GWL_STYLE,
			dwStyle | WS_OVERLAPPEDWINDOW
		);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(
			ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED
		);
		ShowCursor(TRUE);
		gbFullScreen = FALSE;
	}
}

int initialize(void)
{
	// Local function declarations
	void resize(int, int);
	void printGLInfo(void);
	void uninitialize(void);
	BOOL loadGLTexture(GLuint*, TCHAR[]);

	// Variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
	size_t sizeOfCylinderVertices;
	size_t sizeOfCylinderNormals;
	size_t sizeOfCylinderTexCoords;
	size_t sizeOfCylinderElements;

	vec3 eyeVector;
	vec3 centerVector;
	vec3 upVector;

	// Code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	// memset((void*)&pfd, NULL, sizeof(PIXELFORMATDESCRIPTOR))

	// Initialization of PIXELFORMATDESCRIPTOR structure
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;						// 24 also can be done.

	// GetDC
	ghdc = GetDC(ghwnd);

	// Choose pixel format
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormatIndex == 0)
	{
		return(-1);
	}

	// Set chosen pixel format
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		return(-2);
	}

	// Create OpenGL rendering context
	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL)
	{
		return(-3);
	}

	// Make the rendering context as the current context
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		return(-4);
	}

	// GLEW initialization
	if (glewInit() != GLEW_OK)
	{
		return(-5);
	}

	printGLInfo();

	// Creating, Compiling, Error Checking Shaders

	// Vertex shader

	// 01. Writing shader code
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec3 a_normal;" \
		"in vec2 a_texCoord;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform vec4 u_lightPosition;" \
		"uniform int u_lightingEnabled;" \
		"uniform int u_textureEnabled;" \
		"out vec3 transformedNormals;" \
		"out vec3 lightDirection;" \
		"out vec3 viewerVector;" \
		"out vec2 a_texCoord_out;" \
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
		"if(u_textureEnabled == 1)" \
		"{" \
		"a_texCoord_out = a_texCoord;" \
		"}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix *  u_modelMatrix * a_position;" \
		"}";

	// 02. Creating shader object
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// 03. Giving shader code to shader object
	glShaderSource(
		vertexShaderObject,					// Konala dyaychay shader cha source code?
		1,									// Kiti strings denar ahe? Number of shaders.
		(const GLchar**)&vertexShaderSourceCode,	// Shaders cha array. Yachach count adhichya parameter madhe dila ahe apan.
		NULL								// Third parameter jar multiple strings cha array asel tar ha parameter is the array of lengths of each string in the array passed as third parameter. Ithe NULL deun apan as sangto ahot ki amchya saglya strings NULL terminated ahet.
	);

	// 04. Compile the shader
	glCompileShader(vertexShaderObject);

	// 05. Error checking of shader compilation
	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	// 05.a. Getting compilation status
	glGetShaderiv(
		vertexShaderObject,	// Konach status pahije?
		GL_COMPILE_STATUS,	// Kashach status pahije? (Operation)
		&status				// Kashamadhe deu status?
	);
	if (status == GL_FALSE)
	{
		// 05.b. Getting length of log of compilation status
		glGetShaderiv(
			vertexShaderObject,	// Konasathi?
			GL_INFO_LOG_LENGTH,	// Kay pahije?
			&infoLogLength		// Kashamadhe bharun deu?
		);
		if (infoLogLength > 0)
		{
			// 05.c. Allocate enough memory to the buffer to hold compilation log
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				// 05.d. Get the compilation log into this allocated buffer
				GLsizei written;

				glGetShaderInfoLog(
					vertexShaderObject,	// Kontya shader baddl chya compilation cha log pahije?
					infoLogLength,		// Kiti lambichi info pahije?
					&written,			// Kiti length actually lihili geli?
					log					// Kashamadhe log dyaychay?
				);

				// 05.e. Display the contents of buffer/log
				fprintf(gpFile, "Vertex shader compilation log: %s\n", log);

				fflush(gpFile);

				// 05.f. Free the allocated buffer
				free(log);

				// 05.g. Exit the application due to error
				uninitialize();
			}
		}
	}

	// Fragment shader

	// 01. Writing shader code
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec3 transformedNormals;" \
		"in vec3 lightDirection;" \
		"in vec3 viewerVector;" \
		"in vec2 a_texCoord_out;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_materialShininess;" \
		"uniform int u_lightingEnabled;" \
		"uniform int u_textureEnabled;" \
		"uniform sampler2D u_textureSampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec3 phong_ads_light;" \
		"if(u_lightingEnabled == 1)" \
		"{" \
		"vec3 ambient = u_la * u_ka;" \
		"vec3 normalizedTransformedNormals = normalize(transformedNormals);" \
		"vec3 normalizedLightDirection = normalize(lightDirection);" \
		"vec3 diffused = u_ld * u_kd * max(dot(normalizedLightDirection, normalizedTransformedNormals), 0.0);" \
		"vec3 reflectionVector = reflect(-normalizedLightDirection, normalizedTransformedNormals);" \
		"vec3 normalizedViewerVector = normalize(viewerVector);" \
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector, normalizedViewerVector), 0.0), u_materialShininess);" \
		"phong_ads_light = ambient + diffused + specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light = vec3(1.0, 1.0, 1.0);" \
		"}" \
		"if(u_textureEnabled == 1)" \
		"{" \
		"FragColor = vec4(phong_ads_light, 1.0) * texture(u_textureSampler, a_texCoord_out);" \
		"}" \
		"else" \
		"{" \
		"FragColor = vec4(phong_ads_light, 1.0);" \
		"}" \
		"}";

	// 02. Creating shader object
	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// 03. Giving shader code to shader object
	glShaderSource(
		fragmentShaderObject,
		1,
		(const GLchar**)&fragmentShaderSourceCode,
		NULL
	);

	// 04. Compile the shader
	glCompileShader(fragmentShaderObject);

	// 05. Error checking of shader compilation
	status = 0;
	infoLogLength = 0;
	log = NULL;

	// 05.a. Getting compilation status
	glGetShaderiv(
		fragmentShaderObject,
		GL_COMPILE_STATUS,
		&status
	);
	if (status == GL_FALSE)
	{
		// 05.b. Getting length of log of compilation status
		glGetShaderiv(
			fragmentShaderObject,
			GL_INFO_LOG_LENGTH,
			&infoLogLength
		);
		if (infoLogLength > 0)
		{
			// 05.c. Allocate enough memory to the buffer to hold the compilation log
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				// 05.d. Get the compilation log into this allocated buffer
				GLsizei written;
				glGetShaderInfoLog(
					fragmentShaderObject,
					infoLogLength,
					&written,
					log
				);

				// 05.e. Display the contents of the buffer/log
				fprintf(gpFile, "Fragment shader compilation log: %s\n", log);

				fflush(gpFile);

				// 05.f. Free the allocated buffer
				free(log);

				// 05.g. Exit the application due to error
				uninitialize();
			}
		}
	}

	// Shader program object

	// 01. Create shader program object
	shaderProgramObject = glCreateProgram();

	// 02. Attach desired shaders to this shader program object
	glAttachShader(
		shaderProgramObject,	// Konala chiktavaych?
		vertexShaderObject		// Kon chiktavaych?
	);

	glAttachShader(
		shaderProgramObject,
		fragmentShaderObject
	);

	// TEMPLATE PROGRAM STEPS
	// A. In initialize()
	// A.1. Pre-linking and binding of shader program object with vertex attributes.
	glBindAttribLocation(
		shaderProgramObject,
		SAS_ATTRIBUTE_POSITION,
		"a_position"
	);

	glBindAttribLocation(
		shaderProgramObject,
		SAS_ATTRIBUTE_NORMAL,
		"a_normal"
	);

	glBindAttribLocation(
		shaderProgramObject,
		SAS_ATTRIBUTE_TEXTURE0,
		"a_texCoord"
	);

	// 03. Link shader program
	glLinkProgram(shaderProgramObject);

	// 04. Error checking of shader linking
	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetProgramiv(
		shaderProgramObject,	// Konach linking status ghyaychay?
		GL_LINK_STATUS,			// Kasl status ghyaychay?
		&status					// Konamadhe status bharun ghyaychay?
	);
	if (status == GL_FALSE)
	{
		glGetProgramiv(
			shaderProgramObject,
			GL_INFO_LOG_LENGTH,
			&infoLogLength
		);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;

				glGetProgramInfoLog(
					shaderProgramObject,
					infoLogLength,
					&written,
					log
				);

				fprintf(gpFile, "Shader program link log: %s\n", log);

				fflush(gpFile);

				free(log);

				uninitialize();
			}
		}
	}

	// TEMPLATE PROGRAM STEPS
	// A.2. Post linking and retrieving/getting uniform location from the shader program object
	modelMatrixUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_modelMatrix"
	);

	viewMatrixUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_viewMatrix"
	);

	projectionMatrixUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_projectionMatrix"
	);
	// Q. glGetUniformLocation() post linking ch ka karav?
	// Ans. Karan linking kelyashivay tya uniform ch location aplyala milu shakat nahi. Linking zalyavarach tya uniform la location milalel ast.

	textureSamplerUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_textureSampler"
	);

	laUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_la"
	);

	ldUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_ld"
	);

	lsUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_ls"
	);

	lightPositionUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_lightPosition"
	);

	kaUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_ka"
	);

	kdUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_kd"
	);

	ksUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_ks"
	);

	materialShininessUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_materialShininess"
	);

	lightingEnabledUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_lightingEnabled"
	);

	textureEnabledUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_textureEnabled"
	);

	// A.3. Declare vertex data arrays
	gNumVertices = getCylinderVerticesCount(cylinderSlices, cylinderStacks);
	gNumElements = getCylinderElementsCount(cylinderSlices, cylinderStacks);

	sizeOfCylinderVertices = gNumVertices * 3 * sizeof(float);
	sizeOfCylinderNormals = gNumVertices * 3 * sizeof(float);
	sizeOfCylinderTexCoords = gNumVertices * 2 * sizeof(float);
	sizeOfCylinderElements = gNumElements * sizeof(unsigned short);	// Don't need to multiply by 3 here as getCylinderElementsCount() will give exact count of elements.

	cylinder_vertices = (float*)malloc(sizeof(float) * gNumVertices * 3);	// Because each vertex position will have three co-ordinates, x, y and z.
	cylinder_normals = (float*)malloc(sizeof(float) * gNumVertices * 3);	// Because each vertex normal will have three co-ordinates, x, y and z.
	cylinder_textures = (float*)malloc(sizeof(float) * gNumVertices * 2);	// Because each vertex texture will have two co-ordinates, x and y.
	cylinder_elements = (unsigned short *)malloc(sizeof(unsigned short) * gNumElements * 3);	// Because each element will be triangle and each triangle has three vertices.

	getCylinderVerticesData(cylinder_vertices, cylinder_normals, cylinder_textures, cylinder_elements, cylinderBaseRadius, cylinderTopRadius, cylinderSlices, cylinderStacks);
	
	fprintf(gpFile, "Cylinder vertices count: %d.\n", gNumVertices);
	fprintf(gpFile, "Cylinder elements count: %d.\n", gNumElements);

	fprintf(gpFile, "Cylinder vertex positions:\n");
	for (int index = 0; index < gNumVertices * 3;)
	{
		fprintf(gpFile, "%3d: ", index);
		fprintf(gpFile, "(%f,", cylinder_vertices[index++]);
		fprintf(gpFile, " %f,", cylinder_vertices[index++]);
		fprintf(gpFile, " %f)\n", cylinder_vertices[index++]);
	}
	fflush(gpFile);

	fprintf(gpFile, "Cylinder elements:\n");
	for (int index = 0; index < gNumElements * 3;)
	{
		fprintf(gpFile, "%3d: ", index);
		fprintf(gpFile, "(%u,", cylinder_elements[index++]);
		fprintf(gpFile, " %u,", cylinder_elements[index++]);
		fprintf(gpFile, " %u)\n", cylinder_elements[index++]);
	}
	fflush(gpFile);

	fprintf(gpFile, "sizeOfCylinderVertices\t: %zd.\n", sizeOfCylinderVertices);
	fprintf(gpFile, "sizeOfCylinderNormals\t: %zd.\n", sizeOfCylinderNormals);
	fprintf(gpFile, "sizeOfCylinderTexCoords\t: %zd.\n", sizeOfCylinderTexCoords);
	fprintf(gpFile, "sizeOfCylinderElements\t: %zd.\n", sizeOfCylinderElements);
	fflush(gpFile);

	// gVao_cylinder
	glGenVertexArrays(1, &gVao_cylinder);
	glBindVertexArray(gVao_cylinder);

	// position vbo
	glGenBuffers(1, &gVbo_cylinder_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_cylinder_position);
	glBufferData(GL_ARRAY_BUFFER, sizeOfCylinderVertices, cylinder_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SAS_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SAS_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &gVbo_cylinder_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_cylinder_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeOfCylinderNormals, cylinder_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(SAS_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SAS_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// tex co-ord vbo
	glGenBuffers(1, &gVbo_cylinder_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_cylinder_texcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeOfCylinderTexCoords, cylinder_textures, GL_STATIC_DRAW);
	glVertexAttribPointer(SAS_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SAS_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &gVbo_cylinder_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_cylinder_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeOfCylinderElements, cylinder_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind gVao_cylinder
	glBindVertexArray(0);

	// Here starts OpenGL code
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Depth related changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	if (loadGLTexture(&texture_smiley, MAKEINTRESOURCE(ID_BITMAP_SMILEY)) == FALSE)
	{
		fprintf(gpFile, "Failed to load smiley texture.\n");
		return(-6);
	}

	// Enabling the texture
	glEnable(GL_TEXTURE_2D);

	// A.7. Initialize needed matrices using vmath.
	perspectiveProjectionMatrix = mat4::identity();

	centerVector = vec3(0.0f, 0.0f, 0.0f);
	eyeVector = vec3(0.0f, 0.0f, gfCenterToEyeDistance);
	upVector = vec3(0.0f, 1.0f, 0.0f);

	gViewMatrix = vmath::lookat(eyeVector, centerVector, upVector);

	// Warm up resize call
	resize(WIN_WIDTH, WIN_HEIGHT);

	return(0);
}

void printGLInfo(void)
{
	// Variable declarations
	GLint numExtensions = 0;
	
	// Code
	fprintf(gpFile, "OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version: %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	fprintf(gpFile, "Number of supported extensions: %d\n", numExtensions);

	for (int i = 0; i < numExtensions; i++)
	{
		fprintf(
			gpFile,
			"%s\n",
			glGetStringi(
				GL_EXTENSIONS,
				i
			)
		);
	}
}

void resize(int width, int height)
{
	// Code
	if (height == 0) // To avoid divide by zero instruction (illegal instruction) in future
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// B. Use vmath for initialization of particular projection matrix
	perspectiveProjectionMatrix = vmath::perspective(
		45.0f,
		((GLfloat)width / (GLfloat)height),
		0.1f,
		100.0f
	);
}

void display(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Using Shader Program Object In Display

	// 01. Use shader program object
	glUseProgram(shaderProgramObject);

	// 02. Draw the desired graphics/animation/scene

	// 
	// C.2.
	mat4 initialPositionTranslationMatrix = mat4::identity();
	mat4 objectCenterAndRotationPivotMatchingTranslationMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 rotationXMatrix = mat4::identity();
	mat4 rotationYMatrix = mat4::identity();
	mat4 rotationZMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

	//initialPositionTranslationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
	rotationXMatrix = vmath::rotate(gfXRotationAngle, 1.0f, 0.0f, 0.0f);
	rotationYMatrix = vmath::rotate(gfYRotationAngle, 0.0f, 1.0f, 0.0f);
	rotationZMatrix = vmath::rotate(gfZRotationAngle, 0.0f, 0.0f, 1.0f);
	rotationMatrix = rotationXMatrix * rotationYMatrix * rotationZMatrix;	// This replaces glRotatef();
	objectCenterAndRotationPivotMatchingTranslationMatrix = vmath::translate(0.0f, 0.0f, -((cylinderHeight) / 2.0f));
	scaleMatrix = vmath::scale(1.0f, 1.0f, cylinderHeight);					// This replaces glScalef();

	modelMatrix = initialPositionTranslationMatrix * rotationMatrix * objectCenterAndRotationPivotMatchingTranslationMatrix * scaleMatrix;

	viewMatrix = gViewMatrix;

	// 
	// C.3.
	glUniformMatrix4fv(
		modelMatrixUniform,
		1,
		GL_FALSE,
		modelMatrix
	);

	glUniformMatrix4fv(
		viewMatrixUniform,
		1,
		GL_FALSE,
		viewMatrix
	);

	glUniformMatrix4fv(
		projectionMatrixUniform,
		1,
		GL_FALSE,
		perspectiveProjectionMatrix
	);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(
		GL_TEXTURE_2D,
		texture_smiley
	);
	glUniform1i(
		textureSamplerUniform,
		0
	);

	if (bLight == TRUE)
	{
		glUniform1i(lightingEnabledUniform, 1);

		glUniform3fv(laUniform, 1, lightAmbient);
		glUniform3fv(ldUniform, 1, lightDiffused);
		glUniform3fv(lsUniform, 1, lightSpecular);
		glUniform4fv(lightPositionUniform, 1, lightPosition);

		glUniform3fv(kaUniform, 1, materialAmbient);
		glUniform3fv(kdUniform, 1, materialDiffused);
		glUniform3fv(ksUniform, 1, materialSpecular);
		glUniform1f(materialShininessUniform, materialShininess);
	}
	else
	{
		glUniform1i(lightingEnabledUniform, 0);
	}

	if (bTexture == TRUE)
	{
		glUniform1i(textureEnabledUniform, 1);
	}
	else
	{
		glUniform1i(textureEnabledUniform, 0);
	}

	// *** bind vao ***
	glBindVertexArray(gVao_cylinder);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_cylinder_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// *** unbind vao ***
	glBindVertexArray(0);

	//
	// C.7
	// 03. Unuse the shader program object
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	// Variable declarations
	vec3 eyeVector;
	vec3 centerVector;
	vec3 upVector;

	// Code
	centerVector = vec3(0.0f, 0.0f, 0.0f);
	switch (gCameraPosition)
	{
	case FRONT:
		eyeVector = vec3(0.0f, 0.0f, gfCenterToEyeDistance);
		upVector = vec3(0.0f, 1.0f, 0.0f);
		break;

	case RIGHT:
		eyeVector = vec3(gfCenterToEyeDistance, 0.0f, 0.0f);
		upVector = vec3(0.0f, 1.0f, 0.0f);
		break;

	case BACK:
		eyeVector = vec3(0.0f, 0.0f, -gfCenterToEyeDistance);
		upVector = vec3(0.0f, 1.0f, 0.0f);
		break;

	case LEFT:
		eyeVector = vec3(-gfCenterToEyeDistance, 0.0f, 0.0f);
		upVector = vec3(0.0f, 1.0f, 0.0f);
		break;

	case TOP:
		eyeVector = vec3(0.0f, gfCenterToEyeDistance, 0.0f);
		upVector = vec3(0.0f, 0.0f, -1.0f);
		break;

	case BOTTOM:
		eyeVector = vec3(0.0f, -gfCenterToEyeDistance, 0.0f);
		upVector = vec3(0.0f, 0.0f, 1.0f);
		break;

	default:
		eyeVector = vec3(0.0f, 0.0f, gfCenterToEyeDistance);
		upVector = vec3(0.0f, 1.0f, 0.0f);
		break;
	}
	gViewMatrix = vmath::lookat(eyeVector, centerVector, upVector);
}

void uninitialize(void)
{
	// Local function declarations
	void ToggleFullscreen(void);

	// Code
	if (gbFullScreen)
	{
		ToggleFullscreen();
	}

	// 
	// D.1. Delete/uninitialize vertex buffer objects.
	if (gVbo_cylinder_element)
	{
		glDeleteBuffers(
			1,
			&gVbo_cylinder_element
		);
		gVbo_cylinder_element = 0;
	}

	if (gVbo_cylinder_texcoord)
	{
		glDeleteBuffers(
			1,
			&gVbo_cylinder_texcoord
		);
		gVbo_cylinder_texcoord = 0;
	}

	if (gVbo_cylinder_normal)
	{
		glDeleteBuffers(
			1,
			&gVbo_cylinder_normal
		);
		gVbo_cylinder_normal = 0;
	}

	if (gVbo_cylinder_position)
	{
		glDeleteBuffers(
			1,
			&gVbo_cylinder_position
		);
		gVbo_cylinder_position = 0;
	}

	//
	// D.2. Delete/uninitialize vertex array objects.
	if (gVao_cylinder)
	{
		glDeleteVertexArrays(
			1,
			&gVao_cylinder
		);
		gVao_cylinder = 0;
	}

	// Free the memory allocated for cylinder vertices data
	if (cylinder_elements)
	{
		free(cylinder_elements);
		cylinder_elements = NULL;
	}

	if (cylinder_textures)
	{
		free(cylinder_textures);
		cylinder_textures = NULL;
	}

	if (cylinder_normals)
	{
		free(cylinder_normals);
		cylinder_normals = NULL;
	}

	if (cylinder_vertices)
	{
		free(cylinder_vertices);
		cylinder_vertices = NULL;
	}

	// Shader Uninitialization
	
	if (shaderProgramObject)
	{
		// 01. Use the shader program object
		glUseProgram(shaderProgramObject);

		// 02. Get the number of attached shaders
		GLsizei numAttachedShaders;

		glGetProgramiv(
			shaderProgramObject,
			GL_ATTACHED_SHADERS,
			&numAttachedShaders
		);

		// 03. Create empty buffer to hold array of attached shader objects
		GLuint* shaderObjects = NULL;

		// 04. Allocate enough memory to this empty buffer according to the number of attached shaders and fill it with the attached shader objects
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));

		glGetAttachedShaders(
			shaderProgramObject,
			numAttachedShaders,
			&numAttachedShaders,
			shaderObjects
		);

		// 05. As the number of shader objects can be more than one, start a loop and inside that loop, detach the shader one by one, delete the shader one by one and finish the loop.
		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(
				shaderProgramObject,
				shaderObjects[i]
			);

			glDeleteShader(shaderObjects[i]);

			shaderObjects[i] = 0;
		}

		// 06. Free the memory allocated for buffer
		free(shaderObjects);

		shaderObjects = NULL;

		// 07. Unuse the shader program object
		glUseProgram(0);

		// 08. Delete the shader program object
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
		fprintf(gpFile, "Log file successfully closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

BOOL loadGLTexture(GLuint* texture, TCHAR imageResourceId[])
{
	// Variable declarations
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	BOOL bResult = FALSE;

	// Code
	hBitmap = (HBITMAP)LoadImage(
		GetModuleHandle(NULL),
		imageResourceId,
		IMAGE_BITMAP,
		0,
		0,
		LR_CREATEDIBSECTION
	);

	if (hBitmap)
	{
		bResult = TRUE;

		GetObject(
			hBitmap,
			sizeof(bmp),
			&bmp
		);

		glPixelStorei(
			GL_UNPACK_ALIGNMENT,
			1	// Karan shader madhe 1 chi jump hi jast changla performance dete 4 peksha.
		);

		glGenTextures(
			1,
			texture
		);

		glBindTexture(
			GL_TEXTURE_2D,
			*texture
		);

		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			GL_LINEAR
		);

		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR
		);

		// Create texture
		// Below two functions replace gluBuild2DMipmaps()
		glTexImage2D(
			GL_TEXTURE_2D,		// Target: glTexImage2D() cha data kuthe shodhaycha?
			0,					// Mipmap level: 0 means you tell OpenGL to decide the mip-mapping level.
			GL_RGB,				// Format: OpenGL image cha data kasa ghet.
			bmp.bmWidth,		// Image width.
			bmp.bmHeight,		// Image height.
			0,					// Border width.
			GL_BGR,				// Aplya image cha format.
			GL_UNSIGNED_BYTE,	// Datatype of next parameter.
			bmp.bmBits			// Image data.
		);

		glGenerateMipmap(
			GL_TEXTURE_2D
		);

		glBindTexture(
			GL_TEXTURE_2D,
			0
		);

		DeleteObject(hBitmap);
	}

	return(bResult);
}
