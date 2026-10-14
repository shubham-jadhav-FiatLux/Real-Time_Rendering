#include "bloom/BloomLearnOGLShader.h"

static GLuint shaderProgramObject_BloomBrightness;
static GLuint modelMatrixUniform_BloomBrightness;
static GLuint viewMatrixUniform_BloomBrightness;
static GLuint projectionMatrixUniform_BloomBrightness;
static GLuint colorTextureSamplerUniform_BloomBrightness;

GLuint shaderProgramObject_BloomBlur;
GLuint modelMatrixUniform_BloomBlur;
GLuint viewMatrixUniform_BloomBlur;
GLuint projectionMatrixUniform_BloomBlur;
GLuint imageUniform_BloomBlur;
GLuint horizontalUniform_BloomBlur;

GLuint shaderProgramObject_Bloom;
GLuint modelMatrixUniform_Bloom;
GLuint viewMatrixUniform_Bloom;
GLuint projectionMatrixUniform_Bloom;
GLuint colorTextureSamplerUniform_Bloom;
GLuint blurTextureSamplerUniform_Bloom;
GLuint isBloomUniform_Bloom;
GLuint exposureUniform_Bloom;

bool InitializeBloomLearnOGLShader(void)
{
	//######################## BRIGHTNESS SHADER ####################################

#pragma region BRIGHTNESS
	SHADERSHADERTYPEKEYPAIR shadersToCreate[] = {
		GL_VERTEX_SHADER, "shader/bloom/learnOGL/BloomBrightnessShader.vert"
		, GL_FRAGMENT_SHADER, "shader/bloom/learnOGL/BloomBrightnessShader.frag"
	};

	GLuint shaders[2];
	unsigned int createdShadersCount = 0;

	if (!CreateShaders(
		shadersToCreate
		, sizeof(shadersToCreate) / sizeof(shadersToCreate[0])
		, shaders
		, &createdShadersCount
	))
	{
		ErrorLog(NULL, "InitializeBloomLearnOGLShader():: BloomBrightness CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_BloomBrightness = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_BloomBrightness
		, shaders
		, createdShadersCount
	);

	// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
	/*free(shaders);
	shaders = nullptr;*/

	ATTRIBTYPEATTRIBNAMEKEYPAIR attributesToBind[] = {
		AMC_ATTRIBUTE_POSITION, "a_position"
		, AMC_ATTRIBUTE_TEXTURE0, "a_texcoord"
	};

	if (!BindShaderProgramAttribLocations(
		shaderProgramObject_BloomBrightness
		, attributesToBind
		, sizeof(attributesToBind) / sizeof(attributesToBind[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_BloomBrightness))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation[] = {
		&modelMatrixUniform_BloomBrightness, "u_modelMatrix"
		, &viewMatrixUniform_BloomBrightness, "u_viewMatrix"
		, &projectionMatrixUniform_BloomBrightness, "u_projectionMatrix"
		
		, &colorTextureSamplerUniform_BloomBrightness, "u_colorTexture"

	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_BloomBrightness
		, uniformsToBindLocation
		, sizeof(uniformsToBindLocation) / sizeof(uniformsToBindLocation[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}
#pragma endregion

	//######################## BLUR SHADER ####################################

#pragma region BLUR
	SHADERSHADERTYPEKEYPAIR shadersToCreate_blur[] = {
		GL_VERTEX_SHADER, "shader/bloom/learnOGL/BlurShader.vert"
		, GL_FRAGMENT_SHADER, "shader/bloom/learnOGL/BlurShader.frag"
	};

	GLuint shaders_blur[2];
	unsigned int createdShadersCount_blur = 0;

	if (!CreateShaders(
		shadersToCreate_blur
		, sizeof(shadersToCreate_blur) / sizeof(shadersToCreate_blur[0])
		, shaders_blur
		, &createdShadersCount_blur
	))
	{
		ErrorLog(NULL, "InitializeBloomLearnOGLShader():: Blur CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_BloomBlur = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_BloomBlur
		, shaders_blur
		, createdShadersCount_blur
	);

	// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
	/*free(shaders);
	shaders = nullptr;*/

	ATTRIBTYPEATTRIBNAMEKEYPAIR attributesToBind_blur[] = {
		AMC_ATTRIBUTE_POSITION, "a_position"
		, AMC_ATTRIBUTE_TEXTURE0, "a_texcoord"
	};

	if (!BindShaderProgramAttribLocations(
		shaderProgramObject_BloomBlur
		, attributesToBind_blur
		, sizeof(attributesToBind_blur) / sizeof(attributesToBind_blur[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_BloomBlur))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation_blur[] = {
		&modelMatrixUniform_BloomBlur, "u_modelMatrix"
		, &viewMatrixUniform_BloomBlur, "u_viewMatrix"
		, &projectionMatrixUniform_BloomBlur, "u_projectionMatrix"
		
		
		, &horizontalUniform_BloomBlur, "u_horizontal"
		, &imageUniform_BloomBlur, "u_image"

	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_BloomBlur
		, uniformsToBindLocation_blur
		, sizeof(uniformsToBindLocation_blur) / sizeof(uniformsToBindLocation_blur[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}
#pragma endregion
	
	//######################## BLOOM SHADER ####################################

#pragma region BLOOM
	SHADERSHADERTYPEKEYPAIR shadersToCreate_bloom[] = {
		GL_VERTEX_SHADER, "shader/bloom/learnOGL/BloomShader.vert"
		, GL_FRAGMENT_SHADER, "shader/bloom/learnOGL/BloomShader.frag"
	};

	GLuint shaders_bloom[2];
	unsigned int createdShadersCount_bloom = 0;

	if (!CreateShaders(
		shadersToCreate_bloom
		, sizeof(shadersToCreate_bloom) / sizeof(shadersToCreate_bloom[0])
		, shaders_bloom
		, &createdShadersCount_bloom
	))
	{
		ErrorLog(NULL, "InitializeBloomLearnOGLShader():: Bloom CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_Bloom = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_Bloom
		, shaders_bloom
		, createdShadersCount_bloom
	);

	// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
	/*free(shaders);
	shaders = nullptr;*/

	ATTRIBTYPEATTRIBNAMEKEYPAIR attributesToBind_bloom[] = {
		AMC_ATTRIBUTE_POSITION, "a_position"
		, AMC_ATTRIBUTE_TEXTURE0, "a_texcoord"
	};

	if (!BindShaderProgramAttribLocations(
		shaderProgramObject_Bloom
		, attributesToBind_bloom
		, sizeof(attributesToBind_bloom) / sizeof(attributesToBind_bloom[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_Bloom))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation_bloom[] = {
		&modelMatrixUniform_Bloom, "u_modelMatrix"
		, &viewMatrixUniform_Bloom, "u_viewMatrix"
		, &projectionMatrixUniform_Bloom, "u_projectionMatrix"

		, &colorTextureSamplerUniform_Bloom, "u_scene_texture"
		, &blurTextureSamplerUniform_Bloom, "u_bloom_blur_texture"
		, &isBloomUniform_Bloom, "u_bloom"
		, &exposureUniform_Bloom, "u_exposure"
	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_Bloom
		, uniformsToBindLocation_bloom
		, sizeof(uniformsToBindLocation_bloom) / sizeof(uniformsToBindLocation_bloom[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}
#pragma endregion

	return true;
}

void DisplayBloomLearnOGLShader(void)
{

}

void UninitializeBloomLearnOGLShader(void)
{
	ReleaseShaderProgramObject(shaderProgramObject_BloomBrightness);
	ReleaseShaderProgramObject(shaderProgramObject_BloomBlur);
	ReleaseShaderProgramObject(shaderProgramObject_Bloom);
}

void SetBloomBrightnessShader(mat4 modelMatrix, GLuint colorBuffer)
{
	glUseProgram(shaderProgramObject_BloomBrightness);

	glUniformMatrix4fv(modelMatrixUniform_BloomBrightness, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_BloomBrightness, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_BloomBrightness, 1, GL_FALSE, perspectiveProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glUniform1i(colorTextureSamplerUniform_BloomBrightness, 0);
}

void UnsetBloomBrightnessShader()
{
	glUseProgram(0);
}
