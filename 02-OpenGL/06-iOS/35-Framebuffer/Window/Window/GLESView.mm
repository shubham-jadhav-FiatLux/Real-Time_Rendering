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
    
    int FBO_WIDTH;
    int FBO_HEIGHT;
    int winWidth;
    int winHeight;

    // Programmable Pipeline Related Global Variables
    GLuint shaderProgramObject;

    enum
    {
        SBJ_ATTRIBUTE_POSITION = 0,
        SBJ_ATTRIBUTE_COLOR,
        SBJ_ATTRIBUTE_NORMAL,
        SBJ_ATTRIBUTE_TEXTURE0
    };

    GLuint vao_cube;
    GLuint vbo_cube_position;
    GLuint vbo_cube_texcoord;

    GLfloat angleCube;

    GLuint mvpMatrixUniform;
    GLuint textureSamplerUniform;

    mat4 perspectiveProjectionMatrix;

    // FBO Related Global Variables
    GLuint fbo;
    GLuint rbo;
    GLuint fbo_texture;
    bool bFBOResult;

    // Texture Scene Global Vaiables
    float* sphere_vertices;
    float* sphere_normals;
    float* sphere_texcoords;
    unsigned int* sphere_elements;
    unsigned int gNumVertices;
    unsigned int gNumElements;
    int vertex_count;

    BOOL bLight;
    GLfloat lightAngleZero_sphere;
    GLfloat lightAngleOne_sphere;
    GLfloat lightAngleTwo_sphere;

    GLuint shaderProgramObject_sphere;

    GLuint vao_Sphere;
    GLuint vbo_Sphere_Position;
    GLuint vbo_Sphere_Normals;
    GLuint vbo_Sphere_Elements;

    GLuint modelMatrixUniform_sphere;
    GLuint viewMatrixUniform_sphere;
    GLuint projectionMatrixUniform_sphere;

    GLuint laUniform_sphere[3];                // Ambient
    GLuint ldUniform_sphere[3];                // Diffuse
    GLuint lsUniform_sphere[3];                // Specular
    GLuint lightPositionUniform_sphere[3];

    struct Light
    {
        vec4 lightAmbient_sphere;
        vec4 lightDiffuse_sphere;
        vec4 lightSpecular_sphere;
        vec4 lightPosition_sphere;
    };
    Light lights_sphere[3];

    GLuint kaUniform_sphere;
    GLuint kdUniform_sphere;
    GLuint ksUniform_sphere;
    GLuint materialShininessUniform_sphere;

    GLfloat materialAmbient_sphere[4];
    GLfloat materialDiffuse_sphere[4];
    GLfloat materialSpecular_sphere[4];
    GLfloat materialShininess_sphere;

    GLuint lightingEnabledUniform_sphere;

    mat4 perspectiveProjectionMatrix_sphere;
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
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
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
    
    // Check the status of the framebuffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Incomplete Framebuffer In layoutSubviews.\n");
        [self uninitialize];
        [self release];
        exit(0);
    }
    
    // Call Our Resize here
    [self resize:backingWidth :backingHeight];
    
    // Call drawView After resize
    [self drawView:self];
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
    if (bFBOResult == true)
    {
        [self display_sphere : FBO_WIDTH : FBO_HEIGHT];
        [self update_sphere];
    }
    
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
    FBO_WIDTH = 512;
    FBO_HEIGHT = 512;
    angleCube = 0.0f;
    bFBOResult = false;
    
    bLight  = FALSE;
    lightAngleZero_sphere = 0.0f;
    lightAngleOne_sphere = 0.0f;
    lightAngleTwo_sphere = 0.0f;
    
    materialAmbient_sphere[0] = 0.0f;
    materialAmbient_sphere[1] = 0.0f;
    materialAmbient_sphere[2] = 0.0f;
    materialAmbient_sphere[3] = 1.0f;
    materialDiffuse_sphere[0] = 1.0f;
    materialDiffuse_sphere[1] = 1.0f;
    materialDiffuse_sphere[2] = 1.0f;
    materialDiffuse_sphere[3] = 1.0f;
    materialSpecular_sphere[0] = 1.0f;
    materialSpecular_sphere[1] = 1.0f;
    materialSpecular_sphere[2] = 1.0f;
    materialSpecular_sphere[3] = 1.0f;
    
    materialShininess_sphere = 50.0f;
    
    // Vertex Shader
    const GLchar* vertexShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "in vec4 a_position;" \
        "in vec2 a_texcoord;" \
        "uniform mat4 u_mvpMatrix;" \
        "out vec2 a_texcoord_out;" \
        "void main(void)" \
        "{" \
        "gl_Position = u_mvpMatrix * a_position;" \
        "a_texcoord_out = a_texcoord;" \
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
        "in vec2 a_texcoord_out;" \
        "uniform sampler2D u_textureSampler;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = texture(u_textureSampler, a_texcoord_out);" \
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
    glBindAttribLocation(shaderProgramObject, SBJ_ATTRIBUTE_TEXTURE0, "a_texcoord");
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
    textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_textureSampler");

    // Declaration Of Vertex Data Arrays

    const GLfloat cubePosition[] =        {
                                            // top
                                            1.0f, 1.0f, -1.0f,
                                            -1.0f, 1.0f, -1.0f,
                                            -1.0f, 1.0f, 1.0f,
                                            1.0f, 1.0f, 1.0f,

                                            // bottom
                                            1.0f, -1.0f, -1.0f,
                                            -1.0f, -1.0f, -1.0f,
                                            -1.0f, -1.0f,  1.0f,
                                             1.0f, -1.0f,  1.0f,

                                             // front
                                             1.0f, 1.0f, 1.0f,
                                            -1.0f, 1.0f, 1.0f,
                                            -1.0f, -1.0f, 1.0f,
                                             1.0f, -1.0f, 1.0f,

                                             // back
                                             1.0f, 1.0f, -1.0f,
                                            -1.0f, 1.0f, -1.0f,
                                            -1.0f, -1.0f, -1.0f,
                                             1.0f, -1.0f, -1.0f,

                                             // right
                                             1.0f, 1.0f, -1.0f,
                                             1.0f, 1.0f, 1.0f,
                                             1.0f, -1.0f, 1.0f,
                                             1.0f, -1.0f, -1.0f,

                                             // left
                                            -1.0f, 1.0f, 1.0f,
                                            -1.0f, 1.0f, -1.0f,
                                            -1.0f, -1.0f, -1.0f,
                                            -1.0f, -1.0f, 1.0f,

                                        };

    const GLfloat cubeTexcoord[] =        {
                                            0.0f, 0.0f,
                                            1.0f, 0.0f,
                                            1.0f, 1.0f,
                                            0.0f, 1.0f,

                                            0.0f, 0.0f,
                                            1.0f, 0.0f,
                                            1.0f, 1.0f,
                                            0.0f, 1.0f,

                                            0.0f, 0.0f,
                                            1.0f, 0.0f,
                                            1.0f, 1.0f,
                                            0.0f, 1.0f,

                                            0.0f, 0.0f,
                                            1.0f, 0.0f,
                                            1.0f, 1.0f,
                                            0.0f, 1.0f,

                                            0.0f, 0.0f,
                                            1.0f, 0.0f,
                                            1.0f, 1.0f,
                                            0.0f, 1.0f,

                                            0.0f, 0.0f,
                                            1.0f, 0.0f,
                                            1.0f, 1.0f,
                                            0.0f, 1.0f,
                                        };

    // vao And vbo Related Code
    // Cube
    glGenVertexArrays(1, &vao_cube);
    glBindVertexArray(vao_cube);

        // vbo For Position
        glGenBuffers(1, &vbo_cube_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubePosition), cubePosition, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // vbo For Color
        glGenBuffers(1, &vbo_cube_texcoord);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoord);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoord), cubeTexcoord, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_TEXTURE0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    
    
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Enabling The Texture
    glEnable(GL_TEXTURE_2D);

    perspectiveProjectionMatrix = mat4::identity();


    // FBO Code
    bFBOResult = [self createFBO : FBO_WIDTH : FBO_HEIGHT];
    int iRetVal;
    if (bFBOResult == true)
    {
        iRetVal = [self initialize_sphere : FBO_WIDTH : FBO_HEIGHT];

        // Here There Should Be Error Checking Of FBO
    }
    else
    {
        printf("Create FBO Failed...\n");
        return -6;
    }
    
    
    return 0;
}

- (bool)createFBO : (GLint)textureWidth : (GLint)textureHeight
{
    // Local Variable Declarations
    int maxRenderbufferSize;

    // Code
    // Check Available Renderbuffer Size
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);
    if (maxRenderbufferSize < textureWidth || maxRenderbufferSize < textureHeight)
    {
        printf("Insufficient Render Buffer Size \n");
        return false;
    }

    // Create Framebuffer Object
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create Renderbuffer Object
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    // Storage And Format Of The Renderbuffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, textureWidth, textureHeight);

    // Create Empty Texture For Upcoming Target Scene
    glGenTextures(1, &fbo_texture);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 textureWidth,
                 textureHeight,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_SHORT_5_6_5,
                 NULL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);

    // Give Renderbuffer To FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check Whether The Framebuffeer Created Successful or Not
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Frame Buffer Is Not Complete \n");
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

- (int)initialize_sphere : (int)width : (int)height
{
    // Variable Declarations
    GLint status;
    GLint infoLogLength;
    char* log = NULL;

    // Function Declarations
    void resize_sphere(int, int);
    void uninitialize_sphere(void);

    // Vertex Shader PerFragment
    const GLchar* vertexShaderSourceCode_pf =
        "#version 300 es" \
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
                printf("Vertex Shader Compilation Log: %s\n", log);
                free(log);
                [self uninitialize_sphere];
                [self release];
                exit(0);
            }
        }
    }


    // Fragment Shader PerFragment
    const GLchar* fragmentShaderSourceCode_pf =
        "#version 300 es" \
        "\n" \
        "precision highp float;" \
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
        "out vec4 FragColor;" \
        "precision highp int;" \
        "uniform int u_lightingEnabled;" \
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
                printf("Fragment Shader Compilation Log: %s\n", log);
                free(log);
                [self uninitialize_sphere];
                [self release];
                exit(0);
            }
        }
    }


    // Shader Program Object PerFragment

    shaderProgramObject_sphere = glCreateProgram();
    glAttachShader(shaderProgramObject_sphere, vertexShaderObject_pf);
    glAttachShader(shaderProgramObject_sphere, fragmentShaderObject_pf);
    glBindAttribLocation(shaderProgramObject_sphere, SBJ_ATTRIBUTE_POSITION, "a_position");
    glBindAttribLocation(shaderProgramObject_sphere, SBJ_ATTRIBUTE_NORMAL, "a_normal");
    glLinkProgram(shaderProgramObject_sphere);

    status = 0;
    infoLogLength = 0;
    log = NULL;
    glGetProgramiv(shaderProgramObject_sphere, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject_sphere, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            log = (char*)malloc(infoLogLength);
            if (log != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(shaderProgramObject_sphere, infoLogLength, &written, log);
                printf("Shader Program Link Log: %s\n", log);
                free(log);
                [self uninitialize_sphere];
                [self release];
                exit(0);
            }
        }
    }
    modelMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_modelMatrix");
    viewMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_viewMatrix");
    projectionMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_projectionMatrix");
    laUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_sphere, "u_la[0]");
    laUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_sphere, "u_la[1]");
    laUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_sphere, "u_la[2]");
    ldUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_sphere, "u_ld[0]");
    ldUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_sphere, "u_ld[1]");
    ldUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_sphere, "u_ld[2]");
    lsUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_sphere, "u_ls[0]");
    lsUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_sphere, "u_ls[1]");
    lsUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_sphere, "u_ls[2]");
    lightPositionUniform_sphere[0] = glGetUniformLocation(shaderProgramObject_sphere, "u_lightPosition[0]");
    lightPositionUniform_sphere[1] = glGetUniformLocation(shaderProgramObject_sphere, "u_lightPosition[1]");
    lightPositionUniform_sphere[2] = glGetUniformLocation(shaderProgramObject_sphere, "u_lightPosition[2]");
    kaUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_ka");
    kdUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_kd");
    ksUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_ks");
    materialShininessUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_materialShininess");
    lightingEnabledUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "u_lightingEnabled");



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
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // glShadeModel(),glHint() Deprecated In PP

    // Clear The Screen Using Black Color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    lights_sphere[0].lightAmbient_sphere = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights_sphere[0].lightDiffuse_sphere = vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    lights_sphere[0].lightSpecular_sphere = vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    lights_sphere[0].lightPosition_sphere = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    lights_sphere[1].lightAmbient_sphere = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights_sphere[1].lightDiffuse_sphere = vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    lights_sphere[1].lightSpecular_sphere = vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    lights_sphere[1].lightPosition_sphere = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    lights_sphere[2].lightAmbient_sphere = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights_sphere[2].lightDiffuse_sphere = vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    lights_sphere[2].lightSpecular_sphere = vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    lights_sphere[2].lightPosition_sphere = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    perspectiveProjectionMatrix = mat4::identity();

    // Warmup resize() Call
    [self resize_sphere : FBO_WIDTH : FBO_HEIGHT];

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
        printf("failed to allpcate memeory for sphere data\n");
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
    winWidth = width;
    winHeight = height;
    
    if (height <= 0)
        height = 1;
    
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

- (void)resize_sphere : (int)width : (int)height
{
    // Code
    if (height == 0)
        height = 1;                    // To avoid 'Divide By 0' in future

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    perspectiveProjectionMatrix_sphere = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

- (void)display
{
    // Code
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    [self resize : winWidth : winHeight];

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use The Shader Program Object
    glUseProgram(shaderProgramObject);

    // Cube
    // Transformations
    mat4 translationMatrix = mat4::identity();
    mat4 scaleMatrix = mat4::identity();
    mat4 rotationMatrix_x = mat4::identity();
    mat4 rotationMatrix_y = mat4::identity();
    mat4 rotationMatrix_z = mat4::identity();
    mat4 rotationMatrix = mat4::identity();
    mat4 modelViewMatrix = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();

    translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    scaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);
    rotationMatrix_x = vmath::rotate(angleCube, 1.0f, 0.0f, 0.0f);
    rotationMatrix_y = vmath::rotate(angleCube, 0.0f, 1.0f, 0.0f);
    rotationMatrix_z = vmath::rotate(angleCube, 0.0f, 0.0f, 1.0f);
    rotationMatrix = rotationMatrix_x * rotationMatrix_y * rotationMatrix_z;
    modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;                // Here Order Is Very Important
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);                            // 80 Textures Can Be Activated
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glUniform1i(textureSamplerUniform, 0);

    glBindVertexArray(vao_cube);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Unuse The Shader Program Object
    glUseProgram(0);
}

- (void)myUpdate
{
    // Code
    angleCube = angleCube + 0.5f;
    if (angleCube >= 360.0f)
        angleCube = angleCube - 360.0f;
}

- (void)display_sphere : (GLint)textureWidth : (GLint)textureHeight
{
    // Code
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    [self resize_sphere : textureWidth : textureHeight];

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use The Shader Program Object
    glUseProgram(shaderProgramObject_sphere);

    // Transformations
    mat4 translationMatrix = mat4::identity();
    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();

    translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    modelMatrix = translationMatrix;
    glUniformMatrix4fv(modelMatrixUniform_sphere, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform_sphere, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform_sphere, 1, GL_FALSE, perspectiveProjectionMatrix_sphere);

    // Sending Light Related Uniforms
    if (bLight == TRUE)
    {
        glUniform1i(lightingEnabledUniform_sphere, 1);
        for (int i = 0; i < 3; i++)
        {
            glUniform3fv(laUniform_sphere[i], 1, lights_sphere[i].lightAmbient_sphere);
            glUniform3fv(ldUniform_sphere[i], 1, lights_sphere[i].lightDiffuse_sphere);
            glUniform3fv(lsUniform_sphere[i], 1, lights_sphere[i].lightSpecular_sphere);
            glUniform4fv(lightPositionUniform_sphere[i], 1, lights_sphere[i].lightPosition_sphere);
        }

        glUniform3fv(kaUniform_sphere, 1, materialAmbient_sphere);
        glUniform3fv(kdUniform_sphere, 1, materialDiffuse_sphere);
        glUniform3fv(ksUniform_sphere, 1, materialSpecular_sphere);
        glUniform1f(materialShininessUniform_sphere, materialShininess_sphere);
    }
    else
    {
        glUniform1i(lightingEnabledUniform_sphere, 0);
    }

    glBindVertexArray(vao_Sphere);

    // Draw, either by 'glDrawTriangles()' or 'glDrawArrays()' or 'glDrawElements()'
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    // Unuse The Shader Program Object
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

- (void)update_sphere
{
    // Code

    lightAngleZero_sphere += 2.0f;
    lightAngleOne_sphere += 2.0f;
    lightAngleTwo_sphere += 2.0f;

    lights_sphere[0].lightPosition_sphere[1] = 10 * sin(lightAngleZero_sphere / 180.0f * 3.14f);
    lights_sphere[0].lightPosition_sphere[2] = 10 * cos(lightAngleZero_sphere / 180.0f * 3.14f);

    lights_sphere[1].lightPosition_sphere[0] = 10 * sin(lightAngleZero_sphere * 3.14f / 180.0f);
    lights_sphere[1].lightPosition_sphere[2] = 10 * cos(lightAngleZero_sphere * 3.14f / 180.0f);

    lights_sphere[2].lightPosition_sphere[0] = 10 * -sin(lightAngleZero_sphere * 3.14f / 180.0f);
    lights_sphere[2].lightPosition_sphere[1] = 10 * cos(lightAngleZero_sphere * 3.14f / 180.0f);

}

- (void)uninitialize
{
    // Code
    [self uninitialize_sphere];

    //  Deletion And Uninitialization of vbo
    if (vbo_cube_texcoord)
    {
        glDeleteBuffers(1, &vbo_cube_texcoord);
        vbo_cube_texcoord = 0;
    }

    if (vbo_cube_position)
    {
        glDeleteBuffers(1, &vbo_cube_position);
        vbo_cube_position = 0;
    }

    // Deletion And Uninitialization of vao
    if (vao_cube)
    {
        glDeleteVertexArrays(1, &vao_cube);
        vao_cube = 0;
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

- (void)uninitialize_sphere
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
    if (shaderProgramObject_sphere)
    {
        glUseProgram(shaderProgramObject_sphere);
        GLsizei numAttachedShaders;
        glGetProgramiv(shaderProgramObject_sphere, GL_ATTACHED_SHADERS, &numAttachedShaders);
        GLuint* shaderObjects = NULL;
        shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
        //if (shaderObject)
            // Error Checking
        glGetAttachedShaders(shaderProgramObject_sphere, numAttachedShaders, &numAttachedShaders, shaderObjects);
        for (GLsizei i = 0; i < numAttachedShaders; i++)
        {
            glDetachShader(shaderProgramObject_sphere, shaderObjects[i]);
            glDeleteShader(shaderObjects[i]);
            shaderObjects[i] = 0;
        }
        free(shaderObjects);
        shaderObjects = NULL;
        glUseProgram(0);
        glDeleteProgram(shaderProgramObject_sphere);
        shaderProgramObject_sphere = 0;
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
    if (bLight == FALSE)
    {
        bLight = TRUE;
    }
    else
    {
        bLight = FALSE;
    }
}

- (void)onDoubleTap:(UITapGestureRecognizer*)tgr
{
    // Code
    
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
