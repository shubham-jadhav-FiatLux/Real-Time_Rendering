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

var vao_pyramid;
var vbo_pyramid_position;
var vbo_pyramid_color;

var vao_cube;
var vbo_cube_position;
var vbo_cube_color;

var anglePyramid = 0.0;
var angleCube = 0.0;

var texture_stone;
var texture_kundali;

var mvpUniformMatrix;
var textureSamplerUniform;

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
    gl.bindAttribLocation(shaderProgramObject, webglMacros.SBJ_ATTRIBUTE_TEXTURE, "a_texcoord");
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
    textureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "u_textureSampler");


    var pyramidPosition = new Float32Array([
                                                // front
                                                0., 1.0, 0.0,
                                                -1.0, -1.0, 1.0,
                                                1.0, -1.0, 1.0,

                                                // right
                                                0.0, 1.0, 0.0,
                                                1.0, -1.0, 1.0,
                                                1.0, -1.0, -1.0,

                                                // back
                                                0.0, 1.0, 0.0,
                                                1.0, -1.0, -1.0,
                                                -1.0, -1.0, -1.0,

                                                // left
                                                0.0, 1.0, 0.0,
                                                -1.0, -1.0, -1.0,
                                                -1.0, -1.0, 1.0
                                            ]);

    var pyramidTexcoord = new Float32Array([
                                                0.5, 1.0, // front-top
                                                0.0, 0.0, // front-left
                                                1.0, 0.0, // front-right

                                                0.5, 1.0, // right-top
                                                1.0, 0.0, // right-left
                                                0.0, 0.0, // right-right

                                                0.5, 1.0, // back-top
                                                1.0, 0.0, // back-left
                                                0.0, 0.0, // back-right

                                                0.5, 1.0, // left-top
                                                0.0, 0.0, // left-left
                                                1.0, 0.0, // left-right
                                            ]);

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

    // vao triangle
    vao_pyramid = gl.createVertexArray();
    gl.bindVertexArray(vao_pyramid);

    // vbo position
    vbo_pyramid_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_position);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidPosition, gl.STATIC_DRAW);
    gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    // vbo color
    vbo_pyramid_texcoord = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_texcoord);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidTexcoord, gl.STATIC_DRAW);
    gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_TEXTURE, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_TEXTURE);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);

    // vao square
    vao_cube = gl.createVertexArray();
    gl.bindVertexArray(vao_cube);

    // vbo position
    vbo_cube_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_position);
    gl.bufferData(gl.ARRAY_BUFFER, cubePosition, gl.STATIC_DRAW);
    gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    // vbo color
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

    // Clear The Screen By Black Color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    // Texture
    texture_kundali = gl.createTexture();
    texture_kundali.image = new Image();
    texture_kundali.image.src = "Vijay_Kundali.png";
    texture_kundali.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_kundali);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);      

        gl.texImage2D(gl.TEXTURE_2D,
                      0,
                      gl.RGBA,
                      gl.RGBA,
                      gl.UNSIGNED_BYTE,
                      texture_kundali.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    texture_stone = gl.createTexture();
    texture_stone.image = new Image();
    texture_stone.image.src = "Stone.png";
    texture_stone.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_stone);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_stone.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Enabling The Texture
    //gl.enable(gl.TEXTURE_2D);

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


    // Pyramid
    // Transformations
    var translationMatrix = mat4.create();
    var rotationMatrix = mat4.create();
    var rotationMatrix_X = mat4.create();
    var rotationMatrix_Y = mat4.create();
    var rotationMatrix_Z = mat4.create();
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.translate(translationMatrix, translationMatrix, [-1.5, 0.0, -6.0]);
    mat4.rotateY(rotationMatrix, rotationMatrix, anglePyramid);
    mat4.multiply(modelViewMatrix, translationMatrix, rotationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, texture_stone);
    gl.uniform1i(textureSamplerUniform, 0);

    gl.bindVertexArray(vao_pyramid);
        gl.drawArrays(gl.TRIANGLES, 0, 12);
    gl.bindVertexArray(null);

    gl.bindTexture(gl.TEXTURE_2D, null);

    // Cube
    // Transformations
    translationMatrix = mat4.create();
    rotationMatrix = mat4.create();
    scaleMatrix = mat4.create();
    modelViewMatrix = mat4.create();
    modelViewProjectionMatrix = mat4.create();

    mat4.translate(translationMatrix, translationMatrix, [1.5, 0.0, -6.0]);
    mat4.scale(scaleMatrix, scaleMatrix, [0.75, 0.75, 0.75]);
    mat4.rotateX(rotationMatrix_X, rotationMatrix_X, angleCube);
    mat4.rotateY(rotationMatrix_Y, rotationMatrix_Y, angleCube);
    mat4.rotateZ(rotationMatrix_Z, rotationMatrix_Z, angleCube);
    mat4.multiply(rotationMatrix, rotationMatrix_X, rotationMatrix_Y);
    mat4.multiply(rotationMatrix, rotationMatrix, rotationMatrix_Z);
    mat4.multiply(modelViewMatrix, translationMatrix, scaleMatrix);
    mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, texture_kundali);
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
    anglePyramid = anglePyramid + 0.05;
    if (anglePyramid >= (2 * Math.PI))
        anglePyramid = anglePyramid - (2 * Math.PI);

    angleCube = angleCube + 0.05;
    if (angleCube >= (2 * Math.PI))
        angleCube = angleCube - (2 * Math.PI);
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
    }
}

// Mouse Event Listener
function mouseDown() {

    // Code
}

function uninitialize() {

    // Code
    if (texture_stone) {
        gl.deleteTexture(1, texture_stone);
        texture_stone = 0;
    }

    if (texture_kundali) {
        gl.deleteTexture(1, texture_kundali);
        texture_kundali = 0;
    }

    if (vbo_cube_position) {
        gl.deleteBuffer(vbo_cube_position);
        vbo_cube_position = null;
    }

    if (vbo_pyramid_position) {
        gl.deleteBuffer(vbo_pyramid_position);
        vbo_pyramid_position = null;
    }

    if (vbo_cube_color) {
        gl.deleteBuffer(vbo_cube_color);
        vbo_cube_color = null;
    }

    if (vbo_pyramid_color) {
        gl.deleteBuffer(vbo_pyramid_color);
        vbo_pyramid_color = null;
    }

    if (vao_cube) {
        gl.deleteVertexArray(vao_cube);
        vao_cube = null;
    }

    if (vao_pyramid) {
        gl.deleteVertexArray(vao_pyramid);
        vao_pyramid = null;
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
