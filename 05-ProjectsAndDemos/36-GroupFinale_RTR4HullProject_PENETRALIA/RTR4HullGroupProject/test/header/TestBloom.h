#pragma once

#include "Common.h"

bool InitializeTestBloomScene(void);
void DisplayTestBloomScene(void);
void UninitializeTestBloomScene(void);
void TestBloomSceneCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void DisplayDebugDataTestBloom();