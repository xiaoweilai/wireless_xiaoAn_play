#include "tcp_fileserver_recv.h"
#include "ui_tcp_fileserver_recv.h"
#include <QHostAddress>
#include <QProcess>
#include <QNetworkInterface>

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

//是否保存LOG信息到文件
#define SC_LOG
//网络发送数据的内容
#define SC_DATASTREAM_LOG


enum FLAGS_CLIENT
{
    FLAGS_NONE = 0,
    FLAGS_HAVE
};



Tcp_FileServer_Recv::Tcp_FileServer_Recv(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Tcp_FileServer_Recv)
{
    ui->setupUi(this);

    TotalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;
    bytesNeedRecv = 0;
    fileNameValue = 0;
    OnlyOneClient = FLAGS_NONE;

    pProcess = NULL;

    //删除test.mpg文件
    DelteMpgFile();
    LogInitLog();
    start();
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(start()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stop()));

    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(&tcpServer, SIGNAL(newConnection()),
            this, SLOT(acceptConnection()));
}

Tcp_FileServer_Recv::~Tcp_FileServer_Recv()
{
    delete ui;
}

void Tcp_FileServer_Recv::start()
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
    ui->serverStatusLabel->setText(str_china("监听"));
    setOnlyOneClient();
}

void Tcp_FileServer_Recv::acceptConnection()
{
    qDebug() <<"creating one new connection....";

    tcpServerConnection = tcpServer.nextPendingConnection();
    connect(tcpServerConnection, SIGNAL(readyRead()),
            this, SLOT(updateServerProgress()));
    connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    ui->serverStatusLabel->setText(str_china("接受连接"));
    tcpServer.close();
    setOnlyOneClient();
}

void Tcp_FileServer_Recv::stop()
{
    //    tcpServer.close();
    //    tcpServerConnection->close();
    //    ui->serverProgressBar->reset();
    //    ui->serverStatusLabel->setText(str_china("服务端就绪"));
    //    ui->startButton->setEnabled(true);

    TotalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;
    fileNameValue = 0;

    qDebug() <<"stop!!!!!";

    QApplication::restoreOverrideCursor();
    ui->startButton->setEnabled(true);
}

void Tcp_FileServer_Recv::updateServerProgress()
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

#define HEADERPKTSIZE sizeof(qint64)*3
    if(bytesReceived <= HEADERPKTSIZE){
        if((tcpServerConnection->bytesAvailable() >= HEADERPKTSIZE)&&(fileNameSize ==0)){
            in>>TotalBytes>>fileNameSize >> fileNameValue;
            bytesReceived += HEADERPKTSIZE;
            inBlock.resize(0);

            //debug
            qDebug() << "Header Info:-----------------------";
//            qDebug() << "TotalBytes :" << TotalBytes;
//            qDebug() << "fileNameSize" << fileNameSize;
//            qDebug() << "fileNameValue" << fileNameValue;
            LogWriteFile("Header Info:-----------------------\n");
            LogWriteFile("TotalBytes :"    + QString("%1").arg(TotalBytes) + "\n");
            LogWriteFile("fileNameValue :" + QString("%1").arg(fileNameSize) + "\n");
            LogWriteFile("fileNameValue :" + QString("%1").arg(fileNameValue) + "\n");
        }

#if 0 // 获取文件名
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
#endif
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
    ui->serverProgressBar->setMaximum(TotalBytes);
    ui->serverProgressBar->setValue(bytesReceived);
    //    qDebug()<<"bytesReceived---->>>>>>>>>"<< bytesReceived;
    ui->serverStatusLabel->setText(str_china("已接收 %1MB")
                                   .arg(bytesReceived / (1024 * 1024)));

    if (bytesReceived == TotalBytes) {
        //        tcpServerConnection->close();
        //        ui->startButton->setEnabled(true);

        LogWriteDataFile(inBlock);

        startPlayProcess();
#if 0
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
        fileNameValue = 0;

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

void Tcp_FileServer_Recv::displayError(QAbstractSocket::SocketError socketError)
{
    qDebug() <<"find Error!:" << socketError;


    tcpServerConnection->close();
    tcpServer.close();
    ui->serverProgressBar->reset();
    ui->serverStatusLabel->setText(str_china("服务端就绪"));
    ui->startButton->setEnabled(true);

    TotalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;
    fileNameValue = 0;



    QApplication::restoreOverrideCursor();

    QMessageBox::information(this, str_china("网络错误"),
                             str_china("产生如下错误: %1.")
                             .arg(tcpServerConnection->errorString()));
//    LogDeleteFile();
    LogInitLog();
    start();
}

QString Tcp_FileServer_Recv::bindIpAddr()
{
    //    return ui->lineEdit_ip->text();
    ui->lineEdit_ip->setText(Getifconfig());
    return Getifconfig();
}

quint8 Tcp_FileServer_Recv::getOnlyOneClient()
{
    return OnlyOneClient;
}

void Tcp_FileServer_Recv::setOnlyOneClient()
{
    OnlyOneClient = FLAGS_HAVE;
}


//qt实现类似于ifconfig -a功能
QString Tcp_FileServer_Recv::Getifconfig(void)
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



/************************************************/
/*函 数:LogInitLog                               */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:保存log信息                                */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void Tcp_FileServer_Recv::LogInitLog()
{
#ifdef SC_LOG  //将发送网络的数据有效信息进行保存
    QDate date;
    QTime time;
    logfilename = date.currentDate().toString("sclogyyyy-MM-dd");
    logfilename += time.currentTime().toString("_HH-mm-ss");
    logfilename +=".log";
    plogFile = new QFile(logfilename);
    if(!plogFile)
    {
        qDebug() <<"Open file Err:" << logfilename;
        return ;
    }


    LogWriteFile("\nTcpWrite NetData Log!!\n==============>>>>Starting:\n");
    qDebug() <<"Log file:" << logfilename;
#endif


#ifdef SC_DATASTREAM_LOG  //将发送数据的内容进行保存，以二进制形式
    datafilename = date.currentDate().toString("scDatyyyy-MM-dd");
    datafilename += time.currentTime().toString("_HH-mm-ss");
    datafilename +=".mpg";

    datafilename = "test.mpg";//test
    pdataFile = new QFile(datafilename);
    if(!pdataFile)
    {
        qDebug() <<"Open file Err:" << datafilename;
        return ;
    }
    LogWriteDataFile("FFFEFFFE");
    qDebug() <<"Log file:" << datafilename;

#endif

}


/************************************************/
/*函 数:LogWriteFile                              */
/*入 参:str-写入log的数据                           */
/*出 参:无                                        */
/*返 回:                                          */
/*功 能:将str写入文件                              */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void Tcp_FileServer_Recv::LogWriteFile(QString str)
{
#ifdef SC_LOG
    if(plogFile)
    {
        plogFile->open(QIODevice::Append);
        plogFile->write(str.toLocal8Bit());
        plogFile->close();
    }
#endif
}

/************************************************/
/*函 数:LogWriteDataFile                          */
/*入 参:data-写入log的数据                          */
/*出 参:无                                        */
/*返 回:                                          */
/*功 能:将data写入文件,比如二进制数据                */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void Tcp_FileServer_Recv::LogWriteDataFile(const QByteArray &data)
{
#ifdef SC_DATASTREAM_LOG
    if(pdataFile)
    {
        pdataFile->open(QIODevice::Append);
        pdataFile->write(data);
        pdataFile->close();
    }
#endif
}

void Tcp_FileServer_Recv::LogDeleteFile()
{
    qDebug() << "deletefile   :" << logfilename;
    qDebug() << "datafilename :" << datafilename;
#ifdef SC_LOG
    QFile logfile(logfilename);
    if(logfile.exists())
    {
        logfile.remove();
        remove(logfilename.toLocal8Bit().data());
    }

#endif

#ifdef SC_DATASTREAM_LOG  //将发送数据的内容进行保存，以二进制形式
    QFile datefile(datafilename);
    if(datefile.exists())
    {
        datefile.remove();
        remove(datafilename.toLocal8Bit().data());
    }
#endif


}

void Tcp_FileServer_Recv::startPlayProcess()
{
    static int waittime = 0;
    if(++waittime > 1)
    {
        if(!pProcess)
        {
            qDebug() << "start play process!!!!!!!!!!!!";
            const QString program = "./videosdlplay.exe";
            pProcess = new QProcess(this);
            pProcess->start(program);
        }
    }

}

void Tcp_FileServer_Recv::DelteMpgFile()
{
    QString mpgfilename = "./test.mpg";
    remove(mpgfilename.toLocal8Bit().data());
    unlink(mpgfilename.toLocal8Bit().data());
}
