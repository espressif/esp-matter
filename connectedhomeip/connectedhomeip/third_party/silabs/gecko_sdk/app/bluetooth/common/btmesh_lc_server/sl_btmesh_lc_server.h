/***************************************************************************//**
 * @file
 * @brief sl_btmesh_lc_server.h
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_BTMESH_LC_SERVER_H
#define SL_BTMESH_LC_SERVER_H

#include "sl_btmesh_generic_model_capi_types.h"

/***************************************************************************//**
 * @defgroup LC Light Controller Module
 * @brief LC Module Implementation
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup LC
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * LC initialization.
 * This should be called at each boot if provisioning is already done.
 * Otherwise this function should be called after provisioning is completed.
 *
 * @param[in] element  Index of the element where LC model is initialized.
 *
 * @return Status of the initialization operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
sl_status_t sl_btmesh_lc_init(void);

/***************************************************************************//**
 * This function is getter for current light controller mode.
 *
 * @return  current light controller mode
 ******************************************************************************/
uint8_t sl_btmesh_lc_get_mode(void);

/***************************************************************************//**
 * Handle LC Server events.
 *
 * This function is called automatically by Universal Configurator after
 * enabling the component.
 *
 * @param[in] evt  Pointer to incoming event.
 ******************************************************************************/
void sl_btmesh_lc_server_on_event(sl_btmesh_msg_t *evt);

/** @} (end addtogroup LC) */

#endif /* SL_BTMESH_LC_SERVER_H */
