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

var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

var laUniform;				// Ambient
var ldUniform;				// Diffuse
var lsUniform;				// Specular
var lightPositionUniform;

var kaUniform;
var kdUniform;
var ksUniform;
var materialShininessUniform;

var lightingEnabledUniform;

var bLight = false;
var KeyPressed = 0;
var AngleForXRotation = 0.0;
var AngleForYRotation = 0.0;
var AngleForZRotation = 0.0;
var currentWidth;
var currentHeight;

var lightAmbient = [ 1.0, 1.0, 1.0];
var lightDiffuse = [ 1.0, 1.0, 1.0];
var lightSpecular = [ 1.0, 1.0, 1.0];
var lightPosition = [ 0.0, 0.0, 2.0, 1.0 ];

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
    currentWidth = canvas.width;
    currentHeight = canvas.height;

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
    laUniform = gl.getUniformLocation(shaderProgramObject, "u_la");
    ldUniform = gl.getUniformLocation(shaderProgramObject, "u_ld");
    lsUniform = gl.getUniformLocation(shaderProgramObject, "u_ls");
    lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_lightPosition");
    kaUniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
    kdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
    ksUniform = gl.getUniformLocation(shaderProgramObject, "u_ks");
    materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "u_materialShininess");
    lightingEnabledUniform = gl.getUniformLocation(shaderProgramObject, "u_lightingEnabled");

    sphere = new Mesh();
    makeSphere(sphere, 0.8, 30, 30);

    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // Clear The Screen By Black Color
    gl.clearColor(0.3, 0.3, 0.3, 1.0);

    perspectiveProjectionMatrix =  mat4.create();
}

function resize() {

    // Code
    if (bFullscreen == true) {

        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
        currentWidth = window.innerWidth;
        currentHeight = window.innerHeight;
    }
    else {

        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
        currentWidth = canvas_original_width;
        currentHeight = canvas_original_height;
    }

    if (canvas.height == 0)
        canvas.heigth = 1;

    gl.viewport(0, 0, canvas.width, canvas.height);

    mat4.perspective(perspectiveProjectionMatrix, 45.0, (parseFloat(canvas.width) * 1.5) / parseFloat(canvas.height), 0.1, 100.0);
}

function display() {

    // Code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    // Use The Shader Program Object
    gl.useProgram(shaderProgramObject);

    // Cube
    // Transformations
    var translationMatrix = mat4.create();
    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();

    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -3.0]);
    mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
   
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);


    // Sending Light Related Uniforms
    if (bLight == true)
    {
        gl.uniform1i(lightingEnabledUniform, 1);    
    }
    else
    {
        gl.uniform1i(lightingEnabledUniform, 0);
    }

    draw24Spheres();
   
    // Unuse The Shader Program Object
    gl.useProgram(null);

    // Update The Scene
    update();

    // Double Buffering Imulation
    requestAnimationFrame(display, canvas);
}

function update()
{
    // Code
    if (KeyPressed == 1)
    {
        AngleForXRotation += 2.0;
        AngleForYRotation = 0.0;
        AngleForZRotation = 0.0;
    }
    else if (KeyPressed == 2)
    {

        AngleForYRotation += 2.0;
        AngleForXRotation += 2.0;
        AngleForZRotation = 0.0;
    }
    else if (KeyPressed == 3)
    {
        AngleForZRotation += 2.0;
        AngleForXRotation += 2.0;
        AngleForYRotation = 0.0;
    }


    if (KeyPressed == 1)								// X Rotation
    {
        lightPosition[1] = 10 * Math.sin(AngleForXRotation * Math.PI / 180.0);
        lightPosition[2] = 10 * Math.cos(AngleForXRotation * Math.PI / 180.0);
    }
    else if (KeyPressed == 2)							// Y Rotation
    {
        lightPosition[0] = 10 * Math.sin(AngleForYRotation * Math.PI / 180.0);
        lightPosition[2] = 10 * Math.cos(-AngleForYRotation * Math.PI / 180.0);
    }
    else if (KeyPressed == 3)							// Z Rotation
    {
        lightPosition[0] = 10 * -Math.sin(AngleForZRotation * Math.PI / 180.0);
        lightPosition[1] = 10 * Math.cos(AngleForZRotation * Math.PI / 180.0);
    }
    else
    {
        lightPosition[0] = 0.0;
        lightPosition[1] = 0.0;
        lightPosition[2] = 0.0;
    }
}

function draw24Spheres()
{
    // Variable Declarations
    var MaterialAmbient = new Array(3);
    var MaterialDiffuse = new Array(3);
    var MaterialSpecular = new Array(3);
    var MaterialShininess;

    var viewportWidth = currentWidth / 4;
    var viewportHeight = currentHeight / 6;

    var translationMatrix = mat4.create();
    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();

    var zVal = -2.0;

    // Code
    // ***** 1st sphere on 1st column, emerald *****
    // ambient material
    MaterialAmbient[0] = 0.0215; // r
    MaterialAmbient[1] = 0.1745; // g
    MaterialAmbient[2] = 0.0215; // b
    //MaterialAmbient[3] = 1.0;   // a

    // diffuse material
    MaterialDiffuse[0] = 0.07568; // r
    MaterialDiffuse[1] = 0.61424; // g
    MaterialDiffuse[2] = 0.07568; // b
    //MaterialDiffuse[3] = 1.0;    // a

    // specular material
    MaterialSpecular[0] = 0.633;    // r
    MaterialSpecular[1] = 0.727811; // g
    MaterialSpecular[2] = 0.633;    // b
    //MaterialSpecular[3] = 1.0;     // a

    // shininess
    MaterialShininess = 0.6 * 128;

    if (bLight == true)
    {
        gl.uniform3fv(laUniform, lightAmbient);
        gl.uniform3fv(ldUniform, lightDiffuse);
        gl.uniform3fv(lsUniform, lightSpecular);
        gl.uniform4fv(lightPositionUniform, lightPosition);

        gl.uniform3fv(kaUniform, MaterialAmbient);
        gl.uniform3fv(kdUniform, MaterialDiffuse);
        gl.uniform3fv(ksUniform, MaterialSpecular);
        gl.uniform1f(materialShininessUniform, MaterialShininess);
    }

    // geometry
    mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
    //mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    gl.viewport(0, 5 * viewportHeight, viewportWidth, viewportHeight);
    sphere.draw();
    // *******************************************************


	// ***** 2nd sphere on 1st column, jade *****
	// ambient material
	MaterialAmbient[0] = 0.135;  // r
	MaterialAmbient[1] = 0.2225; // g
	MaterialAmbient[2] = 0.1575; // b
	//MaterialAmbient[3] = 1.0f;   // a

	// diffuse material
	MaterialDiffuse[0] = 0.54; // r
	MaterialDiffuse[1] = 0.89; // g
	MaterialDiffuse[2] = 0.63; // b
	//MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.316228; // r
	MaterialSpecular[1] = 0.316228; // g
	MaterialSpecular[2] = 0.316228; // b
	//MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.1 * 128;
	//glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(0, 4 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 3rd sphere on 1st column, obsidian *****
	// ambient material
	MaterialAmbient[0] = 0.05375; // r
	MaterialAmbient[1] = 0.05;    // g
	MaterialAmbient[2] = 0.06625; // b
	//MaterialAmbient[3] = 1.0f;    // a

	// diffuse material
	MaterialDiffuse[0] = 0.18275; // r
	MaterialDiffuse[1] = 0.17;    // g
	MaterialDiffuse[2] = 0.22525; // b
	//MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.332741; // r
	MaterialSpecular[1] = 0.328634; // g
	MaterialSpecular[2] = 0.346435; // b
	//MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.3 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(0, 3 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 4th sphere on 1st column, pearl *****
	// ambient material
	MaterialAmbient[0] = 0.25;    // r
	MaterialAmbient[1] = 0.20725; // g
	MaterialAmbient[2] = 0.20725; // b
	//MaterialAmbient[3] = 1.0f;    // a

	// diffuse material
	MaterialDiffuse[0] = 1.0;   // r
	MaterialDiffuse[1] = 0.829; // g
	MaterialDiffuse[2] = 0.829; // b
	//MaterialDiffuse[3] = 1.0f;  // a

	// specular material
	MaterialSpecular[0] = 0.296648; // r
	MaterialSpecular[1] = 0.296648; // g
	MaterialSpecular[2] = 0.296648; // b
	//MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.088 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(0, 2 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 5th sphere on 1st column, ruby *****
	// ambient material
	MaterialAmbient[0] = 0.1745;  // r
	MaterialAmbient[1] = 0.01175; // g
	MaterialAmbient[2] = 0.01175; // b
	//MaterialAmbient[3] = 1.0f;    // a

	// diffuse material
	MaterialDiffuse[0] = 0.61424; // r
	MaterialDiffuse[1] = 0.04136; // g
	MaterialDiffuse[2] = 0.04136; // b
	//MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.727811; // r
	MaterialSpecular[1] = 0.626959; // g
	MaterialSpecular[2] = 0.626959; // b
	//MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.6 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(0, 1 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 6th sphere on 1st column, turquoise *****
	// ambient material
	MaterialAmbient[0] = 0.1;     // r
	MaterialAmbient[1] = 0.18725; // g
	MaterialAmbient[2] = 0.1745;  // b
	//MaterialAmbient[3] = 1.0f;    // a

	// diffuse material
	MaterialDiffuse[0] = 0.396;   // r
	MaterialDiffuse[1] = 0.74151; // g
	MaterialDiffuse[2] = 0.69102; // b
	//MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.297254; // r
	MaterialSpecular[1] = 0.30829;  // g
	MaterialSpecular[2] = 0.306678; // b
	//MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.1 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(0, 0 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************
	// *******************************************************
	// *******************************************************

	// ***** 1st sphere on 2nd column, brass *****
	// ambient material
	MaterialAmbient[0] = 0.329412; // r
	MaterialAmbient[1] = 0.223529; // g
	MaterialAmbient[2] = 0.027451; // b
	//MaterialAmbient[3] = 1.0f;     // a

	// diffuse material
	MaterialDiffuse[0] = 0.780392; // r
	MaterialDiffuse[1] = 0.568627; // g
	MaterialDiffuse[2] = 0.113725; // b
	//MaterialDiffuse[3] = 1.0f;     // a

	// specular material
	MaterialSpecular[0] = 0.992157; // r
	MaterialSpecular[1] = 0.941176; // g
	MaterialSpecular[2] = 0.807843; // b
	//MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.21794872 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(1 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 2nd sphere on 2nd column, bronze *****
	// ambient material
	MaterialAmbient[0] = 0.2125; // r
	MaterialAmbient[1] = 0.1275; // g
	MaterialAmbient[2] = 0.054;  // b
	//MaterialAmbient[3] = 1.0f;   // a

	// diffuse material
	MaterialDiffuse[0] = 0.714;   // r
	MaterialDiffuse[1] = 0.4284;  // g
	MaterialDiffuse[2] = 0.18144; // b
	//MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.393548; // r
	MaterialSpecular[1] = 0.271906; // g
	MaterialSpecular[2] = 0.166721; // b
	//MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.2 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(1 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 3rd sphere on 2nd column, chrome *****
	// ambient material
	MaterialAmbient[0] = 0.25; // r
	MaterialAmbient[1] = 0.25; // g
	MaterialAmbient[2] = 0.25; // b
	//MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.4;  // r
	MaterialDiffuse[1] = 0.4;  // g
	MaterialDiffuse[2] = 0.4;  // b
	//MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.774597; // r
	MaterialSpecular[1] = 0.774597; // g
	MaterialSpecular[2] = 0.774597; // b
	//MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.6 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(1 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 4th sphere on 2nd column, copper *****
	// ambient material
	MaterialAmbient[0] = 0.19125; // r
	MaterialAmbient[1] = 0.0735;  // g
	MaterialAmbient[2] = 0.0225;  // b
	//MaterialAmbient[3] = 1.0f;    // a

	// diffuse material
	MaterialDiffuse[0] = 0.7038;  // r
	MaterialDiffuse[1] = 0.27048; // g
	MaterialDiffuse[2] = 0.0828;  // b
	//MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.256777; // r
	MaterialSpecular[1] = 0.137622; // g
	MaterialSpecular[2] = 0.086014; // b
	//MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.1 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(1 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 5th sphere on 2nd column, gold *****
	// ambient material
	MaterialAmbient[0] = 0.24725; // r
	MaterialAmbient[1] = 0.1995;  // g
	MaterialAmbient[2] = 0.0745;  // b
	//MaterialAmbient[3] = 1.0f;    // a

	// diffuse material
	MaterialDiffuse[0] = 0.75164; // r
	MaterialDiffuse[1] = 0.60648; // g
	MaterialDiffuse[2] = 0.22648; // b
	//MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.628281; // r
	MaterialSpecular[1] = 0.555802; // g
	MaterialSpecular[2] = 0.366065; // b
	//MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.4 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(1 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 6th sphere on 2nd column, silver *****
	// ambient material
	MaterialAmbient[0] = 0.19225; // r
	MaterialAmbient[1] = 0.19225; // g
	MaterialAmbient[2] = 0.19225; // b
	//MaterialAmbient[3] = 1.0f;    // a

	// diffuse material
	MaterialDiffuse[0] = 0.50754; // r
	MaterialDiffuse[1] = 0.50754; // g
	MaterialDiffuse[2] = 0.50754; // b
	//MaterialDiffuse[3] = 1.0f;    // a

	// specular material
	MaterialSpecular[0] = 0.508273; // r
	MaterialSpecular[1] = 0.508273; // g
	MaterialSpecular[2] = 0.508273; // b
	//MaterialSpecular[3] = 1.0f;     // a

	// shininess
	MaterialShininess = 0.4 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(1 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************
	// *******************************************************
	// *******************************************************

	// ***** 1st sphere on 3rd column, black *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	//MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.01; // r
	MaterialDiffuse[1] = 0.01; // g
	MaterialDiffuse[2] = 0.01; // b
	//MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.50; // r
	MaterialSpecular[1] = 0.50; // g
	MaterialSpecular[2] = 0.50; // b
	//MaterialSpecular[3] = 1.0f; // a

	// shininess
	MaterialShininess = 0.25 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(2 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 2nd sphere on 3rd column, cyan *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.1;  // g
	MaterialAmbient[2] = 0.06; // b
	//MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.0;        // r
	MaterialDiffuse[1] = 0.50980392; // g
	MaterialDiffuse[2] = 0.50980392; // b
	//MaterialDiffuse[3] = 1.0f;       // a

	// specular material
	MaterialSpecular[0] = 0.50196078; // r
	MaterialSpecular[1] = 0.50196078; // g
	MaterialSpecular[2] = 0.50196078; // b
	//MaterialSpecular[3] = 1.0f;       // a

	// shininess
	MaterialShininess = 0.25 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(2 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 3rd sphere on 2nd column, green *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	//MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.1;  // r
	MaterialDiffuse[1] = 0.35; // g
	MaterialDiffuse[2] = 0.1;  // b
	//MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.45; // r
	MaterialSpecular[1] = 0.55; // g
	MaterialSpecular[2] = 0.45; // b
	//MaterialSpecular[3] = 1.0f; // a

	// shininess
	MaterialShininess = 0.25 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(2 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 4th sphere on 3rd column, red *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	//MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.0;  // g
	MaterialDiffuse[2] = 0.0;  // b
	//MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.6;  // g
	MaterialSpecular[2] = 0.6;  // b
	//MaterialSpecular[3] = 1.0f; // a

	// shininess
	MaterialShininess = 0.25 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(2 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 5th sphere on 3rd column, white *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	//MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.55; // r
	MaterialDiffuse[1] = 0.55; // g
	MaterialDiffuse[2] = 0.55; // b
	//MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.70; // r
	MaterialSpecular[1] = 0.70; // g
	MaterialSpecular[2] = 0.70; // b
	//MaterialSpecular[3] = 1.0f; // a

	// shininess
	MaterialShininess = 0.25 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(2 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 6th sphere on 3rd column, yellow plastic *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	//MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.0;  // b
	//MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.60; // r
	MaterialSpecular[1] = 0.60; // g
	MaterialSpecular[2] = 0.50; // b
	//MaterialSpecular[3] = 1.0f; // a

	// shininess
	MaterialShininess = 0.25 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(2 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************
	// *******************************************************
	// *******************************************************

	// ***** 1st sphere on 4th column, black *****
	// ambient material
	MaterialAmbient[0] = 0.02; // r
	MaterialAmbient[1] = 0.02; // g
	MaterialAmbient[2] = 0.02; // b
	//MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.01; // r
	MaterialDiffuse[1] = 0.01; // g
	MaterialDiffuse[2] = 0.01; // b
	//MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.4;  // r
	MaterialSpecular[1] = 0.4;  // g
	MaterialSpecular[2] = 0.4;  // b
	//MaterialSpecular[3] = 1.0f; // a

	// shininess
	MaterialShininess = 0.078125 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(3 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 2nd sphere on 4th column, cyan *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.05; // b
	//MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.4;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.5;  // b
	//MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.04; // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.7;  // b
	//MaterialSpecular[3] = 1.0f; // a

	// shininess
	MaterialShininess = 0.078125 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(3 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 3rd sphere on 4th column, green *****
	// ambient material
	MaterialAmbient[0] = 0.0;  // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.0;  // b
	//MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.4;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.4;  // b
	//MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.04; // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.04; // b
	//MaterialSpecular[3] = 1.0f; // a

	// shininess
	MaterialShininess = 0.078125 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(3 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 4th sphere on 4th column, red *****
	// ambient material
	MaterialAmbient[0] = 0.05; // r
	MaterialAmbient[1] = 0.0;  // g
	MaterialAmbient[2] = 0.0;  // b
	//MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.4;  // g
	MaterialDiffuse[2] = 0.4;  // b
	//MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.04; // g
	MaterialSpecular[2] = 0.04; // b
	//MaterialSpecular[3] = 1.0f; // a

	// shininess
	MaterialShininess = 0.078125 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(3 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 5th sphere on 4th column, white *****
	// ambient material
	MaterialAmbient[0] = 0.05; // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.05; // b
	//MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.5;  // b
	//MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.7;  // b
	//MaterialSpecular[3] = 1.0f; // a

	// shininess
	MaterialShininess = 0.078125 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(3 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************

	// ***** 6th sphere on 4th column, yellow rubber *****
	// ambient material
	MaterialAmbient[0] = 0.05; // r
	MaterialAmbient[1] = 0.05; // g
	MaterialAmbient[2] = 0.0;  // b
	//MaterialAmbient[3] = 1.0f; // a

	// diffuse material
	MaterialDiffuse[0] = 0.5;  // r
	MaterialDiffuse[1] = 0.5;  // g
	MaterialDiffuse[2] = 0.4;  // b
	//MaterialDiffuse[3] = 1.0f; // a

	// specular material
	MaterialSpecular[0] = 0.7;  // r
	MaterialSpecular[1] = 0.7;  // g
	MaterialSpecular[2] = 0.04; // b
	//MaterialSpecular[3] = 1.0f; // a

	// shininess
	MaterialShininess = 0.078125 * 128;

	if (bLight == true) {
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, MaterialAmbient);
		gl.uniform3fv(kdUniform, MaterialDiffuse);
		gl.uniform3fv(ksUniform, MaterialSpecular);
		gl.uniform1f(materialShininessUniform, MaterialShininess);
	}

	// geometry
	mat4.translate(modelMatrix, translationMatrix, [0.0, 0.0, zVal]);
	//mat4.multiply(modelMatrix, modelMatrix, translationMatrix);
	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.viewport(3 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
	sphere.draw();
    // *******************************************************
	// *******************************************************
	// *******************************************************
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

        case 88:
            KeyPressed = 1;
            AngleForXRotation = 0.0;		// Reset
            AngleForYRotation = 0.0;
            AngleForZRotation = 0.0;
            break;

        case 89:
            KeyPressed = 2;
            AngleForXRotation += 0.0;
            AngleForYRotation = 0.0;
            AngleForZRotation = 0.0;
            break;

        case 90:
            KeyPressed = 3;
            AngleForXRotation += 0.0;
            AngleForYRotation = 0.0;
            AngleForZRotation = 0.0;
            break;

        default:
            KeyPressed = 0;
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

    if (sphere) {
        sphere.deallocate();
        sphere = null;
    }
}
