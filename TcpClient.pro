#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T14:25:49
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TcpClient
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    CPlayer.cpp \
    CNet.cpp

HEADERS  += MainWindow.h \
    CPlayer.h \
    CNet.h \
    color.h
