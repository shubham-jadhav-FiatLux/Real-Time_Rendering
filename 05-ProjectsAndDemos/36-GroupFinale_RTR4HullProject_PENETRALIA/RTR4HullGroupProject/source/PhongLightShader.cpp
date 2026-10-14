#include "PhongLightShader.h"

static GLuint shaderProgramObject_PhongADSLight;

static GLuint modelMatrixUniform_PhongADSLight;
static GLuint viewMatrixUniform_PhongADSLight;
static GLuint projectionMatrixUniform_PhongADSLight;

static GLuint lAUniform_PhongADSLight;
static GLuint lDUniform_PhongADSLight;
static GLuint lSUniform_PhongADSLight;
static GLuint lightPositionUniform_PhongADSLight;

static GLuint kAUniform_PhongADSLight;
static GLuint kDUniform_PhongADSLight;
static GLuint kSUniform_PhongADSLight;
static GLuint materialShininessUniform_PhongADSLight;

static GLuint lightEnabledUniform_PhongADSLight;

bool InitializePhongLightShader(void)
{
	// code
	shaderProgramObject_PhongADSLight = CreateShaderProgramObject("shader/lights/PhongADSLight.vert", "shader/lights/PhongADSLight.vert");

	modelMatrixUniform_PhongADSLight = glGetUniformLocation(shaderProgramObject_PhongADSLight, "u_modelMatrix");
	viewMatrixUniform_PhongADSLight = glGetUniformLocation(shaderProgramObject_PhongADSLight, "u_viewMatrix");
	projectionMatrixUniform_PhongADSLight = glGetUniformLocation(shaderProgramObject_PhongADSLight, "u_projectionMatrix");

	lAUniform_PhongADSLight = glGetUniformLocation(shaderProgramObject_PhongADSLight, "u_lA");
	lDUniform_PhongADSLight = glGetUniformLocation(shaderProgramObject_PhongADSLight, "u_lD");
	lSUniform_PhongADSLight = glGetUniformLocation(shaderProgramObject_PhongADSLight, "u_lS");
	lightPositionUniform_PhongADSLight = glGetUniformLocation(shaderProgramObject_PhongADSLight, "u_lightPosition");

	kAUniform_PhongADSLight = glGetUniformLocation(shaderProgramObject_PhongADSLight, "u_kA");
	kDUniform_PhongADSLight = glGetUniformLocation(shaderProgramObject_PhongADSLight, "u_kD");
	kSUniform_PhongADSLight = glGetUniformLocation(shaderProgramObject_PhongADSLight, "u_kS");
	materialShininessUniform_PhongADSLight = glGetUniformLocation(shaderProgramObject_PhongADSLight, "u_materialShininess");

	lightEnabledUniform_PhongADSLight = glGetUniformLocation(shaderProgramObject_PhongADSLight, "u_lightingEnabled");

	return true;
}

void SetPhongLightShader(mat4 modelMatrix, vec3 lightAmbient, vec3 lightDiffuse, vec3 lightSpecular, vec4 lightPosition, vec3 materialAmbient, vec3 materialDiffuse, vec3 materialSpecular, float materialShininess)
{
	// code
	glUseProgram(shaderProgramObject_PhongADSLight);

	if (bLight == true)
	{
		glUniform1i(lightEnabledUniform_PhongADSLight, 1);
		glUniform3fv(lAUniform_PhongADSLight, 1, lightAmbient);
		glUniform3fv(lDUniform_PhongADSLight, 1, lightDiffuse);
		glUniform3fv(lSUniform_PhongADSLight, 1, lightSpecular);
		glUniform4fv(lightPositionUniform_PhongADSLight, 1, lightPosition);

		glUniform3fv(kAUniform_PhongADSLight, 1, materialAmbient);
		glUniform3fv(kDUniform_PhongADSLight, 1, materialDiffuse);
		glUniform3fv(kSUniform_PhongADSLight, 1, materialSpecular);
		glUniform1f(materialShininessUniform_PhongADSLight, materialShininess);
	}
	else
	{
		glUniform1i(lightEnabledUniform_PhongADSLight, 0);
	}
	
	glUniformMatrix4fv(modelMatrixUniform_PhongADSLight, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_PhongADSLight, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_PhongADSLight, 1, GL_FALSE, perspectiveProjectionMatrix);
}

void UnsetPhongLightShader(void)
{
	// code
	glUseProgram(0);
}

void UninitializePhongLightShader(void)
{

}