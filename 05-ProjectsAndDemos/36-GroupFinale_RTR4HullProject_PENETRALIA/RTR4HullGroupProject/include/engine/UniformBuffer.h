#pragma once

#include "Common.h"
#include <engine/ShaderLayouts.h>

#define MAX_DIRECTIONAL_LIGHTS_COUNT 10
#define MAX_POINT_LIGHTS_COUNT 10

#pragma region UniformBufferObjectsDataTypes

typedef struct UniformBufferObject
{
	GLuint uniform;
	UniformBlockLayout layout;
} UNIFORMBUFFEROBJECT;

#pragma region UniformBufferObjectLightsDataTypes

#pragma region DirectionalLightsDataTypes

typedef struct DirectionalLightNew
{
	vec4 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
} DIRECTIONALLIGHT;

typedef struct Lights
{
	DirectionalLightNew directionalLights[MAX_DIRECTIONAL_LIGHTS_COUNT];
	int directionalLightCount;
	//int padding_642_to_643;
} LIGHTS;

typedef struct LightsUBO
{
	UNIFORMBUFFEROBJECT ubo;
	LIGHTS data;
	bool isUpdated;
} LIGHTSUBO;

#pragma endregion DirectionalLightsDataTypes

#pragma region PointLightsDataTypes

typedef struct PointLightNewMirror
{
							// size
	vec4 position;			// 16
	vec4 ambient;			// 16
	vec4 diffuse;			// 16
	vec4 specular;			// 16
	float constant;			// 4
	float linear;			// 4
	float quadratic;		// 4
	float padding_77_to_80;	// 4
} POINTLIGHTMIRROR;			// 80

typedef struct UBOPointLightsMirror
{
															// size
	POINTLIGHTMIRROR pointLights[MAX_POINT_LIGHTS_COUNT];	// 800 (80 * 10)
	int pointLightCount;									// 4
} UBOPOINTLIGHTSMIRROR;										// 804

typedef struct PointLightsUBO
{
	UNIFORMBUFFEROBJECT ubo;
	UBOPOINTLIGHTSMIRROR data;
	bool isUpdated;
} POINTLIGHTSUBO;

#pragma endregion PointLightsDataTypes

#pragma endregion UniformBufferObjectLightsDataTypes

#pragma endregion UniformBufferObjectsDataTypes

#pragma region UniformBufferObjects

extern LIGHTSUBO gLightsUBO;
extern POINTLIGHTSUBO gPointLightsUBO;

#pragma endregion UniformBufferObjects

#pragma region UniformBufferAPIs

/// <summary>
/// This function initializes all the uniform buffer objects.
/// </summary>
/// <param name=""></param>
/// <returns></returns>
bool InitializeUBOs(void);

/// <summary>
/// This function binds all the uniform buffer objects.
/// </summary>
/// <param name=""></param>
/// <returns></returns>
bool BindUBOs(void);

/// <summary>
/// Update/Set all the uniform buffer objects.
/// </summary>
/// <param name=""></param>
void SetUBOs(void);

/// <summary>
/// Uninitialize all the uniform buffer objects.
/// </summary>
/// <param name=""></param>
void UninitializeUBOs(void);

#pragma endregion UniformBufferAPIs
