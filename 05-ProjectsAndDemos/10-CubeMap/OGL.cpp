// Header Files
#include <windows.h>
#include <iostream>
#include "OGL.h"
#include <stdio.h>
#include <string.h>

#include <vector>		//GL

#define _USE_MATH_DEFINES 1
#include <math.h>

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//#include "glext.h"

// OpenGL Header Files
#include <GL/gl.h>
#include <GL/glu.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
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

GLfloat angle = 0.0f;
GLuint textureCube;
GLuint cubemapTexture;

float x_axis = 0.0f;
float z_axis = 50.0f;
float y_axis = 0.0f;
float i = -90.0f;

//std::vector<TCHAR*> testing;

// Cubemap
TCHAR* facesCubemap[] =
{
	MAKEINTRESOURCE(LEFT1), MAKEINTRESOURCE(RIGHT1), MAKEINTRESOURCE(TOP1), MAKEINTRESOURCE(BOTTOM1), MAKEINTRESOURCE(FRONT1),MAKEINTRESOURCE(BACK1)
};

//std::String szCubeFaces[6] = { "pos_x.jpg", "neg_x.jpg", "pos_y.jpg", "neg_y.jpg", "pos_z.jpg","neg_z.jpg" };

/*GLenum cube[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };
*/

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
	else if (iRetVal == -4)
	{
		fprintf(gpFile, "LoadGLtexture() Failed For cubemap.\n");
		uninitialize();
	}
	else if (iRetVal == -6)
	{
		fprintf(gpFile, "Cubemap Failed.\n");
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
	void CubeMap(void);
	bool LoadGLCubemap(void);

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
	//glShadeModel(GL_SMOOTH);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Create Quadric
	//quadric = gluNewQuadric();			// It Internally Calls Malloc, So Dont Use Inside Display()
	
	//if (LoadGLTexture(&textureCube, MAKEINTRESOURCE(IDBITMAP_CUBE)) == FALSE)
		//return -5;

		// Generate Texture Object
	/*glGenTextures(1, &textureCube);

	// Bind That Texture Object
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureCube);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	int width, height, nrChannels;
	unsigned char* data;
	for (unsigned int i = 0; i < sizeof(szCubeFaces); i++)
	{
		data = stbi_load(szCubeFaces[i].c_str(), &width, &height, &nrChannels, 0);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
		);
	}
	*/
	//CubeMap();
	
	// All the faces of the cubemap (make sure they are in this exact order)
	/*std::string facesCubemap[6] =
	{
		"pos_x.bmp", "neg_x.bmp", "pos_y.bmp", "neg_y.bmp", "pos_z.bmp","neg_z.bmp"
	};
	*/
	
	/*
	// Creates the cubemap texture object
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	// This might help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	BOOL bResult = FALSE;
	// Cycles through all the textures and attaches them to the cubemap object
	for (GLuint i = 0; i < 6; i++)
	{
		// Local Variable Declarations
		HBITMAP hbitmap = NULL;
		BITMAP bmp;
		

		// Code
		hbitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
			facesCubemap[i],														//ImageResourceID,
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
			//glGenTextures(1, Texture);

			// Bind That Texture Object
			//glBindTexture(GL_TEXTURE_CUBE_MAP, *Texture);

			// Set Texture Parameters
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			

			fprintf(gpFile, "Bitmap %d Is Loaded\n", i);
			// Create The Texture
			gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				GL_RGB,
				bmp.bmWidth,
				bmp.bmHeight,
				GL_BGR_EXT,
				GL_UNSIGNED_BYTE,
				bmp.bmBits);

			// Unbind Texture Object
			//glBindTexture(GL_TEXTURE_2D, 0);

			// Delete Object
			//DeleteObject(hbitmap);
		}
		else
		{
			fprintf(gpFile, "Bitmap %d Is Not Loaded\n", i);
			return -6;
			
			
		}
		*/
		/*
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGBA,
				width,
				height,
				0,
				GL_BGR_EXT,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
		*/
	//}
	
	//cubemap();
/*testing.push_back(MAKEINTRESOURCE(LEFT));
testing.push_back(MAKEINTRESOURCE(RIGHT));
testing.push_back(MAKEINTRESOURCE(TOP));
testing.push_back(MAKEINTRESOURCE(BOTTOM));
testing.push_back(MAKEINTRESOURCE(FRONT));
testing.push_back(MAKEINTRESOURCE(BACK));
*/
	bool result = LoadGLCubemap();
	if (result == false)
	{
		fprintf(gpFile, "failed in LoadGLCubemap() function\n");
		DestroyWindow(ghwnd);
	}
	glEnable(GL_TEXTURE_CUBE_MAP);

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

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 200.0f);
}

void display(void)
{
	// Function Declarations
	void cube(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glTranslatef(0.0f, 0.0f, -4.0f);
	//gluLookAt(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	gluLookAt(0.0f, 0.0f, 0.0f, x_axis, y_axis, z_axis, 0.0f, 1.0f, 0.0f);

	//glPushMatrix();
	//glRotatef(angle, 0.0f, 1.0f, 0.0f);
	
	//glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	
	cube();
	
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	
	//glPopMatrix();
	SwapBuffers(ghdc);
}

void update(void)
{
	// Code
	angle += 1.0f;
	i += 0.005f;
	x_axis = cos(i);
	//y_axis = cos(i);
	z_axis = sin(i);
}

void cube(void)
{
	// Local Variable Declarations
	GLfloat xCoordinate = 100.0f;
	GLfloat yCoordinate = 100.0f;
	GLfloat zCoordinate = 100.0f;

	// Code

	// Front
	glBegin(GL_QUADS);
	//glColor3f(1.0f, 0.0f, 0.0f);
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
	//glColor3f(0.0f, 1.0f, 0.0f);
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
	//glColor3f(0.0f, 0.0f, 1.0f);
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
	//glColor3f(1.0f, 1.0f, 0.0f);
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
	//glColor3f(1.0f, 0.0f,1.0f);
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
	//glColor3f(0.0f, 1.0f, 1.0f);
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

void cubeOld(void)
{
	// Local Variable Declarations
	GLfloat xCoordinate = 70.0f;
	GLfloat yCoordinate = 70.0f;
	GLfloat zCoordinate = 70.0f;

	// Code

	glBegin(GL_QUADS);


	// Front Face
	//glTexCoord2f(1.0f, 1.0f);
	glTexCoord3f(1.0f, 1.0f, 1.0f);
	//glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	//glTexCoord2f(0.0f, 1.0f);
	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	//glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	//glTexCoord2f(0.0f, 0.0f);
	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	//glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	//glTexCoord2f(1.0f, 0.0f);
	glTexCoord3f(1.0f,-1.0f, 1.0f);
	//glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Right Face
	//glTexCoord2f(1.0f, 1.0f);
	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	//glTexCoord2f(0.0f, 1.0f);
	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	//glTexCoord2f(0.0f, 0.0f);
	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	//glTexCoord2f(1.0f, 0.0f);
	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Back Face
	//glTexCoord2f(1.0f, 1.0f);
	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	//glTexCoord2f(0.0f, 1.0f);
	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	//glTexCoord2f(0.0f, 0.0f);
	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	//glTexCoord2f(1.0f, 0.0f);
	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	// Left Face
	//glTexCoord2f(1.0f, 1.0f);
	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	//glTexCoord2f(0.0f, 1.0f);
	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	//glTexCoord2f(0.0f, 0.0f);
	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	//glTexCoord2f(1.0f, 0.0f);
	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Top Face
	//glTexCoord2f(1.0f, 1.0f);
	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	//glTexCoord2f(0.0f, 1.0f);
	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	//glTexCoord2f(0.0f, 0.0f);
	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	//glTexCoord2f(1.0f, 0.0f);
	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	// Bottom Face

	//glTexCoord2f(1.0f, 1.0f);
	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	//glTexCoord2f(0.0f, 1.0f);
	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	//glTexCoord2f(0.0f, 0.0f);
	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	//glTexCoord2f(1.0f, 0.0f);
	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glEnd();
	/*
	// Front
	glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 0.0f);
		//glTexCoord2f(1.0f, 1.0f);
	//	glTexCoord3f(xCoordinate, yCoordinate, -zCoordinate);
		glVertex3f(xCoordinate, yCoordinate, zCoordinate);
		//glTexCoord2f(0.0f, 1.0f);
		//glTexCoord3f(-xCoordinate, yCoordinate, -zCoordinate);
		glVertex3f(-xCoordinate, yCoordinate, zCoordinate);
		//glTexCoord2f(0.0f, 0.0f);
	//	glTexCoord3f(-xCoordinate, -yCoordinate, -zCoordinate);
		glVertex3f(-xCoordinate, -yCoordinate, zCoordinate);
		//glTexCoord2f(1.0f, 0.0f);
		//glTexCoord3f(xCoordinate, -yCoordinate, -zCoordinate);
		glVertex3f(xCoordinate, -yCoordinate, zCoordinate);
	glEnd();

	// Right
	glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f);
		glTexCoord3f(1.0f, 1.0f, 1.0f);
		glVertex3f(xCoordinate, yCoordinate, -zCoordinate);
		glTexCoord3f(0.0f, 1.0f, 1.0f);
		glVertex3f(xCoordinate, yCoordinate, zCoordinate);
		glTexCoord3f(0.0f, 0.0f, 1.0f);
		glVertex3f(xCoordinate, -yCoordinate, zCoordinate);
		glTexCoord3f(1.0f, 0.0f, 1.0f);
		glVertex3f(xCoordinate, -yCoordinate, -zCoordinate);
	glEnd();

	// Back
	glBegin(GL_QUADS);
		glColor3f(0.0f, 0.0f, 1.0f);
		glTexCoord3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-xCoordinate, yCoordinate, -zCoordinate);
		glTexCoord3f(0.0f, 1.0f, 1.0f);
		glVertex3f(xCoordinate, yCoordinate, -zCoordinate);
		glTexCoord3f(0.0f, 0.0f, 1.0f);
		glVertex3f(xCoordinate, -yCoordinate, -zCoordinate);
		glTexCoord3f(1.0f, 0.0f, 1.0f);
		glVertex3f(-xCoordinate, -yCoordinate, -zCoordinate);
	glEnd();

	// Left
	glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 0.0f);
		glTexCoord3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-xCoordinate, yCoordinate, zCoordinate);
		glTexCoord3f(0.0f, 1.0f, 1.0f);
		glVertex3f(-xCoordinate, yCoordinate, -zCoordinate);
		glTexCoord3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-xCoordinate, -yCoordinate, -zCoordinate);
		glTexCoord3f(1.0f, 0.0f, 1.0f);
		glVertex3f(-xCoordinate, -yCoordinate, zCoordinate);
	glEnd();

	// Top
	glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f,1.0f);
		glTexCoord3f(1.0f, 1.0f, 1.0f);
		glVertex3f(xCoordinate, yCoordinate, -zCoordinate);
		glTexCoord3f(0.0f, 1.0f, 1.0f);
		glVertex3f(-xCoordinate, yCoordinate, -zCoordinate);
		glTexCoord3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-xCoordinate, yCoordinate, zCoordinate);
		glTexCoord3f(1.0f, 0.0f, 1.0f);
		glVertex3f(xCoordinate, yCoordinate, zCoordinate);
	glEnd();

	// Bottom
	glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 1.0f);
		glTexCoord3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-xCoordinate, -yCoordinate, -zCoordinate);
		glTexCoord3f(0.0f, 1.0f, 1.0f);
		glVertex3f(xCoordinate, -yCoordinate, -zCoordinate);
		glTexCoord3f(0.0f, 0.0f, 1.0f);
		glVertex3f(xCoordinate, -yCoordinate, zCoordinate);
		glTexCoord3f(1.0f, 0.0f, 1.0f);
		glVertex3f(-xCoordinate, -yCoordinate, zCoordinate);
	glEnd();
	*/
}

/*void CubeMap(void)
{
	GLbyte* pBytes;
	GLint iWidth, iHeight, iComponents;
	GLenum eFormat;
	// Load Cube Map images
	for (int i = 0; i < 6; i++)
	{
		// Load this texture map
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
		//pBytes = gltLoadTGA(szCubeFaces[i], &iWidth, &iHeight, &iComponents, &eFormat);
		glTexImage2D(cube[i], 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
		free(pBytes);
	}
}*/

// load cubemap
bool LoadGLCubemap()
{
	// variable declarations
	bool bResult = false;

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
		//hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), testing[i], IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
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

			//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		}
		else
		{
			fprintf(gpFile, "bitmap  %d  is not loaded\n", i);
			DestroyWindow(ghwnd);
		}
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	fprintf(gpFile, "End of %s \n", __FUNCTION__);
	return bResult;
}


/*BOOL LoadGLTexture(GLuint* Texture, TCHAR ImageResourceID[])
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
		glBindTexture(GL_TEXTURE_CUBE_MAP, *Texture);

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
*/
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

	if (gpFile)
	{
		fprintf(gpFile, "Log File Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
