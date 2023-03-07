/**
* @file
* Event Distributor module.
* The Event Distributor is a helper module for when using the FreeRTOS task
* notifications as an event group.
* The module can wait for FreeRTOS task notifications, and is based on each
* bit in the Notification value to represent an event. Each set bit in the 
* notification values corresponds to a position in a table of callback function
* pointers (event handlers), which are called when their notification bit is set.
* 
* An event handler table is passed to the module on init, the table positions
* each correspond to a notification bit as:
* bit 0 -> position 0 in handler table
* bit 1 -> position 1 in handler table
* ...
*
* if the Event handler table has less than 32 handlers, the remaining task
* notification bits will be ignored by the EventDistributor.
*
* The EventDistributor sleeps the task until a task notification arrives.
* If the notification value has any bits set, which is assigned to an event
* handler the event handler will be called.
* Once all pending events are handled the method returns the notification value
* with all bits representing handled events cleared.
* This allows handling the remaining notification bits in a custom way.
* If multiple events are pending the first event table entries will be handled
* first.
*
* All notification bits representing events are cleared on exiting  
* "Wait for event", the caller can pass an additional bit mask (NotificationClearMask),
* specifying bits to be cleared along with the event bits. Set bits of the mask
* will be cleared in the notification value.
*
* A specific callback(pNoEvent) is called if the Wait for event times out or if a
* notification is received but no events bits are set.
*
* The simplest usage is just to setup an event handler table, pass it to 
* EventDistributor and run EventDistributor in an endless loop.
*
* @copyright 2018 Silicon Laboratories Inc.
*/

#ifndef _EVENTDISTRIBUTOR_H_
#define _EVENTDISTRIBUTOR_H_

#include <stdint.h>
/**
* @addtogroup Components
* @{
* @addtogroup Events
* @{
* @addtogroup EventDistributor
* @{
 */

/**
* Defines return values from EventDistributorConfigure.
*/
typedef enum EEventDistributorStatus
{
  EEVENTDISTRIBUTOR_STATUS_SUCCESS = 0,
  EEVENTDISTRIBUTOR_STATUS_TABLE_TOO_LARGE
} EEventDistributorStatus;


/**
* Defines callback pointer for Event handler.
* Typedef to simplify syntax.
*/
typedef void (*EventDistributorEventHandler)(void);


/**
* EventDistributor object. All content is private.
*/
typedef struct SEventDistributor
{
  uint8_t iEventHandlerTableSize;       /**< Number of entries in pEventHandlerTable */
  const EventDistributorEventHandler* pEventHandlerTable;   /**< pointer to const array of Callback 
                                                                 function pointers - Event handlers*/
  void(*pNoEvent)(void);                /**< Callback to handler of Notification with no event
                                             and wait for event timeout  */
} SEventDistributor;


/**
* Configures the EventDistributor.
* Configure must be called before calling any other EventDistributor methods.
* EventDistributor supports reconfiguration at all times.
* 
* @param[in]  pThis                   Pointer to the EventDistributor object.
* @param[in]  iEventHandlerTableSize  Number of entries in EventHandlerTable
*                                     pointed to by argument pEventHandlerTable
*                                     Table size range is 0 - 32)
* @param[in]  pEventHandlerTable      Pointer to table of EventHandler function pointers
* @param[in]  pNoEvent                Function pointer that will be called on notifications
*                                     without event or wait for event timeout
* @retval     EEVENTDISTRIBUTOR_STATUS_SUCCESS          Successfull configuration.
* @retval     EEVENTDISTRIBUTOR_STATUS_TABLE_TOO_LARGE  Event handler table too large.
*/
EEventDistributorStatus EventDistributorConfig(
                                            SEventDistributor* pThis,
                                            uint8_t iEventHandlerTableSize,
                                            const EventDistributorEventHandler* pEventHandlerTable,
                                            void(*pNoEvent)(void)
                                          );


/**
* Waits for Task notification and calls event handlers matching the set
* notification bits. Method returns when a notification has arrived (or
* waiting timed out) and any pending events has been handled.
*
* @param[in]    pThis       Pointer to the EventDistributor object
* @param[in]    iEventWait  Timeout (in mS) when waiting for event. Value must NOT be zero.
* @param[in]    NotificationClearMask   Mask of additional notification bits to clear,
*                                       which is not handled by EventDistributor.
*                                       EventDistributor will always clear notification
*                                       bits assigned to events. This mask is additional
*                                       bits to clear.
* @return       uint32_t    Task notification value. All bits handled
*                           by EventDistributor are set to zero.
*/
uint32_t EventDistributorDistribute(
                                    const SEventDistributor* pThis,
                                    uint32_t iEventWait,
                                    uint32_t NotificationClearMask
                                  );
/**
* @} // addtogroup EventDistributor
* @} // addtogroup Events
* @} // addtogroup Components
*/
#endif /* _EVENTDISTRIBUTOR_H_ */
