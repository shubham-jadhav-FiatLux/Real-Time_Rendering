#pragma once
#include "Common.h"

bool InitializeBoat(void);
void DisplayBoat(void);
void UninitializeBoat(void);

extern vec3 currentBoatPivotDeviation;
extern vec3 currentBoatFrontDeviation;

