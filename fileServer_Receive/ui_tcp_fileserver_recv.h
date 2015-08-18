/********************************************************************************
** Form generated from reading UI file 'tcp_fileserver_recv.ui'
**
** Created: Sat Jan 10 21:01:08 2015
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCP_FILESERVER_RECV_H
#define UI_TCP_FILESERVER_RECV_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Tcp_FileServer_Recv
{
public:
    QWidget *centralWidget;
    QLabel *serverStatusLabel;
    QPushButton *startButton;
    QPushButton *quitButton;
    QProgressBar *clientProgressBar;
    QProgressBar *serverProgressBar;
    QLabel *imageLabel;
    QLineEdit *lineEdit_ip;
    QPushButton *stopButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Tcp_FileServer_Recv)
    {
        if (Tcp_FileServer_Recv->objectName().isEmpty())
            Tcp_FileServer_Recv->setObjectName(QString::fromUtf8("Tcp_FileServer_Recv"));
        Tcp_FileServer_Recv->resize(1366, 968);
        centralWidget = new QWidget(Tcp_FileServer_Recv);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        serverStatusLabel = new QLabel(centralWidget);
        serverStatusLabel->setObjectName(QString::fromUtf8("serverStatusLabel"));
        serverStatusLabel->setGeometry(QRect(50, 20, 301, 21));
        startButton = new QPushButton(centralWidget);
        startButton->setObjectName(QString::fromUtf8("startButton"));
        startButton->setGeometry(QRect(60, 40, 71, 51));
        quitButton = new QPushButton(centralWidget);
        quitButton->setObjectName(QString::fromUtf8("quitButton"));
        quitButton->setGeometry(QRect(240, 40, 71, 51));
        clientProgressBar = new QProgressBar(centralWidget);
        clientProgressBar->setObjectName(QString::fromUtf8("clientProgressBar"));
        clientProgressBar->setGeometry(QRect(420, 70, 291, 23));
        clientProgressBar->setValue(0);
        serverProgressBar = new QProgressBar(centralWidget);
        serverProgressBar->setObjectName(QString::fromUtf8("serverProgressBar"));
        serverProgressBar->setGeometry(QRect(420, 40, 291, 23));
        serverProgressBar->setValue(0);
        imageLabel = new QLabel(centralWidget);
        imageLabel->setObjectName(QString::fromUtf8("imageLabel"));
        imageLabel->setGeometry(QRect(0, 100, 1366, 768));
        lineEdit_ip = new QLineEdit(centralWidget);
        lineEdit_ip->setObjectName(QString::fromUtf8("lineEdit_ip"));
        lineEdit_ip->setGeometry(QRect(150, 10, 171, 21));
        stopButton = new QPushButton(centralWidget);
        stopButton->setObjectName(QString::fromUtf8("stopButton"));
        stopButton->setGeometry(QRect(140, 40, 71, 51));
        Tcp_FileServer_Recv->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Tcp_FileServer_Recv);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1366, 23));
        Tcp_FileServer_Recv->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Tcp_FileServer_Recv);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        Tcp_FileServer_Recv->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Tcp_FileServer_Recv);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        Tcp_FileServer_Recv->setStatusBar(statusBar);

        retranslateUi(Tcp_FileServer_Recv);

        QMetaObject::connectSlotsByName(Tcp_FileServer_Recv);
    } // setupUi

    void retranslateUi(QMainWindow *Tcp_FileServer_Recv)
    {
        Tcp_FileServer_Recv->setWindowTitle(QApplication::translate("Tcp_FileServer_Recv", "\346\216\245\346\224\266\346\226\207\344\273\266", 0, QApplication::UnicodeUTF8));
        serverStatusLabel->setText(QApplication::translate("Tcp_FileServer_Recv", "\346\234\215\345\212\241\347\253\257\345\260\261\347\273\252", 0, QApplication::UnicodeUTF8));
        startButton->setText(QApplication::translate("Tcp_FileServer_Recv", "\346\216\245\346\224\266", 0, QApplication::UnicodeUTF8));
        quitButton->setText(QApplication::translate("Tcp_FileServer_Recv", "\351\200\200\345\207\272", 0, QApplication::UnicodeUTF8));
        imageLabel->setText(QString());
        lineEdit_ip->setText(QApplication::translate("Tcp_FileServer_Recv", "192.168.1.104", 0, QApplication::UnicodeUTF8));
        stopButton->setText(QApplication::translate("Tcp_FileServer_Recv", "\345\201\234\346\255\242", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Tcp_FileServer_Recv: public Ui_Tcp_FileServer_Recv {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCP_FILESERVER_RECV_H
