/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include "arcwriter.h"
#include "defines.h"

ArcWriter::ArcWriter() : Module("ArcWriter")
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
    GET_NODE(root, "header", headerNode);
    headerNode = root.firstChildElement("header");

    ATTRIBUTE_TO_UINT_NONZERO(headerNode, "entriesCount", entriesCount, quint32);
    ATTRIBUTE_TO_UINT_NONZERO(headerNode, "dataOffset", dataOffset, quint32);

    GET_NODE_BINARY_DATA(headerNode, undecodedHeader);

    /* Load entries list */
    size_t counter = 0;
    QDomElement entriesNode, entryNode;

    GET_NODE(root, "entries", entriesNode);
    GET_NODE(entriesNode, "entry", entryNode);

    while( !entryNode.isNull())
    {
        arc_entry_t entry;

        ATTRIBUTE_TO_UINT_NONZERO(entryNode, "offset", entry.offset, quint32);
        ATTRIBUTE_TO_UINT_NONZERO(entryNode, "size", entry.size, quint32);
        ATTRIBUTE_TO_UINT(entryNode, "unknown0", entry.unknown0, quint32);
        ATTRIBUTE_TO_UINT(entryNode, "unknown1", entry.unknown1, quint32);

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
    counter = 0;
    QDomElement filesNode, fileNode;
    GET_NODE(root, "files", filesNode);
    GET_NODE(filesNode, "file", fileNode);

    while( !fileNode.isNull())
    {
        quint32 size;
        QString name;

        ATTRIBUTE_TO_UINT_NONZERO(fileNode, "size", size, quint32);
        ATTRIBUTE_TO_STRING_NOT_EMPTY(fileNode, "filename", name);

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
