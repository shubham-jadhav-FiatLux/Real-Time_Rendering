#pragma once

#include "Common.h"

bool InitializeMSAAShader(void);
void SetMSAAShader(mat4, GLuint);
void UnsetMSAAShader(void);
void UninitializeMSAAShader(void);
