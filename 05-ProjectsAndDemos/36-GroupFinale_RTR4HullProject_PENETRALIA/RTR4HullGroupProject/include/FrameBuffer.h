#pragma once
#include "Common.h"


bool createFrameBufferObject(int fboWidth, int fboHeight, GLuint& fbo, GLuint& fboTexture, GLuint& rbo);
void setFBO(GLuint);
void unsetFBO();
//bool createFrameBufferObject(int fboWidth, int fboHeight, GLuint* fbo, GLuint* fboTexture, GLuint* rbo);