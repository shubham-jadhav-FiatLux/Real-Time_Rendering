#include "terrian/TerrianLearnOGL.h"
#include "Texture.h"
#include "terrian/TerrianLearnOGLShader.h"

#define SPPED 0.1f
#define TERRAIN_TRANSLATE_Y -115.0f
#define TERRAIN_SCALE_X 0.25f
#define TERRAIN_SCALE_Y 0.25f
#define TERRAIN_SCALE_Z 0.25f
static GLuint displacementTexture;
static GLuint colorTexture;

static GLuint multitextureBackground, redTexture, greenTexture, blueTexture, iblendMap;

//GLfloat lightPosition_X = 0.0f, lightPosition_Y = 21.0f, lightPosition_Z = 5.0f;
static vec4 lightPosition = vmath::vec4(lightPosition_X, lightPosition_Y, lightPosition_Z, 1.0f);


struct DirLight dirLight_LearnOGL[2];
struct PointLight pointLight_LearnOGL;

int width = 8192, height = 8192;
unsigned rez = 80;


GLuint vao_TerrianLearnOGL;
GLuint vbo_TerrianLearnOGL;
#include "sb7ktx.h"

bool InitializeTerrianLearnOGL(void)
{

	displacementTexture = CreateTexture("resources/textures/terrian/Heightmap_Base_Variant_01_8192x8192.png");
	colorTexture = CreateTexture("resources/textures/terrian/Terrain_edited.jpeg");
	
	multitextureBackground = CreateTexture("resources/textures/multitexture/grassy2.png");
	redTexture = CreateTexture("resources/textures/multitexture/mud.png");
	greenTexture = CreateTexture("resources/textures/multitexture/pinkFlowers.png");
	blueTexture = CreateTexture("resources/textures/multitexture/path.png");
	iblendMap = CreateTexture("resources/textures/multitexture/blendMap.png");

	InitializeTerrianLearnOGLShader();

	std::vector<float> vertices;

	for (unsigned i = 0; i <= rez - 1; i++)
	{
		for (unsigned j = 0; j <= rez - 1; j++)
		{
			vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
			vertices.push_back(i / (float)rez); // u
			vertices.push_back(j / (float)rez); // v

			vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
			vertices.push_back((i + 1) / (float)rez); // u
			vertices.push_back(j / (float)rez); // v

			vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
			vertices.push_back(i / (float)rez); // u
			vertices.push_back((j + 1) / (float)rez); // v

			vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
			vertices.push_back((i + 1) / (float)rez); // u
			vertices.push_back((j + 1) / (float)rez); // v
		}
	}


	glGenVertexArrays(1, &vao_TerrianLearnOGL);
	glBindVertexArray(vao_TerrianLearnOGL);

	glGenBuffers(1, &vbo_TerrianLearnOGL);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_TerrianLearnOGL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	// texCoord attribute
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);

	glBindVertexArray(0);

	return true;
}

void DisplayTerrianLearnOGL(void)
{
	mat4 modelMatrix = vmath::mat4::identity();
	mat4 translateMatrix = vmath::mat4::identity();
	mat4 scaleMatrix = vmath::mat4::identity();
	translateMatrix = translate(0.0f, TERRAIN_TRANSLATE_Y, 0.0f);
	scaleMatrix = scale(TERRAIN_SCALE_X, TERRAIN_SCALE_Y, TERRAIN_SCALE_Z);
	modelMatrix = translateMatrix * scaleMatrix;

	// Filling directional lights data
	dirLight_LearnOGL[0].ambient = vmath::vec3(1.0, 1.0, 1.0 );
	dirLight_LearnOGL[0].diffuse = vmath::vec3(1.0, 1.0, 1.0);
	dirLight_LearnOGL[0].specular = vmath::vec3(0.0, 0.0, 1.0);
	dirLight_LearnOGL[0].direction = vmath::vec4(50.0, 50.0, 50.0, 1.0);

	dirLight_LearnOGL[1].ambient = vmath::vec3(1.0, 0.0, 0.0);
	dirLight_LearnOGL[1].diffuse = vmath::vec3(1.0, 0.0, 0.0);
	dirLight_LearnOGL[1].specular = vmath::vec3(1.0, 1.0, 1.0);
	dirLight_LearnOGL[1].direction = vmath::vec4(-50.0, 50.0, 50.0, 1.0);

	unsigned int lightCount = sizeof(dirLight_LearnOGL) / sizeof(dirLight_LearnOGL[0]);

	SetTerrianLearnOGLShader(modelMatrix, displacementTexture, colorTexture, lightPosition, multitextureBackground, redTexture, greenTexture, blueTexture, iblendMap, bLight, dirLight_LearnOGL, lightCount, cameraPosition);

	glBindVertexArray(vao_TerrianLearnOGL);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);
	UnsetTerrianLearnOGLShader();
}

void UninitializeTerrianLearnOGL(void)
{

}
void TestTerrianLearnOGLCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:
		switch (wParam)
		{
		case 'X':
			lightPosition_X += SPPED;
			lightPosition[0] = lightPosition_X;
			break;

		case 'x':
			lightPosition_X -= SPPED;
			lightPosition[0] = lightPosition_X;
			break;

		case 'Y':
			lightPosition_Y += SPPED;
			lightPosition[1] = lightPosition_Y;
			break;

		case 'y':
			lightPosition_Y -= SPPED;
			lightPosition[1] = lightPosition_Y;
			break;

		case 'Z':
			lightPosition_Z += SPPED;
			lightPosition[2] = lightPosition_Z;
			break;

		case 'z':
			lightPosition_Z -= SPPED;
			lightPosition[2] = lightPosition_Z;
			break;

		/*case 'k':
			quadratic -= 0.001f;
			if (quadratic <= 0.0f)
				quadratic = 0.0f;
			break;

		case 'K':
			quadratic += 0.001f;
			if (quadratic >= 1.0f)
				quadratic = 1.0f;

			break;

		case 'l':
			linear -= 0.001f;
			if (linear <= 0.0f)
				linear = 0.0f;
			break;

		case 'L':
			linear += 0.001f;
			if (linear >= 1.0f)
				linear = 1.0f;

			break;*/

		case 'P':
		case 'p':
			TraceLog(NULL, "light pos =[%f, %f, %f]\n", lightPosition[0], lightPosition[1], lightPosition[2]);
			break;

		default:
			break;
		}
		break; // break of WM_CHAR

	default:
		break;
	}
}
