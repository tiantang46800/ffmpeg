/**
 * 最简单的基于FFmpeg的解码器
 * Simplest FFmpeg Decoder
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本程序实现了视频文件的解码(支持HEVC，H.264，MPEG2等)。
 * 是最简单的FFmpeg视频解码方面的教程。
 * 通过学习本例子可以了解FFmpeg的解码流程。
 * This software is a simplest video decoder based on FFmpeg.
 * Suitable for beginner of FFmpeg.
 *
 */



#include <stdio.h>
#define __STDC_CONSTANT_MACROS

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"


int main(int argc, char* argv[])
{
	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame,*pFrameYUV;
	uint8_t *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;
	//输入文件路径
	char filepath[]="../video_src/Titanic.ts";//文件名称后缀对于后面的代码是否有影响，h264+mp3

	int frame_cnt;

	av_register_all();//注册所有的组件 formats and protocols 
	pFormatCtx = avformat_alloc_context(); //分配内存AVFormatContext
	if( NULL ==  pFormatCtx )
	{
		perror("avformat_alloc_context");
		return -1;
	}

	if(avformat_open_input(&pFormatCtx,filepath,NULL,NULL)!=0){
		printf("Couldn't open input stream.\n");
		return -1;
	}
	if(avformat_find_stream_info(pFormatCtx,NULL)<0){
		printf("Couldn't find stream information.\n");
		return -1;
	}
	videoindex=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++) 
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
			videoindex=i;
			break;
		}
	if(videoindex==-1){
		printf("Didn't find a video stream.\n");
		return -1;
	}

	pCodecCtx=pFormatCtx->streams[videoindex]->codec;//找到视频的那个流里面的解码器
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);//找到视频的那个流里面的解码器
	if(pCodec==NULL){
		printf("Codec not found.\n");
		return -1;
	}
	if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){//打开视频流的解码器
		printf("Could not open codec.\n");
		return -1;
	}
	/*
	 * 在此处添加输出视频信息的代码
	 * 取自于pFormatCtx，使用fprintf()
	 */
	pFrame=av_frame_alloc();
	if( NULL == pFrame )
	{
		perror("av_frame_alloc");
		return -1;
	}
	pFrameYUV=av_frame_alloc();
	if( NULL == pFrameYUV )
	{
		perror("av_frame_alloc");
		return -1;
	}
	out_buffer=(uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_UYVY422, pCodecCtx->width, pCodecCtx->height));
	//out_buffer=(uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height));
//	out_buffer=(uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	if( NULL == out_buffer )
	{
		perror("av_frame_alloc");
		return -1;
	}
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_UYVY422, pCodecCtx->width, pCodecCtx->height);//out_buffer 挂在pFrameYUV下
//	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);//out_buffer 挂在pFrameYUV下
//	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);//out_buffer 挂在pFrameYUV下

	packet=(AVPacket *)av_malloc(sizeof(AVPacket));
	if(NULL == packet )
	{
		perror("av_malloc");
		return -1;
	}
	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx,0,filepath,0);
	printf("-------------------------------------------------\n");
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_UYVY422, SWS_BILINEAR, NULL, NULL, NULL); 
		//pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL); 
		//pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 
	if( NULL == img_convert_ctx )
	{
		perror("sws_getContext");
		return -1;
	}

	frame_cnt=0;
	FILE *fp_264;
	fp_264 = fopen("../video_out/test.h264","w+");
	if( NULL == fp_264)
	{
		perror("fopen");
		return -1;
	}
	FILE *fp_mp3;
	fp_mp3 = fopen("../video_out/test.mp3","w+");
	if( NULL == fp_mp3)
	{
		perror("fopen");
		return -1;
	}
	FILE *fp_yuv;
	fp_yuv = fopen("../video_out/test.yuv","w+");
	//fp_yuv = fopen("test.yuv","w+");//rgb24 yuv420p uyuv422 must 
	if( NULL == fp_yuv)
	{
		perror("fopen");
		return -1;
	}
	while(av_read_frame(pFormatCtx, packet)>=0){//从文件中读取了一个packet（压缩的数据），这个压缩数据可能是视频的，也可能是音频的
		printf("packet->stream_index is %d\n",packet->stream_index);
		if(packet->stream_index!=videoindex)
		{
			ret = fwrite(packet->data,1,packet->size,fp_mp3);//抓取音频流,h264的视频，mp3的音频,ffplay播放时候有卡吨声音
			if( -1 == ret )
			{
				perror("fwrite");
				return -1;
			}
	
		}
		if(packet->stream_index==videoindex){
				/*
				 * 在此处添加输出H264码流的代码
				 * 取自于packet，使用fwrite()
				 */
			ret = fwrite(packet->data,1,packet->size,fp_264);
			if( -1 == ret )
			{
				perror("fwrite");
				return -1;
			}
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);//调用打开的解码器，把packet数据解压到pFrame中
			if(ret < 0){
				printf("Decode Error.\n");
				return -1;
			}
			if(got_picture){
				//解码下来的pFrame中的一帧数据，有多余的数据，使用sws_scale来最终计算出正确的大小
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, 
					pFrameYUV->data, pFrameYUV->linesize);
				printf("Decoded frame index: %d\n",frame_cnt);

				/*
				 * 在此处添加输出YUV的代码
				 * 取自于pFrameYUV，使用fwrite()
				 */
				printf("p h is %d,p w is %d\n",pCodecCtx->height,pCodecCtx->width);
				printf("F h is %d,F w is %d\n",pFrameYUV->height,pFrameYUV->width);
#if 0 
				//yuv420p
				ret = fwrite(pFrameYUV->data[0],1,pCodecCtx->height*pCodecCtx->width,fp_yuv);
				ret = fwrite(pFrameYUV->data[1],1,pCodecCtx->height*pCodecCtx->width/4,fp_yuv);
				ret = fwrite(pFrameYUV->data[2],1,pCodecCtx->height*pCodecCtx->width/4,fp_yuv);
#endif
				//uyuv422
				ret = fwrite(pFrameYUV->data[0],2,pCodecCtx->height*pCodecCtx->width,fp_yuv);
  				//fprintf(fp_yuv, "P6\n%d %d\n255\n", pCodecCtx->width, pCodecCtx->height); //yi ge ppm wen jian 
				//rgb24
				//ret = fwrite(pFrameYUV->data[0],1,pCodecCtx->height*pCodecCtx->width*3,fp_yuv);
				//fclose(fp_yuv);
				//exit(0);
				if( -1 == ret )
				{
					perror("fwrite");
					return -1;
				}
				frame_cnt++;

			}
		}
		av_free_packet(packet);
	}
	fclose(fp_264);
	fclose(fp_yuv);
	fclose(fp_mp3);
	sws_freeContext(img_convert_ctx);

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}
