/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef XMLSYNC_H
#define XMLSYNC_H

#include <QFile>
#include <QtXml>
#include <QString>

#include "defines.h"
#include "module.h"

class XmlSync : public Module
{
public:
    XmlSync();

    void load(QString path, quint32 order);
    void syncStrings(quint32 scanMode);
    void saveXml(QString path);

private:
    size_t countNodeChildren(QDomElement parentNode, QString childTagName);
    void processEntryType05(QDomElement &parentNode, QByteArray *data);
    void replaceEntries(quint32 in_index, quint32 out_index);
    quint32 bytecmp_custom(QByteArray &str1, QByteArray &str2);
    void check1();

private:
    lang_id lang;
    QDomDocument xmlData_in, xmlData_out;
    QDomElement root_in, root_out;
};

#endif // XMLSYNC_H
