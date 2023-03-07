/***************************************************************************//**
 * @file
 * @brief The source for a circular queue of pointer values.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdlib.h>

#include "circular_queue.h"

#include "em_core.h"

bool queueInit(Queue_t *queue, uint16_t size)
{
  // Make sure we have enough room for this queue
  if (size > CIRCULAR_QUEUE_LEN_MAX || queue == NULL) {
    return false;
  }

  // Atomically initialize the queue structure
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  queue->size = size;
  queue->head = 0;
  queue->count = 0;
  queue->callback = NULL;
  CORE_EXIT_CRITICAL();

  return true;
}

bool queueOverflow(Queue_t *queue, QueueOverflowCallback_t callback)
{
  queue->callback = callback;
  return (callback != NULL);
}

bool queueAdd(Queue_t *queue, void *data)
{
  // Do nothing if there's no queue given
  if (queue == NULL) {
    return false;
  }
  bool added = true;

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if (queue->count < queue->size) {
    uint16_t index = (queue->head + queue->count) % queue->size;

    // Insert this item at the end of the queue
    queue->data[index] = data;
    queue->count++;
  } else {
    // If an overflow callback exists, call it to see if the oldest queued
    // item is to be replaced (default) or not.
    if (queue->callback != NULL) {
      added = queue->callback(queue, queue->data[queue->head]);
    }
    // Overwrite what's at the head of the queue since we're out of space
    if (added) {
      queue->data[queue->head] = data;
      queue->head = (queue->head + 1) % queue->size;
    }
  }
  CORE_EXIT_CRITICAL();

  return added;
}

void *queuePeek(Queue_t *queue)
{
  void* ptr = NULL;

  // Do nothing if there's no queue given
  if (queue == NULL) {
    return NULL;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if (queue->count > 0) {
    ptr = queue->data[queue->head];
  }
  CORE_EXIT_CRITICAL();

  return ptr;
}

void *queueRemove(Queue_t *queue)
{
  void* ptr = NULL;

  // Do nothing if there's no queue given
  if (queue == NULL) {
    return NULL;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if (queue->count > 0) {
    ptr = queue->data[queue->head];
    queue->head = (queue->head + 1) % queue->size;
    queue->count--;
  }
  CORE_EXIT_CRITICAL();

  return ptr;
}

bool queueIsEmpty(Queue_t *queue)
{
  bool result;

  if (queue == NULL) {
    return true;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  result = (queue->count == 0);
  CORE_EXIT_CRITICAL();

  return result;
}

bool queueIsFull(Queue_t *queue)
{
  bool result;

  if (queue == NULL) {
    return true;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  result = (queue->count == queue->size);
  CORE_EXIT_CRITICAL();

  return result;
}
