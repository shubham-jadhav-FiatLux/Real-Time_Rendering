// Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>
#include <Mmsystem.h>

#define _USE_MATH_DEFINES 1
#include <math.h>

// Win32 Library For PlaySound()
#pragma comment(lib, "Winmm.lib")

// OpenGL Header Files
#include <GL/gl.h>
#include <GL/glu.h>

// Macro's
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

// Textures
GLuint texture_gold;
GLuint texture_digits;
GLuint texture_letter_j;
GLuint texture_letter_u;
GLuint texture_letter_l;
GLuint texture_letter_y;
GLuint texture_paper;
GLuint texture_paper_roll;
GLuint texture_disk;

int gKeyPressed1 = 0;
int gKeyPressed2 = 0;
int gKeyPressed3 = 0;
int gKeyPressed4 = 0;
int gKeyPressed5 = 0;
int flagDisassemble = 0;
int flagText = 0;

float angle = 0.0f;
float angleRotate = 90.0f;

float x1 = 0.0f;
float y = 0.0f;
float z1 = 30.0f;
float T1 = 0.0f;
float T2 = 0.0f;
float T3 = 0.0f;
//float T4 = 0.0f;
float T5 = 0.0f;
float T6 = 0.0f;
float up = -1.5f;


GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, -1.0f, 2.0f, 1.0f };

GLfloat MaterialAmbientOne[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat MaterialDiffuseOne[] = { 0.8547f, 0.756f, 0.486f, 1.0f };
GLfloat MaterialSpecularOne[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat MaterialShininessOne = 128.0f;

GLfloat LightAmbientZero[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightDiffuseZero[] = { 0.945f, 0.898f, 0.674f, 1.0f };	
GLfloat LightSpecularZero[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightPositionZero[] = { -6.0f, 0.0f, 0.0f, 1.0f };

GLfloat LightAmbientOne[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightDiffuseOne[] = { 0.945f, 0.898f, 0.674f, 1.0f };			
GLfloat LightSpecularOne[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat LightPositionOne[] = { 6.0f, 0.0f, 0.0f, 1.0f };
/*
GLfloat MaterialAmbientTwo[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat MaterialDiffuseTwo[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat MaterialSpecularTwo[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat MaterialShininessTwo = 50.0f;
*/
HFONT hFont = NULL;
GLint nFontList;
LOGFONT logfont;

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
		fprintf(gpFile, "LoadGLTexture() FOR GOLD FAILED.\n");
		uninitialize();
	}
	else if (iRetVal == -6)
	{
		fprintf(gpFile, "LoadGLTexture() FOR DIGITS FAILED.\n");
		uninitialize();
	}

	// Remaining Error Handling For Textures
	// ...
	// ...

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
		
		case '1':
				gKeyPressed1 = 1;
			break;

		case '2':
			gKeyPressed2 = 2;
			break;

		case '3':
			gKeyPressed3 = 3;
			break;

		case '4':
			gKeyPressed4 = 4;
			break;

		case '5':
			gKeyPressed5 = 5;
			break;

		case '6':
			flagDisassemble = 1;
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
	BOOL LoadGLTextureLetters(GLuint*, TCHAR[]);
	void SetupRC(void);

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

	//depthrelated changes
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbientZero);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuseZero);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecularZero);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPositionZero);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2, GL_AMBIENT, LightAmbientOne);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, LightDiffuseOne);
	glLightfv(GL_LIGHT2, GL_SPECULAR, LightSpecularOne);
	glLightfv(GL_LIGHT2, GL_POSITION, LightPositionOne);
	glEnable(GL_LIGHT2);

	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbientOne);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuseOne);
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecularOne);
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininessOne);
	glEnable(GL_LIGHTING);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Create Quadric
	quadric = gluNewQuadric();			// It Internally Calls Malloc, So Dont Use Inside Display()

	if (LoadGLTextureLetters(&texture_digits, MAKEINTRESOURCE(IDBITMAP_DIGITS)) == FALSE)
		return -5;

	if (LoadGLTextureLetters(&texture_letter_j, MAKEINTRESOURCE(IDBITMAP_LETTER_J)) == FALSE)
		return -6;

	if (LoadGLTextureLetters(&texture_letter_u, MAKEINTRESOURCE(IDBITMAP_LETTER_U)) == FALSE)
		return -7;

	if (LoadGLTextureLetters(&texture_letter_l, MAKEINTRESOURCE(IDBITMAP_LETTER_L)) == FALSE)
		return -8;

	if (LoadGLTextureLetters(&texture_letter_y, MAKEINTRESOURCE(IDBITMAP_LETTER_Y)) == FALSE)
		return -9;
	
	if (LoadGLTexture(&texture_gold, MAKEINTRESOURCE(IDBITMAP_GOLD)) == FALSE)
		return -10;

	if (LoadGLTexture(&texture_paper, MAKEINTRESOURCE(IDBITMAP_PAPER)) == FALSE)
		return -11;

	if (LoadGLTexture(&texture_paper_roll, MAKEINTRESOURCE(IDBITMAP_PAPERROLL)) == FALSE)
		return -12;

	if (LoadGLTexture(&texture_disk, MAKEINTRESOURCE(IDBITMAP_DISK)) == FALSE)
		return -13;

	glEnable(GL_LIGHTING);

	// Font
	SetupRC();

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
	void Cylinder(float, float, float, float);
	void Disk(float, float);
	void TextOnQuad(void);
	 
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// View Trandformations
	gluLookAt(x1, y, z1, 0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f);

	// Save Camera Matrix
	glPushMatrix();

	// Bind With The Texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_digits);
	
	if (gKeyPressed1 == 1)
		Cylinder(-0.7f, 0.4f, 0.4f, 0.3f);
	else
	{
		glPushMatrix();
		glRotatef(angle, 1.0f, 0.0f, 0.0f);
		gluQuadricTexture(quadric, GL_TRUE);
		Cylinder(-0.7f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	}
	
	glBindTexture(GL_TEXTURE_2D, texture_letter_j);
	if (gKeyPressed2 == 2)
		Cylinder(-0.35f, 0.4f, 0.4f, 0.3f);
	else
	{
		glPushMatrix();
		glRotatef(angle, 1.0f, 0.0f, 0.0f);
		gluQuadricTexture(quadric, TRUE);
		Cylinder(-0.35f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	}
		
	glBindTexture(GL_TEXTURE_2D, texture_letter_u);
	if (gKeyPressed3 == 3)
		Cylinder(0.0f, 0.4f, 0.4f, 0.3f);
	else
	{
		glPushMatrix();
		glRotatef(angle, 1.0f, 0.0f, 0.0f);
		gluQuadricTexture(quadric, TRUE);
		Cylinder(0.0f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	}
		
	glBindTexture(GL_TEXTURE_2D, texture_letter_l);
	if (gKeyPressed4 == 4)
		Cylinder(0.35f, 0.4f, 0.4f, 0.3f);
	else
	{
		glPushMatrix();
		glRotatef(angle, 1.0f, 0.0f, 0.0f);
		gluQuadricTexture(quadric, TRUE);
		Cylinder(0.35f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	}
	
	glBindTexture(GL_TEXTURE_2D, texture_letter_y);
	if (gKeyPressed5 == 5)
		Cylinder(0.7f, 0.4f, 0.4f, 0.3f);
	else
	{
		glPushMatrix();
		glRotatef(angle, 1.0f, 0.0f, 0.0f);
		gluQuadricTexture(quadric, TRUE);
		Cylinder(0.7f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_gold);
	//gluQuadricTexture(quadric, TRUE);
	Cylinder(0.0f, 0.35f, 0.35f, 2.1f);

	// Edges Of Criptex
	Cylinder(-1.0f, 0.43f, 0.43f, 0.1f);
	Cylinder(-0.87f, 0.43f, 0.43f, 0.1f);
	Cylinder(0.87f, 0.43f, 0.43f, 0.1f);
	Cylinder(1.0f, 0.43f, 0.43f, 0.1f);

	glPushMatrix();
	if (flagDisassemble == 1)
		glTranslatef(T1, 0.0f, 0.0f);
	Cylinder(-1.2f, 0.0f, 0.3f, 0.2f);
	glPopMatrix();

	// Disk
	glPushMatrix();
	if (flagDisassemble == 1)
		glTranslatef(T2, 0.0f, 0.0f);
	//glBindTexture(GL_TEXTURE_2D, texture_gold);
	gluQuadricTexture(quadric, GL_TRUE);
	Disk(-1.05f, 0.43f);
	glPopMatrix();

	glPushMatrix();
	if (flagDisassemble == 1)
		glTranslatef(T3, 0.0f, 0.0f);
	
	glBindTexture(GL_TEXTURE_2D, texture_disk);
	gluQuadricTexture(quadric, GL_TRUE);
	Disk(-1.07f, 0.39f);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_gold);
	glPushMatrix();
	if (flagDisassemble == 1)
		glTranslatef(-T1, 0.0f, 0.0f);
	Cylinder(1.2f, 0.3f, 0.0f, 0.2f);
	glPopMatrix();

	glPushMatrix();
	if (flagDisassemble == 1)
		glTranslatef(-T2, 0.0f, 0.0f);
	
	gluQuadricTexture(quadric, GL_TRUE);
	Disk(1.05f, 0.43f);
	glPopMatrix();

	glPushMatrix();
	if (flagDisassemble == 1)
		glTranslatef(-T3, 0.0f, 0.0f);
	
	glBindTexture(GL_TEXTURE_2D, texture_disk);
	gluQuadricTexture(quadric, GL_TRUE);
	Disk(1.07f, 0.39f);
	glBindTexture(GL_TEXTURE_2D, texture_gold);
	glPopMatrix();

	glPopMatrix();

	if (flagText == 1)
	{
		TextOnQuad();
	}

	SwapBuffers(ghdc);
}

void update(void)
{
	// Local Variable declarations
	float radius = 5.0f;
	
	// Code
	angle += 1.0f;

	if (z1 >= 5.0f)
		z1 -= 0.1f;
	else if (angleRotate < 811.0f)
	{
		x1 = radius * cos(angleRotate * (M_PI / 180.0f));
		z1 = radius * sin(angleRotate * (M_PI / 180.0f));
		angleRotate += 1.0f;
	}

	if (flagDisassemble == 1)
	{
		if (flagDisassemble == 1 && T1 >= -1.0f)
			T1 -= 0.003f;
		else
		{
			
			if (y < 1.6f)
				y += 0.01f;
			else
				flagText = 1;
		}
		if (flagDisassemble == 1 && T2 >= -0.7f)
		{
			T2 = T1 + 0.01f;
			T5 = -T1 - 0.01f;
		}
		
		if (flagDisassemble == 1 && T3 >= -0.4f)
		{
			T3 = T1 + 0.02f;
			T6 = -T1 - 0.02f;
		}
	}

	if (flagText == 1 && up <= 3.0f)
		up += 0.01f;
		
}

void Cylinder(float position, float topR, float baseR, float height)
{
	// Code
	glPushMatrix();

		glTranslatef((position - (height/2)), 0.0f, 0.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		gluQuadricNormals(quadric, GLU_SMOOTH);
		gluQuadricTexture(quadric, GL_TRUE);
		gluCylinder(quadric, topR, baseR, height, 50, 50);
	
	glPopMatrix();
}

void Disk(float position,float baseR)
{
	// Code
	glPushMatrix();

		glTranslatef((position), 0.0f, 0.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		gluQuadricNormals(quadric, GLU_SMOOTH);
		gluQuadricTexture(quadric, GL_TRUE);
		gluDisk(quadric, 0, baseR, 10000, 10);

	glPopMatrix();
}

void TextOnQuad()
{
	// Function Declarations
	void renderingScene(void);

	// Code
	glPushMatrix();
		glTranslatef(-1.0f, up, -1.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		gluQuadricNormals(quadric, GLU_SMOOTH);
		gluCylinder(quadric, 0.1f, 0.1f, 2.0f, 10, 10);

		glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, texture_paper_roll);
			glTranslatef(0.0f, 0.0f, 0.29f);
			gluQuadricTexture(quadric, GL_TRUE);
			gluQuadricNormals(quadric, GLU_SMOOTH);
			gluCylinder(quadric, 0.13f, 0.13f, 1.4f, 10, 10);

			glTranslatef(0.0f, 0.0f, 0.03f);
			glBindTexture(GL_TEXTURE_2D, texture_paper_roll);
			gluQuadricTexture(quadric, GL_TRUE);
			gluQuadricNormals(quadric, GLU_SMOOTH);
			gluCylinder(quadric, 0.14f, 0.14f, 1.32f, 10, 10);
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, texture_gold);
		gluSphere(quadric, 0.2f, 10, 10);
	glPopMatrix();


	glPushMatrix();

		glTranslatef(1.0f, up, -1.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		gluSphere(quadric, 0.2f, 10, 10);

	glPopMatrix();


	glPushMatrix();
		glTranslatef(0.0f, up, -1.0f);
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		glBindTexture(GL_TEXTURE_2D, texture_paper);
		glBegin(GL_QUADS);
			glColor3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(0.7f, 0.1f, 0.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-0.7f, 0.1f, 0.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.7f, -2.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(0.7f, -2.0f, 0.0f);
		glEnd();

	glPopMatrix();

		//glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	renderingScene();
}

void renderingScene(void)
{
	// Code
		glPushMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);

		glPushMatrix();
			glTranslatef(-1.50f, up - 0.8f, -1.0f);
			glRasterPos2i(1, 0);
			glListBase(nFontList);
			glCallLists(7, GL_UNSIGNED_BYTE, "SHUBHAM");
		glPopMatrix();

		/*glPushMatrix();
			glTranslatef(-1.55f, up - 1.0f, -1.0f);
			glRasterPos2i(1, 0);
			glListBase(nFontList);
			glCallLists(8, GL_UNSIGNED_BYTE, "");
		glPopMatrix();
		*/
		glPushMatrix();
			glTranslatef(-1.39f, up - 1.2f, -1.0f);
			glRasterPos2i(1, 0);
			glListBase(nFontList);
			glCallLists(6, GL_UNSIGNED_BYTE, "JADHAV");
		glPopMatrix();

		//wglUseFontBitmaps(ghdc, 0, 128, nFontList);
		glPopMatrix();
}

// setup the Font 
void SetupRC(void)
{
	// Code
	//// Set up the font characteristic
	GLYPHMETRICSFLOAT agmf[128];

	// setup the Font Characteristics
	logfont.lfHeight = 40;
	logfont.lfWidth = 0;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfWeight = FW_ULTRABOLD;
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logfont.lfFaceName, "Hobo Std");

	// Create the font and Display list
	hFont = CreateFontIndirect(&logfont);
	SelectObject(ghdc, hFont);

	// Create display lists for glyphs 0 through 128
	nFontList = glGenLists(128);
	//wglUseFontOutlines(ghdc, 0, 128, nFontList, 0.0f, 0.5f, WGL_FONT_POLYGONS, agmf);
	wglUseFontBitmaps(ghdc, 0, 128, nFontList);
	DeleteObject(hFont);
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

BOOL LoadGLTextureLetters(GLuint* Texture, TCHAR ImageResourceID[])
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

	if (texture_gold)
	{
		glDeleteTextures(1, &texture_gold);
		texture_gold = NULL;
	}

	// Remaining Textures Used
	// ...
	// ...

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
