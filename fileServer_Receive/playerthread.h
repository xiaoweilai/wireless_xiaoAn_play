#ifndef PLAYERTHREAD_H
#define PLAYERTHREAD_H

#include <QThread>
#include <QtGui>

class playerThread : public QThread
{
    Q_OBJECT
public:
    explicit playerThread(QObject *parent = 0);

signals:
    void emitMsgBoxSignal();
    
public slots:
    void dealNums(qint64 ulnums);
    void DelteMpgFile();
protected:
    void run();

private:
    qint64 lRecvPktnums;//接收到的数据包
    qint64 lPlayernums; // 播放的数据包
};

#endif // PLAYERTHREAD_H
