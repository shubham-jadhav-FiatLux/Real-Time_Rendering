#pragma once

// header files
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h> // for exit()
#include <string>
#include <map>
#include <vector>
#include "OGL.h"

// OpenGL header file
#include <GL/glew.h>	// this must get included above/before gl.h header file
#include <GL/gl.h>

#include "vmath.h"
using namespace vmath;

// shader file
#include "Shader.h"

// logger header file
#include "utils/Logger.h"

// global variable declarations
#define WIN_WIDTH 800
#define WIN_HEIGHT 600	
#define MAX_LINE_LENGTH 256
#define FBO_WIDTH 1920
#define FBO_HEIGHT 1080

#define FBO_SHADOW_WIDTH 1024
#define FBO_SHADOW_HEIGHT 1024
#define TEMP_INSTANCE_COUNT_FOR_ALL_MODEL 10 

#define ONE_METER 1.0f
// ----------------------------------------------------------------------------

#define SCENE_0_BANNER 0
#define SCENE_1_OPENING_SCENE 1

enum TRANSFORM_MODES
{
	MODEL_TRANSFORM,
	LIGHT_TRANSFORM,
	NUM_MODES
};

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXTURE0,
	AMC_ATTRIBUTE_TEXTURE1,
	AMC_ATTRIBUTE_TANGENT,
	AMC_ATTRIBUTE_HEIGHT,
	AMC_ATTRIBUTE_SLOPE
};
extern HWND ghwnd;

enum UniformBufferBindingPoint
{
	UBO_DIRECTIONA_LIGHT = 0,
	UBO_LIGHTS,
	UBO_POINT_LIGHTS
};

struct DirLight {
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
};

extern bool result;
extern const char gszLogFileName[15];

extern int currentScene;

extern vmath::vec3 cameraPosition;
extern vmath::vec3 cameraFront;
extern vmath::vec3 cameraUp;
extern vmath::vec3 cameraRight;
extern vmath::vec3 cameraWorldUp;


extern mat4 perspectiveProjectionMatrix;
extern mat4 viewMatrix;

extern unsigned int bLight;

extern char* GLSLVersionString;

extern GLuint screenWidth, screenHeight;

struct Character
{
	unsigned int textureID;
	vec2 size;
	vec2 bearing;
	unsigned int advanced;
};

extern std::map<GLchar, Character> characters_font1;
extern std::map<GLchar, Character> characters_font2;

// Presentation - start

const unsigned int giOpeningScene = 0;

extern GLfloat lightPosition_X, lightPosition_Y, lightPosition_Z;
extern GLfloat linear, quadratic;

// Presentation - end

// Function Declarations
//void FileLog(const char* fileName, int lineNo, const char* formattedString, ...);
