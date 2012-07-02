#ifndef SCFRENAMER_H_INCLUDED
#define SCFRENAMER_H_INCLUDED

#include <QtCore>
#include <QFile>

#include "module.h"

class ScfRenamer : public Module
{
public:
    ScfRenamer();

    void numToReal(QString dirPath);
    void realToNum(QString dirPath);
};

#endif // SCFRENAMER_H_INCLUDED
