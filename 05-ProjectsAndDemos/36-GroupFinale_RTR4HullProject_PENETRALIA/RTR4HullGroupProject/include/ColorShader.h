#pragma once

#include "Common.h"

bool InitializeColorShader(void);
void SetColorShader(mat4 modelMatrix, vec4 color);
void UnsetColorShader(void);
void UninitializeColorShader(void);
