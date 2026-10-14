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
FILE *gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
static BOOL gbFullscreen = FALSE;

GLUquadric* quadric = NULL;
GLuint textureBamboo;
GLuint textureBambooLeaf;

GLfloat LightAmbientOne[] = { 0.450f, 0.682f, 0.219f, 1.0f };
GLfloat LightDiffuseOne[] = { 0.450f, 0.682f, 0.219f, 1.0f };
GLfloat LightSpecularOne[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPositionOne[] = {150.0f, 0.0f, 150.0f, 0.0f};

GLfloat MaterialAmbientOne[] = {0.450f, 0.682f, 0.219f, 1.0f};
GLfloat MaterialDiffuseOne[] = { 0.450f, 0.682f, 0.219f, 1.0f };
GLfloat MaterialSpecularOne[] = { 0.450f, 0.682f, 0.219f, 1.0f };
GLfloat MaterialShininessOne = 50.0f;

GLfloat xLeafTranslation = 0.0f;
GLfloat leafAngle = 0.0f;
int flagForward = 1;
float x = 0.0f;
float y = 0.0f;
float z = 0.0f;
GLfloat angleRotateScene = 0.0f;

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
	TCHAR szAppName[] = TEXT("DS Project - FFP");
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
		fprintf(gpFile, "LoadGLTexture() For textureBamboo Failed.\n");
		uninitialize();
	}
	else if (iRetVal == -6)
	{
		fprintf(gpFile, "LoadGLTexture() For textureBambooLeaf Failed.\n");
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

		case 'a':
			x -= 0.1f;
			break;

		case 'A':
			x += 0.1f;
			break;

		case 'd':
			y -= 0.1f;
			break;

		case 'D':
			y += 0.1f;
			break;

		case 'w':
		case 'W':
			z -= 0.1f;
			break;

		case 's':
		case 'S':
			z += 0.1f;
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

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbientOne);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuseOne);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecularOne);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPositionOne);
	
	glEnable(GL_LIGHT0);
	
	glEnable(GL_LIGHTING);

	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbientOne);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuseOne);
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecularOne);
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininessOne);
	
	if (LoadGLTexture(&textureBamboo, MAKEINTRESOURCE(IDBITMAP_BAMBOO)) == FALSE)
		return -5;
	if (LoadGLTexture(&textureBambooLeaf, MAKEINTRESOURCE(IDBITMAP_BAMBOOLEAF)) == FALSE)
		return -6;

	glEnable(GL_TEXTURE_2D);

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
	void bambooTree(void);
	void Leaf(void);
	void Branch(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -7.0f);

	glRotatef(angleRotateScene, 0.0f, 1.0f, 0.0f);

	//gluLookAt(x, y, z, 0.0f, 0.0f, -7.0f, 0.0f, 1.0f, 0.0f);

	bambooTree();
	//Leaf();
	//Branch();

	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	// Leaf Position
	xLeafTranslation += 0.1f;

	angleRotateScene += 1.0f;

	// Leaf Movement
	if (leafAngle <= 2 && flagForward == 1)
		leafAngle += 0.07f;
	else if (leafAngle >= -2)
	{
		leafAngle -= 0.07f;
		flagForward = 0;
	}
	else
		flagForward = 1;
}

void bambooTree(void)
{
	// Function Declarations
	void Branch(void);

	// Code
	glPushMatrix();
		
		// Trunk
		glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, textureBamboo);
			glTranslatef(0.0f, -2.0f, 0.0f);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			gluQuadricTexture(quadric, GL_TRUE);
			gluCylinder(quadric, 0.08f, 0.03f, 4.5f, 30, 30);
			glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();
		
		glPushMatrix();
			for (float i = 0.5f, j = 1.0f; i <= 4.5f; i += 0.56f, j -= 0.05f)
			{
				glPushMatrix();
					glTranslatef(0.0f, i-2.0f, 0.0f);
					glRotatef(80.0f, 0.0f, 0.0f, 1.0f);
					glScalef(1.0f, j, 1.0f);
					glRotatef(50.0f, 0.0f, 1.0f, 0.0f);
					glPushMatrix();
						Branch();
					glPopMatrix();
				glPopMatrix();
			}
			for (float i = 0.7f, j = 1.0f; i <= 4.5f; i += 0.56f, j -= 0.05f)
			{
				glPushMatrix();
					glTranslatef(0.0f, i-2.0f, 0.0f);
					glRotatef(-80.0f, 0.0f, 0.0f, 1.0f);
					glScalef(1.0f, j, 1.0f);
					glRotatef(-50.0f, 0.0f, 1.0f, 0.0f);
					glPushMatrix();
						Branch();
					glPopMatrix();
				glPopMatrix();
			}
		glPopMatrix();
		
	glPopMatrix();
}

void Branch(void)
{
	// Function Declarations
	void SingleBranch(void);

	// Code
	glPushMatrix();

		glScalef(0.3f, 0.2f, 0.2f);
		glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, textureBamboo);
			glTranslatef(0.0f, 0.0f, 0.0f);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			gluQuadricTexture(quadric, GL_TRUE);
			gluQuadricNormals(quadric, GL_SMOOTH);
			gluCylinder(quadric, 0.01f, 0.005f, 2.7f, 30, 30);
			glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();

		glPushMatrix();
			for (float i = 0.2f, j = 1.0f; i <= 2.3f; i += 0.7f, j -= 0.2f)
			{
				glPushMatrix();
					glTranslatef(-0.07f, i, 0.0f);
					glRotatef(25.0f, 0.0f, 0.0f, 1.0f);
					glScalef(1.0f, j, 1.0f);
					glPushMatrix();
						SingleBranch();
					glPopMatrix();
				glPopMatrix();
			}
			for (float i = 0.2f, j = 1.0f; i <= 2.3f; i += 0.7f, j -= 0.2f)
			{
				glPushMatrix();
					glTranslatef(0.07f, i, 0.0f);
					glRotatef(-25.0f, 0.0f, 0.0f, 1.0f);
					glScalef(1.0f, j, 1.0f);
					glPushMatrix();
						SingleBranch();
					glPopMatrix();
				glPopMatrix();
			}
		glPopMatrix();

		glPushMatrix();
			glScalef(0.5f, 0.5f, 0.5f);
			glTranslatef(0.0f, 5.5f, 0.0f);
			SingleBranch();
		glPopMatrix();

	glPopMatrix();
}

void SingleBranch(void)
{
	// Function Declarations
	void Leaf(void);
	
	// Code
	// Branch
	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, textureBamboo);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		gluQuadricTexture(quadric, GL_TRUE);
		gluQuadricNormals(quadric, GL_SMOOTH);
		gluCylinder(quadric, 0.01f, 0.005f, 1.8f, 30, 30);
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	// Leaf
	glPushMatrix();
		for (float i = 0.2f, j = 0.2f; i <= 1.62f; i += 0.2f, j -= 0.011f)
		{
			glPushMatrix();
				glTranslatef(-0.07f, i, 0.0f);
				glScalef(0.0f + j - 0.05, j, 1.0f);
				glRotatef(35.0f, 0.0f, 0.0f, 1.0f);
				glPushMatrix();
					glRotatef(leafAngle, 1.0f, 0.0f, 1.0f);
					Leaf();
				glPopMatrix();
			glPopMatrix();
		}
		for (float i = 0.2f, j = 0.2f; i <= 1.62f; i += 0.2f, j -= 0.011f)
		{
			glPushMatrix();
				glTranslatef(0.07f, i, 0.0f);
				glScalef(j - 0.05, j, 1.0f);
				glRotatef(-35.0f, 0.0f, 0.0f, 1.0f);
				glPushMatrix();
					glRotatef(leafAngle, 1.0f, 0.0f, 1.0f);
					Leaf();
				glPopMatrix();
			glPopMatrix();
		}
	glPopMatrix();
}

void Leaf(void)
{
	// Code
	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, textureBambooLeaf);
		glBegin(GL_POLYGON);
			glNormal3f(1.0f, 1.0f, 1.0f);
			glNormal3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(0.5f, 1.0f);
			glVertex3f(0.0f, 1.0f, 0.0f);
			glTexCoord2f(0.1, 0.7f);
			glVertex3f(-0.2f, 0.6f, 0.0f);
			glTexCoord2f(0.0, 0.5f);
			glVertex3f(-0.3f, 0.0f, 0.0f);
			glTexCoord2f(0.5, 0.0f);
			glVertex3f(0.0f, -1.0f, 0.0f);
			glTexCoord2f(0.0, 0.5f);
			glVertex3f(0.3f, 0.0f, 0.0f);
			glTexCoord2f(0.1, 0.7f);
			glVertex3f(0.2f, 0.6f, 0.0f);
			glTexCoord2f(0.5f, 1.0f);
			glVertex3f(0.0f, 1.0f, 0.0f);
		glEnd();
		glBindTexture(0, textureBambooLeaf);
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

	if (textureBambooLeaf)
	{
		glDeleteTextures(1, &textureBambooLeaf);
		textureBambooLeaf = NULL;
	}

	if (textureBamboo)
	{
		glDeleteTextures(1, &textureBamboo);
		textureBamboo = NULL;
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
