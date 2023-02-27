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
var vbo_pyramid_normal;

var anglePyramid = 0.0;
var bLight = false;

var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

var laUniform = new Array(2);				// Ambient
var ldUniform = new Array(2);				// Diffuse
var lsUniform = new Array(2);				// Specular
var lightPositionUniform = new Array(2);

var kaUniform;
var kdUniform;
var ksUniform;
var materialShininessUniform;

var lightingEnabledUniform;

var lightAmbient = new Array([2][3]);
var lightDiffuse = new Array([2][3]);
var lightSpecular = new Array([2][3]);
var lightPosition = new Array([2][4]);

var materialAmbient = [ 0.0, 0.0, 0.0];
var materialDiffuse = [ 1.0, 1.0, 1.0];
var materialSpecular = [ 1.0, 1.0, 1.0];
var materialShininess = 50.0;

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
        "uniform vec3 u_la[2];" +
        "uniform vec3 u_ld[2];" +
        "uniform vec3 u_ls[2];" +
        "uniform vec4 u_lightPosition[2];" +
        "uniform vec3 u_ka;" +
        "uniform vec3 u_kd;" +
        "uniform vec3 u_ks;" +
        "uniform float u_materialShininess;" +
        "uniform int u_lightingEnabled;" +
        "out vec3 phong_ads_light;" +
        "void main(void)" +
        "{" +
        "if (u_lightingEnabled == 1)" +
        "{" +
        "vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" +
        "mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" +
        "vec3 transformedNormals = normalize(normalMatrix * a_normal);" +
        "vec3 viewerVector = normalize(-eyeCoordinates.xyz);" +
        "vec3 ambient[2];" +
        "vec3 lightDirection[2];" +
        "vec3 diffuse[2];" +
        "vec3 reflectionVector[2];" +
        "vec3 specular[2];" +
        "for (int i = 0; i < 2; i++)" +
        "{" +
        "ambient[i] = u_la[i] * u_ka;" +
        "lightDirection[i] = normalize(vec3(u_lightPosition[i]) - eyeCoordinates.xyz);" +
        "diffuse[i] = u_ld[i] * u_kd * max(dot(lightDirection[i], transformedNormals), 0.0);" +
        "reflectionVector[i] = reflect(-lightDirection[i], transformedNormals);" +
        "specular[i] = u_ls[i] * u_ks * pow(max(dot(reflectionVector[i], viewerVector), 0.0), u_materialShininess);" +
        "phong_ads_light = phong_ads_light + ambient[i] + diffuse[i] + specular[i];" +
        "}" +
        "}" +
        "else" +
        "{" +
        "phong_ads_light = vec3(1.0, 1.0, 1.0);" +
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
    const fragmentShaderSourceCode =
        "#version 300 es" +
        "\n" +
        "precision highp float;" +
        "in vec3 phong_ads_light;" +
        "out vec4 FragColor;" +
        "void main(void)" +
        "{" +
        "FragColor = vec4(phong_ads_light, 1.0);" +
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
    laUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_la[0]");
    laUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_la[1]");
    ldUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_ld[0]");
    ldUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_ld[1]");
    lsUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_ls[0]");
    lsUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_ls[1]");
    lightPositionUniform[0] = gl.getUniformLocation(shaderProgramObject, "u_lightPosition[0]");
    lightPositionUniform[1] = gl.getUniformLocation(shaderProgramObject, "u_lightPosition[1]");
    kaUniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
    kdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
    ksUniform = gl.getUniformLocation(shaderProgramObject, "u_ks");
    materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "u_materialShininess");
    lightingEnabledUniform = gl.getUniformLocation(shaderProgramObject, "u_lightingEnabled");

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

    var pyramidNormals = new Float32Array([
                                                0.0, 0.447214, 0.894427,// front-top
                                                0.0, 0.447214, 0.894427,// front-left
                                                0.0, 0.447214, 0.894427,// front-right

                                                0.894427, 0.447214, 0.0, // right-top
                                                0.894427, 0.447214, 0.0, // right-left
                                                0.894427, 0.447214, 0.0, // right-right

                                                0.0, 0.447214, -0.894427, // back-top
                                                0.0, 0.447214, -0.894427, // back-left
                                                0.0, 0.447214, -0.894427, // back-right

                                                -0.894427, 0.447214, 0.0, // left-top
                                                -0.894427, 0.447214, 0.0, // left-left
                                                -0.894427, 0.447214, 0.0 // left-right
                                            ]);

    // vao pyramid
    vao_pyramid = gl.createVertexArray();
    gl.bindVertexArray(vao_pyramid);

        // vbo position
        vbo_pyramid_position = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_position);
        gl.bufferData(gl.ARRAY_BUFFER, pyramidPosition, gl.STATIC_DRAW);
        gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

        // vbo normal
        vbo_pyramid_normal = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_normal);
        gl.bufferData(gl.ARRAY_BUFFER, pyramidNormals, gl.STATIC_DRAW);
        gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_NORMAL);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);

    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // Clear The Screen By Black Color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    lightAmbient[0] = [0.0, 0.0, 0.0];
    lightDiffuse[0] = [1.0, 0.0, 0.0];
    lightSpecular[0] = [1.0, 0.0, 0.0];
    lightPosition[0] = [-2.0, 0.0, 0.0, 1.0];

    lightAmbient[1] = [0.0, 0.0, 0.0];
    lightDiffuse[1] = [0.0, 0.0, 1.0];
    lightSpecular[1] = [0.0, 0.0, 1.0];
    lightPosition[1] = [2.0, 0.0, 0.0, 1.0];

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
    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();

    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -4.0]);
    mat4.rotateY(rotationMatrix, rotationMatrix, anglePyramid);
    mat4.multiply(modelMatrix, translationMatrix, rotationMatrix);

    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    // Sending Light Related Uniforms
    if (bLight == true)
    {
        gl.uniform1i(lightingEnabledUniform, 1);

        for (var i = 0; i < 2; i++)
        {
            gl.uniform3fv(laUniform[i], lightAmbient[i]);
            gl.uniform3fv(ldUniform[i], lightDiffuse[i]);
            gl.uniform3fv(lsUniform[i], lightSpecular[i]);
            gl.uniform4fv(lightPositionUniform[i], lightPosition[i]);
        }

        gl.uniform3fv(kaUniform, materialAmbient);
        gl.uniform3fv(kdUniform, materialDiffuse);
        gl.uniform3fv(ksUniform, materialSpecular);
        gl.uniform1f(materialShininessUniform, materialShininess);
    }
    else
    {
        gl.uniform1i(lightingEnabledUniform, 0);
    }

    gl.bindVertexArray(vao_pyramid);

        gl.drawArrays(gl.TRIANGLES, 0, 12);

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
    anglePyramid = anglePyramid + 0.05;
    if (anglePyramid >= (2*Math.PI))
        anglePyramid = anglePyramid - (2 * Math.PI);
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
    if (vbo_pyramid_normal) {
        gl.deleteBuffer(vbo_pyramid_normal);
        vbo_pyramid_normal = null;
    }

    if (vbo_pyramid_position) {
        gl.deleteBuffer(vbo_pyramid_position);
        vbo_pyramid_position = null;
    }

    if (vao_pyramid)
    {
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
