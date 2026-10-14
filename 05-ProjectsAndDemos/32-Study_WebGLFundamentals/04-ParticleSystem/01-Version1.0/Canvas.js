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
    SBJ_ATTRIBUTE_VELOCITY: 2,
    SBJ_ATTRIBUTE_STARTTIME: 3,
    SBJ_ATTRIBUTE_NORMAL: 4,
    SBJ_ATTRIBUTE_TEXTURE: 5
};

var vao;
var vbo_position;
var vbo_color;
var vbo_velocities;
var vbo_startimes;

// Particle System Related Variables
var arrayWidth, arrayHeight;
var verts = new Array();
var colors = new Array();
var velocities = new Array();
var startTimes = new Array();

var w = 500;
var h = 500;
var particleTime = 0.0;
var particleTimeUniform;
var angleRotate = 0.0;

var mvpUniformMatrix;
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
        "in vec4 a_color;" +
        "in vec3 a_velocity;" +
        "in float a_startTime;" +
        "uniform mediump float Time;" +
        "uniform vec4 Background;" +
        "uniform mat4 u_mvpMatrix;" +
        "out vec4 Color;" +
        "void main(void)" +
        "{" +
        "vec4 vert;" +
        "float t = Time - a_startTime;" +
        "if (t >= 0.0)" +
        "{" +
        "vert = a_position + vec4(a_velocity * t, 0.0);" +
        "vert.y -= 4.9 * t * t;" +
        "Color = a_color;" +
        "}" +
        "else" +
        "{" +
        "vert = a_position;" +
        "Color = vec4(0.0, 0.0, 0.0, 1.0);" +
        "}" +
        "gl_PointSize = 2.0;" +
        "gl_Position = u_mvpMatrix * vert;" +
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
        "in vec4 Color;" +
        "out vec4 FragColor;" +
        "void main(void)" +
        "{" +
        "FragColor = Color;" +
        /*"FragColor = vec4(1.0, 1.0, 1.0, 1.0);"+*/
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
    gl.bindAttribLocation(shaderProgramObject, webglMacros.SBJ_ATTRIBUTE_COLOR, "a_color");
    gl.bindAttribLocation(shaderProgramObject, webglMacros.SBJ_ATTRIBUTE_VELOCITY, "a_velocity");
    gl.bindAttribLocation(shaderProgramObject, webglMacros.SBJ_ATTRIBUTE_STARTTIME, "a_startTime");
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

    mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_mvpMatrix");
    particleTimeUniform = gl.getUniformLocation(shaderProgramObject, "Time");

    // Declaration Of Vertex Data Arrays
    verts = new Float32Array(w * h * 3);
    colors = new Float32Array(w * h * 3);
    velocities = new Float32Array(w * h * 3);
    startTimes = new Float32Array(w * h);

    //var vptr, cptr, velptr, stptr;
    //vptr = verts;
    //cptr = colors;
    //velptr = velocities;
    //stptr = startTimes;

    let i, j;
    let index = 0;
    let step = 0;
    //const rand_max = 0x7fff;
    const rand_max = 2;

    for (i = 0.5 / w - 0.5; i < 0.5; i = i + 1.0 / w) {
        for (j = 0.5 / h - 0.5; j < 0.5; j += 1.0 / h) {
            verts[3 * index + 0] = i;
            verts[3 * index + 1] = 0.0;
            verts[3 * index + 2] = j;
            //index += 3;

            colors[3 * index + 0] = (Math.random()) * 0.5 + 0.5;
            colors[3 * index + 1] = (Math.random()) * 0.5 + 0.5;
            colors[3 * index + 2] = (Math.random()) * 0.5 + 0.5;
            //cptr += 3;

            velocities[3 * index + 0] = (Math.random() / rand_max) + 3.0;
            velocities[3 * index + 1] = (Math.random() / rand_max) * 10.0;
            velocities[3 * index + 2] = (Math.random() / rand_max) + 3.0;
            //velptr += 3;
            index += 3;

            startTimes[step] = (Math.random() / 2) * 10.0;
            step++;
        }
    }
    console.log("data");
    console.log(verts);
    console.log(colors);
    console.log(velocities);
    console.log(startTimes);

    arrayWidth = w;
    arrayHeight = h;

    console.log(arrayWidth);
    console.log(arrayHeight);

    vao = gl.createVertexArray();
    gl.bindVertexArray(vao);

    console.log(verts.length);
    console.log(colors.length);

    // vbo position
    vbo_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
    gl.bufferData(gl.ARRAY_BUFFER, verts, gl.STATIC_DRAW);
    gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    
    vbo_color = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color);
    gl.bufferData(gl.ARRAY_BUFFER, colors, gl.STATIC_DRAW);
    gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vbo_velocities = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_velocities);
    gl.bufferData(gl.ARRAY_BUFFER, velocities, gl.STATIC_DRAW);
    gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_VELOCITY, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_VELOCITY);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vbo_startimes = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_startimes);
    gl.bufferData(gl.ARRAY_BUFFER, startTimes, gl.STATIC_DRAW);
    gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_STARTTIME, 1, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_STARTTIME);
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

    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 1000.0);
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
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -10.0]);
    mat4.rotateY(rotationMatrix, rotationMatrix, angleRotate);
    mat4.multiply(modelViewMatrix, modelViewMatrix, translationMatrix);
    mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);
    gl.uniform1f(particleTimeUniform, particleTime);
    //gl.pointSize(2.0);



   // gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 3, gl.FLOAT, gl.FALSE, 0, verts);
    //gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_COLOR, 3, gl.FLOAT, gl.FALSE, 0, colors);
    //gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_VELOCITY, 3, gl.FLOAT, gl.FALSE, 0, velocities);
    //gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_STARTTIME, 1, gl.FLOAT, gl.FALSE, 0, startTimes);

    //gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
    //gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_COLOR);
    //gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_VELOCITY);
    //gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_STARTTIME);
    gl.bindVertexArray(vao);
    gl.drawArrays(gl.POINTS, 0, arrayWidth * arrayHeight);
    gl.bindVertexArray(null);

    //gl.disableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
    //gl.disableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_COLOR);
    //gl.disableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_VELOCITY);
    //gl.disableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_STARTTIME);


    // Unuse The Shader Program Object
    gl.useProgram(null);

    // Update The Scene
    update();

    // Double Buffering Imulation
    requestAnimationFrame(display, canvas);
}

function update() {

    // Code
    particleTime += 0.005;
    angleRotate += 0.05;
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

        case 65:
        case 97:
            //alert("a is pressed");
            z += 1.0;
            break;

        case 66:
        case 98:
            //alert("b is pressed");
            z -= 1.0;
            break;
    }
}

// Mouse Event Listener
function mouseDown() {

    // Code
}

function uninitialize() {

    // Code
   

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
