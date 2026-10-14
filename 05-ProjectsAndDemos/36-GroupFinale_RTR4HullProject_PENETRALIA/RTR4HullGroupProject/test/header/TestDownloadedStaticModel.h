#pragma once

#include "Common.h"

bool InitializeTestDownloadedStaticModel(void);
void DisplayTestDownloadedStaticModel(void);
void UninitializeTestDownloadedStaticModel(void);
void TestDownloadedStaticModelCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void DisplayDebugDataTestDownloadedStaticModel();