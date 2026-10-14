#include "utils/Configuration.h"

char* GLSLVersionString = NULL;
char openGLProfile[] = "";

bool InitializeConfigurations(void)
{
	bool InitializeOpenGLVersion(void);

	if (!InitializeOpenGLVersion())
	{
		ErrorLog(NULL, "InitializeOpenGLVersion() is failed\n");
		return (false);
	}
	return true;
}

bool InitializeOpenGLVersion(void)
{
	char space[] = " ";
	char dot[] = ".";
	
	char* rawGLSLVersion = NULL;
	char* tokenizerInputGLSLVersion = NULL;
	char* versionNumberString = NULL;

	int tokenizerInputLength = 0;
	int GLSLVersionStringLength = 0;

	rawGLSLVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);	// Example: 4.60 NVIDIA
	if (!rawGLSLVersion)
	{
		ErrorLog(NULL, "InitializeOpenGLVersion()::glGetString() failed.");
		return false;
	}

	tokenizerInputLength = strlen(rawGLSLVersion) + 1;	// +1 for \0 to be copied using memcpy

	tokenizerInputGLSLVersion = (char*)malloc(tokenizerInputLength);
	if (!tokenizerInputGLSLVersion)
	{
		ErrorLog(NULL, "InitializeConfigurations()::malloc() failed for tokenizerInputGLSLVersion.");
		return false;
	}

	memcpy(tokenizerInputGLSLVersion, rawGLSLVersion, tokenizerInputLength);

	versionNumberString = strtok(tokenizerInputGLSLVersion, space);
	if (!versionNumberString)
	{
		ErrorLog(NULL, "InitializeOpenGLVersion()::strtok() for versionNumberString failed.");
		return false;
	}
	char* majorVersionNumber = strtok(versionNumberString, dot);
	char* minorVersionNumber = strtok(NULL, dot);

	GLSLVersionStringLength = strlen(majorVersionNumber) + strlen(minorVersionNumber) + 1;
	GLSLVersionString = (char*)malloc(GLSLVersionStringLength);

	if (!GLSLVersionString)
	{
		ErrorLog(NULL, "InitializeOpenGLVersion()::malloc() failed.");
		return(false);
	}

	memset(GLSLVersionString, 0, GLSLVersionStringLength);

	strcat(GLSLVersionString, majorVersionNumber);
	strcat(GLSLVersionString, minorVersionNumber);

	InfoLog(NULL, "InitializeOpenGLVersion()::GLSLVersionString: %s", GLSLVersionString);

	if (tokenizerInputGLSLVersion)
	{
		free(tokenizerInputGLSLVersion);
		tokenizerInputGLSLVersion = NULL;
	}

	return(true);
}

void UninitializeConfigurations(void)
{
	void UninitializeOpenGLVersion(void);

	UninitializeOpenGLVersion();
}

void UninitializeOpenGLVersion(void)
{
	if (GLSLVersionString)
	{
		free(GLSLVersionString);
		GLSLVersionString = NULL;
	}
}
