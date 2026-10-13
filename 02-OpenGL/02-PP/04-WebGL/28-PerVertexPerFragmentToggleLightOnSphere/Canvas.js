// Global Variable Declarations
var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

var sphere = null;
var bLight = false;
var choosenShader = 'v';

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

var laUniform = new Array(2);				// Ambient
var ldUniform = new Array(2);				// Diffuse
var lsUniform = new Array(2);				// Specular
var lightPositionUniform = new Array(2);

var kaUniform = new Array(2);
var kdUniform = new Array(2);
var ksUniform = new Array(2);
var materialShininessUniform = new Array(2);

var lightingEnabledUniform = new Array(2);

var lightAmbient = [ 0.1, 0.1, 0.1];
var lightDiffuse = [ 1.0, 1.0, 1.0];
var lightSpecular = [ 1.0, 1.0, 1.0];
var lightPosition = [ 100.0, 100.0, 100.0, 1.0 ];

var materialAmbient = [ 0.0, 0.0, 0.0];
var materialDiffuse = [ 0.5, 0.2, 0.7];
var materialSpecular = [ 0.7, 0.7, 0.7];
var materialShininess = 128.0;

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
        "uniform vec3 u_la;" +
        "uniform vec3 u_ld;" +
        "uniform vec3 u_ls;" +
        "uniform vec4 u_lightPosition;" +
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
        "vec3 ambient = u_la * u_ka;" +
        "vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" +
        "mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" +
        "vec3 transformedNormals = normalize(normalMatrix * a_normal);" +
        "vec3 lightDirection = normalize(vec3(u_lightPosition) - eyeCoordinates.xyz);" +
        "vec3 diffuse = u_ld * u_kd * max(dot(lightDirection, transformedNormals), 0.0);" +
        "vec3 reflectionVector = reflect(-lightDirection, transformedNormals);" +
        "vec3 viewerVector = normalize(-eyeCoordinates.xyz);" +
        "vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector, viewerVector), 0.0), u_materialShininess);" +
        "phong_ads_light = ambient + diffuse + specular;" +
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
        "uniform vec4 u_lightPosition;" +
        "uniform int u_lightingEnabled;" +
        "out vec3 transformedNormals;" +
        "out vec3 lightDirection;" +
        "out vec3 viewerVector;" +
        "void main(void)" +
        "{" +
        "if (u_lightingEnabled == 1)" +
        "{" +
        "vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" +
        "mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" +
        "transformedNormals = normalMatrix * a_normal;" +
        "lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;" +
        "viewerVector = -eyeCoordinates.xyz;" +
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
        "in vec3 lightDirection;" +
        "in vec3 viewerVector;" +
        "uniform vec3 u_la;" +
        "uniform vec3 u_ld;" +
        "uniform vec3 u_ls;" +
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
        "vec3 ambient = u_la * u_ka;" +
        "vec3 normalized_tranformed_normals = normalize(transformedNormals);" +
        "vec3 normalized_lightDirection = normalize(lightDirection);" +
        "vec3 diffuse = u_ld * u_kd * max(dot(normalized_lightDirection, normalized_tranformed_normals), 0.0);" +
        "vec3 reflectionVector = reflect(-normalized_lightDirection, normalized_tranformed_normals);" +
        "vec3 normalized_viewerVector = normalize(viewerVector);" +
        "vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector, normalized_viewerVector), 0.0), u_materialShininess);" +
        "phong_ads_light = ambient + diffuse + specular;" +
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
    laUniform[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_la");
    ldUniform[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_ld");
    lsUniform[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_ls");
    lightPositionUniform[0] = gl.getUniformLocation(shaderProgramObject_pv, "u_lightPosition");
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
    laUniform[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_la");
    ldUniform[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_ld");
    lsUniform[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_ls");
    lightPositionUniform[1] = gl.getUniformLocation(shaderProgramObject_pf, "u_lightPosition");
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

        if (bLight == true)
        {
            gl.uniform1i(lightingEnabledUniform[0], 1);

            gl.uniform3fv(laUniform[0], lightAmbient);
            gl.uniform3fv(ldUniform[0], lightDiffuse);
            gl.uniform3fv(lsUniform[0], lightSpecular);
            gl.uniform4fv(lightPositionUniform[0], lightPosition);

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

        if (bLight == true)
        {
            gl.uniform1i(lightingEnabledUniform[1], 1);

            gl.uniform3fv(laUniform[1], lightAmbient);
            gl.uniform3fv(ldUniform[1], lightDiffuse);
            gl.uniform3fv(lsUniform[1], lightSpecular);
            gl.uniform4fv(lightPositionUniform[1], lightPosition);

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
        
    //gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    //gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    //gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    // Sending Light Related Uniforms
   

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
