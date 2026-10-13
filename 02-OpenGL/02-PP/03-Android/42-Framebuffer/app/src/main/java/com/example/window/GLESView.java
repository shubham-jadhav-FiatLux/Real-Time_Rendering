package com.example.window;

import android.opengl.GLSurfaceView;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;
import android.opengl.GLES32;
import android.content.Context;

// Event Related Packages
import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.MotionEvent;
import android.view.GestureDetector.OnGestureListener;

// Buffer Related Packages
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

// Matrix Related Package
import android.opengl.Matrix;

public class GLESView extends GLSurfaceView implements OnDoubleTapListener, OnGestureListener, GLSurfaceView.Renderer {

	private GestureDetector gestureDetector;
	private Context context;

	private int FBO_WIDTH = 512;
	private int FBO_HEIGHT = 512;
	private int winWidth;
	private int winHeight;

	private int shaderProgramObject;

	private int vao_cube[] = new int[1];
	private int vbo_cube_position[] = new int[1];
	private int vbo_cube_texcoord[] = new int[1];

	private float angleCube = 0.0f;

	private int mvpMatrixUniform;
	private int textureSamplerUniform;
	
	private float perspectiveProjectionMatrix[] = new float[16]; 


	// FBO Related Global Variables
	private int fbo[] = new int[1];
	private int rbo[] = new int[1];
	private int fbo_texture[] = new int[1];
	private boolean bFBOResult = false;

	// Texture Scene Global Vaiables
	private float sphere_vertices[]=new float[1146];
    private float sphere_normals[]=new float[1146];
    private float sphere_textures[]=new float[764];
    private short sphere_elements[]=new short[2280];
	private int gNumVertices_sphere;
	private int gNumElements_sphere;

	private int shaderProgramObject_sphere;

	private int vao_Sphere[] = new int[1];
	private int vbo_Sphere_Position[] = new int[1];
	private int vbo_Sphere_Normals[] = new int[1];
	private int vbo_Sphere_Elements[] = new int[1];

	private int modelMatrixUniform_sphere;
	private int viewMatrixUniform_sphere;
	private int projectionMatrixUniform_sphere;

	private int laUniform_sphere[] = new int[3];				// Ambient
	private int ldUniform_sphere[] = new int[3];				// Diffuse
	private int lsUniform_sphere[] = new int[3];				// Specular
	private int lightPositionUniform_sphere[] = new int[3];

	private class Light
	{
		float lightAmbient_sphere[] = new float[4];
		float lightDiffuse_sphere[] = new float[4];
		float lightSpecular_sphere[] = new float[4];
		float lightPosition_sphere[] = new float[4];
	}

	private Light lights_sphere[] = new Light[3];

	private int kaUniform_sphere;
	private int kdUniform_sphere;
	private int ksUniform_sphere;
	private int materialShininessUniform_sphere;

	private int lightingEnabledUniform_sphere;
	private boolean bLight  = false;
	private float lightAngleZero_sphere = 0.0f;
	private float lightAngleOne_sphere = 0.0f;
	private float lightAngleTwo_sphere = 0.0f;

	private float materialAmbient_sphere[] = new float[]{0.0f, 0.0f, 0.0f, 1.0f};
	private float materialDiffuse_sphere[] = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
	private float materialSpecular_sphere[] = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
	private float materialShininess_sphere = 50.0f;
	
	private float perspectiveProjectionMatrix_sphere[] = new float[16];

	GLESView(Context _context){
		
		super(_context);

		context = _context;

		setEGLContextClientVersion(3);					// OpenGLES 3.2
		setRenderer(this);
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

		// Gesture Related Code
		gestureDetector = new GestureDetector(context, this, null, false);
		gestureDetector.setOnDoubleTapListener(this);
	}

	// Three Methods Of GLSurfaceView.Renderer
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
	
		// Code
		String glesVersion = gl.glGetString(GL10.GL_VERSION);
		System.out.println("SBJ: " + glesVersion);

		String renderer = gl.glGetString(GL10.GL_RENDERER);
		System.out.println("SBJ: " + renderer);

		String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
		System.out.println("SBJ: " + glslVersion);

		initialize();
	}

	@Override
	public void onSurfaceChanged(GL10 unused, int width, int height) {
	
		// Code

		resize(width, height);
	}

	@Override
	public void onDrawFrame(GL10 unused) {
	
		// Code
		// onDrawFrame Should Be Considered As GameLoop
		display();

		update();
	}


	@Override
	public boolean onTouchEvent(MotionEvent e) {
	
		// Code
		if (!gestureDetector.onTouchEvent(e)) {

			super.onTouchEvent(e);
		}
		return true;
	}


	// Three Methods Of OnDoubleTapListner
	@Override
	public boolean onDoubleTap(MotionEvent e) {
	
		if (bLight == false)
			bLight = true;
		else
			bLight = false;

		return true;
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent e) {
	
		return true;
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent e) {
	
		return true;
	}


	// Six Methods Of OnGestureListner
	@Override
	public boolean onDown(MotionEvent e) {
	
		return true;
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
	
		return true;
	}

	@Override
	public void onLongPress(MotionEvent e) {

	}

	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {

		uninitialize();
		System.exit(0);
		return true;
	}

	@Override
	public void onShowPress(MotionEvent e) {

	}

	@Override
	public boolean onSingleTapUp(MotionEvent e) {
	
		return true;
	}


	// Custom Private Functions
	private void initialize() {

		// Code
		// Vertex Shader
		final String vertexShaderSourceCode = String.format
		(
			"#version 320 es" +
			"\n" +
			"in vec4 a_position;" +
			"in vec2 a_texcoord;" +
			"uniform mat4 u_mvpMatrix;" +
			"out vec2 a_texcoord_out;" +
			"void main(void)" +
			"{" +
			"gl_Position = u_mvpMatrix * a_position;" +
			"a_texcoord_out = a_texcoord;" +
			"}"
		);

		// Creating Shader Object
		int vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		// Giving Shader Code To Shader Object 
		GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);
		// Compile The Shader
		GLES32.glCompileShader(vertexShaderObject);
		// Error Checking
		int status[] = new int[1];
		int infoLogLength[] = new int[1];
		String log = null;
		GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, status, 0);
		if (status[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				log = GLES32.glGetShaderInfoLog(vertexShaderObject);
				System.out.println("SBJ : " + "Vertex Shader Compilation Log: " + log);
				uninitialize();
				System.exit(0);
			}
		}


		// Fragment Shader
		final String fragmentShaderSourceCode = String.format
		(
			"#version 320 es" +
			"\n" +
			"precision highp float;" +
			"in vec2 a_texcoord_out;" +
			"uniform highp sampler2D u_textureSampler;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
			"FragColor = texture(u_textureSampler, a_texcoord_out);" +
			"}"
		);

		// Creating Shader Object
		int fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		// Giving Shader Code To Shader Object 
		GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);
		// Compile The Shader
		GLES32.glCompileShader(fragmentShaderObject);
		// Error Checking
		status[0] = 0;
		infoLogLength[0] = 0;
		log = null;
		GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, status, 0);
		if (status[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				log = GLES32.glGetShaderInfoLog(fragmentShaderObject);
				System.out.println("SBJ : " + "Fragment Shader Compilation Log: " + log);
				uninitialize();
				System.exit(0);
			}
		}


		// Shader Program Object
		shaderProgramObject = GLES32.glCreateProgram();
		GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
		GLES32.glBindAttribLocation(shaderProgramObject, MyGLESMacros.SBJ_ATTRIBUTE_POSITION, "a_position");
		GLES32.glBindAttribLocation(shaderProgramObject, MyGLESMacros.SBJ_ATTRIBUTE_TEXTURE0, "a_texcoord");
		GLES32.glLinkProgram(shaderProgramObject);
		status[0] = 0;
		infoLogLength[0] = 0;
		log = null;
		GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, status, 0);
		if (status[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				log = GLES32.glGetProgramInfoLog(shaderProgramObject);
				System.out.println("SBJ : " + "Shader Program Link Log: " + log);
				uninitialize();
				System.exit(0);
			}
		}
		mvpMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");


		// Declaration Of Vertex Data Arrays
		final float cubePosition[] = new float[]
													{
														// top
														1.0f, 1.0f, -1.0f,
														-1.0f, 1.0f, -1.0f,
														-1.0f, 1.0f, 1.0f,
														1.0f, 1.0f, 1.0f,

														// bottom
														1.0f, -1.0f, -1.0f,
														-1.0f, -1.0f, -1.0f,
														-1.0f, -1.0f,  1.0f,
														1.0f, -1.0f,  1.0f,

														// front
														1.0f, 1.0f, 1.0f,
														-1.0f, 1.0f, 1.0f,
														-1.0f, -1.0f, 1.0f,
														1.0f, -1.0f, 1.0f,

														// back
														1.0f, 1.0f, -1.0f,
														-1.0f, 1.0f, -1.0f,
														-1.0f, -1.0f, -1.0f,
														1.0f, -1.0f, -1.0f,

														// right
														1.0f, 1.0f, -1.0f,
														1.0f, 1.0f, 1.0f,
														1.0f, -1.0f, 1.0f,
														1.0f, -1.0f, -1.0f,

														// left
														-1.0f, 1.0f, 1.0f,
														-1.0f, 1.0f, -1.0f,
														-1.0f, -1.0f, -1.0f,
														-1.0f, -1.0f, 1.0f,
													};

		final float cubeTexcoord[] = new float[]
													{
														0.0f, 0.0f,
														1.0f, 0.0f,
														1.0f, 1.0f,
														0.0f, 1.0f,

														0.0f, 0.0f,
														1.0f, 0.0f,
														1.0f, 1.0f,
														0.0f, 1.0f,

														0.0f, 0.0f,
														1.0f, 0.0f,
														1.0f, 1.0f,
														0.0f, 1.0f,

														0.0f, 0.0f,
														1.0f, 0.0f,
														1.0f, 1.0f,
														0.0f, 1.0f,

														0.0f, 0.0f,
														1.0f, 0.0f,
														1.0f, 1.0f,
														0.0f, 1.0f,

														0.0f, 0.0f,
														1.0f, 0.0f,
														1.0f, 1.0f,
														0.0f, 1.0f,
													};

		// vao And vbo Related Code
		GLES32.glGenVertexArrays(1, vao_cube, 0);
		GLES32.glBindVertexArray(vao_cube[0]);

			// Position
			GLES32.glGenBuffers(1, vbo_cube_position, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_cube_position[0]);

			ByteBuffer byteBuffer = ByteBuffer.allocateDirect(cubePosition.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
			positionBuffer.put(cubePosition);
			positionBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubePosition.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_POSITION);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			// Texcoord
			GLES32.glGenBuffers(1, vbo_cube_texcoord, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_cube_texcoord[0]);

			byteBuffer = ByteBuffer.allocateDirect(cubeTexcoord.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer texcoordBuffer = byteBuffer.asFloatBuffer();
			texcoordBuffer.put(cubeTexcoord);
			texcoordBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeTexcoord.length * 4, texcoordBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_TEXTURE0, 2, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_TEXTURE0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindVertexArray(0);


		// Here Starts OpenGL Code
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		//  Enable Culling
		//GLES32.glEnable(GLES32.GL_CULL_FACE);

		GLES32.glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		// Enabling the texture
		GLES32.glEnable(GLES32.GL_TEXTURE_2D);

		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);

		// FBO Code
		bFBOResult = createFBO(FBO_WIDTH, FBO_HEIGHT);
		int iRetVal;
		if (bFBOResult == true)
		{
			iRetVal = initialize_sphere(FBO_WIDTH, FBO_HEIGHT);

			// Here There Should Be Error Checking Of FBO
		}
		else
		{
			System.out.println("SBJ : " + "Create FBO Failed...");
			uninitialize();
			System.exit(0);
		}
	}

	boolean createFBO(int textureWidth, int textureHeight)
	{
		// Local Variable Declarations
		int maxRenderbufferSize[] = new int[1];

		// Code
		// Check Available Renderbuffer Size
		GLES32.glGetIntegerv(GLES32.GL_MAX_RENDERBUFFER_SIZE, maxRenderbufferSize, 0);
		if (maxRenderbufferSize[0] < textureWidth || maxRenderbufferSize[0] < textureHeight)
		{
			System.out.println("SBJ : " + "Insufficient Render Buffer Size ");
			return false;
		}

		// Create Framebuffer Object
		GLES32.glGenFramebuffers(1, fbo, 0);
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, fbo[0]);

		// Create Renderbuffer Object
		GLES32.glGenRenderbuffers(1, rbo, 0);
		GLES32.glBindRenderbuffer(GLES32.GL_RENDERBUFFER, rbo[0]);

		// Storage And Format Of The Renderbuffer
		GLES32.glRenderbufferStorage(GLES32.GL_RENDERBUFFER, GLES32.GL_DEPTH_COMPONENT16, textureWidth, textureHeight);

		// Create Empty Texture For Upcoming Target Scene
		GLES32.glGenTextures(1, fbo_texture, 0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, fbo_texture[0]);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_S, GLES32.GL_CLAMP_TO_EDGE);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_T, GLES32.GL_CLAMP_TO_EDGE);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR);

		GLES32.glTexImage2D(GLES32.GL_TEXTURE_2D,
					0,
					GLES32.GL_RGB,
					textureWidth,
					textureHeight,
					0,
					GLES32.GL_RGB,
					GLES32.GL_UNSIGNED_SHORT_5_6_5,
					null);

		GLES32.glFramebufferTexture2D(GLES32.GL_FRAMEBUFFER, GLES32.GL_COLOR_ATTACHMENT0, GLES32.GL_TEXTURE_2D, fbo_texture[0], 0);

		// Give Renderbuffer To FBO
		GLES32.glFramebufferRenderbuffer(GLES32.GL_FRAMEBUFFER, GLES32.GL_DEPTH_ATTACHMENT, GLES32.GL_RENDERBUFFER, rbo[0]);

		// Check Whether The Framebuffeer Created Successful or Not
		//enum result = GLES32.glCheckFramebufferStatus(GLES32.GL_FRAMEBUFFER);
		if (GLES32.glCheckFramebufferStatus(GLES32.GL_FRAMEBUFFER) != GLES32.GL_FRAMEBUFFER_COMPLETE)
		{
			System.out.println("SBJ : " + "Frame Buffer Is Not Complete ");
			return false;
		}

		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);

		return true;
	}

	int initialize_sphere(int width, int height)
	{
		// Code
		// Vertex Shader PerFragment
		final String vertexShaderSourceCode_pf = String.format
		(
			"#version 320 es" +
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
			"}"
		);

		// Creating Shader Object
		int vertexShaderObject_pf = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		// Giving Shader Code To Shader Object 
		GLES32.glShaderSource(vertexShaderObject_pf, vertexShaderSourceCode_pf);
		// Compile The Shader
		GLES32.glCompileShader(vertexShaderObject_pf);
		// Error Checking
		int status[] = new int[1];
		int infoLogLength[] = new int[1];
		String log = null;
		GLES32.glGetShaderiv(vertexShaderObject_pf, GLES32.GL_COMPILE_STATUS, status, 0);
		if (status[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObject_pf, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				log = GLES32.glGetShaderInfoLog(vertexShaderObject_pf);
				System.out.println("SBJ : " + "Vertex Shader Compilation Log: " + log);
				uninitialize();
				System.exit(0);
			}
		}


		// Fragment Shader PerFragment
		final String fragmentShaderSourceCode_pf = String.format
		(
			"#version 320 es" +
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
			"uniform highp int u_lightingEnabled;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
			"vec3 phong_ads_light;" +
			"if (u_lightingEnabled == 1)" +
			"{" +
			"vec3 array[3];" +
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
			"array[i] = ambient[i] + diffuse[i] + specular[i];" +
			"}" +
			"phong_ads_light = array[0] + array[1] + array[2];" +
			"}" +
			"else" +
			"{" +
			"phong_ads_light = vec3(1.0, 1.0, 1.0);" +
			"}" +
			"FragColor = vec4(phong_ads_light, 1.0);" +
			"}"
		);

		// Creating Shader Object
		int fragmentShaderObject_pf = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		// Giving Shader Code To Shader Object 
		GLES32.glShaderSource(fragmentShaderObject_pf, fragmentShaderSourceCode_pf);
		// Compile The Shader
		GLES32.glCompileShader(fragmentShaderObject_pf);
		// Error Checking
		status[0] = 0;
		infoLogLength[0] = 0;
		log = null;
		GLES32.glGetShaderiv(fragmentShaderObject_pf, GLES32.GL_COMPILE_STATUS, status, 0);
		if (status[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObject_pf, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				log = GLES32.glGetShaderInfoLog(fragmentShaderObject_pf);
				System.out.println("SBJ : " + "Fragment Shader Compilation Log: " + log);
				uninitialize();
				System.exit(0);
			}
		}


		// Shader Program Object PerFragment
		shaderProgramObject_sphere = GLES32.glCreateProgram();
		GLES32.glAttachShader(shaderProgramObject_sphere, vertexShaderObject_pf);
		GLES32.glAttachShader(shaderProgramObject_sphere, fragmentShaderObject_pf);
		GLES32.glBindAttribLocation(shaderProgramObject_sphere, MyGLESMacros.SBJ_ATTRIBUTE_POSITION, "a_position");
		GLES32.glBindAttribLocation(shaderProgramObject_sphere, MyGLESMacros.SBJ_ATTRIBUTE_NORMAL, "a_normal");
		GLES32.glLinkProgram(shaderProgramObject_sphere);
		status[0] = 0;
		infoLogLength[0] = 0;
		log = null;
		GLES32.glGetProgramiv(shaderProgramObject_sphere, GLES32.GL_LINK_STATUS, status, 0);
		if (status[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObject_sphere, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				log = GLES32.glGetProgramInfoLog(shaderProgramObject_sphere);
				System.out.println("SBJ : " + "Shader Program Link Log: " + log);
				uninitialize();
				System.exit(0);
			}
		}
		
		modelMatrixUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_modelMatrix");
		viewMatrixUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_viewMatrix");
		projectionMatrixUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_projectionMatrix");
		laUniform_sphere[0] = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_la[0]");
		laUniform_sphere[1] = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_la[1]");
		laUniform_sphere[2] = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_la[2]");
		ldUniform_sphere[0] = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_ld[0]");
		ldUniform_sphere[1] = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_ld[1]");
		ldUniform_sphere[2] = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_ld[2]");
		lsUniform_sphere[0] = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_ls[0]");
		lsUniform_sphere[1] = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_ls[1]");
		lsUniform_sphere[2] = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_ls[2]");
		lightPositionUniform_sphere[0] = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_lightPosition[0]");
		lightPositionUniform_sphere[1] = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_lightPosition[1]");
		lightPositionUniform_sphere[2] = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_lightPosition[2]");
		kaUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_ka");
		kdUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_kd");
		ksUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_ks");
		materialShininessUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_materialShininess");
		lightingEnabledUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "u_lightingEnabled");



		// Declaration Of Vertex Data Arrays
		Sphere sphere=new Sphere();
		sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
		gNumVertices_sphere = sphere.getNumberOfSphereVertices();
		gNumElements_sphere = sphere.getNumberOfSphereElements();

		// vao And vbo Related Code
		GLES32.glGenVertexArrays(1, vao_Sphere, 0);
		GLES32.glBindVertexArray(vao_Sphere[0]);

			// Position
			GLES32.glGenBuffers(1, vbo_Sphere_Position, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_Sphere_Position[0]);

			ByteBuffer byteBuffer = ByteBuffer.allocateDirect(sphere_vertices.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
			positionBuffer.put(sphere_vertices);
			positionBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_vertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_POSITION);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			// Normal
			GLES32.glGenBuffers(1, vbo_Sphere_Normals, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_Sphere_Normals[0]);

			byteBuffer = ByteBuffer.allocateDirect(sphere_normals.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer normalBuffer = byteBuffer.asFloatBuffer();
			normalBuffer.put(sphere_normals);
			normalBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_normals.length * 4, normalBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_NORMAL);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			// Elements
			GLES32.glGenBuffers(1, vbo_Sphere_Elements, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_Sphere_Elements[0]);

			byteBuffer = ByteBuffer.allocateDirect(sphere_elements.length * 2);
			byteBuffer.order(ByteOrder.nativeOrder());
			ShortBuffer elementBuffer = byteBuffer.asShortBuffer();
			elementBuffer.put(sphere_elements);
			elementBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_elements.length * 2, elementBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindVertexArray(0);


		// Here Starts OpenGL Code
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		//  Enable Culling
		//GLES32.glEnable(GLES32.GL_CULL_FACE);

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		lights_sphere[0] = new Light();
		lights_sphere[0].lightAmbient_sphere[0] = 0.0f;
		lights_sphere[0].lightAmbient_sphere[1] = 0.0f;
		lights_sphere[0].lightAmbient_sphere[2] = 0.0f;
		lights_sphere[0].lightAmbient_sphere[3] = 1.0f;
		lights_sphere[0].lightDiffuse_sphere[0] = 1.0f;
		lights_sphere[0].lightDiffuse_sphere[1] = 0.0f;
		lights_sphere[0].lightDiffuse_sphere[2] = 0.0f;
		lights_sphere[0].lightDiffuse_sphere[3] = 1.0f;
		lights_sphere[0].lightSpecular_sphere[0] = 1.0f;
		lights_sphere[0].lightSpecular_sphere[1] = 0.0f;
		lights_sphere[0].lightSpecular_sphere[2] = 0.0f;
		lights_sphere[0].lightSpecular_sphere[3] = 1.0f;
		lights_sphere[0].lightPosition_sphere[0] = -2.0f;
		lights_sphere[0].lightPosition_sphere[1] = 0.0f;
		lights_sphere[0].lightPosition_sphere[2] = 0.0f;
		lights_sphere[0].lightPosition_sphere[3] = 1.0f;

		lights_sphere[1] = new Light();
		lights_sphere[1].lightAmbient_sphere[0] = 0.0f;
		lights_sphere[1].lightAmbient_sphere[1] = 0.0f;
		lights_sphere[1].lightAmbient_sphere[2] = 0.0f;
		lights_sphere[1].lightAmbient_sphere[3] = 1.0f;
		lights_sphere[1].lightDiffuse_sphere[0] = 0.0f;
		lights_sphere[1].lightDiffuse_sphere[1] = 1.0f;
		lights_sphere[1].lightDiffuse_sphere[2] = 0.0f;
		lights_sphere[1].lightDiffuse_sphere[3] = 1.0f;
		lights_sphere[1].lightSpecular_sphere[0] = 0.0f;
		lights_sphere[1].lightSpecular_sphere[1] = 1.0f;
		lights_sphere[1].lightSpecular_sphere[2] = 0.0f;
		lights_sphere[1].lightSpecular_sphere[3] = 1.0f;
		lights_sphere[1].lightPosition_sphere[0] = 0.0f;
		lights_sphere[1].lightPosition_sphere[1] = 0.0f;
		lights_sphere[1].lightPosition_sphere[2] = 0.0f;
		lights_sphere[1].lightPosition_sphere[3] = 1.0f;

		lights_sphere[2] = new Light();
		lights_sphere[2].lightAmbient_sphere[0] = 0.0f;
		lights_sphere[2].lightAmbient_sphere[1] = 0.0f;
		lights_sphere[2].lightAmbient_sphere[2] = 0.0f;
		lights_sphere[2].lightAmbient_sphere[3] = 1.0f;
		lights_sphere[2].lightDiffuse_sphere[0] = 0.0f;
		lights_sphere[2].lightDiffuse_sphere[1] = 0.0f;
		lights_sphere[2].lightDiffuse_sphere[2] = 1.0f;
		lights_sphere[2].lightDiffuse_sphere[3] = 1.0f;
		lights_sphere[2].lightSpecular_sphere[0] = 0.0f;
		lights_sphere[2].lightSpecular_sphere[1] = 0.0f;
		lights_sphere[2].lightSpecular_sphere[2] = 1.0f;
		lights_sphere[2].lightSpecular_sphere[3] = 1.0f;
		lights_sphere[2].lightPosition_sphere[0] = 0.0f;
		lights_sphere[2].lightPosition_sphere[1] = 0.0f;
		lights_sphere[2].lightPosition_sphere[2] = 0.0f;
		lights_sphere[2].lightPosition_sphere[3] = 1.0f;

	
		Matrix.setIdentityM(perspectiveProjectionMatrix_sphere, 0);

		// Warmup resize() Call
		resize_sphere(FBO_WIDTH, FBO_HEIGHT);

		return 0;
	}

	private void resize(int width, int height) {
	
		// Code
		if (height == 0)
			height = 1;

		winWidth = width;
		winHeight = height;

		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix , 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}

	private void resize_sphere(int width, int height) {
	
		// Code
		if (height == 0)
			height = 1;

		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix_sphere , 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}

	private void display() {
	
		// Code
		if (bFBOResult == true)
		{
			display_sphere(FBO_WIDTH, FBO_HEIGHT);
			update_sphere();
		}

		GLES32.glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		resize(winWidth, winHeight);

		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		GLES32.glUseProgram(shaderProgramObject);

		// Transformations
		float translationMatrix[] = new float[16];
		Matrix.setIdentityM(translationMatrix, 0);
		float rotationMatrix_x[] = new float[16];
		Matrix.setIdentityM(rotationMatrix_x, 0);
		float rotationMatrix_y[] = new float[16];
		Matrix.setIdentityM(rotationMatrix_y, 0);
		float rotationMatrix_z[] = new float[16];
		Matrix.setIdentityM(rotationMatrix_z, 0);
		float rotationMatrix[] = new float[16];
		Matrix.setIdentityM(rotationMatrix, 0);
		float modelViewMatrix[] = new float[16];
		Matrix.setIdentityM(modelViewMatrix, 0);
		float modelViewProjectionMatrix[] = new float[16];
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);

		Matrix.translateM(translationMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, -6.0f);
		Matrix.setRotateM(rotationMatrix_x, 0, angleCube, 1.0f, 0.0f, 0.0f);
		Matrix.setRotateM(rotationMatrix_y, 0, angleCube, 0.0f, 1.0f, 0.0f);
		Matrix.setRotateM(rotationMatrix_z, 0, angleCube, 0.0f, 0.0f, 1.0f);
		Matrix.multiplyMM(rotationMatrix, 0, rotationMatrix_x, 0, rotationMatrix_y, 0);
		Matrix.multiplyMM(rotationMatrix, 0, rotationMatrix, 0, rotationMatrix_z, 0);
		Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
		GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);							// 80 Textures Can Be Activated
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, fbo_texture[0]);
		GLES32.glUniform1i(textureSamplerUniform, 0);

		GLES32.glBindVertexArray(vao_cube[0]);
	
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);

		GLES32.glBindVertexArray(0);

		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);

		GLES32.glUseProgram(0);

		requestRender();
	}

	private void update(){

		// Code
		angleCube = angleCube + 1.0f;
		if (angleCube >= 360.0f)
			angleCube = angleCube - 360.0f;
	}

	private void display_sphere(int textureWidth, int textureHeight) {
	
		// Code
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, fbo[0]);

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		resize_sphere(textureWidth, textureHeight);

		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		GLES32.glUseProgram(shaderProgramObject_sphere);

		// Transformations
		float translationMatrix[] = new float[16];
		Matrix.setIdentityM(translationMatrix, 0);
		float modelMatrix[] = new float[16];
		Matrix.setIdentityM(modelMatrix, 0);
		float viewMatrix[] = new float[16];
		Matrix.setIdentityM(viewMatrix, 0);

		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, -3.0f);
		GLES32.glUniformMatrix4fv(modelMatrixUniform_sphere, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform_sphere, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform_sphere, 1, false, perspectiveProjectionMatrix_sphere, 0);

		// Sending Light Related Uniforms
		if (bLight == true)
		{
			GLES32.glUniform1i(lightingEnabledUniform_sphere, 1);

			for (int i = 0; i < 3; i++)
			{
				GLES32.glUniform3fv(laUniform_sphere[i], 1, lights_sphere[i].lightAmbient_sphere, 0);
				GLES32.glUniform3fv(ldUniform_sphere[i], 1, lights_sphere[i].lightDiffuse_sphere, 0);
				GLES32.glUniform3fv(lsUniform_sphere[i], 1, lights_sphere[i].lightSpecular_sphere, 0);
				GLES32.glUniform4fv(lightPositionUniform_sphere[i], 1, lights_sphere[i].lightPosition_sphere, 0);
			}
			GLES32.glUniform3fv(kaUniform_sphere, 1, materialAmbient_sphere, 0);
			GLES32.glUniform3fv(kdUniform_sphere, 1, materialDiffuse_sphere, 0);
			GLES32.glUniform3fv(ksUniform_sphere, 1, materialSpecular_sphere, 0);
			GLES32.glUniform1f(materialShininessUniform_sphere, materialShininess_sphere);
		}
		else
		{
			GLES32.glUniform1i(lightingEnabledUniform_sphere, 0);
		}

		GLES32.glBindVertexArray(vao_Sphere[0]);
	
			GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
			GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements_sphere, GLES32.GL_UNSIGNED_SHORT, 0);

		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);

		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);
	}

	void update_sphere()
	{
		// Code

		lightAngleZero_sphere += 2.0f;
		lightAngleOne_sphere += 2.0f;
		lightAngleTwo_sphere += 2.0f;

		lights_sphere[0].lightPosition_sphere[1] = 10 * (float)Math.sin(lightAngleZero_sphere / 180.0f * 3.14f);
		lights_sphere[0].lightPosition_sphere[2] = 10 * (float)Math.cos(lightAngleZero_sphere / 180.0f * 3.14f);

		lights_sphere[1].lightPosition_sphere[0] = 10 * (float)Math.sin(lightAngleZero_sphere * 3.14f / 180.0f);
		lights_sphere[1].lightPosition_sphere[2] = 10 * (float)Math.cos(lightAngleZero_sphere * 3.14f / 180.0f);

		lights_sphere[2].lightPosition_sphere[0] = 10 * -(float)Math.sin(lightAngleZero_sphere * 3.14f / 180.0f);
		lights_sphere[2].lightPosition_sphere[1] = 10 * (float)Math.cos(lightAngleZero_sphere * 3.14f / 180.0f);

	}

	private void uninitialize(){

		// Code
		uninitialize_sphere();

		//  Deletion And Uninitialization of vbo
		if (vbo_cube_texcoord[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_cube_texcoord, 0);
			vbo_cube_texcoord[0] = 0;
		}

		if (vbo_cube_position[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_cube_position, 0);
			vbo_cube_position[0] = 0;
		}

		// Deletion And Uninitialization of vao
		if (vao_cube[0] > 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_cube, 0);
			vao_cube[0] = 0;
		}

		// Shader uninitialization
		if (shaderProgramObject > 0)
		{
			GLES32.glUseProgram(shaderProgramObject);
			int retVal[] = new int[1];
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_ATTACHED_SHADERS, retVal, 0);
			if (retVal[0] > 0)
			{
				int numAttachedShaders = retVal[0];
				int shaderObjects[] = new int[numAttachedShaders];
				GLES32.glGetAttachedShaders(shaderProgramObject, numAttachedShaders, retVal, 0, shaderObjects, 0);
				for (int i = 0; i < numAttachedShaders; i++)
				{
					GLES32.glDetachShader(shaderProgramObject, shaderObjects[i]);
					GLES32.glDeleteShader(shaderObjects[i]);
					shaderObjects[i] = 0;
				}
			}
			
			GLES32.glUseProgram(0);
			GLES32.glDeleteProgram(shaderProgramObject);
			shaderProgramObject = 0;
		}
	}

	private void uninitialize_sphere(){

		// Code
		//  Deletion And Uninitialization of vbo
		if (vbo_Sphere_Elements[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_Sphere_Elements, 0);
			vbo_Sphere_Elements[0] = 0;
		}

		if (vbo_Sphere_Normals[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_Sphere_Normals, 0);
			vbo_Sphere_Normals[0] = 0;
		}

		if (vbo_Sphere_Position[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_Sphere_Position, 0);
			vbo_Sphere_Position[0] = 0;
		}

		// Deletion And Uninitialization of vao
		if (vao_Sphere[0] > 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_Sphere, 0);
			vao_Sphere[0] = 0;
		}

		// Shader uninitialization
		if (shaderProgramObject_sphere > 0)
		{
			GLES32.glUseProgram(shaderProgramObject_sphere);
			int retVal[] = new int[1];
			GLES32.glGetProgramiv(shaderProgramObject_sphere, GLES32.GL_ATTACHED_SHADERS, retVal, 0);
			if (retVal[0] > 0)
			{
				int numAttachedShaders = retVal[0];
				int shaderObjects[] = new int[numAttachedShaders];
				GLES32.glGetAttachedShaders(shaderProgramObject_sphere, numAttachedShaders, retVal, 0, shaderObjects, 0);
				for (int i = 0; i < numAttachedShaders; i++)
				{
					GLES32.glDetachShader(shaderProgramObject_sphere, shaderObjects[i]);
					GLES32.glDeleteShader(shaderObjects[i]);
					shaderObjects[i] = 0;
				}
			}
			
			GLES32.glUseProgram(0);
			GLES32.glDeleteProgram(shaderProgramObject_sphere);
			shaderProgramObject_sphere = 0;
		}
	}
}
