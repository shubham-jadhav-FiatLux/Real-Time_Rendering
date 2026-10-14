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

// Start Field Related Variables
var NUM_STARS = 50;
var seed = 0x13371337;
var time1 = 1.0;
var timeUniform;
var particleSize = 10.0;
var particleSizeUniform;

var vao_starfield;
var vbo_starfield_position;
var vbo_starfield_color;

var texture_credits = null;
var texture_amit = null;
var texture_anusha = null;
var texture_dnyaneshwari = null;
var texture_shekhar = null;
var texture_shubham = null;
var texture_varad = null;
var texture_vipin = null;
var texture_yogesh = null;
var texture_ashwini = null;
var texture_technologiesUsed = null;
var texture_effects = null;
var texture_cm_ps_sl = null;
var texture_pl_hm = null;
var texture_references_book = null;
var texture_references_website = null;
var texture_song_title = null;
var texture_yogeshwarSir = null;
var texture_radhikaMam = null;
var texture_gokhaleMam = null;
var texture_sir = null;
var texture_thankYou = null;
var textureStarUniform;
var textureSmileyUniform;


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
        "uniform float time;" +
        "uniform mat4 u_mvpMatrix;" +
        "flat out vec4 starColor;" +
        "flat out int Flag;" +
        "uniform float particleSize;" +
        "void main(void)" +
        "{" +
        "vec4 newVertex = a_position;" +
        "newVertex.z += time;" +
        "newVertex.z = fract(newVertex.z);" +
        "float size = (30.0 * newVertex.z * newVertex.z);" +
        /*"starColor = smoothstep(1.0, 2.0, size) * a_color * a_color;" +*/
        "starColor = smoothstep(1.0, 3.0, size) * vec4(1.0, 1.0, 1.0, 1.0);" +
        "if (newVertex.x >= -0.01 && newVertex.x <= 0.01)" +
        "{" +
        "Flag = 1;" +
        "newVertex.z = (999.9 * newVertex.z) - 1000.0;" +
        "}" +
        "else" +
        "{" +
        "Flag = 0;" +
        "newVertex.z = (999.9 * newVertex.z) - 950.0;" +
        "}" +
        "gl_Position = u_mvpMatrix * newVertex;" +
        "gl_PointSize = size * particleSize;" +
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
        "flat in vec4 starColor;" +
        "uniform sampler2D u_texture_star;" +
        "uniform sampler2D u_texture_smiley;" +
        "out vec4 FragColor;" +
        "flat in int Flag;" +
        "void main(void)" +
        "{" +
        "const vec4 color1 = vec4(0.6, 0.0, 0.0, 1.0);" +
        "const vec4 color2 = vec4(0.9, 0.7, 1.0, 0.0);" +
        "const vec4 color3 = vec4(0.0, 0.6, 0.0, 1.0);" +
        "vec2 temp = gl_PointCoord - vec2(0.5);" +
        "float f = dot(temp, temp);" +
        "if (f > 0.2)" +
        "discard;" +
        "vec4 Color = mix(color1, color2, smoothstep(0.1, 0.3, f));" +
        "vec4 Color2 = mix(color3, color2, smoothstep(0.1, 0.3, f));" +
        "if (Flag == 1)" +
        "{" +
        "FragColor = starColor * Color2 * texture(u_texture_star, vec2(gl_PointCoord));" +
        "}" +
        "else" +
        "{" +
        "FragColor = starColor * Color;" +
        "}" +
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
    timeUniform = gl.getUniformLocation(shaderProgramObject, "time");
    particleSizeUniform = gl.getUniformLocation(shaderProgramObject, "particleSize");
    textureStarUniform = gl.getUniformLocation(shaderProgramObject, "u_texture_star");
    textureSmileyUniform = gl.getUniformLocation(shaderProgramObject, "u_texture_smiley");

  
    // vao And vbo Related Code
    vao_starfield = gl.createVertexArray();
    gl.bindVertexArray(vao_starfield);

        var star_pos = new Float32Array(1000);
        var star_color = new Float32Array(1000);

        class star_t {
            constructor(position, color) {
                this.position = position;
                this.color = color;
            }
        };

        var i;
        var data = [];

        star_pos[i] = 0.0;
        star_pos[i + 1] = 0.0;
        star_pos[i + 2] = 0.0;

        for (i = 3; i < 1000; i += 3) {

            star_pos[i] = (random_float());
            star_pos[i + 1] = (random_float());
            star_pos[i + 2] = (random_float());

            star_color[i] = (0.8 + random_float() * 0.2);
            star_color[i + 1] = (0.8 + random_float() * 0.2);
            star_color[i + 2] = (0.8 + random_float() * 0.2);
        }


        vbo_starfield_position = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_starfield_position);
        gl.bufferData(gl.ARRAY_BUFFER, star_pos, gl.STATIC_DRAW);
        gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_POSITION, 3, gl.FLOAT,false, 0, 0);
        gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_POSITION);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

        vbo_starfield_color = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo_starfield_color);
        gl.bufferData(gl.ARRAY_BUFFER, star_color, gl.STATIC_DRAW);
        gl.vertexAttribPointer(webglMacros.SBJ_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(webglMacros.SBJ_ATTRIBUTE_COLOR);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);

   

    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // Clear The Screen By Black Color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    // Texture
    // Credits
    texture_credits = gl.createTexture();
    texture_credits.image = new Image();
    texture_credits.image.src = "credits.png";
    texture_credits.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_credits);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_credits.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Amit
    texture_amit = gl.createTexture();
    texture_amit.image = new Image();
    texture_amit.image.src = "amit.png";
    texture_amit.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_amit);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);      

        gl.texImage2D(gl.TEXTURE_2D,
                      0,
                      gl.RGBA,
                      gl.RGBA,
                      gl.UNSIGNED_BYTE,
                      texture_amit.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Anusha
    texture_anusha = gl.createTexture();
    texture_anusha.image = new Image();
    texture_anusha.image.src = "anusha.png";
    texture_anusha.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_anusha);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_anusha.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Dnyaneshwari
    texture_dnyaneshwari = gl.createTexture();
    texture_dnyaneshwari.image = new Image();
    texture_dnyaneshwari.image.src = "dnyaneshwari.png";
    texture_dnyaneshwari.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_dnyaneshwari);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_dnyaneshwari.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Shekhar
    texture_shekhar = gl.createTexture();
    texture_shekhar.image = new Image();
    texture_shekhar.image.src = "shekhar.png";
    texture_shekhar.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_shekhar);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_shekhar.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // shubham
    texture_shubham = gl.createTexture();
    texture_shubham.image = new Image();
    texture_shubham.image.src = "shubham.png";
    texture_shubham.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_shubham);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_shubham.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Varad
    texture_varad = gl.createTexture();
    texture_varad.image = new Image();
    texture_varad.image.src = "varad.png";
    texture_varad.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_varad);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_varad.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Vipin
    texture_vipin = gl.createTexture();
    texture_vipin.image = new Image();
    texture_vipin.image.src = "vipin.png";
    texture_vipin.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_vipin);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_vipin.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Yogesh
    texture_yogesh = gl.createTexture();
    texture_yogesh.image = new Image();
    texture_yogesh.image.src = "yogesh.png";
    texture_yogesh.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_yogesh);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_yogesh.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Ashwini
    texture_ashwini = gl.createTexture();
    texture_ashwini.image = new Image();
    texture_ashwini.image.src = "ashwini.png";
    texture_ashwini.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_ashwini);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_ashwini.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Technologies Used
    texture_technologiesUsed = gl.createTexture();
    texture_technologiesUsed.image = new Image();
    texture_technologiesUsed.image.src = "technologyUsed.png";
    texture_technologiesUsed.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_technologiesUsed);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_technologiesUsed.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Effects
    texture_effects = gl.createTexture();
    texture_effects.image = new Image();
    texture_effects.image.src = "effects.png";
    texture_effects.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_effects);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_effects.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Cubemap_ParticleSystem_SpotLight
    texture_cm_ps_sl = gl.createTexture();
    texture_cm_ps_sl.image = new Image();
    texture_cm_ps_sl.image.src = "cubemap_particleSystem_spotlight.png";
    texture_cm_ps_sl.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_cm_ps_sl);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_cm_ps_sl.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // PointLight_HeightMap
    texture_pl_hm = gl.createTexture();
    texture_pl_hm.image = new Image();
    texture_pl_hm.image.src = "pointLight_heightmap.png";
    texture_pl_hm.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_pl_hm);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_pl_hm.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // References Book
    texture_references_book = gl.createTexture();
    texture_references_book.image = new Image();
    texture_references_book.image.src = "references_books.png";
    texture_references_book.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_references_book);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_references_book.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // References Website
    texture_references_website = gl.createTexture();
    texture_references_website.image = new Image();
    texture_references_website.image.src = "references_website.png";
    texture_references_website.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_references_website);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_references_website.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Ashwini
    texture_song_title = gl.createTexture();
    texture_song_title.image = new Image();
    texture_song_title.image.src = "song_title.png";
    texture_song_title.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_song_title);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_song_title.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Radhika Mam
    texture_radhikaMam = gl.createTexture();
    texture_radhikaMam.image = new Image();
    texture_radhikaMam.image.src = "radhikaMam.png";
    texture_radhikaMam.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_radhikaMam);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_radhikaMam.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Yogeshwar Sir
    texture_yogeshwarSir = gl.createTexture();
    texture_yogeshwarSir.image = new Image();
    texture_yogeshwarSir.image.src = "yogeshwarSir.png";
    texture_yogeshwarSir.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_yogeshwarSir);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_yogeshwarSir.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Gokhale Mam
    texture_gokhaleMam = gl.createTexture();
    texture_gokhaleMam.image = new Image();
    texture_gokhaleMam.image.src = "gokhaleMam.png";
    texture_gokhaleMam.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_gokhaleMam);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_gokhaleMam.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Sir
    texture_sir = gl.createTexture();
    texture_sir.image = new Image();
    texture_sir.image.src = "sir.png";
    texture_sir.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_sir);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_sir.image);

        gl.generateMipmap(gl.TEXTURE_2D);

        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    // Thank You
    texture_thankYou = gl.createTexture();
    texture_thankYou.image = new Image();
    texture_thankYou.image.src = "thankYou.png";
    texture_thankYou.image.onload = function ()               // functure or closure or lambda
    {
        // Code
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, texture_thankYou);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);      // LINEAR and LINEAR_MIPMAP_LINEAR are not portable across browsers
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            texture_thankYou.image);

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

    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 1000.0);
}

function display() {

    // Code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.enable(gl.PROGRAM_POINT_SIZE);

    // Use The Shader Program Object
    gl.useProgram(shaderProgramObject);

    // Transformations
    var translationMatrix = mat4.create();
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);
    gl.uniform1f(timeUniform, time1);
    gl.uniform1f(particleSizeUniform, particleSize);

    gl.activeTexture(gl.TEXTURE0);
    gl.uniform1i(textureStarUniform, 0);

    if (time1 <= 2.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_credits);
    }
    else if (time1 <= 3.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_amit);
    }
    else if (time1 <= 4.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_anusha);
    }
    else if (time1 <= 5.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_dnyaneshwari);
    }
    else if (time1 <= 6.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_shekhar);
    }
    else if (time1 <= 7.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_shubham);
    }
    else if (time1 <= 8.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_varad);
    }
    else if (time1 <= 9.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_vipin);
    }
    else if (time1 <= 10.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_yogesh);
    }
    else if (time1 <= 11.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_ashwini);
    }
    else if (time1 <= 12.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_technologiesUsed);
    }
    else if (time1 <= 13.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_effects);
    }
    else if (time1 <= 14.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_cm_ps_sl);
    }
    else if (time1 <= 15.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_pl_hm);
    }
    else if (time1 <= 16.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_references_book);
    }
    else if (time1 <= 17.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_references_website);
    }
    else if (time1 <= 18.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_song_title);
    }
    else if (time1 <= 19.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_radhikaMam);
    }
    else if (time1 <= 20.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_yogeshwarSir);
    }
    else if (time1 <= 21.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_gokhaleMam);
    }
    else if (time1 <= 22.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_sir);
        particleSize = 20.0;
        gl.uniform1f(particleSizeUniform, particleSize);
    }
    else if (time1 <= 23.0) {
        gl.bindTexture(gl.TEXTURE_2D, texture_thankYou);
        particleSize = 10.0;
        gl.uniform1f(particleSizeUniform, particleSize);
    }
    else {
        NUM_STARS = 0;
    }

    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE);
    gl.enable(gl.POINT_SPRITE);

    gl.bindVertexArray(vao_starfield);
        gl.drawArrays(gl.POINTS, 0, NUM_STARS);
    gl.bindVertexArray(null);

    gl.bindTexture(gl.TEXTURE_2D, null);

    // Unuse The Shader Program Object
    gl.useProgram(null);

    // Update
    update();

    // Double Buffering Imulation
    requestAnimationFrame(display, canvas);
}

function update() {

    // Code
    time1 += 0.003;
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

// Random Number Generator
/*
function random_float()
{
    // Variable Declarations
    var res;
    let temp;

    // Code
    seed *= 16807;
    temp = seed ^ (seed >> 4) ^ (seed << 15);
	res = (temp >> 9) | 0x3F800000;

    return (res - 1.0);
}
*/
function random_float() {
    var max = 100.0;
    var min = -100.0;

    const randNum = (Math.random() * (max - min) + min).toFixed(1);

    return parseFloat(randNum);
}

function uninitialize() {

    // Code
    if (texture_smiley) {
        gl.deleteTexture(1, texture_smiley);
        texture_smiley = 0;
    }

    if (texture_star) {
        gl.deleteTexture(1, texture_star);
        texture_star = 0;
    }

    if (vbo_starfield_position) {
        gl.deleteBuffer(vbo_starfield_position);
        vbo_starfield_position = null;
    }

    if (vbo_starfield_color) {
        gl.deleteBuffer(vbo_starfield_color);
        vbo_starfield_color = null;
    }

    if (vao_starfield)
    {
        gl.deleteVertexArray(vao_starfield);
        vao_starfield = null;
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
