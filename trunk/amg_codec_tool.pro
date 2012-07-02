#-------------------------------------------------
#
# Project created by QtCreator 2011-08-02T00:11:32
#
#-------------------------------------------------

QT       += core xml

TARGET = amg_codec_tool
CONFIG   += console

TEMPLATE = app

SOURCES += main.cpp \
    arcreader.cpp \
    arcwriter.cpp \
    arcpacker.cpp \
    scfencoder.cpp \
    scfdecoder.cpp \
    module.cpp \
    langdb.cpp \
    swizzleprocessor.cpp \
    tm2alphafixer.cpp \
    xmlsync.cpp \
    imgextractor.cpp \
    scfrenamer.cpp \
    cltprocessor.cpp \
    xmlrenamer.cpp \

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
    langdb.h \
    swizzleprocessor.h \
    tm2alphafixer.h \
    xmlsync.h \
    imgextractor.h \
    scfrenamer.h \
    cltprocessor.h \
    xmlrenamer.h
