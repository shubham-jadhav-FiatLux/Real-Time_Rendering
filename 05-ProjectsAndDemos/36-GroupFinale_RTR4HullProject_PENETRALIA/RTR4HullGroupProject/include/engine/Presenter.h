#pragma once

#include "Common.h"

#pragma region SceneRelatedYetSceneAgnosticFunctions

/// <summary>
/// This function initializes all the scenes (scene meta data, next scene, etc.)
/// </summary>
/// <param name=""></param>
/// <returns>true in case all the scenes are initialized successfully, false otherwise.</returns>
bool InitializeScenes(void);

/// <summary>
/// This function uninitializes the scenes (scene queue).
/// </summary>
/// <param name=""></param>
void UninitializeScenes(void);

/// <summary>
/// This function sets up things for the first frame to be able to start the presentation.
/// </summary>
/// <param name=""></param>
/// <returns>true in case everything gets set up correctly to be able to start the presentation, false otherwise.</returns>
bool EnterPresentation(void);

/// <summary>
/// This function cleans up things after the last frame presented.
/// </summary>
/// <param name=""></param>
void ExitPresentation(void);

/// <summary>
/// This function checks if the next scene should be presented based on timestamp.
/// </summary>
/// <param name=""></param>
/// <returns>true in case next scene should be presented, false otherwise.</returns>
bool ShouldGoToNextScene(void);

/// <summary>
/// This function sets up the next scene and dequeues it from the scene queue.
/// </summary>
/// <param name=""></param>
void SetupNextScene(void);

#pragma endregion SceneRelatedYetSceneAgnosticFunctions


#pragma region SceneSetups

#pragma region Scene00

#pragma endregion Scene00

#pragma region Scene01

// Camera will be stationary
const float scene01CameraEyeStart[] = { 0.0f, 0.0f, 3.0f };
const float scene01CameraCenterStart[] = { 0.0f, 0.0f, 0.0f };
const float scene01CameraUpStart[] = { 0.0f, 1.0f, 0.0f };

const float scene01AlphaStart = 1.0f;
const float scene01AlphaEnd = 0.0f;

// Banner
const float scene01BannerTranslationStart[] = { 0.0f, 0.0f, 0.0f };
const float scene01BannerScaleStart[] = { 1.0f, 1.0f, 1.0f };
const float scene01BannerColorStart[] = { 1.0f, 1.0f, 1.0f, 1.0f };

#pragma endregion Scene01

#pragma endregion SceneSetups
