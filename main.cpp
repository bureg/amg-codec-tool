/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include <QString>

#include "langdb.h"

#include "arcreader.h"
#include "arcwriter.h"
#include "arcpacker.h"

#include "scfdecoder.h"
#include "scfencoder.h"

ArcReader reader;
ArcPacker packer;
ArcWriter writer;

ScfDecoder scf2xml;
ScfEncoder xml2scf;

void printHelp()
{
    printf("Usage:\n");

    printf("codec_tool.exe --help\n");
    //printf("codec_tool.exe <file>\n"); //-- TODO: auto mode
    printf("codec_tool.exe --decode-pac <file.PAC> <file.ARC>\n");
    printf("codec_tool.exe --encode-pac <file.ARC> <file.PAC>\n");
    printf("codec_tool.exe --decode-arc <file.ARC> <DIR> <structure.XML>\n");
    printf("codec_tool.exe --encode-arc [--check-size] <DIR> <structure.XML> <file.ARC>\n");
    printf("codec_tool.exe --decode-scf <file.SCF> <file.XML>\n");
    printf("codec_tool.exe --encode-scf [--translate <LANGUAGE>] <file.XML> <file.SCF>\n");

    exit(1);
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "ERR: No parameters! See usage info below.\n");
        printHelp();
    }

    if(strncmp(argv[1], "--help", strlen("--help")) == 0)
    {
        printHelp();
    }
    else if(strncmp(argv[1], "--decode-pac", strlen("--decode-pac")) == 0)
    {
        if(argc != 4)
        {
            fprintf(stderr, "ERR: Incorrect amount of parameters! See usage info below.");
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
            fprintf(stderr, "ERR: Incorrect amount of parameters! See usage info below.");
            printHelp();
        }

        QString inputFilePath(argv[2]);
        QString outputFilePath(argv[3]);

        packer.pack(inputFilePath, outputFilePath);
    }
    else if (strncmp(argv[1], "--encode-arc", strlen("--encode-arc")) == 0)
    {
        if((argc != 5) && (argc != 6))
        {
            fprintf(stderr, "ERR: Incorrect amount of parameters! See usage info below.");
            printHelp();
        }

        int nextParam = 2;
        bool checkSize = false;

        if(strncmp(argv[1], "--check-size", strlen("--check-size")) == 0)
        {
            checkSize = true;
            nextParam = 3;
        }

        QString inputDirPath(argv[nextParam]);
        QString inputStructureFile(argv[nextParam + 1]);
        QString outputFilePath(argv[nextParam + 2]);

        writer.loadStructure(inputStructureFile);
        writer.buildArchive(inputDirPath, outputFilePath, checkSize);
    }
    else if (strncmp(argv[1], "--decode-arc", strlen("--decode-arc")) == 0)
    {
        if(argc != 5)
        {
            fprintf(stderr, "ERR: Incorrect amount of parameters! See usage info below.");
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
            fprintf(stderr, "ERR: Incorrect amount of parameters! See usage info below.");
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
            fprintf(stderr, "ERR: Incorrect amount of parameters! See usage info below.");
            printHelp();
        }

        int nextParam = 2;
        if(strncmp(argv[2], "--translate", strlen("--translate")) == 0)
        {
            lang_id translateTo = getLangIdByName(argv[3]);
            if(translateTo == LANGUAGE_UNKNOWN)
            {
                fprintf(stderr, "ERR: Unknown translation language! See usage info below.");
                printHelp();
            }

            xml2scf.setLanguage(translateTo);
            nextParam = 4;
        }

        QString inputFilePath(argv[nextParam]);
        QString outputFilePath(argv[nextParam + 1]);

        xml2scf.load(inputFilePath);
        xml2scf.saveScf(outputFilePath);
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
            fprintf(stderr, "ERR: Incorrect input file extension! See usage info below.");
            printHelp();
        }
    }
    else
    {
        fprintf(stderr, "ERR: Unknows options! See usage info below.");
        printHelp();
    }

    fprintf(stderr, "Done!\n");
    return 0;
}
