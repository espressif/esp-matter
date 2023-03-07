/***************************************************************************//**
 * @file
 * @brief
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

#ifndef SILABS_UC_TEMP_MACROS_H
#define SILABS_UC_TEMP_MACROS_H

// TODO: We can remove this file when doing the UC_BUILD cleanup works (EMZIGBEE-6321).

// TODO: Once we fully port the pro-compliance app to UC, we can
// get rid of the following and directly use UC arguments and UC functions.
// slxu: sl experimental UC
// chose an unconventional prefix to be able to replace all when we move away from non_uc_build
#ifdef UC_BUILD
#include "sl_component_catalog.h"

#include "zigbee_app_framework_common.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
// ---------- Macros for UC CLI commands ----------
#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"
#define SL_CLI_COMMAND_ARG sl_cli_command_arg_t * arguments
#define emberCommandArgumentCount() sl_cli_get_argument_count(arguments)
#define emberUnsignedCommandArgument(index) sl_cli_get_argument_uint32((arguments), (index))
#define emberSignedCommandArgument(index) sl_cli_get_argument_int32((arguments), (index))
#define emberStringCommandArgument(index, len) sl_zigbee_cli_get_argument_string_and_length((arguments), (index), (len))
#define emberCopyStringArgument(index, dest, max_len, left_pad) sl_zigbee_copy_hex_arg((arguments), (index), (dest), (max_len), (left_pad))
#define emberCopyEui64Argument(index, eui64) sl_zigbee_copy_eui64_arg((arguments), (index), (eui64), false)
#define emberCopyBigEndianEui64Argument(index, eui64) sl_zigbee_copy_eui64_arg((arguments), (index), (eui64), true)
#define emberCopyKeyArgument(index, key_data_ptr)           \
  (sl_zigbee_copy_hex_arg((arguments),                      \
                          (index),                          \
                          emberKeyContents((key_data_ptr)), \
                          EMBER_ENCRYPTION_KEY_SIZE,        \
                          true))
#else // !SL_CATALOG_CLI_PRESENT
#define SL_CLI_COMMAND_ARG void
#endif // SL_CATALOG_CLI_PRESENT
// ---------- Macros for UC Events ----------
// slxu: sl experimental UC
// chose an unconventional prefix to be able to replace all when we move away from non_uc_build
#define slxu_zigbee_event_set_active(x) sl_zigbee_event_set_active(x)
#define slxu_zigbee_event_set_inactive(x) sl_zigbee_event_set_inactive(x)
#define slxu_zigbee_event_set_delay_ms(x, y) sl_zigbee_event_set_delay_ms(x, y)
#define slxu_zigbee_event_set_delay_qs(x, y) sl_zigbee_event_set_delay_qs((x), (y))
#define slxu_zigbee_event_set_delay_minutes(x, y) sl_zigbee_event_set_delay_minutes(x, y)
#define slxu_zigbee_event_is_active(x) sl_zigbee_event_is_scheduled(x)
#define slxu_zigbee_event_init(x, y) sl_zigbee_event_init(x, y)
#define slxu_zigbee_network_event_init(x, y) sl_zigbee_network_event_init(x, y)
#define slxu_zigbee_endpoint_event_set_inactive(x, y) sl_zigbee_endpoint_event_set_inactive(x, y)
#define slxu_zigbee_endpoint_event_set_delay_ms(x, y, z) sl_zigbee_endpoint_event_set_delay_ms(x, y, z)
#define SLXU_UC_EVENT sl_zigbee_event_t * event
// ---------- Macros for UC Printf ----------
// TODO: Once we fully port the pro-compliance app to UC, we can
// clean this up and use "sl_zigbee_stack_debug_print()" instead.
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
#include "sl_zigbee_debug_print.h"
#define emberSerialPrintf(port, ...) local_printf(__VA_ARGS__)
#define emberSerialGuaranteedPrintf(port, ...) local_printf(__VA_ARGS__)
#define emberSerialPrintCarriageReturn(port) local_printf("\r\n")
#define emberSerialPrintfLine(port, ...) local_printf(__VA_ARGS__); local_printf("\r\n")
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
// ---------- Macros for UC Ticks ----------
// EMZIGBEE-6471
#define slxu_zigbee_zcl_schedule_server_tick(endpoint, clusterId, delayMs) sl_zigbee_zcl_schedule_server_tick(endpoint, clusterId, delayMs)
#define slxu_zigbee_zcl_schedule_client_tick(endpoint, clusterId, delayMs) sl_zigbee_zcl_schedule_client_tick(endpoint, clusterId, delayMs)
#define slxu_zigbee_zcl_deactivate_client_tick(endpoint, clusterId) sl_zigbee_zcl_deactivate_client_tick(endpoint, clusterId)
#define slxu_zigbee_zcl_deactivate_server_tick(endpoint, clusterId) sl_zigbee_zcl_deactivate_server_tick(endpoint, clusterId)
#define slxu_zigbee_zcl_schedule_client_tick_extended(endpoint, clusterId, delayMs, pollControl, sleepControl) \
  sl_zigbee_zcl_schedule_client_tick_extended(endpoint, clusterId, delayMs, pollControl, sleepControl)
#define slxu_zigbee_zcl_schedule_server_tick_extended(endpoint, clusterId, delayMs, pollControl, sleepControl) \
  sl_zigbee_zcl_schedule_server_tick_extended(endpoint, clusterId, delayMs, pollControl, sleepControl)
#if defined(SL_CATALOG_SIMPLE_LED_PRESENT)
#include "sl_simple_led_instances.h"
#define slxu_led_turn_on(led) sl_led_turn_on(led)
#define slxu_led_turn_off(led) sl_led_turn_off(led)
#define slxu_led_toggle(led) sl_led_toggle(led)
#else
#define slxu_led_turn_on(led)
#define slxu_led_turn_off(led)
#define slxu_led_toggle(led)
#endif // SL_CATALOG_SIMPLE_LED_PRESENT
// ---------- Macros for Init callbacks ----------
#define SLXU_INIT_ARG uint8_t init_level
#define SLXU_INIT_UNUSED_ARG (void)init_level
extern uint8_t ascii_lut[];
#else // !UC_BUILD
#include "hal.h"
#define SL_CLI_COMMAND_ARG void
#define slxu_zigbee_event_set_active(x) emberEventControlSetActive(x)
#define slxu_zigbee_event_set_inactive(x) emberEventControlSetInactive(x)
#define slxu_zigbee_event_set_delay_ms(x, y) emberEventControlSetDelayMS(x, y)
#define slxu_zigbee_event_set_delay_qs(x, y) emberEventControlSetDelayQS(x, y)
#define slxu_zigbee_event_set_delay_minutes(x, y) emberEventControlSetDelayMinutes(x, y)
#define slxu_zigbee_event_is_active(x) emberEventControlGetActive(x)
#define slxu_zigbee_event_init(x, y)
#define slxu_zigbee_network_event_init(x, y)
#define slxu_zigbee_endpoint_event_set_inactive(x, y) emberAfEndpointEventControlSetInactive(x, y)
#define slxu_zigbee_endpoint_event_set_delay_ms(x, y, z) emberAfEndpointEventControlSetDelayMS(x, y, z)
#define SLXU_UC_EVENT void
#define slxu_zigbee_zcl_schedule_server_tick(endpoint, clusterId, delayMs) emberAfScheduleServerTick(endpoint, clusterId, delayMs)
#define slxu_zigbee_zcl_schedule_client_tick(endpoint, clusterId, delayMs) emberAfScheduleClientTick(endpoint, clusterId, delayMs)
#define slxu_zigbee_zcl_deactivate_server_tick(endpoint, clusterId) emberAfDeactivateServerTick(endpoint, clusterId)
#define slxu_zigbee_zcl_deactivate_client_tick(endpoint, clusterId) emberAfDeactivateClientTick(endpoint, clusterId)
#define slxu_zigbee_zcl_schedule_client_tick_extended(endpoint, clusterId, delayMs, pollControl, sleepControl) \
  emberAfScheduleClientTickExtended(endpoint, clusterId, delayMs, pollControl, sleepControl)
#define slxu_zigbee_zcl_schedule_server_tick_extended(endpoint, clusterId, delayMs, pollControl, sleepControl) \
  emberAfScheduleServerTickExtended(endpoint, clusterId, delayMs, pollControl, sleepControl)
#define slxu_led_turn_on(led) halSetLed(led)
#define slxu_led_turn_off(led) halClearLed(led)
#define slxu_led_toggle(led) halToggleLed(led)
#define SLXU_INIT_ARG void
#define SLXU_INIT_UNUSED_ARG
#endif // UC_BUILD

#endif // SILABS_UC_TEMP_MACROS_H
