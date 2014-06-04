#-------------------------------------------------
#
# Project created by QtCreator 2014-06-02T14:11:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sim
TEMPLATE = app

RESOURCES     = sim.qrc

SOURCES += main.cpp\
        mainwindow.cpp \
    newmips.cpp \
    cpu.cpp \
    multiins.cpp

HEADERS  += mainwindow.h \
    newmips.h \
    exp.h \
    cpu.h \
    multiins.h

FORMS    += mainwindow.ui

OTHER_FILES +=
