/***************************************************************************//**
 * @file
 * @brief BLE related application related common code in the Zigbee BLE DMP sample apps
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
#ifndef __SL_DMP_UI_STUB_H__
#define __SL_DMP_UI_STUB_H__

// If Zigbee display component is present, these definitions will be provided by sl_dmp_ui.h
#ifndef SL_CATALOG_ZIGBEE_DISPLAY_PRESENT
// Stubs
#define sl_dmp_ui_display_zigbee_state(arg)
#define sl_dmp_ui_zigbee_permit_join(arg)
#define sl_dmp_ui_light_off()
#define sl_dmp_ui_light_on()
#define sl_dmp_ui_update_direction(arg)
#define sl_dmp_ui_bluetooth_connected(arg)
#define sl_dmp_ui_set_ble_device_name(arg)
#define sl_dmp_ui_init()
#define sl_dmp_ui_display_help()
#define DMP_UI_DIRECTION_INVALID 0
#define DMP_UI_DIRECTION_BLUETOOTH 1
#define DMP_UI_DIRECTION_SWITCH 2
#define DMP_UI_DIRECTION_ZIGBEE  3
#define DmpUiLightDirection_t  uint8_t
#define sl_dmp_ui_set_light_direction(...)
#define sl_dmp_ui_get_light_direction(...) 0

typedef enum {
  DMP_UI_LIGHT_OFF,
  DMP_UI_LIGHT_ON,
  DMP_UI_LIGHT_UNCHANGED
} sl_dmp_ui_light_state_t;
#endif //#ifndef SL_CATALOG_ZIGBEE_DISPLAY_PRESENT

#endif //__SL_DMP_UI_STUB_H__
