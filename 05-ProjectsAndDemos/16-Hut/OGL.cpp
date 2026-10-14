// Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>

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

float angleCube = 0.0f;
GLUquadric *quadric;
GLuint texture_wall;
GLuint texture_wall2;
GLuint texture_wall3;
GLuint texture_roof;
GLuint texture_roof2;

GLfloat LightAmbient[] = { 1.5f, 0.5f, 0.5f, 1.0f };
GLfloat LightDiffuse[] = { 1.5f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[] = { 0.0f, 2.0f, 5.0f, 0.0f };

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

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	//glEnable(GL_LIGHT1);
	//glEnable(GL_LIGHTING);

	if (LoadGLTexture(&texture_wall, MAKEINTRESOURCE(WALL)) == FALSE)
		return -5;
	if (LoadGLTexture(&texture_wall2, MAKEINTRESOURCE(WALL2)) == FALSE)
		return -6;
	if (LoadGLTexture(&texture_wall3, MAKEINTRESOURCE(WALL3)) == FALSE)
		return -9;
	if (LoadGLTexture(&texture_roof, MAKEINTRESOURCE(ROOF)) == FALSE)
		return -7;
	if (LoadGLTexture(&texture_roof2, MAKEINTRESOURCE(ROOF2)) == FALSE)
		return -8;

	// Enabling The Texture
	glEnable(GL_TEXTURE_2D);

	quadric = gluNewQuadric();

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
	void hutTwo(void);
	void hutThree(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -6.0f);

	
	glRotatef(angleCube, 0.0f, 1.0f, 0.0f);		
	//hutTwo();
	hutThree();
	

	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	angleCube = angleCube + 0.1f;
	if (angleCube >= 360.0f)
		angleCube = angleCube - 360.0f;
}

void hutOne()
{
	// Code
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, texture_roof);
	glPushMatrix();
	gluQuadricTexture(quadric, GLU_TRUE);
	glTranslatef(0.0f, 0.3f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadric, 1.5f, 0.0f, 1.5f, 30, 30);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_LIGHTING);

	// Walls
	glBindTexture(GL_TEXTURE_2D, texture_wall);
	glPushMatrix();
	glScalef(0.75f, 0.75f, 0.75f);
		glBegin(GL_QUADS);

			// Front Face
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			// Right Face
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			// Back Face
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			// Left Face
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			// Top Face
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			// Bottom Face
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);

		glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void hutTwo()
{
	// Code
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, texture_roof2);
	glPushMatrix();
		glTranslatef(0.0f, 1.0f, 0.0f);
		glScalef(1.2f, 0.75f, 1.2f);
		glBegin(GL_TRIANGLES);

			// Front Face
			glTexCoord2f(0.5f, 1.0f);
			glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			// Right Face
			glTexCoord2f(0.5f, 1.0f);
			glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			// Back Face
			glTexCoord2f(0.5f, 1.0f);
			glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			// Left Face
			glTexCoord2f(0.5f, 1.0f);
			glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 1.0f);

		glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_LIGHTING);

	// Walls
	glBindTexture(GL_TEXTURE_2D, texture_wall2);
	glPushMatrix();
	glScalef(0.75f, 0.75f, 0.75f);
	glBegin(GL_QUADS);

	// Front Face
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	// Right Face
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	// Back Face
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	// Left Face
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	// Top Face
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	// Bottom Face
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void hutThree()
{
	// Code
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, texture_wall3);
	glPushMatrix();
		gluQuadricTexture(quadric, GLU_TRUE);
		glTranslatef(0.0f, 0.3f, 0.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(quadric, 1.5f, 0.0f, 1.5f, 30, 30);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	

	// Walls
	glBindTexture(GL_TEXTURE_2D, texture_wall2);
	glPushMatrix();
		gluQuadricTexture(quadric, GLU_TRUE);
		glTranslatef(0.0f, -0.8f, 0.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(quadric, 1.0f, 1.1f, 1.2f, 30, 30);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_LIGHTING);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
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

	if (texture_roof)
	{
		glDeleteTextures(1, &texture_roof);
	}

	if (texture_wall)
	{
		glDeleteTextures(1, &texture_wall);
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log File Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
