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
    
    currentWidth = backingWidth;
    currentHeight = backingHeight;
    
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
    bLight  = FALSE;
    KeyPressed = 0;
    AngleForXRotation = 0.0f;
    AngleForYRotation = 0.0f;
    AngleForZRotation = 0.0f;
    
    CGRect rect = [self bounds];
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
        "#version 300 es" \
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
    
    
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
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
    if (height <= 0)
        height = 1;
    
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width * 1.4f / (GLfloat)height, 0.1f, 200.0f);
}

- (void)display
{
    // Code
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLORglGenRenderbuffers | GL_DEPTH_BUFFER_BIT);
 
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
    if (KeyPressed == 0)
    {
        KeyPressed = 1;
        AngleForXRotation = 0.0f;        // Reset
        AngleForYRotation = 0.0f;
        AngleForZRotation = 0.0f;
    }
    else if (KeyPressed == 1)
    {
        KeyPressed = 2;
        AngleForXRotation += 0.0f;
        AngleForYRotation = 0.0f;
        AngleForZRotation = 0.0f;
    }
    else if (KeyPressed == 2)
    {
        KeyPressed = 3;
        AngleForXRotation += 0.0f;
        AngleForYRotation = 0.0f;
        AngleForZRotation = 0.0f;
    }
    else
        KeyPressed = 0;
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
