#include "water/WaterShader.h"


static GLuint shaderProgramObject_Water;
static GLuint modelMatrixUniform_Water;
static GLuint viewMatrixUniform_Water;
static GLuint projectionMatrixUniform_Water;
static GLuint cameraPositionUniform_Water;
static GLuint lightPositionUniform_Water;
static GLuint reflectionTextureUniform_Water;
static GLuint refractionTextureUniform_Water;
static GLuint dudvMapUniform_Water;
static GLuint normalMapUniform_Water;
static GLuint depthMapUniform_Water;
static GLuint moveFactorUniform_Water;

bool InitializeWaterShader(void)
{
	// code

	SHADERSHADERTYPEKEYPAIR shadersToCreate[] = {
		GL_VERTEX_SHADER, "shader/water/WaterShader.vert"
		, GL_FRAGMENT_SHADER, "shader/water/WaterShader.frag"
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
		ErrorLog(NULL, "InitializeWaterShader()::CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_Water = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_Water
		, shaders
		, createdShadersCount
	);

	// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
	/*free(shaders);
	shaders = nullptr;*/

	ATTRIBTYPEATTRIBNAMEKEYPAIR attributesToBind[] = {
		AMC_ATTRIBUTE_POSITION, "a_position"
	};

	if (!BindShaderProgramAttribLocations(
		shaderProgramObject_Water
		, attributesToBind
		, sizeof(attributesToBind) / sizeof(attributesToBind[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_Water))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation[] = {
		&modelMatrixUniform_Water, "u_modelMatrix"
		, &viewMatrixUniform_Water, "u_viewMatrix"
		, &projectionMatrixUniform_Water, "u_projectionMatrix"

		, &cameraPositionUniform_Water, "u_cameraPosition"
		, &lightPositionUniform_Water, "u_lightPosition"
		, &reflectionTextureUniform_Water, "reflection_texture"
		, &refractionTextureUniform_Water, "refraction_texture"
		, &dudvMapUniform_Water, "dudv_map_texture"
		, &normalMapUniform_Water, "normalMap"
		, &depthMapUniform_Water, "depthMap"
		, &moveFactorUniform_Water, "moveFactor"
	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_Water
		, uniformsToBindLocation
		, sizeof(uniformsToBindLocation) / sizeof(uniformsToBindLocation[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	return(true);
}

void SetWaterShader(mat4 modelMatrix,  GLuint reflectionTexture, GLuint refractionTexture, GLuint dudvMapTexture, GLuint normalMapTexture, GLuint depthMapTexture, GLfloat moveFactor, vec3 camPosition, vec3 lightPos)
{
	// code
	glUseProgram(shaderProgramObject_Water);

	glUniformMatrix4fv(modelMatrixUniform_Water, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_Water, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_Water, 1, GL_FALSE, perspectiveProjectionMatrix);

	glUniform3fv(cameraPositionUniform_Water,1, camPosition);
	glUniform3fv(lightPositionUniform_Water,1, lightPos);
	glUniform1f(moveFactorUniform_Water, moveFactor);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
	glUniform1i(reflectionTextureUniform_Water, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refractionTexture);
	glUniform1i(refractionTextureUniform_Water, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dudvMapTexture);
	glUniform1i(dudvMapUniform_Water, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normalMapTexture);
	glUniform1i(normalMapUniform_Water, 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(depthMapUniform_Water, 4);
}

void UnsetWaterShader(void)
{
	// code
	glUseProgram(0);
}

void UninitializeWaterShader(void)
{
	// code
	ReleaseShaderProgramObject(shaderProgramObject_Water);
}