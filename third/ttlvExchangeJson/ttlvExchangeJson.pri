QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS
INCLUDEPATH += $$PWD/

SOURCES += \
    $$PWD/ttlvDataAnysis.cpp \
    $$PWD/ttlvconvertjson.cpp

HEADERS += \
    $$PWD/ttlvDataAnysis.h \
    $$PWD/ttlvconvertjson.h

FORMS += \
    $$PWD/ttlvconvertjson.ui

