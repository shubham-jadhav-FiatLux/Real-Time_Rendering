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

	private int vao[] = new int[1];
	private int vbo_position[] = new int[1];
	private int mvpMatrixUniform;
	
	private float perspectiveProjectionMatrix[] = new float[16]; 

	// Tesselation Shader Related Variables
	private int numberOfSegmentsUniform;
	private int numberOfStripsUniform;
	private int lineColorUniform;
	private int uiNumberOfSegments;
	private boolean flag = false;


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
	
		return true;
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent e) {
	
		return true;
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent e) {
	
		if (uiNumberOfSegments <= 30 && flag == false){
			uiNumberOfSegments++;	
		}	
		else
			flag = true;

		if (uiNumberOfSegments >= 0 && flag == true){
			uiNumberOfSegments--;
		}
		else
			flag = false;
			

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
			"in vec2 a_position;" +
			"void main(void)" +
			"{" +
			"gl_Position = vec4(a_position, 0.0, 1.0);" +
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


		// Tessellation Control Shader
		final String tessellationControlShaderSourceCode = String.format
		(
			"#version 320 es" +
			"\n" +
			"layout(vertices = 4) out;" +
			"uniform int u_numberOfSegments;" +
			"uniform int u_numberOfStrips;" +
			"void main(void)" +
			"{" +
			"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" +
			"gl_TessLevelOuter[0] = float(u_numberOfStrips);" +
			"gl_TessLevelOuter[1] = float(u_numberOfSegments);" +
			"}"
		);

		// Creating Shader Object
		int tessellationControlShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_CONTROL_SHADER);
		// Giving Shader Code To Shader Object 
		GLES32.glShaderSource(tessellationControlShaderObject, tessellationControlShaderSourceCode);
		// Compile The Shader
		GLES32.glCompileShader(tessellationControlShaderObject);
		// Error Checking
		status[0] = 0;
		infoLogLength[0] = 0;
		log = null;
		GLES32.glGetShaderiv(tessellationControlShaderObject, GLES32.GL_COMPILE_STATUS, status, 0);
		if (status[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(tessellationControlShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				log = GLES32.glGetShaderInfoLog(tessellationControlShaderObject);
				System.out.println("SBJ : " + "Tessellation Control Shader Compilation Log: " + log);
				uninitialize();
				System.exit(0);
			}
		}


		// Tessellation Evaluation Shader
		final String tessellationEvaluationShaderSourceCode = String.format
		(
			"#version 320 es" +
			"\n" +
			"layout(isolines) in;" +
			"uniform mat4 u_mvpMatrix;" +
			"void main(void)" +
			"{" +
			"vec3 p0 = gl_in[0].gl_Position.xyz;" +
			"vec3 p1 = gl_in[1].gl_Position.xyz;" +
			"vec3 p2 = gl_in[2].gl_Position.xyz;" +
			"vec3 p3 = gl_in[3].gl_Position.xyz;" +
			"vec3 p = p0*(1-gl_TessCoord.x)*(1-gl_TessCoord.x)*(1-gl_TessCoord.x) + p1*3*gl_TessCoord.x*(1-gl_TessCoord.x)*(1-gl_TessCoord.x) + p2*3*gl_TessCoord.x*gl_TessCoord.x*(1-gl_TessCoord.x) + p3*gl_TessCoord.x*gl_TessCoord.x*gl_TessCoord.x;" +
			"gl_Position = u_mvpMatrix * vec4(p, 1.0);" +
			"}"
		);

		// Creating Shader Object
		int tessellationEvaluationShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_EVALUATION_SHADER);
		// Giving Shader Code To Shader Object 
		GLES32.glShaderSource(tessellationEvaluationShaderObject, tessellationEvaluationShaderSourceCode);
		// Compile The Shader
		GLES32.glCompileShader(tessellationEvaluationShaderObject);
		// Error Checking
		status[0] = 0;
		infoLogLength[0] = 0;
		log = null;
		GLES32.glGetShaderiv(tessellationEvaluationShaderObject, GLES32.GL_COMPILE_STATUS, status, 0);
		if (status[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(tessellationEvaluationShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
			if (infoLogLength[0] > 0)
			{
				log = GLES32.glGetShaderInfoLog(tessellationEvaluationShaderObject);
				System.out.println("SBJ : " + "Tessellation Evaluation Shader Compilation Log: " + log);
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
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
			"FragColor = vec4(1.0, 1.0, 0.0, 1.0);" +
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
		GLES32.glAttachShader(shaderProgramObject, tessellationControlShaderObject);
		GLES32.glAttachShader(shaderProgramObject, tessellationEvaluationShaderObject);
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
		GLES32.glBindAttribLocation(shaderProgramObject, MyGLESMacros.SBJ_ATTRIBUTE_POSITION, "a_position");
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
		numberOfSegmentsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_numberOfSegments");
		numberOfStripsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_numberOfStrips");
		lineColorUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_lineColor");

		// Declaration Of Vertex Data Arrays
		final float vertices[] = new float[]
													{
														-1.0f, -1.0f,
														-0.5f, 1.0f,
														0.5f, -1.0f,
														1.0f, 1.0f
													};

		// vao And vbo Related Code
		GLES32.glGenVertexArrays(1, vao, 0);
		GLES32.glBindVertexArray(vao[0]);

			// Position
			GLES32.glGenBuffers(1, vbo_position, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

			ByteBuffer byteBuffer = ByteBuffer.allocateDirect(vertices.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
			positionBuffer.put(vertices);
			positionBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, vertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_POSITION, 2, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_POSITION);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		
		GLES32.glBindVertexArray(0);


		// Here Starts OpenGL Code
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		//  Enable Culling
		//GLES32.glEnable(GLES32.GL_CULL_FACE);

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		uiNumberOfSegments = 1;			// Our Line Have One Curve

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
		float modelViewMatrix[] = new float[16];
		Matrix.setIdentityM(modelViewMatrix, 0);
		float modelViewProjectionMatrix[] = new float[16];
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);

		Matrix.translateM(modelViewMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, -4.0f);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
		GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

		GLES32.glUniform1i(numberOfSegmentsUniform, uiNumberOfSegments);

		GLES32.glUniform1i(numberOfStripsUniform, 1);

	
		//float colorMatrix[] = new float[]{1.0f,1.0f,1.0f,1.0f};
		//GLES32.glUniform4fv(lineColorUniform, 1, colorMatrix, 0);
		/*
		if (uiNumberOfSegments < 30)
		{
			colorMatrix[0] = 1.0f;
			colorMatrix[1] = 1.0f;
			colorMatrix[2] = 0.0f;
			colorMatrix[3] = 1.0f;
			
		}
		else
		{
			colorMatrix[0] = 0.5f;
			colorMatrix[1] = 1.0f;
			colorMatrix[2] = 0.0f;
			colorMatrix[3] = 1.0f;
			GLES32.glUniform4fv(lineColorUniform, 1, colorMatrix, 0);
		}
		*/
		
		GLES32.glBindVertexArray(vao[0]);
	
			GLES32.glPatchParameteri(GLES32.GL_PATCH_VERTICES, 4);
			GLES32.glDrawArrays(GLES32.GL_PATCHES, 0, 4);

		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);

		requestRender();
	}

	private void uninitialize(){

		// Code
		//  Deletion And Uninitialization of vbo

		if (vbo_position[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_position, 0);
			vbo_position[0] = 0;
		}

		// Deletion And Uninitialization of vao
		if (vao[0] > 0)
		{
			GLES32.glDeleteVertexArrays(1, vao, 0);
			vao[0] = 0;
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
