// header files
#include <windows.h>
#include <stdio.h>		// for File I/O function
#include <stdlib.h>		// for exit()
#include "OGL.h"
#include <mmsystem.h>
#include <math.h>

// OpenGL Header Files
#include <GL/gl.h>
#include <GL/glu.h>

// OpenGL Liabraries
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")
	
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define PI 3.14

#define TRANSLATE_Z -20

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;					// Handle to OpenGL Rendering context
BOOL gbFullScreen = FALSE;
BOOL gbActiveWindow = FALSE;
FILE *gpFile = NULL;
GLUquadric* quadric = NULL;

void I();
void I1();
void N();
void D();
void A();
void WhitePlane();
void OrangePlane();
void GreenPlane();
void MiddleLineofA();
void IonPlane();
void AonPlane();
void FonPlane();
void IAF();

float moveIx = -6.5f;
float moveI2y = -4.0f;
float moveNy = 4.0f;
float moveA = 6.5f;

float moveWhitePlane = -18.0f;
float moveOrangePlane = -9.0f;
float moveGreenPlane = -9.0;
int ThreeLines = 0;


float DColor1 = 0.0f;
float DColor2 = 0.0f;

int i=0;

float planeangle = 90.0f;
float planepath = 0.0f;

float OrangePlaneX = 0.0f;
float OrangePlaneY = 0.0f;
float WhitePlaneX = 0.0f;
float GreenPlaneX = 0.0f;
float GreenPlaneY = 0.0f;

// entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// fuction declarations
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);

	// variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("First Windows SDK Assignment");
	BOOL bDone = FALSE;
	int screenWidth;
	int screenHeight;
	int iRetVal = 0;

	// code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation for log file failed. Exiting..."), TEXT("File I/O Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File is Successfully Created. \n");
	}
	// Initialization of WNDCLASSEX structure
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

	// get the screen width and height
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Registering above WNDCLASSEX 
	RegisterClassEx(&wndclass);

	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("OGL Window- Vipin Vijay Kodag"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(screenWidth/2)- (WINDOW_WIDTH/2),
		(screenHeight/2)-(WINDOW_HEIGHT/2),
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	// initialize
	iRetVal = initialize();
	if (iRetVal == -1)
	{
		fprintf(gpFile, "Choose Pixel format Failed. \n");
		uninitialize();
	}
	else if (iRetVal == -2)
	{
		fprintf(gpFile, "Set Pixel format Failed. \n");
		uninitialize();
	}
	else if (iRetVal == -3)
	{
		fprintf(gpFile, "Create OpenGL context Failed. \n");
		uninitialize();
	}
	else if (iRetVal == -4)
	{
		fprintf(gpFile, "Making OpenGL context as current context Failed. \n");
		uninitialize();
	}
	else
	{
		fprintf(gpFile, "Choose Pixel format created successfully. \n");
		
	}
	
	// show window
	ShowWindow(hwnd, SW_SHOW);

	// forgounding and focusing the window
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
				// Render the scene
				display();

				// update the scene
				update();
			}
		}
	}
	uninitialize();
	return((int)msg.wParam);
}

// callback function 
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declaration
	void ToggleFullScreen(void);
	void resize(int, int);
	
	// code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;
	case WM_ERASEBKGND:
		return (0);
	case WM_CHAR:
		switch (wParam)
		{
		case 'F':
		case 'f':
			ToggleFullScreen();
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
		resize(LOWORD(lParam),HIWORD(lParam));
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

void ToggleFullScreen(void)
{
	// variable declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;

	// code
	wp.length = sizeof(WINDOWPLACEMENT);

	if (gbFullScreen == FALSE)
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
			gbFullScreen = TRUE;
		}
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullScreen = FALSE;
	}
}

int initialize(void)
{
	// function declarations
	void resize(int, int);
	
	// variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	// code

	//PlaySound(MAKEINTRESOURCE(MYSOUND), NULL, SND_RESOURCE | SND_ASYNC);

	// intialization of PIXELFORMATDESCRIPTOR structure
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));		// memset((void*)&pfd, NULL, sizeof(PIXELFORMATDESCRIPTOR))
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);				// size (n-short integer)
	pfd.nVersion = 1;										// by convension nVersion =1.5, version of OpenGL	
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;      // information of behavier or changes of behavier 
	pfd.iPixelType = PFD_TYPE_RGBA;					// color in Red, Green , Blue, Alpha.
	pfd.cColorBits = 32;							// color 16/32 bits, count of color bits
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	// getDC
	ghdc = GetDC(ghwnd);								// GetDC() is neutral function to get DC, 

	// choose pixel format
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		return(-1);
	}

	// set the choosen pixel format
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		return(-2);
	}

	// create OpenGL rendering context
	ghrc= wglCreateContext(ghdc);						// windows graphics liabrary

	if (ghrc == NULL)
	{
		return(-3);
	}
	
	// make the rendering context to current context
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
		return(-4);

	// Here starts OpenGL code
	// Clear the screen using Black Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// worm-up resize call
	resize(WINDOW_WIDTH, WINDOW_HEIGHT);
	return(0);
}

void resize(int width, int height)
{
	// code
	if (height == 0)		// to avoid divide by zero in future code
		height = 1;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, TRANSLATE_Z);
	glTranslatef(moveIx, 0.0f, 0.0f);
	I();

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, TRANSLATE_Z);
	glTranslatef(-1.25f, moveNy, 0.0f);
	N();

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, TRANSLATE_Z);
	//glTranslatef(1.1f, 0.0f, 0.0f);
	D();

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, TRANSLATE_Z);
	glTranslatef(1.25f, moveI2y, 0.0f);
	I1();

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, TRANSLATE_Z);
	glTranslatef(moveA, 0.0f, 0.0f);
	A();

	glLoadIdentity();
	glTranslatef(-1.0f, 2.0f, TRANSLATE_Z);
	glTranslatef(-5.5f, 6.0f, 0.0f);
	glRotatef(OrangePlaneX, 0.0f, 0.0f, 1.0f);
	glTranslatef((((cos(2 * PI * planeangle) / 180) * 0.2) - 8.0f), (((sin(2 * PI * planeangle) / 180) * 1.0f) + 2.5f), 0.0f);
	glRotatef(270, 0.0f, 0.0f, 1.0f);
	if (OrangePlaneX >= 90.0f)
	{
		glTranslatef(planepath, 0.0f, 0.0f);
		if (planepath >= 15)
		{
			glTranslatef(0.0f, 7.0f, 0.0f);
			glRotatef(OrangePlaneY, 0.0f, 0.0f, 1.0f);
			glTranslatef(((cos(2 * PI * planeangle / 360) * 0.5f) + 0.0f), ((-sin(2 * PI * planeangle / 360) * 6.6f) - 0.1f), 0.0f);
			glRotatef(360, 0.0f, 0.0f, 1.0f);
			fprintf(gpFile, "OrangePlanePath = %f \n OrangePlaneX = %f \n OrangePlaneY = %f \n", planepath, OrangePlaneX, OrangePlaneY);
		}
	}
	OrangePlane();

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, TRANSLATE_Z);
	glTranslatef(-5.5f, -6.0f, 0.0f);
	glRotatef(GreenPlaneX, 0.0f, 0.0f, 1.0f);
	glTranslatef((((cos(2 * PI * planeangle) / 180) * 0.2) - 6.0f), (((sin(2 * PI * planeangle) / 180) * 1.0f) - 3.5f), 0.0f);
	glRotatef(-270, 0.0f, 0.0f, 1.0f);
	fprintf(gpFile, "GreenPlanePath = %f \n GreenPlaneX = %f \n GreenPlaneY = %f \n", planepath, GreenPlaneX, GreenPlaneY);
	fprintf(gpFile, "--------------------------------------------------------------------------- \n");
	if (GreenPlaneX <= -90.0f)
	{

		glTranslatef(planepath, 0.0f, 0.0f);
		if (planepath >= 15)
		{
			glTranslatef(0.0f, -7.0f, 0.0f);
			glRotatef(-OrangePlaneY, 0.0f, 0.0f, 1.0f);
			glTranslatef(((cos(2 * PI * planeangle / 360) * 0.5f) + 0.0f), ((sin(2 * PI * planeangle / 360) * 6.6f) - 0.1f), 0.0f);
			glRotatef(-360, 0.0f, 0.0f, 1.0f);
			fprintf(gpFile, "OrangePlanePath = %f \n OrangePlaneX = %f \n OrangePlaneY = %f \n", planepath, OrangePlaneX, OrangePlaneY);
		}
		/*
		glTranslatef(planepath, 0.0f, 0.0f);
		if (planepath >= 15)
		{
			glTranslatef(0.0f, 7.0f, 0.0f);
			glRotatef(-GreenPlaneY, 0.0f, 0.0f, 1.0f);
			glTranslatef( ((sin(2 * PI * planeangle / 360) * 6.6f) - 0.1f), ((-cos(2 * PI * planeangle / 360) * 0.2f) + 0.0f), 0.0f);
			glRotatef(360, 0.0f, 0.0f, 1.0f);
			//fprintf(gpFile, "GreenPlanePath = %f \n GreenPlaneX = %f \n GreenPlaneY = %f \n", planepath, GreenPlaneX, GreenPlaneY);
			//fprintf(gpFile, "--------------------------------------------------------------------------- \n");
		}
		*/
	}
	GreenPlane();

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, TRANSLATE_Z);
	glTranslatef(moveWhitePlane, 0.0f, 0.0f);
	WhitePlane();
	
	if (ThreeLines == 1)
	{
		glLoadIdentity();
		glTranslatef(2.5f, 0.0f, TRANSLATE_Z);
		//glTranslatef(moveWhitePlane, 0.0f, 0.0f);
		MiddleLineofA();
	}

	SwapBuffers(ghdc);

}

void update(void)
{
	// code
	// Transformation of I
	if (moveIx <= -2.5f)
	{
		moveIx = moveIx + 0.0002f;
	}

	// Transformation of N
	if (moveNy >= 0.0f)
	{
		moveNy = moveNy - 0.0002;
	}

	// Tranformation of Second I
	if (moveI2y <= 0.0f)
	{
		moveI2y = moveI2y + 0.0002;
	}

	// Transformation of A
	if (moveA >= 2.5f)
	{
		moveA = moveA - 0.0002;
	}
	else
	{
		i = 1;
	}

	// D Fade In
	if (DColor1 <= 1.0f)
	{
		DColor1 = DColor1 + 0.0001f;
	}

	if (DColor2<=0.5f)
	{
		DColor2 = DColor2 + 0.0001f;
	}

	/*// move WhitePlane
	if (i == 1)
	{
		if (moveWhitePlane <= 9.0f)
		{
			moveWhitePlane = moveWhitePlane + 0.001;
			if (moveWhitePlane >= 4.0f)
			{
				ThreeLines = 1;
			}
		}
	}*/

	// move OrangePlane
	if (moveNy <= 0.0)
	{
		if (WhitePlaneX < -8.0f)
		{
			WhitePlaneX = WhitePlaneX + 0.001;
		}
		else if (WhitePlaneX < 15.0f)
		{
			WhitePlaneX = WhitePlaneX + 0.001;
		}

		if (OrangePlaneX <= 90)
		{
			OrangePlaneX = OrangePlaneX + 0.01;
		}

		if (planepath >= 15.0)
		{
			if (OrangePlaneY <= 180.0f)
			{
				OrangePlaneY = OrangePlaneY + 0.01;
			}
			
		}

		if (GreenPlaneX >= -90)
		{
			GreenPlaneX = GreenPlaneX - 0.01;
		}

		/*if (GreenPlaneY <= -91.0f)
		{
			GreenPlaneY = GreenPlaneY + 0.01;
		}*/

		if (planepath >= 15.0)
		{
			if (GreenPlaneY >= -180.0f)
			{
				GreenPlaneY = GreenPlaneY - 0.01;
			}

		}

		if (planepath <= 15.0f && OrangePlaneX > 90.0f || GreenPlaneX > 90.0f)
		//if (planepath <= 15.0f && GreenPlaneX > 90.0f)
		{
			planepath = planepath + 0.001;
			/*if (GreenPlaneY <= -180.0f)
			{
				GreenPlaneY = GreenPlaneY - 0.01;
			}*/
		}
			
		// move WhitePlane
		if (i == 1)
		{
			if (moveWhitePlane <= 20.0f)
			{
				moveWhitePlane = moveWhitePlane + 0.001;
				if (moveWhitePlane >= 4.0f)
				{
					ThreeLines = 1;
				}
			}
		}
	}
	
	// move GreenPlane
	/*if (i == 1)
	{
		if (moveGreenPlane <= 11.0f)
			moveGreenPlane = moveGreenPlane + 0.001;
	}*/

}

void WhitePlane()
{
	
	glBegin(GL_TRIANGLES);
	glColor3f(0.72f, 0.88f, 0.93f);
	glVertex3f(-0.5f, 0.7f, 0.0f);
	glVertex3f(-0.5f, -0.7f, 0.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.72f, 0.88f, 0.93f);
	glVertex3f(0.9f, 0.0f, 0.0f);
	glVertex3f(-0.9f, 0.3f, 0.0f);
	glVertex3f(-0.9f, -0.3f, 0.0f);
	glVertex3f(0.9f, -0.0f, 0.0f);
	glEnd();

	// tail
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.9f, 0.1f, 0.0f);
	glVertex3f(-1.5f, 0.1f, 0.0f);
	glVertex3f(-1.5f, -0.1f, 0.0f);
	glVertex3f(-0.9f, -0.1f, 0.0f);
	glEnd();

	IAF();

}

void OrangePlane()
{
	glBegin(GL_TRIANGLES);
	glColor3f(0.72f, 0.88f, 0.93f);
	glVertex3f(-0.5f, 0.7f, 0.0f);
	glVertex3f(-0.5f, -0.7f, 0.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.72f, 0.88f, 0.93f);
	glVertex3f(0.9f, 0.0f, 0.0f);
	glVertex3f(-0.9f, 0.3f, 0.0f);
	glVertex3f(-0.9f, -0.3f, 0.0f);
	glVertex3f(0.9f, -0.0f, 0.0f);
	glEnd();

	// tail
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(-0.9f, 0.1f, 0.0f);
	glVertex3f(-1.5f, 0.1f, 0.0f);
	glVertex3f(-1.5f, -0.1f, 0.0f);
	glVertex3f(-0.9f, -0.1f, 0.0f);
	glEnd();

	IAF();
}

void GreenPlane()
{
		glBegin(GL_TRIANGLES);
		glColor3f(0.72f, 0.88f, 0.93f);
		glVertex3f(-0.5f, 0.7f, 0.0f);
		glVertex3f(-0.5f, -0.7f, 0.0f);
		glVertex3f(0.5f, 0.0f, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(0.72f, 0.88f, 0.93f);
		glVertex3f(0.9f, 0.0f, 0.0f);
		glVertex3f(-0.9f, 0.3f, 0.0f);
		glVertex3f(-0.9f, -0.3f, 0.0f);
		glVertex3f(0.9f, -0.0f, 0.0f);
		glEnd();

		// tail
		glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.9f, 0.1f, 0.0f);
		glVertex3f(-1.5f, 0.1f, 0.0f);
		glVertex3f(-1.5f, -0.1f, 0.0f);
		glVertex3f(-0.9f, -0.1f, 0.0f);
		glEnd();

		IAF();
}

void IonPlane()
{
	// I
	//   -
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glVertex3f(-0.5f, 0.3f, 0.0f);
	glVertex3f(0.5f, 0.3f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glEnd();

	// |
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.1f, 0.3f, 0.0f);
	glVertex3f(-0.1f, -0.3f, 0.0f);
	glVertex3f(0.1f, -0.3f, 0.0f);
	glVertex3f(0.1f, 0.3f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.1f, 0.0f, 0.0f);
	glVertex3f(-0.1f, -0.3f, 0.0f);
	glVertex3f(0.1f, -0.3f, 0.0f);
	glVertex3f(0.1f, 0.0f, 0.0f);
	glEnd();

	//	-
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, -0.3f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.3f, 0.0f);
	glEnd();
}

void AonPlane()
{
	//   -
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	//glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, 0.3f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.3f, 0.0f);
	//glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glEnd();

	// |
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, 0.3f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.3f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.3f, 0.3f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, 0.0f, 0.0f);
	//glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	//glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.3f, -0.5f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.3f, 0.0f, 0.0f);
	glEnd();


	// |
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.3f, 0.3f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.3f, 0.0f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.3f, 0.0f);
	glEnd();

	// |
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.3f, 0.0f, 0.0f);
	//glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.3f, -0.5f, 0.0f);
	//glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glEnd();

	// middle line of A
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.3f, 0.1f, 0.0f);
	glVertex3f(-0.3f, -0.1f, 0.0f);
	glVertex3f(0.3f, -0.1f, 0.0f);
	glVertex3f(0.3f, 0.1f, 0.0f);
	glEnd();
}

void FonPlane()
{
	// Left side of F
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(-0.3f, -0.5f, 0.0f);
	glVertex3f(-0.3f, 0.5f, 0.0f);
	glEnd();

	// Top side of F
	glBegin(GL_QUADS);
	glVertex3f(-0.3f, 0.5f, 0.0f);
	glVertex3f(-0.3f, 0.3f, 0.0f);
	glVertex3f(-0.3f, 0.3f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glEnd();

	// Middle side of F
	glBegin(GL_QUADS);
	glVertex3f(-0.3f, 0.1f, 0.0f);
	glVertex3f(-0.3f, -0.1f, 0.0f);
	glVertex3f(0.2f, -0.1f, 0.0f);
	glVertex3f(0.2f, 0.1f, 0.0f);
	glEnd();
}

void IAF()
{
	glTranslatef(-0.5f, 0.0f, 0.0f);
	glScalef(0.15f, 0.15f, 0.0f);
	IonPlane();
	glTranslatef(1.25f, 0.0f, 0.0f);
	AonPlane();
	glTranslatef(1.25f, 0.0f, 0.0f);
	FonPlane();
}

void I()
{
	// I
	//   -
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, 0.3f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.3f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glEnd();

	// |
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.1f, 0.3f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.1f, -0.3f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.1f, -0.3f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.1f, 0.3f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.1f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.1f, -0.3f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.1f, -0.3f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.1f, 0.0f, 0.0f);
	glEnd();

	//	-
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, -0.3f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, -0.3f, 0.0f);
	glEnd();
}

void I1()
{
		// I
		//   -
		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.5f, 0.0f);
		glVertex3f(-0.5f, 0.5f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.5f, 0.3f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0.3f, 0.0f);
		glColor3f(1.0f, 0.5f, 0.0f);
		glVertex3f(0.5f, 0.5f, 0.0f);
		glEnd();

		// |
		glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.1f, 0.3f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.1f, -0.3f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.1f, -0.3f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.1f, 0.3f, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.1f, 0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.1f, -0.3f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.1f, -0.3f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.1f, 0.0f, 0.0f);
		glEnd();

		//	-
		glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.5f, -0.3f, 0.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.5f, -0.5f, 0.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.5f, -0.5f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, -0.3f, 0.0f);
		glEnd();
}

void N()
{
	// |
	 
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.3f, 0.0f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(-0.3f, 0.5f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.3f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.3f, -0.5f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.3f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.3f, -0.5f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.3f, 0.0f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.3f, 0.5f, 0.0f);
	glEnd();


	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(-0.3f, 0.5f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(-0.35f, 0.35f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, -0.13f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.13f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, -0.13f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.3f, -0.5f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.35f, -0.35f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.13f, 0.0f);
	glEnd();
	
}

void D()
{
	// |
	glBegin(GL_QUADS);
	//glColor3f(1.0f, 1.0f, 0.0f);
	glColor3f(DColor1, DColor1, 0.0f);
	glVertex3f(-0.4f, 0.5f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(-0.4f, 0.0f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(-0.2f, 0.0f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(-0.2f, 0.5f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(-0.4f, -0.5f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(-0.4f, 0.0f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(-0.2f, 0.0f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(-0.2f, -0.5f, 0.0f);
	glEnd();

	// -
	glBegin(GL_QUADS);
	//glColor3f(1.0f, 0.5f, 0.0f);
	glColor3f(DColor1, DColor2, 0.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(-0.5f, 0.3f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(0.3f, 0.3f, 0.0f);
	//glColor3f(1.0f, 0.5f, 0.0f);
	glColor3f(DColor1, DColor2, 0.0f);
	glVertex3f(0.3f, 0.5f, 0.0f);
	glEnd();

	// |
	glBegin(GL_QUADS);
	//glColor3f(1.0f, 0.5f, 0.0f);
	glColor3f(DColor1, DColor2, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(0.5f, 0.0f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(0.3f, 0.0f, 0.0f);
	//glColor3f(1.0f, 0.5f, 0.0f);
	glColor3f(DColor1, DColor2, 0.0f);
	glVertex3f(0.3f, 0.5f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	//glColor3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, DColor1, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(0.5f, 0.0f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(0.3f, 0.0f, 0.0f);
	//glColor3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, DColor1, 0.0f);
	glVertex3f(0.3f, -0.5f, 0.0f);
	glEnd();

	// -
	glBegin(GL_QUADS);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1, DColor1, DColor1);
	glVertex3f(-0.5f, -0.3f, 0.0f);
	//glColor3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, DColor1, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	//glColor3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, DColor1, 0.0f);
	glVertex3f(0.3f, -0.5f, 0.0f);
	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor3f(DColor1,DColor1,DColor1);
	glVertex3f(0.3f, -0.3f, 0.0f);
	glEnd();
}

void A()
{
	//   -
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, 0.3f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.3f, 0.0f);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glEnd();

	// |
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, 0.3f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.3f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.3f, 0.3f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.5f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.3f, -0.5f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.3f, 0.0f, 0.0f);
	glEnd();


	// |
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.3f, 0.3f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.3f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.3f, 0.0f);
	glEnd();

	// |
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.3f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.3f, -0.5f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glEnd();

}

void MiddleLineofA()
{
	// middle line of A
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(-0.3f, 0.1f, 0.0f);
	glVertex3f(-0.3f, 0.05f, 0.0f);
	glVertex3f(0.3f, 0.05f, 0.0f);
	glVertex3f(0.3f, 0.1f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.3f, 0.0f, 0.0f);
	glVertex3f(-0.3f, -0.05f, 0.0f);
	glVertex3f(0.3f, -0.05f, 0.0f);
	glVertex3f(0.3f, 0.0f, 0.0f);
	glEnd();


	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.3f, -0.1f, 0.0f);
	glVertex3f(-0.3f, -0.15f, 0.0f);
	glVertex3f(0.3f, -0.15f, 0.0f);
	glVertex3f(0.3f, -0.1f, 0.0f);
	glEnd();

}

void uninitialize(void)
{
	// function declarations
	void ToggleFullScreen(void);

	// code
	if (gbFullScreen)
	{
		ToggleFullScreen();
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
		fprintf(gpFile, "Log file Successfully Closed. \n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
