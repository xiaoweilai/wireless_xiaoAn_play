/********************************************************************************
** Form generated from reading UI file 'screencap.ui'
**
** Created: Sat Apr 4 22:08:57 2015
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCREENCAP_H
#define UI_SCREENCAP_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ScreenCap
{
public:
    QWidget *centralWidget;
    QLineEdit *lineEditIp;
    QPushButton *pushButtonStart;
    QLabel *label;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ScreenCap)
    {
        if (ScreenCap->objectName().isEmpty())
            ScreenCap->setObjectName(QString::fromUtf8("ScreenCap"));
        ScreenCap->resize(300, 350);
        ScreenCap->setMinimumSize(QSize(300, 350));
        ScreenCap->setMaximumSize(QSize(300, 350));
        centralWidget = new QWidget(ScreenCap);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        lineEditIp = new QLineEdit(centralWidget);
        lineEditIp->setObjectName(QString::fromUtf8("lineEditIp"));
        lineEditIp->setGeometry(QRect(21, 21, 251, 31));
        QFont font;
        font.setFamily(QString::fromUtf8("Times New Roman"));
        font.setPointSize(16);
        lineEditIp->setFont(font);
        pushButtonStart = new QPushButton(centralWidget);
        pushButtonStart->setObjectName(QString::fromUtf8("pushButtonStart"));
        pushButtonStart->setGeometry(QRect(21, 70, 256, 256));
        pushButtonStart->setMinimumSize(QSize(256, 256));
        pushButtonStart->setMaximumSize(QSize(256, 256));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(16);
        pushButtonStart->setFont(font1);
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(23, 21, 251, 31));
        ScreenCap->setCentralWidget(centralWidget);
        label->raise();
        lineEditIp->raise();
        pushButtonStart->raise();
        statusBar = new QStatusBar(ScreenCap);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ScreenCap->setStatusBar(statusBar);

        retranslateUi(ScreenCap);

        QMetaObject::connectSlotsByName(ScreenCap);
    } // setupUi

    void retranslateUi(QMainWindow *ScreenCap)
    {
        ScreenCap->setWindowTitle(QApplication::translate("ScreenCap", "\345\275\225\345\261\217\344\274\240\350\276\223", 0, QApplication::UnicodeUTF8));
        lineEditIp->setText(QApplication::translate("ScreenCap", "192.168.1.102", 0, QApplication::UnicodeUTF8));
        pushButtonStart->setText(QString());
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ScreenCap: public Ui_ScreenCap {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCREENCAP_H
