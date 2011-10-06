/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef ARCPACKER_H
#define ARCPACKER_H

#include <QString>
#include <QFile>
#include <QByteArray>

#include "datatypes.h"
#include "module.h"

class ArcPacker : public Module
{
public:
    ArcPacker();
    int pack(QString inFilePath, QString outFilePath);
    int unpack(QString inFilePath, QString outFilePath);

private:
    void putFileSize(QFile &file, quint32 size);
    void initEncoder();
    void initDecoder();
    int refillBuffer(QFile &inFile);
    int openFiles(QFile &input, QFile &output);
    int findBestMatch(match_info_t &matchInfo, size_t blockSize);
    int dumpToFile(QFile &outFile, packed_block_t &block);
    int fillBlockEntry(packed_block_t &block, size_t index, match_info_t &match);
    int flushToBuffer(size_t length);
    void scanBuffer(size_t &newBufPos, size_t &newEncBufPos, size_t &matchedLength);
    quint16 makeRef(size_t maxChunkStart, size_t maxMatchedLength);
    quint8 getBufItem(size_t index);
    void setBufItem(size_t index, quint8 value);
    quint8 getEncBufItem(size_t index);

    enum {
            REFILL_OK = 0,
            REFILL_FAIL = 1
         };

private:
    static const size_t SECTION_SZ = 256;
    static const size_t N_SECTIONS = 8;
    static const size_t BUF_SZ = SECTION_SZ * N_SECTIONS;
    static const size_t ENC_BUF_SZ = 64;
    static const size_t BUF_START_POSITION = 0xDE + SECTION_SZ * (N_SECTIONS - 1);
    static const size_t BUF_REFILL_AMOUNT = 40;
    static const size_t MAX_CHUNK_LENGTH = 34;
    static const size_t MIN_MATCH_LENGTH = 3;

    quint8 encBuf[ENC_BUF_SZ];
    quint8 buf[BUF_SZ];

    size_t counter;
    size_t encBufPos;
    size_t encBufLast;
    size_t bytesEncoded;
    size_t bufPos;
};

#endif // ARCPACKER_H
