#include <CubeRender.h>
#include <test/TestPointLight.h>
#include <ColorShader.h>
#include <engine/UniformBuffer.h>

bool InitializeTestPointLight(void)
{
	// code

	// TODO_01: The initializations should be conditional initializations to avoid reinitializations.
	// InitializeColorShader();	// This is already initialized while initializing common shaders.

	return true;
}

void DisplayTestPointLight(void)
{
	if (TEST_POINT_LIGHT)
	{
		mat4 modelMatrix = vmath::mat4::identity();

		for (unsigned int pointLightIndex = 0; pointLightIndex < gPointLightsUBO.data.pointLightCount && pointLightIndex < MAX_POINT_LIGHTS_COUNT; pointLightIndex++)
		{
			vec4 currentPointLightPosition = gPointLightsUBO.data.pointLights[pointLightIndex].position;
			vec4 currentPointLightColor = gPointLightsUBO.data.pointLights[pointLightIndex].diffuse;

			modelMatrix = vmath::translate(currentPointLightPosition[0], currentPointLightPosition[1], currentPointLightPosition[2]);

			SetColorShader(modelMatrix, currentPointLightColor);

			RenderCube();

			UnsetColorShader();
		}
	}
}

void UninitializeTestPointLight(void)
{
	// UninitializeColorShader();	// This is already uninitialized while uninitializing common shaders.
}