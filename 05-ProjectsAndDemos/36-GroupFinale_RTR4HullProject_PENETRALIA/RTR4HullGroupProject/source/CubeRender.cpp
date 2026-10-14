#include "CubeRender.h"

//cube
static GLuint vao_cube;
static GLuint vbo_cube_position;
static GLuint vbo_cube_texcoord;
static GLuint vbo_cube_normal;

bool InitializeCubeRenderer(void)
{
	const GLfloat cubePosition[] = {
		//top
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		// bottom
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		// front
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		// back
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		// right
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		// left
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f, };

	const GLfloat cubeTexcoords[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,

			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,

			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,

			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,

			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,

			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f, };

	const GLfloat cubeNormals[] = {
		// top surface
		0.0f, 1.0f, 0.0f, // top-right of top
		0.0f, 1.0f, 0.0f, // top-left of top
		0.0f, 1.0f, 0.0f, // bottom-left of top
		0.0f, 1.0f, 0.0f, // bottom-right of top
		
		// bottom surface
		0.0f, -1.0f, 0.0f, // top-right of bottom
		0.0f, -1.0f, 0.0f, // top-left of bottom
		0.0f, -1.0f, 0.0f, // bottom-left of bottom
		0.0f, -1.0f, 0.0f, // bottom-right of bottom

		// front surface
		0.0f, 0.0f, 1.0f, // top-right of front
		0.0f, 0.0f, 1.0f, // top-left of front
		0.0f, 0.0f, 1.0f, // bottom-left of front
		0.0f, 0.0f, 1.0f, // bottom-right of front

		// back surface
		0.0f, 0.0f, -1.0f, // top-right of back
		0.0f, 0.0f, -1.0f, // top-left of back
		0.0f, 0.0f, -1.0f, // bottom-left of back
		0.0f, 0.0f, -1.0f, // bottom-right of back

		// right surface
		1.0f, 0.0f, 0.0f, // top-right of right
		1.0f, 0.0f, 0.0f, // top-left of right
		1.0f, 0.0f, 0.0f, // bottom-left of right
		1.0f, 0.0f, 0.0f, // bottom-right of right

		// left surface
		-1.0f, 0.0f, 0.0f, // top-right of left
		-1.0f, 0.0f, 0.0f, // top-left of left
		-1.0f, 0.0f, 0.0f, // bottom-left of left
		-1.0f, 0.0f, 0.0f, // bottom-right of left
	};

	
	//vao cube
	glGenVertexArrays(1, &vao_cube);
	glBindVertexArray(vao_cube);
	
	//position
	glGenBuffers(1, &vbo_cube_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubePosition), cubePosition, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal
	glGenBuffers(1, &vbo_cube_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//texture
	glGenBuffers(1, &vbo_cube_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoords), cubeTexcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return true;
}

void RenderCube(void)
{
	//cube
	glBindVertexArray(vao_cube);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);
}

void UninitializeCubeRenderer(void)
{
	if (vbo_cube_position)
	{
		glDeleteBuffers(1, &vbo_cube_position);
		vbo_cube_position = 0;
	}
	if (vbo_cube_texcoord)
	{
		glDeleteBuffers(1, &vbo_cube_texcoord);
		vbo_cube_texcoord = 0;
	}
	if (vbo_cube_normal)
	{
		glDeleteBuffers(1, &vbo_cube_normal);
		vbo_cube_normal = 0;
	}
	
	if (vao_cube)
	{
		glDeleteBuffers(1, &vao_cube);
		vao_cube = 0;
	}
}