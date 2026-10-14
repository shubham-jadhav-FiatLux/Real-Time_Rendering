#include "FrameBuffer.h"
// NOTE : need to check if there is RBO is needed. if we faceany performance issue [FPS drop].

bool createFrameBufferObject(int fboWidth, int fboHeight, GLuint& fbo, GLuint& fboTexture, GLuint& rbo)
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //create a color attachment texture
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fboWidth, fboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

    //create depth and stencil attachment
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    //check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        ErrorLog(NULL, "Error : framebuffer is not complete\\n");
        return (false);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;    
}

void setFBO(GLuint fbo, int fboWidth, int fboHeight)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // this is framebuffer clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, GLsizei(fboWidth), GLsizei(fboHeight));

}

void unsetFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setting default viewport size
    glViewport(0, 0, GLsizei(screenWidth), GLsizei(screenHeight));
}

/*
bool createFrameBufferObject(int fboWidth, int fboHeight, GLuint& fbo, GLuint* fboTexture, GLuint* rbo)
{
    glGenFramebuffers(1, fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
    //create a color attachment texture
    glGenTextures(1, fboTexture);
    glBindTexture(GL_TEXTURE_2D, *fboTexture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fboWidth, fboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *fboTexture, 0);

    //create depth and stencil attachment
    glGenRenderbuffers(1, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);

    //check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        ErrorLog(NULL, "Error : framebuffer is not complete\\n");
        return (false);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}
*/