#include "playerthread.h"
#include <windows.h>
#include <QDebug>

playerThread::playerThread(QObject *parent) :
    QThread(parent)
{
    nums = 0;
}

void playerThread::run()
{
    while(1)
    {
//        qDebug() << "emit signal.....";
        qDebug() << "current nums:" << nums;
        emit emitMsgBoxSignal();
        Sleep(100);
    }
}

void playerThread::dealNums(qint64 ulnums)
{
    nums = ulnums;
}
