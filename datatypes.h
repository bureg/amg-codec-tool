/*
  This file is a part of the
  Amagami SS PS2 Translation Project
  by FreeSmiler

  You may feel free to use this code if you want so.
  I provide no guarantee that this code is working,
  useful or anything else. Use it at your own risk.
*/

#ifndef DATATYPES_H
#define DATATYPES_H

#include <QtGlobal>

struct arc_entry_t
{
    quint32 unknown0;
    quint32 unknown1;
    quint32 offset;
    quint32 size;

    static bool lessThan(const arc_entry_t& entry1, const arc_entry_t& entry2)
    {
        return (entry1.offset < entry2.offset);
    }
};

#endif // DATATYPES_H
