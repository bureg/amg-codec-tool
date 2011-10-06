/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include <QTextCodec>

#include "defines.h"
#include "scfdecoder.h"

ScfDecoder::ScfDecoder()
{
}

void ScfDecoder::checkHeader(QFile &file)
{
    READ_TO_BUF( file, HEADER_LENGTH);
    if( qstrncmp(buffer, header, HEADER_LENGTH))
    {
        fatalExit("Header check FAILED");
    }
}

void ScfDecoder::checkLabelsHeader(QFile &file)
{
    READ_TO_BUF(file, LABELS_HEADER_LENGTH);
    if( qstrncmp(buffer, labelsSectionHeader, LABELS_HEADER_LENGTH))
    {
        fatalExit("Labels section header check FAILED");
    }
}

void ScfDecoder::checkBlocksHeader(QFile &file)
{
    READ_TO_BUF(file, BLOCKS_HEADER_LENGTH);
    if( qstrncmp(buffer, blocksSectionHeader, BLOCKS_HEADER_LENGTH))
    {
        fatalExit("Blocks section header check FAILED");
    }
}

void ScfDecoder::checkFunctionsHeader(QFile &file)
{
    READ_TO_BUF(file, FUNCTIONS_HEADER_LENGTH);
    if( qstrncmp(buffer, functionsSectionHeader, FUNCTIONS_HEADER_LENGTH))
    {
        fatalExit("Functions section header check FAILED");
    }
}

void ScfDecoder::readLabelsSection(QFile &file)
{
    quint16 nameLength;
    QString varName;

    QDomElement lablesNode = xmlData.createElement("labels");

    for(size_t i = 0; i < 2; i++)
    {
        QDomElement itemNode = xmlData.createElement("label");

        READ_TO_VAR_NONZERO(file, nameLength);
        READ_TO_STRING(file, varName, nameLength);

        APPEND_ATTRIBUTE_DEC(xmlData, itemNode, "index", i);
        APPEND_ATTRIBUTE_STR(xmlData, itemNode, "name", varName);

        lablesNode.appendChild(itemNode);
    }

    root.appendChild(lablesNode);
}

void ScfDecoder::readVariablesSection(QFile &file)
{
    quint8 varId;
    quint16 nVars;
    quint16 nameLength;
    QString varName;

    QDomElement varsNode = xmlData.createElement("vars");

    READ_TO_VAR_NONZERO( file, nVars);

    for(size_t i = 0; i < nVars; i++)
    {
        QDomElement itemNode = xmlData.createElement("var");

        READ_TO_VAR(file, varId);
        READ_TO_VAR_NONZERO( file, nameLength);
        READ_TO_STRING(file, varName, nameLength);

        APPEND_ATTRIBUTE_DEC(xmlData, itemNode, "index", i);
        APPEND_ATTRIBUTE_DEC(xmlData, itemNode, "id", varId);
        APPEND_ATTRIBUTE_STR(xmlData, itemNode, "name", varName);

        varsNode.appendChild(itemNode);
    }

    root.appendChild(varsNode);
}

void ScfDecoder::readBlocksSection(QFile &file)
{
    quint16 nBlocks;
    quint16 isMainBlock;
    quint16 nameLength;
    quint16 dataLength;
    QString blockName;

    QDomElement blocksNode = xmlData.createElement("blocks");

    READ_TO_VAR_NONZERO( file, nBlocks);

    for(size_t i = 0; i < nBlocks; i++)
    {
        QDomElement itemNode = xmlData.createElement("block");
        QDomElement itemDataNode = xmlData.createElement("data");

        READ_TO_VAR_NONZERO(file, nameLength);
        READ_TO_STRING(file, blockName, nameLength);
        READ_TO_VAR(file, isMainBlock);
        READ_TO_VAR_NONZERO(file, dataLength);

        APPEND_ATTRIBUTE_DEC(xmlData, itemNode, "index", i);
        APPEND_ATTRIBUTE_STR(xmlData, itemNode, "name", blockName);
        APPEND_ATTRIBUTE_DEC(xmlData, itemNode, "isMain", isMainBlock);
        APPEND_ATTRIBUTE_HEX(xmlData, itemNode, "dataLength", dataLength);

        QByteArray blockData = file.read(dataLength);
        QDomText blockDataEncoded = xmlData.createTextNode(blockData.toBase64());

        itemDataNode.appendChild(blockDataEncoded);
        itemNode.appendChild(itemDataNode);

        blocksNode.appendChild(itemNode);
    }

    root.appendChild(blocksNode);
}

void ScfDecoder::readCodeSection(QFile &file)
{
    quint8 entryType;
    quint16 nEntries;

    QDomElement codeNode = xmlData.createElement("code");

    READ_TO_VAR_NONZERO( file, nEntries);

    for(size_t i = 0; i < nEntries; i++)
    {
        QDomElement itemNode = xmlData.createElement("entry");

        READ_TO_VAR_NONZERO(file, entryType);

        APPEND_ATTRIBUTE_DEC(xmlData, itemNode, "index", i);
        APPEND_ATTRIBUTE_DEC(xmlData, itemNode, "type", entryType);

        switch (entryType)
        {
            case ENTRY_TYPE_03: /* Parameter */
                processEntryType03(file, itemNode);
                break;

            case ENTRY_TYPE_05: /* Text */
                processEntryType05(file, itemNode);
                break;

            case ENTRY_TYPE_06: /* Function */
            case ENTRY_TYPE_07: /* Function */
                processEntryType07(file, itemNode);
                break;

            case ENTRY_TYPE_08: /* Blocks list */
                processEntryType08(file, itemNode);
                break;

            default: /* Unknown type */
                fatalExit("Unknown entry type");
        }

        codeNode.appendChild(itemNode);
    }

    root.appendChild(codeNode);
}

void ScfDecoder::processEntryType03(QFile &file, QDomElement &parentNode)
{
    quint32 value;
    QDomElement itemNode = xmlData.createElement("parameter");

    READ_TO_VAR( file, value);
    APPEND_ATTRIBUTE_HEX(xmlData, itemNode, "value", value);

    parentNode.appendChild(itemNode);
}

void ScfDecoder::processTextString( size_t index,
                                  DataIterator startIt,
                                  DataIterator finishIt,
                                  QTextCodec * codec,
                                  QDomElement &parentNode)
{
    QByteArray chunk;
    DataIterator copyIt;
    QDomElement textNode = xmlData.createElement("text");

    for (copyIt = startIt; copyIt != finishIt; copyIt++)
    {
        chunk.append( *copyIt );
    }

    APPEND_ATTRIBUTE_DEC(xmlData, textNode, "index", index);

    /* Attach original text */
    QDomElement langNodeJp = xmlData.createElement("original");
    QDomText textContainerJp = xmlData.createTextNode(codec->toUnicode(chunk));
    APPEND_ATTRIBUTE_STR(xmlData, langNodeJp, "lang", "jp");
    langNodeJp.appendChild(textContainerJp);
    textNode.appendChild(langNodeJp);

    /* Attach a template for translation */
    QDomElement langNodeEng = xmlData.createElement("translation");
    QDomText textContainerEng = xmlData.createTextNode("TODO");
    APPEND_ATTRIBUTE_STR(xmlData, langNodeEng, "lang", "eng");
    APPEND_ATTRIBUTE_STR(xmlData, langNodeEng, "mode", "off");
    langNodeEng.appendChild(textContainerEng);
    textNode.appendChild(langNodeEng);

    parentNode.appendChild(textNode);
}

void ScfDecoder::processAsciiString(size_t index,
                                  DataIterator startIt,
                                  DataIterator finishIt,
                                  QDomElement &parentNode)
{
    QByteArray chunk;
    DataIterator copyIt;
    QDomElement textNode = xmlData.createElement("ctrl");

    for (copyIt = startIt; copyIt != finishIt; copyIt++)
    {
        chunk.append( *copyIt );
    }

    APPEND_ATTRIBUTE_DEC(xmlData, textNode, "index", index);
    APPEND_ATTRIBUTE_STR(xmlData, textNode, "value", chunk);

    parentNode.appendChild(textNode);
}

void ScfDecoder::processEntryType05(QFile &file, QDomElement &parentNode)
{
    size_t index = 0;
    quint16 dataLength;
    QByteArray data;
    QTextCodec *codec = QTextCodec::codecForName("Shift-JIS");

    READ_TO_VAR_NONZERO( file, dataLength);
    READ_TO_BYTE_ARRAY(file, data, dataLength);

    DataIterator it = data.begin();
    DataIterator chunkStart = it;

    bool isAsciiMode = IS_ASCII_CODE((quint8) *chunkStart);

    while( true )
    {
        quint8 val = (quint8) *it;

        if( IS_ASCII_CODE(val) )
        {
            if( !isAsciiMode )
            {
                processTextString(index, chunkStart, it, codec, parentNode);
                chunkStart = it;
                isAsciiMode = true;
                index++;
            }

            it += 1;
        }
        else
        {
            if( isAsciiMode )
            {
                processAsciiString(index, chunkStart, it, parentNode);
                chunkStart = it;
                isAsciiMode = false;
                index++;
            }

            it += 2;
        }

        if (it == data.end())
        {
            if ( isAsciiMode )
            {
                processAsciiString(index, chunkStart, it, parentNode);
            }
            else
            {
                processTextString(index, chunkStart, it, codec, parentNode);
            }

            break;
        }
    }
}

void ScfDecoder::processEntryType07(QFile &file, QDomElement &parentNode)
{
    quint16 nameLength;
    QString name;
    QDomElement itemNode = xmlData.createElement("function");

    READ_TO_VAR_NONZERO( file, nameLength);
    READ_TO_STRING(file, name, nameLength);

    APPEND_ATTRIBUTE_STR(xmlData, itemNode, "name", name);

    parentNode.appendChild(itemNode);
}

void ScfDecoder::processEntryType08(QFile &file, QDomElement &parentNode)
{
    quint8 type;
    quint16 nBlocks;
    quint16 nameLength;
    QString blockName;

    READ_TO_VAR_NONZERO( file, nBlocks);

    for(size_t i = 0; i < nBlocks; i++)
    {
        QDomElement itemNode = xmlData.createElement("block");

        READ_TO_VAR_NONZERO(file, type);
        READ_TO_VAR_NONZERO(file, nameLength);
        READ_TO_STRING(file, blockName, nameLength);

        assert( type == 0x07);

        APPEND_ATTRIBUTE_DEC(xmlData, itemNode, "index", i);
        APPEND_ATTRIBUTE_HEX(xmlData, itemNode, "type", type);
        APPEND_ATTRIBUTE_STR(xmlData, itemNode, "name", blockName);

        parentNode.appendChild(itemNode);
    }
}

void ScfDecoder::load(QString path)
{
    QFile fileIn(path);

    if( !fileIn.open(QIODevice::ReadOnly))
    {
        fatalExit("Cannot open input file!");
    }

    checkHeader(fileIn);
    createXml();
    checkLabelsHeader(fileIn);
    readLabelsSection(fileIn);
    readVariablesSection(fileIn);
    checkBlocksHeader(fileIn);
    readBlocksSection(fileIn);
    checkFunctionsHeader(fileIn);
    readCodeSection(fileIn);

    fileIn.close();
}

void ScfDecoder::saveXml(QString path)
{
    QFile outFile(path);

    if( !outFile.open(QIODevice::WriteOnly))
    {
        fatalExit("Cannot open output file!");
    }

    QTextStream xmlOutStream(&outFile);
    xmlData.save(xmlOutStream, 2);
    xmlOutStream.flush();

    outFile.close();
}

void ScfDecoder::fatalExit(char *reason)
{
    fprintf(stderr, "XML Coder operation abort! [%s]\n", reason);
    getchar();
    exit(1);
}

void ScfDecoder::createXml()
{
    root = xmlData.createElement("root");
    xmlData.appendChild(root);
}

