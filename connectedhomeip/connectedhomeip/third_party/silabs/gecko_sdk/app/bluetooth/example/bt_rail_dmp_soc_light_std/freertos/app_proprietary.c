/***************************************************************************//**
 * @file
 * @brief Application logic for DMP with standard protocols.
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
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include "rail.h"
#include "app_assert.h"
#include "app_bluetooth.h"
#include "em_system.h"
#include "sl_component_catalog.h"
#include "sl_flex_packet_asm.h"
#include "app_proprietary.h"
#include "sl_simple_led_instances.h"
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
#include "sl_flex_util_802154_protocol_types.h"
#include "sl_flex_util_802154_init_config.h"
#include "sl_flex_rail_ieee802154_config.h"
#include "sl_flex_ieee802154_support.h"
#include "sl_flex_util_802154_init.h"
#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
#include "sl_flex_util_ble_protocol_config.h"
#include "sl_flex_util_ble_init_config.h"
#include "sl_flex_util_ble_init.h"
#else
#endif
// -----------------------------------------------------------------------------
// Constant definitions and macros
#ifdef SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
/// BLE channel number
#define BLE_CHANNEL ((uint8_t) 0)
#endif
// Proprietary Application task
#define PROPRIETARY_APP_TASK_NAME         "Proprietary App Task"
#define PROPRIETARY_APP_TASK_STACK_SIZE   250
#define PROPRIETARY_APP_TASK_PRIO         6u

#define PROPRIETARY_QUEUE_SIZE  32

#define PACKET_HEADER_LEN (2)
#define PROP_RX_BUF_SIZE (256)
#define LIGHT_CONTROL_DATA_BYTE (0x0F)

#define DEMO_CONTROL_PAYLOAD_CMD_DATA                  (0x0F)
#define DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT              (0x80)
#define DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT_SHIFT        (7)
#define DEMO_CONTROL_PAYLOAD_CMD_MASK                  (0x70)
#define DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT            (4)

// -----------------------------------------------------------------------------
// Data type definitions

typedef enum {
  DEMO_CONTROL_CMD_ADVERTISE = 0,
  DEMO_CONTROL_CMD_LIGHT_TOGGLE = 1,
  DEMO_CONTROL_CMD_LIGHT_STATE_REPORT = 2,
  DEMO_CONTROL_CMD_LIGHT_STATE_GET = 3,
} demo_control_command_t;

typedef enum {
  DEMO_CONTROL_ROLE_LIGHT = 0,
  DEMO_CONTROL_ROLE_SWITCH = 1,
} demo_control_role_t;

typedef enum {
  PROP_STATE_ADVERTISE              = 0x00,
  PROP_STATE_READY                  = 0x01
} prop_state_t;

typedef struct {
  prop_state_t state;
} proprietary_t;

proprietary_t proprietary = {
  .state = PROP_STATE_ADVERTISE
};

typedef enum {
  PROP_PKT_ADVERTISE                = 0x00,
  PROP_PKT_STATUS                   = 0x01,
} prop_pkt;

// -----------------------------------------------------------------------------
// Private variables
StackType_t proprietary_app_task_stack[PROPRIETARY_APP_TASK_STACK_SIZE] = { 0 };
StaticTask_t proprietary_app_task_buffer;
TaskHandle_t proprietary_app_task_handle;

static TimerHandle_t  proprietary_timer;

static QueueHandle_t  proprietary_queue;
static prop_msg_t proprietary_queue_storage[PROPRIETARY_QUEUE_SIZE];
static StaticQueue_t proprietary_queue_buffer;

/// Transmit FIFO
static __ALIGNED(RAIL_FIFO_ALIGNMENT) uint8_t tx_payload[TX_PAYLOAD_LENGTH] = {
  0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
  0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xFF, 0x00
};

static __ALIGNED(RAIL_FIFO_ALIGNMENT) uint8_t tx_fifo[RAIL_FIFO_SIZE];

/// Transmit app buffer
static uint8_t tx_frame_buff[RAIL_FIFO_SIZE];

static volatile bool re_start_rx = false;

/// Receive FIFO
static __ALIGNED(RAIL_FIFO_ALIGNMENT) uint8_t rx_fifo[RAIL_FIFO_SIZE];

static uint8_t standard_rx_buf[PROP_RX_BUF_SIZE];

/// Contains the last RAIL Rx/Tx error events
static volatile uint64_t current_rail_err = 0;
/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = 0;

/// Notify reception of packet
static volatile bool rail_packet_received = false;

/// Busy flag for RAIL init
static volatile bool app_rail_busy = true;

#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
/// Status of the application
sl_flex_ieee802154_status_t comm_status = { 0 };

/// IEEE 802.15.4 frame structure for RX direction
sl_flex_ieee802154_frame_t rx_frame = { 0 };
/// IEEE 802.15.4 frame structure for TX direction
sl_flex_ieee802154_frame_t tx_frame = {
  .mhr_config = {
    .frame_control          = DEFAULT_FCF_FIELD,
    .sequence_number        = 0u,
    .destination_pan_id     = DEFAULT_BROADCAST_PAN_ID,
    .destination_address    = DEFAULT_BROADCAST_DEST_ADDR,
    .source_address         = DEFAULT_BROADCAST_SRC_ADDR
  },
  .phr_config = SL_FLEX_IEEE802154G_PHR_MODE_SWITCH_OFF
                | SL_FLEX_IEEE802154G_PHR_CRC_2_BYTE
                | APP_WHITENING_ON_OFF,
};
#endif

// -----------------------------------------------------------------------------
// Private function definitions
static void proprietary_app_task(void *p_arg);
static void proprietary_timer_callback(TimerHandle_t xTimer);
static prop_msg_t proprietary_queue_pend(void);

/*******************************************************************************
 * Handle received packets in loop
 ******************************************************************************/
static void handle_receive(RAIL_Handle_t rail_handle)
{
  RAIL_RxPacketHandle_t rx_packet_handle;
  RAIL_RxPacketDetails_t packet_details;
  RAIL_RxPacketInfo_t packet_info;
  RAIL_Status_t rail_status = RAIL_STATUS_NO_ERROR;
  uint8_t rx_tmp_buff[RAIL_FIFO_SIZE];

  app_log_info("RX event triggered handle", rail_status);
  app_log_nl();

  rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
  while (rx_packet_handle != RAIL_RX_PACKET_HANDLE_INVALID) {
    (void)RAIL_GetRxPacketDetails(rail_handle, rx_packet_handle, &packet_details);
    // copies the data in the RX Buffer
    RAIL_CopyRxPacket(rx_tmp_buff, &packet_info);
    // after the copy of the packet, the RX packet can be release for RAIL
    rail_status = RAIL_ReleaseRxPacket(rail_handle, rx_packet_handle);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_warning("RAIL_ReleaseRxPacket() result:%d", rail_status);
      app_log_nl();
    }

    // Read packet data into our packet structure
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
    // Is it a ACK or a data frame
    if (packet_details.isAck) {
      app_log_info("ACK received");
      app_log_nl();
    } else {
      int16_t res = SL_FLEX_IEEE802154_ERROR;
      // unpack the IEEE802154 frame
      res = sl_flex_ieee802154_unpack_data_frame(sl_flex_ieee802154_get_std(),
                                                 &rx_frame,
                                                 rx_tmp_buff,
                                                 RAIL_FIFO_SIZE);
      // prints the received data based on the standard
      if (res != SL_FLEX_IEEE802154_OK) {
        app_log_error("ieee802154 unpack failed with %d", res);
        app_log_nl();
      } else {
        memcpy(standard_rx_buf, rx_frame.payload, TX_PAYLOAD_LENGTH);
      }
    }
#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
    {
      sl_flex_ble_advertising_packet_t *ble_recv_packet = NULL;
      ble_recv_packet = sl_flex_ble_get_packet(rx_tmp_buff);
      if (ble_recv_packet == NULL) {
        app_log_error("BLE received packet is NULL pointer");
        app_log_nl();
      } else {
        memcpy(standard_rx_buf, sl_flex_ble_get_payload(ble_recv_packet), TX_PAYLOAD_LENGTH);
      }
    }
#else
#endif
    rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
  }
}

// -----------------------------------------------------------------------------
// Public functions
/******************************************************************************
 * Initialization of RAIL TX FIFO
 *****************************************************************************/
int16_t app_set_rail_tx_fifo(RAIL_Handle_t rail_handle)
{
  // RAIL FIFO size allocated by RAIL_SetTxFifo() call
  uint16_t allocated_tx_fifo_size = 0;

  allocated_tx_fifo_size = RAIL_SetTxFifo(rail_handle, tx_fifo,
                                          0u, RAIL_FIFO_SIZE);
  app_assert(allocated_tx_fifo_size == RAIL_FIFO_SIZE,
             "RAIL_SetTxFifo() failed to allocate a large enough fifo"
             " (%d bytes instead of %d bytes%s)",
             allocated_tx_fifo_size, RAIL_FIFO_SIZE,
             APP_LOG_NEW_LINE);
  if (allocated_tx_fifo_size != RAIL_FIFO_SIZE) {
    return -1;
  }

  return 0;
}
/**************************************************************************//**
 * Init proprietary application
 *
 *****************************************************************************/
void init_prop_app(void)
{
  RAIL_Status_t status = RAIL_STATUS_NO_ERROR;
  // Get RAIL handle, used later during the initialization
  RAIL_Handle_t rail_handle = sl_flex_util_get_handle();

  proprietary_timer = xTimerCreate("Proprietary Timer",
                                   10,
                                   pdTRUE,
                                   NULL,
                                   proprietary_timer_callback);

  proprietary_queue = xQueueCreateStatic(PROPRIETARY_QUEUE_SIZE,
                                         sizeof(prop_msg_t),
                                         (uint8_t *)proprietary_queue_storage,
                                         &proprietary_queue_buffer);

  // Create the Proprietary Application task
  proprietary_app_task_handle = xTaskCreateStatic(proprietary_app_task,
                                                  PROPRIETARY_APP_TASK_NAME,
                                                  PROPRIETARY_APP_TASK_STACK_SIZE,
                                                  NULL,
                                                  PROPRIETARY_APP_TASK_PRIO,
                                                  proprietary_app_task_stack,
                                                  &proprietary_app_task_buffer);

  // Busy wait for RAIL init - an event callback from a RAIL ISR will update the flag
  while (app_rail_busy) {
  }

  // sets the RAIL TX FIFO
  (void)app_set_rail_tx_fifo(rail_handle);

#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  // init the selected protocol for IEEE
  sl_flex_ieee802154_protocol_init(rail_handle, SL_FLEX_UTIL_INIT_PROTOCOL_INSTANCE_DEFAULT);
  // updates the status for changes
  sl_flex_ieee802154_update_status(&comm_status, &tx_frame);
#endif

  // Start reception
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  status = RAIL_StartRx(rail_handle, sl_flex_ieee802154_get_channel(), NULL);
#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  status = RAIL_StartRx(rail_handle, BLE_CHANNEL, NULL);
#else
#endif
  if (status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("After initialization RAIL_StartRx() result:%d", status);
    app_log_nl();
  }
}

/**************************************************************************//**
 * Proprietary post queue.
 *
 * @param msg message type
 * @param err error message
 *
 * Post error message
 *****************************************************************************/
void proprietary_queue_post(prop_msg_t msg)
{
  if ( pdTRUE != xQueueSend(proprietary_queue, (void *)&msg, portMAX_DELAY)) {
    app_assert(false, "OS error proprietary_queue_post" APP_LOG_NEW_LINE);
  }
}

static prop_msg_t proprietary_queue_pend(void)
{
  prop_msg_t propMsg;

  if ( pdTRUE != xQueueReceive(proprietary_queue,
                               (void *)&propMsg,
                               portMAX_DELAY)) {
    app_assert(false, "OS error proprietary_queue_pend" APP_LOG_NEW_LINE);
  }

  return propMsg;
}

// -----------------------------------------------------------------------------
// Private functions

/**************************************************************************//**
 * Proprietary timer callback.
 *
 * @param p_tmr is pointer to the user-allocated timer.
 * @param p_arg is argument passed when creating the timer.
 *
 * Called when timer expires
 *****************************************************************************/
static void proprietary_timer_callback(TimerHandle_t xTimer)
{
  (void)xTimer;
  proprietary_queue_post(PROP_TIMER_EXPIRED);
}

/**************************************************************************//**
 * Proprietary packet send.
 *
 * @param pktType Packet type
 *
 * Sends a packet using standard protocol. The data shall be in tx_payload
 *****************************************************************************/
static void standardTxPacket(prop_pkt pktType)
{
  uint16_t packet_size;
  RAIL_SchedulerInfo_t schedulerInfo;
  RAIL_Status_t rail_status;
  RAIL_Handle_t rail_handle = sl_flex_util_get_handle();

  // This assumes the Tx time is around 200us
  schedulerInfo = (RAIL_SchedulerInfo_t){ .priority = 100,
                                          .slipTime = 100000,
                                          .transactionTime = 200 };

  // address of light
  memcpy((void *)&tx_payload[PACKET_HEADER_LEN],
         (void *)demo.own_addr.addr,
         sizeof(demo.own_addr.addr));
  // light role
  tx_payload[LIGHT_CONTROL_DATA_BYTE] &= ~DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT;
  tx_payload[LIGHT_CONTROL_DATA_BYTE] |=
    (DEMO_CONTROL_ROLE_LIGHT << DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT_SHIFT)
    & DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT;
  // advertisement packet
  if (PROP_PKT_ADVERTISE == pktType) {
    tx_payload[LIGHT_CONTROL_DATA_BYTE] &= ~DEMO_CONTROL_PAYLOAD_CMD_MASK;
    tx_payload[LIGHT_CONTROL_DATA_BYTE] |=
      ((uint8_t)DEMO_CONTROL_CMD_ADVERTISE
        << DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT)
      & DEMO_CONTROL_PAYLOAD_CMD_MASK;
    // status packet
  } else if (PROP_PKT_STATUS == pktType) {
    tx_payload[LIGHT_CONTROL_DATA_BYTE] &= ~DEMO_CONTROL_PAYLOAD_CMD_MASK;
    tx_payload[LIGHT_CONTROL_DATA_BYTE] |=
      ((uint8_t)DEMO_CONTROL_CMD_LIGHT_STATE_REPORT
        << DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT)
      & DEMO_CONTROL_PAYLOAD_CMD_MASK;
    tx_payload[LIGHT_CONTROL_DATA_BYTE] &= ~0x01;
    tx_payload[LIGHT_CONTROL_DATA_BYTE] |= (uint8_t)demo.light;
  } else {
  }

#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  // Prepare packet
  sl_flex_ieee802154_prepare_sending(&tx_frame,
                                     tx_payload,
                                     sizeof(tx_payload));
  // packs the data frame using the parameter information and the packed
  // frame is copied into the fifo
  (void)sl_flex_ieee802154_pack_data_frame(sl_flex_ieee802154_get_std(),
                                           &tx_frame,
                                           &packet_size,
                                           tx_frame_buff);

  // Send packet
  // sets the tx options based on the current ACK settings (auto-ACK enabled?)
  rail_status = sl_flex_ieee802154_transmission(rail_handle, tx_frame_buff, packet_size);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("sl_flex_ieee802154_transmission() status: %d", rail_status);
    app_log_nl();
  }
#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  sl_flex_ble_advertising_packet_t *ble_send_packet;
  // Prepare packet
  // set ble_send_packet pointer to tx app buff
  ble_send_packet = sl_flex_ble_get_packet(tx_frame_buff);
  sl_flex_ble_prepare_packet(ble_send_packet, tx_payload, sizeof(tx_payload));
  packet_size = sl_flex_ble_get_packet_size(ble_send_packet);
  // Send Packet
  rail_status = RAIL_WriteTxFifo(rail_handle, tx_frame_buff, packet_size, true);
  if (rail_status != packet_size) {
    app_log_warning("BLE RAIL_WriteTxFifo status: %d", rail_status);
    app_log_nl();
  }
  rail_status = RAIL_StartTx(rail_handle, BLE_CHANNEL, RAIL_TX_OPTIONS_DEFAULT, NULL);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("BLE RAIL_StartTx status: %d", rail_status);
    app_log_nl();
  }
#endif

  if (rail_status != RAIL_STATUS_NO_ERROR) {
    // Try once to resend the packet 100ms later in case of error
    RAIL_ScheduleTxConfig_t scheduledTxConfig =
    { .when = RAIL_GetTime() + 100000,
      .mode = RAIL_TIME_ABSOLUTE };

    // Transmit this packet at the specified time or up to 50 ms late
    rail_status = RAIL_StartScheduledTx((RAIL_Handle_t)sl_flex_util_get_handle(),
                                        0,
                                        RAIL_TX_OPTIONS_DEFAULT,
                                        &scheduledTxConfig,
                                        &schedulerInfo);
  }
}

/**************************************************************************//**
 * Proprietary Application task.
 *
 * @param p_arg Pointer to an optional data area which can pass parameters to
 *              the task when the task executes.
 *
 * This is a minimal Proprietary Application task that only configures the
 * radio.
 *****************************************************************************/
static void proprietary_app_task(void *p_arg)
{
  (void)p_arg;
  prop_msg_t propMsg;
  RAIL_Status_t rail_status;

  RAIL_Handle_t rail_handle = sl_flex_util_get_handle();

  // Start reception
  RAIL_StartRx((RAIL_Handle_t)rail_handle, 0, NULL);

  proprietary.state = PROP_STATE_ADVERTISE;
  if ( pdPASS != xTimerStart(proprietary_timer, 0)) {
    app_assert(false, "OS Timer error" APP_LOG_NEW_LINE);
  }

  while (1) {
    // pending on proprietary message queue
    propMsg = proprietary_queue_pend();
    if (re_start_rx) {
      re_start_rx = false;
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
      rail_status = RAIL_StartRx(rail_handle, sl_flex_ieee802154_get_channel(), NULL);
#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
      rail_status = RAIL_StartRx(rail_handle, BLE_CHANNEL, NULL);
#else
#endif

      if (rail_status != RAIL_STATUS_NO_ERROR) {
        app_log_warning("RAIL_StartRx() result:%d",
                        rail_status);
        app_log_nl();
      }
    }

    if (rail_packet_received) {
      rail_packet_received = false;
      handle_receive(rail_handle);
    }

    switch (proprietary.state) {
      case PROP_STATE_ADVERTISE:
        switch (propMsg) {
          case PROP_TIMER_EXPIRED:
            standardTxPacket(PROP_PKT_ADVERTISE);
            break;

          case PROP_TOGGLE_MODE:
            if ( pdPASS != xTimerStop(proprietary_timer, 0)) {
              app_assert(false, "OS Timer error" APP_LOG_NEW_LINE);
            }
            demo_queue_post(DEMO_EVT_RAIL_READY);
            proprietary.state = PROP_STATE_READY;
            break;

          default:
            break;
        }
        break;

      case PROP_STATE_READY:
        switch (propMsg) {
          case PROP_STATUS_SEND:
            standardTxPacket(PROP_PKT_STATUS);
            break;

          case PROP_TOGGLE_MODE:
            if ( pdPASS != xTimerStart(proprietary_timer, 0)) {
              app_assert(false, "OS Timer error" APP_LOG_NEW_LINE);
            }
            demo_queue_post(DEMO_EVT_RAIL_ADVERTISE);
            proprietary.state = PROP_STATE_ADVERTISE;
            break;

          case PROP_TOGGLE_RXD:
            light_pend();
            if (demo.light == demo_light_off) {
              demo.light = demo_light_on;
            } else {
              demo.light = demo_light_off;
            }
            light_post();
            demo_queue_post(DEMO_EVT_LIGHT_CHANGED_RAIL);
            break;

          case PROP_PROCESS_RXD:
            // process packets in ready mode only
          {
            demo_control_role_t role;
            demo_control_command_t command;

            // packet sent by switch
            role = (demo_control_role_t)(
              (standard_rx_buf[DEMO_CONTROL_PAYLOAD_CMD_DATA]
               & DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT)
              >> DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT_SHIFT);
            if (DEMO_CONTROL_ROLE_SWITCH == role) {
              // Handle only those packets that include the Light's address
              if (0 == memcmp((void *)demo.own_addr.addr,
                              (void *)&standard_rx_buf[PACKET_HEADER_LEN],
                              sizeof(demo.own_addr.addr))) {
                // packet contains toggle command
                command = (demo_control_command_t)(
                  (standard_rx_buf[DEMO_CONTROL_PAYLOAD_CMD_DATA]
                   & DEMO_CONTROL_PAYLOAD_CMD_MASK)
                  >> DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT);
                if (DEMO_CONTROL_CMD_LIGHT_TOGGLE == command) {
                  proprietary_queue_post(PROP_TOGGLE_RXD);
                }
              }
            } else {
              // Add anything app specific when something else is received
            }
            break;
          }

          default:
            break;
        }
        break;
    }
  }
}

/******************************************************************************
 * RAIL callback, called after the RAIL's initialization finished.
 *****************************************************************************/
void sl_rail_util_on_rf_ready(RAIL_Handle_t rail_handle)
{
  (void) rail_handle;
  app_rail_busy = false;
}

/******************************************************************************
 * RAIL callback, called while the RAIL is initializing.
 *****************************************************************************/
RAIL_Status_t RAILCb_SetupRxFifo(RAIL_Handle_t railHandle)
{
  uint16_t rxFifoSize = RAIL_FIFO_SIZE;
  RAIL_Status_t status = RAIL_SetRxFifo(railHandle, rx_fifo, &rxFifoSize);
  if (rxFifoSize != RAIL_FIFO_SIZE) {
    // We set up an incorrect FIFO size
    return RAIL_STATUS_INVALID_PARAMETER;
  }
  if (status == RAIL_STATUS_INVALID_STATE) {
    // Allow failures due to multiprotocol
    return RAIL_STATUS_NO_ERROR;
  }
  return status;
}

/**************************************************************************//**
 * RAIL callback, called if a RAIL event occurs.
 *
 * @param[in] rail_handle RAIL handle
 * @param[in] events RAIL events
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  //----------------- RX --------------------------
  // Handle Rx events
  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      // Keep the packet in the radio buffer,
      // download it later at the state machine
      (void)RAIL_HoldRxPacket(rail_handle);
      rail_packet_received = true;
      proprietary_queue_post(PROP_PROCESS_RXD);
    }
  }

  //----------------- TX --------------------------
  // Handle Tx events
  if ( events & RAIL_EVENTS_TX_COMPLETION) {
    if (!(events & RAIL_EVENT_TX_PACKET_SENT)) {
      // nothing to do for these events - handle errors could take place, though
    }

    RAIL_YieldRadio(rail_handle);
  }

  re_start_rx = true;
  // Perform all calibrations when needed
  if ( events & RAIL_EVENT_CAL_NEEDED ) {
    calibration_status = RAIL_Calibrate(rail_handle, NULL, RAIL_CAL_ALL_PENDING);
    if (calibration_status != RAIL_STATUS_NO_ERROR) {
      current_rail_err = (events & RAIL_EVENT_CAL_NEEDED);
    }
  }
}
