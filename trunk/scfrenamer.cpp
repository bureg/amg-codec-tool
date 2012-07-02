/*
  This file is a part of the
  Amagami PS2 Translation Project

  You may feel free to use this code if you want so.
  We provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include "scfrenamer.h"

ScfRenamer::ScfRenamer() : Module("ScfRenamer")
{
}

void ScfRenamer::numToReal(QString dirPath)
{
    QDir scfdir(dirPath);
    QStringList filter("*.scf");
    QFileInfoList dirlist = scfdir.entryInfoList(filter);
    qint32 i;
    quint16 nameLen;

    for (i = 0; i < dirlist.size(); i++)
    {
        QFileInfo fileInfo = dirlist.at(i);
        QFile scfFile(fileInfo.filePath());
        char temp[32]; //dammit Qt... i can't think

        scfFile.open(QFile::ReadOnly);
        scfFile.seek(6);
        scfFile.read((char*)&nameLen, 2);
        scfFile.read(temp, nameLen);
        scfFile.close();

        temp[nameLen] = 0; //Null-terminate the string
        QString scfNewName = QString().sprintf("%s.scf", temp);

        printf("%s = %s\n", fileInfo.filePath().toAscii().data(), scfNewName.toAscii().data());
        if (scfdir.rename(fileInfo.filePath(), scfNewName) != 1)
        {
            printf("Failed to rename %s\n", fileInfo.filePath().toAscii().data());
        }
    }
}

void ScfRenamer::realToNum(QString dirPath)
{
    QDir scfdir(dirPath);
    QStringList filter("*.scf");
    QStringList dirlist = scfdir.entryList(filter, QDir::Files, QDir::Name);
    qint32 i;

    for (i = 0; i < dirlist.size(); i++)
    {
        QString scfNewName = QString().sprintf("file_%04d.scf", i);
        printf("%s = %s\n", scfNewName.toAscii().data(), dirlist.at(i).toAscii().data());
        if (scfdir.rename(dirlist.at(i), scfNewName) != 1)
        {
            printf("Failed to rename %s\n", dirlist.at(i).toAscii().data());
        }
    }
}
