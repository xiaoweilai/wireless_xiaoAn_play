#ifndef PLAYERTHREAD_H
#define PLAYERTHREAD_H

#include <QThread>

class playerThread : public QThread
{
    Q_OBJECT
public:
    explicit playerThread(QObject *parent = 0);
    
signals:
    void emitMsgBoxSignal();
    
public slots:
    
protected:
    void run();
};

#endif // PLAYERTHREAD_H
