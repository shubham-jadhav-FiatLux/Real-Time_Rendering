// Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>
#include <Mmsystem.h>

#define _USE_MATH_DEFINES 1
#include <math.h>

// OpenGL Header Files
#include <GL/gl.h>
#include <GL/glu.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// OpenGL Libraries
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

// Win32 Library For PlaySound()
#pragma comment(lib, "Winmm.lib")

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable Declarations
FILE* gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
BOOL gbFullscreen = FALSE;
GLUquadric* quadric = NULL;

float oRed = 0.0f;
float oGreen = 0.0f;
float oBlue = 0.0f;
float w = 0.0f;
float gRed = 0.0f;
float gGreen = 0.0f;
float gBlue = 0.0f;

float xi = -1.0f;
float y_n = 1.0f;
float yi = -1.0f;
float xa = 1.0f;
float xaero = -2.5f;
float xaero_1 = -1.3f;
float yaero_1 = 1.0f;
float aeroAngle = -90.0f;
float aeroAngle2 = 90.0f;
float Angle = 180.0f;
float r = yaero_1;
float a = xaero_1 + r;
float b = yaero_1;
float z = -1.0f;

int flagA = 0;
int flagN = 0;
int flagI = 0;
int flagD = 0;
int flagZ = 0;
int flagNegativeZ = 0;
int flagAeroplane = 0;
int flagAeroplane1 = 0;
int flagWhiteStrip = 0;
int flagOrangeStrip = 0;
int flagGreenStrip = 0;

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
		PlaySound(MAKEINTRESOURCE(MYAUDIO), NULL, SND_RESOURCE | SND_ASYNC);
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
	// Clear The Screen Using Black Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Depth Related Changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

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

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// Function Declarations
	void I(float, float, float);
	void N(float, float, float);
	void D(float, float, float);
	void A(float, float, float);
	void Aeroplane(int);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glTranslatef(0.0f, 0.0f, -3.0f);
	
	if (flagZ == 0)
		glTranslatef(0.0f, 0.0f, z);

	else if (flagZ == 1 && flagNegativeZ == 0)
	{
		glTranslatef(0.0f, 0.0f, z);
		if (z <= -2.0f)
			flagAeroplane = 1;
	}
	else
	{
		glTranslatef(0.0f, 0.0f, z);
	}

	if (1)
	{
		glPushMatrix();
		if (xi <= 0.0f)
			glTranslatef(xi, 0.0f, 0.0f);
		else
			flagA = 1;
		I(-0.5f, 0.2f, 0.2f);
		glPopMatrix();
	}

	if (flagN == 1)
	{
		glPushMatrix();
		if (y_n >= 0.0f)
			glTranslatef(0.0f, y_n, 0.0f);
		else
			flagI = 1;
		N(-0.3f, 0.2f, 0.2f);
		glPopMatrix();
	}

	if (flagD == 1)
	{
		glColor3f(0.0f, 0.0f, 0.0f);
		if (oRed >= 0.9f)
			flagZ = 1;
			//flagAeroplane = 1;
		D(-0.1f, 0.2f, 0.2f);
	}

	if (flagI == 1)
	{
		glPushMatrix();
		if (yi <= 0.0f)
			glTranslatef(0.0f, yi, 0.0f);
		else
			flagD = 1;
		I(0.1f, 0.2f, 0.2f);
		glPopMatrix();
	}

	if (flagA == 1)
	{
		glPushMatrix();
		if (xa >= 0.0f)
			glTranslatef(xa, 0.0f, 0.0f);
		else
			flagN = 1;
		A(0.3f, 0.2f, 0.2f);
		glPopMatrix();
	}

	if (flagAeroplane == 1)
	{
		glPushMatrix();
		if (xaero <= 5.0f)
		{
			glTranslatef(xaero, 0.0f, 0.0f);
			if (xaero >= -0.855f)
				flagAeroplane1 = 1;
			if (xaero >= 0.6f)
				flagWhiteStrip = 1;
			if (xaero >= 2.0f)
				flagNegativeZ = 1;
			glScalef(0.5f, 0.5f, 0.5f);
			Aeroplane(2);
		}
		glPopMatrix();

		if (flagAeroplane1 == 1)
		{
			// Upper Plane
			glPushMatrix();
			glTranslatef(xaero_1, yaero_1, 0.0f);
			if (xaero_1 >= 0.8f)
				flagOrangeStrip = 1;
			glRotatef(aeroAngle, 0.0f, 0.0f, 1.0f);
			glScalef(0.5f, 0.5f, 0.5f);
			Aeroplane(1);
			glPopMatrix();

			
			// Lower Plane
			glPushMatrix();
			glTranslatef(xaero_1, -yaero_1, 0.0f);
			if (xaero_1 >= 0.8f)
				flagGreenStrip = 1;
			glRotatef(aeroAngle2, 0.0f, 0.0f, 1.0f);
			glScalef(0.5f, 0.5f, 0.5f);
			Aeroplane(3);
			glPopMatrix();
		}
	}
	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	if (xi <= 0.0f)
		xi += 0.003f;
	if (y_n >= 0.0f && flagN == 1)
		y_n -= 0.0035f;

	if (flagD == 1)
	{
		if (oRed <= 1.0f)
			oRed += 0.0008f;
		if (oGreen < 0.56f)
			oGreen += 0.0005f;
		if (oBlue <= 0.11f)
			oBlue += 0.0001f;
		w = oRed;
		if (gRed < 0.313f)
			gRed += 0.00025f;
		if (gGreen < 0.62f)
			gGreen += 0.00045f;
		if (gBlue < 0.184f)
			gBlue += 0.00015f;
	}
	if (yi <= 0.0f && flagI == 1)
		yi += 0.002f;
	if (xa >= 0.0f && flagA ==1)
		xa -= 0.0025f;
	if (xaero <= 5.0f && flagAeroplane == 1)
		xaero += 0.0015f;

	if (flagAeroplane1 == 1)
	{
		if (Angle <= 270.0f)
		{
			xaero_1 = a + r * cos(Angle * M_PI / 180.0f);
			yaero_1 = b + r * sin(Angle * M_PI / 180.0f);
			Angle += 0.2f;
			aeroAngle += 0.2f;
			aeroAngle2 -= 0.2f;
		}
		else if (Angle >= 270.0f && xaero_1 <= 0.9f)
		{
			xaero_1 += 0.0015f;					
		}
		else
		{
			xaero_1 = (-a + 0.2f) + r * cos(Angle * M_PI / 180.0f);
			yaero_1 = b + r * sin(Angle * M_PI / 180.0f);
			Angle += 0.2f;
			aeroAngle += 0.2f;
			aeroAngle2 -= 0.2f;
		}
	}
	if (z >= -2.0f && flagZ == 1 && flagNegativeZ == 0)
		z -= 0.0035f;
	else if (z <= -1.0f && flagNegativeZ == 1)
		z += 0.003f;
}

void I(float x, float y, float width)
{
	// Code
	glBegin(GL_QUADS);

		// Upper Quad
		glColor3f(1.0f, 0.56f, 0.11f);								// Orange
		glVertex3f((x + width - 0.01), y, 0.0f);
		glVertex3f((x + 0.01f), y, 0.0f);
		glVertex3f((x + 0.01f), (y - 0.05f), 0.0f);
		glVertex3f((x + width - 0.01f), (y - 0.05f), 0.0f);


		// Middle Quad 1
		glVertex3f((x + 2 * width / 3 - 0.01f), (y - 0.05f), 0.0f);
		glVertex3f((x + width / 3 + 0.01f), (y - 0.05f), 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);								// White
		glVertex3f((x + width / 3 + 0.01f), 0.0f, 0.0f);
		glVertex3f((x + 2 * width / 3 - 0.01f), 0.0f, 0.0f);

		// Middle Quad 2
		glVertex3f((x + 2 * width / 3 - 0.01f), 0.0f, 0.0f);
		glVertex3f((x + width / 3 + 0.01f), 0.0f, 0.0f);
		glColor3f(0.313f, 0.62f, 0.184f);							// Green
		glVertex3f((x + width / 3 + 0.01f), (-y + 0.05f), 0.0f);
		glVertex3f((x + 2 * width / 3 - 0.01f), (-y + 0.05f), 0.0f);


		// Lower Quad
		glColor3f(0.313f, 0.62f, 0.184f);
		glVertex3f((x + width - 0.01), (-y + 0.05f), 0.0f);
		glVertex3f((x + 0.01f), (-y + 0.05f), 0.0f);
		glVertex3f((x + 0.01f), -y, 0.0f);
		glVertex3f((x + width - 0.01f), -y, 0.0f);

		glEnd();
}

void N(float x, float y, float width)
{
	// Code
	glBegin(GL_QUADS);
	
	// Left Quad 1
	glColor3f(1.0f, 0.56f, 0.11f);								// Orange
	glVertex3f((x + width / 3 - 0.01f), y, 0.0f);
	glVertex3f((x + 0.01f), y, 0.0f);
	glVertex3f((x + 0.01f), y - 0.05f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), y - 0.05f, 0.0f);
	

	glVertex3f((x + width / 3 - 0.01f), y - 0.05f, 0.0f);
	glVertex3f((x + 0.01f), y - 0.05f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);								// White
	glVertex3f((x + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);
	
	// Left Quad 2
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 0.01f), 0.0f, 0.0f);
	glColor3f(0.313f, 0.62f, 0.184f);							// Green
	glVertex3f((x + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), -y + 0.05f, 0.0f);
	
	glVertex3f((x + width / 3 - 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 0.01f), -y, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), -y, 0.0f);

	// Diagonal Quad 1
	glColor3f(1.0f, 0.56f, 0.11f);									// Orange
	glVertex3f((x + width / 3 - 0.005f), y, 0.0f);
	glVertex3f((x + width / 3 - 0.005f), (y - 0.1f), 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);									// White
	glVertex3f((x + width /2 - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width / 2 + 0.015f), 0.0f, 0.0f);

	// Diagonal Quad 2
	glVertex3f((x + width / 2 + 0.015f), 0.0f, 0.0f);
	glVertex3f((x + width / 2 - 0.01f), 0.0f, 0.0f);
	glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + 2 * width / 3 + 0.005f), -y, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.005f), (-y + 0.1f), 0.0f);

	
	// Right Quad 1
	glColor3f(1.0f, 0.56f, 0.11f);									// Orange
	glVertex3f((x + width - 0.01f), y, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), y, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), y - 0.05f, 0.0f);
	glVertex3f((x + width - 0.01f), y - 0.05f, 0.0f);

	glVertex3f((x + width - 0.01f), y - 0.05f, 0.0f);
	glVertex3f((x + 2 * width/3 + 0.01f), y - 0.05f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);									// White
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width - 0.01f), 0.0f, 0.0f);

	// Right Quad 2
	glVertex3f((x + width - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 2* width /3 + 0.01f), 0.0f, 0.0f);
	glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + 2 * width / 3 + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x +  width - 0.01f), -y + 0.05f, 0.0f);

	glVertex3f((x + width - 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), -y, 0.0f);
	glVertex3f((x + width - 0.01f), -y, 0.0f);
	
	glEnd();
}

void D(float x, float y, float width)
{
	// Code
	glBegin(GL_QUADS);

	// Left Quad 1
	glColor3f(oRed, oGreen, oBlue);
	//glColor3f(1.0f, 0.56f, 0.11f);								// Orange
	glVertex3f((x + width / 3 - 0.01f), (y - 0.05f), 0.0f);
	glVertex3f((x + 0.01f), (y - 0.05f), 0.0f);
	glColor3f(w, w, w);
	//glColor3f(1.0f, 1.0f, 1.0f);									// White
	glVertex3f((x + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);

	// Left Quad 2
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 0.01f), 0.0f, 0.0f);
	glColor3f(gRed, gGreen, gBlue);
	//glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + 0.01f), (-y + 0.05f), 0.0f);
	glVertex3f((x + width / 3 - 0.01f), (-y + 0.05f), 0.0f);


	// Right Quad 1
	glColor3f(oRed, oGreen, oBlue);
	//glColor3f(1.0f, 0.56f, 0.11f);								// Orange
	glVertex3f((x + width - 0.01f), (y-0.05f), 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), y, 0.0f);
	glColor3f(w, w, w);
	//glColor3f(1.0f, 1.0f, 1.0f);									// White
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width - 0.01f), 0.0f, 0.0f);

	// Right Quad 2
	glVertex3f((x + width - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);
	glColor3f(gRed, gGreen, gBlue);
	//glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + 2 * width / 3 + 0.01f), (-y + 0.05f), 0.0f);
	glVertex3f((x + width - 0.01f), (-y + 0.05f), 0.0f);


	// Upper Quad
	glColor3f(oRed, oGreen, oBlue);
	//glColor3f(1.0f, 0.56f, 0.11f);								// Orange
	glVertex3f((x + 2 * width/3 + 0.01), y, 0.0f);
	glVertex3f((x + 0.01f), y, 0.0f);
	glVertex3f((x + 0.01f), (y - 0.05f), 0.0f);
	glVertex3f((x + width - 0.01f), (y - 0.05f), 0.0f);


	// Lower Quad
	glColor3f(gRed, gGreen, gBlue);
	//glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + width - 0.01), (-y + 0.05f), 0.0f);
	glVertex3f((x + 0.01f), (-y + 0.05f), 0.0f);
	glVertex3f((x + 0.01f), -y, 0.0f);
	glVertex3f((x + width - 0.01f), -y, 0.0f);

	glEnd();
}

void A(float x, float y, float width)
{
	// Code
	glBegin(GL_QUADS);

	// Left Quad 1
	glColor3f(1.0f, 0.56f, 0.11f);									// Orange
	glVertex3f((x + width / 3 - 0.01f), (y - 0.05f), 0.0f);
	glVertex3f((x + 0.01f), (y - 0.05f), 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);									// White
	glVertex3f((x + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);

	// Left Quad 2
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 0.01f), 0.0f, 0.0f);
	glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + 0.01f), -y+0.05f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), -y + 0.05f, 0.0f);

	glVertex3f((x + width / 3 - 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 0.01f), -y, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), -y, 0.0f);


	// Right Quad 1
	glColor3f(1.0f, 0.56f, 0.11f);									// Orange
	glVertex3f((x + width - 0.01f), (y - 0.05f), 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), (y - 0.05f), 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);									// White
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width - 0.01f), 0.0f, 0.0f);

	// Right Quad 2
	glVertex3f((x + width - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);
	glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + 2 * width / 3 + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + width - 0.01f), -y + 0.05f, 0.0f);

	glVertex3f((x + width - 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), -y, 0.0f);
	glVertex3f((x + width - 0.01f), -y, 0.0f);

	// Upper Quad
	glColor3f(1.0f, 0.56f, 0.11f);									// Orange
	glVertex3f((x + 2 * width / 3 + 0.01f), y, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), y, 0.0f);
	glVertex3f((x + 0.01f), (y - 0.05f), 0.0f);
	glVertex3f((x + width - 0.01f), (y - 0.05f), 0.0f);


		// Middle Quad 1
	glColor3f(0.0f, 0.0f, 0.0f);
	if (flagOrangeStrip == 1)
	{
		glColor3f(1.0f, 0.56f, 0.11f);								// Orange
	}
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.03f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), 0.03f, 0.0f);
		
	if (flagWhiteStrip == 1)
	{
		glColor3f(1.0f, 1.0f, 1.0f);								// White
	}
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);

	// Middle Quad 2
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);
	
	glColor3f(0.0f, 0.0f, 0.0f);
	if (flagGreenStrip == 1)
	{
		glColor3f(0.313f, 0.62f, 0.184f);							// Green
	}
	glVertex3f((x + width / 3 - 0.01f), -0.03f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), -0.03f, 0.0f);
	
	glEnd();
}

void Aeroplane(int number)
{
	// Code
	// Missiles (Body)
	glBegin(GL_QUADS);
	glColor3f(0.521f, 0.627f, 0.694f);				// Light Sky Blue
	glVertex3f(-0.1f, 0.45f, 0.0f);
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.4f, 0.45f, 0.0f);
	glVertex3f(-0.4f, 0.4f, 0.0f);
	glColor3f(0.521f, 0.627f, 0.694f);				// Light Sky Blue
	glVertex3f(-0.1f, 0.4f, 0.0f);

	glVertex3f(-0.1f, -0.45f, 0.0f);
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.4f, -0.45f, 0.0f);
	glVertex3f(-0.4f, -0.4f, 0.0f);
	glColor3f(0.521f, 0.627f, 0.694f);				// Light Sky Blue
	glVertex3f(-0.1f, -0.4f, 0.0f);
	glEnd();

	// Missiles (Tip)
	glBegin(GL_TRIANGLES);
	glColor3f(0.282f, 0.235f, 0.196f);				// Taupe			
	glVertex3f(0.0f, 0.425f, 0.0f);
	glColor3f(0.541f, 0.498f, 0.502f);				// Rocket Metalic
	glVertex3f(-0.1f, 0.45f, 0.0f);
	glVertex3f(-0.1f, 0.4f, 0.0f);

	glColor3f(0.282f, 0.235f, 0.196f);				// Taupe
	glVertex3f(0.0f, -0.425f, 0.0f);
	glColor3f(0.541f, 0.498f, 0.502f);				// Rocket Metalic
	glVertex3f(-0.1f, -0.45f, 0.0f);
	glVertex3f(-0.1f, -0.4f, 0.0f);

	glColor3f(0.282f, 0.235f, 0.196f);				// Taupe
	glVertex3f(0.25f, 0.1f, 0.0f);
	glColor3f(0.541f, 0.498f, 0.502f);				// Rocket Metalic
	glVertex3f(0.2f, 0.12f, 0.0f);
	glVertex3f(0.2f, 0.08f, 0.0f);

	glColor3f(0.282f, 0.235f, 0.196f);				// Taupe
	glVertex3f(0.25f, -0.1f, 0.0f);
	glColor3f(0.541f, 0.498f, 0.502f);				// Rocket Metalic
	glVertex3f(0.2f, -0.12f, 0.0f);
	glVertex3f(0.2f, -0.08f, 0.0f);
	glEnd();



	// Horizontal Wing
	glBegin(GL_TRIANGLES);
	glColor3f(0.576f, 0.627f, 0.662f);				// Light Gray
	glVertex3f(0.15f, 0.0f, 0.0f);
	glColor3f(0.521f, 0.627f, 0.694f);				// Light Sky Blue
	glVertex3f(-0.4f, 0.5f, 0.0f);
	glVertex3f(-0.4f, -0.5f, 0.0f);
	glEnd();

	// Beautification (Inside Wing)
	glBegin(GL_LINES);
	glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glVertex3f(-0.2f, 0.3f, 0.0f);
	glVertex3f(-0.2, -0.3f, 0.0f);
	glEnd();

	// Beautification (Inside Wing)
	glBegin(GL_QUADS);
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.15f, 0.2f, 0.0f);					// Upper 1
	glColor3f(0.521f, 0.627f, 0.694f);				// Light Sky Blue
	glVertex3f(-0.2f, 0.2f, 0.0f);
	glVertex3f(-0.2f, 0.15f, 0.0f);
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.12f, 0.15f, 0.0f);

	glVertex3f(-0.15f, -0.2f, 0.0f);				// Upper 1
	glColor3f(0.521f, 0.627f, 0.694f);				// Light Sky Blue
	glVertex3f(-0.2f, -0.2f, 0.0f);
	glVertex3f(-0.2f, -0.15f, 0.0f);
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.12f, -0.15f, 0.0f);
	glEnd();

	// Wing Outer quads
	glBegin(GL_QUADS);
	glColor3f(0.458f, 0.458f, 0.458f);				// Sonic Silver
	glVertex3f(-0.4f, 0.53f, 0.0f);					// Upper 1
	//glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.45f, 0.53f, 0.0f);
	glColor3f(0.517f, 0.556f, 0.564f);				// Dark Gray
	glVertex3f(-0.47f, 0.3f, 0.0f);
	glColor3f(0.458f, 0.458f, 0.458f);				// Sonic Silver
	glVertex3f(-0.4f, 0.3f, 0.0f);

	glColor3f(0.458f, 0.458f, 0.458f);				// Sonic Silver
	glVertex3f(-0.4f, 0.3f, 0.0f);					// Upper 2
	//glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.47f, 0.3f, 0.0f);
	glColor3f(0.517f, 0.556f, 0.564f);				// Dark Gray
	glVertex3f(-0.5f, 0.0f, 0.0f);
	glColor3f(0.458f, 0.458f, 0.458f);				// Sonic Silver
	glVertex3f(-0.4f, 0.0f, 0.0f);

	//glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.4f, 0.53f, 0.0f);					// Upper 3
	glColor3f(0.458f, 0.458f, 0.458f);				// Sonic Silver
	glVertex3f(-0.4f, 0.5f, 0.0f);
	glColor3f(0.517f, 0.556f, 0.564f);				// Dark Gray
	glVertex3f(-0.2f, 0.315f, 0.0f);
	glVertex3f(-0.2f, 0.36f, 0.0f);

	//glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.205f, 0.37f, 0.0f);				// Upper 4
	glColor3f(0.458f, 0.458f, 0.458f);				// Sonic Silver
	glVertex3f(-0.205f, 0.315f, 0.0f);
	glColor3f(0.517f, 0.556f, 0.564f);				// Dark Gray
	glVertex3f(0.15f, 0.0f, 0.0f);
	glVertex3f(0.27f, 0.0f, 0.0f);


	glColor3f(0.458f, 0.458f, 0.458f);				// Sonic Silver
	glVertex3f(-0.4f, -0.53f, 0.0f);				// Lower 1
	//glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.45f, -0.53f, 0.0f);
	glColor3f(0.517f, 0.556f, 0.564f);				// Dark Gray
	glVertex3f(-0.47f, -0.3f, 0.0f);
	glColor3f(0.458f, 0.458f, 0.458f);				// Sonic Silver
	glVertex3f(-0.4f, -0.3f, 0.0f);

	glColor3f(0.458f, 0.458f, 0.458f);				// Sonic Silver
	glVertex3f(-0.4f, -0.3f, 0.0f);					// Lower 2
	//glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.47f, -0.3f, 0.0f);
	glColor3f(0.517f, 0.556f, 0.564f);				// Dark Gray
	glVertex3f(-0.5f, -0.0f, 0.0f);
	glColor3f(0.458f, 0.458f, 0.458f);				// Sonic Silver
	glVertex3f(-0.4f, -0.0f, 0.0f);

	//glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.4f, -0.53f, 0.0f);				// Lower 3
	glColor3f(0.458f, 0.458f, 0.458f);				// Sonic Silver
	glVertex3f(-0.4f, -0.5f, 0.0f);
	glColor3f(0.517f, 0.556f, 0.564f);				// Dark Gray
	glVertex3f(-0.2f, -0.315f, 0.0f);
	glVertex3f(-0.2f, -0.36f, 0.0f);

	//glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.205f, -0.37f, 0.0f);				// Lower 4
	glColor3f(0.458f, 0.458f, 0.458f);				// Sonic Silver
	glVertex3f(-0.205f, -0.315f, 0.0f);
	glColor3f(0.517f, 0.556f, 0.564f);				// Dark Gray
	glVertex3f(0.15f, -0.0f, 0.0f);
	glVertex3f(0.27f, -0.0f, 0.0f);

	glEnd();



	// Outer Body
	glBegin(GL_QUADS);
	glColor3f(0.576f, 0.627f, 0.662f);				// Light Gray
	glVertex3f(0.2f, 0.15f, 0.0f);
	glVertex3f(-0.42f, 0.15f, 0.0f);
	glVertex3f(-0.42f, -0.15f, 0.0f);
	glVertex3f(0.2f, -0.15f, 0.0f);

	glVertex3f(-0.4f, 0.1f, 0.0f);
	glVertex3f(-0.5f, 0.1f, 0.0f);
	glVertex3f(-0.55f, 0.07f, 0.0f);
	glVertex3f(-0.4f, 0.07f, 0.0f);

	glVertex3f(-0.4f, -0.1f, 0.0f);
	glVertex3f(-0.5f, -0.1f, 0.0f);
	glVertex3f(-0.55f, -0.07f, 0.0f);
	glVertex3f(-0.4f, -0.07f, 0.0f);
	glEnd();

	// Inner Body
	glBegin(GL_QUADS);
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(0.2f, 0.15f, 0.0f);
	glColor3f(0.521f, 0.627f, 0.694f);				// Light Sky Blue
	glVertex3f(-0.2f, 0.1f, 0.0f);
	glVertex3f(-0.2f, -0.1f, 0.0f);
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(0.2f, -0.15f, 0.0f);

	//glColor3f(0.517f, 0.556f, 0.564f);			// Dark Gray
	glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glVertex3f(0.15f, 0.15f, 0.0f);
	glColor3f(0.321f, 0.321f, 0.321f);
	glVertex3f(0.05f, 0.15f, 0.0f);
	glVertex3f(0.05f, 0.12f, 0.0f);
	glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glVertex3f(0.18f, 0.12f, 0.0f);

	glVertex3f(0.15f, -0.15f, 0.0f);
	glColor3f(0.321f, 0.321f, 0.321f);
	glVertex3f(0.05f, -0.15f, 0.0f);
	glVertex3f(0.05f, -0.12f, 0.0f);
	glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glVertex3f(0.18f, -0.12f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.521f, 0.627f, 0.694f);				// Light Sky Blue
	glVertex3f(-0.2f, 0.09f, 0.0f);
	glVertex3f(-0.4f, 0.07f, 0.0f);
	glVertex3f(-0.4f, -0.07f, 0.0f);
	glVertex3f(-0.2f, -0.09f, 0.0f);
	glEnd();

	// Back Body ( Circular Part At The End)
	glBegin(GL_QUADS);
	glColor3f(0.521f, 0.627f, 0.694f);				// Light Sky Blue
	glVertex3f(-0.4f, 0.07f, 0.0f);
	glVertex3f(-0.6f, 0.07f, 0.0f);
	glVertex3f(-0.6f, -0.07f, 0.0f);
	glVertex3f(-0.4f, -0.07f, 0.0f);

	//glColor3f(0.517f, 0.556f, 0.564f);			// Dark Gray
	glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glVertex3f(-0.6f, 0.07f, 0.0f);
	glColor3f(0.321f, 0.321f, 0.321f);
	glVertex3f(-0.7f, 0.07f, 0.0f);
	glVertex3f(-0.7f, -0.07f, 0.0f);
	glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glVertex3f(-0.6f, -0.07f, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-0.7f, 0.05f, 0.0f);
	glVertex3f(-0.6f, 0.05f, 0.0f);

	glVertex3f(-0.7f, 0.03f, 0.0f);
	glVertex3f(-0.6f, 0.03f, 0.0f);

	glVertex3f(-0.7f, 0.01f, 0.0f);
	glVertex3f(-0.6f, 0.01f, 0.0f);

	glVertex3f(-0.7f, -0.01f, 0.0f);
	glVertex3f(-0.6f, -0.01f, 0.0f);

	glVertex3f(-0.7f, -0.03f, 0.0f);
	glVertex3f(-0.6f, -0.03f, 0.0f);

	glVertex3f(-0.7f, -0.05f, 0.0f);
	glVertex3f(-0.6f, -0.05f, 0.0f);

	glVertex3f(-0.6f, 0.07f, 0.0f);
	glVertex3f(-0.6f, -0.07f, 0.0f);
	glEnd();


	// Front Nozzle
	glBegin(GL_QUADS);
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(0.5f, 0.05f, 0.0f);
	glColor3f(0.576f, 0.627f, 0.662f);				// Light Gray
	glVertex3f(0.2f, 0.05f, 0.0f);
	glVertex3f(0.2f, -0.05f, 0.0f);
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(0.5f, -0.05f, 0.0f);
	glEnd();

	// Middle Nozzle
	glBegin(GL_TRIANGLES);
	glColor3f(0.576f, 0.627f, 0.662f);				// Light Gray
	glVertex3f(0.2f, 0.05f, 0.0f);
	glVertex3f(-0.6f, 0.0f, 0.0f);
	glVertex3f(0.2f, -0.05f, 0.0f);
	glEnd();

	// Head
	glColor3f(0.517f, 0.556f, 0.564f);				// Dark Gray
	glPushMatrix();
	glTranslatef(0.2f, 0.0f, 0.0f);
	glScalef(4.0f, 1.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 0.05f, 30, 30);
	glPopMatrix();

	//glColor3f(0.576f, 0.627f, 0.662f);			// Light Gray
	glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glPushMatrix();
	glTranslatef(0.2f, 0.0f, 0.0f);
	glScalef(2.0f, 1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 0.05f, 40, 40);
	glPopMatrix();



	// vertical Wing
	glBegin(GL_POLYGON);
	//glColor3f(0.517f, 0.556f, 0.564f);			// Dark Gray
	glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glVertex3f(-0.5f, 0.05f, 0.0f);
	glVertex3f(-0.58f, 0.05f, 0.0f);
	glVertex3f(-0.58f, -0.05f, 0.0f);
	glVertex3f(-0.5f, -0.05f, 0.0f);
	glVertex3f(-0.45f, 0.0f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.521f, 0.627f, 0.694f);				// Light Sky Blue	
	glVertex3f(-0.3f, 0.0f, 0.0f);
	glColor3f(0.274f, 0.509f, 0.705f);				// Metalic Blue	
	glVertex3f(-0.5f, 0.03f, 0.0f);
	glVertex3f(-0.6f, 0.0f, 0.0f);
	glVertex3f(-0.5f, -0.03f, 0.0f);

	glColor3f(0.576f, 0.627f, 0.662f);				// Light Gray
	glVertex3f(-0.4f, 0.0f, 0.0f);
	glVertex3f(-0.5f, 0.02f, 0.0f);
	glVertex3f(-0.6f, 0.0f, 0.0f);
	glVertex3f(-0.5f, -0.02f, 0.0f);

	glEnd();

	// Front Tip
	glBegin(GL_TRIANGLES);
	//glColor3f(0.517f, 0.556f, 0.564f);			// Dark Gray
	glColor3f(0.204f, 0.204f, 0.204f);				// Jet
	glVertex3f(0.65f, 0.0f, 0.0f);
	glColor3f(0.321f, 0.321f, 0.321f);
	glVertex3f(0.5f, 0.05f, 0.0f);
	glVertex3f(0.5f, -0.05f, 0.0f);
	glEnd();

	// Front Tip Line
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex3f(0.65f, 0.0f, 0.0f);
	glVertex3f(0.7f, 0.0f, 0.0f);
	glEnd();

	// Beautification
	// Wings
	glBegin(GL_QUADS);

	glVertex3f(-0.35f, 0.2f, 0.0f);			// Upper 1
	glVertex3f(-0.39f, 0.2f, 0.0f);
	glVertex3f(-0.39f, 0.18f, 0.0f);
	glVertex3f(-0.35f, 0.18f, 0.0f);

	glVertex3f(-0.35f, 0.25f, 0.0f);		// 2
	glVertex3f(-0.39f, 0.25f, 0.0f);
	glVertex3f(-0.39f, 0.27f, 0.0f);
	glVertex3f(-0.35f, 0.27f, 0.0f);

	glVertex3f(-0.35f, 0.3f, 0.0f);			// 3
	glVertex3f(-0.39f, 0.3f, 0.0f);
	glVertex3f(-0.39f, 0.28f, 0.0f);
	glVertex3f(-0.35f, 0.28f, 0.0f);

	glVertex3f(-0.35f, 0.33f, 0.0f);		// 4
	glVertex3f(-0.39f, 0.33f, 0.0f);
	glVertex3f(-0.39f, 0.31f, 0.0f);
	glVertex3f(-0.35f, 0.31f, 0.0f);

	glVertex3f(-0.35f, 0.4f, 0.0f);			// 5
	glVertex3f(-0.39f, 0.4f, 0.0f);
	glVertex3f(-0.39f, 0.38f, 0.0f);
	glVertex3f(-0.35f, 0.38f, 0.0f);

	glVertex3f(-0.35f, 0.43f, 0.0f);		// 6
	glVertex3f(-0.39f, 0.43f, 0.0f);
	glVertex3f(-0.39f, 0.41f, 0.0f);
	glVertex3f(-0.35f, 0.41f, 0.0f);


	glVertex3f(-0.35f, -0.2f, 0.0f);		// Lower 1
	glVertex3f(-0.39f, -0.2f, 0.0f);
	glVertex3f(-0.39f, -0.18f, 0.0f);
	glVertex3f(-0.35f, -0.18f, 0.0f);


	glVertex3f(-0.35f, -0.25f, 0.0f);		// 2
	glVertex3f(-0.39f, -0.25f, 0.0f);
	glVertex3f(-0.39f, -0.27f, 0.0f);
	glVertex3f(-0.35f, -0.27f, 0.0f);

	glVertex3f(-0.35f, -0.3f, 0.0f);		// 3
	glVertex3f(-0.39f, -0.3f, 0.0f);
	glVertex3f(-0.39f, -0.28f, 0.0f);
	glVertex3f(-0.35f, -0.28f, 0.0f);

	glVertex3f(-0.35f, -0.33f, 0.0f);		// 4
	glVertex3f(-0.39f, -0.33f, 0.0f);
	glVertex3f(-0.39f, -0.31f, 0.0f);
	glVertex3f(-0.35f, -0.31f, 0.0f);

	glVertex3f(-0.35f, -0.4f, 0.0f);		// 5
	glVertex3f(-0.39f, -0.4f, 0.0f);
	glVertex3f(-0.39f, -0.38f, 0.0f);
	glVertex3f(-0.35f, -0.38f, 0.0f);

	glVertex3f(-0.35f, -0.43f, 0.0f);		// 6
	glVertex3f(-0.39f, -0.43f, 0.0f);
	glVertex3f(-0.39f, -0.41f, 0.0f);
	glVertex3f(-0.35f, -0.41f, 0.0f);
	glEnd();

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glColor3f(0.0, 0.0, 0.0f);				// Black
	glVertex3f(-0.1f, 0.02f, 0.0f);			// I
	glVertex3f(-0.1f, -0.02f, 0.0f);
	glVertex3f(-0.11f, 0.02f, 0.0f);
	glVertex3f(-0.09f, 0.02f, 0.0f);
	glVertex3f(-0.11f, -0.02f, 0.0f);
	glVertex3f(-0.09f, -0.02f, 0.0f);

	glVertex3f(-0.07f, 0.02f, 0.0f);		// A
	glVertex3f(-0.08f, -0.02f, 0.0f);
	glVertex3f(-0.07f, 0.02f, 0.0f);
	glVertex3f(-0.06f, -0.02f, 0.0f);
	glVertex3f(-0.08f, -0.01f, 0.0f);
	glVertex3f(-0.06f, -0.01f, 0.0f);

	glVertex3f(-0.05f, 0.02f, 0.0f);		// F
	glVertex3f(-0.05f, -0.02f, 0.0f);
	glVertex3f(-0.05f, 0.02f, 0.0f);
	glVertex3f(-0.035f, 0.02f, 0.0f);
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glVertex3f(-0.04f, 0.0f, 0.0f);
	glEnd();



	glBegin(GL_POLYGON);
	if (number == 1)
	{
		glColor3f(1.0f, 0.56f, 0.11f);			// Orange
		glVertex3f(-0.71f, 0.05f, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);			// Black
		glVertex3f(-1.0f, 0.05f, 0.0f);
		glVertex3f(-1.0f, -0.05f, 0.0f);
		glColor3f(1.0f, 0.56f, 0.11f);			// Orange
		glVertex3f(-0.71f, -0.05f, 0.0f);
	}
	else if (number == 2)
	{
		glColor3f(1.0f, 1.0f, 1.0f);			// White
		glVertex3f(-0.71f, 0.05f, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);			// Black
		glVertex3f(-1.0f, 0.05f, 0.0f);
		glVertex3f(-1.0f, -0.05f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);			// White
		glVertex3f(-0.71f, -0.05f, 0.0f);
	}
	else
	{
		glColor3f(0.313f, 0.62f, 0.184f);		// Green
		glVertex3f(-0.71f, 0.05f, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);			// Black
		glVertex3f(-1.0f, 0.05f, 0.0f);
		glVertex3f(-1.0f, -0.05f, 0.0f);
		glColor3f(0.313f, 0.62f, 0.184f);		// Green
		glVertex3f(-0.71f, -0.05f, 0.0f);
	}
	glEnd();
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
