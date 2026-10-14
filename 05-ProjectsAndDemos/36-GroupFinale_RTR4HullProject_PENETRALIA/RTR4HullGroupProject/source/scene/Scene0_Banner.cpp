#include "scene/Scene0_Banner.h"
#include "VideoRendering.h"
#include <grass/Grass.h>

bool InitializeScene0Banner(void)
{
	if (!InitializeVideoRendering())
	{
		ErrorLog(NULL, "InitializeCommonShaderProgramObjects()::InitializeVideoRendering() failed.");
		return false;
	}
	return true;;
}

void DisplayScene0Banner()
{
	DisplayGrass();

	DisplayVideoRendering();
}

void UninitializeScene0Banner(void)
{
	UninitializeVideoRendering();
}