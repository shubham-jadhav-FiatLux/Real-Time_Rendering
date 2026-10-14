#include "grass/grass.h"
#include "grass/grassShader.h"
#include "Camera.h"
#include <Texture.h>

static GLuint grass_texture;
static GLuint flowmap_texture;

static GLuint vao_grass;
static GLuint vbo_grass;
static GLuint grassVerticesSize;

bool InitializeGrass(void)
{
    // code
    grass_texture = CreateTexture("resources/grass/grass.png");
    flowmap_texture = CreateTexture("resources/grass/flowmap.png");

    InitializeGrassShader();

    GLfloat Vertices[40 * 40 * 3];
    int i = 0;
    for (float x = -2.0f; x < 2.0f; x += 0.1f)
    {
        for (float z = -2.0f; z < 2.0f; z += 0.1f)
        {
            int randNumberX = rand() % 10 + 1;
            int randNumberZ = rand() % 10 + 1;
            Vertices[i++] = x + (float)randNumberX / 50.0f;
            Vertices[i++] = 0;
            Vertices[i++] = z + (float)randNumberZ / 50.0f;
        }       
    }
    grassVerticesSize = sizeof(Vertices) / sizeof(Vertices[0]);
   
    glGenVertexArrays(1, &vao_grass);
    glBindVertexArray(vao_grass);
    glGenBuffers(1, &vbo_grass);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_grass);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void DisplayGrass(void)
{
   
	// variable declarations
    mat4 translationMatrix = mat4::identity();
    mat4 scaleMatrix = mat4::identity();
    mat4 modelMatrix = mat4::identity();
    mat4 viewMatrix = mat4::identity();
  
    viewMatrix = vmath::lookat(cameraPosition, cameraPosition + cameraFront, cameraUp);
  
    translationMatrix = vmath::translate(0.0f, 0.0f, 0.0f); // glTranslate is replaced by this line
    modelMatrix = translationMatrix * scaleMatrix;
    glPointSize(5.0f);
     
    /*SetTextureShader(modelMatrix, grass_texture);
    SetTextureShader(modelMatrix, flowmap_texture);
    UnsetTextureShader();*/

	SetGrassShader(modelMatrix, grass_texture, flowmap_texture,cameraPosition, vec3(0.0f, 0.0f, -5.0f));


    glBindVertexArray(vao_grass);
    glDrawArrays(GL_POINTS, 0, grassVerticesSize);
    glBindVertexArray(0);

    UnsetGrassShader();
}

void UnInitializeGrass(void)
{
    if (vbo_grass)
    {
        glDeleteBuffers(1, &vbo_grass);
        vbo_grass = 0;
    }

    if (vao_grass)
    {
        glDeleteVertexArrays(1, &vao_grass);
        vao_grass = 0;
    }

    DeleteTexture(flowmap_texture);

    DeleteTexture(grass_texture);

    /*if (flowmap_texture)
    {
        glDeleteTextures(1, &flowmap_texture);
        flowmap_texture = 0;
    }

    if (grass_texture)
    {
        glDeleteTextures(1, &grass_texture);
        grass_texture = 0;
    }*/

    UnInitializeGrassShader();
}