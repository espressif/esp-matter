/***************************************************************************//**
 * @brief Initialization APIs for Bluetooth stack
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_BT_STACK_INIT_H
#define SL_BT_STACK_INIT_H

#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the Bluetooth stack.
 *
 * @return SL_STATUS_OK if the initialization was successful; Other error code
 *   indicates a failure.
 */
sl_status_t sl_bt_stack_init();

#ifdef __cplusplus
}
#endif

#endif
