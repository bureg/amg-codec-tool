/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef TM2ALPHAFIXER_H
#define TM2ALPHAFIXER_H

#include <QString>
#include <stdio.h>

#include "datatypes.h"
#include "module.h"

class AlphaFixer : public Module
{
public:
    AlphaFixer();

    int fixalpha(QString inputPath, int mode);
};
#endif // TM2ALPHAFIXER_H
