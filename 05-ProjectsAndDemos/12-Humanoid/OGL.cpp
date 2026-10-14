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
const GLfloat speedOfScene = 1.0f;
GLfloat rotateScene = 0.0f;
GLfloat angleShoulder = 0.0f;
GLfloat angleElbow = 0.0f;
GLfloat angleLeg = 0.0f;
GLfloat angleLegLower = 0.0f;
GLboolean flagForwardShoulder = 1;
GLboolean flagForwardElbow = 1;
GLboolean flagForwardLeg = 1;
GLboolean flagForwardLegLower = 1;

GLfloat lightAmbientOne[] = { 1.7f, 0.572f, 0.396f, 1.0f };
//GLfloat lightDiffuseOne[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat lightSpecularOne[] = { 0.788f, 0.572f, 0.396f, 1.0f };
//GLfloat lightPositionOne[] = { 30.0f, 5.0f, -10.0f, 0.0f };

GLfloat materialAmbient[] = { 0.5f, 0.5f, 0.5f, 0.0f };
GLfloat materialDiffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininess = 100.0f;

GLuint texture_shirt;
GLuint texture_shirt2;
GLuint texture_pant;
GLuint texture_face;

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
	glClearColor(0.905f, 0.996f, 1.0f, 1.0f);

	// Depth Related Changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbientOne);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuseOne);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightSpecularOne);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, lightPositionOne);
	glEnable(GL_LIGHT0);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);

	glEnable(GL_LIGHTING);

	if (LoadGLTexture(&texture_shirt, MAKEINTRESOURCE(SHIRT)) == FALSE)
		return -5;
	if (LoadGLTexture(&texture_shirt2, MAKEINTRESOURCE(SHIRT2)) == FALSE)
		return -6;
	if (LoadGLTexture(&texture_pant, MAKEINTRESOURCE(PANT)) == FALSE)
		return -7;
	if (LoadGLTexture(&texture_face, MAKEINTRESOURCE(FACE)) == FALSE)
		return -8;

	// Enabling The Texture
	glEnable(GL_TEXTURE_2D);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
	void Humanoid(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 3.0f, -20.0f);

	//glRotatef(rotateScene, 0.0f, 1.0f, 0.0f);

	Humanoid();

	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	rotateScene += speedOfScene;
	// Shoulder
	if (angleShoulder < 20.0f && flagForwardShoulder == 1)
	{
		angleShoulder += speedOfScene * 0.2f;
		flagForwardShoulder = 1;
	}
	else
		flagForwardShoulder = 0;

	if (angleShoulder > -20.0f && flagForwardShoulder == 0)
	{
		angleShoulder -= speedOfScene * 0.2f;
	}
	else
		flagForwardShoulder = 1;

	// Elbow
	if (angleElbow < 30.0f && flagForwardElbow == 1)
	{
		angleElbow += speedOfScene * 0.3f;
		flagForwardElbow = 1;
	}
	else
		flagForwardElbow = 0;

	if (angleElbow > -0.0f && flagForwardElbow == 0)
	{
		angleElbow -= speedOfScene * 0.3f;
	}
	else
		flagForwardElbow = 1;

	// Leg
	if (angleLeg < 20.0f && flagForwardLeg == 1)
	{
		angleLeg += speedOfScene * 0.2f;
		flagForwardLeg = 1;
	}
	else
		flagForwardLeg = 0;

	if (angleLeg > -20.0f && flagForwardLeg == 0)
	{
		angleLeg -= speedOfScene * 0.2f;
	}
	else
		flagForwardLeg = 1;

	if (angleLegLower < 40.0f && flagForwardLegLower == 1)
	{
		angleLegLower += speedOfScene * 0.2f;
	}
	else
		flagForwardLegLower = 0;
	
	if (angleLegLower > 0.0f && flagForwardLegLower == 0)
	{
		angleLegLower -= speedOfScene * 0.2f;
	}
	else
		flagForwardLegLower = 1;

}

void Humanoid(void)
{
	// Function Declarations
	void cube(void);

	// Code
	glPushMatrix();

		// Head
		glPushMatrix();
			glDisable(GL_LIGHTING);
			glBindTexture(GL_TEXTURE_2D, texture_face);
			//glColor3f(1.0f, 0.5f, 0.5f);
			glTranslatef(0.0f, 3.0f, 0.0f);
			glPushMatrix();
				glScalef(0.8f, 1.0f, 0.8f);
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				gluQuadricTexture(quadric, GLU_TRUE);
				gluSphere(quadric, 1.0f, 8, 4);
			glPopMatrix();
			glBindTexture(GL_TEXTURE_2D, 0);

			// hat
			glPushMatrix();
				glColor3f(0.0f, 0.0f, 0.0f);
				glRotatef(-60.0f, 1.0f, 0.0f, 0.0f);
				glPushMatrix();
					glTranslatef(0.0f, 0.0f, 0.1f);
					glScalef(2.0f, 0.2f, 1.8f);
					gluSphere(quadric, 0.3f, 30, 30);
					glScalef(0.9f, 9.2f, 1.1f);
					glTranslatef(0.0f, 0.2f, 0.0f);
					gluSphere(quadric, 0.5f, 8, 10);
				glPopMatrix();
			glPopMatrix();
				
			// Hairs
			glPushMatrix();
					glRotatef(-20.0f, 0.0f, 0.0f, 1.0f);
					glColor3f(0.0f, 0.0f, 0.0f);
					glTranslatef(-0.1f, 0.3f, 0.66f);
					glScalef(0.4f, 0.2f, 0.5f);
					glBegin(GL_POLYGON);
						glVertex3f(0.0f, 0.5f, 0.0f);
						glVertex3f(0.0f, 0.0f, 0.0f);
						glVertex3f(0.5f, 0.0f, 0.0f);
						glVertex3f(1.0f, -0.5f, 0.0f);
						glVertex3f(0.5f, 0.5f, 0.0f);
					glEnd();
					glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
					glTranslatef(-0.6f, -0.4f, 0.1f);
					glBegin(GL_POLYGON);
						glVertex3f(0.0f, 0.5f, 0.0f);
						glVertex3f(0.0f, 0.0f, 0.0f);
						glVertex3f(0.5f, 0.0f, 0.0f);
						glVertex3f(1.0f, -0.5f, 0.0f);
						glVertex3f(0.5f, 0.5f, 0.0f);
					glEnd();
			glPopMatrix();
				
			// Neck
			glPushMatrix();
				glColor3f(0.7f, 0.35f, 0.05f);
				glTranslatef(0.0f, -1.5f, 0.0f);
				glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
				gluCylinder(quadric, 0.5f, 0.35f, 0.6f, 10, 10);
			glPopMatrix();
	
			
			// Ears
			glBindTexture(GL_TEXTURE_2D, texture_face);
			glPushMatrix();
				glColor3f(1.0f, 1.0f, 1.0f);
				glTranslatef(0.7f, -0.1f, 0.2f);
				glScalef(0.8f, 1.5f, 0.5f);
				//glRotatef(-40.0f, 1.0f, 0.0f, 0.0f);
				gluQuadricTexture(quadric, GLU_TRUE);
				gluSphere(quadric, 0.2f, 30, 30);
			glPopMatrix();
			glPushMatrix();
				//glColor3f(0.0f, 0.0f, 0.0f);
				glTranslatef(-0.7f, -0.1f, 0.2f);
				glScalef(0.8f, 1.5f, 0.5f);
				//glRotatef(10.0f, 0.0f, 0.0f, 1.0f);
				gluQuadricTexture(quadric, GLU_TRUE);
				gluSphere(quadric, 0.2f, 30, 30);
			glPopMatrix();
			glBindTexture(GL_TEXTURE_2D, 0);

			glPushMatrix();
				/// Eyes Right
				glColor3f(0.0f, 0.0f, 0.0f);
				glTranslatef(0.3f, -0.2f, 0.75f);
				glPushMatrix();
					glScalef(1.0f, 0.5f, 0.5f);
					gluSphere(quadric, 0.1f, 10, 10);
					// Eyebro
					glColor3f(0.0f, 0.0f, 0.0f);
					glTranslatef(-0.1f, 0.2f, 0.0f);
					glScalef(0.4f, 0.2f, 0.5f);
					glBegin(GL_POLYGON);
						glVertex3f(0.0f, 0.5f, 0.0f);
						glVertex3f(0.0f, 0.0f, 0.0f);
						glVertex3f(0.5f, 0.0f, 0.0f);
						glVertex3f(1.0f, -0.5f, 0.0f);
						glVertex3f(0.5f, 0.5f, 0.0f);
					glEnd();
				glPopMatrix();
				glPushMatrix();
					glColor3f(1.0f, 1.0f, 1.0f);
					glTranslatef(-0.02f, -0.01f, 0.03f);
					gluSphere(quadric, 0.03f, 10, 10);
				glPopMatrix();
			glPopMatrix();

			glPushMatrix();
				/// Eyes Left
				glColor3f(0.0f, 0.0f, 0.0f);
				glTranslatef(-0.3f, -0.2f, 0.75f);
				glPushMatrix();
					glScalef(1.0f, 0.5f, 0.5f);
					gluSphere(quadric, 0.1f, 10, 10);
					// Eyebro
					// Eyebro
					glColor3f(0.0f, 0.0f, 0.0f);
					glTranslatef(0.1f, 0.2f, 0.0f);
					glScalef(0.4f, 0.2f, 0.5f);
					glBegin(GL_POLYGON);
						glVertex3f(0.0f, 0.5f, 0.0f);
						glVertex3f(0.0f, 0.0f, 0.0f);
						glVertex3f(-0.5f, 0.0f, 0.0f);
						glVertex3f(-1.0f, -0.5f, 0.0f);
						glVertex3f(-0.5f, 0.5f, 0.0f);
					glEnd();
				glPopMatrix();
				glColor3f(1.0f, 1.0f, 1.0f);
				glTranslatef(0.02f, -0.01f, 0.03f);
				gluSphere(quadric, 0.03f, 10, 10);
			glPopMatrix();

			glPushMatrix();
				glColor3f(0.7f, 0.35f, 0.05f);
				glTranslatef(0.0f, -0.3f, 0.75f);
				glScalef(0.15f, 0.2f, 1.0f);
				glBegin(GL_QUADS);
					// Front
					glVertex3f(0.3f, 1.0f, 0.0f);
					glVertex3f(-0.3f, 1.0f, 0.0f);
					glVertex3f(-0.4f, -1.0f, 0.0f);
					glVertex3f(0.4f, -1.0f, 0.0f);
					// Right
					glColor3f(0.7f, 0.35f, 0.05f);
					glVertex3f(0.8f, 1.0f, -0.5f);
					glVertex3f(0.3f, 1.0f, 0.0f);
					glVertex3f(0.4f, -1.0f, 0.0f);
					glVertex3f(1.4f, -1.0f, -0.5f);
					// Right
					//glColor3f(0.5f, 0.35f, 0.05f);
					glVertex3f(-0.8f, 1.0f, -0.5f);
					glVertex3f(-0.3f, 1.0f, 0.0f);
					glVertex3f(-0.4f, -1.0f, 0.0f);
					glVertex3f(-1.4f, -1.0f, -0.5f);
				glEnd();
			glPopMatrix();
			
			// Mouth
			glColor3f(0.0f, 0.0f, 0.0f);
			glTranslatef(0.0f, -0.65f, 0.55f);
			glScalef(2.0f, 0.5f, 1.0f);
			gluSphere(quadric, 0.1f, 5, 5);
			//gluPartialDisk(quadric, 0.0f, 0.35f, 5, 5, 110.0f, 140.0f);
				
		glPopMatrix();


		// Chest
		glColor3f(1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, texture_shirt);
		glPushMatrix();
			glScalef(0.8f, 1.0f, 0.6f);
			glTranslatef(0.0f, -0.2f, 0.0f);
			// Front
			glBegin(GL_QUADS);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(2.0f, 2.0f, 1.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-2.0f, 2.0f, 1.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-2.0f, -2.0f, 1.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(2.0f, -2.0f, 1.0f);
			glEnd();
			// Back
			glBegin(GL_QUADS);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(2.0f, 2.0f, -1.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-2.0f, 2.0f, -1.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-2.0f, -2.0f, -1.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(2.0f, -2.0f, -1.0f);
			glEnd();
			// Right
			glBegin(GL_QUADS);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(2.0f, 2.0f, -1.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(2.0f, 2.0f, 1.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(2.0f, -2.0f, 1.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(2.0f, -2.0f, -1.0f);
			glEnd();
			// Left
			glBegin(GL_QUADS);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(-2.0f, 2.0f, 1.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-2.0f, 2.0f, -1.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-2.0f, -2.0f, -1.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(-2.0f, -2.0f, 1.0f);
			glEnd();
			// Bottom Front
			glBegin(GL_QUADS);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(2.0f, -2.0f, 1.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-2.0f, -2.0f, 1.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-1.0f, -3.0f, 0.5f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(1.0f, -3.0f, 0.5f);
			glEnd();
			// Bottom Back
			glBegin(GL_QUADS);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(-2.0f, -2.0f, -1.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(2.0f, -2.0f, -1.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(1.0f, -3.0f, -0.5f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(-1.0f, -3.0f, -0.5f);
			glEnd();
			// Bottom Right
			glBegin(GL_QUADS);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(2.0f, -2.0f, -1.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(2.0f, -2.0f, 1.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(1.0f, -3.0f, 0.5f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(1.0f, -3.0f, -0.5f);
			glEnd();
			// Bottom Left
			glBegin(GL_QUADS);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(-2.0f, -2.0f, 1.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-2.0f, -2.0f, -1.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-1.0f, -3.0f, -0.5f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(-1.0f, -3.0f, 0.5f);
			glEnd();

			glTranslatef(0.0f, 0.5f, 0.0f);
			glScalef(0.75f, 0.6f, 0.45f);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			gluSphere(quadric, 3.0f, 5, 9);
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);



		// Shoulder
		//glPushMatrix();
			//glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
			glPushMatrix();
				glTranslatef(2.0f, 1.5f, 0.0f);
				//glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				glBindTexture(GL_TEXTURE_2D, texture_shirt2);
				gluQuadricTexture(quadric, GLU_TRUE);
				gluSphere(quadric, 0.5f, 10, 10);
				//************************** Shoulder Translation ***********************
				glPushMatrix();
					glRotatef(10.0f, 0.0f, 0.0f, 1.0f);
					glRotatef(-angleShoulder, 1.0f, 0.0f, 0.0f);
					glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
					glTranslatef(0.4f, 0.0f, 0.0f);
					//glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
					glPushMatrix();
						glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
						gluCylinder(quadric, 0.45f, 0.3f, 1.8f, 10, 10);
					glPopMatrix();
					glTranslatef(2.0f, 0.0f, 0.0f);
					//glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
					gluSphere(quadric, 0.25f, 10, 10);
					// *********************** Forearm Translation ***********************
					glPushMatrix();
						glRotatef(-angleElbow, 0.0f, 1.0f, 1.0f);
						glTranslatef(0.23f, 0.0f, 0.0f);
						glPushMatrix();
							glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
							gluCylinder(quadric, 0.25f, 0.17f, 2.0f, 10, 10);
						glPopMatrix();
						glBindTexture(GL_TEXTURE_2D, 0);

						glBindTexture(GL_TEXTURE_2D, texture_face);
						glPushMatrix();
							glTranslatef(2.3f, 0.0f, 0.0f);
							glScalef(0.3f, 0.1f, 0.23f);
							cube();
							glRotatef(-40.0f, 0.0f, 0.0f,1.0f);
							glTranslatef(1.6f, 0.4f, 0.8f);
							glScalef(0.5f, 0.1f, 0.2f);
							cube();
							glTranslatef(0.0f, 0.0f, -2.6f);
							cube();
							glTranslatef(0.0f, 0.0f, -2.6f);
							cube();
							glTranslatef(0.0f, 0.0f, -2.6f);
							cube();
							glTranslatef(-2.0f, 0.0f, 8.6f);
							glRotatef(100.0f, 0.0f, 1.0f, 1.0f);
							glScalef(2.0f, 1.6f, 1.6f);
							
							cube();
						glPopMatrix();
						glBindTexture(GL_TEXTURE_2D, 0);
					glPopMatrix();
					// ********************** Forearm End *******************************
				glPopMatrix();
				// ************************** Shoulder End ****************************
			glPopMatrix();
		//glPopMatrix();
		glPushMatrix();
			glTranslatef(-2.0f, 1.5f, 0.0f);
			//glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glBindTexture(GL_TEXTURE_2D, texture_shirt2);
			gluSphere(quadric, 0.5f, 10, 10);
			//************************** Shoulder Translation ***********************
			glPushMatrix();
				glRotatef(-10.0f, 0.0f, 0.0f, 1.0f);
				glRotatef(angleShoulder, 1.0f, 0.0f, 0.0f);
				glRotatef(90.0f, 0.0f, 0.0f, 1.0f);

				glTranslatef(-0.4f, 0.0f, 0.0f);
				glPushMatrix();
					glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
					gluCylinder(quadric, 0.45f, 0.3f, 1.8f, 10, 10);
				glPopMatrix();
				glTranslatef(-2.0f, 0.0f, 0.0f);
				//glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				gluSphere(quadric, 0.25f, 10, 10);
				// *********************** Forearm Translation ***********************
				glPushMatrix();
					glRotatef(angleElbow, 0.0f, 1.0f, 1.0f);
					glTranslatef(-0.23f, 0.0f, 0.0f);
					glPushMatrix();
						glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
						gluCylinder(quadric, 0.25f, 0.17f, 2.0f, 10, 10);
					glPopMatrix();
					glBindTexture(GL_TEXTURE_2D, 0);
					glBindTexture(GL_TEXTURE_2D, texture_face);
					glPushMatrix();
						glTranslatef(-2.3f, 0.0f, 0.0f);
						glScalef(0.3f, 0.1f, 0.23f);
						cube();
						glRotatef(40.0f, 0.0f, 0.0f, 1.0f);
						glTranslatef(-1.6f, 0.4f, 0.8f);
						glScalef(0.5f, 0.1f, 0.2f);
						cube();
						glTranslatef(0.0f, 0.0f, -2.6f);
						cube();
						glTranslatef(0.0f, 0.0f, -2.6f);
						cube();
						glTranslatef(0.0f, 0.0f, -2.6f);
						cube();
						glTranslatef(2.0f, 0.0f, 8.6f);
						glRotatef(100.0f, 0.0f, 1.0f, 1.0f);
						glScalef(2.0f, 1.6f, 1.6f);
						cube();
						
					glPopMatrix();
					glBindTexture(GL_TEXTURE_2D, 0);
				glPopMatrix();
			glPopMatrix();
		glPopMatrix();

		// BottomHalf
		glBindTexture(GL_TEXTURE_2D, texture_shirt);
		glPushMatrix();
			glTranslatef(0.0f, -3.1f, 0.0f);
			glScalef(1.6f, 0.7f, 1.0f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			gluSphere(quadric, 1.0f, 10, 10);
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);


		glEnable(GL_LIGHTING);
		glBindTexture(GL_TEXTURE_2D, texture_pant);
		// Legs
		glPushMatrix();
			glTranslatef(-0.7f, -3.5f, 0.0f);
			glRotatef(-angleLeg, 1.0f, 0.0f, 0.0f);
			glPushMatrix();
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				gluCylinder(quadric, 0.7f, 0.5f, 2.0f, 5, 5);
			glPopMatrix();
			glTranslatef(0.0f, -2.3f, 0.0f);
			gluSphere(quadric, 0.4f, 10, 10);
			glTranslatef(0.0f, -0.3f, 0.0f);
			glRotatef(angleLegLower, 1.0f, 0.0f, 0.0f);
			glPushMatrix();
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				gluCylinder(quadric, 0.4f, 0.2f, 2.0f, 5, 5);
			glPopMatrix();
			glBindTexture(GL_TEXTURE_2D, 0);

			glBindTexture(GL_TEXTURE_2D, texture_face);
			glTranslatef(0.0f, -2.0f, 0.0f);
			glPushMatrix();
				glScalef(0.3f, 0.3f, 0.3f);
				glBegin(GL_TRIANGLES);
					// Front Face
					glNormal3f(0.0f, 0.447214f, 0.894428f);
					glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
					glVertex3f(-1.0f, -1.0f, 1.0f);
					glVertex3f(1.0f, -1.0f, 1.0f);

					// Right Face
					glNormal3f(0.894428f, 0.447214f, 0.0f);
					glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
					glVertex3f(1.0f, -1.0f, 1.0f);
					glVertex3f(1.0f, -1.0f, -1.0f);

					// Back Face
					glNormal3f(0.0f, 0.447214f, -0.894428f);
					glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
					glVertex3f(1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, -1.0f);

					// Left Face
					glNormal3f(-0.894428f, 0.447214f, 0.0f);
					glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
					glVertex3f(-1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, 1.0f);

				glEnd();
				glTranslatef(0.0f, -1.5f, 1.0f);
				glScalef(1.0f, 0.5f, 2.0f);
				glBegin(GL_QUADS);
					// Front Face
					glNormal3f(0.0f, 0.0f, 1.0f);
					glVertex3f(1.0f, 1.0f, 1.0f);
					glVertex3f(-1.0f, 1.0f, 1.0f);
					glVertex3f(-1.0f, -1.0f, 1.0f);
					glVertex3f(1.0f, -1.0f, 1.0f);

					// Right Face
					glNormal3f(1.0f, 0.0f, 0.0f);
					glVertex3f(1.0f, 1.0f, -1.0f);
					glVertex3f(1.0f, 1.0f, 1.0f);
					glVertex3f(1.0f, -1.0f, 1.0f);
					glVertex3f(1.0f, -1.0f, -1.0f);

					// Back Face
					glNormal3f(0.0f, 0.0f, -1.0f);
					glVertex3f(-1.0f, 1.0f, -1.0f);
					glVertex3f(1.0f, 1.0f, -1.0f);
					glVertex3f(1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, -1.0f);

					// Left Face
					glNormal3f(-1.0f, 0.0f, 0.0f);
					glVertex3f(-1.0f, 1.0f, 1.0f);
					glVertex3f(-1.0f, 1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, 1.0f);

					// Top Face
					glNormal3f(0.0f, 1.0f, 0.0f);
					glVertex3f(1.0f, 1.0f, -1.0f);
					glVertex3f(-1.0f, 1.0f, -1.0f);
					glVertex3f(-1.0f, 1.0f, 1.0f);
					glVertex3f(1.0f, 1.0f, 1.0f);

					// Bottom Face
					glNormal3f(0.0f, -1.0f, 0.0f);
					glVertex3f(1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, 1.0f);
					glVertex3f(1.0f, -1.0f, 1.0f);

				glEnd();
			glPopMatrix();
			glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, texture_pant);
		glPushMatrix();
			glTranslatef(0.7f, -3.5f, 0.0f);
			glRotatef(angleLeg, 1.0f, 0.0f, 0.0f);
			glPushMatrix();
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				gluCylinder(quadric, 0.7f, 0.5f, 2.0f, 5, 5);
			glPopMatrix();
			glTranslatef(0.0f, -2.3f, 0.0f);
			gluSphere(quadric, 0.4f, 10, 10);
			glTranslatef(0.0f, -0.3f, 0.0f);
			glRotatef(angleLegLower, 1.0f, 0.0f, 0.0f);
			glPushMatrix();
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				gluCylinder(quadric, 0.4f, 0.2f, 2.0f, 5, 5);
			glPopMatrix();
			glBindTexture(GL_TEXTURE_2D, 0);

			glBindTexture(GL_TEXTURE_2D, texture_face);
			glTranslatef(0.0f, -2.0f, 0.0f);
			glPushMatrix();
				glScalef(0.3f, 0.3f, 0.3f);
				glBegin(GL_TRIANGLES);
					// Front Face
					glNormal3f(0.0f, 0.447214f, 0.894428f);
					glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
					glVertex3f(-1.0f, -1.0f, 1.0f);
					glVertex3f(1.0f, -1.0f, 1.0f);
					// Right Face
					glNormal3f(0.894428f, 0.447214f, 0.0f);
					glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
					glVertex3f(1.0f, -1.0f, 1.0f);
					glVertex3f(1.0f, -1.0f, -1.0f);
					// Back Face
					glNormal3f(0.0f, 0.447214f, -0.894428f);
					glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
					glVertex3f(1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, -1.0f);
					// Left Face
					glNormal3f(-0.894428f, 0.447214f, 0.0f);
					glVertex3f(0.0f, 1.0f, 0.0f);		// Apex
					glVertex3f(-1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, 1.0f);
				glEnd();
				glTranslatef(0.0f, -1.5f, 1.0f);
				glScalef(1.0f, 0.5f, 2.0f);
				glBegin(GL_QUADS);
					// Front Face
					glNormal3f(0.0f, 0.0f, 1.0f);
					glVertex3f(1.0f, 1.0f, 1.0f);
					glVertex3f(-1.0f, 1.0f, 1.0f);
					glVertex3f(-1.0f, -1.0f, 1.0f);
					glVertex3f(1.0f, -1.0f, 1.0f);
					// Right Face
					glNormal3f(1.0f, 0.0f, 0.0f);
					glVertex3f(1.0f, 1.0f, -1.0f);
					glVertex3f(1.0f, 1.0f, 1.0f);
					glVertex3f(1.0f, -1.0f, 1.0f);
					glVertex3f(1.0f, -1.0f, -1.0f);
					// Back Face
					glNormal3f(0.0f, 0.0f, -1.0f);
					glVertex3f(-1.0f, 1.0f, -1.0f);
					glVertex3f(1.0f, 1.0f, -1.0f);
					glVertex3f(1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, -1.0f);
					// Left Face
					glNormal3f(-1.0f, 0.0f, 0.0f);
					glVertex3f(-1.0f, 1.0f, 1.0f);
					glVertex3f(-1.0f, 1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, 1.0f);
					// Top Face
					glNormal3f(0.0f, 1.0f, 0.0f);
					glVertex3f(1.0f, 1.0f, -1.0f);
					glVertex3f(-1.0f, 1.0f, -1.0f);
					glVertex3f(-1.0f, 1.0f, 1.0f);
					glVertex3f(1.0f, 1.0f, 1.0f);
					// Bottom Face
					glNormal3f(0.0f, -1.0f, 0.0f);
					glVertex3f(1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, -1.0f);
					glVertex3f(-1.0f, -1.0f, 1.0f);
					glVertex3f(1.0f, -1.0f, 1.0f);
				glEnd();
			glPopMatrix();
			glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();

	glPopMatrix();
}

void cube(void)
{
	// Code
	glBegin(GL_QUADS);
		// Front Face
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		// Right Face
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		// Back Face
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		// Left Face
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		// Top Face
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		// Bottom Face
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
	glEnd();
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

	if (texture_shirt)
	{
		glDeleteTextures(1, &texture_shirt);
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
