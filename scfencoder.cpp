/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include <QTextCodec>

#include "langdb.h"
#include "scfencoder.h"

ScfEncoder::ScfEncoder() : Module("ScfEncoder"), lang(LANGUAGE_JAP)
{
}

void ScfEncoder::load(QString path)
{
    QFile structFile(path);

    if(!structFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        fatalExit("Failed to open input file!");
    }

    /* Load all data */
    xmlData.setContent(structFile.readAll());

    structFile.close();

    // Get root
    root = xmlData.documentElement();

    if(root.isNull())
    {
        fatalExit("Root of the input XML file was not detected!");
    }
}

void ScfEncoder::outputScfHeader(QFile &file)
{
    WRITE_HEADER(file, header, HEADER_LENGTH);
}

void ScfEncoder::outputLabelsHeader(QFile &file)
{
    WRITE_HEADER(file, labelsSectionHeader, LABELS_HEADER_LENGTH);
}

void ScfEncoder::outputBlocksHeader(QFile &file)
{
    WRITE_HEADER(file, blocksSectionHeader, BLOCKS_HEADER_LENGTH);
}

void ScfEncoder::outputFunctionsHeader(QFile &file)
{
    WRITE_HEADER(file, functionsSectionHeader, FUNCTIONS_HEADER_LENGTH);
}

void ScfEncoder::outputLabels(QFile &file)
{
    QDomElement labelNode;
    QDomElement labelsNode;

    GET_NODE(root, "labels", labelsNode);
    GET_NODE(labelsNode, "label", labelNode);

    size_t counter = 0;

    while(!labelNode.isNull())
    {
        size_t index;
        QString name;

        ATTRIBUTE_TO_UINT(labelNode, "index", index, size_t);
        ATTRIBUTE_TO_STRING_NOT_EMPTY(labelNode, "name", name);

        assert(index == counter);

        WRITE_STRING_WITH_LENGTH(file, name);

        counter++;
        labelNode = labelNode.nextSiblingElement("label");
    }
}

void ScfEncoder::outputVariables(QFile &file)
{
    QDomElement varsNode;
    QDomElement varNode;

    GET_NODE(root, "vars", varsNode);
    GET_NODE(varsNode, "var", varNode);

    size_t counter = 0;
    quint16 totalEntries = varsNode.childNodes().count();

    WRITE_VAR(file, totalEntries);

    while(!varNode.isNull())
    {
        size_t index;
        quint8 varId;
        QString name;

        ATTRIBUTE_TO_UINT(varNode, "index", index, size_t);
        ATTRIBUTE_TO_UINT(varNode, "id", varId, quint8);
        ATTRIBUTE_TO_STRING_NOT_EMPTY(varNode, "name", name);

        assert(index == counter);

        WRITE_VAR(file, varId);
        WRITE_STRING_WITH_LENGTH(file, name);

        counter++;
        varNode = varNode.nextSiblingElement("var");
    }
}

void ScfEncoder::outputBlocks(QFile &file)
{
    QDomElement blocksNode;
    QDomElement blockNode;

    GET_NODE(root, "blocks", blocksNode);
    GET_NODE(blocksNode, "block", blockNode);

    size_t counter = 0;
    quint16 totalEntries = blocksNode.childNodes().count();

    WRITE_VAR(file, totalEntries);

    while(!blockNode.isNull())
    {
        size_t index;
        quint16 isMain;
        quint16 dataLength;
        QString name;
        QByteArray blockData;
        QDomElement blockDataNode;

        ATTRIBUTE_TO_UINT(blockNode, "index", index, size_t);
        ATTRIBUTE_TO_UINT(blockNode, "isMain", isMain, quint16);
        ATTRIBUTE_TO_UINT(blockNode, "dataLength", dataLength, quint16);
        ATTRIBUTE_TO_STRING_NOT_EMPTY(blockNode, "name", name);

        GET_NODE(blockNode, "data", blockDataNode);
        GET_NODE_BINARY_DATA(blockDataNode, blockData);

        assert(index == counter);
        assert(blockData.length() == dataLength);

        WRITE_STRING_WITH_LENGTH(file, name);
        WRITE_VAR(file, isMain);
        WRITE_VAR(file, dataLength);
        WRITE_BINARY_DATA(file, blockData);

        counter++;
        blockNode = blockNode.nextSiblingElement("block");
    }
}

void ScfEncoder::processEntryType03(QFile &file, QDomElement &parentNode)
{
    QDomElement parameterNode;
    quint32 value;

    GET_NODE(parentNode, "parameter", parameterNode);
    ATTRIBUTE_TO_UINT(parameterNode, "value", value, quint32);
    WRITE_VAR(file, value);
}

void ScfEncoder::convertText(QByteArray &data, QDomElement &parentNode, QTextCodec * codec)
{
    if( getLanguage() == LANGUAGE_JAP)
    {
        QDomElement originalNode;
        QString originalText;

        GET_NODE(parentNode, "original", originalNode);
        GET_NODE_TEXT(originalNode, originalText);

        data.append(codec->fromUnicode(originalText));
    }
    else if (getLanguage() == LANGUAGE_ENG)
    {
        QDomElement translationNode;
        QString translationText;

        GET_NODE(parentNode, "translation", translationNode);

        bool translationEnabled = false;
        while(!translationNode.isNull())
        {
            QString translationLang;
            QString mode;

            ATTRIBUTE_TO_STRING_NOT_EMPTY(translationNode, "lang", translationLang);
            ATTRIBUTE_TO_STRING_NOT_EMPTY(translationNode, "mode", mode);

            if(translationLang == QString("eng"))
            {
                translationEnabled = (mode == QString("on")) ? true : false;
                break;
            }

            translationNode = translationNode.nextSiblingElement("translation");
        }

        if(!translationNode.isNull() && translationEnabled)
        {
            GET_NODE_TEXT(translationNode, translationText);
            convertToSjis(LANGUAGE_ENG, data, translationText);
        }
        else
        {
            // Use default
            QDomElement originalNode;
            QString originalText;

            GET_NODE(parentNode, "original", originalNode);
            GET_NODE_TEXT(originalNode, originalText);

            data.append(codec->fromUnicode(originalText));
        }
    } /* LANGUAGE_ENG */
}

void ScfEncoder::processEntryType05(QFile &file, QDomElement &parentNode)
{
    QDomElement entryNode;
    QByteArray data;
    QTextCodec *codec = QTextCodec::codecForName("Shift-JIS");

    GET_FIRST_CHILD_NODE(parentNode, entryNode);

    size_t counter = 0;

    while(!entryNode.isNull())
    {
        size_t index;

        ATTRIBUTE_TO_UINT(entryNode, "index", index, size_t);

        assert(index == counter);

        if(entryNode.tagName() == QString("text"))
        {
             convertText(data, entryNode, codec);
        }
        else if (entryNode.tagName() == QString("ctrl"))
        {
            QString ctrlString;

            ATTRIBUTE_TO_STRING_NOT_EMPTY(entryNode, "value", ctrlString);
            data.append(ctrlString);
        }
        else
        {
            fatalExit("Unknown tag (not 'text' or 'ctrl')");
        }

        counter++;
        entryNode = entryNode.nextSiblingElement();
    }
    WRITE_BINARY_DATA_WITH_LENGTH(file, data);
}

void ScfEncoder::processEntryType07(QFile &file, QDomElement &parentNode)
{
    QDomElement functionNode;
    QString name;

    GET_NODE(parentNode, "function", functionNode);
    ATTRIBUTE_TO_STRING_NOT_EMPTY(functionNode, "name", name);
    WRITE_STRING_WITH_LENGTH(file, name);
}

void ScfEncoder::processEntryType08(QFile &file, QDomElement &parentNode)
{
    QDomElement blockNode;

    GET_NODE(parentNode, "block", blockNode);

    size_t counter = 0;
    quint16 totalEntries = parentNode.childNodes().count();

    WRITE_VAR(file, totalEntries);

    while(!blockNode.isNull())
    {
        size_t index;
        quint8 entryType;
        QString name;

        ATTRIBUTE_TO_UINT(blockNode, "index", index, size_t);
        ATTRIBUTE_TO_UINT(blockNode, "type", entryType, quint8);
        ATTRIBUTE_TO_STRING_NOT_EMPTY(blockNode, "name", name);

        assert(index == counter);

        WRITE_VAR(file, entryType);
        WRITE_STRING_WITH_LENGTH(file, name);

        counter++;
        blockNode = blockNode.nextSiblingElement("block");
    }
}

void ScfEncoder::outputCodeSection(QFile &file)
{
    QDomElement codeNode;
    QDomElement entryNode;

    GET_NODE(root, "code", codeNode);
    GET_NODE(codeNode, "entry", entryNode);

    size_t counter = 0;
    quint16 totalEntries = codeNode.childNodes().count();

    WRITE_VAR(file, totalEntries);

    while(!entryNode.isNull())
    {
        size_t index;
        quint8 entryType;

        ATTRIBUTE_TO_UINT(entryNode, "index", index, size_t);
        ATTRIBUTE_TO_UINT(entryNode, "type", entryType, quint8);

        assert(index == counter);

        switch (entryType)
        {
            case ENTRY_TYPE_01: /* Parameter */
            case ENTRY_TYPE_03: /* Parameter */
                WRITE_VAR(file, entryType);
                processEntryType03(file, entryNode);
                break;

            case ENTRY_TYPE_05: /* Text */
                WRITE_VAR(file, entryType);
                processEntryType05(file, entryNode);
                break;

            case ENTRY_TYPE_06: /* Function */
            case ENTRY_TYPE_07: /* Function */
                WRITE_VAR(file, entryType);
                processEntryType07(file, entryNode);
                break;

            case ENTRY_TYPE_08: /* Blocks list */
                WRITE_VAR(file, entryType);
                processEntryType08(file, entryNode);
                break;

            default: /* Unknown type */
                fatalExit("Unknown entry type");
        }

        counter++;
        entryNode = entryNode.nextSiblingElement("entry");
    }
}

void ScfEncoder::saveScf(QString path)
{
    QFile fileOut(path);

    if( !fileOut.open(QIODevice::WriteOnly))
    {
        fatalExit("Cannot open output file!");
    }

    outputScfHeader(fileOut);
    outputLabelsHeader(fileOut);
    outputLabels(fileOut);
    outputVariables(fileOut);
    outputBlocksHeader(fileOut);
    outputBlocks(fileOut);
    outputFunctionsHeader(fileOut);
    outputCodeSection(fileOut);

    fileOut.close();
}

void ScfEncoder::setLanguage(lang_id lang)
{
    this->lang = lang;
}

lang_id ScfEncoder::getLanguage()
{
    return this->lang;
}
