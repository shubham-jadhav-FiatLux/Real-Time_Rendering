#include "msaa/MSAAShader.h"


static GLuint shaderProgramObject_MSAA;
static GLuint modelMatrixUniform_MSAA;
static GLuint viewMatrixUniform_MSAA;
static GLuint projectionMatrixUniform_MSAA;
static GLuint textureSamplerUniform_MSAA;

bool InitializeMSAAShader(void)
{
	// code
	//shaderProgramObject_MSAA = CreateShaderProgramObject("shader/MSAAShader.vs", "shader/MSAAShader.fs", ATTRIBUTE_ONLY_TEXTURE);

	SHADERSHADERTYPEKEYPAIR shadersToCreate[] = {
		GL_VERTEX_SHADER, "shader/msaa/MSAAShader.vert"
		, GL_FRAGMENT_SHADER, "shader/msaa/MSAAShader.frag"
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
		ErrorLog(NULL, "InitializeMSAAShader()::CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_MSAA = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_MSAA
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
		shaderProgramObject_MSAA
		, attributesToBind
		, sizeof(attributesToBind) / sizeof(attributesToBind[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_MSAA))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation[] = {
		&modelMatrixUniform_MSAA, "u_modelMatrix"
		, &viewMatrixUniform_MSAA, "u_viewMatrix"
		, &projectionMatrixUniform_MSAA, "u_projectionMatrix"

		, &textureSamplerUniform_MSAA, "u_screenTextureSampler" 
	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_MSAA
		, uniformsToBindLocation
		, sizeof(uniformsToBindLocation) / sizeof(uniformsToBindLocation[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	return(true);
}

void SetMSAAShader(mat4 modelMatrix, GLuint screenTexture)
{
	// code
	glUseProgram(shaderProgramObject_MSAA);

	glUniformMatrix4fv(modelMatrixUniform_MSAA, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_MSAA, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_MSAA, 1, GL_FALSE, perspectiveProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glUniform1i(textureSamplerUniform_MSAA, 0);
}

void UnsetMSAAShader(void)
{
	// code
	glUseProgram(0);
}

void UninitializeMSAAShader(void)
{
	// code
	ReleaseShaderProgramObject(shaderProgramObject_MSAA);
}