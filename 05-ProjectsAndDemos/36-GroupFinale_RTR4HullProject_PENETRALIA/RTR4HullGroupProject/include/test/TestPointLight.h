#pragma once

#include "Common.h"

#define TEST_POINT_LIGHT TRUE

/// <summary>
/// This function does initializations required for testing point lights.
/// </summary>
/// <param name=""></param>
/// <returns>true in case initialization succeeds, false otherwise.</returns>
bool InitializeTestPointLight(void);

/// <summary>
/// This function displays colored cubes with the colors same as diffuse colors given to point lights at positions given to them.
/// </summary>
/// <param name=""></param>
void DisplayTestPointLight(void);

/// <summary>
/// This function does the uninitialization that was required for testing point lights.
/// </summary>
/// <param name=""></param>
void UninitializeTestPointLight(void);

