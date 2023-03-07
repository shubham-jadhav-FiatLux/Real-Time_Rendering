//
//  MyView.m
//  Window
//
//  Created by mac on 27/02/23.
//

#import "GLESView.h"

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "vmath.h"
using namespace vmath;

@implementation GLESView
{
    @private
    EAGLContext* eaglContext;
    GLuint defaultFramebuffer;
    GLuint colorRenderBuffer;
    GLuint depthRenderBuffer;
    
    CADisplayLink* displayLink;
    GLint fps;
    BOOL isAnimating;
    
    BOOL gbToggleSquare;
    BOOL gbToggleCircle;
    BOOL gbToggleTriangle;
    BOOL gbDoubleClick;

    GLuint circleVerticesSize;
    GLuint graphPaperVerticesSize;
    
    GLint count;

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

- (id)initWithFrame:(CGRect)frame
{
    // Code
    self = [super initWithFrame:frame];
    if (self)
    {
        // Create drawable surface called as layer
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*)[super layer];
        
        //Set EAGLLayer properties
        [eaglLayer setOpaque:YES];
        
        NSNumber* boolNumber = [NSNumber numberWithBool:NO];
        NSDictionary* dictionary = [NSDictionary dictionaryWithObjectsAndKeys:boolNumber, kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        [eaglLayer setDrawableProperties:dictionary];
        
        // Create EAGL Context
        eaglContext = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if (eaglContext == nil)
        {
            printf("Failed To Create EAGL Context.\n");
            [self uninitialize];
            [self release];
            exit(0);
        }
        
        // Set this context as the current context
        [EAGLContext setCurrentContext:eaglContext];
        
        // Print OpenGLES Info
        printf("-------------------------------------------------.\n\n");
        printf("OpenGLES Renderer : %s.\n", glGetString(GL_RENDERER));
        printf("OpenGLES Version : %s.\n", glGetString(GL_VERSION));
        printf("GLSL Version : %s.\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        printf("-------------------------------------------------.\n\n");
        
        // Create Framebuffer
        glGenFramebuffers(1, &defaultFramebuffer);
        
        // Bind with above framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        
        // Create Color Render buffer
        glGenRenderbuffers(1, &colorRenderBuffer);
        
        // Bind With above color render buffer
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
        
        //
        [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        
        // set framebuffer according to render buffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);
        
        // Declare backing width
        GLint backingWidth;
        
        // Declare backing height
        GLint backingHeight;
        
        // Get color render buffer width into backing width
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
        
        // Get color render buffer width into backing height
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
        
        // Create depth render buffer
        glGenRenderbuffers(1, &depthRenderBuffer);
        
        // Bind with above depth render buffer
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
        
        // Provide storage to depth render buffer
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
        
        // set framebuffer according to render buffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
        
        // Check the status of the framebuffer
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Incomplete Framebuffer.\n");
            [self uninitialize];
            [self release];
            exit(0);
        }
        
        // Call Initialize
        [self initialize];
        
        fps = 60;           // Started from iOS 8.0
        isAnimating = NO;
        
        // Gesture Recognizer Code
        // Single Tap
        UITapGestureRecognizer* singleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
        [singleTapGestureRecognizer setNumberOfTapsRequired:1];
        [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [singleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        
        // Double Tap
        UITapGestureRecognizer* doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [doubleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        // Differentiate Between Single Tap And Double Tap
        [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
        
        
        // Swipe
        UISwipeGestureRecognizer* swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        
        // Long Press
        UILongPressGestureRecognizer* longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        [self addGestureRecognizer:longPressGestureRecognizer];
    }
    
    return self;
}

+ (Class)layerClass
{
    // Code
    return [CAEAGLLayer class];
}

- (void)layoutSubviews
{
    // Code
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);

    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)[self layer]];
    
    // Declare backing width
    GLint backingWidth;
    
    // Declare backing height
    GLint backingHeight;
    
    // Get color render buffer width into backing width
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
    
    // Get color render buffer width into backing height
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
    
    // Create depth render buffer
    glGenRenderbuffers(1, &depthRenderBuffer);
    
    // Bind with above depth render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    
    // Provide storage to depth render buffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
    
    // set framebuffer according to render buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
    
    // Call Our Resize here
    [self resize:backingWidth :backingHeight];
    
    // Call drawView After resize
    [self drawView:self];
    
    // Check the status of the framebuffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Incomplete Framebuffer.\n");
        [self uninitialize];
        [self release];
        exit(0);
    }
}

/*
- (void)drawRect: (CGRect)dirtyRect
{
    // Code
}
*/

- (void)drawView: (id)sender
{
    // Code
    // Set context as the current context
    [EAGLContext setCurrentContext:eaglContext];
    
    // Bind with framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    // Call Our display And update Here
    [self display];
    
    [self myUpdate];
    
    // Bind With color render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
    
    
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)startAnimation
{
    // Code
    if (isAnimating == NO)
    {
        // create display link
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        
        // Set FPS for the display link
        [displayLink setPreferredFramesPerSecond:fps];
        
        // Add this display link to run loop
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        
        isAnimating = YES;
    }
}

- (void)stopAnimation
{
    // Code
    if (isAnimating == YES)
    {
        // Remove display link from the run loop
        [displayLink invalidate];
        
        isAnimating = NO;
    }
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
    count = 0;
    
    // Vertex Shader
    const GLchar* vertexShaderSourceCode =
        "#version 300 es" \
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
                printf("Vertex Shader Compilation Log: %s\n",  log);
                free(log);
                [self uninitialize];
                [self release];
                exit(0);
            }
        }
    }

    // Fragment Shader
    const GLchar* fragmentShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "precision highp float;" \
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
                printf("Fragment Shader Compilation Log: %s\n", log);
                free(log);
                [self uninitialize];
                [self release];
                exit(0);
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
                printf("Shader Program Link Log: %s\n", log);
                free(log);
                [self uninitialize];
                [self release];
                exit(0);
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

    graphPaperVerticesSize = sizeof(graphPaperVertices) / sizeof(float);

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
    circleVerticesSize = sizeof(circleVertices)/sizeof(float);

    

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
    
    
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
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
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLORglGenRenderbuffers | GL_DEPTH_BUFFER_BIT);
 
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
    //glPointSize(0.5f);
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
        printf("Memory Not Allocated : Malloc Failed\n");
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
        printf("Failed");
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
    
    if (depthRenderBuffer)
    {
        glDeleteRenderbuffers(1, &depthRenderBuffer);
        depthRenderBuffer = 0;
    }
    
    if (colorRenderBuffer)
    {
        glDeleteRenderbuffers(1, &colorRenderBuffer);
        colorRenderBuffer = 0;
    }
    
    if (defaultFramebuffer)
    {
        glDeleteFramebuffers(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }
    
    if ([EAGLContext currentContext] == eaglContext)
    {
        [EAGLContext setCurrentContext:nil];
        [eaglContext release];
        eaglContext = nil;
    }
}

- (BOOL)acceptsFirstResponder
{
    // Code
    return YES;
}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    // Code
    
}

- (void)onSingleTap:(UITapGestureRecognizer*)tgr
{
    // Code
    count++;
    if (count == 1)
    {
    if (gbToggleSquare == FALSE)
        gbToggleSquare = TRUE;
    else
        gbToggleSquare = FALSE;
    }
    else if (count == 2)
    {
    if (gbToggleCircle == FALSE)
        gbToggleCircle = TRUE;
    else
        gbToggleCircle = FALSE;
    }
    else if (count == 3)
    {
    if (gbToggleTriangle == FALSE)
        gbToggleTriangle = TRUE;
    else
        gbToggleTriangle = FALSE;
    }
    else
        count = 0;
}

- (void)onDoubleTap:(UITapGestureRecognizer*)tgr
{
    // Code
    gbDoubleClick = TRUE;
}

- (void)onSwipe:(UISwipeGestureRecognizer*)sgr
{
    // Code
    [self uninitialize];
    [self release];
    exit(0);
}

- (void)onLongPress:(UILongPressGestureRecognizer*)lpgr
{
    // Code
    
}

- (void)dealloc
{
    // Code
    [self uninitialize];
    
    if (displayLink)
    {
        [displayLink release];
        displayLink = nil;
    }
    
    [super dealloc];
}

@end
