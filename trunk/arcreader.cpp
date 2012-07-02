/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include "arcreader.h"
#include "defines.h"

#include <string.h>

ArcReader::ArcReader() : Module("ArcReader")
{
}

ArcReader::~ArcReader()
{
    inputFile.close();
    undecodedHeader.clear();
    entries.clear();
    entries.detach();
}

int ArcReader::open(QString path)
{
    inputFile.setFileName(path);
    if(inputFile.open(QIODevice::ReadOnly))
    {
        return 1;
    }
    else
    {
        fprintf(stderr, "File open failed!\n");
        return 0;
    }
}

int ArcReader::decode()
{
    if(inputFile.openMode() != QIODevice::ReadOnly)
    {
        fprintf(stderr, "File access mode check failed!\n");
        return 0;
    }

    inputFile.read((char*)&entriesCount, sizeof(quint32));
    inputFile.read((char*)&dataOffset, sizeof(quint32));

    // Read undecoded data
    undecodedHeader = inputFile.read(entriesCount * sizeof(quint16) * 2);

    // Read entries data
    for(size_t i = 0; i < entriesCount; i++)
    {
        arc_entry_t newEntry;
        inputFile.read((char*)&newEntry, sizeof(arc_entry_t));

        entries.push_back(newEntry);
    }

    return entries.size();
}

int ArcReader::extract(QString outputDir)
{
    if(inputFile.openMode() != QIODevice::ReadOnly)
    {
        fprintf(stderr, "File access mode check failed!\n");
        return 0;
    }

    QVector<arc_entry_t>::iterator it;

    /* Create root */
    QDomElement root = xmlData.createElement("archive");
    xmlData.appendChild(root);

    /* Prepare header */
    QDomElement headerNode = xmlData.createElement("header");
    APPEND_ATTRIBUTE_DEC(xmlData, headerNode, "entriesCount", entriesCount);
    APPEND_ATTRIBUTE_HEX(xmlData, headerNode, "dataOffset", dataOffset);

    QDomElement undecodedDataNode = xmlData.createElement("undecodedData");
    QDomText undecodedDataValue = xmlData.createTextNode(undecodedHeader.toBase64());

    /* Build header XML tree */
    root.appendChild(headerNode);
    headerNode.appendChild(undecodedDataNode);
    undecodedDataNode.appendChild(undecodedDataValue);

    /* Entries to XML */
    QDomElement entriesNode = xmlData.createElement("entries");
    APPEND_ATTRIBUTE_DEC(xmlData, entriesNode, "entriesCount", entriesCount);
    root.appendChild(entriesNode);

    for(it = entries.begin(); it != entries.end(); ++it)
    {
        quint32 offset = it->offset;
        quint32 size = it->size;
        quint32 unknown0 = it->unknown0;
        quint32 unknown1 = it->unknown1;

        QDomElement entryNode = xmlData.createElement("entry");

        APPEND_ATTRIBUTE_HEX(xmlData, entryNode, "offset", offset);
        APPEND_ATTRIBUTE_HEX(xmlData, entryNode, "size", size);
        APPEND_ATTRIBUTE_HEX(xmlData, entryNode, "unknown0", unknown0);
        APPEND_ATTRIBUTE_HEX(xmlData, entryNode, "unknown1", unknown1);

        entriesNode.appendChild(entryNode);
    }


    /* XML */
    QDomElement filesNode = xmlData.createElement("files");
    APPEND_ATTRIBUTE_DEC(xmlData, filesNode, "entriesCount", entriesCount);
    root.appendChild(filesNode);

    /* Sort entries */
    qSort(entries.begin(), entries.end(), arc_entry_t::lessThan);

    /* Read files */
    size_t fileCounter = 0;
    for(it = entries.begin(); it != entries.end(); ++it)
    {
        quint32 offset = it->offset;
        quint32 size = it->size;

        inputFile.seek(offset);

        QByteArray fileData = inputFile.read(size);

        QString fileName;
        QString fileType;
        if(qstrncmp(fileData.data(), "SCF", 3) == 0)
        {
            fileName.sprintf("file_%04d.scf", fileCounter);
            fileType.append("SCF");
        }
        else if (qstrncmp(fileData.data(), "TIM2", 4) == 0)
        {
            fileName.sprintf("file_%04d.tm2", fileCounter);
            fileType.append("TM2");
        }
        else
        {
            fileName.sprintf("file_%04d.bin", fileCounter);
            fileType.append("Undefined");
        }

        /* Write data to file */
        QFile outputFile(outputDir.append("\\") + fileName);

        outputFile.open(QIODevice::WriteOnly);
        outputFile.write(fileData);
        outputFile.close();

        /* XML */
        QDomElement fileNode = xmlData.createElement("file");

        APPEND_ATTRIBUTE_HEX(xmlData, fileNode, "offset", offset);
        APPEND_ATTRIBUTE_HEX(xmlData, fileNode, "size", size);
        APPEND_ATTRIBUTE_STR(xmlData, fileNode, "filename", fileName);
        APPEND_ATTRIBUTE_STR(xmlData, fileNode, "type", fileType);

        filesNode.appendChild(fileNode);

        ++fileCounter;
    }

    return 0;
}

int ArcReader::saveStructure(QString path)
{
    QFile outFile(path);
    outFile.open(QIODevice::WriteOnly);

    QTextStream xmlOutStream(&outFile);
    xmlData.save(xmlOutStream, 2);
    xmlOutStream.flush();

    outFile.close();
    return 0;
}
