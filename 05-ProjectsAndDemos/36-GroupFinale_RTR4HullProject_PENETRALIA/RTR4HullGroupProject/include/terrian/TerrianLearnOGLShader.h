#pragma once

#include "Common.h"

/*struct DirLight {
	vec4 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};*/

bool InitializeTerrianLearnOGLShader(void);
void SetTerrianLearnOGLShader(mat4, GLuint, GLuint, vec4, GLuint, GLuint, GLuint, GLuint, GLuint, GLuint, struct DirLight[], unsigned int, vec3);
void UnsetTerrianLearnOGLShader(void);
void UninitializeTerrianLearnOGLShader(void);
