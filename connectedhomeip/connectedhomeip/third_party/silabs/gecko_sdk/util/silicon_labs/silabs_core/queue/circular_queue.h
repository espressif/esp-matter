/***************************************************************************//**
 * @file
 * @brief A circular queue of pointers. If the queue becomes full the next add
 * will overwrite the values currently stored in the queue and advance the
 * pointers.
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

#ifndef CIRCULAR_QUEUE_H__
#define CIRCULAR_QUEUE_H__

// Get the standard include types
#include <stdint.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------
// Configuration Macros
// -----------------------------------------------------------------------------
// Pull in correct config file:
#ifdef CIRCULAR_QUEUE_USE_LOCAL_CONFIG_HEADER
  #include "circular_queue_config.h" // component-level config file (new method)
#else // !defined(CIRCULAR_QUEUE_USE_LOCAL_CONFIG_HEADER)
  #ifdef CONFIGURATION_HEADER
    #include CONFIGURATION_HEADER // application-level config file (old method)
  #endif

  #ifdef MAX_QUEUE_LENGTH
    #define CIRCULAR_QUEUE_LEN_MAX MAX_QUEUE_LENGTH
  #else
    #define CIRCULAR_QUEUE_LEN_MAX 10U
  #endif
#endif // defined(CIRCULAR_QUEUE_USE_LOCAL_CONFIG_HEADER)

// -----------------------------------------------------------------------------
// Structures and Types
// -----------------------------------------------------------------------------
typedef struct Queue {
  uint16_t head;
  uint16_t count;
  uint16_t size;
  bool (*callback)(const struct Queue *queue, void *data);
  void* data[CIRCULAR_QUEUE_LEN_MAX];
} Queue_t;

/**
 * Pointer to a queue overflow callback function.
 * @param queue Pointer to the queue structure being overflowed.
 * @param data Pointer to the item being considered for removal from the queue
 * on overflow.
 * @return True to discard the queue item being considered for removal
 * (default behavior) and false for nothing to be discarded from the queue.
 */
typedef bool (*QueueOverflowCallback_t)(const Queue_t *queue, void *data);

// -----------------------------------------------------------------------------
// Standard Queue Functions
// -----------------------------------------------------------------------------
/**
 * Function to initialize a queue structure with the given size. Note that this
 * size must be less than the CIRCULAR_QUEUE_LEN_MAX define.
 * @param queue A pointer to the queue structure to initialize
 * @param size The number of entries we want to allow you to store in this queue
 * @return Returns true if we were able to initialize the queue and false
 * otherwise.
 */
bool queueInit(Queue_t *queue, uint16_t size);

/**
 * Add the specified data pointer to the end of the queue. If the queue is
 * full, the oldest queued item will be replaced by default. Register an
 * overflow callback to change this default behavior.
 * @param queue The queue to add the item to.
 * @param data The pointer object to store in the queue.
 * @return Returns true if the data was stored and false otherwise.
 */
bool queueAdd(Queue_t *queue, void *data);

/**
 * Specify a callback to be called upon queue overflow. The oldest queued item
 * being considered for removal on queue overflow is provided by this callback.
 * @param queue The queue to specify an overflow callback for.
 * @param callback The callback to be called on queue overflow. If callback is
 * NULL, no callback will be issued on queue overflow.
 * @return Return true if a callback will be issued on queue overflow and
 * false otherwise.
 */
bool queueOverflow(Queue_t *queue, QueueOverflowCallback_t callback);

/**
 * Return a pointer to the head of the queue without removing that item.
 * @param queue The queue to peek at the item from.
 * @return Returns a pointer to the data that is at the head of the queue. If
 * the queue is empty this value will be NULL. It's worth noting that NULL can
 * be a valid queued value.
 */
void *queuePeek(Queue_t *queue);

/**
 * Remove an item from the head of the queue and return its pointer.
 * @param queue The queue to remove the item from.
 * @return Returns a pointer to the data that was at the head of the queue. If
 * the queue is empty this value will be NULL. It's worth noting that NULL can
 * also be a valid stored pointer.
 */
void *queueRemove(Queue_t *queue);

/**
 * Determine if the given queue is empty.
 * @param queue The queue to check the status of.
 * @return Returns true if the queue is empty and false otherwise.
 */
bool queueIsEmpty(Queue_t *queue);

/**
 * Determine if the given queue is full.
 * @param queue The queue to check the status of.
 * @return Returns true if the queue is full and false otherwise.
 */
bool queueIsFull(Queue_t *queue);

#endif // CIRCULAR_QUEUE_H__
