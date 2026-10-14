#include "grass/GrassShader.h"


static GLuint shaderProgramObject_Grass;
static GLuint modelMatrixUniform_Grass;
static GLuint viewMatrixUniform_Grass;
static GLuint projectionMatrixUniform_Grass;
static GLuint cameraPositionUniform_Grass;
static GLuint lightPositionUniform_Grass;
static GLuint normalMapUniform_Grass;
static GLuint depthMapUniform_Grass;
static GLuint textureSamplerUniform_Grass;
static GLuint textureflowmapUniform_Grass;
static GLuint timeUniform_Grass;
static float t = 0.0f;


bool InitializeGrassShader(void)
{
	// code

	SHADERSHADERTYPEKEYPAIR shadersToCreate[] = {
		GL_VERTEX_SHADER, "shader/grass/GrassShader.vert"
		, GL_GEOMETRY_SHADER, "shader/grass/GrassShader.gs"
		, GL_FRAGMENT_SHADER, "shader/grass/GrassShader.frag"
	};

	GLuint shaders[3];
	unsigned int createdShadersCount = 0;

	if (!CreateShaders(
		shadersToCreate
		, sizeof(shadersToCreate) / sizeof(shadersToCreate[0])
		, shaders
		, &createdShadersCount
	))
	{
		ErrorLog(NULL, "InitializeGrassShader()::CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_Grass = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_Grass
		, shaders
		, createdShadersCount
	);

	// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
	/*free(shaders);
	shaders = nullptr;*/

	ATTRIBTYPEATTRIBNAMEKEYPAIR attributesToBind[] = {
		AMC_ATTRIBUTE_POSITION, "a_position",
		 AMC_ATTRIBUTE_TEXTURE0, "a_texcoord"
	};

	if (!BindShaderProgramAttribLocations(
		shaderProgramObject_Grass
		, attributesToBind
		, sizeof(attributesToBind) / sizeof(attributesToBind[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_Grass))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation[] = {
		&modelMatrixUniform_Grass, "u_modelMatrix"
		, &viewMatrixUniform_Grass, "u_viewMatrix"
		, &projectionMatrixUniform_Grass, "u_projectionMatrix"
		, &cameraPositionUniform_Grass, "u_cameraPosition"
		, &textureSamplerUniform_Grass, "u_textureSampler"
		, &textureflowmapUniform_Grass, "u_wind"
		, &timeUniform_Grass,"u_time"

	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_Grass
		, uniformsToBindLocation
		, sizeof(uniformsToBindLocation) / sizeof(uniformsToBindLocation[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	return(true);
}

void SetGrassShader(mat4 modelMatrix, GLuint grassTexture, GLuint flowmapTexture, vec3 camPosition, vec3 lightPos)
{
	// code
	glUseProgram(shaderProgramObject_Grass);

	glUniformMatrix4fv(modelMatrixUniform_Grass, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_Grass, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_Grass, 1, GL_FALSE, perspectiveProjectionMatrix);
	glUniform3fv(cameraPositionUniform_Grass,1, camPosition);
//	glUniform3fv(lightPositionUniform_Grass,1, lightPos);
	t += 0.001f;
	glUniform1f(timeUniform_Grass, t);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	glUniform1i(textureSamplerUniform_Grass, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, flowmapTexture);
	glUniform1i(textureflowmapUniform_Grass, 1);
}

void UnsetGrassShader(void)
{
	// code
	glUseProgram(0);
}

void UnInitializeGrassShader(void)
{
	// code
	ReleaseShaderProgramObject(shaderProgramObject_Grass);
}