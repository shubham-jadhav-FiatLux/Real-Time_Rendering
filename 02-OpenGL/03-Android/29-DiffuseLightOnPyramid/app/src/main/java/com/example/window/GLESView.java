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

// Matrix Related Package
import android.opengl.Matrix;

public class GLESView extends GLSurfaceView implements OnDoubleTapListener, OnGestureListener, GLSurfaceView.Renderer {

	private GestureDetector gestureDetector;
	private Context context;

	private int shaderProgramObject;

	private int vao_pyramid[] = new int[1];
	private int vbo_pyramid_position[] = new int[1];
	private int vbo_pyramid_normal[] = new int[1];

	private float anglePyramid = 0.0f;

	private int modelMatrixUniform;
	private int viewMatrixUniform;
	private int projectionMatrixUniform;
	private int ldUniform;
	private int kdUniform;
	private int lightPositionUniform;

	private int lightingEnabledUniform;

	private boolean bLight  = false;
	private float lightDiffuse[] = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
	private float materialDiffuse[] = new float[]{0.5f, 0.5f, 0.5f, 1.0f};
	private float lightPosition[] = new float[]{0.0f, 0.0f, 2.0f, 1.0f};
	
	private float perspectiveProjectionMatrix[] = new float[16]; 

	/*
	private class Light
	{
		float lightAmbient[] = new float[4];
		float lightDiffuse[] = new float[4];
		float lightSpecular[] = new float[4];
		float lightPosition[] = new float[4];
	}

	private Light lights[] = new Light[2];
	*/
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
	
		/*
		doubleTap++;

		if (doubleTap > 1)
			doubleTap = 0;
		*/
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
			"in vec3 a_normal;" +
			"uniform mat4 u_modelMatrix;" +
			"uniform mat4 u_viewMatrix;" +
			"uniform mat4 u_projectionMatrix;" +
			"uniform vec3 u_ld;" +
			"uniform vec3 u_kd;" +
			"uniform vec4 u_lightPosition;" +
			"uniform int u_lightingEnabled;" +
			"out vec3 diffuse_light_color;" +
			"void main(void)" +
			"{" +
			"if (u_lightingEnabled == 1)" +
			"{" +
			"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" +
			"mat3 normalMatrix = mat3(transpose(inverse(u_viewMatrix * u_modelMatrix)));" +
			"vec3 transformedNormals = normalize(normalMatrix * a_normal);" +
			"vec3 lightDirection = normalize(vec3(u_lightPosition - eyeCoordinates));" +
			"diffuse_light_color = u_ld * u_kd * max(dot(lightDirection, transformedNormals), 0.0);" +
			"}" +
			"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;" +
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
			"in vec3 diffuse_light_color;" +
			"uniform highp int u_lightingEnabled;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
			"if (u_lightingEnabled == 1)" +
			"{" +
			"FragColor = vec4(diffuse_light_color, 1.0);" +
			"}" +
			"else" +
			"{" +
			"FragColor = vec4(1.0, 1.0, 1.0, 1.0);" +
			"}" +
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
		GLES32.glBindAttribLocation(shaderProgramObject, MyGLESMacros.SBJ_ATTRIBUTE_NORMAL, "a_normal");
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
	
		modelMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
		viewMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
		projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
		ldUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ld");
		kdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kd");
		lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_lightPosition");
		lightingEnabledUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_lightingEnabled");

		// Declaration Of Vertex Data Arrays
		final float pyramidPosition[] = new float[]
													{
														// front
														0.0f, 1.0f, 0.0f,
														-1.0f, -1.0f, 1.0f,
														1.0f, -1.0f, 1.0f,

														// right
														0.0f, 1.0f, 0.0f,
														1.0f, -1.0f, 1.0f,
														1.0f, -1.0f, -1.0f,

														// back
														0.0f, 1.0f, 0.0f,
														1.0f, -1.0f, -1.0f,
														-1.0f, -1.0f, -1.0f,

														// left
														0.0f, 1.0f, 0.0f,
														-1.0f, -1.0f, -1.0f,
														-1.0f, -1.0f, 1.0f
													};

		final float pyramidNormals[] = new float[]
													{
														0.0f, 0.447214f, 0.894427f,// front-top
														0.0f, 0.447214f, 0.894427f,// front-left
														0.0f, 0.447214f, 0.894427f,// front-right

														0.894427f, 0.447214f, 0.0f, // right-top
														0.894427f, 0.447214f, 0.0f, // right-left
														0.894427f, 0.447214f, 0.0f, // right-right

														0.0f, 0.447214f, -0.894427f, // back-top
														0.0f, 0.447214f, -0.894427f, // back-left
														0.0f, 0.447214f, -0.894427f, // back-right

														-0.894427f, 0.447214f, 0.0f, // left-top
														-0.894427f, 0.447214f, 0.0f, // left-left
														-0.894427f, 0.447214f, 0.0f // left-right
													};

		// vao And vbo Related Code
		GLES32.glGenVertexArrays(1, vao_pyramid, 0);
		GLES32.glBindVertexArray(vao_pyramid[0]);

			// Position
			GLES32.glGenBuffers(1, vbo_pyramid_position, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_pyramid_position[0]);

			ByteBuffer byteBuffer = ByteBuffer.allocateDirect(pyramidPosition.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
			positionBuffer.put(pyramidPosition);
			positionBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidPosition.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_POSITION);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			// Normal
			GLES32.glGenBuffers(1, vbo_pyramid_normal, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_pyramid_normal[0]);

			byteBuffer = ByteBuffer.allocateDirect(pyramidNormals.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer normalBuffer = byteBuffer.asFloatBuffer();
			normalBuffer.put(pyramidNormals);
			normalBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidNormals.length * 4, normalBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_NORMAL);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindVertexArray(0);


		// Here Starts OpenGL Code
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		//  Enable Culling
		//GLES32.glEnable(GLES32.GL_CULL_FACE);

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
	}

	private void resize(int width, int height) {
	
		// Code
		if (height == 0)
		height = 1;

		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix , 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}

	private void display() {
	
		// Code
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		GLES32.glUseProgram(shaderProgramObject);

		// Transformations
		float translationMatrix[] = new float[16];
		Matrix.setIdentityM(translationMatrix, 0);
		float rotationMatrix[] = new float[16];
		Matrix.setIdentityM(rotationMatrix, 0);
		float modelMatrix[] = new float[16];
		Matrix.setIdentityM(modelMatrix, 0);
		float viewMatrix[] = new float[16];
		Matrix.setIdentityM(viewMatrix, 0);

		Matrix.translateM(translationMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, -6.0f);
		Matrix.setRotateM(rotationMatrix, 0, anglePyramid, 0.0f, 1.0f, 0.0f);
		Matrix.multiplyMM(modelMatrix, 0, translationMatrix, 0, rotationMatrix, 0);
		
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);

		// Sending Light Related Uniforms
		if (bLight == true)
		{
			GLES32.glUniform1i(lightingEnabledUniform, 1);

			GLES32.glUniform3fv(ldUniform, 1, lightDiffuse, 0);
			GLES32.glUniform3fv(kdUniform, 1, materialDiffuse, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);
		}
		else
		{
			GLES32.glUniform1i(lightingEnabledUniform, 0);
		}

		GLES32.glBindVertexArray(vao_pyramid[0]);
	
			GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);

		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);

		requestRender();
	}

	private void update(){

		// Code
		anglePyramid = anglePyramid + 1.0f;
		if (anglePyramid >= 360.0f)
			anglePyramid = anglePyramid - 360.0f;
	}

	private void uninitialize(){

		// Code
		//  Deletion And Uninitialization of vbo
		if (vbo_pyramid_normal[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_pyramid_normal, 0);
			vbo_pyramid_normal[0] = 0;
		}

		if (vbo_pyramid_position[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_pyramid_position, 0);
			vbo_pyramid_position[0] = 0;
		}

		// Deletion And Uninitialization of vao
		if (vao_pyramid[0] > 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_pyramid, 0);
			vao_pyramid[0] = 0;
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
}
