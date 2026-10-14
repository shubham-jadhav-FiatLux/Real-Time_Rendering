#pragma once

#include "Common.h"

bool InitializeTextureShader(void);
void SetTextureShader(mat4, GLuint);
void UnsetTextureShader(void);
void UninitializeTextureShader(void);
