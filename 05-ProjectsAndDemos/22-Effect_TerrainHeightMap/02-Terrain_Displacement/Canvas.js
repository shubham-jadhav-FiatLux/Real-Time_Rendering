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

// Terrain related variables
var gridWidth = 40;
var gridDepth = 40;

var vao;
var vbo_position;
var vbo_element;

var texture_disp = null;

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
        /*"in vec2 a_texcoord;" +*/
        "uniform mat4 u_mvpMatrix;" +
        /*"out vec2 a_texcoord_out;" +*/
        "void main(void)" +
        "{" +
        "gl_Position = u_mvpMatrix * a_position;" +
        /*"a_texcoord_out = a_texcoord;" +*/
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
        /*"in vec2 a_texcoord_out;" +
        "uniform sampler2D u_textureSampler;" +*/
        "out vec4 FragColor;" +
        "void main(void)" +
        "{" +
        /*"FragColor = texture(u_textureSampler, a_texcoord_out);" +*/
        "FragColor = vec4(1.0, 0.0, 0.0, 1.0);" +
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

    
    // Texture
    texture_disp = gl.createTexture();
    texture_disp.image = new Image();
    texture_disp.image.src = "Terrain.png";
    texture_disp.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_disp);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_disp.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Enabling The Texture
    //gl.enable(gl.TEXTURE_2D);

    var ctx = document.createElement('canvas').getContext('2d');
    ctx.canvas.width = texture_disp.image.width;
    ctx.canvas.height = texture_disp.image.height;
    ctx.drawImage(texture_disp.image, 0, 0);
    var imgData = ctx.getImageData(0, 0, ctx.canvas.width, ctx.canvas.height);


    //gl.activeTexture(gl.TEXTURE0);
    //gl.bindTexture(gl.TEXTURE_2D, texture_disp);
    //var imgData = gl.getImageData(0, 0, gl.canvas.width, gl.canvas.height);

    //gridWidth = texture_disp.image.width - 1;
    //gridDepth = texture_disp.image.height - 1;
    gridWidth = imgData.width - 1;
    gridDepth = imgData.height - 1;

    
    var gridPoints = new Array();

    for (let z = 0; z <= gridDepth; z++)
    {
        for (let x = 0; x <= gridWidth; x++)
        {
            //var offset = (z * texture_disp.image.width + x) * 4;
            var offset = (z * imgData.width + x) * 4;
            // height from 0 to 10
            //var height = texture_disp.image[offset] * 10 / 255;
            var height = imgData.data[offset] * 10 / 255;
            gridPoints.push(x, height, z);
        }
    }   

    var gridIndices = new Array();
    var rowStride = gridWidth + 1;

    // X lines
    for (let z = 0; z <= gridDepth; z++)
    {
        var rowoff = z * rowStride;
        for (let x = 0; x < gridWidth; x++)
        {
            gridIndices.push(rowoff + x, rowoff + x + 1);
        }
    }
    // Z lines
    for (let x = 0; x <= gridWidth; x++)
    {
        for (let z = 0; z < gridDepth; z++)
        {
            var rowoff = z * rowStride;
            gridIndices.push(rowoff + x, rowoff + x + rowStride);
        }
    }


    // vao
    vao = gl.createVertexArray();
    gl.bindVertexArray(vao);

        
        // vbo position
        vbo_position = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(gridPoints), gl.STATIC_DRAW);
        //gl.bufferData(gl.ARRAY_BUFFER, gridPoints, gl.STATIC_DRAW);
        gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
        
        // vbo Elements
        vbo_element = gl.createBuffer();
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vbo_element);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(gridIndices), gl.STATIC_DRAW);
        //gl.bufferData(gl.ARRAY_BUFFER, gridIndices, gl.STATIC_DRAW);
    /*
        gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_ELEMENTS, 2, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
        */
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);

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

    // Transformations
    var translationMatrix = mat4.create();
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    //mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -10.0]);
    var cameraPosition = [-gridWidth / 8, 10, -gridDepth / 8];
    var target = [gridWidth / 2, -10, gridDepth / 2];
    var up = [0, 1, 0];
    mat4.lookAt(translationMatrix, cameraPosition, target, up);
    mat4.multiply(modelViewMatrix, modelViewMatrix, translationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);

    
   
    
    //gl.uniform1i(textureSamplerUniform, 0);
    

    gl.bindVertexArray(vao);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vbo_element);
    //gl.drawElements(gl.LINES, (gridWidth * 2 * (gridDepth + 1)) + (gridDepth * 2 * (gridWidth + 1)), gl.UNSIGNED_SHORT, 0);
    gl.drawElements(gl.TRIANGLE_FAN, (gridWidth * 2 * (gridDepth + 1)) + (gridDepth * 2 * (gridWidth + 1)), gl.UNSIGNED_SHORT, 0);
    gl.bindVertexArray(null);

    //gl.bindTexture(gl.TEXTURE_2D, null);

    // Unuse The Shader Program Object
    gl.useProgram(null);

    // Double Buffering Imulation
    requestAnimationFrame(display, canvas);
}

function update() {

    // Code
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
    /*
    if (texture_smiley) {
        gl.deleteTexture(1, texture_smiley);
        texture_smiley = 0;
    }
    */

    if (vbo_position) {
        gl.deleteBuffer(vbo_position);
        vbo_position = null;
    }

    if (vbo_elements) {
        gl.deleteBuffer(vbo_elements);
        vbo_elements = null;
    }

    if (vao)
    {
        gl.deleteVertexArray(vao);
        vao = null;
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
