/***************************************************************************//**
 * @brief Bluetooth stack configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_BT_STACK_CONFIG_H
#define SL_BT_STACK_CONFIG_H

#include <stdint.h>
#include "sl_bt_ll_config.h"

typedef struct {
  uint32_t max_buffer_memory; //!< Maximum number of bytes to use for data buffers
  uint32_t linklayer_config;
  sl_bt_bluetooth_ll_priorities * linklayer_priorities; //Priority configuration, if NULL uses default values
}sl_bt_stack_config_t;

/** Flag indicating Bluetooth runs in RTOS */
#define SL_BT_CONFIG_FLAG_RTOS                         256

/** Flag indicating Bluetooth can allow EM2 with inaccurate LF clock */
#define SL_BT_CONFIG_FLAG_INACCURATE_LFCLK_EM2           512

typedef void (*sl_bt_priority_schedule_callback)(void);
typedef void (*sl_bt_stack_schedule_callback)(void);

#define SL_BT_RF_CONFIG_ANTENNA                        (1 << 0)
typedef struct {
  int16_t tx_gain; // RF TX gain. Unit: 0.1 dBm. For example, -20 means -2.0 dBm (signal loss).
  int16_t rx_gain; // RF RX gain. Unit: 0.1 dBm.
  uint8_t flags; // bit 0 is enabling antenna config, other bits 1-7 are reserved.
  uint8_t antenna; // A configuration for antenna selection.
                   // Value should be come from RAIL_AntennaSel_t enum.
                   // See antenna path selection in RAIL rail_chip_specific.h.
  int16_t tx_min_power; // Minimum TX power level. Unit: 0.1 dBm.
  int16_t tx_max_power; // Maximum TX power level. Unit: 0.1 dBm.
} sl_bt_rf_config_t;

typedef struct {
  uint32_t config_flags;
  sl_bt_stack_config_t bluetooth;

  // GATT database (pointer of "sli_bt_gattdb_t*" type)
  const void* gattdb;

  //Callback for priority scheduling, used for RTOS support. If NULL uses pendsv irq.
  //This is called from Interrupt context
  sl_bt_priority_schedule_callback scheduler_callback;

  //Callback for requesting Bluetooth stack scheduling, used for RTOS support
  //This is called from Interrupt context
  sl_bt_stack_schedule_callback stack_schedule_callback;

  uint8_t max_timers;  // Max number of soft timers, up to 16, the application will use through SL_BT_API. Default: 4
  sl_bt_rf_config_t rf;
}sl_bt_configuration_t;

#endif
