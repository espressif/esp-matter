/*******************************************************************************
 * @file
 * @brief STDIO compatibility layer.
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

#ifndef SL_STDIO_H
#define SL_STDIO_H

#include "sl_status.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * @addtogroup stdio Standard I/O
 * @brief Standard I/O allows to get and put characters on stdio stream that is the default in I/O Streams.
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Prototypes

#ifndef DOXYGEN
int sl_putchar_std_wrapper(int c);
int sl_getchar_std_wrapper(void);
#endif

/*******************************************************************************
 * Put a character on the default stream.
 *
 * @param[in] c   Character to print
 *
 * @return  Status result
 ******************************************************************************/
sl_status_t sl_putchar(char c);

/*******************************************************************************
 * Put a character on the default stream (match standard API).
 *
 * @param[in] c   Integer character to print
 *
 * @return  On success, the character written is returned.
 *          If a writing error occurs, -1 is returned (EOF).
 ******************************************************************************/
#define putchar  sl_putchar_std_wrapper

/*******************************************************************************
 * Get a character on the default stream.
 *
 * @param[out] c Pointer to variable that will receive the character.
 *
 * @return  Status result
 ******************************************************************************/
sl_status_t sl_getchar(char *c);

/*******************************************************************************
 * Get a character on the default stream.
 *
 * @return  On success, the character read is returned
 *          If a reading error occurs, -1 is returned (EOF).
 ******************************************************************************/
#define getchar  sl_getchar_std_wrapper

/** @} (end addtogroup stdio) */

#ifdef __cplusplus
}
#endif

#endif /* SL_STDIO_H */
