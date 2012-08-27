#-------------------------------------------------
#
# Project created by QtCreator 2012-05-24T15:53:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FD44Editor
TEMPLATE = app


SOURCES += main.cpp\
        fd44editor.cpp

HEADERS  += fd44editor.h \
    bios.h \
    motherboards.h

FORMS    += fd44editor.ui

OTHER_FILES += \
    TODO.txt
