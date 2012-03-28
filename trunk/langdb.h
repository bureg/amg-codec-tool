/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/
#ifndef LANGDB_H
#define LANGDB_H

#include <QString>

#include "defines.h"

typedef struct sjis_encoder_t
{
    lang_id languageId;
    unsigned char firstByte;
    char upperOffset;
    char lowerOffset;
} sjis_encoder_t;

typedef struct lang_decoder_t
{
    char name[16];
    size_t nameLength;
    lang_id languageId;
} lang_decoder_t;

lang_id getLangIdByName(char * name);
void convertToSjis(lang_id lang, QByteArray &data, QString &text);

#endif // LANGDB_H
