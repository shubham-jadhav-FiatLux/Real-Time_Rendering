#include "DefaultScene.h"
#include "Texture.h"
#include "FontShader.h"
#include "TestEffect.h"
static GLuint image_texture;

#include "Terrian/TerrianSuperBibleShader.h"

bool InitializeDefaultScene(void)
{
	//###########################################################
	
	image_texture = CreateTexture("resources/test.png");
	
	InitializeTerrianSuperBibleShader();

	//###########################################################

	return true;
}

void DisplayDefaultScene(void)
{
	// code
	mat4 translationMatrix = mat4::identity();
	mat4 rotationXMatrix = mat4::identity();
	mat4 rotationYMatrix = mat4::identity();
	mat4 rotationZMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	rotationXMatrix = vmath::rotate(120.0f, 1.0f, 0.0f, 0.0f);
	rotationYMatrix = vmath::rotate(50.0f, 0.0f, 1.0f, 0.0f);
	rotationZMatrix = vmath::rotate(80.0f, 0.0f, 0.0f, 1.0f);
	rotationMatrix = rotationXMatrix * rotationYMatrix * rotationZMatrix;
	modelMatrix = translationMatrix * rotationMatrix;

	SetTextureShader(modelMatrix, image_texture);
	//SetPhongLightShader(modelMatrix);
	RenderCube();
	//UnsetPhongLightShader();
	UnsetTextureShader();

}

void UninitializeDefaultScene(void)
{
	DeleteTexture(image_texture);
}