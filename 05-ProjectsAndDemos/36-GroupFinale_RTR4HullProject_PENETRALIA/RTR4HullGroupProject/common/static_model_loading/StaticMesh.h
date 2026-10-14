#pragma once

#include <vector>

#include "common.h"

extern GLuint shaderProgramObject_StaticModel;

struct Vertex
{
	vmath::vec3 position;
	vmath::vec3 normal;
	vmath::vec2 texcoord;
	vmath::vec3 tangent;
	//vmath::vec3 bitangent;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};


class Mesh
{
public:
	// mesh data
	std::vector<Vertex>			vertices;
	std::vector<unsigned int>	indices;
	std::vector<Texture>			textures;

	GLuint vao;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		setupMesh();
	}
	/*~Mesh()
	{
		for (int i = 0; i < textures.size(); i++)
		{
			if (textures[i].id)
			{
				glDeleteTextures(1, &textures[i].id);
			}
		}
	}*/
	void Draw(int instancedCount)
	{
		// we need to bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int aoNr = 1;
		unsigned int normalNr = 1;
		unsigned int roughnessNr = 1;
		unsigned int specularNr = 1;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			
			glActiveTexture(GL_TEXTURE0 + i);
			std::string number;
			std::string name = textures[i].type;
			TraceLog(NULL, "name type = %s\n", name.c_str());
			if (name == "texture_diffuse")
			{
				number = std::to_string(diffuseNr++);
			}
			else if (name == "texture_ao")
			{
				number = std::to_string(aoNr++);
			}
			else if (name == "texture_normal")
			{
				number = std::to_string(normalNr++);
			}
			else if (name == "texture_roughness")
			{
				number = std::to_string(roughnessNr++);
			}
			else if (name == "texture_metallic")
			{
				number = std::to_string(specularNr++);
			}
			//glUniform1i(glGetUniformLocation(gShaderProgramObject, (name + number).c_str()), i);
			TraceLog(NULL, "formed string = %s\n", name.c_str());
			glUniform1i(glGetUniformLocation(shaderProgramObject_StaticModel, (name).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		// draw mesh
		glBindVertexArray(vao);
		glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0, instancedCount);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}

private:

	unsigned int vbo, ebo;

	void setupMesh(void)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

		glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

		glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
		glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);

		glVertexAttribPointer(AMC_ATTRIBUTE_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
		glEnableVertexAttribArray(AMC_ATTRIBUTE_TANGENT);

		//glVertexAttribPointer(ATB_ATTRIBUTE_BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
		//glEnableVertexAttribArray(ATB_ATTRIBUTE_BITANGENT);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
	}
};
