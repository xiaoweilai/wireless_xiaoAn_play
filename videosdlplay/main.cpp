#include <QtGui/QApplication>
#include "mainwindow.h"
#include <stdio.h>
#include <QLabel>
#include <QWidget>
#include <QDebug>
#include <windows.h>

extern "C"{
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
# include <stdint.h>
#endif
}

#include <QMainWindow>
#include <QBuffer>
#include <QFile>
#include <stdio.h>
#include <stdint.h>
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "SDL/SDL.h"
#include "SDL/SDL_main.h"
};



#define SCREENSHOWWIDTH  (pCodecCtx->width/2)
#define SCREENSHOWHEIGHT  (pCodecCtx->height/2)


#ifdef __MINGW32__
#undef main /* Prevents SDL from overriding main() */
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AVFormatContext *pFormatCtx;
    int             i, videoStream;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVFrame         *pFrame;
    AVPacket        packet;
    int             frameFinished;
    float           aspect_ratio;
    static struct   SwsContext *img_convert_ctx;
    static int sws_flags = SWS_BICUBIC;
    SDL_Overlay     *bmp;
    SDL_Surface     *screen;
    SDL_Rect        rect;
    SDL_Event       event;
    const char* WINDOW_TITLE = "SDL Player";

    av_register_all();
//    avformat_network_init();
//    avdevice_register_all();
//    avcodec_register_all();

    pFormatCtx = avformat_alloc_context();
    if(NULL == pFormatCtx)
    {
        qDebug() << "Couldn't open pFormatCtx";
        return -1; // Couldn't open pFormatCtx
    }

//    avformat_get_context_defaults
//    avformat_get_context_defaults(pFormatCtx);


    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        exit(1);
    }

    // Open video file
    if(avformat_open_input(&pFormatCtx, "./test.mpg", NULL,  NULL)!=0)
    {
        qDebug() << "Couldn't open file";
        return -1; // Couldn't open file
    }
    output_formatctx_info(&pFormatCtx);

    // Retrieve stream information

    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
        return -1; // Couldn't find stream information

    // Dump information about file onto standard error
    //    dump_format(pFormatCtx, 0, "D:\\Flyhigh.wmv", 0);

    // Find the first video stream
    videoStream=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
            videoStream=i;
            break;
        }
    if(videoStream==-1)
        return -1; // Didn't find a video stream

    // Get a pointer to the codec context for the video stream
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL) {
        fprintf(stderr, "Unsupported codec!\n");
        return -1; // Codec not found
    }

    // Open codec
    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
        return -1; // Could not open codec

    // Allocate video frame
    pFrame=avcodec_alloc_frame();

    // Make a screen to put our video
#ifndef __DARWIN__
    screen = SDL_SetVideoMode(SCREENSHOWWIDTH, SCREENSHOWHEIGHT, 0, SDL_HWSURFACE | SDL_DOUBLEBUF );
#else
    screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 24, 0);
#endif
    if(!screen) {
        fprintf(stderr, "SDL: could not set video mode - exiting\n");
        exit(1);
    }
    //SDL±êÌâ
    SDL_WM_SetCaption( WINDOW_TITLE, 0 );

    // Allocate a place to put our YUV image on that screen
    bmp = SDL_CreateYUVOverlay(pCodecCtx->width,
                               pCodecCtx->height,
                               SDL_YV12_OVERLAY,
                               screen);

    // Read frames and save first five frames to disk
    i=0;

    while(av_read_frame(pFormatCtx, &packet)>=0) {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream) {
            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,
                                  &packet);
            // Did we get a video frame?
            if(frameFinished) {
                SDL_LockYUVOverlay(bmp);
                AVPicture *pict;
                pict = new AVPicture;
                pict->data[0] = bmp->pixels[0];
                pict->data[1] = bmp->pixels[2];
                pict->data[2] = bmp->pixels[1];

                pict->linesize[0] = bmp->pitches[0];
                pict->linesize[1] = bmp->pitches[2];
                pict->linesize[2] = bmp->pitches[1];

                // Convert the image into YUV format that SDL uses
                if (pCodecCtx->pix_fmt == PIX_FMT_YUV420P) {
                    /* as we only generate a YUV420P picture, we must convert it
                        to the codec pixel format if needed */
                    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                                     pCodecCtx->pix_fmt,
                                                     SCREENSHOWWIDTH, SCREENSHOWHEIGHT,
                                                     PIX_FMT_YUV420P,
                                                     sws_flags, NULL, NULL, NULL);
                    if (img_convert_ctx == NULL) {
                        fprintf(stderr, "Cannot initialize the conversion context\n");
                        exit(1);
                    }
                    sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize,
                              0, pCodecCtx->height, pict->data, pict->linesize);
                }
                //                    img_convert(&pict, PIX_FMT_YUV420P,
                //                                (AVPicture *)pFrame, pCodecCtx->pix_fmt,
                //                                pCodecCtx->width, pCodecCtx->height);
                SDL_UnlockYUVOverlay(bmp);
                rect.x = 0;
                rect.y = 0;
                rect.w = pCodecCtx->width;
                rect.h = pCodecCtx->height;
                SDL_DisplayYUVOverlay(bmp, &rect);
            }
        }

        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
        SDL_PollEvent(&event);
        switch(event.type) {
        case SDL_QUIT:
            SDL_Quit();
            exit(0);
            break;
        default:
            break;
        }

        Sleep(40);

    }
    // Free the YUV frame
    av_free(pFrame);

    // Close the codec
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);


    return a.exec();
}



void output_formatctx_info(AVFormatContext *pFormatCtx)
{
    if(pFormatCtx)
    {
        qDebug() << "" <<pFormatCtx->video_codec;
        qDebug() << "" <<pFormatCtx->video_codec;
    }
}
