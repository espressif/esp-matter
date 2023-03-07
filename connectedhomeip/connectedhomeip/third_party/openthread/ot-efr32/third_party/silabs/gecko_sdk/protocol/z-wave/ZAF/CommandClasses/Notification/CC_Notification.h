/**
 * @file
 * Handler for Command Class Notification.
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _CC_NOTIFICATION_H_
#define _CC_NOTIFICATION_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_classcmd.h>
#include <CC_Common.h>
#include <agi.h>

/**
 *  the maximum length of the notification bitmask array
 */
#define NOTIFICATION_BITMASK_ARRAY_LENGTH   32
/**
 * Notification type (8 bit).
 */
typedef enum
{
  NOTIFICATION_TYPE_NONE,
  NOTIFICATION_TYPE_SMOKE_ALARM = (0xFF & ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_SMOKE_ALARM),
  NOTIFICATION_TYPE_CO_ALARM = (0xFF & ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_CO_ALARM),
  NOTIFICATION_TYPE_CO2_ALARM = (0xFF & ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_CO2_ALARM),
  NOTIFICATION_TYPE_HEAT_ALARM = (0xFF & ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_HEAT_ALARM),
  NOTIFICATION_TYPE_WATER_ALARM = (0xFF & ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_WATER_ALARM),
  NOTIFICATION_TYPE_ACCESS_CONTROL = (0xFF & ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_ACCESS_CONTROL),
  NOTIFICATION_TYPE_HOME_SECURITY = (0xFF & ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_HOME_SECURITY),
  NOTIFICATION_TYPE_POWER_MANAGEMENT = (0xFF & ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_POWER_MANAGEMENT),
  NOTIFICATION_TYPE_SYSTEM = (0xFF & ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_SYSTEM),
  NOTIFICATION_TYPE_EMERGENCY_ALARM = (0xFF & ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_EMERGENCY_ALARM),
  NOTIFICATION_TYPE_CLOCK = (0xFF & ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_CLOCK),
  NOTIFICATION_TYPE_MULTIDEVICE = (0xFF & ICON_TYPE_SPECIFIC_SENSOR_NOTIFICATION_MULTIDEVICE)
} notification_type_t;


/**
 * Notification event (8 bit) for notification type NOTIFICATION_TYPE_HOME_SECURITY.
 */
typedef enum
{
  NOTIFICATION_EVENT_HOME_SECURITY_NO_EVENT,
  NOTIFICATION_EVENT_HOME_SECURITY_INTRUSION,
  NOTIFICATION_EVENT_HOME_SECURITY_INTRUSION_UNKNOWN_EV,
  NOTIFICATION_EVENT_HOME_SECURITY_TAMPERING_COVERING_REMOVED,
  NOTIFICATION_EVENT_HOME_SECURITY_TAMPERING_INVALID_CODE,
  NOTIFICATION_EVENT_HOME_SECURITY_GLASS_BREAKAGE,
  NOTIFICATION_EVENT_HOME_SECURITY_GLASS_BREAKAGE_UNKNOWN_LOCATION,
  NOTIFICATION_EVENT_HOME_SECURITY_MOTION_DETECTION,
  NOTIFICATION_EVENT_HOME_SECURITY_MOTION_DETECTION_UNKNOWN_LOCATION,
  NOTIFICATION_EVENT_HOME_SECURITY_UNKNOWN_EVENT = 0xFE
}NOTIFICATION_EVENT_HOME_SECURITY;

/**
 * Notification event (8 bit) for notification type NOTIFICATION_TYPE_POWER_MANAGEMENT.
 */
typedef enum
{
  NOTIFICATION_EVENT_POWER_MANAGEMENT_NO_EVENT,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_POWER_HAS_BEEN_APPLIED,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_AC_MAINS_DISCONNECED,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_AC_MAINS_RECONNECED,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_SURGE_DETECTED,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_VOLTAGE_DROP_DRIFT,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_OVERCURRENT_DETECTED,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_OVERVOLTAGE_DETECTION,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_OVERLOADED_DETECTED,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_LOAD_ERROR,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_REPLACE_BATTERY_SOON,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_REPLACE_BATTERY_NOW,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_BATTERY_IS_CHARGING,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_BATTERY_IS_FULLY_CHARGED,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_CHARGE_BATTERY_SOON,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_CHARGE_BATTERY_NOW,
  NOTIFICATION_EVENT_POWER_MANAGEMENT_UNKNOWN_EVENT = 0xFE
}NOTIFICATION_EVENT_POWER_MANAGEMENT;


/**
 * Notification event (8 bit) for notification type NOTIFICATION_TYPE_SMOKE_ALARM.
 */
typedef enum
{
  NOTIFICATION_EVENT_EMERGENCY_ALARM_NO_EVENT,
  NOTIFICATION_EVENT_EMERGENCY_ALARM_CONTACT_POLICE,
  NOTIFICATION_EVENT_EMERGENCY_ALARM_CONTACT_FIRE_SERVICE,
  NOTIFICATION_EVENT_EMERGENCY_ALARM_CONTACT_MEDICAL_SERVICE,
  NOTIFICATION_EVENT_EMERGENCY_UNKNOWN_EVENT = 0xFE
}NOTIFICATION_EVENT_EMERGENCY_ALARM;


/**
 * Notification event (8 bit) for notification type NOTIFICATION_TYPE_SYSTEM.
 */
typedef enum
{
  NOTIFICATION_EVENT_SYSTEM_NO_EVENT,
  NOTIFICATION_EVENT_SYSTEM_HARDWARE_FAILURE,
  NOTIFICATION_EVENT_SYSTEM_SOFTWARE_FAILURE,
  NOTIFICATION_EVENT_SYSTEM_HARDWARE_FAILURE_WITH_MANUFACTURER_PROPRIETARY_FAILURE_CODE,
  NOTIFICATION_EVENT_SYSTEM_SOFTWARE_FAILURE_WITH_MANUFACTURER_PROPRIETARY_FAILURE_CODE,
  NOTIFICATION_EVENT_SYSTEM_UNKNOWN_EVENT = 0xFE
}NOTIFICATION_EVENT_SYSTEM;



/**
 * Notification event (8 bit) for notification type NOTIFICATION_TYPE_SMOKE_ALARM.
 */
typedef enum
{
  NOTIFICATION_EVENT_SMOKE_ALARM_NO_EVENT,
  NOTIFICATION_EVENT_SMOKE_ALARM_SMOKE_DETECTED,
  NOTIFICATION_EVENT_SMOKE_ALARM_SMODE_DETECTED_UNKNOWN_LOCATION,
  NOTIFICATION_EVENT_SMOKE_ALARM_TEST,
  NOTIFICATION_EVENT_SMOKE_ALARM_REPLACEMENT_REQUIRED,
  NOTIFICATION_EVENT_SMOKE_ALARM_UNKNOWN_EVENT = 0xFE
}NOTIFICATION_EVENT_SMOKE_ALARM;

/**
 * Notification status unsolicited set
 */
typedef enum
{
  NOTIFICATION_STATUS_SET_UNSOLICIT_DEACTIVATED = 0x00,
  NOTIFICATION_STATUS_SET_UNSOLICIT_ACTIVATED = 0xFF
} NOTIFICATION_STATUS_SET;


/**
 * Notification status Get
 */
typedef enum
{
  NOTIFICATION_STATUS_UNSOLICIT_DEACTIVATED = 0x00,
  NOTIFICATION_STATUS_NO_PENDING_NOTIFICATION = 0xFE,
  NOTIFICATION_STATUS_UNSOLICIT_ACTIVATED = 0xFF
} NOTIFICATION_STATUS;

typedef enum
{
  NOTIFICATION_EVENT_HEAT_ALARM_RAPID_TEMPERATURE_RISE_LOCATION_PROVIDED = 0x03,
  NOTIFICATION_EVENT_HEAT_ALARM_RAPID_TEMPERATURE_RISE = 0x04,
  NOTIFICATION_EVENT_HEAT_ALARM_RAPID_TEMPERATURE_FALL_LOCATION_PROVIDED = 0x0C,
  NOTIFICATION_EVENT_HEAT_ALARM_RAPID_TEMPERATURE_FALL= 0x0D,
  NOTIFICATION_EVENT_HEAT_ALARM_UNKNOWN_EVENT = 0xFE
} NOTIFICATION_EVENT_HEAT_ALARM;

/**
 * Struct used to pass operational data to TSE module
 */
typedef struct s_CC_notification_data_t_
{
  RECEIVE_OPTIONS_TYPE_EX rxOptions; /**< rxOptions */
  notification_type_t     notificationType;
  uint8_t                 notificationEvent;
  uint8_t                 sourceEndpoint;
} s_CC_notification_data_t;

/**
 * @brief Application specific Notification Set cmd handler.
 * @param[in] notificationType notification type
 * @param[in] notificationStatus notification status of type NOTIFICATION_STATUS_SET
 * @param[in] endpoint is the destination endpoint
 * @return command handler return code
 */
extern e_cmd_handler_return_code_t handleAppNotificationSet(
	notification_type_t notificationType,
    NOTIFICATION_STATUS_SET notificationStatus,
    uint8_t endpoint);

/**
 * @brief CmdClassNotificationGetNotification
 *  User application function. The Notification Status identifier can be set to the
 *  following values:
 *  Notification Status |	Description
 *  --------------------|-------------------------------------------------------------
 *  0x00                | Unsolicited notification is deactivated. The group mapped to Notification Command Class in Association Command Class is not configured with any node IDs.
 *  0xFF                | Unsolicited notification is activated or a pending notification is present.
 * @param notificationType 8 bit type. If 0xFF, return first detected Notification on supported list.
 * @param[in] endpoint is the destination endpoint
 * @return 8 bit notification status.
 */
extern NOTIFICATION_STATUS CmdClassNotificationGetNotificationStatus(
    notification_type_t notificationType,
    uint8_t endpoint);

/**
 *
 * @brief CmdClassNotificationGetNotificationEvent
 * User application function. Read event.
 * Event Parameter 1 ... Event Parameter N (N * Bytes).
 * If the "Event Parameters Length" field is not equal to 0, these field(s)
 * contain(s) the encapsulated information available to the "Notification Type"
 * and "Event".
 * @param[out] pNotificationType pointer to 8 bit type. If 0xFF, return first
 *             detected Notification on supported list.
 * @param[out] pNotificationEvent pointer to 8 bit event.
 * @param[out] pEventPar pointer to Event Parameter 1 ... Event Parameter N.
 * @param[out] pEvNbrs pointer to number of parameters N.
 * @param[in]  endpoint is the destination endpoint
 * @return Returns true if event is legal, and false otherwise.
 */
extern bool CmdClassNotificationGetNotificationEvent(
    notification_type_t * pNotificationType,
    uint8_t * pNotificationEvent,
    uint8_t * pEventPar,
    uint8_t * pEvNbrs,
    uint8_t endpoint);


/**
 * @brief CmdClassNotificationGetType
 * User application function. See SDS11060.doc table "4.73.3.1	Table of defined
 * Notification Types & Events".
 * @param[in] endpoint is the destination endpoint
 * @return 8 bit type.
 */
extern uint8_t CmdClassNotificationGetType(uint8_t endpoint);


/**
 * @brief CmdClassNotificationGetEvent
 * User application function. See SDS11060.doc table "4.73.3.1	Table of defined
 * Notification Types & Events".
 * @param[in] endpoint is the destination endpoint
 * @return 8 bit event.
 */
extern uint8_t CmdClassNotificationGetEvent(uint8_t endpoint);


/**
 * @brief handleCmdClassNotificationSupportedReport
 * The Notification Supported Report Command is used to report the supported
 * Notification Types in the application. The Notification Supported Report Command
 * is transmitted as a result of a received Notification Supported Get Command and
 * MUST not be sent unsolicited.
 * @param[out] pNbrBitMask Indicates the Number of Bit Masks fields (1-31) used
 *             in bytes.
 * @param[out] pBitMaskArray The Bit Mask fields describe the supported Notification
 *             Type(s) by the device
 * @param[in] bBitMaskLen The length of the Bit Mask fields
 * @param[in] endpoint is the destination endpoint
 */
extern void handleCmdClassNotificationSupportedReport(
  uint8_t * pNbrBitMask,
  uint8_t * pBitMaskArray,
  uint8_t   bBitMaskLen,
  uint8_t endpoint);


/**
 * @brief handleAppNotificationEventSupportedReport
 * The Event Supported Report Command is transmitted as a result of a received
 * Event Supported Get Command and MUST not be sent unsolicited.  If an Event
 * Supported Get is received with a not supported Notification Type or Notification
 * @param[in] notificationType notification Type.
 * @param[out] pNbrBitMask Indicates the Number of Bit Masks fields (1-31) used in bytes.
 * @param[out] pBitMaskArray The Bit Mask fields describe the supported Events within the requested Notification Type.
 *             Example if Notification Type = Heat Alarm (0x04):
 * @param[in] endpoint is the destination endpoint
 */
extern void handleCmdClassNotificationEventSupportedReport(
	notification_type_t notificationType,
    uint8_t * pNbrBitMask,
    uint8_t * pBitMaskArray,
    uint8_t endpoint);

/**
 * Transmits a Notification Report.
 *
 * See SDS11060.doc "Table of defined Notification Types & Events".
 *
 * @param[in] pProfile pointer to AGI profile
 * @param[in] sourceEndpoint source endpoint
 * @param[in] notificationType Notification Type (8 bit)
 * @param[in] notificationEvent Event (8 bit)
 * @param[in] pCallback callback function returning state on job
 * @return JOB_STATUS
 */
JOB_STATUS CmdClassNotificationReport(
  AGI_PROFILE const * const pProfile,
  uint8_t sourceEndpoint,
  notification_type_t notificationType,
  uint8_t notificationEvent,
  VOID_CALLBACKFUNC(pCallback)(TRANSMISSION_RESULT * pTransmissionResult));

/**
 * @brief GetGroupIndex
 * Read last active notification type
 * @param[in,out] pNotificationType
 * @param[in] endpoint is the destination endpoint
 * @return Success: group number, else: 0xFF
 */
extern uint8_t GetGroupIndex(notification_type_t * pNotificationType, uint8_t endpoint);

/**
 * Validates or finds a combination of notification type and endpoint.
 *
 * If the endpoint is zero meaning root device and a notification has been added for an endpoint,
 * the function will find this endpoint and modify the variable pointed to.
 * @param[in] notificationType Type of notification.
 * @param[in,out] pEndpoint Multichannel endpoint destination.
 * @return true if endpoint was found, false otherwise.
 */
extern bool FindNotificationEndpoint(notification_type_t notificationType, uint8_t * pEndpoint);

/**
 * Send report via lifeLine when a change happens.
 *
 * Callback used by TSE module. Refer to @ref ZAF_TSE.h for more details.
 *
 * @param txOptions txOptions
 * @param pData Command payload for the report
 */
void CC_Notification_report_stx(
    TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions,
    void* pData);

#endif /*_CC_NOTIFICATION_H_*/
