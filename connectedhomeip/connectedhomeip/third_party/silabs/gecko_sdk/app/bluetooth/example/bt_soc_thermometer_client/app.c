/***************************************************************************//**
 * @file
 * @brief Core application logic.
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
#include <stdbool.h>
#include <math.h>
#include "em_common.h"
#include "app_log.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"
#endif // SL_CATALOG_CLI_PRESENT
#include "app.h"

// connection parameters
#define CONN_INTERVAL_MIN             80   //100ms
#define CONN_INTERVAL_MAX             80   //100ms
#define CONN_RESPONDER_LATENCY        0    //no latency
#define CONN_TIMEOUT                  100  //1000ms
#define CONN_MIN_CE_LENGTH            0
#define CONN_MAX_CE_LENGTH            0xffff

#define TEMP_INVALID                  NAN
#define UNIT_INVALID                  ('?')
#define UNIT_CELSIUS                  ('C')
#define UNIT_FAHRENHEIT               ('F')
#define RSSI_INVALID                  ((int8_t)0x7F)
#define CONNECTION_HANDLE_INVALID     ((uint8_t)0xFFu)
#define SERVICE_HANDLE_INVALID        ((uint32_t)0xFFFFFFFFu)
#define CHARACTERISTIC_HANDLE_INVALID ((uint16_t)0xFFFFu)
#define TABLE_INDEX_INVALID           ((uint8_t)0xFFu)
#define TX_POWER_INVALID              ((uint8_t)0x7C)
#define TX_POWER_CONTROL_ACTIVE       ((uint8_t)0x00)
#define TX_POWER_CONTROL_INACTIVE     ((uint8_t)0x01)
#define PRINT_TX_POWER_DEFAULT        (false)

#if SL_BT_CONFIG_MAX_CONNECTIONS < 1
  #error At least 1 connection has to be enabled!
#endif

// Macro to translate the Flags to Celsius (C) or Fahrenheit (F). Flags is the first byte of the
// Temperature Measurement characteristic value according to the Bluetooth SIG
#define translate_flags_to_temperature_unit(flags) (((flags) & 1) ? UNIT_FAHRENHEIT : UNIT_CELSIUS)

typedef enum {
  scanning,
  opening,
  discover_services,
  discover_characteristics,
  enable_indication,
  running
} conn_state_t;

typedef struct {
  uint8_t  connection_handle;
  int8_t   rssi;
  bool     power_control_active;
  int8_t   tx_power;
  int8_t   remote_tx_power;
  uint16_t server_address;
  uint32_t thermometer_service_handle;
  uint16_t thermometer_characteristic_handle;
  float    temperature;
  char     unit;
} conn_properties_t;

typedef struct {
  uint8_t mantissa_l;
  uint8_t mantissa_m;
  int8_t mantissa_h;
  int8_t exponent;
} IEEE_11073_float;

// Array for holding properties of multiple (parallel) connections
static conn_properties_t conn_properties[SL_BT_CONFIG_MAX_CONNECTIONS];
// Counter of active connections
static uint8_t active_connections_num;
// State of the connection under establishment
static conn_state_t conn_state;
// Health Thermometer service UUID defined by Bluetooth SIG
static const uint8_t thermo_service[2] = { 0x09, 0x18 };
// Temperature Measurement characteristic UUID defined by Bluetooth SIG
static const uint8_t thermo_char[2] = { 0x1c, 0x2a };
// Print out tx power value
static bool print_tx_power = PRINT_TX_POWER_DEFAULT;

static void init_properties(void);
static uint8_t find_service_in_advertisement(uint8_t *data, uint8_t len);
static uint8_t find_index_by_connection_handle(uint8_t connection);
static void add_connection(uint8_t connection, uint16_t address);
// Remove a connection from the connection_properties array
static void remove_connection(uint8_t connection);
static float translate_IEEE_11073_temperature_to_float(IEEE_11073_float const *IEEE_11073_value);
static bd_addr *read_and_cache_bluetooth_address(uint8_t *address_type_out);
static void print_bluetooth_address(void);
// Print RSSI and temperature values
static void print_values(void);

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  // Initialize connection properties
  init_properties();
  app_log_info("soc_thermometer_client initialised\n");
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t* evt)
{
  sl_status_t sc;
  uint8_t *char_value;
  uint16_t addr_value;
  uint8_t table_index;

  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Print boot message.
      app_log_info("Bluetooth stack booted: v%d.%d.%d-b%d\n",
                   evt->data.evt_system_boot.major,
                   evt->data.evt_system_boot.minor,
                   evt->data.evt_system_boot.patch,
                   evt->data.evt_system_boot.build);
      // Print bluetooth address.
      print_bluetooth_address();

      // Set the default connection parameters for subsequent connections
      sc = sl_bt_connection_set_default_parameters(CONN_INTERVAL_MIN,
                                                   CONN_INTERVAL_MAX,
                                                   CONN_RESPONDER_LATENCY,
                                                   CONN_TIMEOUT,
                                                   CONN_MIN_CE_LENGTH,
                                                   CONN_MAX_CE_LENGTH);
      app_assert_status(sc);
      // Start scanning - looking for thermometer devices
      sc = sl_bt_scanner_start(sl_bt_gap_phy_1m, sl_bt_scanner_discover_generic);
      app_assert_status_f(sc,
                          "Failed to start discovery #1\n");
      conn_state = scanning;
      break;

    // -------------------------------
    // This event is generated when an advertisement packet or a scan response
    // is received from a responder
    case sl_bt_evt_scanner_legacy_advertisement_report_id:
      // Parse advertisement packets
      if (evt->data.evt_scanner_legacy_advertisement_report.event_flags
          == (SL_BT_SCANNER_EVENT_FLAG_CONNECTABLE | SL_BT_SCANNER_EVENT_FLAG_SCANNABLE)) {
        // If a thermometer advertisement is found...
        if (find_service_in_advertisement(&(evt->data.evt_scanner_legacy_advertisement_report.data.data[0]),
                                          evt->data.evt_scanner_legacy_advertisement_report.data.len) != 0) {
          // then stop scanning for a while
          sc = sl_bt_scanner_stop();
          app_assert_status(sc);
          // and connect to that device
          if (active_connections_num < SL_BT_CONFIG_MAX_CONNECTIONS) {
            sc = sl_bt_connection_open(evt->data.evt_scanner_legacy_advertisement_report.address,
                                       evt->data.evt_scanner_legacy_advertisement_report.address_type,
                                       sl_bt_gap_phy_1m,
                                       NULL);
            app_assert_status(sc);
            conn_state = opening;
          }
        }
      }
      break;

    // -------------------------------
    // This event is generated when a new connection is established
    case sl_bt_evt_connection_opened_id:
      // Get last two bytes of sender address
      addr_value = (uint16_t)(evt->data.evt_connection_opened.address.addr[1] << 8) + evt->data.evt_connection_opened.address.addr[0];
      // Add connection to the connection_properties array
      add_connection(evt->data.evt_connection_opened.connection, addr_value);
      // Discover Health Thermometer service on the responder device
      sc = sl_bt_gatt_discover_primary_services_by_uuid(evt->data.evt_connection_opened.connection,
                                                        sizeof(thermo_service),
                                                        (const uint8_t*)thermo_service);
      app_assert_status(sc);

      // Set remote connection power reporting - needed for Power Control
      sc = sl_bt_connection_set_remote_power_reporting(
        evt->data.evt_connection_opened.connection,
        sl_bt_connection_power_reporting_enable);
      app_assert_status(sc);

      conn_state = discover_services;
      break;

    // -------------------------------
    // This event is generated when a new service is discovered
    case sl_bt_evt_gatt_service_id:
      table_index = find_index_by_connection_handle(evt->data.evt_gatt_service.connection);
      if (table_index != TABLE_INDEX_INVALID) {
        // Save service handle for future reference
        conn_properties[table_index].thermometer_service_handle = evt->data.evt_gatt_service.service;
      }
      break;

    // -------------------------------
    // This event is generated when a new characteristic is discovered
    case sl_bt_evt_gatt_characteristic_id:
      table_index = find_index_by_connection_handle(evt->data.evt_gatt_characteristic.connection);
      if (table_index != TABLE_INDEX_INVALID) {
        // Save characteristic handle for future reference
        conn_properties[table_index].thermometer_characteristic_handle = evt->data.evt_gatt_characteristic.characteristic;
      }
      break;

    // -------------------------------
    // This event is generated for various procedure completions, e.g. when a
    // write procedure is completed, or service discovery is completed
    case sl_bt_evt_gatt_procedure_completed_id:
      table_index = find_index_by_connection_handle(evt->data.evt_gatt_procedure_completed.connection);
      if (table_index == TABLE_INDEX_INVALID) {
        break;
      }
      // If service discovery finished
      if (conn_state == discover_services && conn_properties[table_index].thermometer_service_handle != SERVICE_HANDLE_INVALID) {
        // Discover thermometer characteristic on the responder device
        sc = sl_bt_gatt_discover_characteristics_by_uuid(evt->data.evt_gatt_procedure_completed.connection,
                                                         conn_properties[table_index].thermometer_service_handle,
                                                         sizeof(thermo_char),
                                                         (const uint8_t*)thermo_char);
        app_assert_status(sc);
        conn_state = discover_characteristics;
        break;
      }
      // If characteristic discovery finished
      if (conn_state == discover_characteristics && conn_properties[table_index].thermometer_characteristic_handle != CHARACTERISTIC_HANDLE_INVALID) {
        // stop discovering
        sl_bt_scanner_stop();
        // enable indications
        sc = sl_bt_gatt_set_characteristic_notification(evt->data.evt_gatt_procedure_completed.connection,
                                                        conn_properties[table_index].thermometer_characteristic_handle,
                                                        sl_bt_gatt_indication);
        app_assert_status(sc);
        conn_state = enable_indication;
        break;
      }
      // If indication enable process finished
      if (conn_state == enable_indication) {
        // and we can connect to more devices
        if (active_connections_num < SL_BT_CONFIG_MAX_CONNECTIONS) {
          // start scanning again to find new devices
          sc = sl_bt_scanner_start(sl_bt_gap_phy_1m, sl_bt_scanner_discover_generic);
          app_assert_status_f(sc,
                              "Failed to start discovery #2\n");
          conn_state = scanning;
        } else {
          conn_state = running;
        }
        break;
      }
      break;

    // -------------------------------
    // This event is generated when a connection is dropped
    case sl_bt_evt_connection_closed_id:
      // remove connection from active connections
      remove_connection(evt->data.evt_connection_closed.connection);
      if (conn_state != scanning) {
        // start scanning again to find new devices
        sc = sl_bt_scanner_start(sl_bt_gap_phy_1m, sl_bt_scanner_discover_generic);
        app_assert_status_f(sc,
                            "Failed to start discovery #3\n");
        conn_state = scanning;
      }
      break;

    // -------------------------------
    // This event is generated when a characteristic value was received e.g. an indication
    case sl_bt_evt_gatt_characteristic_value_id:
      if (evt->data.evt_gatt_characteristic_value.value.len >= 5) {
        char_value = &(evt->data.evt_gatt_characteristic_value.value.data[0]);
        table_index = find_index_by_connection_handle(evt->data.evt_gatt_characteristic_value.connection);
        if (table_index != TABLE_INDEX_INVALID) {
          conn_properties[table_index].temperature = translate_IEEE_11073_temperature_to_float((IEEE_11073_float *)(char_value + 1));
          conn_properties[table_index].unit = translate_flags_to_temperature_unit(char_value[0]);
        }
      } else {
        app_log_warning("Characteristic value too short: %d\n",
                        evt->data.evt_gatt_characteristic_value.value.len);
      }
      // Send confirmation for the indication
      sc = sl_bt_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection);
      app_assert_status(sc);
      // Trigger RSSI measurement on the connection
      sc = sl_bt_connection_get_rssi(evt->data.evt_gatt_characteristic_value.connection);
      app_assert_status(sc);
      break;

    // -------------------------------
    // This event is generated when RSSI value was measured
    case sl_bt_evt_connection_rssi_id:
      table_index = find_index_by_connection_handle(evt->data.evt_connection_rssi.connection);
      if (table_index != TABLE_INDEX_INVALID) {
        conn_properties[table_index].rssi = evt->data.evt_connection_rssi.rssi;
      }

      print_values();
      break;

    // -------------------------------
    // TX Power is updated
    case sl_bt_evt_connection_tx_power_id:

      table_index = find_index_by_connection_handle(
        evt->data.evt_connection_tx_power.connection);

      if (table_index != TABLE_INDEX_INVALID) {
        conn_properties[table_index].tx_power =
          evt->data.evt_connection_tx_power.power_level;
      }

      // TX Power reporting is enabled on the other side.
      conn_properties[table_index].power_control_active =
        TX_POWER_CONTROL_ACTIVE;
      break;

    // -------------------------------
    // Remote TX Power is updated
    case sl_bt_evt_connection_remote_tx_power_id:
      table_index = find_index_by_connection_handle(
        evt->data.evt_connection_remote_tx_power.connection);

      if (table_index != TABLE_INDEX_INVALID) {
        conn_properties[table_index].remote_tx_power =
          evt->data.evt_connection_remote_tx_power.power_level;
      }
      break;

    default:
      break;
  }
}

// Init connection properties
static void init_properties(void)
{
  uint8_t i;
  active_connections_num = 0;

  for (i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    conn_properties[i].connection_handle = CONNECTION_HANDLE_INVALID;
    conn_properties[i].thermometer_service_handle = SERVICE_HANDLE_INVALID;
    conn_properties[i].thermometer_characteristic_handle = CHARACTERISTIC_HANDLE_INVALID;
    conn_properties[i].temperature = TEMP_INVALID;
    conn_properties[i].unit = UNIT_INVALID;
    conn_properties[i].rssi = RSSI_INVALID;
    conn_properties[i].power_control_active = TX_POWER_CONTROL_INACTIVE;
    conn_properties[i].tx_power = TX_POWER_INVALID;
    conn_properties[i].remote_tx_power = TX_POWER_INVALID;
  }
}

// Parse advertisements looking for advertised Health Thermometer service
static uint8_t find_service_in_advertisement(uint8_t *data, uint8_t len)
{
  uint8_t ad_field_length;
  uint8_t ad_field_type;
  uint8_t i = 0;
  // Parse advertisement packet
  while (i < len) {
    ad_field_length = data[i];
    ad_field_type = data[i + 1];
    // Partial ($02) or complete ($03) list of 16-bit UUIDs
    if (ad_field_type == 0x02 || ad_field_type == 0x03) {
      // compare UUID to Health Thermometer service UUID
      if (memcmp(&data[i + 2], thermo_service, 2) == 0) {
        return 1;
      }
    }
    // advance to the next AD struct
    i = i + ad_field_length + 1;
  }
  return 0;
}

// Find the index of a given connection in the connection_properties array
static uint8_t find_index_by_connection_handle(uint8_t connection)
{
  for (uint8_t i = 0; i < active_connections_num; i++) {
    if (conn_properties[i].connection_handle == connection) {
      return i;
    }
  }
  return TABLE_INDEX_INVALID;
}

// Add a new connection to the connection_properties array
static void add_connection(uint8_t connection, uint16_t address)
{
  conn_properties[active_connections_num].connection_handle = connection;
  conn_properties[active_connections_num].server_address    = address;
  active_connections_num++;
}

// Remove a connection from the connection_properties array
static void remove_connection(uint8_t connection)
{
  uint8_t i;
  uint8_t table_index = find_index_by_connection_handle(connection);

  if (active_connections_num > 0) {
    active_connections_num--;
  }
  // Shift entries after the removed connection toward 0 index
  for (i = table_index; i < active_connections_num; i++) {
    conn_properties[i] = conn_properties[i + 1];
  }
  // Clear the slots we've just removed so no junk values appear
  for (i = active_connections_num; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    conn_properties[i].connection_handle = CONNECTION_HANDLE_INVALID;
    conn_properties[i].thermometer_service_handle = SERVICE_HANDLE_INVALID;
    conn_properties[i].thermometer_characteristic_handle = CHARACTERISTIC_HANDLE_INVALID;
    conn_properties[i].temperature = TEMP_INVALID;
    conn_properties[i].unit = UNIT_INVALID;
    conn_properties[i].rssi = RSSI_INVALID;
    conn_properties[i].power_control_active = TX_POWER_CONTROL_INACTIVE;
    conn_properties[i].tx_power = TX_POWER_INVALID;
    conn_properties[i].remote_tx_power = TX_POWER_INVALID;
  }
}

// Translate a IEEE-11073 Temperature Value to a float Value
static float translate_IEEE_11073_temperature_to_float(IEEE_11073_float const *IEEE_11073_value)
{
  int32_t mantissa = 0;
  uint8_t mantissa_l;
  uint8_t mantissa_m;
  int8_t mantissa_h;
  int8_t exponent;

  // Wrong Argument: NULL pointer is passed
  if ( !IEEE_11073_value ) {
    return NAN;
  }

  // Caching Fields
  mantissa_l = IEEE_11073_value->mantissa_l;
  mantissa_m = IEEE_11073_value->mantissa_m;
  mantissa_h = IEEE_11073_value->mantissa_h;
  exponent =  IEEE_11073_value->exponent;

  // IEEE-11073 Standard NaN Value Passed
  if ((mantissa_l == 0xFF) && (mantissa_m == 0xFF) && (mantissa_h == 0x7F) && (exponent == 0x00)) {
    return NAN;
  }

  // Converting a 24bit Signed Value to a 32bit Signed Value
  mantissa |= mantissa_h;
  mantissa <<= 8;
  mantissa |= mantissa_m;
  mantissa <<= 8;
  mantissa |= mantissa_l;
  mantissa <<= 8;
  mantissa >>= 8;

  return ((float)mantissa) * pow(10.0f, (float)exponent);
}

/**************************************************************************//**
 * @brief
 *   Function to Read and Cache Bluetooth Address.
 * @param address_type_out [out]
 *   A pointer to the outgoing address_type. This pointer can be NULL.
 * @return
 *   Pointer to the cached Bluetooth Address
 *****************************************************************************/
static bd_addr *read_and_cache_bluetooth_address(uint8_t *address_type_out)
{
  static bd_addr address;
  static uint8_t address_type;
  static bool cached = false;

  if (!cached) {
    sl_status_t sc = sl_bt_system_get_identity_address(&address, &address_type);
    app_assert_status(sc);
    cached = true;
  }

  if (address_type_out) {
    *address_type_out = address_type;
  }

  return &address;
}

/**************************************************************************//**
 * @brief
 *   Function to Print Bluetooth Address.
 * @return
 *   None
 *****************************************************************************/
static void print_bluetooth_address(void)
{
  uint8_t address_type;
  bd_addr *address = read_and_cache_bluetooth_address(&address_type);

  app_log_info("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
               address_type ? "static random" : "public device",
               address->addr[5],
               address->addr[4],
               address->addr[3],
               address->addr[2],
               address->addr[1],
               address->addr[0]);
}

// Print parameters to STDOUT. CR used to display results.
void print_values(void)
{
  static bool print_header = true;
  static bool previous_print_tx_power = PRINT_TX_POWER_DEFAULT;
  uint8_t i;

  // If TX power print request changes - header should be updated.
  if (previous_print_tx_power != print_tx_power) {
    previous_print_tx_power = print_tx_power;
    print_header = true;
  }

  // Print header
  if (true == print_header) {
    app_log_info("");
    for (i = 0u; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
      if (false == print_tx_power) {
        app_log_append("ADDR   TEMP   RSSI |");
      } else {
        app_log_append("ADDR   TEMP   RSSI    TXPW |");
      }
    }
    app_log_append("\n");

    print_header = false;
  }

  app_log_info("");
  // Print parameters
  for (i = 0u; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if ((TEMP_INVALID != conn_properties[i].temperature) && (RSSI_INVALID != conn_properties[i].rssi) ) {
      app_log_append("%04x ", conn_properties[i].server_address);
      app_log_append("%6.2f", conn_properties[i].temperature);
      app_log_append("%c ", conn_properties[i].unit);
      app_log_append("% 3d", conn_properties[i].rssi);
      app_log_append("dBm");
      if (true == print_tx_power) {
        app_log_append(" %4d", conn_properties[i].tx_power);
        app_log_append("dBm");
      }
      app_log_append("|");
    } else if (false == print_tx_power) {
      app_log_append("---- ------- ------|");
    } else {
      app_log_append("----  ------ ------  ------|");
    }
  }
  app_log_append("\r");
}

#ifdef SL_CATALOG_CLI_PRESENT
void hello(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  print_bluetooth_address();
}

void toggle_print_tx_power(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  print_tx_power = !print_tx_power;
}
#endif // SL_CATALOG_CLI_PRESENT
