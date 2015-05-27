#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T14:25:49
#
#-------------------------------------------------

QT       += core gui
QT       += network
CONFIG   += warn_off

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TcpClient
TEMPLATE = app

SOURCES += main.cpp\
        MainWindow.cpp \
    CPlayer.cpp \
    CNet.cpp \
    CBuf.cpp \
    CProtoc.cpp \
    CConfig.cpp

HEADERS  += MainWindow.h \
    CPlayer.h \
    CNet.h \
    color.h \
    CBuf.h \
    CProtoc.h \
    CConfig.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lprotobuf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lprotobuf
else:unix: LIBS += -L$$PWD/lib/ -lprotobuf

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
