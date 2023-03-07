/*******************************************************************************
* @file  rsi_events.h
* @brief 
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

#ifndef RSI_EVENTS_H
#define RSI_EVENTS_H
/******************************************************
 * *                      Macros
 * ******************************************************/
#ifndef RSI_M4_INTERFACE
#ifdef RSI_ENABLE_DEMOS
// Max number events used in the driver
#define RSI_MAX_NUM_EVENTS 5

// RX event number used in the driver
#define RSI_RX_EVENT 1

// TX event number used in the driver
#define RSI_TX_EVENT 0

// Socket event number used in the driver
#define RSI_SOCKET_EVENT 2

// Application event1 number used in driver
#define RSI_APP_EVENT1 3

// Application event1 number used in driver
#define RSI_APP_EVENT2 4

#else

// Max number events used in the driver
#define RSI_MAX_NUM_EVENTS 3

// RX event number used in the driver
#define RSI_RX_EVENT       0

// TX event number used in the driver
#define RSI_TX_EVENT       1

// Socket event number used in the driver
#define RSI_SOCKET_EVENT   2

#endif
#else

// Max number events used in the driver
#define RSI_MAX_NUM_EVENTS 2

// RX event number used in the driver
#define RSI_RX_EVENT       1

// TX event number used in the driver
#define RSI_TX_EVENT       0

#endif

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// event handler structure
typedef struct rsi_event_cb_s {
  // event handler pointer
  void (*event_handler)(void);
} rsi_event_cb_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

uint16_t rsi_register_event(uint32_t event_id, void (*event_handler_ptr)(void));
uint32_t rsi_find_event(uint32_t event_map);
void rsi_set_event(uint32_t event_num);
void rsi_clear_event(uint32_t event_num);
void rsi_mask_event(uint32_t event_num);
void rsi_unmask_event(uint32_t event_num);
void rsi_tx_event_handler(void);
void rsi_rx_event_handler(void);
void rsi_set_event_from_isr(uint32_t event_num);
void rsi_unmask_event_from_isr(uint32_t event_num);
rsi_error_t rsi_semaphore_post_from_isr(rsi_semaphore_handle_t *semaphore);
#ifndef RSI_M4_INTERFACE

#ifdef RSI_ENABLE_DEMOS

#if (BT_A2DP_SOURCE_WIFI_HTTP_S_RX || BLE_DUAL_MODE_BT_A2DP_SOURCE_WIFI_HTTP_S_RX \
     || BT_A2DP_SOURCE_WIFI_HTTP_S_RX_DYN_COEX)
void rsi_app_event1_handler(void);
void rsi_app_event2_handler(void);
#endif

#endif

void rsi_socket_event_handler(void);
#endif
void rsi_events_init(void);

#endif
