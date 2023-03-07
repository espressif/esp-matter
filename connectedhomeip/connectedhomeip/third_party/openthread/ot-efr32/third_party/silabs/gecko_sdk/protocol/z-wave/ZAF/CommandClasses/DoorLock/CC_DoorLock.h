/**
 * @file
 * Handler for Command Class Door Lock.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _CC_DOORLOCK_H_
#define _CC_DOORLOCK_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_typedefs.h>
#include <ZW_classcmd.h>
#include <CC_Common.h>
#include <agi.h>
#include <ZW_TransportEndpoint.h>
#include <ZAF_types.h>
#include "ZAF_CC_Invoker.h"
/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/**
 * For backwards compatibility.
 */
#define handleCommandClassDoorLockOperationSet(a)          CC_DoorLock_OperationSet_handler(a)
#define handleCommandClassDoorLockConfigurationSet(a)      CC_DoorLock_ConfigurationSet_handler(a)

/*
 * Renamed the Report handlers to Get handlers because they are handlers for the Get frames and not
 * for the Report frames. Upon handling a Get frame, ZAF is transmitting a Report frame.
 */
#define handleCommandClassDoorLockOperationReport(a)     CC_DoorLock_OperationGet_handler(a)
#define handleCommandClassDoorLockConfigurationReport(a) CC_DoorLock_ConfigurationGet_handler(a)

/**
 * Door Lock Mode (8 bit) will set the door lock device in unsecured or
 * secured mode as well as other peripheral settings.
 *
 * 1) Constant mode. Door will be unsecured until set back to secured mode by Command.
 * 2) Timeout mode. Fallback to secured mode after timeout has expired (set by Door Lock Configuration Set).
 * 3) This is Read Only State, i.e. Bolt is not fully retracted/engaged
 */
typedef enum
{
  DOOR_MODE_UNSEC = DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_V2,	/**< Door Unsecured 1)*/                                                                                            //!< DOOR_MODE_UNSEC
  DOOR_MODE_UNSEC_TIMEOUT = DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_WITH_TIMEOUT_V2,	/**< Door Unsecured with timeout 2)*/                                                          //!< DOOR_MODE_UNSEC_TIMEOUT
  DOOR_MODE_UNSEC_INSIDE = DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES_V2,	/**< Door Unsecured for inside Door Handles 1)*/                                     //!< DOOR_MODE_UNSEC_INSIDE
  DOOR_MODE_UNSEC_INSIDE_TIMEOUT = DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_INSIDE_DOOR_HANDLES_WITH_TIMEOUT_V2,	/**< Door Unsecured for inside Door Handles with timeout 2)*/   //!< DOOR_MODE_UNSEC_INSIDE_TIMEOUT
  DOOR_MODE_UNSEC_OUTSIDE = DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES_V2,	/**< Door Unsecured for outside Door Handles 1)*/                                  //!< DOOR_MODE_UNSEC_OUTSIDE
  DOOR_MODE_UNSEC_OUTSIDE_TIMEOUT = DOOR_LOCK_OPERATION_SET_DOOR_UNSECURED_FOR_OUTSIDE_DOOR_HANDLES_WITH_TIMEOUT_V2,	/**< Door Unsecured for outside Door Handles with timeout 2)*///!< DOOR_MODE_UNSEC_OUTSIDE_TIMEOUT
  DOOR_MODE_UNKNOWN = DOOR_LOCK_OPERATION_SET_DOOR_LOCK_STATE_UNKNOWN_V2, /**<	Door/Lock State Unknown 3). (Version 2)*/                                                           //!< DOOR_MODE_UNKNOWN
  DOOR_MODE_SECURED = DOOR_LOCK_OPERATION_SET_DOOR_SECURED_V2	/**< Door Secured*/                                                                                                  //!< DOOR_MODE_SECURED
} door_lock_mode_t;

/**
 * For backwards compatibility.
 */
typedef door_lock_mode_t DOOR_MODE;


/**
 * Operation mode (1byte).
 * The Operation Type field can be set to either constant or timed operation. When
 * timed operation is set, the Lock Timer Minutes and Lock Timer Seconds fields
 * MUST be set to valid values.
 */
typedef enum
{
  DOOR_OPERATION_CONST = 0x01,   /**< Constant operation*/
  DOOR_OPERATION_TIMED = 0x02,   /**< Timed operation*/
  DOOR_OPERATION_RESERVED = 0x03 /**< 0X03..0XFF  Reserved*/
}
cc_door_lock_operation_type_t;

/**
 * Supported Door Components
 * Bitmask of supported door components. Set to 1 if corresponding component is supported.
 */
typedef enum
{
  DOOR_COMPONENT_DOOR = 0x01,
  DOOR_COMPONENT_BOLT = 0x02,
  DOOR_COMPONENT_LATCH = 0x04
} cc_door_lock_door_component_t;

/**
 * Supported Door Handles
 * Bitmask of supported door handles. Set to 1 if corresponding handle is supported.
 */
typedef enum
{
  DOOR_HANDLE_1 = 0x01,
  DOOR_HANDLE_2 = 0x02,
  DOOR_HANDLE_3 = 0x04,
  DOOR_HANDLE_4 = 0x08
} cc_door_lock_handle_t;

/**
 * For backwards compatibility.
 */
typedef cc_door_lock_operation_type_t DOOR_OPERATION;

/**
 * Door Lock Operation Report data structure.
 * -----------------------------------------
 *
 * Inside/outside Door Handles Mode
 * These mode bits indicate if the actual handle can open the door locally.
 *
 * The mode of the four inside and four outside door handles are encoded in
 * \ref insideDoorHandleMode and \ref outsideDoorHandleMode as:
 * - Bit 0: Handle 1
 * - Bit 1: Handle 2
 * - Bit 2: Handle 3
 * - Bit 3: Handle 4
 * Values:
 * - 0: disabled
 * - 1: enabled
 *
 * Door condition.
 * The \ref condition field indicates the status of the door lock components
 * such as bolt and latch states.
 * - Bit 0: Door
 *   - 0: Open
 *   - 1: Closed
 * - Bit 1: Bolt
 *   - 0: Locked
 *   - 1: Unlocked
 * - Bit 2: Latch
 *   - 0: Open
 *   - 1: Closed
 * - Bit 3-7: Reserved
 */
typedef struct
{
  door_lock_mode_t mode;
  uint8_t insideDoorHandleMode : 4; /**< Inside Door Handles Mode (4 bits)*/
  uint8_t outsideDoorHandleMode : 4; /**< Outside Door Handles Mode (4 bits)*/
  uint8_t condition; /**< Door lock condition (8 bits)*/
  uint8_t lockTimeoutMin; /**< Lock Timeout Minutes, valid values 1-254 decimal*/
  uint8_t lockTimeoutSec; /**< Lock Timeout Seconds, valid 1-59 decimal*/
  door_lock_mode_t targetMode; /**< Target Door Lock Mode of ongoing transition */
  uint8_t duration; /**< Duration of the remaining time before target mode is reached*/
} cc_door_lock_operation_report_t;

/**
 * Door Lock Capabilities Report data structure.
 * --------------------------------------------
 *
 * This allows the Application to inform the Command Class handler of the capabilities
 * of the Door Lock.
 *
 * The \ref supportedOperationTypeBitmask field indicates the Supported Operations Modes:
 *   - Bit 0 in Bit Mask 1: Reserved
 *   - Bit 1 in Bit Mask 1: Constant Operation
 *   - Bit 2 in Bit Mask 1: Timed Operation.
 *
 * The \ref supportedDoorLockModeList indicates the Supported Door Lock Modes:
 *   - 0x00: Door Unsecured
 *   - 0x01: Door unsecured with timeout
 *   - 0x10: Door Unsecured for Inside Door Handles
 *   - 0x11: Door Unsecured for Inside Door Handles with timeout
 *   - 0x20: Door Unsecured for Outside Door Handles
 *   - 0x21: Door Unsecured for Outside Door Handles with timeout
 *   - 0xFF: Door Secured
 *
 *
 * Inside/outside Door Handles Modes supported
 * These mode bits indicate which handles are supported on the Door Lock.
 *
 * The mode of the four inside and four outside door handles are encoded in
 * \ref supportedInsideHandleModes and \ref supportedOutsideHandleModes as:
 * - Bit 0: Handle 1
 * - Bit 1: Handle 2
 * - Bit 2: Handle 3
 * - Bit 3: Handle 4
 * Values:
 * - 0: disabled
 * - 1: enabled
 *
 * Supported Door Components.
 * The \ref supportedDoorComponents field indicates the supported Door Lock components:
 * such as bolt and latch states.
 * - Bit 0: Door
 * - Bit 1: Bolt
 * - Bit 2: Latch
 * - Bit 3-7: Reserved
 *
 * Auto Relock Support
 * The \ref autoRelockSupport flag indicates if the Door Lock Supports Auto Relock (1=supported, 0=not supported).
 *
 * Hold And Release Support
 * The \ref holdAndReleaseSupport flag indicates if the Door Lock Supports * Hold And Releaese (1=supported, 0=not supported),
 *
 * Twist Assist Support
 * The \ref twistAssistSupport flag indicates if the Door Lock Supports Twist Assist (1=supported, 0=not supported),
 *
 * Block To Block Support
 * The \ref blockToBlockSupport flag indicates if the Door Lock Supports Block To Block (1=supported, 0=not supported),
 */
typedef struct
{
  uint8_t reserved : 3; /**< Reserved (2 bits)*/
  uint8_t lengthSupportedOperationType : 5; /**< Length of Supported Operation Type Bitmask (5 bits)*/
  uint8_t supportedOperationTypeBitmask; /**< Supported Operation Types. Max. length fixed to 1 byte. Enough to hold both defined types defined in SDS13781. */
  uint8_t lengthSupportedDoorLockModeList; /**<  Length of supported Door Lock Mode List. */
  uint8_t supportedDoorLockModeList[8]; /**< List of Supported Door Lock Modes. Max. Length fixed to 8 byte. Enough to hold all modes defined in SDS13781. */
  uint8_t supportedOutsideHandleModes : 4; /**< Supported Outside Handle Modes bitmask (4 bits) */
  uint8_t supportedInsideHandleModes : 4; /**< Supported Inside Handle Modes bitmask (4 bits) */
  uint8_t supportedDoorComponents; /**< Supported Door Components */
  uint8_t autoRelockSupport : 1; /**< Indicates if the lock supports Auto Relock*/
  uint8_t holdAndReleaseSupport : 1; /**< Indicates if the lock supports Hold And Release */
  uint8_t twistAssistSupport : 1; /**< Indicates if the lock supports Twist Assist */
  uint8_t blockToBlockSupport : 1; /**< Indicates if the lock supports Twist Assist */
} cc_door_lock_capabilities_report_t;


/**
 * For backwards compatibility.
 */
typedef cc_door_lock_operation_report_t CMD_CLASS_DOOR_LOCK_OPERATION_REPORT;

/**
 * Door Lock Configuration Get/Set Structure
 * -----------------------------------------
 *
 * Inside/outside Door Handles Mode
 * These fields are used to configure if each individual outside and inside door handle can
 * open the door locally.
 *
 * The mode of the four inside and four outside door handles are encoded in
 * \ref insideDoorHandleMode and \ref outsideDoorHandleMode as:
 * - Bit 0: Handle 1
 * - Bit 1: Handle 2
 * - Bit 2: Handle 3
 * - Bit 3: Handle 4
 * Values:
 * - 0: disabled
 * - 1: enabled
 */
typedef struct
{
  cc_door_lock_operation_type_t type;
  uint8_t insideDoorHandleMode : 4; /**< Inside Door Handles Mode (4 bits)*/
  uint8_t outsideDoorHandleMode : 4; /**< Outside Door Handles Mode (4 bits)*/
  uint8_t lockTimeoutMin; /**< Lock Timeout Minutes, valid values 1-254 decimal*/
  uint8_t lockTimeoutSec; /**< Lock Timeout Seconds, valid 1-59 decimal*/
  uint8_t autoRelockTime1; /**< Auto-relock time in seconds MSB */
  uint8_t autoRelockTime2; /**< Auto-relock time in seconds LSB */
  uint8_t holdAndReleaseTime1; /**< Hold and release time in seconds MSB */
  uint8_t holdAndReleaseTime2; /**< Hold and release time in seconds LSB */
  uint8_t reservedOptionsFlags; /**< Bit 0: Twist assist, bit 1: Block to block, others reserved */
} cc_door_lock_configuration_t;

/**
 * For backwards compatibility.
 */
typedef cc_door_lock_configuration_t CMD_CLASS_DOOR_LOCK_CONFIGURATION;

/**
 * Cmd Class Door Lock data structure.
 * -----------------------------------
 *
 * Inside/outside Door Handles Mode
 * These mode bits indicate if the actual handle can open the door locally.
 *
 * The mode of the four inside and four outside door handles are encoded in
 * \ref insideDoorHandleMode and \ref outsideDoorHandleMode as:
 * - Bit 0: Handle 1
 * - Bit 1: Handle 2
 * - Bit 2: Handle 3
 * - Bit 3: Handle 4
 * Values:
 * - 0: disabled
 * - 1: enabled
 *
 * Inside/outside Door Handles State
 * These fields indicate if a door handle has been activated.
 * The state of the four inside and four outside door handles are encoded in
 * \ref insideDoorHandleState and \ref outsideDoorHandleState as follows:
 * - Bit 0: Handle 1
 * - Bit 1: Handle 2
 * - Bit 2: Handle 3
 * - Bit 3: Handle 4
 * Values:
 * - 0: inactive
 * - 1: activated
 *
 * Door condition.
 * The \ref condition field indicates the status of the door lock components
 * such as bolt and latch states.
 * - Bit 0: Door
 *   - 0: Open
 *   - 1: Closed
 * - Bit 1: Bolt
 *   - 0: Locked
 *   - 1: Unlocked
 * - Bit 2: Latch
 *   - 0: Open
 *   - 1: Closed
 * - Bit 3-7: Reserved
 */
typedef struct
{
  door_lock_mode_t mode;
  cc_door_lock_operation_type_t type;
  uint8_t insideDoorHandleMode : 4; /**< Inside Door Handles Mode (4 bits)*/
  uint8_t outsideDoorHandleMode : 4; /**< Outside Door Handles Mode (4 bits)*/
  uint8_t insideDoorHandleState : 4; /**< Inside Door Handles State (4 bits)*/
  uint8_t outsideDoorHandleState : 4; /**< Outside Door Handles State (4 bits)*/
  uint8_t condition; /**< Door condition (8 bits)*/
  uint8_t lockTimeoutMin; /**< Lock Timeout Minutes, valid values 1-254 decimal*/
  uint8_t lockTimeoutSec; /**< Lock Timeout Seconds, valid 1-59 decimal*/
} cc_door_lock_data_t;

/**
 * For backwards compatibility.
 */
typedef cc_door_lock_data_t CMD_CLASS_DOOR_LOCK_DATA;

/**
 * Disabling Lock timeout minutes and seconds.
 */
#define DOOR_LOCK_OPERATION_SET_TIMEOUT_NOT_SUPPORTED 0xFE

/**
 * Struct used to pass operational data to TSE module
 */
typedef struct s_CC_doorLock_data_t_
{
  RECEIVE_OPTIONS_TYPE_EX rxOptions; /**< rxOptions */
} s_CC_doorLock_data_t;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/


/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * Called upon receiving a Door Lock Operation Set frame.
 *
 * The function is externally declared and must be implemented by the application.
 *
 * @param mode The mode that the door lock must enter upon receiving this command.
 *
 * @return CMD_HANDLER_RETURN_CODE
 */
extern e_cmd_handler_return_code_t CC_DoorLock_OperationSet_handler(door_lock_mode_t mode);

/**
 * Called upon receiving a Door Lock Operation Get frame.
 *
 * The function is externally declared and must be implemented by the application.
 *
 * @param pData Pointer to the data that will be included in the Operation Report frame. The
 * application must fill in this data in it's implementation.
 */
extern void CC_DoorLock_OperationGet_handler(cc_door_lock_operation_report_t * pData);

/**
 * Called upon receiving a Door Lock Configuration Set frame.
 *
 * The function is externally declared and must be implemented by the application.
 *
 * @param pData Pointer to the data in the Configuration Set frame.
 *
 * @return CMD_HANDLER_RETURN_CODE
 */
extern e_cmd_handler_return_code_t CC_DoorLock_ConfigurationSet_handler(cc_door_lock_configuration_t * pData);

/**
 * Called upon receiving a Door Lock Configuration Get frame.
 *
 * The function is externally declared and must be implemented by the application.
 *
 * @param pData Pointer to the data that will be included in the Configuration Report frame. The
 * application must fill in this data in it's implementation.
 */
extern void CC_DoorLock_ConfigurationGet_handler(cc_door_lock_configuration_t* pData);

/**
 * Called upon receiving a Door Lock Capabilities Get frame.
 *
 * The function is externally declared and must be implemented by the application.
 *
 * @param pData Pointer to the data that will be included in the Capabilities Report frame. The
 * application must fill in this data in it's implementation.
 */
extern void CC_DoorLock_CapabilitiesGet_handler(cc_door_lock_capabilities_report_t* pData);

/**
 * @brief Send DoorLock operation report.
 * @param[in] pProfile pointer to AGI profile
 * @param[in] sourceEndpoint source endpoint
 * @param[in] pData Pointer to door lock data.
 * @param[out] pCallback callback function pointer returning status destination node receive job.
 * @return status on the job.
 */
JOB_STATUS CC_DoorLock_OperationReport_tx(
  AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  cc_door_lock_operation_report_t* pData,
  void(*pCallback)(TRANSMISSION_RESULT * pTransmissionResult));

/**
 * Send report when change triggered by DOOR_LOCK_OPERATION_SET_V4 happen via lifeLine.
 *
 * Callback used by TSE module. Refer to @ref ZAF_TSE.h for more details.
 *
 * @param txOptions txOptions filled by TSE Trigger
 * @param pData Command payload for the report
 */
void CC_DoorLock_operation_report_stx(TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions, void* pData);

/**
 * Send report when change triggered by DOOR_LOCK_CONFIGURATION_SET_V2 happen via lifeLine.
 * Callback used by TSE module. Refer to @ref ZAF_TSE.h for more details.
 *
 * @param txOptions filled by TSE Trigger
 * @param pData Command payload for the report
 */
void CC_DoorLock_configuration_report_stx(TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions, void* pData);

/**
 * This function is used to notify the Application that the CC Door Lock Operation Set
 * status is in a WORKING state. The application can subsequently make the TSE Trigger
 * using the information passed on from the rxOptions.
 * @param pRxOpt pointer used to when triggering the "working state"
*/
extern void CC_DoorLock_operation_report_notifyWorking(RECEIVE_OPTIONS_TYPE_EX *pRxOpt);

/**
 * Prepare the data input for the TSE for any Door Lock CC command based on the pRxOption pointer.
 * @param pRxOpt pointer used to indicate how the frame was received (if any) and what endpoints are affected
*/
extern void* CC_Doorlock_prepare_zaf_tse_data(RECEIVE_OPTIONS_TYPE_EX* pRxOpt);

#endif /* _CC_DOORLOCK_H_ */
