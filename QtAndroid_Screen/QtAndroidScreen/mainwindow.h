#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QIODevice>
#include <QFile>


enum FLAGS_CLIENT
{
    FLAGS_NONE = 0,
    FLAGS_HAVE
};

namespace Ui {
class MainWindow;
}




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



private slots:
    void on_pushButton_clicked();
    void showifconfig(void);
    QString Getifconfig(void);
    void start();
    void stop();
    void acceptConnection();
    void updateServerProgress();
    void displayError(QAbstractSocket::SocketError socketError);
    QString bindIpAddr();
    void setOnlyOneClient();
    quint8 getOnlyOneClient();

    void on_startButton_clicked();

private:
    Ui::MainWindow *ui;
private:
    QTcpServer tcpServer;
    QTcpSocket *tcpServerConnection;
    qint64  TotalBytes;
    qint64  bytesReceived;
    qint64  fileNameSize;
    qint64  bytesNeedRecv;
    QString fileName;
    QFile *localFile;
    QByteArray inBlock;

    qint8 OnlyOneClient; //仅一个客户端
};

#endif // MAINWINDOW_H
