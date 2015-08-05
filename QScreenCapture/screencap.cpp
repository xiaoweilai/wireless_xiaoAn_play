/************************************************/
/*声 明:  @db.com                                */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*功 能:用于捕屏的初始化，捕屏进程创建，网络传输         */
/*author :wxj                                    */
/*email  :wxjlmr@126.com                         */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
/*更新记录:                                        */
/*                                                */
/*************************************************/
#include "screencap.h"
#include "ui_screencap.h"
#include "version.h"
#include <QtDebug>
#include "capthread.h"
#include <QtGui>
#include <QDesktopWidget>
#include <windows.h>
#include <QTime>
#include "capthread.h"

#define NO_FIRST 1

int ScreenCap::isStarted = STAT_STOPPED;
unsigned int ScreenCap::mNo = NO_FIRST;

/************************************************/
/*函 数:ScreenCap                                */
/*入 参:parent-父类                               */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:界面、变量、定时器初始化、版本显示              */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
ScreenCap::ScreenCap(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenCap)
{
    ui->setupUi(this);
    ui->lineEditIp->setStyleSheet("QLineEdit{font: bold italic large \"Times New Roman\";font-size:25px;color:rgb(55,100,255);height:50px;border:4px solid rgb(155,200,33);background-color: rgba(0,0,0,30);border-radius:15px;selection-color:pink}");
    showAppVerion();
    BtnStartPix();
    connect(ui->lineEditIp,SIGNAL(textChanged(QString)),this,SLOT(LineTextTips(QString)));

    pCapThread = NULL;
    TotalBytes = 0;
    byteWritten = 0;
    bytesToWrite = 0;
    picNametime = 1;
    loadSize = 4*1024;     // 4Kb

    sendDoneFlag = SEND_DONE;

    //定时网络发送
    TimerSets();
    //保存log信息初始化
    LogInitLog();
}

/************************************************/
/*函 数:~ScreenCap                               */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:析构函数                                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
ScreenCap::~ScreenCap()
{
    if((NULL != pCapThread)&&(STAT_STARTED == ScreenCap::isStarted))
    {
        qDebug() << "delete pCapThread!!!";
        pCapThread->sendSDLQuit();
        pCapThread->terminate();
        pCapThread->quit();
        pCapThread->deleteLater();
        delete pCapThread;
        pCapThread = NULL;
    }
    delete ui;
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
void ScreenCap::LogInitLog()
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
void ScreenCap::LogWriteFile(QString str)
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
void ScreenCap::LogWriteDataFile(const QByteArray &data)
{
#ifdef SC_LOG
    if(pdataFile)
    {
        pdataFile->open(QIODevice::Append);
        pdataFile->write(data);
        pdataFile->close();
    }
#endif
}

/************************************************/
/*函 数:LogWriteFileComWithName                   */
/*入 参:filename-写入文件的名称，data-写入的数据      */
/*出 参:无                                        */
/*返 回:                                          */
/*功 能:将str写入文件                              */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::LogWriteFileComWithName(QString filename,QByteArray data)
{
#ifdef SC_LOG_DATA_WITHNAME
    QFile file(filename);

    file.open(QIODevice::Append);
    file.write(data);
    file.close();
#endif
}

/************************************************/
/*函 数:WithNetworkInit                          */
/*入 参:ipaddr-IP地址                             */
/*出 参:无                                        */
/*返 回:RET_SUCESS-成功，RET_FAIL-失败              */
/*功 能:网络传输初始化，检测IP地址，连接远程IP地址       */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
int ScreenCap::WithNetworkInit(QString ipaddr)
{
    if(RET_SUCESS != CheckIPAddrValid(ipaddr))
    {
        return RET_FAIL;
    }
    //    p_tcpClient = NULL;//tcp socket
    TotalBytes = 0;

    //创建tcpsocket
    p_tcpClient = new QTcpSocket;
    if(!p_tcpClient)
        return RET_FAIL;

    connect(p_tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(displayNetErr(QAbstractSocket::SocketError)));
    //    connect(p_tcpClient,SIGNAL(bytesWritten(qint64)),this,
    //            SLOT(updateClientProgress(qint64)));
    p_tcpClient->abort();
    p_tcpClient->connectToHost(ipaddr,
                               QString(DEFAULT_PORT).toInt());
    p_tcpClient->setSocketOption(QAbstractSocket::LowDelayOption, 0);//优化为最低延迟，后面的1代码启用该优化。
    //    p_tcpClient->setSocketOption(QAbstractSocket::KeepAliveOption,1);
    //    int send_len = 16 * 1024;
    //    if(setsockopt( fd, SOL_SOCKET, SO_SNDBUF, (void*)&send_len, sizeof(send_len) ) < 0 ){
    //        return RET_FAIL;
    //    }

    //waitForConnected()等待连接知道超过最大等待时间。如果连接建立函数返回true；否则返回false。
    //当返回false时可以调用error来确定无法连接的原因
    if(!p_tcpClient->waitForConnected(3000))
    {
        qDebug() <<"here:" << p_tcpClient;
        if(NULL != p_tcpClient)
        {
            qDebug() <<"Error: "<<p_tcpClient->errorString();
            p_tcpClient->deleteLater();
            p_tcpClient = NULL;

            QMessageBox::information(NULL, str_china("网络"), str_china("连接失败，请检查网络连接！"),NULL,NULL);
            return RET_FAIL;
        }
    }

    return RET_SUCESS;
}

/************************************************/
/*函 数:getSockState                              */
/*入 参:state-socket各种状态                       */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:网络各种状态信息                            */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
QString ScreenCap::getSockState(QAbstractSocket::SocketState state)
{

    switch(state)
    {
    case QAbstractSocket::UnconnectedState:
        return "UnconnectedState";
    case QAbstractSocket::HostLookupState:
        return "HostLookupState";
    case QAbstractSocket::ConnectingState:
        return "ConnectingState";
    case QAbstractSocket::ConnectedState:
        return "ConnectedState";
    case QAbstractSocket::BoundState:
        return "ConnectedState";
    case QAbstractSocket::ListeningState:
        return "ListeningState";
    case QAbstractSocket::ClosingState:
        return "ClosingState";
    default:
        return "unknown err";
    }

    return "unknown err";
}

/************************************************/
/*函 数:displayNetErr                            */
/*入 参:socketError-socket各种错误                 */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:网络各种错误情况处理                         */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::displayNetErr(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Display err!";
    LogWriteFile("Display err!");
    if(p_tcpClient)
    {
        QString timeInfo = QString::fromLocal8Bit("\n-->>N:%1,T:%2%3\n").arg(picNametime).arg(QDate::currentDate().toString("yyyyMMdd")).arg(QTime::currentTime().toString("HHmmss"));
        LogWriteFile(timeInfo);

        //qDebug("State      :%d",p_tcpClient->state());
        qDebug("State      :%s",getSockState(p_tcpClient->state()).toLocal8Bit().data());  // State: 3（ConnectedState）正确
        qDebug("socketError:%s",p_tcpClient->errorString().toLocal8Bit().data());  // socketError:
        qDebug("socketError:%d",socketError);

        LogWriteFile(QString::fromLocal8Bit("State:%1").arg(getSockState(p_tcpClient->state())));
        LogWriteFile(QString::fromLocal8Bit("socketError:%1").arg(p_tcpClient->errorString()));
        LogWriteFile(QString::fromLocal8Bit("socketError:%1").arg(socketError));
    }
    //写log



    //未连接出现的死程序问题
    if((STAT_STOPPED == isStarted) || (STAT_RESTOP == isStarted))
    {
        return;
    }

#if 0
    //服务端被拒绝，停止传输
    //客户端无法写数据
    /*
    State      :ConnectedState
    socketError:Unable to write
    socketError:7 */
    if((QAbstractSocket::ConnectionRefusedError == socketError)
            ||(QAbstractSocket::RemoteHostClosedError == socketError)
            ||(QAbstractSocket::DatagramTooLargeError == socketError))
    {
#endif
        //异常，直接关闭传输
        emitCtrlPthreadStop();//发送停止抓屏蔽和传输,抓屏程序在休息
        //        Sleep(1000);
        pNetSendTimer->stop();
        pCapThread->arrayNetData.clear();
        pCapThread->arrayNetSize.clear();

        p_tcpClient->close();


        isStarted == STAT_RESTOP;
        qDebug() << " isStarted:" << isStarted;
        StopActionSets();
        mNo = NO_FIRST;
#if 0
    }
#endif

    isStarted = STAT_RESTOP;
    qDebug() << " isStarted:" << isStarted;

    if(NULL == p_tcpClient)
    {
        QMessageBox::information(NULL,str_china("网络"),
                                 str_china("连接失败，请检查网络连接！"),NULL,NULL);
        LogWriteFile(QString::fromLocal8Bit("连接失败，请检查网络连接！"));
    }else{
#if 0
        QMessageBox::information(NULL,str_china("网络"),
                                 str_china("产生如下错误： %1")
                                 .arg(p_tcpClient->errorString()),NULL,NULL);
#else
        QMessageBox::information(NULL,str_china("网络"),
                                 str_china("网络中断，请检查网络连接！"),NULL,NULL);
        LogWriteFile(QString::fromLocal8Bit("网络中断，请检查网络连接！"));
#endif
    }

    BtnStartPix();//显示开始图片
    showTextClickOverToReStart();//显示点击传输

}

/************************************************/
/*函 数:writeNetData                             */
/*入 参:iData-待传输的字节                         */
/*出 参:无                                        */
/*返 回:传递成功的数据字节长度                       */
/*功 能:传递网络数据                               */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
qint64 ScreenCap::writeNetData(const QByteArray &iData,WriteDataFlag flag)
{
    static qint64 count = 0;
    qint64 len = p_tcpClient->write(iData);
    //    bool res = p_tcpClient->waitForBytesWritten();
    //    qDebug("res:%d\n",res);
    qDebug("State:%d\n",p_tcpClient->state());  // State: 3（ConnectedState）正确

    //    qDebug() << "To writeData len:" << iData.size();
    //    qDebug() << "writtenData len:" << len;
    QString idateInfo = QString::fromLocal8Bit("%2, To writeData len:%1, \n").arg(iData.size()).arg((count)%2);
    QString writtendateinfo = QString::fromLocal8Bit("%2, writtenData  len:%1\n").arg(len).arg((count++)%2);

#ifdef SC_LOG
    if(iData.size() != len)
    {
        LogWriteFile("!!!!!!!!!!!!!!!!!!ERROR LEN\n");
        LogWriteFile("!!!!!!!!!!!!!!!!!!ERROR LEN\n");
    }else{
        LogWriteFile(idateInfo);
        LogWriteFile(writtendateinfo);
    }
#endif

#ifdef SC_DATASTREAM_LOG
    if(flag == WRITE_BODY)   /* 只记录body */
    {
        LogWriteDataFile(iData);
    }
#endif

    if(len != iData.size())
    {
        qDebug() << "Data not same!!";
        qDebug() << "To writeData len:" << iData.size();
        qDebug() << "writtenData len:" << len;
    }
    return(len);
}

/************************************************/
/*函 数:MergeSendMessage                          */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:组合发送的报文，包括数据总长度，头长度，头内容和数据内容 */
/*     同时将头内容发送出去                          */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::MergeSendMessage()
{

    /************************************************************
*                数据流传送格式
*
*  宽度 ---|   8    |   8       |   8   |   n   |.....
*  含义 ---|  总长度 |  头长度(8) | 头内容 | 数据内容|.....
*  总长度 = |<----------------------------->|.....
*  总长度 = 8 + 8 + 8 + n
*  头内容 ： 0xFFFEFFFE;//值
*  头长度 ： 8
************************************************************/
    TotalBytes = pCapThread->arrayNetSize.at(0); //数据大小
    //    qDebug() << "net size:" << TotalBytes;

    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_3);

    qint64 headerFlag = 0xFFFEFFFE;//值
    sendOut << qint64(0) << qint64(0)  << headerFlag;
    //    qDebug() << "outBlock size:" << outBlock.size();

    TotalBytes +=  outBlock.size();//总大小 = 数据 + 8字节(存在总大小字节数) + 8"头表达式"
    //    qDebug() << "TotalBytes size:" << TotalBytes;
    sendOut.device()->seek(0);
    //    qDebug() << "header size:" << qint64((outBlock.size() - sizeof(qint64) * 2));
    //填写实际的总长度和头长度
    sendOut << TotalBytes << qint64((outBlock.size() - sizeof(qint64) * 2));

    QString HeaderInfo = QString::fromLocal8Bit("\n-->>N:%1,T:%2%3\n").arg(picNametime).arg(QDate::currentDate().toString("yyyyMMdd")).arg(QTime::currentTime().toString("HHmmss"));
    LogWriteFile(HeaderInfo);
    QString TotalBytesInfo = QString::fromLocal8Bit("TotalBytes:%1\n").arg(TotalBytes);
    LogWriteFile(TotalBytesInfo);

    //将头发送出去，并计算剩余的数据长度，即数据内容长度(n)
    qint64 len = writeNetData(outBlock, WRITE_HEAD);


    if(len != outBlock.size())
    {
        qDebug() << "Header not same!!";
        qDebug() << "to write Header len:" << outBlock.size();
        qDebug() << "written Header len:" << len;
    }
    bytesToWrite = TotalBytes - len;
    outBlock.resize(0);
    //    qDebug() << "-->TotalBytes size:" << TotalBytes;
    //    qDebug() << "-->bytesToWrite size:" << bytesToWrite;

    outBlkData = pCapThread->arrayNetData.at(0);

    writeNetData(outBlkData, WRITE_BODY);

    qDebug() << "-->outBlkData size :" << outBlkData.size();

    picNametime++;//发送次数
}


/************************************************/
/*函 数:InfoRecvMessage                           */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:接收报文的信息                              */
/*                                               */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::InfoRecvMessage()
{

    /************************************************************
*                数据流接收格式
*
************************************************************/

    //    QString TotalBytesInfo = QString::fromLocal8Bit("TotalBytes:%1\n").arg(TotalBytes);
    //    LogWriteFile(TotalBytesInfo);
#define COMLENGTH 0
    //接收到的数据的长度
    qint64 len = p_tcpClient->bytesAvailable();
    qDebug() << "---->>Recv Stream len:" << len;
    if(len > COMLENGTH)
    {
        //        qDebug() << "Recv Stream len > 100:" << len;
        QByteArray byteArray = p_tcpClient->read(p_tcpClient->bytesAvailable());
        LogWriteFileComWithName(SC_LOG_RECVDATA_FILENAME, byteArray);
        //        qDebug() << "bytecontent:" <<byteArray;
        byteArray.clear();
    }




    //    if(len != outBlock.size())
    //    {
    //        qDebug() << "Header not same!!";
    //        qDebug() << "to write Header len:" << outBlock.size();
    //        qDebug() << "written Header len:" << len;
    //    }
    //    bytesToWrite = TotalBytes - len;
    //    outBlock.resize(0);
    //    //    qDebug() << "-->TotalBytes size:" << TotalBytes;
    //    //    qDebug() << "-->bytesToWrite size:" << bytesToWrite;

    //    outBlkData = pCapThread->arrayNetData.at(0);

    //    writeNetData(outBlkData);

    //    qDebug() << "-->outBlkData size :" << outBlkData.size();

    //    picNametime++;//发送次数
}


/************************************************/
/*函 数:updateClientProgress                     */
/*入 参:numBytes-传递的字节数                      */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:更新客户侧传递的进度条                        */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::updateClientProgress(qint64 numBytes)
{
    //#ifdef DEBUG
    //    qDebug() << "numBytes:--------->>"<<numBytes;
    //#endif
    //    byteWritten += (int)numBytes;
    //#ifdef DEBUG
    //    qDebug() << "byteWritten:" << byteWritten;
    //    qDebug() << "bytesToWrite:" << bytesToWrite;
    //#endif
    ////    if (bytesToWrite > 0) {
    ////        //        outBlock = buffer.read(qMin(bytesToWrite, loadSize));
    //////        outBlock = buffer.read(qMin(bytesToWrite, loadSize));
    //////        outBlkData = buffer.data();

    ////        qDebug() << "outBlkData size:--------->>"<<outBlkData.size();
    ////        qDebug() << "bytesToWrite size:--------->>"<<bytesToWrite;
    ////        if(bytesToWrite == qMin(bytesToWrite, loadSize))
    ////        {
    ////            qDebug() << "!!write size:--------->>"<<bytesToWrite;
    ////            bytesToWrite -= (int)p_tcpClient->write(outBlkData,bytesToWrite);
    ////        }else{
    ////            qDebug() << "write size:--------->>"<<loadSize;
    ////            bytesToWrite -= (int)p_tcpClient->write(outBlkData,loadSize);
    ////        }

    ////    }
    ////    else{
    ////#ifdef DEBUG
    ////        qDebug() << "-->: send image done!!";
    ////#endif
    ////        sendDoneFlag = SEND_DONE;
    ////        picNametime++;
    ////        TotalBytes = 0;
    ////        byteWritten = 0;
    ////        bytesToWrite = 0;
    ////        if(pCapThread->arrayNetData.count() > 0)
    ////        {
    ////            pCapThread->arrayNetData.removeAt(0);
    ////            pCapThread->arrayNetSize.removeAt(0);
    ////        }
    ////        buffer.close();
    ////        outBlkData.resize(0);
    ////    }


    //#if 0

    //#ifdef DEBUG
    //    qDebug() << "numBytes:--------->>"<<numBytes;
    //#endif
    //    byteWritten += (int)numBytes;
    //    if(bytesToWrite > 0)
    //    {
    //#ifdef DEBUG
    //        //        qDebug() <<"-->:outBlockFile size:" << outBlockFile.size();
    //#endif

    //        bytesToWrite -= (int)p_tcpClient->write(outBlockFile);
    //#ifdef DEBUG
    //        qDebug() <<"-->:bytesToWrite size:" << bytesToWrite;
    //#endif
    //    }
    //    else
    //    {
    //#ifdef DEBUG
    //        qDebug() << "-->: send image done!!";
    //#endif
    //        picNametime++;
    //        TotalBytes = 0;
    //        byteWritten = 0;
    //        //        sendDoneFlag = SEND_DONE;
    //    }
    //#endif
}

/************************************************/
/*函 数:CreateTcpSocket                          */
/*入 参:                                         */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:建立网络套接字                              */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
int ScreenCap::CreateTcpSocket()
{
    QString textIp = ui->lineEditIp->text();

    if(RET_SUCESS != WithNetworkInit(textIp))
    {
        qDebug("CapThread create RET_FAIL!!\n");
        return RET_FAIL;
    }
    return RET_SUCESS;
}


/************************************************/
/*函 数:CreateCapturethread                      */
/*入 参:numBytes-传递的字节数                      */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:创建捕屏线程，默认捕捉全屏，发送信号控制是否捕屏  */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
int ScreenCap::CreateCapturethread()
{
    int ret = RET_SUCESS;
    int w = QApplication::desktop()->width();
    int h = QApplication::desktop()->height();
    qDebug("screen rect,w:%d h:%d\n", w, h);

    if(RET_SUCESS != CreateTcpSocket())
    {
        qDebug("create tcp socket faild!!\n");
        return RET_FAIL;
    }

    pCapThread = new CapThread(w, h, NULL);
    qDebug() << "pCapThread start addr:" << pCapThread;
    //ctrl pthread  of capthread
    connect(this,SIGNAL(emitCtrlPthreadStart()),pCapThread,
            SLOT(SetStartThread()),Qt::QueuedConnection);
    connect(this,SIGNAL(emitCtrlPthreadStop()),pCapThread,
            SLOT(SetStopThread()),Qt::QueuedConnection);
    connect(this,SIGNAL(emitCtrlPthreadQuit()),pCapThread,
            SLOT(SetQuitThread()),Qt::QueuedConnection);

    return RET_SUCESS;
}

/************************************************/
/*函 数:TimerSets                                */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:定时器集，包括（发送网络数据）                 */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::TimerSets()
{
    pNetSendTimer = new QTimer();
    connect(pNetSendTimer,SIGNAL(timeout()),this,SLOT(NetSendData()));
    pNetSendTimer->start(40);
}

/************************************************/
/*函 数:NetSendData                              */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:网络数据发送槽函数                           */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::NetSendData()
{
    if(NULL == pCapThread)
    {
        return;
    }

    if(0 == pCapThread->arrayNetSize.count())
    {
        return;
    }

    if(0 == pCapThread->arrayNetData.count())
    {
        return;
    }

    if(SEND_DONE == sendDoneFlag)
    {
        sendDoneFlag = SEND_UNDO;
        MergeSendMessage();
        InfoRecvMessage();

        pCapThread->arrayNetData.removeAt(0);
        pCapThread->arrayNetSize.removeAt(0);
        //设置传输状态栏显示变化（传输中...）
        showTextTransferingNo(picNametime);

        sendDoneFlag = SEND_DONE;
    }
    //    qDebug() << "~~~arrayNetData count:" <<pCapThread->arrayNetData.count();
    int netsize = pCapThread->arrayNetSize.count();
    if(netsize)
    {
        qDebug() << "~~~arrayNetSize count:" <<pCapThread->arrayNetSize.count();
    }

}

/************************************************/
/*函 数:StopActionSets                           */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:停止传输处理函数集合                          */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::StopActionSets()
{
    qDebug() << " stop!!";
    BtnStartPix();
    BtnDisable();
    showTextTransferOver();

    //停止传输
    StopCapScreen();

    BtnEnable();
}

/************************************************/
/*函 数:on_pushButtonStart_clicked               */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:开始/停止按钮动作，                          */
/*     第一次点击时，默认进入停止按钮，设置为传输中...   */
/*     第二次点击时，进入开始按钮，设置为传输结束        */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::on_pushButtonStart_clicked()
{
    qDebug("here!\n");
    qDebug("mNo:%d",mNo);
    if(0 == ((mNo++)%2)) /* 传输结束 */
    {
        qDebug("here!1111111\n");
        StopActionSets();
    }
    else /* 开始传输 */
    {
        qDebug() << " To start!!";
        qDebug() << " isStarted:" << isStarted;
        BtnStopPix();//显示停止图片

        if(STAT_RESTOP == isStarted)//需要重新连接
        {
            showTextConnecting();
            if(RET_SUCESS != CreateTcpSocket())//重新建立socket
            {
                qDebug("create tcp socket faild!!\n");
                BtnStartPix();//显示开始图片
                showTextClickToStart();//显示点击传输
                mNo = NO_FIRST;//
                qDebug() << " No started!!";
                return;
            }
            qDebug("recreate tcp socket ok!\n");
            emit emitCtrlPthreadStart();
            pNetSendTimer->start();//发送数据定时器



            showTextTransfering();//连接成功
            qDebug() << " restarting!!";

            isStarted = STAT_RESTART;
            return;
        }

        //开始传输
        if(STAT_STOPPED == CaptureScreenOn())//建立连接
        {
            BtnStartPix();//显示开始图片
            showTextClickToStart();//显示点击传输
            mNo = NO_FIRST;//
            qDebug() << " No started!!";
        }
        else
        {
            showTextTransfering();//连接成功
            qDebug() << " starting!!";
        }

    }
}

/************************************************/
/*函 数:BtnStartPix                              */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:开始图片显示及样式                           */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnStartPix(void)
{
    QString str = ":images/start.png";
    BtnSetPix(str);
}

/************************************************/
/*函 数:BtnStopPix                               */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:停止图片显示及样式                           */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnStopPix(void)
{
    QString str = ":images/stop.png";
    BtnSetPix(str);
}

/************************************************/
/*函 数:BtnSetPix                                */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:图片显示及样式                              */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnSetPix(QString str)
{
#define PNGSIZEDIV 1
    QPixmap mypixmap;
    mypixmap.load(str);
    ui->pushButtonStart->setIcon(mypixmap);
    ui->pushButtonStart->setIconSize(QSize(mypixmap.width()/PNGSIZEDIV,mypixmap.height()/PNGSIZEDIV));
}

/************************************************/
/*函 数:BtnEnable                                */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:按钮使能                                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnEnable(void)
{
    ui->pushButtonStart->setEnabled(TRUE);
}

/************************************************/
/*函 数:BtnDisable                                */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:按钮禁止使能                                */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnDisable(void)
{
    ui->pushButtonStart->setEnabled(FALSE);
}

/************************************************/
/*函 数:CaptureScreenOn                          */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:STAT_STARTED-开始，STAT_STOPPED-停止       */
/*功 能:捕屏开始                                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
int ScreenCap::CaptureScreenOn()
{
    showTextConnecting();
    if(STAT_STOPPED == isStarted)
    {
        if(RET_SUCESS == CreateCapturethread())//建立连接并开始抓捕线程
        {
            if(pCapThread)
                pCapThread->start();
            isStarted = STAT_STARTED;
        }
        else
        {
            return STAT_STOPPED;
        }
    }
    else
    {
        emit emitCtrlPthreadStart();
    }
    qDebug() <<"isStarted:"<<isStarted;

    return STAT_STARTED;
}

/************************************************/
/*函 数:StopCapScreen                            */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:捕屏停止                                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::StopCapScreen()
{
    emit emitCtrlPthreadStop();
}

/************************************************/
/*函 数:LineTextTips                             */
/*入 参:str-lineText的文本内容                     */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:文本内容提示，当为空，显示文本显示              */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::LineTextTips(QString str)
{
    str = str.replace(" ","");
    if(str.isEmpty())
    {
        ui->label->setText(QString::fromLocal8Bit(" 请输入IP地址,并点击开始按钮进行传输"));
    }else{
        ui->label->setText(QString::fromLocal8Bit(""));
    }

}

/************************************************/
/*函 数:showStateBarInfo                         */
/*入 参:pstr-字符串                               */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示字符串信息                         */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showStateBarInfo(const char *pstr)
{
    ui->statusBar->showMessage(QString::fromLocal8Bit(pstr));
}

/************************************************/
/*函 数:showTextTransfering                      */
/*入 参:pstr-字符串                               */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示"传输中..."                        */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextTransfering()
{
    showStateBarInfo("传输中...");
}

/************************************************/
/*函 数:showTextTransferingNo                    */
/*入 参:pstr-字符串                               */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示"传输中...按个数"                  */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextTransferingNo(quint64 num)
{
#define CASENUMUNIT 80
    quint64 order = num % CASENUMUNIT;
    switch(order)
    {
    case 0:
        showStateBarInfo("传输中");
        break;
    case 20:
        showStateBarInfo("传输中.");
        break;
    case 40:
        showStateBarInfo("传输中..");
        break;
    case 60:
        showStateBarInfo("传输中...");
        break;
    default:
        //        showStateBarInfo("传输中......");
        break;
    }
}

/************************************************/
/*函 数:showTextTransferOver                     */
/*入 参:pstr-字符串                               */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示"传输结束"                         */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextTransferOver()
{
    showStateBarInfo("传输结束");
}

/************************************************/
/*函 数:CheckIPAddrValid                         */
/*入 参:ipaddr-IP地址                             */
/*出 参:无                                        */
/*返 回:RET_SUCESS-成功，RET_FAIL-失败             */
/*功 能:检测IP地址有效性                            */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
int ScreenCap::CheckIPAddrValid(QString ipaddr)
{
    QRegExp regExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if(!regExp.exactMatch(ipaddr))
    {
        QMessageBox::warning(NULL, str_china("提示"), str_china("ip地址错误"),NULL,NULL);
        return RET_FAIL;
    }

    return RET_SUCESS;
}

/************************************************/
/*函 数:showAppVerion                            */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示程序版本                           */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showAppVerion(void)
{
    QString verinfo = QString::fromLocal8Bit("录屏传输 ") +
            QString::fromLocal8Bit(ScreenCapVersion) +
            QString::fromLocal8Bit("，点击开始传输 ");
    ui->statusBar->showMessage(verinfo);
}

/************************************************/
/*函 数:showTextClickOverToReStart                */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示  "传输中断，点击重新开始传输 "      */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextClickOverToReStart(void)
{
    QString verinfo = QString::fromLocal8Bit("传输中断，点击重新开始传输 ");
    ui->statusBar->showMessage(verinfo);
}


/************************************************/
/*函 数:showTextClickToStart                     */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示  "点击开始传输 "                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextClickToStart(void)
{
    QString verinfo = QString::fromLocal8Bit("点击开始传输 ");
    ui->statusBar->showMessage(verinfo);
}

/************************************************/
/*函 数:showTextConnecting                       */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示"连接中...，请等待 "                */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextConnecting(void)
{
    QString verinfo = QString::fromLocal8Bit("连接中...，请等待 ");
    ui->statusBar->showMessage(verinfo);
}



