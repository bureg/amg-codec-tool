/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include "arcwriter.h"

ArcWriter::ArcWriter()
{
}

int ArcWriter::loadStructure(QString structurePath)
{
    QFile structFile(structurePath);

    if(!structFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return 1;
    }

    /* Load all data */
    xmlData.setContent(structFile.readAll());

    structFile.close();

    // Get root
    QDomElement root;
    root = xmlData.documentElement();

    if(root.isNull())
    {
        fprintf(stderr, "<archive> container not found!\n");
        return 1;
    }

    // Get undecoded header
    QDomElement headerNode;
    headerNode = root.firstChildElement("header");
    if( headerNode.isNull())
    {
        fprintf(stderr, "<header> container not found!\n");
        return 1;
    }

    QString entriesCountString = headerNode.attribute("entriesCount");
    QString dataOffsetString = headerNode.attribute("dataOffset");
    entriesCount = entriesCountString.toInt(NULL, 0);
    dataOffset = dataOffsetString.toInt(NULL, 0);

    undecodedHeader = QByteArray::fromBase64(headerNode.text().toAscii());

    /* Load entries list */
    QDomElement entriesNode;
    entriesNode = root.firstChildElement("entries");
    if( entriesNode.isNull())
    {
        fprintf(stderr, "<entries> container not found!\n");
        return 1;
    }

    size_t counter = 0;
    QDomElement entryNode = entriesNode.firstChildElement("entry");
    while( !entryNode.isNull())
    {
        arc_entry_t entry;

        QString offsetString = entryNode.attribute("offset");
        QString sizeString = entryNode.attribute("size");
        QString unknown0String = entryNode.attribute("unknown0");
        QString unknown1String = entryNode.attribute("unknown1");

        quint32 offset = offsetString.toUInt(NULL, 0);
        quint32 size = sizeString.toUInt(NULL, 0);
        quint32 unknown0 = unknown0String.toUInt(NULL, 0);
        quint32 unknown1 = unknown1String.toUInt(NULL, 0);

        entry.offset = offset;
        entry.size = size;
        entry.unknown0 = unknown0;
        entry.unknown1 = unknown1;

        entries.push_back(entry);

        /* Proceed */
        entryNode = entryNode.nextSiblingElement("entry");
        ++counter;
    }

    if( counter != entriesCount)
    {
        fprintf(stderr, "[entries] Expected %d entries, found %d entries!\n", entriesCount, counter);
        return 1;
    }

    /* Load file names list */
    QDomElement filesNode;
    filesNode = root.firstChildElement("files");
    if( filesNode.isNull())
    {
        fprintf(stderr, "<files> container not found!\n");
        return 1;
    }

    counter = 0;
    QDomElement fileNode = filesNode.firstChildElement("file");
    while( !fileNode.isNull())
    {
        QString name = fileNode.attribute("filename");
        quint32 size = fileNode.attribute("size").toInt();
        files.push_back( qMakePair< QString, quint32>(name, size));

        /* Proceed */
        fileNode = fileNode.nextSiblingElement("file");
        ++counter;
    }

    if( counter != entriesCount)
    {
        fprintf(stderr, "[files] Expected %d entries, found %d entries!\n", entriesCount, counter);
        return 1;
    }

    return 0;
}

int ArcWriter::buildArchive(QString sourceDir, QString outputPath, bool checkSize)
{
    QFile outputFile(outputPath);

    if(!outputFile.open(QIODevice::WriteOnly))
    {
        return 1;
    }

    // Write entries count and data offset
    outputFile.write((char*)&entriesCount, sizeof(quint32));
    outputFile.write((char*)&dataOffset, sizeof(quint32));

    // Write undecoded header
    outputFile.write(undecodedHeader);

    // Write entries list
    QVector<arc_entry_t>::iterator it;
    for( it = entries.begin(); it != entries.end(); ++it)
    {
        arc_entry_t entry = *it;
        //fprintf(stderr, "-> 0x%x\n", it->offset);
        outputFile.write((char*)&entry, sizeof(arc_entry_t));
    }

    // Copy files data
    QVector< QPair< QString, quint32 > >::iterator file_it;

    for( file_it = files.begin(); file_it != files.end(); ++file_it)
    {
        QString path = sourceDir + QString("\\") + file_it->first;
        QFile fileIn(path);

        if(!fileIn.open(QIODevice::ReadOnly))
        {
            fprintf(stderr, "File [%s] open failed!\n", path.toAscii().data());
            return 1;
        }

        if (checkSize && (fileIn.size() != file_it->second))
        {
            fprintf(stderr, "File [%s] size check failed!\n", path.toAscii().data());
            return 1;
        }

        outputFile.write(fileIn.readAll());
        fileIn.close();
    }

    outputFile.close();

    return 0;
}
