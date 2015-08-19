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
    qint64 lRecvPktnums;//���յ������ݰ�
    qint64 lPlayernums; // ���ŵ����ݰ�
};

#endif // PLAYERTHREAD_H
