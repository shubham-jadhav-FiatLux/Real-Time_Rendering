#include "model/BoatModel.h"
#include <ColorShader.h>
#include <CubeRender.h>

//boat
static GLuint vao_boat;
static GLuint vbo_boat_position;
static GLuint vbo_boat_texcoord;
static GLuint vbo_boat_normal;

vmath::vec3 currentBoatPivotPosition = vmath::vec3(1340.0f, 0.0f, -880.0f);
vmath::vec3 boatScaling = vmath::vec3(10.0f, 10.0f, 10.f);

vmath::vec3 currentBoatFrontPosition = vmath::vec3(1340.0f, 0.0f, -880.0f + boatScaling[2]);

vmath::vec4 boatColor = vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f);

vmath::vec3 currentBoatPivotDeviation = vmath::vec3(0.0f, 0.0f, 0.0f);
vmath::vec3 currentBoatFrontDeviation = vmath::vec3(0.0f, 0.0f, 0.0f);

bool InitializeBoat(void)
{
	// TODO_01: Initialize boat model here.
	return true;
}

void DisplayBoat(void)
{
	// Variable declarations
	float boatInclination = 0.0f;
	float theta = 0.0f;

	vmath::mat4 currentBoatPositionMatrix = vmath::translate(currentBoatPivotPosition);
	vmath::mat4 scaleMatrix = vmath::scale(boatScaling);
	vmath::mat4 rotationYZ = vmath::mat4::identity();
	vmath::mat4 currentBoatTranslation = vmath::translate(currentBoatPivotDeviation);
	vmath::mat4 modelMatrix;


	if (
		(currentBoatFrontDeviation[1] - currentBoatPivotDeviation[1])
		&& (currentBoatFrontPosition[2] - currentBoatPivotPosition[2])
		)
	{
		boatInclination = (currentBoatFrontDeviation[1] - currentBoatPivotDeviation[1]) / (currentBoatFrontPosition[2] - currentBoatPivotPosition[2]);
	}

	theta = atanf(boatInclination);

	DebugLog(NULL, "DisplayBoat()::theta: %f", theta);

	rotationYZ = vmath::rotate(vmath::degrees(theta), 1.0f, 0.0f, 0.0f);

	modelMatrix = currentBoatPositionMatrix * currentBoatTranslation * scaleMatrix * rotationYZ;

	SetColorShader(modelMatrix, boatColor);

	RenderCube();

	UnsetColorShader();
}

void UninitializeBoat(void)
{
	// TODO_01: Uninitialize boat model here.
}
