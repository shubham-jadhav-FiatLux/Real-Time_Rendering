#include "water/Water.h"
#include "water/WaterShader.h"
#include "Texture.h"
#include "TextureShader.h"
#include "Camera.h"
#include "CubeRender.h"

GLuint reflectionframeBuffer;
GLuint reflectiontextureColorBuffer;
GLuint reflectionrbo;

GLuint refractionframeBuffer;
GLuint refractionColorTexture;
GLuint refractionDepthTexture;

GLuint dudvMap_texture;
GLuint water_texture;
GLuint waterNormal_texture;

GLuint vao_water_quad;
GLuint vbo_position_water_quad;

extern float pitch;

bool InitializeWater(void)
{
    // code
    water_texture = CreateTexture("resources/test.png");
    dudvMap_texture = CreateTexture("resources/water/dudvMap.bmp");
    waterNormal_texture = CreateTexture("resources/water/matchingNormalMap.png");

    // initalize water shader
    InitializeWaterShader();

    //water quad
    const GLfloat planeVertices[] =
    {
        1.0f, 0.0f, -1.0f,
        -1.0f, 0.0f,-1.0f,
        -1.0f, 0.0f,1.0f,
        1.0f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &vao_water_quad);
    glBindVertexArray(vao_water_quad);
    glGenBuffers(1, &vbo_position_water_quad);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_water_quad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //water refraction fbo
    glGenFramebuffers(1, &refractionframeBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, refractionframeBuffer);
    glGenTextures(1, &refractionColorTexture);
    glBindTexture(GL_TEXTURE_2D, refractionColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionColorTexture, 0);

    //depth attchment texture
    glGenTextures(1, &refractionDepthTexture);
    glBindTexture(GL_TEXTURE_2D, refractionDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, FBO_WIDTH, FBO_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, refractionDepthTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        ErrorLog(NULL, "glCheckFramebufferStatus() is failed for reflection buffer.\n");
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //water reflection fbo
    glGenFramebuffers(1, &reflectionframeBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionframeBuffer);
    glGenTextures(1, &reflectiontextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, reflectiontextureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectiontextureColorBuffer, 0);

    glGenRenderbuffers(1, &reflectionrbo);
    glBindRenderbuffer(GL_RENDERBUFFER, reflectionrbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, FBO_WIDTH, FBO_HEIGHT);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, reflectionrbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        ErrorLog(NULL, "glCheckFramebufferStatus() is failed for reflection buffer.\n");
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void DisplayWater(void)
{
	// variable declarations
	mat4 modelMatrix = mat4::identity();
	mat4 translateMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();
    static GLfloat moveFactor = 0.0f;

    moveFactor = moveFactor + 0.001f;
    if (moveFactor >= 1.0f)
        moveFactor = 0.0f;

	// code

    mat4 tempRefractionViewMatrix;
    float previous_pitch = pitch;
    //pitch *= -1.0f;
    //UpdateCamera();
    //tempRefractionViewMatrix = GetViewMatrix();

   
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionframeBuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0, (GLsizei)FBO_WIDTH, (GLsizei)FBO_HEIGHT);

    modelMatrix = vmath::translate(0.0f, 0.5f, -6.0f);
    SetTextureShader(modelMatrix, water_texture);
    RenderCube();
    UnsetTextureShader();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //pitch = previous_pitch;
    //UpdateCamera();

    // water shader
    //glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    /*modelMatrix = vmath::translate(0.0f, 1.5f, -6.0f);
    SetTextureShader(modelMatrix, viewMatrix, projectionMatrix, water_texture);
    RenderCube();
    UnsetTextureShader();*/

    translateMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
    scaleMatrix = vmath::scale(100.0f, 0.0f, 100.0f);

    modelMatrix = translateMatrix * scaleMatrix;

	SetWaterShader(modelMatrix, reflectiontextureColorBuffer, refractionColorTexture, dudvMap_texture, waterNormal_texture, refractionDepthTexture, moveFactor ,cameraPosition, vec3(0.0f, 0.0f, -5.0f));

    glBindVertexArray(vao_water_quad);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    UnsetWaterShader();
}

void UninitializeWater(void)
{
    UninitializeWaterShader();
}