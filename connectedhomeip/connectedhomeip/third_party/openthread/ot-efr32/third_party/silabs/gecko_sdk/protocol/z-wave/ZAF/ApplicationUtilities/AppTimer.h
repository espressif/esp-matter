/**
* @file
* @brief Application Timer module - Ensures timer callbacks are performed in the Application task.
* @details The module wraps a SwTimerLiaison and presents it as a "singleton", easing its
* availability among the Application modules.
*
* The modules basically wraps a SwTimerLiaison, owning the required objects
* as static data.
*
* The iTaskNotificationBitNumber (given at AppTimerInit) bit will be set as
* TaskNotification to the ReceiverTask (given at AppTimerInit) when a registered
* timer expires. When such a notification is set, the notification bit must be
* cleared and AppTimerNotificationHandler called.
*
* For detailed documentation see SwTimerLiaison.h
*
* @copyright 2018 Silicon Laboratories Inc.
*/

#ifndef _APPTIMER_H_
#define _APPTIMER_H_

#include <stdbool.h>
#include <SwTimer.h>
#include <ZW_basis_api.h>

/**
* Initialize AppTimer
*
* @param[in]     iTaskNotificationBitNumber Number defines which bit to use when notifying
*                                           receiver task of pending timer event (range 0 - 31)
* @param[in]     ReceiverTask               Handle to the Application task
*/
void AppTimerInit(uint8_t iTaskNotificationBitNumber, void * ReceiverTask);


/**
* Configures the Receiver task of callbacks.
*
* Provided as the AppTimer may be needed for registering SwTimers before
* the receiver task is created, meaning AppTimerInit is called prior to
* the Receiver task handle being available.
* Though not the intention, the receiver task can be changed at run time.
*
* @param[in]     ReceiverTask             Handle to the Application task
*/
void AppTimerSetReceiverTask(void * ReceiverTask);


/**
* Register a SwTimer to a TimerLiaison.
* Initial SwTimer configuration is also performed.
*
* Method creates a static FreeRTOS timer from the SwTimer object passed as argument,
* and registers it as requiring callbacks in the Receiver task.
* If calling method AppTimerRegister on the same SwTimer object multiple times,
* all but the first call will be ignored.
*
* @param[in]    pTimer        Pointer to the SwTimer object to register
* @param[in]    bAutoReload   Enable timer auto reload on timeout. Configuration of
*                             AutoReload cannot be changed after registration.
* @param[in]    pCallback     Callback method of type void Callback(SSwTimer* pTimer).
*                             Argument may be NULL, in which case no callback is performed.
* @retval       true          Timer successfully registered to AppTimer TimerLiaison.
* @retval       false         AppTimer is full and cannot register any more SwTimers.
*/
bool AppTimerRegister(
  SSwTimer* pTimer,
  bool bAutoReload,
  void(*pCallback)(SSwTimer* pTimer)
  );


/**
 * Wrapper for AppTimerRegister to register timers that should be re-loaded
 * after deep sleep hibernate.
 *
 * In addition to having AppTimerRegister() register the timer with a
 * TimerLiaison this function also marks the timer to survive deep sleep hibernate.
 *
 * For parameter descriptions @see AppTimerRegister.
 */
bool AppTimerDeepSleepPersistentRegister(SSwTimer *pTimer,
                                   bool bAutoReload,
                                   void (*pCallback)(SSwTimer *pTimer));


/**
 * Wrapper for TimerStart() that ensures the timer value is saved so it
 * can be re-loaded after deep sleep hibernate.
 *
 * For parameter descriptions @see TimerStart.
 */
ESwTimerStatus AppTimerDeepSleepPersistentStart(SSwTimer *pTimer, uint32_t iTimeout);


/**
 * Wrapper for TimerRestart() that ensures the timer status is saved so it
 * can be re-loaded after deep sleep hibernate.
 *
 * For parameter descriptions @see TimerRestart.
 */
ESwTimerStatus AppTimerDeepSleepPersistentRestart(SSwTimer *pTimer);


/**
 * Wrapper for TimerStop() that ensures the timer status is saved so it
 * does not get re-started after deep sleep hibernate.
 *
 * For parameter descriptions @see TimerStop.
 */
ESwTimerStatus AppTimerDeepSleepPersistentStop(SSwTimer *pTimer);


/**
* Must be called from application task when the task notification bit
* assigned to AppTimer is set.
*
* Method will perform pending callbacks. There is no side effects from calling
* method when assigned task notification bit was not set.
*/
void AppTimerNotificationHandler(void);


/**
 * Clear storage used for persisting application timers during deep sleep hibernate
 */
void AppTimerDeepSleepPersistentResetStorage(void);


/**
 * Save SSwTimers that should survive deep sleep Hibernate
 *
 * The number of remaining milliseconds until timeout are saved
 * to the RTCC retention registers for all application timers
 * registered to be persisted during Deep Sleep.
 */
void AppTimerDeepSleepPersistentSaveAll(void);


/**
 * Save the FreeRTOS task tick value just before going to sleep in
 * deep sleep hibernate
 *
 * The saved task tick value is used by AppTimerDeepSleepPersistentLoadAll() to
 * calculate the timeouts the application timers should be restarted
 * with.
 */
void AppTimerSaveTaskTickAtSleep(void);


/**
 * Load SSwTimers after deep sleep hibernate wakeup
 *
 * Activates callback registered for the timer causing the wakeup
 *
 * Must be called AFTER AppTimerInit() and AppTimerDeepSleepPersistentRegister()
 *
 * @param resetReason Reason why the device woke up
 */
void AppTimerDeepSleepPersistentLoadAll(EResetReason_t resetReason);


/**
 * Get index of first retention register used for saving Deep Sleep persistent app timers.
 *
 * The retention registers are manipulated with the zpal_retention_register_xx() API.
 */
uint32_t AppTimerGetFirstRetentionRegister(void);


/**
 * Get index of last retention register used for saving Deep Sleep persistent app timers.
 *
 * NB: The number of retention registers used depends on how many times
 *     AppTimerDeepSleepPersistentRegister() have been called since system startup.
 *     It is not safe to use the returned value to locate "free" retention registers
 *     unless it is guarantees that all calls to AppTimerDeepSleepPersistentRegister() has
 *     been performed.
 *
 * The retention registers are manipulated with the zpal_retention_register_xx() API.
 */
uint32_t AppTimerGetLastRetentionRegister(void);

/**
 * Stops all the running applications timers.
 *
 */ 
void AppTimerStopAll (void);
#endif /* _APPTIMER_H_ */
