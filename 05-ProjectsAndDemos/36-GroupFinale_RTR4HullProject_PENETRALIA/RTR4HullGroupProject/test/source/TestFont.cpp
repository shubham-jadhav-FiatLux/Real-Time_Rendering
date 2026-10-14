#include "TestEffect.h"
#include "FontShader.h"

bool InitializeTestFont(void)
{
	// Note : initalization of multiple fot is laredy done in InitFont() function of FontShaper.cpp

	return true;
}

void DisplayTestFont(void)
{
	// code
	
	mat4 translationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	// font rendering

	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	modelMatrix = translationMatrix;
	SetFontShader(modelMatrix);
	RenderText("hello", 2.3f, 0.0f, 0.01f, vec3(0.0f, 1.0f, 0.0f), characters_font1);
	UnsetFontShader();

	translationMatrix = vmath::translate(-0.5f, 0.0f, -3.0f);
	modelMatrix = translationMatrix;
	SetFontShader(modelMatrix);
	RenderText("welcome", -2.3f, 0.0f, 0.01f, vec3(0.0f, 1.0f, 0.0f), characters_font2);
	UnsetFontShader();
}

void UninitializeTestFont(void)
{
	UninitializeFont();
}