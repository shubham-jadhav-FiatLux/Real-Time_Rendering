#pragma once

#include "Common.h"

bool InitializeScene1OpeningScene(void);
void DisplayScene1OpeningScene(mat4, mat4);
void UninitializeScene1OpeningScene(void);
void Scene1OpeningSceneCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
bool createFBO_ForShadows(void);
void DisplayDebugDataScene1OpeningScene();
