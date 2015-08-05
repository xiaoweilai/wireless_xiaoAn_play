/************************************************/
/*声 明:  @db.com                                */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*功 能:捕屏线程头文件                              */
/*author :wxj                                    */
/*email  :wxjlmr@126.com                         */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
/*更新记录:                                        */
/*                                                */
/*************************************************/
#ifndef CAPTHREAD_H
#define CAPTHREAD_H

#define MAINWORKING
#ifdef MAINWORKING

extern "C"{
#ifdef __cplusplus
 #define __STDC_CONSTANT_MACROS
 #ifdef _STDINT_H
  #undef _STDINT_H
 #endif
 # include <stdint.h>
#endif
}

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
//#include "SDL/SDL_main.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <SDL/SDL.h>
#ifdef __cplusplus
};
#endif
#endif


#include <QThread>
#include <QImage>
#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QDate>
#include <QTime>
#include <stdio.h>



#if 1
//编码汉字
//#define str_china(A)     QString::fromLocal8Bit(#A)
#define str_china(A)     QString::fromLocal8Bit(A)
//#define str_china(A)     QString::fromUtf8(#A)
#else
#define str_china(A)     codec->toUnicode(#A)
#endif


#define OUTPUT_YUV420P 0
//'1' Use Dshow
//'0' Use GDIgrab
#define USE_DSHOW 1  // "video=screen-capture-recorder"
//Refresh Event
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)
//#define ORGSRC
#define DESK_WIDTH  (1366)
#define DESK_HEIGHT (768)
#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
#define DEFAULT_PORT   "16689"

//是否调试
#ifndef DEBUG
#define DEBUG
#endif
//是否将获取的视频转换到文件中
//#define STREAMTOFILE

#define  SC_LOG_AVINFO  //将抓屏线程数据有效信息进行保存

struct AVFrame;
struct AVPacket;
struct AVCodec;
struct AVCodecContext;

enum
{
    STAT_THREAD_RUNNING,
    STAT_THREAD_STOPED,
    STAT_THREAD_QUIT
};


enum
{
    RET_SUCESS,
    RET_FAIL,
    RET_UNKNOWN
};

enum
{
    STAT_STARTED,
    STAT_STOPPED,
    STAT_RESTART,/* 重新启动连接 */
    STAT_RESTOP, /* 需要重新启动连接 */
    STAT_UNKNOWN
};

class CapThread : public QThread
{
    Q_OBJECT

public:
    explicit CapThread(int width,int height, QObject *parent);
    ~CapThread();
    void sendSDLQuit();
    void SetThreadFlag(quint8 flag);
    quint8 GetThreadFlag(void);
    int WithNetworkInit(QString ipaddr);
    int SendPkgData(AVPacket *pkt);

    /*************[保存log]**********************/
    void LogInitLog();
    void LogWriteFile(QString str);
public:
    void show_dshow_device();
    void show_avfoundation_device();
private:
    int resize_width, resize_height;
    AVFormatContext	*pFormatCtx; /*  */
    AVCodecContext	*pEc,*pDc; /* 编码、解码 */
    AVCodec			*pEcodec,*pDcodec;
    AVPacket *pkt;/* 包 */
    AVFrame	 *pEframe,*pDframe,*pDFrameYUV;/* 编码帧、解码帧 */
#ifdef STREAMTOFILE
    FILE* f;/* 文件句柄 */
#endif
    int	i,ret,got_output,got_picture, videoindex;/*  */

    struct SwsContext *img_convert_ctx;
    SDL_Thread *video_tid;
    int execcount;
    int pktnum;
    SDL_Overlay *bmp;
    SDL_Rect rect;
    quint8 m_threadstate;

    /*************[保存log]**********************/
    QFile *plogav;
    QString logavfilename;

public:
    /*************[网络传输]**********************/
    QList<int> arrayNetSize; //包大小
    QList<QByteArray> arrayNetData; //包数据

private:
    void run();
signals:

public slots:
    void capFrame();
    void SetStartThread();
    void SetStopThread();
    void SetQuitThread();
//    void displayErr(QAbstractSocket::SocketError socketError);
//    void updateClientProgress(qint64 numBytes);
};

#endif // CAPTHREAD_H
