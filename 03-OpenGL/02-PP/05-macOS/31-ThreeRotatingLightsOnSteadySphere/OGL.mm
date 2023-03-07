
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
    
    float* sphere_vertices;
    float* sphere_normals;
    float* sphere_texcoords;
    unsigned int* sphere_elements;
    unsigned int gNumVertices;
    unsigned int gNumElements;
    int vertex_count;

    BOOL bLight;
    char choosenShader;
    GLfloat lightAngleZero;
    GLfloat lightAngleOne;
    GLfloat lightAngleTwo;

    // Programmable Pipeline Related Global Variables
    GLuint shaderProgramObject_pv;
    GLuint shaderProgramObject_pf;

    enum
    {
        SBJ_ATTRIBUTE_POSITION = 0,
        SBJ_ATTRIBUTE_COLOR,
        SBJ_ATTRIBUTE_NORMAL,
        SBJ_ATTRIBUTE_TEXTURE0
    };

    GLuint vao_Sphere;
    GLuint vbo_Sphere_Position;
    GLuint vbo_Sphere_Normals;
    GLuint vbo_Sphere_Elements;

    GLuint modelMatrixUniform[2];
    GLuint viewMatrixUniform[2];
    GLuint projectionMatrixUniform[2];

    GLuint laUniform[2][3];                // Ambient
    GLuint ldUniform[2][3];                // Diffuse
    GLuint lsUniform[2][3];                // Specular
    GLuint lightPositionUniform[2][3];

    struct Light
    {
        vec4 lightAmbient;
        vec4 lightDiffuse;
        vec4 lightSpecular;
        vec4 lightPosition;
    };
    Light lights[3];

    GLuint kaUniform;
    GLuint kdUniform;
    GLuint ksUniform;
    GLuint materialShininessUniform;

    GLfloat materialAmbient[4];
    GLfloat materialDiffuse[4];
    GLfloat materialSpecular[4];
    GLfloat materialShininess;

    GLuint lightingEnabledUniform;

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
    bLight  = FALSE;
    choosenShader = 'v';
    lightAngleZero = 0.0f;
    lightAngleOne = 0.0f;
    lightAngleTwo = 0.0f;
    
    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
    materialDiffuse[0] = 1.0f;
    materialDiffuse[1] = 1.0f;
    materialDiffuse[2] = 1.0f;
    materialDiffuse[3] = 1.0f;
    materialSpecular[0] = 1.0f;
    materialSpecular[1] = 1.0f;
    materialSpecular[2] = 1.0f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 50.0f;
    

    // Vertex Shader PerVertex
    const GLchar* vertexShaderSourceCode_pv =
        "#version 410 core" \
        "\n" \
        "in vec4 a_position;" \
        "in vec3 a_normal;" \
        "uniform mat4 u_modelMatrix;" \
        "uniform mat4 u_viewMatrix;" \
        "uniform mat4 u_projectionMatrix;" \
        "uniform vec3 u_la[3];" \
        "uniform vec3 u_ld[3];" \
        "uniform vec3 u_ls[3];" \
        "uniform vec4 u_lightPosition[3];" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_ks;" \
        "uniform float u_materialShininess;" \
        "uniform int u_lightingEnabled;" \
        "out vec3 phong_ads_light;" \
        "void main(void)" \
        "{" \
        "phong_ads_light = vec3(0.0, 0.0, 0.0);" \
        "if (u_lightingEnabled == 1)" \
        "{" \
        "vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" \
        "mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
        "vec3 transformedNormals = normalize(normalMatrix * a_normal);" \
        "vec3 viewerVector = normalize(-eyeCoordinates.xyz);" \
        "vec3 ambient[3];" \
        "vec3 lightDirection[3];" \
        "vec3 diffuse[3];" \
        "vec3 reflectionVector[3];" \
        "vec3 specular[3];" \
        "for (int i = 0; i < 3; i++)" \
        "{" \
        "ambient[i] = u_la[i] * u_ka;" \
        "lightDirection[i] = normalize(vec3(u_lightPosition[i]) - eyeCoordinates.xyz);" \
        "diffuse[i] = u_ld[i] * u_kd * max(dot(lightDirection[i], transformedNormals), 0.0);" \
        "reflectionVector[i] = reflect(-lightDirection[i], transformedNormals);" \
        "specular[i] = u_ls[i] * u_ks * pow(max(dot(reflectionVector[i], viewerVector), 0.0), u_materialShininess);" \
        "phong_ads_light = phong_ads_light + ambient[i] + diffuse[i] + specular[i];" \
        "}" \
        "}" \
        "else" \
        "{" \
        "phong_ads_light = vec3(1.0, 1.0, 1.0);" \
        "}" \
        "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;" \
        "}";

    // Creating Shader Object
    GLuint vertexShaderObject_pv = glCreateShader(GL_VERTEX_SHADER);
    // Giving Shader Code To Shader Object
    glShaderSource(vertexShaderObject_pv, 1, (const GLchar**)&vertexShaderSourceCode_pv, NULL);
    // Compile The Shader
    glCompileShader(vertexShaderObject_pv);
    // Error Checking
    glGetShaderiv(vertexShaderObject_pv, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject_pv, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            log = (char*)malloc(infoLogLength);
            if (log != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject_pv, infoLogLength, &written, log);
                fprintf(gpFile, "Vertex Shader Compilation Log: %s\n", log);
                free(log);
                [self uninitialize];
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    
    // Vertex Shader PerFragment
    const GLchar* vertexShaderSourceCode_pf =
        "#version 410 core" \
        "\n" \
        "in vec4 a_position;" \
        "in vec3 a_normal;" \
        "uniform mat4 u_modelMatrix;" \
        "uniform mat4 u_viewMatrix;" \
        "uniform mat4 u_projectionMatrix;" \
        "uniform vec4 u_lightPosition[3];" \
        "uniform int u_lightingEnabled;" \
        "out vec3 transformedNormals;" \
        "out vec3 lightDirection[3];" \
        "out vec3 viewerVector;" \
        "void main(void)" \
        "{" \
        "if (u_lightingEnabled == 1)" \
        "{" \
        "vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" \
        "mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
        "transformedNormals = normalMatrix * a_normal;" \
        "viewerVector = -eyeCoordinates.xyz;" \
        "for (int i = 0; i < 3; i++)" \
        "{" \
        "lightDirection[i] = vec3(u_lightPosition[i]) - eyeCoordinates.xyz;" \
        "}" \
        "}" \
        "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;" \
        "}";

    // Creating Shader Object
    GLuint vertexShaderObject_pf = glCreateShader(GL_VERTEX_SHADER);
    // Giving Shader Code To Shader Object
    glShaderSource(vertexShaderObject_pf, 1, (const GLchar**)&vertexShaderSourceCode_pf, NULL);
    // Compile The Shader
    glCompileShader(vertexShaderObject_pf);
    // Error Checking
    glGetShaderiv(vertexShaderObject_pf, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject_pf, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            log = (char*)malloc(infoLogLength);
            if (log != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(vertexShaderObject_pf, infoLogLength, &written, log);
                fprintf(gpFile, "Vertex Shader Compilation Log: %s\n", log);
                free(log);
                [self uninitialize];
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    
    // Fragment Shader PerVertex
    const GLchar* fragmentShaderSourceCode_pv =
        "#version 410 core" \
        "\n" \
        "in vec3 phong_ads_light;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = vec4(phong_ads_light, 1.0);" \
        "}";

    GLuint fragmentShaderObject_pv = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject_pv, 1, (const GLchar**)&fragmentShaderSourceCode_pv, NULL);
    glCompileShader(fragmentShaderObject_pv);
    status = 0;
    infoLogLength = 0;
    log = NULL;
    glGetShaderiv(fragmentShaderObject_pv, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject_pv, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            log = (char*)malloc(infoLogLength);
            if (log != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject_pv, infoLogLength, &written, log);
                fprintf(gpFile, "Fragment Shader Compilation Log: %s\n", log);
                free(log);
                [self uninitialize];
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    
    // Fragment Shader PerFragment
    const GLchar* fragmentShaderSourceCode_pf =
        "#version 410 core" \
        "\n" \
        "in vec3 transformedNormals;" \
        "in vec3 lightDirection[3];" \
        "in vec3 viewerVector;" \
        "uniform vec3 u_la[3];" \
        "uniform vec3 u_ld[3];" \
        "uniform vec3 u_ls[3];" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_ks;" \
        "uniform float u_materialShininess;" \
        "uniform int u_lightingEnabled;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "vec3 phong_ads_light;" \
        "if (u_lightingEnabled == 1)" \
        "{" \
        "vec3 array[3];" \
        "vec3 normalized_tranformedNormals = normalize(transformedNormals);" \
        "vec3 normalized_viewerVector = normalize(viewerVector);" \
        "vec3 ambient[3];" \
        "vec3 normalized_lightDirection[3];" \
        "vec3 diffuse[3];" \
        "vec3 reflectionVector[3];" \
        "vec3 specular[3];" \
        "for (int i = 0; i < 3; i++)" \
        "{" \
        "ambient[i] = u_la[i] * u_ka;" \
        "normalized_lightDirection[i] = normalize(lightDirection[i]);" \
        "diffuse[i] = u_ld[i] * u_kd * max(dot(normalized_lightDirection[i], normalized_tranformedNormals), 0.0);" \
        "reflectionVector[i] = reflect(-normalized_lightDirection[i], normalized_tranformedNormals);" \
        "specular[i] = u_ls[i] * u_ks * pow(max(dot(reflectionVector[i], normalized_viewerVector), 0.0), u_materialShininess);" \
        "array[i] = ambient[i] + diffuse[i] + specular[i];" \
        "}" \
        "phong_ads_light = array[0] + array[1] + array[2];" \
        "}" \
        "else" \
        "{" \
        "phong_ads_light = vec3(1.0, 1.0, 1.0);" \
        "}" \
        "FragColor = vec4(phong_ads_light, 1.0);" \
        "}";

    GLuint fragmentShaderObject_pf = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject_pf, 1, (const GLchar**)&fragmentShaderSourceCode_pf, NULL);
    glCompileShader(fragmentShaderObject_pf);
    status = 0;
    infoLogLength = 0;
    log = NULL;
    glGetShaderiv(fragmentShaderObject_pf, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject_pf, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            log = (char*)malloc(infoLogLength);
            if (log != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject_pf, infoLogLength, &written, log);
                fprintf(gpFile, "Fragment Shader Compilation Log: %s\n", log);
                free(log);
                [self uninitialize];
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    
    // Shader Program Object PerVertex

    shaderProgramObject_pv = glCreateProgram();
    glAttachShader(shaderProgramObject_pv, vertexShaderObject_pv);
    glAttachShader(shaderProgramObject_pv, fragmentShaderObject_pv);
    glBindAttribLocation(shaderProgramObject_pv, SBJ_ATTRIBUTE_POSITION, "a_position");
    glBindAttribLocation(shaderProgramObject_pv, SBJ_ATTRIBUTE_NORMAL, "a_normal");
    glLinkProgram(shaderProgramObject_pv);

    status = 0;
    infoLogLength = 0;
    log = NULL;
    glGetProgramiv(shaderProgramObject_pv, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject_pv, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            log = (char*)malloc(infoLogLength);
            if (log != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(shaderProgramObject_pv, infoLogLength, &written, log);
                fprintf(gpFile, "Shader Program Link Log: %s\n", log);
                free(log);
                [self uninitialize];
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    modelMatrixUniform[0] = glGetUniformLocation(shaderProgramObject_pv, "u_modelMatrix");
    viewMatrixUniform[0] = glGetUniformLocation(shaderProgramObject_pv, "u_viewMatrix");
    projectionMatrixUniform[0] = glGetUniformLocation(shaderProgramObject_pv, "u_projectionMatrix");
    laUniform[0][0] = glGetUniformLocation(shaderProgramObject_pv, "u_la[0]");
    laUniform[0][1] = glGetUniformLocation(shaderProgramObject_pv, "u_la[1]");
    laUniform[0][2] = glGetUniformLocation(shaderProgramObject_pv, "u_la[2]");
    ldUniform[0][0] = glGetUniformLocation(shaderProgramObject_pv, "u_ld[0]");
    ldUniform[0][1] = glGetUniformLocation(shaderProgramObject_pv, "u_ld[1]");
    ldUniform[0][2] = glGetUniformLocation(shaderProgramObject_pv, "u_ld[2]");
    lsUniform[0][0] = glGetUniformLocation(shaderProgramObject_pv, "u_ls[0]");
    lsUniform[0][1] = glGetUniformLocation(shaderProgramObject_pv, "u_ls[1]");
    lsUniform[0][2] = glGetUniformLocation(shaderProgramObject_pv, "u_ls[2]");
    lightPositionUniform[0][0] = glGetUniformLocation(shaderProgramObject_pv, "u_lightPosition[0]");
    lightPositionUniform[0][1] = glGetUniformLocation(shaderProgramObject_pv, "u_lightPosition[1]");
    lightPositionUniform[0][2] = glGetUniformLocation(shaderProgramObject_pv, "u_lightPosition[2]");
    kaUniform = glGetUniformLocation(shaderProgramObject_pv, "u_ka");
    kdUniform = glGetUniformLocation(shaderProgramObject_pv, "u_kd");
    ksUniform = glGetUniformLocation(shaderProgramObject_pv, "u_ks");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject_pv, "u_materialShininess");
    lightingEnabledUniform = glGetUniformLocation(shaderProgramObject_pv, "u_lightingEnabled");
    
    
    // Shader Program Object PerFragment

    shaderProgramObject_pf = glCreateProgram();
    glAttachShader(shaderProgramObject_pf, vertexShaderObject_pf);
    glAttachShader(shaderProgramObject_pf, fragmentShaderObject_pf);
    glBindAttribLocation(shaderProgramObject_pf, SBJ_ATTRIBUTE_POSITION, "a_position");
    glBindAttribLocation(shaderProgramObject_pf, SBJ_ATTRIBUTE_NORMAL, "a_normal");
    glLinkProgram(shaderProgramObject_pf);

    status = 0;
    infoLogLength = 0;
    log = NULL;
    glGetProgramiv(shaderProgramObject_pf, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject_pf, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            log = (char*)malloc(infoLogLength);
            if (log != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(shaderProgramObject_pf, infoLogLength, &written, log);
                fprintf(gpFile, "Shader Program Link Log: %s\n", log);
                free(log);
                [self uninitialize];
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    modelMatrixUniform[1] = glGetUniformLocation(shaderProgramObject_pf, "u_modelMatrix");
    viewMatrixUniform[1] = glGetUniformLocation(shaderProgramObject_pf, "u_viewMatrix");
    projectionMatrixUniform[1] = glGetUniformLocation(shaderProgramObject_pf, "u_projectionMatrix");
    laUniform[1][0] = glGetUniformLocation(shaderProgramObject_pf, "u_la[0]");
    laUniform[1][1] = glGetUniformLocation(shaderProgramObject_pf, "u_la[1]");
    laUniform[1][2] = glGetUniformLocation(shaderProgramObject_pf, "u_la[2]");
    ldUniform[1][0] = glGetUniformLocation(shaderProgramObject_pf, "u_ld[0]");
    ldUniform[1][1] = glGetUniformLocation(shaderProgramObject_pf, "u_ld[1]");
    ldUniform[1][2] = glGetUniformLocation(shaderProgramObject_pf, "u_ld[2]");
    lsUniform[1][0] = glGetUniformLocation(shaderProgramObject_pf, "u_ls[0]");
    lsUniform[1][1] = glGetUniformLocation(shaderProgramObject_pf, "u_ls[1]");
    lsUniform[1][2] = glGetUniformLocation(shaderProgramObject_pf, "u_ls[2]");
    lightPositionUniform[1][0] = glGetUniformLocation(shaderProgramObject_pf, "u_lightPosition[0]");
    lightPositionUniform[1][1] = glGetUniformLocation(shaderProgramObject_pf, "u_lightPosition[1]");
    lightPositionUniform[1][2] = glGetUniformLocation(shaderProgramObject_pf, "u_lightPosition[2]");
    kaUniform = glGetUniformLocation(shaderProgramObject_pf, "u_ka");
    kdUniform = glGetUniformLocation(shaderProgramObject_pf, "u_kd");
    ksUniform = glGetUniformLocation(shaderProgramObject_pf, "u_ks");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject_pf, "u_materialShininess");
    lightingEnabledUniform = glGetUniformLocation(shaderProgramObject_pf, "u_lightingEnabled");

    // Initialize Sphere
    [self initializeSphere : 30 : 30 : 0.5];
    

    // vao And vbo Related Code
    glGenVertexArrays(1, &vao_Sphere);
    glBindVertexArray(vao_Sphere);

    glGenBuffers(1, &vbo_Sphere_Position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Position);
    glBufferData(GL_ARRAY_BUFFER, 3 * vertex_count * sizeof(GLfloat), sphere_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_Sphere_Normals);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Normals);
    glBufferData(GL_ARRAY_BUFFER, 3 * vertex_count * sizeof(GLfloat), sphere_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(SBJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SBJ_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_Sphere_Elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gNumElements * sizeof(GLuint), sphere_elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // Here Starts OpenGL Code
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // glShadeModel(),glHint() Deprecated In PP

    // Clear The Screen Using Black Color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    lights[0].lightAmbient = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights[0].lightDiffuse = vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    lights[0].lightSpecular = vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    lights[0].lightPosition = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    lights[1].lightAmbient = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights[1].lightDiffuse = vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    lights[1].lightSpecular = vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    lights[1].lightPosition = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    lights[2].lightAmbient = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights[2].lightDiffuse = vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    lights[2].lightSpecular = vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    lights[2].lightPosition = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    perspectiveProjectionMatrix = mat4::identity();
    
    return 0;
}

- (void)initializeSphere:(int)slices : (int)stacks : (float)radius
{
    // variable declarations
    float x, y, z;
    float u, v;
    int i, j;
    int indexV, indexI;

    // code
    if (radius <= 0.0f)
        radius = 1.0f;

    if (slices < 4)
        slices = 4;

    if (stacks < 3)
        stacks = 3;

    float du = 2.0f * (float)M_PI / (float)(slices - 1);
    float dv = (float)M_PI / (float)(stacks - 1);

    // allocate memory

    vertex_count = slices * stacks;

    //sphere_vertices, sphere_normals, sphere_texcoords, sphere_elements

    sphere_vertices = (GLfloat*)calloc(3 * vertex_count, sizeof(GLfloat));
    sphere_normals = (GLfloat*)calloc(3 * vertex_count, sizeof(GLfloat));
    sphere_texcoords = (GLfloat*)calloc(3 * vertex_count, sizeof(GLfloat));
    sphere_elements = (GLuint*)calloc(2 * (slices - 1) * (stacks - 1) * 3, sizeof(GLuint));

    if (!sphere_vertices || !sphere_normals || !sphere_texcoords || !sphere_elements)
    {
        fprintf(gpFile, "failed to allpcate memeory for sphere data\n");
        fflush(gpFile);
        exit(1);
    }

    // fill arrays
    indexV = 0;
    for (i = 0; i < stacks; i++)
    {
        v = (float)M_PI / 2.0f - (float)i * dv;
        y = sin(v);

        for (j = 0; j < slices; j++)
        {
            u = j * du;
            x = sin(u) * cos(v);
            z = cos(u) * cos(v);

            sphere_vertices[3 * indexV + 0] = radius * x;
            sphere_vertices[3 * indexV + 1] = radius * y;
            sphere_vertices[3 * indexV + 2] = radius * z;

            sphere_normals[3 * indexV + 0] = x;
            sphere_normals[3 * indexV + 1] = y;
            sphere_normals[3 * indexV + 2] = z;

            sphere_texcoords[2 * indexV + 0] = (float)j / (float)(slices - 1);
            sphere_texcoords[2 * indexV + 1] = (float)i / (float)(stacks - 1);

            indexV++;
        }
    }

    indexI = 0;
    for (j = 0; j < stacks - 1; j++)
    {
        unsigned int row_one = j * slices;
        unsigned int row_two = (j + 1) * slices;

        for (i = 0; i < slices - 1; i++)
        {
            sphere_elements[indexI++] = row_one + i;
            sphere_elements[indexI++] = row_two + i;
            sphere_elements[indexI++] = row_two + i + 1;

            sphere_elements[indexI++] = row_one + i;
            sphere_elements[indexI++] = row_two + i + 1;
            sphere_elements[indexI++] = row_one + i + 1;
        }
    }
    gNumVertices = indexV;
    gNumElements = indexI;
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
    
    // Transformations
    mat4 translationMatrix = mat4::identity();
    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();

    translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    modelMatrix = translationMatrix;
    

    // Sending Light Related Uniforms
    
    // Use The Shader Program Object
    if (choosenShader == 'v')
    {
        glUseProgram(shaderProgramObject_pv);
        
        glUniformMatrix4fv(modelMatrixUniform[0], 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniform[0], 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform[0], 1, GL_FALSE, perspectiveProjectionMatrix);
        
        if (bLight == TRUE)
        {
            glUniform1i(lightingEnabledUniform, 1);
            for (int i = 0; i < 3; i++)
            {
                glUniform3fv(laUniform[0][i], 1, lights[i].lightAmbient);
                glUniform3fv(ldUniform[0][i], 1, lights[i].lightDiffuse);
                glUniform3fv(lsUniform[0][i], 1, lights[i].lightSpecular);
                glUniform4fv(lightPositionUniform[0][i], 1, lights[i].lightPosition);
            }

            glUniform3fv(kaUniform, 1, materialAmbient);
            glUniform3fv(kdUniform, 1, materialDiffuse);
            glUniform3fv(ksUniform, 1, materialSpecular);
            glUniform1f(materialShininessUniform, materialShininess);
        }
        else
        {
            glUniform1i(lightingEnabledUniform, 0);
        }
    }
    else
    {
        glUseProgram(shaderProgramObject_pf);
        
        glUniformMatrix4fv(modelMatrixUniform[1], 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniform[1], 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform[1], 1, GL_FALSE, perspectiveProjectionMatrix);
        
        if (bLight == TRUE)
        {
            glUniform1i(lightingEnabledUniform, 1);
            for (int i = 0; i < 3; i++)
            {
                glUniform3fv(laUniform[1][i], 1, lights[i].lightAmbient);
                glUniform3fv(ldUniform[1][i], 1, lights[i].lightDiffuse);
                glUniform3fv(lsUniform[1][i], 1, lights[i].lightSpecular);
                glUniform4fv(lightPositionUniform[1][i], 1, lights[i].lightPosition);
            }

            glUniform3fv(kaUniform, 1, materialAmbient);
            glUniform3fv(kdUniform, 1, materialDiffuse);
            glUniform3fv(ksUniform, 1, materialSpecular);
            glUniform1f(materialShininessUniform, materialShininess);
        }
        else
        {
            glUniform1i(lightingEnabledUniform, 0);
        }
    }

    
    

    glBindVertexArray(vao_Sphere);
    
        // Draw, either by 'glDrawTriangles()' or 'glDrawArrays()' or 'glDrawElements()'
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
        glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    
    // Unuse The Shader Program Object
    glUseProgram(0);
}

- (void)myUpdate
{
    // Code
    lightAngleZero += 2.0f;
    lightAngleOne += 2.0f;
    lightAngleTwo += 2.0f;

    lights[0].lightPosition[1] = 10 * sin(lightAngleZero / 180.0f * 3.14f );
    lights[0].lightPosition[2] = 10 * cos(lightAngleZero / 180.0f * 3.14f );

    lights[1].lightPosition[0] = 10 * sin(lightAngleZero * 3.14f / 180.0f);
    lights[1].lightPosition[2] = 10 * cos(lightAngleZero * 3.14f / 180.0f);

    lights[2].lightPosition[0] = 10 * -sin(lightAngleZero * 3.14f / 180.0f);
    lights[2].lightPosition[1] = 10 * cos(lightAngleZero * 3.14f / 180.0f);
}

- (void)uninitialize
{
    // Code
    //  Deletion And Uninitialization of vbo
    if (vbo_Sphere_Elements)
    {
        glDeleteBuffers(1, &vbo_Sphere_Elements);
        vbo_Sphere_Elements = 0;
    }

    if (vbo_Sphere_Normals)
    {
        glDeleteBuffers(1, &vbo_Sphere_Normals);
        vbo_Sphere_Normals = 0;
    }

    if (vbo_Sphere_Position)
    {
        glDeleteBuffers(1, &vbo_Sphere_Position);
        vbo_Sphere_Position = 0;
    }

    // Deletion And Uninitialization of vao
    if (vao_Sphere)
    {
        glDeleteVertexArrays(1, &vao_Sphere);
        vao_Sphere = 0;
    }

    
    // Shader uninitialization
    if (shaderProgramObject_pf)
    {
        glUseProgram(shaderProgramObject_pf);
        GLsizei numAttachedShaders;
        glGetProgramiv(shaderProgramObject_pf, GL_ATTACHED_SHADERS, &numAttachedShaders);
        GLuint* shaderObjects = NULL;
        shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
        //if (shaderObject)
            // Error Checking
        glGetAttachedShaders(shaderProgramObject_pf, numAttachedShaders, &numAttachedShaders, shaderObjects);
        for (GLsizei i = 0; i < numAttachedShaders; i++)
        {
            glDetachShader(shaderProgramObject_pf, shaderObjects[i]);
            glDeleteShader(shaderObjects[i]);
            shaderObjects[i] = 0;
        }
        free(shaderObjects);
        shaderObjects = NULL;
        glUseProgram(0);
        glDeleteProgram(shaderProgramObject_pf);
        shaderProgramObject_pf = 0;
    }
    
    if (shaderProgramObject_pv)
    {
        glUseProgram(shaderProgramObject_pv);
        GLsizei numAttachedShaders;
        glGetProgramiv(shaderProgramObject_pv, GL_ATTACHED_SHADERS, &numAttachedShaders);
        GLuint* shaderObjects = NULL;
        shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
        //if (shaderObject)
            // Error Checking
        glGetAttachedShaders(shaderProgramObject_pv, numAttachedShaders, &numAttachedShaders, shaderObjects);
        for (GLsizei i = 0; i < numAttachedShaders; i++)
        {
            glDetachShader(shaderProgramObject_pv, shaderObjects[i]);
            glDeleteShader(shaderObjects[i]);
            shaderObjects[i] = 0;
        }
        free(shaderObjects);
        shaderObjects = NULL;
        glUseProgram(0);
        glDeleteProgram(shaderProgramObject_pv);
        shaderProgramObject_pv = 0;
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
            
        case 'Q':
        case 'q':
            [[self window]toggleFullScreen:self];
            break;
            
        case 'L':
        case 'l':
            if (bLight == FALSE)
            {
                bLight = TRUE;
            }
            else
            {
                bLight = FALSE;
            }
            break;

        case 'V':
        case 'v':
            choosenShader = 'v';
            break;

        case 'F':
        case 'f':
            choosenShader = 'f';
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
