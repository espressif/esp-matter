/***************************************************************************//**
 * @file
 * @brief CLI for the DMO Tuning and Testing plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app/framework/include/af.h"
#include "app/framework/util/af-main.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "sl_bt_ll_config.h"
#include "lower-mac.h"
#include "dmp-tuning.h"
#include "dmp-tuning-profiles.h"
#include "sl_bluetooth.h"

void emRadioEnableConfigScheduledCallback(bool enable);
bool emRadioConfigScheduledCallbackEnabled(void);
void emRadioConfigScheduledCallback(bool scheduled);
void emberAfPluginDmpStopRadio(void);

// Zigbee related CLI commands
// Zigbee scheduler priorities
void emberAfPluginDmpTuningSetZigbeeSchedulerPrioritiesCommand(sl_cli_command_arg_t *arguments)
{
  EmberMultiprotocolPriorities pri;
  pri.backgroundRx = sl_cli_get_argument_uint8(arguments, 0);
  pri.activeRx = sl_cli_get_argument_uint8(arguments, 1);
  pri.tx = sl_cli_get_argument_uint8(arguments, 2);
  if (emberRadioSetSchedulerPriorities(&pri) == EMBER_SUCCESS) {
    sl_mac_lower_mac_radio_sleep();
    sl_mac_lower_mac_radio_wakeup();
    emberAfCorePrintln("DmpTuning - SetZigbeeSchedulerPrioritiesCommand: success");
  } else {
    emberAfCorePrintln("invalid call");
  }
}

void emberAfPluginDmpTuningGetZigbeeSchedulerPrioritiesCommand(sl_cli_command_arg_t *arguments)
{
  EmberMultiprotocolPriorities pri;
  emberRadioGetSchedulerPriorities(&pri);
  emberAfCorePrintln("DmpTuning - GetZigbeeSchedulerPrioritiesCommand: backgroundRx:%d activeRx:%d tx:%d",
                     pri.backgroundRx,
                     pri.activeRx,
                     pri.tx);
}

// Zigbee Slip Time
void emberAfPluginDmpTuningSetZigbeeSlipTimeCommand(sl_cli_command_arg_t *arguments)
{
  EmberStatus status = emberRadioSetSchedulerSliptime((uint32_t)emberUnsignedCommandArgument(0));
  if (status == EMBER_SUCCESS) {
    emberAfCorePrintln("DmpTuning - SetZigbeeSlipTime: success");
  } else {
    emberAfCorePrintln("DmpTuning - SetZigbeeSlipTime should only be used in multiprotocol");
  }
}

void emberAfPluginDmpTuningGetZigbeeSlipTimeCommand(sl_cli_command_arg_t *arguments)
{
  uint32_t slipTime;
  EmberStatus status = emberRadioGetSchedulerSliptime(&slipTime);
  if (status == EMBER_SUCCESS) {
    emberAfCorePrintln("DmpTuning - GetZigbeeSlipTime: slip_time:%d", slipTime);
  } else {
    emberAfCorePrintln("DmpTuning - GetZigbeeSlipTime should only be used in multiprotocol");
  }
}

// BLE related CLI commands
// BLE Scan Priorities
void emberAfPluginDmpTuningSetBleScanPrioritiesCommand(sl_cli_command_arg_t *arguments)
{
  ll_priorities.scan_min = sl_cli_get_argument_uint8(arguments, 0);
  ll_priorities.scan_max = sl_cli_get_argument_uint8(arguments, 1);

  sl_status_t status = sl_bt_system_linklayer_configure(sl_bt_system_linklayer_config_key_set_priority_table,
                                                        sizeof(sl_bt_bluetooth_ll_priorities),
                                                        (const uint8_t*) &ll_priorities);

  if ( status ) {
    emberAfCorePrintln("sl_bt_system_linklayer_configure Error : 0x%X", status);
  } else {
    emberAfCorePrintln("DmpTuning - SetBleScanPriorities: success");
  }
}

void emberAfPluginDmpTuningGetBleScanPrioritiesCommand(sl_cli_command_arg_t *arguments)
{
  emberAfCorePrintln("DmpTuning - GetBleScanPriorities: scan_min:%d scan_max:%d",
                     ll_priorities.scan_min,
                     ll_priorities.scan_max);
}

// BLE Advertisement Priorities
void emberAfPluginDmpTuningSetBleAdvertisementPrioritiesCommand(sl_cli_command_arg_t *arguments)
{
  ll_priorities.adv_min = sl_cli_get_argument_uint8(arguments, 0);
  ll_priorities.adv_max = sl_cli_get_argument_uint8(arguments, 1);

  sl_status_t status = sl_bt_system_linklayer_configure(sl_bt_system_linklayer_config_key_set_priority_table,
                                                        sizeof(sl_bt_bluetooth_ll_priorities),
                                                        (const uint8_t*) &ll_priorities);

  if ( status ) {
    emberAfCorePrintln("sl_bt_system_linklayer_configure Error : 0x%X", status);
  } else {
    emberAfCorePrintln("DmpTuning - SetBleAdvertisementPriorities: success");
  }
}

void emberAfPluginDmpTuningGetBleAdvertisementPrioritiesCommand(sl_cli_command_arg_t *arguments)
{
  emberAfCorePrintln("DmpTuning - GetBleAdvertisementPriorities: adv_min:%d adv_max:%d",
                     ll_priorities.adv_min,
                     ll_priorities.adv_max);
}

// BLE Connection Init Priorities
void emberAfPluginDmpTuningSetBleConnectionInitPrioritiesCommand(sl_cli_command_arg_t *arguments)
{
  ll_priorities.init_min = sl_cli_get_argument_uint8(arguments, 0);
  ll_priorities.init_max = sl_cli_get_argument_uint8(arguments, 1);

  sl_status_t status = sl_bt_system_linklayer_configure(sl_bt_system_linklayer_config_key_set_priority_table,
                                                        sizeof(sl_bt_bluetooth_ll_priorities),
                                                        (const uint8_t*) &ll_priorities);

  if ( status ) {
    emberAfCorePrintln("sl_bt_system_linklayer_configure Error : 0x%X", status);
  } else {
    emberAfCorePrintln("DmpTuning - SetBleConnectionInitPriorities: success");
  }
}

void emberAfPluginDmpTuningGetBleConnectionInitPrioritiesCommand(sl_cli_command_arg_t *arguments)
{
  emberAfCorePrintln("DmpTuning - GetBleConnectionInitPriorities: init_min:%d init_max:%d",
                     ll_priorities.init_min,
                     ll_priorities.init_max);
}

// BLE Connection Priorities
void emberAfPluginDmpTuningSetBleConnectionPrioritiesCommand(sl_cli_command_arg_t *arguments)
{
  ll_priorities.conn_min = sl_cli_get_argument_uint8(arguments, 0);
  ll_priorities.conn_max = sl_cli_get_argument_uint8(arguments, 1);

  sl_status_t status = sl_bt_system_linklayer_configure(sl_bt_system_linklayer_config_key_set_priority_table,
                                                        sizeof(sl_bt_bluetooth_ll_priorities),
                                                        (const uint8_t*) &ll_priorities);

  if ( status ) {
    emberAfCorePrintln("sl_bt_system_linklayer_configure Error : 0x%X", status);
  } else {
    emberAfCorePrintln("DmpTuning - SetBleConnectionPriorities: success");
  }
}

void emberAfPluginDmpTuningGetBleConnectionPrioritiesCommand(sl_cli_command_arg_t *arguments)
{
  emberAfCorePrintln("DmpTuning - GetBleConnectionPriorities: conn_min:%d conn_max:%d",
                     ll_priorities.conn_min,
                     ll_priorities.conn_max);
}

// BLE RAIL Mapping
void emberAfPluginDmpTuningSetBleRailMappingCommand(sl_cli_command_arg_t *arguments)
{
  ll_priorities.rail_mapping_offset = sl_cli_get_argument_uint8(arguments, 0);
  ll_priorities.rail_mapping_range = sl_cli_get_argument_uint8(arguments, 1);

  sl_status_t status = sl_bt_system_linklayer_configure(sl_bt_system_linklayer_config_key_set_priority_table,
                                                        sizeof(sl_bt_bluetooth_ll_priorities),
                                                        (const uint8_t*) &ll_priorities);

  if ( status ) {
    emberAfCorePrintln("sl_bt_system_linklayer_configure Error : 0x%X", status);
  } else {
    emberAfCorePrintln("DmpTuning - SetBleRailMapping: success");
  }
}

void emberAfPluginDmpTuningGetBleRailMappingCommand(sl_cli_command_arg_t *arguments)
{
  emberAfCorePrintln("DmpTuning - GetBleRailMapping: rail_mapping_offset:%d rail_mapping_range:%d",
                     ll_priorities.rail_mapping_offset,
                     ll_priorities.rail_mapping_range);
}

void emberAfPluginDmpTuningStopRadioCommand(sl_cli_command_arg_t *arguments)
{
  emberAfPluginDmpStopRadio();
  emberAfCorePrintln("Radio has been stopped");
}

// CLI for protocol switch related RAIL events.
void emberAfPluginDmpTuningGetRailScheduledEventCounters(sl_cli_command_arg_t *arguments)
{
  emberAfCorePrintln("Scheduled event counter:%d Unscheduled event counter:%d",
                     railScheduledEventCntr,
                     railUnscheduledEventCntr);
}

void emberAfPluginDmpTuningEnableRailConfigSchedulerEvents(sl_cli_command_arg_t *arguments)
{
  emRadioEnableConfigScheduledCallback(true);
  emberAfCorePrintln("DmpTuning - EnableRailConfigSchedulerEvents: success");
}

void emberAfPluginDmpTuningDisableRailConfigSchedulerEvents(sl_cli_command_arg_t *arguments)
{
  emRadioEnableConfigScheduledCallback(false);
  emberAfCorePrintln("DmpTuning - DisableRailConfigSchedulerEvents: success");
}

void emberAfPluginDmpTuningGetRailConfigSchedulerEventsEnabled(sl_cli_command_arg_t *arguments)
{
  emberAfCorePrintln("RAIL config scheduler event enabled: %d",
                     emRadioConfigScheduledCallbackEnabled());
}
