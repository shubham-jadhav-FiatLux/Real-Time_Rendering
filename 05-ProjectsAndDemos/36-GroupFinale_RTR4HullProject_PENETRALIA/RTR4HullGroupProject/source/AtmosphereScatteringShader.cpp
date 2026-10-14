#include "AtmosphereScatteringShader.h"

#define RAYLEIGHT_CONSTANT 0.0055
#define MIE_CONSTANT 0.0015

static GLuint shaderProgramObject_AtmosphereScattering;

static GLuint modelMatrixUniform_AtmosphereFromSky;
static GLuint viewMatrixUniform_AtmosphereFromSky;
static GLuint projectionMatrixUniform_AtmosphereFromSky;

static GLuint cameraPositionUniform_AtmosphereFromSky;
static GLuint lightPositionUniform_AtmosphereFromSky;
static GLuint inWaveLengthUniform_AtmosphereFromSky;
static GLuint cameraHeightUniform_AtmosphereFromSky;
static GLuint cameraHeight2Uniform_AtmosphereFromSky;
static GLuint outerRadiusUniform_AtmosphereFromSky;
static GLuint outerRadius2Uniform_AtmosphereFromSky;
static GLuint innerRadiusUniform_AtmosphereFromSky;
static GLuint innerRadius2Uniform_AtmosphereFromSky;
static GLuint krESunUniform_AtmosphereFromSky;
static GLuint kmESunUniform_AtmosphereFromSky;
static GLuint kr4PIUniform_AtmosphereFromSky;
static GLuint km4PIUniform_AtmosphereFromSky;
static GLuint scaleUniform_AtmosphereFromSky;
static GLuint scaleDepthUniform_AtmosphereFromSky;
static GLuint scaleOverScaleDepthUniform_AtmosphereFromSky;
static GLuint gUniform_AtmosphereFromSky;
static GLuint g2Uniform_AtmosphereFromSky;

float g = -0.99f;
float scaleDepth = 0.25f;
vec4 waveLength;

bool InitializeAtmosphereScatteringShader(void)
{
	// code

	SHADERSHADERTYPEKEYPAIR shadersToCreate[] = {
		GL_VERTEX_SHADER, "shader/atmosphericscattering/SkyFromAtmosphereShader.vert"
		, GL_FRAGMENT_SHADER, "shader/atmosphericscattering/SkyFromAtmosphereShader.frag"
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
		ErrorLog(NULL, "InitializeAtmosphereScatteringShader()::CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_AtmosphereScattering = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_AtmosphereScattering
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
		shaderProgramObject_AtmosphereScattering
		, attributesToBind
		, sizeof(attributesToBind) / sizeof(attributesToBind[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_AtmosphereScattering))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation[] = {
		&modelMatrixUniform_AtmosphereFromSky, "u_modelMatrix"
		, &viewMatrixUniform_AtmosphereFromSky, "u_viewMatrix"
		, &projectionMatrixUniform_AtmosphereFromSky, "u_projectionMatrix"

	    , &cameraPositionUniform_AtmosphereFromSky, "cameraPosition"
		, &lightPositionUniform_AtmosphereFromSky, "lightPosition"
		, &inWaveLengthUniform_AtmosphereFromSky, "inWaveLength"
		, &cameraHeightUniform_AtmosphereFromSky, "cameraHeight"
		, &cameraHeight2Uniform_AtmosphereFromSky, "cameraHeight2"
		, &outerRadiusUniform_AtmosphereFromSky, "outerRadius"
		, &outerRadius2Uniform_AtmosphereFromSky, "outerRadius2"
		, &innerRadiusUniform_AtmosphereFromSky, "innerRadius"
		, &innerRadius2Uniform_AtmosphereFromSky, "innerRadius2"
		, &krESunUniform_AtmosphereFromSky, "krESun"
		, &kmESunUniform_AtmosphereFromSky, "kmESun"
		, &kr4PIUniform_AtmosphereFromSky, "kr4PI"
		, &km4PIUniform_AtmosphereFromSky, "km4PI"
		, &scaleUniform_AtmosphereFromSky, "scale"
		, &scaleDepthUniform_AtmosphereFromSky, "scaleDepth"
		, &scaleOverScaleDepthUniform_AtmosphereFromSky, "scaleOverScaleDepth"
		, &gUniform_AtmosphereFromSky, "g"
		, &g2Uniform_AtmosphereFromSky, "g2"
	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_AtmosphereScattering
		, uniformsToBindLocation
		, sizeof(uniformsToBindLocation) / sizeof(uniformsToBindLocation[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	// set the wavelength
	float waveLengthX = 1.0f / (powf(0.65f, 4.0f));
	float waveLengthY = 1.0f / (powf(0.57f, 4.0f));
	float waveLengthZ = 1.0f / (powf(0.475f, 4.0f));

	waveLength = vec4(waveLengthX, waveLengthY, waveLengthZ, 0.0);

	return(true);
}

void SetAtmosphereScatteringShader(mat4 modelMatrix, vec3 cameraPos, vec3 lightPosition, float cameraHeight, float outerSphereRadius, float innerSphereRadius, float krESun, float kmESun, float kr4PI, float km4PI, float scale)
{
	// code
	glUseProgram(shaderProgramObject_AtmosphereScattering);

	static bool flag = true;
	if (flag)
	{
		flag = false;
		ErrorLog(NULL, "outerSphereRadius : %f\n", outerSphereRadius);
		ErrorLog(NULL, "innerSphereRadius : %f\n", innerSphereRadius);
		ErrorLog(NULL, "cameraHeight : %f\n", cameraHeight);
		ErrorLog(NULL, "camPos[%f, %f, %f]\n", cameraPos[0], cameraPos[1], cameraPos[2]);
		ErrorLog(NULL, "lightPos[%f, %f, %f]\n", lightPosition[0], lightPosition[1], lightPosition[2]);
		ErrorLog(NULL, "waveLength[%f, %f, %f]\n", waveLength[0], waveLength[1], waveLength[2]);
	}

	glUniformMatrix4fv(modelMatrixUniform_AtmosphereFromSky, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_AtmosphereFromSky, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_AtmosphereFromSky, 1, GL_FALSE, perspectiveProjectionMatrix);

	glUniform3fv(cameraPositionUniform_AtmosphereFromSky, 1, cameraPos);
	glUniform3fv(lightPositionUniform_AtmosphereFromSky, 1, lightPosition);
	glUniform4fv(inWaveLengthUniform_AtmosphereFromSky, 1, waveLength);

	glUniform1f(cameraHeightUniform_AtmosphereFromSky, cameraHeight);
	glUniform1f(cameraHeight2Uniform_AtmosphereFromSky, cameraHeight * cameraHeight);
	glUniform1f(outerRadiusUniform_AtmosphereFromSky, outerSphereRadius);
	glUniform1f(outerRadius2Uniform_AtmosphereFromSky, outerSphereRadius * outerSphereRadius);
	glUniform1f(innerRadiusUniform_AtmosphereFromSky, innerSphereRadius);
	glUniform1f(innerRadius2Uniform_AtmosphereFromSky, innerSphereRadius * innerSphereRadius);
	glUniform1f(krESunUniform_AtmosphereFromSky, (float)RAYLEIGHT_CONSTANT * 20.0f);
	glUniform1f(kmESunUniform_AtmosphereFromSky, (float)MIE_CONSTANT * 20.0f);
	glUniform1f(kr4PIUniform_AtmosphereFromSky, (float)RAYLEIGHT_CONSTANT * 4 * (float)M_PI);
	glUniform1f(km4PIUniform_AtmosphereFromSky, (float)MIE_CONSTANT * 4 * (float)M_PI);
	glUniform1f(scaleUniform_AtmosphereFromSky, scale);
	glUniform1f(scaleDepthUniform_AtmosphereFromSky, scaleDepth);
	glUniform1f(scaleOverScaleDepthUniform_AtmosphereFromSky, scale / scaleDepth);

	glUniform1f(gUniform_AtmosphereFromSky, g);
	glUniform1f(g2Uniform_AtmosphereFromSky, g * g);
}

void UnsetAtmosphereScatteringShader(void)
{
	// code
	glUseProgram(0);
}

void UninitializeAtmosphereScatteringShader(void)
{
	// code
	ReleaseShaderProgramObject(shaderProgramObject_AtmosphereScattering);
}