/***************************************************************************//**
 * @file
 * @brief IO Stream debug Component.
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
#ifndef SL_IOSTREAM_DEBUG_H
#define SL_IOSTREAM_DEBUG_H

#include "sl_iostream.h"
#include "sl_status.h"
#include "sl_iostream_swo_itm_8.h"

#ifdef __cplusplus
extern "C" {
#endif
// -----------------------------------------------------------------------------
// Global Variables

extern sl_iostream_t *sl_iostream_debug_handle;
extern sl_iostream_instance_info_t sl_iostream_instance_debug_info;

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * Initialize debug stream component.
 *
 * @return  Status Code:
 *            - SL_STATUS_OK
 *            - SL_STATUS_FAIL
 ******************************************************************************/
sl_status_t sl_iostream_debug_init(void);

/***************************************************************************//**
 * Set Debug stream type
 *
 * @return  Status Code:
 *            - SL_STATUS_OK
 *            - SL_STATUS_FAIL
 ******************************************************************************/
sl_status_t sl_iostream_set_debug_type(sl_iostream_swo_itm_8_msg_type_t type);

#ifdef __cplusplus
}
#endif

#endif /* SL_IOSTREAM_debug_H */
