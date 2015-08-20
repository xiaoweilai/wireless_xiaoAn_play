#ifndef PLAYERTHREAD_H
#define PLAYERTHREAD_H

#include <QThread>
#include <QtGui>
#include <QFile>
#include <QDate>
#include <QTime>


class playerThread : public QThread
{
    Q_OBJECT
public:
    explicit playerThread(QObject *parent = 0);

    void LogInitLog();
    void LogWriteFile(QString str);

signals:
    void emitMsgBoxSignal();
    
public slots:
    void dealNums(qint64 ulnums);
    void recvPkt(QByteArray pkt, quint32 size);
    void recvFileName(QString filename);
    int GetFileNameArrSize();
    void DelteMpgFile();
protected:
    void run();

private:
    qint64 lRecvPktnums;//接收到的数据包
    qint64 lPlayernums; // 播放的数据包


    /*************[保存log]**********************/
    QFile *plogFile;
    QString logfilename;

    /*************[保存收到的数据包 数据及大小]**********************/
    QVector<QByteArray> Pktarry;
    QVector<quint32> Pktsizearry;

    /*************[保存收到文件名称]**********************/
    QVector<QString> fileNameArry;
};

#endif // PLAYERTHREAD_H
