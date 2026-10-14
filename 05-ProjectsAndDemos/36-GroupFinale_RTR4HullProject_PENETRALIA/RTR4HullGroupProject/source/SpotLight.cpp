#include "SpotLight.h"

static GLuint shaderProgramObject_SpotLight;

static GLuint modelMatrixUniform;
static GLuint viewMatrixUniform;
static GLuint projectionMatrixUniform;

static GLuint lightingEnabledUniform;

static GLuint laUniform;
static GLuint ldUniform;
static GLuint lsUniform;
static GLuint lpositionUniform;
static GLuint ldirectionUniform;
static GLuint lviewposUniform;
static GLuint lcutOffUniform;
static GLuint louterCutOffUniform;
static GLuint lconstantUniform;
static GLuint llinearUniform;
static GLuint lquadraticUniform;

static GLuint kaUniform;
static GLuint kdUniform;
static GLuint ksUniform;
static GLuint materialShininessUniform;

bool InitializeSpotLight(void)
{
	SHADERSHADERTYPEKEYPAIR shadersToCreate[] = {
		{GL_VERTEX_SHADER, "shader/lights/SpotLight.vert"}
		, {GL_FRAGMENT_SHADER, "shader/lights/SpotLight.frag"}
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
		// TODO: Log error here.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	// RESUME_POINT: Refactor the earlier code and start implementing below functions.

	shaderProgramObject_SpotLight = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_SpotLight
		, shaders
		, createdShadersCount
	);

	/*free(shaders);
	shaders = nullptr;*/

	ATTRIBTYPEATTRIBNAMEKEYPAIR attributesToBind[] = {
		AMC_ATTRIBUTE_POSITION, "a_position"
		, AMC_ATTRIBUTE_COLOR, "a_color"
		, AMC_ATTRIBUTE_NORMAL, "a_normals"
	};

	if (!BindShaderProgramAttribLocations(
		shaderProgramObject_SpotLight
		, attributesToBind
		, sizeof(attributesToBind) / sizeof(attributesToBind[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_SpotLight))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation[] = {
		&modelMatrixUniform, "u_modelMatrix"
		, &viewMatrixUniform, "u_viewMatrix"
		, &projectionMatrixUniform, "u_projectionMatrix"

		, &lightingEnabledUniform, "u_lightingEnabled"

		, &laUniform, "u_la"
		, &ldUniform, "u_ld"
		, &lsUniform, "u_ls"
		, &lpositionUniform, "u_lposition"
		, &ldirectionUniform, "u_ldirection"
		, &lviewposUniform, "u_lviewpos"
		, &lcutOffUniform, "u_lcutOff"
		, &louterCutOffUniform, "u_louterCutOff"
		, &lconstantUniform, "u_lconstant"
		, &llinearUniform, "u_llinear"
		, &lquadraticUniform, "u_lquadratic"

		, &kaUniform, "u_ka"
		, &kdUniform, "u_kd"
		, &ksUniform, "u_ks"
		, &materialShininessUniform, "u_materialShininess"
	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_SpotLight
		, uniformsToBindLocation
		, sizeof(uniformsToBindLocation) / sizeof(uniformsToBindLocation[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	return(true);
}
