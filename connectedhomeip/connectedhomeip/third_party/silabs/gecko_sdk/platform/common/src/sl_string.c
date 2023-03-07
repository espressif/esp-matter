/***************************************************************************//**
 * @file
 * @brief Implementation of safe string functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "sl_string.h"
#include <stdint.h>
#include <ctype.h>

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Copy a string into a buffer.
 ******************************************************************************/
void sl_strcpy_s(char *dst, size_t dst_size, const char *src)
{
  size_t len_copy = 0;

  if (dst == NULL) {
    return;
  }
  if (src == NULL) {
    return;
  }
  if (dst_size == 0) {
    return;
  }
  while ((*src != '\0') && (len_copy < (dst_size - 1))) {
    *dst = *src;
    dst++;
    src++;
    len_copy++;
  }
  *dst = '\0';
}

/***************************************************************************//**
 * Append the source string to the end of destination string
 ******************************************************************************/
void sl_strcat_s(char *dst, size_t dst_size, const char *src)
{
  size_t ofs;

  if (dst == NULL) {
    return;
  }
  if (src == NULL) {
    return;
  }
  if (dst_size == 0) {
    return;
  }
  ofs = sl_strlen(dst);
  if (ofs < dst_size) {
    sl_strcpy_s(&dst[ofs], dst_size - ofs, src);
  }
}

/***************************************************************************//**
 * Get the string length
 ******************************************************************************/
size_t sl_strlen(char *str)
{
  return sl_strnlen(str, SIZE_MAX);
}

/***************************************************************************//**
 * Get the string length, limited to a given length
 ******************************************************************************/
size_t sl_strnlen(char *str, size_t max_len)
{
  size_t len = 0;

  if (str == NULL) {
    return len;
  }
  while ((*str != '\0') && (len < max_len)) {
    str++;
    len++;
  }

  return len;
}

/***************************************************************************//**
 * Check if string is empty.
 ******************************************************************************/
bool sl_str_is_empty(const char *str)
{
  return (str == NULL || *str == '\0');
}

/***************************************************************************//**
 * Compare two strings, ignoring case.
 ******************************************************************************/
int sl_strcasecmp(char const *a, char const *b)
{
  int d = 0;

  if ((a == NULL) || (b == NULL)) {
    return 1;
  }
  for (;; a++, b++) {
    d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
    if ((d != 0) || (!*a)) {
      break;
    }
  }
  return d;
}
