// Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES 1
#include <math.h>

// OpenGL Header Files
#include <GL/gl.h>
#include <GL/glu.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define SPEED_OF_SYSTEM 4.0f
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#define GL_GENERATE_MIPMAP                0x8191
#define I 4
#define J 4
#define K 3
#define degreeToRadian(angle) {angle * M_PI / 180.0f}
#define X 100
#define Z 100
#define QUADSIZE 8.0f
#define XSHIFT (QUADSIZE *2.0f) /X
#define ZSHIFT (QUADSIZE *2.0f) /Z

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

// ************************************* Scene Transformation Related Global Variables *********************************
GLfloat angleOfSceneRotation = 80.0f;

typedef struct SceneParameters
{
	GLboolean startSceneTransition;

}Scene;

GLboolean startSceneTransition[] = {TRUE, FALSE, FALSE, FALSE, FALSE, FALSE , FALSE};
//GLboolean startSceneTransition[] = { FALSE, FALSE, TRUE, FALSE, FALSE, FALSE , FALSE };
//GLboolean startSceneTransition[] = { FALSE, FALSE, FALSE, TRUE, FALSE, FALSE , FALSE};

//GLboolean flagSceneRendering[] = {FALSE, FALSE, FALSE};
GLboolean flagSceneRendering[] = { FALSE, FALSE, FALSE , FALSE, FALSE, FALSE, FALSE, FALSE};

GLboolean flagFirstTimeToStart = TRUE;

static GLboolean flagCounterVariable = 1;
// ----------------------------------------------------------------------------------------------------------------------


// ******************** LookAt variables ***********************
GLfloat xCamera_LookAt = 30.0f;
GLfloat yCamera_LookAt = 10.0f;
GLfloat zCamera_LookAt = 295.0f;

GLfloat xPosObj_LookAt = -250.0f;
GLfloat yPosObj_LookAt = 10.0f;
GLfloat zPosObj_LookAt = 250.0f;

GLfloat xNormal_LookAt = 0.0f;
GLfloat yNormal_LookAt = 1.0f;
GLfloat zNormal_LookAt = 0.0f;

GLboolean flagRotation = 1;
// --------------------------------------------------------------

// ****************************** Cubemap **********************************
GLuint cubemapTexture;
TCHAR* facesCubemap[] =
{
	MAKEINTRESOURCE(CUBE_LEFT), 
	MAKEINTRESOURCE(CUBE_RIGHT), 
	MAKEINTRESOURCE(CUBE_TOP), 
	MAKEINTRESOURCE(CUBE_BOTTOM), 
	MAKEINTRESOURCE(CUBE_FRONT),
	MAKEINTRESOURCE(CUBE_BACK)
};
// --------------------------------------------------------------------------


// ************* Font Related Global Variables *****************
GLuint texture_smoke;
GLfloat xTranslationAeroplane = -3.5f;
GLboolean flagAeroplane = 0;
// Font Array
GLint nFontList[4];
// ------------------------------------------------------------------


// ************************* FadeIn-FadeOut *************************
GLfloat alphaValue = 0.0f;
GLboolean startFadeIn = FALSE;
// -------------------------------------------------------------------

// ************************** Cliff *********************************
GLuint texture_cliff_ground;
GLuint texture_cliff_stone;
GLuint texture_cliff_stone2;
GLuint texture_tree_bark;
// -----------------------------------------------------------------


// ********************** Bamboo Tree ********************************
GLuint textureBamboo;
GLuint textureBambooLeaf;
GLuint texture_ground;

GLfloat LightAmbientOne[] = { 0.450f, 0.682f, 0.219f, 1.0f };
GLfloat LightDiffuseOne[] = { 0.450f, 0.682f, 0.219f, 1.0f };
GLfloat LightSpecularOne[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPositionOne[] = { 150.0f, 0.0f, 150.0f, 0.0f };

GLfloat MaterialAmbientOne[] = { 0.450f, 0.682f, 0.219f, 1.0f };
GLfloat MaterialDiffuseOne[] = { 0.450f, 0.682f, 0.219f, 1.0f };
GLfloat MaterialSpecularOne[] = { 0.450f, 0.682f, 0.219f, 1.0f };
GLfloat MaterialShininessOne = 50.0f;

GLfloat xLeafTranslation = 0.0f;
GLfloat leafAngle = 0.0f;
int flagForwardTree = 1;
typedef struct treePosition
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
}Position;
//Position treePposition[30];
GLfloat treePosition[10][10];
// ---------------------------------------------------------------------


// ************************* Humanoid **********************************
GLfloat angleShoulder = 0.0f;
GLfloat angleElbow = 0.0f;
GLfloat angleLeg = 0.0f;
GLfloat angleLegLower = 0.0f;
GLboolean flagForwardShoulder = 1;
GLboolean flagForwardElbow = 1;
GLboolean flagForwardLeg = 1;
GLboolean flagForwardLegLower = 1;

GLfloat xHumanoid = -300.0f;
GLfloat yHumanoid = 18.0f;
GLfloat zHumanoid = 250.0f;

GLfloat lightAmbientTwo[] = { 1.7f, 0.572f, 0.396f, 1.0f };
//GLfloat lightDiffuseTwo[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat lightSpecularTwo[] = { 0.788f, 0.572f, 0.396f, 1.0f };
//GLfloat lightPositionTwo[] = { 30.0f, 5.0f, -10.0f, 0.0f };

GLfloat materialAmbientTwo[] = { 0.5f, 0.5f, 0.5f, 0.0f };
GLfloat materialDiffuseTwo[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat materialSpecularTwo[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialShininessTwo = 100.0f;

GLuint texture_shirt;
GLuint texture_shirt2;
GLuint texture_pant;
GLuint texture_face;
// ---------------------------------------------------------------------


// ************************* Mountains *********************************
GLfloat controlPoints[I][J][K];
GLUnurbsObj* nurbs = NULL;
GLuint texture_mountain;
GLuint texture_mountain1;
// ---------------------------------------------------------------------


// ************************** Eagle ************************************
GLfloat angleWings = 0.0f;
GLboolean flagForward = 1;
GLboolean flagReverse = 0;
GLboolean flagEagleRotation = 0;


GLuint featherTexture_Head;
GLuint featherTexture_Eye;
GLuint featherTexture_Neck;
GLuint featherTexture_Body;
GLuint featherTexture_Tail;
GLuint featherTexture_Leg;
GLuint featherTexture_Wing;

GLfloat xEagle = 175.0f;
GLfloat yEagle = 50.0f;
GLfloat zEagle = 250.0f;
GLfloat angleRotateEagle = 90.0f;
GLfloat angleRotateEagle2 = 90.0f;
GLfloat angleRotateEagle3 = 0.0f;
GLfloat angleRotateEagle4 = 181.0f;
GLfloat angleRotateEagle5 = 270.0f;
GLfloat angleRotateEagle6 = 270.0f;
/*
GLfloat lightAmbientOne[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat lightDiffuseOne[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat lightSpecularOne[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat lightPositionOne[] = { 30.0f, 5.0f, -10.0f, 0.0f };

GLfloat lightAmbientTwo[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat lightDiffuseTwo[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat lightSpecularTwo[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat lightPositionTwo[] = { -30.0f, 5.0f, -10.0f, 0.0f };

GLfloat lightAmbientThree[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat lightDiffuseThree[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat lightSpecularThree[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat lightPositionThree[] = { 0.0f, 0.0f, 10.0f, 0.0f };

GLfloat materialAmbient[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat materialDiffuse[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat materialSpecular[] = { 0.788f, 0.572f, 0.396f, 1.0f };
GLfloat materialShininess = 100.0f;
*/
// ---------------------------------------------------------------------


// ************************** Village **********************************
GLuint texture_hut_wall1;
GLuint texture_hut_wall2;
GLuint texture_hut_wall3;
GLuint texture_hut_roof1;
GLuint texture_hut_roof2;
GLuint texture_hut_roof3;
GLuint texture_leaves_one;
GLuint texture_leaves_two;
GLuint texture_tree_trunk1;
// ---------------------------------------------------------------------

// ***************************** Water *********************************
GLuint texture_water;
GLfloat water_x[X][Z];
GLfloat water_y[X][Z];
GLfloat water_z[X][Z];
int freq = 12;
float time = 0.0f;
// ---------------------------------------------------------------------

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
		fprintf(gpFile, "LoadGLTexture() FOR 'smoke' FAILED.\n");
		uninitialize();
	}
	else if (iRetVal == -6)
	{
		fprintf(gpFile, "LoadGLTexture() For textureBamboo Failed.\n");
		uninitialize();
	}
	else if (iRetVal == -7)
	{
		fprintf(gpFile, "LoadGLTexture() For textureBambooLeaf Failed.\n");
		uninitialize();
	}
	else if (iRetVal == -13)
	{
		fprintf(gpFile, "LoadGLTexture() For Mountain Failed.\n");
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
			xCamera_LookAt -= 1.0f;
			break;

		case 'A':
			xCamera_LookAt += 1.0f;
			break;

		case 'd':
			yCamera_LookAt -= 1.0f;
			break;

		case 'D':
			yCamera_LookAt += 1.0f;
			break;

		case 'w':
			zCamera_LookAt -= 1.0f;
			break;

		case 'W':
			zCamera_LookAt += 1.0f;
			break;

		case 'r':
			if (flagRotation == 0)
				flagRotation = 1;
			else
				flagRotation = 0;

			break;

		case 'p':
			fprintf(gpFile, "Position : \nxLookAt = %f \n yLookAt = %f, zLookAt = %f\n angleOfSceneRotation = %f\n", xCamera_LookAt, yCamera_LookAt, zCamera_LookAt, angleOfSceneRotation);
			fprintf(gpFile, "xEagle = %f\n", xEagle);
			fprintf(gpFile, "yEagle = %f\n", yEagle);
			fprintf(gpFile, "zEagle = %f\n", zEagle);
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
	BOOL LoadGLTexture(GLuint*, TCHAR[], GLboolean);
	void setupFont(TCHAR[]);
	GLboolean LoadGLCubemap(void);
	void initSurface(void);

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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	
	
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			treePosition[i][j] = rand() % 30;

	for (int i = 0; i < Z; i++)
	{
		for (int j = 0; j < X; j++)
		{
			water_x[i][j] = -QUADSIZE + (j * XSHIFT);
			water_y[i][j] = 0.0f;//sin(temp_x * freq) * cos(temp_z * freq) * 0.1;
			water_z[i][j] = -QUADSIZE + (i * ZSHIFT);
		}
	}
	// ******************* Data Structure *****************************************************
	// Linked List

	Scene* head = NULL;


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	initSurface();

	// Create NURBS Object
	nurbs = gluNewNurbsRenderer();
	gluNurbsProperty(nurbs, GLU_SAMPLING_TOLERANCE, 25.0f);
	gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, GLU_FILL);
	gluNurbsProperty(nurbs, GLU_CULLING, GLU_TRUE);

	// Create Quadric
	quadric = gluNewQuadric();			// It Internally Calls Malloc, So Dont Use Inside Display()


	// *********************** LIGHT **********************************************************************
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbientOne);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuseOne);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecularOne);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPositionOne);

	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbientOne);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuseOne);
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecularOne);
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininessOne);


	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbientTwo);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuseTwo);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightSpecularTwo);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, lightPositionTwo);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbientTwo);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuseTwo);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecularTwo);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininessTwo);

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	/*glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbientOne);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuseOne);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightSpecularOne);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightPositionOne);
	//glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbientTwo);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuseTwo);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightSpecularTwo);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightPositionTwo);
	//glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbientThree);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuseThree);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightSpecularThree);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightPositionThree);
	//glEnable(GL_LIGHT2);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);

	//glEnable(GL_LIGHTING);
	*/

	// *********************** TEXTURES ********************************************************************
	if (LoadGLTexture(&texture_smoke, MAKEINTRESOURCE(IDBITMAP_SMOKE), 0) == FALSE)
		return -5;
	if (LoadGLTexture(&textureBamboo, MAKEINTRESOURCE(TREE_BAMBOO), 0) == FALSE)
		return -6;
	if (LoadGLTexture(&textureBambooLeaf, MAKEINTRESOURCE(TREE_BAMBOOLEAF), 0) == FALSE)
		return -7;
	if (LoadGLTexture(&texture_ground, MAKEINTRESOURCE(GREEN_GROUND), 1) == FALSE)
		return -8;
	if (LoadGLTexture(&texture_shirt, MAKEINTRESOURCE(HUMAN_SHIRT), 0) == FALSE)
		return -9;
	if (LoadGLTexture(&texture_shirt2, MAKEINTRESOURCE(HUMAN_SHIRT2), 0) == FALSE)
		return -10;
	if (LoadGLTexture(&texture_pant, MAKEINTRESOURCE(HUMAN_PANT), 0) == FALSE)
		return -11;
	if (LoadGLTexture(&texture_face, MAKEINTRESOURCE(HUMAN_FACE), 0) == FALSE)
		return -12;
	if (LoadGLTexture(&texture_mountain, MAKEINTRESOURCE(MOUNTAIN), 0) == FALSE)
		return -13;
	if (LoadGLTexture(&texture_mountain1, MAKEINTRESOURCE(MOUNTAIN_ONE), 0) == FALSE)
		return -14;
	if (LoadGLTexture(&texture_hut_wall1, MAKEINTRESOURCE(HUT_WALL_ONE), 0) == FALSE)
		return -15;
	//if (LoadGLTexture(&texture_hut_wall2, MAKEINTRESOURCE(HUT_WALL_TWO), 0) == FALSE)
		//return -16;
	if (LoadGLTexture(&texture_hut_wall3, MAKEINTRESOURCE(HUT_WALL_THREE), 0) == FALSE)
		return -17;
	if (LoadGLTexture(&texture_hut_roof1, MAKEINTRESOURCE(HUT_ROOF_ONE), 0) == FALSE)
		return -18;
	if (LoadGLTexture(&texture_hut_roof2, MAKEINTRESOURCE(HUT_ROOF_TWO), 0) == FALSE)
		return -19;
	if (LoadGLTexture(&texture_hut_roof3, MAKEINTRESOURCE(HUT_ROOF_THREE), 0) == FALSE)
		return -20;
	if (LoadGLTexture(&texture_leaves_one, MAKEINTRESOURCE(TREE_LEAVES_ONE), 0) == FALSE)
		return -21;
	if (LoadGLTexture(&texture_leaves_two, MAKEINTRESOURCE(TREE_LEAVES_TWO), 0) == FALSE)
		return -22;
	if (LoadGLTexture(&texture_tree_trunk1, MAKEINTRESOURCE(TREE_TRUNK_ONE), 0) == FALSE)
		return -23;
	if (LoadGLTexture(&texture_water, MAKEINTRESOURCE(WATER), 1) == FALSE)
		return -24;
	if (LoadGLTexture(&texture_cliff_ground, MAKEINTRESOURCE(CLIFF_GROUND), 0) == FALSE)
		return -25;
	if (LoadGLTexture(&texture_cliff_stone, MAKEINTRESOURCE(CLIFF_STONE), 0) == FALSE)
		return -26;
	if (LoadGLTexture(&texture_cliff_stone2, MAKEINTRESOURCE(CLIFF_STONE2), 0) == FALSE)
		return -27;
	if (LoadGLTexture(&texture_tree_bark, MAKEINTRESOURCE(TREE_BARK), 0) == FALSE)
		return -28;
	if (LoadGLTexture(&featherTexture_Head, MAKEINTRESOURCE(EAGLEFEATHER_HEAD), 0) == FALSE)
		return -29;
	if (LoadGLTexture(&featherTexture_Neck, MAKEINTRESOURCE(EAGLEFEATHER_NECK), 0) == FALSE)
		return -30;
	if (LoadGLTexture(&featherTexture_Body, MAKEINTRESOURCE(EAGLEFEATHER_BODY), 0) == FALSE)
		return -31;
	if (LoadGLTexture(&featherTexture_Tail, MAKEINTRESOURCE(EAGLEFEATHER_TAIL), 0) == FALSE)
		return -32;
	if (LoadGLTexture(&featherTexture_Eye, MAKEINTRESOURCE(EAGLEFEATHER_EYE), 0) == FALSE)
		return -33;
	if (LoadGLTexture(&featherTexture_Leg, MAKEINTRESOURCE(EAGLEFEATHER_LEG), 0) == FALSE)
		return -34;
	if (LoadGLTexture(&featherTexture_Wing, MAKEINTRESOURCE(EAGLEFEATHER_WING), 0) == FALSE)
		return -35;

	// Font
	setupFont("BALLADE");
	setupFont("The Artisan Marker");
	setupFont("CROCHET PATTERN");
	setupFont("Magnific Chaos Personal Use Regular");
	
	// Cubemap
	GLboolean result = LoadGLCubemap();
	if (result == FALSE)
	{
		fprintf(gpFile, "Failed In LoadGLCubemap() Function\n");
		DestroyWindow(ghwnd);
	}

	// temp
	/*xCamera_LookAt = 125.0f;
	yCamera_LookAt = 50.0f;
	zCamera_LookAt = 500.0f;

	xPosObj_LookAt = 150.0f;
	yPosObj_LookAt = 20.0f;
	zPosObj_LookAt = 250.0f;
	*/
	/*xEagle = 175.0f;
	yEagle = 50.0f;
	zEagle = 250.0f;

	xCamera_LookAt = 200.0f;
	yCamera_LookAt = 65.0f;
	zCamera_LookAt = 250.0f;

	xPosObj_LookAt = 160.0f;
	yPosObj_LookAt = 50.0f;
	zPosObj_LookAt = 250.0f;
	*/

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

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 2000.0f);
}

void display(void)
{
	// Function Declarations
	void renderingAstromedicompFont(void);
	void renderingHullGroupFont(void);
	void renderingPostCreditSceneMe(void);
	void renderingPostCreditSceneGL(void);
	void renderingPostCreditSceneSir(void);
	void fadeInOut(void);
	void draw(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	if (startSceneTransition[0] == TRUE)
	{
		flagAeroplane = 1;
		renderingAstromedicompFont();
	}
	
	if (startSceneTransition[1] == TRUE)
		renderingHullGroupFont();
	
	if (startSceneTransition[2] == TRUE)
	{
		//fprintf(gpFile, "In scene 3\n");
		flagCounterVariable = 0;
		draw();
		if(flagFirstTimeToStart == TRUE )
			flagSceneRendering[0] = TRUE;
	}
	
	if (startSceneTransition[3] == TRUE)
	{
		fprintf(gpFile, "In scene 4\n");
		flagCounterVariable = 0;
		draw();
		//if (flagFirstTimeToStart == TRUE)
			//flagSceneRendering[0] = TRUE;
	}

	if (startSceneTransition[4] == TRUE)
		renderingPostCreditSceneMe();

	if (startSceneTransition[5] == TRUE)
		renderingPostCreditSceneGL();

	if (startSceneTransition[6] == TRUE)
		renderingPostCreditSceneSir();
	
	
	{
		fadeInOut();
	}
	glColor3f(1.0f, 1.0f, 1.0f);


	SwapBuffers(ghdc);
}

void update(void)
{
	// Local Variable Declarations
	static int flagFadeIn = 0;
	static GLfloat count = 0.0f;
	static GLfloat radius = 300.0f;

	// Code

	if (flagRotation == 1)
	{
		if (flagSceneRendering[0] == TRUE)
		{
			if (flagFirstTimeToStart == TRUE)
				flagFirstTimeToStart = FALSE;
			//fprintf(gpFile, "flagFirstTimeToStart = %d\n", flagFirstTimeToStart);
			//xCamera_LookAt -= SPEED_OF_SYSTEM * 0.001f;
			angleOfSceneRotation += SPEED_OF_SYSTEM;

			// Look At Updation
			xCamera_LookAt = radius * cos(degreeToRadian(angleOfSceneRotation));
			zCamera_LookAt = radius * sin(degreeToRadian(angleOfSceneRotation));
			yCamera_LookAt += SPEED_OF_SYSTEM * 0.1f;
			//yPosObj_LookAt += SPEED_OF_SYSTEM * 0.1f;


			fprintf(gpFile, "in Scene 1\n");
			fprintf(gpFile, "%d\n\n", flagSceneRendering[0]);
			if (zCamera_LookAt < 200.0f)
			{
				flagSceneRendering[0] = FALSE;
				flagSceneRendering[1] = TRUE;
				angleOfSceneRotation = 290.0f;
			}
		}

		if (flagSceneRendering[1] == TRUE)
		{
			//xCamera_LookAt -= SPEED_OF_SYSTEM * 0.001f;
			angleOfSceneRotation -= SPEED_OF_SYSTEM * 2.0f;

			// Look At Updation
			xCamera_LookAt = -250 + 50 * cos(degreeToRadian(angleOfSceneRotation));
			zCamera_LookAt = 250 + 50 * sin(degreeToRadian(angleOfSceneRotation));
			yCamera_LookAt += SPEED_OF_SYSTEM * 0.4f;
			yPosObj_LookAt += SPEED_OF_SYSTEM * 0.2f;

			fprintf(gpFile, "in Scene 2\n");
			fprintf(gpFile, "angleOfSceneRotation = %f\n", angleOfSceneRotation);
			if (angleOfSceneRotation < 175.0f)
			{
				flagSceneRendering[1] = FALSE;
				flagSceneRendering[2] = TRUE;
				angleOfSceneRotation = 290.0f;
				fprintf(gpFile, "%d\n\n", flagSceneRendering[1]);
				fprintf(gpFile, "%d\n\n", flagSceneRendering[2]);
			}
		}

		if (flagSceneRendering[2] == TRUE)
		{
			fprintf(gpFile, "in Scene 3\n");
			fprintf(gpFile, "angleOfSceneRotation = %f\n", angleOfSceneRotation);

			xCamera_LookAt += SPEED_OF_SYSTEM * 0.2f;
			xPosObj_LookAt += SPEED_OF_SYSTEM;
			fprintf(gpFile, "xCamera = %f\n", xCamera_LookAt);

			//if (xCamera_LookAt > -250.0f)
			if (xHumanoid > -200.0f)
			{
				startFadeIn = TRUE;
				flagFadeIn = 1;
				//flagSceneRendering[2] = FALSE;
				//flagSceneRendering[3] = TRUE;

			}
		}

		if (flagSceneRendering[3] == TRUE)
		{
			if (zCamera_LookAt > 330.0f)
				zCamera_LookAt -= 0.5f;
			else
			{
				angleRotateEagle -= SPEED_OF_SYSTEM * 0.1f;
				if (angleRotateEagle >= 360.0f)
					angleRotateEagle -= 360.0f;

				xCamera_LookAt = 125.0f + 80 * cos(angleRotateEagle * M_PI / 180.0f);
				zCamera_LookAt = 250.0f + 80 * sin(angleRotateEagle * M_PI / 180.0f);

				if (angleRotateEagle <= 90.0f && angleRotateEagle >= 0)
					yCamera_LookAt += SPEED_OF_SYSTEM * 0.05f;
				else if (angleRotateEagle < 0.0f && angleRotateEagle >= -180.0f)
					yCamera_LookAt -= SPEED_OF_SYSTEM * 0.05f;
				else
				{
					//yCamera_LookAt += SPEED_OF_SYSTEM * 0.05f;
					flagSceneRendering[3] = FALSE;
					flagSceneRendering[4] = TRUE;

					xEagle = 175.0f;
					yEagle = 50.0f;
					zEagle = 250.0f;

					xCamera_LookAt = 200.0f;
					yCamera_LookAt = 65.0f;
					zCamera_LookAt = 250.0f;

					xPosObj_LookAt = 160.0f;
					yPosObj_LookAt = 50.0f;
					zPosObj_LookAt = 250.0f;

				}
			}



		}

		if (flagSceneRendering[4] == TRUE)
		{
			if (angleRotateEagle2 <= 270.0f)
			{
				angleRotateEagle3 += SPEED_OF_SYSTEM * 0.5f;


				angleRotateEagle2 += SPEED_OF_SYSTEM * 1.0f;
				xEagle = 175 + 50 * cos(angleRotateEagle2 * M_PI / 180.0f);
				zEagle = 200 + 50 * sin(angleRotateEagle2 * M_PI / 180.0f);
				yEagle -= 0.5f;
				if (zCamera_LookAt <= 175.0f)
				{
					xCamera_LookAt = 190 + 65 * cos(angleRotateEagle2 * M_PI / 180.0f);
					zCamera_LookAt = 225 + 50 * sin(angleRotateEagle2 * M_PI / 180.0f);
					yCamera_LookAt -= 0.5f;
				}
				xPosObj_LookAt = xEagle;// +cos(-angleRotateEagle3 * M_PI / 180.0f);
				zPosObj_LookAt = zEagle;// +sin(-angleRotateEagle3 * M_PI / 180.0f);
				yPosObj_LookAt = yEagle;

			}
			else
			{
				flagSceneRendering[4] = FALSE;
				flagSceneRendering[5] = TRUE;

				xEagle = 175.0f;
				yEagle = -40.0f;
				zEagle = 150.0f;

				xCamera_LookAt = 175.0f;
				yCamera_LookAt = -25;
				zCamera_LookAt = 175.0f;
			}
		}
		if (flagSceneRendering[5] == TRUE)
		{
			if (yEagle >= -450.0f)
			{
				fprintf(gpFile, "Inside if\n\n");
				yEagle -= 1.0f;
				zEagle -= 0.5f;
				yCamera_LookAt -= 1.0f;
				zCamera_LookAt -= 0.5f;
				yPosObj_LookAt = yEagle;
				zPosObj_LookAt = zEagle;
			}
			else if (zEagle > -250.0f)
			{
				fprintf(gpFile, "Inside else if\n\n");
				zEagle -= 0.5f;
				zCamera_LookAt -= 0.5f;
				zPosObj_LookAt = zEagle;
			}
			else
			{
				flagSceneRendering[5] = FALSE;
				flagSceneRendering[6] = TRUE;

				fprintf(gpFile, "Inside else\n\n");
				fprintf(gpFile, "%f\n\n", angleRotateEagle4);
				fprintf(gpFile, "xLookAt = %f\n", xCamera_LookAt);
				fprintf(gpFile, "yLookAt = %f\n", yCamera_LookAt);
				fprintf(gpFile, "zLookAt = %f\n", zCamera_LookAt);

				fprintf(gpFile, "xEagle = %f\n", xEagle);
				fprintf(gpFile, "yEagle = %f\n", yEagle);
				fprintf(gpFile, "zEagle = %f\n", zEagle);
			}
		}
		if (flagSceneRendering[6] == TRUE)
		{
			if (angleRotateEagle3 <= 270.0f)
				angleRotateEagle3 += SPEED_OF_SYSTEM * 0.5f;

			if (zEagle <= -250.0f)
			{

				fprintf(gpFile, "Scene 6\n\n");
				angleRotateEagle4 += 0.5f;
				fprintf(gpFile, "%f\n\n", angleRotateEagle4);
				fprintf(gpFile, "xLookAt = %f\n", xCamera_LookAt);
				fprintf(gpFile, "yLookAt = %f\n", yCamera_LookAt);
				fprintf(gpFile, "zLookAt = %f\n", zCamera_LookAt);

				fprintf(gpFile, "xEagle = %f\n", xEagle);
				fprintf(gpFile, "yEagle = %f\n", yEagle);
				fprintf(gpFile, "zEagle = %f\n", zEagle);

				xEagle = 300.0f + 125 * cos(angleRotateEagle4 * M_PI / 180.0f);
				zEagle = -250.0f + 125 * sin(angleRotateEagle4 * M_PI / 180.0f);
				xCamera_LookAt = 300.0f + 125 * cos((angleRotateEagle4 - 8) * M_PI / 180.0f);
				zCamera_LookAt = -250.0f + 125 * sin((angleRotateEagle4 - 8) * M_PI / 180.0f);
				xPosObj_LookAt = 300.0f + 125 * cos((angleRotateEagle4 + 5) * M_PI / 180.0f);
				zPosObj_LookAt = -250.0f + 125 * sin((angleRotateEagle4 + 5) * M_PI / 180.0f);

			}
			else
			{
				flagSceneRendering[6] = FALSE;
				flagSceneRendering[7] = TRUE;

			}
		}

		if (flagSceneRendering[7] == TRUE)
		{
			if (zEagle < 250.0f)
			{
				zEagle += 1.0f;
				zCamera_LookAt += 1.0f;
				zPosObj_LookAt += 1.0f;
			}
			else if (yEagle <= 400.0f && yEagle != 10.0f)
			{
				flagEagleRotation = 1;
				angleRotateEagle5 += 1.0f;
				angleRotateEagle6 += 1.0f;


				xEagle = 425.0f + 50.0f * cos(angleRotateEagle5 * M_PI / 180.0f);
				zEagle = 300.0f + 50.0f * sin(angleRotateEagle5 * M_PI / 180.0f);
				yEagle += 0.5f;
				xCamera_LookAt = 425.0f + 50.0f * cos((angleRotateEagle5 - 8) * M_PI / 180.0f);
				zCamera_LookAt = 300.0f + 50.0f * sin((angleRotateEagle5 - 8) * M_PI / 180.0f);
				yCamera_LookAt += 0.5f;
				xPosObj_LookAt = 425.0f + 50.0f * cos((angleRotateEagle5 + 5) * M_PI / 180.0f);
				zPosObj_LookAt = 300.0f + 50.0f * sin((angleRotateEagle5 + 5) * M_PI / 180.0f);
				yPosObj_LookAt += 0.5f;
			}
			else
			{
				flagSceneRendering[7] = FALSE;
				flagSceneRendering[8] = TRUE;
			}
		}

		if (flagSceneRendering[8] == TRUE)
		{
			xCamera_LookAt = 150.0f;
			yCamera_LookAt = 10.0f;
			zCamera_LookAt = 250.0f;
			xPosObj_LookAt = 125.0f;
			yPosObj_LookAt = 10.0f;
			zPosObj_LookAt = 250.0f;

		}


	}
	/*if (flagRotation == 1)
	{
		angleOfSceneRotation += SPEED_OF_SYSTEM;

		// Look At Updation
		xCamera_LookAt = radius * cos(degreeToRadian(angleOfSceneRotation));
		zCamera_LookAt = radius * sin(degreeToRadian(angleOfSceneRotation));
	}
	*/

	// Bamboo Tree
	// Leaf Position
	xLeafTranslation += SPEED_OF_SYSTEM * 0.1f;

	// Leaf Movement
	if (leafAngle <= 2 && flagForwardTree == 1)
		leafAngle += SPEED_OF_SYSTEM * 0.7f;
	else if (leafAngle >= -2)
	{
		leafAngle -= SPEED_OF_SYSTEM * 0.7f;
		flagForwardTree = 0;
	}
	else
		flagForwardTree = 1;



	// Humanoid
	if (startSceneTransition[2] == TRUE)
	{
		if (xHumanoid < -100.0f)
			xHumanoid += SPEED_OF_SYSTEM * 0.5f;
		else
			xHumanoid = 97.0f;
	}
	
	if (angleShoulder < 20.0f && flagForwardShoulder == 1)
	{
		angleShoulder += SPEED_OF_SYSTEM * 0.6f;
		flagForwardShoulder = 1;
	}
	else
		flagForwardShoulder = 0;

	if (angleShoulder > -20.0f && flagForwardShoulder == 0)
	{
		angleShoulder -= SPEED_OF_SYSTEM * 0.6f;
	}
	else
		flagForwardShoulder = 1;

	// Elbow
	if (angleElbow < 30.0f && flagForwardElbow == 1)
	{
		angleElbow += SPEED_OF_SYSTEM * 0.8f;
		flagForwardElbow = 1;
	}
	else
		flagForwardElbow = 0;

	if (angleElbow > -0.0f && flagForwardElbow == 0)
	{
		angleElbow -= SPEED_OF_SYSTEM * 0.8f;
	}
	else
		flagForwardElbow = 1;

	// Leg
	if (angleLeg < 20.0f && flagForwardLeg == 1)
	{
		angleLeg += SPEED_OF_SYSTEM * 0.6f;
		flagForwardLeg = 1;
	}
	else
		flagForwardLeg = 0;

	if (angleLeg > -20.0f && flagForwardLeg == 0)
	{
		angleLeg -= SPEED_OF_SYSTEM * 0.6f;
	}
	else
		flagForwardLeg = 1;

	if (angleLegLower < 40.0f && flagForwardLegLower == 1)
	{
		angleLegLower += SPEED_OF_SYSTEM * 0.6f;
	}
	else
		flagForwardLegLower = 0;

	if (angleLegLower > 0.0f && flagForwardLegLower == 0)
	{
		angleLegLower -= SPEED_OF_SYSTEM * 0.6f;
	}
	else
		flagForwardLegLower = 1;


	// Eagle
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


	// Font
	{
		// Aeroplane Translation Along x-axis
		if (xTranslationAeroplane <= 5.0f && flagAeroplane == 1)
			xTranslationAeroplane += SPEED_OF_SYSTEM * 0.01f;
	}

	// First Scene Starting
	if (xTranslationAeroplane > 5.0f && flagAeroplane != 0)
	{
		startFadeIn = TRUE;
		flagAeroplane = 0;
		flagFadeIn = 1;
	}
	
	// Water
	time = time + 0.05f;

	for (int i = 0; i < Z; i++)
	{
		for (int j = 0; j < X; j++)
		{
			float temp_x = (float)(j + 0) / (float)X;
			temp_x = (temp_x * 2.0f) - 1.0f;

			float temp_z = (float)(i + 0) / (float)Z;
			temp_z = (temp_z * 2.0f) - 1.0f;

			water_y[i][j] = sin(temp_x * freq + time) * cos(temp_z * freq + time) * 0.1f;
		}
	}

	// Count For Scenes
	if (flagCounterVariable == 1)
	{
		count += SPEED_OF_SYSTEM * 1.0f;
		fprintf(gpFile, "count = %lf\n", count);
		if (count == 1000)
		{
			startFadeIn = TRUE;
			flagFadeIn = 1;
		}
		if (count == 2000)
		{
			startFadeIn = TRUE;
			flagFadeIn = 1;
		}
		if (count == 3000)
		{
			startFadeIn = TRUE;
			flagFadeIn = 1;
		}
		if (count == 4000)
		{
			startFadeIn = TRUE;
			flagFadeIn = 1;
		}
	}

	// FadeIn-Out
	if (startFadeIn == TRUE)
	{
		if (alphaValue <= 1.1f && flagFadeIn == 1)
		{
			alphaValue += SPEED_OF_SYSTEM * 0.004f;
		}
		else if (alphaValue > 1.1f && flagFadeIn == 1)
		{
			flagFadeIn = 0;
			if (startSceneTransition[0] == TRUE)
			{
				startSceneTransition[0] = FALSE;
				startSceneTransition[1] = TRUE;
			}
			else if (startSceneTransition[1] == TRUE)
			{
				startSceneTransition[1] = FALSE;
				startSceneTransition[2] = TRUE;
			}
			else if (startSceneTransition[2] == TRUE)
			{
				startSceneTransition[2] = FALSE;
				startSceneTransition[3] = TRUE;

				xCamera_LookAt = 125.0f;
				yCamera_LookAt = 50.0f;
				zCamera_LookAt = 500.0f;

				xPosObj_LookAt = 150.0f;
				yPosObj_LookAt = 20.0f;
				zPosObj_LookAt = 250.0f;

				flagSceneRendering[2] = FALSE;
				flagSceneRendering[3] = TRUE;
			}
			else if (startSceneTransition[3] == TRUE)
			{
				startSceneTransition[3] = FALSE;
				startSceneTransition[4] = TRUE;
			}
			else if (startSceneTransition[4] == TRUE)
			{
				startSceneTransition[4] = FALSE;
				startSceneTransition[5] = TRUE;
			}
			else if (startSceneTransition[5] == TRUE)
			{
				startSceneTransition[5] = FALSE;
				startSceneTransition[6] = TRUE;
			}
		}
		else if (alphaValue >= 0.0f && flagFadeIn == 0)
		{
			alphaValue -= SPEED_OF_SYSTEM * 0.02f;
		}
		else
			startFadeIn = FALSE;
	}
}


void draw()
{
	// Function Declarations
	void cliff(void);
	void village(void);
	void ocean(void);
	void cubemap(void);
	

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(xCamera_LookAt, yCamera_LookAt, zCamera_LookAt, xPosObj_LookAt, yPosObj_LookAt, zPosObj_LookAt, xNormal_LookAt, yNormal_LookAt, zNormal_LookAt);

	glPushMatrix();

		// Third Quadrant Scene
		cliff();

		village();
	
		glPushMatrix();
			glTranslatef(300.0f, -495.0f, -200.0f);
			glScalef(40.0f, 40.0f, 50.0f);
			ocean();
		glPopMatrix();

		// cubemap
		glEnable(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		cubemap();
		glDisable(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glPopMatrix();
}

void cliff()
{
	// Function Declartions
	void bambooTree(void);
	void Humanoid(int);
	void mountain1(void);
	void mountain2(void);
	void hutThree(void);
	void valley(void);
	void Eagle(void);
	void nature(void);

	// Local Variable Declarations
	GLfloat xCoordinate = 500.0f;
	GLfloat yCoordinate = 500.0f;
	GLfloat zCoordinate = 500.0f;

	// Code

	// Cliff - Ground
	glPushMatrix();
		glTranslatef(-250.0f, -250.0f, 250.0f);
		glScalef(0.5f, 0.5f, 0.5f);
		
		// Top
		glBindTexture(GL_TEXTURE_2D, texture_cliff_ground);
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(xCoordinate, yCoordinate, -zCoordinate);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-xCoordinate, yCoordinate, -zCoordinate);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-xCoordinate, yCoordinate, zCoordinate);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(xCoordinate, yCoordinate, zCoordinate);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);

		// Right
		glBindTexture(GL_TEXTURE_2D, texture_cliff_stone);
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(xCoordinate, yCoordinate, -zCoordinate);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(xCoordinate, yCoordinate, zCoordinate);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(xCoordinate, -yCoordinate, zCoordinate);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(xCoordinate, -yCoordinate, -zCoordinate);
		glEnd();

		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-xCoordinate, yCoordinate, -zCoordinate);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(xCoordinate, yCoordinate, -zCoordinate);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(xCoordinate, -yCoordinate, -zCoordinate);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-xCoordinate, -yCoordinate, -zCoordinate);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
	
	
	
	
	// Forest
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	
	for (int xPos = 150, j = 0; xPos < 200; xPos+= 20, j++)
	{
		for (int zPos = 30, i = 0; zPos < 470; zPos+=30, i++)
		{
			if (zPos > 200 && zPos < 300)
			{

			}
			else
			{
				glPushMatrix();
					glTranslatef(-xPos + treePosition[j][i], 40.0f, zPos + treePosition[j][i]);
					//glTranslatef(-250.0f, 20.0f, 250.0f);
					glScalef(20.0f, 20.0f, 20.0f);
					glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
						bambooTree();
				glPopMatrix();
			}
		}
	}

	if (startSceneTransition[2] == TRUE)
	{
		for (int xPos = 200, j = 0; xPos < 350; xPos += 20, j++)
		{
			for (int zPos = 150, i = 0; zPos < 350; zPos += 20, i++)
			{
				if (zPos > 200 && zPos < 300)
				{

				}
				else
				{
					glPushMatrix();
					//glTranslatef(-xPos - 10.0f, 40.0f, zPos );
					glTranslatef(-xPos + treePosition[j][i], 40.0f, zPos + treePosition[j][i]);
					//glTranslatef(-250.0f, 20.0f, 250.0f);
					glScalef(20.0f, 20.0f, 20.0f);
					glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
					bambooTree();
					glPopMatrix();
				}
			}
		}
	}

	// Mountain();
	glPushMatrix();

		glPushMatrix();
			glTranslatef(-400.0f, 0.0f, 460.0f);
			glScalef(0.5f, 1.0f, 0.5f);
			mountain1();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-450.0f, 0.0f, 250.0f);
			glScalef(0.5f, 0.5f, 1.0f);
			mountain1();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-350.0f, 70.0f, 80.0f);
			glScalef(50.5f, 70.5f, 100.0f);
			mountain2();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-470.0f, 80.0f, 150.0f);
			glScalef(50.0f, 50.5f, 50.0f);
			mountain2();
		glPopMatrix();
	glPopMatrix();

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	

	// Humanoid
	
	if (startSceneTransition[2] == TRUE)//if (flagSceneRendering[0] == TRUE)
	{
		glPushMatrix();

			glTranslatef(xHumanoid, yHumanoid, zHumanoid);
			glScalef(2.0f, 2.0f, 2.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			Humanoid(1);

		glPopMatrix();
	}
	
	//if (flagSceneRendering[3] == TRUE) 
	if (startSceneTransition[3] == TRUE)
	{
		// Humanoid
		glPushMatrix();

			glTranslatef(xHumanoid - 1.0f, yHumanoid - 9.0f, zHumanoid);
			glScalef(2.0f, 2.0f, 2.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			Humanoid(2);

		glPopMatrix();

		// Eagle
		glPushMatrix();
			glTranslatef(xEagle, yEagle, zEagle);
			glScalef(2.0f, 2.0f, 2.0f);
			if (flagSceneRendering[4] == TRUE || flagSceneRendering[5] == TRUE || flagSceneRendering[6] == TRUE || flagSceneRendering[7] == TRUE)
				glRotatef(-angleRotateEagle3, 0.0f, 1.0f, 0.0f);
			if (flagEagleRotation == TRUE)
				glRotatef(-angleRotateEagle6, 0.0f, 1.0f, 0.0f);
			Eagle();
		glPopMatrix();
	}
	
	
	// Hut
	glPushMatrix();
		glTranslatef(-60.0f, 15.0f, 350.0f);
		glScalef(20.0f, 20.0f, 20.0f);
		glRotatef(150.0f, 0.0f, 1.0f, 0.0f);
		hutThree();
	glPopMatrix();
	

	// Valley
	glPushMatrix();
		//glEnable(GL_LIGHT1);
		//glEnable(GL_LIGHTING);
		valley();
		//glDisable(GL_LIGHT1);
		//glDisable(GL_LIGHTING);
	glPopMatrix();

	// nature
	nature();
}

void valley()
{
	// Function Declarations
	void mountain3(void);
	void mainTree(int);
	
	// Code
	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, texture_cliff_ground);
		glBegin(GL_QUADS);
			// Top
			
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(0.0f, 0.0f, 150.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(100.0f, 0.0f, 230.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(100.0f, 0.0f, 270.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 350.0f);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	
		glBindTexture(GL_TEXTURE_2D, texture_cliff_stone2);
		glBegin(GL_QUADS);
			// Front
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(100.0f, 0.0f, 230.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(100.0f, 0.0f, 270.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(50.0f, -250.0f, 270.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(50.0f, -250.0f, 230.0f);
			// Left
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(100.0f, 0.0f, 270.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(0.0f, 0.0f, 350.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.0f, -250.0f, 270.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(50.0f, -250.0f, 270.0f);
			// Right
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(0.0f, 0.0f, 150.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(100.0f, 0.0f, 230.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(50.0f, -250.0f, 230.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(0.0f, -250.0f, 230.0f);
			
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);

		glPushMatrix();
			glTranslatef(0.0f, 0.0f, 250.0f);
			glScalef(10.0f, 10.0f, 10.0f);
			mainTree(5);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0.0f, -50.0f, 400.0f);
			glScalef(25.0f, 50.0f, 50.0f);
			mountain3();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0.0f, -50.0f, 100.0f);
			glScalef(25.0f, 50.0f, 50.0f);
			mountain3();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0.0f, -200.0f, 250.0f);
			glScalef(50.0f, 100.0f, 100.0f);
			mountain3();
		glPopMatrix();

	glPopMatrix();
}

void nature()
{
	// Function Declarations
	void mountain1(void);
	void mountain2(void);

	// Code
	glPushMatrix();
	glTranslatef(-150.0f, -300.0f, -200.0f);
		glScalef(200.0f, 400.0f, 200.0f);
		mountain2();
	glPopMatrix();

	
	glPushMatrix();
		glTranslatef(-250.0f, -500.0f, -100.0f);
		glScalef(0.5f, 5.0f, 1.0f);
		mountain1();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-250.0f, -500.0f, -250.0f);
		glScalef(2.0f, 1.0f, 2.5f);
		mountain1();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-0.0f, -450.0f, -450.0f);
		glScalef(2.5f, 0.5f, 1.5f);
		//mountain1();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-150.0f, -300.0f, -400.0f);
		glScalef(200.0f, 400.0f, 200.0f);
		mountain2();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-50.0f, -450.0f, -450.0f);
		glScalef(0.5f, 4.0f, 0.5f);
		mountain1();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(100.0f, -490.0f, -495.0f);
		glScalef(0.8f, 1.0f, 0.5f);
		mountain1();
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, texture_mountain);
		glTranslatef(180.0f, -500.0f, -230.0f);
		glScalef(0.5f, 0.5f, 2.5f);
		gluQuadricTexture(quadric, GLU_TRUE);
		gluSphere(quadric, 50.0f, 10, 10);
		glTranslatef(0.0f, 0.0f, 100.0f);
		gluSphere(quadric, 50.0f, 10, 10);
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

}

void village()
{
	// Function Declarations
	void hutOne(void);
	void hutTwo(void);
	void hutThree(void);
	void tree1(void);
	void tree2(void);
	void ground(void);
	void mainTree(int);
	void bambooTree(void);
	void mountain3(void);

	// Code
	glPushMatrix();

		// Starting Of Valley
		glPushMatrix();
			glTranslatef(10.0f, -450.0f, 495.0f);
			glScalef(20.0f, 20.0f, 20.0f);
			mountain3();
			glTranslatef(5.0f, 0.0f, 0.0f);
			bambooTree();
			glTranslatef(2.0f, 0.0f, 0.0f);
			bambooTree();
		glPopMatrix();

		glTranslatef(450.0f, -485.0f, 300.0f);
		glScalef(10.0f, 10.0f, 10.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		glPushMatrix();

			glTranslatef(-1.0f, 0.0f, 0.0f);
			tree2();

			glTranslatef(-3.0f, 0.0f, 2.0f);
			tree1();

			glTranslatef(-1.0f, 0.0f, 1.5f);
			hutThree();

			glTranslatef(1.0f, 0.0f, 3.0f);
			hutOne();

			glTranslatef(0.0f, 0.0f, -4.0f);
			hutTwo();

			glTranslatef(2.0f, 1.0f, 0.0f);
			glScalef(2.0f, 1.0f, 1.5f);
			tree1();

			

		glPopMatrix();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(425.0f, -450.0f, 300.0f);
		glScalef(5.0f, 5.0f, 5.0f);
		mainTree(5);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, texture_ground);
		glTranslatef(250.0f, -488.0f, 300.0f);
		glScalef(10.0f, 10.0f, 10.0f);
		//glTranslatef(450.0f, -485.0f, 300.0f);
		ground();
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
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
	gluCylinder(quadric, 0.08f, 0.03f, 4.5f, 3, 3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	glPushMatrix();
	for (float i = 0.4f, j = 1.0f; i <= 4.5f; i += 0.5f, j -= 0.05f)
	{
		glPushMatrix();
		glTranslatef(0.0f, i - 2.0f, 0.0f);
		glRotatef(80.0f, 0.0f, 0.0f, 1.0f);
		glScalef(1.0f, j, 1.0f);
		glRotatef(50.0f, 0.0f, 1.0f, 0.0f);
		glPushMatrix();
		Branch();
		glPopMatrix();
		glPopMatrix();
	}
	for (float i = 0.7f, j = 1.0f; i <= 4.5f; i += 0.7f, j -= 0.05f)
	{
		glPushMatrix();
		glTranslatef(0.0f, i - 2.0f, 0.0f);
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
	gluCylinder(quadric, 0.01f, 0.005f, 2.7f, 2, 2);
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
	gluCylinder(quadric, 0.01f, 0.005f, 1.8f, 1, 1);
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
	//glBindTexture(GL_TEXTURE_2D, textureBambooLeaf);
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
	//glBindTexture(0, textureBambooLeaf);
	glPopMatrix();
}

void Humanoid(int state)
{
	// Function Declarations
	void cube(void);

	// Code
	glPushMatrix();

	// Head
	glPushMatrix();
	//glDisable(GL_LIGHTING);
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
	gluCylinder(quadric, 0.5f, 0.35f, 0.6f, 5, 7);
	glPopMatrix();


	// Ears
	glBindTexture(GL_TEXTURE_2D, texture_face);
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(0.7f, -0.1f, 0.2f);
	glScalef(0.8f, 1.5f, 0.5f);
	//glRotatef(-40.0f, 1.0f, 0.0f, 0.0f);
	gluQuadricTexture(quadric, GLU_TRUE);
	gluSphere(quadric, 0.2f, 5, 5);
	glPopMatrix();
	glPushMatrix();
	//glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(-0.7f, -0.1f, 0.2f);
	glScalef(0.8f, 1.5f, 0.5f);
	//glRotatef(10.0f, 0.0f, 0.0f, 1.0f);
	gluQuadricTexture(quadric, GLU_TRUE);
	gluSphere(quadric, 0.2f, 5, 5);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);

	glPushMatrix();
	/// Eyes Right
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslatef(0.3f, -0.2f, 0.7f);
	glPushMatrix();
	glScalef(1.0f, 0.5f, 0.2f);
	gluSphere(quadric, 0.1f, 5, 5);
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
	glTranslatef(-0.3f, -0.2f, 0.7f);
	glPushMatrix();
	glScalef(1.0f, 0.5f, 0.3f);
	gluSphere(quadric, 0.1f, 5, 5);
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
	glScalef(0.8f, 1.0f, 0.8f);
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
	glScalef(0.75f, 0.6f, 0.33f);
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
	if (state == 1)
		glRotatef(-angleShoulder, 1.0f, 0.0f, 0.0f);
	else
		glRotatef(-10.0f, 1.0f, 0.0f, 0.0f);
	//glRotatef(-angleShoulder, 1.0f, 0.0f, 0.0f);
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
	if (state == 1)
		glRotatef(-angleElbow, 0.0f, 1.0f, 1.0f);
	else
		glRotatef(-10.0f, 0.0f, 1.0f, 1.0f);
	//glRotatef(-angleElbow, 0.0f, 1.0f, 1.0f);
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
	glRotatef(-40.0f, 0.0f, 0.0f, 1.0f);
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
	if (state == 1)
		glRotatef(angleShoulder, 1.0f, 0.0f, 0.0f);
	else
		glRotatef(-10.0f, 1.0f, 0.0f, 0.0f);
	//glRotatef(angleShoulder, 1.0f, 0.0f, 0.0f);
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
	if (state == 1)
		glRotatef(angleElbow, 0.0f, 1.0f, 1.0f);
	else
		glRotatef(10.0f, 0.0f, 1.0f, 1.0f);
	//glRotatef(angleElbow, 0.0f, 1.0f, 1.0f);
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


	//glEnable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, texture_pant);
	// Legs
	glPushMatrix();
	glTranslatef(-0.7f, -3.5f, 0.0f);
	if (state == 1)
		glRotatef(-angleLeg, 1.0f, 0.0f, 0.0f);
	else
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadric, 0.7f, 0.5f, 2.0f, 10, 10);
	glPopMatrix();
	glTranslatef(0.0f, -2.3f, 0.0f);
	gluSphere(quadric, 0.4f, 10, 10);
	glTranslatef(0.0f, -0.3f, 0.0f);
	if (state == 1)
		glRotatef(angleLegLower, 1.0f, 0.0f, 0.0f);
	else
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	//glRotatef(angleLegLower, 1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadric, 0.4f, 0.2f, 2.0f, 10, 10);
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
	if (state == 1)
		glRotatef(angleLeg, 1.0f, 0.0f, 0.0f);
	else
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadric, 0.7f, 0.5f, 2.0f, 10, 10);
	glPopMatrix();
	glTranslatef(0.0f, -2.3f, 0.0f);
	gluSphere(quadric, 0.4f, 10, 10);
	glTranslatef(0.0f, -0.3f, 0.0f);
	if (state == 1)
		glRotatef(angleLegLower, 1.0f, 0.0f, 0.0f);
	else
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	//glRotatef(angleLegLower, 1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadric, 0.4f, 0.2f, 2.0f, 10, 10);
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
	glColor3f(0.905f, 0.766f, 0.437f);
	glPushMatrix();
	glTranslatef(-1.9f, 1.0f, 0.0f);
	glPushMatrix();
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(quadric, 0.1f, 0.05f, 0.2f, 5, 5);
	glTranslatef(0.0f, -0.05f, 0.0f);
	glScalef(1.0f, 1.3f, 1.5f);
	glColor3f(0.905f, 0.662f, 0.376f);
	gluSphere(quadric, 0.1f, 5, 5);
	glPopMatrix();
	glTranslatef(-0.18f, 0.03f, 0.0f);
	glPushMatrix();
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);
	glColor3f(0.905f, 0.766f, 0.437f);
	gluCylinder(quadric, 0.05f, 0.01f, 0.2f, 4, 4);
	glPopMatrix();
	glPopMatrix();
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
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(quadric, 0.05f, 0.02f, 0.3f, 10, 10);
	glRotatef(160.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(quadric, 0.05f, 0.02f, 0.3f, 10, 10);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(quadric, 0.05f, 0.02f, 0.3f, 10, 10);
	glPopMatrix();
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
	glPushMatrix();
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(quadric, 0.05f, 0.02f, 0.3f, 10, 10);
	glRotatef(160.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(quadric, 0.05f, 0.02f, 0.3f, 10, 10);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(quadric, 0.05f, 0.02f, 0.3f, 10, 10);
	glPopMatrix();
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

void mountain1(void)
{
	// Local Variable Declarations
	GLfloat knots[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };

	// Code
	glBindTexture(GL_TEXTURE_2D, texture_mountain1);
	glPushMatrix();

		glScalef(50.0f, 50.0f, 50.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		gluBeginSurface(nurbs);
		gluNurbsSurface(nurbs,
			8, knots, 8, knots,
			4 * 3, 3, &controlPoints[0][0][0],
			4, 4, GL_MAP2_NORMAL);

		gluNurbsSurface(nurbs,
			8, knots, 8, knots,
			4 * 3, 3, &controlPoints[0][0][0],
			4, 4, GL_MAP2_TEXTURE_COORD_2);

		gluNurbsSurface(nurbs,
			8, knots, 8, knots,
			4 * 3, 3, &controlPoints[0][0][0],
			4, 4, GL_MAP2_VERTEX_3);
		gluEndSurface(nurbs);

	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void mountain2(void)
{
	// Code
	glBindTexture(GL_TEXTURE_2D, texture_mountain);
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
	glBindTexture(GL_TEXTURE_2D, 0);
}

void mountain3(void)
{
	// Code
	glPushMatrix();

		glBindTexture(GL_TEXTURE_2D, texture_cliff_ground);
		glBegin(GL_QUADS);
			// Top Face
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindTexture(GL_TEXTURE_2D, texture_mountain);
		glBegin(GL_TRIANGLES);

		// Front Face
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);		// Apex
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glTexCoord2f(0.5f, 0.0f);
		glVertex3f(0.0f, -1.0f, 0.0f);
		// Right Face
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);		// Apex
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.5f, 0.0f);
		glVertex3f(0.0f, -1.0f, 0.0f);
		// Back Face
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);		// Apex
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f(0.5f, 0.0f);
		glVertex3f(0.0f, -1.0f, 0.0f);
		// Left Face
		
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);		// Apex
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glTexCoord2f(0.5f, 0.0f);
		glVertex3f(0.0f, -1.0f, 0.0f);

		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
		/*glBindTexture(GL_TEXTURE_2D, texture_cliff_ground);
		glBegin(GL_QUADS);
			//glTexCoord3f(0.0f, 0.0f, 350.0f);
			glTexCoord2f(10.0f, 10.0f);
			glVertex3f(0.0f, 0.0f, 350.0f);
			//glTexCoord3f(50.0f, 0.0f, 350.0f);
			glTexCoord2f(0.0f, 10.0f);
			glVertex3f(50.0f, 0.0f, 350.0f);
			//glTexCoord3f(50.0f, 0.0f, 400.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(50.0f, 0.0f, 400.0f);
			//glTexCoord3f(0.0f, 0.0f, 400.0f);
			glTexCoord2f(10.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 400.0f);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindTexture(GL_TEXTURE_2D, texture_cliff_stone2);
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(0.0f, 0.0f, 350.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(50.0f, 0.0f, 350.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(10.0f, -450.0f, 350.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(0.0f, -450.0f, 350.0f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(50.0f, 0.0f, 400.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(0.0f, 0.0f, 400.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.0f, -450.0f, 400.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(10.0f, -450.0f, 400.0f);

			glTexCoord2f(1.0f, 1.0f);
			//glTexCoord3f(50.0f, 0.0f, 350.0f);
			glVertex3f(50.0f, 0.0f, 350.0f);
			glTexCoord2f(0.0f, 1.0f);
			//glTexCoord3f(50.0f, 0.0f, 400.0f);
			glVertex3f(50.0f, 0.0f, 400.0f);
			glTexCoord2f(0.0f, 0.0f);
			//glTexCoord3f(10.0f, -450.0f, 400.0f);
			glVertex3f(10.0f, -450.0f, 400.0f);
			glTexCoord2f(1.0f, 0.0f);
			//glTexCoord3f(10.0f, -450.0f, 350.0f);
			glVertex3f(10.0f, -450.0f, 350.0f);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
		*/
	
	
}

void tree1()
{
	// Code
	glPushMatrix();
	glScalef(1.0f, 1.0f, 1.0f);
	// Leaves
	glBindTexture(GL_TEXTURE_2D, texture_leaves_one);
	glPushMatrix();
	gluQuadricTexture(quadric, GLU_TRUE);
	glTranslatef(0.0f, 1.0f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluSphere(quadric, 1.5f, 30, 30);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);


	// Trunk
	glBindTexture(GL_TEXTURE_2D, texture_tree_trunk1);
	glPushMatrix();
	gluQuadricTexture(quadric, GLU_TRUE);
	glTranslatef(0.0f, -1.8f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadric, 0.5f, 0.3f, 2.2f, 30, 30);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

void tree2()
{
	// Code
	glPushMatrix();
	glScalef(0.5f, 0.5f, 0.5f);

	// Leaves
	glBindTexture(GL_TEXTURE_2D, texture_leaves_two);
	glPushMatrix();
	gluQuadricTexture(quadric, GLU_TRUE);
	glTranslatef(0.0f, 1.0f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluSphere(quadric, 1.5f, 30, 30);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);

	// Trunk
	glBindTexture(GL_TEXTURE_2D, texture_tree_trunk1);
	glPushMatrix();
	gluQuadricTexture(quadric, GLU_TRUE);
	glTranslatef(0.0f, -1.8f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadric, 0.5f, 0.3f, 2.2f, 30, 30);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glPopMatrix();
}

void hutOne()
{
	// Code
	
	glBindTexture(GL_TEXTURE_2D, texture_hut_roof1);
	glPushMatrix();
	gluQuadricTexture(quadric, GLU_TRUE);
	glTranslatef(0.0f, 0.3f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadric, 1.5f, 0.0f, 1.5f, 30, 30);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);


	// Walls
	glBindTexture(GL_TEXTURE_2D, texture_hut_wall1);
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
	
	glBindTexture(GL_TEXTURE_2D, texture_hut_roof2);
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


	// Walls
	glBindTexture(GL_TEXTURE_2D, texture_hut_wall2);
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
	
	glBindTexture(GL_TEXTURE_2D, texture_hut_roof3);
	glPushMatrix();
	gluQuadricTexture(quadric, GLU_TRUE);
	glTranslatef(0.0f, 0.3f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadric, 1.5f, 0.0f, 1.5f, 10, 10);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);


	// Walls
	glBindTexture(GL_TEXTURE_2D, texture_hut_wall3);
	glPushMatrix();
	gluQuadricTexture(quadric, GLU_TRUE);
	glTranslatef(0.0f, -0.8f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadric, 1.0f, 1.1f, 1.2f, 10, 10);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	

	// door

}

void ground(void)
{
	// Code
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	// Front Face
	
	glBindTexture(GL_TEXTURE_2D, texture_ground);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(30.0f, 30.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-30.0f, 30.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-30.0f, -30.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(30.0f, -30.0f, 1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	
}

void ocean()
{
	// Code
	glBindTexture(GL_TEXTURE_2D, texture_water);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < Z - 1; i++)
	{
		for (int j = 0; j < X - 1; j++)
		{
			
			glTexCoord2f(0.5f, 1.0f);
			glVertex3f(water_x[i + 0][j + 0], water_y[i + 0][j + 0], water_z[i + 0][j + 0]);
			
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(water_x[i + 0][j + 1], water_y[i + 0][j + 1], water_z[i + 0][j + 1]);
			
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(water_x[i + 1][j + 0], water_y[i + 1][j + 0], water_z[i + 1][j + 0]);


			//glColor3f(0.0f, 0.0f, 0.7f);
			
			glTexCoord2f(0.5f, 1.0f);
			glVertex3f(water_x[i + 0][j + 1], water_y[i + 0][j + 1], water_z[i + 0][j + 1]);
			
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(water_x[i + 1][j + 0], water_y[i + 1][j + 0], water_z[i + 1][j + 0]);
			
			//glColor3f(0.0f, 0.0f, 0.5f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(water_x[i + 1][j + 1], water_y[i + 1][j + 1], water_z[i + 1][j + 1]);

		}
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void mainTree(int levelOfRecursion)
{
	// Function Declarations
	void trunk(GLfloat, GLfloat, GLfloat);
	void branch1(void);

	// Local Variable Declarations

	// Code
	glPushMatrix();

	// Trunk
	if (levelOfRecursion <= 1)
	{

	}
	else if (levelOfRecursion == 5)
	{
		glPushMatrix();
		//glBindTexture(GL_TEXTURE_2D, textureBamboo);
		trunk(0.8f, 0.4f, 4.0f);
		//glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();
	}

	branch1();


	glPopMatrix();
}

void trunk(GLfloat base, GLfloat top, GLfloat height)
{
	// Code
	glBindTexture(GL_TEXTURE_2D, texture_tree_bark);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluQuadricTexture(quadric, GLU_TRUE);
	gluCylinder(quadric, base, top, height, 10, 10);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void branch1()
{
	// Function Declarations
	void branch2();

	// Code
	glBindTexture(GL_TEXTURE_2D, texture_tree_bark);
	glTranslatef(0.0f, 4.0f, 0.0f);
	glPushMatrix();
	glRotatef(35.0f, 1.0f, 0.0f, 0.0f);
	trunk(0.4f, 0.3f, 3.0f);
	branch2();
	glPopMatrix();

	glPushMatrix();
	glRotatef(-35.0f, 1.0f, 0.0f, 0.0f);
	trunk(0.4f, 0.3f, 3.0f);
	branch2();
	glPopMatrix();

	glPushMatrix();
	glRotatef(35.0f, 0.0f, 0.0f, 1.0f);
	trunk(0.4f, 0.3f, 3.0f);
	branch2();
	glPopMatrix();

	glPushMatrix();
	glRotatef(-35.0f, 0.0f, 0.0f, 1.0f);
	trunk(0.4f, 0.3f, 3.0f);
	branch2();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);

}

void branch2()
{
	// Function Declarations
	void branch3();

	// Code
	glTranslatef(0.0f, 0.0f, 3.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glRotatef(35.0f, 1.0f, 0.0f, 0.0f);
	trunk(0.3f, 0.2f, 2.5f);
	branch3();
	glPopMatrix();

	glPushMatrix();
	glRotatef(-35.0f, 1.0f, 0.0f, 0.0f);
	trunk(0.3f, 0.2f, 2.5f);
	branch3();
	glPopMatrix();

	glPushMatrix();
	glRotatef(35.0f, 0.0f, 0.0f, 1.0f);
	trunk(0.3f, 0.2f, 2.5f);
	branch3();
	glPopMatrix();

	glPushMatrix();
	glRotatef(-35.0f, 0.0f, 0.0f, 1.0f);
	trunk(0.3f, 0.2f, 2.5f);
	branch3();
	glPopMatrix();
}

void branch3()
{
	// Code
	glTranslatef(0.0f, 0.0f, 2.5f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glRotatef(35.0f, 1.0f, 0.0f, 0.0f);
	trunk(0.2f, 0.05f, 2.0f);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-35.0f, 1.0f, 0.0f, 0.0f);
	trunk(0.2f, 0.05f, 2.0f);
	glPopMatrix();

	glPushMatrix();
	glRotatef(35.0f, 0.0f, 0.0f, 1.0f);
	trunk(0.2f, 0.05f, 2.0f);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-35.0f, 0.0f, 0.0f, 1.0f);
	trunk(0.2f, 0.05f, 2.0f);
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

void cubemap()
{
	// Local Variable Declarations
	GLfloat xCoordinate = 500.0f;
	GLfloat yCoordinate = 500.0f;
	GLfloat zCoordinate = 500.0f;

	// Code
	// Front
	glBegin(GL_QUADS);
		glTexCoord3f(xCoordinate, yCoordinate, zCoordinate);
		glVertex3f(xCoordinate, yCoordinate, zCoordinate);
		glTexCoord3f(-xCoordinate, yCoordinate, zCoordinate);
		glVertex3f(-xCoordinate, yCoordinate, zCoordinate);
		glTexCoord3f(-xCoordinate, -yCoordinate, zCoordinate);
		glVertex3f(-xCoordinate, -yCoordinate, zCoordinate);
		glTexCoord3f(xCoordinate, -yCoordinate, zCoordinate);
		glVertex3f(xCoordinate, -yCoordinate, zCoordinate);
	glEnd();

	// Right
	glBegin(GL_QUADS);
		glTexCoord3f(xCoordinate, yCoordinate, -zCoordinate);
		glVertex3f(xCoordinate, yCoordinate, -zCoordinate);
		glTexCoord3f(xCoordinate, yCoordinate, zCoordinate);
		glVertex3f(xCoordinate, yCoordinate, zCoordinate);
		glTexCoord3f(xCoordinate, -yCoordinate, zCoordinate);
		glVertex3f(xCoordinate, -yCoordinate, zCoordinate);
		glTexCoord3f(xCoordinate, -yCoordinate, -zCoordinate);
		glVertex3f(xCoordinate, -yCoordinate, -zCoordinate);
	glEnd();

	// Back
	glBegin(GL_QUADS);
		glTexCoord3f(-xCoordinate, yCoordinate, -zCoordinate);
		glVertex3f(-xCoordinate, yCoordinate, -zCoordinate);
		glTexCoord3f(xCoordinate, yCoordinate, -zCoordinate);
		glVertex3f(xCoordinate, yCoordinate, -zCoordinate);
		glTexCoord3f(xCoordinate, -yCoordinate, -zCoordinate);
		glVertex3f(xCoordinate, -yCoordinate, -zCoordinate);
		glTexCoord3f(-xCoordinate, -yCoordinate, -zCoordinate);
		glVertex3f(-xCoordinate, -yCoordinate, -zCoordinate);
	glEnd();

	// Left
	glBegin(GL_QUADS);
		glTexCoord3f(-xCoordinate, yCoordinate, zCoordinate);
		glVertex3f(-xCoordinate, yCoordinate, zCoordinate);
		glTexCoord3f(-xCoordinate, yCoordinate, -zCoordinate);
		glVertex3f(-xCoordinate, yCoordinate, -zCoordinate);
		glTexCoord3f(-xCoordinate, -yCoordinate, -zCoordinate);
		glVertex3f(-xCoordinate, -yCoordinate, -zCoordinate);
		glTexCoord3f(-xCoordinate, -yCoordinate, zCoordinate);
		glVertex3f(-xCoordinate, -yCoordinate, zCoordinate);
	glEnd();

	// Top
	glBegin(GL_QUADS);
		glTexCoord3f(xCoordinate, yCoordinate, -zCoordinate);
		glVertex3f(xCoordinate, yCoordinate, -zCoordinate);
		glTexCoord3f(-xCoordinate, yCoordinate, -zCoordinate);
		glVertex3f(-xCoordinate, yCoordinate, -zCoordinate);
		glTexCoord3f(-xCoordinate, yCoordinate, zCoordinate);
		glVertex3f(-xCoordinate, yCoordinate, zCoordinate);
		glTexCoord3f(xCoordinate, yCoordinate, zCoordinate);
		glVertex3f(xCoordinate, yCoordinate, zCoordinate);
	glEnd();

	// Bottom
	glBegin(GL_QUADS);
		glTexCoord3f(xCoordinate, -yCoordinate, -zCoordinate);
		glVertex3f(xCoordinate, -yCoordinate, -zCoordinate);
		glTexCoord3f(-xCoordinate, -yCoordinate, -zCoordinate);
		glVertex3f(-xCoordinate, -yCoordinate, -zCoordinate);
		glTexCoord3f(-xCoordinate, -yCoordinate, zCoordinate);
		glVertex3f(-xCoordinate, -yCoordinate, zCoordinate);
		glTexCoord3f(xCoordinate, -yCoordinate, zCoordinate);
		glVertex3f(xCoordinate, -yCoordinate, zCoordinate);
	glEnd();
}

void renderingAstromedicompFont(void)
{
	// Function Declarations
	void Aeroplane(int);

	// Code
	glPushMatrix();
	// Aeroplane
	glPushMatrix();
		glTranslatef(xTranslationAeroplane, 0.0f, -4.0f);
		fprintf(gpFile, "aeropalne Translation = %f\n", xTranslationAeroplane);
		Aeroplane(2);
	glPopMatrix();
	
	// Astromedicomp
	glLoadIdentity();
	glColor3f(0.298f, 0.733f, 0.090f);
	glTranslatef(-6.0f, -0.4f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[0]);
	glCallLists(15, GL_UNSIGNED_BYTE, "ASTROMEDICOMP'S");
	glPopMatrix();
}

void renderingHullGroupFont(void)
{
	// Code
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glLoadIdentity();
	glTranslatef(-5.0f, 0.3f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[1]);
	glCallLists(10, GL_UNSIGNED_BYTE, "HULL GROUP");

	glLoadIdentity();
	glTranslatef(-4.2f, -1.2f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[1]);
	glCallLists(8, GL_UNSIGNED_BYTE, "PRESENTS");
	glPopMatrix();
}

void renderingPostCreditScenePoem(void)
{
	// Code
	glColor3f(1.0f, 0.0f, 0.0f);
	glLoadIdentity();
	glTranslatef(-2.5f, 0.0f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[2]);
	glCallLists(11, GL_UNSIGNED_BYTE, "मन वढाय वढाय");

	glColor3f(1.0f, 1.0f, 1.0f);
	glLoadIdentity();
	glTranslatef(-5.5f, -2.5f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(24, GL_UNSIGNED_BYTE, " Shubham  Balaso  Jadhav");
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
	glCallLists(24, GL_UNSIGNED_BYTE, " Shubham  Balaso  Jadhav");
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
	glTranslatef(-2.9f, 1.0f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(14, GL_UNSIGNED_BYTE, "Our    Captain");

	glLoadIdentity();
	glTranslatef(-3.7f, -0.2f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(16, GL_UNSIGNED_BYTE, "Ashwini  Bhutare");

	glLoadIdentity();
	glTranslatef(-1.3f, -1.2f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[2]);
	glCallLists(1, GL_UNSIGNED_BYTE, "&");

	glLoadIdentity();
	glTranslatef(-5.3f, -2.5f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(25, GL_UNSIGNED_BYTE, "All  Hull  Group  Members");
}

void renderingPostCreditSceneSir(void)
{
	// Code
	glColor3f(1.0f, 0.0f, 0.0f);
	glLoadIdentity();
	glTranslatef(-4.0f, 2.5f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[2]);
	glCallLists(28, GL_UNSIGNED_BYTE, "Thank You For Everything ...");

	glColor3f(1.0f, 1.0f, 1.0f);
	glLoadIdentity();
	glTranslatef(-5.8f, 0.5f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(26, GL_UNSIGNED_BYTE, "Dr.   Rama  Gokhale  Madam");

	glLoadIdentity();
	glTranslatef(-1.5f, -1.2f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[0]);
	glCallLists(1, GL_UNSIGNED_BYTE, "&");

	glLoadIdentity();
	glTranslatef(-5.4f, -2.5f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList[3]);
	glCallLists(31, GL_UNSIGNED_BYTE, "Dr.   Vijay  D.   Gokhale   Sir");
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

void fadeInOut()
{
	// Code
	glPushMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -2.0f);

	glBegin(GL_QUADS);
	glColor4f(0.0f, 0.0f, 0.0f, alphaValue);
	glVertex3f(10.0f, 10.0f, -1.2f);
	glVertex3f(-10.0f, 10.0f, -1.2f);
	glVertex3f(-10.0f, -10.0f, -1.2f);
	glVertex3f(10.0f, -10.0f, -1.2f);
	glEnd();

	glDisable(GL_BLEND);
	glPopMatrix();
}


// Load Textures
BOOL LoadGLTexture(GLuint* Texture, TCHAR ImageResourceID[], GLboolean state)
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
		if (state == 1)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		else
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

// Setup Fonts
void setupFont(TCHAR fontType[])
{
	// Local Variable Declarations
	//GLYPHMETRICSFLOAT agmf_title[128];
	HFONT hFont = NULL;
	LOGFONT logFont;

	// Code
	// setup the Font Characteristics

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

	if (strlen(fontType) == 7)
	{
		logFont.lfHeight = 100;
		strcpy(logFont.lfFaceName, "BALLADE");
	}
	else if (strlen(fontType) == 18)
	{
		logFont.lfHeight = 200;
		strcpy(logFont.lfFaceName, "The Artisan Marker");
	}
	else if (strlen(fontType) == 15)
	{
		logFont.lfHeight = 60;
		strcpy(logFont.lfFaceName, "CROCHET PATTERN");
	}
	else if (strlen(fontType) == 35)
	{
		logFont.lfHeight = 140;
		strcpy(logFont.lfFaceName, "Magnific Chaos Personal Use Regular");
	}

	// Create the font and Display list
	hFont = CreateFontIndirect(&logFont);
	SelectObject(ghdc, hFont);

	// Create display lists for glyphs 0 through 128
	//wglUseFontOutlines(ghdc, 0, 128, nFontList, 0.0f, 0.3f, WGL_FONT_POLYGONS, agmf_title);

	if (strlen(fontType) == 7)
	{
		nFontList[0] = glGenLists(128);
		wglUseFontBitmaps(ghdc, 0, 128, nFontList[0]);
	}
	else if (strlen(fontType) == 18)
	{
		nFontList[1] = glGenLists(128);
		wglUseFontBitmaps(ghdc, 0, 128, nFontList[1]);
	}
	else if (strlen(fontType) == 15)
	{
		nFontList[2] = glGenLists(128);
		wglUseFontBitmaps(ghdc, 0, 128, nFontList[2]);
	}
	else if (strlen(fontType) == 35)
	{
		nFontList[3] = glGenLists(128);
		wglUseFontBitmaps(ghdc, 0, 128, nFontList[3]);
	}

	DeleteObject(hFont);
}

// Load Cubemap
GLboolean LoadGLCubemap()
{
	// variable declarations
	GLboolean bResult = FALSE;

	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for (int i = 0; i < 6; i++)
	{
		HBITMAP hBitmap = NULL;
		BITMAP bmp;

		// code
		hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), facesCubemap[i], IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		if (hBitmap)
		{
			bResult = true;
			// os function to get image information
			GetObject(hBitmap, sizeof(BITMAP), &bmp);

			fprintf(gpFile, "bitmap  %d  is loaded\n", i);
			gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				GL_RGB,
				bmp.bmWidth,
				bmp.bmHeight,
				GL_BGR_EXT,
				GL_UNSIGNED_BYTE,
				bmp.bmBits);
		}
		else
		{
			fprintf(gpFile, "bitmap  %d  is not loaded\n", i);
			DestroyWindow(ghwnd);
		}
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return bResult;
}

// Mountain Init
void initSurface(void)
{
	// Local Variable Declarations
	GLint u, v;

	// Code
	for (u = 0; u < I; u++)
	{
		for (v = 0; v < J; v++)
		{
			controlPoints[u][v][0] = 2.0f * ((GLfloat)u - 1.5f);
			controlPoints[u][v][1] = 2.0f * ((GLfloat)v - 1.5f);

			if ((u == 1 || u == 2) && (v == 1 || v == 2))
				controlPoints[u][v][2] = 6.0f;
			else
				controlPoints[u][v][2] = 0.0f;

		}
	}
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

	if (texture_water)
	{
		glDeleteTextures(1, &texture_water);
	}

	if (texture_mountain1)
	{
		glDeleteTextures(1, &texture_mountain1);
		texture_mountain1 = NULL;
	}

	if (texture_mountain)
	{
		glDeleteTextures(1, &texture_mountain);
		texture_mountain = NULL;
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

	if (texture_smoke)
	{
		glDeleteTextures(1, &texture_smoke);
		texture_smoke = NULL;
	}

	if (nurbs)
	{
		gluDeleteNurbsRenderer(nurbs);
		nurbs = NULL;
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
