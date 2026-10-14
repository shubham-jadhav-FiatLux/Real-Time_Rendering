#pragma once
#include "Common.h"

//GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f };
//GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f };
//GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f };
//GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };
//GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f };
//GLfloat materialDiffuse[] = { 0.5f, 0.5f, 0.5f };
//GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f };
//GLfloat materialShininess = 50.0f;

bool InitializePhongLightShader(void);
void SetPhongLightShader(mat4, mat4, mat4, vec3 lightAmbient = vec3( 0.0f, 0.0f, 0.0), vec3 lightDiffuse = vec3( 1.0f, 1.0f, 1.0f), vec3 lightSpecular = vec3(1.0f, 1.0f, 1.0f), vec4 lightPosition = vec4 (100.0f, 100.0f, 100.0f, 1.0f), vec3 materialAmbient = vec3(0.0f, 0.0f, 0.0f), vec3 materialDiffuse = vec3(0.5f, 0.5f, 0.5f), vec3 materialSpecular = vec3(1.0f, 1.0f, 1.0f), float materialShininess = 50.0f);

void UnsetPhongLightShader(void);
void UninitializePhongLightShader(void);