/*******************************************************************************
* @file  rsi_queue_rom.c
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

/*==============================================*/
/*
  Include files
  */
#include "rsi_driver.h"
#ifndef ROM_WIRELESS

/** @addtogroup DRIVER16
* @{
*/
/*==============================================*/
/**
 *
 * @fn         void ROM_WL_rsi_queues_init(global_cb_t *global_cb_p, rsi_queue_cb_t *queue)
 * @brief      Initialize queue
 * @param[in]  queue       - pointer to queue 
 * @param[in]  global_cb_p - pointer to the global control block
 * @return     void
 *
 */
/// @private
void ROM_WL_rsi_queues_init(global_cb_t *global_cb_p, rsi_queue_cb_t *queue)
{
  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(global_cb_p);
  //! Initialize head pointer to NULL
  queue->head = NULL;

  // Initialize tail pointer to NULL
  queue->tail = NULL;

  // Initialize pending packet count to zero
  queue->pending_pkt_count = 0;

  // Initialize queue mask to zero
  queue->queue_mask = 0;

  // Mutex for  queues
  RSI_MUTEX_CREATE(&queue->queue_mutex);
}

/*==============================================*/
/**
 * @fn          void ROM_WL_rsi_enqueue_pkt(global_cb_t *global_cb_p, rsi_queue_cb_t *queue,rsi_pkt_t *pkt)
 * @brief       Enqueue packet into queue
 * @param[in]   global_cb_p - pointer to the global control block
 * @param[in]   queue       - pointer to queue cb 
 * @param[in]   pkt         - packet pointer which needs to be queued 
 * @return      void
 */

void ROM_WL_rsi_enqueue_pkt(global_cb_t *global_cb_p, rsi_queue_cb_t *queue, rsi_pkt_t *pkt)
{
  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(global_cb_p);
  //! take lock on queue
  RSI_MUTEX_LOCK(&queue->queue_mutex);

  // check queue is empty
  if (!queue->pending_pkt_count) {
    // if empty then add packet as first packet (head & tail point to first packet)
    queue->head = queue->tail = pkt;
  } else {
    // if not empty append the packet to list at tail
    queue->tail->next = pkt;

    // Make packet as tail
    queue->tail = pkt;
  }

  // increment packet pending count
  queue->pending_pkt_count++;

  // Unlock the queue
  RSI_MUTEX_UNLOCK(&queue->queue_mutex);
}
/*==============================================*/
/**
 * @fn          rsi_pkt_t *ROM_WL_rsi_dequeue_pkt(global_cb_t *global_cb_p, rsi_queue_cb_t *queue)
 * @brief       Dequeue packet from queue
 * @param[in]   global_cb_p - pointer to the global control block
 * @param[in]   queue       - queue pointer from which packet needs to be dequeued 
 * @return      Allocated packet pointer - Success \n
 *              NULL                     - Failure
 */
/// @private
rsi_pkt_t *ROM_WL_rsi_dequeue_pkt(global_cb_t *global_cb_p, rsi_queue_cb_t *queue)
{
  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(global_cb_p);
  rsi_pkt_t *pkt;

  // lock the mutex
  RSI_MUTEX_LOCK(&queue->queue_mutex);

  // check queue is empty
  if (!queue->pending_pkt_count) {

    RSI_MUTEX_UNLOCK(&queue->queue_mutex);
    // return NULL if queue is empty
    return NULL;
  }

  // dequeue the packet from queue
  pkt = queue->head;

  // update the queue head and decrement pending count
  queue->head = queue->head->next;

  // Decrease pending packet count
  queue->pending_pkt_count--;

  // if pending count is zero, then reset head and tail
  if (!queue->pending_pkt_count) {
    queue->head = NULL;
    queue->tail = NULL;
  }

  // Unlock the mutex
  RSI_MUTEX_UNLOCK(&queue->queue_mutex);

  // return the packet
  return pkt;
}
/*==============================================*/
/**
 * @fn          uint32_t ROM_WL_rsi_check_queue_status(global_cb_t *global_cb_p, rsi_queue_cb_t *queue)
 * @brief       Return packet pending count in queue
 * @param[in]   global_cb_p - pointer to the global control block
 * @param[in]   queue       - pointer to queue  
 * @return      0                         - If queue masked \n
 *              Number of packets pending - If queue is not masked
 */
/// @private
uint32_t ROM_WL_rsi_check_queue_status(global_cb_t *global_cb_p, rsi_queue_cb_t *queue)
{
  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(global_cb_p);
  uint32_t pkt_count = 0;

  // lock the mutex
  RSI_MUTEX_LOCK(&queue->queue_mutex);

  // check whether queue is masked or not
  if (queue->queue_mask) {

    // Unlock the mutex
    RSI_MUTEX_UNLOCK(&queue->queue_mutex);

    // if queue masked return 0
    return 0;
  } else {
    pkt_count = queue->pending_pkt_count;

    // Unlock the mutex
    RSI_MUTEX_UNLOCK(&queue->queue_mutex);

    // if queue is not masked return number of packets pending
    return pkt_count;
  }
}

/*====================================================*/
/**
 * @fn          void ROM_WL_rsi_block_queue(global_cb_t *global_cb_p, rsi_queue_cb_t *queue)
 * @brief       Block the queue.
 * @param[in]   global_cb_p - pointer to the global control block
 * @param[in]   queue -  pointer to queue
 * @return      void 
 */
/// @private
void ROM_WL_rsi_block_queue(global_cb_t *global_cb_p, rsi_queue_cb_t *queue)
{
  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(global_cb_p);
  // lock the mutex
  RSI_MUTEX_LOCK(&queue->queue_mutex);

  // Mask the queue
  queue->queue_mask = 1;

  // Unlock the mutex
  RSI_MUTEX_UNLOCK(&queue->queue_mutex);
}

/*====================================================*/
/**
 * @fn          void ROM_WL_rsi_unblock_queue(global_cb_t *global_cb_p, rsi_queue_cb_t *queue)
 * @brief       Unblock the queue.
 * @param[in]   global_cb_p - pointer to the global control block
 * @param[in]   queue 		- pointer to queue
 * @return      void
 */
/// @private
void ROM_WL_rsi_unblock_queue(global_cb_t *global_cb_p, rsi_queue_cb_t *queue)
{
  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(global_cb_p);
  // lock the mutex
  RSI_MUTEX_LOCK(&queue->queue_mutex);

  // Unmask the queue
  queue->queue_mask = 0;

  // Unlock the mutex
  RSI_MUTEX_UNLOCK(&queue->queue_mutex);
}
#endif

/*==============================================*/
/**
 * @fn          void ROM_WL_rsi_enqueue_pkt_from_isr(global_cb_t *global_cb_p, rsi_queue_cb_t *queue,rsi_pkt_t *pkt)
 * @brief       Enqueue packet into queue
 * @param[in]   global_cb_p - pointer to the global control block
 * @param[in]   queue 		- pointer to queue cb
 * @param[in]   pkt 		- packet pointer which needs to be queued
 * @return      void
 */
/// @private
void ROM_WL_rsi_enqueue_pkt_from_isr(global_cb_t *global_cb_p, rsi_queue_cb_t *queue, rsi_pkt_t *pkt)
{
  UNUSED_PARAMETER(global_cb_p); //This statement is added only to resolve compilation warning, value is unchanged
  // check queue is empty
  if (!queue->pending_pkt_count) {
    // if empty then add packet as first packet (head & tail point to first packet)
    queue->head = queue->tail = pkt;
  } else {
    // if not empty append the packet to list at tail
    queue->tail->next = pkt;

    // Make packet as tail
    queue->tail = pkt;
  }

  // increment packet pending count
  queue->pending_pkt_count++;
}
/** @} */
