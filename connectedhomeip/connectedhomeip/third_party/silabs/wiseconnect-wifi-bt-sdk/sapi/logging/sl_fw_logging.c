/*******************************************************************************
 * @file  sl_fw_logging.c
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
/**
 * Includes
 */
#include "rsi_driver.h"
#include "sl_fw_logging.h"

/*==================================================================*/
/**
 * @fn         sl_fw_log_configure
 * @param[in]  logging_enable           - enable or disable logging
 * @param[in]  tsf_granularity          - configure granularity for TSF in micro seconds
 * @param[in]  fw_component_log_level   - configure logging level for firmware components
 * @param[in]  log_buffer_size          - configure logging buffer size (Multiples of 512 bytes; Min buffer size = 2048 bytes; Max buffer size = 4096 bytes)
 * @param[in]  sl_fw_log_callback       - logging callback
 * @param[out] status                   - logging initialisation success or failure 
 * @return     0                        - Success  \n
 *             Non-zero value           - Failure
 * @section description
 * This API is used to send the configurable parameters for firmware logging framework
 */
int32_t sl_fw_log_configure(uint8_t logging_enable,
                            uint8_t tsf_granularity,
                            sl_fw_log_level_t *fw_component_log_level,
                            uint32_t log_buffer_size,
                            void (*sl_fw_log_callback)(uint8_t *log_message, uint16_t log_message_length))
{
  int32_t status = RSI_SUCCESS;
  rsi_pkt_t *pkt;
  sl_fw_logging_t *fw_logging;
  // Get common cb structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;
  if ((common_cb->state < RSI_COMMON_OPERMODE_DONE)) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer  from common pool
    pkt = rsi_pkt_alloc(&common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
    // Callback for logging
    common_cb->sl_fw_log_callback = sl_fw_log_callback;
    // Take the user provided data and fill it in debug uart print select structure
    fw_logging = (sl_fw_logging_t *)pkt->data;
    // Logging enable
    fw_logging->logging_enable = logging_enable;

    // TSF Granularity
    fw_logging->tsf_granularity = tsf_granularity;

    // Log level for each component
    fw_logging->component_log_level.common_log_level        = fw_component_log_level->common_log_level;
    fw_logging->component_log_level.cm_pm_log_level         = fw_component_log_level->cm_pm_log_level;
    fw_logging->component_log_level.wlan_lmac_log_level     = fw_component_log_level->wlan_lmac_log_level;
    fw_logging->component_log_level.wlan_umac_log_level     = fw_component_log_level->wlan_umac_log_level;
    fw_logging->component_log_level.wlan_netstack_log_level = fw_component_log_level->wlan_netstack_log_level;
    fw_logging->component_log_level.bt_ble_ctrl_log_level   = fw_component_log_level->bt_ble_ctrl_log_level;
    fw_logging->component_log_level.bt_ble_stack_log_level  = fw_component_log_level->bt_ble_stack_log_level;

    // Log buffer size
    fw_logging->log_buffer_size = log_buffer_size;

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send firmware version query request
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_DEVICE_LOGGING_INIT, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_DEVICE_LOG_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    return status;
  }

  // Get common command response status
  status = rsi_common_get_status();

  // Return status
  return status;
}

/*==============================================*/
/**
 * @fn         sl_set_fw_component_log_levels
 * @brief      this function sets debug log levels for firmware components
 * @param[in]  component_log_level - log levels for firmware components
 * @return     none.
 */
void sl_set_fw_component_log_levels(sl_fw_log_level_t *component_log_level)
{
  component_log_level->common_log_level        = COMMON_LOG_LEVEL;
  component_log_level->cm_pm_log_level         = CM_PM_LOG_LEVEL;
  component_log_level->wlan_lmac_log_level     = WLAN_LMAC_LOG_LEVEL;
  component_log_level->wlan_umac_log_level     = WLAN_UMAC_LOG_LEVEL;
  component_log_level->wlan_netstack_log_level = WLAN_NETSTACK_LOG_LEVEL;
  component_log_level->bt_ble_ctrl_log_level   = BT_BLE_CTRL_LOG_LEVEL;
  component_log_level->bt_ble_stack_log_level  = BT_BLE_STACK_LOG_LEVEL;
}

/*==============================================*/
/**
 * @fn         sl_fw_log_init
 * @brief      this function initializes the firmware logging feature
 * @param[in]  buffer - address of buffer
 * @param[out] size   - allocated size
 */
uint32_t sl_fw_log_init(uint8_t *buffer)
{
  uint32_t size;
  uint8_t *logging_buffer = buffer;
  //! Designate memory for fw_log_cb
  rsi_driver_cb->fw_log_cb = (sl_fw_log_cb_t *)logging_buffer;
  logging_buffer += sizeof(sl_fw_log_cb_t);
  //! Designate memory for log node pool
  logging_buffer += sl_fw_log_pool_init(&rsi_driver_cb->fw_log_cb->fw_log_node_pool, logging_buffer, FW_LOG_QUEUE_SIZE);
  sl_fw_queue_init(&rsi_driver_cb->fw_log_cb->fw_log_queue);
  size = logging_buffer - buffer;
  return size;
}

/*==============================================*/
/**
 * @fn         sl_fw_queue_init
 * @brief      this function initializes creates a queue for firmware log messages
 * @param[in]  queue - logging queue
 * @return     none
 */
void sl_fw_queue_init(sl_fw_log_queue_t *queue)
{
  //! Initialize firmware logging queue
  queue->head = queue->tail = NULL;
  queue->pending_node_count = 0;
  rsi_mutex_create(&queue->fw_log_mutex);
}

/*==============================================*/
/**
 * @fn         sl_fw_log_deinit
 * @brief      this function de-initializes the firmware logging feature
 * @param[in]  none
 * @param[out] none
 */
void sl_fw_log_deinit(void)
{
  //! Destroy log pool mutex
  rsi_mutex_destroy(&rsi_driver_cb->fw_log_cb->fw_log_node_pool.fw_log_pool_mutex);
  //! Destroy log queue mutex
  rsi_mutex_destroy(&rsi_driver_cb->fw_log_cb->fw_log_queue.fw_log_mutex);
}

/*==============================================*/
/**
 * @fn         sl_fw_log_enqueue
 * @brief      this function enqueues a log message node to the firmware log message queue
 * @param[in]  fw_log_queue         - firmware log queue
 * @param[in]  fw_log_node          - log message node from firmware
 * @return     none.
 */
void sl_fw_log_enqueue(sl_fw_log_queue_t *fw_log_queue, sl_fw_log_node_t *fw_log_node)
{
  if (fw_log_node == NULL) {
    return;
  }
  if (fw_log_queue->pending_node_count == 0) {
    // First node in queue
    fw_log_queue->head = fw_log_queue->tail = fw_log_node;
  } else {
    // Every subsequent node in the queue
    fw_log_queue->tail->next = (struct sl_fw_log_node_t *)fw_log_node;
    fw_log_queue->tail       = fw_log_node;
  }
  fw_log_queue->pending_node_count++;
}

/*==============================================*/
/**
 * @fn         sl_fw_log_dequeue
 * @brief      this function dequeues a log message node from the firmware log message queue
 * @param[in]  fw_log_queue           - firmware log queue
 * @param[in]  remaining_node_count   - count of remaining log message nodes in the queue after the current message is dequeued
 * @param[out] temp_log_node          - log node to be deleted
 * @return     NULL           - queue empty \n
 *             !NULL          - node deleted
 */
sl_fw_log_node_t *sl_fw_log_dequeue(sl_fw_log_queue_t *fw_log_queue, uint16_t *remaining_node_count)
{
  sl_fw_log_node_t *temp_log_node;

  if (fw_log_queue->pending_node_count == 0) {
    // Empty log queue
    *remaining_node_count = fw_log_queue->pending_node_count;
    return NULL;
  } else {
    //! Dequeue log message node
    temp_log_node      = fw_log_queue->head;
    fw_log_queue->head = (sl_fw_log_node_t *)fw_log_queue->head->next;
    //! Decrease node count in queue
    fw_log_queue->pending_node_count--;
    if (fw_log_queue->pending_node_count == 0) {
      //! Queue is empty
      fw_log_queue->head = NULL;
      fw_log_queue->tail = NULL;
    }
  }
  *remaining_node_count = fw_log_queue->pending_node_count;

  return temp_log_node;
}

/*==============================================*/
/**
 * @fn         sl_fw_log_pool_init
 * @brief      Initialize/create log pool from the provided buffer
 * @param[in]  pool_cb        - log pool pointer which needs to be initialized
 * @param[in]  buffer         - buffer pointer to create pool 
 * @param[in]  node_count     - number of nodes to preset.
 * @return     0              - Success \n
 *             Negative value - Failure 
 */
int32_t sl_fw_log_pool_init(sl_fw_log_pool_t *pool_cb, uint8_t *buffer, uint32_t node_count)
{
  uint32_t number_of_nodes;
  uint32_t pool_size;
  uint32_t buffer_index   = 0;
  uint8_t *logging_buffer = buffer;
  uint32_t size;
  // Memset pool structure
  memset(pool_cb, 0, sizeof(sl_fw_log_pool_t));

  // Calculate number of packets can be created with given buffer
  number_of_nodes = node_count;

  // Save pool pointer
  pool_cb->pool = (void *)logging_buffer;

  // Calculate pool array size
  pool_size = (number_of_nodes * 4);

  // Increase buffer pointer by pool size
  // Buffer contains pool array then actual buffer
  logging_buffer += pool_size;
  // While total size is available fill pool array
  while (number_of_nodes > 0) {
    // Fill buffer pointer to current index
    pool_cb->pool[buffer_index++] = logging_buffer;

    // Increase available count
    pool_cb->avail++;

    // Increase pool size
    pool_cb->size++;

    // Move buffer pointer to point next buffer
    logging_buffer += sizeof(sl_fw_log_node_t);

    number_of_nodes--;
  }
  // create mutex for fimrware log pool
  rsi_mutex_create(&pool_cb->fw_log_pool_mutex);

  // calculate size to be incremented in buffer
  size = logging_buffer - buffer;
  return size;
}

/*==============================================*/
/**
 * @fn         sl_fw_log_node_alloc
 * @brief      this function allocates a log node from given log pool
 * @param[in]  pool_cb      - pointer to firmware log pool
 * @param[out] fw_log_node  - pointer to firmware log node
 * @return     NULL         - If allocation fails  \n
 *             !NULL        - Allocated packet pointer if Success
 */
sl_fw_log_node_t *sl_fw_log_node_alloc(sl_fw_log_pool_t *pool_cb)
{
  sl_fw_log_node_t *fw_log_node = NULL;

  rsi_mutex_lock(&pool_cb->fw_log_pool_mutex);

  if (pool_cb->avail == 0) {
    // If available count is zero return NULL
    // release mutex lock
    rsi_mutex_unlock(&pool_cb->fw_log_pool_mutex);
    return NULL;
  }

  // If packets are available then return packet pointer from pool
  fw_log_node = (sl_fw_log_node_t *)(pool_cb->pool[pool_cb->avail - 1]);

  // Decrease available count
  pool_cb->avail--;

  // initialize next with NULL
  fw_log_node->next = NULL;

  // release mutex lock
  rsi_mutex_unlock(&pool_cb->fw_log_pool_mutex);

  // Return node pointer
  return fw_log_node;
}

/*==============================================*/
/**
 * @fn         sl_fw_log_node_free
 * @brief      Free the log node
 * @param[in]  pool_cb        - log pool to which log node needs to be freed 
 * @param[in]  fw_log_node    - log node pointer which needs to be freed
 * @return     0              - Success \n
 *             Negative Value - Failure 
 */
void sl_fw_log_node_free(sl_fw_log_pool_t *pool_cb, sl_fw_log_node_t *fw_log_node)
{

  rsi_mutex_lock(&pool_cb->fw_log_pool_mutex);

  if (pool_cb->avail == pool_cb->size) {
    // If available and size are equal then return an error
    // Release mutex lock
    rsi_mutex_unlock(&pool_cb->fw_log_pool_mutex);
    return;
  }

  // Increase pool available count
  pool_cb->avail++;

  // Fill packet pointer into pool array
  pool_cb->pool[pool_cb->avail - 1] = fw_log_node;

  // Release mutex lock
  rsi_mutex_unlock(&pool_cb->fw_log_pool_mutex);
}

/*==============================================*/
/**
 * @fn         sl_fw_log_print
 * @brief      Print the log message from firmware
 * @param[in]  buffer              - log message to be printed
 * @param[in]  len                 - length of log message
 * @param[in]  msg_lost_flag       - flag indicating if previous message was lost
 * @return     none
 */
void sl_fw_log_print(uint8_t *buffer, uint32_t len, uint16_t msg_lost_flag)
{
  rsi_mutex_lock(&rsi_driver_cb_non_rom->debug_prints_mutex);
  int i = 0;

  if (msg_lost_flag) {
    printf("\r\nPrev Msg Lost");
  }
  printf("\r\n&^$ ");
  for (i = 0; i < len; i++) {
    printf("%02x ", buffer[i]);
  }
  printf("\r\n");
  rsi_mutex_unlock(&rsi_driver_cb_non_rom->debug_prints_mutex);
}
#endif
