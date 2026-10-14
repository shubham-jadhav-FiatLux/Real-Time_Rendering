#include "TestMSAA.h"
#include "msaa/MSAAShader.h"
#include "QuadRender.h"
#include "TextureShader.h"
#include "CubeRender.h"
#include "Texture.h"

static GLuint frameBuffer_MSAA = 0;
static GLuint intermediateFrameBuffer_MSAA = 0;
static GLuint screenTexture_MSAA = 0;
static GLuint image_texture = 0;
static unsigned int textureColorBufferMultiSampled = 0;
static unsigned rbo_MSAA = 0;
bool InitializeMSAATestScene()
{
	// code
	InitializeMSAAShader();
	
	image_texture = CreateTexture("resources/test.png");

	// configure MSAA FBO
	
	ResizeMSAATestScene(FBO_WIDTH, FBO_HEIGHT);
	return true;
}

bool ResizeMSAATestScene(int width, int height)
{
	if (rbo_MSAA)
	{
		glDeleteRenderbuffers(1, &rbo_MSAA);
		rbo_MSAA = 0;
	}
	if (textureColorBufferMultiSampled)
	{
		glDeleteTextures(1, &textureColorBufferMultiSampled);
		textureColorBufferMultiSampled = 0;
	}
	if (frameBuffer_MSAA)
	{
		glDeleteFramebuffers(1, &frameBuffer_MSAA);
		frameBuffer_MSAA = 0;
	}

	glGenFramebuffers(1, &frameBuffer_MSAA);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_MSAA);


	glGenTextures(1, &textureColorBufferMultiSampled);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);


	glGenRenderbuffers(1, &rbo_MSAA);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_MSAA);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_MSAA);
	//check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ErrorLog(NULL, "Error : framebuffer is not complete\\n");
		return (false);
	}

	// configure second post-processing
	glGenFramebuffers(1, &intermediateFrameBuffer_MSAA);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFrameBuffer_MSAA);
	glGenTextures(1, &screenTexture_MSAA);
	glBindTexture(GL_TEXTURE_2D, screenTexture_MSAA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture_MSAA, 0);

	//check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ErrorLog(NULL, "Error : framebuffer is not complete\\n");
		return (false);
	}
	return true;
}

void DisplayMSAATestScene(void)
{
	// code
	mat4 translationMatrix = mat4::identity();
	mat4 rotationXMatrix = mat4::identity();
	mat4 rotationYMatrix = mat4::identity();
	mat4 rotationZMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	rotationXMatrix = vmath::rotate(120.0f, 1.0f, 0.0f, 0.0f);
	rotationYMatrix = vmath::rotate(50.0f, 0.0f, 1.0f, 0.0f);
	rotationZMatrix = vmath::rotate(80.0f, 0.0f, 0.0f, 1.0f);
	rotationMatrix = rotationXMatrix * rotationYMatrix * rotationZMatrix;
	modelMatrix = translationMatrix * rotationMatrix;

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_MSAA);
	glViewport(0, 0, (GLsizei)FBO_WIDTH, (GLsizei)FBO_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SetTextureShader(modelMatrix, image_texture);
	RenderCube();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer_MSAA);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFrameBuffer_MSAA);
	glBlitFramebuffer(0, 0, FBO_WIDTH, FBO_HEIGHT, 0, 0, FBO_WIDTH, FBO_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	UnsetTextureShader();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);
	SetMSAAShader(mat4::identity(), screenTexture_MSAA);
	RenderQuad();
	UnsetMSAAShader();
}

void UninitializeMSAATestScene(void)
{
	// code
	DeleteTexture(image_texture);
	UninitializeMSAAShader();
}

