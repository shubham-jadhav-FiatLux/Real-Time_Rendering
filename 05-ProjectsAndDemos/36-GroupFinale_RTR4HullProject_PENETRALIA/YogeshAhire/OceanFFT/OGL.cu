// header files
#include<Windows.h>
#include <windowsx.h>
#include "vmath.h"
#include "OGL.h"
#include <stdio.h>   //For FILE IO Functions
#include <stdlib.h> //For exit function


//OpenGL header files
#include<GL/glew.h> //this must be above gl.h
#include<GL/gl.h>

#include<cuda_runtime.h>
#include<cuda_gl_interop.h>


#include <cufft.h>
#include <math_constants.h>

using namespace vmath;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//OpenGL libraries
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"cudart.lib")
#pragma comment(lib,"cufft.lib")


// Global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global Variable declarations
FILE* gpFile = NULL;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
BOOL gbFullScreen = FALSE;
BOOL gbActiveWindow = FALSE;

//Programable pipeline related global variables
GLuint shaderProgramObject=0;

enum 
{
	YAA_ATTRIBUTE_POSITION=0,
	YAA_ATTRIBUTE_COLOR,
	YAA_ATTRIBUTE_NORMAL,
	YAA_ATTRIBUTE_TEXTURE0
};
GLuint vao;
GLuint vbo;

GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionMatrixUniform;

GLuint heightScaleUniform;
GLuint chopinessUniform;
GLuint sizeUniform;

GLuint deepColorUniform;
GLuint shallowColorUniform;
GLuint skyColorUniform;
GLuint lightDirUniform;

mat4 perspectiveProjectionMatrix;

//SinWave related variables
//const unsigned int meshWidth = 1024;
//const unsigned int meshHeight = 1024;

const unsigned int meshSize = 256;
const unsigned int spectrumW = meshSize + 4;
const unsigned int spectrumH = meshSize + 1;

// simulation parameters
const float g = 9.81f;        // gravitational constant
const float A = 1e-7f;        // wave scale factor
const float patchSize = 100;  // patch size
float windSpeed = 100.0f;
float windDir = CUDART_PI_F / 3.0f;
float dirDepend = 0.07f;


cudaError_t cudaResult;
struct cudaGraphicsResource *graphicsResource = NULL;

GLuint posVertexBuffer;
GLuint indexBuffer;
GLuint heightVertexBuffer, slopeVertexBuffer;
struct cudaGraphicsResource *cuda_posVB_resource, *cuda_heightVB_resource,
    *cuda_slopeVB_resource;  // handles OpenGL-CUDA exchange




float animationTime = 0.0f;

bool onGPU = false;

// FFT data
cufftHandle fftPlan;
float2 *d_h0 = 0;  // heightfield at time 0
float2 *h_h0 = 0;
float2 *d_ht = 0;  // heightfield at time t
float2 *d_slope = 0;

// pointers to device object
float *g_hptr = NULL;
float2 *g_sptr = NULL;


float animTime = 0.0f;

// camera variable
#define YAW -90.0f
#define PITCH 0.0f
#define SPEED 1.0f
#define SENSITIVITY 0.2f
#define ZOOM 45.0f

float yaw = YAW;
float pitch = PITCH;
float movement_speed = SPEED;
float mouse_sensitivity = SENSITIVITY;
float zoom = ZOOM;
float lastX = 0.0f; // (float)SCREEN_WIDTH / 2.0f;
float lastY = 0.0f; // (float)SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;
bool constraintPitch = true;
float mouseXpos;
float mouseYpos;

vmath::vec3 cameraPosition = vec3(0.0f, 0.0f, 6.0f);
vmath::vec3 cameraFront;
vmath::vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
vmath::vec3 cameraRight;
vmath::vec3 cameraWorldUp = cameraUp;

/*
vec3 cameraEye = vec3(0.0f, 0.0f, 6.0f);	// position
vec3 cameraLookat = vec3(0.0f, 0.0f, -1.0f); // front or lookat
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);		// up
*/

void updateCamera()
{
	vmath::vec3 front;
	front[0] = cos(vmath::radians(yaw)) * cos(vmath::radians(pitch));
	front[1] = sin(vmath::radians(pitch));
	front[2] = sin(vmath::radians(yaw)) * cos(vmath::radians(pitch));
	cameraFront = front;
	cameraRight = vmath::normalize(vmath::cross(cameraFront, cameraWorldUp));
	cameraUp = vmath::normalize(vmath::cross(cameraRight, cameraFront));
}




// Round a / b to nearest higher integer value
int cuda_iDivUp(int a, int b) { return (a + (b - 1)) / b; }

// complex math functions
__device__ float2 conjugate(float2 arg) { return make_float2(arg.x, -arg.y); }

__device__ float2 complex_exp(float arg) {
  return make_float2(cosf(arg), sinf(arg));
}

__device__ float2 complex_add(float2 a, float2 b) {
  return make_float2(a.x + b.x, a.y + b.y);
}

__device__ float2 complex_mult(float2 ab, float2 cd) {
  return make_float2(ab.x * cd.x - ab.y * cd.y, ab.x * cd.y + ab.y * cd.x);
}

// generate wave heightfield at time t based on initial heightfield and
// dispersion relationship
__global__ void generateSpectrumKernel(float2 *h0, float2 *ht,
                                       unsigned int in_width,
                                       unsigned int out_width,
                                       unsigned int out_height, float t,
                                       float patchSize) {
  unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
  unsigned int in_index = y * in_width + x;
  unsigned int in_mindex =
      (out_height - y) * in_width + (out_width - x);  // mirrored
  unsigned int out_index = y * out_width + x;

  // calculate wave vector
  float2 k;
  k.x = (-(int)out_width / 2.0f + x) * (2.0f * CUDART_PI_F / patchSize);
  k.y = (-(int)out_width / 2.0f + y) * (2.0f * CUDART_PI_F / patchSize);

  // calculate dispersion w(k)
  float k_len = sqrtf(k.x * k.x + k.y * k.y);
  float w = sqrtf(9.81f * k_len);

  if ((x < out_width) && (y < out_height)) {
    float2 h0_k = h0[in_index];
    float2 h0_mk = h0[in_mindex];

    // output frequency-space complex values
    ht[out_index] =
        complex_add(complex_mult(h0_k, complex_exp(w * t)),
                    complex_mult(conjugate(h0_mk), complex_exp(-w * t)));
    // ht[out_index] = h0_k;
  }
}

// update height map values based on output of FFT
__global__ void updateHeightmapKernel(float *heightMap, float2 *ht,
                                      unsigned int width) {
  unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
  unsigned int i = y * width + x;

  // cos(pi * (m1 + m2))
  float sign_correction = ((x + y) & 0x01) ? -1.0f : 1.0f;

  heightMap[i] = ht[i].x * sign_correction;
}

// update height map values based on output of FFT
__global__ void updateHeightmapKernel_y(float *heightMap, float2 *ht,
                                        unsigned int width) {
  unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
  unsigned int i = y * width + x;

  // cos(pi * (m1 + m2))
  float sign_correction = ((x + y) & 0x01) ? -1.0f : 1.0f;

  heightMap[i] = ht[i].y * sign_correction;
}

// generate slope by partial differences in spatial domain
__global__ void calculateSlopeKernel(float *h, float2 *slopeOut,
                                     unsigned int width, unsigned int height) {
  unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
  unsigned int i = y * width + x;

  float2 slope = make_float2(0.0f, 0.0f);

  if ((x > 0) && (y > 0) && (x < width - 1) && (y < height - 1)) {
    slope.x = h[i + 1] - h[i - 1];
    slope.y = h[i + width] - h[i - width];
  }

  slopeOut[i] = slope;
}

// wrapper functions
 void cudaGenerateSpectrumKernel(float2 *d_h0, float2 *d_ht,
                                           unsigned int in_width,
                                           unsigned int out_width,
                                           unsigned int out_height,
                                           float animTime, float patchSize) {
  dim3 block(8, 8, 1);
  dim3 grid(cuda_iDivUp(out_width, block.x), cuda_iDivUp(out_height, block.y),
            1);
  generateSpectrumKernel<<<grid, block>>>(d_h0, d_ht, in_width, out_width,
                                          out_height, animTime, patchSize);
}

 void cudaUpdateHeightmapKernel(float *d_heightMap, float2 *d_ht,
                                          unsigned int width,
                                          unsigned int height, bool autoTest) {
  dim3 block(8, 8, 1);
  dim3 grid(cuda_iDivUp(width, block.x), cuda_iDivUp(height, block.y), 1);
  if (autoTest) {
    updateHeightmapKernel_y<<<grid, block>>>(d_heightMap, d_ht, width);
  } else {
    updateHeightmapKernel<<<grid, block>>>(d_heightMap, d_ht, width);
  }
}

 void cudaCalculateSlopeKernel(float *hptr, float2 *slopeOut,
                                         unsigned int width,
                                         unsigned int height) {
  dim3 block(8, 8, 1);
  dim3 grid2(cuda_iDivUp(width, block.x), cuda_iDivUp(height, block.y), 1);
  calculateSlopeKernel<<<grid2, block>>>(hptr, slopeOut, width, height);
  cudaError_t err = cudaGetLastError();        // Get error code
 if (err != cudaSuccess)
 {
	 fprintf(gpFile, "CUDA Error in cudaCalculateSlopeKernel: %s\n", cudaGetErrorString(err));
	 exit(-1);
 }
}


// Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function declarations
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);


	// variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	BOOL bDone = FALSE;
	int iRetVal=0;
	int width, height, x, y;

	// code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation of LOG file failed. Exiting..."), TEXT("File I/O Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File is successfully created\n");
	}

	// Initialization of WNDCLASSEX structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// Register WNDCLASSEX
	RegisterClassEx(&wndclass);

	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);
	x = (width / 2) - (WIN_WIDTH / 2);
	y = (height / 2) - (WIN_HEIGHT / 2);


	// create the window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("OGL Window"),
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE,
		x,
		y,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;
	
	//initialize
	iRetVal = initialize();
	if (iRetVal == -1)
	{
		fprintf(gpFile, "Choose Pixel format failed\n");
		uninitialize();
	}
	else if (iRetVal == -2)
	{
		fprintf(gpFile, "Set Pixel format failed\n");
		uninitialize();
	}
	else if (iRetVal == -3)
	{
		fprintf(gpFile, "Create OpenGL Context failed\n");
		uninitialize();
	}
	else if (iRetVal == -4)
	{
		fprintf(gpFile, "Making OpenGL context and current context failed\n");
		uninitialize();
	}
	else if (iRetVal == -5)
	{
		fprintf(gpFile, "glewInit failed\n");
		uninitialize();
	}
	else 
	{
		fprintf(gpFile, "Initialize is Successful\n");
	}

	// show Window
	ShowWindow(hwnd, iCmdShow);

	//Foregrounding and focusing the window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Message Loop
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
				//Render the scene
				display();

				//update the scene
				update();
			}
		}
	}

	return ((int)msg.wParam);
}

// Callback function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Function declaration
	void ToggleFullScreen(void);
	void resize(int, int);
	void uninitialize(void);

	// code
	switch (iMsg)
	{
	
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		fprintf(gpFile, "Window focus is Set\n");
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		fprintf(gpFile, "Window focus is unset\n");
		break;
	case WM_ERASEBKGND:
		//break;
		return 0;
	case WM_CHAR:
		switch (wParam)
		{
		case 'c':
		case 'C':
			onGPU = false;
			break;
		case 'g':
		case 'G':
			onGPU = true;
			break;
		case 'F':
		case 'f':
			ToggleFullScreen();
			break;
		case 'W':
		case 'w':
			cameraPosition += movement_speed * cameraFront;
			break;

		case 'S':
		case 's':
			cameraPosition -= movement_speed * cameraFront;
			break;

		case 'A':
		case 'a':
			cameraPosition -= cameraRight * movement_speed;
			break;

		case 'D':
		case 'd':
			cameraPosition += cameraRight * movement_speed;
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
	case WM_MOUSEMOVE:
		mouseXpos = GET_X_LPARAM(lParam);
		mouseYpos = GET_Y_LPARAM(lParam);

		switch (wParam)
		{
		case MK_LBUTTON:
		{
			/*if (firstMouse)
			{
				lastX = mouseXpos;
				lastY = mouseYpos;
				firstMouse = false;
			}*/

			float xoffset = mouseXpos - lastX;
			float yoffset = lastY - mouseYpos;


			xoffset *= mouse_sensitivity;
			yoffset *= mouse_sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			if (constraintPitch)
			{
				if (pitch > 89.0f)
					pitch = 89.0f;
				if (pitch < -89.0f)
					pitch = -89.0f;
			}
			vmath::vec3 front;
			front[0] = cos(vmath::radians(yaw)) * cos(vmath::radians(pitch));
			front[1] = sin(vmath::radians(pitch));
			front[2] = sin(vmath::radians(yaw)) * cos(vmath::radians(pitch));
			cameraFront = vmath::normalize(front);
			cameraRight = vmath::normalize(vmath::cross(cameraFront, cameraWorldUp));
			cameraUp = vmath::normalize(vmath::cross(cameraRight, cameraFront));
		}
		break;
		}

		lastX = mouseXpos;
		lastY = mouseYpos;
		break;
	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{
	//variable declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;
	

	//code
	wp.length = sizeof(WINDOWPLACEMENT);

	if (gbFullScreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);  //GWL_STYLE - Get window long style
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);

			if (GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			ShowCursor(FALSE);
			gbFullScreen = TRUE;
		}		
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE,dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullScreen = FALSE;
	}
}

int initialize(void)
{
	//function declarations
	void resize(int width, int height);
	void printGLInfo(void);
	void uninitialize(void);
    void createMeshPositionVBO(GLuint *, int , int );
    void createMeshIndexBuffer(GLuint *, int , int );
    void generate_h0(float2 *h0) ;
    void runCuda();
	void updateCamera();
	void createVBO(GLuint * vbo, int size);

	//variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex=0;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));  //memset((void*)&pfd,NULL,sizeof(PIXELFORMATDESCRIPTOR));
	//initialization of PIXELFORMATDESCRIPTOR
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

	//GetDC
	ghdc = GetDC(ghwnd);

	//Choose pixel format
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);  

	if (iPixelFormatIndex == 0)
	{
		return -1;
	}

	//Set the choosen pixel format
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd)==FALSE)
	{
		return -2;
	}

	//Create OpenGL rendering contex
	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL)
		return -3;

	//Make the rendering context as the current context
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
		return -4;

	//GLEW initialization
	if (glewInit() != GLEW_OK)
	{
		return -5;
	}
	//print openGL Info
	//printGLInfo();

	//Cuda initialization
	 int dev_count = 0;
	cudaResult = cudaGetDeviceCount(&dev_count);
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile,"CUDA Runtime API Error - cudaGetDeviceCount() Failed due to %s \n", cudaGetErrorString(cudaResult));
		uninitialize();
		exit(EXIT_FAILURE);

	}
	else if (dev_count == 0)
	{
		fprintf(gpFile, "There is no CUDA supported device on this system\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}

	//select CUDA supported device
	cudaSetDevice(0);  //selecting the default 0th CUDA supported device


	//Vertex Shader
	const GLchar* vertexShadersourcecode =
		"#version 460 compatibility" \
		"\n" \
		"uniform float heightScale;" \
        "uniform float chopiness;" \
        "uniform vec2  size;" \
        "uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
        "out vec3 eyeSpacePos;" \
        "out vec3 worldSpaceNormal;" \
        "out vec3 eyeSpaceNormal;" \
		"void main(void)" \
		"{" \
		"float height     = gl_MultiTexCoord0.x;" \
        "vec2  slope      = gl_MultiTexCoord1.xy;" \
        "vec3 normal      = normalize(cross( vec3(0.0, slope.y*heightScale, 2.0 / size.x), vec3(2.0 / size.y, slope.x*heightScale, 0.0)));" \
        "worldSpaceNormal = normal;" \
        "vec4 pos         = vec4(gl_Vertex.x, height * heightScale, gl_Vertex.z, 1.0);" \
       /*"gl_Position      = vec4(mat3(u_projectionMatrix) * mat3(u_viewMatrix) * mat3(u_modelMatrix)  * pos.xyz,1.0);" \*/
		"gl_Position      = ((u_projectionMatrix) * (u_viewMatrix) * (u_modelMatrix)  * pos);" \
        "eyeSpacePos      = (mat3(u_viewMatrix) * mat3(u_modelMatrix)  * pos.xyz).xyz;" \
        "eyeSpaceNormal   = (mat3(u_viewMatrix) * mat3(u_modelMatrix)  * normal).xyz;" \
		"}";

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShadersourcecode, NULL);

	//compile the code with inline compiler ...human understandable format to machine understandable format ..here machine means GPU
	glCompileShader(vertexShaderObject);

	//error checking in compilation
	GLint status;
	GLint infoLogLength;
	char* log = NULL;
	glGetShaderiv(vertexShaderObject,GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;

				glGetShaderInfoLog(vertexShaderObject, infoLogLength,&written, log);

				fprintf(gpFile, "Vertex Shader Compilation log : %s", log);

				free(log);

				uninitialize();
			}
		}
	}

	//fragment shader
	const GLchar* fragmentShadersourcecode =
		"#version 460 compatibility" \
		"\n" \
        "in vec3 eyeSpacePos;" \
        "in vec3 worldSpaceNormal;" \
        "in vec3 eyeSpaceNormal;" \
        "uniform vec4 deepColor;" \
        "uniform vec4 shallowColor;" \
        "uniform vec4 skyColor;" \
        "uniform vec3 lightDir;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec3 eyeVector              = normalize(eyeSpacePos);" \
        "vec3 eyeSpaceNormalVector   = normalize(eyeSpaceNormal);" \
        "vec3 worldSpaceNormalVector = normalize(worldSpaceNormal);" \
        "float facing    = max(0.0, dot(eyeSpaceNormalVector, -eyeVector));" \
        "float fresnel   = pow(1.0 - facing, 5.0);" \
        "float diffuse   = max(0.0, dot(worldSpaceNormalVector, lightDir));" \
       /* "vec4 waterColor = deepColor;" \*/
		"vec4 waterColor = mix(shallowColor, deepColor, facing);" \
        "FragColor = waterColor*diffuse + skyColor*fresnel;" \
		"}";


	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	

	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShadersourcecode, NULL);

	glCompileShader(fragmentShaderObject);
	
	status=0;
	infoLogLength=0;
	log = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);

			if (log != NULL)
			{
				GLsizei written;

				glGetShaderInfoLog(fragmentShaderObject,infoLogLength,&written,log);

				fprintf(gpFile, "Fragment Shader Compilation log : %s", log);

				free(log);

				uninitialize();
			}
		}
	}

	//Shader program object
	shaderProgramObject = glCreateProgram();

	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject, YAA_ATTRIBUTE_POSITION, "a_position");

	glLinkProgram(shaderProgramObject);

	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);

			if (log != NULL)
			{
				GLsizei written;

				glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, log);

				fprintf(gpFile, "Shader Progam link log : %s", log);

				free(log);

				uninitialize();
			}
		}
	}

	//post link - getting uniform location from shader prog object
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");

	heightScaleUniform = glGetUniformLocation(shaderProgramObject, "heightScale");
	chopinessUniform = glGetUniformLocation(shaderProgramObject, "chopiness");
	sizeUniform = glGetUniformLocation(shaderProgramObject, "size");

	deepColorUniform = glGetUniformLocation(shaderProgramObject, "deepColor");
	shallowColorUniform = glGetUniformLocation(shaderProgramObject, "shallowColor");
	skyColorUniform = glGetUniformLocation(shaderProgramObject, "skyColor");
	lightDirUniform = glGetUniformLocation(shaderProgramObject, "lightDir");

	cufftPlan2d(&fftPlan, meshSize, meshSize, CUFFT_C2C);

    int spectrumSize = spectrumW * spectrumH * sizeof(float2);
	cudaResult = (cudaMalloc((void**)&d_h0, spectrumSize));
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "CUDA Runtime API Error - cudaMalloc() Failed due to %s \n", cudaGetErrorString(cudaResult));
		uninitialize();
		exit(EXIT_FAILURE);

	}
    h_h0 = (float2 *)malloc(spectrumSize);
    generate_h0(h_h0);
	cudaResult = (cudaMemcpy(d_h0, h_h0, spectrumSize, cudaMemcpyHostToDevice));
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "CUDA Runtime API Error - cudaMalloc() Failed due to %s \n", cudaGetErrorString(cudaResult));
		uninitialize();
		exit(EXIT_FAILURE);

	}

	int outputSize = meshSize * meshSize * sizeof(float2);
	cudaResult = (cudaMalloc((void**)&d_ht, outputSize));
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "CUDA Runtime API Error - cudaMalloc() Failed due to %s \n", cudaGetErrorString(cudaResult));
		uninitialize();
		exit(EXIT_FAILURE);

	}
	cudaResult = (cudaMalloc((void**)&d_slope, outputSize));
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "CUDA Runtime API Error - cudaMalloc() Failed due to %s \n", cudaGetErrorString(cudaResult));
		uninitialize();
		exit(EXIT_FAILURE);

	}

	createVBO(&heightVertexBuffer, meshSize* meshSize * sizeof(float));

	//create CUDA Interoperability resource
	cudaResult = cudaGraphicsGLRegisterBuffer(&cuda_heightVB_resource,heightVertexBuffer,cudaGraphicsMapFlagsWriteDiscard);
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsGLRegisterBuffer() Failed due to %s \n", cudaGetErrorString(cudaResult));
		uninitialize();
		exit(EXIT_FAILURE);

	}

	createVBO(&slopeVertexBuffer, outputSize);

   cudaResult = cudaGraphicsGLRegisterBuffer(&cuda_slopeVB_resource,slopeVertexBuffer,cudaGraphicsMapFlagsWriteDiscard);
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsGLRegisterBuffer() Failed due to %s \n", cudaGetErrorString(cudaResult));
		uninitialize();
		exit(EXIT_FAILURE);

	}

      createMeshPositionVBO(&posVertexBuffer, meshSize, meshSize);
      createMeshIndexBuffer(&indexBuffer, meshSize, meshSize);

     //runCuda();

	//Depth and clearcolor related code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	
	
	//Clear the screen using blue color
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	updateCamera();
	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//! Run the Cuda kernels
////////////////////////////////////////////////////////////////////////////////
void runCuda() {

	fprintf(gpFile, "Start : %s\n", __FUNCTION__);
	fflush(gpFile);
	void   uninitialize();

  size_t num_bytes;

  // generate wave spectrum in frequency domain
   cudaGenerateSpectrumKernel(d_h0, d_ht, spectrumW, meshSize, meshSize,
                             animTime, patchSize);
   cudaError_t err = cudaGetLastError();        // Get error code
   if (err != cudaSuccess)
   {
	   fprintf(gpFile, "CUDA Error in cudaGenerateSpectrumKernel: %s\n", cudaGetErrorString(err));
	   exit(-1);
   }

  // execute inverse FFT to convert to spatial domain
  (cufftExecC2C(fftPlan, d_ht, d_ht, CUFFT_INVERSE));

  // update heightmap values in vertex buffer
  cudaResult = (cudaGraphicsMapResources(1, &cuda_heightVB_resource, 0));
  if (cudaResult != cudaSuccess)
  {
	  fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsMapResources() Failed for cuda_heightVB_resource due to %s \n", cudaGetErrorString(cudaResult));
	  uninitialize();
	  exit(EXIT_FAILURE);

  }

  cudaResult = (cudaGraphicsResourceGetMappedPointer((void **)&g_hptr, &num_bytes, cuda_heightVB_resource));
  if (cudaResult != cudaSuccess)
  {
	  fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsResourceGetMappedPointer() Failed for cuda_heightVB_resource due to %s \n", cudaGetErrorString(cudaResult));
	  uninitialize();
	  exit(EXIT_FAILURE);
  }

  cudaUpdateHeightmapKernel(g_hptr, d_ht, meshSize, meshSize, false);
    err = cudaGetLastError();        // Get error code
  if (err != cudaSuccess)
  {
	  fprintf(gpFile, "CUDA Error in cudaUpdateHeightmapKernel: %s\n", cudaGetErrorString(err));
	  exit(-1);
  }
 


  // calculate slope for shading
  cudaResult = (cudaGraphicsMapResources(1, &cuda_slopeVB_resource, 0));
  if (cudaResult != cudaSuccess)
  {
	  fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsMapResources() Failed for cuda_slopeVB_resource due to %s \n", cudaGetErrorString(cudaResult));
	  uninitialize();
	  exit(EXIT_FAILURE);
  }


  cudaResult = (cudaGraphicsResourceGetMappedPointer((void**)&g_sptr, &num_bytes, cuda_slopeVB_resource));
  if (cudaResult != cudaSuccess)
  {
	  fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsResourceGetMappedPointer() Failed for cuda_slopeVB_resource due to %s \n", cudaGetErrorString(cudaResult));
	  uninitialize();
	  exit(EXIT_FAILURE);
  }

  cudaCalculateSlopeKernel(g_hptr, g_sptr, meshSize, meshSize);
  err = cudaGetLastError();        // Get error code
  if (err != cudaSuccess)
  {
	  fprintf(gpFile, "CUDA Error in cudaCalculateSlopeKernel: %s\n", cudaGetErrorString(err));
	  exit(-1);
  }


  cudaResult = (cudaGraphicsUnmapResources(1, &cuda_heightVB_resource, 0));
  if (cudaResult != cudaSuccess)
  {
	  fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsUnmapResources() Failed for cuda_heightVB_resource due to %s \n", cudaGetErrorString(cudaResult));
	  uninitialize();
	  exit(EXIT_FAILURE);
  }

  cudaResult = (cudaGraphicsUnmapResources(1, &cuda_slopeVB_resource, 0));
  if (cudaResult != cudaSuccess)
  {
	  fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsResourceGetMappedPointer() Failed for cuda_slopeVB_resource due to %s \n", cudaGetErrorString(cudaResult));
	  uninitialize();
	  exit(EXIT_FAILURE);
  }

   
  fprintf(gpFile, "End : %s\n", __FUNCTION__);
  fflush(gpFile);
}



// Generate base heightfield in frequency space
void generate_h0(float2 *h0) {
    float phillips(float Kx, float Ky, float Vdir, float V, float A,
               float dir_depend);
               float gauss();

  for (unsigned int y = 0; y <= meshSize; y++) {
    for (unsigned int x = 0; x <= meshSize; x++) {
      float kx = (-(int)meshSize / 2.0f + x) * (2.0f * CUDART_PI_F / patchSize);
      float ky = (-(int)meshSize / 2.0f + y) * (2.0f * CUDART_PI_F / patchSize);

      float P = sqrtf(phillips(kx, ky, windDir, windSpeed, A, dirDepend));

      if (kx == 0.0f && ky == 0.0f) {
        P = 0.0f;
      }

      // float Er = urand()*2.0f-1.0f;
      // float Ei = urand()*2.0f-1.0f;
      float Er = gauss();
      float Ei = gauss();

      float h0_re = Er * P * CUDART_SQRT_HALF_F;
      float h0_im = Ei * P * CUDART_SQRT_HALF_F;

      int i = y * spectrumW + x;
      h0[i].x = h0_re;
      h0[i].y = h0_im;
    }
  }
}


float urand() { return rand() / (float)RAND_MAX; }

// Generates Gaussian random number with mean 0 and standard deviation 1.
float gauss() {
  float u1 = urand();
  float u2 = urand();

  if (u1 < 1e-6f) {
    u1 = 1e-6f;
  }

  return sqrtf(-2 * logf(u1)) * cosf(2 * CUDART_PI_F * u2);
}


// Phillips spectrum
// (Kx, Ky) - normalized wave vector
// Vdir - wind angle in radians
// V - wind speed
// A - constant
float phillips(float Kx, float Ky, float Vdir, float V, float A,
               float dir_depend) {
  float k_squared = Kx * Kx + Ky * Ky;

  if (k_squared == 0.0f) {
    return 0.0f;
  }

  // largest possible wave from constant wind of velocity v
  float L = V * V / g;

  float k_x = Kx / sqrtf(k_squared);
  float k_y = Ky / sqrtf(k_squared);
  float w_dot_k = k_x * cosf(Vdir) + k_y * sinf(Vdir);

  float phillips = A * expf(-1.0f / (k_squared * L * L)) /
                   (k_squared * k_squared) * w_dot_k * w_dot_k;

  // filter out waves moving opposite to wind
  if (w_dot_k < 0.0f) {
    phillips *= dir_depend;
  }

  // damp out waves with very small length w << l
  // float w = L / 10000;
  // phillips *= expf(-k_squared * w * w);

  return phillips;
}


// create index buffer for rendering quad mesh
void createMeshIndexBuffer(GLuint *id, int w, int h) {
  int size = ((w * 2) + 2) * (h - 1) * sizeof(GLuint);

  // create index buffer
  glGenBuffers(1, id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);

  // fill with indices for rendering mesh as triangle strips
  GLuint *indices =
      (GLuint *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

  if (!indices) {
    return;
  }

  for (int y = 0; y < h - 1; y++) {
    for (int x = 0; x < w; x++) {
      *indices++ = y * w + x;
      *indices++ = (y + 1) * w + x;
    }

    // start new strip with degenerate triangle
    *indices++ = (y + 1) * w + (w - 1);
    *indices++ = (y + 1) * w;
  }

  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// create fixed vertex buffer to store mesh vertices
void createMeshPositionVBO(GLuint *id, int w, int h) {

    void createVBO(GLuint *vbo, int size);
  createVBO(id, w * h * 4 * sizeof(float));

  glBindBuffer(GL_ARRAY_BUFFER, *id);
  float *pos = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  if (!pos) {
    return;
  }

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      float u = x / (float)(w - 1);
      float v = y / (float)(h - 1);
      *pos++ = u * 2.0f - 1.0f;
      *pos++ = 0.0f;
      *pos++ = v * 2.0f - 1.0f;
      *pos++ = 1.0f;
    }
  }

  glUnmapBuffer(GL_ARRAY_BUFFER);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}


////////////////////////////////////////////////////////////////////////////////
//! Create VBO
////////////////////////////////////////////////////////////////////////////////
void createVBO(GLuint *vbo, int size) {
  // create buffer object
  glGenBuffers(1, vbo);
  glBindBuffer(GL_ARRAY_BUFFER, *vbo);
  glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

}


void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1; //to avoid 'devide by zero' in future code

	glViewport(0, 0, GLsizei(width), GLsizei(height));
	
	perspectiveProjectionMatrix = vmath::perspective(
		45.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		100.0f
	);
}

void display(void)
{
	//function declaration
	void sinWave(unsigned int, unsigned int, float);
	void uninitialize();
	void  runCuda();

	static float angleCube = 1.0;


	runCuda();

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the shader program object
	glUseProgram(shaderProgramObject);

	// render from the vbo
	glBindBuffer(GL_ARRAY_BUFFER, posVertexBuffer);
	glVertexPointer(4, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, heightVertexBuffer);
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(1, GL_FLOAT, 0, 0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, slopeVertexBuffer);
	glClientActiveTexture(GL_TEXTURE1);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	

	//transformation
    mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();


	viewMatrix = vmath::lookat(cameraPosition, cameraPosition + cameraFront, cameraUp);


	angleCube += 0.1;
	if (angleCube > 360.0)
		angleCube = 1;

	mat4 translationMatrix = vmath::translate(0.0f, 0.0f, 0.0f);
	modelMatrix = translationMatrix;

	//send the above transfomation matrix to 
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    glUniform1f(heightScaleUniform, 0.5f);
	glUniform1f(chopinessUniform, 1.0f);
	glUniform2f(sizeUniform, (float)meshSize, (float)meshSize);

    glUniform4f(deepColorUniform, 0.0f, 0.1f, 0.4f, 1.0f);
    glUniform4f(shallowColorUniform, 0.1f, 0.3f, 0.3f, 1.0f);
    glUniform4f(skyColorUniform, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform3f(lightDirUniform, 0.0f, 1.0f, 0.0f); 

	//Bind VAO
	//glBindVertexArray(vao);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    //glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);
    glPolygonMode(GL_FRONT_AND_BACK,  GL_FILL);
    glDrawElements(GL_TRIANGLE_STRIP, ((meshSize * 2) + 2) * (meshSize - 1),
                   GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//Unused the shader program object
	glUseProgram(0);

	animationTime = animationTime + 0.01f;

	SwapBuffers(ghdc);
}


void update(void)
{
	animTime +=0.01;
}

void uninitialize(void)
{
	//function declarations
	void ToggleFullScreen(void);
	
	//code
	if (gbFullScreen)
	{
		ToggleFullScreen();
	}

	//deletion of vbo
	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}

	//deletion of vao
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	//Shader uninitialization
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
		
		GLsizei numAttachedShaders;

		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);

		GLuint* shaderObjects = NULL;

		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
		
		glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}

		if (shaderObjects)
			free(shaderObjects);

		glUseProgram(0);

		glDeleteProgram(shaderProgramObject);

		shaderProgramObject = 0;
	}
	if (wglGetCurrentContext()) //TODO
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
		fprintf(gpFile, "Log File is successfully closed\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

void printGLInfo(void)
{
	//local variable declarations
	GLint numExtensions=0;


	//code
	fprintf(gpFile, "OpenGL Vendor				: %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderere			: %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version				: %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version:				: %s \n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	fprintf(gpFile, "No of supported Extensions : %d\n", numExtensions);

	for (int i = 0; i < numExtensions; i++)
	{
		fprintf(gpFile, " %s \n", glGetStringi(GL_EXTENSIONS,i));
	}

	fprintf(gpFile, "\n");
}

