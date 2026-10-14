#pragma once

#include "Common.h"

bool InitializeStaticModelShader(void);
void SetStaticModelShader(mat4*, vec4, vec3, float, float,int, int pbrEnabled = 0);
void UnsetStaticModelShader(void);
void UninitializeStaticModelShader(void);