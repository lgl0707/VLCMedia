#-------------------------------------------------
#
# Project created by QtCreator 2017-05-23 沙振宇
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Demo_VLC_Qt
TEMPLATE = app

#添加相关库

INCLUDEPATH += $$PWD/libvlc_include
INCLUDEPATH += $$PWD/include/vlc/
LIBS += -L$$PWD/libvlc_lib \
        -L$$PWD/libs
LIBS += -llibvlc -llibvlccore -ljson_vc71_libmtd

SOURCES += main.cpp\
        mainwindow.cpp \
    mousemovewidget.cpp \
    configmanager.cpp

HEADERS  += mainwindow.h \
    mousemovewidget.h \
    configmanager.h

FORMS    += mainwindow.ui \
    mainwindow.ui

RESOURCES += \
    media.qrc
