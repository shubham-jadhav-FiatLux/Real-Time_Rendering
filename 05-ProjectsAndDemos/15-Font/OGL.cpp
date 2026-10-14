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

GLUquadric* quadric = NULL;
GLuint texture_smoke;
const GLfloat speedOfScene = 1.0f;
GLfloat angleRotateScene = 0.0f;
float xAeroplane = -3.5f;
int flagAeroplane = 1;

GLfloat lightAmbient[] = {0.043f, 0.4f, 0.137f, 1.0f};
GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightPosition[] = {10.0f, 5.0f, 5.0f, 1.0f};

// Font Array
GLint nFontList[4];

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
	void SetupRC1(void);
	void SetupRC2(void);
	void SetupRC3(void);
	void SetupRC4(void);

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

	//glEnable(GL_AUTO_NORMAL);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Create Quadric
	quadric = gluNewQuadric();			// It Internally Calls Malloc, So Dont Use Inside Display()

	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT1);

	if (LoadGLTexture(&texture_smoke, MAKEINTRESOURCE(IDBITMAP_SMOKE)) == FALSE)
		return -5;

	glEnable(GL_TEXTURE_2D);

	//Font
	SetupRC1();
	SetupRC2();
	SetupRC3();
	SetupRC4();

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
	void Aeroplane(int);
	void renderingAstromedicompFont(void);
	void renderingHullGroupFont(void);
	void renderingPostCreditSceneMe(void);
	void renderingPostCreditSceneGL(void);
	void renderingPostCreditSceneSir(void);
	void renderingPostCreditScenePoem(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -4.0f);
	//glRotatef(angleRotateScene, 1.0f, 1.0f, 0.0f);
	
	//renderingAstromedicompFont();
	//renderingHullGroupFont();
	//renderingPostCreditSceneMe();
	//renderingPostCreditSceneGL();
	//renderingPostCreditSceneSir();
	renderingPostCreditScenePoem();

	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	angleRotateScene += speedOfScene;

	if (xAeroplane <= 5.0f && flagAeroplane == 1)
		xAeroplane += speedOfScene * 0.01f;
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

void SetupRC1(void)
{
	// Local Variable Declarations
	GLYPHMETRICSFLOAT agmf_title[128];
	HFONT hFont = NULL;
	LOGFONT logFont;

	// Code
	// setup the Font Characteristics
	logFont.lfHeight = 100;
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = FW_EXTRALIGHT;
	logFont.lfItalic = FALSE;
	logFont.lfUnderline = FALSE;
	logFont.lfCharSet = ANSI_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = DEFAULT_QUALITY;
	logFont.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logFont.lfFaceName, "BALLADE");

	// Create the font and Display list
	hFont = CreateFontIndirect(&logFont);
	SelectObject(ghdc, hFont);

	// Create display lists for glyphs 0 through 128
	nFontList[0] = glGenLists(128);
	//wglUseFontOutlines(ghdc, 0, 128, nFontList, 0.0f, 0.3f, WGL_FONT_POLYGONS, agmf_title);
	wglUseFontBitmaps(ghdc, 0, 128, nFontList[0]);
	DeleteObject(hFont);
}

void SetupRC2(void)
{
	// Local Variable Declarations
	GLYPHMETRICSFLOAT agmf_title[128];
	HFONT hFont = NULL;
	LOGFONT logFont;

	// Code
	// setup the Font Characteristics
	logFont.lfHeight = 200;
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = FW_EXTRALIGHT;
	logFont.lfItalic = FALSE;
	logFont.lfUnderline = FALSE;
	logFont.lfCharSet = ANSI_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = DEFAULT_QUALITY;
	logFont.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logFont.lfFaceName, "The Artisan Marker");

	// Create the font and Display list
	hFont = CreateFontIndirect(&logFont);
	SelectObject(ghdc, hFont);

	// Create display lists for glyphs 0 through 128
	nFontList[1] = glGenLists(128);
	//wglUseFontOutlines(ghdc, 0, 128, nFontList, 0.0f, 0.3f, WGL_FONT_POLYGONS, agmf_title);
	wglUseFontBitmaps(ghdc, 0, 128, nFontList[1]);
	DeleteObject(hFont);
}

void SetupRC3(void)
{
	// Local Variable Declarations
	GLYPHMETRICSFLOAT agmf_title[128];
	HFONT hFont = NULL;
	LOGFONT logFont;

	// Code
	// setup the Font Characteristics
	logFont.lfHeight = 60;
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = FW_EXTRALIGHT;
	logFont.lfItalic = FALSE;
	logFont.lfUnderline = FALSE;
	logFont.lfCharSet = ANSI_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = DEFAULT_QUALITY;
	logFont.lfPitchAndFamily = DEFAULT_PITCH;
	//strcpy(logFont.lfFaceName, "CROCHET PATTERN");
	strcpy(logFont.lfFaceName, "Marathi-Lekhani");

	// Create the font and Display list
	hFont = CreateFontIndirect(&logFont);
	SelectObject(ghdc, hFont);

	// Create display lists for glyphs 0 through 128
	nFontList[2] = glGenLists(128);
	//wglUseFontOutlines(ghdc, 0, 128, nFontList, 0.0f, 0.3f, WGL_FONT_POLYGONS, agmf_title);
	wglUseFontBitmaps(ghdc, 0, 128, nFontList[2]);
	DeleteObject(hFont);
}

void SetupRC4(void)
{
	// Local Variable Declarations
	GLYPHMETRICSFLOAT agmf_title[128];
	HFONT hFont = NULL;
	LOGFONT logFont;

	// Code
	// setup the Font Characteristics
	logFont.lfHeight = 140;
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = FW_EXTRALIGHT;
	logFont.lfItalic = FALSE;
	logFont.lfUnderline = FALSE;
	logFont.lfCharSet = ANSI_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = DEFAULT_QUALITY;
	logFont.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logFont.lfFaceName, "Magnific Chaos Personal Use Regular");

	// Create the font and Display list
	hFont = CreateFontIndirect(&logFont);
	SelectObject(ghdc, hFont);

	// Create display lists for glyphs 0 through 128
	nFontList[3] = glGenLists(128);
	//wglUseFontOutlines(ghdc, 0, 128, nFontList, 0.0f, 0.3f, WGL_FONT_POLYGONS, agmf_title);
	wglUseFontBitmaps(ghdc, 0, 128, nFontList[3]);
	DeleteObject(hFont);
}

void renderingAstromedicompFont(void)
{
	// Function Declarations
	void Aeroplane(int);

	// Code
	// Aeroplane
	glPushMatrix();
		glTranslatef(xAeroplane, 0.0f, 0.0f);
		Aeroplane(2);
	glPopMatrix();

	// Astromedicomp
	glLoadIdentity();
	glColor3f(0.298f, 0.733f, 0.090f);
	glTranslatef(-6.0f, -0.4f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[0]);
	glCallLists(15, GL_UNSIGNED_BYTE, "ASTROMEDICOMP'S");
}

void renderingHullGroupFont(void)
{
	// Code
	glColor3f(1.0f, 0.0f, 0.0f);
	glLoadIdentity();
	glTranslatef(-5.0f, 0.3f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[1]);
	glCallLists(10, GL_UNSIGNED_BYTE, "HULL GROUP");

	glLoadIdentity();
	glTranslatef(-3.8f, -1.2f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[1]);
	glCallLists(8, GL_UNSIGNED_BYTE, "presents");
}

void renderingPostCreditScenePoem(void)
{
	// Code
	glColor3f(1.0f, 0.0f, 0.0f);
	glLoadIdentity();
	glTranslatef(-2.5f, 0.0f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[2]);
	glCallLists(10, GL_UNSIGNED_BYTE, TEXT("mn vdahay  vdhay"));

	glColor3f(1.0f, 1.0f, 1.0f);
	glLoadIdentity();
	glTranslatef(-5.5f, -2.5f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[2]);
	glCallLists(24, GL_UNSIGNED_BYTE, "bihenabai chaudhir");
}

void renderingPostCreditSceneMe(void)
{
	// Code
	glColor3f(1.0f, 0.0f, 0.0f);
	glLoadIdentity();
	glTranslatef(-2.5f, 2.0f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[2]);
	glCallLists(11, GL_UNSIGNED_BYTE, "Devloped By");

	glColor3f(1.0f, 1.0f, 1.0f);
	glLoadIdentity();
	glTranslatef(-5.5f, -0.5f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(21, GL_UNSIGNED_BYTE, "Shubham Balaso Jadhav");
}

void renderingPostCreditSceneGL(void)
{
	// Code
	glColor3f(1.0f, 0.0f, 0.0f);
	glLoadIdentity();
	glTranslatef(-3.0f, 2.5f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[2]);
	glCallLists(17, GL_UNSIGNED_BYTE, "Special Thanks To");

	glColor3f(1.0f, 1.0f, 1.0f);
	glLoadIdentity();
	glTranslatef(-2.7f, 1.0f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(12, GL_UNSIGNED_BYTE, "Our  Captain");

	glLoadIdentity();
	glTranslatef(-3.7f, 0.0f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(15, GL_UNSIGNED_BYTE, "Ashwini Bhutare");

	glLoadIdentity();
	glTranslatef(-1.0f, -1.2f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(1, GL_UNSIGNED_BYTE, "&");

	glLoadIdentity();
	glTranslatef(-5.3f, -2.0f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(22, GL_UNSIGNED_BYTE, "All Hull Group Members");
}

void renderingPostCreditSceneSir(void)
{
	// Code
	glColor3f(1.0f, 0.0f, 0.0f);
	glLoadIdentity();
	glTranslatef(-4.0f, 2.0f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[2]);
	glCallLists(28, GL_UNSIGNED_BYTE, "Thank You For Everything ...");

	glColor3f(1.0f, 1.0f, 1.0f);
	glLoadIdentity();
	glTranslatef(-5.8f, 0.0f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(22, GL_UNSIGNED_BYTE, "Dr. Rama Gokhale Madam");

	glLoadIdentity();
	glTranslatef(-1.5f, -1.2f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(1, GL_UNSIGNED_BYTE, "&");

	glLoadIdentity();
	glTranslatef(-5.2f, -2.5f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(24, GL_UNSIGNED_BYTE, "Dr. Vijay D. Gokhale Sir");
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

	
	// Smoke
	glColor3f(0.0f, 1.0f, 0.0f);				// White
	
	glBindTexture(GL_TEXTURE_2D, texture_smoke);
	gluQuadricTexture(quadric, GLU_TRUE);
	glPushMatrix();
		glTranslatef(-0.6f, 0.0f, 0.0f);
		gluPartialDisk(quadric, 0.1f, 0.5f, 5, 5, 245.0f, 50.0f);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture_smoke);
	gluQuadricTexture(quadric, GLU_TRUE);
	glPushMatrix();
		glTranslatef(-0.6f, 0.0f, 0.0f);
		glRotatef(-180.0f, 0.0f, 1.0f, 0.0f);
		gluPartialDisk(quadric, 0.1f, 0.5f, 5, 5, 65.0f, 50.0f);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// Black Quad
	glBegin(GL_POLYGON);
		glColor3f(0.0f, 0.0f, 0.0f);			// Black
		glVertex3f(10.0f, 5.0f, -0.1f);
		glVertex3f(-1.0f, 5.0f, -0.1f);
		glVertex3f(-1.0f, -5.0f, -0.1f);
		glVertex3f(10.0f, -5.0f, -0.1f);
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

	if (texture_smoke)
	{
		glDeleteTextures(1, &texture_smoke);
		texture_smoke = NULL;
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
