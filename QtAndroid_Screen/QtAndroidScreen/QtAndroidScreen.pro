#-------------------------------------------------
#
# Project created by QtCreator 2015-08-17T20:51:40
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtAndroidScreen
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qDebug2Logcat.cpp

HEADERS  += mainwindow.h \
    qDebug2Logcat.h

FORMS    += mainwindow.ui

CONFIG += mobility
MOBILITY = 

INCLUDEPATH += ./include
LIBS +=-LD:\git\wireless_xiaoAn_play\QtAndroid_Screen\QtAndroidScreen\libs -lavformat -lavcodec -lavcore -lavdevice  -lavutil -lswscale
#LIBS +=-L./libs -lavcodec -lavcore -lavdevice -lavformat -lavutil -lswscale

OTHER_FILES += \
    libs/libavcodec.a \
    libs/libavcore.a \
    libs/libavdevice.a \
    libs/libavfilter.a \
    libs/libavformat.a \
    libs/libavutil.a \
    libs/libswscale.a
