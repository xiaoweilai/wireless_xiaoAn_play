#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <android/log.h>
#include <android/bitmap.h>

//#undef SDL_main
#include "libavcodec/avcodec.h"
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
#define PLAYVIDEONAME  "/mnt/sdcard/h264/test.mpg"

/*
 * the last screen size
 *
 */
#define COOLPAD5217
#ifdef COOLPAD5217
#define SCREENSHOWWIDTH   (480)
#define SCREENSHOWHEIGHT  (320)

#elif ANDROID_TINY210_800_480
#define SCREENSHOWWIDTH   (800)
#define SCREENSHOWHEIGHT  (480)
#else
#define SCREENSHOWWIDTH  (pCodecCtx->width/2)
#define SCREENSHOWHEIGHT  (pCodecCtx->height/2)
#endif



#include "SDL.h"
/*
 * test change it
 *
 *
 * */

#if 1

/* Cheat to keep things simple and just use some globals. */
static int initialed = 0; //是否初始化了
AVFormatContext *pFormatCtx;
AVCodecContext *pCodecCtx;
AVFrame *pFrame;
AVFrame *pFrameRGB;
AVFrame *pFrameYUV;
int videoStream;
int i, numBytes;
char *file_path = NULL;


AVCodec *pCodec;
AVPacket *packet;
uint8_t *out_buffer;

SDL_Texture *bmp = NULL;
SDL_Window *screen = NULL;
SDL_Rect rect;
SDL_Event event;

struct SwsContext *img_convert_ctx;

//	int videoStream, i, numBytes;
int ret, got_picture;

//	SDL_Window* window;
SDL_Renderer* renderer;



int main(int argc, char* argv[])
{

	//	LOGI("Screen->width-height:%d-%d\n", width,height);
	file_path = PLAYVIDEONAME;
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

	//	//	SDL_Window* window;
	//	SDL_Renderer* renderer;

	// Initialize SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return 1;
	}
	LOGI("initialize SDL done!!. \n");

	// Create the window where we will draw.
	screen = SDL_CreateWindow(
			"SDL_RenderClear",         // window title
			SDL_WINDOWPOS_UNDEFINED,   // initial x position
			SDL_WINDOWPOS_UNDEFINED,   // initial y position
			SCREENSHOWWIDTH,                      // width, in pixels
			SCREENSHOWHEIGHT,                       // height, in pixels
			SDL_WINDOW_RESIZABLE          // flags - see below
	);
	//	Remarks
	//
	//	flags may be any of the following OR'd together:
	//
	//	SDL_WINDOW_FULLSCREEN
	//	fullscreen window
	//	SDL_WINDOW_FULLSCREEN_DESKTOP
	//	fullscreen window at the current desktop resolution
	//	SDL_WINDOW_OPENGL
	//	window usable with OpenGL context
	//	SDL_WINDOW_HIDDEN
	//	window is not visible
	//	SDL_WINDOW_BORDERLESS
	//	no window decoration
	//	SDL_WINDOW_RESIZABLE
	//	window can be resized
	//	SDL_WINDOW_MINIMIZED
	//	window is minimized
	//	SDL_WINDOW_MAXIMIZED
	//	window is maximized
	//	SDL_WINDOW_INPUT_GRABBED
	//	window has grabbed input focus
	//	SDL_WINDOW_ALLOW_HIGHDPI
	//	window should be created in high-DPI mode if supported (>= SDL 2.0.1)
	//	SDL_WINDOW_SHOWN is ignored by SDL_CreateWindow(). The SDL_Window is implicitly shown if SDL_WINDOW_HIDDEN is not set. SDL_WINDOW_SHOWN may be queried later using SDL_GetWindowFlags().

	// Check that the window was successfully made
	if (screen == NULL) {
		// In the event that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}
	// We must call SDL_CreateRenderer in order for draw calls to affect this window.
	renderer = SDL_CreateRenderer(screen, -1, 0);

	bmp = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12,
			SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
	//-------------------------------------------------------------//
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
			pCodecCtx->pix_fmt, SCREENSHOWWIDTH, SCREENSHOWHEIGHT,
			PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	numBytes = avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width,
			pCodecCtx->height);
	out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture *) pFrameYUV, out_buffer, PIX_FMT_YUV420P,
			pCodecCtx->width, pCodecCtx->height);


	rect.x = 0;
	rect.y = 0;
	rect.w = pCodecCtx->width;
	rect.h = pCodecCtx->height;

	int y_size = pCodecCtx->width * pCodecCtx->height;
	packet = (AVPacket *) malloc(sizeof(AVPacket));
	av_new_packet(packet, y_size);

	//	av_dump_format(pFormatCtx, 0, file_path, 0);


	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if (packet->stream_index == videoStream) {
			got_picture = -1;
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,
					packet);
			if (ret < 0) {
				LOGE("decode error.\n");
				return -1;
			}

			LOGI("got_picture:%d", got_picture);
			if (got_picture) {
				sws_scale(img_convert_ctx,
						(uint8_t const * const *) pFrame->data,
						pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data,
						pFrameYUV->linesize);
				////iPitch 计算yuv一行数据占的字节数
				SDL_UpdateTexture(bmp, &rect, pFrameYUV->data[0], pFrameYUV->linesize[0]);
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, bmp, &rect, &rect);
				SDL_RenderPresent(renderer);
			}

			SDL_Delay(50);
		}

		av_free_packet(packet);

		SDL_PollEvent(&event);
		switch (event.type) {
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
			break;
		default:
			break;
		}
	}

#if 1
	// Select the color for drawing. It is set to red here.
	SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);

	// Clear the entire screen to our selected color.
	SDL_RenderClear(renderer);

	// Up until now everything was drawn behind the scenes.
	// This will show the new, red contents of the window.
	SDL_RenderPresent(renderer);

	// Give us time to see the window.
	//	SDL_Delay(5000);

	//	// Always be sure to clean up
	//	SDL_Quit();
#endif
	// Close and destroy the window
	SDL_DestroyWindow(screen);
	SDL_DestroyTexture(bmp);
	// Always be sure to clean up
	SDL_Quit();


	av_free(out_buffer);
	av_free(pFrameYUV);
	avcodec_close(pCodecCtx);
	av_close_input_file(pFormatCtx);
	return 0;
}

#else


/*
 * org main,good
 * */
int main(int argc, char* argv[])
{
	SDL_Window* window;
	SDL_Renderer* renderer;

	// Initialize SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	// Create the window where we will draw.
	window = SDL_CreateWindow("SDL_RenderClear",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			512, 512,
			SDL_WINDOW_SHOWN);

	// We must call SDL_CreateRenderer in order for draw calls to affect this window.
	renderer = SDL_CreateRenderer(window, -1, 0);

	// Select the color for drawing. It is set to red here.
	SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);

	// Clear the entire screen to our selected color.
	SDL_RenderClear(renderer);

	// Up until now everything was drawn behind the scenes.
	// This will show the new, red contents of the window.
	SDL_RenderPresent(renderer);

	// Give us time to see the window.
	SDL_Delay(5000);

	// Always be sure to clean up
	SDL_Quit();
	return 0;
}
#endif
