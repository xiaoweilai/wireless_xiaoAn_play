#include "playerthread.h"
#include <windows.h>
#include <QDebug>

playerThread::playerThread(QObject *parent) :
    QThread(parent)
{
}

void playerThread::run()
{
    while(1)
    {
        qDebug() << "emit signal.....";
        emit emitMsgBoxSignal();
        Sleep(1000);
    }
}
