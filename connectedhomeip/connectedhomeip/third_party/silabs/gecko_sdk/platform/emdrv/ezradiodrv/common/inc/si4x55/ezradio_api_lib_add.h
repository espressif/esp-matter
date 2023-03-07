/***************************************************************************//**
 * @file
 * @brief This file contains the additional API library for the listed members of
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

#ifndef _EZRADIO_API_LIB_ADD_H_
#define _EZRADIO_API_LIB_ADD_H_

#include "ezradio_api_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup ezradiodrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup ezradiodrv_api
 * @{
 ******************************************************************************/

EZRADIO_ConfigRet_t ezradio_configuration_init(const uint8_t* pSetPropCmd);

void ezradio_write_ezconfig_array(uint8_t numBytes, uint8_t* pEzConfigArray);
void ezradio_ezconfig_check(uint16_t checksum, ezradio_cmd_reply_t *ezradioReply);

#ifdef EZRADIO_DRIVER_EXTENDED_SUPPORT
/* Extended driver support functions */

#ifdef EZRADIO_DRIVER_FULL_SUPPORT
/* Full driver support functions */

void ezradio_get_adc_reading(uint8_t adc_en, uint8_t adc_cfg, ezradio_cmd_reply_t *ezradioReply);

#endif /* EZRADIO_DRIVER_FULL_SUPPORT */
#endif /* EZRADIO_DRIVER_EXTENDED_SUPPORT */

/** @} (end addtogroup ezradiodrv_api) */
/** @} (end addtogroup ezradiodrv) */

#ifdef __cplusplus
}
#endif

#else //_EZRADIO_API_LIB_ADD_H_
#error Additional EZRadio API library is already defined.
#endif //_EZRADIO_API_LIB_ADD_H_
