// Global Variable Declarations
var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

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
var vbo_cube_normal;

var angleCube = 0.0;

var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;
var ldUniform;
var kdUniform;
var lightPositionUniform;

var lightingEnabledUniform;

var bLight = false;

var lightDiffuse = [1.0, 1.0, 1.0];             // Specify Array Of Only 3 Elements
var materialDiffuse = [0.5, 0.5, 0.5];
var lightPosition = [0.0, 0.0, 2.0];

var perspectiveProjectionMatrix;

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
        "in vec3 a_normal;" +
        "uniform mat4 u_modelMatrix;" +
        "uniform mat4 u_viewMatrix;" +
        "uniform mat4 u_projectionMatrix;" +
        "uniform vec3 u_ld;" +
        "uniform vec3 u_kd;" +
        "uniform vec4 u_lightPosition;" +
        "uniform int u_lightingEnabled;" +
        "out vec3 diffuse_light_color;" +
        "void main(void)" +
        "{" +
        "if (u_lightingEnabled == 1)" +
        "{" +
        "vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" +
        "mat3 normalMatrix = mat3(transpose(inverse(u_viewMatrix * u_modelMatrix)));" +
        "vec3 transformedNormals = normalize(normalMatrix * a_normal);" +
        "vec3 lightDirection = normalize(vec3(u_lightPosition - eyeCoordinates));" +
        "diffuse_light_color = u_ld * u_kd * max(dot(lightDirection, transformedNormals), 0.0);" +
        "}" +
        "gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;" +
        "}";

    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);

    if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(vertexShaderObject);
        if (error.length > 0)
        {
            alert("Vertex Shader Compilation Log: " + error);
            uninitialize();
        }
    }

    // Fragment Shader
    var fragmentShaderSourceCode =
        "#version 300 es" +
        "\n" +
        "precision highp float;" +
        "in vec3 diffuse_light_color;" +
        "out vec4 FragColor;" +
        "precision highp int;" +
        "uniform int u_lightingEnabled;" +
        "void main(void)" +
        "{" +
        "if (u_lightingEnabled == 1)" +
        "{" +
        "FragColor = vec4(diffuse_light_color, 1.0);" +
        "}" +
        "else" +
        "{" +
        "FragColor = vec4(1.0, 1.0, 1.0, 1.0);" +
        "}" +
        "}";

    var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);

    if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if (error.length > 0)
        {
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
    gl.bindAttribLocation(shaderProgramObject, webglMacros.SBJ_ATTRIBUTE_NORMAL, "a_normal");
    // Shader Program Linking
    gl.linkProgram(shaderProgramObject);
    // Link Error Checking
    if (gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false)
    {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if (error.length > 0)
        {
            alert("Fragment Shader Compilation Log: " + error);
            uninitialize();
        }
    }

    modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_modelMatrix");
    viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_viewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_projectionMatrix");
    ldUniform = gl.getUniformLocation(shaderProgramObject, "u_ld");
    kdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
    lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_lightPosition");
    lightingEnabledUniform = gl.getUniformLocation(shaderProgramObject, "u_lightingEnabled");

    // Declaration Of Vertex Data Arrays
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

    var cubeNormals = new Float32Array([
                                            // top surface
                                            0.0, 1.0, 0.0,  // top-right of top
                                            0.0, 1.0, 0.0, // top-left of top
                                            0.0, 1.0, 0.0, // bottom-left of top
                                            0.0, 1.0, 0.0,  // bottom-right of top

                                            // bottom surface
                                            0.0, -1.0, 0.0,  // top-right of bottom
                                            0.0, -1.0, 0.0,  // top-left of bottom
                                            0.0, -1.0, 0.0,  // bottom-left of bottom
                                            0.0, -1.0, 0.0,   // bottom-right of bottom

                                            // front surface
                                            0.0, 0.0, 1.0,  // top-right of front
                                            0.0, 0.0, 1.0, // top-left of front
                                            0.0, 0.0, 1.0, // bottom-left of front
                                            0.0, 0.0, 1.0,  // bottom-right of front

                                            // back surface
                                            0.0, 0.0, -1.0,  // top-right of back
                                            0.0, 0.0, -1.0, // top-left of back
                                            0.0, 0.0, -1.0, // bottom-left of back
                                            0.0, 0.0, -1.0,  // bottom-right of back

                                            // right surface
                                            1.0, 0.0, 0.0,  // top-right of right
                                            1.0, 0.0, 0.0,  // top-left of right
                                            1.0, 0.0, 0.0,  // bottom-left of right
                                            1.0, 0.0, 0.0,  // bottom-right of right

                                            // left surface
                                            - 1.0, 0.0, 0.0, // top-right of left
                                            -1.0, 0.0, 0.0, // top-left of left
                                            -1.0, 0.0, 0.0, // bottom-left of left
                                            -1.0, 0.0, 0.0, // bottom-right of left
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

        // vbo normal
        vbo_cube_normal = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_normal);
        gl.bufferData(gl.ARRAY_BUFFER, cubeNormals, gl.STATIC_DRAW);
        gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_NORMAL);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    gl.bindVertexArray(null);

    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // Clear The Screen By Black Color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    perspectiveProjectionMatrix =  mat4.create();
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

    if (canvas.height == 0)
        canvas.heigth = 1;

    gl.viewport(0, 0, canvas.width, canvas.height);

    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);
}

function display() {

    // Code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    // Use The Shader Program Object
    gl.useProgram(shaderProgramObject);

    // Cube
    // Transformations
    var translationMatrix = mat4.create();
    var rotationMatrix = mat4.create();
    var rotationMatrix_X = mat4.create();
    var rotationMatrix_Y = mat4.create();
    var rotationMatrix_Z = mat4.create();
    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();

    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -6.0]);
    mat4.rotateX(rotationMatrix_X, rotationMatrix_X, angleCube);
    mat4.rotateY(rotationMatrix_Y, rotationMatrix_Y, angleCube);
    mat4.rotateZ(rotationMatrix_Z, rotationMatrix_Z, angleCube);
    mat4.multiply(rotationMatrix, rotationMatrix_X, rotationMatrix_Y);
    mat4.multiply(rotationMatrix, rotationMatrix, rotationMatrix_Z);
    mat4.multiply(modelMatrix, translationMatrix, rotationMatrix);
   
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);


    // Sending Light Related Uniforms
    if (bLight == true) {
        gl.uniform1i(lightingEnabledUniform, 1);

        gl.uniform3fv(ldUniform,lightDiffuse);                  // Specify Only 2 Parameters For uniform*()
        gl.uniform3fv(kdUniform, materialDiffuse);
        gl.uniform4fv(lightPositionUniform, lightPosition);
    }
    else {
        gl.uniform1i(lightingEnabledUniform, 0);
    }

    gl.bindVertexArray(vao_cube);

        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
        gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
        gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
        gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
        gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
        gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

    gl.bindVertexArray(null);


    // Unuse The Shader Program Object
    gl.useProgram(null);

    // Update The Scene
    update();

    // Double Buffering Imulation
    requestAnimationFrame(display, canvas);
}

function update() {

    // Code
    angleCube = angleCube + 0.03;
    if (angleCube >= (2 * Math.PI))
        angleCube = angleCube - (2 * Math.PI);
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
            window.close();         // Not All Browsers Will Follow This
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
    if (vbo_cube_position) {
        gl.deleteBuffer(vbo_cube_position);
        vbo_cube_position = null;
    }

    if (vbo_cube_normal) {
        gl.deleteBuffer(vbo_cube_normal);
        vbo_cube_normal = null;
    }

    if (vao_cube) {
        gl.deleteVertexArray(vao_cube);
        vao_cube = null;
    }
   

    if (shaderProgramObject)
    {
        gl.useProgram(shaderProgramObject);
        var shaderObjects = gl.getAttachedShaders(shaderProgramObject);

        for (let i = 0; i < shaderObjects.length; i++)
        {
            gl.detachShader(shaderProgramObject, shaderObjects[i]);
            gl.deleteShader(shaderObjects[i]);
            shaderObjects[i] = 0;
        }

        gl.useProgram(null);

        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }
}
