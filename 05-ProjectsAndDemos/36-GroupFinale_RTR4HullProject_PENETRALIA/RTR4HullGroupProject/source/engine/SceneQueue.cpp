#include "engine/SceneQueue.h"

#pragma region SceneQueue

SCENEQUEUE* CreateSceneQueue(void)
{
	// Variable declarations
	SCENEQUEUE* newQueue = NULL;

	// Code
	newQueue = (SCENEQUEUE*)malloc(sizeof(SCENEQUEUE));

	if (newQueue == NULL)
	{
		ErrorLog(NULL, "CreateSceneQueue() failed.\n");
	}
	else
	{
		ZeroMemory(newQueue, sizeof(SCENEQUEUE));

		newQueue->front = NULL;
		newQueue->rear = NULL;
	}

	return(newQueue);
}

SCENE* GetNewQueueEntry(SCENEMETADATA sceneMetaData)
{
	// Variable declarations
	SCENE* newQueueEntry = NULL;

	// Code
	newQueueEntry = (SCENE*)malloc(sizeof(SCENE));

	if (newQueueEntry == NULL)
	{
		ErrorLog(NULL, "GetNewQueueEntry() failed.\n");
	}
	else
	{
		ZeroMemory(newQueueEntry, sizeof(SCENE));

		newQueueEntry->sceneMetaData = sceneMetaData;
		newQueueEntry->nextScene = NULL;
	}

	return(newQueueEntry);
}

bool IsQueueEmpty(SCENEQUEUE* queue)
{
	// Variable declarations
	bool bResult = false;

	// Code
	if (queue->front == NULL)
	{
		bResult = true;
	}

	return(bResult);
}

bool Enqueue(SCENEQUEUE** queue, SCENEMETADATA sceneMetaData)
{
	// Local function declarations
	SCENE* GetNewQueueEntry(SCENEMETADATA);

	// Variable declarations
	bool bResult = false;
	SCENE* newScene = NULL;

	// Code
	if (!(newScene = GetNewQueueEntry(sceneMetaData)))
	{
		ErrorLog(NULL, "Enqueue() failed.\n");
		// TODO_01: Log for which scene meta data Enqueue() failed.
	}
	else
	{
		(*queue)->front
			? (*queue)->rear = ((*queue)->rear->nextScene = newScene)
			: (*queue)->front = (*queue)->rear = newScene;

		bResult = true;
	}

	return(bResult);
}

bool Dequeue(SCENEQUEUE** queue, SCENEMETADATA* dequeued)
{
	// Local function declarations
	bool IsQueueEmpty(SCENEQUEUE*);

	// Variable declarations
	bool bResult = false;
	SCENE* dequeuedScene = NULL;

	// Code
	if (!(*queue))
	{
		ErrorLog(NULL, "Dequeue() failed.\n");
		// TODO_01: Log for which scene meta data Enqueue() failed.
	}
	else if (IsQueueEmpty(*queue))
	{
		WarningLog(NULL, "Queue is empty cannot dequeue further.\n");
	}
	else
	{
		dequeuedScene = (*queue)->front;

		(*queue)->front =
			(*queue)->front == (*queue)->rear
			? (*queue)->rear = (*queue)->rear->nextScene	// (*queue)->rear->nextScene will be NULL here
			: dequeuedScene->nextScene;

		*dequeued = dequeuedScene->sceneMetaData;
		dequeuedScene->nextScene = NULL;
		free(dequeuedScene);
		dequeuedScene = NULL;

		DebugLog(NULL, "Dequeued successfully.\n");
		
		bResult = true;
	}

	return(bResult);
}

void DeleteSceneQueue(SCENEQUEUE** queue)
{
	// Local function declarations
	bool Dequeue(SCENEQUEUE**, SCENEMETADATA*);

	// Variable declarations
	SCENE* currentScene = NULL;
	SCENEMETADATA dequeued;

	// Code
	if (*queue && (currentScene = (*queue)->front))
	{
		while (Dequeue(queue, &dequeued));

		free(*queue);
		*queue = NULL;
	}
}

#pragma endregion SceneQueue
