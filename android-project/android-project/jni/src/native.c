/*
 * Copyright 2011 - Churn Labs, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This is mostly based off of the FFMPEG tutorial:
 * http://dranger.com/ffmpeg/
 * With a few updates to support Android output mechanisms and to update
 * places where the APIs have shifted.
 */



#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <android/log.h>
#include <android/bitmap.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswscale/swscale.h>
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_events.h>
#include <SDL_main.h>

#define  LOG_TAG    "FFMPEGSample"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define TARGET_WIDTH  (1366)
#define TARGET_HEIGHT (768)


/* Cheat to keep things simple and just use some globals. */
AVFormatContext *pFormatCtx;
AVCodecContext *pCodecCtx;
AVFrame *pFrame;
AVFrame *pFrameRGB;
AVFrame *pFrameYUV;
int videoStream;
int i, numBytes;


/*
 * init first screen things
 *
 *
 *
 *
 */
int Java_com_churnlabs_ffmpegsample_MainActivity_initScreen(JNIEnv * env, jobject this, jint width, jint height)
{
	LOGI("Screen->width-height:%d-%d\n", width,height);
	char *file_path = "/mnt/sdcard/h264/myvideofile.mpg";
	LOGI("file_path:%s", file_path);

//	AVFormatContext *pFormatCtx;
//	AVCodecContext *pCodecCtx;
	AVCodec *pCodec;
//	AVFrame *pFrame, *pFrameYUV;
	AVPacket *packet;
	uint8_t *out_buffer;

	SDL_Texture *bmp = NULL;
	SDL_Window *screen = NULL;
	SDL_Rect rect;
	SDL_Event event;

	static struct SwsContext *img_convert_ctx;

//	int videoStream, i, numBytes;
	int ret, got_picture;

	av_register_all();
	pFormatCtx = avformat_alloc_context();

	if (av_open_input_file(&pFormatCtx, file_path,  NULL, 0, NULL) != 0) {
		LOGE("can't open the file. \n");
		return -1;
	}

	if (av_find_stream_info(pFormatCtx) < 0) {
		LOGE("Could't find stream infomation.\n");
		return -1;
	}


	videoStream = 1;
	for (i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
		}
	}
	LOGI("videoStream:%d", videoStream);
	if (videoStream == -1) {
		LOGE("Didn't find a video stream.\n");
		return -1;
	}

	pCodecCtx = pFormatCtx->streams[videoStream]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {
		LOGE("Codec not found.\n");
		return -1;
	}
	LOGI("pCodecCtx codec_id:%d", pCodecCtx->codec_id);

	if(avcodec_open(pCodecCtx, pCodec)<0) {
		LOGE("Unable to open codec.\n");
		return -1;
	}

	pFrame = avcodec_alloc_frame();
	pFrameYUV = avcodec_alloc_frame();

//	LOGI("pCodecCtx width:%d", pCodecCtx->width); //1366
//	LOGI("pCodecCtx height:%d", pCodecCtx->height); //768
//


	//---------------------------init sdl---------------------------//
//	SDL_Init(SDL_INIT_EVERYTHING);
	if (SDL_Init(SDL_INIT_VIDEO)) {
		LOGE("Could not initialize SDL - %s. \n", SDL_GetError());
		exit(1);
	}
	LOGI("initialize SDL done!!. \n");

//	screen = SDL_CreateWindow("My Player Window",
//			10, 10,
//			128, 128,
////			width, height,
//			SDL_WINDOW_SHOWN);
//	if(!screen) {
//		LOGE("Could not SDL_CreateWindow SDL - %s. \n", SDL_GetError());
//		exit(1);
//	}

//	SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0);
//	bmp = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12,
//			SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
//
//	//-------------------------------------------------------------//
//	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
//			pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
//			PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
//
//	numBytes = avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width,
//			pCodecCtx->height);
//	out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
//	avpicture_fill((AVPicture *) pFrameYUV, out_buffer, PIX_FMT_YUV420P,
//			pCodecCtx->width, pCodecCtx->height);


//	rect.x = 0;
//	rect.y = 0;
//	rect.w = pCodecCtx->width;
//	rect.h = pCodecCtx->height;
//
//	int y_size = pCodecCtx->width * pCodecCtx->height;
//	packet = (AVPacket *) malloc(sizeof(AVPacket));
//	av_new_packet(packet, y_size);

//	av_dump_format(pFormatCtx, 0, file_path, 0);




//	SDL_DestroyTexture(bmp);
//	av_free(out_buffer);
//	av_free(pFrameYUV);
//	avcodec_close(pCodecCtx);
//	av_close_input_file(&pFormatCtx);

	return 0;


}



/*
 * Write a frame worth of video (in pFrame) into the Android bitmap
 * described by info using the raw pixel buffer.  It's a very inefficient
 * draw routine, but it's easy to read. Relies on the format of the
 * bitmap being 8bits per color component plus an 8bit alpha channel.
 */

static void fill_bitmap(AndroidBitmapInfo*  info, void *pixels, AVFrame *pFrame)
{
    uint8_t *frameLine;
    LOGE("info->height:%d\n", info->height);
    LOGE("info->width:%d\n", info->width);
    int  yy;
    for (yy = 0; yy < info->height; yy++) {
        uint8_t*  line = (uint8_t*)pixels;
        frameLine = (uint8_t *)pFrame->data[0] + (yy * pFrame->linesize[0]);

        int xx;
        for (xx = 0; xx < info->width; xx++) {
            int out_offset = xx * 4;
            int in_offset = xx * 3;

            line[out_offset] = frameLine[in_offset];
            line[out_offset+1] = frameLine[in_offset+1];
            line[out_offset+2] = frameLine[in_offset+2];
            line[out_offset+3] = 0;
        }
        pixels = (char*)pixels + info->stride;
    }
}

void Java_com_churnlabs_ffmpegsample_MainActivity_openFile(JNIEnv * env, jobject this)
{
    int ret;
    int err;
    int i;
    AVCodec *pCodec;
    uint8_t *buffer;
    int numBytes;

    av_register_all();
    LOGE("Registered formats");
    err = av_open_input_file(&pFormatCtx, "/mnt/sdcard/h264/myvideofile.mpg", NULL, 0, NULL);
    LOGE("Called open file");
    if(err!=0) {
        LOGE("Couldn't open file");
        return;
    }
    LOGE("Opened file");
    
    if(av_find_stream_info(pFormatCtx)<0) {
        LOGE("Unable to get stream info");
        return;
    }
    
    videoStream = -1;
    for (i=0; i<pFormatCtx->nb_streams; i++) {
        if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if(videoStream==-1) {
        LOGE("Unable to find video stream");
        return;
    }
    
    LOGI("Video stream is [%d]", videoStream);
    
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;
    
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL) {
        LOGE("Unsupported codec");
        return;
    }
    
    if(avcodec_open(pCodecCtx, pCodec)<0) {
        LOGE("Unable to open codec");
        return;
    }
    
    pFrame=avcodec_alloc_frame();
    pFrameRGB=avcodec_alloc_frame();
    LOGI("Video size is [%d x %d]", pCodecCtx->width, pCodecCtx->height);

    numBytes=avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
    buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_YUV420P,
                            pCodecCtx->width, pCodecCtx->height);
}

void Java_com_churnlabs_ffmpegsample_MainActivity_drawFrame(JNIEnv * env, jobject this, jstring bitmap)
{
    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;

    int err;
    int i;
    int frameFinished = 0;
    AVPacket packet;
    static struct SwsContext *img_convert_ctx;
    int64_t seek_target;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }
    LOGE("Checked on the bitmap");

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }
    LOGE("Grabbed the pixels");

    i = 0;
    while((i==0) && (av_read_frame(pFormatCtx, &packet)>=0)) {
  		if(packet.stream_index==videoStream) {
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
    
    		if(frameFinished) {
                LOGE("packet pts %llu", packet.pts);
                // This is much different than the tutorial, sws_scale
                // replaces img_convert, but it's not a complete drop in.
                // This version keeps the image the same size but swaps to
                // RGB24 format, which works perfect for PPM output.
                int target_width = TARGET_WIDTH;
                int target_height = TARGET_HEIGHT;
                img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, 
                       pCodecCtx->pix_fmt, 
                       target_width, target_height, PIX_FMT_YUV420P, SWS_BICUBIC,
                       NULL, NULL, NULL);
                if(img_convert_ctx == NULL) {
                    LOGE("could not initialize conversion context\n");
                    return;
                }

//                LOGE("pFrame linesize :%d\n", pFrame->linesize);
                LOGE("pCodecCtx height:%d\n", pCodecCtx->height);
                LOGE("pCodecCtx width :%d\n", pCodecCtx->width);
//                LOGE("pFrameRGB linesize :%d\n", pFrameRGB->linesize);

                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, target_height, pFrameRGB->data, pFrameRGB->linesize);

                // save_frame(pFrameRGB, target_width, target_height, i);
                fill_bitmap(&info, pixels, pFrameRGB);
                i = 1;
    	    }
        }
        av_free_packet(&packet);
    }

    AndroidBitmap_unlockPixels(env, bitmap);
}



int  Java_com_churnlabs_ffmpegsample_MainActivity_ProcPacketsDisplay(JNIEnv * env, jobject this,  jbyteArray jarr, jint len)
{
	LOGI("array len:%d\n", len);
	typedef unsigned int BYTE;

	AVPacket packet;

	//»ñÈ¡jbyteArray
	jbyte   *   arr   =   (*env)-> GetByteArrayElements(env, jarr,   NULL);
	char * c=(char *)arr;
//	memcpy(&(packet.data), c, len);

	int i=0;
	for(i=0;i<len;i++)
	{
		LOGI("RECVTEST:%d",c[i]);
	}

	return 0;
}


int seek_frame(int tsms)
{
    int64_t frame;

    frame = av_rescale(tsms,pFormatCtx->streams[videoStream]->time_base.den,pFormatCtx->streams[videoStream]->time_base.num);
    frame/=1000;
    
    if(avformat_seek_file(pFormatCtx,videoStream,0,frame,frame,AVSEEK_FLAG_FRAME)<0) {
        return 0;
    }

    avcodec_flush_buffers(pCodecCtx);

    return 1;
}

void Java_com_churnlabs_ffmpegsample_MainActivity_drawFrameAt(JNIEnv * env, jobject this, jstring bitmap, jint secs)
{
    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;

    int err;
    int i;
    int frameFinished = 0;
    AVPacket packet;
    static struct SwsContext *img_convert_ctx;
    int64_t seek_target;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }
    LOGE("Checked on the bitmap");

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }
    LOGE("Grabbed the pixels");

    seek_frame(secs * 1000);

    i = 0;
    while ((i== 0)&&(0==frameFinished) && (av_read_frame(pFormatCtx, &packet)>=0)) {
  		if(packet.stream_index==videoStream) {
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
    
    		if(frameFinished) {
                // This is much different than the tutorial, sws_scale
                // replaces img_convert, but it's not a complete drop in.
                // This version keeps the image the same size but swaps to
                // RGB24 format, which works perfect for PPM output.
                int target_width = TARGET_WIDTH;
                int target_height = TARGET_HEIGHT;
                LOGE("pCodecCtx width:%d,height:%d", pCodecCtx->width, pCodecCtx->height);
                img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, 
                       pCodecCtx->pix_fmt, 
                       target_width, target_height, PIX_FMT_RGB24, SWS_BICUBIC,
                       NULL, NULL, NULL);
                if(img_convert_ctx == NULL) {
                    LOGE("could not initialize conversion context\n");
                    return;
                }
                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

                // save_frame(pFrameRGB, target_width, target_height, i);
                fill_bitmap(&info, pixels, pFrameRGB);
                frameFinished = 0;
                i = 1;
    	    }
        }
        av_free_packet(&packet);
    }

    AndroidBitmap_unlockPixels(env, bitmap);
}
