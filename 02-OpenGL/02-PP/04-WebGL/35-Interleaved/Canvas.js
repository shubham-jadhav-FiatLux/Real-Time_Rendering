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
var vbo_cube;

var angleCube = 0.0;

// Texture
var texture_marble;
var textureSamplerUniform;

//  Light
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

var lightAmbient = [ 0.1, 0.1, 0.1];
var lightDiffuse = [ 1.0, 1.0, 1.0];
var lightSpecular = [ 1.0, 1.0, 1.0];
var lightPosition = [ 0.0, 0.0, 100.0, 1.0 ];

var materialAmbient = [ 0.0, 0.0, 0.0];
var materialDiffuse = [ 0.5, 0.2, 0.7];
var materialSpecular = [ 0.7, 0.7, 0.7];
var materialShininess = [50.0];

var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

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
        "in vec3 a_normal;" +
        "in vec2 a_texcoord;" +
        "uniform mat4 u_modelMatrix;" +
        "uniform mat4 u_viewMatrix;" +
        "uniform mat4 u_projectionMatrix;" +
        "uniform vec4 u_lightPosition;" +
        "uniform int u_lightingEnabled;" +
        "out vec4 a_color_out;" +
        "out vec3 transformedNormals;" +
        "out vec3 lightDirection;" +
        "out vec3 viewerVector;" +
        "out vec2 a_texcoord_out;" +
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
        "a_color_out = a_color;" +
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
        "in vec4 a_color_out;" +
        "in vec3 transformedNormals;" +
        "in vec3 lightDirection;" +
        "in vec3 viewerVector;" +
        "in vec2 a_texcoord_out;" +
        "uniform vec3 u_la;" +
        "uniform vec3 u_ld;" +
        "uniform vec3 u_ls;" +
        "uniform vec3 u_ka;" +
        "uniform vec3 u_kd;" +
        "uniform vec3 u_ks;" +
        "uniform float u_materialShininess;" +
        "uniform sampler2D u_textureSampler;" +
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
        "FragColor = vec4(vec3(texture(u_textureSampler, a_texcoord_out)) * phong_ads_light * vec3(a_color_out), 1.0);" +
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
    gl.bindAttribLocation(shaderProgramObject, webglMacros.SBJ_ATTRIBUTE_TEXTURE, "a_texcoord");
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
    textureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "u_textureSampler");

    // Declaration Of Vertex Data Arrays

    var cubePCNT = new Float32Array([
        // positions                 //colors               //normals                   //texture

        // Top face               // Top face                 // Top face              // Top face
         1.0, 1.0, -1.0,        0.0, 0.0, 1.0,            0.0, 1.0, 0.0,          1.0, 1.0,
        -1.0, 1.0, -1.0,        0.0, 0.0, 1.0,            0.0, 1.0, 0.0,           0.0, 1.0,
        -1.0, 1.0, 1.0,         0.0, 0.0, 1.0,            0.0, 1.0, 0.0,           0.0, 0.0,
         1.0, 1.0, 1.0,         0.0, 0.0, 1.0,            0.0, 1.0, 0.0,           1.0, 0.0,

        // Bottom face            // Bottom face             // Bottom face            // Bottom face
        1.0, -1.0, -1.0,        0.0, 1.0, 0.0,          0.0, -1.0, 0.0,          1.0, 1.0,
        -1.0, -1.0, -1.0,       0.0, 1.0, 0.0,          0.0, -1.0, 0.0,          0.0, 1.0,
        -1.0, -1.0, 1.0,        0.0, 1.0, 0.0,          0.0, -1.0, 0.0,          0.0, 0.0,
        1.0, -1.0, 1.0,         0.0, 1.0, 0.0,          0.0, -1.0, 0.0,          1.0, 0.0,

        // Front face             // Front face              // Front face             // Front face
        1.0, 1.0, 1.0,          1.0, 0.0, 0.0,           0.0, 0.0, 1.0,           1.0, 1.0,
        -1.0, 1.0, 1.0,         1.0, 0.0, 0.0,           0.0, 0.0, 1.0,           0.0, 1.0,
        -1.0, -1.0, 1.0,        1.0, 0.0, 0.0,           0.0, 0.0, 1.0,           0.0, 0.0,
         1.0, -1.0, 1.0,        1.0, 0.0, 0.0,           0.0, 0.0, 1.0,           1.0, 0.0,

         // Back face              // Back face              // Back face              // Back face
         1.0, 1.0, -1.0,        0.0, 1.0, 1.0,           0.0, 0.0, -1.0,          1.0, 1.0,
        -1.0, 1.0, -1.0,        0.0, 1.0, 1.0,           0.0, 0.0, -1.0,          0.0, 1.0,
        -1.0, -1.0, -1.0,       0.0, 1.0, 1.0,           0.0, 0.0, -1.0,          0.0, 0.0,
        1.0, -1.0, -1.0,        0.0, 1.0, 1.0,           0.0, 0.0, -1.0,          1.0, 0.0,

         // Right face              // Right face          	// Right face              // Right face
         1.0, 1.0, -1.0,        1.0, 0.0, 1.0,           1.0, 0.0, 0.0,           1.0, 1.0,
        1.0, 1.0, 1.0,          1.0, 0.0, 1.0,           1.0, 0.0, 0.0,           0.0, 1.0,
         1.0, -1.0, 1.0,        1.0, 0.0, 1.0,           1.0, 0.0, 0.0,           0.0, 0.0,
         1.0, -1.0, -1.0,       1.0, 0.0, 1.0,           1.0, 0.0, 0.0,           1.0, 0.0,

         // Left face                // Left face           // Left face                // Left face
         -1.0, 1.0, 1.0,        1.0, 1.0, 0.0,           -1.0, 0.0, 0.0,          1.0, 1.0,
         -1.0, 1.0, -1.0,       1.0, 1.0, 0.0,           -1.0, 0.0, 0.0,          0.0, 1.0,
         -1.0, -1.0, -1.0,      1.0, 1.0, 0.0,           -1.0, 0.0, 0.0,          0.0, 0.0,
         -1.0, -1.0, 1.0,       1.0, 1.0, 0.0,           -1.0, 0.0, 0.0,          1.0, 0.0,
	]);

    // vao cube
    vao_cube = gl.createVertexArray();
    gl.bindVertexArray(vao_cube);

        // vbo position
        vbo_cube = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube);
        gl.bufferData(gl.ARRAY_BUFFER, cubePCNT, gl.STATIC_DRAW);

            gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 11 * Float32Array.BYTES_PER_ELEMENT , (0));
            gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);

            gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 11 * Float32Array.BYTES_PER_ELEMENT, (3 * Float32Array.BYTES_PER_ELEMENT));
            gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_COLOR);

            gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false, 11 * Float32Array.BYTES_PER_ELEMENT, (6 * Float32Array.BYTES_PER_ELEMENT));
            gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_NORMAL);

            gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_TEXTURE, 2, gl.FLOAT, false, 11 * Float32Array.BYTES_PER_ELEMENT, (9 * Float32Array.BYTES_PER_ELEMENT));
            gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_TEXTURE);

        gl.bindBuffer(gl.ARRAY_BUFFER, null);
    
    gl.bindVertexArray(null);

    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // Clear The Screen By Black Color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    // Texture
    texture_marble = gl.createTexture();
    texture_marble.image = new Image();
    texture_marble.image.src = "Marble.png";
    texture_marble.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_marble);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_marble.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

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

        gl.uniform3fv(laUniform, lightAmbient);
        gl.uniform3fv(ldUniform, lightDiffuse);
        gl.uniform3fv(lsUniform, lightSpecular);
        gl.uniform4fv(lightPositionUniform, lightPosition);

        gl.uniform3fv(kaUniform, materialAmbient);
        gl.uniform3fv(kdUniform, materialDiffuse);
        gl.uniform3fv(ksUniform, materialSpecular);
        gl.uniform1f(materialShininessUniform, materialShininess);
    }
    else {
        gl.uniform1i(lightingEnabledUniform, 0);
    }

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, texture_marble);
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
    angleCube = angleCube + 0.01;
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
    if (vbo_cube) {
        gl.deleteBuffer(vbo_cube);
        vbo_cube = null;
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
