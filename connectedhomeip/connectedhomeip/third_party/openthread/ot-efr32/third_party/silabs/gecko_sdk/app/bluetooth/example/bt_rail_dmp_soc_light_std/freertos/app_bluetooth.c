/***************************************************************************//**
 * @file
 * @brief Bluetooth application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <string.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "portmacro.h"
#include "em_common.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"
#endif // SL_CATALOG_CLI_PRESENT
#include "demo-ui.h"
#include "sl_bt_api.h"
#include "sl_bt_rtos_adaptation.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "app_bluetooth.h"
#include "app_proprietary.h"
#include "sl_status.h"
#include "sl_simple_timer.h"

// -----------------------------------------------------------------------------
// Constant definitions and macros

#define UINT16_TO_BYTES(n)            ((uint8_t) (n)), ((uint8_t)((n) >> 8))
#define UINT16_TO_BYTE0(n)            ((uint8_t) (n))
#define UINT16_TO_BYTE1(n)            ((uint8_t) ((n) >> 8))

#define TIMER_CLK_FREQ ((uint32_t)32768) // Timer Frequency used.

// Convert to timer ticks
#define TIMER_MS_2_TIMERTICK(ms) ((TIMER_CLK_FREQ * ms) / 1000)
#define TIMER_S_2_TIMERTICK(s) (TIMER_CLK_FREQ * s)

// GATT database definitins
#define LIGHT_STATE_GATTDB     gattdb_light_state_rail
#define TRIGGER_SOURCE_GATTDB  gattdb_trigger_source_rail
#define SOURCE_ADDRESS_GATTDB  gattdb_source_address_rail

// Write response codes
#define ES_WRITE_OK                  0
#define DEV_ID_STR_LEN               9u

#define DEMO_TIMER_DIRECTION_TICKS   5    // Indicates 5 Timer Ticks timeout
// for direction
#define DEMO_TIMER_TICKS             50   // Indicates 500 Timer Ticks timeout
// for refreshing timer
#define TIMEOUT_INDICATE_MS          500  // 500 ms timeout for indication

// Queue Number of messages
#define DEMO_QUEUE_SIZE 32
#define BLE_QUEUE_SIZE 32

/// Demo Task Parameters
#define DEMO_APP_TASK_NAME        "Demo task"
#define DEMO_APP_TASK_STACK_SIZE  250
#define DEMO_APP_TASK_PRIO        5u

/// BT Task Parameters
#define BT_APP_TASK_NAME        "BT event handler Task"
#define BT_APP_TASK_STACK_SIZE  250
#define BT_APP_TASK_PRIO        6u

// We need to put the device name into a scan response packet,
// since it isn't included in the 'standard' beacons -
// I've included the flags, since certain apps seem to expect them
#define DEVNAME "DMP%02X%02X"
#define DEVNAME_LEN 8             // incl term null
#define UUID_LEN 16               // 128-bit UUID
#define IBEACON_MAJOR_NUM 0x0200  // 16-bit major number

// -----------------------------------------------------------------------------
// Data type definitions

typedef enum {
  BLE_INDICATION_TYPE_LIGHT      = 0x0,  // (0x0) Indication for Light
  BLE_INDICATION_TYPE_DIRECTION  = 0x1,  // (0x1) Indication for direction
  BLE_INDICATION_TYPE_ADDRESS    = 0x2   // (0x2) Indication for source address
} ble_indication_type;

typedef enum {
  BLE_STATE_IDLE                    = 0, // (0) Wait for start
  BLE_STATE_INDICATE                = 1, // (1) Send indication/notification
  BLE_STATE_WAIT                    = 2, // (2) Handle timeouts and success
} ble_state_t;

typedef struct {
  uint8_t handle;
  bd_addr address;
  bool in_use;
} ble_conn_t;

typedef enum {
  BLE_COMMAND_NONE,
  BLE_COMMAND_READ_CHARACTERISTIC_VALUE,
  BLE_COMMAND_SEND_WRITE_RESPONSE,
  BLE_COMMAND_INDICATION,
  BLE_COMMAND_INDICATION_SUCCESS,
  BLE_COMMAND_INDICATION_TIMEOUT
} ble_command;

typedef struct {
  uint8_t command;
  uint8_t indication_type;
  uint8_t connection_handle;
  uint8_t param;
} ble_msg_struct_t;

typedef union {
  uint32_t raw_data;
  ble_msg_struct_t message;
} ble_msg_t;

typedef struct {
  uint8_t flags_len;                // Length of the Flags field.
  uint8_t flags_type;               // Type of the Flags field.
  uint8_t flags;                    // Flags field.
  uint8_t short_name_len;           // Length of Shortened Local Name.
  uint8_t short_name_type;          // Shortened Local Name.
  uint8_t short_name[DEVNAME_LEN];  // Shortened Local Name.
  uint8_t uuid_length;              // Length of UUID.
  uint8_t uuid_type;                // Type of UUID.
  uint8_t uuid[UUID_LEN];           // 128-bit UUID.
} response_data_t;

// -----------------------------------------------------------------------------
// Public variables

// Application state
demo_t demo = {
  .state = DEMO_STATE_INIT,
  .light = demo_light_off,
  .light_ind = sl_bt_gatt_disable,
  .direction = demo_light_direction_button,
  .direction_ind = sl_bt_gatt_disable,
  .src_addr = { { 0, 0, 0, 0, 0, 0, 0, 0 } },
  .src_addr_ind = sl_bt_gatt_disable,
  .conn_bluetooth_in_use = 0,
  .conn_proprietary_in_use = 0,
  .indication_ongoing = false,
  .indication_pending = false,
  .own_addr = { { 0, 0, 0, 0, 0, 0, 0, 0 } }
};

// -----------------------------------------------------------------------------
// Private variables

// indication databases array for the indication types
static uint8_t ble_indication_db[3] = { LIGHT_STATE_GATTDB,
                                        TRIGGER_SOURCE_GATTDB,
                                        SOURCE_ADDRESS_GATTDB };

// Demo queue
static QueueHandle_t  demo_queue;
static demo_msg_t demo_queue_storage[DEMO_QUEUE_SIZE];
static StaticQueue_t demo_queue_buffer;
// BLE queue
static QueueHandle_t  ble_queue;
static uint32_t ble_queue_storage[BLE_QUEUE_SIZE];
static StaticQueue_t ble_queue_buffer;
// Demo task
StackType_t demo_app_task_stack[DEMO_APP_TASK_STACK_SIZE] = { 0 };
StaticTask_t demo_app_task_buffer;
TaskHandle_t demo_app_task_handle;
// BLE task
StackType_t bt_app_task_stack[BT_APP_TASK_STACK_SIZE] = { 0 };
StaticTask_t bt_app_task_buffer;
TaskHandle_t bt_app_task_handle;

// Os timers
static TimerHandle_t demo_timer;
static TimerHandle_t demo_timer_direction;

/// BLE connection table
static ble_conn_t ble_conn[SL_BT_CONFIG_MAX_CONNECTIONS];

// The advertising set handle allocated from Bluetooth stack
static uint8_t advertising_set_handle  = 0xff;

static response_data_t response_data = {
  2,                // length (incl type)
  0x01,             // type
  0x04 | 0x02,      // Flags: LE General Discoverable Mode, BR/EDR is disabled.
  DEVNAME_LEN + 1,  // length of local name (incl type)
  0x08,
  { 'D', 'M', '0', '0', ':', '0', '0' }, // shortened local name
  UUID_LEN + 1,     // length of UUID data (incl type)
  0x06,             // incomplete list of service UUID's
  // custom service UUID for silabs light in little-endian format
  { 0x13, 0x87, 0x37, 0x25, 0x42, 0xb0, 0xc3, 0xbf,
    0x78, 0x40, 0x83, 0xb5, 0xe4, 0x96, 0xf5, 0x63 }
};

// Light Mutex
static SemaphoreHandle_t light_mutex_handle;
static StaticSemaphore_t light_mutex_buffer;
/// single timer handle
static sl_simple_timer_t app_single_timer;

// -----------------------------------------------------------------------------
// Private function definitions

static void demo_timer_cb(TimerHandle_t xTimer);
static void demo_timer_direction_cb(TimerHandle_t xTimer);
static void demo_app_task(void *p_arg);
static void bt_event_handler_task(void *p_arg);
static void ble_queue_post(ble_msg_t ble_message);
static demo_msg_t demo_queue_pend(void);

// -----------------------------------------------------------------------------
// Public functions

/**************************************************************************//**
 * Initialization of the demo app
 *
 *****************************************************************************/
void init_demo_app(void)
{
  light_mutex_handle = xSemaphoreCreateMutexStatic(&light_mutex_buffer);

  demo_queue = xQueueCreateStatic(DEMO_QUEUE_SIZE,
                                  sizeof(demo_msg_t),
                                  (uint8_t *)demo_queue_storage,
                                  &demo_queue_buffer);

  ble_queue = xQueueCreateStatic(BLE_QUEUE_SIZE,
                                 sizeof(uint32_t),
                                 (uint8_t *)ble_queue_storage,
                                 &ble_queue_buffer);

  // Create the Demo task.
  demo_app_task_handle = xTaskCreateStatic(demo_app_task,
                                           DEMO_APP_TASK_NAME,
                                           DEMO_APP_TASK_STACK_SIZE,
                                           NULL,
                                           DEMO_APP_TASK_PRIO,
                                           demo_app_task_stack,
                                           &demo_app_task_buffer);

  // Create BT event handler Task
  bt_app_task_handle = xTaskCreateStatic(bt_event_handler_task,
                                         BT_APP_TASK_NAME,
                                         BT_APP_TASK_STACK_SIZE,
                                         NULL,
                                         BT_APP_TASK_PRIO,
                                         bt_app_task_stack,
                                         &bt_app_task_buffer);
}

/**************************************************************************//**
 * Aquire Light mutex
 *****************************************************************************/
void light_pend(void)
{
  BaseType_t ret;
  app_assert( (!CORE_InIrqContext()), "[Error] Calling from IRQ" APP_LOG_NEW_LINE);
  ret = xSemaphoreTake(light_mutex_handle, portMAX_DELAY);
  app_assert(pdTRUE == ret, "Failed to pend light_mutex" APP_LOG_NEW_LINE);
}

/**************************************************************************//**
 * Release Light mutex
 *****************************************************************************/
void light_post(void)
{
  BaseType_t ret;
  ret = xSemaphoreGive(light_mutex_handle);
  app_assert(pdTRUE == ret, "Failed to post light_mutex" APP_LOG_NEW_LINE);
}

/**************************************************************************//**
 * Posting a message to the Demo task
 *
 * @param[in] ble_message message to post
 *****************************************************************************/
void demo_queue_post(demo_msg_t msg)
{
  BaseType_t ret;
  ret = xQueueSend(demo_queue, (void *)&msg, portMAX_DELAY);
  app_assert(pdTRUE == ret, "OS error demo_queue_post" APP_LOG_NEW_LINE);
}

// -----------------------------------------------------------------------------
// Private functions

/**************************************************************************//**
 * Acquire Light mutex
 * @param[in] handle timer handle
 * @param[in] data additional data
 *****************************************************************************/
static void app_single_timer_cb(sl_simple_timer_t *handle,
                                void *data)
{
  (void)data;
  (void)handle;
  ble_msg_t msg;
  msg.message.command = BLE_COMMAND_INDICATION_TIMEOUT;
  ble_queue_post(msg);
}

/**************************************************************************//**
 * Switching LEDs off
 *
 *****************************************************************************/
void appUiLedOff(void)
{
  sl_led_turn_off(SL_SIMPLE_LED_INSTANCE(0));
  sl_led_turn_off(SL_SIMPLE_LED_INSTANCE(1));
}

/**************************************************************************//**
 * Switching LEDs on
 *
 *****************************************************************************/
void appUiLedOn(void)
{
  sl_led_turn_on(SL_SIMPLE_LED_INSTANCE(0));
  sl_led_turn_on(SL_SIMPLE_LED_INSTANCE(1));
}

/**************************************************************************//**
 * Getting a message from Demo queue
 *
 * @returns the message read from the queue
 *****************************************************************************/
static demo_msg_t demo_queue_pend(void)
{
  demo_msg_t demo_msg;
  BaseType_t ret;
  ret = xQueueReceive(demo_queue, (void *)&demo_msg, portMAX_DELAY);
  app_assert(pdTRUE == ret, "OS error demo_queue_pend" APP_LOG_NEW_LINE);
  return demo_msg;
}

/**************************************************************************//**
 * Getting a message from Bluetooth queue
 *
 * @returns the message pointer read from the queue
 *****************************************************************************/
static uint32_t ble_queue_pend(void)
{
  uint32_t ble_msg = 0;
  BaseType_t ret;
  ret = xQueueReceive(ble_queue, (void *)&ble_msg, portMAX_DELAY);
  app_assert(pdTRUE == ret, "OS error ble_queue_pend" APP_LOG_NEW_LINE);
  return ble_msg;
}

/**************************************************************************//**
 * Posting a message to the Bluetooth task
 *
 * @param[in] ble_message message to post
 *****************************************************************************/
static void ble_queue_post(ble_msg_t ble_message)
{
  BaseType_t ret;
  ret = xQueueSend(ble_queue, (void *)&ble_message.raw_data, portMAX_DELAY);
  app_assert(pdTRUE == ret, "OS error ble_queue_post" APP_LOG_NEW_LINE);
}

/**************************************************************************//**
 * Demo Timer callback.
 *
 * @param p_tmr pointer to the user-allocated timer.
 * @param p_arg argument passed when creating the timer.
 *
 *****************************************************************************/
static void demo_timer_cb(TimerHandle_t xTimer)
{
  (void)xTimer;
  demo_queue_post(DEMO_EVT_INDICATION);
}

/**************************************************************************//**
 * Direction Timer callback.
 *
 * @param p_tmr pointer to the user-allocated timer.
 * @param p_arg argument passed when creating the timer.
 *
 *****************************************************************************/
static void demo_timer_direction_cb(TimerHandle_t xTimer)
{
  (void)xTimer;
  demo_queue_post(DEMO_EVT_CLEAR_DIRECTION);
}

/**************************************************************************//**
 * Find a registered connection by specifying the handle
 *
 * @param[in] handle connection handle to find
 * @param[out] position index of the connection
 * @returns true if successfully found
 *****************************************************************************/
//return false if the handle not found
static bool ble_find_conn(uint8_t handle,
                          uint8_t * pos)
{
  for (uint8_t i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if (ble_conn[i].handle == handle) {
      *pos = i;
      return true;
    }
  }
  return false;
}

/**************************************************************************//**
 * Check empty slots for connection registration
 *
 * @param[out] position index of the empty slot
 * @returns false if no free slot found
 *****************************************************************************/
static bool ble_has_empty(uint8_t * position)
{
  for (uint8_t i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if (ble_conn[i].in_use == false) {
      if ( position != NULL ) {
        *position = i;
      }
      return true;
    }
  }
  return false;
}

/**************************************************************************//**
 * Register a Bluetooth connection
 *
 * @param[in] handle connection handle to register
 * @param[in] address Bluetooth address to be registered
 * @returns false on error
 *****************************************************************************/
static bool ble_add_conn(uint8_t handle,
                         bd_addr *address)
{
  uint8_t i;
  // Check empty slots
  if ( ble_has_empty(&i) ) {
    // Aquire mutex
    light_pend();
    ble_conn[i].handle = handle;
    memcpy((void*)&ble_conn[i].address,
           (void*)address,
           sizeof(ble_conn[i].address));
    ble_conn[i].in_use = true;
    // Release mutex
    light_post();
    return true;
  }
  return false;
}

/**************************************************************************//**
 * Removes a Bluetooth connection
 *
 * @param[in] handle connection handle
 * @returns false if it is not registered
 *****************************************************************************/
static bool ble_remove_conn(uint8_t handle)
{
  uint8_t i;
  // Find connection handle
  if ( ble_find_conn(handle, &i) ) {
    // Aquire mutex
    light_pend();
    ble_conn[i].handle = 0;
    memset((void*)&ble_conn[i].address.addr,
           0,
           sizeof(ble_conn[i].address.addr));
    ble_conn[i].in_use = false;
    // Release mutex
    light_post();
    return true;
  }
  return false;
}

/**************************************************************************//**
 * Get Bluetooth address
 *
 * @param[in] handle connection handle
 * @returns Bluetooth address for the specified connection
 *****************************************************************************/
static bd_addr* ble_get_address(uint8_t handle)
{
  uint8_t i;
  if ( ble_find_conn(handle, &i) ) {
    return &(ble_conn[i].address);
  }
  return (bd_addr *)NULL;
}

static ble_indication_type ble_actual_indication = BLE_INDICATION_TYPE_LIGHT;
static uint8_t ble_actual_client = 0;

/**************************************************************************//**
 * Indicate or notify the next client if a registered client is available
 *
 * @param[out] waitForResponse set true if client needs to be indicated
 * @returns false if nothing is sent
 *****************************************************************************/
static bool ble_indicate(bool * waitForResponse)
{
  sl_status_t sc = SL_STATUS_OK;
  uint8_t data_len = 0;
  uint8_t data_send[8];
  bool ret = true;

  // Set default response behavior
  *waitForResponse = false;

  // Return false if this connection is not in use
  if (ble_conn[ble_actual_client].in_use == false) {
    return false;
  }

  // get mutex
  light_pend();

  switch (ble_actual_indication) {
    case BLE_INDICATION_TYPE_LIGHT:
      data_len = (uint8_t)sizeof(demo.light);
      memcpy(data_send, (uint8_t*)&(demo.light), data_len);
      break;
    case BLE_INDICATION_TYPE_DIRECTION:
      data_len = (uint8_t)sizeof(demo.direction);
      memcpy(data_send, (uint8_t*)&(demo.direction), data_len);
      break;
    case BLE_INDICATION_TYPE_ADDRESS:
      data_len = (uint8_t)sizeof(demo.src_addr.addr);
      memcpy(data_send, (uint8_t*)demo.src_addr.addr, data_len);
      break;
  }

  // release mutex
  light_post();

  // Read client configuration descriptor
  uint16_t flags;
  sc = sl_bt_gatt_server_read_client_configuration(ble_conn[ble_actual_client].handle,
                                                   ble_indication_db[ble_actual_indication],
                                                   &flags);
  sl_bt_gatt_client_config_flag_t client_config_flags = (sl_bt_gatt_client_config_flag_t) flags;
  app_assert_status(sc);

  if (client_config_flags & sl_bt_gatt_indication) {
    // Send indication data
    sc = sl_bt_gatt_server_send_indication(ble_conn[ble_actual_client].handle,
                                           ble_indication_db[ble_actual_indication],
                                           (uint8_t)data_len,
                                           (uint8_t*)data_send);
    *waitForResponse = true;
  } else if (client_config_flags & sl_bt_gatt_notification) {
    // Send notification data
    sc = sl_bt_gatt_server_send_notification(ble_conn[ble_actual_client].handle,
                                             ble_indication_db[ble_actual_indication],
                                             (uint8_t)data_len,
                                             (uint8_t*)data_send);
  } else {
    ret = false;
  }
  if (sc != SL_STATUS_OK) {
    ret = false;
  }

  return ret;
}

/**************************************************************************//**
 * Writer for read requests
 *
 * @param[out] waitForResponse set true if client needs to be indicated
 * @returns true if there is a next client to be indicated
 *****************************************************************************/
static bool ble_indicate_next(bool * waitForResponse)
{
  while (ble_actual_indication < 3) {
    while (ble_actual_client < SL_BT_CONFIG_MAX_CONNECTIONS) {
      if (ble_indicate(waitForResponse)) {
        ble_actual_client++;
        return true;
      }
      ble_actual_client++;
    }
    ble_actual_client = 0;
    ble_actual_indication++;
  }
  return false;
}

/**************************************************************************//**
 * Start of indication/notification sequence
 *
 *****************************************************************************/
static void ble_indicate_begin()
{
  // Start from first
  ble_actual_client = 0;
  ble_actual_indication = BLE_INDICATION_TYPE_LIGHT;
}

/**************************************************************************//**
 * Writer for read requests
 *
 * @param[in] connection Bluetooth connection handle
 * @param[in] indication_type type of indication
 * @returns false if writing response to read request was not successful
 *****************************************************************************/
static bool ble_write(uint8_t connection,
                      ble_indication_type indication_type)
{
  sl_status_t sc;
  uint16_t len;
  uint8_t data_len = 0;

  // Get mutex
  light_pend();

  uint8_t data_send[8];

  switch (indication_type) {
    case BLE_INDICATION_TYPE_LIGHT:
      data_len = (uint8_t)sizeof(demo.light);
      memcpy(data_send, (uint8_t*)&(demo.light), data_len);
      break;
    case BLE_INDICATION_TYPE_DIRECTION:
      data_len = (uint8_t)sizeof(demo.direction);
      memcpy(data_send, (uint8_t*)&(demo.direction), data_len);
      break;
    case BLE_INDICATION_TYPE_ADDRESS:
      data_len = (uint8_t)sizeof(demo.src_addr.addr);
      memcpy(data_send, (uint8_t*)demo.src_addr.addr, data_len);
      break;
  }

  // Release mutex
  light_post();

  sc = sl_bt_gatt_server_send_user_read_response(connection,
                                                 ble_indication_db[indication_type],
                                                 0,
                                                 (uint8_t)data_len,
                                                 (uint8_t*)data_send,
                                                 &len
                                                 );
  app_assert_status(sc);

  return true;
}

/**************************************************************************//**
 * Response to write requests
 *
 * @param[in] connection Bluetooth connection handle
 * @param[in] indication_type type of indication
 * @param[in] response true if the response is success
 * @returns Returns false if respond to write request was not successful
 *****************************************************************************/
static bool ble_respond(uint8_t connection,
                        ble_indication_type indication_type,
                        bool response)
{
  sl_status_t sc;

  // Send response to write request
  sc = sl_bt_gatt_server_send_user_write_response(connection,
                                                  ble_indication_db[indication_type],
                                                  !response);
  app_assert_status(sc);
  return true;
}

/**************************************************************************//**
 * Initialize Bluetooth connection storage for this application
 *
 *****************************************************************************/
void ble_init_conn()
{
  for (uint8_t i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++ ) {
    ble_conn[i].handle = 0;
    ble_conn[i].in_use = false;
  }
}

/**************************************************************************//**
 * Simple Button
 * Button state changed callback
 * @param[in] handle Button enevnt handle
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  // button pressed
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&sl_button_btn0 == handle) {
      demo_queue_post(DEMO_EVT_BUTTON0_PRESSED);
    } else if (&sl_button_btn1 == handle) {
      demo_queue_post(DEMO_EVT_BUTTON1_PRESSED);
    }
  }
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc = SL_STATUS_OK;
  bd_addr address;
  uint8_t address_type;
  uint8_t system_id[8];
  uint8_t addr[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  uint8_t *addr_poi = addr;
  ble_msg_t ble_message;

  // -------------------------------
  // Bluetooth stack event handling.
  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!       //
    case sl_bt_evt_system_boot_id:
      light_pend();
      // Extract unique ID from BT Address
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      app_assert_status(sc);

      // Pad and reverse unique ID to get System ID
      system_id[0] = address.addr[5];
      system_id[1] = address.addr[4];
      system_id[2] = address.addr[3];
      system_id[3] = 0xFF;
      system_id[4] = 0xFE;
      system_id[5] = address.addr[2];
      system_id[6] = address.addr[1];
      system_id[7] = address.addr[0];
      sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id,
                                                   0,
                                                   sizeof(system_id),
                                                   system_id);
      app_assert_status(sc);

      // own address: Bluetooth device address
      memset((void *)demo.own_addr.addr, 0, sizeof(demo.own_addr.addr));
      memcpy((void *)demo.own_addr.addr,
             (void *)&address,
             sizeof(address));
      demo.indication_ongoing = false;
      demo_queue_post(DEMO_EVT_BOOTED);
      light_post();
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      sc = ble_add_conn(evt->data.evt_connection_opened.connection,
                        (bd_addr *)&evt->data.evt_connection_opened.address);
      app_assert(sc == true,
                 "[E: 0x%04x] Failed to add conn" APP_LOG_NEW_LINE,
                 (int)sc);
      demo_queue_post(DEMO_EVT_BLUETOOTH_CONNECTED);
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      sc = ble_remove_conn(evt->data.evt_connection_closed.connection);
      app_assert(sc == true,
                 "[E: 0x%04x] Failed to remove conn" APP_LOG_NEW_LINE,
                 (int)sc);
      demo_queue_post(DEMO_EVT_BLUETOOTH_DISCONNECTED);
      break;

    // -------------------------------
    // This event indicates that a remote GATT client is attempting to write
    // a value of a user type attribute in to the local GATT database.
    case sl_bt_evt_gatt_server_user_write_request_id:
      // light state write
      if (LIGHT_STATE_GATTDB == evt->data.evt_gatt_server_user_write_request.characteristic) {
        addr_poi = (uint8_t*)ble_get_address(evt->data.evt_gatt_server_user_write_request.connection);
        app_assert(addr_poi != NULL,
                   "[E: 0x%04x] Failed to get ble address" APP_LOG_NEW_LINE,
                   (int)sc);

        light_pend();
        demo.light = (demo_light_t)evt->data.evt_gatt_server_user_write_request.value.data[0];
        demo.direction = demo_light_direction_bluetooth;
        memcpy(demo.src_addr.addr, (void const*)addr_poi, sizeof(bd_addr));
        light_post();

        // Serve write request
        ble_message.message.command = BLE_COMMAND_SEND_WRITE_RESPONSE;
        ble_message.message.connection_handle = evt->data.evt_gatt_server_user_write_request.connection;
        ble_message.message.indication_type = BLE_INDICATION_TYPE_LIGHT;
        ble_message.message.param = true;
        ble_queue_post(ble_message);

        demo_queue_post(DEMO_EVT_LIGHT_CHANGED_BLUETOOTH);
      }
      break;

    // -------------------------------
    // This event indicates that a remote GATT client is attempting to read a value of an
    // attribute from the local GATT database, where the attribute was defined in the GATT
    // XML firmware configuration file to have type="user".
    case sl_bt_evt_gatt_server_user_read_request_id:
      // light state read
      if (LIGHT_STATE_GATTDB == evt->data.evt_gatt_server_user_read_request.characteristic) {
        // Serve read request
        ble_message.message.command = BLE_COMMAND_READ_CHARACTERISTIC_VALUE;
        ble_message.message.connection_handle = evt->data.evt_gatt_server_user_read_request.connection;
        ble_message.message.indication_type = BLE_INDICATION_TYPE_LIGHT;
        ble_queue_post(ble_message);
      } else if (TRIGGER_SOURCE_GATTDB == evt->data.evt_gatt_server_user_read_request.characteristic) {
        // Serve read request
        ble_message.message.command = BLE_COMMAND_READ_CHARACTERISTIC_VALUE;
        ble_message.message.connection_handle = evt->data.evt_gatt_server_user_read_request.connection;
        ble_message.message.indication_type = BLE_INDICATION_TYPE_DIRECTION;
        ble_queue_post(ble_message);
      } else if (SOURCE_ADDRESS_GATTDB == evt->data.evt_gatt_server_user_read_request.characteristic) {
        // Serve read request
        ble_message.message.command = BLE_COMMAND_READ_CHARACTERISTIC_VALUE;
        ble_message.message.connection_handle = evt->data.evt_gatt_server_user_read_request.connection;
        ble_message.message.indication_type = BLE_INDICATION_TYPE_ADDRESS;
        ble_queue_post(ble_message);
      }
      break;

    // -------------------------------
    // This event indicates either that a local Client Characteristic Configuration descriptor
    // has been changed by the remote GATT client, or that a confirmation from the remote GATT
    // client was received upon a successful reception of the indication.
    case sl_bt_evt_gatt_server_characteristic_status_id:
      if (LIGHT_STATE_GATTDB == evt->data.evt_gatt_server_characteristic_status.characteristic) {
        // confirmation of indication received from remote GATT client
        if (sl_bt_gatt_server_confirmation == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          ble_message.message.command = BLE_COMMAND_INDICATION_SUCCESS;
          ble_message.message.connection_handle = evt->data.evt_gatt_server_characteristic_status.connection;
          ble_message.message.indication_type = BLE_INDICATION_TYPE_LIGHT;
          ble_queue_post(ble_message);
        } else if (sl_bt_gatt_server_client_config == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          // unhandled event
        } else {
        }
      } else if (TRIGGER_SOURCE_GATTDB == evt->data.evt_gatt_server_characteristic_status.characteristic) {
        // confirmation of indication received from GATT client
        if (sl_bt_gatt_server_confirmation == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          ble_message.message.command = BLE_COMMAND_INDICATION_SUCCESS;
          ble_message.message.connection_handle = evt->data.evt_gatt_server_characteristic_status.connection;
          ble_message.message.indication_type = BLE_INDICATION_TYPE_DIRECTION;
          ble_queue_post(ble_message);
        } else if (sl_bt_gatt_server_client_config == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          // unhandled event
        } else {
        }
      } else if (SOURCE_ADDRESS_GATTDB == evt->data.evt_gatt_server_characteristic_status.characteristic) {
        // confirmation of indication received from GATT client
        if (sl_bt_gatt_server_confirmation == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          ble_message.message.command = BLE_COMMAND_INDICATION_SUCCESS;
          ble_message.message.connection_handle = evt->data.evt_gatt_server_characteristic_status.connection;
          ble_message.message.indication_type = BLE_INDICATION_TYPE_ADDRESS;
          ble_queue_post(ble_message);
        } else if (sl_bt_gatt_server_client_config == (sl_bt_gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          // unhandled event
        } else {
        }
      } else {
      }
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}

/**************************************************************************//**
 * Handles read and write operations
 *
 * @param[in] received_ble_msg message to respond to
 * @param[in] handle true if message handling is requested
 * @returns Returns true if needs to be handled
 *****************************************************************************/
static bool bt_read_write_handling(ble_msg_t received_ble_msg, bool handle)
{
  if (!handle) {
    // Not to handle valid requests
    if ((received_ble_msg.message.command
         == BLE_COMMAND_READ_CHARACTERISTIC_VALUE)
        || (received_ble_msg.message.command
            == BLE_COMMAND_SEND_WRITE_RESPONSE)) {
      return true;
    } else {
      return false;
    }
  }
  if (received_ble_msg.message.command
      == BLE_COMMAND_READ_CHARACTERISTIC_VALUE) {
    // Respond to read request
    ble_write(received_ble_msg.message.connection_handle,
              (ble_indication_type)received_ble_msg.message.indication_type);
    return true;
  } else if (received_ble_msg.message.command
             == BLE_COMMAND_SEND_WRITE_RESPONSE) {
    // Respond write request
    ble_respond(received_ble_msg.message.connection_handle,
                (ble_indication_type)received_ble_msg.message.indication_type,
                received_ble_msg.message.param);
    return true;
  }
  return false;
}

/**************************************************************************//**
 * Bluetooth event task.
 *
 * @param[in] p_arg the argument passed by 'OSTaskCreate()'.
 *****************************************************************************/
static void bt_event_handler_task(void *p_arg)
{
  (void)p_arg;
  ble_state_t ble_state = BLE_STATE_IDLE;
  sl_status_t sc;
  ble_msg_t received_ble_msg;
  ble_msg_t ble_message_state;
  bool waitForResponse;

  // Init connection state array
  ble_init_conn();

  while (1) {
    // -------------------------------
    // Bluetooth event handling.
    // Wait for a message
    received_ble_msg.raw_data = (uint32_t) ble_queue_pend();
    switch (ble_state) {
      case BLE_STATE_IDLE:
        //if R/W handling is the message
        if (!bt_read_write_handling(received_ble_msg, true)) {
          if (received_ble_msg.message.command == BLE_COMMAND_INDICATION) {
            // Start indication pointers
            ble_indicate_begin();
            // Move to indication with a message
            ble_state = BLE_STATE_INDICATE;
            ble_message_state.message.command = BLE_COMMAND_INDICATION;
            ble_queue_post(ble_message_state);
          }
        }
        break;
      case BLE_STATE_INDICATE:
        // Handle R/W messages first
        if (!bt_read_write_handling(received_ble_msg, true)) {
          // If indication was successful
          if (ble_indicate_next(&waitForResponse) ) {
            // indication case, wait for response
            if (waitForResponse) {
              // Start the timer
              sc = sl_simple_timer_start(&app_single_timer,
                                         TIMEOUT_INDICATE_MS,
                                         app_single_timer_cb,
                                         (void *)NULL,
                                         false);

              app_assert_status(sc);
              // Go and wait for response in BLE_STATE_WAIT
              ble_state = BLE_STATE_WAIT;
            } else {
              // Stay in this state with a message
              ble_message_state.message.command = BLE_COMMAND_INDICATION;
              ble_queue_post(ble_message_state);
            }
          } else {
            // Finished this round
            demo_queue_post(DEMO_EVT_INDICATION_SUCCESSFUL);
            ble_state = BLE_STATE_IDLE;
          }
        }
        break;
      case BLE_STATE_WAIT:
        // Handle R/W messages first
        if (!bt_read_write_handling(received_ble_msg, true)) {
          // if indication was successful go to next
          if (received_ble_msg.message.command
              == BLE_COMMAND_INDICATION_SUCCESS) {
            // stop direction indication confirmation timer
            sc = sl_simple_timer_stop(&app_single_timer);
            app_assert_status(sc);
            // Go to indication, to indicate next client
            ble_state = BLE_STATE_INDICATE;
            ble_message_state.message.command = BLE_COMMAND_INDICATION;
            ble_queue_post(ble_message_state);
          } else if (received_ble_msg.message.command
                     == BLE_COMMAND_INDICATION_TIMEOUT) {
            // This indication was not successful, go to next
            ble_state = BLE_STATE_INDICATE;
            ble_message_state.message.command = BLE_COMMAND_INDICATION;
            ble_queue_post(ble_message_state);
          }
        }
        break;
      default:
        break;
    }
  }
}

/**************************************************************************//**
 * Enabling Bluetooth advertistments
 *
 *****************************************************************************/
static void enable_ble_advertisements(void)
{
  sl_status_t sc;
  // Create the device name based on the 16-bit device ID.
  uint16_t dev_id;
  static char dev_name[DEVNAME_LEN];
  char adv_name[13] = { 0 };

  if (demo.conn_bluetooth_in_use == 0) {
    dev_id = *((uint16_t *)demo.own_addr.addr);
    // Copy to the local GATT database - this will be used by the BLE stack
    // to put the local device name into the advertisements, but only if we are
    // using default advertisements
    snprintf(dev_name, DEVNAME_LEN, DEVNAME, dev_id >> 8, dev_id & 0xff);

    strcat(adv_name, dev_name);
    strcat(adv_name, " Light");

    sl_bt_gatt_server_write_attribute_value(gattdb_device_name,
                                            0,
                                            strlen(adv_name),
                                            (uint8_t *)adv_name);

    // Copy the shortened device name to the response data, overwriting
    // the default device name which is set at compile time
    memcpy(((uint8_t *)&response_data) + 5, dev_name, 8);

    // Delete previous advertistment
    sl_bt_advertiser_delete_set(advertising_set_handle);
    advertising_set_handle = 0xFF;

    // Create an advertising set
    sc = sl_bt_advertiser_create_set(&advertising_set_handle);
    app_assert_status(sc);

    // Set the response data
    sc = sl_bt_legacy_advertiser_set_data(advertising_set_handle,
                                          0,
                                          sizeof(response_data),
                                          (uint8_t *)&response_data);
    app_assert_status(sc);

    // Set nominal 100ms advertising interval, so we just get
    // a single beacon of each type
    sc = sl_bt_advertiser_set_timing(
      advertising_set_handle, // advertising set handle
      160, // min. adv. interval (milliseconds * 1.6)
      160, // max. adv. interval (milliseconds * 1.6)
      0,   // adv. duration
      0);  // max. num. adv. events
    app_assert_status(sc);

    sl_bt_advertiser_set_report_scan_request(advertising_set_handle, 1);
  }

  // Start advertising in user mode and enable connections, if we are
  //  not already connected
  if (demo.conn_bluetooth_in_use >= SL_BT_CONFIG_MAX_CONNECTIONS) {
    // Start general advertising and disable connections.
    sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                       sl_bt_advertiser_non_connectable); // non-connectable
    app_assert_status(sc);
  } else {
    // Start general advertising and enable connections.
    sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                       sl_bt_advertiser_connectable_scannable); // connectable

    app_assert_status(sc);
  }
}

/**************************************************************************//**
 * Demo task.
 * @param[in] p_arg the argument passed by 'OSTaskCreate()'.
 *****************************************************************************/
static void demo_app_task(void *p_arg)
{
  (void)p_arg;
  demo_msg_t demo_msg;
  BaseType_t ret;
  bool bluetooth_ind = false;
  char *demo_mode_str_adv = "ADVERT";
  char *demo_mode_str_rep = "READY";
  char dev_id_str[DEV_ID_STR_LEN];
  snprintf(dev_id_str,
           DEV_ID_STR_LEN,
           "DMP:%04X",
           *((uint16_t *)demo.own_addr.addr));

  // create timer for periodic indications
  demo_timer = xTimerCreate("Demo Timer",
                            DEMO_TIMER_TICKS,
                            pdTRUE,
                            NULL,
                            demo_timer_cb);

  app_assert(NULL != demo_timer, "OS Timer error" APP_LOG_NEW_LINE);

  // create one-shot timer for direction array
  demo_timer_direction = xTimerCreate("Demo Timer Direction",
                                      DEMO_TIMER_DIRECTION_TICKS,
                                      pdFALSE,
                                      NULL,
                                      demo_timer_direction_cb);

  app_assert(NULL != demo_timer_direction, "OS Timer error" APP_LOG_NEW_LINE);

  demo.state = DEMO_STATE_INIT;

  demoUIInit();
  demoUIClearMainScreen((uint8_t *)"Light", true, true);
  demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_adv);
  demoUIDisplayId(DEMO_UI_PROTOCOL2, (uint8_t *)dev_id_str);

  while (1) {
    // pending on demo message queue
    demo_msg = demo_queue_pend();
    switch (demo.state) {
      case DEMO_STATE_INIT:
        switch (demo_msg) {
          // bluetooth booted
          case DEMO_EVT_BOOTED:
            // initialise demo variables:
            // make it look like the last trigger source of the light
            // was a button press
            light_pend();
            demo.conn_bluetooth_in_use = 0;
            demo.conn_proprietary_in_use = 0;
            demo.indication_ongoing = false;
            demo.direction = demo_light_direction_button;
            memcpy((void*)demo.src_addr.addr,
                   (void*)demo.own_addr.addr,
                   sizeof(demo.src_addr.addr));
            light_post();
            // Enable advertisements for the first time
            enable_ble_advertisements();
            // bluetooth client connected or proprietary link is used
            // start demo timer
            if (demo.conn_bluetooth_in_use || demo.conn_proprietary_in_use) {
              ret = xTimerStart(demo_timer, 0);
              app_assert(pdPASS == ret, "OS Timer error" APP_LOG_NEW_LINE);
            }
            demo.state = DEMO_STATE_READY;
            break;

          default:
            break;
        }
        break;

      case DEMO_STATE_READY:
        switch (demo_msg) {
          case DEMO_EVT_BLUETOOTH_CONNECTED:
            demo.indication_ongoing = false;

            if (demo.conn_bluetooth_in_use < SL_BT_CONFIG_MAX_CONNECTIONS ) {
              demo.conn_bluetooth_in_use++;
              enable_ble_advertisements();

              // Bluetooth connected: start periodic indications if
              // not already ongoing
              if (pdTRUE != xTimerIsTimerActive(demo_timer)) {
                ret = xTimerStart(demo_timer, 0);
                app_assert(pdPASS == ret, "OS Timer error" APP_LOG_NEW_LINE);
              }
              demoUIDisplayProtocol(DEMO_UI_PROTOCOL2, true);
            }
            break;

          case DEMO_EVT_BLUETOOTH_DISCONNECTED:
            if (demo.conn_bluetooth_in_use) {
              demo.conn_bluetooth_in_use--;
            }
            // If this was the last connection
            if ( !demo.conn_bluetooth_in_use ) {
              // Flush pending and ongoing notifications
              demo.indication_pending = false;
              demo.indication_ongoing = false;

              demoUIClearDirection(DEMO_UI_DIRECTION_PROT2);
              demoUIDisplayProtocol(DEMO_UI_PROTOCOL2, false);

              if (pdTRUE == xTimerIsTimerActive(demo_timer)) {
                // No Bluetooth nor RAIL client connected; stop periodic
                // indications
                if ( (demo.conn_bluetooth_in_use == 0)
                     && (demo.conn_proprietary_in_use == 0) ) {
                  ret = xTimerStop(demo_timer, 0);
                  app_assert(pdPASS == ret, "OS Timer error" APP_LOG_NEW_LINE);
                }
              }
            }

            // restart connectable advertising
            enable_ble_advertisements();

            break;

          case DEMO_EVT_RAIL_READY:
            demo.conn_proprietary_in_use = 1;
            // RAIL linked: start periodic indications if not already ongoing
            if (pdTRUE != xTimerIsTimerActive(demo_timer)) {
              ret = xTimerStart(demo_timer, 0);
              app_assert(pdPASS == ret, "OS Timer error" APP_LOG_NEW_LINE);
            }
            demoUIClearMainScreen((uint8_t *)"Light", true, true);
            demoUIClearDirection(DEMO_UI_DIRECTION_PROT1);
            demoUIDisplayLight(demo.light);
            demoUIDisplayProtocol(DEMO_UI_PROTOCOL1, true);
            demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_rep);
            demoUIDisplayId(DEMO_UI_PROTOCOL2, (uint8_t *)dev_id_str);
            break;

          case DEMO_EVT_RAIL_ADVERTISE:
            demo.conn_proprietary_in_use = 0;
            // No Bluetooth nor RAIL client connected; stop periodic indications
            if (!demo.conn_bluetooth_in_use && !demo.conn_proprietary_in_use) {
              ret = xTimerStop(demo_timer, 0);
              app_assert(pdPASS == ret, "OS Timer error" APP_LOG_NEW_LINE);
            }

            demoUIClearMainScreen((uint8_t*)"Light", true, true);
            demoUIClearDirection(DEMO_UI_DIRECTION_PROT1);
            demoUIDisplayLight(demo.light);
            demoUIDisplayProtocol(DEMO_UI_PROTOCOL1, false);
            demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_adv);
            demoUIDisplayId(DEMO_UI_PROTOCOL2, (uint8_t *)dev_id_str);
            break;

          case DEMO_EVT_LIGHT_CHANGED_BLUETOOTH:

            if (demo_light_off == demo.light) {
              appUiLedOff();
            } else {
              appUiLedOn();
            }

            demoUIDisplayLight(demo.light);

            if (demo.conn_proprietary_in_use) {
              demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_rep);
            } else {
              demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_adv);
            }

            demoUIDisplayId(DEMO_UI_PROTOCOL2, (uint8_t *)dev_id_str);
            demoUIDisplayDirection(DEMO_UI_DIRECTION_PROT2);

            ret = xTimerStart(demo_timer_direction, 0);
            app_assert(pdPASS == ret, "OS Timer error" APP_LOG_NEW_LINE);

            demo_queue_post(DEMO_EVT_INDICATION);
            break;

          case DEMO_EVT_LIGHT_CHANGED_RAIL:
            light_pend();
            demo.direction = demo_light_direction_proprietary;

            if (demo_light_off == demo.light) {
              appUiLedOff();
            } else {
              appUiLedOn();
            }

            light_post();
            demoUIDisplayLight(demo.light);
            if (demo.conn_proprietary_in_use) {
              demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_rep);
            } else {
              demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_adv);
            }
            demoUIDisplayId(DEMO_UI_PROTOCOL2, (uint8_t *)dev_id_str);
            demoUIDisplayDirection(DEMO_UI_DIRECTION_PROT1);

            ret = xTimerStart(demo_timer_direction, 0);
            app_assert(pdPASS == ret, "OS Timer error" APP_LOG_NEW_LINE);

            demo_queue_post(DEMO_EVT_INDICATION);
            break;

          case DEMO_EVT_BUTTON0_PRESSED:
            light_pend();

            if (demo_light_off == demo.light) {
              demo.light = demo_light_on;
              appUiLedOn();
            } else {
              demo.light = demo_light_off;
              appUiLedOff();
            }
            demo.direction = demo_light_direction_button;
            memcpy((void *)demo.src_addr.addr,
                   (void *)demo.own_addr.addr,
                   sizeof(demo.src_addr.addr));
            light_post();
            demoUIDisplayLight(demo.light);
            if (demo.conn_proprietary_in_use) {
              demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_rep);
            } else {
              demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_adv);
            }
            demoUIDisplayId(DEMO_UI_PROTOCOL2, (uint8_t *)dev_id_str);
            demo_queue_post(DEMO_EVT_INDICATION);
            break;

          case DEMO_EVT_BUTTON1_PRESSED:
            proprietary_queue_post(PROP_TOGGLE_MODE);
            break;

          case DEMO_EVT_CLEAR_DIRECTION:
            if (demo_light_direction_proprietary == demo.direction) {
              demoUIClearDirection(DEMO_UI_DIRECTION_PROT1);
            } else {
              demoUIClearDirection(DEMO_UI_DIRECTION_PROT2);
            }
            break;

          case DEMO_EVT_INDICATION:
            light_pend();
            bluetooth_ind = demo.conn_bluetooth_in_use;

            // no ongoing indication, free to start sending one out
            if (!demo.indication_ongoing) {
              if (bluetooth_ind || demo.conn_proprietary_in_use) {
                demo.indication_ongoing = true;
              }
              // send indication on BLE side
              if (bluetooth_ind) {
                ble_msg_t ble_message_state;
                ble_message_state.message.command = BLE_COMMAND_INDICATION;
                ble_queue_post(ble_message_state);
              }
              // send indication on proprietary side
              // there is no protocol on proprietary side; BLE side transmission
              // is the slower
              // send out proprietary packets at maximum the same rate as
              // BLE indications
              if (demo.conn_proprietary_in_use) {
                proprietary_queue_post(PROP_STATUS_SEND);
                if (!bluetooth_ind) {
                  demo_queue_post(DEMO_EVT_INDICATION_SUCCESSFUL);
                }
              }
            } else {
              // ongoing indication; set pending flag
              demo.indication_pending = true;
            }
            light_post();
            break;
          case DEMO_EVT_INDICATION_FAILED:
            demo.indication_ongoing = false;
            if (demo.indication_pending) {
              demo.indication_pending = false;
              demo_queue_post(DEMO_EVT_INDICATION);
            }
            break;
          case DEMO_EVT_INDICATION_SUCCESSFUL:
            demo.indication_ongoing = false;
            if (demo.indication_pending) {
              demo.indication_pending = false;
              demo_queue_post(DEMO_EVT_INDICATION);
            }
            break;
          default:
            break;
        }
        break;

      // error
      default:
        break;
    }
  }
}
