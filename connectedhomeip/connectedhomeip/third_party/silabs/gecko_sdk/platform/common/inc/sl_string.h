/*******************************************************************************
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

#ifndef SL_STRING_H
#define SL_STRING_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * @addtogroup string String
 * @brief String module provides APIs to handle string-related operations.
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Defines

// -----------------------------------------------------------------------------
// Prototypes

/*******************************************************************************
 * @brief
 *  Copy a string into a buffer.
 *  Normally, the complete source string including the '\0' termination will be
 *  copied to the destination.
 *  If the destination buffer doesn't have room to receive the
 *  complete source string, the source string will be truncated and the
 *  destination buffer will be '\0' terminated within the destination buffer.
 *
 * @param[in] dst       Destination buffer.
 *
 * @param[in] dst_size  The size of the destination buffer.
 *
 * @param[in] src       Source string.
 ******************************************************************************/
void sl_strcpy_s(char *dst, size_t dst_size, const char *src);

/*******************************************************************************
 * @brief
 *  Append the source string to the end of destination string.
 *  Normally, the complete source string including the '\0' termination will be
 *  appended to the destination, starting at the source strings '\0' termination.
 *  If the destination buffer has no room to receive the
 *  complete source string, the source string will be truncated and the
 *  destination '\0' terminated within the destination buffer.
 *
 * @param[in] dst       Destination string.
 *
 * @param[in] dst_size  The size of the destination string buffer.
 *
 * @param[in] src       Source string.
 ******************************************************************************/
void sl_strcat_s(char *dst, size_t dst_size, const char *src);

/*******************************************************************************
 * @brief
 *  Get the string length.
 *
 * @param[in] str       The string to get the length for.
 *
 * @return              String lenght.
 ******************************************************************************/
size_t sl_strlen(char *str);

/*******************************************************************************
 * @brief
 *  Get the string length, limited to given length.
 *
 * @param[in] str       The string to get the length for.
 *
 * @param[in] max_len   The input string is searched for at most max_lencharacters.
 *
 * @return              String lenght.
 ******************************************************************************/
size_t sl_strnlen(char *str, size_t max_len);

/*******************************************************************************
 * @brief
 *  Check if the string is empty.
 *
 * @param[in] str       The string to check.
 *
 * @return              true if string is empty or null, else return false.
 ******************************************************************************/
bool sl_str_is_empty(const char *str);

/*******************************************************************************
 * @brief
 *  Compare two strings, ignoring case.
 *
 * @param[in] a         String to compare.
 *
 * @param[in] b         String to compare.
 *
 * @return              An integer greater than, or less than 0 if the strings
 *                      are not equal. 0 if the strings are equal.
 ******************************************************************************/
int sl_strcasecmp(char const *a, char const *b);

/** @} (end addtogroup string) */

#ifdef __cplusplus
}
#endif

#endif /* SL_STRING_H */
