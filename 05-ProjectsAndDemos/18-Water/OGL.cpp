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
#define TESSEL_X 100
#define TESSEL_Z 100
#define QUADSIZE 8.0f
#define XSHIFT (QUADSIZE *2.0f) /TESSEL_X
#define ZSHIFT (QUADSIZE *2.0f) /TESSEL_Z

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
GLuint texture_water;

float water_x[TESSEL_X][TESSEL_Z];
float water_y[TESSEL_X][TESSEL_Z];
float water_z[TESSEL_X][TESSEL_Z];
int freq = 12;
float time = 0.0f;
GLfloat angleScene = 0.0f;

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
	BOOL LoadGLTexture(GLuint*, TCHAR[]);

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

	for (int i = 0; i < TESSEL_Z; i++)
	{
		for (int j = 0; j < TESSEL_X; j++)
		{
			/*float temp_x = (float)j / (float)TESSEL_X;
			temp_x = (temp_x * 2.0f) - 1.0f;

			float temp_z = (float)i / (float)TESSEL_Z;
			temp_z = (temp_z * 2.0f) - 1.0f;*/

			water_x[i][j] = -QUADSIZE + (j * XSHIFT);
			water_y[i][j] = 0.0f;//sin(temp_x * freq) * cos(temp_z * freq) * 0.1;
			water_z[i][j] = -QUADSIZE + (i * ZSHIFT);
		}
	}

	if (LoadGLTexture(&texture_water, MAKEINTRESOURCE(WATER)) == FALSE)
		return -5;

	// Enabling The Texture
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_AUTO_NORMAL);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
	void ocean(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -15.0f);
	glRotatef(angleScene, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
		glTranslatef(0.0f, -2.0f, 0.0f);
		//glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
		ocean();
	glPopMatrix();	

	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	time = time + 0.05f;
	// Code
	for (int i = 0; i < TESSEL_Z; i++)
	{
		for (int j = 0; j < TESSEL_X; j++)
		{
			float temp_x = (float)(j + 0) / (float)TESSEL_X;
			temp_x = (temp_x * 2.0f) - 1.0f;

			float temp_z = (float)(i + 0) / (float)TESSEL_Z;
			temp_z = (temp_z * 2.0f) - 1.0f;

			water_y[i][j] = sin(temp_x * freq + time) * cos(temp_z * freq + time) * 0.1f;
		}
	}

	angleScene += 0.1f;
}

void ocean()
{
	// Code
	glBindTexture(GL_TEXTURE_2D, texture_water);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < TESSEL_Z - 1; i++)
	{
		for (int j = 0; j < TESSEL_X - 1; j++)
		{
			/*float temp_x = (float)(j + 0) / (float)TESSEL_X;
			temp_x = (temp_x * 2.0f) - 1.0f;

			float temp_z = (float)(i + 0) / (float)TESSEL_Z;
			temp_z = (temp_z * 2.0f) - 1.0f;

			water_y[i][j] = sin(temp_x * freq + time) * cos(temp_z * freq + time) * 0.2f;*/
			//glColor3f(0.0f, 0.0f, fabs(water_y[i + 0][j + 0]) * 10.0f + 0.5f);
			//glColor3f(0.0f, 0.0f, 0.7f);
			//glColor3f(0.0f, 0.0f, 0.4f);
			glTexCoord2f(0.5f, 1.0f);
			glVertex3f(water_x[i + 0][j + 0], water_y[i + 0][j + 0], water_z[i + 0][j + 0]);
			//glColor3f(0.0f, 0.0f, fabs(water_y[i + 0][j + 1]) * 10.0f + 0.5f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(water_x[i + 0][j + 1], water_y[i + 0][j + 1], water_z[i + 0][j + 1]);
			//glColor3f(0.0f, 0.0f, fabs(water_y[i + 1][j + 0]) * 10.0f + 0.5f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(water_x[i + 1][j + 0], water_y[i + 1][j + 0], water_z[i + 1][j + 0]);

			//glColor3f(0.0f, 0.0f, 0.7f);
			//glColor3f(0.0f, 0.0f, fabs(water_y[i + 0][j + 1]) * 10.0f + 0.5f);
			glTexCoord2f(0.5f, 1.0f);
			glVertex3f(water_x[i + 0][j + 1], water_y[i + 0][j + 1], water_z[i + 0][j + 1]);
			//glColor3f(0.0f, 0.0f, fabs(water_y[i + 1][j + 0]) * 10.0f + 0.5f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(water_x[i + 1][j + 0], water_y[i + 1][j + 0], water_z[i + 1][j + 0]);
			//glColor3f(0.0f, 0.0f, fabs(water_y[i + 1][j + 1]) * 10.0f + 0.5f);
			//glColor3f(0.0f, 0.0f, 0.5f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(water_x[i + 1][j + 1], water_y[i + 1][j + 1], water_z[i + 1][j + 1]);

		}
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
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
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		// Generate Texture Object
		glGenTextures(1, Texture);

		// Bind That Texture Object
		glBindTexture(GL_TEXTURE_2D, *Texture);

		// Set Texture Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		// Create The Texture
		gluBuild2DMipmaps(GL_TEXTURE_2D,
			3,
			bmp.bmWidth,
			bmp.bmHeight,
			GL_BGR_EXT,
			GL_UNSIGNED_BYTE,
			bmp.bmBits);

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

	if (texture_water)
	{
		glDeleteTextures(1, &texture_water);
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
