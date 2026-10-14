#include "ColorShader.h"


static GLuint shaderProgramObject_Color;
static GLuint modelMatrixUniform_Color;
static GLuint viewMatrixUniform_Color;
static GLuint projectionMatrixUniform_Color;
static GLuint ColorSamplerUniform_Color;
static GLuint colorUniform_Color;

bool InitializeColorShader(void)
{
	// code
	//shaderProgramObject_Color = CreateShaderProgramObject("shader/ColorShader.vs", "shader/ColorShader.fs", ATTRIBUTE_ONLY_Color);

	SHADERSHADERTYPEKEYPAIR shadersToCreate[] = {
		GL_VERTEX_SHADER, "shader/ColorShader.vert"
		, GL_FRAGMENT_SHADER, "shader/ColorShader.frag"
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
		ErrorLog(NULL, "InitializeColorShader()::CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_Color = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_Color
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
		shaderProgramObject_Color
		, attributesToBind
		, sizeof(attributesToBind) / sizeof(attributesToBind[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_Color))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation[] = {
		&modelMatrixUniform_Color, "u_modelMatrix"
		, &viewMatrixUniform_Color, "u_viewMatrix"
		, &projectionMatrixUniform_Color, "u_projectionMatrix"

		, &colorUniform_Color, "u_color"
	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_Color
		, uniformsToBindLocation
		, sizeof(uniformsToBindLocation) / sizeof(uniformsToBindLocation[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	return(true);
}

void SetColorShader(mat4 modelMatrix, vec4 color)
{
	// code
	glUseProgram(shaderProgramObject_Color);

	glUniformMatrix4fv(modelMatrixUniform_Color, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_Color, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_Color, 1, GL_FALSE, perspectiveProjectionMatrix);

	glUniform4fv(colorUniform_Color, 1, color);
}

void UnsetColorShader(void)
{
	// code
	glUseProgram(0);
}

void UninitializeColorShader(void)
{
	// code
	ReleaseShaderProgramObject(shaderProgramObject_Color);
}