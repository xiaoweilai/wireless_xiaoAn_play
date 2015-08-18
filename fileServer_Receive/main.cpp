#include <QtGui/QApplication>
#include "tcp_fileserver_recv.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::addLibraryPath("plugins");
//    QTextCodec::setCodecForTr( QTextCodec::codecForName("gb18030"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
    Tcp_FileServer_Recv w;
    w.show();

//    qDebug() << "sizeof(qint64)" << sizeof(qint64);
    
    return a.exec();
}
