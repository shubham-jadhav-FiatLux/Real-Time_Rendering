// header files

#include<windows.h>
#include<stdio.h> // for file io functions
#include<stdlib.h> // for exit function
#include"OGL.h"  // but "" ne include karavi own header file


// OpenGL HEADER file
#include<GL/glew.h> //this must be before include.h 
#include<GL/gl.h>
#include"vmath.h"

using namespace vmath;

// OpenGL libraries (import)
#pragma comment(lib,"glew32.lib") //pp
#pragma comment(lib,"OpenGL32.lib")


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// globle function declaration

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Globle variable declaration

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
BOOL gbFullscreen = FALSE;
FILE* gpFile = NULL;
BOOL gbActiveWindow = FALSE;


//pp global variables
GLuint shaderProgramObject;
enum {

	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXTURE0
};

GLuint vao;
GLuint vbo;
GLuint vbo_texcoord;
GLuint texture_grass, texture_flowmap;
GLuint grassVerticesSize;

GLuint mvpMatrixUniform;
GLuint textureSamplerUniform, textureflowmapUniform;

mat4 perspectiveProjectionMatrix;  // perspective change

// Entry POINT function

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {

	//function declaration
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);

	// variable declaration
	int aiScreenWidth, aiScreenHeight;
	WNDCLASSEX  wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	BOOL bDone = FALSE;
	int iRateVal = 0;
	// code

	if (fopen_s(&gpFile, "log.txt", "w") != 0) {

		MessageBox(NULL, TEXT("Creation of log filefaild. Exitting...."), TEXT("File I/O Error"), MB_OK);

		exit(0);
	}
	else {
		fprintf(gpFile, "Log File is Successfully created.\n");
	}

	// initialization of wndclassEx structure
	
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW |CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;

	// four handal ahet

	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));


	// Register wndclassEx

	RegisterClassEx(&wndclass);

	// get the screen width and height

	aiScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	aiScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Create the window

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,szAppName,
		TEXT("Dnyaneshwari Balaji Mundhe"),
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE,
		(aiScreenWidth /2 - WINDOW_WIDTH /2),
		(aiScreenHeight /2 - WINDOW_HEIGHT /2),
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	// initialize
	iRateVal = initialize();
	if (iRateVal == -1) {

		fprintf(gpFile, "Choose pixel format faild.\n");
		uninitialize();
	}
	if (iRateVal == -2) {

		fprintf(gpFile, "Set pixel format faild.\n");
		uninitialize();
	}
	if (iRateVal == -3) {

		fprintf(gpFile, "Create OpenGl context faild.\n");
		uninitialize();
	}
	if (iRateVal == -4) {

		fprintf(gpFile, "Making current OpenGl context faild.\n");
		uninitialize();
	}
	/* {
		fprintf(gpFile, "Successfull.\n");

	}
	*/
	if (iRateVal == -5) {

		fprintf(gpFile, "Making current OpenGl context faild.\n");
		uninitialize();
	}

	// show window

	ShowWindow(hwnd, iCmdShow);

	// foregrounding and focusing 

	SetForegroundWindow(hwnd); // both will do good
	SetFocus(hwnd);

	//  game loop
	while (bDone == FALSE) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

			if (msg.message == WM_QUIT) {

				bDone = TRUE;
			}
			else {

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

		}
		else {

			if (gbActiveWindow == TRUE) {

				// Render the seen

				display();

				// update the seen

				update();

			}

		}

	}
	uninitialize();
	return((int)msg.wParam);
}

// callback function

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {


	// function declarations

	void ToggleFullscreen(void);
	void resize(int,int);
	//void uninitialize(void);

	// code
	switch (iMsg) {
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;
	case WM_ERASEBKGND:	
		break; // as this is retaind mode graphics wmPaint to paint (still valid)
		//return(0);
	case WM_CHAR:
		switch (wParam) {
		case 'F':
		case 'f':
			ToggleFullscreen();
			break;
		
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case 27: //esc key
			DestroyWindow(hwnd); // 
			break;
		default:
			break;

		}
		break;

	case WM_SIZE:
		
		resize(LOWORD(lParam),HIWORD(lParam));
		break;

	case WM_CLOSE:
		
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		// uninitialize();
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));

}

void ToggleFullscreen(void) {

	// variable declaration
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;


	// code 

	wp.length = sizeof(WINDOWPLACEMENT);

	if (gbFullscreen == FALSE) {

		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW) {

			mi.cbSize = sizeof(MONITORINFO);
			if (GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) {

				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			ShowCursor(FALSE);
			gbFullscreen = TRUE;
		}
	}
	else {
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullscreen = FALSE;


	}


}

int initialize(void) {

	// function declarations
	void resize(int, int);
	void printGLInfo(void);
	void uninitialize(void);
	BOOL loadGLTexture(GLuint*, TCHAR[]);

	// variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	GLint status;
	GLint infoLogLength;
	char* log = NULL;


	// code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	
	// initialization of PIXELFORMATDESCRIPTOR structure
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;

	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32; // 24 also can be done  Depthchange
	// Get dc
	ghdc = GetDC(ghwnd);

	// choose pixel format

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0) {

		return(-1);
	}
	// Set the Choosen pixel format 
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) {

		return(-2);
	}

	// create opengl rendering contex

	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL) {

		return(-3);
	}

	//	make the rendering Current context
	if (wglMakeCurrent(ghdc, ghrc) == FALSE) {     // 2nd api

		return(-4);
	}
	if (glewInit() != GLEW_OK) {

		return(-5);
	}
	//printglinfo

	//printGLInfo();

	// vertex shader

	const GLchar* vertexShaderSourceCode = 
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"void main(void)" \
		"{" \
		"gl_Position = a_position;" \
		"}"; 

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
	//glew Initialisation
	glCompileShader(vertexShaderObject);
	
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {

		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH,&infoLogLength);
		if (infoLogLength > 0) {
			
			log = (char*)malloc(infoLogLength);
			if (log!= NULL) {

				GLsizei written;
		
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);
				
				fprintf(gpFile, "vertex shader compilation log : %s\n", log);
				fflush(gpFile);
				
				
				free(log);
				//cLog =NULL;
				uninitialize();

			}
		}
	}
	
	
	//geometryShader
	
	const GLchar* geometryShaderSourceCode =
	"#version 460 core" \
	"\n" \
	"layout(points) in;" \
	"layout(triangle_strip, max_vertices = 36) out;" \
	"uniform mat4 u_mvpMatrix;" \
	"out vec2 texcoord_out;" \
	"uniform sampler2D u_wind;" \
	"const float c_min_size = 0.4f;" \
	"const float PI = 3.141592653589793;" \
	"float grass_size;" \
	"float random(vec2 st);" \
	"float noise(in vec2 st);" \
	"float fbm(in vec2 _st); " \
	"void createQuad(vec3 base_position, mat4 crossmodel) {" \
	"vec4 vertexPosition[4];" \
	"vertexPosition[0] = vec4(-0.25, 0.0, 0.0, 0.0);" \
	"vertexPosition[1] = vec4(0.25, 0.0, 0.0, 0.0);" \
	"vertexPosition[2] = vec4(-0.25, 0.5, 0.0, 0.0);" \
	"vertexPosition[3] = vec4(0.25, 0.5, 0.0, 0.0);" \
	"vec2 textCoords[4];" \
	"textCoords[0] = vec2(0.0, 0.0);" \
	"textCoords[1] = vec2(1.0, 0.0);" \
	"textCoords[2] = vec2(0.0, 1.0);" \
	"textCoords[3] = vec2(1.0, 1.0);" \
	"for (int i = 0; i < 4; i++) {" \
	"	gl_Position = u_mvpMatrix*(gl_in[0].gl_Position + crossmodel * (vertexPosition[i] * grass_size));" \
	"	texcoord_out = textCoords[i];" \
	"	EmitVertex();" \
	"}" \
	"EndPrimitive();" \
	"}" \

	"void createGrass(int numberQuads) {" \
	"mat4 model0, model45, modelm45;" \
	"model0 = mat4(1.0f);" \
	"createQuad(gl_in[0].gl_Position.xyz, model0);" \
	"}" \
	
	"float random(vec2 st) {" \
	"return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);" \
	"}" \

	"void main(void)" \
	"{" \
	"grass_size = random(gl_in[0].gl_Position.xz) * (1.0f - c_min_size) + c_min_size;" \
	"createGrass(1);" \
	"}";

	

	GLuint geometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShaderObject, 1, (const GLchar**)&geometryShaderSourceCode, NULL);
	//glew Initialisation
	
	
	status=0;
	infoLogLength=0;
	log = NULL;
	
	glCompileShader(geometryShaderObject);
	glGetShaderiv(geometryShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {

		glGetShaderiv(geometryShaderObject, GL_INFO_LOG_LENGTH,&infoLogLength);
		if (infoLogLength > 0) {
			
			log = (char*)malloc(infoLogLength);
			if (log!= NULL) {

				GLsizei written;
		
				glGetShaderInfoLog(geometryShaderObject, infoLogLength, &written, log);
				
				fprintf(gpFile, " geometry shader compilation log : %s\n", log);
				fflush(gpFile);
				
				
				free(log);
				//cLog =NULL;
				uninitialize();

			}
		}
	}
	
	

	// fragment shader code
	const GLchar* fragmentShaderSourceCode = 
		"#version 460 core" \
		"\n" \
		"in vec2 texcoord_out;" \
		"uniform sampler2D u_textureSampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec4 color = texture(u_textureSampler, texcoord_out);" \
		"FragColor = color; " \
		"}"; 
	
	// creating fragment shader object
	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(fragmentShaderObject);
	

	status = 0;
	infoLogLength = 0;
	log = NULL;
	
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	
	if (status == GL_FALSE) {

		glGetShaderiv(fragmentShaderObject,  GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {

			log = (char*)malloc(infoLogLength);
			if (log != NULL) {

				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);
				
				fprintf(gpFile, "fragment shader compilation log : %s\n", log);
				fflush(gpFile);
				
				free(log);
				//cLog = NULL;
				uninitialize();

			}
		}
	}
	//sheder program object
	shaderProgramObject = glCreateProgram(); //
	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, geometryShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	//pre linking binding
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "a_position");//andhar
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXTURE0, "a_texcoord");// bind kela

	
	glLinkProgram(shaderProgramObject);
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix"); // andhar
	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_textureSampler"); // in fragment shader
	textureflowmapUniform = glGetUniformLocation(shaderProgramObject, "u_wind");
	
	
	status = 0;
	infoLogLength = 0;
	log = NULL;
	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);

	if (status == GL_FALSE) {

		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {

			log = (char*)malloc(infoLogLength);
			if (log != NULL) {

				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, log);	
				fprintf(gpFile, "shader Program Link log : %s\n", log);
				fflush(gpFile);
				
				free(log);
				//cLog=NULL;
				uninitialize();

			}
		}
	}
	
	
	fflush(gpFile);
	// DECLARATION OF VRTEX Data ARRAYS
	GLfloat Vertices[40 * 40 * 3];
	int i = 0;
	for (float x = -2.0f; x < 2.0f; x += 0.1f)
	{
		for (float z = -2.0f; z < 2.0f; z += 0.1f)
		{
			int randNumberX = rand() % 10 + 1;
			int randNumberZ = rand() % 10 + 1;
			Vertices[i++] = x + (float)randNumberX / 50.0f;
			Vertices[i++] = 0;
			Vertices[i++] = z + (float)randNumberZ / 50.0f;
		}
		fprintf(gpFile, "Current x iteration: %f\n", x);
		fflush(gpFile);
	}

	grassVerticesSize = sizeof(Vertices)/sizeof(Vertices[0]);
	fprintf(gpFile, "Number of vertices: %d\n", grassVerticesSize);
	//three input ahet
	// vao and vbo related code

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//recording 
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// here starts open gl code
	//depthrelated changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
//	glShadeModel(GL_SMOOTH);   // pp madhe nii
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//clear the screen using blue color

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	if (loadGLTexture(&texture_grass, MAKEINTRESOURCE(IDBITMAP_GRASS)) == FALSE) {
		fprintf(gpFile, "loadGLTexture failed\n");
		uninitialize();
		return(-5);
	}

	if (loadGLTexture(&texture_flowmap, MAKEINTRESOURCE(IDBITMAP_FLOWMAP)) == FALSE) {
		fprintf(gpFile, "loadGLTexture failed\n");
		uninitialize();
		return(-6);
	}
	
	perspectiveProjectionMatrix = mat4::identity();
	resize(WINDOW_WIDTH, WINDOW_HEIGHT);
	return(0);
}

BOOL loadGLTexture(GLuint* texture, TCHAR imageResourceId[]) {

	// vaiable 
	HBITMAP hBitMap = NULL;
	BITMAP bmp;
	BOOL bResult = FALSE;

	//code
	hBitMap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hBitMap) {

		bResult = TRUE;

		GetObject(hBitMap, sizeof(BITMAP), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // for better performance
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//create the texture

		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);   //
		// unbind
		glBindTexture(GL_TEXTURE_2D, 0);
		DeleteObject(hBitMap);
	}

	return bResult;


}

void printGLInfo(void) {

	// local variable declarations
	GLint numExtensions = 0.0f;

	//code
	fprintf(gpFile,"OpenGL Vendor: %s\n", glGetString(GL_VENDOR));// GRAPHIC CARD company
	fprintf(gpFile, "OpenGL Render: %s\n", glGetString(GL_RENDERER));//graphic card cha number
	fprintf(gpFile, "OpenGL Version: %s\n",glGetString(GL_VERSION));//version/driver
	fprintf(gpFile, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION)); // SHADING_VERSION
		
	glGetIntegerv(GL_NUM_EXTENSIONS,&numExtensions);
	fprintf(gpFile, "NO of supported extention: %d\n", numExtensions);


	for (int i = 0; i < numExtensions; i++) {

		fprintf(gpFile,"%s\n",glGetStringi(GL_EXTENSIONS,i));

		
	}
	




}

void resize(int width, int height) {

	// code
	if (height == 0) {

		height = 1;  // to avoid divide by 0 elligal
	}
	glViewport(0, 0, width, height);
	//glMatrixMode(GL_PROJECTION); // projection see
	//glLoadIdentity();
	
	perspectiveProjectionMatrix = vmath::perspective(45.0f,
			(GLfloat)width / (GLfloat)height,
			0.1f, 100.0f);
	
	
}

void display() {

	// code
	glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT); //Depthkadhun takal


	//use sheder program object

	glUseProgram(shaderProgramObject);  // un
	
    //transformation
	mat4 translationMatrix = mat4::identity();
    mat4 modelViewMatrix = mat4::identity();
	mat4 modelVeiwProjectionMtrix = mat4::identity();
	
	// perspective change
	translationMatrix = vmath::translate(0.0f,0.0f,-10.0f); // glTranslate is replaced by this line
	modelViewMatrix = translationMatrix;
	modelVeiwProjectionMtrix = perspectiveProjectionMatrix * modelViewMatrix;
	glPointSize(5.0f);
	
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelVeiwProjectionMtrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_grass); // texture
	glUniform1i(textureSamplerUniform, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_flowmap); // texture
	glUniform1i(textureflowmapUniform,1);

	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, grassVerticesSize);
	glBindVertexArray(0);

	glUseProgram(0); //unused program
	SwapBuffers(ghdc);

}

void update(void) {

	// code

}

void uninitialize(void) {

	// function declaration
	void ToggleFullscreen(void);

	GLsizei numAttachedShaders;
	GLuint* shaderObjects = NULL;


	// code

	if (gbFullscreen) {

		ToggleFullscreen();
	}

	if (vbo) {
		
		glDeleteBuffers(1, &vbo);
		vbo = 0;

	}
	
	if (vao) {
		
		glDeleteVertexArrays(1,&vao);
		vao = 0;

	}
	// sheder uninitialization
	if (shaderProgramObject) {
		
		glUseProgram(shaderProgramObject);
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
		
		//fill it attached shader objects
		glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

			for (GLsizei i = 0; i < numAttachedShaders; i++) {
				glDetachShader(shaderProgramObject, shaderObjects[i]);
				glDeleteShader(shaderObjects[i]);
				
				shaderObjects[i] = 0;
				
			}
			free(shaderObjects);
			shaderObjects = NULL;

			glUseProgram(0);
			glDeleteProgram(shaderProgramObject);
			shaderProgramObject = 0;
	}

	if (wglGetCurrentContext() == ghrc) {

		wglMakeCurrent(NULL, NULL);

	}
	if (ghrc) {

		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc) {

		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (ghwnd) {

		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	if (gpFile) {

		fprintf(gpFile, "Log File is successfully closed.\n");
		fprintf(gpFile, "What is erase.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	

}

