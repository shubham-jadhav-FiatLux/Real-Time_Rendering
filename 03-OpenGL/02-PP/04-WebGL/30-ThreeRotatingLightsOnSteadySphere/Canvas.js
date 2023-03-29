// Global Variable Declarations
var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

var sphere = null;
var bLight = false;
var choosenShader = 'v';
var lightAngleZero = 0.0;
var lightAngleOne = 0.0;
var lightAngleTwo = 0.0;

var shaderProgramObject_pv;
var shaderProgramObject_pf;

const webglMacros =
{
    SBJ_ATTRIBUTE_POSITION: 0,
    SBJ_ATTRIBUTE_COLOR: 1,
    SBJ_ATTRIBUTE_NORMAL: 2,
    SBJ_ATTRIBUTE_TEXTURE: 3
};

var modelMatrixUniform = new Array(2);
var viewMatrixUniform = new Array(2);
var projectionMatrixUniform = new Array(2);

let laUniform_pv = new Array([3]);				// Ambient
let ldUniform_pv = new Array([3]);				// Diffuse
let lsUniform_pv = new Array([3]);				// Specular
let lightPositionUniform_pv = new Array([3]);

let laUniform_pf = new Array([3]);				// Ambient
let ldUniform_pf = new Array([3]);				// Diffuse
let lsUniform_pf = new Array([3]);				// Specular
let lightPositionUniform_pf = new Array([3]);

var kaUniform = new Array(2);
var kdUniform = new Array(2);
var ksUniform = new Array(2);
var materialShininessUniform = new Array(2);

var lightingEnabledUniform = new Array(2);

var lightAmbient = new Array([3][3]);
var lightDiffuse = new Array([3][3]);
var lightSpecular = new Array([3][3]);
var lightPosition = new Array([3][4]);

var materialAmbient = [0.0, 0.0, 0.0];
var materialDiffuse = [1.0, 1.0, 1.0];
var materialSpecular = [1.0, 1.0, 1.0];
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

    // Vertex Shader PerVertex
    var vertexShaderSourceCode_pv =
        "#version 300 es" +
        "\n" +
        "in vec4 a_position;" +
        "in vec3 a_normal;" +
        "uniform mat4 u_modelMatrix;" +
        "uniform mat4 u_viewMatrix;" +
        "uniform mat4 u_projectionMatrix;" +
        "uniform vec3 u_la[3];" +
        "uniform vec3 u_ld[3];" +
        "uniform vec3 u_ls[3];" +
        "uniform vec4 u_lightPosition[3];" +
        "uniform vec3 u_ka;" +
        "uniform vec3 u_kd;" +
        "uniform vec3 u_ks;" +
        "uniform float u_materialShininess;" +
        "uniform int u_lightingEnabled;" +
        "out vec3 phong_ads_light;" +
        "void main(void)" +
        "{" +
        "phong_ads_light = vec3(0.0, 0.0, 0.0);" +
        "if (u_lightingEnabled == 1)" +
        "{" +
        "vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" +
        "mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" +
        "vec3 transformedNormals = normalize(normalMatrix * a_normal);" +
        "vec3 viewerVector = normalize(-eyeCoordinates.xyz);" +
        "vec3 ambient[3];" +
        "vec3 lightDirection[3];" +
        "vec3 diffuse[3];" +
        "vec3 reflectionVector[3];" +
        "vec3 specular[3];" +
        "for (int i = 0; i < 3; i++)" +
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

    var vertexShaderObject_pv = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject_pv, vertexShaderSourceCode_pv);
    gl.compileShader(vertexShaderObject_pv);

    if (gl.getShaderParameter(vertexShaderObject_pv, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(vertexShaderObject_pv);
        if (error.length > 0) {
            alert("Vertex Shader Compilation Log: " + error);
            uninitialize();
        }
    }

    // Vertex Shader PerFragment
    var vertexShaderSourceCode_pf =
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

    var vertexShaderObject_pf = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject_pf, vertexShaderSourceCode_pf);
    gl.compileShader(vertexShaderObject_pf);

    if (gl.getShaderParameter(vertexShaderObject_pf, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(vertexShaderObject_pf);
        if (error.length > 0)
        {
            alert("Vertex Shader Compilation Log: " + error);
            uninitialize();
        }
    }

    // Fragment Shader PerVertex
    var fragmentShaderSourceCode_pv =
        "#version 300 es" +
        "\n" +
        "precision highp float;" +
        "in vec3 phong_ads_light;" +
        "out vec4 FragColor;" +
        "void main(void)" +
        "{" +
        "FragColor = vec4(phong_ads_light, 1.0);" +
        "}";

    var fragmentShaderObject_pv = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject_pv, fragmentShaderSourceCode_pv);
    gl.compileShader(fragmentShaderObject_pv);

    if (gl.getShaderParameter(fragmentShaderObject_pv, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(fragmentShaderObject_pv);
        if (error.length > 0) {
            alert("Fragment Shader Compilation Log: " + error);
            uninitialize();
        }
    }

    // Fragment Shader PerFragment
    var fragmentShaderSourceCode_pf =
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
        "uniform highp int u_lightingEnabled;" +
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

    var fragmentShaderObject_pf = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject_pf, fragmentShaderSourceCode_pf);
    gl.compileShader(fragmentShaderObject_pf);

    if (gl.getShaderParameter(fragmentShaderObject_pf, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(fragmentShaderObject_pf);
        if (error.length > 0)
        {
            alert("Fragment Shader Compilation Log: " + error);
            uninitialize();
        }
    }

    // Shader Program PerVertex
    shaderProgramObject_pv = gl.createProgram();
    gl.attachShader(shaderProgramObject_pv, vertexShaderObject_pv);
    gl.attachShader(shaderProgramObject_pv, fragmentShaderObject_pv);
    // Prelinking Shader Attribute Binding
    gl.bindAttribLocation(shaderProgramObject_pv, webglMacros.SBJ_ATTRIBUTE_POSITION, "a_position");
    gl.bindAttribLocation(shaderProgramObject_pv, webglMacros.SBJ_ATTRIBUTE_NORMAL, "a_normal");
    // Shader Program Linking
    gl.linkProgram(shaderProgramObject_pv);
    // Link Error Checking
    if (gl.getProgramParameter(shaderProgramObject_pv, gl.LINK_STATUS) == false)
    {
        var error = gl.getProgramInfoLog(shaderProgramObject_pv);
        if (error.length > 0)
        {
            alert("Shader Program Compilation Log: " + error);
            uninitialize();
        }
    }

    modelMatrixUniform[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_modelMatrix");
    viewMatrixUniform[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_viewMatrix");
    projectionMatrixUniform[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_projectionMatrix");
    laUniform_pv[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_la[0]");
    laUniform_pv[1] = gl.getUniformLocation(shaderProgramObject_pv, "u_la[1]");
    laUniform_pv[2] = gl.getUniformLocation(shaderProgramObject_pv, "u_la[2]");
    ldUniform_pv[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_ld[0]");
    ldUniform_pv[1] = gl.getUniformLocation(shaderProgramObject_pv, "u_ld[1]");
    ldUniform_pv[2] = gl.getUniformLocation(shaderProgramObject_pv, "u_ld[2]");
    lsUniform_pv[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_ls[0]");
    lsUniform_pv[1] = gl.getUniformLocation(shaderProgramObject_pv, "u_ls[1]");
    lsUniform_pv[2] = gl.getUniformLocation(shaderProgramObject_pv, "u_ls[2]");
    lightPositionUniform_pv[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_lightPosition[0]");
    lightPositionUniform_pv[1] = gl.getUniformLocation(shaderProgramObject_pv, "u_lightPosition[1]");
    lightPositionUniform_pv[2] = gl.getUniformLocation(shaderProgramObject_pv, "u_lightPosition[2]");
    kaUniform[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_ka");
    kdUniform[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_kd");
    ksUniform[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_ks");
    materialShininessUniform[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_materialShininess");
    lightingEnabledUniform[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_lightingEnabled");


    // Shader Program PerFragment
    shaderProgramObject_pf = gl.createProgram();
    gl.attachShader(shaderProgramObject_pf, vertexShaderObject_pf);
    gl.attachShader(shaderProgramObject_pf, fragmentShaderObject_pf);
    // Prelinking Shader Attribute Binding
    gl.bindAttribLocation(shaderProgramObject_pf, webglMacros.SBJ_ATTRIBUTE_POSITION, "a_position");
    gl.bindAttribLocation(shaderProgramObject_pf, webglMacros.SBJ_ATTRIBUTE_NORMAL, "a_normal");
    // Shader Program Linking
    gl.linkProgram(shaderProgramObject_pf);
    // Link Error Checking
    if (gl.getProgramParameter(shaderProgramObject_pf, gl.LINK_STATUS) == false) {
        var error = gl.getProgramInfoLog(shaderProgramObject_pf);
        if (error.length > 0) {
            alert("Fragment Shader Compilation Log: " + error);
            uninitialize();
        }
    }

    modelMatrixUniform[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_modelMatrix");
    viewMatrixUniform[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_viewMatrix");
    projectionMatrixUniform[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_projectionMatrix");
    laUniform_pf[0] = gl.getUniformLocation(shaderProgramObject_pf, "u_la[0]");
    laUniform_pf[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_la[1]");
    laUniform_pf[2] = gl.getUniformLocation(shaderProgramObject_pf, "u_la[2]");
    ldUniform_pf[0] = gl.getUniformLocation(shaderProgramObject_pf, "u_ld[0]");
    ldUniform_pf[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_ld[1]");
    ldUniform_pf[2] = gl.getUniformLocation(shaderProgramObject_pf, "u_ld[2]");
    lsUniform_pf[0] = gl.getUniformLocation(shaderProgramObject_pf, "u_ls[0]");
    lsUniform_pf[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_ls[1]");
    lsUniform_pf[2] = gl.getUniformLocation(shaderProgramObject_pf, "u_ls[2]");
    lightPositionUniform_pf[0] = gl.getUniformLocation(shaderProgramObject_pf, "u_lightPosition[0]");
    lightPositionUniform_pf[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_lightPosition[1]");
    lightPositionUniform_pf[2] = gl.getUniformLocation(shaderProgramObject_pf, "u_lightPosition[2]");
    kaUniform[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_ka");
    kdUniform[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_kd");
    ksUniform[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_ks");
    materialShininessUniform[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_materialShininess");
    lightingEnabledUniform[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_lightingEnabled");


    sphere = new Mesh();
    makeSphere(sphere, 1.0, 30, 30);

    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // Clear The Screen By Black Color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    lightAmbient[0] = [0.0, 0.0, 0.0];
    lightDiffuse[0] = [1.0, 0.0, 0.0];
    lightSpecular[0] = [1.0, 0.0, 0.0];
    lightPosition[0] = [0.0, 0.0, 0.0, 1.0];

    lightAmbient[1] = [0.0, 0.0, 0.0];
    lightDiffuse[1] = [0.0, 1.0, 0.0];
    lightSpecular[1] = [0.0, 1.0, 0.0];
    lightPosition[1] = [0.0, 0.0, 0.0, 1.0];

    lightAmbient[2] = [0.0, 0.0, 0.0];
    lightDiffuse[2] = [0.0, 0.0, 1.0];
    lightSpecular[2] = [0.0, 0.0, 1.0];
    lightPosition[2] = [0.0, 0.0, 0.0, 1.0];

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

    // Cube
    // Transformations
    var translationMatrix = mat4.create();
    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();

    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -4.0]);
    mat4.multiply(modelMatrix, modelMatrix, translationMatrix);

    // Use The Shader Program Object
    if (choosenShader == 'v') {
        gl.useProgram(shaderProgramObject_pv);

        gl.uniformMatrix4fv(modelMatrixUniform[0], false, modelMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform[0], false, viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform[0], false, perspectiveProjectionMatrix);

        // Sending Light Related Uniforms
        if (bLight == true)
        {
           
            gl.uniform1i(lightingEnabledUniform[0], 1);

            
                for (var j = 0; j < 3; j++)
                {
                    gl.uniform3fv(laUniform_pv[j], lightAmbient[j]);
                    gl.uniform3fv(ldUniform_pv[j], lightDiffuse[j]);
                    gl.uniform3fv(lsUniform_pv[j], lightSpecular[j]);
                    gl.uniform4fv(lightPositionUniform_pv[j], lightPosition[j]);
                }
                gl.uniform3fv(kaUniform[0], materialAmbient);
                gl.uniform3fv(kdUniform[0], materialDiffuse);
                gl.uniform3fv(ksUniform[0], materialSpecular);
                gl.uniform1f(materialShininessUniform[0], materialShininess);
        }
        else
        {
            gl.uniform1i(lightingEnabledUniform[0], 0);
        }
    }
    else
    {
        gl.useProgram(shaderProgramObject_pf);

        gl.uniformMatrix4fv(modelMatrixUniform[1], false, modelMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform[1], false, viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform[1], false, perspectiveProjectionMatrix);

        // Sending Light Related Uniforms
        if (bLight == true)
        {
            gl.uniform1i(lightingEnabledUniform[1], 1);

            for (var i = 0; i < 3; i++)
            {
                gl.uniform3fv(laUniform_pf[i], lightAmbient[i]);
                gl.uniform3fv(ldUniform_pf[i], lightDiffuse[i]);
                gl.uniform3fv(lsUniform_pf[i], lightSpecular[i]);
                gl.uniform4fv(lightPositionUniform_pf[i], lightPosition[i]);
            }

            gl.uniform3fv(kaUniform[1], materialAmbient);
            gl.uniform3fv(kdUniform[1], materialDiffuse);
            gl.uniform3fv(ksUniform[1], materialSpecular);
            gl.uniform1f(materialShininessUniform[1], materialShininess);
        }
        else
        {
            gl.uniform1i(lightingEnabledUniform[1], 0);
        }
    }

    sphere.draw();

    // Unuse The Shader Program Object
    gl.useProgram(null);

    // Update The Scene
    update();

    // Double Buffering Imulation
    requestAnimationFrame(display, canvas);
}

function update() {

    // Code
    lightAngleZero += 2.0;
    lightAngleOne += 2.0;
    lightAngleTwo += 2.0;

    lightPosition[0][1] = 10 * Math.sin(lightAngleZero / 180.0 * 3.14);
    lightPosition[0][2] = 10 * Math.cos(lightAngleZero / 180.0 * 3.14);

    lightPosition[1][0] = 10 * Math.sin(lightAngleZero * 3.14 / 180.0);
    lightPosition[1][2] = 10 * Math.cos(lightAngleZero * 3.14 / 180.0);

    lightPosition[2][0] = 10 * -Math.sin(lightAngleZero * 3.14 / 180.0);
    lightPosition[2][1] = 10 * Math.cos(lightAngleZero * 3.14 / 180.0);
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

        case 84:
            toggleFullscreen();
            break;

        case 70:
            choosenShader = 'f';
            break;

        case 86:
            choosenShader = 'v';
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

    if (shaderProgramObject_pv)
    {
        gl.useProgram(shaderProgramObject_pv);
        var shaderObjects = gl.getAttachedShaders(shaderProgramObject_pv);

        for (let i = 0; i < shaderObjects.length; i++)
        {
            gl.detachShader(shaderProgramObject_pv, shaderObjects[i]);
            gl.deleteShader(shaderObjects[i]);
            shaderObjects[i] = 0;
        }

        gl.useProgram(null);

        gl.deleteProgram(shaderProgramObject_pv);
        shaderProgramObject_pv = null;
    }

    if (shaderProgramObject_pf) {
        gl.useProgram(shaderProgramObject_pf);
        var shaderObjects = gl.getAttachedShaders(shaderProgramObject_pf);

        for (let i = 0; i < shaderObjects.length; i++) {
            gl.detachShader(shaderProgramObject_pf, shaderObjects[i]);
            gl.deleteShader(shaderObjects[i]);
            shaderObjects[i] = 0;
        }

        gl.useProgram(null);

        gl.deleteProgram(shaderProgramObject_pf);
        shaderProgramObject_pf = null;
    }

    if (sphere) {
        sphere.deallocate();
        sphere = null;
    }
}
