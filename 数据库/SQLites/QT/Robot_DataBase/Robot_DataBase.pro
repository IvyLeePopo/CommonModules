#-------------------------------------------------
#
# Project created by QtCreator 2023-08-22T14:27:20
#
#-------------------------------------------------

QT       += core network sql

QT       -= gui

TARGET = Robot_DataBase
TEMPLATE = lib
CONFIG +=plugin
DEFINES += ROBOT_DATABASE_LIBRARY

DESTDIR = ../scaner/lib/

LIBS += -L../scaner/lib/ -lrt

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
        robotdatabasemanager.cpp \
        robotdatabaseapi.cpp

HEADERS += \
        robot_database_global.h \
        robotdatabasemanager.h \
        robotdatabaseapi.h

include(General/general.pri)

INCLUDEPATH += -I $$PWD/General

unix {
    target.path = /usr/lib
    INSTALLS += target
}
