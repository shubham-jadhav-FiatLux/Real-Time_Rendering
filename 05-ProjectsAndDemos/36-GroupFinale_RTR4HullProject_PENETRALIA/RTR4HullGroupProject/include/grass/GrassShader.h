#pragma once

#include "Common.h"

bool InitializeGrassShader(void);
void SetGrassShader(mat4 , GLuint , GLuint , vec3 , vec3 );
void UnsetGrassShader(void);
void UnInitializeGrassShader(void);
