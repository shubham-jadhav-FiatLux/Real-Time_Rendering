#include "loadtexture_stbi.h"

#define STB_IMAGE_IMPLEMENTATION 1
#include "stb_image.h"

// load texure using stb_image
bool loadTextureFromFile(const char* filename, GLuint* texture)
{
	//Generate texture ID and load texture data
	// variable declarations
	bool bResult = false;
	HBITMAP hBitmap = NULL;

	std::string texture_filename = std::string(filename);
	DebugLog(NULL, "Texture file name: %s", filename);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(false);

	DebugLog(NULL, "Texture file path: %s", texture_filename.c_str());

	unsigned char* data = stbi_load(texture_filename.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		bResult = true;
		// Now from here onwards, OpenGL texture code starts
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		// setting of the texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	}
	else
	{
		ErrorLog(NULL, "Failed to load texture : %s\n", texture_filename.c_str());
		MessageBox(NULL, TEXT("Texture load failed"), TEXT("failed to load texture"), MB_OK);
	}
	stbi_image_free(data);
	return bResult;
}