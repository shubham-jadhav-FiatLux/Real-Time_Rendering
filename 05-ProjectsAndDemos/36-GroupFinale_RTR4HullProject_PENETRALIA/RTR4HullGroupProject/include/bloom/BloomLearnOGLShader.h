#pragma once
#include "Common.h"

bool InitializeBloomLearnOGLShader(void);
void DisplayBloomLearnOGLShader(void);
bool UpdateBloomLearnOGLShader(void);
void UninitializeBloomLearnOGLShader(void);
void SetBloomBrightnessShader(mat4, GLuint);
void UnsetBloomBrightnessShader();
void SetBloomBlurShader(mat4, mat4, mat4, GLuint[2]);
void UnsetBloomBlurShader();