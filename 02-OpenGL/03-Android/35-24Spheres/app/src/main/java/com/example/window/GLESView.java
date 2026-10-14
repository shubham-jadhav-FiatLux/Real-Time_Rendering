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
	private int KeyPressed = 0;
	private float AngleForXRotation = 0.0f;
	private float AngleForYRotation = 0.0f;
	private float AngleForZRotation = 0.0f;
	private int WIN_WIDTH = 800;
	private int WIN_HEIGHT = 600;
	private float currentWidth = WIN_WIDTH;
	private float currentHeight = WIN_HEIGHT;

	private float LightAmbient[] = new float[]{0.1f, 0.1f, 0.1f, 1.0f};
	private float LightDiffuse[] = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
	private float LightSpecular[] = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
	private float LightPosition[] = new float[]{0.0f, 0.0f, 2.0f, 1.0f};
	
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
		currentWidth = width;
		currentHeight = height;

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
	
		KeyPressed++;
		if (KeyPressed > 3)
			KeyPressed = 0;

		AngleForXRotation = 0.0f;		// Reset
		AngleForYRotation = 0.0f;
		AngleForZRotation = 0.0f;
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
			"uniform highp int u_lightingEnabled;" +
			"out vec4 FragColor;" +
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

		GLES32.glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
	}

	private void resize(int width, int height) {
	
		// Code
		if (height == 0)
		height = 1;

		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix , 0, 45.0f, ((float)width * 1.5f)/ (float)height, 0.1f, 200.0f);
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
			/*
			GLES32.glUniform3fv(laUniform, 1, lightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, lightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, lightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, materialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, materialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, materialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, materialShininess);
			*/
		}
		else
		{
			GLES32.glUniform1i(lightingEnabledUniform, 0);
		}

		Draw24Spheres();

		/*
		GLES32.glBindVertexArray(vao_Sphere[0]);
	
			GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
			GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);

		GLES32.glBindVertexArray(0);
		*/

		GLES32.glUseProgram(0);

		requestRender();
	}

	private void update(){

		// Code
		if (KeyPressed == 1)
		{
			AngleForXRotation += 2.0f;
			AngleForYRotation = 0.0f;
			AngleForZRotation = 0.0f;
			//if (AngleForXRotation >= 360.0f)
				//AngleForXRotation -= 360.0f;
		}
		else if (KeyPressed == 2)
		{

			AngleForYRotation += 2.0f;
			AngleForXRotation += 2.0f;
			AngleForZRotation = 0.0f;
			//if (AngleForYRotation >= 360.0f)
				//AngleForYRotation -= 360.0f;
		}
		else if (KeyPressed == 3)
		{
			AngleForZRotation += 2.0f;
			AngleForXRotation += 2.0f;
			AngleForYRotation = 0.0f;
			//if (AngleForZRotation >= 360.0f)
				//AngleForZRotation -= 360.0f;
		}


		if (KeyPressed == 1)								// X Rotation
		{
			LightPosition[1] = 10 * (float)Math.sin(AngleForXRotation * Math.PI / 180.0f);
			LightPosition[2] = 10 * (float)Math.cos(AngleForXRotation * Math.PI / 180.0f);
		}
		else if (KeyPressed == 2)							// Y Rotation
		{
			LightPosition[0] = 10 * (float)Math.sin(AngleForYRotation * Math.PI / 180.0f);
			LightPosition[2] = 10 * (float)Math.cos(-AngleForYRotation * Math.PI / 180.0f);
		}
		else if (KeyPressed == 3)							// Z Rotation
		{
			LightPosition[0] = 10 * -(float)Math.sin(AngleForZRotation * Math.PI / 180.0f);
			LightPosition[1] = 10 * (float)Math.cos(AngleForZRotation * Math.PI / 180.0f);
		}
		else
		{
			LightPosition[0] = 0.0f;
			LightPosition[1] = 0.0f;
			LightPosition[2] = 0.0f;
		}
	}

	void Draw24Spheres(){

		// Variable Declarations
		float MaterialAmbient[] = new float[4];
		float MaterialDiffuse[] = new float[4];
		float MaterialSpecular[] = new float[4];
		float MaterialShininess;

		int viewportWidth = (int)currentWidth / 4;
		int viewportHeight = (int)currentHeight / 6;

		float translationMatrix[] = new float[16];
		Matrix.setIdentityM(translationMatrix, 0);
		float modelMatrix[] = new float[16];
		Matrix.setIdentityM(modelMatrix, 0);
		float viewMatrix[] = new float[16];
		Matrix.setIdentityM(viewMatrix, 0);

		float zVal = -2.0f;

		// Code
		// ***** 1st sphere on 1st column, emerald *****
		// ambient material
		MaterialAmbient[0] = 0.0215f; // r
		MaterialAmbient[1] = 0.1745f; // g
		MaterialAmbient[2] = 0.0215f; // b
		MaterialAmbient[3] = 1.0f;   // a

		// diffuse material
		MaterialDiffuse[0] = 0.07568f; // r
		MaterialDiffuse[1] = 0.61424f; // g
		MaterialDiffuse[2] = 0.07568f; // b
		MaterialDiffuse[3] = 1.0f;    // a

		// specular material
		MaterialSpecular[0] = 0.633f;    // r
		MaterialSpecular[1] = 0.727811f; // g
		MaterialSpecular[2] = 0.633f;    // b
		MaterialSpecular[3] = 1.0f;     // a

		// shininess
		MaterialShininess = 0.6f * 128.0f;

		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(0, 5 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************
		
		// ***** 2nd sphere on 1st column, jade *****
		// ambient material
		MaterialAmbient[0] = 0.135f;  // r
		MaterialAmbient[1] = 0.2225f; // g
		MaterialAmbient[2] = 0.1575f; // b
		MaterialAmbient[3] = 1.0f;   // a

		// diffuse material
		MaterialDiffuse[0] = 0.54f; // r
		MaterialDiffuse[1] = 0.89f; // g
		MaterialDiffuse[2] = 0.63f; // b
		MaterialDiffuse[3] = 1.0f; // a

		// specular material
		MaterialSpecular[0] = 0.316228f; // r
		MaterialSpecular[1] = 0.316228f; // g
		MaterialSpecular[2] = 0.316228f; // b
		MaterialSpecular[3] = 1.0f;     // a

		// shininess
		MaterialShininess = 0.1f * 128.0f;
		//GLES32.glMaterialf(GLES32.GL_FRONT, GLES32.GL_SHININESS, MaterialShininess, 0);

		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(0, 4 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************
		
		// ***** 3rd sphere on 1st column, obsidian *****
		// ambient material
		MaterialAmbient[0] = 0.05375f; // r
		MaterialAmbient[1] = 0.05f;    // g
		MaterialAmbient[2] = 0.06625f; // b
		MaterialAmbient[3] = 1.0f;    // a

		// diffuse material
		MaterialDiffuse[0] = 0.18275f; // r
		MaterialDiffuse[1] = 0.17f;    // g
		MaterialDiffuse[2] = 0.22525f; // b
		MaterialDiffuse[3] = 1.0f;    // a

		// specular material
		MaterialSpecular[0] = 0.332741f; // r
		MaterialSpecular[1] = 0.328634f; // g
		MaterialSpecular[2] = 0.346435f; // b
		MaterialSpecular[3] = 1.0f;     // a

		// shininess
		MaterialShininess = 0.3f * 128.0f;

		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(0, 3 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 4th sphere on 1st column, pearl *****
		// ambient material
		MaterialAmbient[0] = 0.25f;    // r
		MaterialAmbient[1] = 0.20725f; // g
		MaterialAmbient[2] = 0.20725f; // b
		MaterialAmbient[3] = 1.0f;    // a

		// diffuse material
		MaterialDiffuse[0] = 1.0f;   // r
		MaterialDiffuse[1] = 0.829f; // g
		MaterialDiffuse[2] = 0.829f; // b
		MaterialDiffuse[3] = 1.0f;  // a

		// specular material
		MaterialSpecular[0] = 0.296648f; // r
		MaterialSpecular[1] = 0.296648f; // g
		MaterialSpecular[2] = 0.296648f; // b
		MaterialSpecular[3] = 1.0f;     // a

		// shininess
		MaterialShininess = 0.088f * 128.0f;

		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(0, 2 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 5th sphere on 1st column, ruby *****
		// ambient material
		MaterialAmbient[0] = 0.1745f;  // r
		MaterialAmbient[1] = 0.01175f; // g
		MaterialAmbient[2] = 0.01175f; // b
		MaterialAmbient[3] = 1.0f;    // a

		// diffuse material
		MaterialDiffuse[0] = 0.61424f; // r
		MaterialDiffuse[1] = 0.04136f; // g
		MaterialDiffuse[2] = 0.04136f; // b
		MaterialDiffuse[3] = 1.0f;    // a

		// specular material
		MaterialSpecular[0] = 0.727811f; // r
		MaterialSpecular[1] = 0.626959f; // g
		MaterialSpecular[2] = 0.626959f; // b
		MaterialSpecular[3] = 1.0f;     // a

		// shininess
		MaterialShininess = 0.6f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(0, 1 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 6th sphere on 1st column, turquoise *****
		// ambient material
		MaterialAmbient[0] = 0.1f;     // r
		MaterialAmbient[1] = 0.18725f; // g
		MaterialAmbient[2] = 0.1745f;  // b
		MaterialAmbient[3] = 1.0f;    // a

		// diffuse material
		MaterialDiffuse[0] = 0.396f;   // r
		MaterialDiffuse[1] = 0.74151f; // g
		MaterialDiffuse[2] = 0.69102f; // b
		MaterialDiffuse[3] = 1.0f;    // a

		// specular material
		MaterialSpecular[0] = 0.297254f; // r
		MaterialSpecular[1] = 0.30829f;  // g
		MaterialSpecular[2] = 0.306678f; // b
		MaterialSpecular[3] = 1.0f;     // a

		// shininess
		MaterialShininess = 0.1f * 128.0f;

		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(0, 0 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************
		// *******************************************************
		// *******************************************************

		// ***** 1st sphere on 2nd column, brass *****
		// ambient material
		MaterialAmbient[0] = 0.329412f; // r
		MaterialAmbient[1] = 0.223529f; // g
		MaterialAmbient[2] = 0.027451f; // b
		MaterialAmbient[3] = 1.0f;     // a

		// diffuse material
		MaterialDiffuse[0] = 0.780392f; // r
		MaterialDiffuse[1] = 0.568627f; // g
		MaterialDiffuse[2] = 0.113725f; // b
		MaterialDiffuse[3] = 1.0f;     // a

		// specular material
		MaterialSpecular[0] = 0.992157f; // r
		MaterialSpecular[1] = 0.941176f; // g
		MaterialSpecular[2] = 0.807843f; // b
		MaterialSpecular[3] = 1.0f;     // a

		// shininess
		MaterialShininess = 0.21794872f * 128.0f;

		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(1 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 2nd sphere on 2nd column, bronze *****
		// ambient material
		MaterialAmbient[0] = 0.2125f; // r
		MaterialAmbient[1] = 0.1275f; // g
		MaterialAmbient[2] = 0.054f;  // b
		MaterialAmbient[3] = 1.0f;   // a

		// diffuse material
		MaterialDiffuse[0] = 0.714f;   // r
		MaterialDiffuse[1] = 0.4284f;  // g
		MaterialDiffuse[2] = 0.18144f; // b
		MaterialDiffuse[3] = 1.0f;    // a

		// specular material
		MaterialSpecular[0] = 0.393548f; // r
		MaterialSpecular[1] = 0.271906f; // g
		MaterialSpecular[2] = 0.166721f; // b
		MaterialSpecular[3] = 1.0f;     // a

		// shininess
		MaterialShininess = 0.2f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(1 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 3rd sphere on 2nd column, chrome *****
		// ambient material
		MaterialAmbient[0] = 0.25f; // r
		MaterialAmbient[1] = 0.25f; // g
		MaterialAmbient[2] = 0.25f; // b
		MaterialAmbient[3] = 1.0f; // a

		// diffuse material
		MaterialDiffuse[0] = 0.4f;  // r
		MaterialDiffuse[1] = 0.4f;  // g
		MaterialDiffuse[2] = 0.4f;  // b
		MaterialDiffuse[3] = 1.0f; // a

		// specular material
		MaterialSpecular[0] = 0.774597f; // r
		MaterialSpecular[1] = 0.774597f; // g
		MaterialSpecular[2] = 0.774597f; // b
		MaterialSpecular[3] = 1.0f;     // a

		// shininess
		MaterialShininess = 0.6f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(1 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 4th sphere on 2nd column, copper *****
		// ambient material
		MaterialAmbient[0] = 0.19125f; // r
		MaterialAmbient[1] = 0.0735f;  // g
		MaterialAmbient[2] = 0.0225f;  // b
		MaterialAmbient[3] = 1.0f;    // a

		// diffuse material
		MaterialDiffuse[0] = 0.7038f;  // r
		MaterialDiffuse[1] = 0.27048f; // g
		MaterialDiffuse[2] = 0.0828f;  // b
		MaterialDiffuse[3] = 1.0f;    // a

		// specular material
		MaterialSpecular[0] = 0.256777f; // r
		MaterialSpecular[1] = 0.137622f; // g
		MaterialSpecular[2] = 0.086014f; // b
		MaterialSpecular[3] = 1.0f;     // a

		// shininess
		MaterialShininess = 0.1f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(1 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 5th sphere on 2nd column, gold *****
		// ambient material
		MaterialAmbient[0] = 0.24725f; // r
		MaterialAmbient[1] = 0.1995f;  // g
		MaterialAmbient[2] = 0.0745f;  // b
		MaterialAmbient[3] = 1.0f;    // a

		// diffuse material
		MaterialDiffuse[0] = 0.75164f; // r
		MaterialDiffuse[1] = 0.60648f; // g
		MaterialDiffuse[2] = 0.22648f; // b
		MaterialDiffuse[3] = 1.0f;    // a

		// specular material
		MaterialSpecular[0] = 0.628281f; // r
		MaterialSpecular[1] = 0.555802f; // g
		MaterialSpecular[2] = 0.366065f; // b
		MaterialSpecular[3] = 1.0f;     // a

		// shininess
		MaterialShininess = 0.4f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(1 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 6th sphere on 2nd column, silver *****
		// ambient material
		MaterialAmbient[0] = 0.19225f; // r
		MaterialAmbient[1] = 0.19225f; // g
		MaterialAmbient[2] = 0.19225f; // b
		MaterialAmbient[3] = 1.0f;    // a
		
		// diffuse material
		MaterialDiffuse[0] = 0.50754f; // r
		MaterialDiffuse[1] = 0.50754f; // g
		MaterialDiffuse[2] = 0.50754f; // b
		MaterialDiffuse[3] = 1.0f;    // a
		
		// specular material
		MaterialSpecular[0] = 0.508273f; // r
		MaterialSpecular[1] = 0.508273f; // g
		MaterialSpecular[2] = 0.508273f; // b
		MaterialSpecular[3] = 1.0f;     // a
		
		// shininess
		MaterialShininess = 0.4f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(1 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************
		// *******************************************************
		// *******************************************************

		// ***** 1st sphere on 3rd column, black *****
		// ambient material
		MaterialAmbient[0] = 0.0f;  // r
		MaterialAmbient[1] = 0.0f;  // g
		MaterialAmbient[2] = 0.0f;  // b
		MaterialAmbient[3] = 1.0f; // a
		
		// diffuse material
		MaterialDiffuse[0] = 0.01f; // r
		MaterialDiffuse[1] = 0.01f; // g
		MaterialDiffuse[2] = 0.01f; // b
		MaterialDiffuse[3] = 1.0f; // a
		
		// specular material
		MaterialSpecular[0] = 0.50f; // r
		MaterialSpecular[1] = 0.50f; // g
		MaterialSpecular[2] = 0.50f; // b
		MaterialSpecular[3] = 1.0f; // a
		
		// shininess
		MaterialShininess = 0.25f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(2 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 2nd sphere on 3rd column, cyan *****
		// ambient material
		MaterialAmbient[0] = 0.0f;  // r
		MaterialAmbient[1] = 0.1f;  // g
		MaterialAmbient[2] = 0.06f; // b
		MaterialAmbient[3] = 1.0f; // a
		
		// diffuse material
		MaterialDiffuse[0] = 0.0f;        // r
		MaterialDiffuse[1] = 0.50980392f; // g
		MaterialDiffuse[2] = 0.50980392f; // b
		MaterialDiffuse[3] = 1.0f;       // a
		
		// specular material
		MaterialSpecular[0] = 0.50196078f; // r
		MaterialSpecular[1] = 0.50196078f; // g
		MaterialSpecular[2] = 0.50196078f; // b
		MaterialSpecular[3] = 1.0f;       // a
		
		// shininess
		MaterialShininess = 0.25f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(2 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 3rd sphere on 2nd column, green *****
		// ambient material
		MaterialAmbient[0] = 0.0f;  // r
		MaterialAmbient[1] = 0.0f;  // g
		MaterialAmbient[2] = 0.0f;  // b
		MaterialAmbient[3] = 1.0f; // a

		// diffuse material
		MaterialDiffuse[0] = 0.1f;  // r
		MaterialDiffuse[1] = 0.35f; // g
		MaterialDiffuse[2] = 0.1f;  // b
		MaterialDiffuse[3] = 1.0f; // a
		
		// specular material
		MaterialSpecular[0] = 0.45f; // r
		MaterialSpecular[1] = 0.55f; // g
		MaterialSpecular[2] = 0.45f; // b
		MaterialSpecular[3] = 1.0f; // a
		
		// shininess
		MaterialShininess = 0.25f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(2 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 4th sphere on 3rd column, red *****
		// ambient material
		MaterialAmbient[0] = 0.0f;  // r
		MaterialAmbient[1] = 0.0f;  // g
		MaterialAmbient[2] = 0.0f;  // b
		MaterialAmbient[3] = 1.0f; // a
		
		// diffuse material
		MaterialDiffuse[0] = 0.5f;  // r
		MaterialDiffuse[1] = 0.0f;  // g
		MaterialDiffuse[2] = 0.0f;  // b
		MaterialDiffuse[3] = 1.0f; // a
		
		// specular material
		MaterialSpecular[0] = 0.7f;  // r
		MaterialSpecular[1] = 0.6f;  // g
		MaterialSpecular[2] = 0.6f;  // b
		MaterialSpecular[3] = 1.0f; // a
		
		// shininess
		MaterialShininess = 0.25f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(2 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 5th sphere on 3rd column, white *****
		// ambient material
		MaterialAmbient[0] = 0.0f;  // r
		MaterialAmbient[1] = 0.0f;  // g
		MaterialAmbient[2] = 0.0f;  // b
		MaterialAmbient[3] = 1.0f; // a
		
		// diffuse material
		MaterialDiffuse[0] = 0.55f; // r
		MaterialDiffuse[1] = 0.55f; // g
		MaterialDiffuse[2] = 0.55f; // b
		MaterialDiffuse[3] = 1.0f; // a
		
		// specular material
		MaterialSpecular[0] = 0.70f; // r
		MaterialSpecular[1] = 0.70f; // g
		MaterialSpecular[2] = 0.70f; // b
		MaterialSpecular[3] = 1.0f; // a
		
		// shininess
		MaterialShininess = 0.25f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(2 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 6th sphere on 3rd column, yellow plastic *****
		// ambient material
		MaterialAmbient[0] = 0.0f;  // r
		MaterialAmbient[1] = 0.0f;  // g
		MaterialAmbient[2] = 0.0f;  // b
		MaterialAmbient[3] = 1.0f; // a
		
		// diffuse material
		MaterialDiffuse[0] = 0.5f;  // r
		MaterialDiffuse[1] = 0.5f;  // g
		MaterialDiffuse[2] = 0.0f;  // b
		MaterialDiffuse[3] = 1.0f; // a
		
		// specular material
		MaterialSpecular[0] = 0.60f; // r
		MaterialSpecular[1] = 0.60f; // g
		MaterialSpecular[2] = 0.50f; // b
		MaterialSpecular[3] = 1.0f; // a
		
		// shininess
		MaterialShininess = 0.25f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(2 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************
		// *******************************************************
		// *******************************************************

		// ***** 1st sphere on 4th column, black *****
		// ambient material
		MaterialAmbient[0] = 0.02f; // r
		MaterialAmbient[1] = 0.02f; // g
		MaterialAmbient[2] = 0.02f; // b
		MaterialAmbient[3] = 1.0f; // a
		
		// diffuse material
		MaterialDiffuse[0] = 0.01f; // r
		MaterialDiffuse[1] = 0.01f; // g
		MaterialDiffuse[2] = 0.01f; // b
		MaterialDiffuse[3] = 1.0f; // a
		
		// specular material
		MaterialSpecular[0] = 0.4f;  // r
		MaterialSpecular[1] = 0.4f;  // g
		MaterialSpecular[2] = 0.4f;  // b
		MaterialSpecular[3] = 1.0f; // a
		
		// shininess
		MaterialShininess = 0.078125f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(3 * viewportWidth, 5 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 2nd sphere on 4th column, cyan *****
		// ambient material
		MaterialAmbient[0] = 0.0f;  // r
		MaterialAmbient[1] = 0.05f; // g
		MaterialAmbient[2] = 0.05f; // b
		MaterialAmbient[3] = 1.0f; // a
		
		// diffuse material
		MaterialDiffuse[0] = 0.4f;  // r
		MaterialDiffuse[1] = 0.5f;  // g
		MaterialDiffuse[2] = 0.5f;  // b
		MaterialDiffuse[3] = 1.0f; // a
		
		// specular material
		MaterialSpecular[0] = 0.04f; // r
		MaterialSpecular[1] = 0.7f;  // g
		MaterialSpecular[2] = 0.7f;  // b
		MaterialSpecular[3] = 1.0f; // a
		
		// shininess
		MaterialShininess = 0.078125f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(3 * viewportWidth, 4 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 3rd sphere on 4th column, green *****
		// ambient material
		MaterialAmbient[0] = 0.0f;  // r
		MaterialAmbient[1] = 0.05f; // g
		MaterialAmbient[2] = 0.0f;  // b
		MaterialAmbient[3] = 1.0f; // a
		
		// diffuse material
		MaterialDiffuse[0] = 0.4f;  // r
		MaterialDiffuse[1] = 0.5f;  // g
		MaterialDiffuse[2] = 0.4f;  // b
		MaterialDiffuse[3] = 1.0f; // a
		
		// specular material
		MaterialSpecular[0] = 0.04f; // r
		MaterialSpecular[1] = 0.7f;  // g
		MaterialSpecular[2] = 0.04f; // b
		MaterialSpecular[3] = 1.0f; // a
		
		// shininess
		MaterialShininess = 0.078125f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(3 * viewportWidth, 3 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 4th sphere on 4th column, red *****
		// ambient material
		MaterialAmbient[0] = 0.05f; // r
		MaterialAmbient[1] = 0.0f;  // g
		MaterialAmbient[2] = 0.0f;  // b
		MaterialAmbient[3] = 1.0f; // a

		// diffuse material
		MaterialDiffuse[0] = 0.5f;  // r
		MaterialDiffuse[1] = 0.4f;  // g
		MaterialDiffuse[2] = 0.4f;  // b
		MaterialDiffuse[3] = 1.0f; // a
		
		// specular material
		MaterialSpecular[0] = 0.7f;  // r
		MaterialSpecular[1] = 0.04f; // g
		MaterialSpecular[2] = 0.04f; // b
		MaterialSpecular[3] = 1.0f; // a
		
		// shininess
		MaterialShininess = 0.078125f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(3 * viewportWidth, 2 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 5th sphere on 4th column, white *****
		// ambient material
		MaterialAmbient[0] = 0.05f; // r
		MaterialAmbient[1] = 0.05f; // g
		MaterialAmbient[2] = 0.05f; // b
		MaterialAmbient[3] = 1.0f; // a

		// diffuse material
		MaterialDiffuse[0] = 0.5f;  // r
		MaterialDiffuse[1] = 0.5f;  // g
		MaterialDiffuse[2] = 0.5f;  // b
		MaterialDiffuse[3] = 1.0f; // a
		
		// specular material
		MaterialSpecular[0] = 0.7f;  // r
		MaterialSpecular[1] = 0.7f;  // g
		MaterialSpecular[2] = 0.7f;  // b
		MaterialSpecular[3] = 1.0f; // a
		
		// shininess
		MaterialShininess = 0.078125f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(3 * viewportWidth, 1 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************

		// ***** 6th sphere on 4th column, yellow rubber *****
		// ambient material
		MaterialAmbient[0] = 0.05f; // r
		MaterialAmbient[1] = 0.05f; // g
		MaterialAmbient[2] = 0.0f;  // b
		MaterialAmbient[3] = 1.0f; // a
		
		// diffuse material
		MaterialDiffuse[0] = 0.5f;  // r
		MaterialDiffuse[1] = 0.5f;  // g
		MaterialDiffuse[2] = 0.4f;  // b
		MaterialDiffuse[3] = 1.0f; // a
		
		// specular material
		MaterialSpecular[0] = 0.7f;  // r
		MaterialSpecular[1] = 0.7f;  // g
		MaterialSpecular[2] = 0.04f; // b
		MaterialSpecular[3] = 1.0f; // a
		
		// shininess
		MaterialShininess = 0.078125f * 128.0f;
		
		if (bLight == true)
		{
			GLES32.glUniform3fv(laUniform, 1, LightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, LightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, LightSpecular, 0);
			GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);

			GLES32.glUniform3fv(kaUniform, 1, MaterialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, MaterialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, MaterialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, MaterialShininess);
		}

		// geometry
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.translateM(modelMatrix, 0, translationMatrix, 0, 0.0f, 0.0f, zVal);
		GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);
		GLES32.glViewport(3 * viewportWidth, 0 * viewportHeight, viewportWidth, viewportHeight);
		GLES32.glBindVertexArray(vao_Sphere[0]);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_Sphere_Elements[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, gNumElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);
		// *******************************************************
		// *******************************************************
		// *******************************************************
		
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
