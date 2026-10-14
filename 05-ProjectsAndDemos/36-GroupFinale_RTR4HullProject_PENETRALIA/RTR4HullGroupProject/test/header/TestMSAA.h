#pragma once

#include "Common.h"

bool InitializeMSAATestScene(void);
bool ResizeMSAATestScene(int width, int height);
void DisplayMSAATestScene(void);
void UninitializeMSAATestScene(void);
void MSAATestSceneCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void DisplayDebugDataMSAATest();