/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef SCFCODER_H
#define SCFCODER_H

#include <QFile>
#include <QtXml>
#include <QString>

#include "defines.h"
#include "module.h"

typedef QByteArray::iterator DataIterator;

class ScfEncoder : public Module
{
public:
    ScfEncoder();

    void load(QString path);
    void saveScf(QString path);

    void setLanguage(lang_id lang);
    lang_id getLanguage();

private:
    void outputLabels(QFile &file);
    void outputLabelsHeader(QFile &file);
    void outputScfHeader(QFile &file);
    void outputVariables(QFile &file);
    void outputBlocksHeader(QFile &file);
    void outputBlocks(QFile &file);
    void outputFunctionsHeader(QFile &file);
    void outputCodeSection(QFile &file);
    void processEntryType03(QFile &file, QDomElement &parentNode);
    void processEntryType05(QFile &file, QDomElement &parentNode);
    void processEntryType07(QFile &file, QDomElement &parentNode);
    void processEntryType08(QFile &file, QDomElement &parentNode);
    void convertText(QByteArray &data, QDomElement &parentNode, QTextCodec *codec);

private:
    // ----- XML data container
    QDomDocument xmlData;
    QDomElement root;
    lang_id lang;
};

#endif // SCFCODER_H
