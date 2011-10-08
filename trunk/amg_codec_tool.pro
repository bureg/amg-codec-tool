#-------------------------------------------------
#
# Project created by QtCreator 2011-08-02T00:11:32
#
#-------------------------------------------------

QT       += core xml
QT       -= gui

TARGET = amg_codec_tool
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    arcreader.cpp \
    arcwriter.cpp \
    arcpacker.cpp \
    scfencoder.cpp \
    scfdecoder.cpp \
    module.cpp \
    langdb.cpp

HEADERS += \
    arcreader.h \
    arcwriter.h \
    arcpacker.h \
    datatypes.h \
    scfencoder.h \
    scfdecoder.h \
    defines.h \
    module.h \
    xmlhelpers.h \
    langdb.h
