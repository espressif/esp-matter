/*******************************************************************************
 * @file  fw_logging_utils.c
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

#ifdef FW_LOGGING_ENABLE
//! Includes
#include <stdio.h>
#include <string.h>
#include "rsi_driver.h"
#include "sl_fw_logging.h"

//! Defines
#ifdef RSI_WITH_OS
#define RSI_FOREVER 1
#else
#define RSI_FOREVER 0
#endif

//! Variables
static uint32_t fw_log_flag = 0;
rsi_semaphore_handle_t fw_log_app_sem;
uint16_t fw_log_message_lost;

/*==============================================*/
/**
 * @fn         sl_fw_log_dump
 * @brief      this function handles dequeuing and printing of log messages
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function dequeues log messages, prints them and frees the messages from the logging pool
 */
void sl_fw_log_dump(void)
{
  sl_fw_log_node_t *log_node;
  uint16_t queue_node_count;

  rsi_mutex_lock(&rsi_driver_cb->fw_log_cb->fw_log_queue.fw_log_mutex);
  //! Dequeue firmware logging message node
  log_node = sl_fw_log_dequeue(&rsi_driver_cb->fw_log_cb->fw_log_queue, &queue_node_count);
  if (!queue_node_count) {
    fw_log_flag = 0;
  }
  //! Release mutex lock
  rsi_mutex_unlock(&rsi_driver_cb->fw_log_cb->fw_log_queue.fw_log_mutex);

  if (log_node != NULL) {
    //! Print firmware log message
    sl_fw_log_print(log_node->fw_log_message, log_node->fw_log_message_length, log_node->prev_message_lost);
    //! Free log node from log pool
    sl_fw_log_node_free(&rsi_driver_cb->fw_log_cb->fw_log_node_pool, log_node);
  }
}

/*==============================================*/
/**
 * @fn         sl_fw_log_task
 * @brief      task function for firmware logging
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function handles the firmware logging task
 */
void sl_fw_log_task(void)
{
  do {
#ifdef RSI_WITH_OS
    rsi_semaphore_wait(&fw_log_app_sem, 0);
#endif
    if (fw_log_flag) {
      sl_fw_log_dump();
    }
  } while (RSI_FOREVER);
}
/*==============================================*/
/**
 * @fn         sl_fw_log_callback
 * @brief      callback function to process log messages from firmware and enqueue messages
 * @param[in]  log_message         - debug log message from firmware
 * @param[in]  log_message_length  - length of debug log message from firmware
 * @return     none.
 * @section description
 * This function processes the log messages from firmware, allocates memory to log nodes and enqueues the log nodes
 */
void sl_fw_log_callback(uint8_t *log_message, uint16_t log_message_length)
{
  sl_fw_log_node_t *log_message_node;

  //! Create fw log node
  log_message_node = sl_fw_log_node_alloc(&rsi_driver_cb->fw_log_cb->fw_log_node_pool);

  if (log_message_node == NULL) {
    //! Could not allocate memory for log message node
    //! Set message lost flag
    fw_log_message_lost = 1;
    return;
  }

  //! Fill local fw log message node
  log_message_node->prev_message_lost = fw_log_message_lost;
  // Reset message lost flag
  fw_log_message_lost                     = 0;
  log_message_node->fw_log_message_length = log_message_length;
  memcpy(log_message_node->fw_log_message, log_message, log_message_length);

  rsi_mutex_lock(&rsi_driver_cb->fw_log_cb->fw_log_queue.fw_log_mutex);
  //! Enqueue firmware log message node
  sl_fw_log_enqueue(&rsi_driver_cb->fw_log_cb->fw_log_queue, log_message_node);
  //! Set firmware logging flag to 1
  fw_log_flag = 1;
  rsi_mutex_unlock(&rsi_driver_cb->fw_log_cb->fw_log_queue.fw_log_mutex);

#ifdef RSI_WITH_OS
  rsi_semaphore_post(&fw_log_app_sem);
#endif
}

#endif