// Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>
#include "sb7ktx.h"

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

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable Declarations
FILE* gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
static BOOL gbFullscreen = FALSE;

// Terrain Related Global Variables
GLfloat dmap_depth = 6.0f;
//static double last_time = 1.0;
static double total_time = 0.0;
GLfloat t1, r1, h1;
GLuint tex_displacement;
GLuint tex_color;
//GLuint tex_colorUniform;
GLuint dmap_depthUniform;
GLuint tex_displacementUniform;

// Tesselation Shader Related Variables
/*
GLuint numberOfSegmentsUniform;
GLuint numberOfStripsUniform;
GLuint lineColorUniform;
unsigned int uiNumberOfSegments;
*/
// Programmable Pipeline Related Global Variables
GLuint shaderProgramObject;

enum
{
	SBJ_ATTRIBUTE_POSITION = 0,
	SBJ_ATTRIBUTE_COLOR,
	SBJ_ATTRIBUTE_NORMAL,
	SBJ_ATTRIBUTE_TEXTURE0
};

GLuint vao;
GLuint vbo;
GLuint viewMatrixUniform;
GLuint perspectiveProjectionMatrixUniform;
GLuint mvpMatrixUniform;

mat4 perspectiveProjectionMatrix;

// Entry-Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Declarations
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);

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
		TEXT("OpenGL Window : Terrain Rendering Using Displacement Map"),
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
			/*
		case VK_UP:
			uiNumberOfSegments++;
			if (uiNumberOfSegments >= 30)
				uiNumberOfSegments = 30;
			break;

		case VK_DOWN:
			uiNumberOfSegments--;
			if (uiNumberOfSegments <= 1)
				uiNumberOfSegments = 1;
			break;
			*/
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


	// Vertex Shader
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"out vec2 texcoord;" \
		"void main(void)" \
		"{" \
		"const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0), vec4(0.5, 0.0, -0.5, 1.0), vec4(-0.5,0.0, 0.5, 1.0), vec4(0.5, 0.0, 0.5, 1.0));" \
		"int x = gl_InstanceID & 63;" \
		"int y = gl_InstanceID >> 6;" \
		"vec2 offset = vec2(x, y);" \
		"texcoord = (vertices[gl_VertexID].xz + offset + vec2(0.5)) / 64.0;" \
		"gl_Position = vertices[gl_VertexID] + vec4(float(x - 32), 0.0, float(y - 32), 0.0);" \
		"}";
	/*
	// Vertex Shader
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \*/
		/*"uniform mat4 u_mvpMatrix;"*/
		/*"out vec2 texcoord;" \
		"void main(void)" \
		"{" \
		"int x = gl_InstanceID & 63;" \
		"int y = gl_InstanceID >> 6;" \
		"vec2 offset = vec2(x, y);" \*/
		/*"gl_Position = u_mvpMatrix * (a_position + vec4(float(x - 32), 0.0, float(y - 32), 0.0));" \*/
		/*"gl_Position = (a_position + vec4(float(x - 32), 0.0, float(y - 32), 0.0));" \
		"texcoord = (a_position.xz + offset + vec2(0.5)) / 64.0;" \
		"}";*/

	// Creating Shader Object
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	// Giving Shader Code To Shader Object 
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
	// Compile The Shader
	glCompileShader(vertexShaderObject);
	status = 0;
	infoLogLength = 0;
	log = NULL;
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
				fprintf(gpFile, "Vertex Shader Compilation Log: %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}


	// Tessellation Control Shader
	const GLchar* tessellationControlShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"layout(vertices = 4) out;" \
		"in vec2 texcoord[];" \
		"uniform mat4 u_mvpMatrix;" \
		"out vec2 tcs_texcoord_out[];" \
		"void main(void)" \
		"{" \
		"if (gl_InvocationID == 0)" \
		"{" \
		"vec4 p0 = u_mvpMatrix * gl_in[0].gl_Position;" \
		"vec4 p1 = u_mvpMatrix * gl_in[1].gl_Position;" \
		"vec4 p2 = u_mvpMatrix * gl_in[2].gl_Position;" \
		"vec4 p3 = u_mvpMatrix * gl_in[3].gl_Position;" \
		/*"vec4 p0 = gl_in[0].gl_Position;" \
		"vec4 p1 = gl_in[1].gl_Position;" \
		"vec4 p2 = gl_in[2].gl_Position;" \
		"vec4 p3 = gl_in[3].gl_Position;" \*/
		"p0 /= p0.w;" \
		"p1 /= p1.w;" \
		"p2 /= p2.w;" \
		"p3 /= p3.w;" \
		"if (p0.z <= 0.0 ||p1.z <= 0.0 ||p2.z <= 0.0 ||p3.z <= 0.0)" \
		"{" \
		"gl_TessLevelOuter[0] = 0.0;" \
		"gl_TessLevelOuter[1] = 0.0;" \
		"gl_TessLevelOuter[2] = 0.0;" \
		"gl_TessLevelOuter[3] = 0.0;" \
		"}" \
		"else" \
		"{" \
		"float l0 = length(p2.xy - p0.xy) * 16.0 + 1.0;" \
		"float l1 = length(p3.xy - p2.xy) * 16.0 + 1.0;" \
		"float l2 = length(p3.xy - p1.xy) * 16.0 + 1.0;" \
		"float l3 = length(p1.xy - p0.xy) * 16.0 + 1.0;" \
		"gl_TessLevelOuter[0] = l0;" \
		"gl_TessLevelOuter[1] = l1;" \
		"gl_TessLevelOuter[2] = l2;" \
		"gl_TessLevelOuter[3] = l3;" \
		"gl_TessLevelInner[0] = min(l1, l3);" \
		"gl_TessLevelInner[1] = min(l0, l2);" \
		"}" \
		"}" \
		"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" \
		"tcs_texcoord_out[gl_InvocationID] = texcoord[gl_InvocationID];" \
		"}";

	// Creating Shader Object
	GLuint tessellationControlShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);
	// Giving Shader Code To Shader Object 
	glShaderSource(tessellationControlShaderObject, 1, (const GLchar**)&tessellationControlShaderSourceCode, NULL);
	// Compile The Shader
	glCompileShader(tessellationControlShaderObject);
	status = 0;
	infoLogLength = 0;
	log = NULL;
	// Error Checking
	glGetShaderiv(tessellationControlShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(tessellationControlShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(tessellationControlShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "Tessellation Control Shader Compilation Log: %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}

	
	// Tessellation Evaluation Shader
	const GLchar* tessellationEvaluationShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"layout(quads, fractional_odd_spacing) in;" \
		"in vec2 tcs_texcoord_out[];" \
		"uniform sampler2D tex_displacement;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_pMatrix;" \
		"uniform float u_dmap_depth;" \
		"out vec2 texcoord;" \
		"out vec3 world_coord;" \
		"out vec3 eye_coord;" \
		"void main(void)" \
		"{" \
		"vec2 tc1 = mix(tcs_texcoord_out[0], tcs_texcoord_out[1], gl_TessCoord.x);" \
		"vec2 tc2 = mix(tcs_texcoord_out[2], tcs_texcoord_out[3], gl_TessCoord.x);" \
		"vec2 tc = mix(tc2, tc1, gl_TessCoord.y);" \
		"vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);" \
		"vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);" \
		"vec4 p = mix(p2, p1, gl_TessCoord.y);" \
		"p.y += texture(tex_displacement, tc).r * u_dmap_depth;" \
		/*"p.y += vec2(tc).r * u_dmap_depth;" \*/
		/*"p.y += vec2(tc).r;" \*/
		/*"gl_Position = u_viewMatrix * p;" \
		"texcoord = tc;" \*/
		"vec4 P_eye = u_viewMatrix * p;" \
		"texcoord = tc;" \
		"world_coord = p.xyz;" \
		"eye_coord = P_eye.xyz;" \
		"gl_Position = u_pMatrix * P_eye;" \
		"}";

	// Creating Shader Object
	GLuint tessellationEvaluationShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);
	// Giving Shader Code To Shader Object 
	glShaderSource(tessellationEvaluationShaderObject, 1, (const GLchar**)&tessellationEvaluationShaderSourceCode, NULL);
	// Compile The Shader
	glCompileShader(tessellationEvaluationShaderObject);
	status = 0;
	infoLogLength = 0;
	log = NULL;
	// Error Checking
	glGetShaderiv(tessellationEvaluationShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(tessellationEvaluationShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(tessellationEvaluationShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "Tessellation Evaluation Shader Compilation Log: %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}
	


	// Fragment Shader
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec2 texcoord;" \
		"in vec3 world_coord;" \
		"in vec3 eye_coord;" \
		"uniform sampler2D tex_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec4 landscape = texture(tex_color, texcoord);" \
		"FragColor = landscape;" \
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
	glAttachShader(shaderProgramObject, tessellationControlShaderObject);
	glAttachShader(shaderProgramObject, tessellationEvaluationShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);
	glBindAttribLocation(shaderProgramObject, SBJ_ATTRIBUTE_POSITION, "a_position");
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
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
	perspectiveProjectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_pMatrix");
	dmap_depthUniform = glGetUniformLocation(shaderProgramObject, "u_dmap_depth");
	//tex_displacementUniform = glGetUniformLocation(shaderProgramObject, "tex_displacement");
	//tex_colorUniform = glGetUniformLocation(shaderProgramObject, "u_tex_color");
	//numberOfSegmentsUniform = glGetUniformLocation(shaderProgramObject, "u_numberOfSegments");
	//numberOfStripsUniform = glGetUniformLocation(shaderProgramObject, "u_numberOfStrips");
	//lineColorUniform = glGetUniformLocation(shaderProgramObject, "u_lineColor");

	// Declaration Of Vertex Data Arrays
	
	/*
	const GLfloat vertices[] =
	{
		-0.5f, -0.5f,
		0.5f, -0.5f,
		-0.5f, 0.5f,
		0.5f, 0.5f
	};
	*/
	

	// vao And vbo Related Code
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/*
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);
	*/
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*
	glBindBuffer(GL_ARRAY_BUFFER, tex_displacement);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_displacement), &tex_displacement, GL_STATIC_DRAW);
	glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);
	*/
	glBindVertexArray(0);

	// Here Starts OpenGL Code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);                  // glShadeModel(),glHint() Deprecated In PP 

	glEnable(GL_CULL_FACE);

	// Clear The Screen Using Black Color
	glClearColor(0.85f, 0.95f, 1.0f, 1.0f);

	//uiNumberOfSegments = 1;			// Our Line Have One Curve

	
	//glUniform1f(tex_displacementUniform, tex_displacement);
	//glActiveTexture(NULL);
	

	glActiveTexture(GL_TEXTURE1);
	tex_color = sb7::ktx::file::load("textures/terragen_color.ktx");
	

	
	
	//glUniform1f(tex_colorUniform, tex_color);

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

	perspectiveProjectionMatrix = vmath::perspective(60.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
}

void display(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use The Shader Program Object
	glUseProgram(shaderProgramObject);

	// Transformations
	mat4 translationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	t1 = (float)total_time + 1.03f;
	r1 = sin(t1 * 5.37f) * 15.0f + 16.0f;
	h1 = cos(t1 * 4.79f) * 2.0f + 3.2f;

	//viewMatrix = vmath::translate(0.0f, -5.0f, -10.0f);
	//modelViewMatrix = translationMatrix;
	viewMatrix = vmath::lookat(vmath::vec3(sinf(t1) * r1, h1, cosf(t1) * r1),
						vmath::vec3(0.0f, 0.0f, 0.0f),
						vmath::vec3(0.0, 1.0, 0.0));
	modelViewProjectionMatrix = perspectiveProjectionMatrix * viewMatrix;
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(perspectiveProjectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(dmap_depthUniform, dmap_depth);

	glActiveTexture(GL_TEXTURE0);
	tex_displacement = sb7::ktx::file::load("textures/terragen1.ktx");
	
	//tex_displacement = sb7::ktx::file::load("textures/terragen1.ktx");
	
	//glUniform1i(numberOfSegmentsUniform, uiNumberOfSegments);
	
	
	/*
	TCHAR str[255];
	wsprintf(str, TEXT("OpenGL Window | Number Of Segments = %d"), uiNumberOfSegments);
	SetWindowText(ghwnd, str);

	glUniform1i(numberOfStripsUniform, 1);

	if (uiNumberOfSegments < 30)
		glUniform4fv(lineColorUniform, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	else
		glUniform4fv(lineColorUniform, 1, vmath::vec4(0.5f, 1.0f, 0.0f, 1.0f));
	*/
	glBindVertexArray(vao);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	//glEnable(GL_CULL_FACE);
	glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
	//glDrawArraysInstanced(GL_TRIANGLES, 0, 4, 64 * 64);

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
	/*
	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}
	*/
	// Deletion And Uninitialization of vao
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
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

// KTX texture
namespace sb7
{

	namespace ktx
	{

		namespace file
		{

			static const unsigned char identifier[] =
			{
				0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
			};

			static const unsigned int swap32(const unsigned int u32)
			{
				union
				{
					unsigned int u32;
					unsigned char u8[4];
				} a, b;

				a.u32 = u32;
				b.u8[0] = a.u8[3];
				b.u8[1] = a.u8[2];
				b.u8[2] = a.u8[1];
				b.u8[3] = a.u8[0];

				return b.u32;
			}

			static const unsigned short swap16(const unsigned short u16)
			{
				union
				{
					unsigned short u16;
					unsigned char u8[2];
				} a, b;

				a.u16 = u16;
				b.u8[0] = a.u8[1];
				b.u8[1] = a.u8[0];

				return b.u16;
			}

			static unsigned int calculate_stride(const header& h, unsigned int width, unsigned int pad = 4)
			{
				unsigned int channels = 0;

				switch (h.glbaseinternalformat)
				{
				case GL_RED:    channels = 1;
					break;
				case GL_RG:     channels = 2;
					break;
				case GL_BGR:
				case GL_RGB:    channels = 3;
					break;
				case GL_BGRA:
				case GL_RGBA:   channels = 4;
					break;
				}

				unsigned int stride = h.gltypesize * channels * width;

				stride = (stride + (pad - 1)) & ~(pad - 1);

				return stride;
			}

			static unsigned int calculate_face_size(const header& h)
			{
				unsigned int stride = calculate_stride(h, h.pixelwidth);

				return stride * h.pixelheight;
			}

			extern
				unsigned int load(const char* filename, unsigned int tex)
			{
				FILE* fp;
				GLuint temp = 0;
				GLuint retval = 0;
				header h;
				size_t data_start, data_end;
				unsigned char* data;
				GLenum target = GL_NONE;

				fp = fopen(filename, "rb");

				if (!fp)
					return 0;

				if (fread(&h, sizeof(h), 1, fp) != 1)
					goto fail_read;

				if (memcmp(h.identifier, identifier, sizeof(identifier)) != 0)
					goto fail_header;

				if (h.endianness == 0x04030201)
				{
					// No swap needed
				}
				else if (h.endianness == 0x01020304)
				{
					// Swap needed
					h.endianness = swap32(h.endianness);
					h.gltype = swap32(h.gltype);
					h.gltypesize = swap32(h.gltypesize);
					h.glformat = swap32(h.glformat);
					h.glinternalformat = swap32(h.glinternalformat);
					h.glbaseinternalformat = swap32(h.glbaseinternalformat);
					h.pixelwidth = swap32(h.pixelwidth);
					h.pixelheight = swap32(h.pixelheight);
					h.pixeldepth = swap32(h.pixeldepth);
					h.arrayelements = swap32(h.arrayelements);
					h.faces = swap32(h.faces);
					h.miplevels = swap32(h.miplevels);
					h.keypairbytes = swap32(h.keypairbytes);
				}
				else
				{
					goto fail_header;
				}

				// Guess target (texture type)
				if (h.pixelheight == 0)
				{
					if (h.arrayelements == 0)
					{
						target = GL_TEXTURE_1D;
					}
					else
					{
						target = GL_TEXTURE_1D_ARRAY;
					}
				}
				else if (h.pixeldepth == 0)
				{
					if (h.arrayelements == 0)
					{
						if (h.faces == 0)
						{
							target = GL_TEXTURE_2D;
						}
						else
						{
							target = GL_TEXTURE_CUBE_MAP;
						}
					}
					else
					{
						if (h.faces == 0)
						{
							target = GL_TEXTURE_2D_ARRAY;
						}
						else
						{
							target = GL_TEXTURE_CUBE_MAP_ARRAY;
						}
					}
				}
				else
				{
					target = GL_TEXTURE_3D;
				}

				// Check for insanity...
				if (target == GL_NONE ||                                    // Couldn't figure out target
					(h.pixelwidth == 0) ||                                  // Texture has no width???
					(h.pixelheight == 0 && h.pixeldepth != 0))              // Texture has depth but no height???
				{
					goto fail_header;
				}

				temp = tex;
				if (tex == 0)
				{
					glGenTextures(1, &tex);
				}

				glBindTexture(target, tex);

				data_start = ftell(fp) + h.keypairbytes;
				fseek(fp, 0, SEEK_END);
				data_end = ftell(fp);
				fseek(fp, data_start, SEEK_SET);

				data = new unsigned char[data_end - data_start];
				memset(data, 0, data_end - data_start);

				fread(data, 1, data_end - data_start, fp);

				if (h.miplevels == 0)
				{
					h.miplevels = 1;
				}

				switch (target)
				{
				case GL_TEXTURE_1D:
					glTexStorage1D(GL_TEXTURE_1D, h.miplevels, h.glinternalformat, h.pixelwidth);
					glTexSubImage1D(GL_TEXTURE_1D, 0, 0, h.pixelwidth, h.glformat, h.glinternalformat, data);
					break;
				case GL_TEXTURE_2D:
					// glTexImage2D(GL_TEXTURE_2D, 0, h.glinternalformat, h.pixelwidth, h.pixelheight, 0, h.glformat, h.gltype, data);
					if (h.gltype == GL_NONE)
					{
						glCompressedTexImage2D(GL_TEXTURE_2D, 0, h.glinternalformat, h.pixelwidth, h.pixelheight, 0, 420 * 380 / 2, data);
					}
					else
					{
						glTexStorage2D(GL_TEXTURE_2D, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight);
						{
							unsigned char* ptr = data;
							unsigned int height = h.pixelheight;
							unsigned int width = h.pixelwidth;
							glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
							for (unsigned int i = 0; i < h.miplevels; i++)
							{
								glTexSubImage2D(GL_TEXTURE_2D, i, 0, 0, width, height, h.glformat, h.gltype, ptr);
								ptr += height * calculate_stride(h, width, 1);
								height >>= 1;
								width >>= 1;
								if (!height)
									height = 1;
								if (!width)
									width = 1;
							}
						}
					}
					break;
				case GL_TEXTURE_3D:
					glTexStorage3D(GL_TEXTURE_3D, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight, h.pixeldepth);
					glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, h.pixelwidth, h.pixelheight, h.pixeldepth, h.glformat, h.gltype, data);
					break;
				case GL_TEXTURE_1D_ARRAY:
					glTexStorage2D(GL_TEXTURE_1D_ARRAY, h.miplevels, h.glinternalformat, h.pixelwidth, h.arrayelements);
					glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0, 0, 0, h.pixelwidth, h.arrayelements, h.glformat, h.gltype, data);
					break;
				case GL_TEXTURE_2D_ARRAY:
					glTexStorage3D(GL_TEXTURE_2D_ARRAY, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight, h.arrayelements);
					glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, h.pixelwidth, h.pixelheight, h.arrayelements, h.glformat, h.gltype, data);
					break;
				case GL_TEXTURE_CUBE_MAP:
					glTexStorage2D(GL_TEXTURE_CUBE_MAP, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight);
					// glTexSubImage3D(GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, h.pixelwidth, h.pixelheight, h.faces, h.glformat, h.gltype, data);
					{
						unsigned int face_size = calculate_face_size(h);
						for (unsigned int i = 0; i < h.faces; i++)
						{
							glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, h.pixelwidth, h.pixelheight, h.glformat, h.gltype, data + face_size * i);
						}
					}
					break;
				case GL_TEXTURE_CUBE_MAP_ARRAY:
					glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight, h.arrayelements);
					glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, 0, h.pixelwidth, h.pixelheight, h.faces * h.arrayelements, h.glformat, h.gltype, data);
					break;
				default:                                               // Should never happen
					goto fail_target;
				}

				if (h.miplevels == 1)
				{
					glGenerateMipmap(target);
				}

				retval = tex;

			fail_target:
				delete[] data;

			fail_header:;
			fail_read:;
				fclose(fp);

				return retval;
			}

			bool save(const char* filename, unsigned int target, unsigned int tex)
			{
				header h;

				memset(&h, 0, sizeof(h));
				memcpy(h.identifier, identifier, sizeof(identifier));
				h.endianness = 0x04030201;

				glBindTexture(target, tex);

				glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, (GLint*)&h.pixelwidth);
				glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, (GLint*)&h.pixelheight);
				glGetTexLevelParameteriv(target, 0, GL_TEXTURE_DEPTH, (GLint*)&h.pixeldepth);

				return true;
			}

		}
	}
}
