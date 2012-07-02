/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef SWIZZLEPROCESSOR_H
#define SWIZZLEPROCESSOR_H

#include <QString>
#include <stdio.h>
#include <stdlib.h>

#include "datatypes.h"
#include "module.h"

class SwizzleProcessor : public Module
{
public:
    SwizzleProcessor();

    int swizzleProcess(QString inputPath, QString outputPath, int mode);

private:
    void swizzle(unsigned char *timbuf, unsigned char *newbuf, int mode);
    void alphafix(unsigned char *buf, int mode);
    void tm2Hack(unsigned char *newbuf, int fontver);
};
#endif // SWIZZLEPROCESSOR_H
