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
#ifndef __SL_DMP_ZIGBEE_BLE_EVENT_HANDLER_H__
#define __SL_DMP_ZIGBEE_BLE_EVENT_HANDLER_H__

#include PLATFORM_HEADER
#include "hal.h"
#include "ember.h"
#include "af.h"
#include "sl_bluetooth.h"

void zb_ble_dmp_read_light_state(uint8_t connection);
void zb_ble_dmp_read_trigger_source(uint8_t connection);
void zb_ble_dmp_read_source_address(uint8_t connection);

void zb_ble_dmp_write_light_state(uint8_t connection, uint8array *writeValue);

void zb_ble_dmp_notify_light(uint8_t lightState);
void zb_ble_dmp_notify_trigger_source(uint8_t connection, uint8_t triggerSource);
void zb_ble_dmp_notify_source_address(uint8_t connection);

void zb_ble_dmp_set_source_address (EmberEUI64 set_address);
void zb_ble_dmp_print_ble_address(uint8_t *address);
void zb_ble_dmp_print_ble_connections();
void sli_ble_application_init(uint8_t init_level);

#endif //__SL_DMP_ZIGBEE_BLE_EVENT_HANDLER_H__
