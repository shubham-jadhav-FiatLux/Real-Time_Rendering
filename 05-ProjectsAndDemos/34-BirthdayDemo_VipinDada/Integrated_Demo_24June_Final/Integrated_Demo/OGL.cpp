#include <windows.h>
#include <stdio.h> //for file io functions
#include <stdlib.h> // for exit() function
#include "OGL.h"

//OpenGL header files
#include <GL/gl.h>
#include <GL/glu.h> //graphic library utility
#define _USE_MATH_DEFINES 1
#include <math.h>

//macros
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define R(c) ((GLfloat)(c/255.0f))
#define G(c) ((GLfloat)(c/255.0f))
#define B(c) ((GLfloat)(c/255.0f))
#define DEGREE_TO_RADIAN(angle) ((float) angle * M_PI / 180.0f)

//OpenGL Libraries
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")
#pragma comment(lib, "Winmm.lib")

//structs
struct Color
{
	float red;
	float green;
	float blue;
};

//enums
enum scenes
{
	TITLE,
	EARTH,
	PYRAMID,
	INVERTED_PYRAMID,
	SMALL_CRYPTEX,
	CRYPTEX,
	CREDITS
};

//global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//global variables
HWND ghwnd = NULL;
BOOL gbFullscreen = FALSE;
FILE *gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

BOOL transit = FALSE;
GLUquadric *quadric = NULL;

BOOL toTranslateScene[] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};

/*************Pyramid related Variables*****************/
float anglePyramid = 0.0f;
GLuint texture_pyramid;
float zAxisValue = 16.0f;
float yAxisValue = 0.0f;
float pyramidZPos = -16.0f;

/********************Yogesh_Pyramid********************/
GLfloat lightAmbientZero[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat ligthDefusedZero[] = { 1.0f, 0.0f, 0.0f, 1.0f }; //red
GLfloat ligthSpecularZero[] = { 1.0f, 0.0f, 0.0f, 1.0f };//red
GLfloat ligthPositionZero[] = { 0.0f, 2.0f, 0.0f, 1.0f };
GLfloat spot_direction[] = { 0.0, -2.0, 0.0 };
/*******************************************************/

/********************Amit FadeInOut********************/
float gAlphaOld = 1.0f;
GLuint texture_map;
GLuint texture_sideWall;
GLuint texture_stone;
GLuint texture_marble;
GLuint texture_palace;
GLuint texture_jamin;
GLuint texture_floor;
BOOL isCameraInsidePyramid = FALSE;
/*******************************************************/

/********************Anusha Earth********************/
GLuint texture_earth;
GLfloat angleEarth = 0.1f;
GLfloat onetime = 20.0f;
GLfloat angleCube = 0.0f;
BOOL toRotateEarth = TRUE;
/*******************************************************/

/******************Shubham Small Cryptex**********************/
GLuint texture_gold;
GLuint texture_digits;
GLuint texture_letter_j;
GLuint texture_letter_u;
GLuint texture_letter_l;
GLuint texture_letter_y;
float angle_cryptex = 0.0f;
float angleRotate_cryptex = 90.0f;
float x1_camPos_cryptex = 0.0f;
float y1_camPos_cryptex = 0.0f;
float z1_camPos_cryptex = 0.0f;
GLUquadric* quadric_cryptex = NULL;
/*******************************************************/

/******************Shubham Big Cryptex**********************/
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

//font for title
HFONT hFont_title = NULL;
GLint nFontList_title;
LOGFONT logfont_title;

//font for birthday text
HFONT hFont_bday = NULL;
GLint nFontList_bday;
LOGFONT logfont_bday;
/******************************************************/

/*******************Varad PostWork**********************/
BOOL isSoundPlyaed = FALSE;
GLfloat yTranslationInvertedPyramid = -2.0f;
GLfloat initSmallCryptexYTranslation = 0.0f;
/******************************************************/

/*******************Dynamic Fade-in-out**************************/
GLfloat gAlpha = 0.0f;
float multVal = 1.0f;
BOOL toStopFading = FALSE;
BOOL toFade = FALSE;
BOOL toTransitNextFade = FALSE;
/********************************************************/

// Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function declarations
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);
	void ToggleFullscreen(void);

	//variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	BOOL bDone = FALSE;
	int screenHeight;
	int screenWidth;
	int iRetVal = 0;

	//code
	if(fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation of Log file failed. Exitting..."), TEXT("File I/O Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile,"Log File successfully created.\n");
	}
	//initialization of WNDCLASSEX structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// Registring above wndclass
	RegisterClassEx(&wndclass);

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Create the window
	hwnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			szAppName,
			TEXT("HULL GROUP"),
			WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
			(screenWidth/2 - WINDOW_WIDTH/2),
			(screenHeight/2 - WINDOW_HEIGHT/2),
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			NULL,
			NULL,
			hInstance,
			NULL
			);

	ghwnd = hwnd;

	//initialize
	iRetVal = initialize();
	if(iRetVal == -1)
	{
		fprintf(gpFile, "Choose Pixel Format Failed...\n\n");
		uninitialize();
	}
	else if(iRetVal == -2)
	{
		fprintf(gpFile, "Set Pixel Format Failed...\n\n");
		uninitialize();
	}
	else if(iRetVal == -3)
	{
		fprintf(gpFile, "Create OpenGLContext Failed...\n\n");
		uninitialize();	
	}
	else if(iRetVal == -4)
	{
		fprintf(gpFile, "Making OpenGL Context as Curret Context Failed...\n\n");
		uninitialize();
	}
	else
	{
		fprintf(gpFile, "initialize successfull\n\n");
	}

	// Show window
	ShowWindow(hwnd, iCmdShow);

	//fullscreen
	ToggleFullscreen();

	// Foregrounding and focusing the window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//game loop
	while(bDone == FALSE)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				bDone = TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if(gbActiveWindow == TRUE)
			{
				//render the scene
				display();

				//update the scene
				update();
			}
		}
	}

	uninitialize();

	return((int)msg.wParam);
}

// Callback function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function declarations
	void ToggleFullscreen(void);
	void resize(int, int);

	// code
	switch(iMsg)
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
		switch(wParam)
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
		switch(wParam)
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
	//variable declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;

	//code
	wp.length = sizeof(WINDOWPLACEMENT);

	if(gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE); // GWL -> GetWindowLong
		if(dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);

			if(GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, (mi.rcMonitor.right - mi.rcMonitor.left), (mi.rcMonitor.bottom - mi.rcMonitor.top), SWP_NOZORDER | SWP_FRAMECHANGED); //HWND_TOP -> WS_OVERLAPPEDWINDOW  //SWP_FRAMECHANGED -> WM_LCCALCSIZE (consider non-client area)
			}
			ShowCursor(FALSE);
			gbFullscreen = TRUE;
		}
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED); //already placed hence set to 0

		ShowCursor(TRUE);
		gbFullscreen = FALSE;
	}

	return;
}

int initialize(void)
{
	//function declarations
	void resize(int, int);
	BOOL LoadGLTexture(GLuint *, TCHAR[]);
	BOOL LoadGLTextureLetters(GLuint *, TCHAR[]);
	void SetupRC(void);
	void SetupRCTitle(void);

	//variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	//code
	//initialization of PIXELFORMATDESCRIPTOR structure
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;// 24 bit also can be done

	//GetDC
	ghdc = GetDC(ghwnd);

	//choose pixel format
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if(iPixelFormatIndex == 0)
	{
		return -1;
	}

	//Set the choosen pixel format
	if(SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		return -2;
	}

	//Create OpenGL rendering context
	ghrc = wglCreateContext(ghdc);
	if(ghrc == NULL)
	{
		return -3;
	}

	//Make the rendering context as the current context
	if(wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		return -4;
	}

	//Here starts OpenGL code
	//Clear the screen using Black color
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClearColor(0.53f, 0.5f, 0.98f, 0.3f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//depth related changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH); //GL_FLAT can also be given
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//depthrelated changes
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	quadric = gluNewQuadric();

	//Textures
	{
		if(LoadGLTexture(&texture_earth, MAKEINTRESOURCE(IDEARTH)) == FALSE)
		{
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for earth!!!\n");
			//uninitialize();
			return -5;
		}

		if (LoadGLTexture(&texture_map, MAKEINTRESOURCE(IDMAP)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for Map!!!");
			//uninitialize();
			return -6;
		}

		if (LoadGLTexture(&texture_sideWall, MAKEINTRESOURCE(IDSIDEWALL)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for Wall!!!");
			//uninitialize();
			return -7;
		}

		if (LoadGLTexture(&texture_stone, MAKEINTRESOURCE(IDBITMAP_STONE)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for diamond!!!");
			//uninitialize();
			return -8;
		}

		if (LoadGLTexture(&texture_marble, MAKEINTRESOURCE(IDBITMAP_MARBLE)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for texture_marble!!!");
			//uninitialize();
			return -9;
		}

		if (LoadGLTexture(&texture_palace, MAKEINTRESOURCE(IDBITMAP_PALACE)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for texture_palace!!!");
			//uninitialize();
			return -10;
		}

		if (LoadGLTexture(&texture_jamin, MAKEINTRESOURCE(IDBITMAP_JAMIN)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for texture_jamin!!!");
			//uninitialize();
			return -11;
		}

		if (LoadGLTexture(&texture_floor, MAKEINTRESOURCE(IDBITMAP_FLOOR)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for texture_floor!!!");
			//uninitialize();
			return -12;
		}

		/*******************small cryptex textures************************/
		if (LoadGLTexture(&texture_gold, MAKEINTRESOURCE(IDBITMAP_GOLD)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for texture_gold!!!");
			//uninitialize();
			return -13;
		}

		if (LoadGLTextureLetters(&texture_digits, MAKEINTRESOURCE(IDBITMAP_DIGITS)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for texture_digits!!!");
			//uninitialize();
			return -14;
		}
		if (LoadGLTextureLetters(&texture_letter_j, MAKEINTRESOURCE(IDBITMAP_LETTER_J)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for texture_letter_j!!!");
			//uninitialize();
			return -15;
		}

		if (LoadGLTextureLetters(&texture_letter_u, MAKEINTRESOURCE(IDBITMAP_LETTER_U)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for texture_letter_u!!!");
			//uninitialize();
			return -16;
		}

		if (LoadGLTextureLetters(&texture_letter_l, MAKEINTRESOURCE(IDBITMAP_LETTER_L)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for texture_letter_l!!!");
			//uninitialize();
			return -17;
		}

		if (LoadGLTextureLetters(&texture_letter_y, MAKEINTRESOURCE(IDBITMAP_LETTER_Y)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for texture_letter_y!!!");
			//uninitialize();
			return -18;
		}

		/*******************big cryptex textures************************/
		if (LoadGLTexture(&texture_paper, MAKEINTRESOURCE(IDBITMAP_PAPER)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for texture_paper!!!");
			//uninitialize();
			return -19;
		}

		if (LoadGLTexture(&texture_paper_roll, MAKEINTRESOURCE(IDBITMAP_PAPERROLL)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for texture_paper_roll!!!");
			//uninitialize();
			return -20;
		}

		if (LoadGLTexture(&texture_disk, MAKEINTRESOURCE(IDBITMAP_DISK)) == FALSE) {
			fprintf(gpFile, "[ERROR] failed LoadGLTexture for texture_disk!!!");
			//uninitialize();
			return -21;
		}
		/****************************************************************/
	}

	//enabling the texture
	glEnable(GL_TEXTURE_2D);

	// Create Quadric for small cryptex
	quadric_cryptex = gluNewQuadric();
	/******************************************************************/

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	//glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbientZero);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuseZero);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecularZero);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPositionZero);
	//glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2, GL_AMBIENT, LightAmbientOne);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, LightDiffuseOne);
	glLightfv(GL_LIGHT2, GL_SPECULAR, LightSpecularOne);
	glLightfv(GL_LIGHT2, GL_POSITION, LightPositionOne);
	//glEnable(GL_LIGHT2);

	glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbientOne);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuseOne);
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecularOne);
	glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininessOne);

	SetupRC();
	SetupRCTitle();

	//warm-up resize call
	resize(WINDOW_WIDTH, WINDOW_HEIGHT);

	return 0;
}

void resize(int width, int height)
{
	//code
	if(height == 0)
	{
		height = 1;// To avoid divided by zero in future code
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	//function declarations
	void displayTitle(void);
	void mapLoad(void);
	void scene_Pyramid(void);
	//void FadeInOut(void);
	void FadeInOut(BOOL *, GLfloat, Color);
	void scene_Earth(void);
	void display_bigCryptex(void);

	//code
	//play song
	if(!isSoundPlyaed)// conition to call function only once
	{
		PlaySound(MAKEINTRESOURCE(BGTRACK), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
		isSoundPlyaed = TRUE;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(!toTranslateScene[TITLE])
	{
		displayTitle();
	}
	else
	{
		if(!toTranslateScene[PYRAMID] && !toTranslateScene[CRYPTEX] && !toTranslateScene[CREDITS])
			toTranslateScene[EARTH] = TRUE;
	}

	if(toTranslateScene[EARTH] && !toTranslateScene[CRYPTEX] && !toTranslateScene[CREDITS])
	{
		mapLoad();
		scene_Earth();
	}

	if(toTranslateScene[PYRAMID] && !toTranslateScene[EARTH]  && !toTranslateScene[CREDITS])
	{
		scene_Pyramid();
	}
	
	if(toTranslateScene[CRYPTEX] && !toTranslateScene[PYRAMID] && !toTranslateScene[EARTH] && !toTranslateScene[CREDITS])
	{
		fprintf(gpFile, "BIG C\n");
		toFade = TRUE;
		//Big Cryptex scene starts here
		display_bigCryptex();
	}

	//FadeInOut();
	if(!toFade)
		FadeInOut(&toTranslateScene[TITLE], 0.004f, {0.0f, 0.0f, 0.0f});

	SwapBuffers(ghdc);
}

void update_fast(void)
{
	//code
	/*********Pyramidal Update*************/
	if(toTranslateScene[PYRAMID])
	{
		if(zAxisValue <= 1.5f)
		{
			
			if (anglePyramid <= 360.0f)
				anglePyramid += 1.00f;
			else
			{
				//toTranslateScene[CRYPTEX] = TRUE;
				toTransitNextFade = TRUE;
				toTranslateScene[PYRAMID] = FALSE;
			}
			
		}
		else if (zAxisValue >= 1.5f)
		{
			zAxisValue -= 0.1f;
		}

		if(pyramidZPos < 0.0f)
		{
			pyramidZPos += 0.05f;
		}
	}
	/**********************/

	/**********FadeInOut************/
	/*gAlpha -= 0.005f;
	if (gAlpha <= 0.0f)
	{
		gAlpha = 0.0f;
		//toTranslateScene[TITLE] = TRUE;
	}*/

	/**********************/

	/***********Earth***********/
	if(toTranslateScene[EARTH])
	{
		if(toRotateEarth)
		{
			angleEarth += 2.0f;//0.35f;
			if (angleEarth >= 375.0f)
				angleEarth -= 375.0f;

			onetime -= 0.1f;
			if (onetime <= 0.5f)
				onetime = 0.8f;

			if(angleEarth >= 360.0f)
			toRotateEarth = FALSE;
		}
		else
		{
			onetime -= 0.2f;
			if(onetime <= -2.5f)
			{
				onetime = -2.5f;
				toTranslateScene[PYRAMID] = TRUE;
				toTranslateScene[EARTH] = FALSE;
			}
		}
	}
	/**********************/

	/***************Inverted Pyramid Translation************************/
	if(toTranslateScene[INVERTED_PYRAMID])
	{
		yTranslationInvertedPyramid += 0.0015f;
		if(yTranslationInvertedPyramid >= 0.0f)
		{
			yTranslationInvertedPyramid = 0.0f;
			toTranslateScene[SMALL_CRYPTEX] = TRUE;
		}
	}

	/**********Small Cryptex************/
	if(toTranslateScene[SMALL_CRYPTEX])
	{
		static const float radius_cryptex = 0.002f;
		static float multVal = 1.0f;

		angle_cryptex += 1.0f;
		if(angleRotate_cryptex >= 360.0f)
			angleRotate_cryptex -= 360.0f;

		if(initSmallCryptexYTranslation >= 3.3f)
		{
			y1_camPos_cryptex = y1_camPos_cryptex + (multVal * 0.007f);
			if(fabs(y1_camPos_cryptex) >= 0.5f)
			{
				if(y1_camPos_cryptex / fabs(y1_camPos_cryptex) <= -1.0f)
				{
					multVal *= y1_camPos_cryptex / fabs(y1_camPos_cryptex);
				}
				else
				{
					multVal *= -1.0f;
				}
			}
		}

		x1_camPos_cryptex = radius_cryptex * cos(DEGREE_TO_RADIAN(angleRotate_cryptex));
		z1_camPos_cryptex = radius_cryptex * sin(DEGREE_TO_RADIAN(angleRotate_cryptex));

		angleRotate_cryptex += 1.0f;
		if(angleRotate_cryptex >= 360.0f)
		{
			angleRotate_cryptex -= 360.0f;	
		}

		initSmallCryptexYTranslation += 0.004f;
		if(initSmallCryptexYTranslation >= 3.3f)
			initSmallCryptexYTranslation = 3.3f;
	}
	/**********************************/

	/**********************Big Cryptex************************************/
	if(toTranslateScene[CRYPTEX])
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
	/*****************************************************************/
}

void update(void)
{
	//code
	/*********Pyramidal Update*************/
	if(toTranslateScene[PYRAMID])
	{
		if(zAxisValue <= 1.5f)
		{
			
			if (anglePyramid <= 360.0f)
				anglePyramid += 0.155f;
			else
			{
				//toTranslateScene[CRYPTEX] = TRUE;
				toTransitNextFade = TRUE;
				toTranslateScene[PYRAMID] = FALSE;
			}
			
		}
		else if (zAxisValue >= 1.5f)
		{
			zAxisValue -= 0.015f;
		}

		if(pyramidZPos < 0.0f)
		{
			pyramidZPos += 0.05f;
		}
	}
	/**********************/

	/**********FadeInOut************/
	/*gAlpha -= 0.005f;
	if (gAlpha <= 0.0f)
	{
		gAlpha = 0.0f;
		//toTranslateScene[TITLE] = TRUE;
	}*/

	/**********************/

	/***********Earth***********/
	if(toTranslateScene[EARTH])
	{
		if(toRotateEarth)
		{
			angleEarth += 0.55f;//0.35f;
			if (angleEarth >= 375.0f)
				angleEarth -= 375.0f;

			onetime -= 0.1f;
			if (onetime <= 0.5f)
				onetime = 0.5f;

			if(angleEarth >= 360.0f)
			toRotateEarth = FALSE;
		}
		else
		{
			onetime -= 0.01f;
			if(onetime <= -2.5f)
			{
				onetime = -2.5f;
				toTranslateScene[PYRAMID] = TRUE;
				toTranslateScene[EARTH] = FALSE;
			}
		}
	}
	/**********************/

	/***************Inverted Pyramid Translation************************/
	if(toTranslateScene[INVERTED_PYRAMID])
	{
		yTranslationInvertedPyramid += 0.00135f;
		if(yTranslationInvertedPyramid >= 0.0f)
		{
			yTranslationInvertedPyramid = 0.0f;
			toTranslateScene[SMALL_CRYPTEX] = TRUE;
		}
	}

	/**********Small Cryptex************/
	if(toTranslateScene[SMALL_CRYPTEX])
	{
		static const float radius_cryptex = 0.002f;
		static float multVal = 1.0f;

		angle_cryptex += 1.0f;
		if(angleRotate_cryptex >= 360.0f)
			angleRotate_cryptex -= 360.0f;

		if(initSmallCryptexYTranslation >= 3.3f)
		{
			y1_camPos_cryptex = y1_camPos_cryptex + (multVal * 0.007f);
			if(fabs(y1_camPos_cryptex) >= 0.5f)
			{
				if(y1_camPos_cryptex / fabs(y1_camPos_cryptex) <= -1.0f)
				{
					multVal *= y1_camPos_cryptex / fabs(y1_camPos_cryptex);
				}
				else
				{
					multVal *= -1.0f;
				}
			}
		}

		x1_camPos_cryptex = radius_cryptex * cos(DEGREE_TO_RADIAN(angleRotate_cryptex));
		z1_camPos_cryptex = radius_cryptex * sin(DEGREE_TO_RADIAN(angleRotate_cryptex));

		angleRotate_cryptex += 1.079f;
		if(angleRotate_cryptex >= 360.0f)
		{
			angleRotate_cryptex -= 360.0f;	
		}

		initSmallCryptexYTranslation += 0.006f;
		if(initSmallCryptexYTranslation >= 3.3f)
			initSmallCryptexYTranslation = 3.3f;
	}
	/**********************************/

	/**********************Big Cryptex************************************/
	if(toTranslateScene[CRYPTEX])
	{
		// Local Variable declarations
		float radius = 5.0f;
		
		// Code
		angle += 1.0f;

		if (z1 >= 5.0f)
			z1 -= 0.060f;
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
	/*****************************************************************/
}

void uninitialize(void)
{
	//function declarations
	void ToggleFullscreen(void);

	//code
	if(gbFullscreen)
	{
		ToggleFullscreen();
	}

	if(wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if(ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if(ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if(ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	glDeleteTextures(1, &texture_sideWall);
	glDeleteTextures(1, &texture_stone);
	glDeleteTextures(1, &texture_marble);
	glDeleteTextures(1, &texture_palace);
	glDeleteTextures(1, &texture_jamin);
	glDeleteTextures(1, &texture_floor);
	glDeleteTextures(1, &texture_gold);
	glDeleteTextures(1, &texture_digits);
	glDeleteTextures(1, &texture_letter_j);
	glDeleteTextures(1, &texture_letter_u);
	glDeleteTextures(1, &texture_letter_l);
	glDeleteTextures(1, &texture_letter_y);
	glDeleteTextures(1, &texture_paper);
	glDeleteTextures(1, &texture_paper_roll);
	glDeleteTextures(1, &texture_disk);
	texture_sideWall = NULL;
	texture_stone = NULL;
	texture_marble = NULL;
	texture_palace = NULL;
	texture_jamin = NULL;
	texture_floor = NULL;
	texture_gold = NULL;
	texture_digits = NULL;
	texture_letter_j = NULL;
	texture_letter_u = NULL;
	texture_letter_l = NULL;
	texture_letter_y = NULL;
	texture_paper = NULL;
	texture_paper_roll = NULL;
	texture_disk = NULL;

	//stop playing song
		if(isSoundPlyaed)
			PlaySound(NULL, 0, 0);


	if(gpFile)
	{
		fprintf(gpFile, "Log File successfully closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

BOOL LoadGLTexture(GLuint *texture, TCHAR imageResourcId[])
{
	//variable declarations
	HBITMAP hbitmap = NULL;
	BITMAP bmp;
	BOOL bResult = FALSE;

	//code
	hbitmap = (HBITMAP) LoadImage(
		GetModuleHandle(NULL),
		imageResourcId,
		IMAGE_BITMAP,
		0,
		0,
		LR_CREATEDIBSECTION
		);
	if(hbitmap)
	{
		bResult = TRUE;
		GetObject(hbitmap, sizeof(bmp), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//create the texture
		gluBuild2DMipmaps(
			GL_TEXTURE_2D,
			3,
			bmp.bmWidth,
			bmp.bmHeight,
			GL_BGR_EXT,
			GL_UNSIGNED_BYTE,
			bmp.bmBits
			);
		glBindTexture(GL_TEXTURE_2D, 0);

		DeleteObject(hbitmap);
	}
	return bResult;
}

//================================================================================
/* -----------------------------FADE-IN-OUT----------------------------------*/
void FadeInOut(BOOL *readyToTrasnit, GLfloat fadingFactor, Color fadingColor)
{
	/********Valuer updations***********/
	static float multVal = 1.0f;

	if(!toStopFading)
	{
		gAlpha = gAlpha + (multVal * fadingFactor);
		if(gAlpha <= 0.0f)
		{
			multVal = 1.0f;
			toStopFading = TRUE; //flag to stop repeatative fading
		}
		else if(gAlpha >= 1.0f)
		{
			fprintf(gpFile, "[READY to transit]\n");
			*readyToTrasnit = TRUE;
			multVal = -1.0f;
		}
	}

	/********************/
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLoadIdentity();
	glTranslatef(0.0f,0.0f, -2.0f);
	glBegin(GL_QUADS);

	//glColor4f(1.0f, 1.0f, 1.0f, gAlpha);
	glColor4f(fadingColor.red, fadingColor.green, fadingColor.blue, gAlpha);

	glVertex3f(11.0f, 11.0f, 1.2f);
	glVertex3f(-10.0f, 11.0f, 1.2f);
	glVertex3f(-10.0f, -11.0f, 1.2f);
	glVertex3f(10.0f, -11.0f, 1.2f);

	glEnd();
	glDisable(GL_BLEND);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

/* -----------------------------SCENE_PYARAMID----------------------------------*/
void scene_Pyramid(void)
{
	//function declaration
	void drawBaseOfPyramid(void);
	void drawMainEntranceOfPyramid();
	void drawPyramid(void);

	glLoadIdentity();

	if ( anglePyramid <= 360.0f)
		gluLookAt(0.0f, -0.85f, zAxisValue, 0.0f, -0.85f, 0.0f, 0.0f, 1.0f, 0.0f);
	else
	{
		//gluLookAt(0.0f, -0.7f, 1.5f, 0.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f);
		toTranslateScene[CRYPTEX] = TRUE;
		toTranslateScene[PYRAMID] = FALSE;
		fprintf(gpFile, "INSIDE ELSE scene 3!!!\n");
	}

	glTranslatef(0.0f,-1.0f,0.0f);

	if (zAxisValue <= 1.5f && anglePyramid<=360.0f)
	{
		isCameraInsidePyramid = TRUE;
		toTranslateScene[INVERTED_PYRAMID] = TRUE;
		gluLookAt(0.0f, -0.04f, -0.3f, 0.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f);
		//gluLookAt(0.0f, -0.5f, 0.0, 0.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(anglePyramid, 0.0f, 1.0f, 0.0f); //Spin
	}
	/*else if (zAxisValue >= 1.5f && anglePyramid >= 360.0f)
	{
		toTranslateScene[CRYPTEX] = TRUE;
		fprintf(gpFile, "INSIDE ELSE scene 3!!!\n");
	}*/

	 glTranslatef(0.0f, 0.0f, pyramidZPos);
	 drawBaseOfPyramid();
	 drawMainEntranceOfPyramid();
	 drawPyramid();
}

void drawPyramid()
{
	//front face3
	glLineWidth(5.0f);
	glColor3f(R(49), G(78), B(82));
	glBegin(GL_LINE_LOOP);	
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(-1.5f, 0.0f, 1.5f);
		glVertex3f(1.5f, 0.0f, 1.5f);
	glEnd();

	glLineWidth(1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
		glVertex3f(1.0f, 0.0f, 1.5f);
		glVertex3f(-0.4f, 1.5f, 0.38f);

		//glVertex3f(0.5f, 0.0f, 1.5f);
		glVertex3f(0.27f, 0.23f, 1.32f);
		glVertex3f(-0.65f, 1.15f, 0.65f);

		glVertex3f(0.0f, 0.0f, 1.5f);
		glVertex3f(-0.9f, 0.8f, 0.9f);

		glVertex3f(-0.5f, 0.0f, 1.5f);
		glVertex3f(-1.1f, 0.5f, 1.12f);

		glVertex3f(-1.0f, 0.0f, 1.5f);
		glVertex3f(-1.3f, 0.24f, 1.32f);

		//===============================

		glVertex3f(-1.0f, 0.0f, 1.5f);
		glVertex3f(0.4f, 1.5f, 0.38f);

		//glVertex3f(-0.5f, 0.0f, 1.5f);
		glVertex3f(-0.27f, 0.23f, 1.32f);
		glVertex3f(0.65f, 1.15f, 0.65f);


		glVertex3f(0.0f, 0.0f, 1.5f);
		glVertex3f(0.9f, 0.8f, 0.9f);

		glVertex3f(0.5f, 0.0f, 1.5f);
		glVertex3f(1.1f, 0.5f, 1.12f);

		glVertex3f(1.0f, 0.0f, 1.5f);
		glVertex3f(1.3f, 0.24f, 1.32f);
	glEnd();


	//Back face
	glLineWidth(5.0f);
	glColor3f(R(49), G(78), B(82));
	glBegin(GL_LINE_LOOP);		
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(-1.5f, 0.0f, -1.5f);
		glVertex3f(1.5f, 0.0f, -1.5f);
	glEnd();


	glLineWidth(1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
		glVertex3f(1.0f, 0.0f, -1.5f);
		glVertex3f(-0.4f, 1.5f, -0.38f);

		glVertex3f(0.5f, 0.0f, -1.5f);
		glVertex3f(-0.65f, 1.15f, -0.65f);

		glVertex3f(0.0f, 0.0f, -1.5f);
		glVertex3f(-0.9f, 0.8f, -0.9f);

		glVertex3f(-0.5f, 0.0f, -1.5f);
		glVertex3f(-1.1f, 0.5f, -1.12f);

		glVertex3f(-1.0f, 0.0f, -1.5f);
		glVertex3f(-1.3f, 0.24f, -1.32f);

		//===============================

		glVertex3f(-1.0f, 0.0f, -1.5f);
		glVertex3f(0.4f, 1.5f, -0.38f);

		glVertex3f(-0.5f, 0.0f, -1.5f);
		glVertex3f(0.65f, 1.15f, -0.65f);


		glVertex3f(0.0f, 0.0f, -1.5f);
		glVertex3f(0.9f, 0.8f, -0.9f);

		glVertex3f(0.5f, 0.0f, -1.5f);
		glVertex3f(1.1f, 0.5f, -1.12f);

		glVertex3f(1.0f, 0.0f, -1.5f);
		glVertex3f(1.3f, 0.24f, -1.32f);
	glEnd();

	//left
	glLineWidth(5.0f);
	glColor3f(R(49), G(78), B(82));
	glBegin(GL_LINE_LOOP);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(1.5f, 0.0f, 1.5f);
		glVertex3f(1.5f, 0.0f, -1.5f);
		glEnd();

		glLineWidth(1.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);

		glVertex3f(-1.5f, 0.0f, 1.0f);
		glVertex3f(-0.38f, 1.5f, -0.4f);

		glVertex3f(-1.5f, 0.0f, 0.5f);
		glVertex3f(-0.65f, 1.15f, -0.65f);

		glVertex3f(-1.5f, 0.0f, 0.0f);
		glVertex3f(-0.9f, 0.8f, -0.9f);

		glVertex3f(-1.5f, 0.0f, -0.5f);
		glVertex3f(-1.12f, 0.5f, -1.1f);

		glVertex3f(-1.5f, 0.0f, -1.0f);
		glVertex3f(-1.32f, 0.24f, -1.3f);

		////===============================

		glVertex3f(-1.5f, 0.0f, -1.0f);
		glVertex3f(-0.38f, 1.5f, 0.4f);

		glVertex3f(-1.5f, 0.0f, -0.5f);
		glVertex3f(-0.65f, 1.15f, 0.65f);

		glVertex3f(-1.5f, 0.0f, 0.0f);
		glVertex3f(-0.9f, 0.8f, 0.9f);

		glVertex3f(-1.5f, 0.0f, 0.5f);
		glVertex3f(-1.12f, 0.5f, 1.1f);

		glVertex3f(-1.5f, 0.0f, 1.0f);
		glVertex3f(-1.32f, 0.24f, 1.3f);
	glEnd();


	//right
	glLineWidth(5.0f);
	glColor3f(R(49), G(78), B(82));
	glBegin(GL_LINE_LOOP);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(-1.5f, 0.0f, 1.5f);
		glVertex3f(-1.5f, 0.0f, -1.5f);
		glEnd();

		glLineWidth(1.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);

		glVertex3f(1.5f, 0.0f, 1.0f);
		glVertex3f(0.38f, 1.5f, -0.4f);

		glVertex3f(1.5f, 0.0f, 0.5f);
		glVertex3f(0.65f, 1.15f, -0.65f);

		glVertex3f(1.5f, 0.0f, 0.0f);
		glVertex3f(0.9f, 0.8f, -0.9f);

		glVertex3f(1.5f, 0.0f, -0.5f);
		glVertex3f(1.12f, 0.5f, -1.1f);

		glVertex3f(1.5f, 0.0f, -1.0f);
		glVertex3f(1.32f, 0.24f, -1.3f);

		////===============================

		glVertex3f(1.5f, 0.0f, -1.0f);
		glVertex3f(0.38f, 1.5f, 0.4f);

		glVertex3f(1.5f, 0.0f, -0.5f);
		glVertex3f(0.65f, 1.15f, 0.65f);

		glVertex3f(1.5f, 0.0f, 0.0f);
		glVertex3f(0.9f, 0.8f, 0.9f);

		glVertex3f(1.5f, 0.0f, 0.5f);
		glVertex3f(1.12f, 0.5f, 1.1f);

		glVertex3f(1.5f, 0.0f, 1.0f);
		glVertex3f(1.32f, 0.24f, 1.3f);
	glEnd();
}

void drawMainEntranceOfPyramid()
{

	//main entrance borders
	glLineWidth(5.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
		glVertex3f(0.27f, 0.23f, 1.3f);
		glVertex3f(-0.27f, 0.23f, 1.3f);

		glVertex3f(0.27f, 0.23f, 1.5f);
		glVertex3f(-0.27f, 0.23f, 1.5f);

		glVertex3f(-0.27f, 0.23f, 1.3f);
		glVertex3f(-0.27f, 0.23f, 1.5f);

		glVertex3f(0.27f, 0.23f, 1.3f);
		glVertex3f(0.27f, 0.23f, 1.5f);

		glVertex3f(0.27f, 0.23f, 1.5f);
		glVertex3f(0.5f, 0.0f, 1.5f);

		glVertex3f(-0.27f, 0.23f, 1.5f);
		glVertex3f(-0.5f, 0.0f, 1.5f);

		glVertex3f(0.27f, 0.23f, 1.3f);
		glVertex3f(0.5f, -0.02f, 1.3f);

		glVertex3f(-0.27f, 0.23f, 1.3f);
		glVertex3f(-0.5f, -0.02f, 1.3f);



	glEnd();

	//main entrance 
	glBegin(GL_QUADS);
	//front
	//glColor3f(R(102), G(51), B(0));
	glColor3f(R(49), G(78), B(82));
	glVertex3f(0.27f, 0.23f, 1.5f);
	glVertex3f(-0.27f, 0.23f, 1.5f);
	glVertex3f(-0.5f, 0.0f, 1.5f);
	glVertex3f(0.5f, 0.0f, 1.5f);

	//back
	glVertex3f(0.27f, 0.23f, 1.3f);
	glVertex3f(-0.27f, 0.23f, 1.3f);
	glVertex3f(-0.5f, -0.02f, 1.3f);
	glVertex3f(0.5f, -0.02f, 1.3f);


	//top
	glVertex3f(0.27f, 0.23f, 1.3f);
	glVertex3f(-0.27f, 0.23f, 1.3f);
	glVertex3f(-0.27f, 0.23f, 1.5f);
	glVertex3f(0.27f, 0.23f, 1.5f);

	//left
	glVertex3f(0.27f, 0.23f, 1.3f);
	glVertex3f(0.27f, 0.23f, 1.5f);
	glVertex3f(0.5f, 0.0f, 1.5f);
	glVertex3f(0.5f, 0.0f, 1.5f);

	//right
	glVertex3f(-0.27f, 0.23f, 1.3f);
	glVertex3f(-0.27f, 0.23f, 1.5f);
	glVertex3f(-0.5f, 0.0f, 1.5f);
	glVertex3f(-0.5f, 0.0f, 1.5f);
	glEnd();
}

void drawBaseOfPyramid()
{
	//function declaration
	void loadWalls(void);
	void centeralInvertedPyramid(void);
	void draw_smallCryptex(void);

	//Base - 
	glColor3f(R(199), G(206), B(223));
	glBindTexture(GL_TEXTURE_2D, texture_floor);
	glBegin(GL_QUADS);
	//front
	glVertex3f(1.5f, 0.0f, 1.5f);
	glVertex3f(-1.5f, 0.0f, 1.5f);
	glVertex3f(-1.5f, -0.2f, 1.5f);
	glVertex3f(1.5f, -0.2f, 1.5f);

	//back
	glVertex3f(1.5f, 0.0f, -1.5f);
	glVertex3f(-1.5f, 0.0f, -1.5f);
	glVertex3f(-1.5f, -0.2f, -1.5f);
	glVertex3f(1.5f, -0.2f, -1.5f);

	//right
	glVertex3f(1.5f, 0.0f, -1.5f);
	glVertex3f(1.5f, 0.0f, 1.5f);
	glVertex3f(1.5f, -0.2f, 1.5f);
	glVertex3f(1.5f, -0.2f, -1.5f);

	//left
	glVertex3f(-1.5f, 0.0f, -1.5f);
	glVertex3f(-1.5f, 0.0f, 1.5f);
	glVertex3f(-1.5f, -0.2f, 1.5f);
	glVertex3f(-1.5f, -0.2f, -1.5f);

	//bottom
	glNormal3f(0.0f, 1.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1, 1);
	glVertex3f(1.5f, -0.2f, -1.5f);
	glTexCoord2f(0, 1);
	glVertex3f(-1.5f, -0.2f, -1.5f);
	glTexCoord2f(0, 0);
	glVertex3f(-1.5f, -0.2f, 1.5f);
	glTexCoord2f(1, 0);
	glVertex3f(1.5f, -0.2f, 1.5f);
	glColor3f(0.5f, 0.5f, 0.5f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	//side walls
	loadWalls();

	//Jamin
	glBindTexture(GL_TEXTURE_2D, texture_jamin);
	glBegin(GL_QUADS);
		glTexCoord2f(10, 10);
		glVertex3f(50.0f, -0.21f, 50.0f);
		glTexCoord2f(10, 0);
		glVertex3f(-50.0f, -0.21f, 50.0f);
		glTexCoord2f(0, 0);
		glVertex3f(-50.0f, -0.21f, -50.0f);
		glTexCoord2f(0, 10);
		glVertex3f(50.0f, -0.21f, -50.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	//central small floating cryptex
	draw_smallCryptex();

	//central cube
	centeralInvertedPyramid();

	//line on jamin
	/*glLineWidth(30.0f);
	glBegin(GL_LINES);
		glColor3f(236.0f/255.0f, 179.0f/255.0f, 30.0f/255.0f);
		glVertex3f(0.0f, -0.20f, 0.0f);
		glVertex3f(0.0f, -0.20f, 50.0f);
	glEnd();*/
	//glBindTexture(GL_TEXTURE_2D, texture_gold);
	glColor3f(R(190), G(150), B(38));//golden line color
	glBegin(GL_QUADS);
		//glTexCoord2f(5, 0.05);
		glVertex3f(0.02f, -0.193f, 0.0f);
		//glTexCoord2f(5, 0);
		glVertex3f(-0.02f, -0.193f, 0.0f);
		//glTexCoord2f(0, 0);
		glVertex3f(-0.02f, -0.193f, 50.0f);
		//glTexCoord2f(0, 0.05);
		glVertex3f(0.02f, -0.193f, 50.0f);
	glEnd();
	//glBindTexture(GL_TEXTURE_2D, 0);
}

/* --------------------------------------------------------------------------*/
void mapLoad(void)
{
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -18.0f);
	//glRotatef(anglePyramid, 0.0f, 1.0f, 0.0f); // spinning
	glRotatef(angleEarth, 0.0f, 0.0f, 1.0f); //y->z z->y
	glBindTexture(GL_TEXTURE_2D, texture_map);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(19.0f, 19.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-19.0f, 19.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-19.0f, -19.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(19.0f, -19.0f, 1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void loadWalls(void)
{
	//variables
	static float x = 25.0f;
	static float y = 25.0f;
	static float z = 20.0f;

	//code
	//glTranslatef(0.0f, 0.0f, -18.0f);
	glBindTexture(GL_TEXTURE_2D, texture_palace);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 1);
	glVertex3f(-x, y, z);
	glTexCoord2d(0, 0);
	glVertex3f(-x, -y, z);
	glTexCoord2d(1,0);
	glVertex3f(-x, -y, -z);
	glTexCoord2d(1, 1);
	glVertex3f(-x, y, -z);
	glEnd();

	glBegin
	(GL_QUADS);
	glTexCoord2d(1, 1);
	glVertex3f(x, y, -z);
	glTexCoord2d(0, 1);
	glVertex3f(-x, y, -z);
	glTexCoord2d(0, 0);
	glVertex3f(-x, -y, -z);
	glTexCoord2d(1, 0);
	glVertex3f(x, -y, -z);
	glEnd();
	
	glBegin
	(GL_QUADS);
	glTexCoord2d(1, 1);
	glVertex3f(x, y, z);
	glTexCoord2d(0, 1);
	glVertex3f(x, y, -z);
	glTexCoord2d(0, 0);
	glVertex3f(x, -y, -z);
	glTexCoord2d(1, 0);
	glVertex3f(x, -y, z);
	glEnd();

	if(isCameraInsidePyramid)
	{
		glBegin
		(GL_QUADS);
		glTexCoord2d(1, 1);
		glVertex3f(x, y, z);
		glTexCoord2d(0, 1);
		glVertex3f(-x, y, z);
		glTexCoord2d(0, 0);
		glVertex3f(-x, -y, z);
		glTexCoord2d(1, 0);
		glVertex3f(x, -y, z);
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void centeralInvertedPyramid(void)
{
	glBindTexture(GL_TEXTURE_2D, texture_stone);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 1.0f, 1.0f);
		//front face
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(0.0f, -0.19f + yTranslationInvertedPyramid, 0.0f);//Apex
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-0.1f, 0.1f + yTranslationInvertedPyramid, 0.1f);//left bottom
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.1f, 0.1f + yTranslationInvertedPyramid, 0.1f);// right bottom
		
		//right face
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(0.0f, -0.19f + yTranslationInvertedPyramid, 0.0f);//Apex
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.1f, 0.1f + yTranslationInvertedPyramid, 0.1f);// right surfaces cha left bottom
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.1f, 0.1f + yTranslationInvertedPyramid, -0.1f);
		
		//back face
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(0.0f, -0.19f + yTranslationInvertedPyramid, 0.0f);//Apex
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.1f, 0.1f + yTranslationInvertedPyramid, -0.1f);// left bottom
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-0.1f, 0.1f + yTranslationInvertedPyramid, -0.1f);
		
		//left face
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(0.0f, -0.19f + yTranslationInvertedPyramid, 0.0f);//Apex
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-0.1f, 0.1f + yTranslationInvertedPyramid, -0.1f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-0.1f, 0.1f + yTranslationInvertedPyramid, 0.1f);
		glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3f(1.0f, 1.0f, 1.0f);
}

/* --------------------------------Earth------------------------------------------*/
void scene_Earth(void)
{
	//code
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, onetime + 3.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(angleEarth, 0.0f, 0.0f, 1.0f); //y->z z->y
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindTexture(GL_TEXTURE_2D, texture_earth);
	gluQuadricTexture(quadric, GL_TRUE);
	gluSphere(quadric, 1.0f, 50, 50);
	glBindTexture(GL_TEXTURE_2D, 0);

	//inner golden sphere
	glColor3f(R(190), G(150), B(38));//golden color
	gluSphere(quadric, 0.89f, 50, 50);
}


/* -----------------------------Small Cryptex---------------------------------------------*/
void draw_smallCryptex(void)
{
	// Function Declarations
	void Cylinder(float, float, float, float);
	void Disk(float, float);

	//code
	//glLoadIdentity();

	glPushMatrix();

	// View Trandformations
	gluLookAt(x1_camPos_cryptex, 0.0f, z1_camPos_cryptex, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	//Transformations on whole cryptex
	glScalef(0.05f, 0.05f, 0.05f);
	glTranslatef(0.0f, initSmallCryptexYTranslation, 0.0f);
	if(initSmallCryptexYTranslation >= 3.3f)
		glTranslatef(0.0f, y1_camPos_cryptex, 0.0f);

	// Bind With The Texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_digits);
	
	
		glPushMatrix();
		glRotatef(angle_cryptex, 1.0f, 0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		gluQuadricTexture(quadric_cryptex, GL_TRUE);
		Cylinder(-0.7f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	
	glBindTexture(GL_TEXTURE_2D, texture_letter_j);
	
		glPushMatrix();
		glRotatef(angle_cryptex, 1.0f, 0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		gluQuadricTexture(quadric_cryptex, TRUE);
		Cylinder(-0.35f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	
		
	glBindTexture(GL_TEXTURE_2D, texture_letter_u);
	
		glPushMatrix();
		glRotatef(angle_cryptex, 1.0f, 0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		gluQuadricTexture(quadric_cryptex, TRUE);
		Cylinder(0.0f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	
		
	glBindTexture(GL_TEXTURE_2D, texture_letter_l);
	
		glPushMatrix();
		glRotatef(angle_cryptex, 1.0f, 0.0f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		gluQuadricTexture(quadric_cryptex, TRUE);
		Cylinder(0.35f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	
	
	glBindTexture(GL_TEXTURE_2D, texture_letter_y);
	
		glPushMatrix();
		glRotatef(angle_cryptex, 1.0f, 0.0f, 0.0f);
		gluQuadricTexture(quadric_cryptex, TRUE);
		glColor3f(1.0f, 1.0f, 1.0f);
		Cylinder(0.7f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_gold);
	gluQuadricTexture(quadric_cryptex, TRUE);
	glColor3f(1.0f, 1.0f, 1.0f);
	Cylinder(0.0f, 0.35f, 0.35f, 2.1f);

	// Edges Of Criptex
	
	Cylinder(-1.0f, 0.43f, 0.43f, 0.1f);
	Cylinder(-0.87f, 0.43f, 0.43f, 0.1f);
	Cylinder(0.87f, 0.43f, 0.43f, 0.1f);
	Cylinder(1.0f, 0.43f, 0.43f, 0.1f);

	
	Cylinder(-1.2f, 0.0f, 0.3f, 0.2f);
	Disk(-1.05f, 0.43f);
	Disk(-1.07f, 0.40f);
	// Cone
	
	//glPopMatrix();

	//glPushMatrix();
	
	Cylinder(1.2f, 0.3f, 0.0f, 0.2f);
	Disk(1.05f, 0.43f);
	Disk(1.07f, 0.40f);
	
	glPopMatrix();
}

void Cylinder(float position, float topR, float baseR, float height)
{
	// Code
	glPushMatrix();
	//glLoadIdentity();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glTranslatef((position - (height/2)), 0.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluQuadricNormals(quadric_cryptex, GL_TRUE);
	gluCylinder(quadric_cryptex, topR, baseR, height, 50, 50);
	//gluDisk(quadric_cryptex, 0, baseR, 10000, 10);
	glPopMatrix();
}

void Disk(float position,float baseR)
{
	// Code
	glPushMatrix();
	glTranslatef((position), 0.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluQuadricNormals(quadric_cryptex, GL_TRUE);
	gluDisk(quadric_cryptex, 0, baseR, 10000, 10);
	glPopMatrix();
}

/* -----------------------------Big Cryptex---------------------------------------------*/
void display_bigCryptex(void)
{
	// Function Declarations
	void Cylinder_big(float, float, float, float);
	void Disk_big(float, float);
	void TextOnQuad(void);

	//code
	//LIGHT
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHTING);

	glColor3f(1.0f, 1.0f, 1.0f);
	glLoadIdentity();

	// View Trandformations
	gluLookAt(x1, y, z1, 0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f);

	// Save Camera Matrix
	glPushMatrix();

	// Bind With The Texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_digits);
	
	if (gKeyPressed1 == 1)
		Cylinder_big(-0.7f, 0.4f, 0.4f, 0.3f);
	else
	{
		glPushMatrix();
		glRotatef(angle, 1.0f, 0.0f, 0.0f);
		gluQuadricTexture(quadric, GL_TRUE);
		Cylinder_big(-0.7f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	}
	
	glBindTexture(GL_TEXTURE_2D, texture_letter_j);
	if (gKeyPressed2 == 2)
		Cylinder_big(-0.35f, 0.4f, 0.4f, 0.3f);
	else
	{
		glPushMatrix();
		glRotatef(angle, 1.0f, 0.0f, 0.0f);
		gluQuadricTexture(quadric, TRUE);
		Cylinder_big(-0.35f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	}
		
	glBindTexture(GL_TEXTURE_2D, texture_letter_u);
	if (gKeyPressed3 == 3)
		Cylinder_big(0.0f, 0.4f, 0.4f, 0.3f);
	else
	{
		glPushMatrix();
		glRotatef(angle, 1.0f, 0.0f, 0.0f);
		gluQuadricTexture(quadric, TRUE);
		Cylinder_big(0.0f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	}
		
	glBindTexture(GL_TEXTURE_2D, texture_letter_l);
	if (gKeyPressed4 == 4)
		Cylinder_big(0.35f, 0.4f, 0.4f, 0.3f);
	else
	{
		glPushMatrix();
		glRotatef(angle, 1.0f, 0.0f, 0.0f);
		gluQuadricTexture(quadric, TRUE);
		Cylinder_big(0.35f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	}
	
	glBindTexture(GL_TEXTURE_2D, texture_letter_y);
	if (gKeyPressed5 == 5)
		Cylinder_big(0.7f, 0.4f, 0.4f, 0.3f);
	else
	{
		glPushMatrix();
		glRotatef(angle, 1.0f, 0.0f, 0.0f);
		gluQuadricTexture(quadric, TRUE);
		Cylinder_big(0.7f, 0.4f, 0.4f, 0.3f);
		glPopMatrix();
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_gold);
	//gluQuadricTexture(quadric, TRUE);
	Cylinder_big(0.0f, 0.35f, 0.35f, 2.1f);

	// Edges Of Criptex
	Cylinder_big(-1.0f, 0.43f, 0.43f, 0.1f);
	Cylinder_big(-0.87f, 0.43f, 0.43f, 0.1f);
	Cylinder_big(0.87f, 0.43f, 0.43f, 0.1f);
	Cylinder_big(1.0f, 0.43f, 0.43f, 0.1f);

	glPushMatrix();
	if (flagDisassemble == 1)
		glTranslatef(T1, 0.0f, 0.0f);
	Cylinder_big(-1.2f, 0.0f, 0.3f, 0.2f);
	glPopMatrix();

	// Disk_big
	glPushMatrix();
	if (flagDisassemble == 1)
		glTranslatef(T2, 0.0f, 0.0f);
	gluQuadricTexture(quadric, GL_TRUE);
	Disk_big(-1.05f, 0.43f);
	glPopMatrix();

	glPushMatrix();
	if (flagDisassemble == 1)
		glTranslatef(T3, 0.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, texture_disk);
	gluQuadricTexture(quadric, GL_TRUE);
	Disk_big(-1.07f, 0.38f);
	glPopMatrix();

	glPushMatrix();
	if (flagDisassemble == 1)
		glTranslatef(-T1, 0.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, texture_gold);
	Cylinder_big(1.2f, 0.3f, 0.0f, 0.2f);
	glPopMatrix();

	glPushMatrix();
	if (flagDisassemble == 1)
		glTranslatef(-T2, 0.0f, 0.0f);
	gluQuadricTexture(quadric, GL_TRUE);
	Disk_big(1.05f, 0.43f);
	glPopMatrix();

	glPushMatrix();
	if (flagDisassemble == 1)
		glTranslatef(-T3, 0.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, texture_disk);
	gluQuadricTexture(quadric, GL_TRUE);
	Disk_big(1.07f, 0.38f);
	glBindTexture(GL_TEXTURE_2D, texture_gold);
	glPopMatrix();

	glPopMatrix();

	if (flagText == 1)
	{
		TextOnQuad();
	}
}

void Cylinder_big(float position, float topR, float baseR, float height)
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

void Disk_big(float position,float baseR)
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
			glTranslatef(-1.35f, up - 0.7f, -1.0f);
			glRasterPos2i(1, 0);
			glListBase(nFontList_bday);
			glCallLists(5, GL_UNSIGNED_BYTE, "HAPPY");
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-1.52f, up - 1.0f, -1.0f);
			glRasterPos2i(1, 0);
			glListBase(nFontList_bday);
			glCallLists(8, GL_UNSIGNED_BYTE, "BIRTHDAY");
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-1.28f, up - 1.3f, -1.0f);
			glRasterPos2i(1, 0);
			glListBase(nFontList_bday);
			glCallLists(5, GL_UNSIGNED_BYTE, "VIPIN");
		glPopMatrix();

		//wglUseFontBitmaps(ghdc, 0, 128, nFontList_bday);
		glPopMatrix();
}

// setup the Font 
void SetupRC(void)
{
	// Code
	//// Set up the font characteristic
	//GLYPHMETRICSFLOAT agmf[128];

	// setup the Font Characteristics
	logfont_bday.lfHeight = 35;
	logfont_bday.lfWidth = 0;
	logfont_bday.lfEscapement = 0;
	logfont_bday.lfOrientation = 0;
	logfont_bday.lfWeight = FW_BOLD;
	logfont_bday.lfItalic = FALSE;
	logfont_bday.lfUnderline = FALSE;
	logfont_bday.lfCharSet = ANSI_CHARSET;
	logfont_bday.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont_bday.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont_bday.lfQuality = DEFAULT_QUALITY;
	logfont_bday.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logfont_bday.lfFaceName, "Arial");

	// Create the font and Display list
	hFont_bday = CreateFontIndirect(&logfont_bday);
	SelectObject(ghdc, hFont_bday);

	// Create display lists for glyphs 0 through 128
	nFontList_bday = glGenLists(128);
	//wglUseFontOutlines(ghdc, 0, 128, nFontList_bday, 0.0f, 0.5f, WGL_FONT_POLYGONS, agmf);
	wglUseFontBitmaps(ghdc, 0, 128, nFontList_bday);
	DeleteObject(hFont_bday);
}

//only for letters
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

/*************************** Title ***********************************************/
void displayTitle(void)
{
	//function declarations
	void renderingTitleFont(void);

	//lines
	glLoadIdentity();
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(R(145), G(0), B(0));
	glVertex3f(-3.0f, 0.5f, -10.0f);
	glVertex3f(3.0F, 0.5F, -10.0f);

	glVertex3f(-3.0f, -0.7f, -10.0f);
	glVertex3f(3.0f, -0.7f, -10.0f);
	glEnd();

	renderingTitleFont();

	/*for(float f = 0.0f; f <= 5.0f; f += 0.001f)
		{}*/
	//toTranslateScene[EARTH] = TRUE;
}

void SetupRCTitle(void)
{
	// Code
	//// Set up the font characteristic
	GLYPHMETRICSFLOAT agmf_title[128];

	// setup the Font Characteristics
	logfont_title.lfHeight = 40;
	logfont_title.lfWidth = 0;
	logfont_title.lfEscapement = 0;
	logfont_title.lfOrientation = 0;
	logfont_title.lfWeight = FW_EXTRALIGHT;
	logfont_title.lfItalic = FALSE;
	logfont_title.lfUnderline = FALSE;
	logfont_title.lfCharSet = ANSI_CHARSET;
	logfont_title.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont_title.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont_title.lfQuality = DEFAULT_QUALITY;
	logfont_title.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logfont_title.lfFaceName, "HARRINGTON");

	// Create the font and Display list
	hFont_title = CreateFontIndirect(&logfont_title);
	SelectObject(ghdc, hFont_title);

	// Create display lists for glyphs 0 through 128
	nFontList_title = glGenLists(128);
	wglUseFontOutlines(ghdc, 0, 128, nFontList_title, 0.0f, 0.3f, WGL_FONT_POLYGONS, agmf_title);
	//wglUseFontBitmaps(ghdc, 0, 128, nFontList_title);
	DeleteObject(hFont_title);
}

void renderingTitleFont(void)
{
	// Code
	glLoadIdentity();
	//glColor3f(R(190), G(150), B(38)); //gold
	glColor3f(R(180), G(180), B(180));
	glTranslatef(-1.3f, 1.4f, -20.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList_title);
	glCallLists(3, GL_UNSIGNED_BYTE, "THE");

	glLoadIdentity();
	glTranslatef(-2.7f, -0.4f, -10.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList_title);
	glCallLists(10, GL_UNSIGNED_BYTE, "HULL GROUP");

	glLoadIdentity();
	glTranslatef(-2.55f, -2.3f, -20.0f);
	//glColor3f(1.0f, 0.0f, 0.0f);
	glRasterPos2i(1, 0);
	glListBase(nFontList_title);
	glCallLists(8, GL_UNSIGNED_BYTE, "PRESENTS");

		//wglUseFontBitmaps(ghdc, 0, 128, nFontList_title);
}