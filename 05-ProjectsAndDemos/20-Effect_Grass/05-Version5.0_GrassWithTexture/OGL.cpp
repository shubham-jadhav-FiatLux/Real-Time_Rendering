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

// Programmable Pipeline Related Global Variables
GLuint shaderProgramObject;

enum
{
	SBJ_ATTRIBUTE_POSITION = 0,
	SBJ_ATTRIBUTE_COLOR,
	SBJ_ATTRIBUTE_NORMAL,
	SBJ_ATTRIBUTE_TEXTURE0,
	SBJ_ATTRIBUTE_TEXTURE1,
	SBJ_ATTRIBUTE_TEXTURE2,
	SBJ_ATTRIBUTE_TEXTURE3,
	SBJ_ATTRIBUTE_TEXTURE4
};

// Grass Related Global Variables
float time = 1.0f;
float r = 550.0f;
GLuint tex_grass_color;
GLuint tex_grass_length;
GLuint tex_grass_orientation;
GLuint tex_grass_bend;

GLuint vao_grass;
GLuint vbo_grass_position;
//GLuint vbo_texcoord;

//GLuint texture_smiley;

GLuint mvpMatrixUniform;
/*
GLuint textureSamplerUniform;
GLuint grasspalette_textureUniform;
GLuint length_textureUniform;
GLuint orientation_textureUniform;
GLuint grasscolor_textureUniform;
GLuint bend_textureUniform;
*/

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
		TEXT("OpenGL Window : Grass Using  Instancing"),
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
		"uniform mat4 u_mvpMatrix;" \
		"uniform sampler1D grasspalette_texture;" \
		"uniform sampler2D length_texture;" \
		"uniform sampler2D orientation_texture;" \
		"uniform sampler2D grasscolor_texture;" \
		"uniform sampler2D bend_texture;" \
		"out vec4 color;" \
		"int random(int seed, int iterations)" \
		"{" \
		"int value = seed;" \
		"int n;" \
		"for(n = 0; n < iterations; n++)" \
		"{" \
		"value = ((value >> 7) ^ (value << 9)) * 15485863;" \
		"}" \
		"return value;" \
		"}" \
		"vec4 random_vector(int seed)" \
		"{" \
		"int r = random(gl_InstanceID, 4);" \
		"int g = random(r, 2);" \
		"int b = random(g, 2);" \
		"int a = random(b, 2);" \
		"return vec4(float(r & 0x3FF) / 1024.0,float(g & 0x3FF) / 1024.0,float(b & 0x3FF) / 1024.0,float(a & 0x3FF) / 1024.0);" \
		"}" \
		"mat4 construct_rotation_matrix(float angle)" \
		"{" \
		"float st = sin(angle);" \
		"float ct = cos(angle);" \
		"return mat4(vec4(ct, 0.0, st, 0.0), vec4(0.0, 1.0, 0.0, 0.0), vec4(-st, 0.0, ct, 0.0), vec4(0.0, 0.0, 0.0, 1.0));" \
		"}" \
		"void main(void)" \
		"{" \
		"vec4 offset = vec4(float(gl_InstanceID >> 10) - 512.0, 0.0f, float(gl_InstanceID & 0x3FF) - 512.0, 0.0f);" \
		"int number1 = random(gl_InstanceID, 3);" \
		"int number2 = random(number1, 2);" \
		"offset += vec4(float(number1 & 0xFF) / 256.0, 0.0f, float(number2 & 0xFF) / 256.0, 0.0f);" \
		"vec2 texcoord = offset.xz / 1024.0 + vec2(0.5);" \
		"float bend_factor = texture(bend_texture, texcoord).r * 2.0;" \
		"float bend_amount = cos(a_position.y);" \
		"float angle = texture(orientation_texture, texcoord).r * 2.0 * 3.141592;" \
		"mat4 rot = construct_rotation_matrix(angle);" \
		"vec4 position = (rot * (a_position + vec4(0.0, 0.0, bend_amount * bend_factor, 0.0))) + offset;" \
		"position *= vec4(1.0, texture(length_texture, texcoord).r * 0.9 + 0.3, 1.0, 1.0);" \
		"gl_Position = u_mvpMatrix * position;" \
		"color = texture(grasspalette_texture, texture(grasscolor_texture, texcoord).r) + vec4(random_vector(gl_InstanceID).xyz * vec3(0.1, 0.5, 0.1), 1.0);" \
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
				fprintf(gpFile, "Vertex Shader Compilation Log: %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}

	// Fragment Shader
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = color;" \
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
	//glBindAttribLocation(shaderProgramObject, SBJ_ATTRIBUTE_TEXTURE0, "a_texcoord");
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
	/*
	grasspalette_textureUniform = glGetuniformLocation(shaderProgramObject, "u_grasspalette_texture");
	length_textureUniform = glGetUniformLocation(shaderProgramObject, "u_length_texture");
	orientation_textureUniform = glGetUniformLocation(shaderProgramObject, "u_orientation_texture");
	grasscolor_textureUniform = glGetUniformLocation(shaderProgramObject, "u_grasscolor_texture");
	bend_textureUniform = glGetUniformLocation(shaderProgramObject, "u_bend_texture");
	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_textureSampler");
	*/

	// Declaration Of Vertex Data Arrays
	const GLfloat grass_blade_position[] =	
											{
												-0.3, 0.0,
												0.3, 0.0,
												-0.20, 1.0,
												0.1, 1.3,
												-0.05, 2.3,
												0.0f, 3.3
											};

	// vao And vbo Related Code
	glGenVertexArrays(1, &vao_grass);
	glBindVertexArray(vao_grass);

		// vbo For Position
		glGenBuffers(1, &vbo_grass_position);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_grass_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(grass_blade_position), grass_blade_position, GL_STATIC_DRAW);
		glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//glVertexAttrib3f();

		/*
		// vbo For Texcoord
		glGenBuffers(1, &vbo_texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_STATIC_DRAW);
		glVertexAttribPointer(SBJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SBJ_ATTRIBUTE_TEXTURE0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		*/

	glBindVertexArray(0);

	// Here Starts OpenGL Code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);							// glShadeModel(),glHint() Deprecated In PP 

	// Clear The Screen Using Black Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//if (LoadGLTexture(&texture_smiley, MAKEINTRESOURCE(IDBITMAP_SMILEY)) == FALSE)
		//return -6;

	// Enabling The Texture
	//glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	tex_grass_length = sb7::ktx::file::load("textures/grass_length.ktx");

	glActiveTexture(GL_TEXTURE2);
	tex_grass_orientation = sb7::ktx::file::load("textures/grass_orientation.ktx");

	glActiveTexture(GL_TEXTURE3);
	tex_grass_color = sb7::ktx::file::load("textures/grass_color.ktx");

	glActiveTexture(GL_TEXTURE4);
	tex_grass_bend = sb7::ktx::file::load("textures/grass_bend.ktx");

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

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
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

	//translationMatrix = vmath::translate(0.0f, 0.0f, -200.0f);
	//modelMatrix = translationMatrix ;							// Here Order Is Very Important
	viewMatrix = lookat(vmath::vec3(sinf(time) * r, 25.0f, cosf(time) * r),
						vmath::vec3(0.0f, -50.0f, 0.0f),
						vmath::vec3(0.0, 1.0, 0.0));
	modelViewProjectionMatrix = perspectiveProjectionMatrix * viewMatrix;
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture_smiley);
	//glUniform1i(textureSamplerUniform, 0);

	glBindVertexArray(vao_grass);
	
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, 1024 * 1024);

	glBindVertexArray(0);

	//glBindTexture(GL_TEXTURE_2D, 0);

	// Unuse The Shader Program Object
	glUseProgram(0);
	
	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	time += 0.05f;
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
	if (texture_smiley)
	{
		glDeleteTextures(1, &texture_smiley);
		texture_smiley = 0;
	}
	*/
	//  Deletion And Uninitialization of vbo
	/*
	if (vbo_texcoord)
	{
		glDeleteBuffers(1, &vbo_texcoord);
		vbo_texcoord = 0;
	}
	*/
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