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

    GLuint vao;
    GLuint vbo_position;
    GLuint vbo_texcoord;

    GLuint texture_smiley;
    int keyPressed;

    GLuint mvpMatrixUniform;
    GLuint textureSamplerUniform;
    GLuint keypressUniform;

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
    keyPressed = -1;
    
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
        "uniform int u_keyPressed;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "if (u_keyPressed == 1)" \
        "{" \
        "FragColor = texture(u_textureSampler, a_texcoord_out);" \
        "}" \
        "else" \
        "{" \
        "FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);" \
        "}" \
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
    keypressUniform = glGetUniformLocation(shaderProgramObject, "u_keyPressed");

    // Declaration Of Vertex Data Arrays
    const GLfloat position[] =    {
                                    1.0f, 1.0f, 0.0f,
                                    -1.0f, 1.0f, 0.0f,
                                    -1.0f, -1.0f, 0.0f,
                                    1.0f, -1.0f, 0.0f
                                };

    // vao And vbo Related Code
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

        // vbo For Position
        glGenBuffers(1, &vbo_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
        glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_POSITION);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // vbo For Texcoord
        glGenBuffers(1, &vbo_texcoord);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
        glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(SBJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SBJ_ATTRIBUTE_TEXTURE0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    
    
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    texture_smiley = [self loadGLTexture:@"Smiley" :@"bmp"];
    if (texture_smiley == 0)
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

    // Transformations
    mat4 translationMatrix = mat4::identity();
    mat4 modelViewMatrix = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();

    translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    modelViewMatrix = translationMatrix ;                            // Here Order Is Very Important
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_smiley);
    glUniform1i(textureSamplerUniform, 0);

    glBindVertexArray(vao);
    
    GLfloat texCoord[8];

    if (keyPressed == 1)
    {
        texCoord[0] = 0.5f;
        texCoord[1] = 0.5f;
        texCoord[2] = 0.0f;
        texCoord[3] = 0.5f;
        texCoord[4] = 0.0f;
        texCoord[5] = 0.0f;
        texCoord[6] = 0.5f;
        texCoord[7] = 0.0f;

        glUniform1i(keypressUniform, 1);
    }
    else if (keyPressed == 2)
    {
        texCoord[0] = 1.0f;
        texCoord[1] = 1.0f;
        texCoord[2] = 0.0f;
        texCoord[3] = 1.0f;
        texCoord[4] = 0.0f;
        texCoord[5] = 0.0f;
        texCoord[6] = 1.0f;
        texCoord[7] = 0.0f;

        glUniform1i(keypressUniform, 1);
    }
    else if (keyPressed == 3)
    {
        texCoord[0] = 2.0f;
        texCoord[1] = 2.0f;
        texCoord[2] = 0.0f;
        texCoord[3] = 2.0f;
        texCoord[4] = 0.0f;
        texCoord[5] = 0.0f;
        texCoord[6] = 2.0f;
        texCoord[7] = 0.0f;

        glUniform1i(keypressUniform, 1);
    }
    else if (keyPressed == 4)
    {
        texCoord[0] = 0.5f;
        texCoord[1] = 0.5f;
        texCoord[2] = 0.5f;
        texCoord[3] = 0.5f;
        texCoord[4] = 0.5f;
        texCoord[5] = 0.5f;
        texCoord[6] = 0.5f;
        texCoord[7] = 0.5f;

        glUniform1i(keypressUniform, 1);
    }
    else
    {
        glUniform1i(keypressUniform, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Unuse The Shader Program Object
    glUseProgram(0);
}

- (void)myUpdate
{
    // Code
}

- (void)uninitialize
{
    // Code
    if (texture_smiley)
    {
        glDeleteTextures(1, &texture_smiley);
        texture_smiley = 0;
    }

    //  Deletion And Uninitialization of vbo

    if (vbo_texcoord)
    {
        glDeleteBuffers(1, &vbo_texcoord);
        vbo_texcoord = 0;
    }

    if (vbo_position)
    {
        glDeleteBuffers(1, &vbo_position);
        vbo_position = 0;
    }

    // Deletion And Uninitialization of vao

    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
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
    if (keyPressed <=4)
    {
        keyPressed += 1;
    }
    else
        keyPressed = 0;
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
