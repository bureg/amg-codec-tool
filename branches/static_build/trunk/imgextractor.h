/*
  This file is a part of the
  Amagami PS2 Translation Project

  You may feel free to use this code if you want so.
  We provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef IMGEXTRACTOR_H
#define IMGEXTRACTOR_H

#include <QString>
#include <stdio.h>
#include <stdlib.h>

#include "module.h"

class ImgExtractor : public Module
{
public:
    ImgExtractor();

    int extract(QString inputPath, QString outputPath, QString structurePath);
};
#endif // IMGEXTRACTOR_H
