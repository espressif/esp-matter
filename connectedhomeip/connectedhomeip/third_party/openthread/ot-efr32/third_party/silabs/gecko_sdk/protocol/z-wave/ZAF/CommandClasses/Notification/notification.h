/**
 * @file
 * Command Class Notification helper module.
 *
 * How to use Notification module:
 *
 * Current version cannot queue up more events, but it is possible to implement a queue in current
 * design.
 *
 * Initialization:
 * 1. Call constructor InitNotification(zpal_nvm_handle_t pFS)
 * 2. Add application Notification types and events to the module by calling
 *    AddNotification(...)
 *
 * Trigger event:
 * 1.  First trigger an event on Communication Module by calling
 *     NotificationEventTrigger (..) and then call AGI_NodeIdListInit (..) to
 *     trig profile-event at AGI.
 * 2. AGI now sends an unsolicited events to all nodes. It has the knowledge Command class and
 *    and command (NextUnsolicitedEvent).
 * 3. Finally notification-type/event must to be cleared from the queue. This
 *    done by calling ClearLastNotificationAction(). ClearLastNotificationAction ()
 *    is called when it's done with the unsolicited event jobs.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _NOTIFICATION_H_
#define _NOTIFICATION_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_typedefs.h>
#include <CC_Notification.h>
#include <agi.h>
#include <config_app.h>
#include <zpal_nvm.h>
/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

// Used by the application file system.
typedef struct SNotificationData
{
  uint8_t AlarmStatus[MAX_NUM_OF_NOTIFICATION_GROUPS];
} SNotificationData;

#define ZAF_FILE_SIZE_NOTIFICATIONDATA     (sizeof(SNotificationData))

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/


/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/



/**
 * @brief Init notification module
 */
void InitNotification(zpal_nvm_handle_t pFS);


/**
 * @brief Set default notification status if it is active or deactive.
 * @param status of type NOTIFICATION_STATUS
 */
void DefaultNotificationStatus(NOTIFICATION_STATUS status);


/**
 * @brief Configure notification event linked to a specific AGI profile. Notification module can
 * handle notification event as a state. Example setting up notification = Power management
 * (0x08), event = Over-load detected (0x08) and stateless = false. If the event is triggered
 * Notification module remeber the state that over-load is active until application trigger event
 * notification = 0x08, event = 0x00, eventPar = Over-load detected (0x08) that clear the state.
 * @param pAgiProfile of type AGI_PROFILE is used to link notification to AGI event.
 * @param type of type NOTIFICATION_TYPE
 * @param pSuppportedEvents is notification event list for the specific notification type.
 * @param suppportedEventsLen is length of the list
 * @param stateless define current notification has a state or is stateless.
 * If stateless is false do Notification-module remember last event until it is disabled.
 * @param endpoint setting up the notification for a specific endpoint.
 * @param defaultNotificationStatus enable/disable notifications by default.
 * @param fileAlreadySet do not save defaultNotificationStatus to file if true==fileAlreadySet
 * @return true if success else false.
 */
bool AddNotification( AGI_PROFILE const * const pAgiProfile,
  notification_type_t type,
  uint8_t * pSuppportedEvents,
  uint8_t suppportedEventsLen,
  bool stateless,
  uint8_t endpoint,
  NOTIFICATION_STATUS defaultNotificationStatus,
  bool fileAlreadySet);

/**
 * @brief Add event on queue (queue size 1)
 * @param[in] pAgiProfile of type AGI_PROFILE is used to link notification to AGI event.
 * @param[in] type notification type.
 * @param[in] notificationEvent notification event.
 * @param[in] pEvPar point to event parameters.
 * @param[in] evParLen length of event parameters.
 * @param[in] sourceEndpoint source endpoint
 */
void NotificationEventTrigger(
    AGI_PROFILE const * const pAgiProfile,
	notification_type_t type,
    uint8_t notificationEvent,
    uint8_t * pEvPar,
    uint8_t evParLen,
    uint8_t sourceEndpoint);

/**
 * @brief Send unsolicited notification event to node in pnList.
 * @param[in] pProfile pointer to AGI profile
 * @param[in] sourceEndpoint source endpoint
 * @param[out] pCallback callback function returning state on job
 * @return JOB_STATUS
 */
JOB_STATUS UnsolicitedNotificationAction(
  AGI_PROFILE const * const pProfile,
  uint8_t sourceEndpoint,
  VOID_CALLBACKFUNC(pCallback)(TRANSMISSION_RESULT * pTransmissionResult));


/**
 * @brief Clear event on queue
 * @param[in] pAgiProfile pointer to AGI profile
 * @param[in] sourceEndpoint source endpoint
 */
void ClearLastNotificationAction(AGI_PROFILE const * const pAgiProfile, uint8_t sourceEndpoint);


#endif /* _NOTIFICATION_H_ */


