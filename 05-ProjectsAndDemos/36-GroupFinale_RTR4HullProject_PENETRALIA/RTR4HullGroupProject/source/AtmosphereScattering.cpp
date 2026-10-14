#include "AtmosphereScattering.h"
#include "AtmosphereScatteringShader.h"
#include "ColorShader.h"

#define RAYLEIGHT_CONSTANT 0.0055
#define MIE_CONSTANT 0.0015
#define SECTOR_COUNT 300
#define STACK_COUNT 300

GLuint vao_outersphere;
GLuint vbo_outersphere_position;
GLuint vbo_outersphere_elements;
GLuint outersphere_vertices_count;
GLuint outersphere_indices_count;

GLfloat* outerspherePosition = nullptr;
GLuint* outersphereIndices = nullptr;

GLfloat innerSphereRadius = 40000.0f;
GLfloat outerSphereRadius = 41000.0f;// innerSphereRadius * 1.025f
float gScale;
float sun = 20.0f;
vec3 atmo_lightPosSphere;
float cameraHeight;
vec3 camPos;

bool InitializeAtmosphereScattering(void)
{
	InitializeAtmosphereScatteringShader();

	InitializeOuterSphere(SECTOR_COUNT, STACK_COUNT, outerSphereRadius);

	gScale = 1.0f / (outerSphereRadius - innerSphereRadius);

	return true;
}

int InitializeOuterSphere(int slices, int stacks, float radius)
{
	// variable declarations
	float x, y, z;
	float u, v;
	int i, j;
	int indexV, indexI;

	int vertex_count;

	// code
	if (radius <= 0.0f)
		radius = 1.0f;

	if (slices < 4)
		slices = 4;

	if (stacks < 3)
		stacks = 3;

	float du = 2.0f * (float)M_PI / (float)(slices - 1);
	float dv = (float)M_PI / (float)(stacks - 1);

	// allocate memory

	vertex_count = slices * stacks;

	outerspherePosition = (GLfloat*)calloc(3 * vertex_count, sizeof(GLfloat));
	//sphereNormal = (GLfloat*)calloc(3 * vertex_count, sizeof(GLfloat));
	//sphereTexcoord = (GLfloat*)calloc(3 * vertex_count, sizeof(GLfloat));
	outersphereIndices = (GLuint*)calloc(2 * (slices - 1) * (stacks - 1) * 3, sizeof(GLuint));

	if (!outerspherePosition || !outersphereIndices)/*!sphereNormal || !sphereTexcoord ||*/
	{
		ErrorLog(NULL, "failed to allocate memeory for sphere data\n");
		exit(1);
	}

	// fill arrays
	indexV = 0;
	for (i = 0; i < stacks; i++)
	{
		v = (float)M_PI / 2.0f - (float)i * dv;
		y = (float)sin(v);

		for (j = 0; j < slices; j++)
		{
			u = j * du;
			x = (float)sin(u) * (float)cos(v);
			z = (float)cos(u) * (float)cos(v);

			outerspherePosition[3 * indexV + 0] = radius * x;
			outerspherePosition[3 * indexV + 1] = radius * y;
			outerspherePosition[3 * indexV + 2] = radius * z;

			/*sphereNormal[3 * indexV + 0] = x;
			sphereNormal[3 * indexV + 1] = y;
			sphereNormal[3 * indexV + 2] = z;

			sphereTexcoord[2 * indexV + 0] = (float)j / (float)(slices - 1);
			sphereTexcoord[2 * indexV + 1] = (float)i / (float)(stacks - 1);*/

			indexV++;
		}
	}

	indexI = 0;
	for (j = 0; j < stacks - 1; j++)
	{
		unsigned int row_one = j * slices;
		unsigned int row_two = (j + 1) * slices;

		for (i = 0; i < slices - 1; i++)
		{
			outersphereIndices[indexI++] = row_one + i;
			outersphereIndices[indexI++] = row_two + i;
			outersphereIndices[indexI++] = row_two + i + 1;

			outersphereIndices[indexI++] = row_one + i;
			outersphereIndices[indexI++] = row_two + i + 1;
			outersphereIndices[indexI++] = row_one + i + 1;
		}
	}
	outersphere_vertices_count = indexV;
	outersphere_indices_count = indexI;


	// create vao and vbo
	glGenVertexArrays(1, &vao_outersphere);
	glBindVertexArray(vao_outersphere);

	glGenBuffers(1, &vbo_outersphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_outersphere_position);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertex_count * sizeof(GLfloat), outerspherePosition, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*glGenBuffers(1, &vbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, 3 * vertex_count * sizeof(GLfloat), sphereNormal, GL_STATIC_DRAW);
	glVertexAttribPointer(ATB_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATB_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_sphere_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_texcoord);
	glBufferData(GL_ARRAY_BUFFER, 2 * vertex_count * sizeof(GLfloat), sphereTexcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(ATB_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATB_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/

	glGenBuffers(1, &vbo_outersphere_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_outersphere_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, outersphere_indices_count * sizeof(GLuint), outersphereIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);


	return true;
}

void CalculateCameraHeight(vec3 camPosition, float* height, vec3* pos)
{
	float x = camPosition[0];
	float y = camPosition[1] + innerSphereRadius;
	float z = camPosition[2];

	*pos = vec3(x, y, z);

	float cameraHeightVector = vmath::length(*pos);
	*height = cameraHeightVector;
}

void DrawAtmosphereScattering(void)
{
	// variable declarations
	float krESun = (float)RAYLEIGHT_CONSTANT * sun;
	float kmESun = (float)MIE_CONSTANT * sun;
	float kr4PI = (float)RAYLEIGHT_CONSTANT * 4 * (float)M_PI;
	float km4PI = (float)MIE_CONSTANT * 4 * (float)M_PI;
	
	
	mat4 modelMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();

	// code
	translationMatrix = vmath::translate(0.0f, -innerSphereRadius, 0.0f);
	//translationMatrix = vmath::translate(0.0f, 10.0f, 0.0f);
	modelMatrix = translationMatrix;

	atmo_lightPosSphere = vec3(0.0f, -0.8f, -10.0f);
	vec3 lightPos = atmo_lightPosSphere;
	lightPos = lightPos / sqrtf((lightPos[0] * lightPos[0]) + (lightPos[1] * lightPos[1]) + (lightPos[2] * lightPos[2]));

	CalculateCameraHeight(cameraPosition, &cameraHeight, &camPos);

	static bool flag = true;
	if (flag)
	{
		flag = false;
		ErrorLog(NULL, "cameraHeight : %f\n", cameraHeight);
		ErrorLog(NULL, "outersphere_indices_count : %d\n", outersphere_indices_count);
		ErrorLog(NULL, "camPos[%f, %f, %f]\n", camPos[0], camPos[1], camPos[2]);
		ErrorLog(NULL, "lightPos[%f, %f, %f]\n", lightPos[0], lightPos[1], lightPos[2]);
	}

	SetAtmosphereScatteringShader(modelMatrix, camPos, lightPos, cameraHeight, outerSphereRadius, innerSphereRadius,krESun, kmESun, kr4PI, km4PI, gScale);
	
	//glPointSize(5.0f);

	glBindVertexArray(vao_outersphere);
	
	//glDrawArrays(GL_POINTS, 0, 3 * SECTOR_COUNT * STACK_COUNT);
	//SetColorShader(modelMatrix);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_outersphere_elements);
	glDrawElements(GL_TRIANGLES, outersphere_indices_count, GL_UNSIGNED_INT, 0);
	//UnsetColorShader();
	glBindVertexArray(0);
	
	UnsetAtmosphereScatteringShader();
}

void UninitializeAtmosphereScattering(void)
{
	UninitializeAtmosphereScatteringShader();
}