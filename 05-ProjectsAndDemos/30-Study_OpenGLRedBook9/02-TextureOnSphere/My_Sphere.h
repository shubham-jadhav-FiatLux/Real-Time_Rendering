#pragma once
#include "My_Vector.h"
#include "MySphere_Declspec.h"

vec_2d_float_t* sphereVertices = nullptr;
vec_2d_float_t* sphereNormals = nullptr;
vec_2d_float_t* sphereTexcoords = nullptr;
vec_2d_int_t* sphereIndices = nullptr;
unsigned int number_of_vertices = 0;
unsigned int number_of_texcoords = 0;
unsigned short number_of_indices = 0;

GLuint vao_sphere;
GLuint vbo_position_sphere;
GLuint vbo_texcoord_sphere;
GLuint ibo_sphere;

bool init_sphere( void)
{
	//code
	sphereVertices = create_vec_2d_float();
	if (sphereVertices == nullptr)
	{
		fprintf(gpFile, "failed to allocate memory for sphereVertices\n");
		return false;
	}

	sphereTexcoords = create_vec_2d_float();
	if (sphereTexcoords == nullptr)
	{
		fprintf(gpFile, "failed to allocate memory for sphereTexcoords\n");
		return false;
	}

	sphereIndices = create_vec_2d_int();
	if (sphereIndices == nullptr)
	{
		fprintf(gpFile, "failed to allocate memory for sphereIndices\n");
		return false;
	}

	InitSphereVertices(0.8f, 30.0f, 30.0f, gpFile, &sphereVertices, &sphereNormals, &sphereTexcoords, &sphereIndices);
	number_of_vertices = GetNumberOfSphereVertices();
	number_of_indices = GetNumberOfSphereIndices();

	fprintf(gpFile, "vertices count : %d\n", number_of_vertices);
	fprintf(gpFile, "indices count : %d\n", number_of_indices);

	glGenVertexArrays(1, &vao_sphere);
	glBindVertexArray(vao_sphere);

	glGenBuffers(1, &vbo_position_sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_sphere);
	//glBufferData(GL_ARRAY_BUFFER, sphereVertices->size * sizeof(GLfloat), sphereVertices->p_arr, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, number_of_vertices * 3 * sizeof(GLfloat), sphereVertices->p_arr, GL_STATIC_DRAW);
	glVertexAttribPointer(ATB_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATB_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_texcoord_sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord_sphere);
	//glBufferData(GL_ARRAY_BUFFER, sphereTexcoords->size * sizeof(GLfloat), sphereTexcoords->p_arr, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, number_of_vertices * 2 * sizeof(GLfloat), sphereTexcoords->p_arr, GL_STATIC_DRAW);
	glVertexAttribPointer(ATB_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ATB_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ibo_sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_sphere);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices->size * sizeof(int), sphereIndices->p_arr, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, number_of_indices * sizeof(int), sphereIndices->p_arr, GL_STATIC_DRAW);

	// anyway we are setting this color from display function.
	glVertexAttrib3f(ATB_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

	glBindVertexArray(0);

	return true;
}

void display_sphere(void)
{
	glBindVertexArray(vao_sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_sphere);
	glDrawElements(GL_TRIANGLES, number_of_indices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void uninitialize_sphere(void)
{
	clean_vec_2d_float(sphereVertices);
	clean_vec_2d_float(sphereNormals);
	clean_vec_2d_float(sphereTexcoords);
	clean_vec_2d_int(sphereIndices);

	if (vao_sphere)
	{
		glDeleteVertexArrays(1,	&vao_sphere);
		vao_sphere = 0;
	}

	if (vbo_position_sphere)
	{
		glDeleteBuffers(1, &vbo_position_sphere);
		vbo_position_sphere = 0;
	}

	if (vbo_texcoord_sphere)
	{
		glDeleteBuffers(1, &vbo_texcoord_sphere);
		vbo_texcoord_sphere = 0;
	}

	if (ibo_sphere)
	{
		glDeleteBuffers(1, &ibo_sphere);
		ibo_sphere = 0;
	}
}