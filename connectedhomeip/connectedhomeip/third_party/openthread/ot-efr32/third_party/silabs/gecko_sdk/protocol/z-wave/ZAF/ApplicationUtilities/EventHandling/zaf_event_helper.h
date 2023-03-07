/**
 * @file
 * Wraps the QueueNotifyingSendToBack function to achieve a simpler API for events in the
 * application.
 * @copyright 2019 Silicon Laboratories Inc.
 */

#ifndef ZAF_APPLICATIONUTILITIES_EVENT_HELPER_H_
#define ZAF_APPLICATIONUTILITIES_EVENT_HELPER_H_

#include <stdbool.h>
#include "QueueNotifying.h"
/**
* @addtogroup Components
* @{
* @addtogroup Events
* @{
* @addtogroup EventHandling
* @{
 */

/**
 * Initializes the Event Helper.
 * @param pQueueNotifyingHandle Handler to the Queue Notifying object used by the application.
 */
void ZAF_EventHelperInit(SQueueNotifying * pQueueNotifyingHandle);

/**
 * @brief Checks to see whether the module is initialized and ready!
 * @return true if initialized and ready!
 */
bool ZAF_EventHelperIsInitialized(void);

/**
 * Enqueues an event in the queue that the Event Helper was initialized with.
 * @param event The event to enqueue.
 * @return Returns true if the event was enqueued and false otherwise.
 */
bool ZAF_EventHelperEventEnqueue(const uint8_t event);

/**
 * Enqueues an event in the queue that the Event Helper was initialized with.
 * @param event The event to enqueue.
 * @return Returns true if the event was enqueued and false otherwise.
 */
bool ZAF_EventHelperEventEnqueueFromISR(const uint8_t event);
/**
* @} // addtogroup EventHandling
* @} // addtogroup Events
* @} // addtogroup Components
*/
#endif /* ZAF_APPLICATIONUTILITIES_EVENT_HELPER_H_ */
