#include "TestStaticModel.h"
#include "static_model_loading/StaticModel.h"
#include "StaticModelShader.h"
static Model test_model;
#define SPPED 0.1f

vec4 lightPosition = vmath::vec4(lightPosition_X, lightPosition_Y, lightPosition_Z, 1.0f );
char str[256];
bool InitializeTestStaticModel(void)
{
	//test_model = Model("E:/AMC/RTR4/FinalProject/Models/23-04-2023/Terrain_V_0.0.1/FBX/Terrain_FBX.fbx");
	test_model = Model("resources\\models\\test_models\\backpack\\backpack.obj");
	//test_model = Model("E:/AMC/RTR4/FinalProject/Models/PBR/OBJ/wall/wall.obj");
	//test_model = Model("E:\\AMC\\RTR4\\FinalProject\\Models\\PBR\\house\\house.obj");
	//test_model = Model("E:\\AMC\\RTR4\\FinalProject\\Models\\PBR\\house_try\\fbx\\house.fbx");
	//test_model = Model("resources\\models\\pbr_test\\house\\house.obj");
	//test_model = Model("E:\\AMC\\RTR4\\FinalProject\\Models\\PBR\\house_try\\wall.obj");
	


	return true;
}


void DisplayTestStaticModel(void)
{
	mat4 translationMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	
	mat4 modelMatrix[TEMP_INSTANCE_COUNT_FOR_ALL_MODEL];
	float shift_x = -10.0f;
	for (int i = 0; i < TEMP_INSTANCE_COUNT_FOR_ALL_MODEL; i++)
	{
		translationMatrix = vmath::translate(shift_x, 0.0f, -2.0f);
		scaleMatrix = vmath::scale(0.5f, 0.5f, 0.5f);

		modelMatrix[i] = translationMatrix * scaleMatrix;
		shift_x += 2.0f;
	}
	
	SetStaticModelShader(modelMatrix, lightPosition ,cameraPosition, linear, quadratic, TEMP_INSTANCE_COUNT_FOR_ALL_MODEL);
	test_model.Draw(TEMP_INSTANCE_COUNT_FOR_ALL_MODEL);

	//// second backpack model
	//translationMatrix = vmath::translate(-2.0f, 0.0f, -2.0f);
	//scaleMatrix = vmath::scale(0.5f, 0.5f, 0.5f);

	//modelMatrix = translationMatrix * scaleMatrix;

	//SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, TEMP_INSTANCE_COUNT);
	//test_model.Draw();

	//// third 
	//translationMatrix = vmath::translate(4.0f, 0.0f, -5.0f);
	//scaleMatrix = vmath::scale(0.5f, 0.5f, 0.5f);

	//modelMatrix = translationMatrix * scaleMatrix;

	//SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, TEMP_INSTANCE_COUNT);
	//test_model.Draw();

	//// fourth
	//translationMatrix = vmath::translate(3.0f, 4.0f, -5.0f);
	//scaleMatrix = vmath::scale(0.5f, 0.5f, 0.5f);

	//modelMatrix = translationMatrix * scaleMatrix;

	//SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, TEMP_INSTANCE_COUNT);
	//test_model.Draw();

	//// five
	//translationMatrix = vmath::translate(-1.0f, 3.0f, -5.0f);
	//scaleMatrix = vmath::scale(0.5f, 0.5f, 0.5f);

	//modelMatrix = translationMatrix * scaleMatrix;

	//SetStaticModelShader(modelMatrix, lightPosition, cameraPosition, linear, quadratic, TEMP_INSTANCE_COUNT);
	//test_model.Draw();

	UnsetStaticModelShader();

	sprintf(str, "liner : %f, quadratic : %f", linear, quadratic);
	SetWindowTextA(ghwnd, str);
}

void UninitializeTestStaticModel(void)
{
	UninitializeStaticModelShader();
}

void TestStaticModelSceneCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:
		switch (wParam)
		{
		case 'X':
			lightPosition_X += SPPED;
			lightPosition[0] = lightPosition_X;
			break;

		case 'x':
			lightPosition_X -= SPPED;
			lightPosition[0] = lightPosition_X;
			break;

		case 'Y':
			lightPosition_Y += SPPED;
			lightPosition[1] = lightPosition_Y;
			break;

		case 'y':
			lightPosition_Y -= SPPED;
			lightPosition[1] = lightPosition_Y;
			break;

		case 'Z':
			lightPosition_Z += SPPED;
			lightPosition[2] = lightPosition_Z;
			break;

		case 'z':
			lightPosition_Z -= SPPED;
			lightPosition[2] = lightPosition_Z;
			break;

		case 'k':
			quadratic -= 0.001f;
			if (quadratic <= 0.0f)
				quadratic = 0.0f;
			break;

		case 'K':
			quadratic += 0.001f;
			if (quadratic >= 1.0f)
				quadratic = 1.0f;

			break;

		case 'l':
			linear -= 0.001f;
			if (linear <= 0.0f)
				linear = 0.0f;
			break;

		case 'L':
			linear += 0.001f;
			if (linear >= 1.0f)
				linear = 1.0f;

			break;

		case 'P':
		case 'p':
			TraceLog(NULL, "light pos =[%f, %f, %f]\n", lightPosition[0], lightPosition[1], lightPosition[2]);
			break;

		default:
			break;
		}
		break; // break of WM_CHAR

	default:
		break;
	}
}