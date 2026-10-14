#include "bloom/BloomLearnOGL.h"
#include "bloom/BloomLearnOGLShader.h"
#include "StaticModelShader.h"
#include "static_model_loading/StaticModel.h"
#include "TextureShader.h"
#include "QuadRender.h"

GLuint hdrFBO_Brightness;
GLuint colorBuffer_Brightness;

// hdr framebuffer variables
GLuint hdrFBO;
unsigned int colorBuffer;
unsigned int isBloomEnable = 1;
float exposure = 4.9f;

// pingpong lighting variables for blurr
GLuint pingpongFBO[2];
unsigned int pingpongColorBuffers[2];
unsigned int window_width = 1920, window_height = 1080;

static Model mushroom_model;
static Model alienTree_model;
static Model lotus_model;


extern GLuint shaderProgramObject_BloomBlur;
extern GLuint modelMatrixUniform_BloomBlur;
extern GLuint viewMatrixUniform_BloomBlur;
extern GLuint projectionMatrixUniform_BloomBlur;
extern GLuint imageUniform_BloomBlur;
extern GLuint horizontalUniform_BloomBlur;

extern GLuint shaderProgramObject_Bloom;
extern GLuint modelMatrixUniform_Bloom;
extern GLuint viewMatrixUniform_Bloom;
extern GLuint projectionMatrixUniform_Bloom;
extern GLuint colorTextureSamplerUniform_Bloom;
extern GLuint blurTextureSamplerUniform_Bloom;
extern GLuint isBloomUniform_Bloom;
extern GLuint exposureUniform_Bloom;

bool InitializeBloomLearnOGL(void)
{

#pragma region HDR_FBO

	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ErrorLog(NULL, "creation of framebuffer is failed for hdr\n");
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

#pragma region BRIGHTNESS_FBO

	glGenFramebuffers(1, &hdrFBO_Brightness);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO_Brightness);

	glGenTextures(1, &colorBuffer_Brightness);

	glBindTexture(GL_TEXTURE_2D, colorBuffer_Brightness);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer_Brightness, 0);


	unsigned int rboDepth_Brightness;
	glGenRenderbuffers(1, &rboDepth_Brightness);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth_Brightness);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth_Brightness);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ErrorLog(NULL, "creation of framebuffer is failed for brightness\n");
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

#pragma region PINGPONG_FBO

	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorBuffers);

	for (int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffers[i], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			ErrorLog(NULL, "creation of framebuffer is failed for pingpong lights\n");
			return false;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion

	mushroom_model = Model("resources/models/ToTestBloomEffect/01_Mushroom/mushroom.obj");
	alienTree_model = Model("resources/models/ToTestBloomEffect/02_Alien-plant/alien-plant.fbx");
	lotus_model = Model("resources/models/ToTestBloomEffect/03_Lotus/lotus.fbx");

	return true;
}

void DisplayBloomLearnOGL(void)
{
	// variable declarations
	mat4 modelMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	vec4 lightPosition = vmath::vec4(0.0f, 0.0f, -2.0f, 1.0f);

	// code
	//#################################################################
	SetHDRFBO();
	translationMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
	scaleMatrix = vmath::scale(0.02f, 0.02f, 0.02f);
	modelMatrix = translationMatrix * scaleMatrix;
	//SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, 0.09f, 0.32f, TEMP_INSTANCE_COUNT);
	mushroom_model.Draw(1);
	UnsetStaticModelShader();
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	translationMatrix = vmath::translate(1.0f, 0.0f, -2.0f);
	rotationMatrix = vmath::rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	scaleMatrix = vmath::scale(0.3f, 0.3f, 0.3f);
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;
	//SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, 0.09f, 0.32f, TEMP_INSTANCE_COUNT);
	lotus_model.Draw(1);

	UnSetHDRFBO();
	UnsetStaticModelShader();
	//#################################################################
	
	// brightness shader
	SetBrightnessFBO();
	modelMatrix = translate(0.0f, 0.0f, 0.0f);
	SetBloomBrightnessShader(mat4::identity(), colorBuffer);

	RenderQuad();
	
	UnSetBrightnessFBO();
	UnsetBloomBrightnessShader();
	//#################################################################

	// blur shader
	bool horizontal = true, first_iteration = true;
	unsigned int amount = 10;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramObject_BloomBlur);
	glViewport(0, 0, (GLsizei)window_width, (GLsizei)window_height);

	glUniformMatrix4fv(modelMatrixUniform_BloomBlur, 1, GL_FALSE, mat4::identity());
	glUniformMatrix4fv(viewMatrixUniform_BloomBlur, 1, GL_FALSE, mat4::identity());
	glUniformMatrix4fv(projectionMatrixUniform_BloomBlur, 1, GL_FALSE, mat4::identity());

	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform1i(horizontalUniform_BloomBlur, horizontal);

		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorBuffers[!horizontal]);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffer_Brightness : pingpongColorBuffers[!horizontal]);
		glUniform1i(imageUniform_BloomBlur, 0);

		// redner quad
		RenderQuad();

		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	//#################################################################
	
	// final bloom draw call
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramObject_Bloom);
	glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);
	modelMatrix = translate(0.0f, 0.0f, 0.0f);
	glUniformMatrix4fv(modelMatrixUniform_Bloom, 1, GL_FALSE, mat4::identity());
	glUniformMatrix4fv(viewMatrixUniform_Bloom, 1, GL_FALSE, mat4::identity());
	glUniformMatrix4fv(projectionMatrixUniform_Bloom, 1, GL_FALSE, mat4::identity());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glUniform1i(colorTextureSamplerUniform_Bloom, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[!horizontal]);
	glUniform1i(blurTextureSamplerUniform_Bloom, 1);

	glUniform1i(isBloomUniform_Bloom, isBloomEnable); // bloom on or off
	glUniform1f(exposureUniform_Bloom, exposure); // exposure

	// render quad
	RenderQuad();
	glUseProgram(0);
	
	//#################################################################
}

void UninitializeBloomLearnOGL(void)
{
	
}

void SetHDRFBO(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)window_width, (GLsizei)window_height);
}

void UnSetHDRFBO(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SetBrightnessFBO(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO_Brightness);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glViewport(0, 0, (GLsizei)window_width, (GLsizei)window_height);
}

void UnSetBrightnessFBO(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SetBlurFBO(void)
{
	//glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO_Brightness);
}

void UnSetBlurFBO(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SetBloom(void)
{

}

void UnSetBloom(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}