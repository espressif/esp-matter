/***************************************************************************//**
 * @file
 * @brief Bluetooth event handler registered to SL Bluetooth stack.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_BT_HAL_EVENT_HANDLER_H
#define SL_BT_HAL_EVENT_HANDLER_H

/* Silicon Labs includes */
#include "sl_bt_api.h"

/**
 * @brief Function invoked by SL Bluetooth stack on a Bluetooth event
 *
 * @param[in] evt The Bluetooth event
 */
void sl_bt_hal_on_event(sl_bt_msg_t* evt);

#endif /* SL_BT_HAL_EVENT_HANDLER_H */
