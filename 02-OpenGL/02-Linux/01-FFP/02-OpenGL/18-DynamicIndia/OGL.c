// Standard Header Files
#include <stdio.h>			// For Standard I/O
#include <stdlib.h>			// For exit()
#include <memory.h>			// For memset()
//#include <Mmsystem.h>		// For Sound API

// OpenAL
#include <AL/al.h>
#include <AL/alc.h>

#define _USE_MATH_DEFINES 1
#include <math.h>			// For Trigonometric Functions

// X11 Headers
#include <X11/Xlib.h>
#include <X11/Xutil.h>		// XVisualInfo
#include <X11/XKBlib.h>		// For Keyboard

// OpenGL Header Files
#include <GL/gl.h>			// For OpenGL Functionality
#include <GL/glx.h>			// For Bridging API's
#include <GL/glu.h>			// For GLU Functions

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600


// Global Variables
Display *display = NULL;
XVisualInfo *visualInfo = NULL;	// Pointer
Colormap colormap;
Window window;
Bool fullscreen = False;
Bool activeWindow = False;
FILE* gpFile = NULL;

GLUquadric* quadric = NULL;

float oRed = 0.0f;
float oGreen = 0.0f;
float oBlue = 0.0f;
float w = 0.0f;
float gRed = 0.0f;
float gGreen = 0.0f;
float gBlue = 0.0f;

float xi = -1.0f;
float y_n = 1.0f;
float yi = -1.0f;
float xa = 1.0f;
float xaero = -2.5f;
float xaero_1 = -1.3f;
float yaero_1 = 1.0f;
float aeroAngle = -90.0f;
float aeroAngle2 = 90.0f;
float Angle = 180.0f;
/*
float r = yaero_1;
float a = xaero_1 + r;
float b = yaero_1;
*/
float r = (1.0f);
float a = ((-1.3f) + 1.0f);
float b = 1.0f;

float z = -1.0f;

int flagA = 0;
int flagN = 0;
int flagI = 0;
int flagD = 0;
int flagZ = 0;
int flagNegativeZ = 0;
int flagAeroplane = 0;
int flagAeroplane1 = 0;
int flagWhiteStrip = 0;
int flagOrangeStrip = 0;
int flagGreenStrip = 0;
int flagTricolor = 0;
int flagTemp = 0;

// OpenGL Related Variables
GLXContext glxContext;

// *********************
ALCdevice *device;
ALCcontext *context;

ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
ALuint source;
ALuint buffer;
ALsizei size, freq;
ALenum format;
ALvoid *data;
ALboolean loop = AL_FALSE;

//WaveInfo *wave;
char *bufferData;
int ret;


// Entry-Point Function
int main(void)
{
	// Function Declarations
	void toggleFullscreen(void);
	int initialize(void);
	void resize(int, int);
	void draw(void);
	void update(void);
	void uninitialize(void);
	void sound(void);

	// Local Variables
	int defaultScreen;
	int defaultDepth;
	XSetWindowAttributes windowAttributes;
	int styleMask;
	Atom wm_delete_window_atom;
	XEvent event;
	KeySym keySym;
	int screenWidth;
	int screenHeight;
	char keys[26];
	static int winWidth, winHeight;
	int iRetVal = 0; 
	
	static int frameBufferAttributes[] = 
		{ GLX_DOUBLEBUFFER, True,
		  GLX_RGBA,
		  GLX_RED_SIZE, 8,
		  GLX_GREEN_SIZE, 8,
		  GLX_BLUE_SIZE, 8,
		  GLX_ALPHA_SIZE, 8,
		  GLX_DEPTH_SIZE, 24,
		  None };				// Static For Style
	Bool bDone = False;

	// Code
	gpFile = fopen("Log.txt", "w");
	if (gpFile == NULL)
	{
		printf("Creation of Log file failed. Exiting ...\n");
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Successfully Opened.\n");
	}
	
	// Open The Display
	display = XOpenDisplay(NULL);
	if (display == NULL)
	{
		printf("ERROR: XOpenDisplay() Failed...\n");
		uninitialize();
		exit(1);
	}

	// Get Default Screen By Using Display
	defaultScreen = XDefaultScreen(display);
	// Get Default Depth
	defaultDepth = XDefaultDepth(display, defaultScreen);
	// Get XVisualInfo
	visualInfo = glXChooseVisual(display, defaultScreen, frameBufferAttributes);
	if (visualInfo == NULL)
	{
		printf("ERROR: glxChooseVisualInfo() Failed...\n");
		uninitialize();
		exit(1);
	}

	memset(&windowAttributes, 0, sizeof(XSetWindowAttributes));
	windowAttributes.border_pixel = 0;
	windowAttributes.background_pixel = XBlackPixel(display, defaultScreen);
	windowAttributes.background_pixmap = 0;
	windowAttributes.colormap = XCreateColormap(display,
						     RootWindow(display, visualInfo->screen),
						     visualInfo->visual,
						     AllocNone);
	windowAttributes.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask | FocusChangeMask;

	colormap = windowAttributes.colormap;

	styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;
	
	window = XCreateWindow(display,
						RootWindow(display, visualInfo->screen),
						0,
						0,
						WIN_WIDTH,
						WIN_HEIGHT,
						0,
						visualInfo->depth,
						InputOutput,
						visualInfo->visual,
						styleMask,
						&windowAttributes);
	if (!window)
	{
		printf("ERROR: XCreateWindow() Failed...\n");
		uninitialize();
		exit(1);
	}

	XStoreName(display, window, "SBJ:OpenGL");

	wm_delete_window_atom = XInternAtom(display, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(display, window, &wm_delete_window_atom, 1);

	XMapWindow(display, window);

	// Centering Of Window
	screenWidth = XWidthOfScreen(XScreenOfDisplay(display, defaultScreen));
	screenHeight = XHeightOfScreen(XScreenOfDisplay(display, defaultScreen));
	XMoveWindow(display, window, (screenWidth / 2 - WIN_WIDTH / 2), (screenHeight / 2 - WIN_HEIGHT / 2));

	// initialize
	iRetVal = initialize();
	if (iRetVal == -1)
	{
		fprintf(gpFile, "LoadGLTexture() FOR SMILEY TEXTURE FAILED.\n");
		uninitialize();
	}
	else
		fprintf(gpFile, "Succeed.\n");


	sound();


	// Message Loop
	while (bDone == False)
	{
		while (XPending(display))								
		{
			XNextEvent(display, &event);								
			switch (event.type)
			{
			case MapNotify:
				//sound();
				break;

			case FocusIn:
				activeWindow = True;
				break;
				
			case FocusOut:
				activeWindow = False;
				break;
				
			case KeyPress:
				keySym = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);			// like WM_KEYDOWN
				switch (keySym)
				{
				case XK_Escape:
					bDone = True;
					break;
				}

				XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);				// Like WM_CHAR
				switch (keys[0])
				{
				case 'F':
				case 'f':
					if (fullscreen == False)
					{
						toggleFullscreen();
						fullscreen = True;
					}

					else
					{
						toggleFullscreen();
						fullscreen = False;
					}
					break;

				default:
					break;
				}
				break;
				
			case ConfigureNotify:
				winWidth = event.xconfigure.width;
				winHeight = event.xconfigure.height;
				resize(winWidth, winHeight);
				break;

			case 33:
				bDone = True;
				break;

			default:
				break;
			}
		}
		
		if (activeWindow == True)
		{
			update();
		
			draw();
		}
	}
	
	uninitialize();

	return 0;
}

void toggleFullscreen(void)
{
	// Local Variables
	Atom wm_current_state_atom;
	Atom wm_state_fullscreen_atom;
	XEvent event;

	// Code
	wm_current_state_atom = XInternAtom(display, "_NET_WM_STATE", False);
	wm_state_fullscreen_atom = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

	memset(&event, 0, sizeof(XEvent));
	event.type = ClientMessage;
	event.xclient.window = window;
	event.xclient.message_type = wm_current_state_atom;
	event.xclient.format = 32;
	event.xclient.data.l[0] = fullscreen ? 0 : 1;
	event.xclient.data.l[1] = wm_state_fullscreen_atom;

	XSendEvent(display,																			// Like SendMessage And PostMessage
		       RootWindow(display, visualInfo->screen),
		       False,
		       SubstructureNotifyMask,
		       &event);
}

int initialize(void)
{
	// Function Declarations
	void resize(int, int);
	//void sound();
	
	// Code
	glxContext = glXCreateContext(display, visualInfo, NULL, True);
	glXMakeCurrent(display, window, glxContext);
	
	// Here Starts OpenGL Code
	// Clear The Screen Using Black Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Depth Related Changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Create Quadric
	quadric = gluNewQuadric();			// It Internally Calls Malloc, So Dont Use Inside Display()

	//sound();

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

void draw(void)
{
	// Function Declarations
	void I(float, float, float);
	void N(float, float, float);
	void D(float, float, float);
	void A(float, float, float);
	void Aeroplane(int);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glTranslatef(0.0f, 0.0f, -2.0f);
	
	if (flagZ == 0)
	{
		glTranslatef(0.0f, 0.0f, z);
	}
	else if (flagZ == 1 && flagNegativeZ == 0)
	{
		glTranslatef(0.0f, 0.0f, z);
		if (z <= -2.0f)
			flagAeroplane = 1;
	}
	else
	{
		glTranslatef(0.0f, 0.0f, z);
	}
	
	if (1)
	{
		glPushMatrix();
		if (xi <= 0.0f)
			glTranslatef(xi, 0.0f, 0.0f);
		else
			flagA = 1;
		I(-0.5f, 0.2f, 0.2f);
		glPopMatrix();
	}

	if (flagN == 1)
	{
		glPushMatrix();
		if (y_n >= 0.0f)
			glTranslatef(0.0f, y_n, 0.0f);
		else
			flagI = 1;
		N(-0.3f, 0.2f, 0.2f);
		glPopMatrix();
	}

	if (flagD == 1)
	{
		glColor3f(0.0f, 0.0f, 0.0f);
		if (oRed >= 0.9f)
			flagZ = 1;
			//flagAeroplane = 1;
		D(-0.1f, 0.2f, 0.2f);
	}

	if (flagI == 1)
	{
		glPushMatrix();
		if (yi <= 0.0f)
			glTranslatef(0.0f, yi, 0.0f);
		else
			flagD = 1;
		I(0.1f, 0.2f, 0.2f);
		glPopMatrix();
	}

	if (flagA == 1)
	{
		glPushMatrix();
		if (xa >= 0.0f)
			glTranslatef(xa, 0.0f, 0.0f);
		else
			flagN = 1;
		A(0.3f, 0.2f, 0.2f);
		glPopMatrix();
	}

	//flagAeroplane = 1;
	if (flagAeroplane == 1)
	{
		glPushMatrix();
		if (xaero <= 5.0f)
		{
			glTranslatef(xaero, 0.0f, 0.0f);
			if (xaero >= -0.999f)
				flagAeroplane1 = 1;
			if (xaero >= 0.6f)
				flagWhiteStrip = 1;
			if (xaero >= 2.0f)
				flagNegativeZ = 1;
			glScalef(0.4f, 0.4f, 0.4f);
			Aeroplane(2);
		}
		glPopMatrix();

		if (flagAeroplane1 == 1)
		{
			// Upper Plane
			glPushMatrix();
			glTranslatef(xaero_1, yaero_1, 0.0f);
			if (xaero_1 >= 0.7f)
				flagOrangeStrip = 1;
			glRotatef(aeroAngle, 0.0f, 0.0f, 1.0f);
			glScalef(0.4f, 0.4f, 0.4f);
			Aeroplane(1);
			glPopMatrix();

			
			// Lower Plane
			glPushMatrix();
			glTranslatef(xaero_1, -yaero_1, 0.0f);
			if (xaero_1 >= 0.7f)
				flagGreenStrip = 1;
			glRotatef(aeroAngle2, 0.0f, 0.0f, 1.0f);
			glScalef(0.4f, 0.4f, 0.4f);
			Aeroplane(3);
			glPopMatrix();
		}
	}

	glXSwapBuffers(display, window);
}

void update(void)
{
	// Code
	if (xi <= 0.0f)
		xi += 0.003f;
	if (y_n >= 0.0f && flagN == 1)
		y_n -= 0.0035f;

	if (flagD == 1)
	{
		if (oRed <= 1.0f)
			oRed += 0.0008f;
		if (oGreen < 0.56f)
			oGreen += 0.0005f;
		if (oBlue <= 0.11f)
			oBlue += 0.0001f;
		w = oRed;
		if (gRed < 0.313f)
			gRed += 0.00025f;
		if (gGreen < 0.62f)
			gGreen += 0.00045f;
		if (gBlue < 0.184f)
			gBlue += 0.00015f;
	}
	if (yi <= 0.0f && flagI == 1)
		yi += 0.002f;
	if (xa >= 0.0f && flagA ==1)
		xa -= 0.0025f;
	if (xaero <= 2.0f && flagAeroplane == 1)
		xaero += 0.0015f;
	//if (xaero > 0.7f && xaero <= 2.0f && flagAeroplane == 1)
		//xaero += 0.002f;

	if (flagAeroplane1 == 1)
	{
		if (Angle <= 270.0f)
		{
			xaero_1 = a + r * cos(Angle * M_PI / 180.0f);
			yaero_1 = b + r * sin(Angle * M_PI / 180.0f);
			Angle += 0.195f;
			aeroAngle += 0.195f;
			aeroAngle2 -= 0.195f;
		}
		else if (Angle >= 270.0f && xaero_1 <= 0.75f || Angle < 271.0f)
		{
			xaero_1 += 0.0015f;
			Angle = 271.0f;
			flagTemp = 1;
		}
		else if (flagTemp == 1 && Angle <= 400)
		{
			xaero_1 = (0.75f) + (r - 0.2f) * cos(Angle * M_PI / 180.0f);
			yaero_1 = (b - 0.2f) + (r - 0.2f) * sin(Angle * M_PI / 180.0f);
			Angle += 0.195f;
			aeroAngle += 0.185f;
			aeroAngle2 -= 0.185f;
		}
	}
	if (z >= -2.0f && flagZ == 1 && flagNegativeZ == 0)
		z -= 0.0035f;
	else if (z <= -1.0f && flagNegativeZ == 1)
		z += 0.003f;
}

void sound(void)
{
	
device = alcOpenDevice(NULL);

context = alcCreateContext(device, NULL);
if (!alcMakeContextCurrent(context))
{
fprintf(gpFile, "failed context\n");
}
alListener3f(AL_POSITION, 0, 0, 1.0f);
// check for errors
alListener3f(AL_VELOCITY, 0, 0, 0);
// check for errors
alListenerfv(AL_ORIENTATION, listenerOri);

alGenSources((ALuint)1, &source);
// check for errors

alSourcef(source, AL_PITCH, 1);
// check for errors
alSourcef(source, AL_GAIN, 1);
// check for errors
alSource3f(source, AL_POSITION, 0, 0, 0);
// check for errors
alSource3f(source, AL_VELOCITY, 0, 0, 0);
// check for errors
alSourcei(source, AL_LOOPING, AL_FALSE);

alutLoadWAVFile("dulhan_chali_pahen_chali.wav", &format, &data, &size, &freq, &loop);

alGenBuffers((ALuint)1, &buffer);

alBufferData(buffer, format, data, size, freq);

alSourcei(source, AL_BUFFER, buffer);
alSourcePlay(source);

}

void I(float x, float y, float width)
{
	// Code
	glBegin(GL_QUADS);

		// Upper Quad
		glColor3f(1.0f, 0.56f, 0.11f);								// Orange
		glVertex3f((x + width - 0.01), y, 0.0f);
		glVertex3f((x + 0.01f), y, 0.0f);
		glVertex3f((x + 0.01f), (y - 0.05f), 0.0f);
		glVertex3f((x + width - 0.01f), (y - 0.05f), 0.0f);


		// Middle Quad 1
		glVertex3f((x + 2 * width / 3 - 0.01f), (y - 0.05f), 0.0f);
		glVertex3f((x + width / 3 + 0.01f), (y - 0.05f), 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);								// White
		glVertex3f((x + width / 3 + 0.01f), 0.0f, 0.0f);
		glVertex3f((x + 2 * width / 3 - 0.01f), 0.0f, 0.0f);

		// Middle Quad 2
		glVertex3f((x + 2 * width / 3 - 0.01f), 0.0f, 0.0f);
		glVertex3f((x + width / 3 + 0.01f), 0.0f, 0.0f);
		glColor3f(0.313f, 0.62f, 0.184f);							// Green
		glVertex3f((x + width / 3 + 0.01f), (-y + 0.05f), 0.0f);
		glVertex3f((x + 2 * width / 3 - 0.01f), (-y + 0.05f), 0.0f);


		// Lower Quad
		glColor3f(0.313f, 0.62f, 0.184f);
		glVertex3f((x + width - 0.01), (-y + 0.05f), 0.0f);
		glVertex3f((x + 0.01f), (-y + 0.05f), 0.0f);
		glVertex3f((x + 0.01f), -y, 0.0f);
		glVertex3f((x + width - 0.01f), -y, 0.0f);

		glEnd();
}

void N(float x, float y, float width)
{
	// Code
	glBegin(GL_QUADS);
	
	// Left Quad 1
	glColor3f(1.0f, 0.56f, 0.11f);								// Orange
	glVertex3f((x + width / 3 - 0.01f), y, 0.0f);
	glVertex3f((x + 0.01f), y, 0.0f);
	glVertex3f((x + 0.01f), y - 0.05f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), y - 0.05f, 0.0f);
	

	glVertex3f((x + width / 3 - 0.01f), y - 0.05f, 0.0f);
	glVertex3f((x + 0.01f), y - 0.05f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);								// White
	glVertex3f((x + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);
	
	// Left Quad 2
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 0.01f), 0.0f, 0.0f);
	glColor3f(0.313f, 0.62f, 0.184f);							// Green
	glVertex3f((x + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), -y + 0.05f, 0.0f);
	
	glVertex3f((x + width / 3 - 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 0.01f), -y, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), -y, 0.0f);

	// Diagonal Quad 1
	glColor3f(1.0f, 0.56f, 0.11f);									// Orange
	glVertex3f((x + width / 3 - 0.005f), y, 0.0f);
	glVertex3f((x + width / 3 - 0.005f), (y - 0.1f), 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);									// White
	glVertex3f((x + width /2 - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width / 2 + 0.015f), 0.0f, 0.0f);

	// Diagonal Quad 2
	glVertex3f((x + width / 2 + 0.015f), 0.0f, 0.0f);
	glVertex3f((x + width / 2 - 0.01f), 0.0f, 0.0f);
	glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + 2 * width / 3 + 0.005f), -y, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.005f), (-y + 0.1f), 0.0f);

	
	// Right Quad 1
	glColor3f(1.0f, 0.56f, 0.11f);									// Orange
	glVertex3f((x + width - 0.01f), y, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), y, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), y - 0.05f, 0.0f);
	glVertex3f((x + width - 0.01f), y - 0.05f, 0.0f);

	glVertex3f((x + width - 0.01f), y - 0.05f, 0.0f);
	glVertex3f((x + 2 * width/3 + 0.01f), y - 0.05f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);									// White
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width - 0.01f), 0.0f, 0.0f);

	// Right Quad 2
	glVertex3f((x + width - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 2* width /3 + 0.01f), 0.0f, 0.0f);
	glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + 2 * width / 3 + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x +  width - 0.01f), -y + 0.05f, 0.0f);

	glVertex3f((x + width - 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), -y, 0.0f);
	glVertex3f((x + width - 0.01f), -y, 0.0f);
	
	glEnd();
}

void D(float x, float y, float width)
{
	// Code
	glBegin(GL_QUADS);

	// Left Quad 1
	glColor3f(oRed, oGreen, oBlue);
	//glColor3f(1.0f, 0.56f, 0.11f);								// Orange
	glVertex3f((x + width / 3 - 0.01f), (y - 0.05f), 0.0f);
	glVertex3f((x + 0.01f), (y - 0.05f), 0.0f);
	glColor3f(w, w, w);
	//glColor3f(1.0f, 1.0f, 1.0f);									// White
	glVertex3f((x + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);

	// Left Quad 2
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 0.01f), 0.0f, 0.0f);
	glColor3f(gRed, gGreen, gBlue);
	//glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + 0.01f), (-y + 0.05f), 0.0f);
	glVertex3f((x + width / 3 - 0.01f), (-y + 0.05f), 0.0f);


	// Right Quad 1
	glColor3f(oRed, oGreen, oBlue);
	//glColor3f(1.0f, 0.56f, 0.11f);								// Orange
	glVertex3f((x + width - 0.01f), (y-0.05f), 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), y, 0.0f);
	glColor3f(w, w, w);
	//glColor3f(1.0f, 1.0f, 1.0f);									// White
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width - 0.01f), 0.0f, 0.0f);

	// Right Quad 2
	glVertex3f((x + width - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);
	glColor3f(gRed, gGreen, gBlue);
	//glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + 2 * width / 3 + 0.01f), (-y + 0.05f), 0.0f);
	glVertex3f((x + width - 0.01f), (-y + 0.05f), 0.0f);


	// Upper Quad
	glColor3f(oRed, oGreen, oBlue);
	//glColor3f(1.0f, 0.56f, 0.11f);								// Orange
	glVertex3f((x + 2 * width/3 + 0.01), y, 0.0f);
	glVertex3f((x + 0.01f), y, 0.0f);
	glVertex3f((x + 0.01f), (y - 0.05f), 0.0f);
	glVertex3f((x + width - 0.01f), (y - 0.05f), 0.0f);


	// Lower Quad
	glColor3f(gRed, gGreen, gBlue);
	//glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + width - 0.01), (-y + 0.05f), 0.0f);
	glVertex3f((x + 0.01f), (-y + 0.05f), 0.0f);
	glVertex3f((x + 0.01f), -y, 0.0f);
	glVertex3f((x + width - 0.01f), -y, 0.0f);

	glEnd();
}

void A(float x, float y, float width)
{
	// Code
	glBegin(GL_QUADS);

	// Left Quad 1
	glColor3f(1.0f, 0.56f, 0.11f);									// Orange
	glVertex3f((x + width / 3 - 0.01f), (y - 0.05f), 0.0f);
	glVertex3f((x + 0.01f), (y - 0.05f), 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);									// White
	glVertex3f((x + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);

	// Left Quad 2
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 0.01f), 0.0f, 0.0f);
	glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + 0.01f), -y+0.05f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), -y + 0.05f, 0.0f);

	glVertex3f((x + width / 3 - 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 0.01f), -y, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), -y, 0.0f);


	// Right Quad 1
	glColor3f(1.0f, 0.56f, 0.11f);									// Orange
	glVertex3f((x + width - 0.01f), (y - 0.05f), 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), (y - 0.05f), 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);									// White
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width - 0.01f), 0.0f, 0.0f);

	// Right Quad 2
	glVertex3f((x + width - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);
	glColor3f(0.313f, 0.62f, 0.184f);								// Green
	glVertex3f((x + 2 * width / 3 + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + width - 0.01f), -y + 0.05f, 0.0f);

	glVertex3f((x + width - 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), -y + 0.05f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), -y, 0.0f);
	glVertex3f((x + width - 0.01f), -y, 0.0f);

	// Upper Quad
	glColor3f(1.0f, 0.56f, 0.11f);									// Orange
	glVertex3f((x + 2 * width / 3 + 0.01f), y, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), y, 0.0f);
	glVertex3f((x + 0.01f), (y - 0.05f), 0.0f);
	glVertex3f((x + width - 0.01f), (y - 0.05f), 0.0f);


	// Middle Quad 1
	glColor3f(0.0f, 0.0f, 0.0f);
	if (flagOrangeStrip == 1)
	{
		glColor3f(1.0f, 0.56f, 0.11f);								// Orange
	}
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.02f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), 0.02f, 0.0f);
		
	if (flagWhiteStrip == 1)
	{
		glColor3f(1.0f, 1.0f, 1.0f);								// White
	}
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);

	// Middle Quad 2
	glVertex3f((x + 2 * width / 3 + 0.01f), 0.0f, 0.0f);
	glVertex3f((x + width / 3 - 0.01f), 0.0f, 0.0f);
	
	glColor3f(0.0f, 0.0f, 0.0f);
	if (flagGreenStrip == 1)
	{
		glColor3f(0.313f, 0.62f, 0.184f);							// Green
	}
	glVertex3f((x + width / 3 - 0.01f), -0.02f, 0.0f);
	glVertex3f((x + 2 * width / 3 + 0.01f), -0.02f, 0.0f);
	
	glEnd();
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



	glBegin(GL_QUADS);
	if (number == 1)
	{
		glColor3f(1.0f, 0.56f, 0.11f);			// Orange
		glVertex3f(-0.71f, 0.05f, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);			// Black
		glVertex3f(-1.0f, 0.05f, 0.0f);
		glVertex3f(-1.0f, -0.05f, 0.0f);
		glColor3f(1.0f, 0.56f, 0.11f);			// Orange
		glVertex3f(-0.71f, -0.05f, 0.0f);
	}
	else if (number == 2)
	{
		if (xaero <= -0.3f || xaero > 0.75f)
		{
			flagTricolor = 0;
			glColor3f(1.0f, 1.0f, 1.0f);			// White
			glVertex3f(-0.71f, 0.05f, 0.0f);
			glColor3f(0.0f, 0.0f, 0.0f);			// Black
			glVertex3f(-1.0f, 0.05f, 0.0f);
			glVertex3f(-1.0f, -0.05f, 0.0f);
			glColor3f(1.0f, 1.0f, 1.0f);			// White
			glVertex3f(-0.71f, -0.05f, 0.0f);
		}
		else if ( xaero >= -0.3f && xaero <= 0.75f)
		{
			flagTricolor = 1;
		}
		
	}
	else if (number == 3)
	{
		if (flagTricolor == 0)
		{
			glColor3f(0.313f, 0.62f, 0.184f);		// Green
			glVertex3f(-0.71f, 0.05f, 0.0f);
			glColor3f(0.0f, 0.0f, 0.0f);			// Black
			glVertex3f(-1.0f, 0.05f, 0.0f);
			glVertex3f(-1.0f, -0.05f, 0.0f);
			glColor3f(0.313f, 0.62f, 0.184f);		// Green
			glVertex3f(-0.71f, -0.05f, 0.0f);
		}
		else
		{
			glColor3f(1.0f, 0.56f, 0.11f);			// Orange
			glVertex3f(-0.71f, 0.055f, 0.0f);
			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex3f(-1.0f, 0.055f, 0.0f);
			glVertex3f(-1.0f, 0.02f, 0.0f);
			//glColor3f(1.0f, 1.0f, 1.0f);			// White
			glColor3f(1.0f, 0.56f, 0.11f);		// Orange
			glVertex3f(-0.71f, 0.02f, 0.0f);

			
			glColor3f(1.0f, 1.0f, 1.0f);			// White
			glVertex3f(-0.71f, 0.02f, 0.0f);
			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex3f(-1.0f, 0.02f, 0.0f);
			glVertex3f(-1.0f, -0.02f, 0.0f);
			glColor3f(1.0f, 1.0f, 1.0f);			// White
			glVertex3f(-0.71f, -0.02f, 0.0f);
			

			glColor3f(0.313f, 0.62f, 0.184f);		// Green
			glVertex3f(-0.71f, -0.02f, 0.0f);
			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex3f(-1.0f, -0.02f, 0.0f);
			glVertex3f(-1.0f, -0.055f, 0.0f);
			glColor3f(0.313f, 0.62f, 0.184f);		// Green
			glVertex3f(-0.71f, -0.055f, 0.0f);
		}
	}
	glEnd();
}

void uninitialize(void)
{
	// Local Variable
	GLXContext currentContext;
	
	// Code
	currentContext = glXGetCurrentContext();
	if (currentContext && currentContext == glxContext)
	{
		glXMakeCurrent(display, 0, 0);
	}
	
	if (glxContext)
	{
		glXDestroyContext(display, glxContext);
		glxContext = NULL;
	}
	
	if (visualInfo)
	{
		free(visualInfo);
		visualInfo = NULL;
	}
	
	if (window)
	{
		XDestroyWindow(display, window);
	}

	if (colormap)
	{
		XFreeColormap(display, colormap);
	}

	if (display)
	{
		XCloseDisplay(display);
		display = NULL;
	}

	// OpenAL
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
	
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
