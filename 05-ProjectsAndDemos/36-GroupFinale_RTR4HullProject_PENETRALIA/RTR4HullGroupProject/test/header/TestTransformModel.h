#pragma once

#include "Common.h"

bool InitializeTestTransformModelScene(void);
void DisplayTestTransformModelScene(void);
void UninitializeTestTransformModelScene(void);
void TestTransformModelSceneCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void DisplayDebugData();