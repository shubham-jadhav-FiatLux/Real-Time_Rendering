// Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>

// OpenGL Header Files
#include <GL/gl.h>
#include <GL/glu.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define CHECKERBOARD_WIDTH 64
#define CHECKERBOARD_HEIGHT 64

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
BOOL gbFullscreen = FALSE;
GLubyte checkerboard[CHECKERBOARD_WIDTH][CHECKERBOARD_HEIGHT][4];
GLuint Texture_Checkerboard;

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
	// Function Declarations
	void resize(int, int);
	void LoadGLTexture(void);

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
	pfd.cDepthBits = 32;

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
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Depth Related Changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);

	LoadGLTexture();

	// Warmup resize() Call
	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void LoadGLTexture(void)
{
	// Function Declarations
	void MakeCheckerboard(void);

	// Code
	MakeCheckerboard();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &Texture_Checkerboard);
	glBindTexture(GL_TEXTURE_2D, Texture_Checkerboard);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		CHECKERBOARD_WIDTH,
		CHECKERBOARD_HEIGHT,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		checkerboard);

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);			// FFP Is Implemented

	glBindTexture(GL_TEXTURE_2D, 0);
}

void MakeCheckerboard(void)
{
	// Variable Declarations
	int c;

	// Code
	for (int i = 0; i < CHECKERBOARD_HEIGHT; i++)
	{
		for (int j = 0; j < CHECKERBOARD_WIDTH; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

			checkerboard[i][j][0] = (GLubyte)c;
			checkerboard[i][j][1] = (GLubyte)c;
			checkerboard[i][j][2] = (GLubyte)c;
			checkerboard[i][j][3] = (GLubyte)255;
		}
	}
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
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -3.0f);

	glBindTexture(GL_TEXTURE_2D, Texture_Checkerboard);

	glBegin(GL_QUADS);
	
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-2.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-2.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.0f, -1.0f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(2.41421f, 1.0f, -1.41421f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(2.41421f, -1.0f, -1.41421f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D,0);

	SwapBuffers(ghdc);
}

void update(void)
{
	// Code

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

	if (Texture_Checkerboard)
	{
		glDeleteTextures(1, &Texture_Checkerboard);
		Texture_Checkerboard = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log File Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
