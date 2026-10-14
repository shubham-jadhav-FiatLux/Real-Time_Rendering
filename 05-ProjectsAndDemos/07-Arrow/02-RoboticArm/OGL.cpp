#pragma once
//Standard Header files
#include <windows.h>
#include <stdio.h> //for file io functions
#include <stdlib.h> // for exit() function
#include "OGL.h"
#include "Sphere.h"

//OpenGL header files
#include <GL/glew.h> //THIS MUST BE ABOVE gl.h
#include <GL/gl.h>
//#include "vgl/vgl.h"
#include "vmath.h"
using namespace vmath;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SEND_MVP_UNIFORMS(m, v, p) {\
		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, m);\
		glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, v);\
		glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, p);\
}

//OpenGL Libraries
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib,"Sphere.lib")

//global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//global variables
HWND ghwnd = NULL;
BOOL gbFullscreen = FALSE;
FILE *gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HDC ghdc = NULL;
HGLRC ghrc = NULL; //handle to global rendering context

//Programmable Pipeline related global variables
GLuint shaderProgramObject;

enum
{
	VML_ATTRIBUTE_POSITION = 0,
	VML_ATTRIBUTE_COLOR,
	VML_ATTRIBUTE_NORMAL,
	VML_ATTRIBUTE_TEXTURE0
};

//sphere
GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_color;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionMatrixUniform;
mat4 perspectiveProjectionMatrix;

GLuint gNumVertices;
GLuint gNumElements;
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

//robotic arm variables
int shoulder = 0;
int elbow = 0;
int wrist = 0;

// Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function declarations
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);

	//variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	BOOL bDone = FALSE;
	int screenHeight;
	int screenWidth;
	int iRetVal = 0;

	//code
	if(fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation of Log file failed. Exitting..."), TEXT("File I/O Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile,"Log File successfully created.\n");
	}
	//initialization of WNDCLASSEX structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// Registring above wndclass
	RegisterClassEx(&wndclass);

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Create the window
	hwnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			szAppName,
			TEXT("OpenGl Window"),
			WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
			(screenWidth/2 - WINDOW_WIDTH/2),
			(screenHeight/2 - WINDOW_HEIGHT/2),
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			NULL,
			NULL,
			hInstance,
			NULL
			);

	ghwnd = hwnd;

	//initialize
	iRetVal = initialize();
	if(iRetVal == -1)
	{
		fprintf(gpFile, "Choose Pixel Format Failed...\n\n");
		uninitialize();
	}
	else if(iRetVal == -2)
	{
		fprintf(gpFile, "Set Pixel Format Failed...\n\n");
		uninitialize();
	}
	else if(iRetVal == -3)
	{
		fprintf(gpFile, "Create OpenGLContext Failed...\n\n");
		uninitialize();	
	}
	else if(iRetVal == -4)
	{
		fprintf(gpFile, "Making OpenGL Context as Curret Context Failed...\n\n");
		uninitialize();
	}
	else if(iRetVal == -5)
	{
		fprintf(gpFile, "glewInit() failed...\n\n!");
		uninitialize();
	}
	else
	{
		fprintf(gpFile, "initialize successfull\n\n");
	}

	// Show window
	ShowWindow(hwnd, iCmdShow);

	// Foregrounding and focusing the window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//game loop
	while(bDone == FALSE)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				bDone = TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if(gbActiveWindow == TRUE)
			{
				//render the scene
				display();

				//update the scene
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
	//function declarations
	void ToggleFullscreen(void);
	void resize(int, int);

	// code
	switch(iMsg)
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
		switch(wParam)
		{
		case 'F':
		case 'f':
			ToggleFullscreen();
			break;
		case 'S':
			shoulder = (shoulder + 3) % 360;
			break;
		case 's':
			shoulder = (shoulder - 3) % 360;
			break;
		case 'E':
			elbow = (elbow + 3) % 360;
			break;
		case 'e':
			elbow = (elbow - 3) % 360;
			break;
		case 'W':
			wrist = (wrist + 3) % 360;
			break;
		case 'w':
			wrist = (wrist - 3) % 360;
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch(wParam)
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
	//variable declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;

	//code
	wp.length = sizeof(WINDOWPLACEMENT);

	if(gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE); // GWL -> GetWindowLong
		if(dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);

			if(GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, (mi.rcMonitor.right - mi.rcMonitor.left), (mi.rcMonitor.bottom - mi.rcMonitor.top), SWP_NOZORDER | SWP_FRAMECHANGED); //HWND_TOP -> WS_OVERLAPPEDWINDOW  //SWP_FRAMECHANGED -> WM_LCCALCSIZE (consider non-client area)
			}
			ShowCursor(FALSE);
			gbFullscreen = TRUE;
		}
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED); //already placed hence set to 0

		ShowCursor(TRUE);
		gbFullscreen = FALSE;
	}

	return;
}

int initialize(void)
{
	//function declarations
	void resize(int, int);
	void printGLInfo(void);
	void uninitialize(void);

	//variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	//code
	//initialization of PIXELFORMATDESCRIPTOR structure
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));// memset((void*)&pfd, NULL, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;// 24 bit also can be done

	//GetDC
	ghdc = GetDC(ghwnd);

	//choose pixel format
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if(iPixelFormatIndex == 0)
	{
		return -1;
	}

	//Set the choosen pixel format
	if(SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		return -2;
	}

	//Create OpenGL rendering context
	ghrc = wglCreateContext(ghdc); //wgl : window's graphics library -> bridging api
	if(ghrc == NULL)
	{
		return -3;
	}

	//Make the rendering context as the current context
	if(wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		return -4;
	}

	//GLEW initialization
	if(glewInit() != GLEW_OK)
	{
		return -5;
	}

	//printGLInfo();

	//Vertex Shader
	GLuint vertexShaderObject;
	{
		const GLchar* vertexShaderSourceCode =
			"#version 460 core" \
			"\n" \
			"in vec4 a_position;" \
			"in vec4 a_color;" \
			"uniform mat4 u_modelMatrix;" \
			"uniform mat4 u_viewMatrix;" \
			"uniform mat4 u_projectionMatrix;" \
			"out vec4 a_color_out;" \
			"void main(void)" \
			"{" \
			"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;" \
			"a_color_out = a_color;" \
			"}";
		vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
		
		glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
		glCompileShader(vertexShaderObject);
		
		//error checking
		GLint status;
		GLint infoLogLength;
		char* log = NULL;

		glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
		if(status == GL_FALSE)
		{
			glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
			if(infoLogLength > 0)
			{
				log = (char *)malloc(infoLogLength);
				if(log != NULL)
				{
					GLsizei written;//actual filled log size
					glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);
					fprintf(gpFile, "[Vertex Shader Compilation Log]: %s\n", log);
					free(log);
					uninitialize();
				}
			}
		}
	}

	//Fragment Shader
	GLuint fragmentShaderObject;
	{
		const GLchar* fragmentShaderSourceCode =
			"#version 460 core" \
			"\n" \
			"in vec4 a_color_out;" \
			"out vec4 FragColor;" \
			"void main(void)" \
			"{" \
			"FragColor = a_color_out;" \
			"}";

		fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
		
		glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
		glCompileShader(fragmentShaderObject);
		//error checking
		GLint status;
		GLint infoLogLength;
		char* log = NULL;

		glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
		if(status == GL_FALSE)
		{
			glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
			if(infoLogLength > 0)
			{
				log = (char *)malloc(infoLogLength);
				if(log != NULL)
				{
					GLsizei written;//actual filled log size
					glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);
					fprintf(gpFile, "[Fragment Shader Compilation Log]: %s\n", log);
					free(log);
					uninitialize();
				}
			}
		}
	}

	//Shader program object
	{
		shaderProgramObject = glCreateProgram();
		glAttachShader(shaderProgramObject, vertexShaderObject);
		glAttachShader(shaderProgramObject, fragmentShaderObject);
		glBindAttribLocation(shaderProgramObject, VML_ATTRIBUTE_POSITION, "a_position");
		glBindAttribLocation(shaderProgramObject, VML_ATTRIBUTE_COLOR, "a_color");
		glLinkProgram(shaderProgramObject);
		//error checking
		GLint status;
		GLint infoLogLength;
		char* log = NULL;
		glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
		if(status == GL_FALSE)
		{
			glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
			if(infoLogLength > 0)
			{
				log = (char *)malloc(infoLogLength);
				if(log != NULL)
				{
					GLsizei written;
					glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, log);
					fprintf(gpFile, "[Shader Program Linking Log]: %s\n", log);
					free(log);
					uninitialize();
				}
			}
		}

		modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
		viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
		projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
	}

	//Here starts OpenGL code
	//declaration of Vertex data arrays
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
    gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();
    fprintf(gpFile, "[gNumVertices] : %d\n\n", gNumVertices);

    //vao sphere
    glGenVertexArrays(1, &gVao_sphere);
    glBindVertexArray(gVao_sphere);

    // position vbo
    glGenBuffers(1, &gVbo_sphere_position);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VML_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(VML_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);    

    //color gVbo_sphere_color
    glVertexAttrib3f(VML_ATTRIBUTE_COLOR, 0.5f, 0.35f, 0.05f); //we will use it in display

	// normal vbo
    glGenBuffers(1, &gVbo_sphere_normal);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(VML_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(VML_ATTRIBUTE_NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // element vbo
    glGenBuffers(1, &gVbo_sphere_element);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // unbind vao
    glBindVertexArray(0);

	//depth & clearColor related code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	//warm-up resize call
	resize(WINDOW_WIDTH, WINDOW_HEIGHT);

	return 0;
}

void printGLInfo(void)
{
	//local variable declarations
	GLint numExtensions = 0;

	//code
	fprintf(gpFile, "OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version: %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "OpenGL GLSLVersion: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));	

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	fprintf(gpFile, "Number of Supported Extensions : %d\n\n", numExtensions);

	for(int i = 0; i < numExtensions; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}
}

void resize(int width, int height)
{
	//code
	if(height == 0)
	{
		height = 1;// To avoid divided by zero in future code
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	//functions declarations
	void vglPushMatrix(vmath::mat4 inMatrix);
	vmath::mat4 vglPopMatrix(void);

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the shader Program Object
	glUseProgram(shaderProgramObject);

	//Transformations matrices
	static mat4 translationMatrix = mat4::identity();
	static mat4 rotationMatrix = mat4::identity();
		static mat4 rotationMatrix_x = mat4::identity();
		static mat4 rotationMatrix_y = mat4::identity();
		static mat4 rotationMatrix_z = mat4::identity();
	static mat4 scaleMatrix = mat4::identity();

	//mvp matrices
	static mat4 modelMatrix = mat4::identity();
	static mat4 viewMatrix = vmath::lookat(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));//mat4::identity();
	static mat4 projectionMatrix = mat4::identity();

	//vglPushMatrix(modelMatrix);
	//Shoulder/Arm
	{
		vglPushMatrix(modelMatrix);
		//actual transformation
		translationMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
		rotationMatrix_x = vmath::rotate(0.0f, 1.0f, 0.0f, 0.0f);
		rotationMatrix_y = vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f);
		rotationMatrix_z = vmath::rotate((GLfloat)shoulder, 0.0f, 0.0f, 1.0f);
		rotationMatrix = rotationMatrix_x * rotationMatrix_y * rotationMatrix_z;
		scaleMatrix = vmath::scale(2.0f, 0.5f, 1.0f);

		modelMatrix = rotationMatrix * translationMatrix * scaleMatrix;
		projectionMatrix = perspectiveProjectionMatrix;

		vglPushMatrix(rotationMatrix * translationMatrix);//push RT of arm

		SEND_MVP_UNIFORMS(modelMatrix, viewMatrix, projectionMatrix);
		glVertexAttrib3f(VML_ATTRIBUTE_COLOR, 0.5f, 0.35f, 0.05f);//yellow

		//draw sphere
		glBindVertexArray(gVao_sphere);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	    //restore camera
	    modelMatrix = vglPopMatrix();
		//SEND_MVP_UNIFORMS(modelMatrix, viewMatrix, projectionMatrix);

		glBindVertexArray(0);
	}

	//ForeArm/elbow
	{
		//loadIdentity
		vglPushMatrix(modelMatrix);
		vglPushMatrix(modelMatrix);

		//actual transformation
		translationMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
		rotationMatrix_x = vmath::rotate(0.0f, 1.0f, 0.0f, 0.0f);
		rotationMatrix_y = vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f);
		rotationMatrix_z = vmath::rotate((GLfloat)elbow, 0.0f, 0.0f, 1.0f);
		rotationMatrix = rotationMatrix_x * rotationMatrix_y * rotationMatrix_z;
		scaleMatrix = vmath::scale(2.0f, 0.4f, 1.0f);

		modelMatrix = vglPopMatrix() * translationMatrix * rotationMatrix * translationMatrix * scaleMatrix;
		projectionMatrix = perspectiveProjectionMatrix;

		vglPushMatrix(vglPopMatrix() * translationMatrix * rotationMatrix * translationMatrix);

		SEND_MVP_UNIFORMS(modelMatrix, viewMatrix, projectionMatrix);
		glVertexAttrib3f(VML_ATTRIBUTE_COLOR, 0.5f, 0.35f, 0.05f);//blue

		//draw sphere
		glBindVertexArray(gVao_sphere);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	    modelMatrix = vglPopMatrix();
	    //SEND_MVP_UNIFORMS(modelMatrix, viewMatrix, projectionMatrix);

		glBindVertexArray(0);
	}

	//palm/wrist
	{
		//loadIdentity
		vglPushMatrix(modelMatrix);

		//actual transformation
		translationMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
		vmath::mat4 translationMatrixSecond = vmath::translate(0.5f, 0.0f, 0.0f);
		rotationMatrix_x = vmath::rotate(0.0f, 1.0f, 0.0f, 0.0f);
		rotationMatrix_y = vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f);
		rotationMatrix_z = vmath::rotate((GLfloat)wrist, 0.0f, 0.0f, 1.0f);
		rotationMatrix = rotationMatrix_x * rotationMatrix_y * rotationMatrix_z;
		scaleMatrix = vmath::scale(1.0f, 0.4f, 0.5f);

		modelMatrix = vglPopMatrix() * translationMatrix * rotationMatrix * translationMatrixSecond * scaleMatrix;
		projectionMatrix = perspectiveProjectionMatrix;

		vglPushMatrix(modelMatrix);
		/*rotationMatrix_y = vmath::rotate((GLfloat)wrist, 0.0f, 1.0f, 0.0f);
		rotationMatrix_x = vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f);
		rotationMatrix_z = vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f);
		rotationMatrix = rotationMatrix_x * rotationMatrix_y * rotationMatrix_z;
		modelMatrix = vglPopMatrix() * rotationMatrix;*/

		SEND_MVP_UNIFORMS(modelMatrix, viewMatrix, projectionMatrix);
		glVertexAttrib3f(VML_ATTRIBUTE_COLOR, 0.5f, 0.35f, 0.05f);//white

		//draw sphere
		glBindVertexArray(gVao_sphere);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		modelMatrix = vglPopMatrix();
		//SEND_MVP_UNIFORMS(modelMatrix, viewMatrix, projectionMatrix);

		glBindVertexArray(0);
	}

	//Un-use the shader program object
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	//code
}

void uninitialize(void)
{
	//function declarations
	void ToggleFullscreen(void);

	//code
	if(gbFullscreen)
	{
		ToggleFullscreen();
	}

	//Delete/uninitialize gVbo_sphere_color
	if(gVbo_sphere_color)
	{
		glDeleteBuffers(1, &gVbo_sphere_color);
		gVbo_sphere_color = 0;
	}

	//Delete/uninitialize gVbo_sphere_position
	if(gVbo_sphere_position)
	{
		glDeleteBuffers(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}

	//Delete/uninitialize gVao_sphere
	if(gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}

	//shader uninitilazation
	if(shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
		GLsizei numAttachedShaders = 0;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);
		GLuint *attachedShaderObjects = NULL;
		attachedShaderObjects = (GLuint *)malloc(numAttachedShaders * sizeof(GLuint));
		if(attachedShaderObjects != NULL)
		{
			glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, attachedShaderObjects);
			for(GLsizei i = 0; i < numAttachedShaders; i++)
			{
				glDetachShader(shaderProgramObject, attachedShaderObjects[i]);
				glDeleteShader(attachedShaderObjects[i]);
				attachedShaderObjects[i] = 0;
			}
			free(attachedShaderObjects);
			attachedShaderObjects = NULL;
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}

	if(wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if(ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if(ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if(ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	if(gpFile)
	{
		fprintf(gpFile, "Log File successfully closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}


/************************ PUSH-POP Related Code**************************************/

typedef struct __NODE
{
	//int id;
	vmath::mat4 matData;
	struct __NODE *next;
}Node;

//variables
Node *pushPopStackHead = NULL;
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
	Node *newNode = NULL;

	//code
	newNode = (Node *)malloc(sizeof(Node));
	if(newNode == NULL)
	{
		//fprintf(gpFile, "MALLOC FAILED!!!\n");
		return;
	}
	newNode->matData = inMatrix;
	newNode->next = NULL;

	if(top == -1 || pushPopStackHead == NULL)
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
	Node *temp = NULL;

	//code
	if(pushPopStackHead == NULL)
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
