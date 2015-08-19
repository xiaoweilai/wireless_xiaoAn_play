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
protected:
    void run();

private:
    quint32 nums;
};

#endif // PLAYERTHREAD_H
