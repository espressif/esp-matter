/***************************************************************************//**
 * @brief Bluetooth Link Layer configuration
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

#ifndef SL_BT_BLUETOOTH_LL_PRIORITIES_DEFINED
#define SL_BT_BLUETOOTH_LL_PRIORITIES_DEFINED

typedef struct {
  uint8_t scan_min;
  uint8_t scan_max;
  uint8_t adv_min;
  uint8_t adv_max;
  uint8_t conn_min;
  uint8_t conn_max;
  uint8_t init_min;
  uint8_t init_max;
  uint8_t rail_mapping_offset;
  uint8_t rail_mapping_range;
  uint8_t _reserved;
  uint8_t adv_step;
  uint8_t scan_step;
} sl_bt_bluetooth_ll_priorities;

//Default priority configuration
#define SL_BT_BLUETOOTH_PRIORITIES_DEFAULT { 191, 143, 175, 127, 135, 0, 55, 15, 16, 16, 0, 4, 4 }

#define SL_BT_BLUETOOTH_PA_AUTOMODE 0xff

typedef struct {
  uint8_t activate_power_control;

  int8_t golden_rssi_min_1m; //<! Golden range lowest RSSI for 1M PHY.
  int8_t golden_rssi_max_1m; //<! Golden range highest RSSI for 1M PHY.

  int8_t golden_rssi_min_2m; //<! Golden range lowest RSSI for 2M PHY.
  int8_t golden_rssi_max_2m; //<! Golden range highest RSSI for 2M PHY.

  int8_t golden_rssi_min_coded_s8; //<! Golden range lowest RSSI for Coded PHY w/ S=8.
  int8_t golden_rssi_max_coded_s8; //<! Golden range highest RSSI for Coded PHY w/ S=8.

  int8_t golden_rssi_min_coded_s2; //<! Golden range lowest RSSI for Coded PHY w/ S=2.
  int8_t golden_rssi_max_coded_s2; //<! Golden range highest RSSI for Coded PHY w/ S=2.
} sl_bt_ll_power_control_config_t;

#define SL_BT_USE_MAX_POWER_LEVEL_SUPPORTED_BY_RADIO 0x7fff
#define SL_BT_USE_MIN_POWER_LEVEL_SUPPORTED_BY_RADIO 0x7fff

#endif
