#-------------------------------------------------
#
# Project created by QtCreator 2014-12-23T22:40:56
#
#-------------------------------------------------

QT       += core gui network

TARGET = fileServer_Receive
TEMPLATE = app


SOURCES += main.cpp\
        tcp_fileserver_recv.cpp \
    playerthread.cpp

HEADERS  += tcp_fileserver_recv.h \
    playerthread.h

FORMS    += tcp_fileserver_recv.ui



#ffmpeg and SDL header dir
INCLUDEPATH += ./ffmpeg/include

#ffmpeg lib
LIBS += -L./ffmpeg/lib -lavcodec.dll -lavdevice.dll -lavfilter.dll -lavformat.dll  -lavutil.dll -lpostproc.dll -lswresample.dll -lswscale.dll

#SDL lib
LIBS += -L./ffmpeg/lib -lSDL.dll -lSDLmain

