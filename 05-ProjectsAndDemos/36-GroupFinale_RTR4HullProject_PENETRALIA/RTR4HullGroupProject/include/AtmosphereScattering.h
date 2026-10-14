#pragma once
#include "Common.h"

bool InitializeAtmosphereScattering(void);
void DrawAtmosphereScattering(void);
void UninitializeAtmosphereScattering(void);
void CalculateCameraHeight(vec3 camPosition, float* height, vec3* pos);
int InitializeOuterSphere(int slices, int stacks, float radius);
