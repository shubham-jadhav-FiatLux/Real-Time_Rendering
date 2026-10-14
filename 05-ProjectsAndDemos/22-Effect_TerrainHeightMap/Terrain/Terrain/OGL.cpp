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
//terrain
#define     MAP_SIZE    100      //size Of our .RAW height Map (NEW )
#define     STEP_SIZE   16        //width and Height Of each Quad (NEW )
#define     HEIGHT_RATIO    1.5f  //ratio that the Y Is scaled according To The X And Z (NEW )
static int vertexCount = 0;

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
GLuint vbo_position;
GLuint vbo_color;

GLuint vao_quad;
GLuint vbo_quad_position;
GLuint mvpMatrixUniform;

mat4 perspectiveProjectionMatrix;  // perspective change

//terrain
BYTE g_HeightMap[MAP_SIZE*MAP_SIZE];   // Holds The Height Map Data ( NEW )
//float scaleValue = 0.15f;    // Scale Value For The Terrain ( NEW )
 BOOL        bRender = TRUE;
		
GLfloat colorTerrain[MAP_SIZE];
GLfloat positionTerrain[MAP_SIZE];	
int var=0;	
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
	case WM_LBUTTONDOWN:          
        
         bRender = !bRender;         
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
	void LoadRawFile(LPSTR , int , BYTE* );
	void RenderHeightMap(BYTE[]);
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

	printGLInfo();

	// vertex shader

	const GLchar* vertexShaderSourceCode = 
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		/*"in vec4 a_color;" \*/
		"uniform mat4 u_mvpMatrix;" \
		"out vec4 a_color_out;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvpMatrix * a_position;" \
		"a_color_out = vec4(1.0, 0.0, 0.0, 1.0);" \
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
	

	// fragment shader code
	const GLchar* fragmentShaderSourceCode = 
		"#version 460 core" \
		"\n" \
		"in vec4 a_color_out;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = a_color_out;" \
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
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	//pre linking binding
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "a_position");//andhar
	//glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_COLOR, "a_color");// bind kela
	glLinkProgram(shaderProgramObject);
	
	
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
	
	
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix"); // andhar
	
	// DECLARATION OF VRTEX Data ARRAYS
	GLfloat positionTerrain[MAP_SIZE*MAP_SIZE *12];

	static int vertexCount = 0;
	int indexCount = vertexCount;

	for (int X = 0; X < (MAP_SIZE - STEP_SIZE); X += STEP_SIZE)
	{
		for (int Y = 0; Y < (MAP_SIZE - STEP_SIZE); Y += STEP_SIZE)
		{
			positionTerrain[vertexCount++] = X;
			positionTerrain[vertexCount++] = 0.0f;
			//positionTerrain[vertexCount++] = Height(pHeightMap, X, Y);
			positionTerrain[vertexCount++] = Y;

			positionTerrain[vertexCount++] = X;
			positionTerrain[vertexCount++] = 0.0f;
			//positionTerrain[vertexCount++] = Height(pHeightMap, X, Y);
			positionTerrain[vertexCount++] = Y;

			positionTerrain[vertexCount++] = X;
			positionTerrain[vertexCount++] = 0.0f;
			//positionTerrain[vertexCount++] = Height(pHeightMap, X, Y);
			positionTerrain[vertexCount++] = Y;

			positionTerrain[vertexCount++] = X;
			positionTerrain[vertexCount++] = 0.0f;
			//positionTerrain[vertexCount++] = Height(pHeightMap, X, Y);
			positionTerrain[vertexCount++] = Y;
		}
	}
		


	//RenderHeightMap(g_HeightMap);

	// vao and vbo related code

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

		//recording 
		glGenBuffers(1, &vbo_position);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positionTerrain),positionTerrain, GL_STATIC_DRAW);
	
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	
		/*
		//vbo for color
		glGenBuffers(1, &vbo_color);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colorTerrain), colorTerrain, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind karaych vbo
		*/
	glBindVertexArray(0);

	/*
	glGenVertexArrays(1, &vao_quad);
	glBindVertexArray(vao_quad);
	
	//vbo for position
	//recording 
	glGenBuffers(1, &vbo_quad_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPosition), quadPosition, GL_STATIC_DRAW);
	
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // position unbind vbo cha
	
	//vbo for color
	glVertexAttrib3f(AMC_ATTRIBUTE_COLOR,0.0f,0.0f,1.0f); // single color the one line 
		
	glBindVertexArray(0);  // unbind vao cha
	*/

	
	// here starts open gl code
	//depthrelated changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glShadeModel(GL_SMOOTH);   // pp madhe nii
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//clear the screen using blue color

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	LoadRawFile("Data/Island.raw", MAP_SIZE * MAP_SIZE, g_HeightMap);  // ( NEW )
  
	perspectiveProjectionMatrix = mat4::identity();
	resize(WINDOW_WIDTH, WINDOW_HEIGHT);
	return(0);
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
			0.1f, 500.0f);
	
	
}

void LoadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap){
   
   FILE *pFile = NULL;
 
    // Open The File In Read / Binary Mode.
    pFile = fopen( strName, "rb" );
 
    // Check To See If We Found The File And Could Open It
    if ( pFile == NULL ){
        // Display Error Message And Stop The Function
        MessageBox(NULL, "Can't Find The Height Map!", "Error", MB_OK);
        return;
    }
	 // Here We Load The .RAW File Into Our pHeightMap Data Array
    // We Are Only Reading In '1', And The Size Is (Width * Height)
    fread( pHeightMap, 1, nSize, pFile );
 
    // After We Read The Data, It's A Good Idea To Check If Everything Read Fine
    int result = ferror( pFile );
 
    // Check If We Received An Error
    if (result)
    {
        MessageBox(NULL, "Failed To Get Data!", "Error", MB_OK);
    }
 
    // Close The File
    fclose(pFile);
}


int Height(BYTE *pHeightMap, int X, int Y) {         // this returns The Height From A Height Map Index

    int x = X % MAP_SIZE;                   // error Check Our x Value
    int y = Y % MAP_SIZE;                   // error Check Our y Value
 
    if(!pHeightMap) return 0;               // make Sure Our Data Is Valid	
	return pHeightMap[x + (y * MAP_SIZE)];          // Index Into Our Height Array And Return The Height
}

/*
void RenderHeightMap(BYTE pHeightMap[]){             // this Renders The Height Map As Quads


	//void SetVertexColor(BYTE* , int , int ); 

    int X = 0, Y = 0;                   // Create Some Variables To Walk The Array With.
    int x, y, z;                        // Create Some Variables For Readability
 
    if(!pHeightMap) return;  	// Make Sure Our Height Data Is Valid
	
	
	
	for ( X = 0; X < (MAP_SIZE-STEP_SIZE); X += STEP_SIZE )
    for ( Y = 0; Y < (MAP_SIZE-STEP_SIZE); Y += STEP_SIZE )
    {
        // Get The (X, Y, Z) Value For The Bottom Left Vertex
        x = X;                         
        y = Height(pHeightMap, X, Y ); 
        z = Y;                         
 
        // Set The Color Value Of The Current Vertex
        SetVertexColor(pHeightMap, x, z);
 
       // glVertex3i(x, y, z);            // Send This Vertex To OpenGL To Be Rendered
		positionTerrain[var++] = x;
		positionTerrain[var++] = y;
		positionTerrain[var++] = z;
        // Get The (X, Y, Z) Value For The Top Left Vertex
        x = X;                                     
        y = Height(pHeightMap, X, Y + STEP_SIZE ); 
        z = Y + STEP_SIZE ;                        
         
        // Set The Color Value Of The Current Vertex
        SetVertexColor(pHeightMap, x, z);
 
       // glVertex3i(x, y, z);            // Send This Vertex To OpenGL To Be Rendered
		positionTerrain[var++] = x;
		positionTerrain[var++] = y;
		positionTerrain[var++] = z;
        // Get The (X, Y, Z) Value For The Top Right Vertex
        x = X + STEP_SIZE;
        y = Height(pHeightMap, X + STEP_SIZE, Y + STEP_SIZE );
        z = Y + STEP_SIZE ;
 
        // Set The Color Value Of The Current Vertex
        SetVertexColor(pHeightMap, x, z);
         
      //  glVertex3i(x, y, z);            // Send This Vertex To OpenGL To Be Rendered
		positionTerrain[var++] = x;
		positionTerrain[var++] = y;
		positionTerrain[var++] = z;
        // Get The (X, Y, Z) Value For The bottom Right Vertex
        x = X + STEP_SIZE;
        y = Height(pHeightMap, X + STEP_SIZE, Y );
        z = Y;
 
        // Set The Color Value Of The Current Vertex
        SetVertexColor(pHeightMap, x, z);
 
        //glVertex3i(x, y, z);            // Send This Vertex To OpenGL To Be Rendered
		positionTerrain[var++] = x;
		positionTerrain[var++] = y;
		positionTerrain[var++] = z;
   }
	
}
*/

/*
void SetVertexColor(BYTE *pHeightMap, int x, int y){     // this Sets The Color Value For A Particular Index
                              // depending On The Height Index
    if(!pHeightMap) return;                 // Make sure our Height Data Is Valid
 
    float fColor = -0.15f + (Height(pHeightMap, x, y ) / 256.0f);
 
    // Assign This Blue Shade To The Current Vertex
    //glColor3f(0.0f, 0.0f, fColor );
	colorTerrain[var++] = 0.0f;
	colorTerrain[var++] = 0.0f;
	colorTerrain[var++] = fColor;
}
*/

void display() {

	// code
	glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT); //Depthkadhun takal


	//use sheder program object
	//gluLookAt(212, 60, 194,  186, 55, 171,  0, 1, 0);
	glUseProgram(shaderProgramObject);  // un
	
    //transformation
	mat4 translationMatrix = mat4::identity();
    mat4 modelViewMatrix = mat4::identity();
	mat4 modelVeiwProjectionMtrix = mat4::identity();
	
	// perspective change
	translationMatrix = vmath::translate(0.0f,0.0f,-50.0f); // glTranslate is replaced by this line
	modelViewMatrix = translationMatrix;
	modelVeiwProjectionMtrix = perspectiveProjectionMatrix * modelViewMatrix;
	
	
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelVeiwProjectionMtrix);
	
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
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

	if (vbo_position) {
		
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;

	}
	if (vbo_color) {
		
		glDeleteBuffers(1, &vbo_color);
		vbo_color = 0;

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
		fclose(gpFile);
		gpFile = NULL;
	}

	

}

