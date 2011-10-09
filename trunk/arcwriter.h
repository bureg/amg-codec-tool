/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef ARCWRITER_H
#define ARCWRITER_H

#include <QString>
#include <QFile>
#include <QByteArray>
#include <QVector>
#include <QtXml>
#include <QHash>

#include "datatypes.h"
#include "module.h"

class ArcWriter : public Module
{
public:
    ArcWriter();

    int loadStructure(QString structurePath);
    int buildArchive(QString sourceDir, QString outputPath);

private:
    QDomDocument xmlData;

    quint32 entriesCount;
    quint32 dataOffset;

    QByteArray undecodedHeader;
    QVector<arc_entry_t> entries;
    QVector<QString> files;
    QHash<quint32, QString> offsetToFileName;
};

#endif // ARCWRITER_H
