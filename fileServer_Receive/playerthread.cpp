#include "playerthread.h"
#include <windows.h>
#include <QDebug>

#include <QtGui/QApplication>
//#include "mainwindow.h"
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


#define SC_LOG


void output_formatctx_info(AVFormatContext *pFormatCtx);



//#ifdef __MINGW32__
//#undef main /* Prevents SDL from overriding main() */
//#endif

//int main(int argc, char *argv[])
int runPlayer(void)
{
    //    QApplication a(argc, argv);

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
    output_formatctx_info(pFormatCtx);

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
    //SDL标题
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

        //        Sleep(30);
        SDL_Delay(40);

    }
    // Free the YUV frame
    av_free(pFrame);

    // Close the codec
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);


    //    return a.exec();
    return 0;
}



void output_formatctx_info(AVFormatContext *pFormatCtx)
{
    if(pFormatCtx)
    {
        qDebug() << "" <<pFormatCtx->video_codec;
        qDebug() << "" <<pFormatCtx->video_codec;
    }
}


playerThread::playerThread(QObject *parent) :
    QThread(parent)
{
    lPlayernums  = 0;
    lRecvPktnums = 0;
    LogInitLog();
    Pktarry.clear();
    Pktsizearry.clear();
}

#if 1
void playerThread::run()
{
    while(1)
    {
        if(0 == Pktarry.size())
        {
            lPlayernums = 0;
        }
        static quint8 isStarted = TRUE;
        if(Pktarry.size() > 3 )
        {
//            LogWriteFile( "starting \n");
            AVFormatContext *pFormatCtx;
            int             i, videoStream;
            AVCodecContext  *pCodecCtx;
            AVCodec         *pCodec;
            AVFrame         *pFrame;
            AVPacket        packet;
            AVPacket        *pPkt;
            int             frameFinished = 0;
            float           aspect_ratio;
            static struct   SwsContext *img_convert_ctx;
            static int sws_flags = SWS_BICUBIC;
            SDL_Overlay     *bmp;
            SDL_Surface     *screen;
            SDL_Rect        rect;
            SDL_Event       event;
            const char* WINDOW_TITLE = "SDL Player";
            #if TEST_HEVC
              AVCodecID codec_id=AV_CODEC_ID_HEVC;
              char filepath_in[]="bigbuckbunny_480x272.hevc";
            #else
              AVCodecID codec_id=AV_CODEC_ID_H264;
              char filepath_in[]="bigbuckbunny_480x272.h264";
            #endif


            if(isStarted)
            {
                av_register_all();
                //    avformat_network_init();
                //    avdevice_register_all();
                //    avcodec_register_all();

//                pFormatCtx = avformat_alloc_context();
//                if(NULL == pFormatCtx)
//                {
//                    qDebug() << "Couldn't open pFormatCtx";
//                    LogWriteFile("Couldn't open pFormatCtx");
//                    //                return -1; // Couldn't open pFormatCtx
//                }

                //    avformat_get_context_defaults
                //    avformat_get_context_defaults(pFormatCtx);


                if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
                    fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
                    LogWriteFile(QString( "Could not initialize SDL - %s\n").arg(SDL_GetError()));
                    //                exit(1);
                    continue;
                }

//                // Open video file
//                if(avformat_open_input(&pFormatCtx, "./mpg/test.mpg", NULL,  NULL)!=0)
//                {
//                    qDebug() << "Couldn't open file";
//                    LogWriteFile( "Couldn't open file\n");
//                    //                return -1; // Couldn't open file
//                    SDL_Delay(30);
//                    continue;
//                }
//                output_formatctx_info(pFormatCtx);

                // Retrieve stream information

//                if(avformat_find_stream_info(pFormatCtx, NULL)<0)
//                    //                return -1; // Couldn't find stream information
//                    continue;

                // Dump information about file onto standard error
                //    dump_format(pFormatCtx, 0, "D:\\Flyhigh.wmv", 0);

                // Find the first video stream
//                videoStream=-1;
//                for(i=0; i<pFormatCtx->nb_streams; i++)
//                    if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
//                        videoStream=i;
//                        LogWriteFile(QString("videoStream:%1\n").arg(videoStream));
//                        qDebug() << "here break 1\n";
//                        LogWriteFile( "here break 1\n");
//                        break;
//                    }
//                if(videoStream==-1)
//                    //                return -1; // Didn't find a video stream
//                    continue;

                videoStream = 1;

                LogWriteFile( "Get a pointer to the \n");
//                // Get a pointer to the codec context for the video stream
//                pCodecCtx=pFormatCtx->streams[videoStream]->codec;
////                LogWriteFile(QString("videoStream:%d\n").arg(videoStream));

//                pCodecCtx=AVCodecContext();avformat_alloc_context();
//                pCodecCtx = avcodec_alloc_context3();
//                AVFormatContext
                pCodec = avcodec_find_decoder(codec_id);
                if (!pCodec) {
                    qDebug("Codec not found\n");
                    LogWriteFile("Codec not found\n");
                    continue;
                }
                pCodecCtx = avcodec_alloc_context3(pCodec);
                if (!pCodecCtx){
                    qDebug("Could not allocate video codec context\n");
                    LogWriteFile("Could not allocate video codec context\n");
                    continue;
                }
                LogWriteFile( "Find the decoder for the video stream \n");

//                // Find the decoder for the video stream
//                pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
//                if(pCodec==NULL) {
//                    fprintf(stderr, "Unsupported codec!\n");
//                    LogWriteFile( "Unsupported codec!\n");
//                    //                return -1; // Codec not found
//                    continue;
//                }

                // Open codec
                if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
                    //                return -1; // Could not open codec
                    continue;

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
                    LogWriteFile( "SDL: could not set video mode - exiting\n");
                    exit(1);
                }
                //SDL标题
                SDL_WM_SetCaption( WINDOW_TITLE, 0 );

                // Allocate a place to put our YUV image on that screen
                bmp = SDL_CreateYUVOverlay(pCodecCtx->width,
                                           pCodecCtx->height,
                                           SDL_YV12_OVERLAY,
                                           screen);

                // Read frames and save first five frames to disk
                i=0;

                isStarted = FALSE;
            }
//            isStarted = FALSE;
#if 1
            while(1) {
//                int ret = av_read_frame(pFormatCtx, &packet);
//                if(ret < 0)
//                {
//                    qDebug() << "read frame rest:" << ret;
////                    LogWriteFile(QString( "read frame rest:%1\n").arg(ret));
//                    SDL_Delay(30);
//                    continue;
//                }


                //                if(nums)
//FF_INPUT_BUFFER_PADDING_SIZE
                // Is this a packet from the video stream?
//                if(packet.stream_index==videoStream) {
                if(1){
//                    pPkt = (AVPacket        *)&Pktarry.at(0);
                    char *ptr = (char *)Pktarry.at(0).data();
//                    quint32 pktsize = Pktsizearry.at(0);
//                    packet.data = (uint8_t *)malloc(pktsize);
                    LogWriteFile("memcpying ....\n");
//                    memcpy((uint8_t *)packet.data, ptr, Pktsizearry.at(0));
                    packet.data =(uint8_t *)Pktarry.at(0).data();
                    LogWriteFile("memcpyed!\n");

                    av_parser_init();
                    av_parser_parse2();

                    frameFinished = 0;
                    // Decode video frame
                    int ret =  avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,
                                          &packet);
                    LogWriteFile(QString("ret:%1\n").arg(ret));
                    LogWriteFile("avcodec_decode_video2!");
                    // Did we get a video frame?
                    LogWriteFile(QString("frameFinished:%1").arg(frameFinished));
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

                    Pktarry.remove(0);
                    Pktsizearry.remove(0);
//                    free(packet.data);
                }

                // Free the packet that was allocated by av_read_frame
                av_free_packet(&packet);
//                SDL_PollEvent(&event);
//                switch(event.type)
//                {
//                case SDL_QUIT:
//                    SDL_Quit();
//                    exit(0);
//                    qDebug() << "here break 2";
//                    break;
//                default:
//                    qDebug() << "here break 3";
//                    break;
//                }

                qDebug() << "lRecvPktnums - lPlayernums nums:" << lRecvPktnums - lPlayernums;
                qDebug() << "lRecvPktnums nums:" << lRecvPktnums;
                qDebug() << "lPlayernums  nums:" << lPlayernums;
                //        Sleep(30);
                if(lRecvPktnums - lPlayernums > 10 )
                {
                    SDL_Delay(30);
                }
                else if(lRecvPktnums - lPlayernums > 3 )
                {
                    SDL_Delay(40);
                }
                else if(lRecvPktnums == lPlayernums)
                {
                    DelteMpgFile();
                    continue;
                }
                else
                {
                    SDL_Delay(50);
                }
                //                SDL_Delay(40);

                emit emitMsgBoxSignal();
                lPlayernums++;
            }
            #endif
            // Free the YUV frame
            av_free(pFrame);

            // Close the codec
            avcodec_close(pCodecCtx);

//            // Close the video file
//            avformat_close_input(&pFormatCtx);


        }


        //        qDebug() << "emit signal.....";
        qDebug() << "current nums:" << lRecvPktnums;
        emit emitMsgBoxSignal();
        Sleep(100);
        //        static quint8 started = TRUE;
        //        if(nums > 3 && started)
        //        {
        //            qDebug() << "start player!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        //            runPlayer();
        //            started = FALSE;
        //        }
    }
}
#else
void playerThread::run()
{
    while(1)
    {
        if(0 == lRecvPktnums)
        {
            lPlayernums = 0;
        }
        static quint8 isStarted = TRUE;
        if(lRecvPktnums - lPlayernums > 3 )
        {
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

            if(isStarted)
            {
                av_register_all();
                //    avformat_network_init();
                //    avdevice_register_all();
                //    avcodec_register_all();

                pFormatCtx = avformat_alloc_context();
                if(NULL == pFormatCtx)
                {
                    qDebug() << "Couldn't open pFormatCtx";
                    LogWriteFile("Couldn't open pFormatCtx");
                    //                return -1; // Couldn't open pFormatCtx
                }

                //    avformat_get_context_defaults
                //    avformat_get_context_defaults(pFormatCtx);


                if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
                    fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
                    LogWriteFile(QString( "Could not initialize SDL - %s\n").arg(SDL_GetError()));
                    //                exit(1);
                    continue;
                }

                // Open video file
                if(avformat_open_input(&pFormatCtx, "./mpg/test.mpg", NULL,  NULL)!=0)
                {
                    qDebug() << "Couldn't open file";
                    LogWriteFile( "Couldn't open file\n");
                    //                return -1; // Couldn't open file
                    SDL_Delay(30);
                    continue;
                }
                output_formatctx_info(pFormatCtx);

                // Retrieve stream information

                if(avformat_find_stream_info(pFormatCtx, NULL)<0)
                    //                return -1; // Couldn't find stream information
                    continue;

                // Dump information about file onto standard error
                //    dump_format(pFormatCtx, 0, "D:\\Flyhigh.wmv", 0);

                // Find the first video stream
                videoStream=-1;
                for(i=0; i<pFormatCtx->nb_streams; i++)
                    if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
                        videoStream=i;
                        LogWriteFile(QString("videoStream:%1\n").arg(videoStream));
                        qDebug() << "here break 1\n";
                        LogWriteFile( "here break 1\n");
                        break;
                    }
                if(videoStream==-1)
                    //                return -1; // Didn't find a video stream
                    continue;

                // Get a pointer to the codec context for the video stream
                pCodecCtx=pFormatCtx->streams[videoStream]->codec;
//                LogWriteFile(QString("videoStream:%d\n").arg(videoStream));


                // Find the decoder for the video stream
                pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
                if(pCodec==NULL) {
                    fprintf(stderr, "Unsupported codec!\n");
                    LogWriteFile( "Unsupported codec!\n");
                    //                return -1; // Codec not found
                    continue;
                }

                // Open codec
                if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
                    //                return -1; // Could not open codec
                    continue;

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
                    LogWriteFile( "SDL: could not set video mode - exiting\n");
                    exit(1);
                }
                //SDL标题
                SDL_WM_SetCaption( WINDOW_TITLE, 0 );

                // Allocate a place to put our YUV image on that screen
                bmp = SDL_CreateYUVOverlay(pCodecCtx->width,
                                           pCodecCtx->height,
                                           SDL_YV12_OVERLAY,
                                           screen);

                // Read frames and save first five frames to disk
                i=0;

                isStarted = FALSE;
            }
//            isStarted = FALSE;

            while(1) {
                int ret = av_read_frame(pFormatCtx, &packet);
                if(ret < 0)
                {
                    qDebug() << "read frame rest:" << ret;
//                    LogWriteFile(QString( "read frame rest:%1\n").arg(ret));
                    SDL_Delay(30);
                    continue;
                }


                //                if(nums)
//FF_INPUT_BUFFER_PADDING_SIZE
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
//                SDL_PollEvent(&event);
//                switch(event.type)
//                {
//                case SDL_QUIT:
//                    SDL_Quit();
//                    exit(0);
//                    qDebug() << "here break 2";
//                    break;
//                default:
//                    qDebug() << "here break 3";
//                    break;
//                }

                qDebug() << "lRecvPktnums - lPlayernums nums:" << lRecvPktnums - lPlayernums;
                qDebug() << "lRecvPktnums nums:" << lRecvPktnums;
                qDebug() << "lPlayernums  nums:" << lPlayernums;
                //        Sleep(30);
                if(lRecvPktnums - lPlayernums > 10 )
                {
                    SDL_Delay(30);
                }
                else if(lRecvPktnums - lPlayernums > 3 )
                {
                    SDL_Delay(40);
                }
                else if(lRecvPktnums == lPlayernums)
                {
                    DelteMpgFile();
                    continue;
                }
                else
                {
                    SDL_Delay(50);
                }
                //                SDL_Delay(40);

                emit emitMsgBoxSignal();
                lPlayernums++;
            }
            // Free the YUV frame
            av_free(pFrame);

            // Close the codec
            avcodec_close(pCodecCtx);

            // Close the video file
            avformat_close_input(&pFormatCtx);


        }


        //        qDebug() << "emit signal.....";
        qDebug() << "current nums:" << lRecvPktnums;
        emit emitMsgBoxSignal();
        Sleep(100);
        //        static quint8 started = TRUE;
        //        if(nums > 3 && started)
        //        {
        //            qDebug() << "start player!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        //            runPlayer();
        //            started = FALSE;
        //        }
    }
}
#endif


void playerThread::dealNums(qint64 ulnums)
{
    lRecvPktnums = ulnums;
}


void playerThread::recvPkt(QByteArray pkt, quint32 size)
{
//    qDebug() << "i am received packet!!\n";
    Pktarry.append(pkt);
    Pktsizearry.append(size);
    LogWriteFile("Recv Info:-------------------\n");
    LogWriteFile(QString("==player Pktarry     size:%1\n").arg(Pktarry.size()));
    LogWriteFile(QString("==player Pktsizearry size:%1\n").arg(Pktsizearry.size()));
    LogWriteFile(QString("==player pkt         size:%1\n").arg(size));
}



void playerThread::DelteMpgFile()
{
    QString mpgfilename = "./test.mpg";
    remove(mpgfilename.toLocal8Bit().data());
    unlink(mpgfilename.toLocal8Bit().data());
}



/************************************************/
/*函 数:LogInitLog                               */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:保存log信息                                */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void playerThread::LogInitLog()
{
#ifdef SC_LOG  //将发送网络的数据有效信息进行保存
    QDate date;
    QTime time;
    logfilename = date.currentDate().toString("log/Playlogyyyy-MM-dd");
    logfilename += time.currentTime().toString("_HH-mm-ss");
    logfilename +=".log";
    plogFile = new QFile(logfilename);
    if(!plogFile)
    {
        qDebug() <<"Open file Err:" << logfilename;
        return ;
    }


    LogWriteFile("\nTcpWrite NetData Log!!\n==============>>>>Starting:\n");
    qDebug() <<"Log file:" << logfilename;
#endif

}


/************************************************/
/*函 数:LogWriteFile                              */
/*入 参:str-写入log的数据                           */
/*出 参:无                                        */
/*返 回:                                          */
/*功 能:将str写入文件                              */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void playerThread::LogWriteFile(QString str)
{
#ifdef SC_LOG
    if(plogFile)
    {
        plogFile->open(QIODevice::Append);
        plogFile->write(str.toLocal8Bit());
        plogFile->close();
    }
#endif
}


