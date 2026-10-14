#include "FontShader.h"

static GLuint shaderProgramObject_Font;
GLuint modelMatrixUniform_Font;
GLuint viewMatrixUniform_Font;
GLuint projectionMatrixUniform_Font;
GLuint textureColorUniform_Font;
GLuint textureUniform_Font;

GLuint vao_font;
GLuint vbo_position_font;

std::map<GLchar, Character> characters_font1;
std::map<GLchar, Character> characters_font2;

bool InitializeFont()
{
	// code

	SHADERSHADERTYPEKEYPAIR shadersToCreate[] = {
		GL_VERTEX_SHADER, "shader/font/FontShader.vert"
		, GL_FRAGMENT_SHADER, "shader/font/FontShader.frag"
	};

	GLuint shaders[2];
	unsigned int createdShadersCount = 0;

	if (!CreateShaders(
		shadersToCreate
		, sizeof(shadersToCreate) / sizeof(shadersToCreate[0])
		, shaders
		, &createdShadersCount
	))
	{
		ErrorLog(NULL, "InitializeTerrianSuperBibleShader()::CreateShaders() failed.");
		// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
		/*if (shaders)
		{
			free(shaders);
			shaders = nullptr;
		}*/
		return(false);
	}

	shaderProgramObject_Font = CreateShaderProgram();

	AttachShadersToShaderProgram(
		shaderProgramObject_Font
		, shaders
		, createdShadersCount
	);

	// TODO: Create DeleteShaders() function instead of explicit memory deallocation.
	/*free(shaders);
	shaders = nullptr;*/

	ATTRIBTYPEATTRIBNAMEKEYPAIR attributesToBind[] = {
		AMC_ATTRIBUTE_POSITION, "a_position"
		//, AMC_ATTRIBUTE_TEXTURE0, "a_texcoord"
	};

	if (!BindShaderProgramAttribLocations(
		shaderProgramObject_Font
		, attributesToBind
		, sizeof(attributesToBind) / sizeof(attributesToBind[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	if (!LinkShaderProgram(shaderProgramObject_Font))
	{
		// TODO: Log error here.
		return(false);
	}

	UNIFORMUNIFORMNAMEKEYPAIR uniformsToBindLocation[] = {
		&modelMatrixUniform_Font, "u_modelMatrix"
		, &viewMatrixUniform_Font, "u_viewMatrix"
		, &projectionMatrixUniform_Font, "u_projectionMatrix"

		, &textureColorUniform_Font , "textColor"
		, &textureUniform_Font, "text"
	};

	if (!BindShaderProgramUniformLocations(
		shaderProgramObject_Font
		, uniformsToBindLocation
		, sizeof(uniformsToBindLocation) / sizeof(uniformsToBindLocation[0])
	))
	{
		// TODO: Log error here.
		return(false);
	}

	glGenVertexArrays(1, &vao_font);
	glBindVertexArray(vao_font);

	// follwing six statements to push position data to shader
	glGenBuffers(1, &vbo_position_font);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_font);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	if (!InitializeMultipleFont())
	{
		ErrorLog(NULL, "InitializeMultipleFont() is failed.", __FUNCTION__);
		return false;
	}

	return true;
}

bool InitializeMultipleFont()
{
	// code
	if (!Initalize_FreeType("resources/font/Antonio-Bold.ttf", characters_font1))
	{
		return false;
	}
	if(!Initalize_FreeType("resources/font/AMS Chhatrapati Regular.ttf", characters_font2))
	{
		return false;
	}

	return true;
}

void DisplayFont(void)
{

}

void UninitializeFont(void)
{
	// code
	ReleaseShaderProgramObject(shaderProgramObject_Font);
}

void SetFontShader(mat4 modelMatrix)
{
	// code
	glUseProgram(shaderProgramObject_Font);

	glUniformMatrix4fv(modelMatrixUniform_Font, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_Font, 1, GL_FALSE, mat4::identity());
	glUniformMatrix4fv(projectionMatrixUniform_Font, 1, GL_FALSE, mat4::identity());
}

void UnsetFontShader(void)
{
	// code
	glUseProgram(0);
}

// init freetype
bool Initalize_FreeType(std::string font_name, std::map<GLchar, Character> &characters)
{
	TraceLog(NULL, "%s started.", __FUNCTION__);

	// variable declarations
	FT_Library ft;


	// code

	// All functions non zero value when error comes 
	if (FT_Init_FreeType(&ft))
	{
		ErrorLog(NULL, "Failed to initialize freetype library.");
		return false;
	}

	// load font as face;

	//std::string font_name = "Antonio-Bold.ttf";
	FT_Face face;

	if (FT_New_Face(ft, font_name.c_str(), 0, &face))
	{
		ErrorLog(NULL, "Failed to load font: %s.", font_name.c_str());
		return false;
	}
	else
	{
		FT_Set_Pixel_Sizes(face, 0, 40);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// load first 128 ASCII characters set
	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			ErrorLog(NULL, "Failed to load glyph for character: %c.", c);
			continue;
		}

		// generate texture for loaded character
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// now store character for future use
		Character current_ch = {
		texture,
		vec2((float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows),
		vec2((float)face->glyph->bitmap_left, (float)face->glyph->bitmap_top),
		static_cast<unsigned int>(face->glyph->advance.x)
		};

		// now insert into map
		characters.insert(std::pair<char, Character>(c, current_ch));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	TraceLog(NULL, "%s end.", __FUNCTION__);
	return true;
}

// render text
void RenderText(std::string text, float x, float y, float scale, vec3 color, std::map<GLchar, Character> &characters)
{
	// code
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// set color to uniform
	glUniform3fv(textureColorUniform_Font, 1, color);

	glBindVertexArray(vao_font);

	// now iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = characters[*c];

		float xpos = x + ch.bearing[0] * scale;
		float ypos = y - (ch.size[1] - ch.bearing[1]) * scale;

		float w = ch.size[0] * scale;
		float h = ch.size[1] * scale;

		float vertices[6][4] = {
			{xpos, ypos + h, 0.0f, 0.0f },
			{xpos, ypos,	0.0f, 1.0f},
			{xpos + w, ypos, 1.0f, 1.0f},

			{xpos, ypos + h, 0.0f, 0.0f},
			{xpos + w, ypos, 1.0f, 1.0f},
			{xpos + w, ypos + h, 1.0f, 0.0f}
		};

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		glUniform1i(textureUniform_Font, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_position_font);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.advanced >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);
}
