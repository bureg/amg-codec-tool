/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include "arcreader.h"

#include <string.h>

ArcReader::ArcReader()
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
    QDomAttr entriesCountAttr = xmlData.createAttribute("entriesCount");
    QDomAttr dataOffsetAttr = xmlData.createAttribute("dataOffset");

    QDomElement undecodedDataNode = xmlData.createElement("undecodedData");
    QDomText undecodedDataValue = xmlData.createTextNode(undecodedHeader.toBase64());

    entriesCountAttr.setValue(QString().sprintf("%d", entriesCount));
    dataOffsetAttr.setValue(QString().sprintf("0x%x", dataOffset));

    /* Build header XML tree */
    root.appendChild(headerNode);
    headerNode.appendChild(undecodedDataNode);
    headerNode.setAttributeNode(entriesCountAttr);
    headerNode.setAttributeNode(dataOffsetAttr);
    undecodedDataNode.appendChild(undecodedDataValue);

    /* Entries to XML */
    QDomElement entriesNode = xmlData.createElement("entries");
    entriesNode.setAttributeNode(entriesCountAttr);
    root.appendChild(entriesNode);

    for(it = entries.begin(); it != entries.end(); ++it)
    {
        quint32 offset = it->offset;
        quint32 size = it->size;
        quint32 unknown0 = it->unknown0;
        quint32 unknown1 = it->unknown1;

        QDomElement entryNode = xmlData.createElement("entry");
        QDomAttr offsetAttr = xmlData.createAttribute("offset");
        QDomAttr sizeAttr = xmlData.createAttribute("size");
        QDomAttr unknown0Attr = xmlData.createAttribute("unknown0");
        QDomAttr unknown1Attr = xmlData.createAttribute("unknown1");

        offsetAttr.setValue(QString().sprintf("0x%x", offset));
        sizeAttr.setValue(QString().sprintf("0x%x", size));
        unknown0Attr.setValue(QString().sprintf("0x%x", unknown0));
        unknown1Attr.setValue(QString().sprintf("0x%x", unknown1));

        entryNode.setAttributeNode(offsetAttr);
        entryNode.setAttributeNode(sizeAttr);
        entryNode.setAttributeNode(unknown0Attr);
        entryNode.setAttributeNode(unknown1Attr);

        entriesNode.appendChild(entryNode);
    }


    /* XML */
    QDomElement filesNode = xmlData.createElement("files");
    filesNode.setAttributeNode(entriesCountAttr);
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
            fileName.sprintf("file_%04d.tim2", fileCounter);
            fileType.append("TIM2");
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
        QDomAttr offsetAttr = xmlData.createAttribute("offset");
        QDomAttr sizeAttr = xmlData.createAttribute("size");
        QDomAttr fileNameAttr = xmlData.createAttribute("filename");
        QDomAttr fileTypeAttr = xmlData.createAttribute("type");

        offsetAttr.setValue(QString().sprintf("0x%x", offset));
        sizeAttr.setValue(QString().sprintf("0x%x", size));
        fileNameAttr.setValue(fileName);
        fileTypeAttr.setValue(fileType);

        filesNode.appendChild(fileNode);
        fileNode.setAttributeNode(offsetAttr);
        fileNode.setAttributeNode(sizeAttr);
        fileNode.setAttributeNode(fileNameAttr);
        fileNode.setAttributeNode(fileTypeAttr);

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
