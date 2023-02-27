
// Header Files
#import <Foundation/Foundation.h>                       // #import == #pragma once
#import <Cocoa/cocoa.h>

#import <QuartzCore/CVDisplayLink.h>

#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>

// Global Function Declarations
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp*, const CVTimeStamp*, CVOptionFlags, CVOptionFlags*, void*);

// Global Variable Declarations
FILE* gpFile = NULL;

// Interface Declarations
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface GLView : NSOpenGLView
@end

// Entry-Point Function
int main(int argc, char* argv[])
{
    // Code
    // Create AutoreleasePool
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc]init];

    // Create Application Object
    NSApp = [NSApplication sharedApplication];

    // Create AppDelegate Object
    AppDelegate* appDelegate = [[AppDelegate alloc]init];

    // give Our AppDelegate To NSApp
    [NSApp setDelegate : appDelegate];

    // Start RunLoop
    [NSApp run];

    // Tell AutoreleasePool To Release
    [pool release];

    return 0;
}

// Implementation Of AppDelegate
@implementation AppDelegate
{
    @private
    NSWindow *window;
    GLView *view;
}
- (void)applicationDidFinishLaunching : (NSNotification*)notification
{
    // Code
    NSBundle* appBundle = [NSBundle mainBundle];
    NSString* appDirPath = [appBundle bundlePath];
    NSString* parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    NSString* logFileNameWithPath = [NSString stringWithFormat : @"%@/Log.txt", parentDirPath];
    if (logFileNameWithPath == nil)
    {
        [self release];
        [NSApp terminate : self];
    }
    //const char* pszLogFileNameWithPath = [logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
    const char* pszLogFileNameWithPath = [logFileNameWithPath UTF8String];
    gpFile = fopen(pszLogFileNameWithPath, "w");
    if (gpFile == NULL)
    {
        [self release];
        [NSApp terminate : self];
    }
    fprintf(gpFile,"Program Started Successfully..\n");
    
    NSRect rect = NSMakeRect(0.0, 0.0, 800.0, 600.0);
    window = [[NSWindow alloc]initWithContentRect : rect
                                        styleMask : NSWindowStyleMaskTitled |                         NSWindowStyleMaskClosable |                       NSWindowStyleMaskMiniaturizable |                 NSWindowStyleMaskResizable
                                          backing : NSBackingStoreBuffered
                                            defer : NO];
    [window setTitle : @"macOS Window : SBJ"];
    NSColor* backgroundColor = [NSColor blackColor];
    [window setBackgroundColor : backgroundColor];
    [window center];
    
    view = [[GLView alloc]initWithFrame : rect];
    [window setContentView : view];
    
    // Set window
    [window setDelegate : self];
    [window makeKeyAndOrderFront : self];
}

- (void)applicationWillTerminate:(NSNotification*)notification
{
    // Code
    if (gpFile)
    {
        fprintf(gpFile, "Program Finished Successfully..\n");
        fclose(gpFile);
        gpFile = NULL;
    }
}

- (void)windowWillClose:(NSNotification*)notification
{
    // Code
    [NSApp terminate : self];
}

- (void)dealloc
{
    // Code
    if (view)
    {
        [view release];
        view = nil;
    }
    
    if (window)
    {
        [window release];
        window = nil;
    }

    [super dealloc];
}

@end

// Implementation Of MyView
@implementation GLView
{
    @private
    CVDisplayLinkRef displayLink;
}

- (id)initWithFrame: (NSRect)frame
{
    // Code
    self = [super initWithFrame : frame];
    if (self)
    {
        // Initialize the array of OpenGL pixel format attributes
        NSOpenGLPixelFormatAttribute openGLPixelFormatAttributes[] = {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
            NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFANoRecovery,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFADoubleBuffer,
            0
        };
        
        // Create OpenGL pixel format using above attributes
        NSOpenGLPixelFormat *glPixelFormat =[[[NSOpenGLPixelFormat alloc]initWithAttributes : openGLPixelFormatAttributes]autorelease];
        if(glPixelFormat == nil)
        {
            fprintf(gpFile, "Failed To Create OpenGL Pixel Format.\n");
            [self uninitialize];
            [self release];
            [NSApp terminate:self];
        }
        
        // Create OpenGL context using above pixel format
        NSOpenGLContext *glContext = [[[NSOpenGLContext alloc]initWithFormat:glPixelFormat shareContext:nil]autorelease];
        if(glContext == nil)
        {
            fprintf(gpFile, "Failed To Create OpenGL Context.\n");
            [self uninitialize];
            [self release];
            [NSApp terminate:self];
        }
        
        // Set this view's pixel format by using above pixel format
        [self setPixelFormat : glPixelFormat];
        
        // Set view's OpenGL context by using above OpenGL context
        [self setOpenGLContext : glContext];
    }
    
    return self;
}

// Define getFrameForTime custom method with predefined signature
- (CVReturn)getFrameForTime : (const CVTimeStamp *)outputTime
{
    // Code
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc]init];
    
    [self drawView];
    
    [pool release];
    
    return kCVReturnSuccess;
}

- (void)prepareOpenGL
{
    // Code
    [super prepareOpenGL];
    
    // Make OpenGL contect current
    [[self openGLContext] makeCurrentContext];
    
    // Set double buffer swaping interval to 1
    GLint swapInterval = 1;
    [[self openGLContext] setValues : &swapInterval forParameter : NSOpenGLCPSwapInterval];
    
    // OpenGL Log
    fprintf(gpFile, "-------------------------------------------------.\n\n");
    fprintf(gpFile, "OpenGL Renderer : %s.\n", glGetString(GL_RENDERER));
    fprintf(gpFile, "OpenGL Version : %s.\n", glGetString(GL_VERSION));
    fprintf(gpFile, "GLSL Version : %s.\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    fprintf(gpFile, "-------------------------------------------------.\n\n");
    
    // Call our initialize
    [self initialize];
    
    // Create, Configure, Start Display Link
    // Create the display link
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    
    // Set the callback with display link
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
    
    // Convert NSOpenGL pixel format to CGL pixel Format
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    
    // Conert NSOpenGL context to CGL context
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext]CGLContextObj];
    
    // Using above info, set current CG display to CGL pixel format and context
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    
    // Start the display link
    CVDisplayLinkStart(displayLink);
}

- (void)reshape
{
    // Code
    [super reshape];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    NSRect rect = [self bounds];
    int width = rect.size.width;
    int height = rect.size.height;
    
    // Call our resize()
    [self resize : width :height];
    
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

- (void)drawRect : (NSRect)dirtyRect
{
    // Code
    // To prevent flickering
    [self drawView];
}

- (void)drawView
{
    // Code
    [[self openGLContext] makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    // Call our Display
    [self display];
    
    // Do Double Buffering
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

- (int)initialize
{
    // Code
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);       // Blue Color
    return 0;
}

- (void)resize :(int)width :(int)height
{
    // Code
    if (height <= 0)
        height = 1;
    
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

- (void)display
{
    // Code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

- (void)myUpdate
{
    // Code
}

- (void)uninitialize
{
    // Code
}

- (BOOL)acceptsFirstResponder
{
    // Code
    [[self window]makeFirstResponder : self];
    return YES;
}

- (void)keyDown: (NSEvent *) event
{
    // Code
    int key = (int) [[event characters] characterAtIndex:0];

    switch (key)
    {
        case 27:
            [self uninitialize];
            [self release];
            [NSApp terminate : self];
            break;
            
        case 'F':
        case 'f':
            [[self window]toggleFullScreen:self];
            break;

        default:
            break;
    }
}

- (void)mouseDown: (NSEvent *) event
{
    // Code
}

- (void)dealloc
{
    // Code
    if (displayLink)
    {
        CVDisplayLinkStop(displayLink);
        CVDisplayLinkRelease(displayLink);
        displayLink = nil;
    }
    [super dealloc];
}

@end


// Implement DisplayLink Callback Function
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *currentTime, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* view)
{
    // Code
    CVReturn result = [(GLView *)view getFrameForTime : outputTime];
    
    return result;
}
