/***************************************************************************//**
 * @file
 * @brief IO Stream SWO Component.
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
#ifndef SLI_IOSTREAM_SWO_ITM_8_H
#define SLI_IOSTREAM_SWO_ITM_8_H

#include "sl_iostream.h"
#include "sl_status.h"
#include "sl_iostream_swo_itm_8.h"

#ifdef __cplusplus
extern "C" {
#endif

void *sli_iostream_swo_itm_8_init(void);

sl_status_t sli_iostream_swo_itm_8_write(void *context,
                                         const void *buffer,
                                         size_t buffer_length,
                                         sl_iostream_swo_itm_8_msg_type_t type,
                                         uint8_t *seq_nbr);

#ifdef __cplusplus
}
#endif

#endif /* SLI_IOSTREAM_SWO_ITM_8_H */
