#include <windows.h>
#include <stdio.h>
#include <string>
#include <gl/glew.h>
#include <gl/GL.h>

#include "vmath.h"
#include "OGL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define X_POS_TRANSLATE -50.0f

#define TERRIAN_HEIGHT 257
#define TERRIAN_WIDTH 257

using namespace vmath;

//global variables 
FILE* gpFile = nullptr;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};
bool gbFullScreen = false;
bool gbActiveWindow = false;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
GLuint vertexShaderObject;
GLuint fragmentShaderObject;
GLuint shaderProgramObject;

GLuint vao;
GLuint vbo_position;
GLuint vbo_texcoord;

GLuint vbo_element;

GLuint heightmap_texture;

GLuint terrian_texture;

GLuint modelTransform;
GLuint viewTransform;
GLuint projectionTransform;
GLuint textureSamplerUniform;

mat4 perspectiveProjectionMatrix;

float cubePosition[10][3] = {
	{ 0.0f,  0.0f,  0.0f },
	{2.0f,  5.0f, -15.0f },
	{-1.5f, -2.2f, -2.5f },
	{-3.8f, -2.0f, -12.3f },
	{2.4f, -0.4f, -3.5f },
	{-1.7f,  3.0f, -7.5f },
	{1.3f, -2.0f, -2.5f },
	{1.5f,  2.0f, -2.5f },
	{1.5f,  0.2f, -1.5f },
	{-1.3f,  1.0f, -1.5f }
};

// ##############################################################

// terrian variables

int terrianWidth = TERRIAN_WIDTH;
int terrianHeight = TERRIAN_HEIGHT;
int vertexCount;
int indexCount;

float* verticesPosition = NULL;
float* verticesTexcoord = NULL;
float* verticesColor = NULL;
unsigned int* indices = NULL;


// ##############################################################

vec3 cameraEye = vec3(0.0f, 10.0f, 50.0f);	// position
vec3 cameraLookat = vec3(0.0f, 0.0f, -1.0f); // front or lookat
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);		// up

// function prototype
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

enum
{
	ATB_ATTRIBUTE_POSITION = 0,
	ATB_ATTRIBUTE_COLOR,
	ATB_ATTRIBUTE_NORMAL,
	ATB_ATTRIBUTE_TEXCOORD
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	// function prototype
	void Initalize(void);
	void Display(void);
	void Uninitialize(void);

	// variable declaration
	WNDCLASSEX window;
	MSG msg = { 0 };
	bool bDone = false;
	TCHAR szAppName[] = TEXT("ashwini_window");
	HWND hwnd;

	// code
	if(fopen_s(&gpFile, "ATBLog.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Failed to open ATBLog.txt"), TEXT("MYERRORMESSAGE"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Created Successfully\n");
		fflush(gpFile);
	}
	// fill window class structure
	window.cbSize = sizeof(window);
	window.cbClsExtra = 0;
	window.cbWndExtra = 0;
	window.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	window.lpfnWndProc = WinProc;
	window.lpszClassName = szAppName;
	window.lpszMenuName = NULL;
	window.hInstance = hInstance;
	window.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	window.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	window.hCursor = LoadCursor(NULL, IDC_ARROW);
	window.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register window class
	RegisterClassEx(&window);

	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,szAppName, TEXT("LearnOpenGL-hello-window"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
	if (hwnd == 0)
	{
		fprintf(gpFile, "Failed to create window\n");
		fflush(gpFile);

		Uninitialize();

		exit(0);
	}
	ghwnd = hwnd;
	Initalize();

	// show window
	ShowWindow(hwnd, cmdShow);
	
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// heart of the window
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow)
			{
				Display();
			}
		}

	}
	return ((int)(msg.wParam));
}

LRESULT CALLBACK  WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// function prototype
	void ToggleFullScreen(void);
	void Resize(int, int);
	void Uninitialize(void);

	// code
	switch (msg)
	{
		case WM_SETFOCUS:
			fprintf(gpFile, "In WM_SETFOCUS message \n");
			fflush(gpFile);
			gbActiveWindow = true;
			break;

		case WM_KILLFOCUS:
			gbActiveWindow = false;
			break;

		case WM_ERASEBKGND:
			return(0);

		case WM_SIZE:
		{
			Resize(LOWORD(lParam), HIWORD(lParam));
			break;
		}

		case WM_KEYDOWN:
		{

			switch (wParam)
			{
			case VK_ESCAPE:
				DestroyWindow(hwnd);
				break;
			default:
				break;
			}
			break;
		}

		case WM_CHAR:
		{
			const float cameraSpeed = 0.5f;
			switch (wParam)
			{
				case 'F':
				case 'f':
					ToggleFullScreen();
					break;

				case 'W':
				case 'w':
					cameraEye += cameraSpeed * cameraLookat;
					break;

				case 'S':
				case 's':
					cameraEye -= cameraSpeed * cameraLookat;
					break;

				case 'A':
				case 'a':
					cameraEye -= vmath::normalize(vmath::cross(cameraLookat, cameraUp)) * cameraSpeed;
					break;

				case 'D':
				case 'd':
					cameraEye += vmath::normalize(vmath::cross(cameraLookat, cameraUp)) * cameraSpeed;
					break;

				case 'P':
				case 'p':
					fprintf(gpFile,"camera position = [%f, %f, %f]\n", cameraEye[0], cameraEye[1], cameraEye[2]);
					break;

				default:
					break;
			}
			break;
		}
		case WM_DESTROY:
		{
			Uninitialize();
			PostQuitMessage(0);
			break;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ToggleFullScreen(void)
{
	// variable declarations
	MONITORINFO mi = { sizeof(MONITORINFO) };

	if (gbFullScreen == false)
	{
		// do fullscreen
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd,									// remove WS_OVERLAPPED from the style
					GWL_STYLE,
					(dwStyle & ~WS_OVERLAPPEDWINDOW));

				SetWindowPos(ghwnd,										// window ch handle
					HWND_TOP,											// z-order madhe top la tev hi window
					mi.rcMonitor.left,									// left
					mi.rcMonitor.top,									// top
					(mi.rcMonitor.right - mi.rcMonitor.left),			//right
					(mi.rcMonitor.bottom - mi.rcMonitor.top),			// bottom
					SWP_NOZORDER | SWP_FRAMECHANGED						// window sizing and positioning flags.  do not change z order and calculate non client area size
				);
			}
		}
		ShowCursor(false);	// hide cursor
		gbFullScreen = true;
	}
	else
	{
		// show normal window [back to normal]
		// set window style back to WS_OVERLAPPEDWINDOW
		SetWindowLong(ghwnd,
			GWL_STYLE,
			dwStyle | WS_OVERLAPPEDWINDOW);

		// set previous window placement 
		SetWindowPlacement(ghwnd, &wpPrev);

		//set window position
		SetWindowPos(ghwnd,		// window handle
			HWND_TOP,	// keep on top. it asks for z-oreer of the window to set.
			0,			// x,y,width,height alreday given in wpPrev struct
			0,			// that's why we are not mentioning it 
			0,			// over here
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		// show cursor 
		ShowCursor(true);
		gbFullScreen = false;
	}
}

// initalize function
void Initalize(void)
{
	fprintf(gpFile, " in Initalize()\n");
	fflush(gpFile);

	// function prototype
	void Resize(int,int);
	void Uninitialize(void);
	bool LoadGLTexture(GLuint * texture, TCHAR resourceID[]);
	bool LoadTextureFromFile(const char* filename, GLuint * texture);
	void InitializeTerrian(const char* filename);

	// variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// code
	ghdc = GetDC(ghwnd);

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW || PFD_SUPPORT_OPENGL || PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "ChoosePixelFormat() is failed\n");
		fflush(gpFile);
		DestroyWindow(ghwnd);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "SetPixelFormat() is failed\n");
		fflush(gpFile);
		DestroyWindow(ghwnd);
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext() is failed\n");
		fflush(gpFile);
		DestroyWindow(ghwnd);
	}
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent() is failed");
		fflush(gpFile);
		DestroyWindow(ghwnd);
	}

	//--------------------------------------------------------------

	GLenum glew_error = glewInit();									// enable extensions for Programmable Pipeline
	if (glew_error != GLEW_OK)
	{
		fprintf(gpFile, "Failed to enable extensions for Programmable Pipeline\n");
		Uninitialize();
		DestroyWindow(ghwnd);
	}

	// OpenGL related log information

	fprintf(gpFile, "OpenGL Vendor		: %s\n", glGetString(GL_VENDOR));

	fprintf(gpFile, "OpenGL Renderer    : %s\n", glGetString(GL_RENDERER));

	fprintf(gpFile, "OpenGL Version		: %s\n", glGetString(GL_VERSION));

	fprintf(gpFile, "GLSL Version		: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// OpenGL enabled extensions

	GLint numExtensions;

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	fprintf(gpFile, "OpenGL Enabled Extensions list\n");
	for (int i = 0; i < numExtensions; i++)
	{
		fprintf(gpFile, " %s\n", glGetStringi(GL_EXTENSIONS, i));
	}

	//******VERTEX SHADER********
	// 1. Create empty vertex shader
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// create source code for vertex shader
	const GLchar* vertexShaderSourceCode =
		"#version 450 core"							\
		"\n"										\
		"in vec4 a_position;"						\
		"in vec2 a_texcoord;"						\
		"uniform mat4 u_model_matrix;"				\
		"uniform mat4 u_view_matrix;"				\
		"uniform mat4 u_projection_matrix;"			\
		"out vec2 a_texcoord_out;"					\
		"void main(void)"							\
		"{"											\
			"a_texcoord_out = a_texcoord;" \
			"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * a_position;"\
		"}";

	// 2. provide source code to vertex shader
	glShaderSource(vertexShaderObject,							// to which shader we wanted to set source code
		1,											// number of shaders count
		(const GLchar**)&vertexShaderSourceCode,	// which source code we want to provide
		NULL);										// if we provide multiple shader, then this place will be used

// 3. compile vertex shader
	glCompileShader(vertexShaderObject);

	GLint infoLogLength = 0;
	GLint shaderCompiledStatus = 0;
	GLchar* szBuffer = NULL;

	glGetShaderiv(vertexShaderObject,					// which shader object info you want
		GL_COMPILE_STATUS,						// which info do you want. give shader status
		&shaderCompiledStatus);					// pass variable to get status

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject,				// which shader object info do you want
			GL_INFO_LOG_LENGTH,							// give info log length
			&infoLogLength);							// pass variable to get info log Length

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength * sizeof(GLchar));

			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject,
					infoLogLength,
					&written,
					szBuffer);

				fprintf(gpFile, "vertex shader compilation log : %s", szBuffer);

				free(szBuffer);
				szBuffer = NULL;

				DestroyWindow(ghwnd);
			}
		}
	}
	//*******FRAGMENT SHADER******
	// 1. create empty fragment shader
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// create source code for fragment shader
	const GLchar* fragmentShaderSourceCode =
		"#version 450 core"	\
		"\n"			\
		"in vec2 a_texcoord_out;"					\
		"out vec4 fragColor;"\
		"uniform sampler2D u_textureSampler;"\
		"void main(void)"	\
		"{"				\
			"fragColor = texture(u_textureSampler, a_texcoord_out);"\
		"}";

	// here the formula used by the mix() function x * (1 - a) + y * a

	// 2. provide source code to fragment shader
	glShaderSource(fragmentShaderObject,						// to which shader you want to set source code
		1,											// number of shaders
		(const GLchar**)&fragmentShaderSourceCode,	// which source code you want to provide
		NULL);										// used we use multiple shader

	// 3. comiple fragment shader
	glCompileShader(fragmentShaderObject);

	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(fragmentShaderObject,
		GL_COMPILE_STATUS,
		&shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject,
			GL_INFO_LOG_LENGTH,
			&infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength * sizeof(GLchar));

			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject,
					infoLogLength,
					&written,
					szBuffer);

				fprintf(gpFile, "fragment shader compilation log : %s", szBuffer);

				free(szBuffer);
				szBuffer = NULL;

				DestroyWindow(ghwnd);
			}
		}
	}

	// create shader program
	shaderProgramObject = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(shaderProgramObject,	// name of the shader program
		vertexShaderObject);	// name of the shader program which you wanted to attach

	glAttachShader(shaderProgramObject,
		fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject,
		ATB_ATTRIBUTE_POSITION,
		"a_position");
	glBindAttribLocation(shaderProgramObject,
		ATB_ATTRIBUTE_TEXCOORD,
		"a_texcoord");

	glLinkProgram(shaderProgramObject);

	GLint shaderProgramLinkStatus = 0;
	infoLogLength = 0;
	szBuffer = NULL;

	glGetProgramiv(shaderProgramObject,
		GL_LINK_STATUS,
		&shaderProgramLinkStatus);

	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject,
			GL_INFO_LOG_LENGTH,
			&infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength * sizeof(GLchar));

			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject,
					infoLogLength,
					&written,
					szBuffer);

				fprintf(gpFile, "shader program link log : %s", szBuffer);

				free(szBuffer);
				szBuffer = NULL;

				DestroyWindow(ghwnd);
			}
		}
	}

	modelTransform = glGetUniformLocation(shaderProgramObject, "u_model_matrix");
	viewTransform = glGetUniformLocation(shaderProgramObject, "u_view_matrix");
	projectionTransform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");
	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_textureSampler");
	//-----------------------------------------------------------------

	InitializeTerrian("heightmap.png");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), verticesPosition, GL_STATIC_DRAW);
	glVertexAttribPointer(ATB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(ATB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), verticesTexcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(ATB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATB_ATTRIBUTE_TEXCOORD);		// ethe data actually apla send hoto vertex shader la 

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//--------------------------------------------------------------

	/*bool result = LoadGLTexture(&heightmap_texture, MAKEINTRESOURCE(HEIGHTMAP_BITMAP));
	if (result == false)
	{
		fprintf(gpFile, "failed to load texture\n");
		Uninitialize();
	}*/

	bool result = LoadTextureFromFile("texture.png", &terrian_texture);
	if (result == false)
	{
		fprintf(gpFile, "failed to load texture\n");
		Uninitialize();
	}

	//--------------------------------------------------------------

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glEnable(GL_CULL_FACE);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	Resize(WIN_WIDTH, WIN_HEIGHT);

	fprintf(gpFile, " out Initalize()\n");
	fflush(gpFile);
}

void InitializeTerrian(const char* filename)
{
	fprintf(gpFile, "entry to %s\n", __FUNCTION__);
	fflush(gpFile);

	// variable declartaions
	int index, index1, index2, index3, index4, i, j, pos_index, indices_index;
	float positionX, positionZ;

	// code

	float* posData_X = (float*)malloc(terrianWidth * terrianHeight * sizeof(float));
	float* posData_Z = (float*)malloc(terrianWidth * terrianHeight * sizeof(float));

	//---------------------------------------------------------

	std::string texture_filename = std::string(filename);
	fprintf(gpFile, "texture file path : %s\n", texture_filename.c_str());

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	stbi_set_flip_vertically_on_load(false);

	fprintf(gpFile, "texture file path : %s\n", texture_filename.c_str());
	fflush(gpFile);

	
	float* heightData;
	unsigned char* data = stbi_load(texture_filename.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{

		fprintf(gpFile, "\n\nimageWidth = %d\n", width);
		fprintf(gpFile, "\n\nimageHeight = %d\n", height);

		fprintf(gpFile, "\n\nterrianWidth = %d\n", terrianWidth);
		fprintf(gpFile, "\n\nterrianHeight = %d\n\n", terrianHeight);

		fflush(gpFile);

		index = 0;
		int k = 0;
		float height;
		heightData = (float*)malloc(terrianWidth * terrianHeight * sizeof(float));
		for (int i = 0; i < terrianHeight; i++)
		{
			for (int j = 0; j < terrianWidth; j++)
			{
				index = (terrianWidth * (terrianHeight - 1 - j)) + i;

				height = (float)(data[k]);
				fprintf(gpFile, "height = %f\n", height);
				fprintf(gpFile, "index = %d\n", index);
				heightData[index] = (float)height ;
				fprintf(gpFile, "heightData = %f\n", heightData[index]);
				//index++;
				k += 3;
			}
		}

		fprintf(gpFile, "index = %d\n", terrianWidth * terrianHeight);
		fprintf(gpFile, "index = %d\n", index);
	}
	//---------------------------------------------------------

	// setup terrian cordinates 
	index = 0;
	for (int i = 0; i < terrianHeight; i++)
	{
		for (int j = 0; j < terrianWidth; j++)
		{
			index = (terrianWidth * j) + i;
			posData_X[index] = (float)i;
			posData_Z[index] = -(float)j;

			posData_Z[index] += (float)(terrianHeight - 1);

			heightData[index] /= 12.0f;
			//index++;
		}
	}


	vertexCount = (terrianWidth - 1) * (terrianHeight - 1) * 6 * 3; // here 6 is = to triangle list/strip we need 6 points
	// 3 for three vertices
	indexCount = vertexCount;

	verticesPosition = (float*)malloc(vertexCount * 3 * sizeof(float));
	verticesTexcoord = (float*)malloc(vertexCount * 2 * sizeof(float));

	indices = (unsigned int*)malloc(indexCount * 3 * sizeof(unsigned int));
	pos_index = 0;
	indices_index = 0;
	index = 0;

	for (j = 0; j < (terrianHeight - 1); j++)
	{
		for (i = 0; i < (terrianWidth - 1); i++)
		{
			// NOTE : clockwise data
			
			index1 = (terrianWidth * j) + i; // upper left
			index2 = (terrianWidth * j) + (i+1); // upper right
			index3 = (terrianWidth * (j+1)) + i; // bottom left
			index4 = (terrianWidth * (j+1)) + (i + 1); // bottom right


			// line 1 
			// upper left
			
			verticesPosition[pos_index + 0] = posData_X[index1];
			verticesPosition[pos_index + 1] = heightData[index1];
			verticesPosition[pos_index + 2] = posData_Z[index1];
			verticesTexcoord[indices_index + 0] = 0.0f;
			verticesTexcoord[indices_index + 1] = 0.0f;

			indices[index] = index;
			index++;
			pos_index += 3;
			indices_index += 2;
			// upper right

			verticesPosition[pos_index + 0] = posData_X[index2];
			verticesPosition[pos_index + 1] = heightData[index2];
			verticesPosition[pos_index + 2] = posData_Z[index2];
			verticesTexcoord[indices_index + 0] = 1.0f;
			verticesTexcoord[indices_index + 1] = 0.0f;

			indices[index] = index;
			index++;
			pos_index += 3;
			indices_index += 2;

			//------------------------------------------------------

			// line 2
			// upper right

			verticesPosition[pos_index + 0] = posData_X[index3];
			verticesPosition[pos_index + 1] = heightData[index3];
			verticesPosition[pos_index + 2] = posData_Z[index3];
			verticesTexcoord[indices_index + 0] = 0.0f;
			verticesTexcoord[indices_index + 1] = 1.0f;

			indices[index] = index;
			index++;
			pos_index += 3;
			indices_index += 2;
			//bottom right 

			verticesPosition[pos_index + 0] = posData_X[index3];
			verticesPosition[pos_index + 1] = heightData[index3];
			verticesPosition[pos_index + 2] = posData_Z[index3];
			verticesTexcoord[indices_index + 0] = 0.0f;
			verticesTexcoord[indices_index + 1] = 1.0f;

			indices[index] = index;
			index++;
			pos_index += 3;
			indices_index += 2;

			// line 3
			//bottom right 

			verticesPosition[pos_index + 0] = posData_X[index2];
			verticesPosition[pos_index + 1] = heightData[index2];
			verticesPosition[pos_index + 2] = posData_Z[index2];
			verticesTexcoord[indices_index + 0] = 1.0f;
			verticesTexcoord[indices_index + 1] = 0.0f;

			indices[index] = index;
			index++;
			pos_index += 3;
			indices_index += 2;

			//bottom left 

			verticesPosition[pos_index + 0] = posData_X[index4];
			verticesPosition[pos_index + 1] = heightData[index4];
			verticesPosition[pos_index + 2] = posData_Z[index4];
			verticesTexcoord[indices_index + 0] = 1.0f;
			verticesTexcoord[indices_index + 1] = 1.0f;

			indices[index] = index;
			index++;
			pos_index += 3;
			indices_index += 2;
		}
	}

	int id = 0;

	fprintf(gpFile, "vertex count = %d\n", vertexCount);

	/*for (int k = 0; k < vertexCount; k = k + 3)
	{
		fprintf(gpFile, "position[%d] = [%f, %f, %f]\n", id, verticesPosition[k], verticesPosition[k + 1], verticesPosition[k + 2]);
		id++;
	}*/

	

	fprintf(gpFile, "exit from %s\n", __FUNCTION__);
	fflush(gpFile);
}

// load texure using stb_image
bool LoadTextureFromFile(const char* filename, GLuint* texture)
{
	fprintf(gpFile, "Start of : %s\n", __FUNCTION__);
	fflush(gpFile);

	// variable declarations
	bool bResult = false;
	HBITMAP hBitmap = NULL;
	BITMAP bmp;

	std::string texture_filename = std::string(filename);
	fprintf(gpFile, "texture file path : %s\n", texture_filename.c_str());
	
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	stbi_set_flip_vertically_on_load(false);

	fprintf(gpFile, "texture file path : %s\n", texture_filename.c_str());
	fflush(gpFile);

	unsigned char* data = stbi_load(texture_filename.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{

		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		bResult = true;
		// Now from here onwards, OpenGL texture code starts
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		// setting of the texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	}
	else
	{
		MessageBox(NULL, TEXT("my error"), TEXT("failed to load texture"), MB_OK);
	}
	stbi_image_free(data);
	fprintf(gpFile, "End of : %s\n", __FUNCTION__);
	fflush(gpFile);
	return bResult;
}

void Resize(int width, int height)
{
	// code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
}

void Display(void)
{
	// variable declarations
	const float radius = 10.0f;
	static float cameraAngle = 0.0f;

	float camX = sin(cameraAngle) * radius;
	float camZ = cos(cameraAngle) * radius;

	//glFrontFace(GL_CW);

	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramObject);
	
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 projectionMatrix = mat4::identity();

	viewMatrix = vmath::lookat(cameraEye, cameraEye + cameraLookat , cameraUp);
	
	projectionMatrix = perspectiveProjectionMatrix;
	
	translationMatrix = mat4::identity();
	translationMatrix = vmath::translate(X_POS_TRANSLATE, -7.5f, -3.0f);

	modelMatrix = translationMatrix * scaleMatrix;// *rotationMatrix;

	glUniformMatrix4fv(modelTransform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewTransform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionTransform, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrian_texture);
	glUniform1i(textureSamplerUniform, 0);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element);
	//glDrawElements(GL_POINTS, indexCount, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_POINTS, 0, vertexCount);


	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glBindVertexArray(0);
	
	//glFrontFace(GL_CCW);
	
	glUseProgram(0);
	SwapBuffers(ghdc);

	cameraAngle = cameraAngle + 0.01f;
	if (cameraAngle > 360.0f)
		cameraAngle = 0.0f;


}

void Uninitialize(void)
{
	// if window is in full screen, get back to normal
	if (gbFullScreen)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);

		SetWindowPlacement(ghwnd, &wpPrev);

		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		ShowCursor(true);
		gbFullScreen = false;
	}

	if (vao)
	{
		glDeleteVertexArrays(1,
			&vao);
		vao = 0;
	}


	if (vbo_position)
	{
		glDeleteBuffers(1,
			&vbo_position);
		vbo_position = 0;
	}
	/*if (vbo_element)
	{
		glDeleteBuffers(1,
			&vbo_element);
		vbo_element = 0;
	}*/
	// detach shader from the Shader program object	
	if (shaderProgramObject)
	{
		// 1.  set the program again because 
		glUseProgram(shaderProgramObject);

		// 2. get the number of shader attached count to gShaderProgram
		GLsizei shaderAttachedCount;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &shaderAttachedCount);

		// 3. allocate memeory for attaches shaders
		GLuint* pShaders = NULL;
		pShaders = (GLuint*)malloc(shaderAttachedCount * sizeof(GLuint));

		if (pShaders)
		{
			fprintf(gpFile, "Log file closed and program end successfully\n");
		}
		// 4. get attached shader one by one into array
		for (GLsizei i = 0; i < shaderAttachedCount; i++)
		{
			glGetAttachedShaders(shaderProgramObject, shaderAttachedCount, &shaderAttachedCount, pShaders);
		}

		// 5. detach and delete shader
		for (GLsizei i = 0; i < shaderAttachedCount; i++)
		{
			glDetachShader(shaderProgramObject, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}

		if (pShaders)
		{
			free(pShaders);
			pShaders = NULL;
		}
	}

	// delete shader program object
	if (shaderProgramObject)
	{
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}

	// unlink shader program
	glUseProgram(0);

	if (wglGetCurrentContext() == ghrc)						// if current context is rendering context
	{
		wglMakeCurrent(NULL, NULL);							// set current context as NULL
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);								// delete rendering context
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);								// delete DC context
		ghdc = NULL;
	}
	if (gpFile)
	{
		fclose(gpFile);
		fprintf(gpFile, "Log file closed successfully\n");
		fflush(gpFile);
		gpFile = nullptr;
	}
}
