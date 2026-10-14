#include "Shader.h"
#include<regex>

#pragma warning(disable : 4996)

// TODO: Deprecate this.
GLuint CreateShaderProgramObject(const char* vsFileName, const char* fsFileName, ATTRIBUTE_SET_PARAMS isOnlyTextureBindingEnable)
{
	if (vsFileName == nullptr)
	{
		ErrorLog(NULL, "%s is null", vsFileName);
		exit(1);
	}
	if (fsFileName == nullptr)
	{
		ErrorLog(NULL, "%s is null", fsFileName);
		exit(1);
	}

	// Vertex shader
	FILE* vsFile = nullptr;
	if (fopen_s(&vsFile, vsFileName, "r") != 0)
	{
		ErrorLog(NULL, "Failed to open %s file", vsFileName);
		exit(1);
	}
	char* line = nullptr;
	long numbytes;

	fseek(vsFile, 0L, SEEK_END);
	numbytes = ftell(vsFile);
	fseek(vsFile, 0L, SEEK_SET);

	line = (char*)calloc(numbytes, sizeof(char));
	if (line == nullptr)
	{
		ErrorLog(NULL, "Failed to allocate memory for vertex shader string");
		exit(1);
	}

	fread(line, sizeof(char), numbytes, vsFile);

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&line, NULL);
	glCompileShader(vertexShaderObject);

	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength * sizeof(char));
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);

				ErrorLog(NULL, "vertex shader compilation log : %s", log);
				free(log);
				log = NULL;

				if (line)
				{
					free(line);
					line = nullptr;
				}
				exit(1);
			}
		}
	}

	if (line)
	{
		free(line);
		line = nullptr;
	}
	fclose(vsFile);

	// Fragment shader
	FILE* fsFile = nullptr;
	numbytes = 0;

	if (fopen_s(&fsFile, fsFileName, "r") != 0)
	{
		ErrorLog(NULL, "Failed to open %s file", fsFileName);
	}

	fseek(fsFile, 0L, SEEK_END);
	numbytes = ftell(fsFile);
	fseek(fsFile, 0L, SEEK_SET);

	line = (char*)calloc(numbytes, sizeof(char));
	if (line == nullptr)
	{
		ErrorLog(NULL, "Failed to allocate memory for fragment shader string");
		exit(1);
	}

	fread(line, sizeof(char), numbytes, fsFile);


	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&line, NULL);
	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength * sizeof(char));
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);

				ErrorLog(NULL, "fragment shader compilation log : %s", log);
				free(log);
				log = NULL;

				if (line)
				{
					free(line);
					line = nullptr;
				}

				exit(1);
			}
		}
	}

	if (line)
	{
		free(line);
		line = nullptr;
	}
	fclose(fsFile);


	GLuint shaderProgramObject = glCreateProgram();

	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	if (isOnlyTextureBindingEnable == ATTRIBUTE_ONLY_POSITION)
	{
		glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "a_position");
	}
	else if (isOnlyTextureBindingEnable == ATTRIBUTE_ONLY_TEXTURE)
	{
		glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "a_position");
		glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXTURE0, "a_texcoord");
	}
	else if (isOnlyTextureBindingEnable == ATTRIBUTE_POS_NORM_TEX)
	{
		glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "a_position");
		glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_NORMAL, "a_normal");
		glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXTURE0, "a_texcoord");
	}	

	glLinkProgram(shaderProgramObject);

	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength * sizeof(char));
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(shaderProgramObject, infoLogLength, &written, log);

				ErrorLog(NULL, "shader program link log : %s", log);

				free(log);
				log = NULL;

				exit(1);
			}
		}
	}

	if (shaderProgramObject == 0)
	{
		ErrorLog(NULL, "shaderProgramObject is zero : %s", log);
		exit(1);
	}

	return shaderProgramObject;
}

void ReleaseShaderProgramObject(GLuint& shaderProgramObject)
{
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);

		GLsizei numAttachedShaders;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);

		if (numAttachedShaders > 0)
		{
			GLuint* shaderObjects = NULL;
			shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));

			glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

			for (GLsizei i = 0; i < numAttachedShaders; i++)
			{
				glDetachShader(shaderProgramObject, shaderObjects[i]);
				glDeleteShader(shaderObjects[i]);
				shaderObjects[i] = 0;
			}
			if (shaderObjects)
			{
				free(shaderObjects);
				shaderObjects = NULL;
			}	
		}

		glUseProgram(0);
		if (shaderProgramObject)
		{
			glDeleteProgram(shaderProgramObject);
			shaderProgramObject = 0;
		}
	}
}

// ***********************************

// TODO: Change all BOOL return types to bool.
// TODO: Update the description of below function.
/// <summary>
/// This function creates the shaders from given list of shader source files and return the array of created shader objects.
/// </summary>
/// <param name="shadersToCreate">is an array of key-value pair where key is type of shader to create and value is the name of file containing shader source code.</param>
/// <param name="shaderCount">is the count of shaders to create.</param>
/// <returns>true in case shaders are created successfully, false otherwise.</returns>
bool CreateShaders(
	SHADERSHADERTYPEKEYPAIR* shadersToCreate
	, unsigned int shaderCount
	, GLuint* rShaders
	, unsigned int* rCreatedShadersCount
)
{
	// Local function declarations
	bool CreateShader(SHADERSHADERTYPEKEYPAIR, GLuint*);

	// Code
	//*rShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);
	//if (*rShaders == NULL)
	//{
	//	// TODO: Log the error here.
	//	return(false);
	//}

	for (unsigned int shaderIndex = 0; shaderIndex < shaderCount; shaderIndex++)
	{
		if (!CreateShader(shadersToCreate[shaderIndex], &(rShaders[shaderIndex])))
		{
			ErrorLog(NULL, "CreateShader() failed for %s", shadersToCreate[shaderIndex].file);
			if (rShaders)
			{
				free(rShaders);
				rShaders = nullptr;
			}

			return(false);
		}

		(*rCreatedShadersCount)++;
	}

	return(true);
}

/// <summary>
/// This function creates shader of given type using the source code present in the file given as argument.
/// </summary>
/// <param name="shaderToCreate">A key-value pair where key is type of shader to create and value is the name of file containing shader source code.</param>
/// <param name="rShader">Returns pointer to the created shader object on success, null otherwise.</param>
/// <returns></returns>
bool CreateShader(SHADERSHADERTYPEKEYPAIR shaderToCreate, GLuint* rShader)
{
	// Local function declarations
	bool ReadFile(const char*, char**);

	// Variable declarations
	char glslVersionLine[1024] = { 0 };
	char* shaderSourceCodeWithoutVersion = nullptr;
	char* shaderSourceCode = nullptr;
	size_t shaderSourceCodeLength = 0;
	
	// Code
	if (shaderToCreate.file == nullptr)
	{
		// TODO: c error here.
		return(false);
	}

	if (!ReadFile(shaderToCreate.file, &shaderSourceCodeWithoutVersion))
	{
		ErrorLog(NULL, "Failed to read file: %s", shaderToCreate.file);

		if (shaderSourceCodeWithoutVersion)
		{
			free(shaderSourceCodeWithoutVersion);
			shaderSourceCodeWithoutVersion = nullptr;
		}
		return(false);
	}

	strcat(glslVersionLine, "#version");
	strcat(glslVersionLine, " ");
	strcat(glslVersionLine, GLSLVersionString);
	strcat(glslVersionLine, " ");
	strcat(glslVersionLine, "core");

	InfoLog(NULL, "CreateShader()::glslVersionLine: %s", glslVersionLine);

	shaderSourceCodeLength = strlen(glslVersionLine) + strlen(shaderSourceCodeWithoutVersion) + 2;	// + 1 for \n, +1 for \0

	shaderSourceCode = (char*)malloc(shaderSourceCodeLength);
	if (!shaderSourceCode)
	{
		ErrorLog(NULL, "CreateShader()::malloc() failed for shaderSourceCode.");
		return(false);
	}

	memset((void*)shaderSourceCode, 0, shaderSourceCodeLength);

	strcat(shaderSourceCode, glslVersionLine);
	strcat(shaderSourceCode, "\n");
	strcat(shaderSourceCode, shaderSourceCodeWithoutVersion);

	//InfoLog(NULL, "CreateShader()::shaderSourceCode: %s", shaderSourceCode);

	GLuint shaderObject = glCreateShader(shaderToCreate.type);
	glShaderSource(shaderObject, 1, (const GLchar**)&shaderSourceCode, NULL);
	glCompileShader(shaderObject);

	free(shaderSourceCode);
	shaderSourceCode = NULL;

	free(shaderSourceCodeWithoutVersion);
	shaderSourceCodeWithoutVersion = nullptr;

	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength * sizeof(char));
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(shaderObject, infoLogLength, &written, log);

				// TODO: Log shader compilation log here.
				ErrorLog(NULL, "Shader compilation log : %s", log);
				free(log);
				log = NULL;

				return(false);
			}
		}
	}

	*rShader = shaderObject;

	return(true);
}

// TODO: Move this code to a different file and refer in this file wherever needed.
/// <summary>
/// This function reads the contents of file and stores it in the string whose pointer is passed as an argument.
/// </summary>
/// <param name="shaderSourceFileName">Name of the file whose contents are to be read.</param>
/// <param name="shaderSourceCodeWithoutVersion">Out parameter which will hold the contents of the file if reading succeeds.</param>
/// <returns>TRUE if the operation succeeds, FALSE otherwise.</returns>
bool ReadFile(const char* shaderSourceFileName, char** rShaderSourceCode)
{
	// Variable declarations
	FILE* shaderSourceFile = nullptr;
	long byteCount;

	// Code
	if (fopen_s(&shaderSourceFile, shaderSourceFileName, "r") != 0)
	{
		// TODO: Log error here.
		return(false);
	}

	fseek(shaderSourceFile, 0L, SEEK_END);
	byteCount = ftell(shaderSourceFile);
	fseek(shaderSourceFile, 0L, SEEK_SET);

	*rShaderSourceCode = (char*)calloc(byteCount, sizeof(char));
	if (*rShaderSourceCode == nullptr)
	{
		// TODO: Log error here.
		return(false);
	}

	fread((void*)*rShaderSourceCode, sizeof(char), byteCount, shaderSourceFile);
	// TODO: Log shader source here.

	fclose(shaderSourceFile);

	return(true);
}

/// <summary>
/// This function creates a shader program.
/// </summary>
/// <param name=""></param>
/// <returns>Created shader program object.</returns>
GLuint CreateShaderProgram(void)
{
	// Code
	GLuint shaderProgramObject = glCreateProgram();

	return(shaderProgramObject);
}

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
)
{
	// Code
	for (unsigned int shaderIndex = 0; shaderIndex < shaderCount; shaderIndex++)
	{
		glAttachShader(shaderProgram, shaders[shaderIndex]);
	}
}

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
)
{

	// Local function declarations
	bool ShaderVariableExists(const char*, char**, unsigned int);
	// Variable declarations
	char** attributeNames = NULL;

	// Code
//#if 0
//	attributeNames = (char**)malloc(sizeof(char*) * attributeCount);
//
//	for (unsigned int attributeIndex = 0; attributeIndex < attributeCount; attributeIndex++)
//	{
//		attributeNames[attributeIndex] = (char*)malloc(sizeof(char) * strlen(attributesToBind[attributeIndex].attributeName));
//		strcpy_s(attributeNames[attributeIndex], sizeof(char) * strlen(attributesToBind[attributeIndex].attributeName), attributesToBind[attributeIndex].attributeName);
//	}
//
//	// TODO: Change the first parameter passed to ShaderVariableExists().
//	if (!ShaderVariableExists("temp.shader", attributeNames, attributeCount))
//	{
//		// TODO: Log error here.
//		return(false);
//	}
//
//	if (attributeNames)
//	{
//		for (unsigned int attributeIndex = 0; attributeIndex < attributeCount; attributeIndex++)
//		{
//			if (attributeNames[attributeIndex])
//			{
//				free(attributeNames[attributeIndex]);
//				attributeNames[attributeIndex] = NULL;
//			}
//		}
//
//		free(attributeNames);
//		attributeNames = NULL;
//	}
//#endif

	for (unsigned int attributeIndex = 0; attributeIndex < attributeCount; attributeIndex++)
	{
		glBindAttribLocation((GLuint)shaderProgram, (GLuint)attributesToBind[attributeIndex].attribute, (const GLchar*)attributesToBind[attributeIndex].attributeName);
	}
	return(true);
}

/// <summary>
/// This function checks if the given attribute is present in the vertex shader source code in the file with given name.
/// </summary>
/// <param name="vertexShaderSourceFileName">Name of vertex shader source file in which attribute is to be searched.</param>
/// <param name="attributeName">Name of attribute to be searched.</param>
/// <returns>true in case attribute is found, false otherwise.</returns>
bool ShaderVariableExists(const char* vertexShaderSourceFileName, char** variableNames, unsigned int variableCount)
{
	// Local function declarations
	bool ReadFile(const char*, char**);

	// Variable declarations
	bool* variablePresence;
	char regexString[256] = {0};
	char* shaderSourceCode = nullptr;
	char* line = nullptr;

	// Code
	// TODO: Make this function robust and then remove the below return statement, till then this function should always return true otherwise dependent function might fail even if the variable exist in the shader.
	return(true);

	if (!ReadFile(vertexShaderSourceFileName, &shaderSourceCode))
	{
		if (shaderSourceCode)
		{
			free(shaderSourceCode);
			shaderSourceCode = nullptr;
		}
		return(false);
	}

	variablePresence = (bool*)calloc(variableCount, sizeof(bool));

	strcat(regexString, "^(\\s)*(([a-zA-Z_][a-zA-Z0-9_]+)(\\s)+)+");
	strcat(regexString, variableNames[0]);
	strcat(regexString, "(\\[[0-9]*\\])*[;]+$");

	line = strtok(shaderSourceCode, "\n");

	while(line != NULL)
	{
		if (std::regex_match(line, std::regex(regexString)))
		{
			// TODO: Log here.
			return(true);
		}
		line = strtok(NULL, "\n");
	}

	free(variablePresence);
	variablePresence = NULL;

	return(false);
}

/// <summary>
/// This function links the given shader program.
/// </summary>
/// <param name="shaderProgram"></param>
/// <returns>true in case shader program is linked successfully, false otherwise.</returns>
bool LinkShaderProgram(GLuint shaderProgram)
{
	// Code
	glLinkProgram(shaderProgram);

	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength * sizeof(char));
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(shaderProgram, infoLogLength, &written, log);

				ErrorLog(NULL, "Shader link log : %s", log);

				free(log);
				log = NULL;

				return(false);
			}
		}
	}

	return(true);
}

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
)
{
	// Local function declarations
	bool ShaderVariableExists(const char*, char**, unsigned int);

	// Variable declarations
	char** uniformNames = NULL;

	// Code
#if 0
	uniformNames = (char**)malloc(sizeof(char*) * uniformCount);

	if (uniformNames == NULL)
	{
		// TODO: Log error
		return(false);
	}

	for (unsigned int uniformIndex = 0; uniformIndex < uniformCount; uniformIndex++)
	{
		uniformNames[uniformIndex] = (char*)malloc(sizeof(char) * strlen(uniformsToBind[uniformIndex].uniformName));
		strcpy_s(uniformNames[uniformIndex], sizeof(char) * strlen(uniformsToBind[uniformIndex].uniformName), uniformsToBind[uniformIndex].uniformName);
	}

	// TODO: Change the first parameter passed to ShaderVariableExists().
	if (!ShaderVariableExists("temp.shader", uniformNames, uniformCount))
	{
		// TODO: Log error here.
		return(false);
	}

	if (uniformNames)
	{
		for (unsigned int uniformIndex = 0; uniformIndex < uniformCount; uniformIndex++)
		{
			if (uniformNames[uniformIndex])
			{
				free(uniformNames[uniformIndex]);
				uniformNames[uniformIndex] = NULL;
			}
		}

		free(uniformNames);
		uniformNames = NULL;
	}
#endif
	for (unsigned int uniformIndex = 0; uniformIndex < uniformCount; uniformIndex++)
	{
		*(uniformsToBind[uniformIndex].pUniform) = glGetUniformLocation(shaderProgram, uniformsToBind[uniformIndex].uniformName);
	}

	return(true);
}

bool BindShaderProgramUniformBlockIndices(
	GLuint shaderProgram
	, UNIFORMBLOCKUNIFORMBLOCKBINDINGKEYPAIR* uniformBlocksToBind
	, unsigned int uniformBlockCount
)
{
	// Local function declarations
	bool ShaderVariableExists(const char*, char**, unsigned int);

	// Variable declarations
	char** uniformNames = NULL;
	GLuint uniformBlockIndex;

	// Code
	// TODO_N: Check if the given uniform blocks are present or not.
	// TODO_02: Error handling using https://www.khronos.org/opengl/wiki/OpenGL_Error

	for (unsigned int uniformBlockIndex = 0; uniformBlockIndex < uniformBlockCount; uniformBlockIndex++)
	{
		uniformBlockIndex = glGetUniformBlockIndex(shaderProgram, uniformBlocksToBind[uniformBlockIndex].uniformBlockName);

		if (uniformBlockIndex == GL_INVALID_INDEX)
		{
			WarningLog(NULL, "BindShaderProgramUniformBlockIndices()::glGetUniformBlockIndex(): %s does not identify an active uniform block of program.", uniformBlocksToBind[uniformBlockIndex].uniformBlockName);
		}
		else if (uniformBlockIndex == GL_INVALID_OPERATION)
		{
			WarningLog(NULL, "BindShaderProgramUniformBlockIndices()::glGetUniformBlockIndex(): Could not get index of %s. Given shader program object is not valid or glLinkProgram() is not called in the past for it.", uniformBlocksToBind[uniformBlockIndex].uniformBlockName);
		}
		else {
			glUniformBlockBinding(
				shaderProgram
				, uniformBlockIndex
				, uniformBlocksToBind[uniformBlockIndex].uniformBlockBinding
			);
		}
	}

	return(true);
}

