/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef CLTPROCESSOR_H
#define CLTPROCESSOR_H

#include <QString>
#include <stdio.h>
#include <stdlib.h>

#include "datatypes.h"
#include "module.h"

class CltProcessor : public Module
{
public:
    CltProcessor();

    int appendClut(char *inputPath, char *inputCLTFilePath, char *outputPath);
    int removeClut(char *inputPath, char *outputPath);
};

//Bare-minimum struct. do not re-use!
typedef struct
{
    char na[16];
    int file_size, clut_size;
    char nb[6];
    short clut_numcolors;
} s_timhdr;

#endif // CLTPROCESSOR_H
