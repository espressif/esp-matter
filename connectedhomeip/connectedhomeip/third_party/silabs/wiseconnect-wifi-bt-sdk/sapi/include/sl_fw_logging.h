/*******************************************************************************
* @file  sl_logging.h
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
#ifndef __SL_FW_LOGGING_H__
#define __SL_FW_LOGGING_H__
#ifdef FW_LOGGING_ENABLE
#include "rsi_common.h"
extern rsi_driver_cb_non_rom_t *rsi_driver_cb_non_rom;

// Max firmware log message length
#define MAX_FW_LOG_MSG_LEN \
  ((((FW_LOG_BUFFER_SIZE + 1023) & ~1023) / 2) > 1600) ? 1600 : (((FW_LOG_BUFFER_SIZE + 1023) & ~1023) / 2)

// Firmware component log level values
typedef enum { FW_LOG_OFF, FW_LOG_ERROR, FW_LOG_WARN, FW_LOG_TRACE, FW_LOG_INFO } fw_log_levels_t;

// Firmware logging memory pool structure
typedef struct sl_fw_log_pool_s {
  //! Firmware log mutex
  rsi_mutex_handle_t fw_log_pool_mutex;
  //! Pool total node count
  uint16_t size;
  //! Pool available node count
  uint16_t avail;
  //! Pool pointer
  void **pool;
} sl_fw_log_pool_t;

// Firmware logging message node structure
typedef struct sl_fw_log_node_s {
  //! Message lost flag
  uint16_t prev_message_lost;
  //! Log message length
  uint16_t fw_log_message_length;
  //! Log message
  uint8_t fw_log_message[MAX_FW_LOG_MSG_LEN];
  //! Pointer to next log node
  struct sl_fw_log_node_t *next;
} sl_fw_log_node_t;

//Firmware logging queue structure
typedef struct sl_fw_log_queue_s {
  //! Head of queue
  sl_fw_log_node_t *head;
  //! Tail of queue
  sl_fw_log_node_t *tail;
  //! Nodes pending in the queue
  volatile uint16_t pending_node_count;
  //! Mutex for firmware logging
  rsi_mutex_handle_t fw_log_mutex;
} sl_fw_log_queue_t;

//Firmware logging control block structure
typedef struct sl_fw_log_cb_s {
  //! Firmware log queue
  sl_fw_log_queue_t fw_log_queue;
  //! Firmware log pool
  sl_fw_log_pool_t fw_log_node_pool;
} sl_fw_log_cb_t;

// Firmware component log level structure
typedef struct sl_fw_log_level_s {
  uint8_t common_log_level;
  uint8_t cm_pm_log_level;
  uint8_t wlan_lmac_log_level;
  uint8_t wlan_umac_log_level;
  uint8_t wlan_netstack_log_level;
  uint8_t bt_ble_ctrl_log_level;
  uint8_t bt_ble_stack_log_level;
  uint8_t reserved;
} sl_fw_log_level_t;

// Firmware logging configuration frame structure
typedef struct sl_fw_logging_s {
  uint8_t logging_enable;
  uint8_t tsf_granularity;
  uint16_t reserved_1;
  sl_fw_log_level_t component_log_level; // log levels for the 7 firmware components and 1 byte reserved
  uint32_t reserved_2;
  uint32_t log_buffer_size;
} sl_fw_logging_t;

// Function prototypes to set firmware logging configuration
// Firmware logging configuration functions
void sl_set_fw_component_log_levels(sl_fw_log_level_t *component_log_level);
int32_t sl_fw_log_configure(uint8_t logging_enable,
                            uint8_t tsf_granularity,
                            sl_fw_log_level_t *component_log_level,
                            uint32_t log_buffer_size,
                            void (*sl_fw_log_callback)(uint8_t *log_message, uint16_t log_message_length));

// Firmware log init/deinit functions
uint32_t sl_fw_log_init(uint8_t *buffer);
void sl_fw_log_deinit(void);

// Firmware log queue functions
void sl_fw_queue_init(sl_fw_log_queue_t *queue);
void sl_fw_log_enqueue(sl_fw_log_queue_t *fw_log_queue, sl_fw_log_node_t *fw_log_node);
sl_fw_log_node_t *sl_fw_log_dequeue(sl_fw_log_queue_t *fw_log_queue, uint16_t *remaining_node_count);

// Firmware log pool functions
int32_t sl_fw_log_pool_init(sl_fw_log_pool_t *pool_cb, uint8_t *buffer, uint32_t node_count);
sl_fw_log_node_t *sl_fw_log_node_alloc(sl_fw_log_pool_t *pool_cb);
void sl_fw_log_node_free(sl_fw_log_pool_t *pool_cb, sl_fw_log_node_t *fw_log_node);

// Firmware log message print function
void sl_fw_log_print(uint8_t *buffer, uint32_t len, uint16_t msg_lost_flag);

#endif
#endif