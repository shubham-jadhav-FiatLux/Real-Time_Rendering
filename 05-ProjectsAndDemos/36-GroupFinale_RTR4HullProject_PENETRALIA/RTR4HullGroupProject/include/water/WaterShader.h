#pragma once

#include "Common.h"

bool InitializeWaterShader(void);
void SetWaterShader(mat4 , GLuint , GLuint , GLuint,  GLuint, GLuint , GLfloat , vec3 , vec3 );
void UnsetWaterShader(void);
void UninitializeWaterShader(void);
