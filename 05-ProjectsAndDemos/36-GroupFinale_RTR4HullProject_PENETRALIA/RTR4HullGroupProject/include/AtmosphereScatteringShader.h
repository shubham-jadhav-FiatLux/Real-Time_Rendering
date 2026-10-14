#pragma once

#include "Common.h"

bool InitializeAtmosphereScatteringShader(void);
void SetAtmosphereScatteringShader(mat4 modelMatrix, vec3 cameraPosition, vec3 lightPosition, float cameraHeight, float outerSphereRadius, float innerSphereRadius, float krESun, float kmESun, float kr4PI, float km4PI, float gScale);
void UnsetAtmosphereScatteringShader(void);
void UninitializeAtmosphereScatteringShader(void);
