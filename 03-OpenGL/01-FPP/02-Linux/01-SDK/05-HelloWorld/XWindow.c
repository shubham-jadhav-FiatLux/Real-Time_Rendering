// Standard Header Files
#include <stdio.h>			// For Standard I/O
#include <stdlib.h>			// For exit()
#include <memory.h>			// For memset()
#include <string.h>			// For strlen()

// X11 Headers
#include <X11/Xlib.h>
#include <X11/Xutil.h>			// XVisualInfo
#include <X11/XKBlib.h>		// For Keyboard

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global Variables
Display *display = NULL;
XVisualInfo visualInfo;
Colormap colormap;
Window window;
Bool fullscreen = False;

// Entry-Point Function
int main(void)
{
	// Function Declarations
	void uninitialise(void);
	void toggleFullscreen(void);

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
	char keys[26];
	static GC gc;
	XGCValues gcValues;
	static XFontStruct *fontStruct = NULL;
	XColor fontColor;
	char str[] = "Hello World !!!";
	int strLength;
	int strWidth;
	int fontHeight;
	static int winWidth;
	static int winHeight;

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
	windowAttributes.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

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
			fontStruct = XLoadQueryFont(display, "fixed");
			break;

		case KeyPress:
			keySym = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);			// like WM_KEYDOWN
			switch (keySym)
			{
			case XK_Escape:
				XUnloadFont(display, fontStruct->fid);
				XFreeGC(display, gc);
				uninitialise();
				exit(0);
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
			}
			break;

		case ConfigureNotify:																// Like WM_SIZE
			winWidth = event.xconfigure.width;
			winHeight = event.xconfigure.height;
			break;

		case Expose:
			gc = XCreateGC(display, window, 0, &gcValues);

			XSetFont(display, gc, fontStruct->fid);

			XAllocNamedColor(display, colormap, "green", &fontColor, &fontColor);
			XSetForeground(display, gc, fontColor.pixel);

			strLength = strlen(str);
			strWidth = XTextWidth(fontStruct, str, strLength);
			fontHeight = fontStruct->ascent + fontStruct->descent;

			XDrawString(display,
				window,
				gc,
				(winWidth / 2 - strWidth / 2),
				(winHeight / 2 - fontHeight / 2),
				str,
				strLength);
			break;

		case 33:
			XUnloadFont(display, fontStruct->fid);
			XFreeGC(display, gc);
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
		       RootWindow(display, visualInfo.screen),
		       False,
		       SubstructureNotifyMask,
		       &event);
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
