
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

    // Programmable Pipeline Related Global Variables
    GLuint shaderProgramObject;

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

    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;

    GLuint laUniform;                // Ambient
    GLuint ldUniform;                // Diffuse
    GLuint lsUniform;                // Specular
    GLuint lightPositionUniform;

    GLuint kaUniform;
    GLuint kdUniform;
    GLuint ksUniform;
    GLuint materialShininessUniform;

    GLuint lightingEnabledUniform;

    GLint KeyPressed;
    GLfloat AngleForXRotation;
    GLfloat AngleForYRotation;
    GLfloat AngleForZRotation;
    GLfloat currentWidth;
    GLfloat currentHeight;

    GLfloat LightAmbient[4];
    GLfloat LightDiffuse[4];
    GLfloat LightSpecular[4];
    GLfloat LightPosition[4];

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
    
    currentWidth = width;
    currentHeight = height;
    
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
    KeyPressed = 0;
    AngleForXRotation = 0.0f;
    AngleForYRotation = 0.0f;
    AngleForZRotation = 0.0f;
    
    NSRect rect = [self bounds];
    currentWidth = rect.size.width;
    currentHeight = rect.size.height;
    
    LightAmbient[0] = 0.0f;
    LightAmbient[1] = 0.0f;
    LightAmbient[2] = 0.0f;
    LightAmbient[3] = 1.0f;
    
    LightDiffuse[0] = 1.0f;
    LightDiffuse[1] = 1.0f;
    LightDiffuse[2] = 1.0f;
    LightDiffuse[3] = 1.0f;
    
    LightSpecular[0] = 1.0f;
    LightSpecular[1] = 1.0f;
    LightSpecular[2] = 1.0f;
    LightSpecular[3] = 1.0f;
    
    LightPosition[0] = 0.0f;
    LightPosition[1] = 0.0f;
    LightPosition[2] = 2.0f;
    LightPosition[3] = 1.0f;
    
    // Vertex Shader
    const GLchar* vertexShaderSourceCode =
        "#version 410 core" \
        "\n" \
        "in vec4 a_position;" \
        "in vec3 a_normal;" \
        "uniform mat4 u_modelMatrix;" \
        "uniform mat4 u_viewMatrix;" \
        "uniform mat4 u_projectionMatrix;" \
        "uniform vec4 u_lightPosition;" \
        "uniform int u_lightingEnabled;" \
        "out vec3 transformedNormals;" \
        "out vec3 lightDirection;" \
        "out vec3 viewerVector;" \
        "void main(void)" \
        "{" \
        "if (u_lightingEnabled == 1)" \
        "{" \
        "vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" \
        "mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
        "transformedNormals = normalMatrix * a_normal;" \
        "lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;" \
        "viewerVector = -eyeCoordinates.xyz;" \
        "}" \
        "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;" \
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
        "in vec3 transformedNormals;" \
        "in vec3 lightDirection;" \
        "in vec3 viewerVector;" \
        "uniform vec3 u_la;" \
        "uniform vec3 u_ld;" \
        "uniform vec3 u_ls;" \
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
        "vec3 ambient = u_la * u_ka;" \
        "vec3 normalized_tranformed_normals = normalize(transformedNormals);" \
        "vec3 normalized_lightDirection = normalize(lightDirection);" \
        "vec3 diffuse = u_ld * u_kd * max(dot(normalized_lightDirection, normalized_tranformed_normals), 0.0);" \
        "vec3 reflectionVector = reflect(-normalized_lightDirection, normalized_tranformed_normals);" \
        "vec3 normalized_viewerVector = normalize(viewerVector);" \
        "vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector, normalized_viewerVector), 0.0), u_materialShininess);" \
        "phong_ads_light = ambient + diffuse + specular;" \
        "}" \
        "else" \
        "{" \
        "phong_ads_light = vec3(1.0, 1.0, 1.0);" \
        "}" \
        "FragColor = vec4(phong_ads_light, 1.0);" \
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
    glBindAttribLocation(shaderProgramObject, SBJ_ATTRIBUTE_NORMAL, "a_normal");
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
    modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
    viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
    projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
    laUniform = glGetUniformLocation(shaderProgramObject, "u_la");
    ldUniform = glGetUniformLocation(shaderProgramObject, "u_ld");
    lsUniform = glGetUniformLocation(shaderProgramObject, "u_ls");
    lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_lightPosition");
    kaUniform = glGetUniformLocation(shaderProgramObject, "u_ka");
    kdUniform = glGetUniformLocation(shaderProgramObject, "u_kd");
    ksUniform = glGetUniformLocation(shaderProgramObject, "u_ks");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_materialShininess");
    lightingEnabledUniform = glGetUniformLocation(shaderProgramObject, "u_lightingEnabled");

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
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    
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
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width * 1.4f / (GLfloat)height, 0.1f, 200.0f);
}

- (void)display
{
    // Code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use The Shader Program Object
    glUseProgram(shaderProgramObject);

    // Transformations
    mat4 translationMatrix = mat4::identity();
    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();

    translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    

    // Sending Light Related Uniforms
    if (bLight == TRUE)
    {
        glUniform1i(lightingEnabledUniform, 1);

        /*glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);
        */
    }
    else
    {
        glUniform1i(lightingEnabledUniform, 0);
    }

    [self Draw24Spheres];
    
    // Unuse The Shader Program Object
    glUseProgram(0);
}

- (void)myUpdate
{
    // Code
    if (KeyPressed == 1)
    {
        AngleForXRotation += 2.0f;
        AngleForYRotation = 0.0f;
        AngleForZRotation = 0.0f;
        //if (AngleForXRotation >= 360.0f)
            //AngleForXRotation -= 360.0f;
    }
    else if (KeyPressed == 2)
    {

        AngleForYRotation += 2.0f;
        AngleForXRotation += 2.0f;
        AngleForZRotation = 0.0f;
        //if (AngleForYRotation >= 360.0f)
            //AngleForYRotation -= 360.0f;
    }
    else if (KeyPressed == 3)
    {
        AngleForZRotation += 2.0f;
        AngleForXRotation += 2.0f;
        AngleForYRotation = 0.0f;
        //if (AngleForZRotation >= 360.0f)
            //AngleForZRotation -= 360.0f;
    }


    if (KeyPressed == 1)                                // X Rotation
    {
        LightPosition[1] = 10 * sin(AngleForXRotation * M_PI / 180.0f);
        LightPosition[2] = 10 * cos(AngleForXRotation * M_PI / 180.0f);
    }
    else if (KeyPressed == 2)                            // Y Rotation
    {
        LightPosition[0] = 10 * sin(AngleForYRotation * M_PI / 180.0f);
        LightPosition[2] = 10 * cos(-AngleForYRotation * M_PI / 180.0f);
    }
    else if (KeyPressed == 3)                            // Z Rotation
    {
        LightPosition[0] = 10 * -sin(AngleForZRotation * M_PI / 180.0f);
        LightPosition[1] = 10 * cos(AngleForZRotation * M_PI / 180.0f);
    }
    else
    {
        LightPosition[0] = 0.0f;
        LightPosition[1] = 0.0f;
        LightPosition[2] = 0.0f;
    }
}

- (void)Draw24Spheres
{
    // Variable Declarations
    GLfloat MaterialAmbient[4];
    GLfloat MaterialDiffuse[4];
    GLfloat MaterialSpecular[4];
    GLfloat MaterialShininess;

    GLfloat viewportWidth = (GLsizei)currentWidth / 4;
    GLfloat viewportHeight = (GLsizei)currentHeight / 6;

    mat4 translationMatrix = mat4::identity();
    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();

    GLfloat zVal = -2.0f;

    // Code
    // ***** 1st sphere on 1st column, emerald *****
    // ambient material
    MaterialAmbient[0] = 0.0215; // r
    MaterialAmbient[1] = 0.1745; // g
    MaterialAmbient[2] = 0.0215; // b
    MaterialAmbient[3] = 1.0f;   // a

    // diffuse material
    MaterialDiffuse[0] = 0.07568; // r
    MaterialDiffuse[1] = 0.61424; // g
    MaterialDiffuse[2] = 0.07568; // b
    MaterialDiffuse[3] = 1.0f;    // a

    // specular material
    MaterialSpecular[0] = 0.633;    // r
    MaterialSpecular[1] = 0.727811; // g
    MaterialSpecular[2] = 0.633;    // b
    MaterialSpecular[3] = 1.0f;     // a

    // shininess
    MaterialShininess = 0.6 * 128;

    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(0, 5 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************
    
    // ***** 2nd sphere on 1st column, jade *****
    // ambient material
    MaterialAmbient[0] = 0.135;  // r
    MaterialAmbient[1] = 0.2225; // g
    MaterialAmbient[2] = 0.1575; // b
    MaterialAmbient[3] = 1.0f;   // a

    // diffuse material
    MaterialDiffuse[0] = 0.54; // r
    MaterialDiffuse[1] = 0.89; // g
    MaterialDiffuse[2] = 0.63; // b
    MaterialDiffuse[3] = 1.0f; // a

    // specular material
    MaterialSpecular[0] = 0.316228; // r
    MaterialSpecular[1] = 0.316228; // g
    MaterialSpecular[2] = 0.316228; // b
    MaterialSpecular[3] = 1.0f;     // a

    // shininess
    MaterialShininess = 0.1 * 128;
    //glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(0, 4 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************
    
    // ***** 3rd sphere on 1st column, obsidian *****
    // ambient material
    MaterialAmbient[0] = 0.05375; // r
    MaterialAmbient[1] = 0.05;    // g
    MaterialAmbient[2] = 0.06625; // b
    MaterialAmbient[3] = 1.0f;    // a

    // diffuse material
    MaterialDiffuse[0] = 0.18275; // r
    MaterialDiffuse[1] = 0.17;    // g
    MaterialDiffuse[2] = 0.22525; // b
    MaterialDiffuse[3] = 1.0f;    // a

    // specular material
    MaterialSpecular[0] = 0.332741; // r
    MaterialSpecular[1] = 0.328634; // g
    MaterialSpecular[2] = 0.346435; // b
    MaterialSpecular[3] = 1.0f;     // a

    // shininess
    MaterialShininess = 0.3 * 128;

    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(0, 3 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 4th sphere on 1st column, pearl *****
    // ambient material
    MaterialAmbient[0] = 0.25;    // r
    MaterialAmbient[1] = 0.20725; // g
    MaterialAmbient[2] = 0.20725; // b
    MaterialAmbient[3] = 1.0f;    // a

    // diffuse material
    MaterialDiffuse[0] = 1.0;   // r
    MaterialDiffuse[1] = 0.829; // g
    MaterialDiffuse[2] = 0.829; // b
    MaterialDiffuse[3] = 1.0f;  // a

    // specular material
    MaterialSpecular[0] = 0.296648; // r
    MaterialSpecular[1] = 0.296648; // g
    MaterialSpecular[2] = 0.296648; // b
    MaterialSpecular[3] = 1.0f;     // a

    // shininess
    MaterialShininess = 0.088 * 128;

    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(0, 2 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 5th sphere on 1st column, ruby *****
    // ambient material
    MaterialAmbient[0] = 0.1745;  // r
    MaterialAmbient[1] = 0.01175; // g
    MaterialAmbient[2] = 0.01175; // b
    MaterialAmbient[3] = 1.0f;    // a

    // diffuse material
    MaterialDiffuse[0] = 0.61424; // r
    MaterialDiffuse[1] = 0.04136; // g
    MaterialDiffuse[2] = 0.04136; // b
    MaterialDiffuse[3] = 1.0f;    // a

    // specular material
    MaterialSpecular[0] = 0.727811; // r
    MaterialSpecular[1] = 0.626959; // g
    MaterialSpecular[2] = 0.626959; // b
    MaterialSpecular[3] = 1.0f;     // a

    // shininess
    MaterialShininess = 0.6 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(0, 1 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 6th sphere on 1st column, turquoise *****
    // ambient material
    MaterialAmbient[0] = 0.1;     // r
    MaterialAmbient[1] = 0.18725; // g
    MaterialAmbient[2] = 0.1745;  // b
    MaterialAmbient[3] = 1.0f;    // a

    // diffuse material
    MaterialDiffuse[0] = 0.396;   // r
    MaterialDiffuse[1] = 0.74151; // g
    MaterialDiffuse[2] = 0.69102; // b
    MaterialDiffuse[3] = 1.0f;    // a

    // specular material
    MaterialSpecular[0] = 0.297254; // r
    MaterialSpecular[1] = 0.30829;  // g
    MaterialSpecular[2] = 0.306678; // b
    MaterialSpecular[3] = 1.0f;     // a

    // shininess
    MaterialShininess = 0.1 * 128;

    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(0, 0 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************
    // *******************************************************
    // *******************************************************

    // ***** 1st sphere on 2nd column, brass *****
    // ambient material
    MaterialAmbient[0] = 0.329412; // r
    MaterialAmbient[1] = 0.223529; // g
    MaterialAmbient[2] = 0.027451; // b
    MaterialAmbient[3] = 1.0f;     // a

    // diffuse material
    MaterialDiffuse[0] = 0.780392; // r
    MaterialDiffuse[1] = 0.568627; // g
    MaterialDiffuse[2] = 0.113725; // b
    MaterialDiffuse[3] = 1.0f;     // a

    // specular material
    MaterialSpecular[0] = 0.992157; // r
    MaterialSpecular[1] = 0.941176; // g
    MaterialSpecular[2] = 0.807843; // b
    MaterialSpecular[3] = 1.0f;     // a

    // shininess
    MaterialShininess = 0.21794872 * 128;

    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(1 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 2nd sphere on 2nd column, bronze *****
    // ambient material
    MaterialAmbient[0] = 0.2125; // r
    MaterialAmbient[1] = 0.1275; // g
    MaterialAmbient[2] = 0.054;  // b
    MaterialAmbient[3] = 1.0f;   // a

    // diffuse material
    MaterialDiffuse[0] = 0.714;   // r
    MaterialDiffuse[1] = 0.4284;  // g
    MaterialDiffuse[2] = 0.18144; // b
    MaterialDiffuse[3] = 1.0f;    // a

    // specular material
    MaterialSpecular[0] = 0.393548; // r
    MaterialSpecular[1] = 0.271906; // g
    MaterialSpecular[2] = 0.166721; // b
    MaterialSpecular[3] = 1.0f;     // a

    // shininess
    MaterialShininess = 0.2 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(1 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 3rd sphere on 2nd column, chrome *****
    // ambient material
    MaterialAmbient[0] = 0.25; // r
    MaterialAmbient[1] = 0.25; // g
    MaterialAmbient[2] = 0.25; // b
    MaterialAmbient[3] = 1.0f; // a

    // diffuse material
    MaterialDiffuse[0] = 0.4;  // r
    MaterialDiffuse[1] = 0.4;  // g
    MaterialDiffuse[2] = 0.4;  // b
    MaterialDiffuse[3] = 1.0f; // a

    // specular material
    MaterialSpecular[0] = 0.774597; // r
    MaterialSpecular[1] = 0.774597; // g
    MaterialSpecular[2] = 0.774597; // b
    MaterialSpecular[3] = 1.0f;     // a

    // shininess
    MaterialShininess = 0.6 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(1 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 4th sphere on 2nd column, copper *****
    // ambient material
    MaterialAmbient[0] = 0.19125; // r
    MaterialAmbient[1] = 0.0735;  // g
    MaterialAmbient[2] = 0.0225;  // b
    MaterialAmbient[3] = 1.0f;    // a

    // diffuse material
    MaterialDiffuse[0] = 0.7038;  // r
    MaterialDiffuse[1] = 0.27048; // g
    MaterialDiffuse[2] = 0.0828;  // b
    MaterialDiffuse[3] = 1.0f;    // a

    // specular material
    MaterialSpecular[0] = 0.256777; // r
    MaterialSpecular[1] = 0.137622; // g
    MaterialSpecular[2] = 0.086014; // b
    MaterialSpecular[3] = 1.0f;     // a

    // shininess
    MaterialShininess = 0.1 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(1 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 5th sphere on 2nd column, gold *****
    // ambient material
    MaterialAmbient[0] = 0.24725; // r
    MaterialAmbient[1] = 0.1995;  // g
    MaterialAmbient[2] = 0.0745;  // b
    MaterialAmbient[3] = 1.0f;    // a

    // diffuse material
    MaterialDiffuse[0] = 0.75164; // r
    MaterialDiffuse[1] = 0.60648; // g
    MaterialDiffuse[2] = 0.22648; // b
    MaterialDiffuse[3] = 1.0f;    // a

    // specular material
    MaterialSpecular[0] = 0.628281; // r
    MaterialSpecular[1] = 0.555802; // g
    MaterialSpecular[2] = 0.366065; // b
    MaterialSpecular[3] = 1.0f;     // a

    // shininess
    MaterialShininess = 0.4 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(1 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 6th sphere on 2nd column, silver *****
    // ambient material
    MaterialAmbient[0] = 0.19225; // r
    MaterialAmbient[1] = 0.19225; // g
    MaterialAmbient[2] = 0.19225; // b
    MaterialAmbient[3] = 1.0f;    // a
    
    // diffuse material
    MaterialDiffuse[0] = 0.50754; // r
    MaterialDiffuse[1] = 0.50754; // g
    MaterialDiffuse[2] = 0.50754; // b
    MaterialDiffuse[3] = 1.0f;    // a
    
    // specular material
    MaterialSpecular[0] = 0.508273; // r
    MaterialSpecular[1] = 0.508273; // g
    MaterialSpecular[2] = 0.508273; // b
    MaterialSpecular[3] = 1.0f;     // a
    
    // shininess
    MaterialShininess = 0.4 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(1 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************
    // *******************************************************
    // *******************************************************

    // ***** 1st sphere on 3rd column, black *****
    // ambient material
    MaterialAmbient[0] = 0.0;  // r
    MaterialAmbient[1] = 0.0;  // g
    MaterialAmbient[2] = 0.0;  // b
    MaterialAmbient[3] = 1.0f; // a
    
    // diffuse material
    MaterialDiffuse[0] = 0.01; // r
    MaterialDiffuse[1] = 0.01; // g
    MaterialDiffuse[2] = 0.01; // b
    MaterialDiffuse[3] = 1.0f; // a
    
    // specular material
    MaterialSpecular[0] = 0.50; // r
    MaterialSpecular[1] = 0.50; // g
    MaterialSpecular[2] = 0.50; // b
    MaterialSpecular[3] = 1.0f; // a
    
    // shininess
    MaterialShininess = 0.25 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(2 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 2nd sphere on 3rd column, cyan *****
    // ambient material
    MaterialAmbient[0] = 0.0;  // r
    MaterialAmbient[1] = 0.1;  // g
    MaterialAmbient[2] = 0.06; // b
    MaterialAmbient[3] = 1.0f; // a
    
    // diffuse material
    MaterialDiffuse[0] = 0.0;        // r
    MaterialDiffuse[1] = 0.50980392; // g
    MaterialDiffuse[2] = 0.50980392; // b
    MaterialDiffuse[3] = 1.0f;       // a
    
    // specular material
    MaterialSpecular[0] = 0.50196078; // r
    MaterialSpecular[1] = 0.50196078; // g
    MaterialSpecular[2] = 0.50196078; // b
    MaterialSpecular[3] = 1.0f;       // a
    
    // shininess
    MaterialShininess = 0.25 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(2 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 3rd sphere on 2nd column, green *****
    // ambient material
    MaterialAmbient[0] = 0.0;  // r
    MaterialAmbient[1] = 0.0;  // g
    MaterialAmbient[2] = 0.0;  // b
    MaterialAmbient[3] = 1.0f; // a

    // diffuse material
    MaterialDiffuse[0] = 0.1;  // r
    MaterialDiffuse[1] = 0.35; // g
    MaterialDiffuse[2] = 0.1;  // b
    MaterialDiffuse[3] = 1.0f; // a
    
    // specular material
    MaterialSpecular[0] = 0.45; // r
    MaterialSpecular[1] = 0.55; // g
    MaterialSpecular[2] = 0.45; // b
    MaterialSpecular[3] = 1.0f; // a
    
    // shininess
    MaterialShininess = 0.25 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(2 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 4th sphere on 3rd column, red *****
    // ambient material
    MaterialAmbient[0] = 0.0;  // r
    MaterialAmbient[1] = 0.0;  // g
    MaterialAmbient[2] = 0.0;  // b
    MaterialAmbient[3] = 1.0f; // a
    
    // diffuse material
    MaterialDiffuse[0] = 0.5;  // r
    MaterialDiffuse[1] = 0.0;  // g
    MaterialDiffuse[2] = 0.0;  // b
    MaterialDiffuse[3] = 1.0f; // a
    
    // specular material
    MaterialSpecular[0] = 0.7;  // r
    MaterialSpecular[1] = 0.6;  // g
    MaterialSpecular[2] = 0.6;  // b
    MaterialSpecular[3] = 1.0f; // a
    
    // shininess
    MaterialShininess = 0.25 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(2 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 5th sphere on 3rd column, white *****
    // ambient material
    MaterialAmbient[0] = 0.0;  // r
    MaterialAmbient[1] = 0.0;  // g
    MaterialAmbient[2] = 0.0;  // b
    MaterialAmbient[3] = 1.0f; // a
    
    // diffuse material
    MaterialDiffuse[0] = 0.55; // r
    MaterialDiffuse[1] = 0.55; // g
    MaterialDiffuse[2] = 0.55; // b
    MaterialDiffuse[3] = 1.0f; // a
    
    // specular material
    MaterialSpecular[0] = 0.70; // r
    MaterialSpecular[1] = 0.70; // g
    MaterialSpecular[2] = 0.70; // b
    MaterialSpecular[3] = 1.0f; // a
    
    // shininess
    MaterialShininess = 0.25 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(2 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 6th sphere on 3rd column, yellow plastic *****
    // ambient material
    MaterialAmbient[0] = 0.0;  // r
    MaterialAmbient[1] = 0.0;  // g
    MaterialAmbient[2] = 0.0;  // b
    MaterialAmbient[3] = 1.0f; // a
    
    // diffuse material
    MaterialDiffuse[0] = 0.5;  // r
    MaterialDiffuse[1] = 0.5;  // g
    MaterialDiffuse[2] = 0.0;  // b
    MaterialDiffuse[3] = 1.0f; // a
    
    // specular material
    MaterialSpecular[0] = 0.60; // r
    MaterialSpecular[1] = 0.60; // g
    MaterialSpecular[2] = 0.50; // b
    MaterialSpecular[3] = 1.0f; // a
    
    // shininess
    MaterialShininess = 0.25 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(2 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************
    // *******************************************************
    // *******************************************************

    // ***** 1st sphere on 4th column, black *****
    // ambient material
    MaterialAmbient[0] = 0.02; // r
    MaterialAmbient[1] = 0.02; // g
    MaterialAmbient[2] = 0.02; // b
    MaterialAmbient[3] = 1.0f; // a
    
    // diffuse material
    MaterialDiffuse[0] = 0.01; // r
    MaterialDiffuse[1] = 0.01; // g
    MaterialDiffuse[2] = 0.01; // b
    MaterialDiffuse[3] = 1.0f; // a
    
    // specular material
    MaterialSpecular[0] = 0.4;  // r
    MaterialSpecular[1] = 0.4;  // g
    MaterialSpecular[2] = 0.4;  // b
    MaterialSpecular[3] = 1.0f; // a
    
    // shininess
    MaterialShininess = 0.078125 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(3 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 2nd sphere on 4th column, cyan *****
    // ambient material
    MaterialAmbient[0] = 0.0;  // r
    MaterialAmbient[1] = 0.05; // g
    MaterialAmbient[2] = 0.05; // b
    MaterialAmbient[3] = 1.0f; // a
    
    // diffuse material
    MaterialDiffuse[0] = 0.4;  // r
    MaterialDiffuse[1] = 0.5;  // g
    MaterialDiffuse[2] = 0.5;  // b
    MaterialDiffuse[3] = 1.0f; // a
    
    // specular material
    MaterialSpecular[0] = 0.04; // r
    MaterialSpecular[1] = 0.7;  // g
    MaterialSpecular[2] = 0.7;  // b
    MaterialSpecular[3] = 1.0f; // a
    
    // shininess
    MaterialShininess = 0.078125 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(3 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 3rd sphere on 4th column, green *****
    // ambient material
    MaterialAmbient[0] = 0.0;  // r
    MaterialAmbient[1] = 0.05; // g
    MaterialAmbient[2] = 0.0;  // b
    MaterialAmbient[3] = 1.0f; // a
    
    // diffuse material
    MaterialDiffuse[0] = 0.4;  // r
    MaterialDiffuse[1] = 0.5;  // g
    MaterialDiffuse[2] = 0.4;  // b
    MaterialDiffuse[3] = 1.0f; // a
    
    // specular material
    MaterialSpecular[0] = 0.04; // r
    MaterialSpecular[1] = 0.7;  // g
    MaterialSpecular[2] = 0.04; // b
    MaterialSpecular[3] = 1.0f; // a
    
    // shininess
    MaterialShininess = 0.078125 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(3 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 4th sphere on 4th column, red *****
    // ambient material
    MaterialAmbient[0] = 0.05; // r
    MaterialAmbient[1] = 0.0;  // g
    MaterialAmbient[2] = 0.0;  // b
    MaterialAmbient[3] = 1.0f; // a

    // diffuse material
    MaterialDiffuse[0] = 0.5;  // r
    MaterialDiffuse[1] = 0.4;  // g
    MaterialDiffuse[2] = 0.4;  // b
    MaterialDiffuse[3] = 1.0f; // a
    
    // specular material
    MaterialSpecular[0] = 0.7;  // r
    MaterialSpecular[1] = 0.04; // g
    MaterialSpecular[2] = 0.04; // b
    MaterialSpecular[3] = 1.0f; // a
    
    // shininess
    MaterialShininess = 0.078125 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(3 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 5th sphere on 4th column, white *****
    // ambient material
    MaterialAmbient[0] = 0.05; // r
    MaterialAmbient[1] = 0.05; // g
    MaterialAmbient[2] = 0.05; // b
    MaterialAmbient[3] = 1.0f; // a

    // diffuse material
    MaterialDiffuse[0] = 0.5;  // r
    MaterialDiffuse[1] = 0.5;  // g
    MaterialDiffuse[2] = 0.5;  // b
    MaterialDiffuse[3] = 1.0f; // a
    
    // specular material
    MaterialSpecular[0] = 0.7;  // r
    MaterialSpecular[1] = 0.7;  // g
    MaterialSpecular[2] = 0.7;  // b
    MaterialSpecular[3] = 1.0f; // a
    
    // shininess
    MaterialShininess = 0.078125 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(3 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************

    // ***** 6th sphere on 4th column, yellow rubber *****
    // ambient material
    MaterialAmbient[0] = 0.05; // r
    MaterialAmbient[1] = 0.05; // g
    MaterialAmbient[2] = 0.0;  // b
    MaterialAmbient[3] = 1.0f; // a
    
    // diffuse material
    MaterialDiffuse[0] = 0.5;  // r
    MaterialDiffuse[1] = 0.5;  // g
    MaterialDiffuse[2] = 0.4;  // b
    MaterialDiffuse[3] = 1.0f; // a
    
    // specular material
    MaterialSpecular[0] = 0.7;  // r
    MaterialSpecular[1] = 0.7;  // g
    MaterialSpecular[2] = 0.04; // b
    MaterialSpecular[3] = 1.0f; // a
    
    // shininess
    MaterialShininess = 0.078125 * 128;
    
    if (bLight == TRUE)
    {
        glUniform3fv(laUniform, 1, LightAmbient);
        glUniform3fv(ldUniform, 1, LightDiffuse);
        glUniform3fv(lsUniform, 1, LightSpecular);
        glUniform4fv(lightPositionUniform, 1, LightPosition);

        glUniform3fv(kaUniform, 1, MaterialAmbient);
        glUniform3fv(kdUniform, 1, MaterialDiffuse);
        glUniform3fv(ksUniform, 1, MaterialSpecular);
        glUniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    translationMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    translationMatrix = vmath::translate(0.0f, 0.0f, zVal);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
    glViewport(3 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
    glBindVertexArray(vao_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // *******************************************************
    // *******************************************************
    // *******************************************************
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
            
        case 'X':
        case 'x':
            KeyPressed = 1;
            AngleForXRotation = 0.0f;        // Reset
            AngleForYRotation = 0.0f;
            AngleForZRotation = 0.0f;
            break;

        case 'Y':
        case 'y':
            KeyPressed = 2;
            AngleForXRotation += 0.0f;
            AngleForYRotation = 0.0f;
            AngleForZRotation = 0.0f;
            break;

        case 'Z':
        case 'z':
            KeyPressed = 3;
            AngleForXRotation += 0.0f;
            AngleForYRotation = 0.0f;
            AngleForZRotation = 0.0f;
            break;

        default:
            KeyPressed = 0;
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
