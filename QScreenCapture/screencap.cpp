/************************************************/
/*�� ��:  @db.com                                */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*�� ��:���ڲ����ĳ�ʼ�����������̴��������紫��         */
/*author :wxj                                    */
/*email  :wxjlmr@126.com                         */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
/*���¼�¼:                                        */
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
/*�� ��:ScreenCap                                */
/*�� ��:parent-����                               */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:���桢��������ʱ����ʼ�����汾��ʾ              */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
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

    //��ʱ���緢��
    TimerSets();
    //����log��Ϣ��ʼ��
    LogInitLog();
}

/************************************************/
/*�� ��:~ScreenCap                               */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��������                                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
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
/*�� ��:LogInitLog                               */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:����log��Ϣ                                */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::LogInitLog()
{
#ifdef SC_LOG  //�����������������Ч��Ϣ���б���
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


#ifdef SC_DATASTREAM_LOG  //���������ݵ����ݽ��б��棬�Զ�������ʽ
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
/*�� ��:LogWriteFile                              */
/*�� ��:str-д��log������                           */
/*�� ��:��                                        */
/*�� ��:                                          */
/*�� ��:��strд���ļ�                              */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
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
/*�� ��:LogWriteDataFile                          */
/*�� ��:data-д��log������                          */
/*�� ��:��                                        */
/*�� ��:                                          */
/*�� ��:��dataд���ļ�,�������������                */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
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
/*�� ��:LogWriteFileComWithName                   */
/*�� ��:filename-д���ļ������ƣ�data-д�������      */
/*�� ��:��                                        */
/*�� ��:                                          */
/*�� ��:��strд���ļ�                              */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
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
/*�� ��:WithNetworkInit                          */
/*�� ��:ipaddr-IP��ַ                             */
/*�� ��:��                                        */
/*�� ��:RET_SUCESS-�ɹ���RET_FAIL-ʧ��              */
/*�� ��:���紫���ʼ�������IP��ַ������Զ��IP��ַ       */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
int ScreenCap::WithNetworkInit(QString ipaddr)
{
    if(RET_SUCESS != CheckIPAddrValid(ipaddr))
    {
        return RET_FAIL;
    }
    //    p_tcpClient = NULL;//tcp socket
    TotalBytes = 0;

    //����tcpsocket
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
    p_tcpClient->setSocketOption(QAbstractSocket::LowDelayOption, 0);//�Ż�Ϊ����ӳ٣������1�������ø��Ż���
    //    p_tcpClient->setSocketOption(QAbstractSocket::KeepAliveOption,1);
    //    int send_len = 16 * 1024;
    //    if(setsockopt( fd, SOL_SOCKET, SO_SNDBUF, (void*)&send_len, sizeof(send_len) ) < 0 ){
    //        return RET_FAIL;
    //    }

    //waitForConnected()�ȴ�����֪���������ȴ�ʱ�䡣������ӽ�����������true�����򷵻�false��
    //������falseʱ���Ե���error��ȷ���޷����ӵ�ԭ��
    if(!p_tcpClient->waitForConnected(3000))
    {
        qDebug() <<"here:" << p_tcpClient;
        if(NULL != p_tcpClient)
        {
            qDebug() <<"Error: "<<p_tcpClient->errorString();
            p_tcpClient->deleteLater();
            p_tcpClient = NULL;

            QMessageBox::information(NULL, str_china("����"), str_china("����ʧ�ܣ������������ӣ�"),NULL,NULL);
            return RET_FAIL;
        }
    }

    return RET_SUCESS;
}

/************************************************/
/*�� ��:getSockState                              */
/*�� ��:state-socket����״̬                       */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:�������״̬��Ϣ                            */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
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
/*�� ��:displayNetErr                            */
/*�� ��:socketError-socket���ִ���                 */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:������ִ����������                         */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
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
        qDebug("State      :%s",getSockState(p_tcpClient->state()).toLocal8Bit().data());  // State: 3��ConnectedState����ȷ
        qDebug("socketError:%s",p_tcpClient->errorString().toLocal8Bit().data());  // socketError:
        qDebug("socketError:%d",socketError);

        LogWriteFile(QString::fromLocal8Bit("State:%1").arg(getSockState(p_tcpClient->state())));
        LogWriteFile(QString::fromLocal8Bit("socketError:%1").arg(p_tcpClient->errorString()));
        LogWriteFile(QString::fromLocal8Bit("socketError:%1").arg(socketError));
    }
    //дlog



    //δ���ӳ��ֵ�����������
    if((STAT_STOPPED == isStarted) || (STAT_RESTOP == isStarted))
    {
        return;
    }

#if 0
    //����˱��ܾ���ֹͣ����
    //�ͻ����޷�д����
    /*
    State      :ConnectedState
    socketError:Unable to write
    socketError:7 */
    if((QAbstractSocket::ConnectionRefusedError == socketError)
            ||(QAbstractSocket::RemoteHostClosedError == socketError)
            ||(QAbstractSocket::DatagramTooLargeError == socketError))
    {
#endif
        //�쳣��ֱ�ӹرմ���
        emitCtrlPthreadStop();//����ֹͣץ���κʹ���,ץ����������Ϣ
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
        QMessageBox::information(NULL,str_china("����"),
                                 str_china("����ʧ�ܣ������������ӣ�"),NULL,NULL);
        LogWriteFile(QString::fromLocal8Bit("����ʧ�ܣ������������ӣ�"));
    }else{
#if 0
        QMessageBox::information(NULL,str_china("����"),
                                 str_china("�������´��� %1")
                                 .arg(p_tcpClient->errorString()),NULL,NULL);
#else
        QMessageBox::information(NULL,str_china("����"),
                                 str_china("�����жϣ������������ӣ�"),NULL,NULL);
        LogWriteFile(QString::fromLocal8Bit("�����жϣ������������ӣ�"));
#endif
    }

    BtnStartPix();//��ʾ��ʼͼƬ
    showTextClickOverToReStart();//��ʾ�������

}

/************************************************/
/*�� ��:writeNetData                             */
/*�� ��:iData-��������ֽ�                         */
/*�� ��:��                                        */
/*�� ��:���ݳɹ��������ֽڳ���                       */
/*�� ��:������������                               */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
qint64 ScreenCap::writeNetData(const QByteArray &iData,WriteDataFlag flag)
{
    static qint64 count = 0;
    qint64 len = p_tcpClient->write(iData);
    //    bool res = p_tcpClient->waitForBytesWritten();
    //    qDebug("res:%d\n",res);
    qDebug("State:%d\n",p_tcpClient->state());  // State: 3��ConnectedState����ȷ

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
    if(flag == WRITE_BODY)   /* ֻ��¼body */
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
/*�� ��:MergeSendMessage                          */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��Ϸ��͵ı��ģ����������ܳ��ȣ�ͷ���ȣ�ͷ���ݺ��������� */
/*     ͬʱ��ͷ���ݷ��ͳ�ȥ                          */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::MergeSendMessage()
{

    /************************************************************
*                ���������͸�ʽ
*
*  ��� ---|   8    |   8       |   8   |   n   |.....
*  ���� ---|  �ܳ��� |  ͷ����(8) | ͷ���� | ��������|.....
*  �ܳ��� = |<----------------------------->|.....
*  �ܳ��� = 8 + 8 + 8 + n
*  ͷ���� �� 0xFFFEFFFE;//ֵ
*  ͷ���� �� 8
************************************************************/
    TotalBytes = pCapThread->arrayNetSize.at(0); //���ݴ�С
    //    qDebug() << "net size:" << TotalBytes;

    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_3);

    qint64 headerFlag = 0xFFFEFFFE;//ֵ
    sendOut << qint64(0) << qint64(0)  << headerFlag;
    //    qDebug() << "outBlock size:" << outBlock.size();

    TotalBytes +=  outBlock.size();//�ܴ�С = ���� + 8�ֽ�(�����ܴ�С�ֽ���) + 8"ͷ���ʽ"
    //    qDebug() << "TotalBytes size:" << TotalBytes;
    sendOut.device()->seek(0);
    //    qDebug() << "header size:" << qint64((outBlock.size() - sizeof(qint64) * 2));
    //��дʵ�ʵ��ܳ��Ⱥ�ͷ����
    sendOut << TotalBytes << qint64((outBlock.size() - sizeof(qint64) * 2));

    QString HeaderInfo = QString::fromLocal8Bit("\n-->>N:%1,T:%2%3\n").arg(picNametime).arg(QDate::currentDate().toString("yyyyMMdd")).arg(QTime::currentTime().toString("HHmmss"));
    LogWriteFile(HeaderInfo);
    QString TotalBytesInfo = QString::fromLocal8Bit("TotalBytes:%1\n").arg(TotalBytes);
    LogWriteFile(TotalBytesInfo);

    //��ͷ���ͳ�ȥ��������ʣ������ݳ��ȣ����������ݳ���(n)
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

    picNametime++;//���ʹ���
}


/************************************************/
/*�� ��:InfoRecvMessage                           */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:���ձ��ĵ���Ϣ                              */
/*                                               */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::InfoRecvMessage()
{

    /************************************************************
*                ���������ո�ʽ
*
************************************************************/

    //    QString TotalBytesInfo = QString::fromLocal8Bit("TotalBytes:%1\n").arg(TotalBytes);
    //    LogWriteFile(TotalBytesInfo);
#define COMLENGTH 0
    //���յ������ݵĳ���
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

    //    picNametime++;//���ʹ���
}


/************************************************/
/*�� ��:updateClientProgress                     */
/*�� ��:numBytes-���ݵ��ֽ���                      */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:���¿ͻ��ഫ�ݵĽ�����                        */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
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
/*�� ��:CreateTcpSocket                          */
/*�� ��:                                         */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:���������׽���                              */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
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
/*�� ��:CreateCapturethread                      */
/*�� ��:numBytes-���ݵ��ֽ���                      */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:���������̣߳�Ĭ�ϲ�׽ȫ���������źſ����Ƿ���  */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
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
/*�� ��:TimerSets                                */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��ʱ�����������������������ݣ�                 */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::TimerSets()
{
    pNetSendTimer = new QTimer();
    connect(pNetSendTimer,SIGNAL(timeout()),this,SLOT(NetSendData()));
    pNetSendTimer->start(40);
}

/************************************************/
/*�� ��:NetSendData                              */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:�������ݷ��Ͳۺ���                           */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
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
        //���ô���״̬����ʾ�仯��������...��
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
/*�� ��:StopActionSets                           */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:ֹͣ���䴦��������                          */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::StopActionSets()
{
    qDebug() << " stop!!";
    BtnStartPix();
    BtnDisable();
    showTextTransferOver();

    //ֹͣ����
    StopCapScreen();

    BtnEnable();
}

/************************************************/
/*�� ��:on_pushButtonStart_clicked               */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��ʼ/ֹͣ��ť������                          */
/*     ��һ�ε��ʱ��Ĭ�Ͻ���ֹͣ��ť������Ϊ������...   */
/*     �ڶ��ε��ʱ�����뿪ʼ��ť������Ϊ�������        */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::on_pushButtonStart_clicked()
{
    qDebug("here!\n");
    qDebug("mNo:%d",mNo);
    if(0 == ((mNo++)%2)) /* ������� */
    {
        qDebug("here!1111111\n");
        StopActionSets();
    }
    else /* ��ʼ���� */
    {
        qDebug() << " To start!!";
        qDebug() << " isStarted:" << isStarted;
        BtnStopPix();//��ʾֹͣͼƬ

        if(STAT_RESTOP == isStarted)//��Ҫ��������
        {
            showTextConnecting();
            if(RET_SUCESS != CreateTcpSocket())//���½���socket
            {
                qDebug("create tcp socket faild!!\n");
                BtnStartPix();//��ʾ��ʼͼƬ
                showTextClickToStart();//��ʾ�������
                mNo = NO_FIRST;//
                qDebug() << " No started!!";
                return;
            }
            qDebug("recreate tcp socket ok!\n");
            emit emitCtrlPthreadStart();
            pNetSendTimer->start();//�������ݶ�ʱ��



            showTextTransfering();//���ӳɹ�
            qDebug() << " restarting!!";

            isStarted = STAT_RESTART;
            return;
        }

        //��ʼ����
        if(STAT_STOPPED == CaptureScreenOn())//��������
        {
            BtnStartPix();//��ʾ��ʼͼƬ
            showTextClickToStart();//��ʾ�������
            mNo = NO_FIRST;//
            qDebug() << " No started!!";
        }
        else
        {
            showTextTransfering();//���ӳɹ�
            qDebug() << " starting!!";
        }

    }
}

/************************************************/
/*�� ��:BtnStartPix                              */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��ʼͼƬ��ʾ����ʽ                           */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnStartPix(void)
{
    QString str = ":images/start.png";
    BtnSetPix(str);
}

/************************************************/
/*�� ��:BtnStopPix                               */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:ֹͣͼƬ��ʾ����ʽ                           */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnStopPix(void)
{
    QString str = ":images/stop.png";
    BtnSetPix(str);
}

/************************************************/
/*�� ��:BtnSetPix                                */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:ͼƬ��ʾ����ʽ                              */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
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
/*�� ��:BtnEnable                                */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��ťʹ��                                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnEnable(void)
{
    ui->pushButtonStart->setEnabled(TRUE);
}

/************************************************/
/*�� ��:BtnDisable                                */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��ť��ֹʹ��                                */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnDisable(void)
{
    ui->pushButtonStart->setEnabled(FALSE);
}

/************************************************/
/*�� ��:CaptureScreenOn                          */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:STAT_STARTED-��ʼ��STAT_STOPPED-ֹͣ       */
/*�� ��:������ʼ                                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
int ScreenCap::CaptureScreenOn()
{
    showTextConnecting();
    if(STAT_STOPPED == isStarted)
    {
        if(RET_SUCESS == CreateCapturethread())//�������Ӳ���ʼץ���߳�
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
/*�� ��:StopCapScreen                            */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:����ֹͣ                                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::StopCapScreen()
{
    emit emitCtrlPthreadStop();
}

/************************************************/
/*�� ��:LineTextTips                             */
/*�� ��:str-lineText���ı�����                     */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:�ı�������ʾ����Ϊ�գ���ʾ�ı���ʾ              */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::LineTextTips(QString str)
{
    str = str.replace(" ","");
    if(str.isEmpty())
    {
        ui->label->setText(QString::fromLocal8Bit(" ������IP��ַ,�������ʼ��ť���д���"));
    }else{
        ui->label->setText(QString::fromLocal8Bit(""));
    }

}

/************************************************/
/*�� ��:showStateBarInfo                         */
/*�� ��:pstr-�ַ���                               */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:״̬����ʾ�ַ�����Ϣ                         */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::showStateBarInfo(const char *pstr)
{
    ui->statusBar->showMessage(QString::fromLocal8Bit(pstr));
}

/************************************************/
/*�� ��:showTextTransfering                      */
/*�� ��:pstr-�ַ���                               */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:״̬����ʾ"������..."                        */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextTransfering()
{
    showStateBarInfo("������...");
}

/************************************************/
/*�� ��:showTextTransferingNo                    */
/*�� ��:pstr-�ַ���                               */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:״̬����ʾ"������...������"                  */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextTransferingNo(quint64 num)
{
#define CASENUMUNIT 80
    quint64 order = num % CASENUMUNIT;
    switch(order)
    {
    case 0:
        showStateBarInfo("������");
        break;
    case 20:
        showStateBarInfo("������.");
        break;
    case 40:
        showStateBarInfo("������..");
        break;
    case 60:
        showStateBarInfo("������...");
        break;
    default:
        //        showStateBarInfo("������......");
        break;
    }
}

/************************************************/
/*�� ��:showTextTransferOver                     */
/*�� ��:pstr-�ַ���                               */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:״̬����ʾ"�������"                         */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextTransferOver()
{
    showStateBarInfo("�������");
}

/************************************************/
/*�� ��:CheckIPAddrValid                         */
/*�� ��:ipaddr-IP��ַ                             */
/*�� ��:��                                        */
/*�� ��:RET_SUCESS-�ɹ���RET_FAIL-ʧ��             */
/*�� ��:���IP��ַ��Ч��                            */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
int ScreenCap::CheckIPAddrValid(QString ipaddr)
{
    QRegExp regExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if(!regExp.exactMatch(ipaddr))
    {
        QMessageBox::warning(NULL, str_china("��ʾ"), str_china("ip��ַ����"),NULL,NULL);
        return RET_FAIL;
    }

    return RET_SUCESS;
}

/************************************************/
/*�� ��:showAppVerion                            */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:״̬����ʾ����汾                           */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::showAppVerion(void)
{
    QString verinfo = QString::fromLocal8Bit("¼������ ") +
            QString::fromLocal8Bit(ScreenCapVersion) +
            QString::fromLocal8Bit("�������ʼ���� ");
    ui->statusBar->showMessage(verinfo);
}

/************************************************/
/*�� ��:showTextClickOverToReStart                */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:״̬����ʾ  "�����жϣ�������¿�ʼ���� "      */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextClickOverToReStart(void)
{
    QString verinfo = QString::fromLocal8Bit("�����жϣ�������¿�ʼ���� ");
    ui->statusBar->showMessage(verinfo);
}


/************************************************/
/*�� ��:showTextClickToStart                     */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:״̬����ʾ  "�����ʼ���� "                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextClickToStart(void)
{
    QString verinfo = QString::fromLocal8Bit("�����ʼ���� ");
    ui->statusBar->showMessage(verinfo);
}

/************************************************/
/*�� ��:showTextConnecting                       */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:��                                        */
/*�� ��:״̬����ʾ"������...����ȴ� "                */
/*author :wxj                                    */
/*version:1.0                                    */
/*ʱ ��:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextConnecting(void)
{
    QString verinfo = QString::fromLocal8Bit("������...����ȴ� ");
    ui->statusBar->showMessage(verinfo);
}



