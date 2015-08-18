#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T10:58:25
#
#-------------------------------------------------

QT       += core gui

TARGET = videosdlplay
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

#INCLUDEPATH +=  SDL/include
#LIBS += SDL/lib/libSDL.dll.a

#INCLUDEPATH +=  ffmpeg/include
#LIBS += ffmpeg/lib/libavcodec.dll.a \
#        ffmpeg/lib/libavdevice.dll.a \
#        ffmpeg/lib/libavfilter.dll.a \
#        ffmpeg/lib/libavformat.dll.a \
#        ffmpeg/lib/libavutil.dll.a \
#        ffmpeg/lib/libpostproc.dll.a \
#        ffmpeg/lib/libswscale.dll.a \


#ffmpeg and SDL header dir
INCLUDEPATH += ./ffmpeg/include

#ffmpeg lib
LIBS += -L./ffmpeg/lib -lavcodec.dll -lavdevice.dll -lavfilter.dll -lavformat.dll  -lavutil.dll -lpostproc.dll -lswresample.dll -lswscale.dll

#SDL lib
LIBS += -L./ffmpeg/lib -lSDL.dll -lSDLmain

