#include "engine/Presenter.h"
#include "engine/Synchronizer.h"
#include "engine/SceneQueue.h"

#include "scenes/Scene00.h"
#include "scenes/Scene01.h"
#include "scenes/Scene02.h"

DWORD gdwDemoStartTime;

#pragma region SceneRelatedFunctions

#pragma region SceneRelatedYetSceneAgnosticFunctions

bool InitializeScenes(void)
{
	// Variable declarations
	SCENEMETADATA sceneMetaData;

	// Code

	// Create queue of scenes
	{
		if ((gpqScenes = CreateSceneQueue()) == NULL)
		{
			ErrorLog(NULL, "InitializeScenes()::CreateSceneQueue() failed.");
			return(false);
		}
	}

	// Initialize scene00
	{
		ZeroMemory(&sceneMetaData, sizeof(SCENEMETADATA));

		sceneMetaData.display = Scene00Display;
		sceneMetaData.update = Scene00Update;
		sceneMetaData.nextSceneSetup = Scene01Setup;
		sceneMetaData.startTime = { (unsigned short)0, (unsigned short)0, (unsigned short)0 };
		sceneMetaData.endTime = { (unsigned short)0, (unsigned short)10, (unsigned short)0 };
		// TODO: Either remove or update this code to be able to debug specific scene.
		/*if (gbDebug)
		{
			sceneMetaData.startTime = { (unsigned short)0, (unsigned short)0, (unsigned short)0 };
			sceneMetaData.endTime = { (unsigned short)0, (unsigned short)10, (unsigned short)0 };
		}*/
		sceneMetaData.duration = GET_DIFFERENCE_IN_TIMES(sceneMetaData.startTime, sceneMetaData.endTime);

		if (!Enqueue(&gpqScenes, sceneMetaData))
		{
			ErrorLog(NULL, "InitializeScenes()::Enqueue() failed to queue Scene 00.");
			return(false);
		}
		else
		{
			InfoLog(NULL, "InitializeScenes()::Enqueue() successfully enqueued Scene 00.");
		}
	}

	// Initialize scene01
	{
		ZeroMemory(&sceneMetaData, sizeof(SCENEMETADATA));

		sceneMetaData.display = Scene01Display;
		sceneMetaData.update = Scene01Update;
		sceneMetaData.nextSceneSetup = Scene02Setup;
		sceneMetaData.startTime = { (unsigned short)0, (unsigned short)10, (unsigned short)1 };
		sceneMetaData.endTime = { (unsigned short)0, (unsigned short)20, (unsigned short)0 };
		// TODO: Either remove or update this code to be able to debug specific scene.
		/*if (gbDebug)
		{
			sceneMetaData.startTime = { (unsigned short)0, (unsigned short)0, (unsigned short)0 };
			sceneMetaData.endTime = { (unsigned short)0, (unsigned short)10, (unsigned short)0 };
		}*/
		sceneMetaData.duration = GET_DIFFERENCE_IN_TIMES(sceneMetaData.startTime, sceneMetaData.endTime);

		if (!Enqueue(&gpqScenes, sceneMetaData))
		{
			ErrorLog(NULL, "InitializeScenes()::Enqueue() failed to queue Scene 01.");
			return(false);
		}
		else
		{
			InfoLog(NULL, "InitializeScenes()::Enqueue() successfully enqueued Scene 01.");
		}
	}

	// Initialize scene02
	{
		ZeroMemory(&sceneMetaData, sizeof(SCENEMETADATA));

		sceneMetaData.display = Scene02Display;
		sceneMetaData.update = Scene02Update;
		sceneMetaData.nextSceneSetup = NULL;
		sceneMetaData.startTime = { (unsigned short)0, (unsigned short)20, (unsigned short)1 };
		sceneMetaData.endTime = { (unsigned short)0, (unsigned short)30, (unsigned short)0 };
		// TODO: Either remove or update this code to be able to debug specific scene.
		/*if (gbDebug)
		{
			sceneMetaData.startTime = { (unsigned short)0, (unsigned short)0, (unsigned short)1 };
			sceneMetaData.endTime = { (unsigned short)0, (unsigned short)10, (unsigned short)0 };
		}*/
		sceneMetaData.duration = GET_DIFFERENCE_IN_TIMES(sceneMetaData.startTime, sceneMetaData.endTime);

		if (!Enqueue(&gpqScenes, sceneMetaData))
		{
			ErrorLog(NULL, "InitializeScenes()::Enqueue() failed to queue Scene 02.");
			return(false);
		}
		else
		{
			InfoLog(NULL, "InitializeScenes()::Enqueue() successfully enqueued Scene 02.");
		}
	}

	// TODO: Wire the scenes using nextSceneSetup function pointer in the SCENEMETADATA of each scene.

	return(true);
}

void UninitializeScenes(void)
{
	// Local function declarations

	// Code
	DeleteSceneQueue(&gpqScenes);
}

bool EnterPresentation(void)
{
	// Local function declarations
	bool SetupOpeningScene(void);
	//BOOL initializeDemoTextures(void);

	// Code
	//// Here starts OpenGL code
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//// Enabling blending
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//// Depth related changes
	//glClearDepth(1.0f);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	//glShadeModel(GL_SMOOTH);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//if (loadGLTexture(&texture_vitthal_murti, MAKEINTRESOURCE(ID_BITMAP_VITTHAL_MURTI)) == FALSE)
	//{
	//	fprintf(gpFile, "Failed to load Vitthal murti texture.\n");
	//	fflush(gpFile);
	//	return(FALSE);
	//}

	//if (loadGLTexture(&texture_tukaram_maharaj, MAKEINTRESOURCE(ID_BITMAP_TUKARAM_MAHARAJ_FACING_INTO_SCREEN)) == FALSE)
	//{
	//	fprintf(gpFile, "Failed to load Tukaram Maharaj texture.\n");
	//	fflush(gpFile);
	//	return(FALSE);
	//}

	//if (initializeDemoTextures() == FALSE)
	//{
	//	fprintf(gpFile, "Failed to initialize demo textures.\n");
	//	return(FALSE);
	//}

	//fprintf(gpFile, "Initialized the demo textures successfully.\n");
	//fflush(gpFile);

	//// Enabling the texture
	//glEnable(GL_TEXTURE_2D);

	//// Warm up resize call
	//resize(WIN_WIDTH, WIN_HEIGHT);

	gbEndDemo = false;

	if (!SetupOpeningScene())
	{
		ErrorLog(NULL, "EnterPresentation()::SetupOpeningScene() failed.");
		return(false);
	}

	InfoLog(NULL, "EnterPresentation()::SetupOpeningScene() successfully set up opening scene.");
	
	DebugLog(NULL, "EnterPresentation()::Getting tick count.");
	
	gdwDemoStartTime = GetTickCount();

	// TODO_00: Log demo start time.

	return(true);
}

void ExitPresentation(void)
{
	// Local function declarations
	//void uninitializeDemoTextures(void);

	// Code
	/*uninitializeDemoTextures();

	if (texture_tukaram_maharaj)
	{
		glDeleteTextures(
			1
			, &texture_tukaram_maharaj
		);
		texture_tukaram_maharaj = 0;
	}

	if (texture_vitthal_murti)
	{
		glDeleteTextures(
			1
			, &texture_vitthal_murti
		);
		texture_vitthal_murti = 0;
	}*/
}

/// <summary>
/// This function resets everything to default values (camera, objects, etc.)
/// </summary>
/// <param name=""></param>
void ResetScene(void)
{
	// Code

	// Reset camera
	{
		/*gfCameraEye[X] = gfDefaultCameraEye[X];
		gfCameraEye[Y] = gfDefaultCameraEye[Y];
		gfCameraEye[Z] = gfDefaultCameraEye[Z];

		gfCameraCenter[X] = gfDefaultCameraCenter[X];
		gfCameraCenter[Y] = gfDefaultCameraCenter[Y];
		gfCameraCenter[Z] = gfDefaultCameraCenter[Z];

		gfCameraUp[X] = gfDefaultCameraUp[X];
		gfCameraUp[Y] = gfDefaultCameraUp[Y];
		gfCameraUp[Z] = gfDefaultCameraUp[Z];*/
	}

	// Reset alpha
	{
		/*gfSceneAlpha = gfDefaultSceneAlpha;*/
	}

	// TODO_02: Resetting of objects can be taken out somewhere else.
	// Reset Object1
	{
		/*gfObject1Translation[X] = gfDefaultObject1Translation[X];
		gfObject1Translation[Y] = gfDefaultObject1Translation[Y];
		gfObject1Translation[Z] = gfDefaultObject1Translation[Z];

		gfObject1Scale[X] = gfDefaultObject1Scale[X];
		gfObject1Scale[Y] = gfDefaultObject1Scale[Y];
		gfObject1Scale[Z] = gfDefaultObject1Scale[Z];

		gfObject1Color[X] = gfDefaultObject1Color[X];
		gfObject1Color[Y] = gfDefaultObject1Color[Y];
		gfObject1Color[Z] = gfDefaultObject1Color[Z];
		gfObject1Color[A] = gfDefaultObject1Color[A];*/
	}

	// TODO_02: Resetting of objects can be taken out somewhere else.
	// Reset Object2
	{
		/*gfObject2Translation[X] = gfDefaultObject2Translation[X];
		gfObject2Translation[Y] = gfDefaultObject2Translation[Y];
		gfObject2Translation[Z] = gfDefaultObject2Translation[Z];

		gfObject2Scale[X] = gfDefaultObject2Scale[X];
		gfObject2Scale[Y] = gfDefaultObject2Scale[Y];
		gfObject2Scale[Z] = gfDefaultObject2Scale[Z];

		gfObject2Color[X] = gfDefaultObject2Color[X];
		gfObject2Color[Y] = gfDefaultObject2Color[Y];
		gfObject2Color[Z] = gfDefaultObject2Color[Z];
		gfObject2Color[A] = gfDefaultObject2Color[A];*/
	}
}

/// <summary>
/// This function sets up the opening scene for the presentation based on the configured values.
/// </summary>
/// <param name=""></param>
/// <returns>true in case opening scene is set up successfully, false otherwise.</returns>
bool SetupOpeningScene(void)
{
	// Local function declarations
	void ResetScene(void);

	// Code
	for (unsigned int currentScene = 0; currentScene <= giOpeningScene; currentScene++)
	{
		if (!Dequeue(&gpqScenes, &gsmdCurrentSceneMetaData))
		{
			ErrorLog(NULL, "SetupOpeningScene()::Dequeue() failed: Opening scene out of limit.");
			return(false);
		}
	}

	if (!gpssSceneSetups[giOpeningScene])
	{
		ErrorLog(NULL, "SetupOpeningScene()::Dequeue() failed: Opening scene setup is not available.");
		return(false);
	}

	// Reset the scene
	ResetScene();

	// Setup opening scene
	gpssSceneSetups[giOpeningScene]();

	return(true);
}

/// <summary>
/// This function checks if the next scene should be presented based on timestamp.
/// </summary>
/// <param name=""></param>
/// <returns>true in case next scene should be presented, false otherwise.</returns>
bool ShouldGoToNextScene(void)
{
	// Local function declarations

	// Code
	if (TIME_TO_MILLIS(gsmdCurrentSceneMetaData.endTime) < (unsigned long)GetElapsedTime())
	{
		return(TRUE);
	}

	return(FALSE);
}

/// <summary>
/// This function sets up the next scene and dequeues it from the scene queue.
/// </summary>
/// <param name=""></param>
void SetupNextScene(void)
{
	// Local function declarations
	bool Dequeue(SCENEQUEUE**, SCENEMETADATA*);
	void ResetScene(void);

	// Variable declarations
	PSCENESETUP pNextSceneSetup = NULL;

	// Code
	pNextSceneSetup = gsmdCurrentSceneMetaData.nextSceneSetup;

	if (pNextSceneSetup)
	{
		if (!Dequeue(&gpqScenes, &gsmdCurrentSceneMetaData))
		{
			ErrorLog(NULL, "SetupNextScene()::Dequeue() failed: Next scene setup is available but the scene not found.");
			gbEndDemo = true;
			return;
		}
	}
	else
	{
		if (!Dequeue(&gpqScenes, &gsmdCurrentSceneMetaData))
		{
			InfoLog(NULL, "SetupNextScene()::Dequeue() failed: Reached end of the demo successfully.");
		}
		else
		{
			ErrorLog(NULL, "SetupNextScene()::Dequeue() failed: Next scene is not available but the scene found.");
		}

		gbEndDemo = true;
		return;
	}

	// Reset the scene
	ResetScene();

	// Setup next scene
	pNextSceneSetup();
}

#pragma endregion SceneRelatedYetSceneAgnosticFunctions

#pragma endregion SceneRelatedFunctions

// RESUME_POINT: Check the code currently present and add things if necessary. Go ahead with adding the scene queue functions.

