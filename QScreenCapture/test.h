#ifndef TEST_H
#define TEST_H


int main(int argc, char *argv[]) {

    char *file_path = argv[1];

    LOGI("file_path:%s", file_path);



    AVFormatContext *pFormatCtx;

    AVCodecContext *pCodecCtx;

    AVCodec *pCodec;

    AVFrame *pFrame, *pFrameYUV;

    AVPacket *packet;

    uint8_t *out_buffer;



    SDL_Texture *bmp = NULL;

    SDL_Window *screen = NULL;

    SDL_Rect rect;

    SDL_Event event;



    static struct SwsContext *img_convert_ctx;



    int videoStream, i, numBytes;

    int ret, got_picture;



    av_register_all();

    pFormatCtx = avformat_alloc_context();



    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {

        LOGE("Could not initialize SDL - %s. \n", SDL_GetError());

        exit(1);

    }



    if (avformat_open_input(&pFormatCtx, file_path, NULL, NULL) != 0) {

        LOGE("can't open the file. \n");

        return -1;

    }



    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {

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



    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {

        LOGE("Could not open codec.\n");

        return -1;

    }



    pFrame = av_frame_alloc();

    pFrameYUV = av_frame_alloc();



    //---------------------------init sdl---------------------------//

    screen = SDL_CreateWindow("My Player Window", SDL_WINDOWPOS_UNDEFINED,

                              SDL_WINDOWPOS_UNDEFINED, pCodecCtx->width, pCodecCtx->height,

                              SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);



    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0);



    bmp = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12,

                            SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);



    //-------------------------------------------------------------//



    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,

                                     pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,

                                     AV_PIX_FMT_YUV, SWS_BICUBIC, NULL, NULL, NULL);



    numBytes = avpicture_get_size(AV_PIX_FMT_YUV, pCodecCtx->width,

                                  pCodecCtx->height);

    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));

    avpicture_fill((AVPicture *) pFrameYUV, out_buffer, AV_PIX_FMT_YUV,

                   pCodecCtx->width, pCodecCtx->height);



    rect.x = 0;

    rect.y = 0;

    rect.w = pCodecCtx->width;

    rect.h = pCodecCtx->height;



    int y_size = pCodecCtx->width * pCodecCtx->height;



    packet = (AVPacket *) malloc(sizeof(AVPacket));

    av_new_packet(packet, y_size);



    av_dump_format(pFormatCtx, 0, file_path, 0);



    while (av_read_frame(pFormatCtx, packet) >= 0) {

        if (packet->stream_index == videoStream) {

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

    SDL_DestroyTexture(bmp);



    av_free(out_buffer);

    av_free(pFrameYUV);

    avcodec_close(pCodecCtx);

    avformat_close_input(&pFormatCtx);



    return 0;

}

#endif // TEST_H
