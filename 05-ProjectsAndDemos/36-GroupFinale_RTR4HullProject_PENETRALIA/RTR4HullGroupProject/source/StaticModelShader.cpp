#include "StaticModelShader.h"
#define DIRECTIONAL_LIGHT_MEMBER_COUNT 4
#define POINT_LIGHT_MEMBER_COUNT 7

GLuint shaderProgramObject_StaticModel;
static GLuint modelMatrixUniform_StaticModel;
static GLuint viewMatrixUniform_StaticModel;
static GLuint projectionMatrixUniform_StaticModel;
static GLuint lightPositionUniform_StaticModel;
static GLuint viewPositionUniform_StaticModel;
static GLuint lKeypressed_StaticModel;
static GLuint isPBREnabled_StaticModel;
static GLuint linear_StaticModel;
static GLuint quadratic_StaticModel;

struct DirLightUniform
{
	GLuint la;
	GLuint ld;
	GLuint ls;
	GLuint lightDirection;
}dirLightUniform[1];

struct PointLightUniform
{
	GLuint la;
	GLuint ld;
	GLuint ls;
	GLuint lightPosition;
	GLuint constant;
	GLuint linear;
	GLuint quadratic;
}pointLightUniform[1];

bool InitializeStaticModelShader(void)
{
	SHADERSHADERTYPEKEYPAIR shadersToCreate[] = {
		GL_VERTEX_SHADER, "shader/static_model_loading/StaticModelLoadingShader.vert"
		, GL_FRAGMENT_SHADER, "shader/static_model_loading/StaticModelLoadingShader.frag"
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
		ErrorLog(NULL, "InitializeStaticModelShader()::CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_StaticModel = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_StaticModel
		, shaders
		, createdShadersCount
	);

	// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
	/*free(shaders);
	shaders = nullptr;*/

	ATTRIBTYPEATTRIBNAMEKEYPAIR attributesToBind[] = {
		AMC_ATTRIBUTE_POSITION, "a_position"
		, AMC_ATTRIBUTE_NORMAL, "a_normal"
		, AMC_ATTRIBUTE_TEXTURE0, "a_texcoord"
		, AMC_ATTRIBUTE_TANGENT, "a_tangent"
	};

	if (!BindShaderProgramAttribLocations(
		shaderProgramObject_StaticModel
		, attributesToBind
		, sizeof(attributesToBind) / sizeof(attributesToBind[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_StaticModel))
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
		shaderProgramObject_StaticModel
		, uniformBlocksToBindIndices
		, sizeof(uniformBlocksToBindIndices) / sizeof(uniformBlocksToBindIndices[0])
	))
	{
		ErrorLog(NULL, "InitializeStaticModelShader()::BindShaderProgramUniformBlockIndices() failed.");
		return(false);
	}
	else
	{
		DebugLog(NULL, "InitializeStaticModelShader()::BindShaderProgramUniformBlockIndices() succeeded.");
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation[] = {
		  &modelMatrixUniform_StaticModel, "u_modelMatrix"
		, &viewMatrixUniform_StaticModel, "u_viewMatrix"
		, &projectionMatrixUniform_StaticModel, "u_projectionMatrix"

		, &lightPositionUniform_StaticModel, "u_lightPosition"
		, &viewPositionUniform_StaticModel, "u_viewPosition"
		, &lKeypressed_StaticModel, "u_l_key_pressed"
		, &isPBREnabled_StaticModel, "u_pbr_enabled"
		, &quadratic_StaticModel, "u_quadratic"
		, &linear_StaticModel, "u_linear"
	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_StaticModel
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

	lightCount = sizeof(dirLightUniform) / sizeof(dirLightUniform[0]);
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
		DebugLog(NULL, "lightUniformsToBindLocation[%d].uniformName = %s", i + 0, uniformName);
		uniformNameLength = strlen(uniformName);
		lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 0].uniformName = (const char*)malloc(uniformNameLength + 1);
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
		DebugLog(NULL, "lightUniformsToBindLocation[%d].uniformName = %s", i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 2, uniformName);
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
		DebugLog(NULL, "lightUniformsToBindLocation[%d].uniformName = %s", i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 3, uniformName);
		uniformNameLength = strlen(uniformName);
		lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 3].uniformName = (const char*)malloc(uniformNameLength + 1);
		//lightUniformsToBindLocation[i + 3].uniformName = "u_dirLight[0].direction";
		if (!(lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 3].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for lightUniformsToBindLocation[%d].uniformName", i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 3);
			return false;
		}
		memset((void*)lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 3].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)lightUniformsToBindLocation[i * DIRECTIONAL_LIGHT_MEMBER_COUNT + 3].uniformName, (void*)uniformName, uniformNameLength + 1);
	}

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_StaticModel
		, lightUniformsToBindLocation
		, lightCount * DIRECTIONAL_LIGHT_MEMBER_COUNT						// TODO_00: Make enum for number of properties in the light structure.
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (lightUniformsToBindLocation)
	{
		for (int i = 0; i < lightCount; i++)
		{
			for (int j = 0; j < DIRECTIONAL_LIGHT_MEMBER_COUNT; j++)
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

	// point light

	lightCount = sizeof(pointLightUniform) / sizeof(pointLightUniform[0]);
	UNIFORMUNIFORMNAMEKEYPAIR* pointLightUniformsToBindLocation = (UNIFORMUNIFORMNAMEKEYPAIR*)calloc(lightCount, POINT_LIGHT_MEMBER_COUNT * sizeof(UNIFORMUNIFORMNAMEKEYPAIR));

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
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 4].pUniform = &(pointLightUniform[i].constant);
		sprintf(uniformName, "u_pointLight[%d].constant", i);
		DebugLog(NULL, "pointLightUniformsToBindLocation[%d].uniformName = %s", i * POINT_LIGHT_MEMBER_COUNT + 4, uniformName);
		uniformNameLength = strlen(uniformName);
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 4].uniformName = (const char*)malloc(uniformNameLength + 1);
		//pointLightUniformsToBindLocation[i + 3].uniformName = "u_dirLight[0].direction";
		if (!(pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 4].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for pointLightUniformsToBindLocation[%d].uniformName", i * POINT_LIGHT_MEMBER_COUNT + 3);
			return false;
		}
		memset((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 4].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 4].uniformName, (void*)uniformName, uniformNameLength + 1);

		// linear
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 5].pUniform = &(pointLightUniform[i].linear);
		sprintf(uniformName, "u_pointLight[%d].linear", i);
		DebugLog(NULL, "pointLightUniformsToBindLocation[%d].uniformName = %s", i * POINT_LIGHT_MEMBER_COUNT + 5, uniformName);
		uniformNameLength = strlen(uniformName);
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 5].uniformName = (const char*)malloc(uniformNameLength + 1);
		//pointLightUniformsToBindLocation[i + 3].uniformName = "u_dirLight[0].direction";
		if (!(pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 5].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for pointLightUniformsToBindLocation[%d].uniformName", i * POINT_LIGHT_MEMBER_COUNT + 3);
			return false;
		}
		memset((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 5].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 5].uniformName, (void*)uniformName, uniformNameLength + 1);

		// quadratic
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 6].pUniform = &(pointLightUniform[i].quadratic);
		sprintf(uniformName, "u_pointLight[%d].quadratic", i);
		DebugLog(NULL, "pointLightUniformsToBindLocation[%d].uniformName = %s", i * POINT_LIGHT_MEMBER_COUNT + 6, uniformName);
		uniformNameLength = strlen(uniformName);
		pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 6].uniformName = (const char*)malloc(uniformNameLength + 1);
		//pointLightUniformsToBindLocation[i + 3].uniformName = "u_dirLight[0].direction";
		if (!(pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 6].uniformName))
		{
			ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::malloc() failed for pointLightUniformsToBindLocation[%d].uniformName", i * POINT_LIGHT_MEMBER_COUNT + 6);
			return false;
		}
		memset((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 6].uniformName, 0, sizeof(uniformNameLength + 1));
		memcpy((void*)pointLightUniformsToBindLocation[i * POINT_LIGHT_MEMBER_COUNT + 6].uniformName, (void*)uniformName, uniformNameLength + 1);
	}

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_StaticModel
		, pointLightUniformsToBindLocation
		, lightCount * POINT_LIGHT_MEMBER_COUNT						// TODO_00: Make enum for number of properties in the light structure.
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (pointLightUniformsToBindLocation)
	{
		for (int i = 0; i < lightCount; i++)
		{
			for (int j = 0; j < POINT_LIGHT_MEMBER_COUNT; j++)
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

void SetStaticModelShader(mat4* modelMatrix, vec4 lightPos, vec3 cameraPos, float linear, float quadratic,int instanceCount, int pbrEnabled)
{
	glUseProgram(shaderProgramObject_StaticModel);

	glUniformMatrix4fv(modelMatrixUniform_StaticModel, instanceCount, GL_FALSE, (GLfloat*)(&modelMatrix[0]));
	
	glUniformMatrix4fv(viewMatrixUniform_StaticModel, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_StaticModel, 1, GL_FALSE, perspectiveProjectionMatrix);

	glUniform4fv(lightPositionUniform_StaticModel, 1, lightPos);
	glUniform3fv(viewPositionUniform_StaticModel, 1, cameraPos);
	glUniform1i(lKeypressed_StaticModel, bLight);
	glUniform1i(isPBREnabled_StaticModel, pbrEnabled);

	glUniform1f(quadratic_StaticModel, quadratic);
	glUniform1f(linear_StaticModel, linear);

	for (unsigned int i = 0; i < 1; i++)
	{
		glUniform3fv(dirLightUniform[i].la, 1, vec3(1.0, 1.0, 1.0));
		glUniform3fv(dirLightUniform[i].ld, 1, vec3(1.0, 1.0, 1.0));
		glUniform3fv(dirLightUniform[i].ls, 1, vec3(0.0, 0.0, 1.0));
		glUniform4fv(dirLightUniform[i].lightDirection, 1, vec4(100.0, 100.0, 100.0, 1.0));


		glUniform3fv(pointLightUniform[i].la, 1, vec3(1.0, 1.0, 1.0));
		glUniform3fv(pointLightUniform[i].ld, 1, vec3(1.0, 1.0, 1.0));
		glUniform3fv(pointLightUniform[i].ls, 1, vec3(1.0f, 1.0f, 1.0f));
		glUniform3fv(pointLightUniform[i].lightPosition, 1, lightPos);
		glUniform1f(pointLightUniform[i].linear, linear);
		glUniform1f(pointLightUniform[i].constant, 1.0f);
		glUniform1f(pointLightUniform[i].quadratic, quadratic);


	}
}

void UnsetStaticModelShader(void)
{
	glUseProgram(NULL);
}

void UninitializeStaticModelShader(void)
{
	ReleaseShaderProgramObject(shaderProgramObject_StaticModel);
}