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

// Eagle
GLUquadric* quadric = NULL;
GLfloat angleRotateSceneY = 0.0f;
GLfloat angleWings = 0.0f;
GLboolean flagForward = 1;
GLboolean flagReverse = 0;

GLuint featherTexture_Head;
GLuint featherTexture_Eye;
GLuint featherTexture_Neck;
GLuint featherTexture_Body;
GLuint featherTexture_Tail;
GLuint featherTexture_Leg;
GLuint featherTexture_Wing;

/*
GLfloat lightAmbientZero[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightDiffuseZero[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecularZero[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPositionZero[] = { 100.0f, 100.0f, 100.0f, 0.0f };

GLfloat lightAmbientOne[] = { 0.960f, 0.780f, 0.102f, 1.0f };
GLfloat lightDiffuseOne[] = { 0.960f, 0.780f, 0.102f, 1.0f };
GLfloat lightSpecularOne[] = { 0.960f, 0.780f, 0.102f, 1.0f };
GLfloat lightPositionOne[] = {-10.0f, 10.0f, 10.0f, 0.0f};

GLfloat lightAmbientTwo[] = { 0.545f, 0.270f, 0.074f, 1.0f };
GLfloat lightDiffuseTwo[] = { 0.545f, 0.270f, 0.074f, 1.0f };
GLfloat lightSpecularTwo[] = { 0.545f, 0.270f, 0.074f, 1.0f };
GLfloat lightPositionTwo[] = { 0.0f, 10.0f, 10.0f, 1.0f };

//GLfloat lightAmbientThree[] = { 0.788f, 0.572f, 0.396f, 1.0f };
//GLfloat lightDiffuseThree[] = { 0.788f, 0.572f, 0.396f, 1.0f };
//GLfloat lightSpecularThree[] = { 0.788f, 0.572f, 0.396f, 1.0f };
//GLfloat lightPositionThree[] = { 0.0f, 0.0f, 10.0f, 0.0f };

GLfloat materialAmbientOne[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialDiffuseOne[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecularOne[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininessOne = 150.0f;
*/

GLfloat Light_Model_Ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat Light_Model_Local_Viewer[] = { 0.0f };

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
		fprintf(gpFile, "LoadGLTexture() For featherTexture_Head FAILED.\n");
		uninitialize();
	}
	else if (iRetVal == -6)
	{
		fprintf(gpFile, "LoadGLTexture() For featherTexture_Neck FAILED.\n");
		uninitialize();
	}
	else if (iRetVal == -7)
	{
		fprintf(gpFile, "LoadGLTexture() For featherTexture_Body FAILED.\n");
		uninitialize();
	}
	else if (iRetVal == -8)
	{
		fprintf(gpFile, "LoadGLTexture() For featherTexture_Tail FAILED.\n");
		uninitialize();
	}
	else if (iRetVal == -9)
	{
		fprintf(gpFile, "LoadGLTexture() For featherTexture_Eye FAILED.\n");
		uninitialize();
	}
	else if (iRetVal == -10)
	{
		fprintf(gpFile, "LoadGLTexture() For featherTexture_Leg FAILED.\n");
		uninitialize();
	}
	else if (iRetVal == -11)
	{
		fprintf(gpFile, "LoadGLTexture() For featherTexture_Wing FAILED.\n");
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

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Create Quadric
	quadric = gluNewQuadric();			// It Internally Calls Malloc, So Dont Use Inside Display()

	/*
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbientZero);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuseZero);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, lightSpecularZero);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightPositionZero);

	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbientOne);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuseOne);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightSpecularOne);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightPositionOne);
	
	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbientTwo);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuseTwo);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightSpecularTwo);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightPositionTwo);
	//glEnable(GL_LIGHT2);
	*/
	/*
	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbientThree);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuseThree);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightSpecularThree);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightPositionThree);
	//glEnable(GL_LIGHT2);
	*/
	//glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbientOne);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuseOne);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecularOne);
	//glMaterialf(GL_FRONT, GL_SHININESS, materialShininessOne);

	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_Model_Ambient);
	//glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, Light_Model_Local_Viewer);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	if (LoadGLTexture(&featherTexture_Head, MAKEINTRESOURCE(EAGLEFEATHER_HEAD)) == FALSE)
		return -5;
	if (LoadGLTexture(&featherTexture_Neck, MAKEINTRESOURCE(EAGLEFEATHER_NECK)) == FALSE)
		return -6;
	if (LoadGLTexture(&featherTexture_Body, MAKEINTRESOURCE(EAGLEFEATHER_BODY)) == FALSE)
		return -7;
	if (LoadGLTexture(&featherTexture_Tail, MAKEINTRESOURCE(EAGLEFEATHER_TAIL)) == FALSE)
		return -8;
	if (LoadGLTexture(&featherTexture_Eye, MAKEINTRESOURCE(EAGLEFEATHER_EYE)) == FALSE)
		return -9;
	if (LoadGLTexture(&featherTexture_Leg, MAKEINTRESOURCE(EAGLEFEATHER_LEG)) == FALSE)
		return -10;
	if (LoadGLTexture(&featherTexture_Wing, MAKEINTRESOURCE(EAGLEFEATHER_WING)) == FALSE)
		return -11;

	glEnable(GL_TEXTURE_2D);

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
	void Eagle(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -12.0f);


	glRotatef(angleRotateSceneY , 0.0f, 1.0f, 0.0f);
	

	// Eagle
	Eagle();

	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	angleRotateSceneY += 1.0f;

	if (angleWings <= 45.0f && flagForward == 1)
	{
		angleWings += 0.9f;
	}
	else
	{
		flagReverse = 1;
		flagForward = 0;
	}


	if (angleWings >= 0.0f && flagReverse == 1)
	{
		angleWings -= 0.9f;
	}
	else
	{
		flagReverse = 0;
		flagForward = 1;
	}
}

void Eagle(void)
{
	// Code
	glPushMatrix();
		
		// Head
		glBindTexture(GL_TEXTURE_2D, featherTexture_Head);
		gluQuadricTexture(quadric, GL_TRUE);
		glPushMatrix();
			glTranslatef(-1.5f, 1.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			gluSphere(quadric, 0.4f, 20, 20);
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);

		// Eyes
		glBindTexture(GL_TEXTURE_2D, featherTexture_Eye);
		glPushMatrix();
			glTranslatef(-1.5f, 1.0f, 0.0f);
			glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(-10.0f, 0.0f, 0.0f, 1.0f);
			glTranslatef(-0.38f, 0.0f, 0.0f);
			gluSphere(quadric, 0.05f, 5, 5);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-1.5f, 1.0f, 0.0f);
			glRotatef(-45.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(-10.0f, 0.0f, 0.0f, 1.0f);
			glTranslatef(-0.38f, 0.0f, 0.0f);
			gluSphere(quadric, 0.05f, 5, 5);
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);

		// Trunk
		//glEnable(GL_LIGHT1);
		glColor3f(0.905f, 0.766f, 0.437f);
		glPushMatrix();
			glTranslatef(-1.9f, 1.0f, 0.0f);
			glPushMatrix();
				glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
				gluQuadricNormals(quadric, GLU_SMOOTH);
				gluCylinder(quadric, 0.1f, 0.05f, 0.2f, 5, 5);
				glTranslatef(0.0f, -0.05f, 0.0f);
				glScalef(1.0f, 1.3f, 1.5f);
				glColor3f(0.905f, 0.662f, 0.376f);
				gluQuadricNormals(quadric, GLU_SMOOTH);
				gluSphere(quadric, 0.1f, 5, 5);
			glPopMatrix();
			glTranslatef(-0.18f, 0.03f, 0.0f);
			glPushMatrix();
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);
				glColor3f(0.905f, 0.766f, 0.437f);
				gluQuadricNormals(quadric, GLU_SMOOTH);
				gluCylinder(quadric, 0.05f, 0.01f, 0.2f, 4, 4);
			glPopMatrix();
		glPopMatrix();
		//glDisable(GL_LIGHT1);
		glColor3f(1.0f, 1.0f, 1.0f);

		// Neck
		glBindTexture(GL_TEXTURE_2D, featherTexture_Neck);
		gluQuadricTexture(quadric, GL_TRUE);
		glPushMatrix();
			glTranslatef(-1.45f, 1.0f, 0.0f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glRotatef(28.0f, 0.0f, 1.0f, 0.0f);
			gluCylinder(quadric, 0.3f, 0.5f, 1.0f, 20, 20);
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);

		// Body
		//glEnable(GL_LIGHT2);
		glBindTexture(GL_TEXTURE_2D, featherTexture_Body);
		glPushMatrix();
			glScalef(1.5f, 0.7f, 0.7f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
			gluSphere(quadric, 1.0f, 20, 20);
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);
		
		// Wings
		glPushMatrix();
			glTranslatef(0.5f, 0.0f, 0.0f);
			glScalef(0.5f, 0.5f, 0.5f);
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			glRotatef(70.0f, 1.0f, 0.0f, 0.0f);
			glPushMatrix();

				for (GLint i = 0; i < 2; i++, angleWings *= -1)
				{

					glPushMatrix();
					glBindTexture(GL_TEXTURE_2D, featherTexture_Body);
						glTranslatef(0.5f, 0.0f, 0.0f);
						glRotatef(angleWings, 0.0f, 1.0f, 0.0f);
						glBegin(GL_POLYGON);
							glTexCoord3f(0.0f, 3.0f, 0.0f);
							glVertex3f(0.0f, 3.0f, 0.0f);
							glTexCoord3f(3.0f, 3.0f, 0.0f);
							glVertex3f(3.0f, 3.0f, 0.0f);
							glTexCoord3f(4.0f, 2.5f, 0.0f);
							glVertex3f(4.0f, 2.5f, 0.0f);
							glTexCoord3f(4.5f, 2.0f, 0.0f);
							glVertex3f(4.5f, 2.0f, 0.0f);
							glTexCoord3f(2.0f, 1.5f, 0.0f);
							glVertex3f(2.0f, 1.5f, 0.0f);
							glTexCoord3f(0.0f, 0.0f, 0.0f);
							glVertex3f(0.0f, 0.0f, 0.0f);
						glEnd();
						glBindTexture(GL_TEXTURE_2D, 0);

						
						GLfloat xScale = 1.0f, yScale = 1.0f;
						GLfloat zRotate = -5.0f;
						for (GLint i = 0; i < 7; i++, xScale -= 0.08f, yScale -= 0.03f, zRotate -= 0.25f)
						{
							glScalef(xScale, yScale, 1.0f);
							glBindTexture(GL_TEXTURE_2D, featherTexture_Wing);
							glBegin(GL_POLYGON);
								glTexCoord3f(0.0f, 3.0f, 0.0f);
								glVertex3f(0.0f, 3.0f, 0.0f);
								glTexCoord3f(12.0f, 0.3f, 0.0f);
								glVertex3f(12.0f, 0.3f, 0.0f);
								glTexCoord3f(10.8f, 0.3f, 0.0f);
								glVertex3f(10.8f, 0.3f, 0.0f);
								glTexCoord3f(8.6f, 0.5f, 0.0f);
								glVertex3f(8.6f, 0.5f, 0.0f);
								glTexCoord3f(6.4f, 0.9f, 0.0f);
								glVertex3f(6.4f, 0.9f, 0.0f);
							glEnd();
							glBindTexture(GL_TEXTURE_2D, 0);
							glRotatef(zRotate, 0.0f, 0.0f, 1.0f);
						}
					glPopMatrix();
					
					glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
				}
			glPopMatrix();
		glPopMatrix();
		//glDisable(GL_LIGHT2);

		// Legs
		glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, featherTexture_Leg);
			glPushMatrix();
				glTranslatef(0.3f, -0.35f, 0.0f);
				glScalef(2.0f, 1.0f, 1.2f);
				gluSphere(quadric, 0.5f, 20, 20);
			glPopMatrix();
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindTexture(GL_TEXTURE_2D, featherTexture_Neck);
			glTranslatef(0.4f, -0.4f, 0.2f);
			glPushMatrix();
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				gluCylinder(quadric, 0.3f, 0.12f, 0.8f, 20, 20);
			glPopMatrix();
			glBindTexture(GL_TEXTURE_2D, 0);
			glColor3f(0.905f, 0.662f, 0.376f);
			glTranslatef(0.0f, -0.6f, 0.0f);
			glPushMatrix();
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				gluCylinder(quadric, 0.15f, 0.05f, 0.4f, 20, 20);
			glPopMatrix();
			glTranslatef(0.0f, -0.4f, 0.0f);
			glRotatef(20.0f, 0.0f, 0.0f, 1.0f);
			glPushMatrix();
			//glEnable(GL_LIGHT1);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
				gluCylinder(quadric, 0.05f, 0.02f, 0.3f, 10, 10);
				glRotatef(160.0f, 0.0f, 1.0f, 0.0f);
				gluCylinder(quadric, 0.05f, 0.02f, 0.3f, 10, 10);
				glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
				gluCylinder(quadric, 0.05f, 0.02f, 0.3f, 10, 10);
			glPopMatrix();
			//glDisable(GL_LIGHT1);
		glPopMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);

		glBindTexture(GL_TEXTURE_2D, featherTexture_Neck);
		glPushMatrix();
			glTranslatef(0.4f, -0.4f, -0.2f);
			glPushMatrix();
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				gluCylinder(quadric, 0.3f, 0.12f, 0.8f, 20, 20);
			glPopMatrix();
			glBindTexture(GL_TEXTURE_2D, 0);
			glColor3f(0.905f, 0.662f, 0.376f);
			glTranslatef(0.0f, -0.6f, 0.0f);
			glPushMatrix();
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				gluCylinder(quadric, 0.15f, 0.05f, 0.4f, 20, 20);
			glPopMatrix();
			glTranslatef(0.0f, -0.4f, 0.0f);
			glRotatef(20.0f, 0.0f, 0.0f, 1.0f);
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
			//glEnable(GL_LIGHT1);
			glPushMatrix();
				glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
				gluQuadricNormals(quadric, GLU_SMOOTH);
				gluQuadricOrientation(quadric, GLU_OUTSIDE);
				gluCylinder(quadric, 0.05f, 0.02f, 0.3f, 10, 10);
				glRotatef(160.0f, 0.0f, 1.0f, 0.0f);
				gluQuadricNormals(quadric, GLU_FLAT);
				gluCylinder(quadric, 0.05f, 0.02f, 0.3f, 10, 10);
				glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
				gluQuadricNormals(quadric, GLU_FLAT);
				gluCylinder(quadric, 0.05f, 0.02f, 0.3f, 10, 10);
			glPopMatrix();
			//glDisable(GL_LIGHT1);
		glPopMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);

		// Tail
		glBindTexture(GL_TEXTURE_2D, featherTexture_Tail);
		glPushMatrix();
			glTranslatef(0.5f, 0.0f, 0.0f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
			gluPartialDisk(quadric, 0.0f, 3.0f, 5, 10, 70.0f, 40.0f);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

	if (featherTexture_Wing)
	{
		glDeleteTextures(1, &featherTexture_Wing);
		featherTexture_Wing = NULL;
	}

	if (featherTexture_Leg)
	{
		glDeleteTextures(1, &featherTexture_Leg);
		featherTexture_Leg = NULL;
	}

	if (featherTexture_Eye)
	{
		glDeleteTextures(1, &featherTexture_Eye);
		featherTexture_Eye = NULL;
	}

	if (featherTexture_Tail)
	{
		glDeleteTextures(1, &featherTexture_Tail);
		featherTexture_Tail = NULL;
	}

	if (featherTexture_Body)
	{
		glDeleteTextures(1, &featherTexture_Body);
		featherTexture_Body = NULL;
	}

	if (featherTexture_Neck)
	{
		glDeleteTextures(1, &featherTexture_Neck);
		featherTexture_Neck = NULL;
	}

	if (featherTexture_Head)
	{
		glDeleteTextures(1, &featherTexture_Head);
		featherTexture_Head = NULL;
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
