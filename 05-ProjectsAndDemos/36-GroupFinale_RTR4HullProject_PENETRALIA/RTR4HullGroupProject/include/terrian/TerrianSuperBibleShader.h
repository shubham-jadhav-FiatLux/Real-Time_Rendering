#pragma once

#include "Common.h"

bool InitializeTerrianSuperBibleShader(void);
void SetTerrianSuperBibleShader(mat4, GLuint, GLuint, vec4, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, struct DirLight[], unsigned int, vec3);
void UnsetTerrianSuperBibleShader(void);
void UninitializeTerrianSuperBibleShader(void);
