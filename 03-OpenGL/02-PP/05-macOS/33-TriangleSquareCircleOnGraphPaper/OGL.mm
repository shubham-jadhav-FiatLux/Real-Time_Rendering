
// Header Files
#import <Foundation/Foundation.h>                       // #import == #pragma once
#import <Cocoa/cocoa.h>

#import <QuartzCore/CVDisplayLink.h>

#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>

#import "vmath.h"
using namespace vmath;

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
    
    BOOL gbToggleSquare;
    BOOL gbToggleCircle;
    BOOL gbToggleTriangle;
    BOOL gbDoubleClick;

    GLuint circleVerticesSize;
    GLuint graphPaperVerticesSize;

    // Programmable Pipeline Related Global Variables
    GLuint shaderProgramObject;

    enum
    {
        SBJ_ATTRIBUTE_POSITION = 0,
        SBJ_ATTRIBUTE_COLOR,
        SBJ_ATTRIBUTE_NORMAL,
        SBJ_ATTRIBUTE_TEXTURE0
    };

    GLuint vao_triangle;
    GLuint vbo_triangle;

    GLuint vao_square;
    GLuint vbo_square;

    GLuint vao_circle;
    GLuint vbo_circle;

    GLuint vao_graph;
    GLuint vbo_graph;

    GLuint vao_graphAxis;
    GLuint vbo_graphAxis;

    GLuint mvpMatrixUniform;

    mat4 perspectiveProjectionMatrix;

    // Push-Pop Related Variables
    struct Node
    {
        mat4 data;
        struct Node* next;
    };

    Node* head;
    int top;
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
    fprintf(gpFile, "\n-------------------------------------------------.\n\n");
    
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
    // Variable Declarations
    GLint status;
    GLint infoLogLength;
    char* log = NULL;
    
    // Code
    gbToggleSquare = FALSE;
    gbToggleCircle = FALSE;
    gbToggleTriangle = FALSE;
    gbDoubleClick = FALSE;
    head = NULL;
    top = -1;
    
    // Vertex Shader
    const GLchar* vertexShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec4 a_position;" \
        "in vec4 a_color;" \
        "uniform mat4 u_mvpMatrix;" \
        "out vec4 a_color_out;" \
        "void main(void)" \
        "{" \
        "gl_Position = u_mvpMatrix * a_position;" \
        "a_color_out = a_color;" \
        "}";

    // Creating Shader Object
    GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    // Giving Shader Code To Shader Object
    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
    // Compile The Shader
    glCompileShader(vertexShaderObject);
    // Error Checking
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            log = (char*)malloc(infoLogLength);
            if (log != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);
                fprintf(gpFile, "Vertex Shader Compilation Log: %s\n",  log);
                free(log);
                [self uninitialize];
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    // Fragment Shader
    const GLchar* fragmentShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec4 a_color_out;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = a_color_out;" \
        "}";

    GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
    glCompileShader(fragmentShaderObject);
    status = 0;
    infoLogLength = 0;
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
                glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);
                fprintf(gpFile, "Fragment Shader Compilation Log: %s\n", log);
                free(log);
                [self uninitialize];
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    // Shader Program Object
    shaderProgramObject = glCreateProgram();
    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);
    glBindAttribLocation(shaderProgramObject, SBJ_ATTRIBUTE_POSITION, "a_position");
    glBindAttribLocation(shaderProgramObject, SBJ_ATTRIBUTE_COLOR, "a_color");
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
                fprintf(gpFile, "Shader Program Link Log: %s\n", log);
                free(log);
                [self uninitialize];
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");

    // Declaration Of Vertex Data Arrays
    const GLfloat triangleVertices[] =
                                        {
                                            0.0f, 0.5f,
                                            0.5f * (float)cos(225 / 180.f * M_PI), 0.5f * (float)sin(225 / 180.f * M_PI),

                                            0.5f * (float)cos(225 / 180.f * M_PI), 0.5f * (float)sin(225 / 180.f * M_PI),
                                            0.5f * (float)cos(315 / 180.f * M_PI), 0.5f * (float)sin(315 / 180.f * M_PI),

                                            0.5f * (float)cos(315 / 180.f * M_PI), 0.5f * (float)sin(315 / 180.f * M_PI),
                                            0.0f, 0.5f,
                                        };

    const GLfloat triangleColor[] =
                                        {
                                            1.0f, 1.0f, 0.0f,
                                            1.0f, 1.0f, 0.0f,

                                            1.0f, 1.0f, 0.0f,
                                            1.0f, 1.0f, 0.0f,

                                            1.0f, 1.0f, 0.0f,
                                            1.0f, 1.0f, 0.0f,
                                        };

    const GLfloat squareVertices[] =
                                        {
                                            0.5f, 0.5f,
                                            -0.5f, 0.5f,

                                            -0.5f, 0.5f,
                                            -0.5f, -0.5f,

                                            -0.5f, -0.5f,
                                            0.5f, -0.5f,

                                            0.5f, -0.5f,
                                            0.5f, 0.5f,
                                        };

    const GLfloat squareColor[] =
                                        {
                                            1.0f, 1.0f, 0.0f,
                                            1.0f, 1.0f, 0.0f,

                                            1.0f, 1.0f, 0.0f,
                                            1.0f, 1.0f, 0.0f,

                                            1.0f, 1.0f, 0.0f,
                                            1.0f, 1.0f, 0.0f,

                                            1.0f, 1.0f, 0.0f,
                                            1.0f, 1.0f, 0.0f,
                                        };

    const GLfloat graphPaperVertices[] =
                                        {

                                                -1.000000, 1.000000,
                                                1.000000, 1.000000,
                                                -1.000000, 0.950000,
                                                1.000000, 0.950000,
                                                -1.000000, 0.900000,
                                                1.000000, 0.900000,
                                                -1.000000, 0.850000,
                                                1.000000, 0.850000,
                                                -1.000000, 0.800000,
                                                1.000000, 0.800000,
                                                -1.000000, 0.750000,
                                                1.000000, 0.750000,
                                                -1.000000, 0.700000,
                                                1.000000, 0.700000,
                                                -1.000000, 0.650000,
                                                1.000000, 0.650000,
                                                -1.000000, 0.600000,
                                                1.000000, 0.600000,
                                                -1.000000, 0.550000,
                                                1.000000, 0.550000,
                                                -1.000000, 0.500000,
                                                1.000000, 0.500000,
                                                -1.000000, 0.450000,
                                                1.000000, 0.450000,
                                                -1.000000, 0.400000,
                                                1.000000, 0.400000,
                                                -1.000000, 0.350000,
                                                1.000000, 0.350000,
                                                -1.000000, 0.300000,
                                                1.000000, 0.300000,
                                                -1.000000, 0.250000,
                                                1.000000, 0.250000,
                                                -1.000000, 0.200000,
                                                1.000000, 0.200000,
                                                -1.000000, 0.150000,
                                                1.000000, 0.150000,
                                                -1.000000, 0.100000,
                                                1.000000, 0.100000,
                                                -1.000000, 0.050000,
                                                1.000000, 0.050000,


                                                -1.000000, -1.000000,
                                                1.000000, -1.000000,
                                                -1.000000, -0.950000,
                                                1.000000, -0.950000,
                                                -1.000000, -0.900000,
                                                1.000000, -0.900000,
                                                -1.000000, -0.850000,
                                                1.000000, -0.850000,
                                                -1.000000, -0.800000,
                                                1.000000, -0.800000,
                                                -1.000000, -0.750000,
                                                1.000000, -0.750000,
                                                -1.000000, -0.700000,
                                                1.000000, -0.700000,
                                                -1.000000, -0.650000,
                                                1.000000, -0.650000,
                                                -1.000000, -0.600000,
                                                1.000000, -0.600000,
                                                -1.000000, -0.550000,
                                                1.000000, -0.550000,
                                                -1.000000, -0.500000,
                                                1.000000, -0.500000,
                                                -1.000000, -0.450000,
                                                1.000000, -0.450000,
                                                -1.000000, -0.400000,
                                                1.000000, -0.400000,
                                                -1.000000, -0.350000,
                                                1.000000, -0.350000,
                                                -1.000000, -0.300000,
                                                1.000000, -0.300000,
                                                -1.000000, -0.250000,
                                                1.000000, -0.250000,
                                                -1.000000, -0.200000,
                                                1.000000, -0.200000,
                                                -1.000000, -0.150000,
                                                1.000000, -0.150000,
                                                -1.000000, -0.100000,
                                                1.000000, -0.100000,
                                                -1.000000, -0.050000,
                                                1.000000, -0.050000,


                                                -1.000000, 1.000000,
                                                -1.000000, -1.000000,
                                                -0.950000, 1.000000,
                                                -0.950000, -1.000000,
                                                -0.900000, 1.000000,
                                                -0.900000, -1.000000,
                                                -0.850000, 1.000000,
                                                -0.850000, -1.000000,
                                                -0.800000, 1.000000,
                                                -0.800000, -1.000000,
                                                -0.750000, 1.000000,
                                                -0.750000, -1.000000,
                                                -0.700000, 1.000000,
                                                -0.700000, -1.000000,
                                                -0.650000, 1.000000,
                                                -0.650000, -1.000000,
                                                -0.600000, 1.000000,
                                                -0.600000, -1.000000,
                                                -0.550000, 1.000000,
                                                -0.550000, -1.000000,
                                                -0.500000, 1.000000,
                                                -0.500000, -1.000000,
                                                -0.450000, 1.000000,
                                                -0.450000, -1.000000,
                                                -0.400000, 1.000000,
                                                -0.400000, -1.000000,
                                                -0.350000, 1.000000,
                                                -0.350000, -1.000000,
                                                -0.300000, 1.000000,
                                                -0.300000, -1.000000,
                                                -0.250000, 1.000000,
                                                -0.250000, -1.000000,
                                                -0.200000, 1.000000,
                                                -0.200000, -1.000000,
                                                -0.150000, 1.000000,
                                                -0.150000, -1.000000,
                                                -0.100000, 1.000000,
                                                -0.100000, -1.000000,
                                                -0.050000, 1.000000,
                                                -0.050000, -1.000000,


                                                1.000000, 1.000000,
                                                1.000000, -1.000000,
                                                0.950000, 1.000000,
                                                0.950000, -1.000000,
                                                0.900000, 1.000000,
                                                0.900000, -1.000000,
                                                0.850000, 1.000000,
                                                0.850000, -1.000000,
                                                0.800000, 1.000000,
                                                0.800000, -1.000000,
                                                0.750000, 1.000000,
                                                0.750000, -1.000000,
                                                0.700000, 1.000000,
                                                0.700000, -1.000000,
                                                0.650000, 1.000000,
                                                0.650000, -1.000000,
                                                0.600000, 1.000000,
                                                0.600000, -1.000000,
                                                0.550000, 1.000000,
                                                0.550000, -1.000000,
                                                0.500000, 1.000000,
                                                0.500000, -1.000000,
                                                0.450000, 1.000000,
                                                0.450000, -1.000000,
                                                0.400000, 1.000000,
                                                0.400000, -1.000000,
                                                0.350000, 1.000000,
                                                0.350000, -1.000000,
                                                0.300000, 1.000000,
                                                0.300000, -1.000000,
                                                0.250000, 1.000000,
                                                0.250000, -1.000000,
                                                0.200000, 1.000000,
                                                0.200000, -1.000000,
                                                0.150000, 1.000000,
                                                0.150000, -1.000000,
                                                0.100000, 1.000000,
                                                0.100000, -1.000000,
                                                0.050000, 1.000000,
                                                0.050000, -1.000000,
                                        };

    graphPaperVerticesSize = sizeof(graphPaperVertices);

    const GLfloat graphPaperColor[] =
    {

            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,


            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,


            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,


                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
    };

    const GLfloat graphPaperAxisVertices[] =
                                                {
                                                    -1.0f, 0.0f,
                                                    1.0f, 0.0f,

                                                    0.0f, 1.0f,
                                                    0.0f, -1.0f
                                                };

    const GLfloat graphPaperAxisColor[] =
                                                {
                                                    1.0f, 0.0f, 0.0f,
                                                    1.0f, 0.0f, 0.0f,

                                                    0.0f, 1.0f, 0.0f,
                                                    0.0f, 1.0f, 0.0f
                                                };

    const GLfloat circleVertices[] = {
                                        0.500000, 0.000000,
                                        0.499981, 0.004363,
                                        0.499924, 0.008726,
                                        0.499829, 0.013088,
                                        0.499695, 0.017450,
                                        0.499524, 0.021810,
                                        0.499315, 0.026168,
                                        0.499067, 0.030524,
                                        0.498782, 0.034878,
                                        0.498459, 0.039230,
                                        0.498097, 0.043578,
                                        0.497698, 0.047923,
                                        0.497261, 0.052264,
                                        0.496786, 0.056602,
                                        0.496273, 0.060935,
                                        0.495722, 0.065263,
                                        0.495134, 0.069587,
                                        0.494508, 0.073905,
                                        0.493844, 0.078217,
                                        0.493143, 0.082524,
                                        0.492404, 0.086824,
                                        0.491627, 0.091118,
                                        0.490814, 0.095404,
                                        0.489962, 0.099684,
                                        0.489074, 0.103956,
                                        0.488148, 0.108220,
                                        0.487185, 0.112476,
                                        0.486185, 0.116723,
                                        0.485148, 0.120961,
                                        0.484074, 0.125190,
                                        0.482963, 0.129410,
                                        0.481815, 0.133619,
                                        0.480631, 0.137819,
                                        0.479410, 0.142008,
                                        0.478152, 0.146186,
                                        0.476858, 0.150353,
                                        0.475528, 0.154508,
                                        0.474162, 0.158652,
                                        0.472759, 0.162784,
                                        0.471321, 0.166903,
                                        0.469846, 0.171010,
                                        0.468336, 0.175104,
                                        0.466790, 0.179184,
                                        0.465209, 0.183251,
                                        0.463592, 0.187303,
                                        0.461940, 0.191342,
                                        0.460252, 0.195366,
                                        0.458530, 0.199375,
                                        0.456773, 0.203368,
                                        0.454981, 0.207347,
                                        0.453154, 0.211309,
                                        0.451293, 0.215256,
                                        0.449397, 0.219186,
                                        0.447467, 0.223099,
                                        0.445503, 0.226995,
                                        0.443505, 0.230874,
                                        0.441474, 0.234736,
                                        0.439409, 0.238579,
                                        0.437310, 0.242405,
                                        0.435178, 0.246212,
                                        0.433013, 0.250000,
                                        0.430815, 0.253769,
                                        0.428584, 0.257519,
                                        0.426320, 0.261249,
                                        0.424024, 0.264960,
                                        0.421696, 0.268650,
                                        0.419335, 0.272320,
                                        0.416943, 0.275968,
                                        0.414519, 0.279596,
                                        0.412063, 0.283203,
                                        0.409576, 0.286788,
                                        0.407058, 0.290351,
                                        0.404508, 0.293893,
                                        0.401928, 0.297411,
                                        0.399318, 0.300908,
                                        0.396677, 0.304381,
                                        0.394005, 0.307831,
                                        0.391304, 0.311257,
                                        0.388573, 0.314660,
                                        0.385812, 0.318039,
                                        0.383022, 0.321394,
                                        0.380203, 0.324724,
                                        0.377355, 0.328030,
                                        0.374478, 0.331310,
                                        0.371572, 0.334565,
                                        0.368639, 0.337795,
                                        0.365677, 0.340999,
                                        0.362687, 0.344177,
                                        0.359670, 0.347329,
                                        0.356625, 0.350455,
                                        0.353553, 0.353553,
                                        0.350455, 0.356625,
                                        0.347329, 0.359670,
                                        0.344177, 0.362687,
                                        0.340999, 0.365677,
                                        0.337795, 0.368639,
                                        0.334565, 0.371572,
                                        0.331310, 0.374478,
                                        0.328030, 0.377355,
                                        0.324724, 0.380203,
                                        0.321394, 0.383022,
                                        0.318039, 0.385812,
                                        0.314660, 0.388573,
                                        0.311257, 0.391304,
                                        0.307831, 0.394005,
                                        0.304381, 0.396677,
                                        0.300908, 0.399318,
                                        0.297411, 0.401928,
                                        0.293893, 0.404509,
                                        0.290351, 0.407058,
                                        0.286788, 0.409576,
                                        0.283203, 0.412063,
                                        0.279596, 0.414519,
                                        0.275969, 0.416943,
                                        0.272320, 0.419335,
                                        0.268650, 0.421696,
                                        0.264960, 0.424024,
                                        0.261249, 0.426320,
                                        0.257519, 0.428584,
                                        0.253769, 0.430815,
                                        0.250000, 0.433013,
                                        0.246212, 0.435178,
                                        0.242405, 0.437310,
                                        0.238579, 0.439409,
                                        0.234736, 0.441474,
                                        0.230874, 0.443505,
                                        0.226995, 0.445503,
                                        0.223099, 0.447467,
                                        0.219186, 0.449397,
                                        0.215256, 0.451293,
                                        0.211309, 0.453154,
                                        0.207347, 0.454981,
                                        0.203368, 0.456773,
                                        0.199375, 0.458530,
                                        0.195366, 0.460252,
                                        0.191342, 0.461940,
                                        0.187303, 0.463592,
                                        0.183251, 0.465209,
                                        0.179184, 0.466790,
                                        0.175104, 0.468336,
                                        0.171010, 0.469846,
                                        0.166903, 0.471321,
                                        0.162784, 0.472759,
                                        0.158652, 0.474162,
                                        0.154508, 0.475528,
                                        0.150353, 0.476858,
                                        0.146186, 0.478152,
                                        0.142008, 0.479410,
                                        0.137819, 0.480631,
                                        0.133619, 0.481815,
                                        0.129410, 0.482963,
                                        0.125190, 0.484074,
                                        0.120961, 0.485148,
                                        0.116723, 0.486185,
                                        0.112476, 0.487185,
                                        0.108220, 0.488148,
                                        0.103956, 0.489074,
                                        0.099684, 0.489962,
                                        0.095405, 0.490814,
                                        0.091118, 0.491627,
                                        0.086824, 0.492404,
                                        0.082524, 0.493143,
                                        0.078217, 0.493844,
                                        0.073905, 0.494508,
                                        0.069587, 0.495134,
                                        0.065263, 0.495722,
                                        0.060935, 0.496273,
                                        0.056602, 0.496786,
                                        0.052264, 0.497261,
                                        0.047923, 0.497698,
                                        0.043578, 0.498097,
                                        0.039230, 0.498459,
                                        0.034878, 0.498782,
                                        0.030524, 0.499067,
                                        0.026168, 0.499315,
                                        0.021810, 0.499524,
                                        0.017450, 0.499695,
                                        0.013088, 0.499829,
                                        0.008726, 0.499924,
                                        0.004363, 0.499981,
                                        0.000000, 0.500000,
                                        - 0.004363, 0.49998,
                                        - 0.008726, 0.499924,
                                        - 0.013088, 0.499829,
                                        - 0.017450, 0.499695,
                                        - 0.021810, 0.499524,
                                        - 0.026168, 0.499315,
                                        - 0.030524, 0.499067,
                                        - 0.034878, 0.498782,
                                        - 0.039230, 0.498459,
                                        - 0.043578, 0.498097,
                                        - 0.047923, 0.497698,
                                        - 0.052264, 0.497261,
                                        - 0.056602, 0.496786,
                                        - 0.060935, 0.496273,
                                        - 0.065263, 0.495722,
                                        - 0.069587, 0.495134,
                                        - 0.073905, 0.494508,
                                        - 0.078217, 0.493844,
                                        - 0.082524, 0.493143,
                                        - 0.086824, 0.492404,
                                        - 0.091118, 0.491627,
                                        - 0.095404, 0.490814,
                                        - 0.099684, 0.489962,
                                        - 0.103956, 0.489074,
                                        - 0.108220, 0.488148,
                                        - 0.112476, 0.487185,
                                        - 0.116723, 0.486185,
                                        - 0.120961, 0.485148,
                                        - 0.125190, 0.484074,
                                        - 0.129409, 0.482963,
                                        - 0.133619, 0.481815,
                                        - 0.137819, 0.480631,
                                        - 0.142008, 0.479410,
                                        - 0.146186, 0.478152,
                                        - 0.150353, 0.476858,
                                        - 0.154509, 0.475528,
                                        - 0.158652, 0.474162,
                                        - 0.162784, 0.472759,
                                        - 0.166903, 0.471321,
                                        - 0.171010, 0.469846,
                                        - 0.175104, 0.468336,
                                        - 0.179184, 0.466790,
                                        - 0.183251, 0.465209,
                                        - 0.187303, 0.463592,
                                        - 0.191342, 0.461940,
                                        - 0.195366, 0.460252,
                                        - 0.199375, 0.458530,
                                        - 0.203368, 0.456773,
                                        - 0.207347, 0.454981,
                                        - 0.211309, 0.453154,
                                        - 0.215256, 0.451293,
                                        - 0.219186, 0.449397,
                                        - 0.223099, 0.447467,
                                        - 0.226995, 0.445503,
                                        - 0.230874, 0.443505,
                                        - 0.234736, 0.441474,
                                        - 0.238579, 0.439409,
                                        - 0.242405, 0.437310,
                                        - 0.246212, 0.435178,
                                        - 0.250000, 0.433013,
                                        - 0.253769, 0.430815,
                                        - 0.257519, 0.428584,
                                        - 0.261249, 0.426320,
                                        - 0.264960, 0.424024,
                                        - 0.268650, 0.421696,
                                        - 0.272320, 0.419335,
                                        - 0.275968, 0.416943,
                                        - 0.279596, 0.414519,
                                        - 0.283203, 0.412063,
                                        - 0.286788, 0.409576,
                                        - 0.290351, 0.407058,
                                        - 0.293893, 0.404509,
                                        - 0.297411, 0.401928,
                                        - 0.300908, 0.399318,
                                        - 0.304381, 0.396677,
                                        - 0.307831, 0.394005,
                                        - 0.311257, 0.391304,
                                        - 0.314660, 0.388573,
                                        - 0.318039, 0.385812,
                                        - 0.321394, 0.383022,
                                        - 0.324724, 0.380203,
                                        - 0.328030, 0.377355,
                                        - 0.331310, 0.374478,
                                        - 0.334565, 0.371572,
                                        - 0.337795, 0.368639,
                                        - 0.340999, 0.365677,
                                        - 0.344177, 0.362687,
                                        - 0.347329, 0.359670,
                                        - 0.350455, 0.356625,
                                        - 0.353553, 0.353553,
                                        - 0.356625, 0.350455,
                                        - 0.359670, 0.347329,
                                        - 0.362687, 0.344177,
                                        - 0.365677, 0.340999,
                                        - 0.368639, 0.337795,
                                        - 0.371572, 0.334565,
                                        - 0.374478, 0.331310,
                                        - 0.377355, 0.328030,
                                        - 0.380203, 0.324724,
                                        - 0.383022, 0.321394,
                                        - 0.385812, 0.318039,
                                        - 0.388573, 0.314660,
                                        - 0.391304, 0.311257,
                                        - 0.394005, 0.307831,
                                        - 0.396677, 0.304381,
                                        - 0.399318, 0.300908,
                                        - 0.401928, 0.297411,
                                        - 0.404509, 0.293893,
                                        - 0.407058, 0.290351,
                                        - 0.409576, 0.286788,
                                        - 0.412063, 0.283203,
                                        - 0.414519, 0.279596,
                                        - 0.416943, 0.275968,
                                        - 0.419335, 0.272320,
                                        - 0.421696, 0.268650,
                                        - 0.424024, 0.264960,
                                        - 0.426320, 0.261249,
                                        - 0.428584, 0.257519,
                                        - 0.430815, 0.253769,
                                        - 0.433013, 0.250000,
                                        - 0.435178, 0.246212,
                                        - 0.437310, 0.242405,
                                        - 0.439409, 0.238579,
                                        - 0.441474, 0.234736,
                                        - 0.443505, 0.230874,
                                        - 0.445503, 0.226995,
                                        - 0.447467, 0.223099,
                                        - 0.449397, 0.219186,
                                        - 0.451293, 0.215256,
                                        - 0.453154, 0.211309,
                                        - 0.454981, 0.207347,
                                        - 0.456773, 0.203368,
                                        - 0.458530, 0.199375,
                                        - 0.460252, 0.195366,
                                        - 0.461940, 0.191342,
                                        - 0.463592, 0.187303,
                                        - 0.465209, 0.183251,
                                        - 0.466790, 0.179184,
                                        - 0.468336, 0.175104,
                                        - 0.469846, 0.171010,
                                        - 0.471321, 0.166903,
                                        - 0.472759, 0.162784,
                                        - 0.474162, 0.158652,
                                        - 0.475528, 0.154509,
                                        - 0.476858, 0.150353,
                                        - 0.478152, 0.146186,
                                        - 0.479410, 0.142008,
                                        - 0.480631, 0.137819,
                                        - 0.481815, 0.133619,
                                        - 0.482963, 0.129409,
                                        - 0.484074, 0.125190,
                                        - 0.485148, 0.120961,
                                        - 0.486185, 0.116723,
                                        - 0.487185, 0.112476,
                                        - 0.488148, 0.108220,
                                        - 0.489074, 0.103956,
                                        - 0.489962, 0.099684,
                                        - 0.490814, 0.095404,
                                        - 0.491627, 0.091118,
                                        - 0.492404, 0.086824,
                                        - 0.493143, 0.082524,
                                        - 0.493844, 0.078217,
                                        - 0.494508, 0.073905,
                                        - 0.495134, 0.069587,
                                        - 0.495722, 0.065263,
                                        - 0.496273, 0.060935,
                                        - 0.496786, 0.05660,
                                        - 0.497261, 0.052264,
                                        - 0.497698, 0.047923,
                                        - 0.498097, 0.043578,
                                        - 0.498459, 0.039230,
                                        - 0.498782, 0.034878,
                                        - 0.499067, 0.030524,
                                        - 0.499315, 0.026168,
                                        - 0.499524, 0.021810,
                                        - 0.499695, 0.017450,
                                        - 0.499829, 0.013088,
                                        - 0.499924, 0.008726,
                                        - 0.499981, 0.004363,
                                        - 0.500000, 0.000000,
                                        - 0.499981, -0.004363,
                                        - 0.499924, -0.008726,
                                        - 0.499829, -0.013088,
                                        - 0.499695, -0.017450,
                                        - 0.499524, -0.021810,
                                        - 0.499315, -0.026168,
                                        - 0.499067, -0.030524,
                                        - 0.498782, -0.034878,
                                        - 0.498459, -0.039230,
                                        - 0.498097, -0.043578,
                                        - 0.497698, -0.047923,
                                        - 0.497261, -0.052264,
                                        - 0.496786, -0.056602,
                                        - 0.496273, -0.060935,
                                        - 0.495722, -0.065263,
                                        - 0.495134, -0.069587,
                                        - 0.494508, -0.073905,
                                        - 0.493844, -0.078217,
                                        - 0.493143, -0.082524,
                                        - 0.492404, -0.086824,
                                        - 0.491627, -0.091118,
                                        - 0.490814, -0.095404,
                                        - 0.489962, -0.099684,
                                        - 0.489074, -0.103956,
                                        - 0.488148, -0.108220,
                                        - 0.487185, -0.112476,
                                        - 0.486185, -0.116723,
                                        - 0.485148, -0.120961,
                                        - 0.484074, -0.125190,
                                        - 0.482963, -0.129410,
                                        - 0.481815, -0.133619,
                                        - 0.480631, -0.137819,
                                        - 0.479410, -0.142008,
                                        - 0.478152, -0.146186,
                                        - 0.476858, -0.150353,
                                        - 0.475528, -0.154509,
                                        - 0.474162, -0.158652,
                                        - 0.472759, -0.162784,
                                        - 0.471321, -0.166903,
                                        - 0.469846, -0.171010,
                                        - 0.468336, -0.175104,
                                        - 0.466790, -0.179184,
                                        - 0.465209, -0.183251,
                                        - 0.463592, -0.187303,
                                        - 0.461940, -0.191342,
                                        - 0.460252, -0.195366,
                                        - 0.458530, -0.199374,
                                        - 0.456773, -0.203368,
                                        - 0.454981, -0.207347,
                                        - 0.453154, -0.211309,
                                        - 0.451293, -0.215256,
                                        - 0.449397, -0.219186,
                                        - 0.447467, -0.223099,
                                        - 0.445503, -0.226995,
                                        - 0.443505, -0.230874,
                                        - 0.441474, -0.234736,
                                        - 0.439409, -0.238579,
                                        - 0.437310, -0.242405,
                                        - 0.435178, -0.246212,
                                        - 0.433013, -0.250000,
                                        - 0.430815, -0.253769,
                                        - 0.428584, -0.257519,
                                        - 0.426320, -0.261249,
                                        - 0.424024, -0.264960,
                                        - 0.421696, -0.268650,
                                        - 0.419335, -0.272319,
                                        - 0.416943, -0.275968,
                                        - 0.414519, -0.279596,
                                        - 0.412063, -0.283203,
                                        - 0.409576, -0.286788,
                                        - 0.407058, -0.290351,
                                        - 0.404508, -0.293893,
                                        - 0.401928, -0.297411,
                                        - 0.399318, -0.300908,
                                        - 0.396677, -0.304381,
                                        - 0.394005, -0.307831,
                                        - 0.391304, -0.311257,
                                        - 0.388573, -0.314660,
                                        - 0.385812, -0.318039,
                                        - 0.383022, -0.321394,
                                        - 0.380203, -0.324724,
                                        - 0.377355, -0.328029,
                                        - 0.374478, -0.331310,
                                        - 0.371572, -0.334565,
                                        - 0.368639, -0.337795,
                                        - 0.365677, -0.340999,
                                        - 0.362687, -0.344177,
                                        - 0.359670, -0.347329,
                                        - 0.356625, -0.350455,
                                        - 0.353553, -0.353553,
                                        - 0.350455, -0.356625,
                                        - 0.347329, -0.359670,
                                        - 0.344177, -0.362687,
                                        - 0.340999, -0.365677,
                                        - 0.337795, -0.368639,
                                        - 0.334565, -0.371572,
                                        - 0.331310, -0.374478,
                                        - 0.328029, -0.377355,
                                        - 0.324724, -0.380203,
                                        - 0.321394, -0.383022,
                                        - 0.318039, -0.385812,
                                        - 0.314660, -0.388573,
                                        - 0.311257, -0.391304,
                                        - 0.307831, -0.394005,
                                        - 0.304381, -0.396677,
                                        - 0.300908, -0.399318,
                                        - 0.297411, -0.401928,
                                        - 0.293893, -0.404508,
                                        - 0.290351, -0.407058,
                                        - 0.286788, -0.409576,
                                        - 0.283203, -0.412063,
                                        - 0.279596, -0.414519,
                                        - 0.275968, -0.416943,
                                        - 0.272319, -0.419335,
                                        - 0.268650, -0.421696,
                                        - 0.264960, -0.424024,
                                        - 0.261249, -0.426320,
                                        - 0.257519, -0.428584,
                                        - 0.253769, -0.430815,
                                        - 0.250000, -0.433013,
                                        - 0.246212, -0.435178,
                                        - 0.242405, -0.437310,
                                        - 0.238579, -0.439409,
                                        - 0.234736, -0.441474,
                                        - 0.230874, -0.443505,
                                        - 0.226995, -0.445503,
                                        - 0.223099, -0.447467,
                                        - 0.219186, -0.449397,
                                        - 0.215256, -0.451293,
                                        - 0.211309, -0.453154,
                                        - 0.207347, -0.454981,
                                        - 0.203368, -0.456773,
                                        - 0.199374, -0.458530,
                                        - 0.195366, -0.460252,
                                        - 0.191342, -0.461940,
                                        - 0.187303, -0.463592,
                                        - 0.183251, -0.465209,
                                        - 0.179184, -0.466790,
                                        - 0.175104, -0.468336,
                                        - 0.171010, -0.469846,
                                        - 0.166903, -0.471321,
                                        - 0.162784, -0.472759,
                                        - 0.158652, -0.474162,
                                        - 0.154509, -0.475528,
                                        - 0.150353, -0.476858,
                                        - 0.146186, -0.478152,
                                        - 0.142008, -0.479410,
                                        - 0.137819, -0.480631,
                                        - 0.133619, -0.481815,
                                        - 0.129410, -0.482963,
                                        - 0.125190, -0.484074,
                                        - 0.120961, -0.485148,
                                        - 0.116723, -0.486185,
                                        - 0.112476, -0.487185,
                                        - 0.108220, -0.488148,
                                        - 0.103956, -0.489074,
                                        - 0.099684, -0.489962,
                                        - 0.095404, -0.490814,
                                        - 0.091118, -0.491627,
                                        - 0.086824, -0.492404,
                                        - 0.082524, -0.493143,
                                        - 0.078217, -0.493844,
                                        - 0.073905, -0.494508,
                                        - 0.069587, -0.495134,
                                        - 0.065263, -0.495722,
                                        - 0.060935, -0.496273,
                                        - 0.056602, -0.496786,
                                        - 0.052264, -0.497261,
                                        - 0.047923, -0.497698,
                                        - 0.043578, -0.498097,
                                        - 0.039230, -0.498459,
                                        - 0.034878, -0.498782,
                                        - 0.030524, -0.499067,
                                        - 0.026168, -0.499315,
                                        - 0.021810, -0.499524,
                                        - 0.017450, -0.499695,
                                        - 0.013088, -0.499829,
                                        - 0.008726, -0.499924,
                                        - 0.004363, -0.499981,
                                        - 0.000000, -0.500000,
                                        0.004363, -0.499981,
                                        0.008726, -0.499924,
                                        0.013088, -0.499829,
                                        0.017450, -0.499695,
                                        0.021810, -0.499524,
                                        0.026168, -0.499315,
                                        0.030524, -0.499067,
                                        0.034878, -0.498782,
                                        0.039230, -0.498459,
                                        0.043578, -0.498097,
                                        0.047923, -0.497698,
                                        0.052264, -0.497261,
                                        0.056602, -0.496786,
                                        0.060935, -0.496273,
                                        0.065263, -0.495722,
                                        0.069587, -0.495134,
                                        0.073905, -0.494508,
                                        0.078217, -0.493844,
                                        0.082524, -0.493143,
                                        0.086824, -0.492404,
                                        0.091118, -0.491627,
                                        0.095404, -0.490814,
                                        0.099684, -0.489962,
                                        0.103956, -0.489074,
                                        0.108220, -0.488148,
                                        0.112476, -0.487185,
                                        0.116723, -0.486185,
                                        0.120961, -0.485148,
                                        0.125190, -0.484074,
                                        0.129410, -0.482963,
                                        0.133619, -0.481815,
                                        0.137819, -0.480631,
                                        0.142008, -0.479410,
                                        0.146186, -0.478152,
                                        0.150353, -0.476858,
                                        0.154509, -0.475528,
                                        0.158652, -0.474162,
                                        0.162784, -0.472759,
                                        0.166903, -0.471321,
                                        0.171010, -0.469846,
                                        0.175104, -0.468336,
                                        0.179184, -0.466790,
                                        0.183251, -0.465209,
                                        0.187303, -0.463592,
                                        0.191342, -0.461940,
                                        0.195366, -0.460252,
                                        0.199374, -0.458530,
                                        0.203368, -0.456773,
                                        0.207347, -0.454981,
                                        0.211309, -0.453154,
                                        0.215256, -0.451293,
                                        0.219186, -0.449397,
                                        0.223099, -0.447467,
                                        0.226995, -0.445503,
                                        0.230874, -0.443505,
                                        0.234736, -0.441474,
                                        0.238579, -0.439409,
                                        0.242405, -0.437310,
                                        0.246212, -0.435178,
                                        0.250000, -0.433013,
                                        0.253769, -0.430815,
                                        0.257519, -0.428584,
                                        0.261249, -0.426320,
                                        0.264960, -0.424024,
                                        0.268650, -0.421696,
                                        0.272319, -0.419335,
                                        0.275968, -0.416943,
                                        0.279596, -0.414519,
                                        0.283203, -0.412063,
                                        0.286788, -0.409576,
                                        0.290351, -0.407058,
                                        0.293893, -0.404508,
                                        0.297411, -0.401928,
                                        0.300908, -0.399318,
                                        0.304381, -0.396677,
                                        0.307831, -0.394005,
                                        0.311257, -0.391304,
                                        0.314660, -0.388573,
                                        0.318039, -0.385812,
                                        0.321394, -0.383022,
                                        0.324724, -0.380203,
                                        0.328029, -0.377355,
                                        0.331310, -0.374478,
                                        0.334565, -0.371572,
                                        0.337795, -0.368639,
                                        0.340999, -0.365677,
                                        0.344177, -0.362687,
                                        0.347329, -0.359670,
                                        0.350455, -0.356625,
                                        0.353553, -0.353553,
                                        0.356625, -0.350455,
                                        0.359670, -0.347329,
                                        0.362687, -0.344177,
                                        0.365677, -0.340999,
                                        0.368639, -0.337795,
                                        0.371572, -0.334565,
                                        0.374478, -0.331310,
                                        0.377355, -0.328029,
                                        0.380203, -0.324724,
                                        0.383022, -0.321394,
                                        0.385812, -0.318039,
                                        0.388573, -0.314660,
                                        0.391304, -0.311257,
                                        0.394005, -0.307831,
                                        0.396677, -0.304381,
                                        0.399318, -0.300908,
                                        0.401928, -0.297411,
                                        0.404508, -0.293893,
                                        0.407058, -0.290351,
                                        0.409576, -0.286788,
                                        0.412063, -0.283203,
                                        0.414519, -0.279596,
                                        0.416943, -0.275968,
                                        0.419335, -0.272319,
                                        0.421696, -0.268650,
                                        0.424024, -0.264960,
                                        0.426320, -0.261249,
                                        0.428584, -0.257519,
                                        0.430815, -0.253769,
                                        0.433013, -0.250000,
                                        0.435178, -0.246212,
                                        0.437310, -0.242405,
                                        0.439409, -0.238579,
                                        0.441474, -0.234736,
                                        0.443505, -0.230874,
                                        0.445503, -0.226995,
                                        0.447467, -0.223099,
                                        0.449397, -0.219186,
                                        0.451293, -0.215256,
                                        0.453154, -0.211309,
                                        0.454981, -0.207347,
                                        0.456773, -0.203368,
                                        0.458530, -0.199374,
                                        0.460252, -0.195366,
                                        0.461940, -0.191342,
                                        0.463592, -0.187303,
                                        0.465209, -0.183251,
                                        0.466790, -0.179184,
                                        0.468336, -0.175104,
                                        0.469846, -0.171010,
                                        0.471321, -0.166903,
                                        0.472759, -0.162784,
                                        0.474162, -0.158652,
                                        0.475528, -0.154509,
                                        0.476858, -0.150353,
                                        0.478152, -0.146186,
                                        0.479410, -0.142008,
                                        0.480631, -0.137819,
                                        0.481815, -0.133619,
                                        0.482963, -0.129410,
                                        0.484074, -0.125190,
                                        0.485148, -0.120961,
                                        0.486185, -0.116723,
                                        0.487185, -0.112476,
                                        0.488148, -0.108220,
                                        0.489074, -0.103956,
                                        0.489962, -0.099684,
                                        0.490814, -0.095404,
                                        0.491627, -0.091118,
                                        0.492404, -0.086824,
                                        0.493143, -0.082524,
                                        0.493844, -0.078217,
                                        0.494508, -0.073905,
                                        0.495134, -0.069587,
                                        0.495722, -0.065263,
                                        0.496273, -0.060935,
                                        0.496786, -0.056602,
                                        0.497261, -0.052264,
                                        0.497698, -0.047923,
                                        0.498097, -0.043578,
                                        0.498459, -0.039230,
                                        0.498782, -0.034878,
                                        0.499067, -0.030524,
                                        0.499315, -0.026168,
                                        0.499524, -0.021810,
                                        0.499695, -0.017450,
                                        0.499829, -0.013088,
                                        0.499924, -0.008726,
                                        0.499981, -0.004363,
                                        0.500000, -0.000000,
                                    };
    circleVerticesSize = sizeof(circleVertices);

    const GLfloat circleColor[] = {
                                        1.0f, 1.0f, 1.0f
                                     };

    // vao And vbo Related Code
    // Triangle
    glGenVertexArrays(1, &vao_triangle);
    glBindVertexArray(vao_triangle);

        glGenBuffers(1, &vbo_triangle);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //glVertexAttrib3f(SBJ_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);

        glGenBuffers(1, &vbo_triangle);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColor), triangleColor, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // Square
    glGenVertexArrays(1, &vao_square);
    glBindVertexArray(vao_square);

        glGenBuffers(1, &vbo_square);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_square);
        glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_square);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_square);
        glBufferData(GL_ARRAY_BUFFER, sizeof(squareColor), squareColor, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // Circle
    glGenVertexArrays(1, &vao_circle);
    glBindVertexArray(vao_circle);

        glGenBuffers(1, &vbo_circle);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_circle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glVertexAttrib3f(SBJ_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
        /*
        glGenBuffers(1, &vbo_circle);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_circle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(circleColor), circleColor, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        */
    glBindVertexArray(0);

    // GraphLines
    glGenVertexArrays(1, &vao_graph);
    glBindVertexArray(vao_graph);

        glGenBuffers(1, &vbo_graph);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_graph);
        glBufferData(GL_ARRAY_BUFFER, sizeof(graphPaperVertices), graphPaperVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //glVertexAttrib3f(SBJ_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);
        
        glGenBuffers(1, &vbo_graph);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_graph);
        glBufferData(GL_ARRAY_BUFFER, sizeof(graphPaperColor), graphPaperColor, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);

    // GraphAxis
    glGenVertexArrays(1, &vao_graphAxis);
    glBindVertexArray(vao_graphAxis);

        glGenBuffers(1, &vbo_graphAxis);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_graphAxis);
        glBufferData(GL_ARRAY_BUFFER, sizeof(graphPaperAxisVertices), graphPaperAxisVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_graphAxis);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_graphAxis);
        glBufferData(GL_ARRAY_BUFFER, sizeof(graphPaperAxisColor), graphPaperAxisColor, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);


    // Here Starts OpenGL Code
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);                            // glShadeModel(),glHint() Deprecated In PP

    // Clear The Screen Using Black Color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    perspectiveProjectionMatrix = mat4::identity();
    
    return 0;
}

- (void)resize :(int)width :(int)height
{
    // Code
    if (height <= 0)
        height = 1;
    
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

- (void)display
{
    // Function Declarations
    void GraphPaperLines(void);
    void DrawSquare(void);
    void DrawCircle(void);
    void DrawTriangle(void);
    void sglPushMatrix(mat4);
    vmath::mat4 sglPopMatrix(void);

    // Code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use The Shader Program Object
    glUseProgram(shaderProgramObject);

    // Transformations
    mat4 translationMatrix = mat4::identity();
    mat4 modelViewMatrix = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();

    translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    modelViewMatrix = translationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);



    glBindVertexArray(vao_graphAxis);

    glDrawArrays(GL_LINES, 0, 4);

    glBindVertexArray(0);

    if (gbDoubleClick)
        [self GraphPaperLines];

    if (gbToggleSquare)
        [self DrawSquare];

    if (gbToggleCircle)
        [self DrawCircle];

    if (gbToggleTriangle)
        [self DrawTriangle];
    
    // Unuse The Shader Program Object
    glUseProgram(0);
}

- (void)myUpdate
{
    // Code
}

- (void)GraphPaperLines
{
    // Code
    glBindVertexArray(vao_graph);

    glDrawArrays(GL_LINES, 0, graphPaperVerticesSize);

    glBindVertexArray(0);
}

- (void)DrawSquare
{
    // Code
    glBindVertexArray(vao_square);

    glDrawArrays(GL_LINES, 0, 8);

    glBindVertexArray(0);
}

- (void)DrawCircle
{
    // Code
    glPointSize(0.5f);
    glBindVertexArray(vao_circle);
    
    glDrawArrays(GL_POINTS, 0, circleVerticesSize);

    glBindVertexArray(0);
}

- (void)DrawTriangle
{
    // Code
    glBindVertexArray(vao_triangle);

    glDrawArrays(GL_LINES, 0, 6);

    glBindVertexArray(0);
}


- (void)sglPushMatrix : (mat4)in
{
    // Code
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        fprintf(gpFile, "Memory Not Allocated : Malloc Failed\n");
        return;
    }
    newNode->data = in;
    newNode->next = NULL;

    if (top == -1 || head == NULL)
    {
        head = newNode;
        top += 1;
    }
    else
    {
        newNode->next = head;
        head = newNode;
        top += 1;
    }
}

- (mat4)sglPopMatrix
{
    mat4 out;
    Node* temp = NULL;

    // Code
    if (head == NULL)
    {
        fprintf(gpFile, "Failed");
    }
    else
    {
        out = head->data;
        temp = head;
        head = head->next;
        free(temp);
        top -= 1;
    }

    return out;
}

- (void)uninitialize
{
    // Code
    //  Deletion And Uninitialization of vbo
    if (vbo_triangle)
    {
        glDeleteBuffers(1, &vbo_triangle);
        vbo_triangle = 0;
    }

    if (vbo_square)
    {
        glDeleteBuffers(1, &vbo_square);
        vbo_square = 0;
    }

    if (vbo_circle)
    {
        glDeleteBuffers(1, &vbo_circle);
        vbo_circle = 0;
    }

    // Deletion And Uninitialization of vao
    if (vao_circle)
    {
        glDeleteVertexArrays(1, &vao_circle);
        vao_circle = 0;
    }

    if (vao_square)
    {
        glDeleteVertexArrays(1, &vao_square);
        vao_square = 0;
    }

    if (vao_triangle)
    {
        glDeleteVertexArrays(1, &vao_triangle);
        vao_triangle = 0;
    }

    // Shader uninitialization
    if (shaderProgramObject)
    {
        glUseProgram(shaderProgramObject);
        GLsizei numAttachedShaders;
        glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);
        GLuint* shaderObjects = NULL;
        shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
        //if (shaderObject)
            // Error Checking
        glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);
        for (GLsizei i = 0; i < numAttachedShaders; i++)
        {
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
            
        case 'S':
        case 's':
            if (gbToggleSquare == FALSE)
                gbToggleSquare = TRUE;
            else
                gbToggleSquare = FALSE;
            break;

        case 'C':
        case 'c':
            if (gbToggleCircle == FALSE)
                gbToggleCircle = TRUE;
            else
                gbToggleCircle = FALSE;
            break;

        case 'T':
        case 't':
            if (gbToggleTriangle == FALSE)
                gbToggleTriangle = TRUE;
            else
                gbToggleTriangle = FALSE;
            break;

        default:
            break;
    }
}

- (void)mouseDown: (NSEvent *) event
{
    // Code
    gbDoubleClick = TRUE;
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
