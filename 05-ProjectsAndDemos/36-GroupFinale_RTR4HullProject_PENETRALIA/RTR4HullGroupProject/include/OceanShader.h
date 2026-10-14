#pragma once

#include "Common.h"

bool InitializeOceanShader(void);
void SetOceanShader(mat4, float,float,vec2, vec4, vec4, vec4, vec3);
void UnsetOceanShader(void);
void UninitializeOceanShader(void);
