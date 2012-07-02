/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  We provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include "tm2alphafixer.h"

AlphaFixer::AlphaFixer() : Module("AlphaFixer")
{
}

int AlphaFixer::fixalpha(QString inputPath, int mode)
{
    FILE *infile;
    char buf[4];
    unsigned int clut[256];
    unsigned int temp;
    int pos, i;

    infile = fopen(inputPath.toAscii(), "rb+");

    if(infile == NULL)
    {
        printf("\nFailed to open TM2 image file.\n");
        return 1;
    }

    fread(buf, 4, 1, infile);

    if(memcmp(buf, "TIM2", 4)!= 0)
    {
        printf("\nThis is not a TM2 image\n");
        return 1;
    }

    fseek(infile, 0, SEEK_END);
    pos = ftell(infile) - 1024;
    fseek(infile, pos, SEEK_SET);
    fread(clut, 1024, 1, infile);

    if(mode == 1)  //from 0-255 to 0-128
    {
        printf("\nDownsampling TM2 image...\n");

        for(i = 0; i < 256; i++)
        {
            temp = (((clut[i] & 0xFF000000) >> 24) + 1) >> 1;
            clut[i] = (clut[i] & 0x00FFFFFF) | (temp << 24);
        }
    }

    else if(mode == 2)  //from 0-128 to 0-255
    {
        printf("\nUpsampling TM2 image...\n");

        for(i = 0; i < 256; i++)
        {
            temp = (((clut[i] & 0xFF000000) >> 24) + 0) << 1;

            if (temp > 255)
            {
                temp = 255;
            }

            clut[i] = (clut[i] & 0x00FFFFFF) | (temp << 24);
        }
    }

    else
    {
        fatalExit("Mode is set wrongly.");
    }

    fseek(infile, pos, SEEK_SET);
    fwrite(clut, 1024, 1, infile);

    fclose(infile);

    return 0;
}
