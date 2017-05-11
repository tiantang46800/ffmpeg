
/**
 * 最简单的SDL2播放视频的例子（SDL2播放RGB/YUV）
 * Simplest Video Play SDL2 (SDL2 play RGB/YUV) 
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本程序使用SDL2播放RGB/YUV视频像素数据。SDL实际上是对底层绘图
 * API（Direct3D，OpenGL）的封装，使用起来明显简单于直接调用底层
 * API。
 *
 * This software plays RGB/YUV raw video data using SDL2.
 * SDL is a wrapper of low-level API (Direct3D, OpenGL).
 * Use SDL is much easier than directly call these low-level API.  
 */

#include <stdio.h>
#define __STDC_CONSTANT_MACROS

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"


#include "SDL.h"

const int bpp=12;

int screen_w=500,screen_h=500;// 预定义显示的窗口的大小
//const int pixel_w=320,pixel_h=180;


//Refresh Event
#define REFRESH_EVENT  (SDL_USEREVENT + 1)
//Break
#define BREAK_EVENT  (SDL_USEREVENT + 2)

int thread_exit=0;

int refresh_video(void *opaque)//线程
{
	thread_exit=0;
	while (thread_exit==0) {
		SDL_Event event;
		event.type = REFRESH_EVENT;
		SDL_PushEvent(&event);//提交事件
		SDL_Delay(40);//delay的默认时间是40，但是这里恢复回来就卡吨
	}
	thread_exit=0;
	//Break
	SDL_Event event;
	event.type = BREAK_EVENT;
	SDL_PushEvent(&event);
	return 0;
}

int main(int argc, char* argv[])
{

	AVFormatContext	*pFormatCtx;//未来存放封装格式信息
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;//未来存放编码解码？器上下文结构体，包含视频的大小数据
	AVCodec			*pCodec;
	AVFrame	*pFrame,*pFrameYUV;//未来存放 原始数据帧
	uint8_t *out_buffer;//什么作用不是特别清除
	AVPacket *packet;//一个编好码的包的数据
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;
	//输入文件路径
	char filepath[]="../video_src/Titanic.ts";//文件名称后缀对于后面的代码是否有影响，h264+mp3

	int frame_cnt;//用来计算一共有多少数据帧

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
			videoindex=i;//查找到视频流是数组streams哪个下标
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
	out_buffer=(uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	if( NULL == out_buffer )
	{
		perror("av_frame_alloc");
		return -1;
	}
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);//out_buffer 挂在pFrameYUV下 干什么作用？

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
			pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 
	if( NULL == img_convert_ctx )
	{
		perror("sws_getContext");
		return -1;
	}




	unsigned char buffer[pCodecCtx->width*pCodecCtx->height*bpp/8];//根据一帧YUV图像的大小创建内存，单位字节
	if(SDL_Init(SDL_INIT_VIDEO))//初始化sdl系统，参数什么含义
	 {  
		printf( "Could not initialize SDL - %s\n", SDL_GetError()); 
		return -1;
	} 

	SDL_Window *screen; 
	//SDL 2.0 Support for multiple windows
	screen = SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screen_w, screen_h,SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);//构建窗口，窗口大小
	if(!screen) {  
		printf("SDL: could not create window - exiting:%s\n",SDL_GetError());  
		return -1;
	}
	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(screen, -1, 0); //构建渲染器 

	Uint32 pixformat=0;
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	pixformat= SDL_PIXELFORMAT_IYUV;  //这个是什么格式？

	SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer,pixformat, SDL_TEXTUREACCESS_STREAMING,pCodecCtx->width,pCodecCtx->height);//创建纹理，视频实际大小
	if( NULL == sdlTexture)
	{
		perror("SDL_CreateTexture");
		return -1;
	}


	SDL_Rect sdlRect;  

	SDL_Thread *refresh_thread = SDL_CreateThread(refresh_video,NULL,NULL);//线程 
	if( NULL == refresh_thread )
	{
		perror("SDL_CreateThread");
		return -1;
	}
	SDL_Event event;

	while(1){
		//Wait
		SDL_WaitEvent(&event);
		if(event.type==REFRESH_EVENT){
#if 0
			if (fread(buffer, 1, pixel_w*pixel_h*bpp/8, fp) != pixel_w*pixel_h*bpp/8){
				// Loop
				fseek(fp, 0, SEEK_SET);
				fread(buffer, 1, pixel_w*pixel_h*bpp/8, fp);
			}
#endif 
			//buffer <<-- Output 
			while(1)
			{
				ret = av_read_frame(pFormatCtx, packet);
				if( -1 == ret )
				{
					break;
				}
				if(packet->stream_index==videoindex){
					break;
				}
			}
			//从文件中读取了一个packet（压缩的数据），这个压缩数据可能是视频的，也可能是音频的
			//printf("packet->stream_index is %d\n",packet->stream_index);
			if(packet->stream_index!=videoindex)
			{
			}
			if(packet->stream_index==videoindex){
				/*
				 * 在此处添加输出H264码流的代码
				 * 取自于packet，使用fwrite()
				 */
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
					frame_cnt++;

				}
			}
			av_free_packet(packet);
			memcpy(buffer,pFrameYUV->data[0],pCodecCtx->height*pCodecCtx->width);
			memcpy(buffer+pCodecCtx->height*pCodecCtx->width,pFrameYUV->data[1],pCodecCtx->height*pCodecCtx->width/4);
			memcpy(buffer+pCodecCtx->height*pCodecCtx->width*5/4,pFrameYUV->data[2],pCodecCtx->height*pCodecCtx->width/4);

			//SDL_UpdateTexture( sdlTexture, NULL, pFrameYUV->data, pixel_w); //更新纹理 ，buffer是一帧yuv的数据
			SDL_UpdateTexture( sdlTexture, NULL, buffer, pCodecCtx->width); //更新纹理 ，buffer是一帧yuv的数据

			//FIX: If window is resize
			sdlRect.x = 0;  
			sdlRect.y = 0;  
			sdlRect.w = screen_w;  
			sdlRect.h = screen_h;  
			
			SDL_RenderClear( sdlRenderer );   
			SDL_RenderCopy( sdlRenderer, sdlTexture, NULL, &sdlRect); //显示 
			SDL_RenderPresent( sdlRenderer );  
			
		}else if(event.type==SDL_WINDOWEVENT){
			//If Resize
			SDL_GetWindowSize(screen,&screen_w,&screen_h);
		}else if(event.type==SDL_QUIT){
			thread_exit=1;
		}else if(event.type==BREAK_EVENT){
			break;
		}
	}
	SDL_Quit();


	sws_freeContext(img_convert_ctx);

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
	return 0;
}
