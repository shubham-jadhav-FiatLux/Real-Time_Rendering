// Header Files
#include <Windows.h>
#include "OGL.h"
#include <stdio.h>			// For File I/O
#include <stdlib.h>			// for exit()

#define _USE_MATH_DEFINES 1
#include <math.h>
#include <Mmsystem.h>

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

//Global Variable Declarations
FILE *gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
BOOL gbFullscreen = FALSE;
float day = 0.0f;
float year = 0.0f;
GLUquadric *quadric = NULL;
float x = 0.0f;
float y = 0.0f;
float i = 0.0f;
int noOfRotation = 0;
GLuint texture_stone;

/*
GLfloat lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 0.0f};
GLfloat lightSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightPosition2[] = { 0.0f, 0.0f, 0.0f, 1.0f };

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 0.5f, 0.2f, 0.7f, 1.0f };
GLfloat materialSpecular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat materialShininess = 128.0f;
*/

// Entry-Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Declarations
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);
	void ToggleFullscreen(void);

	// Local variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	BOOL bDone = FALSE;
	int iRetVal = 0;

	// Code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation Of Log File Failed. Exiting ... "), TEXT("File I/O Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File successfully Opened.\n");
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

	// Register WNDCLASSEX Structure
	RegisterClassEx(&wndclass);

	// Create The Window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("OpenGL Window"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(GetSystemMetrics(SM_CXSCREEN) - WIN_WIDTH) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - WIN_HEIGHT) / 2,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	// Initilize
	iRetVal = initialize();
	if (iRetVal == -1)
	{
		fprintf(gpFile, "ChoosePixelFormat() Failed.\n");
		uninitialize();
	}
	else if (iRetVal == -2)
	{
		fprintf(gpFile, "SetPixelformat() Failed.\n");
		uninitialize();
	}
	else if (iRetVal == -3)
	{
		fprintf(gpFile, "Create OpenGL Context Failed.\n");
		uninitialize();
	}
	else if (iRetVal == -4)
	{
		fprintf(gpFile, "Making OpenGL Context As Current Context Failed.\n");
		uninitialize();
	}
	else if (iRetVal == -5)
	{
		fprintf(gpFile, "LoadGLTexture() FOR STONE TEXTURE FAILED.\n");
		uninitialize();
	}
	else
		fprintf(gpFile, "Succeed.\n");

	// Show Window
	ShowWindow(hwnd, iCmdShow);

	ToggleFullscreen();
	
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
	case WM_CREATE:
		//PlaySound(TEXT("abc.wav"), NULL, SND_SYNC | SND_NODEFAULT);
		break;

	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		PlaySound(TEXT("Audio.wav"), NULL, SND_ASYNC);
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

		default :
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

	// Initialization Of PIXELFORMATDESCRIPTOR Structure
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;			// 24 Can Also Be Used Instead Of 32

	// Get DC
	ghdc = GetDC(ghwnd);

	// Choose Pixel Format
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		return - 1;
	}

	// Set The Choosen Pixel Format
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
		return -2;

	// Create OpenGL Rendering Context
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
		return -3;

	// Make The Rendering Context As The Current Context
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
		return -4;

	// Here Starts OpenGL Code
	// Clear The Screen Using black Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Depth Related Changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Create Quadric
	quadric = gluNewQuadric();			// It Internally Calls Malloc, So Dont Use Inside Display()

	if (LoadGLTexture(&texture_stone, MAKEINTRESOURCE(IDBITMAP_STONE)) == FALSE)
		return -5;

	// Enabling The Texture
	glEnable(GL_TEXTURE_2D);

	/*
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glEnable(GL_LIGHT1);
	*/
	/*
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);

	glEnable(GL_LIGHTING);
	*/

	// Warmup resize() Call
	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void resize(int width, int height)
{
	// Code
	if (height == 0)
		height = 1;						// To Avoid 'Divide By 0' Error In Future

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// Function Declarations
	void Mercury(float);
	void Venus(float);
	void Earth(float);
	void Mars(float);
	void Jupiter(float);
	void Saturn(float);
	void Uranus(float);
	void Neptune(float);

	void Asteroids(float, float);

	// Local variable Declarations
	float shade = 1.0f;
	//float radius = 7.0f;

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// View TransFormation
	if (noOfRotation < 780)
		gluLookAt(0.0f, x, y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	else //if (noOfRotation < 25000)
		gluLookAt(x , 3.0f, y , 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	//else
		//gluLookAt(0.0f, 3.0f, 11.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	
	// Save Camera Matrix
	glPushMatrix();

	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);			// Beautification 1

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1.0f, 0.27f, 0.0f);				// beautification 2

	
	glBindTexture(GL_TEXTURE_2D, texture_stone);
	gluQuadricTexture(quadric, GL_TRUE);
	
	// Draw Sphere (Sun)
	gluSphere(quadric, 0.75f, 30, 30);

	// Restore The Saved Camera Matrix (Pop)
	glPopMatrix();

	// Mercury
	Mercury(1.0f);

	// Venus
	Venus(1.5f);

	// Earth
	Earth(2.0f);

	// Mars
	Mars(2.5f);

	// Astorides Ring
	Asteroids(2.8f, 3.2f);

	// Jupiter
	Jupiter(3.5f);

	// Saturn
	Saturn(4.0f);

	// Uranus
	Uranus(4.5f);

	// Neptune
	Neptune(5.0f);

	// Astorides Ring
	Asteroids(6.0f, 7.0f);

	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	year = (year + 0.1f);
	day = (day + 0.5f);

	if (noOfRotation < 2700)
	{
		x = (11.0f * cos(i * M_PI / 180.0f));
		y = (11.0f * sin(i * M_PI / 180.0f));
		i += 0.1f;
	}
	noOfRotation++;
}

void Mercury(float OrbitRadius)
{
	// Function Declarations
	void DrawOrbit(float);

	// Code
	// Save The Current camera Matrix
	glPushMatrix();

	DrawOrbit(OrbitRadius);

	glRotatef((GLfloat)year * 5, 0.0f, 1.0f, 0.0f);

	// To Translate For Mercury
	glTranslatef(OrbitRadius, 0.0f, 0.0f);

	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);			// Beautification 3

	// Self Rotation Of Earth
	glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);

	// Draw Earth
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(0.5f,0.5f, 0.5f);				// Beautification 4

	// Draw Sphere
	gluSphere(quadric, 0.1f, 10, 10);

	glPopMatrix();
}

void Venus(float OrbitRadius)
{
	// Function Declarations
	void DrawOrbit(float);

	// Code
	// Save The Current camera Matrix
	glPushMatrix();

	DrawOrbit(OrbitRadius);

	glRotatef((GLfloat)year * 3, 0.0f, 1.0f, 0.0f);

	// To Translate For Mercury
	glTranslatef(0.0f, 0.0f, OrbitRadius);

	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);			// Beautification 3

	// Self Rotation Of Earth
	glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);

	// Draw Earth
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1.0f, 0.6f, 0.0f);				// Beautification 4

	// Draw Sphere
	gluSphere(quadric, 0.13f, 10, 10);

	glPopMatrix();
}

void Earth(float OrbitRadius)
{
	// Function Declarations
	void DrawOrbit(float);
	void Moon(void);

	// Code
	// Save The Current camera Matrix
	glPushMatrix();

	DrawOrbit(OrbitRadius);

	glRotatef((GLfloat)year, 0.0f, 1.0f, 0.0f);

	// To Translate For Mercury
	glTranslatef(OrbitRadius, 0.0f, 0.0f);

	Moon();

	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);			// Beautification 3

	// Self Rotation Of Earth
	glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);

	// Draw Earth
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glColor3f(0.0f, 0.74f, 1.0f);				// Beautification 4

	// Draw Sphere
	gluSphere(quadric, 0.2f, 10, 10);

	glPopMatrix();
}

void Mars(float OrbitRadius)
{
	// Function Declarations
	void DrawOrbit(float);

	// Code
	// Save The Current camera Matrix
	glPushMatrix();

	DrawOrbit(OrbitRadius);

	glRotatef((GLfloat)year * 2, 0.0f, 1.0f, 0.0f);

	// To Translate For Mercury
	glTranslatef(-OrbitRadius, 0.0f, 0.0f);

	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);			// Beautification 3

	// Self Rotation Of Earth
	glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);

	// Draw Earth
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1.0f, 0.4f, 0.0f);				// Beautification 4

	// Draw Sphere
	gluSphere(quadric, 0.15f, 15, 15);

	glPopMatrix();
}

void Jupiter(float OrbitRadius)
{
	// Function Declarations
	void DrawOrbit(float);

	// Code
	// Save The Current camera Matrix
	glPushMatrix();

	DrawOrbit(OrbitRadius);

	glRotatef((GLfloat)year * 0.5f, 0.0f, 1.0f, 0.0f);

	// To Translate For Mercury
	glTranslatef(OrbitRadius, 0.0f, 0.0f);

	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);			// Beautification 3

	// Self Rotation Of Earth
	glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);

	// Draw Earth
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1.0f, 0.70f, 0.27f);				// Beautification 4

	// Draw Sphere
	gluSphere(quadric, 0.4f, 50, 50);

	// Draw Ring
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	
	glColor3f(1.0f, 0.50f, 0.27f);
	for (int i = 0; i < 360; i++)
		glVertex3f((0.50f * cos(i * M_PI / 180.0f)), 0.0f, (0.45f * sin(i * M_PI / 180.0f)));

	glEnd();

	glPopMatrix();
}

void Saturn(float OrbitRadius)
{
	// Function Declarations
	void DrawOrbit(float);

	// Code
	// Save The Current camera Matrix
	glPushMatrix();

	DrawOrbit(OrbitRadius);

	glRotatef((GLfloat)year * 1.5, 0.0f, 1.0f, 0.0f);

	// To Translate For Mercury
	glTranslatef(-OrbitRadius, 0.0f, 0.0f);

	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);			// Beautification 3

	// Self Rotation Of Earth
	glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);

	// Draw Earth
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1.0f,	0.85f, 0.72f);				// Beautification 4

	// Draw Sphere
	gluSphere(quadric, 0.3f, 40, 40);

	// Draw Ring
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	glBegin(GL_POINTS);

	glColor3f(1.0f, 0.65f, 0.52f);
	for (int i = 0; i < 360; i++)
	{
		glVertex3f((0.45f * cos(i * M_PI / 180.0f)), 0.0f, (0.45f * sin(i * M_PI / 180.0f)));
		glVertex3f((0.47f * cos(i * M_PI / 180.0f)), 0.0f, (0.45f * sin(i * M_PI / 180.0f)));
		glVertex3f((0.49f * cos(i * M_PI / 180.0f)), 0.0f, (0.45f * sin(i * M_PI / 180.0f)));
		glVertex3f((0.51f * cos(i * M_PI / 180.0f)), 0.0f, (0.45f * sin(i * M_PI / 180.0f)));
		glVertex3f((0.53f * cos(i * M_PI / 180.0f)), 0.0f, (0.45f * sin(i * M_PI / 180.0f)));
	}
	glEnd();

	glPopMatrix();
}

void Uranus(float OrbitRadius)
{
	// Function Declarations
	void DrawOrbit(float);

	// Code
	// Save The Current camera Matrix
	glPushMatrix();

	DrawOrbit(OrbitRadius);

	glRotatef((GLfloat)year * 3, 0.0f, 1.0f, 0.0f);

	// To Translate For Mercury
	glTranslatef(0.0f, 0.0f, -OrbitRadius);

	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);			// Beautification 3

	// Self Rotation Of Earth
	glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);

	// Draw Earth
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(0.58f, 0.87f, 0.81f);				// Beautification 4

	// Draw Sphere
	gluSphere(quadric, 0.25f, 35, 35);

	// Draw Ring
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	glBegin(GL_POINTS);

	glColor3f(0.58f, 0.87f, 0.81f);
	for (int i = 0; i < 360; i++)
	{
		glVertex3f((0.35f * cos(i * M_PI / 180.0f)), 0.0f, (0.45f * sin(i * M_PI / 180.0f)));
	}
	glEnd();

	glPopMatrix();
}

void Neptune(float OrbitRadius)
{
	// Function Declarations
	void DrawOrbit(float);

	// Code
	// Save The Current camera Matrix
	glPushMatrix();

	DrawOrbit(OrbitRadius);

	glRotatef((GLfloat)year, 0.0f, 1.0f, 0.0f);

	// To Translate For Mercury
	glTranslatef(0.0f, 0.0f, OrbitRadius);

	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);			// Beautification 3

	// Self Rotation Of Earth
	glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);

	// Draw Earth
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(0.29f, 0.0f, 0.50f);				// Beautification 4

	// Draw Sphere
	gluSphere(quadric, 0.22f, 30, 30);

	// Draw Ring
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	glBegin(GL_POINTS);

	glColor3f(0.29f, 0.0f, 0.50f);
	for (int i = 0; i < 360; i++)
	{
		glVertex3f((0.30f * cos(i * M_PI / 180.0f)), 0.0f, (0.45f * sin(i * M_PI / 180.0f)));
		glVertex3f((0.31f * cos(i * M_PI / 180.0f)), 0.0f, (0.45f * sin(i * M_PI / 180.0f)));
		glVertex3f((0.32f * cos(i * M_PI / 180.0f)), 0.0f, (0.45f * sin(i * M_PI / 180.0f)));
	}
	glEnd();

	glPopMatrix();
}

void Moon(void)
{
	// Code
	glPushMatrix();
	glRotatef((GLfloat)year * 3, 0.0f, 1.0f, 0.0f);

	// To Translate For Mercury
	glTranslatef(0.3f, 0.0f, 0.0f);

	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);			// Beautification 3

	// Self Rotation Of Earth
	glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);

	// Draw Earth
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1.0f, 1.0f, 1.0f);				// Beautification 4

	// Draw Sphere
	gluSphere(quadric, 0.05f, 10, 10);

	glPopMatrix();
}

void Asteroids(float InnerRadius, float OuterRadius)
{
	int flag = 0;
	// Code
	// Save The Current camera Matrix
	

	for (int i = 0; i < 360; i += 5)
	{
		glPushMatrix();
		
		glTranslatef(InnerRadius * cos(i * M_PI / 180.0f), 0.0f, InnerRadius * sin(i * M_PI / 180.0f));
		glScalef(0.75f, 0.75f, 0.75f);
		glBegin(GL_TRIANGLES);
		if (flag == 0)
		{
			// Front Face
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);		// Apex
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(-0.05f, -0.05f, 0.05f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(0.05f, -0.05f, 0.05f);

			// Right Face
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);		// Apex
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(0.05f, -0.05f, 0.05f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(0.05f, -0.05f, -0.05f);

			// Back Face
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);		// Apex
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(0.05f, -0.05f, -0.05f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(-0.05f, -0.05f, -0.05f);

			// Left Face
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);		// Apex
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(-0.05f, -0.05f, -0.05f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(-0.05f, -0.05f, 0.05f);

			flag = 1;
		}
		else
		{
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(-0.05f, -0.05f, 0.0f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(0.05f, -0.05f, 0.0f);

			flag = 0;
		}
		glEnd();

		glPopMatrix();
	}

	for (int i = 2; i < 360; i += 5)
	{
		glPushMatrix();

		glTranslatef((InnerRadius + 0.15f) * cos(i * M_PI / 180.0f), 0.0f, (InnerRadius + 0.15f) * sin(i * M_PI / 180.0f));
		glScalef(0.75f, 0.75f, 0.75f);
		glBegin(GL_TRIANGLES);
		if (flag == 2)
		{
			// Front Face
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);		// Apex
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(-0.05f, -0.05f, 0.05f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(0.05f, -0.05f, 0.05f);

			// Right Face
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);		// Apex
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(0.05f, -0.05f, 0.05f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(0.05f, -0.05f, -0.05f);

			// Back Face
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);		// Apex
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(0.05f, -0.05f, -0.05f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(-0.05f, -0.05f, -0.05f);

			// Left Face
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);		// Apex
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(-0.05f, -0.05f, -0.05f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(-0.05f, -0.05f, 0.05f);

			flag = 0;
		}
		else
		{
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(-0.05f, -0.05f, 0.0f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(0.05f, -0.05f, 0.0f);

			flag++;
		}
		glEnd();

		glPopMatrix();
	}

	for (int i = 4; i < 360; i += 5)
	{
		glPushMatrix();

		glTranslatef((InnerRadius + 0.3f) * cos(i * M_PI / 180.0f), 0.0f, (InnerRadius + 0.3f) * sin(i * M_PI / 180.0f));
		glRotatef(i* M_PI / 180.0f, 1.0f, 0.0f, 0.0f);
		glScalef(0.75f, 0.75f, 0.75f);
		glBegin(GL_TRIANGLES);
		if (flag == 0)
		{
			// Front Face
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);		// Apex
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(-0.05f, -0.05f, 0.05f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(0.05f, -0.05f, 0.05f);

			// Right Face
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);		// Apex
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(0.05f, -0.05f, 0.05f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(0.05f, -0.05f, -0.05f);

			// Back Face
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);		// Apex
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(0.05f, -0.05f, -0.05f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(-0.05f, -0.05f, -0.05f);

			// Left Face
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);		// Apex
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(-0.05f, -0.05f, -0.05f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(-0.05f, -0.05f, 0.05f);

			flag = 1;
		}
		else
		{
			glColor3f(0.6f, 0.16f, 0.16f);
			glVertex3f(0.0f, 0.05f, 0.0f);
			glColor3f(0.41f, 0.41f, 0.41f);
			glVertex3f(-0.05f, -0.05f, 0.0f);
			glColor3f(0.18f, 0.30f, 0.30f);
			glVertex3f(0.05f, -0.05f, 0.0f);

			flag = 0;
		}
		glEnd();

		glPopMatrix();
	}
}

void DrawOrbit(float radius)
{
	// Code
	glPointSize(0.5f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 1.0f);

	for (int i = 0; i < 360; i++)
		glVertex3f((radius * cos(i * M_PI / 180.0f)), 0.0f, (radius * sin(i * M_PI / 180.0f)));

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

	if (texture_stone)
	{
		glDeleteTextures(1, &texture_stone);
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log FileSuccessfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
