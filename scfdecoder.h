/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef XMLCODER_H
#define XMLCODER_H

#include <QFile>
#include <QtXml>
#include <QString>

#include "module.h"

typedef QByteArray::iterator DataIterator;

class ScfDecoder : public Module
{
public:
    ScfDecoder();

    void load(QString path);
    void saveXml(QString path);

private:
    void createXml();
    void checkHeader(QFile &file);
    void checkLabelsHeader(QFile &file);
    void readLabelsSection(QFile &file);
    void readVariablesSection(QFile &file);
    void readBlocksSection(QFile &file);
    void readCodeSection(QFile &file);
    void processEntryType(QFile &file, QDomElement &parentNode, size_t subi);
    void processEntryType03(QFile &file, QDomElement &parentNode);
    void processTextString(size_t index,
                           DataIterator startIt,
                           DataIterator finishIt,
                           QTextCodec *codec,
                           QDomElement &parentNode);
    void processAsciiString(size_t index,
                            DataIterator startIt,
                            DataIterator finishIt,
                            QDomElement &parentNode);
    void processEntryType05(QFile &file, QDomElement &parentNode);
    void processEntryType07(QFile &file, QDomElement &parentNode);
    void processEntryType08(QFile &file, QDomElement &parentNode);

private:
    // ----- Read buffer 64 KB
    char buffer[64 * 1024];

    // ----- XML data container
    QDomDocument xmlData;
    QDomElement root;
};

#endif // XMLCODER_H
