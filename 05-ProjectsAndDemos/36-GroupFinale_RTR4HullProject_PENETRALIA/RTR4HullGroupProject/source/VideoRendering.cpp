// header files

#include "VideoRendering.h"
#include "TextureShader.h"
#include "QuadRender.h"
#include "Texture.h"

extern "C"
{
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
#include <inttypes.h>
}

#include "VideoReader.h"

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")

// variable for texture loading
GLuint textImage_VideoRendering;

// shader variables
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

// uniform
GLuint mvpUniform_VideoRendering;
GLuint textureSamplerUniform_VideoRendering;

VideoReaderState vr_state;
int frame_width;
int frame_height;
uint8_t* frame_data = nullptr;
// enum
enum
{
	ATB_ATTRIBUTE_POSITION = 0,
	ATB_ATTRIBUTE_COLOR,
	ATB_ATTRIBUTE_NORMAL,
	ATB_ATTRIBUTE_TEXCOORD
};

bool InitializeVideoRendering(void)
{
	if (!video_reader_open_file(&vr_state, "resources/amc_banner/AMCBanner-V2_60fps_3.mp4"))
	{
		ErrorLog(NULL, "video_reader_open_file() is failed couldn't open video file\n");
		return false;
	}
	else
	{
		TraceLog(NULL, "video_reader_open_file() is successful\n");
	}

	frame_width = vr_state.width;
	frame_height = vr_state.height;
	frame_data = new uint8_t[frame_width * frame_height * 4];

	glGenTextures(1, &textImage_VideoRendering);
	glBindTexture(GL_TEXTURE_2D, textImage_VideoRendering);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	return true;
}

void DisplayVideoRendering(void)
{
	// code
	int64_t pts;
	if (!video_reader_read_frame(&vr_state, frame_data, &pts))
	{
		ErrorLog(NULL, "video_reader_read_frame() is failed couldn't load frame\n");
		return;
	}

	float time = 0.0f;
	while (time <= 60.0f)
	{
		time = time + 0.00001f;
		if (time >= 60.0f)
		{
			break;
		}
	}

	glBindTexture(GL_TEXTURE_2D, textImage_VideoRendering);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	mat4 modelMatrix = rotate(180.0f, 1.0f, 0.0f, 0.0f);

	SetTextureShader(modelMatrix,textImage_VideoRendering);
	RenderQuad();
	UnsetTextureShader();

}

bool UpdateVideoRendering(void)
{
	return true;
}

void UninitializeVideoRendering(void)
{
	DeleteTexture(textImage_VideoRendering);
}
