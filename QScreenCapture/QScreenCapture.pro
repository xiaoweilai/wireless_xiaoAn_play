#-------------------------------------------------
#
# Project created by QtCreator 2015-04-04T14:17:45
#
#-------------------------------------------------

QT       += core gui
QT       += network
TARGET = QScreenCapture
TEMPLATE = app


SOURCES += main.cpp\
        screencap.cpp \
    capthread.cpp

HEADERS  += screencap.h \
    version.h \
    capthread.h \
    test.h

FORMS    += screencap.ui

RESOURCES += \
    gSource.qrc

RC_FILE = capsrnapp.rc

#ffmpeg and SDL header dir
INCLUDEPATH += ./ffmpeg/include

#ffmpeg lib
LIBS += -L./ffmpeg/lib -lavcodec.dll -lavdevice.dll -lavfilter.dll -lavformat.dll  -lavutil.dll -lpostproc.dll -lswresample.dll -lswscale.dll

#SDL lib
LIBS += -L./ffmpeg/lib -lSDL.dll -lSDLmain

