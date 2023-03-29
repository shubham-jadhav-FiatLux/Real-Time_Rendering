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

	private float sphere_vertices[]=new float[1146];
    private float sphere_normals[]=new float[1146];
    private float sphere_textures[]=new float[764];
    private short sphere_elements[]=new short[2280];
	private int gNumVertices;
	private int gNumElements;

	private int shaderProgramObject;

	private int vao_Sphere[] = new int[1];
	private int vbo_Sphere_Position[] = new int[1];
	private int vbo_Sphere_Normals[] = new int[1];
	private int vbo_Sphere_Elements[] = new int[1];

	private int modelMatrixUniform;
	private int viewMatrixUniform;
	private int projectionMatrixUniform;

	private int laUniform;				// Ambient
	private int ldUniform;				// Diffuse
	private int lsUniform;				// Specular
	private int lightPositionUniform;

	private int kaUniform;
	private int kdUniform;
	private int ksUniform;
	private int materialShininessUniform;

	private int lightingEnabledUniform;
	private boolean bLight  = false;

	private float lightAmbient[] = new float[]{0.1f, 0.1f, 0.1f, 1.0f};
	private float lightDiffuse[] = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
	private float lightSpecular[] = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
	private float lightPosition[] = new float[]{100.0f, 100.0f, 100.0f, 1.0f};

	private float materialAmbient[] = new float[]{0.0f, 0.0f, 0.0f, 1.0f};
	private float materialDiffuse[] = new float[]{0.5f, 0.2f, 0.7f, 1.0f};
	private float materialSpecular[] = new float[]{0.7f, 0.7f, 0.7f, 1.0f};
	private float materialShininess = 128.0f;
	
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
			"in vec3 phong_ads_light;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
			"FragColor = vec4(phong_ads_light, 1.0);" +
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
		laUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_la");
		ldUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ld");
		lsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ls");
		lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_lightPosition");
		kaUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ka");
		kdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kd");
		ksUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ks");
		materialShininessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_materialShininess");
		lightingEnabledUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_lightingEnabled");

		// Declaration Of Vertex Data Arrays
		Sphere sphere=new Sphere();
		sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
		gNumVertices = sphere.getNumberOfSphereVertices();
		gNumElements = sphere.getNumberOfSphereElements();

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

			//GLES32.glVertexAttribPointer(MyGLESMacros.SBJ_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
			//GLES32.glEnableVertexAttribArray(MyGLESMacros.SBJ_ATTRIBUTE_POSITION);
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
		float modelMatrix[] = new float[16];
		Matrix.setIdentityM(modelMatrix, 0);
		float viewMatrix[] = new float[16];
		Matrix.setIdentityM(viewMatrix, 0);

		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, -3.0f);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);

		// Sending Light Related Uniforms
		if (bLight == true)
		{
			GLES32.glUniform1i(lightingEnabledUniform, 1);

			GLES32.glUniform3fv(laUniform, 1, lightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, lightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, lightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, materialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, materialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, materialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, materialShininess);
		}
		else
		{
			GLES32.glUniform1i(lightingEnabledUniform, 0);
		}

		GLES32.glBindVertexArray(vao_Sphere[0]);
	
			GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
			GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);

		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);

		requestRender();
	}

	private void uninitialize(){

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
