#include "OceanShader.h"

static GLuint shaderProgramObject_Ocean;
static GLuint modelMatrixUniform_Ocean;
static GLuint viewMatrixUniform_Ocean;
static GLuint projectionMatrixUniform_Ocean;

static GLuint heightScaleUniform_Ocean;
static GLuint chopinessUniform_Ocean;
static GLuint sizeUniform_Ocean;

static GLuint deepColorUniform_Ocean;
static GLuint shallowColorUniform_Ocean;
static GLuint skyColorUniform_Ocean;
static GLuint lightDirUniform_Ocean;

bool InitializeOceanShader(void)
{

	// code
	//shaderProgramObject_Ocean = CreateShaderProgramObject("shader/TextureShader.vs", "shader/TextureShader.fs", ATTRIBUTE_ONLY_TEXTURE);

	SHADERSHADERTYPEKEYPAIR shadersToCreate[] = {
		GL_VERTEX_SHADER, "shader/ocean/Ocean.vert"
		, GL_FRAGMENT_SHADER, "shader/ocean/Ocean.frag"
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
		ErrorLog(NULL, "InitializeOceanShader()::CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_Ocean = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_Ocean
		, shaders
		, createdShadersCount
	);

	// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
	/*free(shaders);
	shaders = nullptr;*/

	ATTRIBTYPEATTRIBNAMEKEYPAIR attributesToBind[] = {
		AMC_ATTRIBUTE_POSITION, "a_position"
		,AMC_ATTRIBUTE_HEIGHT, "a_height"
		,AMC_ATTRIBUTE_TEXTURE1, "a_texcoord"
	};

	if (!BindShaderProgramAttribLocations(
		shaderProgramObject_Ocean
		, attributesToBind
		, sizeof(attributesToBind) / sizeof(attributesToBind[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_Ocean))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation[] = {
		&modelMatrixUniform_Ocean, "u_modelMatrix"
		, &viewMatrixUniform_Ocean, "u_viewMatrix"
		, &projectionMatrixUniform_Ocean, "u_projectionMatrix"

		,&heightScaleUniform_Ocean,"heightScale"
		,&chopinessUniform_Ocean,"chopiness"
		,&sizeUniform_Ocean,"size"
		
		,&deepColorUniform_Ocean,"deepColor"
		,&shallowColorUniform_Ocean,"shallowColor"
		,&skyColorUniform_Ocean,"skyColor"
		,&lightDirUniform_Ocean,"lightDir"
	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_Ocean
		, uniformsToBindLocation
		, sizeof(uniformsToBindLocation) / sizeof(uniformsToBindLocation[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	return(true);
}

void SetOceanShader(mat4 modelMatrix, float heightScale, float chopiness, vec2 size, vec4 deepColor, vec4 shallowColor, vec4 skyColor, vec3 lightDir)
{
	glUseProgram(shaderProgramObject_Ocean);
	//send the above transfomation matrix to 
	glUniformMatrix4fv(modelMatrixUniform_Ocean, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_Ocean, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_Ocean, 1, GL_FALSE, perspectiveProjectionMatrix);

	glUniform1f(heightScaleUniform_Ocean, heightScale);
	glUniform1f(chopinessUniform_Ocean, chopiness);
	glUniform2fv(sizeUniform_Ocean,1, size);

	glUniform4fv(deepColorUniform_Ocean,1, deepColor);
	glUniform4fv(shallowColorUniform_Ocean, 1, shallowColor);
	glUniform4fv(skyColorUniform_Ocean, 1, skyColor);
	glUniform3fv(lightDirUniform_Ocean, 1, lightDir);

}

void UnsetOceanShader(void)
{
	glUseProgram(0);
}

void UninitializeOceanShader(void)
{
	ReleaseShaderProgramObject(shaderProgramObject_Ocean);
}