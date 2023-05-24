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

import android.graphics.Bitmap;
import android.opengl.GLUtils;

public class GLESView extends GLSurfaceView implements OnDoubleTapListener, OnGestureListener, GLSurfaceView.Renderer {

	private GestureDetector gestureDetector;
	private Context context;

	private int shaderProgramObject;

	private int vao[] = new int[1];
	private int vbo_position[] = new int[1];
	private int mvpMatrixUniform;

	private int vbo_texcord[] = new int[1];
	private int mvpUniformMatrix;
	private int textureSamplerUniform;
	private int texture_checkerboard[] = new int[1];
	private final int CHECKERBOARD_WIDTH = 64;
	private final int CHECKERBOARD_HEIGHT = 64;
	byte checkImage[] = new byte[CHECKERBOARD_WIDTH * CHECKERBOARD_HEIGHT * 4];
	private int text_Image[] = new int[1];
	
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
			"uniform highp sampler2D u_textureSampler;" +
			"in vec2 a_texcoord_out;" +
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
		textureSamplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_textureSampler");

		// Declaration Of Vertex Data Arrays
		final float texCoord[] = new float[]
											{
												1.0f, 1.0f,
												0.0f, 1.0f,
												0.0f, 0.0f,
												1.0f, 0.0f
											};

		// vao And vbo Related Code
		GLES32.glGenVertexArrays(1, vao, 0);
		GLES32.glBindVertexArray(vao[0]);

			// Position
			GLES32.glGenBuffers(1, vbo_position, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4*3*4, null, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(MyGLESMacros.APS_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(MyGLESMacros.APS_ATTRIBUTE_POSITION);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glGenBuffers(1, vbo_texcord, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texcord[0]);
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(texCoord.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(texCoord);
		positionBuffer.position(0);
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, texCoord.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(MyGLESMacros.APS_ATTRIBUTE_TEXTURE0, 2, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(MyGLESMacros.APS_ATTRIBUTE_TEXTURE0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glBindVertexArray(0);


		// Here Starts OpenGL Code
		GLES32.glClearDepthf(1.0f);
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glEnable(GLES32.GL_CULL_FACE);

		//  Enable Culling
		GLES32.glEnable(GLES32.GL_CULL_FACE);

		GLES32.glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		GLES32.glEnable(GLES32.GL_TEXTURE_2D);

		text_Image[0] = loadGLTexture();

		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
	}

	private int loadGLTexture()
	{
		// code
		makeCheckerBoard();
		// conversion of byte array to byteBuffer
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(CHECKERBOARD_WIDTH * CHECKERBOARD_HEIGHT * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		byteBuffer.put(checkImage);
		byteBuffer.position(0);

		Bitmap bitmap = Bitmap.createBitmap(CHECKERBOARD_WIDTH, CHECKERBOARD_HEIGHT, Bitmap.Config.ARGB_8888);

		bitmap.copyPixelsFromBuffer(byteBuffer);

		int texture[] = new int[1];
		GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);
		GLES32.glGenTextures(1, texture,0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_S, GLES32.GL_REPEAT);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_T, GLES32.GL_REPEAT);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_NEAREST);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_NEAREST);

		// create a texture
		GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
		// no call to glGen
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);
		return texture[0];
	}

	private void makeCheckerBoard()
	{
		// variable declarations
		int c;

		// code
		for (int i = 0; i < CHECKERBOARD_WIDTH; i++)
		{
			for (int j = 0; j < CHECKERBOARD_HEIGHT; j++)
			{
				c = ((i & 8) ^ (j & 8)) * 255;

				checkImage[(i*64+j)*4 + 0] = (byte)c;
				checkImage[(i*64+j)*4 + 1] = (byte)c;
				checkImage[(i*64+j)*4 + 2] = (byte)c;
				checkImage[(i*64+j)*4 + 3] = (byte)0xff;
			}
		}
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

		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, text_Image[0]);
		GLES32.glUniform1i(textureSamplerUniform, 0);

		GLES32.glBindVertexArray(vao[0]);

		final float position[] = new float[12] ;
		position[0] = 0.0f;
		position[1] = 1.0f;
		position[2] = 0.0f;
		position[3] = -2.0f;
		position[4] = 1.0f;
		position[5] = 0.0f;
		position[6] = -2.0f;
		position[7] = -1.0f;
		position[8] = 0.0f;
		position[9] = 0.0f;
		position[10] = -1.0f;
		position[11] = 0.0f;

		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(position.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(position);
		positionBuffer.position(0);		

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, position.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		
		position[0] = 2.41421f;
		position[1] = 1.0f;
		position[2] = -1.41421f;
		position[3] = 1.0f;
		position[4] = 1.0f;
		position[5] = 0.0f;
		position[6] = 1.0f;
		position[7] = -1.0f;
		position[8] = 0.0f;
		position[9] = 2.41421f;
		position[10] = -1.0f;
		position[11] = -1.41421f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		byteBuffer = ByteBuffer.allocateDirect(position.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(position);
		positionBuffer.position(0);		

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, position.length * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		GLES32.glBindVertexArray(0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);

		GLES32.glUseProgram(0);

		requestRender();
	}

	private void uninitialize(){

		// Code
		// texture deletion
		if(texture_checkerboard[0] > 0)
		{
			GLES32.glDeleteTextures(1, texture_checkerboard, 0);
			texture_checkerboard[0] = 0;
		}

			// deletion and uninitilization vbo_cube_position
		if (vbo_texcord[0] > 0)
		{
			GLES32.glDeleteBuffers(1, vbo_texcord,0);
			vbo_texcord[0]  = 0;
		}

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
