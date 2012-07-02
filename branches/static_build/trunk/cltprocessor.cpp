/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  We provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include "cltprocessor.h"

CltProcessor::CltProcessor() : Module("cltprocessor")
{
}

int CltProcessor::appendClut(char *inputPath, char *inputCLTPath, char *outputDir)
{
    FILE *infile;
    int fsize, i;
    char clut[8192];
    char *buf;
    s_timhdr *tm2header;

    int namestart;
    QString temp, basename;

    infile = fopen(inputCLTPath, "rb");

    if (infile == NULL)
    {
        printf("\nError opening GRAPH0.CLT.\n");
        return 1;
    }

    fread(clut, 8192, 1, infile);
    fclose(infile);

    infile = fopen(inputPath, "rb");

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
        free(buf);
        return 1;
    }

    tm2header = (s_timhdr*)buf;
    if ((tm2header->file_size + 16) == fsize)
    {
        printf("\nThis TM2 already seems to have a CLUT\n");
    }

//start getting crazy
    //we want only the base filename. prepare to eliminate extraneous stuff.
    //there's probably a better way than this... whatever.
    temp=QString(inputPath);
    namestart=(temp.lastIndexOf("\\") > temp.lastIndexOf("/")) ? temp.lastIndexOf("\\") : temp.lastIndexOf("/");
    namestart++;

    i=temp.lastIndexOf("."); //check for file extension
    if (i < 0) { i=strlen(inputPath); }

    basename=QString().fromAscii(inputPath + namestart, i - namestart);
//enough crazy

    for (i = 0; i < 8; i++)
    {
        QString nametmp = QString().sprintf("%s/%s_%d.tm2", outputDir, basename.toAscii().data(), i);

        infile = fopen(nametmp.toAscii().data(), "wb");
        if (infile == NULL)
        {
            printf("Unable to create outfile file! (Does the output directory exist?)\n");
            break;
        }
        fwrite(buf, fsize, 1, infile);
        fwrite(clut+1024*i, 1024, 1, infile);

        fclose(infile);
    }

    free(buf);
    return 0;
}

int CltProcessor::removeClut(char *inputPath, char *outputPath)
{
    FILE *infile;
    int fsize;
    char *buf;
    s_timhdr *tm2header;

    infile = fopen(inputPath, "rb");

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
        free(buf);
        return 1;
    }

    tm2header = (s_timhdr*)buf;
    if (tm2header->clut_size != 1024)
    {
        //only the fonts in GRAPH0 should cause this error
        printf("\nThis TM2 does not have a 1024-byte CLUT??\n");
    }

    if ((tm2header->file_size - tm2header->clut_size + 16) == fsize)
    {
        printf("\nThis TM2 file doesn't seem to have a CLUT. Not going to proceed.\n");
        free(buf);
        return 1;
    }

    infile = fopen(outputPath, "wb");

    if (infile == NULL)
    {
        printf("Unable to create outfile file!\n");
        free(buf);
        return 1;
    }

    fwrite(buf, fsize - tm2header->clut_size, 1, infile);
    fclose(infile);
    free(buf);

    return 0;
}
