/**
 * @file
 * Handler for Command Class Wake Up.
 *
 * CC WakeUp can be used in two scenarios:
 * 1) as a standalone wake up node waking up from reset, or
 * 2) combined with FLiRS waking up from sleep.
 *
 * In both scenarios, an application might want to do something, like sending a Battery Report
 * before transmitting a Wake Up Notification.
 *
 * Having an init function that takes a reset reason and a callback function, the module could
 * address both scenarios like this:
 *
 * 1) The init function is called with a reset reason set to ERESETREASON_DEEP_SLEEP_WUT. This means
 * that the chip woke up from reset caused by the wake up timer. In this case, the callback must be
 * invoked so that the application can do something.
 *
 * 2) The callback passed using the init function is registered with the timer. It is then invoked
 * when the timer triggers.
 *
 * In both scenarios, the application must call CC_WakeUp_notification_tx() when done with
 * something.
 *
 * General initialization:
 * 1) Call init and set values
 * 2) On resetting app: Call reset function
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _COMMANDCLASSWAKEUP_H_
#define _COMMANDCLASSWAKEUP_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <ZW_TransportEndpoint.h>
#include <ZW_application_transport_interface.h>
#include <zpal_nvm.h>

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/**
 * Wakeup parameter types
 */
typedef enum _WAKEUP_PAR_
{
  WAKEUP_PAR_SLEEP_STEP,
  WAKEUP_PAR_MIN_SLEEP_TIME,
  WAKEUP_PAR_MAX_SLEEP_TIME,
  WAKEUP_PAR_DEFAULT_SLEEP_TIME,
  WAKEUP_PAR_COUNT
}
wakeup_parameter_t;

/**
 * For backwards compatibility.
 */
#define WakeUpNotification(a) CC_WakeUp_notification_tx(a)
#define SetWakeUpConfiguration(a,b) CC_WakeUp_setConfiguration(a,b)
#define CmdClassWakeUpNotificationMemorySetDefault(a) CC_WakeUp_notificationMemorySetDefault(a)
#define handleWakeUpIntervalGet() CC_WakeUp_intervalGet_handler()
#define WakeUpCC_StayAwakeIfActive() CC_WakeUp_stayAwakeIfActive()

typedef wakeup_parameter_t WAKEUP_PAR;


/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Used by application file system
typedef struct SWakeupCcData
{
  uint8_t MasterNodeId;
  uint32_t SleepPeriod;
} SWakeupCcData;

#define ZAF_FILE_SIZE_WAKEUPCCDATA        (sizeof(SWakeupCcData))


/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * Initialize the WakeUp Command class with a file ID for the Wake Up Command Class data.
 * @param[in] resetReason reset Reason
 * @param[in] pFS Pointer to Application File System
 */
void CC_WakeUp_init(EResetReason_t resetReason,
                    zpal_nvm_handle_t pFS);

/**
 * Transmits a Wake Up Notification command and handles callback.
 * @param[in] pCallback Pointer to function which will be called on Wake Up Notification ACK/NACK.
 */
void CC_WakeUp_notification_tx(void (*pCallback)(uint8_t txStatus, TX_STATUS_TYPE* pExtendedTxStatus));

/**
 * Set up the parameters defined in wakeup_parameter_t. The values are given in seconds.
 * @param[in] type Wake up parameter.
 * @param[in] time Time in seconds.
 */
void CC_WakeUp_setConfiguration(
    wakeup_parameter_t type,
    uint32_t time);

/**
 * Resets the saved node ID in NVM.
 */

void CC_WakeUp_notificationMemorySetDefault(zpal_nvm_handle_t pFS);

/**
 * Ensure that the node stays awake to wait for more messages from the
 * wake up destination node.
 *
 * Should be called to ensure that the node stays awake the next 10 seconds if
 * it is active because it has woken up and notified the wake up destination
 * node. (the wake up destination node can cancel the 10 second sleep delay
 * by sending a WAKE_UP_NO_MORE_INFORMATION)
 */
void
CC_WakeUp_stayAwakeIfActive(void);


/**
 * Ensures that the node stays awake for 10 seconds
 */
void
CC_WakeUp_stayAwake10s(void);

/**
 * Start the timer for sending wakeup notifications.
 *
 * This function doesn't differentiate whether the timer is already
 * running or not. In either case the timer is reset and started with
 * the current timeout configuration.
 */
void CC_WakeUp_startWakeUpNotificationTimer(void);

/**
 * Tell the device to auto extend the 10 second timer following an inclusion
 *
 * Every accepted application frame sent or received will normally cause a
 * device to stays awake for 10 seconds more when it has been woken up.
 *
 * Since entering learn mode for a not-included node is not triggering the
 * usual wakeup logic this function must be called to tell the wakeup CC to
 * extend the awake period in this case also.
 *
 * NB: A function to turn this special mode off is not needed. When we
 *     finally fall asleep after learn mode has completed we will go to
 *     deep sleep. Next wakeup will therefore be a reset where this mode by
 *     default is off.
 */
void CC_WakeUp_AutoStayAwakeAfterInclusion(void);

#endif /* _COMMANDCLASSWAKEUP_H_ */
