#ifndef XMLRENAMER_H_INCLUDED
#define XMLRENAMER_H_INCLUDED

#include <QtCore>
#include <QFile>

#include "module.h"

class XmlRenamer : public Module
{
public:
    XmlRenamer();

    void numToReal(QString dirPath);
    void realToNum(QString dirPath);
};

#endif // XMLRENAMER_H_INCLUDED
