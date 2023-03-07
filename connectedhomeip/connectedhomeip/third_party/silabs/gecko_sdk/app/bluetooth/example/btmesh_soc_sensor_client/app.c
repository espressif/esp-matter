/***************************************************************************//**
 * @file
 * @brief Application code
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <stdio.h>
#include "em_common.h"
#include "sl_status.h"

#include "sl_btmesh.h"
#include "sl_bluetooth.h"
#include "app.h"
#include "app_assert.h"
#include "app_log.h"

#include "gatt_db.h"

#include "sl_btmesh_sensor_client.h"

/* Buttons and LEDs headers */
#include "app_button_press.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "sl_simple_timer.h"
#include "sl_btmesh_factory_reset.h"
#include "sl_btmesh_provisioning_decorator.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_BTMESH_WSTK_LCD_PRESENT
#include "sl_btmesh_wstk_lcd.h"
#endif // SL_CATALOG_BTMESH_WSTK_LCD_PRESENT

/// High Priority
#define HIGH_PRIORITY                  0
/// No Timer Options
#define NO_FLAGS                       0
/// Callback has not parameters
#define NO_CALLBACK_DATA               (void *)NULL
/// timeout for registering new devices after startup
#define DEVICE_REGISTER_SHORT_TIMEOUT  100
/// timeout for registering new devices after startup
#define DEVICE_REGISTER_LONG_TIMEOUT   20000
/// timeout for periodic sensor data update
#define SENSOR_DATA_TIMEOUT            2000
/// Timeout for Blinking LED during provisioning
#define APP_LED_BLINKING_TIMEOUT       250
/// Connection uninitialized
#define UNINITIALIZED_CONNECTION       0xFF
/// Advertising Provisioning Bearer
#define PB_ADV                         0x1
/// GATT Provisioning Bearer
#define PB_GATT                        0x2
/// Length of the display name buffer
#define NAME_BUF_LEN                   20
/// Length of boot error message buffer
#define BOOT_ERR_MSG_BUF_LEN           30
/// Used button indexes
#define BUTTON_PRESS_BUTTON_0          0
#define BUTTON_PRESS_BUTTON_1          1

#ifdef SL_CATALOG_BTMESH_WSTK_LCD_PRESENT
#define lcd_print(...) sl_btmesh_LCD_write(__VA_ARGS__)
#else
#define lcd_print(...)
#endif // SL_CATALOG_BTMESH_WSTK_LCD_PRESENT

// -------------------------------
// Periodic timer handles
static sl_simple_timer_t app_sensor_data_timer;
static sl_simple_timer_t app_update_registered_devices_timer;
static sl_simple_timer_t app_led_blinking_timer;

// -------------------------------
// Periodic timer callbacks
static void app_sensor_data_timer_cb(sl_simple_timer_t *handle,
                                     void *data);
static void app_update_registered_devices_timer_cb(sl_simple_timer_t *handle,
                                                   void *data);
static void app_led_blinking_timer_cb(sl_simple_timer_t *handle,
                                      void *data);

/// Number of active Bluetooth connections
static uint8_t num_connections = 0;

static bool init_done = false;

/// Currently displayed property ID
static mesh_device_properties_t current_property = PRESENT_AMBIENT_TEMPERATURE;

/// Property IDs supported by application
static void sensor_client_change_current_property(void);

/***************************************************************************//**
 * Change buttons to LEDs in case of shared pin
 *
 ******************************************************************************/
void change_buttons_to_leds(void)
{
  app_button_press_disable();
  // Disable button and enable led
  sl_simple_button_disable(&sl_button_btn0);
  sl_simple_led_init(sl_led_led0.context);
  // Disable button and enable led
#ifndef SINGLE_BUTTON
  sl_simple_button_disable(&sl_button_btn1);
#endif // SINGLE_BUTTON
#ifndef SINGLE_LED
  sl_simple_led_init(sl_led_led1.context);
#endif //SINGLE_LED
}

/***************************************************************************//**
 * Change LEDs to buttons in case of shared pin
 *
 ******************************************************************************/
void change_leds_to_buttons(void)
{
  // Enable buttons
  sl_simple_button_enable(&sl_button_btn0);
#ifndef SINGLE_BUTTON
  sl_simple_button_enable(&sl_button_btn1);
#endif // SINGLE_BUTTON
  // Wait
  sl_sleeptimer_delay_millisecond(1);
  // Enable button presses
  app_button_press_enable();
}

/*******************************************************************************
 * Application Init.
 ******************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
  app_log("Bt Mesh Sensor Client initialized\r\n");
  // Ensure right init order in case of shared pin for enabling buttons
  change_buttons_to_leds();
  // Change LEDs to buttons in case of shared pin
  change_leds_to_buttons();
}

/*******************************************************************************
 * Application Process Action.
 ******************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/*******************************************************************************
 * Get the currently set property ID
 *
 * @return Current property ID
 ******************************************************************************/
mesh_device_properties_t app_get_current_property(void)
{
  return current_property;
}

/***************************************************************************//**
 * Set device name in the GATT database. A unique name is generated using
 * the two last bytes from the UUID of this device. Name is also
 * displayed on the LCD.
 *
 * @param[in] uuid  Pointer to device UUID.
 ******************************************************************************/
static void set_device_name(uuid_128 *uuid)
{
  char name[NAME_BUF_LEN];
  sl_status_t result;

  // Create unique device name using the last two bytes of the device UUID
  snprintf(name,
           NAME_BUF_LEN,
           "sensor client %02x%02x",
           uuid->data[14],
           uuid->data[15]);

  app_log("Device name: '%s'\r\n", name);

  result = sl_bt_gatt_server_write_attribute_value(gattdb_device_name,
                                                   0,
                                                   strlen(name),
                                                   (uint8_t *)name);
  if (result != SL_STATUS_OK) {
    app_log("sl_bt_gatt_server_write_attribute_value() failed, code %x\r\n",
            result);
  }

  // Show device name on the LCD
  lcd_print(name, SL_BTMESH_WSTK_LCD_ROW_NAME_CFG_VAL);
}

/***************************************************************************//**
 * Handles button press and does a factory reset
 *
 * @return true if there is no button press
 ******************************************************************************/
bool handle_reset_conditions(void)
{
  // If PB0 is held down then do full factory reset
  if (sl_simple_button_get_state(&sl_button_btn0)
      == SL_SIMPLE_BUTTON_PRESSED) {
    // Disable button presses
    app_button_press_disable();
    // Full factory reset
    sl_btmesh_initiate_full_reset();
    return false;
  }

#ifndef SINGLE_BUTTON
  // If PB1 is held down then do node factory reset
  if (sl_simple_button_get_state(&sl_button_btn1)
      == SL_SIMPLE_BUTTON_PRESSED) {
    // Disable button presses
    app_button_press_disable();
    // Node factory reset
    sl_btmesh_initiate_node_reset();
    return false;
  }
#endif // SL_CATALOG_BTN1_PRESENT
  return true;
}

/***************************************************************************//**
 * Handling of boot event.
 * If needed it performs factory reset. In other case it sets device name
 * and initialize mesh node.
 ******************************************************************************/
static void handle_boot_event(void)
{
  sl_status_t sc;
  char buf[BOOT_ERR_MSG_BUF_LEN];
  uuid_128 uuid;
  // Check reset conditions and continue if not reset.
  if (handle_reset_conditions()) {
    // Initialize Mesh stack in Node operation mode, wait for initialized event
    sc = sl_btmesh_node_init();
    if (sc != SL_STATUS_OK) {
      snprintf(buf, BOOT_ERR_MSG_BUF_LEN, "init failed (0x%lx)", sc);
      lcd_print(buf, SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
      app_log("Initialization failed (0x%x)\r\n", sc);
    } else {
      sc = sl_btmesh_node_get_uuid(&uuid);
      app_assert_status_f(sc, "Failed to get UUID\n");
      set_device_name(&uuid);
    }
  }
}

/***************************************************************************//**
 * Handling of mesh node initialized event.
 * If device is provisioned it initializes the sensor server node.
 * If device is unprovisioned it starts sending Unprovisioned Device Beacons.
 *
 * @param[in] evt  Pointer to mesh node initialized event.
 ******************************************************************************/
static void handle_node_initialized_event(
  sl_btmesh_evt_node_initialized_t* evt)
{
  if (evt->provisioned) {
    sl_status_t sc =
      sl_simple_timer_start(&app_update_registered_devices_timer,
                            DEVICE_REGISTER_SHORT_TIMEOUT,
                            app_update_registered_devices_timer_cb,
                            NO_CALLBACK_DATA,
                            false);
    app_assert_status_f(sc, "Failed to start timer\r\n");
  } else {
    // Enable ADV and GATT provisioning bearer
    sl_status_t sc = sl_btmesh_node_start_unprov_beaconing(PB_ADV | PB_GATT);

    app_assert_status_f(sc, "Failed to start unprovisioned beaconing\n");
  }
}

/***************************************************************************//**
 *  Handling of mesh node provisioning events.
 *  It handles:
 *   - mesh_node_provisioning_started
 *   - mesh_node_provisioned
 *   - mesh_node_provisioning_failed
 *
 *  @param[in] evt  Pointer to incoming provisioning event.
 ******************************************************************************/
void handle_node_provisioning_events(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    sl_status_t sc;
    case sl_btmesh_evt_node_provisioned_id:
      // Update registered devices after startup
      sc = sl_simple_timer_start(&app_update_registered_devices_timer,
                                 DEVICE_REGISTER_LONG_TIMEOUT,
                                 app_update_registered_devices_timer_cb,
                                 NO_CALLBACK_DATA,
                                 false);
      app_assert_status_f(sc, "Failed to start timer\r\n");
      break;

    default:
      break;
  }
}

/***************************************************************************//**
 *  Handling of le connection events.
 *  It handles:
 *   - le_connection_opened
 *   - le_connection_parameters
 *   - le_connection_closed
 *
 *  @param[in] evt  Pointer to incoming connection event.
 ******************************************************************************/
static void handle_le_connection_events(sl_bt_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_connection_opened_id:
      num_connections++;
      lcd_print("connected", SL_BTMESH_WSTK_LCD_ROW_CONNECTION_CFG_VAL);
      app_log("Connected\r\n");
      break;

    case sl_bt_evt_connection_closed_id:
      if (num_connections > 0) {
        if (--num_connections == 0) {
          lcd_print("", SL_BTMESH_WSTK_LCD_ROW_CONNECTION_CFG_VAL);
          app_log("Disconnected\r\n");
        }
      }
      break;

    default:
      break;
  }
}

/***************************************************************************//**
 * Handling of stack events. Both Bluetooth LE and Bluetooth mesh events
 * are handled here.
 * @param[in] evt_id  Incoming event ID.
 * @param[in] evt     Pointer to incoming event.
 ******************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      handle_boot_event();
      break;

    case sl_bt_evt_connection_opened_id:
    case sl_bt_evt_connection_parameters_id:
    case sl_bt_evt_connection_closed_id:
      handle_le_connection_events(evt);
      break;

    default:
      break;
  }
}

/***************************************************************************//**
 * Bluetooth Mesh stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Pointer to incoming event from the Bluetooth Mesh stack.
 ******************************************************************************/
void sl_btmesh_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
      handle_node_initialized_event(&(evt->data.evt_node_initialized));
      break;

    case sl_btmesh_evt_node_provisioned_id:
      handle_node_provisioning_events(evt);
      break;

    default:
      break;
  }
}

void update_registered_devices(void)
{
  sl_status_t sc;

  sl_btmesh_sensor_client_update_registered_devices(current_property);

  sc = sl_simple_timer_start(&app_sensor_data_timer,
                             SENSOR_DATA_TIMEOUT,
                             app_sensor_data_timer_cb,
                             NO_CALLBACK_DATA,
                             true);

  app_assert_status_f(sc, "Failed to start periodic timer\n");
}

/***************************************************************************//**
 * Callbacks
 ******************************************************************************/
void app_button_press_cb(uint8_t button, uint8_t duration)
{
  (void)duration;
  if (duration == APP_BUTTON_PRESS_NONE) {
    return;
  }
  // button pressed
  if (button == BUTTON_PRESS_BUTTON_0) {
    if (duration < APP_BUTTON_PRESS_DURATION_LONG) {
      app_log("PB0 pressed\r\n");
      sensor_client_change_current_property();
    } else {
      app_log("PB0 long pressed\r\n");
      update_registered_devices();
    }
  } else if (button == BUTTON_PRESS_BUTTON_1) {
    app_log("PB1 pressed\r\n");
    update_registered_devices();
  }
}

/***************************************************************************//**
 * Timer Callbacks
 ******************************************************************************/
static void app_sensor_data_timer_cb(sl_simple_timer_t *handle,
                                     void *data)
{
  (void)data;
  (void)handle;
  sl_btmesh_sensor_client_get_sensor_data(current_property);
}

static void app_update_registered_devices_timer_cb(sl_simple_timer_t *handle,
                                                   void *data)
{
  (void)data;
  (void)handle;
  sl_status_t sc;

  sl_btmesh_sensor_client_update_registered_devices(current_property);

  sc = sl_simple_timer_start(&app_sensor_data_timer,
                             SENSOR_DATA_TIMEOUT,
                             app_sensor_data_timer_cb,
                             NO_CALLBACK_DATA,
                             true);

  app_assert_status_f(sc, "Failed to start periodic timer\r\n");
}

static void app_led_blinking_timer_cb(sl_simple_timer_t *handle,
                                      void *data)
{
  (void)data;
  (void)handle;
  if (!init_done) {
    // Toggle LEDs
    sl_led_led0.toggle(sl_led_led0.context);
#ifndef SINGLE_LED
    sl_led_led1.toggle(sl_led_led1.context);
#endif // SINGLE_LED
  }
}

/***************************************************************************//**
 * It changes currently displayed property ID.
 ******************************************************************************/
static void sensor_client_change_current_property(void)
{
  switch (current_property) {
    case PRESENT_AMBIENT_TEMPERATURE:
      current_property = PEOPLE_COUNT;
      break;
    case PEOPLE_COUNT:
      current_property = PRESENT_AMBIENT_LIGHT_LEVEL;
      break;
    case PRESENT_AMBIENT_LIGHT_LEVEL:
      current_property = PRESENT_AMBIENT_TEMPERATURE;
      break;
    default:
      app_log("Unsupported property ID change\r\n");
      break;
  }
}

/***************************************************************************//**
 * Provisioning Decorator Callbacks
 ******************************************************************************/
// Called when the Provisioning starts
void sl_btmesh_on_node_provisioning_started(uint16_t result)
{
  // Change buttons to LEDs in case of shared pin
  change_buttons_to_leds();

  sl_status_t sc = sl_simple_timer_start(&app_led_blinking_timer,
                                         APP_LED_BLINKING_TIMEOUT,
                                         app_led_blinking_timer_cb,
                                         NO_CALLBACK_DATA,
                                         true);

  app_assert_status_f(sc, "Failed to start periodic timer\r\n");

  app_show_btmesh_node_provisioning_started(result);
}

// Called when the Provisioning finishes successfully
void sl_btmesh_on_node_provisioned(uint16_t address,
                                   uint32_t iv_index)
{
  sl_status_t sc = sl_simple_timer_stop(&app_led_blinking_timer);
  app_assert_status_f(sc, "Failed to stop periodic timer\r\n");
  // Turn off LED
  init_done = true;
  sl_led_led0.turn_off(sl_led_led0.context);
#ifndef SINGLE_LED
  sl_led_led1.turn_off(sl_led_led1.context);
#endif // SINGLE_LED
  // Change LEDs to buttons in case of shared pin
  change_leds_to_buttons();

  app_show_btmesh_node_provisioned(address, iv_index);
}
