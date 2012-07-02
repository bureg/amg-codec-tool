/*
  This file is a part of the
  Amagami SS PS2 Translation Project

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/
#include <QTextCodec>

#include "langdb.h"
#include <stdio.h>

/* Language names database */
static const size_t N_LANG_ENTRIES = 4;
static const lang_decoder_t decoder[] =    {{"eng", 3, LANGUAGE_ENG},
                                            {"en",  2, LANGUAGE_ENG},
                                            {"rus", 3, LANGUAGE_RUS},
                                            {"ru",  2, LANGUAGE_RUS}};

static const size_t N_ENCODER_ENTRIES = 1;
static const sjis_encoder_t sjisEncoder[] = {{LANGUAGE_ENG, 0x82, 0x1F, 0x20},
                                             {LANGUAGE_RUS, 0x84, 0x30, 0x40}};

bool quoteEnd; //this needs to persist longer than it did before.

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
    bool ASCII_out = false;

    for(int i = 0; i < text.length(); i++)
    {
        QChar c = text.at(i);
        char c_ASCII = c.toAscii();

        //Japanese ellipse
        if (c.unicode() == (ushort) 8230)
        {
            data.append((char)0x81);
            data.append((char)0x63);
        }

        //Center dot
        else if (c.unicode() == (ushort) 12539)
        {
            data.append((char)0x81);
            data.append((char)0x45);
        }

        //Opening single-quote
        else if (c.unicode() == (ushort) 8216)
        {
            data.append((char)0x81);
            data.append((char)0x65);
        }

        //Closing single-quote
        else if (c.unicode() == (ushort) 8217)
        {
            data.append((char)0x81);
            data.append((char)0x66);
        }

        //Opening double-quote
        else if (c.unicode() == (ushort) 8220)
        {
            data.append((char)0x81);
            data.append((char)0x67);
        }

        //Closing double-quote
        else if (c.unicode() == (ushort) 8221)
        {
            data.append((char)0x81);
            data.append((char)0x68);
        }

        else if(c.isUpper())
        {
            switch (ASCII_out)
            {
                case true:
                    data.append(c);
                    break;
                case false:
                    data.append(encoder.firstByte);
                    data.append(c.cell() + encoder.upperOffset);
                    break;
            }
        }

        else if(c.isLower())
        {
            switch (ASCII_out)
            {
                case true:
                    data.append(c);
                    break;
                case false:
                    data.append(encoder.firstByte);
                    data.append(c.cell() + encoder.lowerOffset);
                    break;
            }
        }

        else if(c.isDigit())
        {
            switch (ASCII_out)
            {
                case true:
                    data.append(c);
                    break;
                case false:
                    data.append(encoder.firstByte);
                    data.append(c.cell() + encoder.upperOffset);
                    break;
            }
        }

        else if(c.isSpace())
        {
            /* Unused code to get empty space */
            data.append((char)0x81);
            data.append((char)0x40);
        }

        else
        {
            //All of this is added by *Name retracted*, hacked code to get punctuation to work
            switch (c_ASCII)
            {
                case '&':
                    data.append((char)0x81);
                    data.append((char)0x95);
                    break;

                case '>':
                    data.append((char)0x81);
                    data.append((char)0x84);
                    break;

                case '[':
                    data.append((char)0x81);
                    data.append((char)0x6D);
                    break;

                case ']':
                    data.append((char)0x81);
                    data.append((char)0x6E);
                    break;

                case '<':
                    data.append((char)0x81);
                    data.append((char)0x83);
                    break;

                case '~':
                    data.append((char)0x81);
                    data.append((char)0x60);
                    break;

                case '!':
                    data.append((char)0x81);
                    data.append((char)0x49);
                    break;

                case '.':
                    data.append((char)0x81);
                    data.append((char)0x44);
                    break;

                case ':':
                    data.append((char)0x81);
                    data.append((char)0x46);
                    break;

                case ';':
                    data.append((char)0x81);
                    data.append((char)0x47);
                    break;

                case '?':
                    data.append((char)0x81);
                    data.append((char)0x48);
                    break;

                case ',':
                    data.append((char)0x81);
                    data.append((char)0x43);
                    break;

                case '-':
                    data.append((char)0x81);
                    data.append((char)0x5D);
                    break;

                case '*':
                    data.append((char)0x81);
                    data.append((char)0x96);
                    break;

                case '(':
                    data.append((char)0x81);
                    data.append((char)0x69);
                    break;

                case ')':
                    data.append((char)0x81);
                    data.append((char)0x6A);
                    break;

                case '+':
                    data.append((char)0x81);
                    data.append((char)0x7B);
                    break;

                case '\'':
                    data.append((char)0x81);
                    data.append((char)0x66);
                    break;

                case '/':
                    //Important because this codes for a full width solidus "/" which calls for a new line.
                    data.append((char)0x81);
                    data.append((char)0x5E);
                    break;

                case '"':
                    switch (quoteEnd)
                    {
                        case true:
                            //Closing quotes
                            data.append((char)0x81);
                            data.append((char)0x68);
                            quoteEnd = false;
                            break;
                        case false:
                            //Opening quotes
                            data.append((char)0x81);
                            data.append((char)0x67);
                            quoteEnd = true;
                    }
                    break;

                //Escape character to print ASCII, no workaround to actually print a "\"
                //since it will never be needed for our purposes. Feel free to implement one.
                case '\\':
                    switch (ASCII_out)
                    {
                        case true:
                            //Second "\" stops the sequence
                            ASCII_out = false;
                            break;
                        case false:
                            //First "\" starts the sequence
                            ASCII_out = true;
                            break;
                    }
                    break;

                default:
                    //assert(!"Unsupported type of symbol!"); //screw dat
                    QTextCodec *codec = QTextCodec::codecForName("Shift-JIS");
                    QByteArray achar = codec->fromUnicode(c);
                    printf("Using fallback for unknown unicode character: %d (utf16, dec)\n", c.unicode());
                    data.append(achar.data());
            }
        }
    }
}
