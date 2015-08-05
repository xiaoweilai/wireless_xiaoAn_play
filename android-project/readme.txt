#define PLAYVIDEONAME  "/mnt/sdcard/h264/test.mpg"    文件位置

/*
 * the last screen size
 *
 */
分辨率
#define COOLPAD5217  默认使用这个，如果手机屏幕不合适，请修改
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
