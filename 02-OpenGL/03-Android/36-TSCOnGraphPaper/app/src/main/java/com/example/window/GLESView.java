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

	private boolean gbToggleSquare = false;
	private boolean gbToggleCircle = false;
	private boolean gbToggleTriangle = false;
	private boolean gbDoubleClick = false;

	private int circleVerticesSize;
	private int graphPaperVerticesSize;
	private int countSingleTap = 0;

	private int shaderProgramObject;

	private int vao_triangle[] = new int[1];
	private int vbo_triangle_position[] = new int[1];
	private int vbo_triangle_color[] = new int[1];

	private int vao_square[] = new int[1];
	private int vbo_square[] = new int[1];

	private int vao_circle[] = new int[1];
	private int vbo_circle[] = new int[1];

	private int vao_graph[] = new int[1];
	private int vbo_graph[] = new int[1];

	private int vao_graphAxis[] = new int[1];
	private int vbo_graphAxis[] = new int[1];

	//private int vbo_color[] = new int[1];
	private int mvpMatrixUniform;
	
	private float perspectiveProjectionMatrix[] = new float[16]; 


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
	
		if (gbDoubleClick == false)
			gbDoubleClick = true;
		else
			gbDoubleClick = false;

		return true;
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent e) {
	
		countSingleTap++;
		if (countSingleTap == 1){
			gbToggleTriangle = true;
			gbToggleSquare = false;
			gbToggleCircle = false;
		}
		else if (countSingleTap == 2){

			gbToggleTriangle = false;
			gbToggleSquare = true;
			gbToggleCircle = false;
		}
		else if (countSingleTap == 3){

			gbToggleTriangle = false;
			gbToggleSquare = false;
			gbToggleCircle = true;
		}
		else{
			countSingleTap = 0;
			gbToggleTriangle = false;
			gbToggleSquare = false;
			gbToggleCircle = false;
		}

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
			"in vec4 a_color;" +
			"uniform mat4 u_mvpMatrix;" +
			"out vec4 a_color_out;" +
			"void main(void)" +
			"{" +
			"gl_Position = u_mvpMatrix * a_position;" +
			"a_color_out = a_color;" +
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
			"in vec4 a_color_out;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
			"FragColor = a_color_out;" +
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
		GLES32.glBindAttribLocation(shaderProgramObject, MyGLESMacros.SBJ_ATTRIBUTE_COLOR, "a_color");
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
		
		
		final float trianglePosition[] = new float[]
													{
														0.0f, 0.5f,
														0.5f * (float)Math.cos(225 / 180.f * Math.PI), 0.5f * (float)Math.sin(225 / 180.f * Math.PI),

														0.5f * (float)Math.cos(225 / 180.f * Math.PI), 0.5f * (float)Math.sin(225 / 180.f * Math.PI),
														0.5f * (float)Math.cos(315 / 180.f * Math.PI), 0.5f * (float)Math.sin(315 / 180.f * Math.PI),

														0.5f * (float)Math.cos(315 / 180.f * Math.PI), 0.5f * (float)Math.sin(315 / 180.f * Math.PI),
														0.0f, 0.5f
													};

		final float triangleColor[] = new float[]
													{
														1.0f, 1.0f, 0.0f,
														1.0f, 1.0f, 0.0f,

														1.0f, 1.0f, 0.0f,
														1.0f, 1.0f, 0.0f,

														1.0f, 1.0f, 0.0f,
														1.0f, 1.0f, 0.0f,
													};

		final float squareVertices[] = new float[]
													{
														0.5f, 0.5f,
														-0.5f, 0.5f,

														-0.5f, 0.5f,
														-0.5f, -0.5f,

														-0.5f, -0.5f,
														0.5f, -0.5f,

														0.5f, -0.5f,
														0.5f, 0.5f,
													};

		final float squareColor[] = new float[]
													{
														1.0f, 1.0f, 0.0f,
														1.0f, 1.0f, 0.0f,

														1.0f, 1.0f, 0.0f,
														1.0f, 1.0f, 0.0f,

														1.0f, 1.0f, 0.0f,
														1.0f, 1.0f, 0.0f,

														1.0f, 1.0f, 0.0f,
														1.0f, 1.0f, 0.0f,
													};

		final float graphPaperVertices[] = new float[]
													{
														-1.000000f, 1.000000f,
														1.000000f, 1.000000f,
														-1.000000f, 0.950000f,
														1.000000f, 0.950000f,
														-1.000000f, 0.900000f,
														1.000000f, 0.900000f,
														-1.000000f, 0.850000f,
														1.000000f, 0.850000f,
														-1.000000f, 0.800000f,
														1.000000f, 0.800000f,
														-1.000000f, 0.750000f,
														1.000000f, 0.750000f,
														-1.000000f, 0.700000f,
														1.000000f, 0.700000f,
														-1.000000f, 0.650000f,
														1.000000f, 0.650000f,
														-1.000000f, 0.600000f,
														1.000000f, 0.600000f,
														-1.000000f, 0.550000f,
														1.000000f, 0.550000f,
														-1.000000f, 0.500000f,
														1.000000f, 0.500000f,
														-1.000000f, 0.450000f,
														1.000000f, 0.450000f,
														-1.000000f, 0.400000f,
														1.000000f, 0.400000f,
														-1.000000f, 0.350000f,
														1.000000f, 0.350000f,
														-1.000000f, 0.300000f,
														1.000000f, 0.300000f,
														-1.000000f, 0.250000f,
														1.000000f, 0.250000f,
														-1.000000f, 0.200000f,
														1.000000f, 0.200000f,
														-1.000000f, 0.150000f,
														1.000000f, 0.150000f,
														-1.000000f, 0.100000f,
														1.000000f, 0.100000f,
														-1.000000f, 0.050000f,
														1.000000f, 0.050000f,


														-1.000000f, -1.000000f,
														1.000000f, -1.000000f,
														-1.000000f, -0.950000f,
														1.000000f, -0.950000f,
														-1.000000f, -0.900000f,
														1.000000f, -0.900000f,
														-1.000000f, -0.850000f,
														1.000000f, -0.850000f,
														-1.000000f, -0.800000f,
														1.000000f, -0.800000f,
														-1.000000f, -0.750000f,
														1.000000f, -0.750000f,
														-1.000000f, -0.700000f,
														1.000000f, -0.700000f,
														-1.000000f, -0.650000f,
														1.000000f, -0.650000f,
														-1.000000f, -0.600000f,
														1.000000f, -0.600000f,
														-1.000000f, -0.550000f,
														1.000000f, -0.550000f,
														-1.000000f, -0.500000f,
														1.000000f, -0.500000f,
														-1.000000f, -0.450000f,
														1.000000f, -0.450000f,
														-1.000000f, -0.400000f,
														1.000000f, -0.400000f,
														-1.000000f, -0.350000f,
														1.000000f, -0.350000f,
														-1.000000f, -0.300000f,
														1.000000f, -0.300000f,
														-1.000000f, -0.250000f,
														1.000000f, -0.250000f,
														-1.000000f, -0.200000f,
														1.000000f, -0.200000f,
														-1.000000f, -0.150000f,
														1.000000f, -0.150000f,
														-1.000000f, -0.100000f,
														1.000000f, -0.100000f,
														-1.000000f, -0.050000f,
														1.000000f, -0.050000f,


														-1.000000f, 1.000000f,
														-1.000000f, -1.000000f,
														-0.950000f, 1.000000f,
														-0.950000f, -1.000000f,
														-0.900000f, 1.000000f,
														-0.900000f, -1.000000f,
														-0.850000f, 1.000000f,
														-0.850000f, -1.000000f,
														-0.800000f, 1.000000f,
														-0.800000f, -1.000000f,
														-0.750000f, 1.000000f,
														-0.750000f, -1.000000f,
														-0.700000f, 1.000000f,
														-0.700000f, -1.000000f,
														-0.650000f, 1.000000f,
														-0.650000f, -1.000000f,
														-0.600000f, 1.000000f,
														-0.600000f, -1.000000f,
														-0.550000f, 1.000000f,
														-0.550000f, -1.000000f,
														-0.500000f, 1.000000f,
														-0.500000f, -1.000000f,
														-0.450000f, 1.000000f,
														-0.450000f, -1.000000f,
														-0.400000f, 1.000000f,
														-0.400000f, -1.000000f,
														-0.350000f, 1.000000f,
														-0.350000f, -1.000000f,
														-0.300000f, 1.000000f,
														-0.300000f, -1.000000f,
														-0.250000f, 1.000000f,
														-0.250000f, -1.000000f,
														-0.200000f, 1.000000f,
														-0.200000f, -1.000000f,
														-0.150000f, 1.000000f,
														-0.150000f, -1.000000f,
														-0.100000f, 1.000000f,
														-0.100000f, -1.000000f,
														-0.050000f, 1.000000f,
														-0.050000f, -1.000000f,


														1.000000f, 1.000000f,
														1.000000f, -1.000000f,
														0.950000f, 1.000000f,
														0.950000f, -1.000000f,
														0.900000f, 1.000000f,
														0.900000f, -1.000000f,
														0.850000f, 1.000000f,
														0.850000f, -1.000000f,
														0.800000f, 1.000000f,
														0.800000f, -1.000000f,
														0.750000f, 1.000000f,
														0.750000f, -1.000000f,
														0.700000f, 1.000000f,
														0.700000f, -1.000000f,
														0.650000f, 1.000000f,
														0.650000f, -1.000000f,
														0.600000f, 1.000000f,
														0.600000f, -1.000000f,
														0.550000f, 1.000000f,
														0.550000f, -1.000000f,
														0.500000f, 1.000000f,
														0.500000f, -1.000000f,
														0.450000f, 1.000000f,
														0.450000f, -1.000000f,
														0.400000f, 1.000000f,
														0.400000f, -1.000000f,
														0.350000f, 1.000000f,
														0.350000f, -1.000000f,
														0.300000f, 1.000000f,
														0.300000f, -1.000000f,
														0.250000f, 1.000000f,
														0.250000f, -1.000000f,
														0.200000f, 1.000000f,
														0.200000f, -1.000000f,
														0.150000f, 1.000000f,
														0.150000f, -1.000000f,
														0.100000f, 1.000000f,
														0.100000f, -1.000000f,
														0.050000f, 1.000000f,
														0.050000f, -1.000000f,
													};

		graphPaperVerticesSize = graphPaperVertices.length;

		final float graphPaperColor[] = new float[]
													{
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,


														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,


														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
														0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,


															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
															0.0f, 0.0f, 1.0f,
													};


		final float graphPaperAxisVertices[] = new float[]
													{
														-1.0f, 0.0f,
														1.0f, 0.0f,

														0.0f, 1.0f,
														0.0f, -1.0f
													};

		final float graphPaperAxisColor[] = new float[]
													{
														1.0f, 0.0f, 0.0f,
														1.0f, 0.0f, 0.0f,

														0.0f, 1.0f, 0.0f,
														0.0f, 1.0f, 0.0f
													};

		final float circleVertices[] = new float[]
													{
														0.500000f, 0.000000f,
														0.499981f, 0.004363f,
														0.499924f, 0.008726f,
														0.499829f, 0.013088f,
														0.499695f, 0.017450f,
														0.499524f, 0.021810f,
														0.499315f, 0.026168f,
														0.499067f, 0.030524f,
														0.498782f, 0.034878f,
														0.498459f, 0.039230f,
														0.498097f, 0.043578f,
														0.497698f, 0.047923f,
														0.497261f, 0.052264f,
														0.496786f, 0.056602f,
														0.496273f, 0.060935f,
														0.495722f, 0.065263f,
														0.495134f, 0.069587f,
														0.494508f, 0.073905f,
														0.493844f, 0.078217f,
														0.493143f, 0.082524f,
														0.492404f, 0.086824f,
														0.491627f, 0.091118f,
														0.490814f, 0.095404f,
														0.489962f, 0.099684f,
														0.489074f, 0.103956f,
														0.488148f, 0.108220f,
														0.487185f, 0.112476f,
														0.486185f, 0.116723f,
														0.485148f, 0.120961f,
														0.484074f, 0.125190f,
														0.482963f, 0.129410f,
														0.481815f, 0.133619f,
														0.480631f, 0.137819f,
														0.479410f, 0.142008f,
														0.478152f, 0.146186f,
														0.476858f, 0.150353f,
														0.475528f, 0.154508f,
														0.474162f, 0.158652f,
														0.472759f, 0.162784f,
														0.471321f, 0.166903f,
														0.469846f, 0.171010f,
														0.468336f, 0.175104f,
														0.466790f, 0.179184f,
														0.465209f, 0.183251f,
														0.463592f, 0.187303f,
														0.461940f, 0.191342f,
														0.460252f, 0.195366f,
														0.458530f, 0.199375f,
														0.456773f, 0.203368f,
														0.454981f, 0.207347f,
														0.453154f, 0.211309f,
														0.451293f, 0.215256f,
														0.449397f, 0.219186f,
														0.447467f, 0.223099f,
														0.445503f, 0.226995f,
														0.443505f, 0.230874f,
														0.441474f, 0.234736f,
														0.439409f, 0.238579f,
														0.437310f, 0.242405f,
														0.435178f, 0.246212f,
														0.433013f, 0.250000f,
														0.430815f, 0.253769f,
														0.428584f, 0.257519f,
														0.426320f, 0.261249f,
														0.424024f, 0.264960f,
														0.421696f, 0.268650f,
														0.419335f, 0.272320f,
														0.416943f, 0.275968f,
														0.414519f, 0.279596f,
														0.412063f, 0.283203f,
														0.409576f, 0.286788f,
														0.407058f, 0.290351f,
														0.404508f, 0.293893f,
														0.401928f, 0.297411f,
														0.399318f, 0.300908f,
														0.396677f, 0.304381f,
														0.394005f, 0.307831f,
														0.391304f, 0.311257f,
														0.388573f, 0.314660f,
														0.385812f, 0.318039f,
														0.383022f, 0.321394f,
														0.380203f, 0.324724f,
														0.377355f, 0.328030f,
														0.374478f, 0.331310f,
														0.371572f, 0.334565f,
														0.368639f, 0.337795f,
														0.365677f, 0.340999f,
														0.362687f, 0.344177f,
														0.359670f, 0.347329f,
														0.356625f, 0.350455f,
														0.353553f, 0.353553f,
														0.350455f, 0.356625f,
														0.347329f, 0.359670f,
														0.344177f, 0.362687f,
														0.340999f, 0.365677f,
														0.337795f, 0.368639f,
														0.334565f, 0.371572f,
														0.331310f, 0.374478f,
														0.328030f, 0.377355f,
														0.324724f, 0.380203f,
														0.321394f, 0.383022f,
														0.318039f, 0.385812f,
														0.314660f, 0.388573f,
														0.311257f, 0.391304f,
														0.307831f, 0.394005f,
														0.304381f, 0.396677f,
														0.300908f, 0.399318f,
														0.297411f, 0.401928f,
														0.293893f, 0.404509f,
														0.290351f, 0.407058f,
														0.286788f, 0.409576f,
														0.283203f, 0.412063f,
														0.279596f, 0.414519f,
														0.275969f, 0.416943f,
														0.272320f, 0.419335f,
														0.268650f, 0.421696f,
														0.264960f, 0.424024f,
														0.261249f, 0.426320f,
														0.257519f, 0.428584f,
														0.253769f, 0.430815f,
														0.250000f, 0.433013f,
														0.246212f, 0.435178f,
														0.242405f, 0.437310f,
														0.238579f, 0.439409f,
														0.234736f, 0.441474f,
														0.230874f, 0.443505f,
														0.226995f, 0.445503f,
														0.223099f, 0.447467f,
														0.219186f, 0.449397f,
														0.215256f, 0.451293f,
														0.211309f, 0.453154f,
														0.207347f, 0.454981f,
														0.203368f, 0.456773f,
														0.199375f, 0.458530f,
														0.195366f, 0.460252f,
														0.191342f, 0.461940f,
														0.187303f, 0.463592f,
														0.183251f, 0.465209f,
														0.179184f, 0.466790f,
														0.175104f, 0.468336f,
														0.171010f, 0.469846f,
														0.166903f, 0.471321f,
														0.162784f, 0.472759f,
														0.158652f, 0.474162f,
														0.154508f, 0.475528f,
														0.150353f, 0.476858f,
														0.146186f, 0.478152f,
														0.142008f, 0.479410f,
														0.137819f, 0.480631f,
														0.133619f, 0.481815f,
														0.129410f, 0.482963f,
														0.125190f, 0.484074f,
														0.120961f, 0.485148f,
														0.116723f, 0.486185f,
														0.112476f, 0.487185f,
														0.108220f, 0.488148f,
														0.103956f, 0.489074f,
														0.099684f, 0.489962f,
														0.095405f, 0.490814f,
														0.091118f, 0.491627f,
														0.086824f, 0.492404f,
														0.082524f, 0.493143f,
														0.078217f, 0.493844f,
														0.073905f, 0.494508f,
														0.069587f, 0.495134f,
														0.065263f, 0.495722f,
														0.060935f, 0.496273f,
														0.056602f, 0.496786f,
														0.052264f, 0.497261f,
														0.047923f, 0.497698f,
														0.043578f, 0.498097f,
														0.039230f, 0.498459f,
														0.034878f, 0.498782f,
														0.030524f, 0.499067f,
														0.026168f, 0.499315f,
														0.021810f, 0.499524f,
														0.017450f, 0.499695f,
														0.013088f, 0.499829f,
														0.008726f, 0.499924f,
														0.004363f, 0.499981f,
														0.000000f, 0.500000f,
														- 0.004363f, 0.49998f,
														- 0.008726f, 0.499924f,
														- 0.013088f, 0.499829f,
														- 0.017450f, 0.499695f,
														- 0.021810f, 0.499524f,
														- 0.026168f, 0.499315f,
														- 0.030524f, 0.499067f,
														- 0.034878f, 0.498782f,
														- 0.039230f, 0.498459f,
														- 0.043578f, 0.498097f,
														- 0.047923f, 0.497698f,
														- 0.052264f, 0.497261f,
														- 0.056602f, 0.496786f,
														- 0.060935f, 0.496273f,
														- 0.065263f, 0.495722f,
														- 0.069587f, 0.495134f,
														- 0.073905f, 0.494508f,
														- 0.078217f, 0.493844f,
														- 0.082524f, 0.493143f,
														- 0.086824f, 0.492404f,
														- 0.091118f, 0.491627f,
														- 0.095404f, 0.490814f,
														- 0.099684f, 0.489962f,
														- 0.103956f, 0.489074f,
														- 0.108220f, 0.488148f,
														- 0.112476f, 0.487185f,
														- 0.116723f, 0.486185f,
														- 0.120961f, 0.485148f,
														- 0.125190f, 0.484074f,
														- 0.129409f, 0.482963f,
														- 0.133619f, 0.481815f,
														- 0.137819f, 0.480631f,
														- 0.142008f, 0.479410f,
														- 0.146186f, 0.478152f,
														- 0.150353f, 0.476858f,
														- 0.154509f, 0.475528f,
														- 0.158652f, 0.474162f,
														- 0.162784f, 0.472759f,
														- 0.166903f, 0.471321f,
														- 0.171010f, 0.469846f,
														- 0.175104f, 0.468336f,
														- 0.179184f, 0.466790f,
														- 0.183251f, 0.465209f,
														- 0.187303f, 0.463592f,
														- 0.191342f, 0.461940f,
														- 0.195366f, 0.460252f,
														- 0.199375f, 0.458530f,
														- 0.203368f, 0.456773f,
														- 0.207347f, 0.454981f,
														- 0.211309f, 0.453154f,
														- 0.215256f, 0.451293f,
														- 0.219186f, 0.449397f,
														- 0.223099f, 0.447467f,
														- 0.226995f, 0.445503f,
														- 0.230874f, 0.443505f,
														- 0.234736f, 0.441474f,
														- 0.238579f, 0.439409f,
														- 0.242405f, 0.437310f,
														- 0.246212f, 0.435178f,
														- 0.250000f, 0.433013f,
														- 0.253769f, 0.430815f,
														- 0.257519f, 0.428584f,
														- 0.261249f, 0.426320f,
														- 0.264960f, 0.424024f,
														- 0.268650f, 0.421696f,
														- 0.272320f, 0.419335f,
														- 0.275968f, 0.416943f,
														- 0.279596f, 0.414519f,
														- 0.283203f, 0.412063f,
														- 0.286788f, 0.409576f,
														- 0.290351f, 0.407058f,
														- 0.293893f, 0.404509f,
														- 0.297411f, 0.401928f,
														- 0.300908f, 0.399318f,
														- 0.304381f, 0.396677f,
														- 0.307831f, 0.394005f,
														- 0.311257f, 0.391304f,
														- 0.314660f, 0.388573f,
														- 0.318039f, 0.385812f,
														- 0.321394f, 0.383022f,
														- 0.324724f, 0.380203f,
														- 0.328030f, 0.377355f,
														- 0.331310f, 0.374478f,
														- 0.334565f, 0.371572f,
														- 0.337795f, 0.368639f,
														- 0.340999f, 0.365677f,
														- 0.344177f, 0.362687f,
														- 0.347329f, 0.359670f,
														- 0.350455f, 0.356625f,
														- 0.353553f, 0.353553f,
														- 0.356625f, 0.350455f,
														- 0.359670f, 0.347329f,
														- 0.362687f, 0.344177f,
														- 0.365677f, 0.340999f,
														- 0.368639f, 0.337795f,
														- 0.371572f, 0.334565f,
														- 0.374478f, 0.331310f,
														- 0.377355f, 0.328030f,
														- 0.380203f, 0.324724f,
														- 0.383022f, 0.321394f,
														- 0.385812f, 0.318039f,
														- 0.388573f, 0.314660f,
														- 0.391304f, 0.311257f,
														- 0.394005f, 0.307831f,
														- 0.396677f, 0.304381f,
														- 0.399318f, 0.300908f,
														- 0.401928f, 0.297411f,
														- 0.404509f, 0.293893f,
														- 0.407058f, 0.290351f,
														- 0.409576f, 0.286788f,
														- 0.412063f, 0.283203f,
														- 0.414519f, 0.279596f,
														- 0.416943f, 0.275968f,
														- 0.419335f, 0.272320f,
														- 0.421696f, 0.268650f,
														- 0.424024f, 0.264960f,
														- 0.426320f, 0.261249f,
														- 0.428584f, 0.257519f,
														- 0.430815f, 0.253769f,
														- 0.433013f, 0.250000f,
														- 0.435178f, 0.246212f,
														- 0.437310f, 0.242405f,
														- 0.439409f, 0.238579f,
														- 0.441474f, 0.234736f,
														- 0.443505f, 0.230874f,
														- 0.445503f, 0.226995f,
														- 0.447467f, 0.223099f,
														- 0.449397f, 0.219186f,
														- 0.451293f, 0.215256f,
														- 0.453154f, 0.211309f,
														- 0.454981f, 0.207347f,
														- 0.456773f, 0.203368f,
														- 0.458530f, 0.199375f,
														- 0.460252f, 0.195366f,
														- 0.461940f, 0.191342f,
														- 0.463592f, 0.187303f,
														- 0.465209f, 0.183251f,
														- 0.466790f, 0.179184f,
														- 0.468336f, 0.175104f,
														- 0.469846f, 0.171010f,
														- 0.471321f, 0.166903f,
														- 0.472759f, 0.162784f,
														- 0.474162f, 0.158652f,
														- 0.475528f, 0.154509f,
														- 0.476858f, 0.150353f,
														- 0.478152f, 0.146186f,
														- 0.479410f, 0.142008f,
														- 0.480631f, 0.137819f,
														- 0.481815f, 0.133619f,
														- 0.482963f, 0.129409f,
														- 0.484074f, 0.125190f,
														- 0.485148f, 0.120961f,
														- 0.486185f, 0.116723f,
														- 0.487185f, 0.112476f,
														- 0.488148f, 0.108220f,
														- 0.489074f, 0.103956f,
														- 0.489962f, 0.099684f,
														- 0.490814f, 0.095404f,
														- 0.491627f, 0.091118f,
														- 0.492404f, 0.086824f,
														- 0.493143f, 0.082524f,
														- 0.493844f, 0.078217f,
														- 0.494508f, 0.073905f,
														- 0.495134f, 0.069587f,
														- 0.495722f, 0.065263f,
														- 0.496273f, 0.060935f,
														- 0.496786f, 0.05660f,
														- 0.497261f, 0.052264f,
														- 0.497698f, 0.047923f,
														- 0.498097f, 0.043578f,
														- 0.498459f, 0.039230f,
														- 0.498782f, 0.034878f,
														- 0.499067f, 0.030524f,
														- 0.499315f, 0.026168f,
														- 0.499524f, 0.021810f,
														- 0.499695f, 0.017450f,
														- 0.499829f, 0.013088f,
														- 0.499924f, 0.008726f,
														- 0.499981f, 0.004363f,
														- 0.500000f, 0.000000f,
														- 0.499981f, -0.004363f,
														- 0.499924f, -0.008726f,
														- 0.499829f, -0.013088f,
														- 0.499695f, -0.017450f,
														- 0.499524f, -0.021810f,
														- 0.499315f, -0.026168f,
														- 0.499067f, -0.030524f,
														- 0.498782f, -0.034878f,
														- 0.498459f, -0.039230f,
														- 0.498097f, -0.043578f,
														- 0.497698f, -0.047923f,
														- 0.497261f, -0.052264f,
														- 0.496786f, -0.056602f,
														- 0.496273f, -0.060935f,
														- 0.495722f, -0.065263f,
														- 0.495134f, -0.069587f,
														- 0.494508f, -0.073905f,
														- 0.493844f, -0.078217f,
														- 0.493143f, -0.082524f,
														- 0.492404f, -0.086824f,
														- 0.491627f, -0.091118f,
														- 0.490814f, -0.095404f,
														- 0.489962f, -0.099684f,
														- 0.489074f, -0.103956f,
														- 0.488148f, -0.108220f,
														- 0.487185f, -0.112476f,
														- 0.486185f, -0.116723f,
														- 0.485148f, -0.120961f,
														- 0.484074f, -0.125190f,
														- 0.482963f, -0.129410f,
														- 0.481815f, -0.133619f,
														- 0.480631f, -0.137819f,
														- 0.479410f, -0.142008f,
														- 0.478152f, -0.146186f,
														- 0.476858f, -0.150353f,
														- 0.475528f, -0.154509f,
														- 0.474162f, -0.158652f,
														- 0.472759f, -0.162784f,
														- 0.471321f, -0.166903f,
														- 0.469846f, -0.171010f,
														- 0.468336f, -0.175104f,
														- 0.466790f, -0.179184f,
														- 0.465209f, -0.183251f,
														- 0.463592f, -0.187303f,
														- 0.461940f, -0.191342f,
														- 0.460252f, -0.195366f,
														- 0.458530f, -0.199374f,
														- 0.456773f, -0.203368f,
														- 0.454981f, -0.207347f,
														- 0.453154f, -0.211309f,
														- 0.451293f, -0.215256f,
														- 0.449397f, -0.219186f,
														- 0.447467f, -0.223099f,
														- 0.445503f, -0.226995f,
														- 0.443505f, -0.230874f,
														- 0.441474f, -0.234736f,
														- 0.439409f, -0.238579f,
														- 0.437310f, -0.242405f,
														- 0.435178f, -0.246212f,
														- 0.433013f, -0.250000f,
														- 0.430815f, -0.253769f,
														- 0.428584f, -0.257519f,
														- 0.426320f, -0.261249f,
														- 0.424024f, -0.264960f,
														- 0.421696f, -0.268650f,
														- 0.419335f, -0.272319f,
														- 0.416943f, -0.275968f,
														- 0.414519f, -0.279596f,
														- 0.412063f, -0.283203f,
														- 0.409576f, -0.286788f,
														- 0.407058f, -0.290351f,
														- 0.404508f, -0.293893f,
														- 0.401928f, -0.297411f,
														- 0.399318f, -0.300908f,
														- 0.396677f, -0.304381f,
														- 0.394005f, -0.307831f,
														- 0.391304f, -0.311257f,
														- 0.388573f, -0.314660f,
														- 0.385812f, -0.318039f,
														- 0.383022f, -0.321394f,
														- 0.380203f, -0.324724f,
														- 0.377355f, -0.328029f,
														- 0.374478f, -0.331310f,
														- 0.371572f, -0.334565f,
														- 0.368639f, -0.337795f,
														- 0.365677f, -0.340999f,
														- 0.362687f, -0.344177f,
														- 0.359670f, -0.347329f,
														- 0.356625f, -0.350455f,
														- 0.353553f, -0.353553f,
														- 0.350455f, -0.356625f,
														- 0.347329f, -0.359670f,
														- 0.344177f, -0.362687f,
														- 0.340999f, -0.365677f,
														- 0.337795f, -0.368639f,
														- 0.334565f, -0.371572f,
														- 0.331310f, -0.374478f,
														- 0.328029f, -0.377355f,
														- 0.324724f, -0.380203f,
														- 0.321394f, -0.383022f,
														- 0.318039f, -0.385812f,
														- 0.314660f, -0.388573f,
														- 0.311257f, -0.391304f,
														- 0.307831f, -0.394005f,
														- 0.304381f, -0.396677f,
														- 0.300908f, -0.399318f,
														- 0.297411f, -0.401928f,
														- 0.293893f, -0.404508f,
														- 0.290351f, -0.407058f,
														- 0.286788f, -0.409576f,
														- 0.283203f, -0.412063f,
														- 0.279596f, -0.414519f,
														- 0.275968f, -0.416943f,
														- 0.272319f, -0.419335f,
														- 0.268650f, -0.421696f,
														- 0.264960f, -0.424024f,
														- 0.261249f, -0.426320f,
														- 0.257519f, -0.428584f,
														- 0.253769f, -0.430815f,
														- 0.250000f, -0.433013f,
														- 0.246212f, -0.435178f,
														- 0.242405f, -0.437310f,
														- 0.238579f, -0.439409f,
														- 0.234736f, -0.441474f,
														- 0.230874f, -0.443505f,
														- 0.226995f, -0.445503f,
														- 0.223099f, -0.447467f,
														- 0.219186f, -0.449397f,
														- 0.215256f, -0.451293f,
														- 0.211309f, -0.453154f,
														- 0.207347f, -0.454981f,
														- 0.203368f, -0.456773f,
														- 0.199374f, -0.458530f,
														- 0.195366f, -0.460252f,
														- 0.191342f, -0.461940f,
														- 0.187303f, -0.463592f,
														- 0.183251f, -0.465209f,
														- 0.179184f, -0.466790f,
														- 0.175104f, -0.468336f,
														- 0.171010f, -0.469846f,
														- 0.166903f, -0.471321f,
														- 0.162784f, -0.472759f,
														- 0.158652f, -0.474162f,
														- 0.154509f, -0.475528f,
														- 0.150353f, -0.476858f,
														- 0.146186f, -0.478152f,
														- 0.142008f, -0.479410f,
														- 0.137819f, -0.480631f,
														- 0.133619f, -0.481815f,
														- 0.129410f, -0.482963f,
														- 0.125190f, -0.484074f,
														- 0.120961f, -0.485148f,
														- 0.116723f, -0.486185f,
														- 0.112476f, -0.487185f,
														- 0.108220f, -0.488148f,
														- 0.103956f, -0.489074f,
														- 0.099684f, -0.489962f,
														- 0.095404f, -0.490814f,
														- 0.091118f, -0.491627f,
														- 0.086824f, -0.492404f,
														- 0.082524f, -0.493143f,
														- 0.078217f, -0.493844f,
														- 0.073905f, -0.494508f,
														- 0.069587f, -0.495134f,
														- 0.065263f, -0.495722f,
														- 0.060935f, -0.496273f,
														- 0.056602f, -0.496786f,
														- 0.052264f, -0.497261f,
														- 0.047923f, -0.497698f,
														- 0.043578f, -0.498097f,
														- 0.039230f, -0.498459f,
														- 0.034878f, -0.498782f,
														- 0.030524f, -0.499067f,
														- 0.026168f, -0.499315f,
														- 0.021810f, -0.499524f,
														- 0.017450f, -0.499695f,
														- 0.013088f, -0.499829f,
														- 0.008726f, -0.499924f,
														- 0.004363f, -0.499981f,
														- 0.000000f, -0.500000f,
														0.004363f, -0.499981f,
														0.008726f, -0.499924f,
														0.013088f, -0.499829f,
														0.017450f, -0.499695f,
														0.021810f, -0.499524f,
														0.026168f, -0.499315f,
														0.030524f, -0.499067f,
														0.034878f, -0.498782f,
														0.039230f, -0.498459f,
														0.043578f, -0.498097f,
														0.047923f, -0.497698f,
														0.052264f, -0.497261f,
														0.056602f, -0.496786f,
														0.060935f, -0.496273f,
														0.065263f, -0.495722f,
														0.069587f, -0.495134f,
														0.073905f, -0.494508f,
														0.078217f, -0.493844f,
														0.082524f, -0.493143f,
														0.086824f, -0.492404f,
														0.091118f, -0.491627f,
														0.095404f, -0.490814f,
														0.099684f, -0.489962f,
														0.103956f, -0.489074f,
														0.108220f, -0.488148f,
														0.112476f, -0.487185f,
														0.116723f, -0.486185f,
														0.120961f, -0.485148f,
														0.125190f, -0.484074f,
														0.129410f, -0.482963f,
														0.133619f, -0.481815f,
														0.137819f, -0.480631f,
														0.142008f, -0.479410f,
														0.146186f, -0.478152f,
														0.150353f, -0.476858f,
														0.154509f, -0.475528f,
														0.158652f, -0.474162f,
														0.162784f, -0.472759f,
														0.166903f, -0.471321f,
														0.171010f, -0.469846f,
														0.175104f, -0.468336f,
														0.179184f, -0.466790f,
														0.183251f, -0.465209f,
														0.187303f, -0.463592f,
														0.191342f, -0.461940f,
														0.195366f, -0.460252f,
														0.199374f, -0.458530f,
														0.203368f, -0.456773f,
														0.207347f, -0.454981f,
														0.211309f, -0.453154f,
														0.215256f, -0.451293f,
														0.219186f, -0.449397f,
														0.223099f, -0.447467f,
														0.226995f, -0.445503f,
														0.230874f, -0.443505f,
														0.234736f, -0.441474f,
														0.238579f, -0.439409f,
														0.242405f, -0.437310f,
														0.246212f, -0.435178f,
														0.250000f, -0.433013f,
														0.253769f, -0.430815f,
														0.257519f, -0.428584f,
														0.261249f, -0.426320f,
														0.264960f, -0.424024f,
														0.268650f, -0.421696f,
														0.272319f, -0.419335f,
														0.275968f, -0.416943f,
														0.279596f, -0.414519f,
														0.283203f, -0.412063f,
														0.286788f, -0.409576f,
														0.290351f, -0.407058f,
														0.293893f, -0.404508f,
														0.297411f, -0.401928f,
														0.300908f, -0.399318f,
														0.304381f, -0.396677f,
														0.307831f, -0.394005f,
														0.311257f, -0.391304f,
														0.314660f, -0.388573f,
														0.318039f, -0.385812f,
														0.321394f, -0.383022f,
														0.324724f, -0.380203f,
														0.328029f, -0.377355f,
														0.331310f, -0.374478f,
														0.334565f, -0.371572f,
														0.337795f, -0.368639f,
														0.340999f, -0.365677f,
														0.344177f, -0.362687f,
														0.347329f, -0.359670f,
														0.350455f, -0.356625f,
														0.353553f, -0.353553f,
														0.356625f, -0.350455f,
														0.359670f, -0.347329f,
														0.362687f, -0.344177f,
														0.365677f, -0.340999f,
														0.368639f, -0.337795f,
														0.371572f, -0.334565f,
														0.374478f, -0.331310f,
														0.377355f, -0.328029f,
														0.380203f, -0.324724f,
														0.383022f, -0.321394f,
														0.385812f, -0.318039f,
														0.388573f, -0.314660f,
														0.391304f, -0.311257f,
														0.394005f, -0.307831f,
														0.396677f, -0.304381f,
														0.399318f, -0.300908f,
														0.401928f, -0.297411f,
														0.404508f, -0.293893f,
														0.407058f, -0.290351f,
														0.409576f, -0.286788f,
														0.412063f, -0.283203f,
														0.414519f, -0.279596f,
														0.416943f, -0.275968f,
														0.419335f, -0.272319f,
														0.421696f, -0.268650f,
														0.424024f, -0.264960f,
														0.426320f, -0.261249f,
														0.428584f, -0.257519f,
														0.430815f, -0.253769f,
														0.433013f, -0.250000f,
														0.435178f, -0.246212f,
														0.437310f, -0.242405f,
														0.439409f, -0.238579f,
														0.441474f, -0.234736f,
														0.443505f, -0.230874f,
														0.445503f, -0.226995f,
														0.447467f, -0.223099f,
														0.449397f, -0.219186f,
														0.451293f, -0.215256f,
														0.453154f, -0.211309f,
														0.454981f, -0.207347f,
														0.456773f, -0.203368f,
														0.458530f, -0.199374f,
														0.460252f, -0.195366f,
														0.461940f, -0.191342f,
														0.463592f, -0.187303f,
														0.465209f, -0.183251f,
														0.466790f, -0.179184f,
														0.468336f, -0.175104f,
														0.469846f, -0.171010f,
														0.471321f, -0.166903f,
														0.472759f, -0.162784f,
														0.474162f, -0.158652f,
														0.475528f, -0.154509f,
														0.476858f, -0.150353f,
														0.478152f, -0.146186f,
														0.479410f, -0.142008f,
														0.480631f, -0.137819f,
														0.481815f, -0.133619f,
														0.482963f, -0.129410f,
														0.484074f, -0.125190f,
														0.485148f, -0.120961f,
														0.486185f, -0.116723f,
														0.487185f, -0.112476f,
														0.488148f, -0.108220f,
														0.489074f, -0.103956f,
														0.489962f, -0.099684f,
														0.490814f, -0.095404f,
														0.491627f, -0.091118f,
														0.492404f, -0.086824f,
														0.493143f, -0.082524f,
														0.493844f, -0.078217f,
														0.494508f, -0.073905f,
														0.495134f, -0.069587f,
														0.495722f, -0.065263f,
														0.496273f, -0.060935f,
														0.496786f, -0.056602f,
														0.497261f, -0.052264f,
														0.497698f, -0.047923f,
														0.498097f, -0.043578f,
														0.498459f, -0.039230f,
														0.498782f, -0.034878f,
														0.499067f, -0.030524f,
														0.499315f, -0.026168f,
														0.499524f, -0.021810f,
														0.499695f, -0.017450f,
														0.499829f, -0.013088f,
														0.499924f, -0.008726f,
														0.499981f, -0.004363f,
														0.500000f, -0.000000f,
													};

		circleVerticesSize = circleVertices.length;

		final float circleColor[] = new float[]{
													1.0f, 1.0f, 1.0f
												};

		// vao And vbo Related Code
		// Triangle
		GLES32.glGenVertexArrays(1, vao_triangle, 0);
		GLES32.glBindVertexArray(vao_triangle[0]);

			// Position
			GLES32.glGenBuffers(1, vbo_triangle_position, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_triangle_position[0]);

			ByteBuffer byteBuffer = ByteBuffer.allocateDirect(trianglePosition.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
			positionBuffer.put(trianglePosition);
			positionBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, trianglePosition.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_POSITION);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			// Color
			GLES32.glGenBuffers(1, vbo_triangle_color, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_triangle_color[0]);

			byteBuffer = ByteBuffer.allocateDirect(triangleColor.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer colorBuffer = byteBuffer.asFloatBuffer();
			colorBuffer.put(triangleColor);
			colorBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleColor.length * 4, colorBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_COLOR);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glBindVertexArray(0);


		// Square
		GLES32.glGenVertexArrays(1, vao_square, 0);
		GLES32.glBindVertexArray(vao_square[0]);

			// Position
			GLES32.glGenBuffers(1, vbo_square, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_square[0]);

			byteBuffer = ByteBuffer.allocateDirect(squareVertices.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			positionBuffer = byteBuffer.asFloatBuffer();
			positionBuffer.put(squareVertices);
			positionBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, squareVertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_POSITION);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			// Color
			GLES32.glGenBuffers(1, vbo_square, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_square[0]);

			byteBuffer = ByteBuffer.allocateDirect(squareColor.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			colorBuffer = byteBuffer.asFloatBuffer();
			colorBuffer.put(squareColor);
			colorBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, squareColor.length * 4, colorBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_COLOR);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glBindVertexArray(0);


		// Circle
		GLES32.glGenVertexArrays(1, vao_circle, 0);
		GLES32.glBindVertexArray(vao_circle[0]);

			// Position
			GLES32.glGenBuffers(1, vbo_circle, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_circle[0]);

			byteBuffer = ByteBuffer.allocateDirect(circleVertices.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			positionBuffer = byteBuffer.asFloatBuffer();
			positionBuffer.put(circleVertices);
			positionBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, circleVertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_POSITION);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			// Color
			GLES32.glVertexAttrib3f(MyGLESMacros.SBJ_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);
			/*
			GLES32.glGenBuffers(1, vbo_color, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

			byteBuffer = ByteBuffer.allocateDirect(triangleColor.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer colorBuffer = byteBuffer.asFloatBuffer();
			colorBuffer.put(triangleColor);
			colorBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleColor.length * 4, colorBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_COLOR);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
			*/
		
		GLES32.glBindVertexArray(0);


		// Graph Lines
		GLES32.glGenVertexArrays(1, vao_graph, 0);
		GLES32.glBindVertexArray(vao_graph[0]);

			// Position
			GLES32.glGenBuffers(1, vbo_graph, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_graph[0]);

			byteBuffer = ByteBuffer.allocateDirect(graphPaperVertices.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			positionBuffer = byteBuffer.asFloatBuffer();
			positionBuffer.put(graphPaperVertices);
			positionBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, graphPaperVertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_POSITION);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			// Color
			GLES32.glGenBuffers(1, vbo_graph, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_graph[0]);

			byteBuffer = ByteBuffer.allocateDirect(graphPaperColor.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			colorBuffer = byteBuffer.asFloatBuffer();
			colorBuffer.put(graphPaperColor);
			colorBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, graphPaperColor.length * 4, colorBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_COLOR);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glBindVertexArray(0);


		// GraphAxis
		GLES32.glGenVertexArrays(1, vao_graphAxis, 0);
		GLES32.glBindVertexArray(vao_graphAxis[0]);

			// Position
			GLES32.glGenBuffers(1, vbo_graphAxis, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_graphAxis[0]);

			byteBuffer = ByteBuffer.allocateDirect(graphPaperAxisColor.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			positionBuffer = byteBuffer.asFloatBuffer();
			positionBuffer.put(graphPaperAxisColor);
			positionBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, graphPaperAxisColor.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_POSITION);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			// Color
			GLES32.glGenBuffers(1, vbo_graphAxis, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_graphAxis[0]);

			byteBuffer = ByteBuffer.allocateDirect(graphPaperAxisColor.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			colorBuffer = byteBuffer.asFloatBuffer();
			colorBuffer.put(graphPaperAxisColor);
			colorBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, graphPaperAxisColor.length * 4, colorBuffer, GLES32.GL_STATIC_DRAW);

			GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_COLOR);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glBindVertexArray(0);


		// Here Starts OpenGL Code
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		//  Enable Culling
		GLES32.glEnable(GLES32.GL_CULL_FACE);

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
		float modelViewMatrix[] = new float[16];
		Matrix.setIdentityM(modelViewMatrix, 0);
		float modelViewProjectionMatrix[] = new float[16];
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);

		Matrix.translateM(modelViewMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, -4.0f);
		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
		GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

		GLES32.glBindVertexArray(vao_graphAxis[0]);
	
			GLES32.glDrawArrays(GLES32.GL_LINES, 0, 4);

		GLES32.glBindVertexArray(0);

		if (gbDoubleClick)
		GraphPaperLines();

		if (gbToggleSquare)
			DrawSquare();

		if (gbToggleCircle)
			DrawCircle();

		if (gbToggleTriangle)
			DrawTriangle();

		GLES32.glUseProgram(0);

		requestRender();
	}

	void GraphPaperLines()
	{
		// Code
		GLES32.glBindVertexArray(vao_graph[0]);

		GLES32.glDrawArrays(GLES32.GL_LINES, 0, graphPaperVerticesSize);

		GLES32.glBindVertexArray(0);
	}

	void DrawSquare()
	{
		// Code
		GLES32.glBindVertexArray(vao_square[0]);

		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 8);

		GLES32.glBindVertexArray(0);
	}

	void DrawCircle()
	{
		// Code
		//GLES32.glPointSize(0.5f);
		GLES32.glBindVertexArray(vao_circle[0]);
		
		GLES32.glDrawArrays(GLES32.GL_POINTS, 0, circleVerticesSize);

		GLES32.glBindVertexArray(0);
	}

	void DrawTriangle()
	{
		// Code
		GLES32.glBindVertexArray(vao_triangle[0]);

		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 6);

		GLES32.glBindVertexArray(0);
	}

	private void uninitialize(){

		// Code
		//  Deletion And Uninitialization of vbo
		if (vbo_triangle_color[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_triangle_color, 0);
			vbo_triangle_color[0] = 0;
		}

		if (vbo_square[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_square, 0);
			vbo_square[0] = 0;
		}

		if (vbo_circle[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_circle, 0);
			vbo_circle[0] = 0;
		}

		// Deletion And Uninitialization of vao
		if (vao_circle[0] > 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_circle, 0);
			vao_circle[0] = 0;
		}

		if (vao_square[0] > 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_square, 0);
			vao_square[0] = 0;
		}

		if (vao_triangle[0] > 0)
		{
			GLES32.glDeleteVertexArrays(1, vao_triangle, 0);
			vao_triangle[0] = 0;
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
