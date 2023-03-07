/***************************************************************************//**
 * @file
 * @brief queues of buffers
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

#ifndef BUFFER_QUEUE_H
#define BUFFER_QUEUE_H

bool emBufferQueueIsEmpty(Buffer *queue);
Buffer emGenericQueueHead(Buffer *queue, uint16_t i);
#define emBufferQueueHead(queue) (emGenericQueueHead((queue), QUEUE_LINK))
#define emPayloadQueueHead(queue) (emGenericQueueHead((queue), PAYLOAD_LINK))

void emGenericQueueAdd(Buffer *queue, Buffer newTail, uint16_t i);
#define emBufferQueueAdd(queue, newTail) \
  (emGenericQueueAdd((queue), (newTail), QUEUE_LINK))
#define emPayloadQueueAdd(queue, newTail) \
  (emGenericQueueAdd((queue), (newTail), PAYLOAD_LINK))

void emBufferQueuePreInsertBuffer(Buffer *queue, Buffer bufferToInsert, Buffer bufferToInsertBefore);

// Add as the head, not the tail.
void emBufferQueueAddToHead(Buffer *queue, Buffer newHead);

Buffer emGenericQueueRemoveHead(Buffer *queue, uint16_t i);
#define emBufferQueueRemoveHead(queue) \
  (emGenericQueueRemoveHead((queue), QUEUE_LINK))
#define emPayloadQueueRemoveHead(queue) \
  (emGenericQueueRemoveHead((queue), PAYLOAD_LINK))

uint16_t emGenericQueueRemove(Buffer *queue, Buffer buffer, uint16_t i);
#define emBufferQueueRemove(queue, buffer) \
  (emGenericQueueRemove((queue), (buffer), QUEUE_LINK))

// The number of buffers in the queue, not the number of bytes.
uint16_t emBufferQueueLength(Buffer *queue);

// Doubled up with emBufferQueueRemove() to save flash.
#define emBufferQueueByteLength(queueIndex) \
  (emGenericQueueRemove((queueIndex), NULL_BUFFER, QUEUE_LINK))
#define emPayloadQueueByteLength(queueIndex) \
  (emGenericQueueRemove((queueIndex), NULL_BUFFER, PAYLOAD_LINK))

// How to walk down a queue:
//   Buffer finger = emBufferQueueHead(queue);
//   while (finger != NULL_BUFFER) {
//     ...
//     finger = emBufferQueueNext(queue, finger);
//   }
//
// To remove one or more elements while walking a queue, put all the
// buffers on temporary queue and then move the live values back to the
// original:
//
//  Buffer temp = queue;
//  queue = NULL_BUFFER;
//  while (! emBufferQueueIsEmpty(&temp)) {
//    Buffer next = emBufferQueueRemoveHead(&temp);
//    if (isLive next) {
//      emBufferQueueAdd(&queue, next);
//    }
//  }

Buffer emBufferQueueNext(Buffer *queue, Buffer finger);

// Remove the first 'count' bytes from the queue of buffers, truncating
// the final head if necessary;

uint16_t emRemoveBytesFromGenericQueue(Buffer *queue, uint16_t count, uint16_t i);
#define emRemoveBytesFromBufferQueue(queue, count) \
  (emRemoveBytesFromGenericQueue((queue), (count), QUEUE_LINK))
#define emRemoveBytesFromPayloadQueue(queue, count) \
  (emRemoveBytesFromGenericQueue((queue), (count), PAYLOAD_LINK))

void emCopyFromGenericQueue(Buffer *queue, uint16_t count, uint8_t *to, uint16_t i);
#define emCopyFromBufferQueue(queue, count, to) \
  (emCopyFromGenericQueue((queue), (count), (to), QUEUE_LINK))
#define emCopyFromPayloadQueue(queue, count, to) \
  (emCopyFromGenericQueue((queue), (count), (to), PAYLOAD_LINK))

// Converting back and forth between linked payloads and queues.
void emLinkedPayloadToPayloadQueue(Buffer *queue);
void emPayloadQueueToLinkedPayload(Buffer *queue);

//----------------------------------------------------------------
// Experimental utilities to use a queue of buffers as an extensible
// vector (a one-dimensional array).

typedef struct {
  Buffer values;        // initialize this to NULL_BUFFER
  uint16_t valueSize;     // sizeof(whateverGoesInTheVector)
  uint16_t valueCount;    // initialize to zero
  uint16_t emptyCount;    // initialize to zero
} Vector;

typedef bool (*EqualityPredicate)(const void *x, const void *y);

// Returns the first element in the vector for which
// predicate(element, target) returns true.  If indexLoc is non-NULL
// the index of the element is stored there.
void *emVectorSearch(Vector *vector,
                     EqualityPredicate predicate,
                     const void *target,
                     uint16_t *indexLoc);

// Return the 'index'th element of the vector.
void *emVectorRef(Vector *vector, uint16_t index);

// Add space for 'quanta' more elements to the vector.
void *emVectorAdd(Vector *vector, uint16_t quanta);

// Returns the index of 'value' in 'vector', or -1 if it isn't there.
// This is intended for use in debugging.
uint16_t emVectorFindIndex(Vector *vector, const uint8_t *value);

// Returns the number of elements that match predicate(element, target)
uint16_t emVectorMatchCount(Vector *vector,
                            EqualityPredicate predicate,
                            const void *target);

#endif // BUFFER_QUEUE_H
