// Global Variable Declarations
var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

var gbToggleSquare = false;
var gbToggleCircle = false;
var gbToggleTriangle = false;
var gbDoubleClick = false;

var circleVerticesSize;
var graphPaperVerticesSize;

var shaderProgramObject;

const webglMacros =
{
    SBJ_ATTRIBUTE_POSITION: 0,
    SBJ_ATTRIBUTE_COLOR: 1,
    SBJ_ATTRIBUTE_NORMAL: 2,
    SBJ_ATTRIBUTE_TEXTURE: 3
};

var vao_triangle;
var vbo_triangle;

var vao_square;
var vbo_square;

var vao_circle;
var vbo_circle;

var vao_graph;
var vbo_graph;

var vao_graphAxis;
var vbo_graphAxis;

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
        "uniform mat4 u_mvpMatrix;" +
        "out vec4 a_color_out;" +
        "void main(void)" +
        "{" +
        "gl_Position = u_mvpMatrix * a_position;" +
        "a_color_out = a_color;" +
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
        "out vec4 FragColor;" +
        "void main(void)" +
        "{" +
        "FragColor = a_color_out;" +
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


    var triangleVertices = new Float32Array([
                                                0.0, 0.5,
												0.5 * Math.cos(225 / 180.0 * Math.PI), 0.5 * Math.sin(225 / 180.0 * Math.PI),

												0.5 * Math.cos(225 / 180.0 * Math.PI), 0.5 * Math.sin(225 / 180.0 * Math.PI),
												0.5 * Math.cos(315 / 180.0 * Math.PI), 0.5 * Math.sin(315 / 180.0 * Math.PI),

												0.5 * Math.cos(315 / 180.0 * Math.PI), 0.5 * Math.sin(315 / 180.0 * Math.PI),
                                                0.0, 0.5,
    ]);

    var triangleColor = new Float32Array([
        1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,

		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,

		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
    ]);

    var squareVertices = new Float32Array([
        0.5, 0.5,
        -0.5, 0.5,

        -0.5, 0.5,
        -0.5, -0.5,

        -0.5, -0.5,
        0.5, -0.5,

        0.5, -0.5,
        0.5, 0.5,
    ]);

    var squareColor = new Float32Array([
        1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,

		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,

		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,

		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
    ]);

    var graphPaperVertices = new Float32Array([
        -1.000000, 1.000000,
        1.000000, 1.000000,
        -1.000000, 0.950000,
        1.000000, 0.950000,
        -1.000000, 0.900000,
        1.000000, 0.900000,
        -1.000000, 0.850000,
        1.000000, 0.850000,
        -1.000000, 0.800000,
        1.000000, 0.800000,
        -1.000000, 0.750000,
        1.000000, 0.750000,
        -1.000000, 0.700000,
        1.000000, 0.700000,
        -1.000000, 0.650000,
        1.000000, 0.650000,
        -1.000000, 0.600000,
        1.000000, 0.600000,
        -1.000000, 0.550000,
        1.000000, 0.550000,
        -1.000000, 0.500000,
        1.000000, 0.500000,
        -1.000000, 0.450000,
        1.000000, 0.450000,
        -1.000000, 0.400000,
        1.000000, 0.400000,
        -1.000000, 0.350000,
        1.000000, 0.350000,
        -1.000000, 0.300000,
        1.000000, 0.300000,
        -1.000000, 0.250000,
        1.000000, 0.250000,
        -1.000000, 0.200000,
        1.000000, 0.200000,
        -1.000000, 0.150000,
        1.000000, 0.150000,
        -1.000000, 0.100000,
        1.000000, 0.100000,
        -1.000000, 0.050000,
        1.000000, 0.050000,


        -1.000000, -1.000000,
        1.000000, -1.000000,
        -1.000000, -0.950000,
        1.000000, -0.950000,
        -1.000000, -0.900000,
        1.000000, -0.900000,
        -1.000000, -0.850000,
        1.000000, -0.850000,
        -1.000000, -0.800000,
        1.000000, -0.800000,
        -1.000000, -0.750000,
        1.000000, -0.750000,
        -1.000000, -0.700000,
        1.000000, -0.700000,
        -1.000000, -0.650000,
        1.000000, -0.650000,
        -1.000000, -0.600000,
        1.000000, -0.600000,
        -1.000000, -0.550000,
        1.000000, -0.550000,
        -1.000000, -0.500000,
        1.000000, -0.500000,
        -1.000000, -0.450000,
        1.000000, -0.450000,
        -1.000000, -0.400000,
        1.000000, -0.400000,
        -1.000000, -0.350000,
        1.000000, -0.350000,
        -1.000000, -0.300000,
        1.000000, -0.300000,
        -1.000000, -0.250000,
        1.000000, -0.250000,
        -1.000000, -0.200000,
        1.000000, -0.200000,
        -1.000000, -0.150000,
        1.000000, -0.150000,
        -1.000000, -0.100000,
        1.000000, -0.100000,
        -1.000000, -0.050000,
        1.000000, -0.050000,


        -1.000000, 1.000000,
        -1.000000, -1.000000,
        -0.950000, 1.000000,
        -0.950000, -1.000000,
        -0.900000, 1.000000,
        -0.900000, -1.000000,
        -0.850000, 1.000000,
        -0.850000, -1.000000,
        -0.800000, 1.000000,
        -0.800000, -1.000000,
        -0.750000, 1.000000,
        -0.750000, -1.000000,
        -0.700000, 1.000000,
        -0.700000, -1.000000,
        -0.650000, 1.000000,
        -0.650000, -1.000000,
        -0.600000, 1.000000,
        -0.600000, -1.000000,
        -0.550000, 1.000000,
        -0.550000, -1.000000,
        -0.500000, 1.000000,
        -0.500000, -1.000000,
        -0.450000, 1.000000,
        -0.450000, -1.000000,
        -0.400000, 1.000000,
        -0.400000, -1.000000,
        -0.350000, 1.000000,
        -0.350000, -1.000000,
        -0.300000, 1.000000,
        -0.300000, -1.000000,
        -0.250000, 1.000000,
        -0.250000, -1.000000,
        -0.200000, 1.000000,
        -0.200000, -1.000000,
        -0.150000, 1.000000,
        -0.150000, -1.000000,
        -0.100000, 1.000000,
        -0.100000, -1.000000,
        -0.050000, 1.000000,
        -0.050000, -1.000000,


        1.000000, 1.000000,
        1.000000, -1.000000,
        0.950000, 1.000000,
        0.950000, -1.000000,
        0.900000, 1.000000,
        0.900000, -1.000000,
        0.850000, 1.000000,
        0.850000, -1.000000,
        0.800000, 1.000000,
        0.800000, -1.000000,
        0.750000, 1.000000,
        0.750000, -1.000000,
        0.700000, 1.000000,
        0.700000, -1.000000,
        0.650000, 1.000000,
        0.650000, -1.000000,
        0.600000, 1.000000,
        0.600000, -1.000000,
        0.550000, 1.000000,
        0.550000, -1.000000,
        0.500000, 1.000000,
        0.500000, -1.000000,
        0.450000, 1.000000,
        0.450000, -1.000000,
        0.400000, 1.000000,
        0.400000, -1.000000,
        0.350000, 1.000000,
        0.350000, -1.000000,
        0.300000, 1.000000,
        0.300000, -1.000000,
        0.250000, 1.000000,
        0.250000, -1.000000,
        0.200000, 1.000000,
        0.200000, -1.000000,
        0.150000, 1.000000,
        0.150000, -1.000000,
        0.100000, 1.000000,
        0.100000, -1.000000,
        0.050000, 1.000000,
        0.050000, -1.000000,
    ]);
    graphPaperVerticesSize = graphPaperVertices.length;

    var graphPaperColor = new Float32Array([
        0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,


		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,


		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,


		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
    ]);

    var graphPaperAxisVertices = new Float32Array([
														-1.0, 0.0,
														1.0, 0.0,

														0.0, 1.0,
														0.0, -1.0
													]);

    var graphPaperAxisColor = new Float32Array([
													1.0, 0.0, 0.0,
													1.0, 0.0, 0.0,

													0.0, 1.0, 0.0,
													0.0, 1.0, 0.0
												]);

    var circleVertices = new Float32Array([
		0.500000, 0.000000,
		0.499981, 0.004363,
		0.499924, 0.008726,
		0.499829, 0.013088,
		0.499695, 0.017450,
		0.499524, 0.021810,
		0.499315, 0.026168,
		0.499067, 0.030524,
		0.498782, 0.034878,
		0.498459, 0.039230,
		0.498097, 0.043578,
		0.497698, 0.047923,
		0.497261, 0.052264,
		0.496786, 0.056602,
		0.496273, 0.060935,
		0.495722, 0.065263,
		0.495134, 0.069587,
		0.494508, 0.073905,
		0.493844, 0.078217,
		0.493143, 0.082524,
		0.492404, 0.086824,
		0.491627, 0.091118,
		0.490814, 0.095404,
		0.489962, 0.099684,
		0.489074, 0.103956,
		0.488148, 0.108220,
		0.487185, 0.112476,
		0.486185, 0.116723,
		0.485148, 0.120961,
		0.484074, 0.125190,
		0.482963, 0.129410,
		0.481815, 0.133619,
		0.480631, 0.137819,
		0.479410, 0.142008,
		0.478152, 0.146186,
		0.476858, 0.150353,
		0.475528, 0.154508,
		0.474162, 0.158652,
		0.472759, 0.162784,
		0.471321, 0.166903,
		0.469846, 0.171010,
		0.468336, 0.175104,
		0.466790, 0.179184,
		0.465209, 0.183251,
		0.463592, 0.187303,
		0.461940, 0.191342,
		0.460252, 0.195366,
		0.458530, 0.199375,
		0.456773, 0.203368,
		0.454981, 0.207347,
		0.453154, 0.211309,
		0.451293, 0.215256,
		0.449397, 0.219186,
		0.447467, 0.223099,
		0.445503, 0.226995,
		0.443505, 0.230874,
		0.441474, 0.234736,
		0.439409, 0.238579,
		0.437310, 0.242405,
		0.435178, 0.246212,
		0.433013, 0.250000,
		0.430815, 0.253769,
		0.428584, 0.257519,
		0.426320, 0.261249,
		0.424024, 0.264960,
		0.421696, 0.268650,
		0.419335, 0.272320,
		0.416943, 0.275968,
		0.414519, 0.279596,
		0.412063, 0.283203,
		0.409576, 0.286788,
		0.407058, 0.290351,
		0.404508, 0.293893,
		0.401928, 0.297411,
		0.399318, 0.300908,
		0.396677, 0.304381,
		0.394005, 0.307831,
		0.391304, 0.311257,
		0.388573, 0.314660,
		0.385812, 0.318039,
		0.383022, 0.321394,
		0.380203, 0.324724,
		0.377355, 0.328030,
		0.374478, 0.331310,
		0.371572, 0.334565,
		0.368639, 0.337795,
		0.365677, 0.340999,
		0.362687, 0.344177,
		0.359670, 0.347329,
		0.356625, 0.350455,
		0.353553, 0.353553,
		0.350455, 0.356625,
		0.347329, 0.359670,
		0.344177, 0.362687,
		0.340999, 0.365677,
		0.337795, 0.368639,
		0.334565, 0.371572,
		0.331310, 0.374478,
		0.328030, 0.377355,
		0.324724, 0.380203,
		0.321394, 0.383022,
		0.318039, 0.385812,
		0.314660, 0.388573,
		0.311257, 0.391304,
		0.307831, 0.394005,
		0.304381, 0.396677,
		0.300908, 0.399318,
		0.297411, 0.401928,
		0.293893, 0.404509,
		0.290351, 0.407058,
		0.286788, 0.409576,
		0.283203, 0.412063,
		0.279596, 0.414519,
		0.275969, 0.416943,
		0.272320, 0.419335,
		0.268650, 0.421696,
		0.264960, 0.424024,
		0.261249, 0.426320,
		0.257519, 0.428584,
		0.253769, 0.430815,
		0.250000, 0.433013,
		0.246212, 0.435178,
		0.242405, 0.437310,
		0.238579, 0.439409,
		0.234736, 0.441474,
		0.230874, 0.443505,
		0.226995, 0.445503,
		0.223099, 0.447467,
		0.219186, 0.449397,
		0.215256, 0.451293,
		0.211309, 0.453154,
		0.207347, 0.454981,
		0.203368, 0.456773,
		0.199375, 0.458530,
		0.195366, 0.460252,
		0.191342, 0.461940,
		0.187303, 0.463592,
		0.183251, 0.465209,
		0.179184, 0.466790,
		0.175104, 0.468336,
		0.171010, 0.469846,
		0.166903, 0.471321,
		0.162784, 0.472759,
		0.158652, 0.474162,
		0.154508, 0.475528,
		0.150353, 0.476858,
		0.146186, 0.478152,
		0.142008, 0.479410,
		0.137819, 0.480631,
		0.133619, 0.481815,
		0.129410, 0.482963,
		0.125190, 0.484074,
		0.120961, 0.485148,
		0.116723, 0.486185,
		0.112476, 0.487185,
		0.108220, 0.488148,
		0.103956, 0.489074,
		0.099684, 0.489962,
		0.095405, 0.490814,
		0.091118, 0.491627,
		0.086824, 0.492404,
		0.082524, 0.493143,
		0.078217, 0.493844,
		0.073905, 0.494508,
		0.069587, 0.495134,
		0.065263, 0.495722,
		0.060935, 0.496273,
		0.056602, 0.496786,
		0.052264, 0.497261,
		0.047923, 0.497698,
		0.043578, 0.498097,
		0.039230, 0.498459,
		0.034878, 0.498782,
		0.030524, 0.499067,
		0.026168, 0.499315,
		0.021810, 0.499524,
		0.017450, 0.499695,
		0.013088, 0.499829,
		0.008726, 0.499924,
		0.004363, 0.499981,
		0.000000, 0.500000,
		- 0.004363, 0.49998,
		- 0.008726, 0.499924,
		- 0.013088, 0.499829,
		- 0.017450, 0.499695,
		- 0.021810, 0.499524,
		- 0.026168, 0.499315,
		- 0.030524, 0.499067,
		- 0.034878, 0.498782,
		- 0.039230, 0.498459,
		- 0.043578, 0.498097,
		- 0.047923, 0.497698,
		- 0.052264, 0.497261,
		- 0.056602, 0.496786,
		- 0.060935, 0.496273,
		- 0.065263, 0.495722,
		- 0.069587, 0.495134,
		- 0.073905, 0.494508,
		- 0.078217, 0.493844,
		- 0.082524, 0.493143,
		- 0.086824, 0.492404,
		- 0.091118, 0.491627,
		- 0.095404, 0.490814,
		- 0.099684, 0.489962,
		- 0.103956, 0.489074,
		- 0.108220, 0.488148,
		- 0.112476, 0.487185,
		- 0.116723, 0.486185,
		- 0.120961, 0.485148,
		- 0.125190, 0.484074,
		- 0.129409, 0.482963,
		- 0.133619, 0.481815,
		- 0.137819, 0.480631,
		- 0.142008, 0.479410,
		- 0.146186, 0.478152,
		- 0.150353, 0.476858,
		- 0.154509, 0.475528,
		- 0.158652, 0.474162,
		- 0.162784, 0.472759,
		- 0.166903, 0.471321,
		- 0.171010, 0.469846,
		- 0.175104, 0.468336,
		- 0.179184, 0.466790,
		- 0.183251, 0.465209,
		- 0.187303, 0.463592,
		- 0.191342, 0.461940,
		- 0.195366, 0.460252,
		- 0.199375, 0.458530,
		- 0.203368, 0.456773,
		- 0.207347, 0.454981,
		- 0.211309, 0.453154,
		- 0.215256, 0.451293,
		- 0.219186, 0.449397,
		- 0.223099, 0.447467,
		- 0.226995, 0.445503,
		- 0.230874, 0.443505,
		- 0.234736, 0.441474,
		- 0.238579, 0.439409,
		- 0.242405, 0.437310,
		- 0.246212, 0.435178,
		- 0.250000, 0.433013,
		- 0.253769, 0.430815,
		- 0.257519, 0.428584,
		- 0.261249, 0.426320,
		- 0.264960, 0.424024,
		- 0.268650, 0.421696,
		- 0.272320, 0.419335,
		- 0.275968, 0.416943,
		- 0.279596, 0.414519,
		- 0.283203, 0.412063,
		- 0.286788, 0.409576,
		- 0.290351, 0.407058,
		- 0.293893, 0.404509,
		- 0.297411, 0.401928,
		- 0.300908, 0.399318,
		- 0.304381, 0.396677,
		- 0.307831, 0.394005,
		- 0.311257, 0.391304,
		- 0.314660, 0.388573,
		- 0.318039, 0.385812,
		- 0.321394, 0.383022,
		- 0.324724, 0.380203,
		- 0.328030, 0.377355,
		- 0.331310, 0.374478,
		- 0.334565, 0.371572,
		- 0.337795, 0.368639,
		- 0.340999, 0.365677,
		- 0.344177, 0.362687,
		- 0.347329, 0.359670,
		- 0.350455, 0.356625,
		- 0.353553, 0.353553,
		- 0.356625, 0.350455,
		- 0.359670, 0.347329,
		- 0.362687, 0.344177,
		- 0.365677, 0.340999,
		- 0.368639, 0.337795,
		- 0.371572, 0.334565,
		- 0.374478, 0.331310,
		- 0.377355, 0.328030,
		- 0.380203, 0.324724,
		- 0.383022, 0.321394,
		- 0.385812, 0.318039,
		- 0.388573, 0.314660,
		- 0.391304, 0.311257,
		- 0.394005, 0.307831,
		- 0.396677, 0.304381,
		- 0.399318, 0.300908,
		- 0.401928, 0.297411,
		- 0.404509, 0.293893,
		- 0.407058, 0.290351,
		- 0.409576, 0.286788,
		- 0.412063, 0.283203,
		- 0.414519, 0.279596,
		- 0.416943, 0.275968,
		- 0.419335, 0.272320,
		- 0.421696, 0.268650,
		- 0.424024, 0.264960,
		- 0.426320, 0.261249,
		- 0.428584, 0.257519,
		- 0.430815, 0.253769,
		- 0.433013, 0.250000,
		- 0.435178, 0.246212,
		- 0.437310, 0.242405,
		- 0.439409, 0.238579,
		- 0.441474, 0.234736,
		- 0.443505, 0.230874,
		- 0.445503, 0.226995,
		- 0.447467, 0.223099,
		- 0.449397, 0.219186,
		- 0.451293, 0.215256,
		- 0.453154, 0.211309,
		- 0.454981, 0.207347,
		- 0.456773, 0.203368,
		- 0.458530, 0.199375,
		- 0.460252, 0.195366,
		- 0.461940, 0.191342,
		- 0.463592, 0.187303,
		- 0.465209, 0.183251,
		- 0.466790, 0.179184,
		- 0.468336, 0.175104,
		- 0.469846, 0.171010,
		- 0.471321, 0.166903,
		- 0.472759, 0.162784,
		- 0.474162, 0.158652,
		- 0.475528, 0.154509,
		- 0.476858, 0.150353,
		- 0.478152, 0.146186,
		- 0.479410, 0.142008,
		- 0.480631, 0.137819,
		- 0.481815, 0.133619,
		- 0.482963, 0.129409,
		- 0.484074, 0.125190,
		- 0.485148, 0.120961,
		- 0.486185, 0.116723,
		- 0.487185, 0.112476,
		- 0.488148, 0.108220,
		- 0.489074, 0.103956,
		- 0.489962, 0.099684,
		- 0.490814, 0.095404,
		- 0.491627, 0.091118,
		- 0.492404, 0.086824,
		- 0.493143, 0.082524,
		- 0.493844, 0.078217,
		- 0.494508, 0.073905,
		- 0.495134, 0.069587,
		- 0.495722, 0.065263,
		- 0.496273, 0.060935,
		- 0.496786, 0.05660,
		- 0.497261, 0.052264,
		- 0.497698, 0.047923,
		- 0.498097, 0.043578,
		- 0.498459, 0.039230,
		- 0.498782, 0.034878,
		- 0.499067, 0.030524,
		- 0.499315, 0.026168,
		- 0.499524, 0.021810,
		- 0.499695, 0.017450,
		- 0.499829, 0.013088,
		- 0.499924, 0.008726,
		- 0.499981, 0.004363,
		- 0.500000, 0.000000,
		- 0.499981, -0.004363,
		- 0.499924, -0.008726,
		- 0.499829, -0.013088,
		- 0.499695, -0.017450,
		- 0.499524, -0.021810,
		- 0.499315, -0.026168,
		- 0.499067, -0.030524,
		- 0.498782, -0.034878,
		- 0.498459, -0.039230,
		- 0.498097, -0.043578,
		- 0.497698, -0.047923,
		- 0.497261, -0.052264,
		- 0.496786, -0.056602,
		- 0.496273, -0.060935,
		- 0.495722, -0.065263,
		- 0.495134, -0.069587,
		- 0.494508, -0.073905,
		- 0.493844, -0.078217,
		- 0.493143, -0.082524,
		- 0.492404, -0.086824,
		- 0.491627, -0.091118,
		- 0.490814, -0.095404,
		- 0.489962, -0.099684,
		- 0.489074, -0.103956,
		- 0.488148, -0.108220,
		- 0.487185, -0.112476,
		- 0.486185, -0.116723,
		- 0.485148, -0.120961,
		- 0.484074, -0.125190,
		- 0.482963, -0.129410,
		- 0.481815, -0.133619,
		- 0.480631, -0.137819,
		- 0.479410, -0.142008,
		- 0.478152, -0.146186,
		- 0.476858, -0.150353,
		- 0.475528, -0.154509,
		- 0.474162, -0.158652,
		- 0.472759, -0.162784,
		- 0.471321, -0.166903,
		- 0.469846, -0.171010,
		- 0.468336, -0.175104,
		- 0.466790, -0.179184,
		- 0.465209, -0.183251,
		- 0.463592, -0.187303,
		- 0.461940, -0.191342,
		- 0.460252, -0.195366,
		- 0.458530, -0.199374,
		- 0.456773, -0.203368,
		- 0.454981, -0.207347,
		- 0.453154, -0.211309,
		- 0.451293, -0.215256,
		- 0.449397, -0.219186,
		- 0.447467, -0.223099,
		- 0.445503, -0.226995,
		- 0.443505, -0.230874,
		- 0.441474, -0.234736,
		- 0.439409, -0.238579,
		- 0.437310, -0.242405,
		- 0.435178, -0.246212,
		- 0.433013, -0.250000,
		- 0.430815, -0.253769,
		- 0.428584, -0.257519,
		- 0.426320, -0.261249,
		- 0.424024, -0.264960,
		- 0.421696, -0.268650,
		- 0.419335, -0.272319,
		- 0.416943, -0.275968,
		- 0.414519, -0.279596,
		- 0.412063, -0.283203,
		- 0.409576, -0.286788,
		- 0.407058, -0.290351,
		- 0.404508, -0.293893,
		- 0.401928, -0.297411,
		- 0.399318, -0.300908,
		- 0.396677, -0.304381,
		- 0.394005, -0.307831,
		- 0.391304, -0.311257,
		- 0.388573, -0.314660,
		- 0.385812, -0.318039,
		- 0.383022, -0.321394,
		- 0.380203, -0.324724,
		- 0.377355, -0.328029,
		- 0.374478, -0.331310,
		- 0.371572, -0.334565,
		- 0.368639, -0.337795,
		- 0.365677, -0.340999,
		- 0.362687, -0.344177,
		- 0.359670, -0.347329,
		- 0.356625, -0.350455,
		- 0.353553, -0.353553,
		- 0.350455, -0.356625,
		- 0.347329, -0.359670,
		- 0.344177, -0.362687,
		- 0.340999, -0.365677,
		- 0.337795, -0.368639,
		- 0.334565, -0.371572,
		- 0.331310, -0.374478,
		- 0.328029, -0.377355,
		- 0.324724, -0.380203,
		- 0.321394, -0.383022,
		- 0.318039, -0.385812,
		- 0.314660, -0.388573,
		- 0.311257, -0.391304,
		- 0.307831, -0.394005,
		- 0.304381, -0.396677,
		- 0.300908, -0.399318,
		- 0.297411, -0.401928,
		- 0.293893, -0.404508,
		- 0.290351, -0.407058,
		- 0.286788, -0.409576,
		- 0.283203, -0.412063,
		- 0.279596, -0.414519,
		- 0.275968, -0.416943,
		- 0.272319, -0.419335,
		- 0.268650, -0.421696,
		- 0.264960, -0.424024,
		- 0.261249, -0.426320,
		- 0.257519, -0.428584,
		- 0.253769, -0.430815,
		- 0.250000, -0.433013,
		- 0.246212, -0.435178,
		- 0.242405, -0.437310,
		- 0.238579, -0.439409,
		- 0.234736, -0.441474,
		- 0.230874, -0.443505,
		- 0.226995, -0.445503,
		- 0.223099, -0.447467,
		- 0.219186, -0.449397,
		- 0.215256, -0.451293,
		- 0.211309, -0.453154,
		- 0.207347, -0.454981,
		- 0.203368, -0.456773,
		- 0.199374, -0.458530,
		- 0.195366, -0.460252,
		- 0.191342, -0.461940,
		- 0.187303, -0.463592,
		- 0.183251, -0.465209,
		- 0.179184, -0.466790,
		- 0.175104, -0.468336,
		- 0.171010, -0.469846,
		- 0.166903, -0.471321,
		- 0.162784, -0.472759,
		- 0.158652, -0.474162,
		- 0.154509, -0.475528,
		- 0.150353, -0.476858,
		- 0.146186, -0.478152,
		- 0.142008, -0.479410,
		- 0.137819, -0.480631,
		- 0.133619, -0.481815,
		- 0.129410, -0.482963,
		- 0.125190, -0.484074,
		- 0.120961, -0.485148,
		- 0.116723, -0.486185,
		- 0.112476, -0.487185,
		- 0.108220, -0.488148,
		- 0.103956, -0.489074,
		- 0.099684, -0.489962,
		- 0.095404, -0.490814,
		- 0.091118, -0.491627,
		- 0.086824, -0.492404,
		- 0.082524, -0.493143,
		- 0.078217, -0.493844,
		- 0.073905, -0.494508,
		- 0.069587, -0.495134,
		- 0.065263, -0.495722,
		- 0.060935, -0.496273,
		- 0.056602, -0.496786,
		- 0.052264, -0.497261,
		- 0.047923, -0.497698,
		- 0.043578, -0.498097,
		- 0.039230, -0.498459,
		- 0.034878, -0.498782,
		- 0.030524, -0.499067,
		- 0.026168, -0.499315,
		- 0.021810, -0.499524,
		- 0.017450, -0.499695,
		- 0.013088, -0.499829,
		- 0.008726, -0.499924,
		- 0.004363, -0.499981,
		- 0.000000, -0.500000,
		0.004363, -0.499981,
		0.008726, -0.499924,
		0.013088, -0.499829,
		0.017450, -0.499695,
		0.021810, -0.499524,
		0.026168, -0.499315,
		0.030524, -0.499067,
		0.034878, -0.498782,
		0.039230, -0.498459,
		0.043578, -0.498097,
		0.047923, -0.497698,
		0.052264, -0.497261,
		0.056602, -0.496786,
		0.060935, -0.496273,
		0.065263, -0.495722,
		0.069587, -0.495134,
		0.073905, -0.494508,
		0.078217, -0.493844,
		0.082524, -0.493143,
		0.086824, -0.492404,
		0.091118, -0.491627,
		0.095404, -0.490814,
		0.099684, -0.489962,
		0.103956, -0.489074,
		0.108220, -0.488148,
		0.112476, -0.487185,
		0.116723, -0.486185,
		0.120961, -0.485148,
		0.125190, -0.484074,
		0.129410, -0.482963,
		0.133619, -0.481815,
		0.137819, -0.480631,
		0.142008, -0.479410,
		0.146186, -0.478152,
		0.150353, -0.476858,
		0.154509, -0.475528,
		0.158652, -0.474162,
		0.162784, -0.472759,
		0.166903, -0.471321,
		0.171010, -0.469846,
		0.175104, -0.468336,
		0.179184, -0.466790,
		0.183251, -0.465209,
		0.187303, -0.463592,
		0.191342, -0.461940,
		0.195366, -0.460252,
		0.199374, -0.458530,
		0.203368, -0.456773,
		0.207347, -0.454981,
		0.211309, -0.453154,
		0.215256, -0.451293,
		0.219186, -0.449397,
		0.223099, -0.447467,
		0.226995, -0.445503,
		0.230874, -0.443505,
		0.234736, -0.441474,
		0.238579, -0.439409,
		0.242405, -0.437310,
		0.246212, -0.435178,
		0.250000, -0.433013,
		0.253769, -0.430815,
		0.257519, -0.428584,
		0.261249, -0.426320,
		0.264960, -0.424024,
		0.268650, -0.421696,
		0.272319, -0.419335,
		0.275968, -0.416943,
		0.279596, -0.414519,
		0.283203, -0.412063,
		0.286788, -0.409576,
		0.290351, -0.407058,
		0.293893, -0.404508,
		0.297411, -0.401928,
		0.300908, -0.399318,
		0.304381, -0.396677,
		0.307831, -0.394005,
		0.311257, -0.391304,
		0.314660, -0.388573,
		0.318039, -0.385812,
		0.321394, -0.383022,
		0.324724, -0.380203,
		0.328029, -0.377355,
		0.331310, -0.374478,
		0.334565, -0.371572,
		0.337795, -0.368639,
		0.340999, -0.365677,
		0.344177, -0.362687,
		0.347329, -0.359670,
		0.350455, -0.356625,
		0.353553, -0.353553,
		0.356625, -0.350455,
		0.359670, -0.347329,
		0.362687, -0.344177,
		0.365677, -0.340999,
		0.368639, -0.337795,
		0.371572, -0.334565,
		0.374478, -0.331310,
		0.377355, -0.328029,
		0.380203, -0.324724,
		0.383022, -0.321394,
		0.385812, -0.318039,
		0.388573, -0.314660,
		0.391304, -0.311257,
		0.394005, -0.307831,
		0.396677, -0.304381,
		0.399318, -0.300908,
		0.401928, -0.297411,
		0.404508, -0.293893,
		0.407058, -0.290351,
		0.409576, -0.286788,
		0.412063, -0.283203,
		0.414519, -0.279596,
		0.416943, -0.275968,
		0.419335, -0.272319,
		0.421696, -0.268650,
		0.424024, -0.264960,
		0.426320, -0.261249,
		0.428584, -0.257519,
		0.430815, -0.253769,
		0.433013, -0.250000,
		0.435178, -0.246212,
		0.437310, -0.242405,
		0.439409, -0.238579,
		0.441474, -0.234736,
		0.443505, -0.230874,
		0.445503, -0.226995,
		0.447467, -0.223099,
		0.449397, -0.219186,
		0.451293, -0.215256,
		0.453154, -0.211309,
		0.454981, -0.207347,
		0.456773, -0.203368,
		0.458530, -0.199374,
		0.460252, -0.195366,
		0.461940, -0.191342,
		0.463592, -0.187303,
		0.465209, -0.183251,
		0.466790, -0.179184,
		0.468336, -0.175104,
		0.469846, -0.171010,
		0.471321, -0.166903,
		0.472759, -0.162784,
		0.474162, -0.158652,
		0.475528, -0.154509,
		0.476858, -0.150353,
		0.478152, -0.146186,
		0.479410, -0.142008,
		0.480631, -0.137819,
		0.481815, -0.133619,
		0.482963, -0.129410,
		0.484074, -0.125190,
		0.485148, -0.120961,
		0.486185, -0.116723,
		0.487185, -0.112476,
		0.488148, -0.108220,
		0.489074, -0.103956,
		0.489962, -0.099684,
		0.490814, -0.095404,
		0.491627, -0.091118,
		0.492404, -0.086824,
		0.493143, -0.082524,
		0.493844, -0.078217,
		0.494508, -0.073905,
		0.495134, -0.069587,
		0.495722, -0.065263,
		0.496273, -0.060935,
		0.496786, -0.056602,
		0.497261, -0.052264,
		0.497698, -0.047923,
		0.498097, -0.043578,
		0.498459, -0.039230,
		0.498782, -0.034878,
		0.499067, -0.030524,
		0.499315, -0.026168,
		0.499524, -0.021810,
		0.499695, -0.017450,
		0.499829, -0.013088,
		0.499924, -0.008726,
		0.499981, -0.004363,
		0.500000, -0.000000,
	]);
	circleVerticesSize = circleVertices.length;

	var circleVertices = new Float32Array([
		1.0, 1.0, 1.0
	]);

    // vao triangle
    vao_triangle = gl.createVertexArray();
    gl.bindVertexArray(vao_triangle);

        // vbo
        vbo_triangle = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_triangle);
        gl.bufferData(gl.ARRAY_BUFFER, triangleVertices, gl.STATIC_DRAW);
        gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 2, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		vbo_triangle = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_triangle);
		gl.bufferData(gl.ARRAY_BUFFER, triangleColor, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_COLOR);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);

	// vao square
	vao_square = gl.createVertexArray();
	gl.bindVertexArray(vao_square);

		// vbo
		vbo_square = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_square);
		gl.bufferData(gl.ARRAY_BUFFER, squareVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		vbo_square = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_square);
		gl.bufferData(gl.ARRAY_BUFFER, squareColor, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_COLOR);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);

	// vao Circle
	vao_circle = gl.createVertexArray();
	gl.bindVertexArray(vao_circle);

		// vbo
		vbo_circle = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_circle);
		gl.bufferData(gl.ARRAY_BUFFER, circleVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		gl.vertexAttrib3f(webglMacros.SBJ_ATTRIBUTE_COLOR, 1.0, 1.0, 0.0);
		/*
		vbo_circle = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_circle);
		gl.bufferData(gl.ARRAY_BUFFER, circleColor, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_COLOR);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);
		*/
	gl.bindVertexArray(null);

	// vao graph lines
	vao_graph = gl.createVertexArray();
	gl.bindVertexArray(vao_graph);

		// vbo
		vbo_graph = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_graph);
		gl.bufferData(gl.ARRAY_BUFFER, graphPaperVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		vbo_graph = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_graph);
		gl.bufferData(gl.ARRAY_BUFFER, graphPaperColor, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_COLOR);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);


	// vao graph axis
	vao_graphAxis = gl.createVertexArray();
	gl.bindVertexArray(vao_graphAxis);

		// vbo
		vbo_graphAxis = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_graphAxis);
		gl.bufferData(gl.ARRAY_BUFFER, graphPaperAxisVertices, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 2, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
		gl.bindBuffer(gl.ARRAY_BUFFER, null);

		vbo_graphAxis = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, vbo_graphAxis);
		gl.bufferData(gl.ARRAY_BUFFER, graphPaperAxisColor, gl.STATIC_DRAW);
		gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_COLOR);
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

    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -3.0]);
    mat4.multiply(modelViewMatrix, modelViewMatrix, translationMatrix);
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);

    gl.bindVertexArray(vao_graphAxis);

		gl.drawArrays(gl.LINES, 0, 4);

	gl.bindVertexArray(null);

	if (gbDoubleClick)
		GraphPaperLines();

	if (gbToggleSquare)
		DrawSquare();

	if (gbToggleCircle)
		DrawCircle();

	if (gbToggleTriangle)
		DrawTriangle();

    // Unuse The Shader Program Object
    gl.useProgram(null);

    // Double Buffering Imulation
    requestAnimationFrame(display, canvas);
}

function update() {

    // Code
}

function GraphPaperLines()
{
	// Code
	gl.bindVertexArray(vao_graph);

	gl.drawArrays(gl.LINES, 0, graphPaperVerticesSize);

	gl.bindVertexArray(null);
}

function DrawSquare() {
	// Code
	gl.bindVertexArray(vao_square);

	gl.drawArrays(gl.LINES, 0, 8);

	gl.bindVertexArray(null);
}

function DrawCircle()
{
	// Code
	//gl.pointSize(0.5);
	gl.bindVertexArray(vao_circle);

	gl.drawArrays(gl.LINE_STRIP, 0, circleVerticesSize / 2);

	gl.bindVertexArray(null);
}

function DrawTriangle()
{
	// Code
	gl.bindVertexArray(vao_triangle);

	gl.drawArrays(gl.LINES, 0, 6);

	gl.bindVertexArray(null);
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

        //case 'S':
        case 83:
            if (gbToggleSquare == false)
                gbToggleSquare = true;
            else
                gbToggleSquare = false;
            break;

        //case 'C':
        case 67:
            if (gbToggleCircle == false)
                gbToggleCircle = true;
            else
                gbToggleCircle = false;
            break;

        //case 'T':
        case 84:
            if (gbToggleTriangle == false)
                gbToggleTriangle = true;
            else
                gbToggleTriangle = false;
            break;

        default:
            break;
    }
}

// Mouse Event Listener
function mouseDown() {

    // Code
    gbDoubleClick = true;
}

function uninitialize() {

    // Code
    if (vbo_graphAxis) {
		gl.deleteBuffer(vbo_graphAxis);
		vbo_graphAxis = null;
	}

	if (vbo_graph) {
		gl.deleteBuffer(vbo_graph);
		vbo_graph = null;
	}

	if (vbo_circle) {
		gl.deleteBuffer(vbo_circle);
		vbo_circle = null;
	}

	if (vbo_square) {
		gl.deleteBuffer(vbo_square);
		vbo_square = null;
	}

	if (vbo_triangle) {
		gl.deleteBuffer(vbo_triangle);
		vbo_triangle = null;
	}

	// vao
    if (vao_graphAxis)
    {
		gl.deleteVertexArray(vao_graphAxis);
		vao_graphAxis = null;
    }

	if (vao_graph) {
		gl.deleteVertexArray(vao_graph);
		vao_graph = null;
	}

	if (vao_circle) {
		gl.deleteVertexArray(vao_circle);
		vao_circle = null;
	}

	if (vao_square) {
		gl.deleteVertexArray(vao_square);
		vao_square = null;
	}

	if (vao_triangle) {
		gl.deleteVertexArray(vao_triangle);
		vao_triangle = null;
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
