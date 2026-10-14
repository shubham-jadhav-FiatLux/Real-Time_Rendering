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
    
    // Programmable Pipeline Related Global Variables
    GLuint shaderProgramObject;

    enum
    {
        SBJ_ATTRIBUTE_POSITION = 0,
        SBJ_ATTRIBUTE_COLOR,
        SBJ_ATTRIBUTE_NORMAL,
        SBJ_ATTRIBUTE_TEXTURE0
    };

    GLuint vao_pyramid;
    GLuint vbo_pyramid_position;
    GLuint vbo_pyramid_normal;

    GLfloat anglePyramid;

    BOOL bLight;

    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;

    GLuint laUniform[2];                // Ambient
    GLuint ldUniform[2];                // Diffuse
    GLuint lsUniform[2];                // Specular
    GLuint lightPositionUniform[2];

    struct Light
    {
        vec4 lightAmbient;
        vec4 lightDiffuse;
        vec4 lightSpecular;
        vec4 lightPosition;
    };
    Light lights[2];

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
    anglePyramid = 0.0f;
    
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
    
    // Vertex Shader
    const GLchar* vertexShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "in vec4 a_position;" \
        "in vec3 a_normal;" \
        "uniform mat4 u_modelMatrix;" \
        "uniform mat4 u_viewMatrix;" \
        "uniform mat4 u_projectionMatrix;" \
        "uniform vec3 u_la[2];" \
        "uniform vec3 u_ld[2];" \
        "uniform vec3 u_ls[2];" \
        "uniform vec4 u_lightPosition[2];" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_ks;" \
        "uniform float u_materialShininess;" \
        "uniform int u_lightingEnabled;" \
        "out vec3 phong_ads_light;" \
        "void main(void)" \
        "{" \
        "if (u_lightingEnabled == 1)" \
        "{" \
        "vec3 array[2];" \
        "vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" \
        "mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
        "vec3 transformedNormals = normalize(normalMatrix * a_normal);" \
        "vec3 viewerVector = normalize(-eyeCoordinates.xyz);" \
        "vec3 ambient[2];" \
        "vec3 lightDirection[2];" \
        "vec3 diffuse[2];" \
        "vec3 reflectionVector[2];" \
        "vec3 specular[2];" \
        "for (int i = 0; i < 2; i++)" \
        "{" \
        "ambient[i] = u_la[i] * u_ka;" \
        "lightDirection[i] = normalize(vec3(u_lightPosition[i]) - eyeCoordinates.xyz);" \
        "diffuse[i] = u_ld[i] * u_kd * max(dot(lightDirection[i], transformedNormals), 0.0);" \
        "reflectionVector[i] = reflect(-lightDirection[i], transformedNormals);" \
        "specular[i] = u_ls[i] * u_ks * pow(max(dot(reflectionVector[i], viewerVector), 0.0), u_materialShininess);" \
        "array[i] = ambient[i] + diffuse[i] + specular[i];" \
        "}" \
        "phong_ads_light = array[0] + array[1];" \
        "}" \
        "else" \
        "{" \
        "phong_ads_light = vec3(1.0, 1.0, 1.0);" \
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
        "in vec3 phong_ads_light;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
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
    laUniform[0] = glGetUniformLocation(shaderProgramObject, "u_la[0]");
    laUniform[1] = glGetUniformLocation(shaderProgramObject, "u_la[1]");
    ldUniform[0] = glGetUniformLocation(shaderProgramObject, "u_ld[0]");
    ldUniform[1] = glGetUniformLocation(shaderProgramObject, "u_ld[1]");
    lsUniform[0] = glGetUniformLocation(shaderProgramObject, "u_ls[0]");
    lsUniform[1] = glGetUniformLocation(shaderProgramObject, "u_ls[1]");
    lightPositionUniform[0] = glGetUniformLocation(shaderProgramObject, "u_lightPosition[0]");
    lightPositionUniform[1] = glGetUniformLocation(shaderProgramObject, "u_lightPosition[1]");
    kaUniform = glGetUniformLocation(shaderProgramObject, "u_ka");
    kdUniform = glGetUniformLocation(shaderProgramObject, "u_kd");
    ksUniform = glGetUniformLocation(shaderProgramObject, "u_ks");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_materialShininess");
    lightingEnabledUniform = glGetUniformLocation(shaderProgramObject, "u_lightingEnabled");

    // Declaration Of Vertex Data Arrays
    const GLfloat pyramidPosition[] =    {
                                            // front
                                            0.0f, 1.0f, 0.0f,
                                            -1.0f, -1.0f, 1.0f,
                                            1.0f, -1.0f, 1.0f,

                                            // right
                                            0.0f, 1.0f, 0.0f,
                                            1.0f, -1.0f, 1.0f,
                                            1.0f, -1.0f, -1.0f,

                                            // back
                                            0.0f, 1.0f, 0.0f,
                                            1.0f, -1.0f, -1.0f,
                                            -1.0f, -1.0f, -1.0f,

                                            // left
                                            0.0f, 1.0f, 0.0f,
                                            -1.0f, -1.0f, -1.0f,
                                            -1.0f, -1.0f, 1.0f

                                        };

    const GLfloat pyramidNormals[] = {
                                            0.0f, 0.447214f, 0.894427f,// front-top
                                            0.0f, 0.447214f, 0.894427f,// front-left
                                            0.0f, 0.447214f, 0.894427f,// front-right

                                            0.894427f, 0.447214f, 0.0f, // right-top
                                            0.894427f, 0.447214f, 0.0f, // right-left
                                            0.894427f, 0.447214f, 0.0f, // right-right

                                            0.0f, 0.447214f, -0.894427f, // back-top
                                            0.0f, 0.447214f, -0.894427f, // back-left
                                            0.0f, 0.447214f, -0.894427f, // back-right

                                            -0.894427f, 0.447214f, 0.0f, // left-top
                                            -0.894427f, 0.447214f, 0.0f, // left-left
                                            -0.894427f, 0.447214f, 0.0f // left-right

                                    };

    // vao And vbo Related Code
    // Pyramid
    glGenVertexArrays(1, &vao_pyramid);
    glBindVertexArray(vao_pyramid);

        // vbo For Position
        glGenBuffers(1, &vbo_pyramid_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPosition), pyramidPosition, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // vbo For Normal
        glGenBuffers(1, &vbo_pyramid_normal);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_normal);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_NORMAL);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    
    
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    lights[0].lightAmbient = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights[0].lightDiffuse = vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    lights[0].lightSpecular = vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    lights[0].lightPosition = vmath::vec4(-2.0f, 0.0f, 0.0f, 1.0f);

    lights[1].lightAmbient = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lights[1].lightDiffuse = vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    lights[1].lightSpecular = vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    lights[1].lightPosition = vmath::vec4(2.0f, 0.0f, 0.0f, 1.0f);
    
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
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLORglGenRenderbuffers | GL_DEPTH_BUFFER_BIT);
 
    // Use The Shader Program Object
    glUseProgram(shaderProgramObject);

    // Pyramid
    // Transformations
    mat4 translationMatrix = mat4::identity();
    mat4 rotationMatrix = mat4::identity();
    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();

    translationMatrix = vmath::translate(0.0f, 0.0f, -5.0f);
    rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);
    modelMatrix = translationMatrix * rotationMatrix;                            // Here Order Is Very Important
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    // Sending Light Related Uniforms
    if (bLight == TRUE)
    {
        glUniform1i(lightingEnabledUniform, 1);
        for (int i = 0; i < 2; i++)
        {
            glUniform3fv(laUniform[i], 1, lights[i].lightAmbient);
            glUniform3fv(ldUniform[i], 1, lights[i].lightDiffuse);
            glUniform3fv(lsUniform[i], 1, lights[i].lightSpecular);
            glUniform4fv(lightPositionUniform[i], 1, lights[i].lightPosition);
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

    glBindVertexArray(vao_pyramid);
    
    glDrawArrays(GL_TRIANGLES, 0, 12);

    glBindVertexArray(0);

    // Unuse The Shader Program Object
    glUseProgram(0);
}

- (void)myUpdate
{
    // Code
    anglePyramid = anglePyramid + 1.0f;
    if (anglePyramid >= 360.0f)
        anglePyramid = anglePyramid - 360.0f;
}

- (void)uninitialize
{
    // Code
    //  Deletion And Uninitialization of vbo
    if (vbo_pyramid_normal)
    {
        glDeleteBuffers(1, &vbo_pyramid_normal);
        vbo_pyramid_normal = 0;
    }

    if (vbo_pyramid_position)
    {
        glDeleteBuffers(1, &vbo_pyramid_position);
        vbo_pyramid_position = 0;
    }

    // Deletion And Uninitialization of vao
    if (vao_pyramid)
    {
        glDeleteVertexArrays(1, &vao_pyramid);
        vao_pyramid = 0;
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
        bLight = TRUE;
    else
        bLight = FALSE;
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
