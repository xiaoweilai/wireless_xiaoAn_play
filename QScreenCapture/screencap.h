/************************************************/
/*�� ��:  @db.com                                */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*�� ��:���ڲ����ĳ�ʼ�����������̴��������紫��ͷ�ļ�    */
/*author :wxj                                    */
/*email  :wxjlmr@126.com                         */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
/*���¼�¼:                                        */
/*                                                */
/*************************************************/

#ifndef SCREENCAP_H
#define SCREENCAP_H
//#define __STDC_CONSTANT_MACROS

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
#include <capthread.h>
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
//#include "SDL/SDL_main.h"
};

enum{
    SEND_UNDO,
    SEND_DONE,
    SEND_UNKNOWN
};

enum WriteDataFlag{
    WRITE_HEAD,
    WRITE_BODY,
    WRITE_ALL,
    WRITE_INVLAID
};

//�Ƿ񱣴�LOG��Ϣ���ļ�
#define SC_LOG
//���緢�����ݵ�����
#define SC_DATASTREAM_LOG
//���ļ���������д���ļ�
#define SC_LOG_DATA_WITHNAME

#define SC_LOG_RECVDATA_FILENAME "sclog_recvdata.bin"


namespace Ui {
class ScreenCap;
}

class ScreenCap : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ScreenCap(QWidget *parent = 0);
    void StartTransferThread(void);
    ~ScreenCap();

    void SetThreadFlag(quint8 flag);
    quint8 GetThreadFlag(void);
    CapThread *pCapThread;
    int CreateCapturethread();
    int CreateTcpSocket();

private slots:
    void on_pushButtonStart_clicked();
    void LineTextTips(QString str);
    void StopActionSets();
private:
    void showTextTransfering();
    void showTextTransferingNo(quint64 num);
    void showTextTransferOver();
    void showStateBarInfo(const char *pstr);
    void showAppVerion(void);
    void showTextClickToStart(void);
    void showTextClickOverToReStart(void);
    void showTextConnecting(void);
    void BtnStartPix(void);
    void BtnStopPix(void);
    void BtnSetPix(QString str);
    void BtnEnable(void);
    void BtnDisable(void);
    int CaptureScreenOn();
    void StopCapScreen();

    /*************[���紫��]**********************/
    qint64 writeNetData(const QByteArray &iData, WriteDataFlag flag);
private:
    Ui::ScreenCap *ui;
    static int isStarted;
    static unsigned int mNo;

    /*************[���紫��]**********************/
    QTcpSocket *p_tcpClient;
    QByteArray outBlock;       //����һ�η��͵�����
    QByteArray outBlkData;//����һ�η��͵�����
    qint64 TotalBytes;
    qint64 byteWritten;
    qint64 bytesToWrite;
    quint64 picNametime;
    QTimer *pNetSendTimer;
    qint64  loadSize;          //����ʼ��Ϊһ��4Kb�ĳ���
    QBuffer buffer;//�����������ݵ�һ������
    QByteArray tmpbyte;//������������n��������
    quint8 sendDoneFlag;//�����Ƿ������


    /*************[����log]**********************/
    QFile *plogFile;
    QString logfilename;
    QFile *pdataFile;
    QString datafilename;

private:
    void LogInitLog();
    void LogWriteFile(QString str);
    void LogWriteDataFile(const QByteArray &data);
    void LogWriteFileComWithName(QString filename,QByteArray data);

signals:
    void emitCtrlPthreadStart();
    void emitCtrlPthreadStop();
    void emitCtrlPthreadQuit();


public slots:
    int  CheckIPAddrValid(QString ipaddr);
    int  WithNetworkInit(QString ipaddr);
    void displayNetErr(QAbstractSocket::SocketError socketError);
    QString getSockState(QAbstractSocket::SocketState state);
    void updateClientProgress(qint64 numBytes);
    void NetSendData();
    void TimerSets();
    void MergeSendMessage();
    void InfoRecvMessage();
};

#endif // SCREENCAP_H
