/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef ARCREADER_H
#define ARCREADER_H

#include <QString>
#include <QFile>
#include <QByteArray>
#include <QVector>
#include <QtXml>
#include <QPair>

#include "datatypes.h"
#include "module.h"

class ArcReader : public Module
{
public:
    ArcReader();
    ~ArcReader();

    int open(QString path);
    int decode();
    int extract(QString outputDir);
    int saveStructure(QString path);

private:
    QFile inputFile;
    quint32 entriesCount;
    quint32 dataOffset;

    QByteArray undecodedHeader;
    QVector<arc_entry_t> entries;

    QDomDocument xmlData;

};

#endif // ARCREADER_H
