/***************************************************************************//**
 * @file
 * @brief Parse header file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef PARSE_H_
#define PARSE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

/***********************************************************************************************//**
 * \defgroup par Parse Code
 * \brief Parse application arguments implementation
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup Parse
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup par
 * @{
 **************************************************************************************************/

/***************************************************************************************************
 * Public Macros and Definitions
 **************************************************************************************************/
#define STR_UART_PORT_SIZE (50)

/***************************************************************************************************
 * Public Function Declarations
 **************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  Parse application arguments.
 *  \param[in]  argc  number of arguments
 *  \param[in]  argv  arguments array
 **************************************************************************************************/
void PAR_parse(int argc, char **argv);
void help(void);

/** @} (end addtogroup par) */
/** @} (end addtogroup Parse) */

#ifdef __cplusplus
};
#endif

#endif /* PARSE_H_ */
