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

RC_FILE = fd44editor.rc

ICON = fd44editor.icns