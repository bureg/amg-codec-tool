#ifndef CLTAPPENDER_H
#define CLTAPPENDER_H

#include <QString>
#include <stdio.h>

#include "datatypes.h"
#include "module.h"

class CLTAppender : public Module
{
public:
    CLTAppender();

    int append(QString inputPath, QString inputCLTFilePath, QString outputPath);
};

#endif // CLTAPPENDER_H
