
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
    GLuint vbo_triangle_position;
    GLuint vbo_triangle_color;

    GLuint vao_square;
    GLuint vbo_square_position;

    GLfloat angleTriangle;
    GLfloat angleSquare;

    GLuint mvpMatrixUniform;

    mat4 perspectiveProjectionMatrix;
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
    
    // Call our Update
    [self myUpdate];
    
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
    angleTriangle = 0.0f;
    angleSquare = 0.0f;
    
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
    const GLfloat trianglePosition[] =    {
                                            0.0f, 1.0f, 0.0f,
                                            -1.0f, -1.0f, 0.0f,
                                            1.0f, -1.0f, 0.0f
                                        };

    const GLfloat triangleColor[] =        {
                                            1.0f, 0.0f, 0.0f,        // Red
                                            0.0f, 1.0f, 0.0f,        // Green
                                            0.0f, 0.0f, 1.0f,        // Blue
                                        };

    const GLfloat squarePosition[] =    {
                                            1.0f, 1.0f, 0.0f,
                                            -1.0f, 1.0f, 0.0f,
                                            -1.0f, -1.0f, 0.0f,
                                            1.0f, -1.0f, 0.0f,
                                        };

    // vao And vbo Related Code
    // Triangle
    glGenVertexArrays(1, &vao_triangle);
    glBindVertexArray(vao_triangle);

        // vbo For Position
        glGenBuffers(1, &vbo_triangle_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(trianglePosition), trianglePosition, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // vbo For Color
        glGenBuffers(1, &vbo_triangle_color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColor), triangleColor, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_COLOR);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // Square
    glGenVertexArrays(1, &vao_square);
    glBindVertexArray(vao_square);

        // vbo For Position
        glGenBuffers(1, &vbo_square_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_square_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(squarePosition), squarePosition, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glVertexAttrib3f(SBJ_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);

    glBindVertexArray(0);

    // Here Starts OpenGL Code
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // glShadeModel(),glHint() Deprecated In PP

    // Clear The Screen Using Blue Color
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
    // Code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use The Shader Program Object
    glUseProgram(shaderProgramObject);

    // Triangle
    // Transformations
    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();
    mat4 modelViewMatrix = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();

    translationMatrix = vmath::translate(-1.5f, 0.0f, -6.0f);
    rotationMatrix = vmath::rotate(angleTriangle, 0.0f, 1.0f, 0.0f);
    modelViewMatrix = translationMatrix * rotationMatrix;                            // Here Order Is Very Important
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glBindVertexArray(vao_triangle);
    
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);

    // Square
    // Transformations
    translationMatrix = mat4::identity();
    rotationMatrix = mat4::identity();
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    translationMatrix = vmath::translate(1.5f, 0.0f, -6.0f);
    rotationMatrix = vmath::rotate(angleSquare, 1.0f, 0.0f, 0.0f);
    modelViewMatrix = translationMatrix * rotationMatrix;                            // Here Order Is Very Important
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glBindVertexArray(vao_square);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);
     
    // Unuse The Shader Program Object
    glUseProgram(0);
}

- (void)myUpdate
{
    // Code
    angleTriangle = angleTriangle + 1.0f;
    if (angleTriangle >= 360.0f)
        angleTriangle = angleTriangle - 360.0f;

    angleSquare = angleSquare + 1.0f;
    if (angleSquare >= 360.0f)
        angleSquare = angleSquare - 360.0f;
}

- (void)uninitialize
{
    // Code
    //  Deletion And Uninitialization of vbo
    if (vbo_square_position)
    {
        glDeleteBuffers(1, &vbo_square_position);
        vbo_square_position = 0;
    }

    if (vbo_triangle_color)
    {
        glDeleteBuffers(1, &vbo_triangle_color);
        vbo_triangle_color = 0;
    }

    if (vbo_triangle_position)
    {
        glDeleteBuffers(1, &vbo_triangle_position);
        vbo_triangle_position = 0;
    }

    // Deletion And Uninitialization of vao
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
