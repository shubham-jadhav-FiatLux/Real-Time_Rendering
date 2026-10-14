#pragma once
extern "C"
{
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
#include <inttypes.h>
}

struct VideoReaderState
{
	int width, height;
	AVRational time_base;

	AVFormatContext* av_format_context;
	AVCodecContext* av_codec_context;
	int video_stream_index;
	AVFrame* av_frame;
	AVPacket* av_packet;
	SwsContext* sws_scaler_context;
};

bool video_reader_open_file(VideoReaderState *state, const char*filename)
{
	// code
	auto& width = state->width;
	auto&height = state->height;
	auto& time_base = state->time_base;
	auto& av_format_context = state->av_format_context;
	auto& av_codec_context = state->av_codec_context;
	auto& video_stream_index = state->video_stream_index;
	auto& av_frame = state->av_frame;
	auto& av_packet = state->av_packet;
	auto& sws_scaler_context = state->sws_scaler_context;
	

	// open file using libavformat
	av_format_context = avformat_alloc_context();

	// code
	if (!av_format_context)
	{
		ErrorLog(NULL, "Couldn't create AVFormatContext\n");
		return false;
	}

	int ret_value = avformat_open_input(&av_format_context, filename, NULL, NULL);
	TraceLog(NULL, "avformat_open_input return value : %d\n", ret_value);

	if (ret_value != 0)
	{
		ErrorLog(NULL, "Couldn't open video file\n");
		return false;
	}
	else
	{
		TraceLog(NULL, "avformat_open_input is successful\n");
	}

	video_stream_index = -1;
	AVCodecParameters* av_codec_params = nullptr;
	AVCodec* av_codec = nullptr;
	for (unsigned int i = 0; i < av_format_context->nb_streams; ++i)
	{
		av_codec_params = av_format_context->streams[i]->codecpar;
		av_codec = (AVCodec*)avcodec_find_decoder(av_codec_params->codec_id);
		time_base = av_format_context->streams[i]->time_base;
		if (!av_codec)
		{
			continue;
		}

		if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_stream_index = i;
			width = av_codec_params->width;
			height = av_codec_params->height;
			break;
		}
	}
	if (video_stream_index == -1)
	{
		ErrorLog(NULL, "Couldn't find valid video stream inside file\n");
		return false;
	}
	// setup codec context for the decoder
	av_codec_context = avcodec_alloc_context3(av_codec);
	if (!av_codec_context)
	{
		ErrorLog(NULL, "Couldn't create AVCodecContext\n");
		return false;
	}

	if (avcodec_parameters_to_context(av_codec_context, av_codec_params) < 0)
	{
		ErrorLog(NULL, "Couldn't initialize AVCodecContext\n");
		return false;
	}

	if (avcodec_open2(av_codec_context, av_codec, NULL) < 0)
	{
		ErrorLog(NULL, "Couldn't open codec\n");
		return false;
	}


	av_frame = av_frame_alloc();
	if (!av_frame)
	{
		ErrorLog(NULL, "Couldn't allocate AVFrame\n");
		return false;
	}

	av_packet = av_packet_alloc();
	if (!av_packet)
	{
		ErrorLog(NULL, "Couldn't allocate AVPacket\n");
		return false;
	}

	return true;
}

bool video_reader_read_frame(VideoReaderState *state, uint8_t* frame_buffer, int64_t *pts)
{
	auto& width = state->width;
	auto& height = state->height;
	auto& av_format_context = state->av_format_context;
	auto& av_codec_context = state->av_codec_context;
	auto& video_stream_index = state->video_stream_index;
	auto& av_frame = state->av_frame;
	auto& av_packet = state->av_packet;
	auto& sws_scaler_context = state->sws_scaler_context;

	// decode one frame
	int reponse;
	while (av_read_frame(av_format_context, av_packet) >= 0)
	{
		if (av_packet->stream_index != video_stream_index)
		{
			av_packet_unref(av_packet);
			continue;
		}
		reponse = avcodec_send_packet(av_codec_context, av_packet);
		if (reponse < 0)
		{
			ErrorLog(NULL, "Failed to decode packet\n"); //= %s\n", av_make_error(reponse));
			return false;
		}

		reponse = avcodec_receive_frame(av_codec_context, av_frame);
		if (reponse == AVERROR(EAGAIN) || reponse == AVERROR_EOF)
		{
			av_packet_unref(av_packet);
			continue;
		}
		else if (reponse < 0)
		{
			ErrorLog(NULL, "Failed to decode packet\n"); // = % s\n", av_make_error(reponse));
			return false;
		}

		av_packet_unref(av_packet);
		break;
	}

	*pts = av_frame->pts;

	TraceLog(NULL, "%c (%d pts %I64d dts  %I64d key_frame %d [coded_picture_number %d, display_picture_number %d]\n",
		av_get_picture_type_char(av_frame->pict_type),
		av_codec_context->frame_number,
		av_frame->pts,
		av_frame->pkt_dts,
		av_frame->key_frame,
		av_frame->coded_picture_number,
		av_frame->display_picture_number);


	if (!sws_scaler_context)
	{
		sws_scaler_context = sws_getContext(width, height, av_codec_context->pix_fmt,
											width, height, AV_PIX_FMT_RGB0,
											SWS_BILINEAR, NULL, NULL, NULL);

		if (!sws_scaler_context)
		{
			ErrorLog(NULL, "Couldn't initalize sw scaler\n");
		}
	}
	

	uint8_t* dest[4] = { frame_buffer, NULL, NULL, NULL };
	int dest_lines[4] = { width * 4, 0, 0, 0 };
	sws_scale(sws_scaler_context, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_lines);

	TraceLog(NULL, "exit\n");

	return true;
}

void video_reader_close_file(VideoReaderState *state)
{
	avformat_close_input(&state->av_format_context);
	avformat_free_context(state->av_format_context);
	av_frame_free(&state->av_frame);
	av_packet_free(&state->av_packet);
	avcodec_free_context(&state->av_codec_context);
	sws_freeContext(state->sws_scaler_context);
}