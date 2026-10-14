#include "TextureShader.h"


static GLuint shaderProgramObject_Texcoord;
static GLuint modelMatrixUniform_Texcoord;
static GLuint viewMatrixUniform_Texcoord;
static GLuint projectionMatrixUniform_Texcoord;
static GLuint textureSamplerUniform_Texcoord;

bool InitializeTextureShader(void)
{
	// code
	//shaderProgramObject_Texcoord = CreateShaderProgramObject("shader/TextureShader.vs", "shader/TextureShader.fs", ATTRIBUTE_ONLY_TEXTURE);

	SHADERSHADERTYPEKEYPAIR shadersToCreate[] = {
		GL_VERTEX_SHADER, "shader/TextureShader.vert"
		, GL_FRAGMENT_SHADER, "shader/TextureShader.frag"
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
		ErrorLog(NULL, "InitializeTextureShader()::CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_Texcoord = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_Texcoord
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
		shaderProgramObject_Texcoord
		, attributesToBind
		, sizeof(attributesToBind) / sizeof(attributesToBind[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_Texcoord))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation[] = {
		&modelMatrixUniform_Texcoord, "u_modelMatrix"
		, &viewMatrixUniform_Texcoord, "u_viewMatrix"
		, &projectionMatrixUniform_Texcoord, "u_projectionMatrix"

		, &textureSamplerUniform_Texcoord, "u_textureSampler" 
	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_Texcoord
		, uniformsToBindLocation
		, sizeof(uniformsToBindLocation) / sizeof(uniformsToBindLocation[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	return(true);
}

void SetTextureShader(mat4 modelMatrix, GLuint imageTexture)
{
	// code
	glUseProgram(shaderProgramObject_Texcoord);

	glUniformMatrix4fv(modelMatrixUniform_Texcoord, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_Texcoord, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_Texcoord, 1, GL_FALSE, perspectiveProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, imageTexture);
	glUniform1i(textureSamplerUniform_Texcoord, 0);
}

void UnsetTextureShader(void)
{
	// code
	glUseProgram(0);
}

void UninitializeTextureShader(void)
{
	// code
	ReleaseShaderProgramObject(shaderProgramObject_Texcoord);
}