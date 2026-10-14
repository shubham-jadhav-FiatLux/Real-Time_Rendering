#include "terrian/TerrianSuperBibleShader.h"
#include "Texture.h"

#define DIRECTIONAL_LIGHT_MEMBER_COUNT 4
#define POINT_LIGHT_MEMBER_COUNT 7

static GLuint shaderProgramObject_TerrianSuperBible;
static GLuint modelMatrixUniform_TerrianSuperBible;
static GLuint viewMatrixUniform_TerrianSuperBible;
static GLuint projectionMatrixUniform_TerrianSuperBible;
static GLuint mvpMatrixUniform_TerrianSuperBible;

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

//static GLuint uboDirectionalLight;

static struct DirLightUniform
{
	GLuint la;
	GLuint ld;
	GLuint ls;
	GLuint lightDirection;
}dirLightUniform[2];

static struct PointLightUniform
{
	GLuint la;
	GLuint ld;
	GLuint ls;
	GLuint lightPosition;
	GLuint constant;
	GLuint linear;
	GLuint quadratic;
}pointLightUniform[2];

bool InitializeTerrianSuperBibleShader(void)
{
	// code
	//shaderProgramObject_TerrianSuperBible = CreateShaderProgramObject("shader/TerrianSuperBibleShader.vs", "shader/TerrianSuperBibleShader.fs", ATTRIBUTE_ONLY_TerrianSuperBible);

	SHADERSHADERTYPEKEYPAIR shadersToCreate[] = {
		GL_VERTEX_SHADER, "shader/terrian/superbible/TerrianSuperBible.vert"
		,GL_TESS_CONTROL_SHADER, "shader/terrian/superbible/TerrianSuperBible.tcs"
		,GL_TESS_EVALUATION_SHADER, "shader/terrian/superbible/TerrianSuperBible.tes"
		, GL_FRAGMENT_SHADER, "shader/terrian/superbible/TerrianSuperBible.frag"
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
		ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_TerrianSuperBible = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_TerrianSuperBible
		, shaders
		, createdShadersCount
	);

	// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
	/*free(shaders);
	shaders = nullptr;*/

	ATTRIBTYPEATTRIBNAMEKEYPAIR attributesToBind[] = {
		AMC_ATTRIBUTE_POSITION, "a_position"
		//, AMC_ATTRIBUTE_TEXTURE0, "a_texcoord"
	};

	if (!BindShaderProgramAttribLocations(
		shaderProgramObject_TerrianSuperBible
		, attributesToBind
		, sizeof(attributesToBind) / sizeof(attributesToBind[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_TerrianSuperBible))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMBLOCKUNIFORMBLOCKBINDINGKEYPAIR uniformBlocksToBindIndices[] = {
		//UBO_DIRECTIONA_LIGHT, "DirectionalLight",
		UBO_LIGHTS, "Lights"
	};

	if (!BindShaderProgramUniformBlockIndices(
		shaderProgramObject_TerrianSuperBible
		, uniformBlocksToBindIndices
		, sizeof(uniformBlocksToBindIndices) / sizeof(uniformBlocksToBindIndices[0])
	))
	{
		ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::BindShaderProgramUniformBlockIndices() failed.");
		return(false);
	}
	else
	{
		DebugLog(NULL, "InitializeTerrianSuperBibleShader()::BindShaderProgramUniformBlockIndices() succeeded.");
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation[] = {
		 &modelMatrixUniform_TerrianSuperBible, "u_modelMatrix"
		, &viewMatrixUniform_TerrianSuperBible, "u_viewMatrix"
		, &projectionMatrixUniform_TerrianSuperBible, "u_projectionMatrix"
		/*,& mvpMatrixUniform_TerrianSuperBible, "u_mvpMatrix"*/

		, &dmap_depthUniform, "u_dmapDepth"
		, &displacementTextureUniform, "u_textureDisplacement"
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
		shaderProgramObject_TerrianSuperBible
		, uniformsToBindLocation
		, sizeof(uniformsToBindLocation) / sizeof(uniformsToBindLocation[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	int lightCount;
	char uniformName[512] = { '\0' };
	int uniformNameLength = 0;

	lightCount  = sizeof(dirLightUniform) / sizeof(dirLightUniform[0]);
	UNIFORMUNIFORMNAMEKEYPAIR* lightUniformsToBindLocation = (UNIFORMUNIFORMNAMEKEYPAIR*)calloc(lightCount, 4 * sizeof(UNIFORMUNIFORMNAMEKEYPAIR));
	
	if (!lightUniformsToBindLocation)
	{
		ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::Calloc() failed for lightUniformsToBindLocation.");
		return (false);
	}

	//TODO_00: De-allocate lightUniformsToBindLocation ellegently

	

	// TODO_00: Make it i+=4;
	for (int i = 0; i < lightCount; i++)
	{
		//la
		lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 0].pUniform = &(dirLightUniform[i].la);
		sprintf(uniformName, "u_dirLight[%d].ambient", i);
		DebugLog(NULL, "lightUniformsToBindLocation[%d].uniformName = %s", i+0, uniformName);
		uniformNameLength = strlen(uniformName);
		lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 0].uniformName = (const char*) malloc(uniformNameLength + 1);
		//lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 0].uniformName = "u_dirLight[0].ambient";
		if (!(lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 0].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for lightUniformsToBindLocation[%d].uniformName", i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 0);
			return false;
		}
		memset((void*)lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 0].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 0].uniformName, (void*)uniformName, uniformNameLength + 1);

		//ld
		lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 1].pUniform = &(dirLightUniform[i].ld);
		sprintf(uniformName, "u_dirLight[%d].diffuse", i);
		DebugLog(NULL, "lightUniformsToBindLocation[%d].uniformName = %s", i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 1, uniformName);
		uniformNameLength = strlen(uniformName);
		lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 1].uniformName = (const char*)malloc(uniformNameLength + 1);
		//lightUniformsToBindLocation[i + 1].uniformName = "u_dirLight[0].diffuse";
		if (!(lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 1].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for lightUniformsToBindLocation[%d].uniformName", i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 1);
			return false;
		}
		memset((void*)lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 1].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 1].uniformName, (void*)uniformName, uniformNameLength + 1);

		//ls
		lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 2].pUniform = &(dirLightUniform[i].ls);
		sprintf(uniformName, "u_dirLight[%d].specular", i);
		DebugLog(NULL, "lightUniformsToBindLocation[%d].uniformName = %s", i* DIRECTIONAL_LIGHT_MEMBER_COUNT + 2, uniformName);
		uniformNameLength = strlen(uniformName);
		lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 2].uniformName = (const char*)malloc(uniformNameLength + 1);
		//lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 2].uniformName = "u_dirLight[0].specular";
		if (!(lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 2].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for lightUniformsToBindLocation[%d].uniformName", i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 2);
			return false;
		}
		memset((void*)lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 2].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 2].uniformName, (void*)uniformName, uniformNameLength + 1);

		//lightDirection
		lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 3].pUniform = &(dirLightUniform[i].lightDirection);
		sprintf(uniformName, "u_dirLight[%d].direction", i);
		DebugLog(NULL, "lightUniformsToBindLocation[%d].uniformName = %s", i* DIRECTIONAL_LIGHT_MEMBER_COUNT + 3, uniformName);
		uniformNameLength = strlen(uniformName);
		lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 3].uniformName = (const char*)malloc(uniformNameLength + 1);
		//lightUniformsToBindLocation[i + 3].uniformName = "u_dirLight[0].direction";
		if (!(lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 3].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for lightUniformsToBindLocation[%d].uniformName", i* DIRECTIONAL_LIGHT_MEMBER_COUNT + 3);
			return false;
		}
		memset((void*)lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 3].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 3].uniformName, (void*)uniformName, uniformNameLength + 1);
	}

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_TerrianSuperBible
		, lightUniformsToBindLocation
		, lightCount * 4						// TODO_00: Make enum for number of properties in the light structure.
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (lightUniformsToBindLocation)
	{
		for (int i = 0; i < lightCount; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (lightUniformsToBindLocation[i + j].uniformName)
				{
					free((void*)(lightUniformsToBindLocation[i + j].uniformName));
					lightUniformsToBindLocation[i + j].uniformName = NULL;
				}
			}
		}
		free(lightUniformsToBindLocation);
		lightUniformsToBindLocation = NULL;
	}

	// for point light

	lightCount = sizeof(pointLightUniform) / sizeof(pointLightUniform[0]);
	UNIFORMUNIFORMNAMEKEYPAIR* pointLightUniformsToBindLocation = (UNIFORMUNIFORMNAMEKEYPAIR*)calloc(lightCount, 4 * sizeof(UNIFORMUNIFORMNAMEKEYPAIR));

	if (!pointLightUniformsToBindLocation)
	{
		ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::Calloc() failed for point lightUniformsToBindLocation.");
		return (false);
	}

	//TODO_00: De-allocate lightUniformsToBindLocation ellegently

	// TODO_00: Make it i+=4;
	for (int i = 0; i < lightCount; i++)
	{
		//la
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 0].pUniform = &(pointLightUniform[i].la);
		sprintf(uniformName, "u_pointLight[%d].ambient", i);
		DebugLog(NULL, "pointLightUniformsToBindLocation[%d].uniformName = %s", i + 0, uniformName);
		uniformNameLength = strlen(uniformName);
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 0].uniformName = (const char*)malloc(uniformNameLength + 1);
		//pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 0].uniformName = "u_dirLight[0].ambient";
		if (!(pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 0].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for pointLightUniformsToBindLocation[%d].uniformName", i * POINT_LIGHT_MEMBER_COUNT + 0);
			return false;
		}
		memset((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 0].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 0].uniformName, (void*)uniformName, uniformNameLength + 1);

		//ld
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 1].pUniform = &(pointLightUniform[i].ld);
		sprintf(uniformName, "u_pointLight[%d].diffuse", i);
		DebugLog(NULL, "pointLightUniformsToBindLocation[%d].uniformName = %s", i * POINT_LIGHT_MEMBER_COUNT + 1, uniformName);
		uniformNameLength = strlen(uniformName);
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 1].uniformName = (const char*)malloc(uniformNameLength + 1);
		//pointLightUniformsToBindLocation[i + 1].uniformName = "u_dirLight[0].diffuse";
		if (!(pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 1].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for pointLightUniformsToBindLocation[%d].uniformName", i * POINT_LIGHT_MEMBER_COUNT + 1);
			return false;
		}
		memset((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 1].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 1].uniformName, (void*)uniformName, uniformNameLength + 1);

		//ls
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 2].pUniform = &(pointLightUniform[i].ls);
		sprintf(uniformName, "u_pointLight[%d].specular", i);
		DebugLog(NULL, "pointLightUniformsToBindLocation[%d].uniformName = %s", i * POINT_LIGHT_MEMBER_COUNT + 2, uniformName);
		uniformNameLength = strlen(uniformName);
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 2].uniformName = (const char*)malloc(uniformNameLength + 1);
		//pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 2].uniformName = "u_dirLight[0].specular";
		if (!(pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 2].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for pointLightUniformsToBindLocation[%d].uniformName", i * POINT_LIGHT_MEMBER_COUNT + 2);
			return false;
		}
		memset((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 2].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 2].uniformName, (void*)uniformName, uniformNameLength + 1);

		//lightPosition
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].pUniform = &(pointLightUniform[i].lightPosition);
		sprintf(uniformName, "u_pointLight[%d].position", i);
		DebugLog(NULL, "pointLightUniformsToBindLocation[%d].uniformName = %s", i * POINT_LIGHT_MEMBER_COUNT + 3, uniformName);
		uniformNameLength = strlen(uniformName);
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName = (const char*)malloc(uniformNameLength + 1);
		//pointLightUniformsToBindLocation[i + 3].uniformName = "u_dirLight[0].direction";
		if (!(pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for pointLightUniformsToBindLocation[%d].uniformName", i * POINT_LIGHT_MEMBER_COUNT + 3);
			return false;
		}
		memset((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName, (void*)uniformName, uniformNameLength + 1);

		// constant
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].pUniform = &(pointLightUniform[i].constant);
		sprintf(uniformName, "u_pointLight[%d].position", i);
		DebugLog(NULL, "pointLightUniformsToBindLocation[%d].uniformName = %s", i * POINT_LIGHT_MEMBER_COUNT + 3, uniformName);
		uniformNameLength = strlen(uniformName);
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName = (const char*)malloc(uniformNameLength + 1);
		//pointLightUniformsToBindLocation[i + 3].uniformName = "u_dirLight[0].direction";
		if (!(pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for pointLightUniformsToBindLocation[%d].uniformName", i * POINT_LIGHT_MEMBER_COUNT + 3);
			return false;
		}
		memset((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName, (void*)uniformName, uniformNameLength + 1);

		// linear
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].pUniform = &(pointLightUniform[i].linear);
		sprintf(uniformName, "u_pointLight[%d].position", i);
		DebugLog(NULL, "pointLightUniformsToBindLocation[%d].uniformName = %s", i* POINT_LIGHT_MEMBER_COUNT + 3, uniformName);
		uniformNameLength = strlen(uniformName);
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName = (const char*)malloc(uniformNameLength + 1);
		//pointLightUniformsToBindLocation[i + 3].uniformName = "u_dirLight[0].direction";
		if (!(pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for pointLightUniformsToBindLocation[%d].uniformName", i * POINT_LIGHT_MEMBER_COUNT + 3);
			return false;
		}
		memset((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName, (void*)uniformName, uniformNameLength + 1);

		// quadratic
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].pUniform = &(pointLightUniform[i].quadratic);
		sprintf(uniformName, "u_pointLight[%d].position", i);
		DebugLog(NULL, "pointLightUniformsToBindLocation[%d].uniformName = %s", i* POINT_LIGHT_MEMBER_COUNT + 3, uniformName);
		uniformNameLength = strlen(uniformName);
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName = (const char*)malloc(uniformNameLength + 1);
		//pointLightUniformsToBindLocation[i + 3].uniformName = "u_dirLight[0].direction";
		if (!(pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for pointLightUniformsToBindLocation[%d].uniformName", i * POINT_LIGHT_MEMBER_COUNT + 3);
			return false;
		}
		memset((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 3].uniformName, (void*)uniformName, uniformNameLength + 1);
	}

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_TerrianSuperBible
		, pointLightUniformsToBindLocation
		, lightCount * 4						// TODO_00: Make enum for number of properties in the light structure.
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (pointLightUniformsToBindLocation)
	{
		for (int i = 0; i < lightCount; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (pointLightUniformsToBindLocation[i + j].uniformName)
				{
					free((void*)(pointLightUniformsToBindLocation[i + j].uniformName));
					pointLightUniformsToBindLocation[i + j].uniformName = NULL;
				}
			}
		}
		free(pointLightUniformsToBindLocation);
		pointLightUniformsToBindLocation = NULL;
	}
	return(true);
}

void SetTerrianSuperBibleShader(mat4 modelMatrix, GLuint displacementTexture, GLuint colorTexture, vec4 lightPos,
	GLuint multitextureBackground, GLuint redTexture, GLuint greenTexture, GLuint blueTexture, GLuint iblendMap, GLuint light, struct DirLight dirLight[], unsigned int lightCount, vec3 cameraPos)
{
	// code
	glUseProgram(shaderProgramObject_TerrianSuperBible);

	glUniformMatrix4fv(modelMatrixUniform_TerrianSuperBible, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_TerrianSuperBible, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_TerrianSuperBible, 1, GL_FALSE, perspectiveProjectionMatrix);
	
	glUniformMatrix4fv(mvpMatrixUniform_TerrianSuperBible, 1, GL_FALSE, (perspectiveProjectionMatrix * viewMatrix * modelMatrix));
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

	for (unsigned int i = 0; i < lightCount; i++)
	{
		glUniform3fv(dirLightUniform[i].la, 1, dirLight[i].ambient);
		glUniform3fv(dirLightUniform[i].ld, 1, dirLight[i].diffuse);
		glUniform3fv(dirLightUniform[i].ls, 1, dirLight[i].specular);
		glUniform4fv(dirLightUniform[i].lightDirection, 1, dirLight[i].direction);
	}
}

void UnsetTerrianSuperBibleShader(void)
{
	// code
	glUseProgram(0);
}

void UninitializeTerrianSuperBibleShader(void)
{
	// code
	ReleaseShaderProgramObject(shaderProgramObject_TerrianSuperBible);
}