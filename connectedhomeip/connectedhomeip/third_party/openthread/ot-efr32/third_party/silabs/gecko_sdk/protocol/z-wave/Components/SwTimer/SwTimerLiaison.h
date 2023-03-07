/**
* @file
* @brief Software timer liaison module.
* @copyright 2018 Silicon Laboratories Inc.
*
* @details The module ensures that the @ref SwTimer.h callbacks are performed
* in the correct task context.
*
* The SwTimerLiaison is a service module for the SwTimers, performing time
* out callbacks in the correct task context.
* Each SwTimer must have (be registered to) a SwTimerLiaison to correctly
* perform the timeout callbacks. One SwTimerLiaison can perform this task
* for multiple timers.
*
* A SwTimerLiaison can only pass callbacks to one specific task.
*
* The SwTimer and SwTimerLiaison modules are split up to provide a simpler
* interface for modules that just use a timer, but does not create it or
* have any interest in which task context the callback is performed in.
*
* The SwTimer encapsulates a FreeRTOS timer, adding an additional callback
* pointer, a SwTimerLiaison pointer and an ID for the SwTimerLiaison to keep
* track of timers.
*
* The SwTimerLiaison contains a Task handle and a list of registered timers.
* The list of timers can be no larger than 24 (Limited by FreeRTOS event
* group limitations).
*
* FreeRTOS timers performs callbacks in the timer daemon task, which means the
* programmer has to be carefull what he does, at its in a special task.
* The SwTimer and SwTimerLiaison moves the callback to a task specified by the 
* programmer.
* 
* The SwTimerLiaison uses the FreeRTOS timer callback to call a SwTimerLiaison
* method on Timer timeout. The SwTimerLiaison then uses the callback to set
* a FreeRTOS event group bit matching the TimerID and sending a Task
* notification to the receiving task.
* The receiving task must call the TimerLiaisonNotificationHandler when the
* task notification is received (implemented outside of the 
* SwTimer/SwTimerLiaison components). The TimerLiaison (now out of FreeRTOS 
* timer daemon task context and in correct receiver task context) checks which
* timers has a pending callback by reading the event group notification set
* during the FreeRTOS callback. The SwTimerLiaison then performs the callbacks
* of the timers which has pending callbacks.
* 
* The SwTimerLiaison contains a FreeRTOS static event group, and thus
* uses no FreeRTOS heap.
* 
* @startuml
* title SwTimer / SwTimerLiaison  usage sequence
* participant TimerUser
* participant SwTimerLiaison
* participant SwTimer
* participant FreeRTOS_Timer
* participant ReceiverTask
* == Initialization ==
* group Receiver task context
*   TimerUser->SwTimerLiaison: Register SwTimer
*   SwTimerLiaison->FreeRTOS_Timer: Create FreeRTOS timer
*   SwTimerLiaison->SwTimer: Setup SwTimer
* end
* == Usage ==
* group Receiver task context
*   TimerUser->SwTimer: Start Timer
*   SwTimer->FreeRTOS_Timer: Start FreeRTOS timer
* end
* ...Time passes until timer expires...
* group FreeRTOS timer daemon task context
*   FreeRTOS_Timer->SwTimerLiaison: Expired callback (to TimerExpiredLiaisonCallback)
*   SwTimerLiaison->ReceiverTask: Task Notification
* end
* group Receiver task context
* ReceiverTask->SwTimerLiaison: SwTimerLiaisonNotificationHandler
* SwTimerLiaison->SwTimer: Look up callback
* SwTimerLiaison->TimerUser: Perform callback
* end
* @enduml
*
*/

#ifndef _SWTIMERLIAISON_H_
#define _SWTIMERLIAISON_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

/**
* @addtogroup Components
* @{
* @addtogroup Timer
* @{
* @addtogroup SwTimerLiasion
* @{
*/

/**
* Defines return values from SwTimerLiaison methods.
*/
typedef enum ESwTimerLiaisonStatus
{
  ESWTIMERLIAISON_STATUS_SUCCESS = 0,       //!< ESWTIMERLIAISON_STATUS_SUCCESS
  ESWTIMERLIAISON_STATUS_LIST_FULL,         //!< ESWTIMERLIAISON_STATUS_LIST_FULL
  ESWTIMERLIAISON_STATUS_ALREADY_REGISTRERED//!< ESWTIMERLIAISON_STATUS_ALREADY_REGISTRERED
} ESwTimerLiaisonStatus;

/**
 * Space for holding FreeRTOS StaticTimer_t
 */
#define PRIVATE_STATIC_TIMER_SIZE      9

/**
* Forward declaration needed to include pointer for struct SSwTimer in struct
* SSwTimerLiaison, without requiring include of definition SSwTimer
*/
struct SSwTimer;

/**
* TimerLiaison object. All content is private.
*/
typedef struct  SSwTimerLiaison
{
  uint32_t dummy[PRIVATE_STATIC_TIMER_SIZE];
  uint8_t iTimerCount;                /**< Keeps count of added timers */
  uint8_t iTimerListSize;             /**< Size of pTimerList */
  struct SSwTimer** pTimerList;       /**< Pointer to array of pointers to SwTimer objects */
  uint8_t iTaskNotificationBitNumber; /**< which bit to use when notifying receiver
                                           task of pending timer event(range 0 - 31) */
  void* ReceiverTask;                 /**< Handle to task in which callbacks
                                           should be performed */
} SSwTimerLiaison;


/**
* Initialize TimerLiaison
*
* @param[in]     pThis                    Pointer to the TimerLiaison object
* @param[in]     iTimerPointerArraySize   Number of entries in provided SwTimer pointer
*                                         array (range 0 - 24)
* @param[in]     pTimerPointerArray       Pointer to provided array of pointers to SwTimer objects
* @param[in]     iTaskNotificationBitNumber Number defines which bit to use when notifying
*                                           receiver task of pending timer event (range 0 - 31)
* @param[in]     ReceiverTask             Handle to FreeRTOS task that Timer callbacks Will be
*                                         performed in. Argument may be NULL if task handle
*                                         is not available at Liaison init time, but ReceiverTask
*                                         must then be set with TimerLiaisonSetReceiverTask before
*                                         any registered Timer times out.
*/
void TimerLiaisonInit(
  SSwTimerLiaison* pThis,
  uint32_t iTimerPointerArraySize,
  SSwTimer** pTimerPointerArray,
  uint8_t iTaskNotificationBitNumber,
  void* ReceiverTask
  );


/**
* Configures the Receiver task of callbacks.
* 
* Provided as the TimerLiaison may be needed for registrering SwTimers before
* the receiver task is created, meaning TimerLiaision init is called prior to
* the Receiver task handle being available.
* Though not the intention, the receiver task can be changed run time.
*
* @param[in]     pThis                    Pointer to the TimerLiaison object
* @param[in]     ReceiverTask             Handle to FreeRTOS task that Timer callbacks
* will be performed in.
*/
void TimerLiaisonSetReceiverTask(SSwTimerLiaison* pThis, void* ReceiverTask);


/**
* Register a SwTimer to a TimerLiaison.
* Initial SwTimer configuration is also performed.
* 
* Method creates a static FreeRTOS timer from the SwTimer object passed as argument.
* An SwTimer can NOT be registered to several TimerLiaisons.
* If calling method TimerLiaisonRegister on the same SwTimer object multiple times,
* all but the first call will be ignored.
* Unregistered timers are expected to be zero initialized SSwTimer structs.
*
* @param[in]    pThis           Pointer to the TimerLiaison object
* @param[in]    pTimer          Pointer to the SwTimer object to register
* @param[in]    bAutoReload     Enable timer auto reload on timeout. Configuration of
*                               AutoReload cannot be changed after registration.
* @param[in]     pCallback      Callback method of type void Callback(SSwTimer* pTimer).
*                               Argument may be NULL, in which case no callback is performed.
* @retval       ESWTIMERLIAISON_STATUS_SUCCESS    Timer successfully registered to Liaison
* @retval       ESWTIMERLIAISON_STATUS_LIST_FULL  Liaison is full and cannot register
*                                                 any more SwTimers.
*/
ESwTimerLiaisonStatus TimerLiaisonRegister(SSwTimerLiaison* pThis,
  SSwTimer* pTimer,
  bool bAutoReload,
  void(*pCallback)(SSwTimer* pTimer)
  );


/**
* Must be called from requested callback task when the task notification bit
* assigned to TimerLiaison is set.
*
* Method will perform pending callbacks. There is no side effects from calling 
* method when assigned task notification bit was not set.
*
* @param[in]     pThis                    Pointer to the TimerLiaison object
*/
void TimerLiaisonNotificationHandler(SSwTimerLiaison* pThis);


/**
* Method provided for the SwTimers to call.
*
* Method clears pending timeout callback (if one is pending).
*
* @param[in]     pThis                    Pointer to the TimerLiaison object
* @param[in]     TimerId                  Which timer to clear pending events of.
*/
void TimerLiaisonClearPendingTimerEvent(SSwTimerLiaison* pThis, uint32_t TimerId);

/**
* Method provided for the SwTimers to call from ISR.
*
* Method clears pending timeout callback (if one is pending).
*
* @param[in]     pThis                    Pointer to the TimerLiaison object
* @param[in]     TimerId                  Which timer to clear pending events of.
*/
void TimerLiaisonClearPendingTimerEventFromISR(SSwTimerLiaison* pThis, uint32_t TimerId);

/**
* Method provided for the SwTimers to call.
*
* Method returns if a timer has a pending timeout callback.
*
* @param[in]     pThis                    Pointer to the TimerLiaison object
* @param[in]     TimerId                  Which timer to check for pending
*                                         callback.
*/
bool TimerLiaisonHasPendingTimerEvent(SSwTimerLiaison* pThis, uint32_t TimerId);


/**
* Method provided for the SwTimers to call from ISR.
*
* Method returns if a timer has a pending timeout callback.
*
* @param[in]     pThis                    Pointer to the TimerLiaison object
* @param[in]     TimerId                  Which timer to check for pending
*                                         callback.
*/
bool TimerLiaisonHasPendingTimerEventFromISR(SSwTimerLiaison* pThis, uint32_t TimerId);
/**
 * @} // addtogroup SwTimerLiasions
 * @} // addtogroup SwTimer
 * @} // addtogroup Components
 */

/**
* Callback method for the FreeRTOS timers wrapped in SwTimer.
* Method set event group bit matching TimerId and notifies receiver task.
*
* @param[in]     pTimer pointer to the timer object that timed out
*/
void TimerLiaisonExpiredTimerCallback(SSwTimer* pTimer);

#endif /* SWTIMERLIAISON */
