/*
  This file is a part of the
  Amagami PS2 Translation Project

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include <QTextCodec>

#include "langdb.h"
#include "xmlsync.h"

/*nomenclature for this file:

"in" refers to the "input" xml
"out" refers to the "template" xml

As for the "output" file, it's
created at the very end of the process.*/

XmlSync::XmlSync() : Module("XmlSync"), lang(LANGUAGE_JAP)
{
}

size_t XmlSync::countNodeChildren(QDomElement parentNode, QString childTagName)
{
    size_t i = 0;
    QDomElement currentNode = parentNode.firstChildElement(childTagName);

    while (!currentNode.isNull())
    {
        if (currentNode.tagName().compare(childTagName, Qt::CaseInsensitive) == 0)
        {
            i++;
        }

        currentNode=currentNode.nextSiblingElement(childTagName);
    }

    return i;
}

void XmlSync::load(QString path, quint32 order)
{
    QFile structFile(path);
    QXmlSimpleReader simpleread;
    QXmlInputSource input;
    QDomDocument *xmlData;
    QDomElement *root;

    if(!structFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        fatalExit("Failed to open input file!");
    }

    if (order == 0) //Input file
    {
        xmlData = &xmlData_in;
        root = &root_in;
    }
    else //Template file
    {
        xmlData = &xmlData_out;
        root = &root_out;
    }

    //Use QXmlSimpleReader to read document since the default kills whitespace nodes which are needed.
    simpleread.setFeature(QLatin1String("http://trolltech.com/xml/features/report-whitespace-only-CharData"), true);
    input.setData(structFile.readAll());
    xmlData->setContent(&input, &simpleread);

    structFile.close();

    // Get root
    *root = xmlData->documentElement();

    if(root->isNull())
    {
        fatalExit("Root of the input XML file was not detected!");
    }
}

void XmlSync::processEntryType05(QDomElement &parentNode, QByteArray *data)
{
    QDomElement entryNode = parentNode.firstChildElement();
    QTextCodec *codec = QTextCodec::codecForName("Shift-JIS");

    while(!entryNode.isNull())
    {
        if(entryNode.tagName() == QString("text"))
        {
            QDomElement originalNode;
            QString originalText;

            GET_NODE(entryNode, "original", originalNode);
            GET_NODE_TEXT(originalNode, originalText);

            data->append(codec->fromUnicode(originalText));
        }
        else if (entryNode.tagName() == QString("ctrl"))
        {
            QString ctrlString;

            ATTRIBUTE_TO_STRING_NOT_EMPTY(entryNode, "value", ctrlString);
            data->append(ctrlString);
        }
        else
        {
            fatalExit("Unknown tag (not 'text' or 'ctrl')");
        }

        entryNode = entryNode.nextSiblingElement();
    }
}

void XmlSync::replaceEntries(quint32 in_index, quint32 out_index)
{
    QDomElement codeNode_in, codeNode_out;
    QDomElement entryNode_in, entryNode_out;
    QDomNode res;
    quint32 i;

    //Get to the nodes
    GET_NODE(root_in, "code", codeNode_in);
    GET_NODE(root_out, "code", codeNode_out);
    GET_FIRST_CHILD_NODE(codeNode_in, entryNode_in);
    GET_FIRST_CHILD_NODE(codeNode_out, entryNode_out);

    //maybe include warning about non-equal indexes?

    for (i = 0; i < in_index; i++)
    {
        entryNode_in=entryNode_in.nextSiblingElement();
    }

    for (i = 0; i < out_index; i++)
    {
        entryNode_out=entryNode_out.nextSiblingElement();
    }

    if (out_index != in_index)
    {
        //index consistency fix -- easy, hopefully safe.
        QDomAttr index_attr = entryNode_in.attributeNode("index");
        index_attr.setNodeValue(QString().sprintf("%d", out_index));
    }

    res = codeNode_out.replaceChild(entryNode_in.cloneNode(), entryNode_out);

    if (res.isNull())
    {
        //fatalExit("failed to replace entry node");
        printf("WARNING: Failed to replace entry node\n");
    }
}

#ifdef DEBUGGING
//show entry index sequences -- for debug purposes.
void XmlSync::check1()
{
    QDomElement codeNode_in, codeNode_out;
    QDomElement entryNode_in, entryNode_out;
    quint32 i, index;

    GET_NODE(root_in, "code", codeNode_in);
    GET_NODE(root_out, "code", codeNode_out);
    GET_FIRST_CHILD_NODE(codeNode_in, entryNode_in);
    GET_FIRST_CHILD_NODE(codeNode_out, entryNode_out);

    for (i = 0; i < 30; i++)
    {
        ATTRIBUTE_TO_UINT(entryNode_in, "index", index, quint32);
        printf("%d ", index);
        entryNode_in = entryNode_in.nextSiblingElement();
    }

    printf("\n\n");

    for (i = 0; i < 30; i++)
    {
        ATTRIBUTE_TO_UINT(entryNode_out, "index", index, quint32);
        printf("%i ", index);
        entryNode_out=entryNode_out.nextSiblingElement();
    }

    printf("\n\n");
}
#endif

quint32 XmlSync::bytecmp_custom(QByteArray &str1, QByteArray &str2)
{
    /*This is used in place of a standard qstrcmp/memcmp function which
    annonyingly denies a match in at least one type of script edit,
    which is currently the Rubi control code.*/

    quint32 len1 = str1.size();
    quint32 len2 = str2.size();
	quint32 i, in_rubi;

    in_rubi = 0;

    if (len1 != len2)
	{
        return 1; //Failed
	}

    for (i = 0; i < len1; i++)
	{
		if (str1.data()[i] != str2.data()[i])
        {
            return 1; //Failed
        }

        if (((quint8*)str1.data())[i] < 0x80) //ASCII
        {
            if (((quint8*)str1.data())[i] == 'R') //Avoid Rubi code
            {
                if(in_rubi == 1)
                {
                    i++; //Skip a single ASCII digit.
                }

                in_rubi ^= 1;
            }
        }
        else
        {
            i++; //pass sjis
        }
	}

	return 0; //both inputs match
}

void XmlSync::syncStrings(quint32 scanMode)
{
    /*This implementation runs through the nodes in both xml files,
    generating byte arrays for all type05 entries. these byte arrays
    will be used to locate matching entries between the xml files.
    When a match is found, the contents in the "out"/template node
    will be replaced with the corresponding data from the "in" xml.*/

    quint32 numEntries_in, numEntries_out, i, o;
    quint32 stats_numEntries=0, stats_numMatched=0;
    quint32 entryType_in, entryType_out;
    QDomElement codeNode_in, codeNode_out;
    QDomElement entryNode_in, entryNode_out;
    QByteArray *byte_in, *byte_out;

    GET_NODE(root_in, "code", codeNode_in);
    GET_NODE(root_out, "code", codeNode_out);
    GET_FIRST_CHILD_NODE(codeNode_in, entryNode_in);
    GET_FIRST_CHILD_NODE(codeNode_out, entryNode_out);

    numEntries_in=countNodeChildren(codeNode_in, "entry");
    numEntries_out=countNodeChildren(codeNode_out, "entry");
    byte_in = new QByteArray[numEntries_in]; //at the most
    byte_out = new QByteArray[numEntries_out]; //at the most

    //check1();

    for(i = 0; i < numEntries_in; i++) //build bytearray from "in"
    {
        ATTRIBUTE_TO_UINT(entryNode_in, "type", entryType_in, quint32);

        if (entryType_in == 5)
        {
            processEntryType05(entryNode_in, &byte_in[i]);
        }

        entryNode_in = entryNode_in.nextSiblingElement();
    }

    for(i = 0; i < numEntries_out; i++) //build bytearray from "out"
    {
        ATTRIBUTE_TO_UINT(entryNode_out, "type", entryType_out, quint32);

        if (entryType_out == 5)
        {
            processEntryType05(entryNode_out, &byte_out[i]);
            stats_numEntries++;
        }

        entryNode_out = entryNode_out.nextSiblingElement();
    }

    //scanMode handling
    if (scanMode==0)
    {
        if (numEntries_in == numEntries_out)
        {
            scanMode = 1;
        }
        else
        {
            scanMode = 2;
        }
    }

#if 0
    //method 0: BLIND REPLACE <-- testing only!
    printf("Using scan method 0: Blind replacement\n\n");
    for (i=0; i<numEntries_out; i++)
    {
        replaceEntries(i,i);
    }
#endif

    //Compare byte arrays

    if (scanMode==1)
    {
        //Method 1: NOT-BLIND REPLACE for complementary indexes

        printf("Using scan method 1: 1-to-1\n\n");

        if (numEntries_in!=numEntries_out)
        {
            printf("WARNING: Non-equal number of entries between files! Tool will stop early.\n");

            if (numEntries_in > numEntries_out)
            {
                numEntries_in = numEntries_out;
            }
            else
            {
                numEntries_out = numEntries_in;
            }
        }

        for (i = 0; i < numEntries_out; i++)
        {
            if (byte_out[i].size()>0 && byte_in[i].size()>0)
            {
                //if (qstrcmp(byte_out[i], byte_in[i])==0)
                if (bytecmp_custom(byte_out[i], byte_in[i])==0)
                {
                    //printf("match at %d\n", i);
                    replaceEntries(i,i);
                    stats_numMatched++;
                }

                else
                {
                    printf("NOTE: Skipped index %d due to non-match\n", i);
                }
            }
        }
    }

    else if (scanMode==2)
    {
        //Method 2: SEARCH AND REPLACE of the brute force variety

        printf("Using scan method 2: Search and replace\n\n");

        for (o = 0; o < numEntries_out; o++)
        {
            for (i = 0; i < numEntries_in; i++)
            {
                if (byte_out[o].size() > 0 && byte_in[i].size() > 0)
                {
                    if (bytecmp_custom(byte_out[o], byte_in[i]) == 0)
                    {
                        //printf("match at O:%d / I:%d\n", o, i);
                        replaceEntries(i,o);
                        stats_numMatched++;
                        break;
                    }
                }
            }
        }
    }

    else
    {
        printf("Invalid scan mode\n");
    }

//check1();
printf("\nxmlsync: %d of %d entries matched\n", stats_numMatched, stats_numEntries);
}

void XmlSync::saveXml(QString path)
{
    QFile outFile(path);

    if( !outFile.open(QIODevice::WriteOnly))
    {
        fatalExit("Cannot open output file!");
    }

    QTextStream xmlOutStream(&outFile);
    xmlData_out.save(xmlOutStream, 2);
    xmlOutStream.flush();

    outFile.close();
}
