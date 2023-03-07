/***************************************************************************//**
 * @file
 * @brief Header for EZSP host queue functions
 *
 * See @ref ezsp_util for documentation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

/** @addtogroup ezsp_util
 *
 * See ezsp-host-queues.h.
 *
 *@{
 */

#ifndef SILABS_EZSP_HOST_QUEUE_H
#define SILABS_EZSP_HOST_QUEUE_H

/** @brief The number of transmit buffers must be set to the number of receive buffers
 * -- to hold the immediate ACKs sent for each callabck frame received --
 * plus 3 buffers for the retransmit queue and one each for an automatic ACK
 * (due to data flow control) and a command.
 */
#define TX_POOL_BUFFERS   (EZSP_HOST_RX_POOL_SIZE + 5)

/** @brief Define the limits used to decide if the host will hold off the ncp from
 * sending normal priority frames.
 */
#define RX_FREE_LWM 8
#define RX_FREE_HWM 12

/** @brief Buffer to hold a DATA frame.
 */
typedef struct ezspBuffer {
  struct ezspBuffer *link;
  uint8_t len;
  uint8_t data[EZSP_MAX_FRAME_LENGTH];
} EzspBuffer;

/** @brief Simple queue (singly-linked list)
 */
typedef struct {
  EzspBuffer *tail;
} EzspQueue;

/** @brief Simple free list (singly-linked list)
 */
typedef struct {
  EzspBuffer *link;
} EzspFreeList;

/** @brief Initializes all queues and free lists.
 *  All receive buffers are put into rxFree, and rxQueue is empty.
 *  All transmit buffers are put into txFree, and txQueue and reTxQueue are
 *  empty.
 */
void ezspInitQueues(void);

/** @brief  Add a buffer to the free list.
 *
 * @param list    pointer to the free list
 * @param buffer  pointer to the buffer
 */
void ezspFreeBuffer(EzspFreeList *list, EzspBuffer *buffer);

/** @brief  Get a buffer from the free list.
 *
 * @param list    pointer to the free list
 *
 * @return        pointer to the buffer allocated, NULL if free list was empty
 */
EzspBuffer *ezspAllocBuffer(EzspFreeList *list);

/** @brief  Remove the buffer at the head of a queue. The queue must not
 *  be empty.
 *
 * @param queue   pointer to the queue
 *
 * @return        pointer to the buffer that had been the head of the queue
 */
EzspBuffer *ezspRemoveQueueHead(EzspQueue *queue);

/** @brief  Get a pointer to the buffer at the head of the queue. The
 *  queue must not be empty.
 *
 * @param queue   pointer to the queue
 *
 * @return        pointer to the buffer at the nead of the queue
 */
EzspBuffer *ezspQueueHead(EzspQueue *queue);

/** @brief  Get a pointer to the Nth entry in a queue. The tail is entry
 *  number 1, and if the queue has N entries, the head is entry number N.
 *  The queue must not be empty.
 *
 * @param queue   pointer to the queue
 * @param n       number of the entry to which a pointer will be returned
 *
 * @return        pointer to the Nth queue entry
 */
EzspBuffer *ezspQueueNthEntry(EzspQueue *queue, uint8_t n);

/** @brief  Get a pointer to the queue entry before (closer to the tail)
 *  than the specified entry.
 *  If the entry specified is the tail, NULL is returned.
 *  If the entry specifed is NULL, a pointer to the head is returned.
 *
 * @param queue   pointer to the queue
 * @param buffer  pointer to the buffer whose predecessor is wanted
 *
 * @return        pointer to the buffer before that specifed, or NULL if none
 */
EzspBuffer *ezspQueuePrecedingEntry(EzspQueue *queue, EzspBuffer *buffer);

/** @brief  Removes the buffer from the queue, and returns a pointer to
 * its predecssor, if there is one, otherwise it returns NULL.
 *
 * @param queue   pointer to the queue
 * @param buffer  pointer to the buffer to be removed
 *
 * @return        pointer to the buffer before that removed, or NULL if none
 */
EzspBuffer *ezspRemoveQueueEntry(EzspQueue *queue, EzspBuffer *buffer);

/** @brief  Returns the number of entries in the queue.
 *
 * @param queue   pointer to the queue
 *
 * @return        number of entries in the queue
 */
uint8_t ezspQueueLength(EzspQueue *queue);

/** @brief  Returns the number of entries in the free list.
 *
 * @param list    pointer to the free list
 *
 * @return        number of entries in the free list
 */
uint8_t ezspFreeListLength(EzspFreeList *list);

/** @brief Add a buffer to the tail of the queue.
 *
 * @param queue   pointer to the queue
 * @param buffer  pointer to the buffer
 */
void ezspAddQueueTail(EzspQueue *queue, EzspBuffer *buffer);

/** @brief  Returns true if the queue is empty.
 *
 * @param queue   pointer to the queue
 *
 * @return        true if the queue is empty
 */
bool ezspQueueIsEmpty(EzspQueue *queue);

extern EzspQueue txQueue;
extern EzspQueue reTxQueue;
extern EzspQueue rxQueue;
extern EzspFreeList txFree;
extern EzspFreeList rxFree;

#endif //__EZSP_HOST_QUEUE_H__

/** @} END addtogroup
 */
