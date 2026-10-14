#include "Texture.h"
#include "Loadtexture_stbi.h"

GLuint CreateTexture(const char* filePath)
{
	GLuint imageTexture = 0;
	if (loadTextureFromFile(filePath, &imageTexture) == false)
	{
		ErrorLog(NULL, "failed to load texture");
		exit(1);
	}
	if (imageTexture == 0)
	{
		ErrorLog(NULL, "imageTexture is null");
		exit(1);
	}
	return imageTexture;
}

void DeleteTexture(GLuint& imageTexture)
{
	if (imageTexture)
	{
		glDeleteTextures(1, &imageTexture);
		imageTexture = 0;
	}
}