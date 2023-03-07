/***************************************************************************//**
 * @file
 * @brief Application logic for DMP with proprietary protocols.
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
#include "os.h"
#include "rail.h"
#include "sl_rail_util_init.h"
#include "app_assert.h"
#include "app_bluetooth.h"
#include "app_proprietary.h"
#include "rtos_err.h"

// -----------------------------------------------------------------------------
// Constant definitions and macros

// Proprietary Application task
#define PROPRIETARY_APP_TASK_PRIO         6u
#define PROPRIETARY_APP_TASK_STACK_SIZE   (2000 / sizeof(CPU_STK))
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

static CPU_STK proprietary_app_task_stk[PROPRIETARY_APP_TASK_STACK_SIZE];
static OS_TCB  proprietary_app_task_tcb;
static void    proprietary_app_task(void *p_arg);
static OS_TMR  proprietary_timer;
static OS_Q    proprietary_queue;

static uint8_t data_packet[] =
{
  0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
  0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0x00
};

/// Receive FIFO
static __ALIGNED(RAIL_FIFO_ALIGNMENT) uint8_t rx_fifo[RAIL_FIFO_SIZE];

static uint8_t proprietary_rx_buf[PROP_RX_BUF_SIZE];

/// Contains the last RAIL Rx/Tx error events
static volatile uint64_t current_rail_err = 0;
/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = 0;

/// Notify reception of packet
static volatile bool rail_packet_received = false;
// -----------------------------------------------------------------------------
// Private function definitions

static void proprietary_timer_callback(void *p_tmr, void *p_arg);
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
    uint16_t length = packet_info.packetBytes;
    Mem_Copy(proprietary_rx_buf,
             packet_info.firstPortionData,
             packet_info.firstPortionBytes);
    Mem_Copy(proprietary_rx_buf + packet_info.firstPortionBytes,
             packet_info.lastPortionData,
             length - packet_info.firstPortionBytes);

    rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
  }
}

// -----------------------------------------------------------------------------
// Public functions

/**************************************************************************//**
 * Init proprietary application
 *
 *****************************************************************************/
void init_prop_app(void)
{
  RTOS_ERR err;

  OSTmrCreate(&proprietary_timer,           // Pointer to user-allocated timer.
              "Proprietary Timer",          // Name used for debugging.
              0,                            // 0 initial delay.
              10,                           // 100 Timer Ticks period.
              OS_OPT_TMR_PERIODIC,          // Timer is periodic.
              &proprietary_timer_callback,  // Called when timer expires.
              DEF_NULL,                     // No arguments to callback.
              &err);

  OSQCreate((OS_Q     *)&proprietary_queue,
            (CPU_CHAR *)"Proprietary Queue",
            (OS_MSG_QTY) 32,
            (RTOS_ERR *)&err);

  // Create the Proprietary Application task
  OSTaskCreate(&proprietary_app_task_tcb,
               "Proprietary App Task",
               proprietary_app_task,
               0u,
               PROPRIETARY_APP_TASK_PRIO,
               &proprietary_app_task_stk[0u],
               (PROPRIETARY_APP_TASK_STACK_SIZE / 10u),
               PROPRIETARY_APP_TASK_STACK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &err);
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
  RTOS_ERR err;

  OSQPost((OS_Q *)&proprietary_queue,
          (void *) msg,
          (OS_MSG_SIZE) sizeof(void*),
          (OS_OPT) OS_OPT_POST_FIFO + OS_OPT_POST_ALL,
          (RTOS_ERR *) &err);

  app_assert(err.Code == RTOS_ERR_NONE,
             "[E: 0x%04x] OS error proprietary_queue_post" APP_LOG_NEW_LINE,
             (int)err.Code);
}

static prop_msg_t proprietary_queue_pend(void)
{
  RTOS_ERR err;

  prop_msg_t propMsg;
  OS_MSG_SIZE propMsgSize;
  propMsg = (prop_msg_t)(uint32_t)OSQPend((OS_Q *)&proprietary_queue,
                                          (OS_TICK) 0,
                                          (OS_OPT) OS_OPT_PEND_BLOCKING,
                                          (OS_MSG_SIZE *)&propMsgSize,
                                          (CPU_TS *) DEF_NULL,
                                          (RTOS_ERR *) &err);

  app_assert(err.Code == RTOS_ERR_NONE,
             "[E: 0x%04x] OS error proprietary_queue_pend" APP_LOG_NEW_LINE,
             (int)err.Code);

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
static void proprietary_timer_callback(void *p_tmr, void *p_arg)
{
  PP_UNUSED_PARAM(p_tmr);
  PP_UNUSED_PARAM(p_arg);

  proprietary_queue_post(PROP_TIMER_EXPIRED);
}

/**************************************************************************//**
 * Proprietary packet send.
 *
 * @param pktType Packet type
 *
 * Sends a packet using RAIL. The data shall be in data_packet
 *****************************************************************************/
static void proprietaryTxPacket(prop_pkt pktType)
{
  RAIL_SchedulerInfo_t schedulerInfo;
  RAIL_Status_t res;
  RAIL_Handle_t rail_handle =
    sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // This assumes the Tx time is around 200us
  schedulerInfo = (RAIL_SchedulerInfo_t){ .priority = 100,
                                          .slipTime = 100000,
                                          .transactionTime = 200 };

  // address of light
  Mem_Copy((void *)&data_packet[PACKET_HEADER_LEN],
           (void *)demo.own_addr.addr,
           sizeof(demo.own_addr.addr));
  // light role
  data_packet[LIGHT_CONTROL_DATA_BYTE] &= ~DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT;
  data_packet[LIGHT_CONTROL_DATA_BYTE] |=
    (DEMO_CONTROL_ROLE_LIGHT << DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT_SHIFT)
    & DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT;
  // advertisement packet
  if (PROP_PKT_ADVERTISE == pktType) {
    data_packet[LIGHT_CONTROL_DATA_BYTE] &= ~DEMO_CONTROL_PAYLOAD_CMD_MASK;
    data_packet[LIGHT_CONTROL_DATA_BYTE] |=
      ((uint8_t)DEMO_CONTROL_CMD_ADVERTISE
        << DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT)
      & DEMO_CONTROL_PAYLOAD_CMD_MASK;
    // status packet
  } else if (PROP_PKT_STATUS == pktType) {
    data_packet[LIGHT_CONTROL_DATA_BYTE] &= ~DEMO_CONTROL_PAYLOAD_CMD_MASK;
    data_packet[LIGHT_CONTROL_DATA_BYTE] |=
      ((uint8_t)DEMO_CONTROL_CMD_LIGHT_STATE_REPORT
        << DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT)
      & DEMO_CONTROL_PAYLOAD_CMD_MASK;
    data_packet[LIGHT_CONTROL_DATA_BYTE] &= ~0x01;
    data_packet[LIGHT_CONTROL_DATA_BYTE] |= (uint8_t)demo.light;
  } else {
  }

  RAIL_WriteTxFifo((RAIL_Handle_t)rail_handle,
                   data_packet,
                   sizeof(data_packet),
                   true);

  res = RAIL_StartTx((RAIL_Handle_t)rail_handle,
                     0,
                     RAIL_TX_OPTIONS_DEFAULT,
                     &schedulerInfo);

  if (res != RAIL_STATUS_NO_ERROR) {
    // Try once to resend the packet 100ms later in case of error
    RAIL_ScheduleTxConfig_t scheduledTxConfig =
    { .when = RAIL_GetTime() + 100000,
      .mode = RAIL_TIME_ABSOLUTE };

    // Transmit this packet at the specified time or up to 50 ms late
    res = RAIL_StartScheduledTx((RAIL_Handle_t)sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0),
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
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR err;
  prop_msg_t propMsg;

  RAIL_Handle_t rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // Start reception
  RAIL_StartRx((RAIL_Handle_t)rail_handle, 0, NULL);

  proprietary.state = PROP_STATE_ADVERTISE;
  OSTmrStart(&proprietary_timer, &err);
  app_assert(err.Code == RTOS_ERR_NONE,
             "[E: 0x%04x] OS Timer error" APP_LOG_NEW_LINE,
             (int)err.Code);

  while (DEF_TRUE) {
    // pending on proprietary message queue
    propMsg = proprietary_queue_pend();

    if (rail_packet_received) {
      rail_packet_received = false;
      handle_receive(rail_handle);
    }

    switch (proprietary.state) {
      case PROP_STATE_ADVERTISE:
        switch (propMsg) {
          case PROP_TIMER_EXPIRED:
            proprietaryTxPacket(PROP_PKT_ADVERTISE);
            break;

          case PROP_TOGGLE_MODE:
            OSTmrStop(&proprietary_timer, OS_OPT_TMR_NONE, DEF_NULL, &err);
            app_assert(err.Code == RTOS_ERR_NONE,
                       "[E: 0x%04x] OS Timer error" APP_LOG_NEW_LINE,
                       (int)err.Code);
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
            proprietaryTxPacket(PROP_PKT_STATUS);
            break;

          case PROP_TOGGLE_MODE:
            OSTmrStart(&proprietary_timer, &err);
            app_assert(err.Code == RTOS_ERR_NONE,
                       "[E: 0x%04x] OS Timer error" APP_LOG_NEW_LINE,
                       (int)err.Code);
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
              (proprietary_rx_buf[DEMO_CONTROL_PAYLOAD_CMD_DATA]
               & DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT)
              >> DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT_SHIFT);
            if (DEMO_CONTROL_ROLE_SWITCH == role) {
              // Handle only those packets that include the Light's address
              if (0 == memcmp((void *)demo.own_addr.addr,
                              (void *)&proprietary_rx_buf[PACKET_HEADER_LEN],
                              sizeof(demo.own_addr.addr))) {
                // packet contains toggle command
                command = (demo_control_command_t)(
                  (proprietary_rx_buf[DEMO_CONTROL_PAYLOAD_CMD_DATA]
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

  // Perform all calibrations when needed
  if ( events & RAIL_EVENT_CAL_NEEDED ) {
    calibration_status = RAIL_Calibrate(rail_handle, NULL, RAIL_CAL_ALL_PENDING);
    if (calibration_status != RAIL_STATUS_NO_ERROR) {
      current_rail_err = (events & RAIL_EVENT_CAL_NEEDED);
    }
  }
}
