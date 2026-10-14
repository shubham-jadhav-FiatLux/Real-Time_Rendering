#pragma once

#include "Common.h"

#pragma region UserDefinedDataTypes

typedef void (*PSCENESETUP)(void);
typedef void (*PDISPLAY)(void);
typedef void (*PUPDATE)(void);

typedef struct Time
{
	unsigned short minutes;
	unsigned short seconds;
	unsigned short milliseconds;
} TIME;

typedef struct SceneMetaData
{
	PDISPLAY display;
	PUPDATE update;
	PSCENESETUP nextSceneSetup;

	TIME startTime;
	TIME endTime;

	unsigned long duration;
} SCENEMETADATA;

typedef struct Scene
{
	SCENEMETADATA sceneMetaData;

	struct Scene* nextScene;
} SCENE;

enum Timings
{
	START = 0,
	END
};

#pragma endregion UserDefinedDataTypes


#pragma region SceneQueue

typedef struct SceneQueue
{
	SCENE* front;
	SCENE* rear;
} SCENEQUEUE;

/// <summary>
/// This function creates a new scene queue.
/// </summary>
/// <param name=""></param>
/// <returns>A valid pointer to the front of new scene queue if it is created successfully, NULL otherwise.</returns>
SCENEQUEUE* CreateSceneQueue(void);

/// <summary>
/// This functions enqueues a scene in given scene queue.
/// </summary>
/// <param name="queue">The queue in which the scene is to be enqueued.</param>
/// <param name="sceneMetaData">The metadata of the scene to be enqueued.</param>
/// <returns>true if the scene is enqueued successfully, false otherwise.</returns>
bool Enqueue(SCENEQUEUE** queue, SCENEMETADATA sceneMetaData);

/// <summary>
/// This function dequeues a scene from given scene queue.
/// </summary>
/// <param name="queue">The queue from which scene is to be dequeued.</param>
/// <param name="outDequeued">Pointer to store the location of metadata of scene that will be dequeued.</param>
/// <returns>true in case the scene is successfully dequeued from the scene queue, false otherwise.</returns>
bool Dequeue(SCENEQUEUE** queue, SCENEMETADATA* outDequeued);

/// <summary>
/// This function deletes the given scene queue.
/// </summary>
/// <param name="queue">The scene queue to be deleted.</param>
void DeleteSceneQueue(SCENEQUEUE** queue);

#pragma endregion SceneQueue
