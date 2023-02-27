// Global Variable Declarations
var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

var sphere = null;
var bLight = false;
var lightAngleZero = 0.0;
var lightAngleOne = 0.0;
var lightAngleTwo = 0.0;
var winWidth;
var winHeight;

var shaderProgramObject;

const webglMacros =
{
    SBJ_ATTRIBUTE_POSITION: 0,
    SBJ_ATTRIBUTE_COLOR: 1,
    SBJ_ATTRIBUTE_NORMAL: 2,
    SBJ_ATTRIBUTE_TEXTURE: 3
};

var vao_cube;
var vbo_cube_position;
var vbo_cube_texcoord;
var angleCube = 0.0;
var mvpUniformMatrix;
var textureSamplerUniform;
var perspectiveProjectionMatrix;

// FBO Related Global Variables
var fbo;
var rbo;
var fbo_texture;
var bFBOResult = false;
var FBO_WIDTH = 512;
var FBO_HEIGHT = 512;

// Texture Scene Global Vaiables
var shaderProgramObject_sphere;

var modelMatrixUniform_sphere;
var viewMatrixUniform_sphere ;
var projectionMatrixUniform_sphere;

var laUniform_sphere = new Array([3]);				// Ambient
var ldUniform_sphere = new Array([3]);				// Diffuse
var lsUniform_sphere = new Array([3]);				// Specular
var lightPositionUniform_sphere = new Array([3]);

var kaUniform_sphere;
var kdUniform_sphere;
var ksUniform_sphere;
var materialShininessUniform_sphere;

var lightingEnabledUniform_sphere;

var lightAmbient_sphere = new Array([3][3]);
var lightDiffuse_sphere = new Array([3][3]);
var lightSpecular_sphere = new Array([3][3]);
var lightPosition_sphere = new Array([3][4]);

var materialAmbient_sphere = [ 0.0, 0.0, 0.0];
var materialDiffuse_sphere = [ 1.0, 1.0, 1.0];
var materialSpecular_sphere = [ 1.0, 1.0, 1.0];
var materialShininess_sphere = 50.0;

var perspectiveProjectionMatrix_sphere;

var requestAnimationFrame = window.requestAnimationFrame ||
                            window.mozRequestAnimationFrame ||
                            window.webkitRequestAnimationFrame ||
                            window.oRequestAnimationFrame ||
                            window.msRequestAnimationFrame;

function main() {

    // Code
    // Get Canvas
    canvas = document.getElementById("SBJ");
    if (!canvas)
        console.log("Obtaining Canvas Failed !\n");
    else
        console.log("Obtaining Canvas Succeeded.\n");

    // Backup Canvas Dimentions
    canvas_original_width = canvas.width;
    canvas_original_height = canvas.height;

    // Initialize
    initialize();

    // Warmup Resize
    resize();

    // Display
    display();

    // Adding Keyboard And Mouse Event Listener
    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
    window.addEventListener("resize", resize, false);
    window.addEventListener("resize", resize_sphere, false);
}

function toggleFullscreen() {

    // Code
    var fullscreen_element = document.fullscreenElement ||
                             document.mozFullScreenElement ||
                             document.webkitFullscreenElement ||
                             document.msFullscreenElement ||
                             null;

    if (fullscreen_element == null) {

        if (canvas.requestFullscreen)
            canvas.requestFullscreen();
        else if (canvas.mozRequestFullScreen)
            canvas.mozRequestFullScreen();
        else if (canvas.webkitRequestFullscreen)
            canvas.webkitRequestFullscreen();
        else if (canvas.msRequestFullscreen)
            canvas.msRequestFullscreen();

        bFullscreen = true;
    }
    else {

        if (document.exitFullscreen)
            document.exitFullscreen();
        else if (document.mozExitFullScreen)
            document.mozExitFullScreen();
        else if (document.webkitExitFullScreen)
            document.webkitExitFullScreen();
        else if (document.msExitFullScreen)
            document.msExitFullScreen();

        bFullscreen = false;
    }
}

function initialize(){

    // Code
    // Get WbGL 2 Context From Canvas
    var context = canvas.getContext("webgl2");
    if (!context)
        console.log("Obtaining WebGL 2.0 Context Failed !\n");
    else
        console.log("Obtaining WebGL 2.0 context Succeeded.\n");

    // Getting Context Into Variable 'gl'
    gl = context;

    // Set Viewport Width And Height Of Context
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

    // Vertex Shader
    var vertexShaderSourceCode =
        "#version 300 es" +
        "\n" +
        "in vec4 a_position;" +
        "in vec2 a_texcoord;" +
        "uniform mat4 u_mvpMatrix;" +
        "out vec2 a_texcoord_out;" +
        "void main(void)" +
        "{" +
        "gl_Position = u_mvpMatrix * a_position;" +
        "a_texcoord_out = a_texcoord;" +
        "}";

    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);

    if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(vertexShaderObject);
        if (error.length > 0) {
            alert("Vertex Shader Compilation Log: " + error);
            uninitialize();
        }
    }

    // Fragment Shader
    const fragmentShaderSourceCode =
        "#version 300 es" +
        "\n" +
        "precision highp float;" +
        "in vec2 a_texcoord_out;" +
        "uniform sampler2D u_textureSampler;" +
        "out vec4 FragColor;" +
        "void main(void)" +
        "{" +
        "FragColor = texture(u_textureSampler, a_texcoord_out);" +
        "}";

    var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);

    if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if (error.length > 0) {
            alert("Fragment Shader Compilation Log: " + error);
            uninitialize();
        }
    }

    // Shader Program
    shaderProgramObject = gl.createProgram();
    gl.attachShader(shaderProgramObject, vertexShaderObject);
    gl.attachShader(shaderProgramObject, fragmentShaderObject);
    // Prelinking Shader Attribute Binding
    gl.bindAttribLocation(shaderProgramObject, webglMacros.SBJ_ATTRIBUTE_POSITION, "a_position");
    gl.bindAttribLocation(shaderProgramObject, webglMacros.SBJ_ATTRIBUTE_TEXTURE, "a_texcoord");
    // Shader Program Linking
    gl.linkProgram(shaderProgramObject);
    // Link Error Checking
    if (gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false) {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if (error.length > 0) {
            alert("Fragment Shader Compilation Log: " + error);
            uninitialize();
        }
    }

    mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_mvpMatrix");
    textureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "u_textureSampler");

    var cubePosition = new Float32Array([
        // top
        1.0, 1.0, -1.0,
        -1.0, 1.0, -1.0,
        -1.0, 1.0, 1.0,
        1.0, 1.0, 1.0,

        // bottom
        1.0, -1.0, -1.0,
        -1.0, -1.0, -1.0,
        -1.0, -1.0, 1.0,
        1.0, -1.0, 1.0,

        // front
        1.0, 1.0, 1.0,
        -1.0, 1.0, 1.0,
        -1.0, -1.0, 1.0,
        1.0, -1.0, 1.0,

        // back
        1.0, 1.0, -1.0,
        -1.0, 1.0, -1.0,
        -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,

        // right
        1.0, 1.0, -1.0,
        1.0, 1.0, 1.0,
        1.0, -1.0, 1.0,
        1.0, -1.0, -1.0,

        // left
        -1.0, 1.0, 1.0,
        -1.0, 1.0, -1.0,
        -1.0, -1.0, -1.0,
        -1.0, -1.0, 1.0,
    ]);

    var cubeTexcoord = new Float32Array([
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,

        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0

    ]);

    // vao cube
    vao_cube = gl.createVertexArray();
    gl.bindVertexArray(vao_cube);

        // vbo position
        vbo_cube_position = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_position);
        gl.bufferData(gl.ARRAY_BUFFER, cubePosition, gl.STATIC_DRAW);
        gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

        // vbo texcoord
        vbo_cube_texcoord = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_texcoord);
        gl.bufferData(gl.ARRAY_BUFFER, cubeTexcoord, gl.STATIC_DRAW);
        gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_TEXTURE, 2, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_TEXTURE);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);

    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // Clear The Screen By Gray Color
    gl.clearColor(0.3, 0.3, 0.3, 1.0);

    perspectiveProjectionMatrix = mat4.create();

    // FBO Code
    bFBOResult = createFBO(FBO_WIDTH, FBO_HEIGHT);
    var iRetVal;
    if (bFBOResult == true) {
        iRetVal = initialize_sphere(FBO_WIDTH, FBO_HEIGHT);

        // Here There Should Be Error Checking Of FBO
    }
    else {
        alert("Create FBO Failed...: " + error);
        return -6;
    } 
}


function createFBO(textureWidth, textureHeight)
{
    // Local Variable Declarations
    var maxRenderbufferSize;

    // Code
    // Check Available Renderbuffer Size
    //gl.getIntegerv(gl.MAX_RENDERBUFFER_SIZE, maxRenderbufferSize);
    //if (maxRenderbufferSize < textureWidth || maxRenderbufferSize < textureHeight) {
        //console.log("Insufficient Render Buffer Size \n");
        //return false;
    //}

    // Create Framebuffer Object
    fbo = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);

    // Create Renderbuffer Object
    rbo = gl.createRenderbuffer();
    gl.bindRenderbuffer(gl.RENDERBUFFER, rbo);

    // Storage And Format Of The Renderbuffer
    gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, textureWidth, textureHeight);

    // Create Empty Texture For Upcoming Target Scene
    fbo_texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, fbo_texture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);

    
    gl.texImage2D(gl.TEXTURE_2D,
        0,
        gl.RGBA,
        textureWidth,
        textureHeight,
        0,
        gl.RGBA,
        gl.UNSIGNED_BYTE,
        fbo_texture.image);
   
    //gl.generateMipmap(gl.TEXTURE_2D);

    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, fbo_texture, 0);

    // Give Renderbuffer To FBO
    gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER, rbo);

    // Check Whether The Framebuffeer Created Successful or Not
    var result = gl.checkFramebufferStatus(gl.FRAMEBUFFER);
    if (result != gl.FRAMEBUFFER_COMPLETE) {
        console.log("Frame Buffer Is Not Complete \n");
        return false;
    }

    gl.bindFramebuffer(gl.FRAMEBUFFER, null);

    return true;

}



function initialize_sphere(width, height)
{
    // Code
    // Vertex Shader PerFragment
    var vertexShaderSourceCode =
        "#version 300 es" +
        "\n" +
        "in vec4 a_position;" +
        "in vec3 a_normal;" +
        "uniform mat4 u_modelMatrix;" +
        "uniform mat4 u_viewMatrix;" +
        "uniform mat4 u_projectionMatrix;" +
        "uniform vec4 u_lightPosition[3];" +
        "uniform int u_lightingEnabled;" +
        "out vec3 transformedNormals;" +
        "out vec3 lightDirection[3];" +
        "out vec3 viewerVector;" +
        "void main(void)" +
        "{" +
        "if (u_lightingEnabled == 1)" +
        "{" +
        "vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" +
        "mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" +
        "transformedNormals = normalMatrix * a_normal;" +
        "viewerVector = -eyeCoordinates.xyz;" +
        "for (int i = 0; i < 3; i++)" +
        "{" +
        "lightDirection[i] = vec3(u_lightPosition[i]) - eyeCoordinates.xyz;" +
        "}" +
        "}" +
        "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;" +
        "}";

    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);

    if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(vertexShaderObject);
        if (error.length > 0) {
            alert("Vertex Shader Compilation Log: " + error);
            uninitialize();
        }
    }

    // Fragment Shader PerFragment
    var fragmentShaderSourceCode =
        "#version 300 es" +
        "\n" +
        "precision highp float;" +
        "in vec3 transformedNormals;" +
        "in vec3 lightDirection[3];" +
        "in vec3 viewerVector;" +
        "uniform vec3 u_la[3];" +
        "uniform vec3 u_ld[3];" +
        "uniform vec3 u_ls[3];" +
        "uniform vec3 u_ka;" +
        "uniform vec3 u_kd;" +
        "uniform vec3 u_ks;" +
        "uniform float u_materialShininess;" +
        "out vec4 FragColor;" +
        "precision highp int;" +
        "uniform int u_lightingEnabled;" +
        "void main(void)" +
        "{" +
        "vec3 phong_ads_light;" +
        "if (u_lightingEnabled == 1)" +
        "{" +
        "vec3 normalized_tranformedNormals = normalize(transformedNormals);" +
        "vec3 normalized_viewerVector = normalize(viewerVector);" +
        "vec3 ambient[3];" +
        "vec3 normalized_lightDirection[3];" +
        "vec3 diffuse[3];" +
        "vec3 reflectionVector[3];" +
        "vec3 specular[3];" +
        "for (int i = 0; i < 3; i++)" +
        "{" +
        "ambient[i] = u_la[i] * u_ka;" +
        "normalized_lightDirection[i] = normalize(lightDirection[i]);" +
        "diffuse[i] = u_ld[i] * u_kd * max(dot(normalized_lightDirection[i], normalized_tranformedNormals), 0.0);" +
        "reflectionVector[i] = reflect(-normalized_lightDirection[i], normalized_tranformedNormals);" +
        "specular[i] = u_ls[i] * u_ks * pow(max(dot(reflectionVector[i], normalized_viewerVector), 0.0), u_materialShininess);" +
        "phong_ads_light += ambient[i] + diffuse[i] + specular[i];" +
        "}" +
        "}" +
        "else" +
        "{" +
        "phong_ads_light = vec3(1.0, 1.0, 1.0);" +
        "}" +
        "FragColor = vec4(phong_ads_light, 1.0);" +
        "}";

    var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);

    if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if (error.length > 0) {
            alert("Fragment Shader Compilation Log: " + error);
            uninitialize();
        }
    }


    // Shader Program PerFragment
    shaderProgramObject_sphere = gl.createProgram();
    gl.attachShader(shaderProgramObject_sphere, vertexShaderObject);
    gl.attachShader(shaderProgramObject_sphere, fragmentShaderObject);
    // Prelinking Shader Attribute Binding
    gl.bindAttribLocation(shaderProgramObject_sphere, webglMacros.SBJ_ATTRIBUTE_POSITION, "a_position");
    gl.bindAttribLocation(shaderProgramObject_sphere, webglMacros.SBJ_ATTRIBUTE_NORMAL, "a_normal");
    // Shader Program Linking
    gl.linkProgram(shaderProgramObject_sphere);
    // Link Error Checking
    if (gl.getProgramParameter(shaderProgramObject_sphere, gl.LINK_STATUS) == false) {
        var error = gl.getProgramInfoLog(shaderProgramObject_sphere);
        if (error.length > 0) {
            alert("Fragment Shader Compilation Log: " + error);
            uninitialize();
        }
    }

    modelMatrixUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "u_modelMatrix");
    viewMatrixUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "u_viewMatrix");
    projectionMatrixUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "u_projectionMatrix");
    laUniform_sphere[0] = gl.getUniformLocation(shaderProgramObject_sphere, "u_la[0]");
    laUniform_sphere[1] = gl.getUniformLocation(shaderProgramObject_sphere, "u_la[1]");
    laUniform_sphere[2] = gl.getUniformLocation(shaderProgramObject_sphere, "u_la[2]");
    ldUniform_sphere[0] = gl.getUniformLocation(shaderProgramObject_sphere, "u_ld[0]");
    ldUniform_sphere[1] = gl.getUniformLocation(shaderProgramObject_sphere, "u_ld[1]");
    ldUniform_sphere[2] = gl.getUniformLocation(shaderProgramObject_sphere, "u_ld[2]");
    lsUniform_sphere[0] = gl.getUniformLocation(shaderProgramObject_sphere, "u_ls[0]");
    lsUniform_sphere[1] = gl.getUniformLocation(shaderProgramObject_sphere, "u_ls[1]");
    lsUniform_sphere[2] = gl.getUniformLocation(shaderProgramObject_sphere, "u_ls[2]");
    lightPositionUniform_sphere[0] = gl.getUniformLocation(shaderProgramObject_sphere, "u_lightPosition[0]");
    lightPositionUniform_sphere[1] = gl.getUniformLocation(shaderProgramObject_sphere, "u_lightPosition[1]");
    lightPositionUniform_sphere[2] = gl.getUniformLocation(shaderProgramObject_sphere, "u_lightPosition[2]");
    kaUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "u_ka");
    kdUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "u_kd");
    ksUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "u_ks");
    materialShininessUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "u_materialShininess");
    lightingEnabledUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "u_lightingEnabled");


    sphere = new Mesh();
    makeSphere(sphere, 1.0, 30, 30);

    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // Clear The Screen By Gray Color
    gl.clearColor(0.3, 0.3, 0.3, 1.0);

    lightAmbient_sphere[0] = [0.0, 0.0, 0.0];
    lightDiffuse_sphere[0] = [1.0, 0.0, 0.0];
    lightSpecular_sphere[0] = [1.0, 0.0, 0.0];
    lightPosition_sphere[0] = [0.0, 0.0, 0.0, 1.0];

    lightAmbient_sphere[1] = [0.0, 0.0, 0.0];
    lightDiffuse_sphere[1] = [0.0, 1.0, 0.0];
    lightSpecular_sphere[1] = [0.0, 1.0, 0.0];
    lightPosition_sphere[1] = [0.0, 0.0, 0.0, 1.0];

    lightAmbient_sphere[2] = [0.0, 0.0, 0.0];
    lightDiffuse_sphere[2] = [0.0, 0.0, 1.0];
    lightSpecular_sphere[2] = [0.0, 0.0, 1.0];
    lightPosition_sphere[2] = [0.0, 0.0, 0.0, 1.0];

    perspectiveProjectionMatrix_sphere = mat4.create();

}

function resize() {

    // Code
    if (bFullscreen == true) {

        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else {

        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

    winWidth = canvas.width;
    winHeight = canvas.height;

    if (canvas.height == 0)
        canvas.heigth = 1;

    gl.viewport(0, 0, canvas.width, canvas.height);

    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);
}


function resize_sphere(width, height) {

    // Code
    /*
    if (bFullscreen == true) {

        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else {

        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }
    */
    if (height == 0)
        heigth = 1;

    gl.viewport(0, 0, width, height);

    mat4.perspective(perspectiveProjectionMatrix_sphere, 45.0, parseFloat(width) / parseFloat(height), 0.1, 100.0);
}

function display()
{
    // Code
    if (bFBOResult == true) {
       display_sphere(FBO_WIDTH, FBO_HEIGHT);
       update_sphere();
    }

    gl.clearColor(0.3, 0.3, 0.3, 1.0);
    resize(winWidth, winHeight);

    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    // Transformations
    var translationMatrix = mat4.create();
    var rotationMatrix = mat4.create();
    var rotationMatrix_X = mat4.create();
    var rotationMatrix_Y = mat4.create();
    var rotationMatrix_Z = mat4.create();
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -5.0]);
    mat4.rotateX(rotationMatrix_X, rotationMatrix_X, angleCube);
    mat4.rotateY(rotationMatrix_Y, rotationMatrix_Y, angleCube);
    mat4.rotateZ(rotationMatrix_Z, rotationMatrix_Z, angleCube);
    mat4.multiply(rotationMatrix, rotationMatrix_X, rotationMatrix_Y);
    mat4.multiply(rotationMatrix, rotationMatrix, rotationMatrix_Z);
    mat4.multiply(modelViewMatrix, translationMatrix, rotationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, fbo_texture);
    gl.uniform1i(textureSamplerUniform, 0);

    gl.bindVertexArray(vao_cube);

    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

    gl.bindVertexArray(null);

    gl.bindTexture(gl.TEXTURE_2D, null);

    // Unuse The Shader Program Object
    gl.useProgram(null);

    // Update The Scene
    update();

    // Double Buffering Imulation
    requestAnimationFrame(display, canvas);
}

function update() {

    // Code
    angleCube = angleCube + 0.01;
    if (angleCube >= (2 * Math.PI))
        angleCube = angleCube - (2 * Math.PI);
}


function display_sphere(textureWidth, textureHeight)
{
    // Code
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);

    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    resize_sphere(textureWidth, textureHeight);

    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject_sphere);

    // Sphere
    // Transformations
    var translationMatrix = mat4.create();
    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();

    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -3.0]);
    mat4.multiply(modelMatrix, modelMatrix, translationMatrix);

    gl.uniformMatrix4fv(modelMatrixUniform_sphere, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform_sphere, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform_sphere, false, perspectiveProjectionMatrix_sphere);

    // Sending Light Related Uniforms
    if (bLight == true) {
        gl.uniform1i(lightingEnabledUniform_sphere, 1);

        for (var i = 0; i < 3; i++) {
            gl.uniform3fv(laUniform_sphere[i], lightAmbient_sphere[i]);
            gl.uniform3fv(ldUniform_sphere[i], lightDiffuse_sphere[i]);
            gl.uniform3fv(lsUniform_sphere[i], lightSpecular_sphere[i]);
            gl.uniform4fv(lightPositionUniform_sphere[i], lightPosition_sphere[i]);
        }

        gl.uniform3fv(kaUniform_sphere, materialAmbient_sphere);
        gl.uniform3fv(kdUniform_sphere, materialDiffuse_sphere);
        gl.uniform3fv(ksUniform_sphere, materialSpecular_sphere);
        gl.uniform1f(materialShininessUniform_sphere, materialShininess_sphere);
    }
    else {
        gl.uniform1i(lightingEnabledUniform_sphere, 0);
    }

    sphere.draw();

    // Update The Scene
    update_sphere();

    // Unuse The Shader Program Object
    gl.useProgram(null);

    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
}

function update_sphere() {

    // Code
    lightAngleZero += 2.0;
    lightAngleOne += 2.0;
    lightAngleTwo += 2.0;

    lightPosition_sphere[0][1] = 10 * Math.sin(lightAngleZero / 180.0 * 3.14);
    lightPosition_sphere[0][2] = 10 * Math.cos(lightAngleZero / 180.0 * 3.14);

    lightPosition_sphere[1][0] = 10 * Math.sin(lightAngleZero * 3.14 / 180.0);
    lightPosition_sphere[1][2] = 10 * Math.cos(lightAngleZero * 3.14 / 180.0);

    lightPosition_sphere[2][0] = 10 * -Math.sin(lightAngleZero * 3.14 / 180.0);
    lightPosition_sphere[2][1] = 10 * Math.cos(lightAngleZero * 3.14 / 180.0);
}

function degToRad(degrees)
{
    // Code
    return (degrees * Math.PI) / 180.0;
}

// Keyboard Event Listener
function keyDown(event) {

    // Code
    switch (event.keyCode) {

        case 27:
            uninitialize();
            window.close();
            break;

        case 81:                    // Press Q For Exit
            uninitialize();
            window.close();         // Not All Browsers Will Follow This
            break;

        case 70:
            toggleFullscreen();
            break;

        case 76:
            if (bLight == false)
                bLight = true;
            else
                bLight = false;
            break;

        default:
            break;
    }
}

// Mouse Event Listener
function mouseDown() {

    // Code
}

function uninitialize() {

    // Code
    // vbo

    uninitialize_sphere();

    if (vbo_cube_position) {
        gl.deleteBuffer(vbo_cube_position);
        vbo_cube_position = null;
    }

    if (vbo_cube_texcoord) {
        gl.deleteBuffer(vbo_cube_texcoord);
        vbo_cube_texcoord = null;
    }

    // vao
    if (vao_cube) {
        gl.deleteVertexArray(vao_cube);
        vao_cube = null;
    }

    if (shaderProgramObject) {
        gl.useProgram(shaderProgramObject);
        var shaderObjects = gl.getAttachedShaders(shaderProgramObject);

        for (let i = 0; i < shaderObjects.length; i++) {
            gl.detachShader(shaderProgramObject, shaderObjects[i]);
            gl.deleteShader(shaderObjects[i]);
            shaderObjects[i] = 0;
        }

        gl.useProgram(null);

        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }
}

function uninitialize_sphere() {

    // Code
    if (sphere) {
        sphere.deallocate();
        sphere = null;
    }

    if (shaderProgramObject_sphere) {
        gl.useProgram(shaderProgramObject_sphere);
        var shaderObjects = gl.getAttachedShaders(shaderProgramObject_sphere);

        for (let i = 0; i < shaderObjects.length; i++) {
            gl.detachShader(shaderProgramObject_sphere, shaderObjects[i]);
            gl.deleteShader(shaderObjects[i]);
            shaderObjects[i] = 0;
        }

        gl.useProgram(null);

        gl.deleteProgram(shaderProgramObject_sphere);
        shaderProgramObject_sphere = null;
    }


}

