/*******************************************************************************
* @file  rsi_queue.c
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
/** @addtogroup DRIVER16
* @{
*/
/*==============================================*/
/**
 *
 * @fn         void rsi_queues_init(rsi_queue_cb_t *queue)
 * @brief      Initialize queue
 * @param[in]  queue - pointer to queue 
 * @return     void   
 *
 */
// the below group is mentioned for hiding below internal API.
/// @private
void rsi_queues_init(rsi_queue_cb_t *queue)
{
#ifdef ROM_WIRELESS
  ROMAPI_WL->rsi_queues_init(global_cb_p, queue);
#else
  api_wl->rsi_queues_init(global_cb_p, queue);
#endif
}
/*==============================================*/
/**
 * @fn          void rsi_enqueue_pkt(rsi_queue_cb_t *queue, rsi_pkt_t *pkt)
 * @brief       Enqueue packet into queue
 * @param[in]   queue - pointer to queue cb 
 * @param[in]   pkt   - packet pointer which needs to be queued
 * @return      void  
 *
 */
/// @private
void rsi_enqueue_pkt(rsi_queue_cb_t *queue, rsi_pkt_t *pkt)
{
#ifdef ROM_WIRELESS
  ROMAPI_WL->rsi_enqueue_pkt(global_cb_p, queue, pkt);
#else
  api_wl->rsi_enqueue_pkt(global_cb_p, queue, pkt);
#endif
}
/*==============================================*/
/**
 * @fn          rsi_pkt_t *rsi_dequeue_pkt(rsi_queue_cb_t *queue)
 * @brief       Dequeue packet from queue
 * @param[in]   queue - queue pointer from which packet needs to be dequeued \n 
 * @return      Allocated pocket pointer - Success \n
 *              NULL                     - Failure
 *
 */
/// @private
rsi_pkt_t *rsi_dequeue_pkt(rsi_queue_cb_t *queue)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_dequeue_pkt(global_cb_p, queue);
#else
  return api_wl->rsi_dequeue_pkt(global_cb_p, queue);
#endif
}
/*==============================================*/
/**
 * @fn          uint32_t rsi_check_queue_status(rsi_queue_cb_t *queue)
 * @brief       Return packet pending count in queue
 * @param[in]   queue - pointer to queue  
 * @return      0                         - If queue masked \n
 *              Number of packets pending - If queue is not masked 
 */
/// @private
uint32_t rsi_check_queue_status(rsi_queue_cb_t *queue)
{
#ifdef ROM_WIRELESS
  return ROMAPI_WL->rsi_check_queue_status(global_cb_p, queue);
#else
  return api_wl->rsi_check_queue_status(global_cb_p, queue);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_block_queue(rsi_queue_cb_t *queue)
 * @brief       Block the queue.
 * @param[in]   queue - pointer to queue  
 * @return      void  
 */
/// @private
void rsi_block_queue(rsi_queue_cb_t *queue)
{
#ifdef ROM_WIRELESS
  ROMAPI_WL->rsi_block_queue(global_cb_p, queue);
#else
  api_wl->rsi_block_queue(global_cb_p, queue);
#endif
}

/*====================================================*/
/**
 * @fn          void rsi_unblock_queue(rsi_queue_cb_t *queue)
 * @brief       Unblock the queue.
 * @param[in]   queue - pointer to queue 
 * @return      void  
 */
/// @private
void rsi_unblock_queue(rsi_queue_cb_t *queue)
{
#ifdef ROM_WIRELESS
  ROMAPI_WL->rsi_unblock_queue(global_cb_p, queue);
#else
  api_wl->rsi_unblock_queue(global_cb_p, queue);
#endif
}
/** @} */
