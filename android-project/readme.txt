#define PLAYVIDEONAME  "/mnt/sdcard/h264/test.mpg"    �ļ�λ��

/*
 * the last screen size
 *
 */
�ֱ���
#define COOLPAD5217  Ĭ��ʹ�����������ֻ���Ļ�����ʣ����޸�
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
