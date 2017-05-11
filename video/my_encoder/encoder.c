/**
 * 最简单的基于FFmpeg的视频编码器
 * Simplest FFmpeg Video Encoder
 * 
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 * 
 * 本程序实现了YUV像素数据编码为视频码流（H264，MPEG2，VP8等等）。
 * 是最简单的FFmpeg视频编码方面的教程。
 * 通过学习本例子可以了解FFmpeg的编码流程。
 * This software encode YUV420P data to H.264 bitstream.
 * It's the simplest video encoding software based on FFmpeg. 
 * Suitable for beginner of FFmpeg 
 */

#include <stdio.h>

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#else
//Linux...
#ifdef __cplusplus
#endif
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifdef __cplusplus
#endif
#endif


int flush_encoder(AVFormatContext *fmt_ctx,unsigned int stream_index){
	int ret;
	int got_frame;
	AVPacket enc_pkt;
	if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities &
		CODEC_CAP_DELAY))
		return 0;
	while (1) {
		enc_pkt.data = NULL;
		enc_pkt.size = 0;
		av_init_packet(&enc_pkt);
		ret = avcodec_encode_video2 (fmt_ctx->streams[stream_index]->codec, &enc_pkt,
			NULL, &got_frame);
		av_frame_free(NULL);
		if (ret < 0)
			break;
		if (!got_frame){
			ret=0;
			break;
		}
		printf("Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n",enc_pkt.size);
		/* mux encoded frame */
		ret = av_write_frame(fmt_ctx, &enc_pkt);
		if (ret < 0)
			break;
	}
	return ret;
}

int main(int argc, char* argv[])
{
	AVFormatContext* pFormatCtx;
	AVOutputFormat* fmt;
	AVStream* video_st;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;
	AVPacket pkt;
	uint8_t* picture_buf;
	AVFrame* pFrame;
	int picture_size;
	int y_size;
	int framecnt=0;
	//FILE *in_file = fopen("src01_480x272.yuv", "rb");	//Input raw YUV data
	FILE *in_file = fopen("../video_src/ds_480x272.yuv", "rb");   //Input raw YUV data
	int in_w=480,in_h=272;                              //Input data's width and height
	int framenum=100;                                   //Frames to encode
	//const char* out_file = "src01.h264";              //Output Filepath 
	//const char* out_file = "src01.ts";
	//const char* out_file = "src01.hevc";
	const char* out_file = "../video_out/ds.h264";

	av_register_all();//注册FFmpeg所有编解码器
	//Method1.
	pFormatCtx = avformat_alloc_context();//初始化输出码流的AVFormatContext。
	//Guess Format
	fmt = av_guess_format(NULL, out_file, NULL);
	pFormatCtx->oformat = fmt;
	
	//Method 2.
	//avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
	//fmt = pFormatCtx->oformat;


	//Open output URL
	if (avio_open(&pFormatCtx->pb,out_file, AVIO_FLAG_READ_WRITE) < 0)//打开输出文件
	{
		printf("Failed to open output file! \n");
		return -1;
	}

	video_st = avformat_new_stream(pFormatCtx, 0);//创建输出码流的AVStream
	//video_st->time_base.num = 1; 
	//video_st->time_base.den = 25;  

	if (video_st==NULL){
		return -1;
	}
	//Param that must set
	pCodecCtx = video_st->codec;
	//pCodecCtx->codec_id =AV_CODEC_ID_HEVC;
	pCodecCtx->codec_id = fmt->video_codec;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->width = in_w;  
	pCodecCtx->height = in_h;
	pCodecCtx->bit_rate = 400000;  
	pCodecCtx->gop_size=250;

	pCodecCtx->time_base.num = 1;  
	pCodecCtx->time_base.den = 25;  

	//H264
	//pCodecCtx->me_range = 16;
	//pCodecCtx->max_qdiff = 4;
	//pCodecCtx->qcompress = 0.6;
	pCodecCtx->qmin = 10;
	pCodecCtx->qmax = 51;

	//Optional Param
	pCodecCtx->max_b_frames=3;

	// Set Option
	AVDictionary *param = 0;
	//H.264
	if(pCodecCtx->codec_id == AV_CODEC_ID_H264) {
		av_dict_set(&param, "preset", "slow", 0);
		av_dict_set(&param, "tune", "zerolatency", 0);
		//av_dict_set(&param, "profile", "main", 0);
	}
	//AVCodecID testId;
	//H.265
	if(pCodecCtx->codec_id == AV_CODEC_ID_H265){
		av_dict_set(&param, "preset", "ultrafast", 0);
		av_dict_set(&param, "tune", "zero-latency", 0);
	}

	//Show some Information
	av_dump_format(pFormatCtx, 0, out_file, 1);

	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);//查找编码器。
	if (!pCodec){
		printf("Can not find encoder! \n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec,&param) < 0)//打开编码器
	{
		printf("Failed to open encoder! \n");
		return -1;
	}


	pFrame = av_frame_alloc();
	picture_size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	picture_buf = (uint8_t *)av_malloc(picture_size);
	avpicture_fill((AVPicture *)pFrame, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

	//Write File Header写文件头（对于某些没有文件头的封装格式，不需要此函数。比如说MPEG2TS）
	avformat_write_header(pFormatCtx,NULL);

	av_new_packet(&pkt,picture_size);

	y_size = pCodecCtx->width * pCodecCtx->height;

	int i;
	for (i=0; i<framenum; i++){
		//Read raw YUV data
		if (fread(picture_buf, 1, y_size*3/2, in_file) <= 0){
			printf("Failed to read raw data! \n");
			return -1;
		}else if(feof(in_file)){
			break;
		}
		pFrame->data[0] = picture_buf;              // Y
		pFrame->data[1] = picture_buf+ y_size;      // U 
		pFrame->data[2] = picture_buf+ y_size*5/4;  // V
		//PTS
		//pFrame->pts=i;
		pFrame->pts=i*(video_st->time_base.den)/((video_st->time_base.num)*25);
		int got_picture=0;
		//Encode，编码一帧视频。即将AVFrame（存储YUV像素数据）编码为AVPacket（存储H.264等格式的码流数据）
		int ret = avcodec_encode_video2(pCodecCtx, &pkt,pFrame, &got_picture);//
		if(ret < 0){
			printf("Failed to encode! \n");
			return -1;
		}
		if (got_picture==1){
			printf("Succeed to encode frame: %5d\tsize:%5d\n",framecnt,pkt.size);
			framecnt++;
			pkt.stream_index = video_st->index;
			ret = av_write_frame(pFormatCtx, &pkt);//将编码后的视频码流写入文件
			av_free_packet(&pkt);
		}
	}
	//Flush Encoder
	int ret = flush_encoder(pFormatCtx,0);//输入的像素数据读取完成后调用此函数。用于输出编码器中剩余的AVPacket
	if (ret < 0) {
		printf("Flushing encoder failed\n");
		return -1;
	}

	//Write file trailer写文件尾（对于某些没有文件头的封装格式，不需要此函数。比如说MPEG2TS）
	av_write_trailer(pFormatCtx);

	//Clean
	if (video_st){
		avcodec_close(video_st->codec);
		av_free(pFrame);
		av_free(picture_buf);
	}
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);

	fclose(in_file);

	return 0;
}

