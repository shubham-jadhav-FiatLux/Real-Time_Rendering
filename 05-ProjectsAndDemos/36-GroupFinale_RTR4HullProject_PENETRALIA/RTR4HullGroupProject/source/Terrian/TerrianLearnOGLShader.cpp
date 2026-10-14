#include "terrian/TerrianLearnOGLShader.h"
#include "Texture.h"

#define DIRECTIONAL_LIGHT_MEMBER_COUNT 4
#define POINT_LIGHT_MEMBER_COUNT 7
#define NUM_POINT_LIGHTS 5

static GLuint shaderProgramObject_TerrianLearnOGL;
static GLuint modelMatrixUniform_TerrianLearnOGL;
static GLuint viewMatrixUniform_TerrianLearnOGL;
static GLuint projectionMatrixUniform_TerrianLearnOGL;
static GLuint mvpMatrixUniform_TerrianLearnOGL;

static GLuint multitextureBackgroundUniform;
static GLuint rTextureUniform;
static GLuint gTextureUniform;
static GLuint bTextureUniform;
static GLuint blendmapUniform;

static GLuint colorTextureUniform;
static GLuint cameraPositionUniform;
static GLuint dmap_depthUniform;
static GLuint displacementTextureUniform;
static GLuint lightPositionUniform;
static GLuint bLightUniform;

struct dirLightUniform_LearnOGL
{
	GLuint la;
	GLuint ld;
	GLuint ls;
	GLuint lightDirection;
}dirLightUniform_LearnOGL[2];

struct pointLightUniform_LearnOGL
{
	GLuint la;
	GLuint ld;
	GLuint ls;
	GLuint lightPosition;
	GLuint constant;
	GLuint linear;
	GLuint quadratic;
}pointLightUniform_LearnOGL[NUM_POINT_LIGHTS];

std::vector<vec3> pointLightPositions;
std::vector<vec3> pointLightColors;

bool InitializeTerrianLearnOGLShader(void)
{
	// code
	//shaderProgramObject_TerrianLearnOGL = CreateShaderProgramObject("shader/TerrianLearnOGLShader.vs", "shader/TerrianLearnOGLShader.fs", ATTRIBUTE_ONLY_TerrianLearnOGL);

	SHADERSHADERTYPEKEYPAIR shadersToCreate[] = {
		GL_VERTEX_SHADER, "shader/terrian/learnOGL/TerrianLearnOGL.vert"
		,GL_TESS_CONTROL_SHADER, "shader/terrian/learnOGL/TerrianLearnOGL.tcs"
		,GL_TESS_EVALUATION_SHADER, "shader/terrian/learnOGL/TerrianLearnOGL.tes"
		, GL_FRAGMENT_SHADER, "shader/terrian/learnOGL/TerrianLearnOGL.frag"
	};

	GLuint shaders[4];
	unsigned int createdShadersCount = 0;

	if (!CreateShaders(
		shadersToCreate
		, sizeof(shadersToCreate) / sizeof(shadersToCreate[0])
		, shaders
		, &createdShadersCount
	))
	{
		ErrorLog(NULL, "InitializeTerrianLearnOGLShader()::CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_TerrianLearnOGL = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_TerrianLearnOGL
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
		shaderProgramObject_TerrianLearnOGL
		, attributesToBind
		, sizeof(attributesToBind) / sizeof(attributesToBind[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_TerrianLearnOGL))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMBLOCKUNIFORMBLOCKBINDINGKEYPAIR uniformBlocksToBindIndices[] = {
		//UBO_DIRECTIONA_LIGHT, "DirectionalLight",
		UBO_LIGHTS, "Lights"
		, UBO_POINT_LIGHTS, "PointLights"
	};

	if (!BindShaderProgramUniformBlockIndices(
		shaderProgramObject_TerrianLearnOGL
		, uniformBlocksToBindIndices
		, sizeof(uniformBlocksToBindIndices) / sizeof(uniformBlocksToBindIndices[0])
	))
	{
		ErrorLog(NULL, "InitializeTerrianLearnOGLShader()::BindShaderProgramUniformBlockIndices() failed.");
		return(false);
	}
	else
	{
		DebugLog(NULL, "InitializeTerrianLearnOGLShader()::BindShaderProgramUniformBlockIndices() succeeded.");
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation[] = {
		 &modelMatrixUniform_TerrianLearnOGL, "u_modelMatrix"
		, &viewMatrixUniform_TerrianLearnOGL, "u_viewMatrix"
		, &projectionMatrixUniform_TerrianLearnOGL, "u_projectionMatrix"
		/*,& mvpMatrixUniform_TerrianLearnOGL, "u_mvpMatrix"*/

		, &dmap_depthUniform, "u_dmapDepth"
		, &displacementTextureUniform, "u_heightMap"
		, &colorTextureUniform, "u_textureColor"
		, &lightPositionUniform, "u_lightPosition"
		, &cameraPositionUniform, "u_cameraPosition"

		, &multitextureBackgroundUniform, "u_backgroundTexture"
		, &rTextureUniform, "u_rTexture"
		, &gTextureUniform, "u_gTexture"
		, &bTextureUniform, "u_bTexture"
		, &blendmapUniform, "u_blendMap"
		, &bLightUniform, "u_bLight"

	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_TerrianLearnOGL
		, uniformsToBindLocation
		, sizeof(uniformsToBindLocation) / sizeof(uniformsToBindLocation[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	pointLightPositions.push_back(vec3(1.0, 21.0f, 5.0f));
	pointLightPositions.push_back(vec3(1.0, 21.0f, -20.0f));
	pointLightPositions.push_back(vec3(1.0, 21.0f, -5.0f));
	pointLightPositions.push_back(vec3(5.0, 21.0f, -5.0f));
	pointLightPositions.push_back(vec3(0.0, 21.0f, 35.0f));

	pointLightColors.push_back(vec3(1.0, 0.0, 0.0));
	pointLightColors.push_back(vec3(0.0, 1.0, 0.0));
	pointLightColors.push_back(vec3(0.0, 0.0, 1.0));
	pointLightColors.push_back(vec3(1.0, 1.0, 0.0));
	pointLightColors.push_back(vec3(1.0, 0.0, 1.0));

	return(true);
}

void SetTerrianLearnOGLShader(mat4 modelMatrix, GLuint displacementTexture, GLuint colorTexture, vec4 lightPos,
	GLuint multitextureBackground, GLuint redTexture, GLuint greenTexture, GLuint blueTexture, GLuint iblendMap, GLuint light, struct DirLight dirLight[], unsigned int lightCount, vec3 cameraPos)
{
	// code
	glUseProgram(shaderProgramObject_TerrianLearnOGL);

	glUniformMatrix4fv(modelMatrixUniform_TerrianLearnOGL, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_TerrianLearnOGL, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_TerrianLearnOGL, 1, GL_FALSE, perspectiveProjectionMatrix);
	
	glUniformMatrix4fv(mvpMatrixUniform_TerrianLearnOGL, 1, GL_FALSE, (perspectiveProjectionMatrix * viewMatrix * modelMatrix));
	glUniform1f(dmap_depthUniform, 6.0f);
	glUniform1i(bLightUniform, light);

	glUniform4fv(lightPositionUniform, 1, lightPos);
	glUniform3fv(cameraPositionUniform, 1, cameraPos);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, displacementTexture);
	glUniform1i(displacementTextureUniform, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glUniform1i(colorTextureUniform, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, multitextureBackground);
	glUniform1i(multitextureBackgroundUniform, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, redTexture);
	glUniform1i(rTextureUniform, 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, greenTexture);
	glUniform1i(gTextureUniform, 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, blueTexture);
	glUniform1i(bTextureUniform, 5);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, iblendMap);
	glUniform1i(blendmapUniform, 6);

	/*glUniform3fv(dirLightUniform_LearnOGL[0].la, 1, dirLight[0].ambient);
	glUniform3fv(dirLightUniform_LearnOGL[0].ld, 1, dirLight[0].diffuse);
	glUniform3fv(dirLightUniform_LearnOGL[0].ls, 1, dirLight[0].specular);
	glUniform4fv(dirLightUniform_LearnOGL[0].lightDirection, 1, dirLight[0].direction);

	for (unsigned int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		glUniform3fv(pointLightUniform_LearnOGL[i].la, 1, vec3(1.0, 1.0, 1.0));
		glUniform3fv(pointLightUniform_LearnOGL[i].ld, 1, pointLightColors[i]);
		glUniform3fv(pointLightUniform_LearnOGL[i].ls, 1, pointLightColors[i]);
		glUniform3fv(pointLightUniform_LearnOGL[i].lightPosition, 1, pointLightPositions[i]);
		glUniform1f(pointLightUniform_LearnOGL[i].linear, 0.09f);
		glUniform1f(pointLightUniform_LearnOGL[i].constant, 1.0f);
		glUniform1f(pointLightUniform_LearnOGL[i].quadratic, 0.032f);

	}*/
}

void UnsetTerrianLearnOGLShader(void)
{
	// code
	glUseProgram(0);
}

void UninitializeTerrianLearnOGLShader(void)
{
	// code
	ReleaseShaderProgramObject(shaderProgramObject_TerrianLearnOGL);
}