#pragma once
#include "Common.h"

// in future we might need to add more attributes as per particle system or for fire 

enum ATTRIBUTE_SET_PARAMS
{
	ATTRIBUTE_ONLY_POSITION = 0,
	ATTRIBUTE_ONLY_TEXTURE,
	ATTRIBUTE_POS_NORM_TEX,
};

GLuint CreateShaderProgramObject(const char* vsFileName, const char* fsFileName, ATTRIBUTE_SET_PARAMS isOnlyTextureBindingEnable = ATTRIBUTE_POS_NORM_TEX);
void ReleaseShaderProgramObject(GLuint& shaderProgramObject);

// *************************************************************

typedef struct ShaderShaderTypeKeypair {
	GLenum type;
	const char* file;
} SHADERSHADERTYPEKEYPAIR;

typedef struct AttribTypeAttribNameKeypair
{
	int attribute;
	const char* attributeName;
} ATTRIBTYPEATTRIBNAMEKEYPAIR;

typedef struct UniformUniformNameKeypair
{
	GLuint* pUniform;
	const char* uniformName;
} UNIFORMUNIFORMNAMEKEYPAIR;

typedef struct UniformBlockUniformBlockBindingKeyPair
{
	int uniformBlockBinding;
	const char* uniformBlockName;
} UNIFORMBLOCKUNIFORMBLOCKBINDINGKEYPAIR;

/// <summary>
/// This function creates the shaders from given list of shader source files and return the array of created shader objects.
/// </summary>
/// <param name="shadersToCreate">is an array of key-value pair where key is type of shader to create and value is the name of file containing shader source code.</param>
/// <param name="shaderCount">is the count of shaders to create.</param>
/// <returns>true in case shaders are created successfully, false otherwise.</returns>
bool CreateShaders(
	SHADERSHADERTYPEKEYPAIR* shadersToCreate
	, unsigned int shaderCount
	, GLuint* shaders
	, unsigned int* rCreatedShadersCount
);

/// <summary>
/// This function creates a shader program.
/// </summary>
/// <param name=""></param>
/// <returns>Created shader program object.</returns>
GLuint CreateShaderProgram(void);

/// <summary>
/// This function attaches given shaders to the given shader program object.
/// </summary>
/// <param name="shaderProgram">is the shader program to which shaders are to be attached.</param>
/// <param name="shaders">are the shaders to be attached.</param>
/// <param name="shaderCount">is the count of shaders to be attached.</param>
void AttachShadersToShaderProgram(
	GLuint shaderProgram
	, GLuint* shaders
	, unsigned int shaderCount
);

/// <summary>
/// This function checks if given attributes are present in the shader program and binds the attribute locations of given shader program.
/// </summary>
/// <param name="shaderProgram">is the shader program of which attributes are to be bound.</param>
/// <param name="attributesToBind">are the attributes to be bound.</param>
/// <param name="attributeCount">is the count of attributes to be bound.</param>
/// <returns>true in case all attributes are bound successfully, false in case there is any attribute not present in the shader source code, false otherwise.</returns>
bool BindShaderProgramAttribLocations(
	GLuint shaderProgram
	, ATTRIBTYPEATTRIBNAMEKEYPAIR* attributesToBind
	, unsigned int attributeCount
);

/// <summary>
/// This function links the given shader program.
/// </summary>
/// <param name="shaderProgram"></param>
/// <returns>true in case shader program is linked successfully, false otherwise.</returns>
bool LinkShaderProgram(GLuint shaderProgram);

/// <summary>
/// This function checks if given uniforms are present in the shader program and binds their locations to the given uniform variables.
/// </summary>
/// <param name="shaderProgram">is the shader program of which uniform locations are to be bound to the uniform variables.</param>
/// <param name="uniformsToBind">are the key value pairs of uniform variables and uniform names to be bound.</param>
/// <param name="uniformCount">is the count of uniforms to be bound.</param>
/// <returns>true if all the uniform locations are bound to uniform variables successfully, false otherwise.</returns>
bool BindShaderProgramUniformLocations(
	GLuint shaderProgram
	, UNIFORMUNIFORMNAMEKEYPAIR* uniformsToBind
	, unsigned int uniformCount
);

/// <summary>
/// This function checks if given uniforms blocks are present in the shader program and binds their indices to the given uniform block variables.
/// </summary>
/// <param name="shaderProgram">is the shader program of which uniform block indices are to be bound to the uniform variables.</param>
/// <param name="uniformBlocksToBind">are the key value pairs of binding points and uniform block names to be bound.</param>
/// <param name="uniformBlockCount">is the count of uniforms blocks to be bound.</param>
/// <returns></returns>
bool BindShaderProgramUniformBlockIndices(
	GLuint shaderProgram
	, UNIFORMBLOCKUNIFORMBLOCKBINDINGKEYPAIR* uniformBlocksToBind
	, unsigned int uniformBlockCount
);

