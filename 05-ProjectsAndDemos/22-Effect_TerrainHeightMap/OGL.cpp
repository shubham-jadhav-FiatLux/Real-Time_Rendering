// Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>

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

// Programmable Pipeline Related Global Variables
GLuint shaderProgramObject;

enum
{
	SBJ_ATTRIBUTE_POSITION = 0,
	SBJ_ATTRIBUTE_COLOR,
	SBJ_ATTRIBUTE_NORMAL,
	SBJ_ATTRIBUTE_TEXTURE0
};

// Height Map
/* Map height updates */
#define MAX_CIRCLE_SIZE (5.0f)
#define MAX_DISPLACEMENT (1.0f)
#define DISPLACEMENT_SIGN_LIMIT (0.3f)
#define MAX_ITER (200)
#define NUM_ITER_AT_A_TIME (1)

/* Map general information */
#define MAP_SIZE (10.0f)
#define MAP_NUM_VERTICES (80)
#define MAP_NUM_TOTAL_VERTICES (MAP_NUM_VERTICES*MAP_NUM_VERTICES)
#define MAP_NUM_LINES (3* (MAP_NUM_VERTICES - 1) * (MAP_NUM_VERTICES - 1) + \
               2 * (MAP_NUM_VERTICES - 1))

static GLfloat map_vertices[3][MAP_NUM_TOTAL_VERTICES];
static GLuint  map_line_indices[2 * MAP_NUM_LINES];

GLuint vao_mesh;
GLuint vbo_mesh[4];

GLuint xUniform;
GLuint yUniform;
GLuint zUniform;

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
	else if (iRetVal == -6)
	{
		fprintf(gpFile, "LoadGLTexture() For Smiley Failed.\n");
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
	void generate_heightmap__circle(float* center_x, float* center_y, float* size, float* displacement);

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
		/*"in vec4 a_position;" \*/
		"in float x;" \
		"in float y;" \
		"in float z;" \
		"uniform mat4 u_mvpMatrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvpMatrix * vec4(x, y, z, 1.0);" \
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
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(0.2, 1.0, 0.2, 1.0);" \
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
	//glBindAttribLocation(shaderProgramObject, SBJ_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(shaderProgramObject, xUniform, "x");
	glBindAttribLocation(shaderProgramObject, yUniform, "y");
	glBindAttribLocation(shaderProgramObject, zUniform, "z");
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
	xUniform = glGetUniformLocation(shaderProgramObject, "x");
	yUniform = glGetUniformLocation(shaderProgramObject, "y");
	zUniform = glGetUniformLocation(shaderProgramObject, "z");

	// Declaration Of Vertex Data Arrays
	int i;
	int j;
	int k;
	GLfloat step = MAP_SIZE / (MAP_NUM_VERTICES - 1);
	GLfloat x = 0.0f;
	GLfloat z = 0.0f;
	/* Create a flat grid */
	k = 0;
	for (i = 0; i < MAP_NUM_VERTICES; ++i)
	{
		for (j = 0; j < MAP_NUM_VERTICES; ++j)
		{
			map_vertices[0][k] = x;
			map_vertices[1][k] = 0.0f;
			map_vertices[2][k] = z;
			z += step;
			++k;
		}
		x += step;
		z = 0.0f;
	}
#if DEBUG_ENABLED
	for (i = 0; i < MAP_NUM_TOTAL_VERTICES; ++i)
	{
		printf("Vertice %d (%f, %f, %f)\n",
			i, map_vertices[0][i], map_vertices[1][i], map_vertices[2][i]);

	}
#endif
	/* create indices */
	/* line fan based on i
	 * i+1
	 * |  / i + n + 1
	 * | /
	 * |/
	 * i --- i + n
	 */

	 /* close the top of the square */
	k = 0;
	for (i = 0; i < MAP_NUM_VERTICES - 1; ++i)
	{
		map_line_indices[k++] = (i + 1) * MAP_NUM_VERTICES - 1;
		map_line_indices[k++] = (i + 2) * MAP_NUM_VERTICES - 1;
	}
	/* close the right of the square */
	for (i = 0; i < MAP_NUM_VERTICES - 1; ++i)
	{
		map_line_indices[k++] = (MAP_NUM_VERTICES - 1) * MAP_NUM_VERTICES + i;
		map_line_indices[k++] = (MAP_NUM_VERTICES - 1) * MAP_NUM_VERTICES + i + 1;
	}

	for (i = 0; i < (MAP_NUM_VERTICES - 1); ++i)
	{
		for (j = 0; j < (MAP_NUM_VERTICES - 1); ++j)
		{
			int ref = i * (MAP_NUM_VERTICES)+j;
			map_line_indices[k++] = ref;
			map_line_indices[k++] = ref + 1;

			map_line_indices[k++] = ref;
			map_line_indices[k++] = ref + MAP_NUM_VERTICES;

			map_line_indices[k++] = ref;
			map_line_indices[k++] = ref + MAP_NUM_VERTICES + 1;
		}
	}

#ifdef DEBUG_ENABLED
	for (k = 0; k < 2 * MAP_NUM_LINES; k += 2)
	{
		int beg, end;
		beg = map_line_indices[k];
		end = map_line_indices[k + 1];
		printf("Line %d: %d -> %d (%f, %f, %f) -> (%f, %f, %f)\n",
			k / 2, beg, end,
			map_vertices[0][beg], map_vertices[1][beg], map_vertices[2][beg],
			map_vertices[0][end], map_vertices[1][end], map_vertices[2][end]);
	}
#endif

	GLuint num_iter = MAX_ITER;

	while (num_iter)
	{
		/* center of the circle */
		float center_x;
		float center_z;
		float circle_size;
		float disp;
		size_t ii;
		generate_heightmap__circle(&center_x, &center_z, &circle_size, &disp);
		disp = disp / 2.0f;
		for (ii = 0u; ii < MAP_NUM_TOTAL_VERTICES; ++ii)
		{
			GLfloat dx = center_x - map_vertices[0][ii];
			GLfloat dz = center_z - map_vertices[2][ii];
			GLfloat pd = (2.0f * sqrtf((dx * dx) + (dz * dz))) / circle_size;
			if (fabs(pd) <= 1.0f)
			{
				/* tx,tz is within the circle */
				GLfloat new_height = disp + (float)(cos(pd * 3.14f) * disp);
				map_vertices[1][ii] += new_height;
			}
		}
		--num_iter;
	}
	/*
	const GLfloat grass_blade_position[] =	
											{
												-0.3, 0.0,
												0.3, 0.0,
												-0.20, 1.0,
												0.1, 1.3,
												-0.05, 2.3,
												0.0f, 3.3
											};
											*/

	// vao And vbo Related Code
	glGenVertexArrays(1, &vao_mesh);
	glBindVertexArray(vao_mesh);

		// vbo For Position
		glGenBuffers(4, vbo_mesh);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_mesh[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * MAP_NUM_LINES * 2, map_line_indices, GL_STATIC_DRAW);

		/* Prepare the attributes for rendering */
		//attrloc = pglGetAttribLocation(program, "x");
		glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * MAP_NUM_TOTAL_VERTICES, &map_vertices[0][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(xUniform);
		glVertexAttribPointer(xUniform, 1, GL_FLOAT, GL_FALSE, 0, 0);

		//attrloc = pglGetAttribLocation(program, "z");
		glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * MAP_NUM_TOTAL_VERTICES, &map_vertices[2][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(zUniform);
		glVertexAttribPointer(zUniform, 1, GL_FLOAT, GL_FALSE, 0, 0);

		//attrloc = pglGetAttribLocation(program, "y");
		glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * MAP_NUM_TOTAL_VERTICES, &map_vertices[1][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(yUniform);
		glVertexAttribPointer(yUniform, 1, GL_FLOAT, GL_FALSE, 0, 0);
		/*
		glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh);
		glBufferData(GL_ARRAY_BUFFER, sizeof(grass_blade_position), grass_blade_position, GL_STATIC_DRAW);
		glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		*/
	glBindVertexArray(0);

	// Here Starts OpenGL Code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);							// glShadeModel(),glHint() Deprecated In PP 

	// Clear The Screen Using Black Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	// Warmup resize() Call
	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void generate_heightmap__circle(float* center_x, float* center_y,float* size, float* displacement)
{
	float sign;
	/* random value for element in between [0-1.0] */
	*center_x = (MAP_SIZE * rand()) / (1.0f * RAND_MAX);
	*center_y = (MAP_SIZE * rand()) / (1.0f * RAND_MAX);
	*size = (MAX_CIRCLE_SIZE * rand()) / (1.0f * RAND_MAX);
	sign = (1.0f * rand()) / (1.0f * RAND_MAX);
	sign = (sign < DISPLACEMENT_SIGN_LIMIT) ? -1.0f : 1.0f;
	*displacement = (sign * (MAX_DISPLACEMENT * rand())) / (1.0f * RAND_MAX);
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

	fprintf(gpFile, "Number Of Supported Extentions: %d\n\n\n", numExtentions);
	for (int i = 0; i < numExtentions; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}

	fprintf(gpFile, "\n\n\n");
}

void resize(int width, int height)
{
	// Code
	if (height == 0)
		height = 1;					// To avoid 'Divide By 0' in future

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
}

void display(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use The Shader Program Object
	glUseProgram(shaderProgramObject);

	// Transformations
	mat4 translationMatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	//translationMatrix = vmath::translate(0.0f, 0.0f, -20.0f);
	modelViewMatrix = lookat(vec3(0.0, 50.0, 50.0),vec3(0.0, 0.0, 0.0),vec3(0.0, 1.0, 0.0));							// Here Order Is Very Important
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	
	glBindVertexArray(vao_mesh);
	
	glDrawElements(GL_LINES, 2 * MAP_NUM_LINES, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);


	// Unuse The Shader Program Object
	glUseProgram(0);
	
	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
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

void uninitialize(void)
{
	// Function Declarations
	void ToggleFullscreen(void);

	// Code
	if (gbFullscreen)
	{
		ToggleFullscreen();
	}

	/*
	//  Deletion And Uninitialization of vbo
	if (vbo_grass_position)
	{
		glDeleteBuffers(1, &vbo_grass_position);
		vbo_grass_position = 0;
	}

	// Deletion And Uninitialization of vao

	if (vao_grass)
	{
		glDeleteVertexArrays(1, &vao_grass);
		vao_grass = 0;
	}
	*/
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
