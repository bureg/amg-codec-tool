/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  We provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#include "swizzleprocessor.h"

SwizzleProcessor::SwizzleProcessor() : Module("SwizzleProcessor")
{
}

//This comes from file_0452.tm2 (Regular GRAPH0.arc)
char fontclut1[256]=
{
    0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x08, 0xFF, 0xFF, 0xFF, 0x11, 0xFF, 0xFF, 0xFF, 0x19,
    0xFF, 0xFF, 0xFF, 0x22, 0xFF, 0xFF, 0xFF, 0x2A, 0xFF, 0xFF, 0xFF, 0x33, 0xFF, 0xFF, 0xFF, 0x3B,
    0xFF, 0xFF, 0xFF, 0x4C, 0xFF, 0xFF, 0xFF, 0x55, 0xFF, 0xFF, 0xFF, 0x5D, 0xFF, 0xFF, 0xFF, 0x5D,
    0xFF, 0xFF, 0xFF, 0x66, 0xFF, 0xFF, 0xFF, 0x6E, 0xFF, 0xFF, 0xFF, 0x77, 0xFF, 0xFF, 0xFF, 0x80,
    0xC8, 0xC8, 0xFF, 0x00, 0xC8, 0xC8, 0xFF, 0x08, 0xC8, 0xC8, 0xFF, 0x11, 0xC8, 0xC8, 0xFF, 0x19,
    0xC8, 0xC8, 0xFF, 0x22, 0xC8, 0xC8, 0xFF, 0x2A, 0xC8, 0xC8, 0xFF, 0x33, 0xC8, 0xC8, 0xFF, 0x3B,
    0xC8, 0xC8, 0xFF, 0x4C, 0xC8, 0xC8, 0xFF, 0x55, 0xC8, 0xC8, 0xFF, 0x5D, 0xC8, 0xC8, 0xFF, 0x5D,
    0xC8, 0xC8, 0xFF, 0x66, 0xC8, 0xC8, 0xFF, 0x6E, 0xC8, 0xC8, 0xFF, 0x77, 0xC8, 0xC8, 0xFF, 0x80,
    0xFF, 0xBE, 0xFF, 0x00, 0xFF, 0xBE, 0xFF, 0x08, 0xFF, 0xBE, 0xFF, 0x11, 0xFF, 0xBE, 0xFF, 0x19,
    0xFF, 0xBE, 0xFF, 0x22, 0xFF, 0xBE, 0xFF, 0x2A, 0xFF, 0xBE, 0xFF, 0x33, 0xFF, 0xBE, 0xFF, 0x3B,
    0xFF, 0xBE, 0xFF, 0x4C, 0xFF, 0xBE, 0xFF, 0x55, 0xFF, 0xBE, 0xFF, 0x5D, 0xFF, 0xBE, 0xFF, 0x5D,
    0xFF, 0xBE, 0xFF, 0x66, 0xFF, 0xBE, 0xFF, 0x6E, 0xFF, 0xBE, 0xFF, 0x77, 0xFF, 0xBE, 0xFF, 0x80,
    0x3F, 0x3F, 0x3F, 0x00, 0x3F, 0x3F, 0x3F, 0x08, 0x3F, 0x3F, 0x3F, 0x11, 0x3F, 0x3F, 0x3F, 0x19,
    0x3F, 0x3F, 0x3F, 0x22, 0x3F, 0x3F, 0x3F, 0x2A, 0x3F, 0x3F, 0x3F, 0x33, 0x3F, 0x3F, 0x3F, 0x3B,
    0x3F, 0x3F, 0x3F, 0x4C, 0x3F, 0x3F, 0x3F, 0x55, 0x3F, 0x3F, 0x3F, 0x5D, 0x3F, 0x3F, 0x3F, 0x5D,
    0x3F, 0x3F, 0x3F, 0x66, 0x3F, 0x3F, 0x3F, 0x6E, 0x3F, 0x3F, 0x3F, 0x77, 0x3F, 0x3F, 0x3F, 0x80
};

//This comes from file_0453.tm2 (Regular GRAPH0.arc)
char fontclut2[256]=
{
    0x00, 0x00, 0x00, 0x00, 0x46, 0x46, 0x46, 0x12, 0x46, 0x46, 0x46, 0x22, 0x46, 0x46, 0x46, 0x3D,
    0x46, 0x46, 0x46, 0x53, 0x46, 0x46, 0x46, 0x5F, 0x46, 0x46, 0x46, 0x72, 0x46, 0x46, 0x46, 0x80,
    0x57, 0x57, 0x57, 0x80, 0x6F, 0x6F, 0x6F, 0x80, 0x8A, 0x8A, 0x8A, 0x80, 0xA5, 0xA5, 0xA5, 0x80,
    0xBD, 0xBD, 0xBD, 0x80, 0xD8, 0xD8, 0xD8, 0x80, 0xF0, 0xF0, 0xF0, 0x80, 0xFF, 0xFF, 0xFF, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x46, 0x46, 0x1E, 0x46, 0x46, 0x46, 0x3C,
    0x46, 0x46, 0x46, 0x5D, 0x46, 0x46, 0x46, 0x7F, 0x57, 0x57, 0x57, 0x80, 0x44, 0x44, 0x44, 0x80,
    0x79, 0x79, 0x79, 0x80, 0x8A, 0x8A, 0x8A, 0x80, 0xA5, 0xA5, 0xA5, 0x80, 0xBD, 0xBD, 0xBD, 0x80,
    0xCE, 0xCE, 0xCE, 0x80, 0xDF, 0xDF, 0xDF, 0x80, 0xF0, 0xF0, 0xF0, 0x80, 0xFF, 0xFF, 0xFF, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4B, 0x4B, 0xC3, 0x80, 0x53, 0x53, 0xC3, 0x80,
    0x69, 0x69, 0xCD, 0x80, 0x7B, 0x7B, 0xD3, 0x80, 0x8F, 0x8F, 0xDA, 0x80, 0x9F, 0x9F, 0xDF, 0x80,
    0xBC, 0xBC, 0xE9, 0x80, 0xCF, 0xCF, 0xEF, 0x80, 0xDB, 0xDB, 0xF3, 0x80, 0xE7, 0xE7, 0xF7, 0x80,
    0xF3, 0xF3, 0xFB, 0x80, 0xFF, 0xFF, 0xFF, 0x80, 0x4B, 0x4B, 0xC3, 0x80, 0x4B, 0x4B, 0xC3, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x3D,
    0x00, 0x00, 0x00, 0x53, 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x72, 0x00, 0x00, 0x00, 0x80,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80,
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80
};


//Bare-minimum struct. do not re-use!
typedef struct
{
    char na[16];
    int file_size, clut_size;
    char nb[6];
    short clut_numcolors;
} s_timhdr;

int SwizzleProcessor::swizzleProcess(QString inputPath, QString outputPath, int mode)
{
    FILE *infile;
    unsigned char *timbuf, *newbuf;
    int fontver;

    infile=fopen(inputPath.toAscii(), "rb");

    if(infile == NULL)
    {
        printf("Error: Opening failed.\n");
        return 1;
    }

    timbuf = (unsigned char*)malloc(1473344);
    newbuf = (unsigned char*)malloc(1473344);

    fread(timbuf, 1473344, 1, infile);
    fclose(infile);

    swizzle(timbuf, newbuf, mode);
    alphafix(newbuf, mode);

    if(mode == 1)
    {
        fontver = (timbuf[1473088] == 0) ? 1 : 0; //quickly check whether it's file 452 or 453...
        printf("%sitalic font detected\n", fontver?"":"non-");
        tm2Hack(newbuf, fontver);
    }

    infile = fopen(outputPath.toAscii(),"wb");
    fwrite(newbuf, 1473344, 1, infile);
    fclose(infile);

    free(timbuf);
    free(newbuf);

    return 0;
}

void SwizzleProcessor::swizzle(unsigned char *timbuf, unsigned char *newbuf, int mode)
{
    unsigned int i,j,k;
    unsigned char *obuf, *pbuf;

    memcpy(newbuf, timbuf, 64);
    memcpy(newbuf + 1473088, timbuf + 1473088, 256);

    obuf = timbuf + 64;
    pbuf = newbuf + 64;

    for(k = 0; k < 4368; k++)  //Glyphs
        for(j = 0; j < 28; j++)  //Height
            for(i = 0; i < 12; i++)  //Width
            {
                if(mode == 0)
                {
                    pbuf[(336 * j + ((k / 28) * (336 * 28)) + (k % 28) * 12 + i)] = obuf[336 * k + 12 * j + i];    //unswizzle
                }
                else if(mode == 1)
                {
                    pbuf[336 * k + 12 * j + i] = obuf[(336 * j + ((k / 28) * (336 * 28)) + (k % 28) * 12 + i)];    //reswizzle
                }
            }
}

void SwizzleProcessor::alphafix(unsigned char *buf, int mode)
{
    unsigned int i, temp;
    s_timhdr *timhdr = (s_timhdr*)buf;
    unsigned int *clut = (unsigned int*)((char*)buf + timhdr->file_size - timhdr->clut_size + 16);

    if(mode == 0)  //From 0-128 to 0-255
    {
        for(i = 0; i < (int)timhdr->clut_numcolors; i++)
        {
            temp = (((clut[i] & 0xFF000000) >> 24) + 0) << 1;

            if(temp > 255)
            {
                temp = 255;
            }

            clut[i] = (clut[i] & 0x00FFFFFF) | (temp << 24);
        }
    }

    else if(mode == 1)  //From 0-255 to 0-128
    {
        for(i = 0; i < (int)timhdr->clut_numcolors; i++)
        {
            temp = (((clut[i] & 0xFF000000) >> 24) + 1) >> 1;
            clut[i] = (clut[i] & 0x00FFFFFF) | (temp << 24);
        }
    }
}

void SwizzleProcessor::tm2Hack(unsigned char *newbuf, int fontver)
{
    s_timhdr *timhdr = (s_timhdr*)newbuf;
    char *fontclut;

    if (fontver == 0) //non-italic font's palette
    {
        fontclut = fontclut1;
    }
    else //italic font's palette
    {
        fontclut = fontclut2;
    }

    timhdr->file_size = 1473344 - 16;
    timhdr->clut_numcolors = 64;
    timhdr->clut_size = timhdr->clut_numcolors * 4;

    memcpy(newbuf + 1473088, fontclut, timhdr->clut_size);
}