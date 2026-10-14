// common header file
#include "Common.h"
#include "utils/Configuration.h"
#include "loadtexture_stbi.h"

// shaders/effects header file
#include "TextureShader.h"
#include "ColorShader.h"
#include "PhongLightShader.h"
#include "FontShader.h"
#include "StaticModelShader.h"
#include "Terrian/TerrianLearnOGL.h"

// to test your own effect
#include "TestEffect.h"
#include "TestStaticModel.h"

// common renderer header file
#include "CubeRender.h"
#include "QuadRender.h"

// camera header file
#include "Camera.h"

// scenes header files
#include "DefaultScene.h"
#include "scene/Scene0_Banner.h"
#include "scene/Scene1_OpeningScene.h"

// scene test transform model
#include "TestDownloadedStaticModel.h"
#include "TestTransformModel.h"
#include "engine/UniformBuffer.h"

#include "engine/Presenter.h"
#include "engine/SceneQueue.h"
#include "engine/Synchronizer.h"
#include "grass/Grass.h"

//OpenGL libraries
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

// Suppress unsecure warnings
#pragma warning(disable : 4996)

enum DisplayMode
{
	DEFAULT_SCENE,
	TEST_EFFECT,
	PRESENT_DEMO
};

#define TEST_TRANSFORM 1
#define DISPLAY_MODE DEFAULT_SCENE

#define ENABLE_FPS_LOCK 1
#define DESIRED_FPS 60

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HDC ghdc = NULL;
HGLRC ghrc = NULL;
BOOL gbActiveWindow = FALSE;
BOOL gbFullscreen = FALSE;
bool result;

mat4 perspectiveProjectionMatrix = mat4::identity();
mat4 viewMatrix = mat4::identity();

float mouseXpos;
float mouseYpos;
float lastX = 0.0f;
float lastY = 0.0f;

// delta time related variables
LARGE_INTEGER startingTime, endingTime;
LARGE_INTEGER frequency;
double delta = 0.0;
double timeElapsed = 0.0;
short FPS_rate = 0;
short g_FPS_rate = 0;
char frameRateString[128];

GLuint screenWidth = WIN_WIDTH, screenHeight = WIN_HEIGHT;

// win-main entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function prototype
	bool Initialize(void);
	void Display(void);
	void Update(double);
	void Uninitialize(void);
	void ToggleFullscreen(void);

	// variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("RTR4-HullGroup");
	BOOL bDone = FALSE;
	bool iRetVal = false;

	// code
	if (InitializeLogger() == false)
	{
		MessageBox(NULL, TEXT("Creation of log file is failed. Exiting..."), TEXT("FileI/O error"), MB_OK);
		exit(1);
	}
	else
	{
		InfoLog(NULL, "WinMain()::InitializeLogger() succeeded.");
	}

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MY_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MY_ICON));

	// register above window class
	RegisterClassEx(&wndclass);

	//Get screen width and height
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	// create the window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("RTR4-HullGroup"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, ((rect.right / 2) - (WIN_WIDTH / 2)), ((rect.bottom / 2) - (WIN_HEIGHT / 2)), WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

	ghwnd = hwnd;

	// Initialize
	iRetVal = Initialize();
	if (iRetVal == false)
	{
		ErrorLog(NULL, "Initialization failed.\n");
		Uninitialize();
		DestroyWindow(hwnd);
	}
	else
	{
		InfoLog(NULL, "Initialization successful.\n");
	}

	ShowWindow(hwnd, iCmdShow);

	//forgrounding and focusing window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	if (gbFullscreen == FALSE)
		ToggleFullscreen();

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startingTime);

#if ENABLE_FPS_LOCK
	double timePerFrame = 1.0 / DESIRED_FPS;
#endif
	double freq = 1.0 / (double)frequency.QuadPart;

	//game loop
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
				// render scene
				Display();

				// Update scene
				Update(delta);

				QueryPerformanceCounter(&endingTime);
				delta = (double)(endingTime.QuadPart - startingTime.QuadPart) * freq;

#if ENABLE_FPS_LOCK
				while (delta < timePerFrame)
				{
					QueryPerformanceCounter(&endingTime);
					delta = (double)(endingTime.QuadPart - startingTime.QuadPart) * freq;
				}
#endif
			timeElapsed += delta;
			FPS_rate++;

			if (timeElapsed >= 1.0)
			{
				g_FPS_rate = FPS_rate;
				FPS_rate = 0;
				timeElapsed = 0.0;
			}

			startingTime = endingTime;
			}
		}

		if (gbEndDemo)
		{
			bDone = TRUE;
		}
	}

	Uninitialize();

	return((int)msg.wParam);
}

// callback function
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// function prototype
	void ToggleFullscreen(void);
	void Resize(int, int);

	switch (message)
	{
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;

	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case 'W':
		case 'w':
		case VK_UP:
			CameraMoveBackward();
			break;

		case 'S':
		case 's':
		case VK_DOWN:
			CameraMoveForward();
			break;

		case 'A':
		case 'a':
		case VK_LEFT:
			CameraMoveLeft();
			break;

		case 'D':
		case 'd':
		case VK_RIGHT:
			CameraMoveRight();
			break;

		case 'Q':
		case 'q':
			CameraMoveDown();
			break;

		case 'E':
		case 'e':
			CameraMoveUp();
			break;

		case 'L':
		case 'l':
			if (bLight == 0)
				bLight = 1;
			else
				bLight = 0;
			break;

		default:
			break;
		}

		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'f':
		case 'F':
			ToggleFullscreen();
			break;

		default:
			break;
		}
		break;

	case WM_MOUSEMOVE:
		mouseXpos = (float)GET_X_LPARAM(lParam);
		mouseYpos = (float)GET_Y_LPARAM(lParam);

		switch (wParam)
		{
		case MK_LBUTTON:
		{
			CameraMoveOnMouse(mouseXpos, mouseYpos, lastX, lastY);
		}
		break;
		}

		lastX = mouseXpos;
		lastY = mouseYpos;
		break;

	case WM_ERASEBKGND:
		// OS you don't do ERASE Background. OpenGL and DirectX will do ERASE
		return(0);

	case WM_SIZE:
		screenWidth = LOWORD(lParam);
		screenHeight = HIWORD(lParam);
		Resize(LOWORD(lParam), HIWORD(lParam));
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

	if (currentScene == SCENE_1_OPENING_SCENE)
		Scene1OpeningSceneCallback(hwnd, message, wParam, lParam);
	if (DISPLAY_MODE == TEST_EFFECT)
	{
		TestDownloadedStaticModelCallback(hwnd, message, wParam, lParam);
	}
#if TEST_TRANSFORM
	TestTransformModelSceneCallback(hwnd, message, wParam, lParam);
#endif
	
	return(DefWindowProc(hwnd, message, wParam, lParam));
}

// toggle fullscreen
void ToggleFullscreen(void)
{
	// variable declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;

	// code
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

				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, (mi.rcMonitor.right - mi.rcMonitor.left), (mi.rcMonitor.bottom - mi.rcMonitor.top), SWP_NOZORDER | SWP_FRAMECHANGED); // WM_NCCALCSIZE
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

bool Initialize(void)
{
	// function prototype
	void Resize(int, int);
	void PrintOpenGLInfo(void);
	void Uninitialize(void);
	bool loadTextureFromFile(const char* filename, GLuint * texture);
	bool InitializeCommonShaderProgramObjects(void);
	bool InitializeCommonRendererVaoVbo(void);
	bool InitializeAllScenes(void);

	// variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	// code

	// initialization PFD
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

	pfd.cDepthBits = 32;

	// get DC
	ghdc = GetDC(ghwnd);

	// choose pixel format
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ErrorLog(NULL, "Failed to choose pixel format.\n");
		return(false);
	}

	// set pixel format
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		ErrorLog(NULL, "Failed to set pixel format.\n");
		return(false);
	}

	// create OpenGL rendering context
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ErrorLog(NULL, "Failed to OpenGL context.\n");
		return(false);
	}

	// make rednering current as current context
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		ErrorLog(NULL, "Failed to make OpenGL context the current context.\n");
		return(false);
	}

	// here starts OpenGL code

	// glew initialization
	if (glewInit() != GLEW_OK)
	{
		return(false);
	}

	// print OpenGL information
	PrintOpenGLInfo();

	//###########################################################

	InitializeCamera();

	if (!InitializeConfigurations())
	{
		ErrorLog(NULL, "Initialize()::InitializeConfigurations() failed.");
		return(false);
	}
	else
	{
		InfoLog(NULL, "Initialize()::InitializeConfigurations() succeeded.");
	}

	if (!InitializeUBOs())
	{
		ErrorLog(NULL, "Initialize()::InitializeUBOs() failed.");
		return(false);
	}
	else
	{
		InfoLog(NULL, "Initialize()::InitializeUBOs() succeeded.");
	}

	result = InitializeCommonShaderProgramObjects();
	if (!result)
	{
		ErrorLog(NULL, "Initialize()::InitializeCommonShaderProgramObjects() failed.");
		return false;
	}
	else
	{
		InfoLog(NULL, "Initialize()::InitializeCommonShaderProgramObjects() succeeded.");
	}

	// ------ RENDERER INITIALIZATION ------
	result = InitializeCommonRendererVaoVbo();
	if (!result)
	{
		ErrorLog(NULL, "Initialize()::InitializeCommonRendererVaoVbo() failed.");
		return false;
	}
	else
	{
		InfoLog(NULL, "Initialize()::InitializeCommonRendererVaoVbo() succeeded.");
	}

	// ------ SCENES INITIALIZATION ------
	result = InitializeAllScenes();
	if (!result)
	{
		ErrorLog(NULL, "Initialize()::InitializeAllScenes() failed.");
		return false;
	}
	else
	{
		InfoLog(NULL, "Initialize()::InitializeAllScenes() succeeded.");
	}

	// NOTE : keep it after all scene initalization
	if (!BindUBOs())
	{
		ErrorLog(NULL, "Initialize()::BindUBOs failed.");
		return(false);
	}
	else
	{
		InfoLog(NULL, "Initialize()::BindUBOs succeeded.");
	}

	if (DISPLAY_MODE == PRESENT_DEMO)
	{
		if (!EnterPresentation())
		{
			ErrorLog(NULL, "Initialize()::EnterPresentation() failed.");
			return(false);
		}
		else
		{
			InfoLog(NULL, "Initialize()::EnterPresentation() succeeded.");
		}
	}


	//###########################################################

	// depth changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glEnable(GL_CULL_FACE);

	// clear the screen using black color
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	Resize(WIN_WIDTH, WIN_HEIGHT);

	return true;;
}

bool InitializeCommonShaderProgramObjects(void)
{
	// code

	if (!InitializeTextureShader())
	{
		ErrorLog(NULL, "InitializeCommonShaderProgramObjects()::InitializeTextureShader() failed.");
		return false;
	}
	if (!InitializeColorShader())
	{
		ErrorLog(NULL, "InitializeCommonShaderProgramObjects()::InitializeColorShader() failed.");
		return false;
	}
	/*if (!InitializePhongLightShader())
	{
		ErrorLog(NULL, "InitializeCommonShaderProgramObjects()::InitializePhongLightShader() failed.");
		return false;
	}*/
	if (!InitializeFont())
	{
		ErrorLog(NULL, "InitializeCommonShaderProgramObjects()::InitializeFont() failed.");
		return false;
	}
	if (!InitializeStaticModelShader())
	{
		ErrorLog(NULL, "InitializeCommonShaderProgramObjects()::InitializeStaticModelShader() failed.");
		return false;
	}

	if (!InitializeGrass())
	{
		ErrorLog(NULL, "InitializeCommonShaderProgramObjects()::InitializeGrass() failed.");
		return false;
	}
	
	return true;
}

bool InitializeCommonRendererVaoVbo(void)
{
	// code

	InitializeCubeRenderer();
	InitializeQuadRenderer();
	
	return true;
}

bool InitializeAllScenes(void)
{
	switch (DISPLAY_MODE)
	{
	case TEST_EFFECT:
		if (!InitializeTestScene())
		{
			ErrorLog(NULL, "InitializeTestScene is failed\n");
			return false;
		}
		else
		{
			TraceLog(NULL, "InitializeTestScene is successed\n");
		}
		break;

	case PRESENT_DEMO:
		InitializeTestScene();
		if (!InitializeScenes())
		{
			ErrorLog(NULL, "InitializeAllScenes()::InitializeScenes() failed.");
			return false;
		}
		else
		{
			InfoLog(NULL, "InitializeAllScenes()::InitializeScenes() succeeded.");
		}
		break;

	case DEFAULT_SCENE:
	default:
		if (currentScene == SCENE_0_BANNER)
		{
			if (InitializeScene0Banner() == false) return false;
		}
		else if (currentScene == SCENE_1_OPENING_SCENE)
		{
			if (InitializeScene1OpeningScene() == false) return false;
		}


		/*if (!InitializeDefaultScene())
		{
			ErrorLog(NULL, "InitializeAllScenes()::InitializeDefaultScene() failed.");
			return false;
		}
		else
		{
			InfoLog(NULL, "InitializeAllScenes()::InitializeDefaultScene() succeeded.");
		}*/
		break;
	}

	return true;
}

void PrintOpenGLInfo(void)
{
	//local variable declarations
	GLint numExtensions;

	// code
	InfoLog(NULL, "OpenGL Vendor		: %s", glGetString(GL_VENDOR));
	InfoLog(NULL, "OpenGL Renderer    \t: %s", glGetString(GL_RENDERER));
	InfoLog(NULL, "OpenGL Version		: %s", glGetString(GL_VERSION));
	InfoLog(NULL, "GLSL Version		: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	InfoLog(NULL, "Number of supported extensions : %d", numExtensions);
	for (int i = 0; i < numExtensions; i++)
	{
		InfoLog(NULL, " %s\n", glGetStringi(GL_EXTENSIONS, i));
	}
}

void Resize(int width, int height)
{
	// code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 50000.0f);
}

void Display(void)
{
	// func prototype
	void DisplayFPS(void);


	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewMatrix = mat4::identity();
	viewMatrix = vmath::lookat(cameraPosition, cameraPosition + cameraFront, cameraUp);

	// Set uniform buffer objects data.
	SetUBOs();

	
	switch (DISPLAY_MODE)
	{
//#if TEST_EFFECT
//	DisplayTestScene(void);
//#endif
	case TEST_EFFECT:
		DisplayTestScene();
		break;

	case PRESENT_DEMO:
		// TODO_00: Push matrix
		gsmdCurrentSceneMetaData.display();
		// TODO_01: Pop matrix
		break;

//#if DEFAULT_SCENE
//	DisplayDefaultScene(void);
//#endif
	case DEFAULT_SCENE:
	default:
		//DisplayDefaultScene(viewMatrix, perspectiveProjectionMatrix);
		if (currentScene == SCENE_0_BANNER)
		{
			DisplayScene0Banner();
		}
		else if (currentScene == SCENE_1_OPENING_SCENE)
		{
			DisplayScene1OpeningScene(viewMatrix, perspectiveProjectionMatrix);
		}
		break;
	}

	DisplayFPS();
	SwapBuffers(ghdc);
}

void DisplayFPS(void)
{
	mat4 translationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	// font rendering

	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	modelMatrix = translationMatrix;
	SetFontShader(mat4::identity());

	sprintf(frameRateString, "FPS: %u", g_FPS_rate);
	RenderText(frameRateString, 0.8f, 0.8f, 0.001f, vec3(1.0f, 1.0f, 0.0f), characters_font1);
	UnsetFontShader();

	// font rendering
	SetFontShader(mat4::identity());
	char str[1024];
	sprintf(str, "Camera Translate : [%f, %f, %f]", cameraPosition[0], cameraPosition[1], cameraPosition[2]);
	RenderText(str, -1.0f, -0.9f, 0.001f, vec3(1.0f, 1.0f, 0.0f), characters_font1);
	UnsetFontShader();
}

void Update(double delta)
{
	// code
	switch (DISPLAY_MODE)
	{
	case PRESENT_DEMO:
		if (ShouldGoToNextScene())
		{
			/*if (gbDebug)
			{
				fprintf(gpFile, "scene22Update():if (shouldGoToNextScene())");
				fflush(gpFile);
			}*/

			SetupNextScene();
			return;
		}

		guiElapsedSceneDuration = ((unsigned long)GetElapsedTime()) - TIME_TO_MILLIS(gsmdCurrentSceneMetaData.startTime);

		gsmdCurrentSceneMetaData.update();
		break;

	default:
		break;
	}
}

void Uninitialize(void)
{
	// function prototype
	// Function declarations
	void UninitializeCommonShaderProgramObjects(void);
	void UninitializeCommonRendererVaoVbo(void);
	void UninitializeAllScenes(void);
	void ToggleFullscreen(void);

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

	UninitializeUBOs();
	UninitializeAllScenes();
	UninitializeCommonRendererVaoVbo();
	UninitializeCommonShaderProgramObjects();
	UninitializeConfigurations();

	UninitializeLogger();
	// TODO: Remove below commented code.
	/*if (gpFile)
	{
		fprintf(gpFile, "Log file is closed successfully");
		fclose(gpFile);
		gpFile = NULL;
	}*/
}

void UninitializeCommonShaderProgramObjects(void)
{
	// here we will be releasing all commonly used shaders
	UnInitializeGrass();
	UninitializeTextureShader();
	UninitializeColorShader();
	UninitializePhongLightShader();

}

void UninitializeCommonRendererVaoVbo(void)
{
	// here we will be deleteing all used common vao and vbo
	UninitializeCubeRenderer();
	UninitializeQuadRenderer();
}

void UninitializeAllScenes(void)
{
	// uninitialize all scenes
	switch (DISPLAY_MODE)
	{
	case TEST_EFFECT:
		UninitializeTestScene();
		break;

	case PRESENT_DEMO:
		UninitializeScenes();
		UninitializeTestScene();
		break;

	case DEFAULT_SCENE:
	default:
		//UninitializeDefaultScene();
		if (currentScene == SCENE_0_BANNER)
		{
			UninitializeScene0Banner();
		}
		else if (currentScene == SCENE_1_OPENING_SCENE)
		{
			UninitializeScene1OpeningScene();
		}
		break;
	}
}
