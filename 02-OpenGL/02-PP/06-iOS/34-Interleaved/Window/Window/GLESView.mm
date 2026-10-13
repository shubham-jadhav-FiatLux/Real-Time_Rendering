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

    GLuint vao_cube;
    GLuint vbo_cube;

    // Texture
    GLfloat angleCube;
    GLuint texture_marble;
    GLuint textureSamplerUniform;

    //Light
    GLuint laUniform;                // Ambient
    GLuint ldUniform;                // Diffuse
    GLuint lsUniform;                // Specular
    GLuint lightPositionUniform;

    GLuint kaUniform;
    GLuint kdUniform;
    GLuint ksUniform;
    GLuint materialShininessUniform;

    GLuint lightingEnabledUniform;

    GLfloat lightAmbient[4];
    GLfloat lightDiffuse[4];
    GLfloat lightSpecular[4];
    GLfloat lightPosition[4];

    GLfloat materialAmbient[4];
    GLfloat materialDiffuse[4];
    GLfloat materialSpecular[4];
    GLfloat materialShininess;

    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;

    mat4 perspectiveProjectionMatrix;
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
    bLight = FALSE;
    angleCube = 0.0f;
    
    lightAmbient[0] = 0.1f;
    lightAmbient[1] = 0.1f;
    lightAmbient[2] = 0.1f;
    lightAmbient[3] = 1.0f;
    
    lightDiffuse[0] = 1.0f;
    lightDiffuse[1] = 1.0f;
    lightDiffuse[2] = 1.0f;
    lightDiffuse[3] = 1.0f;
    
    lightSpecular[0] = 1.0f;
    lightSpecular[1] = 1.0f;
    lightSpecular[2] = 1.0f;
    lightSpecular[3] = 1.0f;
    
    lightPosition[0] = 0.0f;
    lightPosition[1] = 0.0f;
    lightPosition[2] = 100.0f;
    lightPosition[3] = 1.0f;

    materialAmbient[0] = 0.0f;
    materialAmbient[1] = 0.0f;
    materialAmbient[2] = 0.0f;
    materialAmbient[3] = 1.0f;
    
    materialDiffuse[0] = 0.5f;
    materialDiffuse[1] = 0.2f;
    materialDiffuse[2] = 0.7f;
    materialDiffuse[3] = 1.0f;
    
    materialSpecular[0] = 0.7f;
    materialSpecular[1] = 0.7f;
    materialSpecular[2] = 0.7f;
    materialSpecular[3] = 1.0f;
    
    materialShininess = 50.0f;
    
    // Vertex Shader
    const GLchar* vertexShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "in vec4 a_position;" \
        "in vec4 a_color;" \
        "in vec3 a_normal;" \
        "in vec2 a_texcoord;" \
        "uniform mat4 u_modelMatrix;" \
        "uniform mat4 u_viewMatrix;" \
        "uniform mat4 u_projectionMatrix;" \
        "uniform vec4 u_lightPosition;" \
        "uniform int u_lightingEnabled;" \
        "out vec4 a_color_out;" \
        "out vec3 transformedNormals;" \
        "out vec3 lightDirection;" \
        "out vec3 viewerVector;" \
        "out vec2 a_texcoord_out;" \
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
        "a_color_out = a_color;" \
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
        "in vec4 a_color_out;" \
        "in vec3 transformedNormals;" \
        "in vec3 lightDirection;" \
        "in vec3 viewerVector;" \
        "in vec2 a_texcoord_out;" \
        "uniform vec3 u_la;" \
        "uniform vec3 u_ld;" \
        "uniform vec3 u_ls;" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_ks;" \
        "uniform float u_materialShininess;" \
        "uniform sampler2D u_textureSampler;" \
        "out vec4 FragColor;" \
        "precision highp int;" \
        "uniform int u_lightingEnabled;" \
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
        "FragColor = vec4(vec3(texture(u_textureSampler, a_texcoord_out)) * phong_ads_light * vec3(a_color_out), 1.0);" \
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
    glBindAttribLocation(shaderProgramObject, SBJ_ATTRIBUTE_NORMAL, "a_normal");
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
    textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_textureSampler");

    // Declaration Of Vertex Data Arrays

    const GLfloat cubePCNT[] = {
                                    // positions                 //colors               //normals                   //texture

                                    // Top face               // Top face                 // Top face              // Top face
                                    1.0f, 1.0f, -1.0f,       0.0f,0.0f,1.0f,          0.0f, 1.0f, 0.0f,            1.0f,1.0f,
                                    -1.0f, 1.0f, -1.0f,      0.0f,0.0f,1.0f,          0.0f, 1.0f, 0.0f,            0.0f,1.0f,
                                    -1.0f, 1.0f, 1.0f,       0.0f,0.0f,1.0f,          0.0f, 1.0f, 0.0f,            0.0f,0.0f,
                                    1.0f, 1.0f, 1.0f,        0.0f,0.0f,1.0f,          0.0f, 1.0f, 0.0f,            1.0f,0.0f,

                                    // Bottom face            // Bottom face             // Bottom face            // Bottom face
                                    1.0f, -1.0f, -1.0f,      0.0f,1.0f,0.0f,         0.0f, -1.0f, 0.0f,            1.0f,1.0f,
                                    -1.0f, -1.0f, -1.0f,     0.0f,1.0f,0.0f,         0.0f, -1.0f, 0.0f,            0.0f,1.0f,
                                    -1.0f, -1.0f, 1.0f,      0.0f,1.0f,0.0f,         0.0f, -1.0f, 0.0f,            0.0f,0.0f,
                                    1.0f, -1.0f, 1.0f,       0.0f,1.0f,0.0f,         0.0f, -1.0f, 0.0f,            1.0f,0.0f,

                                    // Front face             // Front face              // Front face             // Front face
                                    1.0f, 1.0f, 1.0f,        1.0f,0.0f,0.0f,         0.0f, 0.0f, 1.0f,             1.0f,1.0f,
                                    -1.0f, 1.0f, 1.0f,       1.0f,0.0f,0.0f,         0.0f, 0.0f, 1.0f,             0.0f,1.0f,
                                    -1.0f, -1.0f, 1.0f,      1.0f,0.0f,0.0f,         0.0f, 0.0f, 1.0f,             0.0f,0.0f,
                                    1.0f, -1.0f, 1.0f,       1.0f,0.0f,0.0f,         0.0f, 0.0f, 1.0f,             1.0f,0.0f,

                                    // Back face              // Back face              // Back face              // Back face
                                    1.0f, 1.0f, -1.0f,       0.0f,1.0f,1.0f,         0.0f, 0.0f, -1.0f,             1.0f,1.0f,
                                    -1.0f, 1.0f, -1.0f,      0.0f,1.0f,1.0f,         0.0f, 0.0f, -1.0f,             0.0f,1.0f,
                                    -1.0f, -1.0f, -1.0f,     0.0f,1.0f,1.0f,         0.0f, 0.0f, -1.0f,             0.0f,0.0f,
                                    1.0f, -1.0f, -1.0f,      0.0f,1.0f,1.0f,         0.0f, 0.0f, -1.0f,             1.0f,0.0f,

                                    // Right face              // Right face              // Right face              // Right face
                                    1.0f, 1.0f, -1.0f,      1.0f,0.0f,1.0f,         1.0f, 0.0f, 0.0f,               1.0f,1.0f,
                                    1.0f, 1.0f, 1.0f,       1.0f,0.0f,1.0f,         1.0f, 0.0f, 0.0f,               0.0f,1.0f,
                                    1.0f, -1.0f, 1.0f,      1.0f,0.0f,1.0f,         1.0f, 0.0f, 0.0f,               0.0f,0.0f,
                                    1.0f, -1.0f, -1.0f,     1.0f,0.0f,1.0f,         1.0f, 0.0f, 0.0f,               1.0f,0.0f,

                                    // Left face                // Left face           // Left face                // Left face
                                    -1.0f, 1.0f, 1.0f,      1.0f,1.0f,0.0f,         -1.0f, 0.0f, 0.0f,              1.0f,1.0f,
                                    -1.0f, 1.0f, -1.0f,     1.0f,1.0f,0.0f,         -1.0f, 0.0f, 0.0f,              0.0f,1.0f,
                                    -1.0f, -1.0f, -1.0f,    1.0f,1.0f,0.0f,         -1.0f, 0.0f, 0.0f,              0.0f,0.0f,
                                    -1.0f, -1.0f, 1.0f,     1.0f,1.0f,0.0f,         -1.0f, 0.0f, 0.0f,              1.0f,0.0f,
                                };

    // vao And vbo Related Code
    // Cube
    glGenVertexArrays(1, &vao_cube);
    glBindVertexArray(vao_cube);

        // vbo For Cube
        glGenBuffers(1, &vbo_cube);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubePCNT), cubePCNT, GL_STATIC_DRAW);        // sizeof(cubePCNT) is nothing but 11*24*sizeof(float) or 264*sizeof(float)

        // Position
        glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(0));
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);

        // Color
        glVertexAttribPointer(SBJ_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_COLOR);

        // Normal
        glVertexAttribPointer(SBJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_NORMAL);

        // Texcoord
        glVertexAttribPointer(SBJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(9 * sizeof(GLfloat)));
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_TEXTURE0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        

    glBindVertexArray(0);
    
    
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    texture_marble = [self loadGLTexture:@"marble" :@"bmp"];
    if (texture_marble == 0)
    {
        [self uninitialize];
        [self release];
        exit(0);
    }

    // Enabling The Texture
    glEnable(GL_TEXTURE_2D);
    
    perspectiveProjectionMatrix = mat4::identity();
    
    return 0;
}

- (GLuint)loadGLTexture: (NSString*)textureFileName :(NSString*)extension
{
    // Code
    NSBundle* appBundle = [NSBundle mainBundle];
    
    NSString* textureFileNameWithExtension = [appBundle pathForResource:textureFileName ofType:extension];
    
    // Get NSImage Representation of texture file
    UIImage* image = [[UIImage alloc]initWithContentsOfFile:textureFileNameWithExtension];
    if (image == nil)
    {
        printf("\n[ERROR] : Failed To Obtain NSImage From Texture File...\n");
        return 0;
    }
    
    // From above NSImage representation obtain CGImage representation of texture file
    CGImageRef cgImage = [image CGImage];
    
    // From this CGImage representation get width and height of the image
    int width = (int)CGImageGetWidth(cgImage);
    int height = (int)CGImageGetHeight(cgImage);
    
    // From this CGImage representation get CGData provider
    CGDataProviderRef cgDataProvider = CGImageGetDataProvider(cgImage);
    
    // Using CGData provider get image data in the form of CFData
    CFDataRef imageData = CGDataProviderCopyData(cgDataProvider);
    
    // Convert this CFData formated image data into void*
    void* pixels = (void*)CFDataGetBytePtr(imageData);
    
    // Proceed with usual texture creation code here onwards
    GLuint texture = 0;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 width,
                 height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 pixels);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Unbind Texture Object
    glBindTexture(GL_TEXTURE_2D, 0);
    
    CFRelease(imageData);
    
    return texture;
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
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLORglGenRenderbuffers | GL_DEPTH_BUFFER_BIT);
 
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
    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();

    translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
    scaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);
    rotationMatrix_x = vmath::rotate(angleCube, 1.0f, 0.0f, 0.0f);
    rotationMatrix_y = vmath::rotate(angleCube, 0.0f, 1.0f, 0.0f);
    rotationMatrix_z = vmath::rotate(angleCube, 0.0f, 0.0f, 1.0f);
    rotationMatrix = rotationMatrix_x * rotationMatrix_y * rotationMatrix_z;
    modelMatrix = translationMatrix * rotationMatrix;                // Here Order Is Very Important

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    // Sending Light Related Uniforms
    if (bLight == TRUE)
    {
        glUniform1i(lightingEnabledUniform, 1);

        glUniform3fv(laUniform, 1, lightAmbient);
        glUniform3fv(ldUniform, 1, lightDiffuse);
        glUniform3fv(lsUniform, 1, lightSpecular);
        glUniform4fv(lightPositionUniform, 1, lightPosition);

        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDiffuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        glUniform1f(materialShininessUniform, materialShininess);
    }
    else
    {
        glUniform1i(lightingEnabledUniform, 0);
    }

    glActiveTexture(GL_TEXTURE0);                            // 80 Textures Can Be Activated
    glBindTexture(GL_TEXTURE_2D, texture_marble);
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
    angleCube = angleCube + 1.0f;
    if (angleCube >= 360.0f)
        angleCube = angleCube - 360.0f;
}

- (void)uninitialize
{
    // Code
    if (texture_marble)
    {
        glDeleteTextures(1, &texture_marble);
        texture_marble = 0;
    }

    //  Deletion And Uninitialization of vbo

    if (vbo_cube)
    {
        glDeleteBuffers(1, &vbo_cube);
        vbo_cube = 0;
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
