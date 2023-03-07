/*******************************************************************************
* @file  rsi_queue.h
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

#ifndef RSI_QUEUE_H
#define RSI_QUEUE_H
/******************************************************
 * *                      Macros
 * ******************************************************/

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
/******************************************************
 * *                    Structures
 * ******************************************************/

// queue structure
typedef struct rsi_queue_cb_s {
  // queue head
  rsi_pkt_t *head;

  // queue tail
  rsi_pkt_t *tail;

  // number of packets pending in queue
  volatile uint16_t pending_pkt_count;

  // mask status of the queue
  uint16_t queue_mask;

  // Mutex handle to queues
  rsi_mutex_handle_t queue_mutex;

} rsi_queue_cb_t;

/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
extern void rsi_queues_init(rsi_queue_cb_t *queue);
extern void rsi_enqueue_pkt(rsi_queue_cb_t *queue, rsi_pkt_t *pkt);
extern rsi_pkt_t *rsi_dequeue_pkt(rsi_queue_cb_t *queue);
extern uint32_t rsi_check_queue_status(rsi_queue_cb_t *queue);
extern void rsi_block_queue(rsi_queue_cb_t *queue);
extern void rsi_unblock_queue(rsi_queue_cb_t *queue);
#endif
