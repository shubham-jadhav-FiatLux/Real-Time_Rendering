#include "engine/Synchronizer.h"
#include "engine/SceneQueue.h"

SCENEQUEUE* gpqScenes = NULL;
SCENEMETADATA gsmdCurrentSceneMetaData;
bool gbEndDemo = false;
unsigned long guiElapsedSceneDuration = 0;

DWORD GetElapsedTime(void)
{
	// Code
	return(GetTickCount() - gdwDemoStartTime);
}
