/**
* @file
* Notifying Queue module.
*
* @details The module depends on FreeRTOS task and queue.
* The module provides a queue that sets a bit in a tasks notification
* register when an item is put on the queue.
*
* The module is a very simple wrapper of a FreeRTOS queue.
* For Queue functionality beyond "SendToFront" and "SendToBack", simply
* use the Queue interface directly.
*
* @copyright 2018 Silicon Laboratories Inc.
*/

#ifndef _QUEUENOTIFYING_H_
#define _QUEUENOTIFYING_H_

#include <stdint.h>

/**
* @addtogroup Components
* @{
* @addtogroup Queue
* @{
* @addtogroup QueueNotifying
* @{
 */

/**
* Defines return values from QueueBlocking methods.
*/
typedef enum EQueueNotifyingStatus
{
  EQUEUENOTIFYING_STATUS_SUCCESS = 0, /**< Item successfully put on queue
                                           and receiver task notified. */
  EQUEUENOTIFYING_STATUS_TIMEOUT = 1  /**< Failed to put item on queue as it was full.
                                           Queue remained full until timeout */
} EQueueNotifyingStatus;

/**
* Notifying Queue object. All content is public.
*/
typedef struct SQueueNotifying
{
  void *Queue;                          /**< FreeRTOS Queue handle */

  uint8_t iTaskNotificationBitNumber;   /**< which bit to use when notifying receiver
                                             task of pending queue item (range 0 - 31) */

  void *ReceiverTask;                   /**< Handle to queue receiver task where
                                             notifications are sent to */
} SQueueNotifying;


/**
* Initialize QueueNotifying.
* 
* Must be called prior to calling any other QueueNotifying methods on a QueueNotifying object.
*
* @param[in]     pThis              Pointer to the QueueNotifying object
* @param[in]     Queue              FreeRTOS queue handle. The Queue wrapped by QueueNotifying.
* @param[in]     ReceiverTask       FreeRTOS task handle for the task to be notified on adding
*                                   items to queue.
* @param[in]     iTaskNotificationBitNumber   Number defines which bit to use when notifying
*                                             receiver task of item added to queue (range 0 - 31)
*/
void QueueNotifyingInit(SQueueNotifying* pThis,
                        void *Queue,
                        void *ReceiverTask,
                        uint8_t iTaskNotificationBitNumber);

/**
* Adds item to back of queue and notifies receiver task.
*
* Mainly a wrapper method for FreeRTOS xQueueSendToBack().
* A notification is also sent to the receiver task.
*
* @param[in]     pThis        Pointer to the QueueNotifying object.
* @param[in]     pItem        Pointer to item to put on Queue. Expected size is the size
*                             configured in the Queue passed on QueueNotifyingInit.
* @param[in]     iTimeToWait  How many milliseconds to wait if queue is full. 0 means dont wait.
*/
EQueueNotifyingStatus QueueNotifyingSendToBack(SQueueNotifying* pThis, const uint8_t* pItem, uint32_t iTimeToWait);

/**
* Adds item to back of queue and notifies receiver task.
*
* Mainly a wrapper method for FreeRTOS xQueueSendToBackFromISR().
* A notification is also sent to the receiver task.
*
* @param[in]     pThis        Pointer to the QueueNotifying object.
* @param[in]     pItem        Pointer to item to put on Queue. Expected size if the size
*                             configured in the Queue passed on QueueNotifyingInit.
*/
EQueueNotifyingStatus
QueueNotifyingSendToBackFromISR(SQueueNotifying* pThis,
                                const uint8_t* pItem);

/**
* Adds item to front of queue and notifies receiver task.
*
* Mainly a wrapper method for FreeRTOS xQueueSendToFront().
* A notification is also sent to the receiver task.
* It is recommended to use QueueNotifyingSendToBack() instead, unless its a
* specific wish to put item in front of other items already on the queue.
*
* @param[in]     pThis        Pointer to the QueueNotifying object.
* @param[in]     pItem        Pointer to item to put on Queue. Expected size if the size
*                             configured in the Queue passed on QueueNotifyingInit.
* @param[in]     iTimeToWait  How many milliseconds to wait if queue is full. 0 means dont wait.
*/
EQueueNotifyingStatus QueueNotifyingSendToFront(SQueueNotifying* pThis, const uint8_t* pItem, uint32_t iTimeToWait);
/**
* @} // addtogroup QueueNotifying
* @} // addtogroup Queue
* @} // addtogroup Components
*/

#endif /* _QUEUENOTIFYING_H_ */
