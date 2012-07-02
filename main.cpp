/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include <QString>

#include "langdb.h"

#include "arcreader.h"
#include "arcwriter.h"
#include "arcpacker.h"

#include "imgextractor.h"

#include "scfdecoder.h"
#include "scfencoder.h"

#include "scfrenamer.h"
#include "xmlrenamer.h"

#include "cltprocessor.h"
#include "swizzleprocessor.h"
#include "tm2alphafixer.h"
#include "xmlsync.h"

ArcReader reader;
ArcPacker packer;
ArcWriter writer;

ImgExtractor extractor;

ScfDecoder scf2xml;
ScfEncoder xml2scf;

ScfRenamer scfrename;
XmlRenamer xmlrename;

AlphaFixer alphafix;
CltProcessor cltprocessor;
SwizzleProcessor swizzleprocessor;
XmlSync xmlsync;

void printHelp()
{
    printf("Usage:\n\n");

    printf("amg_codec_tool.exe --help\n");
    //printf("amg_codec_tool.exe <file>\n"); //-- TODO: auto mode
    printf("amg_codec_tool.exe --decode-pac <file.PAC> <file.ARC>\n");
    printf("amg_codec_tool.exe --encode-pac <file.ARC> <file.PAC>\n");
    printf("amg_codec_tool.exe --decode-arc <file.ARC> <DIR> <structure.XML>\n");
    printf("amg_codec_tool.exe --encode-arc <DIR> <structure.XML> <file.ARC>\n");
    printf("amg_codec_tool.exe --extract-img <file.IMG> <DIR> <structure.XML>\n");
    printf("amg_codec_tool.exe --decode-scf <file.SCF> <file.XML>\n");
    printf("amg_codec_tool.exe --encode-scf [--translate <LANGUAGE>] <file.XML> <file.SCF>\n");
    printf("amg_codec_tool.exe --rename-scf <--toReal or --toNum> <DIR>\n");
    printf("amg_codec_tool.exe --rename-xml <--toReal or --toNum> <DIR>\n");
    printf("amg_codec_tool.exe --cltappend <file.TM2> <GRAPH0.CLT> <DIR>\n");
    printf("amg_codec_tool.exe --cltremove <input.TM2> <output.TM2>\n");
    printf("amg_codec_tool.exe --swizzleprocess <-d or -r> <font.TM2> <outputfont.TM2>\n");
    printf("amg_codec_tool.exe --alphafix <-u or -d> <image.TM2>\n");
    printf("amg_codec_tool.exe --sync-xml <newest_in.xml> <template_in.xml> <output.xml> <-1 or -2>\n");

    exit(1);
}

int main(int argc, char *argv[])
{
    putenv("UNICODEMAP_JP=cp932"); //Encoder needs this to encode in the correct varient of S-JIS

    if(argc < 2)
    {
        fprintf(stderr, "\nERR: No parameters! See usage info below.\n\n");
        printHelp();
    }

    if(strncmp(argv[1], "--help", strlen("--help")) == 0)
    {
        printf("\n");
        printHelp();
    }
    else if(strncmp(argv[1], "--decode-pac", strlen("--decode-pac")) == 0)
    {
        if(argc != 4)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        QString inputFilePath(argv[2]);
        QString outputFilePath(argv[3]);

        packer.unpack(inputFilePath, outputFilePath);
    }
    else if (strncmp(argv[1], "--encode-pac", strlen("--encode-pac")) == 0)
    {
        if(argc != 4)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        QString inputFilePath(argv[2]);
        QString outputFilePath(argv[3]);

        packer.pack(inputFilePath, outputFilePath);
    }
    else if (strncmp(argv[1], "--encode-arc", strlen("--encode-arc")) == 0)
    {
        if(argc != 5)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        QString inputDirPath(argv[2]);
        QString inputStructureFile(argv[3]);
        QString outputFilePath(argv[4]);

        writer.loadStructure(inputStructureFile);
        writer.buildArchive(inputDirPath, outputFilePath);
    }
    else if (strncmp(argv[1], "--decode-arc", strlen("--decode-arc")) == 0)
    {
        if(argc != 5)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        QString inputFilePath(argv[2]);
        QString outputDirPath(argv[3]);
        QString outputStructureFile(argv[4]);

        reader.open(inputFilePath);
        reader.decode();
        reader.extract(outputDirPath);
        reader.saveStructure(outputStructureFile);
    }
    else if(strncmp(argv[1], "--decode-scf", strlen("--decode-scf")) == 0)
    {
        if(argc < 4)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        QString inputFilePath(argv[2]);
        QString outputFilePath(argv[3]);

        scf2xml.load(inputFilePath);
        scf2xml.saveXml(outputFilePath);
    }
    else if(strncmp(argv[1], "--encode-scf", strlen("--encode-scf")) == 0)
    {
        if(argc < 4)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        int nextParam = 2;
        if(strncmp(argv[2], "--translate", strlen("--translate")) == 0)
        {
            lang_id translateTo = getLangIdByName(argv[3]);
            if(translateTo == LANGUAGE_UNKNOWN)
            {
                fprintf(stderr, "\nERR: Unknown translation language! See usage info below.\n\n");
                printHelp();
            }

            xml2scf.setLanguage(translateTo);
        }

        QString inputFilePath(argv[nextParam]);
        QString outputFilePath(argv[nextParam + 1]);

        xml2scf.load(inputFilePath);
        xml2scf.saveScf(outputFilePath);
    }

    else if(strncmp(argv[1], "--rename-scf", strlen("--rename-scf")) == 0)
    {
        if(argc != 4)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        QString dirPath(argv[3]);

        if (strnicmp(argv[2], "--toReal", strlen("--toReal")) == 0)
        {
            printf("toReal...\n");
            scfrename.numToReal(dirPath);
        }
        else if (strnicmp(argv[2], "--toNum", strlen("--toNum")) == 0)
        {
            printf("toNum...\n");
            scfrename.realToNum(dirPath);
        }
        else
        {
            printHelp();
        }
    }
    else if(strncmp(argv[1], "--rename-xml", strlen("--rename-xml")) == 0)
    {
        if(argc != 4)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        QString dirPath(argv[3]);

        if (strnicmp(argv[2], "--toReal", strlen("--toReal")) == 0)
        {
            printf("toReal...\n");
            xmlrename.numToReal(dirPath);
        }
        else if (strnicmp(argv[2], "--toNum", strlen("--toNum")) == 0)
        {
            printf("toNum...\n");
            xmlrename.realToNum(dirPath);
        }
        else
        {
            printHelp();
        }
    }

    else if(strncmp(argv[1], "--alphafix", strlen("--alphafix")) == 0)
    {
        if(argc != 4)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        QString modeSelect(argv[2]);
        QString inputFilePath(argv[3]);

        if (modeSelect == "-u")
        {
            alphafix.fixalpha(inputFilePath, 2);
        }
        else if (modeSelect == "-d")
        {
            alphafix.fixalpha(inputFilePath, 1);
        }
        else
        {
            fprintf(stderr, "\nERR: Incorrect mode imput! See usage info below.\n\n");
            printHelp();
        }
    }
    else if(strncmp(argv[1], "--cltappend", strlen("--cltappend")) == 0)
    {
        if(argc != 5)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        char *inputFilePath=argv[2];
        char *inputCLTFilePath=argv[3];
        char *outputFilePath=argv[4];

        cltprocessor.appendClut(inputFilePath, inputCLTFilePath, outputFilePath);
    }
    else if(strncmp(argv[1], "--cltremove", strlen("--cltremove")) == 0)
    {
        if(argc != 4)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        char *inputFilePath=argv[2];
        char *outputFilePath=argv[3];

        cltprocessor.removeClut(inputFilePath, outputFilePath);
    }
    else if (strncmp(argv[1], "--extract-img", strlen("--extract-img")) == 0)
    {
        if(argc != 5)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        QString inputFilePath(argv[2]);
        QString outputDirPath(argv[3]);
        QString outputStructureFile(argv[4]);

        extractor.extract(inputFilePath, outputDirPath, outputStructureFile);
    }
    else if(strncmp(argv[1], "--sync-xml", strlen("--sync-xml")) == 0)
    {
        if(argc < 5)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        QString inputFilePath(argv[2]); //source file: containing newest strings
        QString templateFilePath(argv[3]); //template file: new strings applied to this
        QString outputFilePath(argv[4]); //output file: new file for updated template

        xmlsync.load(inputFilePath, 0);
        xmlsync.load(templateFilePath, 1);

        if(argc == 6 && (strncmp(argv[5], "-2", strlen("-2")) == 0))
        {
            xmlsync.syncStrings(2); //Slower, brute-force scan
        }
        else if(argc == 6 && (strncmp(argv[5], "-1", strlen("-1")) == 0))
        {
            xmlsync.syncStrings(1); //Faster, one-to-one scan
        }
        else
        {
            xmlsync.syncStrings(0); //Automatically pick mode
        }

        xmlsync.saveXml(outputFilePath);
    }
    else if(strncmp(argv[1], "--swizzleprocess", strlen("--swizzleprocess")) == 0)
    {
        if(argc != 5)
        {
            fprintf(stderr, "\nERR: Incorrect amount of parameters! See usage info below.\n\n");
            printHelp();
        }

        QString modeSelect(argv[2]);
        QString inputFilePath(argv[3]);
        QString outputFilePath(argv[4]);

        if (modeSelect == "-d")
        {
            swizzleprocessor.swizzleProcess(inputFilePath, outputFilePath, 0);
        }
        else if (modeSelect == "-r")
        {
            swizzleprocessor.swizzleProcess(inputFilePath, outputFilePath, 1);
        }
        else
        {
            fprintf(stderr, "\nERR: Incorrect mode imput! See usage info below.\n\n");
            printHelp();
        }
    }
    else if (argc == 3)
    {
        QString inputFilePath(argv[2]);

        if(inputFilePath.endsWith(".xml"))
        {
            QString outputFilePath = inputFilePath.replace(inputFilePath.length() - 4, 3, "scf");
            xml2scf.load(inputFilePath);
            xml2scf.saveScf(outputFilePath);
        }
        else if(inputFilePath.endsWith(".scf"))
        {
            QString outputFilePath = inputFilePath.replace(inputFilePath.length() - 4, 3, "xml");
            scf2xml.load(inputFilePath);
            scf2xml.saveXml(outputFilePath);
        }
        else if(inputFilePath.endsWith(".pac"))
        {
            QString outputFilePath = inputFilePath.replace(inputFilePath.length() - 4, 3, "arc");
            packer.unpack(inputFilePath, outputFilePath);
        }
        else
        {
            fprintf(stderr, "\nERR: Incorrect input file extension! See usage info below.\n\n");
            printHelp();
        }
    }
    else
    {
        fprintf(stderr, "\nERR: Unknown command! See usage info below.\n\n");
        printHelp();
    }

    fprintf(stderr, "\nDone!\n");
    return 0;
}
