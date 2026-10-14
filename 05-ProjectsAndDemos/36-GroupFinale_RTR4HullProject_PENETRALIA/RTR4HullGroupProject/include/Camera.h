#pragma once
#include "Common.h"

void InitializeCamera(void);
void UpdateCamera(void);
void CameraMoveForward(float frameTime = 0.5f);
void CameraMoveBackward(float frameTime = 0.5f);
void CameraMoveRight(float frameTime = 0.5f);
void CameraMoveLeft(float frameTime = 0.5f);
void CameraMoveUp(float frameTime = 0.5f);
void CameraMoveDown(float frameTime = 0.5f);
void CameraMoveOnMouse(float mouseXpos, float mouseYpos, float lastX, float lastY);
mat4 GetViewMatrix();