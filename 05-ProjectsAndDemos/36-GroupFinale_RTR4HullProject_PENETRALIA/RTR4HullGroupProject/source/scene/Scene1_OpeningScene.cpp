#include "scene/Scene1_OpeningScene.h"
#include "transform_model/TransformModel.h"
#include "CubeRender.h"
#include "TextureShader.h"
#include "Texture.h"
#include "AtmosphereScattering.h"
#include "water/Water.h"
#include "StaticModelShader.h"
#include "static_model_loading/StaticModel.h"
#include "FontShader.h"
#include "terrian/TerrianLearnOGL.h"
#include "Ocean.h"
#include <engine/UniformBuffer.h>
#include <test/TestPointLight.h>
#include <grass/Grass.h>
#include "model/BoatModel.h"

static float steps = 0.1f;
static std::vector<POINTLIGHTMIRROR> lights;
static std::vector<struct  Transform> transformModel;
static std::vector<struct  Transform> mashroomTransforms;
//Transform Light
static int currentLightIndex = -1;
static int currentLightMember = 0;	//9
// Transform Model
static int currentModel = -1;
static int currentTranform = 0;
static GLuint image_texture = 0;

// models
static Model _01_alien_flower_model;
static Model _03_alien_plant_1_model;
static Model _06_alien_plant_stl_textured_model;
static Model _10_bioluminescent_mushroom_model;
static Model _11_blue_flower_animated_model;
static Model _16_coral_model;
static Model _17_coral_piece_model;
static Model _19_exotic_pink_leafed_plant_model;
static Model _20_fantastic_plant_illustrated_by_hand_made_model;
static Model _22_fantasy_plant_2_model;
static Model _24_glowing_mushroom_model;
static Model _29_mushrooms_blue_oyster_model;
static Model _31_pine_cone_model;
static Model _33_planta_alienigena_texturizada_model;
static Model _34_planta_fantasia_de_colores_pasteles_model;
static Model _36_planta_fantasia_en_flor_model;
static Model _40_stylized_mushrooms_collection_model;
static Model _41_stylized_tree_model;
static Model _44_tree_speed_sculpt_model;

//model transforms
static std::vector<struct  Transform> _01_alienTransformModel;
static std::vector<struct  Transform> _03_alien_plantTransformModel;
static std::vector<struct  Transform> _06_alien_plantTransformModel;
static std::vector<struct  Transform> _10_bioluminescentTransformModel;
static std::vector<struct  Transform> _11_blue_flowerTransformModel;
static std::vector<struct  Transform> _16_coralTransformModel;
static std::vector<struct  Transform> _17_coralTransformModel;
static std::vector<struct  Transform> _19_exotic_pinkTransformModel;
static std::vector<struct  Transform> _20_fantastic_plantTransformModel;
static std::vector<struct  Transform> _22_fantasyTransformModel;
static std::vector<struct  Transform> _24_glowingTransformModel;
static std::vector<struct  Transform> _29_mushroomsTransformModel;
static std::vector<struct  Transform> _31_pine_coneTransformModel;
static std::vector<struct  Transform> _36_plantaTransformModel;
static std::vector<struct  Transform> _33_plantaTransformModel;
static std::vector<struct  Transform> _34_plantaTransformModel;
static std::vector<struct  Transform> _40_stylizedTransformModel;
static std::vector<struct  Transform> _41_stylizedTransformModel;
static std::vector<struct  Transform> _44_treeTransformModel;

#define NUM_LIGHTS 6
static TRANSFORM_MODES iTransformMode = TRANSFORM_MODES::MODEL_TRANSFORM;
GLfloat range = 100.0f;
GLfloat quadratic = 1.8f, linear = 0.7f;;
GLfloat lightPosition_X = -14.0f, lightPosition_Y = 37.0f, lightPosition_Z = 537.0f;

// for shadows
static GLuint depth_fbo = 0;
static GLuint depth_texture;

// static models
static Model mushroom_model;
static Model lotus_model;

bool InitializeScene1OpeningScene(void)
{
	if (!InitializeStaticModelShader())
		return false;

	if (!InitializeWater())
	{
		return false;
	}
	if (!InitializeOcean())
	{
		return false;
	}
	if (InitializeAtmosphereScattering() == false)
	{
		return false;
	}
	if (InitializeTerrianLearnOGL() == false)
	{
		return false;
	}

	image_texture = CreateTexture("resources/test.png");

	mushroom_model = Model("resources\\models\\ToTestBloomEffect\\01_Mushroom\\mushroom.fbx");
	lotus_model = Model("resources\\models\\ToTestBloomEffect\\03_Lotus\\lotus.fbx");

	_01_alien_flower_model = Model("resources\\models\\tested\\01_alien_flower\\01_alien-flower.obj");
	_03_alien_plant_1_model = Model("resources\\models\\tested\\03_alien_plant_1\\03_alien-plant-1.obj");
	_06_alien_plant_stl_textured_model = Model("resources\\models\\tested\\06_alien_plant_stl_textured\\06_alien-plant-stl-textured.obj");
	_10_bioluminescent_mushroom_model = Model("resources\\models\\tested\\10_bioluminescent_mushroom\\10_bioluminescent-mushroom.obj");
	_11_blue_flower_animated_model = Model("resources\\models\\tested\\11_blue_flower_animated\\11_blue_flower_animated.obj");
	_16_coral_model = Model("resources\\models\\tested\\16_coral\\coral.obj");
	_17_coral_piece_model = Model("resources\\models\\tested\\17_coral_piece\\17_coral-piece.obj");
	_19_exotic_pink_leafed_plant_model = Model("resources\\models\\tested\\19_exotic_pink_leafed_plant\\19_exotic-pink-leafed-plant.obj");
	_20_fantastic_plant_illustrated_by_hand_made_model = Model("resources\\models\\tested\\20_fantastic_plant_illustrated_by_hand_made\\20_fantastic-plant-illustrated-by-hand-made.obj");
	//_21_fantasy_light_plant_animated_model = Model("resources\\models\\tested\\21_fantasy_light_plant_animated\\backpack.obj");
	_22_fantasy_plant_2_model = Model("resources\\models\\tested\\22_fantasy_plant_2\\22_fantasy-plant-2.obj");
	_24_glowing_mushroom_model = Model("resources\\models\\tested\\24_glowing_mushroom\\24_glowing-mushroom.obj");
	_29_mushrooms_blue_oyster_model = Model("resources\\models\\tested\\29_mushrooms_blue_oyster\\29_mushrooms-blue-oyster.obj");
	_31_pine_cone_model = Model("resources\\models\\tested\\31_pine_cone\\31_pine-cone.obj");
	_33_planta_alienigena_texturizada_model = Model("resources\\models\\tested\\33_planta_alienigena_texturizada\\33_planta-alienigena-texturizada-a-mano.obj");
	_34_planta_fantasia_de_colores_pasteles_model = Model("resources\\models\\tested\\34_planta_fantasia_de_colores_pasteles\\34_planta-fantasia-de-colores-pasteles (1).obj");
	_36_planta_fantasia_en_flor_model = Model("resources\\models\\tested\\36_planta_fantasia_en_flor\\36_planta-fantasia-en-flor.obj");
	_40_stylized_mushrooms_collection_model = Model("resources\\models\\tested\\40_stylized_mushrooms_collection\\40_stylized-mushrooms-collection.obj");
	_41_stylized_tree_model = Model("resources\\models\\tested\\41_stylized_tree\\41_stylized-tree.obj");
	_44_tree_speed_sculpt_model = Model("resources\\models\\tested\\44_tree_speed_sculpt\\44_tree-speed-sculpt.obj");

	if (createFBO_ForShadows() == false)
	{
		return false;
	}

	// transform model change

	//transformModel.push_back(Transform(0.000000f, 0.120000f, 9.800001f, 0.020000f, 0.020000f, 0.020000f, 0.000000f, 0.000000f, 0.000000f));

	_41_stylizedTransformModel.push_back(Transform(-66.000000f, 44.399982f, 98.700020f, 1.0f, 1.0f, 1.0f, 0.0f, -0.0f, 0.0f));
	_36_plantaTransformModel.push_back(Transform(-40.499954f, 44.399982f, 85.900215f, 0.400000f, 0.400000f, 0.400000f, 0.0f, -0.0f, 0.0f));
	_34_plantaTransformModel.push_back(Transform(-29.200075f, 44.399982f, 100.000000f, 0.200000f, 0.200000f, 0.200000f, 0.0f, -0.0f, 0.0f));
	_33_plantaTransformModel.push_back(Transform(21.800047f, 44.399982f, 87.200195f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f));
	_31_pine_coneTransformModel.push_back(Transform(-6.099997f, 44.399982f, 92.200119f, 0.100000f, 0.100000f, 0.100000f, 0.0f, 0.0f, 0.0f));
	_29_mushroomsTransformModel.push_back(Transform(7.199996f, 44.399982f, 105.599915f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f));
	_24_glowingTransformModel.push_back(Transform(7.199996f, 44.399982f, 105.599915f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f));
	_22_fantasyTransformModel.push_back(Transform(-13.600016f, 44.399982f, 92.600113f, 7.399995f, 7.699995f, 8.299995f, 0.0f, 0.0f, 0.0f));
	_20_fantastic_plantTransformModel.push_back(Transform(24.800058f, 44.399982f, 100.000000f, 0.200000f, 0.200000f, 0.200000f, 0.0f, -0.0f, 0.0f));
	_19_exotic_pinkTransformModel.push_back(Transform(21.900047f, 44.399982f, 100.000000f, 2.700000f, 2.700000f, 2.700000f, 0.0f, -0.0f, 0.0f));
	_16_coralTransformModel.push_back(Transform(14.680020f, 44.399982f, 100.000000f, 0.100000f, 0.100000f, 0.100000f, -90.000000f, 0.000000f, 0.000000f));
	_11_blue_flowerTransformModel.push_back(Transform(-9.400000f, 44.399982f, 100.000000f, 4.099998f, 3.299999f, 3.099999f, 0.0f, 0.0f, 0.0f));
	_10_bioluminescentTransformModel.push_back(Transform(12.600012f, 44.399982f, 85.299461f, 0.600000f, 0.600000f, 0.600000f, 0.0f, -0.0f, 0.0f));
	_06_alien_plantTransformModel.push_back(Transform(-7.199996f, 44.399982f, 81.799515f, 5.099998f, 5.099998f, 5.099998f, 0.0f, -0.0f, 0.0f));

	_40_stylizedTransformModel.push_back(Transform(-178.000000f, 37.399967f, 870.000000f, 34.000000f, 34.000000f, 34.000000f, 0.000000f, -0.000000f, 0.000000f));
	_40_stylizedTransformModel.push_back(Transform(-62.300003f, 37.399967f, 860.000000f, 34.000000f, 34.000000f, 34.000000f, 0.000000f, -0.000000f, 0.000000f));
	_40_stylizedTransformModel.push_back(Transform(257.700012f, 37.399967f, 860.000000f, 34.000000f, 34.000000f, 34.000000f, 0.000000f, -0.000000f, 0.000000f));
	_40_stylizedTransformModel.push_back(Transform(307.700012f, 39.399967f, 730.000000f, 34.000000f, 34.000000f, 34.000000f, 0.000000f, -0.000000f, 0.000000f));
	_40_stylizedTransformModel.push_back(Transform(194.700012f, 39.399967f, 730.000000f, 34.000000f, 34.000000f, 34.000000f, 0.000000f, -0.000000f, 0.000000f));
	_40_stylizedTransformModel.push_back(Transform(129.700012f, 39.399967f, 643.000000f, 34.000000f, 34.000000f, 34.000000f, 0.000000f, -0.000000f, 0.000000f));

	_34_plantaTransformModel.push_back(Transform(274.000000f, 19.399982f, 811.099976f, 1.000000f, 1.000000f, 1.000000f, 0.000000f, -0.000000f, 0.000000f));
	_34_plantaTransformModel.push_back(Transform(254.000000f, 19.399982f, 903.099976f, 1.000000f, 1.000000f, 1.000000f, 0.000000f, -0.000000f, 0.000000f));
	_34_plantaTransformModel.push_back(Transform(-98.000000f, 19.399982f, 903.099976f, 1.000000f, 1.000000f, 1.000000f, 0.000000f, -0.000000f, 0.000000f));
	_34_plantaTransformModel.push_back(Transform(-267.000000f, 19.399982f, 852.099976f, 1.000000f, 1.000000f, 1.000000f, 0.000000f, -0.000000f, 0.000000f));

	_22_fantasyTransformModel.push_back(Transform(0.000000f, 46.399982f, 850.000000f, 11.000000f, 11.000000f, 11.000000f, 0.000000f, -0.000000f, 0.000000f));
	_22_fantasyTransformModel.push_back(Transform(-60.000000f, 46.399982f, 810.000000f, 11.000000f, 11.000000f, 11.000000f, 0.000000f, -0.000000f, 0.000000f));
	_22_fantasyTransformModel.push_back(Transform(110.000000f, 46.399982f, 810.000000f, 11.000000f, 11.000000f, 11.000000f, 0.000000f, -0.000000f, 0.000000f));
	_22_fantasyTransformModel.push_back(Transform(110.000000f, 46.399982f, 870.000000f, 11.000000f, 11.000000f, 11.000000f, 0.000000f, -0.000000f, 0.000000f));
	_22_fantasyTransformModel.push_back(Transform(180.000000f, 46.399982f, 870.000000f, 11.000000f, 11.000000f, 11.000000f, 0.000000f, -0.000000f, 0.000000f));
	_22_fantasyTransformModel.push_back(Transform(180.000000f, 46.399982f, 930.000000f, 11.000000f, 11.000000f, 11.000000f, 0.000000f, -0.000000f, 0.000000f));

	_17_coralTransformModel.push_back(Transform(-70.000000f, 45.000004f, 875.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));
	_17_coralTransformModel.push_back(Transform(-70.000000f, 45.000004f, 835.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));
	_17_coralTransformModel.push_back(Transform(-108.000000f, 45.000004f, 835.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));
	_17_coralTransformModel.push_back(Transform(-141.000000f, 45.000004f, 795.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));
	_17_coralTransformModel.push_back(Transform(-141.000000f, 45.000004f, 712.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));
	_17_coralTransformModel.push_back(Transform(-210.000000f, 45.000004f, 712.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));
	_17_coralTransformModel.push_back(Transform(-210.000000f, 45.000004f, 806.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));
	_17_coralTransformModel.push_back(Transform(72.000000f, 45.000004f, 882.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));
	_17_coralTransformModel.push_back(Transform(120.000000f, 45.000004f, 825.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));
	_17_coralTransformModel.push_back(Transform(210.000000f, 45.000004f, 868.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));

	_03_alien_plantTransformModel.push_back(Transform(257.700012f, 35.800022f, 768.000000f, 8.000000f, 8.000000f, 8.000000f, 0.000000f, -0.000000f, 0.000000f));
	_03_alien_plantTransformModel.push_back(Transform(257.700012f, 35.800022f, 684.000000f, 8.000000f, 8.000000f, 8.000000f, 0.000000f, -0.000000f, 0.000000f));
	_03_alien_plantTransformModel.push_back(Transform(-382.299988f, 35.800022f, 684.000000f, 8.000000f, 8.000000f, 8.000000f, 0.000000f, -0.000000f, 0.000000f));
	_03_alien_plantTransformModel.push_back(Transform(-382.299988f, 35.800022f, 854.000000f, 8.000000f, 8.000000f, 8.000000f, 0.000000f, -0.000000f, 0.000000f));
	_03_alien_plantTransformModel.push_back(Transform(-222.299988f, 35.800022f, 854.000000f, 8.000000f, 8.000000f, 8.000000f, 0.000000f, -0.000000f, 0.000000f));
	_03_alien_plantTransformModel.push_back(Transform(-262.299988f, 43.800018f, 509.000000f, 8.000000f, 8.000000f, 8.000000f, 0.000000f, -0.000000f, 0.000000f));

	_01_alienTransformModel.push_back(Transform(-10.000000f, 34.999996f, 741.000000f, 2.000000f, 2.000000f, 2.000000f, 0.000000f, -0.000000f, 0.000000f));
	_01_alienTransformModel.push_back(Transform(-50.000000f, 34.999996f, 700.000000f, 2.000000f, 2.000000f, 2.000000f, 0.000000f, -0.000000f, 0.000000f));
	_01_alienTransformModel.push_back(Transform(-50.000000f, 34.999996f, 760.000000f, 2.000000f, 2.000000f, 2.000000f, 0.000000f, -0.000000f, 0.000000f));
	_01_alienTransformModel.push_back(Transform(-170.000000f, 34.999996f, 760.000000f, 2.000000f, 2.000000f, 2.000000f, 0.000000f, -0.000000f, 0.000000f));
	_01_alienTransformModel.push_back(Transform(-330.000000f, 34.999996f, 690.000000f, 2.000000f, 2.000000f, 2.000000f, 0.000000f, -0.000000f, 0.000000f));
	_01_alienTransformModel.push_back(Transform(-330.000000f, 34.999996f, 820.000000f, 2.000000f, 2.000000f, 2.000000f, 0.000000f, -0.000000f, 0.000000f));

	_44_treeTransformModel.push_back(Transform(0.000000f, 36.399982f, 100.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));
	_44_treeTransformModel.push_back(Transform(0.000000f, 38.399982f, 637.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));
	_44_treeTransformModel.push_back(Transform(-97.000000f, 38.399982f, 637.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));
	_44_treeTransformModel.push_back(Transform(-97.000000f, 44.399982f, 765.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));
	_44_treeTransformModel.push_back(Transform(126.000000f, 44.399982f, 765.000000f, 3.000000f, 3.000000f, 3.000000f, 0.000000f, -0.000000f, 0.000000f));

	transformModel.push_back(Transform(0.0f, 36.399982f, 100.0f, 1.0f, 1.0f, 1.0f, 0.0f, -0.0f, 0.0f));


	lights.push_back(gPointLightsUBO.data.pointLights[0]);
	lights.push_back(gPointLightsUBO.data.pointLights[1]);
	lights.push_back(gPointLightsUBO.data.pointLights[2]);
	lights.push_back(gPointLightsUBO.data.pointLights[3]);
	lights.push_back(gPointLightsUBO.data.pointLights[4]);
	lights.push_back(gPointLightsUBO.data.pointLights[5]);
	lights.push_back(gPointLightsUBO.data.pointLights[6]);
	lights.push_back(gPointLightsUBO.data.pointLights[7]);
	lights.push_back(gPointLightsUBO.data.pointLights[8]);
	lights.push_back(gPointLightsUBO.data.pointLights[9]);

	currentModel = (UINT)transformModel.size() - 1U;
	currentLightIndex = (UINT)lights.size() - 1U;
	return true;
}

void DisplayScene1OpeningScene(mat4 viewMatrix, mat4 projectionMatrix)
{
	// variable declarations
	mat4 modelMatrix[TEMP_INSTANCE_COUNT_FOR_ALL_MODEL];
	// code
	vec4 lightPosition = vmath::vec4(0.0f, 10.0f, -8.0f, 1.0f);
	// display terrian

	// set depth buffer FBO
	/*glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
	glViewport(0, 0, (GLsizei)FBO_SHADOW_WIDTH, (GLsizei)FBO_SHADOW_WIDTH);

	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Enable polygon offset to resolve depth-fighting isuses
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0f, 4.0f);

	// display scene here
	{
		DisplayTerrianSuperBible(viewMatrix, projectionMatrix);

		// render cube
		mat4 modelMatrix[i] = mat4::identity();
		modelMatrix[i] = translate(0.0f, 7.0f, 0.0f);

		SetTextureShader(modelMatrix, viewMatrix, projectionMatrix, image_texture);
		RenderCube();
		UnsetTextureShader();
	}
	glDisable(GL_POLYGON_OFFSET_FILL);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	*/

	// regular/common framebuffer
	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);*/

	if (range >= 0.5f)
	{
		linear = 4.5f / range;
		quadratic = 7.5f / (range * range);
	}
	else
	{
		linear = 0.7f;   //values for range 7 default
		quadratic = 1.8f;
	}

	// display scene here
	{
		//DrawAtmosphereScattering();
		DisplayTerrianLearnOGL();
		DrawOcean();
		DisplayBoat();
		//DisplayTestPointLight();
		//DisplayGrass();

	}
	/*for (int i = 0; i < mashroomTransforms.size(); i++)
	{
		modelMatrix[i] = mashroomTransforms[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, 0.09f, 0.32f);
		mushroom_model.Draw();
	}*/

	// draw models

	/* {
#pragma region STATIC_MODEL_LOAD
		for (int i = 0; i < _01_alienTransformModel.size(); i++)
			modelMatrix[i] = _01_alienTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _01_alienTransformModel.size());
		_01_alien_flower_model.Draw(_01_alienTransformModel.size());

		for (int i = 0; i < _03_alien_plantTransformModel.size(); i++)
			modelMatrix[i] = _03_alien_plantTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _03_alien_plantTransformModel.size());
		_03_alien_plant_1_model.Draw(_03_alien_plantTransformModel.size());

		for (int i = 0; i < _06_alien_plantTransformModel.size(); i++)
			modelMatrix[i] = _06_alien_plantTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _06_alien_plantTransformModel.size());
		_06_alien_plant_stl_textured_model.Draw(_06_alien_plantTransformModel.size());

		for (int i = 0; i < _10_bioluminescentTransformModel.size(); i++)
			modelMatrix[i] = _10_bioluminescentTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _10_bioluminescentTransformModel.size());
		_10_bioluminescent_mushroom_model.Draw(_10_bioluminescentTransformModel.size());

		for (int i = 0; i < _11_blue_flowerTransformModel.size(); i++)
			modelMatrix[i] = _11_blue_flowerTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _11_blue_flowerTransformModel.size());
		_11_blue_flower_animated_model.Draw(_11_blue_flowerTransformModel.size());

		for (int i = 0; i < _16_coralTransformModel.size(); i++)
			modelMatrix[i] = _16_coralTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _16_coralTransformModel.size());
		_16_coral_model.Draw(_16_coralTransformModel.size());

		for (int i = 0; i < _17_coralTransformModel.size(); i++)
			modelMatrix[i] = _17_coralTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _17_coralTransformModel.size());
		_17_coral_piece_model.Draw(_17_coralTransformModel.size());

		for (int i = 0; i < _19_exotic_pinkTransformModel.size(); i++)
			modelMatrix[i] = _19_exotic_pinkTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _19_exotic_pinkTransformModel.size());
		_19_exotic_pink_leafed_plant_model.Draw(_19_exotic_pinkTransformModel.size());

		for (int i = 0; i < _20_fantastic_plantTransformModel.size(); i++)
			modelMatrix[i] = _20_fantastic_plantTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _20_fantastic_plantTransformModel.size());
		_20_fantastic_plant_illustrated_by_hand_made_model.Draw(_20_fantastic_plantTransformModel.size());

		for (int i = 0; i < _22_fantasyTransformModel.size(); i++)
			modelMatrix[i] = _22_fantasyTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _22_fantasyTransformModel.size());
		_22_fantasy_plant_2_model.Draw(_22_fantasyTransformModel.size());

		for (int i = 0; i < _24_glowingTransformModel.size(); i++)
			modelMatrix[i] = _24_glowingTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _24_glowingTransformModel.size());
		_24_glowing_mushroom_model.Draw(_24_glowingTransformModel.size());

		for (int i = 0; i < _29_mushroomsTransformModel.size(); i++)
			modelMatrix[i] = _29_mushroomsTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _29_mushroomsTransformModel.size());
		_29_mushrooms_blue_oyster_model.Draw(_29_mushroomsTransformModel.size());

		for (int i = 0; i < _31_pine_coneTransformModel.size(); i++)
			modelMatrix[i] = _31_pine_coneTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _31_pine_coneTransformModel.size());
		_31_pine_cone_model.Draw(_31_pine_coneTransformModel.size());

		for (int i = 0; i < _33_plantaTransformModel.size(); i++)
			modelMatrix[i] = _33_plantaTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _33_plantaTransformModel.size());
		_33_planta_alienigena_texturizada_model.Draw(_33_plantaTransformModel.size());

		for (int i = 0; i < _34_plantaTransformModel.size(); i++)
			modelMatrix[i] = _34_plantaTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _34_plantaTransformModel.size());
		_34_planta_fantasia_de_colores_pasteles_model.Draw(_34_plantaTransformModel.size());

		for (int i = 0; i < _36_plantaTransformModel.size(); i++)
			modelMatrix[i] = _36_plantaTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _36_plantaTransformModel.size());
		_36_planta_fantasia_en_flor_model.Draw(_36_plantaTransformModel.size());

		for (int i = 0; i < _40_stylizedTransformModel.size(); i++)
			modelMatrix[i] = _40_stylizedTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _40_stylizedTransformModel.size());
		_40_stylized_mushrooms_collection_model.Draw(_40_stylizedTransformModel.size());

		for (int i = 0; i < _41_stylizedTransformModel.size(); i++)
			modelMatrix[i] = _41_stylizedTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _41_stylizedTransformModel.size());
		_41_stylized_tree_model.Draw(_41_stylizedTransformModel.size());

		for (int i = 0; i < _44_treeTransformModel.size(); i++)
			modelMatrix[i] = _44_treeTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, _44_treeTransformModel.size());
		_44_tree_speed_sculpt_model.Draw(_44_treeTransformModel.size());

#pragma endregion
	}*/

	/*for (int i = 0; i < transformModel.size(); i++)
	{
		modelMatrix[i] = transformModel[i].GetTransformationMatrix();
	}
	SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, transformModel.size());
	_36_planta_fantasia_en_flor_model.Draw(transformModel.size());
	UnsetStaticModelShader();*/

	DisplayDebugDataScene1OpeningScene();
}

void UninitializeScene1OpeningScene(void)
{
	UninitializeTerrianLearnOGL();
}

void Scene1OpeningSceneCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Local function declarations
	void UpdatePointLightUBOData(std::vector<POINTLIGHTMIRROR> lights);

	// variable decl
	vec4 position;
	switch (message)
	{
	case WM_CHAR:
		switch (wParam)
		{
		case 'X':
			if (iTransformMode == TRANSFORM_MODES::MODEL_TRANSFORM)
				ModelTransform(transformModel, currentModel, currentTranform, steps, 1.0f, 0.0f, 0.0f);
			else if (iTransformMode == TRANSFORM_MODES::LIGHT_TRANSFORM)
			{
				UpdateLightParameter(lights, currentLightIndex, currentLightMember, steps, 1.0f, 0.0f, 0.0 , range);
				UpdatePointLightUBOData(lights);
			}
			break;

		case 'x':
			if (iTransformMode == TRANSFORM_MODES::MODEL_TRANSFORM)
				ModelTransform(transformModel, currentModel, currentTranform, steps, -1.0f, 0.0f, 0.0f);
			else if (iTransformMode == TRANSFORM_MODES::LIGHT_TRANSFORM)
			{
				UpdateLightParameter(lights, currentLightIndex, currentLightMember, steps, -1.0f, 0.0f, 0.0f, range);
				UpdatePointLightUBOData(lights);
			}
			break;

		case 'Y':
			if (iTransformMode == TRANSFORM_MODES::MODEL_TRANSFORM)
				ModelTransform(transformModel, currentModel, currentTranform, steps, 0.0f, 1.0f, 0.0f);
			else if (iTransformMode == TRANSFORM_MODES::LIGHT_TRANSFORM)
			{
				UpdateLightParameter(lights, currentLightIndex, currentLightMember, steps, 0.0f, 1.0f, 0.0f, range);
				UpdatePointLightUBOData(lights);
			}
			break;

		case 'y':
			if (iTransformMode == TRANSFORM_MODES::MODEL_TRANSFORM)
				ModelTransform(transformModel, currentModel, currentTranform, steps, 0.0f, -1.0f, 0.0f);
			else if (iTransformMode == TRANSFORM_MODES::LIGHT_TRANSFORM)
			{
				UpdateLightParameter(lights, currentLightIndex, currentLightMember, steps, 0.0f, -1.0f, 0.0f, range);
				UpdatePointLightUBOData(lights);
			}
			break;

		case 'Z':
			if (iTransformMode == TRANSFORM_MODES::MODEL_TRANSFORM)
				ModelTransform(transformModel, currentModel, currentTranform, steps, 0.0f, 0.0f, 1.0f);
			else if (iTransformMode == TRANSFORM_MODES::LIGHT_TRANSFORM)
			{
				UpdateLightParameter(lights, currentLightIndex, currentLightMember, steps, 0.0f, 0.0f, 1.0f, range);
				UpdatePointLightUBOData(lights);
			}
			break;

		case 'z':
			if (iTransformMode == TRANSFORM_MODES::MODEL_TRANSFORM)
				ModelTransform(transformModel, currentModel, currentTranform, steps, 0.0f, 0.0f, -1.0f);
			else if (iTransformMode == TRANSFORM_MODES::LIGHT_TRANSFORM)
			{
				UpdateLightParameter(lights, currentLightIndex, currentLightMember, steps, 0.0f, 0.0f, -1.0f, range);
				UpdatePointLightUBOData(lights);
			}
			break;

		case '*':
			steps *= 10.0f;
			break;

		case '/':
			steps /= 10.0f;
			break;

		case 't':
		case 'T':
			if (iTransformMode == TRANSFORM_MODES::MODEL_TRANSFORM)
				currentTranform = (currentTranform + 1) % 3;
			else if (iTransformMode == TRANSFORM_MODES::LIGHT_TRANSFORM)
				currentLightMember = (currentLightMember + 1) % NUM_LIGHTS;
			break;

		case 'n':
		case 'N':
			if (iTransformMode == TRANSFORM_MODES::MODEL_TRANSFORM)
			{
				if (transformModel.size() == 0)
				{
					currentModel = -1;
					break;
				}
				currentModel = (currentModel + 1U) % transformModel.size();
			}
			else if (iTransformMode == TRANSFORM_MODES::LIGHT_TRANSFORM)
			{
				if (lights.size() == 0)
				{
					currentLightIndex = -1;
					break;
				}
				currentLightIndex = (currentLightIndex + 1U) % lights.size();
			}
			break;

		case 'r':
		case 'R':
			if (iTransformMode == TRANSFORM_MODES::MODEL_TRANSFORM)
			{
				if (transformModel.size() == 0)
				{
					currentModel = -1;
				}
				else if (currentModel == 0)
				{
					currentModel = (UINT)transformModel.size() - 1U;
				}
				else
				{
					currentModel = (currentModel - 1) % transformModel.size();
				}
			}
			else if (iTransformMode == TRANSFORM_MODES::LIGHT_TRANSFORM)
			{
				if (lights.size() == 0)
				{
					currentLightIndex = -1;
				}
				else if (currentLightIndex == 0)
				{
					currentLightIndex = (UINT)lights.size() - 1U;
				}
				else
				{
					currentLightIndex = (currentLightIndex - 1) % lights.size();
				}
			}
			
			break;

		case 'c':
		case 'C':
		case 'i':
		case 'I':
			if (iTransformMode == TRANSFORM_MODES::MODEL_TRANSFORM)
			{
				if (wParam == 'i' || wParam == 'I')
				{
					if (transformModel.size() > 0)
					{
						transformModel.push_back(transformModel[currentModel]);
					}
					else
					{
						transformModel.push_back(Transform());
					}
				}
				else
				{
					vec3 pos = cameraPosition + cameraFront * 10.0f;
					transformModel.push_back(Transform(pos[0], pos[1], pos[2], 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f));
				}

				currentModel = (int)transformModel.size() - 1;
			}
			

			break;

		// to change tranform mode
		case '1':
			iTransformMode = (TRANSFORM_MODES)((iTransformMode + 1) % TRANSFORM_MODES::NUM_MODES);
			break;

		case '2':
			if (iTransformMode == TRANSFORM_MODES::LIGHT_TRANSFORM)
			{
				if (lights.size() > 0)
					position = lights[currentLightIndex].position;
				else
					position = vec4(0.0f, 0.0f, 0.0f, 1.0f);

				AddPointLight(lights, position);

				currentLightIndex = (UINT)lights.size() - 1U;
			}
			break;

		case 'P':
		case 'p':
			for (int i = 0; i < transformModel.size(); i++)
			{
				DebugLog(NULL, "T[%ff, %ff, %ff], S[%ff, %ff, %ff], R[%ff, %ff, %ff] //%d\n",
					transformModel[i].tx, transformModel[i].ty, transformModel[i].tz,
					transformModel[i].sx, transformModel[i].sy, transformModel[i].sz,
					transformModel[i].rx, transformModel[i].ry, transformModel[i].rz, i);
			}
			break;

		case 'O':
			range += 10.0f;
			if (iTransformMode == TRANSFORM_MODES::LIGHT_TRANSFORM)
			{
				UpdateLightParameter(lights, currentLightIndex, currentLightMember, steps, 0.0f, 0.0f, 0.0f, range);
				UpdatePointLightUBOData(lights);
			}
			break;

		case 'o':
			range -= 10.0f;
			if (iTransformMode == TRANSFORM_MODES::LIGHT_TRANSFORM)
			{
				UpdateLightParameter(lights, currentLightIndex, currentLightMember, steps, 0.0f, 0.0f, 0.0f, range);
				UpdatePointLightUBOData(lights);
			}
			break;

		default:
			break;
		}
		break; // break of WM_CHAR

	default:
		break;
	}
}

bool createFBO_ForShadows(void)
{
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &depth_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, FBO_SHADOW_WIDTH, FBO_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);
	glDrawBuffer(GL_NONE); // No color buffer is drawn to.

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	return true;
}

void DisplayDebugDataScene1OpeningScene()
{
	// font rendering
	SetFontShader(mat4::identity());
	char str[1024];
	sprintf(str, "Camera Translate : [%f, %f, %f]", cameraPosition[0], cameraPosition[1], cameraPosition[2]);
	RenderText(str, -1.0f, -0.9f, 0.001f, vec3(1.0f, 1.0f, 0.0f), characters_font1);

	char transformStr[20];

	if (iTransformMode == MODEL_TRANSFORM)
	{
		if (currentTranform == 0)
		{
			memcpy(transformStr, "Translate", strlen("Translate") + 1);
		}
		else if (currentTranform == 1)
		{
			memcpy(transformStr, "Scale", strlen("Scale") + 1);
		}
		else
		{
			memcpy(transformStr, "Rotate", strlen("Rotate") + 1);
		}

		sprintf(str, "object index:[%d], steps: %f, Transform:%s, ObjectCount: %u",
			currentModel, steps, transformStr, (UINT)transformModel.size());
		RenderText(str, -1.0f, 0.9f, 0.001f, vec3(1.0f, 1.0f, 0.0f), characters_font1);

		sprintf(str, "Translate:[%f, %f, %f]", transformModel[currentModel].tx, transformModel[currentModel].ty, transformModel[currentModel].tz);
		RenderText(str, -1.0f, 0.8f, 0.001f, vec3(1.0f, 1.0f, 0.0f), characters_font1);

		sprintf(str, "Scale:[%f, %f, %f]", transformModel[currentModel].sx, transformModel[currentModel].sy, transformModel[currentModel].sz);
		RenderText(str, -1.0f, 0.7f, 0.001f, vec3(1.0f, 1.0f, 0.0f), characters_font1);

		sprintf(str, "Rotate:[%f, %f, %f]", transformModel[currentModel].rx, transformModel[currentModel].ry, transformModel[currentModel].rz);
		RenderText(str, -1.0f, 0.6f, 0.001f, vec3(1.0f, 1.0f, 0.0f), characters_font1);
	}
	else if (iTransformMode == LIGHT_TRANSFORM)
	{
		if(currentLightMember == 0)
		{
			memcpy(transformStr, "ambient", strlen("ambient") + 1);
		}
		else if (currentLightMember == 1)
		{
			memcpy(transformStr, "diffuse", strlen("diffuse") + 1);
		}
		else if(currentLightMember == 2)
		{
			memcpy(transformStr, "specular", strlen("specular") + 1);
		}
		else if (currentLightMember == 3)
		{
			memcpy(transformStr, "position", strlen("position") + 1);
		}
		else if (currentLightMember == 4)
		{
			memcpy(transformStr, "linear", strlen("linear") + 1);
		}
		else if (currentLightMember == 5)
		{
			memcpy(transformStr, "constant", strlen("constant") + 1);
		}
			 
		sprintf(str, "light index:[%d], steps: %f, transform : %s  ObjectCount:%u, currentLightMember:%d, range:%f",
			currentLightIndex, steps, transformStr, (UINT)lights.size(), currentLightMember, range);
		RenderText(str, -1.0f, 0.9f, 0.001f, vec3(1.0f, 1.0f, 0.0f), characters_font1);

		sprintf(str, "Translate:[%f, %f, %f]", lights[currentLightIndex].position[0], lights[currentLightIndex].position[1], lights[currentLightIndex].position[2]);
		RenderText(str, -1.0f, 0.8f, 0.001f, vec3(1.0f, 1.0f, 0.0f), characters_font1);
		sprintf(str, "ambient:[%f, %f, %F], diffuse : [%f, %f, %f], specular[%f, %f, %f]", lights[currentLightIndex].ambient[0], lights[currentLightIndex].ambient[1], lights[currentLightIndex].ambient[2], lights[currentLightIndex].diffuse[0], lights[currentLightIndex].diffuse[1], lights[currentLightIndex].diffuse[2], lights[currentLightIndex].specular[0], lights[currentLightIndex].specular[1], lights[currentLightIndex].specular[2]);
		RenderText(str, -1.0f, 0.7f, 0.001f, vec3(1.0f, 1.0f, 0.0f), characters_font1);
		sprintf(str, "linear :[%f], quadratic : [%f]", lights[currentLightIndex].linear, lights[currentLightIndex].quadratic);
		RenderText(str, -1.0f, 0.6f, 0.001f, vec3(1.0f, 1.0f, 0.0f), characters_font1);	
	}
	

	UnsetFontShader();
}

void UpdatePointLightUBOData(std::vector<POINTLIGHTMIRROR> lights)
{
	for (int pointLightIndex = 0; pointLightIndex < MAX_POINT_LIGHTS_COUNT && pointLightIndex < lights.size(); pointLightIndex++)
	{
		gPointLightsUBO.data.pointLights[pointLightIndex] = lights[pointLightIndex];
	}
	gPointLightsUBO.isUpdated = true;
}