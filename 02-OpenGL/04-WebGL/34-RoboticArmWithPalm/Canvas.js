// Global Variable Declarations
var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

var sphere = null;

var shaderProgramObject;

const webglMacros =
{
    SBJ_ATTRIBUTE_POSITION: 0,
    SBJ_ATTRIBUTE_COLOR: 1,
    SBJ_ATTRIBUTE_NORMAL: 2,
    SBJ_ATTRIBUTE_TEXTURE: 3
};


var shoulder = 0;
var elbow = 0;
var palm = 0;

var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

var modelMatrix = mat4.create();
var perspectiveProjectionMatrix;

var requestAnimationFrame = window.requestAnimationFrame ||
                            window.mozRequestAnimationFrame ||
                            window.webkitRequestAnimationFrame ||
                            window.oRequestAnimationFrame ||
                            window.msRequestAnimationFrame;

// Push-Pop Related Variables
var SIZE = 50;
var Stack = new Array([10][4][4]);
var top = -1;

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
        "uniform mat4 u_modelMatrix;" +
        "uniform mat4 u_viewMatrix;" +
        "uniform mat4 u_projectionMatrix;" +
        "void main(void)" +
        "{" +
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
    const fragmentShaderSourceCode =
        "#version 300 es" +
        "\n" +
        "precision highp float;" +
        "out vec4 FragColor;" +
        "void main(void)" +
        "{" +
        "FragColor = vec4(0.5, 0.35, 0.05, 1.0);" +
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
    // Shader Program Linking
    gl.linkProgram(shaderProgramObject);
    // Link Error Checking
    if (gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false)
    {
        var error = gl.getProgramIngoLog(shaderProgramObject);
        if (error.length > 0)
        {
            alert("Fragment Shader Compilation Log: " + error);
            uninitialize();
        }
    }

    modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_modelMatrix");
    viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_viewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_projectionMatrix");

    sphere = new Mesh();
    makeSphere(sphere, 2.0, 30, 30);

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

    // Transformations
    var translationMatrix = mat4.create();
    var scaleMatrix = mat4.create();
    var rotateMatrix = mat4.create();
    var viewMatrix = mat4.create();

    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -10.0]);
    mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
    
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    sglPushMatrix();
        // Arm
        mat4.rotateZ(rotateMatrix, rotateMatrix, shoulder);
        mat4.multiply(modelMatrix, modelMatrix, rotateMatrix);
        gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);

        mat4.translate(translationMatrix, translationMatrix, [1.0, 0.0, 0.0]);
        mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
        gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);

        mat4.scale(scaleMatrix, scaleMatrix, [2.0, 0.5, 1.0]);
        mat4.multiply(modelMatrix, modelMatrix, scaleMatrix);
        gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);

        sphere.draw();
    sglPopMatrix();
    /*
    // Forearm
    mat4.translate(translationMatrix, translationMatrix, [1.0, 0.0, 0.0]);
    mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);

    mat4.rotateZ(rotateMatrix, rotateMatrix, elbow);
    mat4.multiply(modelMatrix, modelMatrix, rotateMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);

    mat4.translate(translationMatrix, translationMatrix, [1.0, 0.0, 0.0]);
    mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);

    sglPushMatrix();

        mat4.scale(scaleMatrix, scaleMatrix, [2.0, 0.5, 1.0]);
        mat4.multiply(modelMatrix, modelMatrix, scaleMatrix);
        gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);

        sphere.draw();

    sglPopMatrix();
    */

    // Unuse The Shader Program Object
    gl.useProgram(null);

    // Double Buffering Imulation
    requestAnimationFrame(display, canvas);
}

function update() {

    // Code
}

function sglPushMatrix() {
    // Code
    if (top == 9) {
        console.log("Error: Stack Overflow !\n");
        uninitialize();
    }
    else {
        top += 1;
        var j;
        for (j = 0; j < 16; j += 1) {
            Stack[top][j] = modelMatrix[j];
        }
    }
}

function sglPopMatrix() {
    // Variable Declarations
    var tempVar = new Array(16);

    // Code
    if (top == -1) {
        console.log("Error: Stack Underflow !\n");
        uninitialize();
    }
    else {

        var j;
        for (var i = 0; i < 4; i += 1) {

            for (j = 0; j < 4; j += 1) {
                modelMatrix[i][j] = Stack[top][i][j];
            }
        }
        top -= 1;
        //return tempVar;
    }
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

        case 83:
            shoulder = (shoulder + 3) % 360;
            break;
        /*
        case 's':
            shoulder = (shoulder - 3) % 360;
            break;
            */
        case 69:
            elbow = (elbow + 3) % 360;
            break;

        /*
        case 'e':
            elbow = (elbow - 3) % 360;
            break;
            */
        case 80:
            palm = (palm + 2) % 360;
            break;
        /*
        case 'p':
            palm = (palm - 2) % 360;
            break;
            */
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
    if (sphere)
    {
        sphere.deallocate();
        sphere = null;
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
