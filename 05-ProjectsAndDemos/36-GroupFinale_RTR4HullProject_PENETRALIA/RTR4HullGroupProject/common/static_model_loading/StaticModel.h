
#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "vmath.h"

#include "assimp/Importer.hpp"
#include "assimp/Scene.h"
#include "assimp/postprocess.h"

#include "common.h"
#include "StaticMesh.h"
#include "Loadtexture_stbi.h"

using namespace vmath;
using namespace std;

// function declaration/prototype
static bool LoadTextureFromFile(const char* filename, std::string directory, GLuint* texture);

class Model
{

public:

	// model data
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;

	Model()
	{

	}

	Model(std::string const& path)
	{
		TraceLog(NULL, "Entry of %s \n", __FUNCTION__);
		
		loadModel(path);

		TraceLog(NULL, "End of %s \n", __FUNCTION__);
	}

	void Draw(int instancedCount)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(instancedCount);
		}
	}

private:
	void loadModel(std::string const& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			ErrorLog(NULL, "ERROR::Assimp = %s \n", importer.GetErrorString());
			return;
		}

		this->directory = path.substr(0, path.find_last_of('\\'));
		//this->directory = path.substr(0, path.find_last_of('/'));

		TraceLog(NULL, "\n\ndirectory = %s\n\n", this->directory.c_str());
		this->ProcessNode(scene->mRootNode, scene);
	}

	void ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (GLuint i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			this->meshes.push_back(this->processMesh(mesh, scene));
		}

		for (GLuint i = 0; i < node->mNumChildren; i++)
		{
			this->ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vmath::vec3 vector;

			// position
			vector[0] = mesh->mVertices[i].x;
			vector[1] = mesh->mVertices[i].y;
			vector[2] = mesh->mVertices[i].z;
			vertex.position = vector;

			// normals
			if (mesh->HasNormals())
			{
				vector[0] = mesh->mNormals[i].x;
				vector[1] = mesh->mNormals[i].y;
				vector[2] = mesh->mNormals[i].z;
				vertex.normal = vector;
			}

			// texture coordinates
			if (mesh->mTextureCoords[0])
			{
				vmath::vec2 tex_vec;

				tex_vec[0] = mesh->mTextureCoords[0][i].x;
				tex_vec[1] = mesh->mTextureCoords[0][i].y;
				vertex.texcoord = tex_vec;

				
			}
			else
			{
				vertex.texcoord = vec2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}

		// mesh faces
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		int index0, index1, index2;
		// calculate tangents and bitangents
		for (int i = 0; i < indices.size(); i = i + 3)
		{
			index0 = indices[i];
			index1 = indices[i + 1];
			index2 = indices[i + 2];

			vec3 v0 = vertices[index0].position;
			vec3 v1 = vertices[index1].position;
			vec3 v2 = vertices[index2].position;

			vec2 uv0 = vertices[index0].texcoord;
			vec2 uv1 = vertices[index1].texcoord;
			vec2 uv2 = vertices[index2].texcoord;

			vec3 edge1 = v1 - v0;
			vec3 edge2 = v2 - v0;

			vec2 deltaUV1 = uv1 - uv0;
			vec2 deltaUV2 = uv2 - uv0;

			float f = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV1[1] * deltaUV2[0]);

			vec3 tangent1, bitangent1;
			tangent1[0] = f * (deltaUV2[1] * edge1[0] - deltaUV1[1] * edge2[0]);
			tangent1[1] = f * (deltaUV2[1] * edge1[1] - deltaUV1[1] * edge2[1]);
			tangent1[2] = f * (deltaUV2[1] * edge1[2] - deltaUV1[1] * edge2[2]);

			/*bitangent1[0] = f * (-deltaUV2[0] * edge1[0] + deltaUV1[0] * edge2[0]);
			bitangent1[1] = f * (-deltaUV2[0] * edge1[1] + deltaUV1[0] * edge2[1]);
			bitangent1[2] = f * (-deltaUV2[0] * edge1[2] + deltaUV1[0] * edge2[2]);*/


			// tangent
			vertices[index0].tangent += tangent1;
			vertices[index1].tangent += tangent1;
			vertices[index2].tangent += tangent1;

			/*vertices[index0].bitangent += bitangent1;
			vertices[index1].bitangent += bitangent1;
			vertices[index2].bitangent += bitangent1;*/

			//vector[0] = mesh->mTangents[i].x;
			//vector[1] = mesh->mTangents[i].y;
			//vector[2] = mesh->mTangents[i].z;
			//vertex.tangent = vector;

			//// bitangent
			//vector[0] = mesh->mBitangents[i].x;
			//vector[1] = mesh->mBitangents[i].y;
			//vector[2] = mesh->mBitangents[i].z;
			//vertex.bitangent = vector;

		}

		for (int i = 0; i < vertices.size(); i ++)
		{
			vertices[i].tangent = vmath::normalize(vertices[i].tangent);
			//vertices[i].bitangent = vmath::normalize(vertices[i].bitangent);
		}

		// materials
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			// for obj model
			vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			vector<Texture> heightMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal"); // bump = height 
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			vector<Texture> heightMaps1 = this->loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal"); // bump = height 
			textures.insert(textures.end(), heightMaps1.begin(), heightMaps1.end());

			vector<Texture> roughnessMaps = this->loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_roughness"); // emissive = roughness 
			textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

			vector<Texture> aoMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_ao"); // specular = metallic for PBR
			textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());

			vector<Texture> metallicMaps = this->loadMaterialTextures(material, aiTextureType_METALNESS, "texture_metallic"); // ambient = ambient occulsion from .mtl file
			textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
			

			/*vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular"); // specular = ao texture for PBR
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal"); // bump = height 
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			vector<Texture> disMaps = this->loadMaterialTextures(material, aiTextureType_DISPLACEMENT, "texture_normal"); // bump = height 
			textures.insert(textures.end(), disMaps.begin(), disMaps.end());

			vector<Texture> heightMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height"); // bump = height 
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			vector<Texture> aoMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ao"); // ambient = ambient occulsion from .mtl file
			textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());

			vector<Texture> metallicMaps = this->loadMaterialTextures(material, aiTextureType_METALNESS, "texture_meatllic"); // ambient = ambient occulsion from .mtl file
			textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());

			vector<Texture> RoughnessMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness"); // emissive = roughness 
			textures.insert(textures.end(), RoughnessMaps.begin(), RoughnessMaps.end());

			vector<Texture> sheenMaps = this->loadMaterialTextures(material, aiTextureType_SHEEN, "texture_roughness"); // emissive = roughness 
			textures.insert(textures.end(), sheenMaps.begin(), sheenMaps.end());

			vector<Texture> clearcoatMaps = this->loadMaterialTextures(material, aiTextureType_CLEARCOAT, "texture_normal"); // emissive = roughness 
			textures.insert(textures.end(), RoughnessMaps.begin(), RoughnessMaps.end());

			vector<Texture> transmissionMaps = this->loadMaterialTextures(material, aiTextureType_TRANSMISSION, "texture_normal"); // emissive = roughness 
			textures.insert(textures.end(), RoughnessMaps.begin(), RoughnessMaps.end());

			vector<Texture> unknownMaps = this->loadMaterialTextures(material, aiTextureType_UNKNOWN, "texture_roughness"); // emissive = roughness 
			textures.insert(textures.end(), RoughnessMaps.begin(), RoughnessMaps.end());*/

		}
		/*for (int i = 0; i < textures.size(); i++)
		{
			fprintf(gpFile, "type string = %s\n", textures[i].type.c_str());
			fprintf(gpFile, "path string = %s\n", textures[i].path.c_str());
		}*/
		return Mesh(vertices, indices, textures);

	} // EOF processMesh()

	// load materials
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType typeName, string tex_typename)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(typeName); i++)
		{
			aiString str;
			mat->GetTexture(typeName, i, &str);

			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				Texture texture;
				bool result = LoadTextureFromFile(str.C_Str(), this->directory, &texture.id);
				if (!result)
				{
					MessageBox(NULL, NULL, NULL, MB_OK);
				}
				texture.type = tex_typename;
				texture.path = str.C_Str();
				textures.push_back(texture);

				this->textures_loaded.push_back(texture);
			}
		}

		return textures;
	}

};



// load texure using stb_image
bool LoadTextureFromFile(const char* filename, std::string directory, GLuint* texture)
{
	//Generate texture ID and load texture data
	// variable declarations
	bool bResult = true;

	TraceLog(NULL, "\n\nfile directory : %s\n", directory.c_str());

	std::string texture_filename = std::string(filename);
	TraceLog(NULL, "\n\nfile name : %s\n", filename);
	texture_filename = directory + "\\" + filename;
	//texture_filename = directory + "/" + filename;

	// load image, create texture and generate mipmaps
	int width = 0, height = 0, nrChannels = 0;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	//stbi_set_flip_vertically_on_load(false);

	TraceLog(NULL, "\n\ntexture file path : %s\n", texture_filename.c_str());

	if (!loadTextureFromFile(texture_filename.c_str(), texture))
	{
		ErrorLog(NULL, "Failed to load tecture : %s\n", texture_filename.c_str());
		bResult = false;
	}

	return bResult;
}