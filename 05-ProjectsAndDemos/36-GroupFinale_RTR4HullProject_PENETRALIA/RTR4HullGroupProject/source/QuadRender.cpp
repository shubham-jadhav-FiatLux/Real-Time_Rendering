#include "QuadRender.h"

//Quad
static GLuint vao_quad;
static GLuint vbo_quad_position;
static GLuint vbo_quad_texcoord;
static GLuint vbo_quad_normal;

bool InitializeQuadRenderer(void)
{
	const GLfloat QuadPosition[] = {
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		 };

	const GLfloat QuadTexcoords[] = {
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f
			};

	const GLfloat QuadNormals[] = {

		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f	
	};

	
	//vao Quad
	glGenVertexArrays(1, &vao_quad);
	glBindVertexArray(vao_quad);
	
	//position
	glGenBuffers(1, &vbo_quad_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadPosition), QuadPosition, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal
	glGenBuffers(1, &vbo_quad_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadNormals), QuadNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//texture
	glGenBuffers(1, &vbo_quad_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_texcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadTexcoords), QuadTexcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return true;
}

void RenderQuad(void)
{
	//Quad
	glBindVertexArray(vao_quad);
	glDrawArrays(GL_QUADS, 0, 4);
	glBindVertexArray(0);
}

void UninitializeQuadRenderer(void)
{
	if (vbo_quad_position)
	{
		glDeleteBuffers(1, &vbo_quad_position);
		vbo_quad_position = 0;
	}
	if (vbo_quad_texcoord)
	{
		glDeleteBuffers(1, &vbo_quad_texcoord);
		vbo_quad_texcoord = 0;
	}
	if (vbo_quad_normal)
	{
		glDeleteBuffers(1, &vbo_quad_normal);
		vbo_quad_normal = 0;
	}
	
	if (vao_quad)
	{
		glDeleteBuffers(1, &vao_quad);
		vao_quad = 0;
	}
}