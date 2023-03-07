/**
* @file
* Software timer module.
* @copyright 2018 Silicon Laboratories Inc.
*
* @details The module exposes basic software timer functionality.
*
* The modules basically wraps static FreeRTOS timers, but provides additional
* functionality as it allows the SwTimerLiaison module to ensure that the
* callback is done in a specific task context.
* The module is heavily dependant on the SwTimerLiaison module, which passes
* the callback to the correct receiver task.
* Each SwTimer must have (be registered to) a SwTimerLiaison to correctly
* perform the timeout callbacks. One SwTimerLiaison can perform this task
* for multiple timers.
* The SwTimer and SwTimerLiaison modules are split up to provide a simpler
* interface for modules that just use a timer, but does not create it or
* have any interest in which task context the callback is performed in.
*
* The SwTimer contains a FreeRTOS static timer, and thus
* uses no FreeRTOS heap.
*
* @note No methods of SwTimer may be called on an SwTimer object that is not
* registered with a SwTimerLiaison.
*
* For more detailed documentation see @ref SwTimerLiaison.h
*
*/

#ifndef _SWTIMER_H_
#define _SWTIMER_H_

#include <stdbool.h>
#include <stdint.h>

/**
* @addtogroup Components
* @{
* @addtogroup Timer
* @{
* @addtogroup SwTimer
* @{
*/

/**
* Defines return values from SwTimer methods.
*/
typedef enum ESwTimerStatus
{
  ESWTIMER_STATUS_SUCCESS = 0,//!< ESWTIMER_STATUS_SUCCESS
  ESWTIMER_STATUS_FAILED = 1  //!< ESWTIMER_STATUS_FAILED
} ESwTimerStatus;

/**
 * Space for holding FreeRTOS StaticEventGroup_t and EventGroupHandle_t
 */
#define PRIVATE_TIMER_SIZE             11

/**
* Forward declaration of SSwTimer to allow SSwTimer to contain
* a callback function pointer with its own type a Argument
*/
struct SSwTimer;

/**
* Typedef needed to include pointer for struct SSwTimerLiaison in struct
* SSwTimer, without requiring include of definition SSwTimerLiaison
*/
typedef struct SSwTimerLiaison* SSwTimerLiaisonPtr;

typedef struct SSwTimer
{
  uint32_t dummy[PRIVATE_TIMER_SIZE];
  void* TimerHandle;                    /**< FreeRTOS timer handle */
  uint8_t Id;                           /**< Identifies timer to SwTimerLiaison */
  void* ptr;                            /**< User pointer, which is free to be used. */
  SSwTimerLiaisonPtr pLiaison;          /**< Pointer to TimerLiaison object which
                                             timer is registered to */
  void(* pCallback)( struct SSwTimer * pTimer); /**< Callback function pointer */
} SSwTimer;

/**
* Configures the callback method called on timer timeout.
* If called on a timer that has a pending callback, the callback will be executed
* on the new callback method.
* @param[in]     pTimer     Pointer to the timer object
* @param[in]     pCallback  Callback method of type void Callback(SSwTimer* pTimer).
*                           Argument may be NULL, in which case no callback is performed.
*/
void TimerSetCallback(SSwTimer* pTimer, void(*pCallback)(SSwTimer* pTimer));

/**
* Configures Timer timeout and starts the timer.
* If method is called on a timer that is already started, the timer will restart
* from zero and count towards the new timeout.
*
* @param[in]    pTimer                   Pointer to the timer object
* @param[in]    iTimeout                 Timeout value in ms. Zero is illegal.
* @retval       ESWTIMER_STATUS_SUCCESS  Timer started Successfully.
* @retval       ESWTIMER_STATUS_FAILED   Never returned under normal circumstances.
*                                        Can be returned e.g. if OS is not started yet.
*/
ESwTimerStatus TimerStart(SSwTimer* pTimer, uint32_t iTimeout);

/**
* Configures Timer timeout and starts the timer from an Interrupt Service Routine.
* If method is called on a timer that is already started, the timer will restart
* from zero and count towards the new timeout.
*
* @param[in]    pTimer                   Pointer to the timer object
* @param[in]    iTimeout                 Timeout value in ms. Zero is illegal.
* @retval       ESWTIMER_STATUS_SUCCESS  Timer started Successfully.
* @retval       ESWTIMER_STATUS_FAILED   Never returned under normal circumstances.
*                                        Can be returned e.g. if OS is not started yet.
*/
ESwTimerStatus TimerStartFromISR(SSwTimer* pTimer, uint32_t iTimeout);

/**
* Restarts the timer.
* Starts an inactive timer.
* If called on a timer that is already started, the timer will restart from zero.
*
* @param[in]    pTimer                   Pointer to the timer object
* @retval       ESWTIMER_STATUS_SUCCESS  Timer started Successfully.
* @retval       ESWTIMER_STATUS_FAILED   Never returned under normal circumstances.
*                                        Can be returned e.g. if OS is not started yet.
*/
ESwTimerStatus TimerRestart(SSwTimer* pTimer);

/**
* Restarts the timer.
* Starts an inactive timer.
* If called on a timer that is already started, the timer will restart from zero.
*
* @param[in]    pTimer                   Pointer to the timer object
* @retval       ESWTIMER_STATUS_SUCCESS  Timer started Successfully.
* @retval       ESWTIMER_STATUS_FAILED   Never returned under normal circumstances.
*                                        Can be returned e.g. if OS is not started yet.
*/
ESwTimerStatus TimerRestartFromISR(SSwTimer* pTimer);

/**
* Stops an active timer from an Interrupt Service Routine.
* Method may be called on inactive timers.
* Any pending callbacks that has not been performed yet will be cleared.
*
* @param[in]    pTimer                   Pointer to the timer object
* @retval       ESWTIMER_STATUS_SUCCESS  Timer stopped.
* @retval       ESWTIMER_STATUS_FAILED   Never returned under normal circumstances.
*                                        Can be returned e.g. if OS is not started yet.
*/
ESwTimerStatus TimerStop(SSwTimer* pTimer);

/**
* Stops an active timer from an Interrupt Service Routine.
* Method may be called on inactive timers.
* Any pending callbacks that has not been performed yet will be cleared.
*
* @param[in]    pTimer                   Pointer to the timer object
* @retval       ESWTIMER_STATUS_SUCCESS  Timer stopped.
* @retval       ESWTIMER_STATUS_FAILED   Never returned under normal circumstances.
*                                        Can be returned e.g. if OS is not started yet.
*/
ESwTimerStatus TimerStopFromISR(SSwTimer* pTimer);

/**
* Returns if the timer is active.
* Active is defined as the timer counting, or having a pending callback.
*
* @param[in]    pTimer    Pointer to the timer object
* @retval       true      Indicates that timer is active.
* @retval       false     Indicatas that timer is inactive.
*/
bool TimerIsActive(SSwTimer* pTimer);

/**
* Returns if timer has a timeout callback pending.
*
* @param[in]    pTimer    Pointer to the timer object
* @retval       true      Indicates that timer has a callback pending.
* @retval       false     Indicatas that timer does NOT have a callback pending.
*/
bool TimerHasPendingCallback(SSwTimer* pTimer);

/**
* Returns if timer has a timeout callback pending from an Interrupt Service Routine.
*
* @param[in]    pTimer    Pointer to the timer object
* @retval       true      Indicates that timer has a callback pending.
* @retval       false     Indicatas that timer does NOT have a callback pending.
*/
bool TimerHasPendingCallbackFromISR(SSwTimer* pTimer);


/**
* Queries a timer for number of milliseconds remaining before time out
* Method may be called on inactive timers, but in that case ESWTIMER_STATUS_FAILED
* will be returned and pMsUntilTimeout will not be modified.
*
* @param[in]  pTimer                  Pointer to the timer object
* @param[in]  refTaskTickCount        The xTaskGetTickCount() value to use as a reference for the timeout.
* @param[out] pMsUntilTimeout         Pointer to variable to receive remaining time until timeout
* @retval     ESWTIMER_STATUS_SUCCESS Timer was active. Remaining ms written to pMsUntilTimeout.
* @retval     ESWTIMER_STATUS_FAILED  Timer was not active. pMsUntilTimeout not modified.
*/
ESwTimerStatus TimerGetMsUntilTimeout(SSwTimer* pTimer, uint32_t refTaskTickCount, uint32_t *pMsUntilTimeout);

/**
* Returns the period of a freeRTOS timer.
* @param[in]  pTimer                  Pointer to the timer object
* @retval     uint32_t                The period of the timer in ticks.
*/
uint32_t TimerGetPeriod(SSwTimer *pTimer);

/**
 * @} // addtogroup SwTimer
 * @} // addtogroup Timer
 * @} // addtogroup Components
 */

#endif /* _SWTIMER_H_ */
