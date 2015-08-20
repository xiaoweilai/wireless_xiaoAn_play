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
    qint64 lRecvPktnums;//���յ������ݰ�
    qint64 lPlayernums; // ���ŵ����ݰ�


    /*************[����log]**********************/
    QFile *plogFile;
    QString logfilename;

    /*************[�����յ������ݰ� ���ݼ���С]**********************/
    QVector<QByteArray> Pktarry;
    QVector<quint32> Pktsizearry;

    /*************[�����յ��ļ�����]**********************/
    QVector<QString> fileNameArry;
};

#endif // PLAYERTHREAD_H
