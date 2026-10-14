#include "TestEffect.h"
#include "TestFont.h"
#include "VideoRendering.h"
#include "AtmosphereScattering.h"
#include "Ocean.h"
#include "transform_model/TransformModel.h"
#include "TestTransformModel.h"
#include "terrian/TerrianSuperBible.h"
#include "terrian/TerrianLearnOGL.h"
#include "TestBloom.h"
#include "TestMSAA.h"
#include "TestStaticModel.h"
#include "water/Water.h"
#include "TestDownloadedStaticModel.h"

bool InitializeTestScene(void)
{
	// code
	
	// initlization of test scene 
	InitializeTestStaticModel();
	//InitializeAtmosphereScattering();
	//InitializeOcean();
	//InitializeTerrianSuperBible();
	//InitializeTerrianLearnOGL();
	//InitializeTestTransformModelScene();
	//InitializeTestBloomScene();

	//InitializeMSAATestScene();

	//InitializeWater();
	
	//InitializeTestDownloadedStaticModel();

	return true;
}

void DisplayTestScene(void)
{
	//DisplayTestFont();
	DisplayTestStaticModel();
	//DisplayVideoRendering();
	//DrawAtmosphereScattering();
	//DrawOcean();
	
	//DisplayTerrianSuperBible();

	//DisplayTerrianLearnOGL();

	//DisplayTestDownloadedStaticModel();

	//DisplayTestBloomScene();
	//DisplayMSAATestScene();

	//DisplayWater();
	//DisplayTestBloomScene(viewMatrix, projectionMatrix);
	//DisplayMSAATestScene(viewMatrix, projectionMatrix);


	//DisplayTestTransformModelScene();

}

void UninitializeTestScene(void)
{

	//UninitializeTestTransformModelScene();
	//UninitializeTerrianSuperBible();
	//UninitializeTestBloomScene();
	//UninitializeMSAATestScene();
	//UninitializeWater();
}


