// Standard Header Files
#include <stdio.h>			// For Standard I/O
#include <stdlib.h>			// For exit()
#include <memory.h>			// For memset()

// X11 Headers
#include <X11/Xlib.h>
#include <X11/Xutil.h>		// XVisualInfo
#include <X11/XKBlib.h>		// For Keyboard

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global Variables
Display *display = NULL;
XVisualInfo visualInfo;
Colormap colormap;
Window window;

// Entry-Point Function
int main(void)
{
	// Function Declarations
	void uninitialise(void);

	// Local Variables
	int defaultScreen;
	int defaultDepth;
	Status status;
	XSetWindowAttributes windowAttributes;
	int styleMask;
	Atom wm_delete_window_atom;
	XEvent event;
	KeySym keySym;
	int screenWidth;
	int screenHeight;

	// Code
	// Open The Display
	display = XOpenDisplay(NULL);
	if (display == NULL)
	{
		printf("ERROR: XOpenDisplay() Failed...\n");
		uninitialise();
		exit(1);
	}

	// Get Default Screen By Using Display
	defaultScreen = XDefaultScreen(display);
	// Get Default Depth
	defaultDepth = XDefaultDepth(display, defaultScreen);
	// Get XVisualInfo
	status = XMatchVisualInfo(display, defaultScreen, defaultDepth, TrueColor, &visualInfo);
	if (status == 0)
	{
		printf("ERROR: XMatchVisualInfo() Failed...\n");
		uninitialise();
		exit(1);
	}

	memset(&windowAttributes, 0, sizeof(XSetWindowAttributes));
	windowAttributes.border_pixel = 0;
	windowAttributes.background_pixel = XBlackPixel(display, defaultScreen);
	windowAttributes.background_pixmap = 0;
	windowAttributes.colormap = XCreateColormap(display,
												RootWindow(display, visualInfo.screen),
												visualInfo.visual,
												AllocNone);
	windowAttributes.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | FocusChangeMask;

	colormap = windowAttributes.colormap;

	styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;
	
	window = XCreateWindow(display,
						RootWindow(display, visualInfo.screen),
						0,
						0,
						WIN_WIDTH,
						WIN_HEIGHT,
						0,
						visualInfo.depth,
						InputOutput,
						visualInfo.visual,
						styleMask,
						&windowAttributes);
	if (!window)
	{
		printf("ERROR: XCreateWindow() Failed...\n");
		uninitialise();
		exit(1);
	}

	XStoreName(display, window, "SBJ:Window");

	wm_delete_window_atom = XInternAtom(display, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(display, window, &wm_delete_window_atom, 1);

	XMapWindow(display, window);

	// Centering Of Window
	screenWidth = XWidthOfScreen(XScreenOfDisplay(display, defaultScreen));
	screenHeight = XHeightOfScreen(XScreenOfDisplay(display, defaultScreen));
	XMoveWindow(display, window, (screenWidth / 2 - WIN_WIDTH / 2), (screenHeight / 2 - WIN_HEIGHT / 2));

	// Message Loop
	while (1)
	{
		XNextEvent(display, &event);
		switch (event.type)
		{
		case MapNotify:
			break;

		case KeyPress:
			keySym = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);
			switch (keySym)
			{
			case XK_Escape:
				uninitialise();
				exit(0);
				break;
			}
			break;

		case ButtonPress:
			switch (event.xbutton.button)
			{
			case 1:
				printf("Left Mouse Button Is Clicked.\n");
				break;

			case 2:
				printf("Middle Mouse Button Is Clicked.\n");
				break;

			case 3:
				printf("Right Mouse Button Is Clicked.\n");
				break;
			}
			break;

		case FocusIn:
			printf("Window Has Got Focus.\n");
			break;

		case FocusOut:
			printf("Window Looses Its Focus.\n");
			break;

		case 33:
			uninitialise();
			exit(0);
			break;

		default:
			break;
		}
	}

	uninitialise();

	return 0;
}

void uninitialise(void)
{
	// Code
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
}
