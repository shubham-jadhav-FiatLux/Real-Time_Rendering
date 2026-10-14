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

// Grass Related Global Variables
var time = 1.0;
var r = 550.0;
var tex_grass_color;
var tex_grass_length;
var tex_grass_orientation;
var tex_grass_bend;

var y = 0.0;
var z = 0.0;
var waveAmount = 1.0;
var waveAmountUniform;

var vao_grass;
var vbo_grass_position;

let mvpMatrixUniform;
//var textureSamplerUniform;

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
        "uniform mat4 u_mvpMatrix;" +
        "uniform float wave;" +
        /*"uniform float grasspalette_texture;" +*/
        "uniform sampler2D length_texture;" +
        "uniform sampler2D orientation_texture;" +
        "uniform sampler2D grasscolor_texture;" +
        "uniform sampler2D bend_texture;" +
        "out vec4 color;" +
        "int random(int seed, int iterations)" +
        "{" +
        "int value = seed;" +
        "int n;" +
        "for(n = 0; n < iterations; n++)" +
        "{" +
        "value = ((value >> 7) ^ (value << 9)) * 15485863;" +
        "}" +
        "return value;" +
        "}" +
        "vec4 random_vector(int seed)" +
        "{" +
        "int r = random(gl_InstanceID, 4);" +
        "int g = random(r, 2);" +
        "int b = random(g, 2);" +
        "int a = random(b, 2);" +
        "return vec4(float(r & 0x3FF) / 1024.0,float(g & 0x3FF) / 1024.0,float(b & 0x3FF) / 1024.0,float(a & 0x3FF) / 1024.0);" +
        "}" +
        "mat4 construct_rotation_matrix(float angle)" +
        "{" +
        "float st = sin(angle);" +
        "float ct = cos(angle);" +
        "return mat4(vec4(ct, 0.0, st, 0.0), vec4(0.0, 1.0, 0.0, 0.0), vec4(-st, 0.0, ct, 0.0), vec4(0.0, 0.0, 0.0, 1.0));" +
        "}" +
        "void main(void)" +
        "{" +
        "vec4 offset = vec4(float(gl_InstanceID >> 10) - 512.0, 0.0f, float(gl_InstanceID & 0x3FF) - 512.0, 0.0f);" +
        "int number1 = random(gl_InstanceID, 3);" +
        "int number2 = random(number1, 2);" +
        "offset += vec4(float(number1 & 0xFF) / 256.0, 0.0f, float(number2 & 0xFF) / 256.0, 0.0f);" +
        "vec2 texcoord = offset.xz / 1024.0 + vec2(0.5);" +
        "float bend_factor = texture(bend_texture, texcoord).r * 2.0;" +
        "float bend_amount = cos(a_position.y);" +
        /*"float bend_amount = cos(wave);" +*/
        "float angle = texture(orientation_texture, texcoord).r * 2.0 * 3.141592;" +
        "mat4 rot = construct_rotation_matrix(angle);" +
        "vec4 position = (rot * (a_position + vec4(0.0, 0.0, bend_amount * bend_factor, 0.0))) + offset;" +
        "position *= vec4(1.0, texture(length_texture, texcoord).r * 0.9 + 0.3, 1.0, 1.0);" +
        "gl_Position = u_mvpMatrix * position;" +
        /*"color = texture(grasspalette_texture, texture(grasscolor_texture, texcoord).r) + vec4(random_vector(gl_InstanceID).xyz * vec3(0.1, 0.5, 0.1), 1.0);" +*/
        "color = texture(grasscolor_texture, texcoord).r + vec4(random_vector(gl_InstanceID).xyz * vec3(0.1, 0.5, 0.1), 1.0);" +
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
        "in vec4 color;" +
        "out vec4 FragColor;" +
        "void main(void)" +
        "{" +
        "FragColor = color;" +
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
    //gl.bindAttribLocation(shaderProgramObject, webglMacros.SBJ_ATTRIBUTE_TEXTURE, "a_texcoord");
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
    //textureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "u_textureSampler");
    wave = gl.getUniformLocation(shaderProgramObject, "wave");


    // Declaration Of Vertex Data Arrays
    var grass_blade_position = new Float32Array([
												    -0.3, 0.0,
                                                    0.3, 0.0,
                                                    -0.20, 1.0,
                                                    0.1, 1.3,
                                                    -0.05, 2.3,
                                                    0.0, 3.3
                                                ]);

    var squareTexcoord = new Float32Array([
                                            1.0, 1.0,
                                            0.0, 1.0,
                                            0.0, 0.0,
                                            1.0, 0.0
                                        ]);

    // vao
    vao_grass = gl.createVertexArray();
    gl.bindVertexArray(vao_grass);

        // vbo position
        vbo_grass_position = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_grass_position);
        gl.bufferData(gl.ARRAY_BUFFER, grass_blade_position, gl.STATIC_DRAW);
        gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 2, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);


        /*
        // vbo texcoord
        vbo_texcoord = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texcoord);
        gl.bufferData(gl.ARRAY_BUFFER, squareTexcoord, gl.STATIC_DRAW);
        gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_TEXTURE, 2, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_TEXTURE);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
        */

    gl.bindVertexArray(null);

    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // Clear The Screen By Black Color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);



    // tex_grass_length
    tex_grass_length = gl.createTexture();
    tex_grass_length.image = new Image();
    tex_grass_length.image.src = "grass_length.png";
    tex_grass_length.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, tex_grass_length);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);      

        gl.texImage2D(gl.TEXTURE_2D,
                      0,
                      gl.RGBA,
                      gl.RGBA,
                      gl.UNSIGNED_BYTE,
                    tex_grass_length.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // tex_grass_orientation
    tex_grass_orientation = gl.createTexture();
    tex_grass_orientation.image = new Image();
    tex_grass_orientation.image.src = "grass_orientation.png";
    tex_grass_orientation.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, tex_grass_orientation);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            tex_grass_orientation.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // tex_grass_color
    tex_grass_color = gl.createTexture();
    tex_grass_color.image = new Image();
    tex_grass_color.image.src = "grass_color.png";
    tex_grass_color.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, tex_grass_color);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            tex_grass_color.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // tex_grass_bend
    tex_grass_bend = gl.createTexture();
    tex_grass_bend.image = new Image();
    tex_grass_bend.image.src = "grass_bend.png";
    tex_grass_bend.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, tex_grass_bend);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            tex_grass_bend.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };
  
    /*
    // Texture
    texture_smiley = gl.createTexture();
    texture_smiley.image = new Image();
    texture_smiley.image.src = "Smiley.png";
    texture_smiley.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_smiley);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_smiley.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Texture
    texture_smiley = gl.createTexture();
    texture_smiley.image = new Image();
    texture_smiley.image.src = "Smiley.png";
    texture_smiley.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_smiley);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_smiley.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };
    */

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

    // Transformations
    var translationMatrix = mat4.create();
    var viewMatrix = mat4.create();
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.lookAt(viewMatrix, [0.0, 10.0 + y, 1000.0 + z], [0.0, 0.0, -500.0], [0.0, 1.0, 0.0]);
    //mat4.rotateZ(translationMatrix, translationMatrix, waveAmount);
    //mat4.multiply(modelViewMatrix, modelViewMatrix, translationMatrix);
    mat4.multiply(modelViewMatrix, modelViewMatrix, viewMatrix);

    
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);

    gl.uniform1f(waveAmountUniform, waveAmount);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, tex_grass_length);
    //gl.uniform1i(textureSamplerUniform, 0);

    gl.activeTexture(gl.TEXTURE1);
    gl.bindTexture(gl.TEXTURE_2D, tex_grass_orientation);
    //gl.uniform1i(textureSamplerUniform, 1);

    gl.activeTexture(gl.TEXTURE2);
    gl.bindTexture(gl.TEXTURE_2D, tex_grass_color);
    //gl.uniform1i(textureSamplerUniform, 2);

    gl.activeTexture(gl.TEXTURE3);
    gl.bindTexture(gl.TEXTURE_2D, tex_grass_bend);
    //gl.uniform1i(textureSamplerUniform, 3);

    gl.bindVertexArray(vao_grass);
    gl.drawArraysInstanced(gl.TRIANGLE_STRIP, 0, 6, 1024 * 1024);
    gl.bindVertexArray(null);

    gl.bindTexture(gl.TEXTURE_2D, null);

    // Unuse The Shader Program Object
    gl.useProgram(null);

    // Double Buffering Imulation
    requestAnimationFrame(display, canvas);
}

function update() {

    // Code
    time += 0.05;
    waveAmount += 0.1;
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
            y += 1.5;
            break;

        case 83:
            y -= 1.5;
            break;

        case 66:
            z += 1.5;
            break;

        case 78:
            z -= 1.5;
            break;
    }
}

// Mouse Event Listener
function mouseDown() {

    // Code
}

function uninitialize() {

    // Code
    /*
    if (texture_smiley) {
        gl.deleteTexture(1, texture_smiley);
        texture_smiley = 0;
    }*/

    if (vbo_grass_position) {
        gl.deleteBuffer(vbo_grass_position);
        vbo_grass_position = null;
    }

    /*
    if (vbo_texcoord) {
        gl.deleteBuffer(vbo_texcoord);
        vbo_texcoord = null;
    }*/

    if (vao_grass)
    {
        gl.deleteVertexArray(vao_grass);
        vao_grass = null;
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
