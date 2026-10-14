#pragma once

#include "Common.h"

#include "engine/SceneQueue.h"

#include "scenes/Scene00.h"
#include "scenes/Scene01.h"
#include "scenes/Scene02.h"

#pragma region TimeMacros

#define MILLIS_IN_A_SECOND ((unsigned long)(1000))
#define MILLIS_IN_A_MINUTE (((unsigned long)(60)) * MILLIS_IN_A_SECOND)

#pragma endregion TimeMacros

#pragma region TimeMacroFunctions

#define TO_MILLI_SECONDS(milliseconds, seconds) ((unsigned long)(((unsigned long)(seconds)) + ((unsigned long)(seconds) * MILLIS_IN_A_SECOND)))
#define TIME_TO_MILLIS(time) ((unsigned long)(((unsigned long)(time.milliseconds)) + ((unsigned long)(time.seconds) * MILLIS_IN_A_SECOND) + ((unsigned long)(time.minutes) * MILLIS_IN_A_MINUTE)))
#define GET_DIFFERENCE_IN_TIMES(startTime, endTime) ((unsigned long)(((unsigned long)(TIME_TO_MILLIS((endTime)))) - ((unsigned long)(TIME_TO_MILLIS((startTime))))))
#pragma endregion TimeMacroFunctions


// TODO_00: Add function description.

/// <summary>
/// This function returns elapsed time of the demo.
/// </summary>
/// <param name=""></param>
/// <returns></returns>
DWORD GetElapsedTime(void);


#pragma region DemoRelatedGlobalVariables

extern SCENEQUEUE* gpqScenes;
extern SCENEMETADATA gsmdCurrentSceneMetaData;
extern bool gbEndDemo;
extern DWORD gdwDemoStartTime;		// This is the start timestamp of the demo, initialized just before starting the presentation.
extern unsigned long guiElapsedSceneDuration;


const PSCENESETUP gpssSceneSetups[] = {
	Scene00Setup,
	Scene01Setup,
	Scene02Setup
};

#pragma endregion DemoRelatedGlobalVariables
