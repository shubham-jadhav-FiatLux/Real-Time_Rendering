#include "TestBloom.h"
#include "transform_model/TransformModel.h"
#include "Texture.h"
#include "TextureShader.h"
#include "CubeRender.h"
#include "FontShader.h"


#include "bloom/BloomLearnOGLShader.h"
#include "bloom/BloomLearnOGL.h"


static GLuint image_texture;

#define DEBUG_DATA  1

static float steps = 0.1f;
static std::vector<struct  Transform> transformModel;
static int currentModel = -1;
static int currentTranform = 0;

bool InitializeTestBloomScene(void)
{
	InitializeBloomLearnOGLShader();
	InitializeBloomLearnOGL();



	image_texture = CreateTexture("resources/test.png");

	transformModel.push_back(Transform(0.0f, 0.0f, -3.0f, 1.0f, 1.0f, 1.0f, 67.0f, 45.0f, 90.0f));

	currentModel = (int)transformModel.size() - 1;
	return true;
}

void DisplayTestBloomScene(void)
{
	mat4 modelMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();

	/*for (int i = 0; i < transformModel.size(); i++)
	{
		modelMatrix = transformModel[i].GetTransformationMatrix();
		SetTextureShader(modelMatrix, image_texture);
		RenderCube();
	}
	UnsetTextureShader();*/

	/*translationMatrix = vmath::translate(4.0f, 0.0f, -7.0f);
	scaleMatrix = vmath::scale(0.3f, 0.3f, 0.3f);
	modelMatrix = translationMatrix * scaleMatrix;
	SetStaticModelShader(modelMatrix, lightPosition, cameraPosition);
	alienTree_model.Draw();
	UnsetStaticModelShader();*/

	DisplayBloomLearnOGL();

#if DEBUG_DATA
	DisplayDebugDataTestBloom();
#endif
}

void UninitializeTestBloomScene(void)
{
	transformModel.clear();
	transformModel.shrink_to_fit();
}

void DisplayDebugDataTestBloom()
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

void TestBloomSceneCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			currentTranform = (currentModel + 1) % 3;
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
				ErrorLog(NULL, "T[%ff, %ff, %ff], S[%ff, %ff, %ff], R[%ff, %ff, %ff] //%d\n",
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