/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/
#include "langdb.h"

/* Language names database */
static const size_t N_LANG_ENTRIES = 4;
static const lang_decoder_t decoder[] =    {{"eng", 3, LANGUAGE_ENG},
                                            {"en",  2, LANGUAGE_ENG},
                                            {"rus", 3, LANGUAGE_RUS},
                                            {"ru",  2, LANGUAGE_RUS}};

static const size_t N_ENCODER_ENTRIES = 1;
static const sjis_encoder_t sjisEncoder[] = {{LANGUAGE_ENG, 0x82, 0x1F, 0x20},
                                             {LANGUAGE_RUS, 0x84, 0x30, 0x40}};

/* Accessory functions */
lang_id getLangIdByName(char * name)
{
    for(size_t i = 0; i < N_LANG_ENTRIES; i++)
    {
        if(qstrnicmp(name, decoder[i].name, decoder[i].nameLength) == 0)
        {
            return decoder[i].languageId;
        }
    }
    return LANGUAGE_UNKNOWN;
}

void convertToSjis(lang_id lang, QByteArray &data, QString &text)
{
    sjis_encoder_t encoder = sjisEncoder[lang];

    for(int i = 0; i < text.length(); i++)
    {
        QChar c = text.at(i);

        if(c.isUpper())
        {
            data.append(encoder.firstByte);
            data.append(c.cell() + encoder.upperOffset);
        }
        else if(c.isLower())
        {
            data.append(encoder.firstByte);
            data.append(c.cell() + encoder.lowerOffset);
        }
        else if(c.isSpace())
        {
            data.append(0x20);
        }
        else
        {
            assert(!"Unsupported type of symbol!");
        }
    }
}
