// Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>

// OpenGL Header Files
#include <GL/gl.h>
#include <GL/glu.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

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

GLUquadric* quadric = NULL;
BOOL gbLight = FALSE;

GLfloat LightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat LightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat LightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat LightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f};

GLfloat Light_Model_Ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat Light_Model_Local_Viewer[] = {0.0f};

GLfloat AngleForXRotation = 0.0f;
GLfloat AngleForYRotation = 0.0f;
GLfloat AngleForZRotation = 0.0f;

GLint KeyPressed = 0;
GLfloat currentWidth = WIN_WIDTH;
GLfloat currentHeight = WIN_HEIGHT;

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
			if (gbLight == FALSE)
			{
				glEnable(GL_LIGHTING);
				gbLight = TRUE;
			}
			else
			{
				glDisable(GL_LIGHTING);
				gbLight = FALSE;
			}
			break;

		case 'X':
		case 'x':
			KeyPressed = 1;
			AngleForXRotation = 0.0f;		// Reset
			break;

		case 'Y':
		case 'y':
			KeyPressed = 2;
			AngleForYRotation = 0.0f;		// Reset
			break;

		case 'Z':
		case 'z':
			KeyPressed = 3;
			AngleForZRotation = 0.0f;		// Reset
			break;

		default:
			KeyPressed = 0;
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
		currentWidth = LOWORD(lParam);
		currentHeight = HIWORD(lParam);
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
	// Function Declarations
	void resize(int, int);

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
	// Clear The Screen Using Gray Color
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	// Depth Related Changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_Model_Ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, Light_Model_Local_Viewer);
	glEnable(GL_LIGHT0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Create Quadric
	quadric = gluNewQuadric();			// It Internally Calls Malloc, So Dont Use Inside Display()

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

	/*if (width <= height)
	{
		glOrtho(0.0f,
			15.0f,
			(0.0f * ((GLfloat)height / (GLfloat)width)),
			(15.0f * ((GLfloat)height / (GLfloat)width)),
			-10.0f,
			10.0f);
	}
	else
	{
		glOrtho((0.0f * ((GLfloat)width / (GLfloat)height)),
			(15.0f * ((GLfloat)width / (GLfloat)height)),
			0.0f,
			15.0f,
			-10.0f,
			10.0f);
	}*/
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// Function Declaration
	void Draw24Spheres(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (KeyPressed == 1)								// X Rotation
	{
		//glRotatef(AngleForXRotation, 1.0f, 0.0f, 0.0f);
		LightPosition[1] = 4.0f * cos(AngleForXRotation * M_PI / 180.0f);
		LightPosition[2] =-3.0f + 4.0f * sin(AngleForXRotation * M_PI / 180.0f);
	}
	else if (KeyPressed == 2)							// Y Rotation
	{
		//glRotatef(AngleForYRotation, 0.0f, 1.0f, 0.0f);
		LightPosition[0] = 4.0f * cos(AngleForYRotation * M_PI / 180.0f);
		LightPosition[2] = -3.0f + 4.0f * sin(-AngleForYRotation * M_PI / 180.0f);
	}
	else if (KeyPressed == 3)							// Z Rotation
	{
		//glRotatef(AngleForZRotation, 0.0f, 0.0f, 1.0f);
		LightPosition[0] = 4.0f * cos(AngleForZRotation * M_PI / 180.0f);
		LightPosition[1] = 4.0f * sin(AngleForZRotation * M_PI / 180.0f);
	}
	else
	{
		LightPosition[0] = 0.0f;
	}
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -3.0f);

	Draw24Spheres();

	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	if (KeyPressed == 1)
	{
		AngleForXRotation += 1.5f;
		if (AngleForXRotation >= 360.0f)
			AngleForXRotation -= 360.0f;
	}
	else if (KeyPressed == 2)
	{
		AngleForYRotation += 1.5f;
		if (AngleForYRotation >= 360.0f)
			AngleForYRotation -= 360.0f;
	}
	else if (KeyPressed == 3)
	{
		AngleForZRotation += 1.5f;
		if (AngleForZRotation >= 360.0f)
			AngleForZRotation -= 360.0f;
	}
}

void Draw24Spheres(void)
{
	// Variable Declarations
	GLfloat MaterialAmbient[4];
	GLfloat MaterialDiffuse[4];
	GLfloat MaterialSpecular[4];
	GLfloat MaterialShininess;

	GLfloat viewportWidth = (GLsizei)currentWidth / 4;
	GLfloat viewportHeight = (GLsizei)currentHeight / 6;

	// Code
	// ***** 1st sphere on 1st column, emerald *****
	// ambient material
	MaterialAmbient[0] = 0.0215; // r
	MaterialAmbient[1] = 0.1745; // g
	MaterialAmbient[2] = 0.0215; // b
	MaterialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.07568; // r
	MaterialDiffuse[1] = 0.61424; // g
	MaterialDiffuse[2] = 0.07568; // b
	MaterialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.633;    // r
	MaterialSpecular[1] = 0.727811; // g
	MaterialSpecular[2] = 0.633;    // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.6 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(1.5f, 14.0f, -50.0f);
	glViewport(0, 5 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 2nd sphere on 1st column, jade *****
	// ambient material
	MaterialAmbient[0] = 0.135;  // r
	MaterialAmbient[1] = 0.2225; // g
	MaterialAmbient[2] = 0.1575; // b
	MaterialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.54; // r
	MaterialDiffuse[1] = 0.89; // g
	MaterialDiffuse[2] = 0.63; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.316228; // r
	MaterialSpecular[1] = 0.316228; // g
	MaterialSpecular[2] = 0.316228; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.1 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(1.5f, 11.5f, 0.0f);
	glViewport(0, 4 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 3rd sphere on 1st column, obsidian *****
	// ambient material
	MaterialAmbient[0] = 0.05375; // r
	MaterialAmbient[1] = 0.05;    // g
	MaterialAmbient[2] = 0.06625; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.18275; // r
	MaterialDiffuse[1] = 0.17;    // g
	MaterialDiffuse[2] = 0.22525; // b
	MaterialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.332741; // r
	MaterialSpecular[1] = 0.328634; // g
	MaterialSpecular[2] = 0.346435; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.3 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(1.5f, 9.0f, 0.0f);
	glViewport(0, 3 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 4th sphere on 1st column, pearl *****
	// ambient material
	MaterialAmbient[0] = 0.25;    // r
	MaterialAmbient[1] = 0.20725; // g
	MaterialAmbient[2] = 0.20725; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 1.0;   // r
	MaterialDiffuse[1] = 0.829; // g
	MaterialDiffuse[2] = 0.829; // b
	MaterialDiffuse[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.296648; // r
	MaterialSpecular[1] = 0.296648; // g
	MaterialSpecular[2] = 0.296648; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
	
	// shininess
	MaterialShininess = 0.088 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(1.5f, 6.5f, 0.0f);
	glViewport(0, 2 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 5th sphere on 1st column, ruby *****
	// ambient material
	MaterialAmbient[0] = 0.1745;  // r
	MaterialAmbient[1] = 0.01175; // g
	MaterialAmbient[2] = 0.01175; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.61424; // r
	MaterialDiffuse[1] = 0.04136; // g
	MaterialDiffuse[2] = 0.04136; // b
	MaterialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.727811; // r
	MaterialSpecular[1] = 0.626959; // g
	MaterialSpecular[2] = 0.626959; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.6 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(1.5f, 4.0f, 0.0f);
	glViewport(0, 1 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 6th sphere on 1st column, turquoise *****
	// ambient material
	MaterialAmbient[0] = 0.1;     // r
	MaterialAmbient[1] = 0.18725; // g
	MaterialAmbient[2] = 0.1745;  // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.396;   // r
	MaterialDiffuse[1] = 0.74151; // g
	MaterialDiffuse[2] = 0.69102; // b
	MaterialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.297254; // r
	MaterialSpecular[1] = 0.30829;  // g
	MaterialSpecular[2] = 0.306678; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.1 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(1.5f, 1.5f, 0.0f);
	glViewport(0, 0 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************
	// *******************************************************
	// *******************************************************

	// ***** 1st sphere on 2nd column, brass *****
	// ambient material
	MaterialAmbient[0] = 0.329412; // r
	MaterialAmbient[1] = 0.223529; // g
	MaterialAmbient[2] = 0.027451; // b
	MaterialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.780392; // r
	MaterialDiffuse[1] = 0.568627; // g
	MaterialDiffuse[2] = 0.113725; // b
	MaterialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.992157; // r
	MaterialSpecular[1] = 0.941176; // g
	MaterialSpecular[2] = 0.807843; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.21794872 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(7.5f, 14.0f, 0.0f);
	glViewport(1 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 2nd sphere on 2nd column, bronze *****
	// ambient material
	MaterialAmbient[0] = 0.2125; // r
	MaterialAmbient[1] = 0.1275; // g
	MaterialAmbient[2] = 0.054;  // b
	MaterialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.714;   // r
	MaterialDiffuse[1] = 0.4284;  // g
	MaterialDiffuse[2] = 0.18144; // b
	MaterialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.393548; // r
	MaterialSpecular[1] = 0.271906; // g
	MaterialSpecular[2] = 0.166721; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.2 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(7.5f, 11.5f, 0.0f);
	glViewport(1 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 3rd sphere on 2nd column, chrome *****
	// ambient material
	MaterialAmbient[0] = 0.25; // r
	MaterialAmbient[1] = 0.25; // g
	MaterialAmbient[2] = 0.25; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.4;  // r
	MaterialDiffuse[1] = 0.4;  // g
	MaterialDiffuse[2] = 0.4;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.774597; // r
	MaterialSpecular[1] = 0.774597; // g
	MaterialSpecular[2] = 0.774597; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.6 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(7.5f, 9.0f, 0.0f);
	glViewport(1 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 4th sphere on 2nd column, copper *****
	// ambient material
	MaterialAmbient[0] = 0.19125; // r
	MaterialAmbient[1] = 0.0735;  // g
	MaterialAmbient[2] = 0.0225;  // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.7038;  // r
	MaterialDiffuse[1] = 0.27048; // g
	MaterialDiffuse[2] = 0.0828;  // b
	MaterialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.256777; // r
	MaterialSpecular[1] = 0.137622; // g
	MaterialSpecular[2] = 0.086014; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.1 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(7.5f, 6.5f, 0.0f);
	glViewport(1 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 5th sphere on 2nd column, gold *****
	// ambient material
	MaterialAmbient[0] = 0.24725; // r
	MaterialAmbient[1] = 0.1995;  // g
	MaterialAmbient[2] = 0.0745;  // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.75164; // r
	MaterialDiffuse[1] = 0.60648; // g
	MaterialDiffuse[2] = 0.22648; // b
	MaterialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.628281; // r
	MaterialSpecular[1] = 0.555802; // g
	MaterialSpecular[2] = 0.366065; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.4 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(7.5f, 4.0f, 0.0f);
	glViewport(1 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 6th sphere on 2nd column, silver *****
	// ambient material
	MaterialAmbient[0] = 0.19225; // r
	MaterialAmbient[1] = 0.19225; // g
	MaterialAmbient[2] = 0.19225; // b
	MaterialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.50754; // r
	MaterialDiffuse[1] = 0.50754; // g
	MaterialDiffuse[2] = 0.50754; // b
	MaterialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.508273; // r
	MaterialSpecular[1] = 0.508273; // g
	MaterialSpecular[2] = 0.508273; // b
	MaterialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.4 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(7.5f, 1.5f, 0.0f);
	glViewport(1 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************
	// *******************************************************
	// *******************************************************

	// ***** 1st sphere on 3rd column, black *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.01; // r
	MaterialDiffuse[1] = 0.01; // g
	MaterialDiffuse[2] = 0.01; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.50; // r
	MaterialSpecular[1] = 0.50; // g
	MaterialSpecular[2] = 0.50; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(13.5f, 14.0f, 0.0f);
	glViewport(2 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 2nd sphere on 3rd column, cyan *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.1;  // g
	MaterialAmbient[2] = 0.06; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.0;        // r
	MaterialDiffuse[1] = 0.50980392; // g
	MaterialDiffuse[2] = 0.50980392; // b
	MaterialDiffuse[3] = 1.0f;       // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.50196078; // r
	MaterialSpecular[1] = 0.50196078; // g
	MaterialSpecular[2] = 0.50196078; // b
	MaterialSpecular[3] = 1.0f;       // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(13.5f, 11.5f, 0.0f);
	glViewport(2 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 3rd sphere on 2nd column, green *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.1;  // r
	MaterialDiffuse[1] = 0.35; // g
	MaterialDiffuse[2] = 0.1;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.45; // r
	MaterialSpecular[1] = 0.55; // g
	MaterialSpecular[2] = 0.45; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(13.5f, 9.0f, 0.0f);
	glViewport(2 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 4th sphere on 3rd column, red *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.0;  // g
	MaterialDiffuse[2] = 0.0;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.6;  // g
	MaterialSpecular[2] = 0.6;  // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(13.5f, 6.5f, 0.0f);
	glViewport(2 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 5th sphere on 3rd column, white *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.55; // r
	MaterialDiffuse[1] = 0.55; // g
	MaterialDiffuse[2] = 0.55; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.70; // r
	MaterialSpecular[1] = 0.70; // g
	MaterialSpecular[2] = 0.70; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(13.5f, 4.0f, 0.0f);
	glViewport(2 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 6th sphere on 3rd column, yellow plastic *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.0;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.60; // r
	MaterialSpecular[1] = 0.60; // g
	MaterialSpecular[2] = 0.50; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.25 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(13.5f, 1.5f, 0.0f);
	glViewport(2 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************
	// *******************************************************
	// *******************************************************

	// ***** 1st sphere on 4th column, black *****
	// ambient material
	MaterialAmbient[0] = 0.02; // r
	MaterialAmbient[1] = 0.02; // g
	MaterialAmbient[2] = 0.02; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.01; // r
	MaterialDiffuse[1] = 0.01; // g
	MaterialDiffuse[2] = 0.01; // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.4;  // r
	MaterialSpecular[1] = 0.4;  // g
	MaterialSpecular[2] = 0.4;  // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(19.5f, 14.0f, 0.0f);
	glViewport(3 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 2nd sphere on 4th column, cyan *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.05; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.4;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.5;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.04; // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.7;  // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(19.5f, 11.5f, 0.0f);
	glViewport(3 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 3rd sphere on 4th column, green *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.4;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.4;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.04; // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.04; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(19.5f, 9.0f, 0.0f);
	glViewport(3 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 4th sphere on 4th column, red *****
	// ambient material
	MaterialAmbient[0] = 0.05; // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.4;  // g
	MaterialDiffuse[2] = 0.4;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.04; // g
	MaterialSpecular[2] = 0.04; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(19.5f, 6.5f, 0.0f);
	glViewport(3 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 5th sphere on 4th column, white *****
	// ambient material
	MaterialAmbient[0] = 0.05; // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.05; // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.5;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.7;  // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(19.5f, 4.0f, 0.0f);
	glViewport(3 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************

	// ***** 6th sphere on 4th column, yellow rubber *****
	// ambient material
	MaterialAmbient[0] = 0.05; // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.0;  // b
	MaterialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);

	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.4;  // b
	MaterialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);

	// specular material
	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.04; // b
	MaterialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

	// shininess
	MaterialShininess = 0.078125 * 128;
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	// geometry
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(19.5f, 1.5f, 0.0f);
	glViewport(3 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
	glTranslatef(0.0f, 0.0f, -4.0f);
	gluSphere(quadric, 1.0f, 30, 30);
	// *******************************************************
	// *******************************************************
	// *******************************************************
	
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

	if (quadric)
	{
		gluDeleteQuadric(quadric);
		quadric = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log File Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
