#pragma once
#include <map>
#include "Common.h"
#include "ft2build.h"
#include FT_FREETYPE_H


bool InitializeFont();
bool InitializeMultipleFont();
void DisplayFont(void);
void UninitializeFont(void);
bool Initalize_FreeType(std::string font_name, std::map<GLchar, Character>&);
void RenderText(std::string text, float x, float y, float scale, vec3 color, std::map<GLchar, Character>&);
void SetFontShader(mat4);
void UnsetFontShader(void);
