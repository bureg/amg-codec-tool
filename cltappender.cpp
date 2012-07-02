/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  Added by Anonymous and ported by *Name redacted*

  You may feel free to use this code if you want so.
  We provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include "cltappender.h"

CLTAppender::CLTAppender() : Module("CLTAppender")
{
}

int CLTAppender::append(QString inputPath, QString inputCLTPath, QString outputPath)
{
    /* Original code was written in C by Anonymous. Ported here without any
    major problems with slight modifications made. Some native Qt types used
    but nothing major beyond that */
    FILE *infile;
    int fsize, i, nameSize = outputPath.append("\\").toAscii().size() + inputPath.toAscii().size();
    char nametmp[nameSize], basename[nameSize];
    char clut[8192];
    char *buf;

    infile = fopen(inputCLTPath.toAscii(),"rb");

    if (infile == NULL)
    {
        printf("\nError opening GRAPH0.CLT.\n");
        return 1;
    }

    fread(clut, 8192, 1, infile);
    fclose(infile);

    infile = fopen(inputPath.toAscii(), "rb");

    if (infile == NULL)
    {
        printf("\nError opening .TM2 image.\n");
        return 1;
    }

    fseek(infile, 0, SEEK_END);
    fsize = ftell(infile);
    fseek(infile, 0, SEEK_SET);

    buf = (char*)malloc(fsize);
    fread(buf, fsize, 1, infile);
    fclose(infile);

    if (memcmp(buf, "TIM2", 4)!= 0)
    {
        printf("\nFile is not a TIM2 image.\n");
        return 1;
    }

    for (i = strlen(inputPath.toAscii()) - 1; i > 0; i--)
    {
        if (inputPath.toAscii()[i] == '.')
        {
            break;
        }
    }

    if (i == 0)
    {
        i = strlen(inputPath.toAscii());
    }

    memset(basename, 0, sizeof(basename));
    strncpy(basename, outputPath.append("\\").toAscii(), outputPath.append("\\").toAscii().size());
    strncat(basename, inputPath.toAscii(), i);

    for (i = 0; i < 8; i++)
    {
        sprintf(nametmp, "%s_%d.tm2", basename, i);

        infile = fopen(nametmp, "wb");
        fwrite(buf, fsize, 1, infile);
        fwrite(clut+1024*i, 1024, 1, infile);

        fclose(infile);
    }

    return 0;
}
