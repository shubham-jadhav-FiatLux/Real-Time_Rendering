// Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>
#include <Mmsystem.h>

// OpenGL Header Files
#include <GL/gl.h>
#include <GL/glu.h>

// Win32 Library For PlaySound()
#pragma comment(lib, "Winmm.lib")

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

GLfloat LightAmbientOne[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat LightDiffuseOne[] = { 0.455f, 0.231f, 0.847f, 1.0f };
GLfloat LightSpecularOne[] = { 0.455f, 0.231f, 0.847f, 1.0f };
GLfloat LightPositionOne[] = {0.0f, 0.0f, 10.0f, 1.0f};

GLfloat LightAmbientTwo[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat LightDiffuseTwo[] = { 0.455f, 0.231f, 0.847f, 1.0f };
GLfloat LightSpecularTwo[] = { 0.455f, 0.231f, 0.847f, 1.0f };
GLfloat LightPositionTwo[] = { 0.0f, 0.0f, -10.0f, 1.0f };

GLfloat LightAmbientThree[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat LightDiffuseThree[] = { 0.768f, 0.705f, 0.992f, 1.0f };
GLfloat LightSpecularThree[] = { 0.768f, 0.705f, 0.992f, 1.0f };
GLfloat LightPositionThree[] = { 0.0f, -10.0f, 1000.0f, 1.0f };

GLfloat MaterialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat MaterialDiffuse[] = { 0.768f, 0.705f, 0.992f, 1.0f };
GLfloat MaterialSpecular[] = { 0.768f, 0.705f, 0.992f, 1.0f };
GLfloat MaterialShininess = 0.0f;

GLuint texture_feather;
float angle = 0.0f;
float angleWings = 0.0f;
int flagF = 1;
int flagR = 0;
int gKeyPressed = 0;

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
	else if (iRetVal == -5)
	{
		fprintf(gpFile, "LoadGLTexture() for feather Failed.\n");
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
		PlaySound(MAKEINTRESOURCE(MYAUDIO), NULL, SND_RESOURCE | SND_ASYNC | SND_LOOP);
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

		case 'R':
		case 'r':
			if (gKeyPressed == 0)
				gKeyPressed = 1;
			else
				gKeyPressed = 0;
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


	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbientOne);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuseOne);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecularOne);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPositionOne);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbientTwo);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuseTwo);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecularTwo);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPositionTwo);
	glEnable(GL_LIGHT2);
	/*
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbientThree);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuseThree);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecularThree);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPositionThree);
	//glEnable(GL_LIGHT3);
	*/
	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
	
	glEnable(GL_LIGHTING);

	if (LoadGLTexture(&texture_feather, MAKEINTRESOURCE(IDBITMAP_FEATHER)) == FALSE)
		return -5;

	glEnable(GL_TEXTURE_2D);

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

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// Function Declarations
	void Fulpakhru(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// View Transformation
	gluLookAt(0.0f, 0.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
		glRotatef(angle, 0.0f, 1.0f, 0.0f);
		Fulpakhru();
	glPopMatrix();

	SwapBuffers(ghdc);
}

void update(void)
{
	//static flag = 0;
	// Code
	if (gKeyPressed == 1)
		angle += 0.5f;
	else
		angle = 0.0f;

	if (angleWings <= 45.0f && flagF == 1)
	{
		angleWings += 0.9f;
	}
	else
	{
		flagR = 1;
		flagF = 0;
	}


	if (angleWings >= 0.0f && flagR == 1)
	{
		angleWings -= 0.9f;
	}
	else
	{
		flagR = 0;
		flagF = 1;
	}
}

void Fulpakhru()
{
	// Code
	glPushMatrix();
		glTranslatef(0.0f, 2.0f, 0.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		// Head
		gluSphere(quadric, 0.9f, 50, 50);
	glPopMatrix();
	
	glPushMatrix();
		glDisable(GL_LIGHTING);
		glColor3f(0.0f, 0.0f, 0.0f);
		glTranslatef(-0.5f, 2.0f, 0.7f);
		/// Eyes
		gluSphere(quadric, 0.1, 5, 5);
		glTranslatef(1.0f, 0.0f, 0.0f);
		gluSphere(quadric, 0.1, 5, 5);
		glTranslatef(-0.5f, -0.3f, 0.0f);
		// Mouth
		gluPartialDisk(quadric, 0.0f, 0.35f, 5, 5, 90.0f, 180.0f);
		glEnable(GL_LIGHTING);

		glTranslatef(-0.3f, 1.9f, -0.2f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(quadric, 0.0f, 0.05f, 1.0f, 20, 20);

		glTranslatef(0.6f, 0.0f, 0.0f);
		gluCylinder(quadric, 0.0f, 0.05f, 1.0f, 5, 5);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0.0f, -2.0f, 0.0f);
		glScalef(0.55f, 1.5f, 0.55f); 
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		// Body
		gluSphere(quadric, 2.0f, 50, 50);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0.0f, -1.0f, -0.9f);
		// Front Wings
		glBindTexture(GL_TEXTURE_2D, texture_feather);

		gluQuadricTexture(quadric, GLU_TRUE);
		glPushMatrix();
			glRotatef(angleWings, 0.0f, 1.0f, 0.0f);
			gluPartialDisk(quadric, 0.0f, 5.0f, 5, 10, 30.0f, 50.0f);
			//gluPartialDisk(quadric, 0.0f, 5.0f, 5, 5, 90.0f, 70.0f);
			gluPartialDisk(quadric, 0.0f, 5.0f, 5, 10, 90.0f, 70.0f);
		glPopMatrix();

		glPushMatrix();
			glRotatef(-angleWings, 0.0f, 1.0f, 0.0f);
			gluPartialDisk(quadric, 0.0f, 5.0f, 5, 10, 280.0f, 50.0f);
			gluPartialDisk(quadric, 0.0f, 5.0f, 5, 10, 200.0f,70.0f);
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);
		
		// Back Wings
		glBindTexture(GL_TEXTURE_2D, texture_feather);

		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		gluQuadricTexture(quadric, GLU_TRUE);
		glPushMatrix();
			glRotatef(-angleWings, 0.0f, 1.0f, 0.0f);
			gluPartialDisk(quadric, 0.0f, 5.0f, 5, 5, 30.0f, 50.0f);
			//gluPartialDisk(quadric, 0.0f, 5.0f, 5, 5, 90.0f, 70.0f);
			gluPartialDisk(quadric, 0.0f, 5.0f, 3, 10, 90.0f, 70.0f);
		glPopMatrix();

		glPushMatrix();
			glRotatef(angleWings, 0.0f, 1.0f, 0.0f);
			gluPartialDisk(quadric, 0.0f, 5.0f, 5, 5, 280.0f, 50.0f);
			gluPartialDisk(quadric, 0.0f, 5.0f, 3, 10, 200.0f, 70.0f);
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);
		
	glPopMatrix();
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

	if (texture_feather)
	{
		glDeleteTextures(1, &texture_feather);
		texture_feather = NULL;
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
