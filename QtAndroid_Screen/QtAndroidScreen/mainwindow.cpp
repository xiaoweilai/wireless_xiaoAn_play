#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <QHostAddress>
#include <QHostAddress>
#include <QProcess>
#include <QNetworkInterface>
#include <QMessageBox>
#include "qDebug2Logcat.h"

#if 1
//编码汉字
//#define str_china(A)     QString::fromLocal8Bit(#A)
#define str_china(A)     QString::fromLocal8Bit(A)
//#define str_china(A)     QString::fromUtf8(#A)
#else
#define str_china(A)     codec->toUnicode(#A)
#endif

#define DEFAULT_PORT   "16689"


#define T1M (1*1024*1024)
#define T1K (1*1024)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    TotalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;
    bytesNeedRecv = 0;
    OnlyOneClient = FLAGS_NONE;


//    showifconfig();
//    installLogcatMessageHandler("ScreenCapture");
    if(Getifconfig().contains("192.168"))
    {
        ui->label->setText(ui->label->text() + Getifconfig());
    }
    else
    {
        QString str = QString::fromLocal8Bit("未获取正常的IP地址，请检查后查看。");
        ui->label->setText(str);
        return;
    }
    qDebug() << "bind current ip addr:" << Getifconfig();

//    start();
//    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(start()));
//    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stop()));

//    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(&tcpServer, SIGNAL(newConnection()),
            this, SLOT(acceptConnection()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//qt实现类似于ifconfig -a功能
void MainWindow::showifconfig(void)
{
    QStringList envVariables;
    QByteArray username;
    QList<QHostAddress> broadcastAddresses;
    QList<QHostAddress> ipAddresses;

    envVariables << "USERNAME.*" <<"USER.*" <<"USERDOMAIN.*"
                 <<"HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();
    foreach (QString string, envVariables) {
        int index = environment.indexOf(QRegExp(string));
        if(-1 != index){
            QStringList stringList = environment.at(index).split("=");
            if(stringList.size() == 2){
                username = stringList.at(1).toUtf8();
                qDebug() << username.data();
                break;
            }
        }
    }

    broadcastAddresses.clear();
    ipAddresses.clear();
    qDebug() << "Interface numbers:"
             <<QNetworkInterface::allInterfaces().count();

    foreach (QNetworkInterface interface,
             QNetworkInterface::allInterfaces()) {
        qDebug() << "Interface name:" << interface.name() <<endl
                 <<"Interface hardwareAddress:"
                <<interface.hardwareAddress()<<endl
               <<"entry numbers:" << interface.addressEntries().count();
        foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
            QHostAddress broadcastAddress = entry.broadcast();
            qDebug() << "entry ip:" << entry.ip()
                     <<"entry netmask:" <<entry.netmask();
            if(broadcastAddress != QHostAddress::Null){
                broadcastAddresses << broadcastAddress;
                ipAddresses << entry.ip();
            }
        }

    }
}


//qt实现类似于ifconfig -a功能
QString MainWindow::Getifconfig(void)
{
    QStringList envVariables;
    QByteArray username;
    QList<QHostAddress> broadcastAddresses;
    QList<QHostAddress> ipAddresses;
    QString ipAddressesstr;

    envVariables << "USERNAME.*" <<"USER.*" <<"USERDOMAIN.*"
                 <<"HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();
    foreach (QString string, envVariables) {
        int index = environment.indexOf(QRegExp(string));
        if(-1 != index){
            QStringList stringList = environment.at(index).split("=");
            if(stringList.size() == 2){
                username = stringList.at(1).toUtf8();
                qDebug() << username.data();
                break;
            }
        }
    }

    broadcastAddresses.clear();
    ipAddresses.clear();
    qDebug() << "Interface numbers:"
             <<QNetworkInterface::allInterfaces().count();

    foreach (QNetworkInterface interface,
             QNetworkInterface::allInterfaces()) {
        qDebug() << "Interface name:" << interface.name() <<endl
                 <<"Interface hardwareAddress:"
                <<interface.hardwareAddress()<<endl
               <<"entry numbers:" << interface.addressEntries().count();
        foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
            QHostAddress broadcastAddress = entry.broadcast();
            qDebug() << "entry ip:" << entry.ip()
                     <<"entry netmask:" <<entry.netmask();
            qDebug() << "ip addr:" << entry.ip().toString();
            ipAddressesstr = entry.ip().toString();
            if(ipAddressesstr.contains("192.168"))
            {
                return ipAddressesstr;
            }
            if(broadcastAddress != QHostAddress::Null){
                broadcastAddresses << broadcastAddress;
                ipAddresses << entry.ip();
            }
        }

    }
}


void MainWindow::start()
{
    ui->startButton->setEnabled(false);


    QApplication::setOverrideCursor(Qt::WaitCursor);
    bytesReceived = 0;
    QHostAddress hostaddr;
    hostaddr.setAddress(bindIpAddr());

    while (!tcpServer.isListening() &&
           //           !tcpServer.listen(QHostAddress::LocalHost,16689)) {
           //           !tcpServer.listen(bindIpAddr(),16689)) {
           !tcpServer.listen(hostaddr, QString(DEFAULT_PORT).toInt())) {

        QMessageBox::StandardButton ret = QMessageBox::critical(this,
                                                                str_china("回环"),
                                                                str_china("无法开始测试: %1.")
                                                                .arg(tcpServer.errorString()),
                                                                QMessageBox::Retry
                                                                | QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel)
            return;
    }
    //ui->serverStatusLabel->setText(str_china("监听"));
    ui->label->setText(str_china("监听"));
    setOnlyOneClient();
}

void MainWindow::on_startButton_clicked()
{
    qDebug() << "on start button clicked!!!";
    Getifconfig();
    qDebug() << "starting listen!!" << endl;

    start();
}


void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    qDebug() <<"find Error!:" << socketError;


    tcpServerConnection->close();
    tcpServer.close();
//    ui->serverProgressBar->reset();
    //ui->serverStatusLabel->setText(str_china("服务端就绪"));
    ui->label->setText(str_china("服务端就绪"));
    ui->startButton->setEnabled(true);

    TotalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;



    QApplication::restoreOverrideCursor();

    QMessageBox::information(this, str_china("网络错误"),
                             str_china("产生如下错误: %1.")
                             .arg(tcpServerConnection->errorString()));
    start();
}

QString MainWindow::bindIpAddr()
{
    return Getifconfig();
}

quint8 MainWindow::getOnlyOneClient()
{
    return OnlyOneClient;
}

void MainWindow::setOnlyOneClient()
{
    OnlyOneClient = FLAGS_HAVE;
}


void MainWindow::acceptConnection()
{
    qDebug() <<"creating one new connection....";

    tcpServerConnection = tcpServer.nextPendingConnection();
    connect(tcpServerConnection, SIGNAL(readyRead()),
            this, SLOT(updateServerProgress()));
    connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

//    ui->serverStatusLabel->setText(str_china("接受连接"));
    ui->label->setText(str_china("接受连接"));
    tcpServer.close();
    setOnlyOneClient();
}

void MainWindow::stop()
{
//    tcpServer.close();
//    tcpServerConnection->close();
//    ui->serverProgressBar->reset();
//    ui->serverStatusLabel->setText(str_china("服务端就绪"));
//    ui->startButton->setEnabled(true);

    TotalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;

    qDebug() <<"stop!!!!!";

    QApplication::restoreOverrideCursor();
    ui->startButton->setEnabled(true);
}



void MainWindow::updateServerProgress()
{
#if 1
    QDataStream in(tcpServerConnection);
    in.setVersion(QDataStream::Qt_4_0);
    //    QTime cTime;
    //    cTime.start();
    //    static quint64 etime = 0;
////0xff 0xfe 0xfd 0xfc
//#define HEADSIZE  (4)
//    quint8 headbuf[HEADSIZE];

//    if(bytesReceived <= HEADSIZE)
//    {
//        if(tcpServerConnection->bytesAvailable() >= HEADSIZE){
//            in>>TotalBytes>>fileNameSize;
//            bytesReceived += sizeof(qint64)*2;
//            inBlock.resize(0);
//        }
//    }

    if(bytesReceived <= sizeof(qint64)*2){
        if((tcpServerConnection->bytesAvailable() >= sizeof(qint64)*2)&&(fileNameSize ==0)){
            in>>TotalBytes>>fileNameSize;
            bytesReceived += sizeof(qint64)*2;
            inBlock.resize(0);
        }
        if((tcpServerConnection->bytesAvailable() >= fileNameSize)&&(fileNameSize !=0)){
            in>>fileName;
            qDebug() << "filename:" <<fileName;

            bytesReceived += fileNameSize;
            //            localFile = new QFile(fileName);
            //            if (!localFile->open(QFile::WriteOnly )) {
            //                QMessageBox::warning(this, str_china("应用程序"),
            //                                     str_china("无法读取文件 %1:\n%2.")
            //                                     .arg(fileName)
            //                                     .arg(localFile->errorString()));
            //                return;
            //            }
        }else{
            return;
        }
    }


    if (bytesReceived < TotalBytes){
#if 1
        /* 实际需要接收的字节数 */
        bytesNeedRecv = TotalBytes - bytesReceived;



        bytesReceived += tcpServerConnection->bytesAvailable();
        //        inBlock = tcpServerConnection->readAll();
        //        localFile->write(inBlock);
        if(bytesReceived >= TotalBytes){
//            tcpServerConnection->setReadBufferSize();
            inBlock.append(tcpServerConnection->read(bytesNeedRecv));
            bytesReceived = TotalBytes;
        }else{
            inBlock.append(tcpServerConnection->readAll());
        }


#else
        bytesReceived += tcpServerConnection->bytesAvailable();
        inBlock = tcpServerConnection->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
#endif
        qDebug() << "bytesReceived:"<< bytesReceived;
        qDebug() << "TotalBytes   :"<< TotalBytes;
    }
//    ui->serverProgressBar->setMaximum(TotalBytes);
//    ui->serverProgressBar->setValue(bytesReceived);
    //    qDebug()<<"bytesReceived---->>>>>>>>>"<< bytesReceived;
//    ui->serverStatusLabel->setText(str_china("已接收 %1MB")
//                                   .arg(bytesReceived / (1024 * 1024)));
    ui->label->setText(str_china("已接收 %1MB")
                                   .arg(bytesReceived / (1024 * 1024)));

    if (bytesReceived == TotalBytes) {
        qDebug() << "recived one !!!";
#if 0
        //        tcpServerConnection->close();
        //        ui->startButton->setEnabled(true);

        //        QByteArray array = socket.read( dataSize );
        QBuffer buffer(&inBlock);
        buffer.open( QIODevice::ReadOnly );

        QImageReader reader(&buffer, STREAM_PIC_FORT);
        QImage image = reader.read();

        if ( !image.isNull() ) {
            ui->imageLabel->setPixmap( QPixmap::fromImage( image ) );
            //            ui->imageLabel->setText( tr("") );
        }
        else {
            ui->imageLabel->setText( tr("<i>Invalid image received!</i>") );
        }
#endif

        QApplication::restoreOverrideCursor();

        inBlock.resize(0);

        //        localFile->close();
        TotalBytes = 0;
        bytesReceived = 0;
        fileNameSize = 0;

        bytesNeedRecv = 0;





        //        qDebug() << "consume time:" << etime;
        //        qDebug() << "transfer rate:" << (bytesReceived/T1M)/(etime/1000.00)
        //                 <<"MB/s";
        //        qDebug() << "transfer rate:" << (bytesReceived/T1K)/(etime/1000)
        //                 <<"KB/s";
    }
    //    etime += cTime.elapsed();
    //    qDebug() << "consume time:" << etime;

#else
    QDataStream in(tcpServerConnection);
    in.setVersion(QDataStream::Qt_4_3);
    QTime cTime;
    cTime.start();
    static quint64 etime = 0;


    if(bytesReceived <= sizeof(qint64)*2){
        if((tcpServerConnection->bytesAvailable() >= sizeof(qint64)*2)&&(fileNameSize ==0)){
            in>>TotalBytes>>fileNameSize;
            bytesReceived += sizeof(qint64)*2;
        }
        if((tcpServerConnection->bytesAvailable() >= fileNameSize)&&(fileNameSize !=0)){
            in>>fileName;
            qDebug() << "filename:" <<fileName;

            bytesReceived += fileNameSize;
            localFile = new QFile(fileName);
            if (!localFile->open(QFile::WriteOnly )) {
                QMessageBox::warning(this, str_china("应用程序"),
                                     str_china("无法读取文件 %1:\n%2.")
                                     .arg(fileName)
                                     .arg(localFile->errorString()));
                return;
            }
        }else{
            return;
        }
    }


    if (bytesReceived < TotalBytes){
        bytesReceived += tcpServerConnection->bytesAvailable();
        inBlock = tcpServerConnection->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }
    ui->serverProgressBar->setMaximum(TotalBytes);
    ui->serverProgressBar->setValue(bytesReceived);
    qDebug()<<bytesReceived;
    ui->serverStatusLabel->setText(str_china("已接收 %1MB")
                                   .arg(bytesReceived / (1024 * 1024)));

    if (bytesReceived == TotalBytes) {
        tcpServerConnection->close();
        ui->startButton->setEnabled(true);
        QApplication::restoreOverrideCursor();

        localFile->close();
        qDebug() << "consume time:" << etime;
        qDebug() << "transfer rate:" << (bytesReceived/T1M)/(etime/1000.00)
                 <<"MB/s";
        qDebug() << "transfer rate:" << (bytesReceived/T1K)/(etime/1000)
                 <<"KB/s";
    }
    etime += cTime.elapsed();
    qDebug() << "consume time:" << etime;
#endif
}
