/*
  This file is a part of the
  Amagami PS2 Translation Project

  You may feel free to use this code if you want so.
  We provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include "xmlrenamer.h"

XmlRenamer::XmlRenamer() : Module("XmlRenamer")
{
}

void XmlRenamer::numToReal(QString dirPath)
{
    QDir xmldir(dirPath);
    QStringList filter("*.xml");
    QFileInfoList dirlist = xmldir.entryInfoList(filter);
    qint32 i, j;

    for (i = 0; i < dirlist.size(); i++)
    {
        QFileInfo fileInfo = dirlist.at(i);
        QFile xmlFile(fileInfo.filePath());
        char buffer[100];
        char *ptr = buffer;

        xmlFile.open(QFile::ReadOnly);
        xmlFile.read((char*)&buffer, 100);
        xmlFile.close();

        //locate 'name' property
        for (j = 0; j < 94; j++)
        {
            if (qstrncmp((const char*)"name=", ptr, 5) == 0)
            {
                break;
            }

            ptr++;
        }
        if (j == 62) //that should be far enough
        {
            fatalExit("Couldn't find script name");
        }

        ptr += 6; //advance over: name="

        //get name length
        for (j = 0; j < 32; j++)
        {
            if (qstrncmp((const char*)"\"", ptr + j, 1) == 0)
            {
                break;
            }
        }
        if (j == 32)
        {
            fatalExit("Name property seems too long");
        }

        ptr[j] = 0; //Null-terminate the string
        QString xmlNewName = QString().sprintf("%s.xml", ptr);

        printf("%s = %s\n", fileInfo.filePath().toAscii().data(), xmlNewName.toAscii().data());
        if (xmldir.rename(fileInfo.filePath(), xmlNewName) != 1)
        {
            printf("Failed to rename %s\n", fileInfo.filePath().toAscii().data());
        }
    }
}

void XmlRenamer::realToNum(QString dirPath)
{
    QDir xmldir(dirPath);
    QStringList filter("*.xml");
    QStringList dirlist = xmldir.entryList(filter, QDir::Files, QDir::Name);
    qint32 i;

    for (i = 0; i < dirlist.size(); i++)
    {
        QString xmlNewName = QString().sprintf("file_%04d.xml", i);
        printf("%s = %s\n", xmlNewName.toAscii().data(), dirlist.at(i).toAscii().data());
        if (xmldir.rename(dirlist.at(i), xmlNewName) != 1)
        {
            printf("Failed to rename %s\n", dirlist.at(i).toAscii().data());
        }
    }
}
