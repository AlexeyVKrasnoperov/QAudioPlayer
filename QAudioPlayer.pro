#-------------------------------------------------
#
# Project created by QtCreator 2018-02-19T23:09:09
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QAudioPlayer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        qaudiodialog.cpp \
    audiobuffer.cpp \
    audiofile.cpp \
    audiofilereader.cpp \
    player.cpp

HEADERS += \
        qaudiodialog.h \
    audiobuffer.h \
    audiofile.h \
    audiofilereader.h \
    fileformat.h \
    player.h

FORMS += \
        qaudiodialog.ui

RESOURCES += \
    res.qrc


win32: {
    FFMPEG_DIR=$$PWD/ffmpeg-3.2.2-lgpl
    INCLUDEPATH += $$FFMPEG_DIR/include
    LIBS += -L$$FFMPEG_DIR/bin/w64
}

LIBS += -lavformat \
        -lavcodec \
        -lavutil \
        -lswresample
