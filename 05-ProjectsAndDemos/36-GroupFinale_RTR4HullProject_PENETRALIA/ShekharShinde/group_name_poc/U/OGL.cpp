// Header files
#include<windows.h>
#include<stdio.h> // For file I/O functions
#include<stdlib.h> // For exit()
#include"OGL.h"
#include"vmath.h"

using namespace vmath;

// OpenGL header files
#include<C:/glew/include/GL/glew.h>	// THIS MUST BE ABOVE gl.h
#include<GL/gl.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// OpenGL libraries
#pragma comment(lib, "glew32.lib")	// There is no compulsion that this should be before OpenGL32.lib. We are keeping this for parity with include files.
#pragma comment(lib, "OpenGL32.lib")

// Global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declarations
BOOL gbFullScreen = FALSE;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
FILE* gpFile = NULL;
BOOL gbActiveWindow = FALSE;

// Animation variables
GLfloat angleTriangle = 0.0f;
GLfloat angleSquare = 0.0f;

// Programmable pipeline related global variables
GLuint shaderProgramObject;
enum
{
	SAS_ATTRIBUTE_POSITION = 0,
	SAS_ATTRIBUTE_COLOR,
	SAS_ATTRIBUTE_NORMAL,
	SAS_ATTRIBUTE_TEXTURE0
};
GLuint vao_triangle;			// Vertex Array Object for triangle
GLuint vbo_triangle_position;	// Vertex Buffer Object for triangle positions
GLuint vao_square;				// Vertex Array Object for square
GLuint vbo_square_position;		// Vertex Buffer Object for square positions
GLuint mvpMatrixUniform;
mat4 perspectiveProjectionMatrix;	// mat4 is defined in vmath

// Tessellation Shader related variables
GLuint numberOfSegmentsUniform;
GLuint numberOfStripsUniform;
GLuint lineColorUniform;
unsigned int uiNumberOfSegments;

vec4 gLineColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);

// Entry-point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Local function declarations
	int initialize(void);
	void display(void);
	void update(void);
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
				update();
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

		case VK_UP:
			uiNumberOfSegments++;
			if (uiNumberOfSegments >= 30)
			{
				uiNumberOfSegments = 30;
				gLineColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
			}
			break;

		case VK_DOWN:
			uiNumberOfSegments--;
			if (uiNumberOfSegments <= 1)
			{
				uiNumberOfSegments = 1;
				gLineColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
			}
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

	// Variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

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
		"in vec2 a_position;" \
		"void main(void)" \
		"{" \
		"gl_Position = vec4(a_position, 0.0, 1.0);" \
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

	// Tessellation-Control shader
	const GLchar* tessellationControlShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"layout(vertices = 4)out;" \
		"uniform int u_numberOfSegments;" \
		"uniform int u_numberOfStrips;" \
		"void main(void)" \
		"{" \
		"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" \
		"gl_TessLevelOuter[0] = float(u_numberOfStrips);" \
		"gl_TessLevelOuter[1] = float(u_numberOfSegments);" \
		"}";

	// 02. Creating shader object
	GLuint tessellationControlShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);

	// 03. Giving shader code to shader object
	glShaderSource(
		tessellationControlShaderObject,					// Konala dyaychay shader cha source code?
		1,									// Kiti strings denar ahe? Number of shaders.
		(const GLchar**)&tessellationControlShaderSourceCode,	// Shaders cha array. Yachach count adhichya parameter madhe dila ahe apan.
		NULL								// Third parameter jar multiple strings cha array asel tar ha parameter is the array of lengths of each string in the array passed as third parameter. Ithe NULL deun apan as sangto ahot ki amchya saglya strings NULL terminated ahet.
	);

	// 04. Compile the shader
	glCompileShader(tessellationControlShaderObject);

	// 05. Error checking of shader compilation
	status;
	infoLogLength;
	log = NULL;

	// 05.a. Getting compilation status
	glGetShaderiv(
		tessellationControlShaderObject,	// Konach status pahije?
		GL_COMPILE_STATUS,					// Kashach status pahije? (Operation)
		&status								// Kashamadhe deu status?
	);
	if (status == GL_FALSE)
	{
		// 05.b. Getting length of log of compilation status
		glGetShaderiv(
			tessellationControlShaderObject,	// Konasathi?
			GL_INFO_LOG_LENGTH,					// Kay pahije?
			&infoLogLength						// Kashamadhe bharun deu?
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
					tessellationControlShaderObject,	// Kontya shader baddl chya compilation cha log pahije?
					infoLogLength,						// Kiti lambichi info pahije?
					&written,							// Kiti length actually lihili geli?
					log									// Kashamadhe log dyaychay?
				);

				// 05.e. Display the contents of buffer/log
				fprintf(gpFile, "Tessellation control shader compilation log: %s\n", log);

				fflush(gpFile);

				// 05.f. Free the allocated buffer
				free(log);

				// 05.g. Exit the application due to error
				uninitialize();
			}
		}
	}

	// Tessellation-Evaluation shader
	const GLchar* tessellationEvaluationShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"layout(isolines)in;" \
		"uniform mat4 u_mvpMatrix;" \
		"void main(void)" \
		"{" \
		"vec3 p0 = gl_in[0].gl_Position.xyz;" \
		"vec3 p1 = gl_in[1].gl_Position.xyz;" \
		"vec3 p2 = gl_in[2].gl_Position.xyz;" \
		"vec3 p3 = gl_in[3].gl_Position.xyz;" \
		"float u = gl_TessCoord.x;" \
		"vec3 p = p0 * (1 - u) * (1 - u) * (1 - u) + p1 * 3 * u * (1 - u) * (1 - u) + p2 * 3 * u * u * (1 - u) + p3 * u * u * u;" \
		"gl_Position = u_mvpMatrix * vec4(p, 1.0);" \
		"}";

	// 02. Creating shader object
	GLuint tessellationEvaluationShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);

	// 03. Giving shader code to shader object
	glShaderSource(
		tessellationEvaluationShaderObject,					// Konala dyaychay shader cha source code?
		1,									// Kiti strings denar ahe? Number of shaders.
		(const GLchar**)&tessellationEvaluationShaderSourceCode,	// Shaders cha array. Yachach count adhichya parameter madhe dila ahe apan.
		NULL								// Third parameter jar multiple strings cha array asel tar ha parameter is the array of lengths of each string in the array passed as third parameter. Ithe NULL deun apan as sangto ahot ki amchya saglya strings NULL terminated ahet.
	);

	// 04. Compile the shader
	glCompileShader(tessellationEvaluationShaderObject);

	// 05. Error checking of shader compilation
	status;
	infoLogLength;
	log = NULL;

	// 05.a. Getting compilation status
	glGetShaderiv(
		tessellationEvaluationShaderObject,	// Konach status pahije?
		GL_COMPILE_STATUS,					// Kashach status pahije? (Operation)
		&status								// Kashamadhe deu status?
	);
	if (status == GL_FALSE)
	{
		// 05.b. Getting length of log of compilation status
		glGetShaderiv(
			tessellationEvaluationShaderObject,	// Konasathi?
			GL_INFO_LOG_LENGTH,					// Kay pahije?
			&infoLogLength						// Kashamadhe bharun deu?
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
					tessellationEvaluationShaderObject,	// Kontya shader baddl chya compilation cha log pahije?
					infoLogLength,						// Kiti lambichi info pahije?
					&written,							// Kiti length actually lihili geli?
					log									// Kashamadhe log dyaychay?
				);

				// 05.e. Display the contents of buffer/log
				fprintf(gpFile, "Tessellation evaluation shader compilation log: %s\n", log);

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
		"uniform vec4 u_lineColor;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = u_lineColor;" \
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
		tessellationControlShaderObject
	);

	glAttachShader(
		shaderProgramObject,
		tessellationEvaluationShaderObject
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
	mvpMatrixUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_mvpMatrix"
	);

	numberOfSegmentsUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_numberOfSegments"
	);

	numberOfStripsUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_numberOfStrips"
	);

	lineColorUniform = glGetUniformLocation(
		shaderProgramObject,
		"u_lineColor"
	);

	// A.3. Declare vertex data arrays
	const GLfloat vertices[] =
	{
		-0.5f, 1.0f,
		-0.5f, -1.0f,
		0.5f, -1.0f,
		0.5f, 1.0f
	};

	// vao_triangle and vbo_triangle_position related code
	// TEMPLATE PROGRAM STEPS
	// A.4. Create vertex array object.
	glGenVertexArrays(
		1,
		&vao_triangle
	);

	// A.5. Bind with respective vertex array objects and start recording buffer and buffer data related steps.
	glBindVertexArray(vao_triangle);

	glGenBuffers(
		1,
		&vbo_triangle_position
	);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_position);

	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vertices),
		vertices,
		GL_STATIC_DRAW
	);

	// A.5.d. Specify where, how much, which buffer data to be considered as vertex data arrays.
	glVertexAttribPointer(
		SAS_ATTRIBUTE_POSITION,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL
	);

	// A.5.e. Enable the respective buffer binding 
	glEnableVertexAttribArray(
		SAS_ATTRIBUTE_POSITION
	);

	glBindBuffer(
		GL_ARRAY_BUFFER,
		0
	);

	glBindVertexArray(0);

	// Here starts OpenGL code
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Depth related changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	uiNumberOfSegments = 1; // Because our geometry will have at least one line.

	// A.7. Initialize needed matrices using vmath.
	perspectiveProjectionMatrix = mat4::identity();

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
	mat4 translationMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	// Triangle
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);	// This replaces glTranslatef()
	//rotationMatrix = vmath::rotate(angleTriangle, 0.0f, 1.0f, 0.0f);	// This replaces glRotatef()

	modelViewMatrix = translationMatrix * rotationMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// 
	// C.3.
	glUniformMatrix4fv(
		mvpMatrixUniform,
		1,
		GL_FALSE,
		modelViewProjectionMatrix
	);

	glUniform1i(
		numberOfSegmentsUniform,
		uiNumberOfSegments
	);

	TCHAR str[255];
	wsprintf(str, TEXT("OGL Window | Number of Segments = %d"), uiNumberOfSegments);
	SetWindowText(ghwnd, str);

	glUniform1i(
		numberOfStripsUniform, // number of strip = 1 line
		1
	);

	glUniform4fv(
		lineColorUniform,
		1,
		gLineColor
	);
	glLineWidth(5.0f);
	glPointSize(15.0f);

	// 
	// C.4.
	glBindVertexArray(vao_triangle);

	glPatchParameteri(
		GL_PATCH_VERTICES,
		4
	);

	// 
	// C.5.
	glDrawArrays(
		GL_PATCHES,
		0,
		4
	);

	// 
	// C.6.
	glBindVertexArray(0);

	//
	// C.7
	// 03. Unuse the shader program object
	glUseProgram(0);

	glPointSize(1.0f);

	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	angleTriangle = angleTriangle + 1.0f;
	if (angleTriangle >= 360.0f)
	{
		angleTriangle = 0.0f;
	}

	angleSquare = angleSquare + 1.0f;
	if (angleSquare >= 360.0f)
	{
		angleSquare = 0.0f;
	}
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
	if (vbo_square_position)
	{
		glDeleteBuffers(
			1,
			&vbo_square_position
		);
		vbo_square_position = 0;
	}

	if (vbo_triangle_position)
	{
		glDeleteBuffers(
			1,
			&vbo_triangle_position
		);
		vbo_triangle_position = 0;
	}

	//
	// D.2. Delete/uninitialize vertex array objects.
	if (vao_square)
	{
		glDeleteVertexArrays(
			1,
			&vao_square
		);
		vao_square = 0;
	}

	if (vao_triangle)
	{
		glDeleteVertexArrays(
			1,
			&vao_triangle
		);
		vao_triangle = 0;
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
