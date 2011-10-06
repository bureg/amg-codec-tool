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

#include "xmlhelpers.h"

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

#define IS_ASCII_CODE(c) ((c) < 0x80)

/* Entry types */
#define ENTRY_TYPE_03 0x03
#define ENTRY_TYPE_05 0x05
#define ENTRY_TYPE_06 0x06
#define ENTRY_TYPE_07 0x07
#define ENTRY_TYPE_08 0x08

#endif // DEFINES_H
