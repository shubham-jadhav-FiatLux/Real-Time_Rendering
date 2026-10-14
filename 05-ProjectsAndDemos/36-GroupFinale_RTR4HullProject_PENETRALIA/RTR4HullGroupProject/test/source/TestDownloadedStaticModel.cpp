#include "TestDownloadedStaticModel.h"
#include "transform_model/TransformModel.h"
#include "Texture.h"
#include "TextureShader.h"
#include "CubeRender.h"
#include "FontShader.h"
#include "static_model_loading/StaticModel.h"
#include "StaticModelShader.h"
#include "terrian/TerrianLearnOGL.h"

static GLuint image_texture;
static Model test_model;

static Model _01_alien_flower_model;
static Model _03_alien_plant_1_model;
static Model _06_alien_plant_stl_textured_model;
static Model _10_bioluminescent_mushroom_model;
static Model _11_blue_flower_animated_model;
static Model _16_coral_model;
static Model _17_coral_piece_model;
static Model _19_exotic_pink_leafed_plant_model;
static Model _20_fantastic_plant_illustrated_by_hand_made_model;
//static Model _21_fantasy_light_plant_animated_model;
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

#define DEBUG_DATA  1

static float steps = 0.1f;
static std::vector<struct  Transform> transformModel;
static int currentModel = -1;
static int currentTranform = 0;

//static GLfloat lightPosition_X = 100.0f, lightPosition_Y = 0.0f, lightPosition_Z = 0.0f;
static vec4 lightPosition = vmath::vec4(lightPosition_X, lightPosition_Y, lightPosition_Z, 1.0f);

static std::vector<struct  Transform> _01_alienTransformModel;
static std::vector<struct  Transform> _03_alien_plantTransformModel;
static std::vector<struct  Transform> _06_alien_plantTransformModel;
static std::vector<struct  Transform> _10_bioluminescentTransformModel;
static std::vector<struct  Transform> _11_blue_flowerTransformModel;
static std::vector<struct  Transform> _16_coralTransformModel;
static std::vector<struct  Transform> _17_coralTransformModel;
static std::vector<struct  Transform> _19_exotic_pinkTransformModel;
static std::vector<struct  Transform> _20_fantastic_plantTransformModel;
//static std::vector<struct  Transform> _21_fantasy_lightTransformModel;
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


bool InitializeTestDownloadedStaticModel(void)
{
	InitializeTerrianLearnOGL();
	image_texture = CreateTexture("resources/test.png");

	//test_model = Model("resources\\models\\test_models\\backpack\\backpack.obj");
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
	
	//_21_fantasy_lightTransformModel.push_back(Transform(0.0f, -6.400001f, 50.0f, 1.0f, 1.0f, 1.0f, 0.0f, -0.0f, 0.0f));
	
	
	
	
	_44_treeTransformModel.push_back(Transform(-24.299995f, -4.300003f, 76.600052f, 1.0f, 1.0f, 1.0f, 0.0f, -0.0f, 0.0f));
	
	transformModel.push_back(Transform(0.0f, -6.400001f, 100.0f, 1.0f, 1.0f, 1.0f, 0.0f, -0.0f, 0.0f));

	_41_stylizedTransformModel.push_back(Transform(-66.000000f, -6.400001f, 98.700020f, 1.0f, 1.0f, 1.0f, 0.0f, -0.0f, 0.0f));
	_40_stylizedTransformModel.push_back(Transform(-12.770013f, -5.940000f, 110.399841f, 6.099997f, 6.099997f, 6.999996f, 0.0f, -0.0f, 0.0f));
	_36_plantaTransformModel.push_back(Transform(-40.499954f, -5.100001f, 85.900215f, 0.400000f, 0.400000f, 0.400000f, 0.0f, -0.0f, 0.0f));
	_34_plantaTransformModel.push_back(Transform(-29.200075f, -9.200003f, 100.000000f, 0.200000f, 0.200000f, 0.200000f, 0.0f, -0.0f, 0.0f));
	_33_plantaTransformModel.push_back(Transform(21.800047f, -6.200001f, 87.200195f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f));
	_31_pine_coneTransformModel.push_back(Transform(-6.099997f, -5.500002f, 92.200119f, 0.100000f, 0.100000f, 0.100000f, 0.0f, 0.0f, 0.0f));
	_29_mushroomsTransformModel.push_back(Transform(7.199996f, -6.400001f, 105.599915f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f));
	_24_glowingTransformModel.push_back(Transform(7.199996f, -6.400001f, 105.599915f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f));
	_22_fantasyTransformModel.push_back(Transform(-13.600016f, -6.400001f, 92.600113f, 7.399995f, 7.699995f, 8.299995f, 0.0f, 0.0f, 0.0f));
	_20_fantastic_plantTransformModel.push_back(Transform(24.800058f, -6.000001f, 100.000000f, 0.200000f, 0.200000f, 0.200000f, 0.0f, -0.0f, 0.0f));
	_19_exotic_pinkTransformModel.push_back(Transform(21.900047f, -6.400001f, 100.000000f, 2.700000f, 2.700000f, 2.700000f, 0.0f, -0.0f, 0.0f));
	_17_coralTransformModel.push_back(Transform(-15.500023f, -5.600002f, 100.000000f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f));
	_16_coralTransformModel.push_back(Transform(14.680020f, -6.599994f, 100.000000f, 0.100000f, 0.100000f, 0.100000f, -90.000000f, 0.000000f, 0.000000f));
	_11_blue_flowerTransformModel.push_back(Transform(-9.400000f, -6.400001f, 100.000000f, 4.099998f, 3.299999f, 3.099999f, 0.0f, 0.0f, 0.0f));
	_10_bioluminescentTransformModel.push_back(Transform(12.600012f, -6.670002f, 85.299461f, 0.600000f, 0.600000f, 0.600000f, 0.0f, -0.0f, 0.0f));
	_06_alien_plantTransformModel.push_back(Transform(-7.199996f, -6.000001f, 81.799515f, 5.099998f, 5.099998f, 5.099998f, 0.0f, -0.0f, 0.0f));
	_03_alien_plantTransformModel.push_back(Transform(-10.500004f, -6.200001f, 78.299568f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f));
	_01_alienTransformModel.push_back(Transform(0.000000f, -6.400001f, 77.900002f, 0.700000f, 0.700000f, 0.700000f, 0.0f, 0.0f, 0.0f)); // 01

	currentModel = (int)transformModel.size() - 1;
	return true;
}

void DisplayTestDownloadedStaticModel(void)
{
	mat4 modelMatrix = mat4::identity();

	DisplayTerrianLearnOGL();

	/*for (int i = 0; i < _01_alienTransformModel.size(); i++)
	{
		modelMatrix = _01_alienTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_01_alien_flower_model.Draw();
	}

	for (int i = 0; i < _03_alien_plantTransformModel.size(); i++)
	{
		modelMatrix = _03_alien_plantTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_03_alien_plant_1_model.Draw();
	}

	for (int i = 0; i < _06_alien_plantTransformModel.size(); i++)
	{
		modelMatrix = _06_alien_plantTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_06_alien_plant_stl_textured_model.Draw();
	}

	for (int i = 0; i < _10_bioluminescentTransformModel.size(); i++)
	{
		modelMatrix = _10_bioluminescentTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_10_bioluminescent_mushroom_model.Draw();
	}

	for (int i = 0; i < _11_blue_flowerTransformModel.size(); i++)
	{
		modelMatrix = _11_blue_flowerTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_11_blue_flower_animated_model.Draw();
	}

	for (int i = 0; i < _16_coralTransformModel.size(); i++)
	{
		modelMatrix = _16_coralTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_16_coral_model.Draw();
	}

	for (int i = 0; i < _17_coralTransformModel.size(); i++)
	{
		modelMatrix = _17_coralTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_17_coral_piece_model.Draw();
	}

	for (int i = 0; i < _19_exotic_pinkTransformModel.size(); i++)
	{
		modelMatrix = _19_exotic_pinkTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_19_exotic_pink_leafed_plant_model.Draw();
	}

	for (int i = 0; i < _20_fantastic_plantTransformModel.size(); i++)
	{
		modelMatrix = _20_fantastic_plantTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_20_fantastic_plant_illustrated_by_hand_made_model.Draw();
	}

	for (int i = 0; i < _22_fantasyTransformModel.size(); i++)
	{
		modelMatrix = _22_fantasyTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_22_fantasy_plant_2_model.Draw();
	}
	for (int i = 0; i < _24_glowingTransformModel.size(); i++)
	{
		modelMatrix = _24_glowingTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_24_glowing_mushroom_model.Draw();
	}
	for (int i = 0; i < _29_mushroomsTransformModel.size(); i++)
	{
		modelMatrix = _29_mushroomsTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_29_mushrooms_blue_oyster_model.Draw();
	}
	for (int i = 0; i < _31_pine_coneTransformModel.size(); i++)
	{
		modelMatrix = _31_pine_coneTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_31_pine_cone_model.Draw();
	}
	for (int i = 0; i < _33_plantaTransformModel.size(); i++)
	{
		modelMatrix = _33_plantaTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_33_planta_alienigena_texturizada_model.Draw();
	}
	for (int i = 0; i < _34_plantaTransformModel.size(); i++)
	{
		modelMatrix = _34_plantaTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_34_planta_fantasia_de_colores_pasteles_model.Draw();
	}
	for (int i = 0; i < _36_plantaTransformModel.size(); i++)
	{
		modelMatrix = _36_plantaTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_36_planta_fantasia_en_flor_model.Draw();
	}
	for (int i = 0; i < _40_stylizedTransformModel.size(); i++)
	{
		modelMatrix = _40_stylizedTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_40_stylized_mushrooms_collection_model.Draw();
	}
	for (int i = 0; i < _41_stylizedTransformModel.size(); i++)
	{
		modelMatrix = _41_stylizedTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_41_stylized_tree_model.Draw();
	}
	for (int i = 0; i < _44_treeTransformModel.size(); i++)
	{
		modelMatrix = _44_treeTransformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_44_tree_speed_sculpt_model.Draw();
	}
	for (int i = 0; i < transformModel.size(); i++)
	{
		modelMatrix = transformModel[i].GetTransformationMatrix();
		SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic);
		_44_tree_speed_sculpt_model.Draw();
	}
	UnsetStaticModelShader();*/
#if DEBUG_DATA
	DisplayDebugDataTestDownloadedStaticModel();
#endif
}

void UninitializeTestDownloadedStaticModel(void)
{
	transformModel.clear();
	transformModel.shrink_to_fit();
}

void DisplayDebugDataTestDownloadedStaticModel()
{
	// font rendering
	SetFontShader(mat4::identity());
	char str[1024];
	sprintf(str, "Camera Translate : [%f, %f, %f]", cameraPosition[0], cameraPosition[1], cameraPosition[2]);
	RenderText(str, -1.0f, -0.9f, 0.001f, vec3(1.0f, 1.0f, 0.0f), characters_font1);

	char transformStr[20];
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

	UnsetFontShader();
}

void TestDownloadedStaticModelCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:
		switch (wParam)
		{
		case 'X':
			ModelTransform(transformModel, currentModel, currentTranform, steps, 1.0f, 0.0f, 0.0f);
			break;

		case 'x':
			ModelTransform(transformModel, currentModel, currentTranform, steps, -1.0f, 0.0f, 0.0f);
			break;

		case 'Y':
			ModelTransform(transformModel, currentModel, currentTranform, steps, 0.0f, 1.0f, 0.0f);
			break;

		case 'y':
			ModelTransform(transformModel, currentModel, currentTranform, steps, 0.0f, -1.0f, 0.0f);
			break;

		case 'Z':
			ModelTransform(transformModel, currentModel, currentTranform, steps, 0.0f, 0.0f, 1.0f);
			break;

		case 'z':
			ModelTransform(transformModel, currentModel, currentTranform, steps, 0.0f, 0.0f, -1.0f);
			break;

		case '*':
			steps *= 10.0f;
			break;

		case '/':
			steps /= 10.0f;
			break;

		case 't':
		case 'T':
			currentTranform = (currentTranform + 1) % 3;
			break;

		case 'n':
		case 'N':
			if (transformModel.size() == 0)
			{
				currentModel = -1;
				break;
			}
			currentModel = (currentModel + 1U) % transformModel.size();
			break;

		case 'r':
		case 'R':
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
			break;

		case 'c':
		case 'C':
		case 'i':
		case 'I':

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

			break;

		case 'P':
			for (int i = 0; i < transformModel.size(); i++)
			{
				DebugLog(NULL, "T[%ff, %ff, %ff], S[%ff, %ff, %ff], R[%ff, %ff, %ff] //%d\n",
					transformModel[i].tx, transformModel[i].ty, transformModel[i].tz,
					transformModel[i].sx, transformModel[i].sy, transformModel[i].sz,
					transformModel[i].rx, transformModel[i].ry, transformModel[i].rz, i);
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