// Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES 1
#include <math.h>

// OpenGL Header Files
#include <GL/gl.h>
#include <GL/glu.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define I 4
#define J 4
#define K 3

//Accepted by the <property> parameter of NurbsProperty :

#define GLU_NURBS_MODE_EXT             100160

//Accepted by the <value> parameter of NurbsProperty :

#define GLU_NURBS_TESSELLATOR_EXT      100161
#define GLU_NURBS_RENDERER_EXT         100162


//Accepted by the <which> parameter of NurbsCallback :

#define GLU_NURBS_BEGIN_EXT	               100164
#define GLU_NURBS_VERTEX_EXT                   100165
#define GLU_NURBS_NORMAL_EXT                   100166
#define GLU_NURBS_END_EXT                      100169

#define GLU_NURBS_BEGIN_DATA_EXT	       100170
#define GLU_NURBS_VERTEX_DATA_EXT              100171
#define GLU_NURBS_NORMAL_DATA_EXT              100172
#define GLU_NURBS_COLOR_DATA_EXT               100173
#define GLU_NURBS_TEXTURE_COORD_DATA_EXT       100174
#define GLU_NURBS_END_DATA_EXT                 100175

// OpenGL Libraries
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable Declarations
FILE* gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
static BOOL gbFullscreen = FALSE;

GLfloat controlPoints[I][J][K];
GLint showPoints = 0;
const GLfloat speed = 0.5f;
GLfloat angleRotateScene = 0.0f;
GLfloat angle = 0.0f;

GLUnurbsObj* nurbs = NULL;

GLfloat materialDiffuse[] = {0.0f, 1.5f, 0.0f, 1.0f};
GLfloat materialSpecular[] = {10.0f, 10.0f, 100.0f, 1.0f};
GLfloat materialShininess = 50.0f;

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
		(GetSystemMetrics(SM_CXSCREEN) - WIN_WIDTH )/2,
		(GetSystemMetrics(SM_CYSCREEN) - WIN_HEIGHT)/2,
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

		case 'c':
		case 'C':
			showPoints = !showPoints;
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


void CALLBACK beginCallback(GLenum whichType)
{
	glBegin(whichType);
	fprintf(gpFile, "glBegin(");
	switch (whichType)
	{
	case GL_LINES:
		
		break;


	default:
		break;
	}



}

int initialize(void)
{
	// Function Declarations
	void resize(int, int);
	void initSurface(void);

	// Variable Declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	// Code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

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
	pfd.cDepthBits = 32;			// 24 also can be used instead of 32.

	// GetDC
	ghdc = GetDC(ghwnd);

	// Chose Pixel Format
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

	// Here Starts OpenGL Code
	// Clear The Screen Using Black Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Depth Related Changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	initSurface();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Create NURBS Object
	nurbs = gluNewNurbsRenderer();			
	gluNurbsProperty(nurbs, GLU_NURBS_MODE_EXT, GLU_NURBS_TESSELLATOR_EXT);
	gluNurbsProperty(nurbs, GLU_SAMPLING_TOLERANCE, 25.0f);
	gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, GLU_FILL);
	gluNurbsProperty(nurbs, GLU_CULLING, GLU_TRUE);
	gluNurbsCallback(nurbs, GLU_NURBS_BEGIN_EXT, beginCallback);
	gluNurbsCallback(nurbs, GLU_NURBS_VERTEX_EXT, vertexCallback);
	gluNurbsCallback(nurbs, GLU_NURBS_NORMAL_EXT, normalCallback);
	gluNurbsCallback(nurbs, GLU_NURBS_END_EXT, endCallback);

	// Warmup resize() Call
	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}




void resize(int width, int height)
{
	// Code
	if (height == 0)
		height = 1;					// To avoid 'Divide By 0' in future

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// Function Declarations
	void initSurface(void);

	// Local Variable Declarations
	GLfloat knots[8] = {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -5.0f);

	glRotatef(angleRotateScene, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
		
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glScalef(0.2f, 0.2f, 0.2f);
		gluBeginSurface(nurbs);
		gluNurbsSurface(nurbs,
			8, knots, 8, knots,
			4 * 3, 3, &controlPoints[0][0][0],
			4, 4, GL_MAP2_NORMAL);
		
		gluNurbsSurface(nurbs,
			8, knots, 8, knots,
			4 * 3, 3, &controlPoints[0][0][0],
			4, 4, GL_MAP2_VERTEX_3);
		gluEndSurface(nurbs);

		if (showPoints)
		{
			glPointSize(5.0f);
			glDisable(GL_LIGHTING);
			glColor3f(1.0f, 1.0f, 0.0f);
			glBegin(GL_POINTS);

				for (GLint i = 0; i < I; i++)
				{
					for (GLint j = 0; j < J; j++)
					{
						
						glVertex3f(controlPoints[i][j][0], controlPoints[i][j][1], controlPoints[i][j][2]);
						
					}
				}
			glEnd();
			glEnable(GL_LIGHTING);
		}
		
	glPopMatrix();
	
	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	angleRotateScene += speed;
	angle += 5.0f;
}

void initSurface(void)
{
	// Local Variable Declarations
	GLint u, v;

	// Code
	for (u = 0; u < I; u++)
	{
		for (v = 0; v < J; v++)
		{
			controlPoints[u][v][0] = 2.0f *((GLfloat)u - 1.5f);
			controlPoints[u][v][1] = 2.0f *((GLfloat)v - 1.5f);

			if ((u == 1 || u == 2) && (v == 1 || v == 2))
				controlPoints[u][v][2] = 3.0f; 
			else
				controlPoints[u][v][2] = 0.0f; 
			
		}
	}
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

	if (nurbs)
	{
		gluDeleteNurbsRenderer(nurbs);
		nurbs = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log File Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
