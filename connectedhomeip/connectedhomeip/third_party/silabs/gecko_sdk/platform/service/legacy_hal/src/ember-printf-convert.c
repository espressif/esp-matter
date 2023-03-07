/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
// *****************************************************************************
// * ember-printf-convert.c
// *
// * Transform Ember printf format specifier into standard printf format
// * specifier.  Only available on systems with malloc/free.
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

#include PLATFORM_HEADER //compiler/micro specifics, types

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#define strnlen(string, n) strlen((string))
#endif

//------------------------------------------------------------------------------

// Compare two characters, ignoring case (like strcasecmp)
static bool charCaseCompare(const char c1, const char c2)
{
  uint8_t i;
  char c[2];
  c[0] = c1;
  c[1] = c2;
  for (i = 0; i < 2; i++) {
    // Convert both to lowercase if they are letters
    if (c[i] >= 'A' && c[i] <= 'Z') {
      c[i] += 0x20;
    }
  }
  return (c[0] == c[1]);
}

// Work backwards from the end of the string shifting all charactes to
// the right by a number of characters
static void shiftStringRight(char* string, uint8_t length, uint8_t charsToShift)
{
  int i = length - 1;
  while (i >= charsToShift) {
    string[i] = string[i - charsToShift];
    i--;
  }
}

// Transform Ember printfs to standard printfs.
//
// Ember uses %p to represent a pointer to a flash string, change this to the
// standard %s.  In addition, we don't need both '\r' and '\n' at the end of the
// string, '\n' will suffice.  The last and most difficult conversion is
// because Ember Printf interprets the %x, %2x, and %4x in a non-standard way.
// Normally a number after a '%' is interpreted as a minimum width field
// (with spaces instead of 0 padding).  For Ember, this is interpreted to mean
// a 1, 2, or 4 byte integer value respectively and always 0 padding.
// Therefore to maintain parity we change %x to %02x, %2x to %04x, and
//  %4x to %08x.
//
// We also change %l -> %d since %l is Ember's long integer print (32-bit) and
// we assume that the Unix system native integer size is 32-bit.
//
// Returns a pointer to a newly allocated and modified string (caller must free).
//   Or NULL if it failed to allocate memory.

char* transformEmberPrintfToStandardPrintf(const char* input,
                                           bool filterSlashR)
{
  // Assume no string is longer than 255.  This is true for most of
  // our embedded code so we assume host specific code was written
  // the same way.
  uint8_t length = strnlen(input, 254) + 1;    // add 1 for '\0'
  bool percentFound = false;
  uint8_t paddingNeeded = 0;
  uint8_t i = 0;
  char* newFormatString = malloc(length);

  if (newFormatString == NULL) {
    return NULL;
  }

  strncpy(newFormatString, input, length - 1);
  newFormatString[length - 1] = '\0';

  // Replacing %p with %s is straightforward because it doesn't change the size
  // of the string.  Changing %x, %2x and %4x to the correct values involves
  // some additional space in the string.  So record how much space is needed for
  // lengthening the string later to add characters.

  while (i < (length - 1)) {
    char a = newFormatString[i];
    if (percentFound) {
      if (a == 'p') {
        newFormatString[i] = 's';
      } else if (charCaseCompare(a, 'x')) {
        // Need to add two characters to change %x -> %02x
        paddingNeeded += 2;
      } else if (((i + 1 < (length - 1)
                   && charCaseCompare(newFormatString[i + 1], 'x')))) {
        paddingNeeded++;
      } else if (a == 'l') {
        // We assume a Unix system with integers that
        // are natively 32-bits.  So we don't need '%ld' we just
        // need '%d'
        newFormatString[i] = 'd';
      }

      percentFound = false;
    } else if (a == '%') {
      percentFound = true;
    }

    // Filter out all '\r'.  Unnecessary for Cygwin/Linux.
    // In the case of remote connections, we need native line-endings.
    // DOS telnet client wants both, therefore we send it.
    if (!filterSlashR && a == '\r') {
      newFormatString[i] = ' ';
    }

    i++;
  }

  assert((uint16_t)length + (uint16_t)paddingNeeded <= 255);
  length += paddingNeeded;

  if (paddingNeeded) {
    char* temp = realloc(newFormatString, length);
    if (temp == NULL) {
      fprintf(stderr, "Error: Failed to realloc, freeing existing memory.\n");
      free(newFormatString);
      return NULL;
    }
    newFormatString = temp;

    i = 0;
    percentFound = false;

    // Change %x  -> %02X
    //        %2x -> %04X
    //        %4x -> %08X
    // We change to uppercase hex to match the way the Embedded code
    // works.
    while (i < (length - 1)) {
      char a = newFormatString[i];
      if (percentFound
          && (charCaseCompare(a, 'x')
              || (i + 1 < length
                  && charCaseCompare(newFormatString[i + 1], 'x')
                  && (a == '2'
                      || a == '4')))) {
        uint8_t charsToAdd = charCaseCompare(a, 'x') ? 2 : 1;
        shiftStringRight(&(newFormatString[i]), length - i, charsToAdd);

        newFormatString[i] = '0';
        if (charsToAdd == 2) {
          newFormatString[i + 1] = '2';
        } else if (a == '2') {
          newFormatString[i + 1] = '4';
        } else {
          newFormatString[i + 1] = '8';
        }
        // The 'x' is now two characters from where we are currently.
        // Transform to uppercase.
        newFormatString[i + 2] = 'X';

        percentFound = false;
      } else if (a == '%') {
        percentFound = true;
      } else {
        percentFound = false;
      }
      i++;
    }
  }
  return newFormatString;
}
