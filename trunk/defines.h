/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef DEFINES_H
#define DEFINES_H

#include <assert.h>
#include <stddef.h>

/* Header */
const char header[] = "SCF";
const size_t HEADER_LENGTH = 3;

/* Labels section header  */
const char labelsSectionHeader[] = { 0x1A, 0x00, 0x04 };
const size_t LABELS_HEADER_LENGTH = 3;

/* Blocks section header  */
const char blocksSectionHeader[] = { 0x00, 0x00 };
const size_t BLOCKS_HEADER_LENGTH = 2;

/* Functons section header  */
const char functionsSectionHeader[] = { 0x00, 0x00 };
const size_t FUNCTIONS_HEADER_LENGTH = 2;

/* ------------------------------------ MACROS ------------------------------------ */
#define READ_TO_VAR(file, dest) \
{ \
    if( file.read((char*)&(dest), sizeof(dest)) != sizeof(dest)) \
    { \
        file.close(); \
        fatalExit("Failed to read from file"); \
    } \
}

#define READ_TO_VAR_NONZERO(file, dest) \
{ \
    READ_TO_VAR((file), (dest)); \
    assert((dest) && "Expected to be nonzero"); \
}

#define WRITE_HEADER(file, array, size) \
{ \
    if( file.write((array), (size)) != (size)) \
    { \
        file.close(); \
        fatalExit("Failed to write into the file"); \
    } \
}

#define WRITE_VAR(file, var) \
{ \
    if( file.write((char*)&(var), sizeof(var)) != sizeof(var)) \
    { \
        file.close(); \
        fatalExit("Failed to write into the file"); \
    } \
}

#define WRITE_STR(file, string) \
{ \
    QByteArray _tmpArray = string.toAscii(); \
    if( file.write(_tmpArray) != _tmpArray.size()) \
    { \
        file.close(); \
        fatalExit("Failed to write into the file"); \
    } \
}

#define WRITE_BINARY_DATA(file, data) \
{ \
    if( file.write(data) != data.size()) \
    { \
        file.close(); \
        fatalExit("Failed to write into the file"); \
    } \
}

#define WRITE_STRING_WITH_LENGTH(file, string) \
{ \
    quint16 _length = (quint16)string.length(); \
    WRITE_VAR(file, _length); \
    WRITE_STR(file, string); \
}

#define WRITE_BINARY_DATA_WITH_LENGTH(file, data) \
{ \
    quint16 _length = (quint16)data.size(); \
    WRITE_VAR(file, _length); \
    WRITE_BINARY_DATA(file, data); \
}

#define READ_TO_BUF(file, size) \
{ \
    if( file.read(buffer, (size)) != (size)) \
    { \
        file.close(); \
        fatalExit("Failed to read from file"); \
    } \
}

#define READ_TO_STRING(file, dest, size) \
{ \
    READ_TO_BUF((file), (size)); \
    dest = QString::fromAscii(buffer, (size)); \
}

#define READ_TO_BYTE_ARRAY(file, dest, size) \
{ \
    dest = file.read((size)); \
}

#define APPEND_ATTRIBUTE_DEC(xml, node, name, var) \
{ \
    QDomAttr _tmpAttr = xml.createAttribute((name)); \
    _tmpAttr.setValue(QString().sprintf("%d", (var))); \
    node.setAttributeNode(_tmpAttr); \
}

#define APPEND_ATTRIBUTE_HEX(xml, node, name, var) \
{ \
    QDomAttr _tmpAttr = xml.createAttribute((name)); \
    _tmpAttr.setValue(QString().sprintf("0x%x", (var))); \
    node.setAttributeNode(_tmpAttr); \
}

#define APPEND_ATTRIBUTE_STR(xml, node, name, var) \
{ \
    QDomAttr _tmpAttr = xml.createAttribute((name)); \
    _tmpAttr.setValue((var)); \
    node.setAttributeNode(_tmpAttr); \
}

#define ATTRIBUTE_TO_INT(node, name, dest, type) \
{ \
    QString _tmpAttrString = node.attribute((name)); \
    if(_tmpAttrString.isNull()) \
    { \
        fatalExit("Attribute [" ## name ## "] not found!"); \
    } \
    bool ok = false; \
    dest = (type)_tmpAttrString.toInt(&ok, 0); \
    if(!ok) \
    { \
        fatalExit("Attribute [" ## name ## "] has invalid value!"); \
    } \
}

#define ATTRIBUTE_TO_INT_NONZERO(node, name, dest, type) \
{ \
    ATTRIBUTE_TO_INT(node, name, dest, type); \
    assert((dest) && "Expected to be nonzero"); \
}

#define ATTRIBUTE_TO_UINT(node, name, dest, type) \
{ \
    QString _tmpAttrString = node.attribute((name)); \
    if(_tmpAttrString.isNull()) \
    { \
        fatalExit("Attribute [" ## name ## "] not found!"); \
    } \
    bool ok = false; \
    dest = (type)_tmpAttrString.toUInt(&ok, 0); \
    if(!ok) \
    { \
        fatalExit("Attribute [" ## name ## "] has invalid value!"); \
    } \
}

#define ATTRIBUTE_TO_UINT_NONZERO(node, name, dest, type) \
{ \
    ATTRIBUTE_TO_UINT(node, name, dest, type); \
    assert((dest) && "Expected to be nonzero"); \
}

#define ATTRIBUTE_TO_STRING_NOT_EMPTY(node, name, var) \
{ \
    var = node.attribute((name)); \
    if(var.isNull()) \
    { \
        fatalExit("Attribute [" ## name ## "] not found or empty!"); \
    } \
}

#define GET_NODE(root, name, var) \
{ \
    var = root.firstChildElement(name); \
    if (var.isNull()) \
    { \
        fatalExit("Cannot find <" ## name ## "> node!"); \
    } \
}

#define GET_FIRST_CHILD_NODE(root, var) \
{ \
    var = root.firstChildElement(); \
    if (var.isNull()) \
    { \
        fatalExit("Cannot find a child node!"); \
    } \
}

#define GET_NODE_TEXT(node, dest) \
{ \
    dest = node.text(); \
}

#define GET_NODE_BINARY_DATA(node, dest) \
{ \
    dest = QByteArray::fromBase64(node.text().toAscii()); \
}

#define IS_ASCII_CODE(c) ((c) < 0x80)

/* Entry types */
#define ENTRY_TYPE_03 0x03
#define ENTRY_TYPE_05 0x05
#define ENTRY_TYPE_06 0x06
#define ENTRY_TYPE_07 0x07
#define ENTRY_TYPE_08 0x08

#endif // DEFINES_H
