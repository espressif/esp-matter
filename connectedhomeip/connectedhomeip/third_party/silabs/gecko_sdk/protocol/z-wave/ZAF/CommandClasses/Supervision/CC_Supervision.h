/**
 * @file
 * Handler for Command Class Supervision.
 * @copyright 2018 Silicon Laboratories Inc.
 *
 * ## Description
 * Command Class Supervision is built into the Application Framework and handle
 * Supervision communication on S2 encapsulated frames. Each Command Class
 * request job setup if Supervision should be enabled. Only Set and Report commands
 * are Supervision Get enabled why Get is disabled.
 *
 * Command Class Supervision is default configuration to not support more Supervision
 * reports on a Supervision Get command. In Supervision Get command is 'more status
 * updates' field set to CC_SUPERVISION_STATUS_UPDATES_NOT_SUPPORTED.
 *
 * ## Configuration scenarios
 *
 * ### Default configuration
 * Device does not handle more Supervision reports. Device receive request jobs and return
 * always only one Supervision report result = CC_SUPERVISION_STATUS_SUCCESS. No need to
 * call CommandClassSupervisionInit() for initialization of Command Class Supervision.
 *
 * @startuml
 * title Handle one Supervision Report sequence
 * actor User
 * box "Source node" #LightBlue
 *    participant WallSwitch_appl
 *    participant Framework_protocol
 * end box
 * participant host_node
 * == Usage ==
 * User->WallSwitch_appl: key press
 * WallSwitch_appl->Framework_protocol: Basic Set
 * activate Framework_protocol
 * Framework_protocol->host_node: S2[SupervisionGet[BasicSet]]
 * activate host_node
 * note right
 *  Status Updates = STATUS_UPDATES_NOT_SUPPORTED
 *  why only one Supervision report is returned
 * end note
 * host_node-->Framework_protocol: ack
 * Framework_protocol-->WallSwitch_appl: status
 * deactivate Framework_protocol
 * host_node->Framework_protocol: S2[SupervisionReport(SUPERVISION STATUS)]
 * deactivate host_node
 * host_node->Framework_protocol : S2[SupervisionGet[BasicSet]]
 * activate Framework_protocol
 * Framework_protocol->WallSwitch_appl : Basic Set
 * WallSwitch_appl-->Framework_protocol
 * Framework_protocol->host_node: S2[SupervisionReport(SUPERVISION STATUS)]
 * deactivate Framework_protocol
 * @enduml
 *
 * ### Handle more Supervision Reports
 * Device has the possibility to display destination node is working. Example Wall controller
 * with a display showing a device is working (CC_SUPERVISION_STATUS_WORKING) until position
 * is reached (CC_SUPERVISION_STATUS_SUCCESS).
 *
 * @startuml
 * title Handle more Supervision reports sequence
 * actor User
 * box "Source node" #LightBlue
 *    participant WallSwitch_appl
 *    participant Framework_protocol
 * end box
 * participant host_node
 * == Initialization ==
 *   WallSwitch_appl->Framework_protocol: CommandClassSupervisionInit(..)
 *   Framework_protocol-->WallSwitch_appl
 * == Usage ==
 * User->WallSwitch_appl: key press
 * WallSwitch_appl->Framework_protocol: Basic Set
 * activate Framework_protocol
 * Framework_protocol->host_node: S2[SupervisionGet[BasicSet]]
 * activate host_node
 * host_node-->Framework_protocol: ack
 * Framework_protocol-->WallSwitch_appl: status BasicSet
 * deactivate Framework_protocol
 * host_node->Framework_protocol: S2[SupervisionReport(WORKING)]
 * Framework_protocol->WallSwitch_appl: CC_SUPERVISION_STATUS_WORKING
 * WallSwitch_appl->User: Display indication working
 * host_node->Framework_protocol: S2[SupervisionReport(SUCCESS)]
 * deactivate host_node
 * Framework_protocol->WallSwitch_appl: CC_SUPERVISION_STATUS_SUCCESS
 * WallSwitch_appl->User: Display indication finish
 * host_node->Framework_protocol : S2[SupervisionGet[BasicSet]]
 * activate Framework_protocol
 * Framework_protocol->WallSwitch_appl : Basic Set
 * WallSwitch_appl-->Framework_protocol
 * Framework_protocol->host_node: S2[SupervisionReport(SUPERVISION STATUS)]
 * deactivate Framework_protocol
 * @enduml
 *
 * ### Control Supervision Reports
 * Device has the possibility to send more Supervision Reports to report
 * ongoing Request job. Example application Doorlock Keypad reports back Doorlock
 * operation is started and a report when it is finish.
 *
 * @startuml
 * title Control Supervision reports sequence
 * box "Doorlock Keypad node" #LightBlue
 *    participant DoorlockKeyPad_appl
 *    participant Framework_protocol
 * end box
 * participant controller_node
 * == Initialization ==
 *   DoorlockKeyPad_appl -> Framework_protocol: CommandClassSupervisionInit(..)
 *   Framework_protocol --> DoorlockKeyPad_appl
 * == Usage controller do not support more Supervision Reports ==
 * controller_node -> Framework_protocol : S2[SupervisionGet[DoorlockOperation]]
 * activate Framework_protocol
 * Framework_protocol -->  controller_node : ack
 * Framework_protocol -> DoorlockKeyPad_appl : DoorlockOperation
 * DoorlockKeyPad_appl --> Framework_protocol
 * Framework_protocol -> DoorlockKeyPad_appl : ZCB_CommandClassSupervisionGetReceived
 * DoorlockKeyPad_appl --> Framework_protocol : CC_SUPERVISION_STATUS_WORKING & duration = 2
 * Framework_protocol -> controller_node : SupervisionReport
 * deactivate Framework_protocol
 * DoorlockKeyPad_appl -> DoorlockKeyPad_appl : DoorlockOperation finish
 * == Usage controller support more Supervision Reports ==
 * controller_node -> Framework_protocol : S2[SupervisionGet[DoorlockOperation]]
 * activate Framework_protocol
 * Framework_protocol -->  controller_node : ack
 * Framework_protocol -> DoorlockKeyPad_appl : DoorlockOperation
 * DoorlockKeyPad_appl --> Framework_protocol
 * Framework_protocol -> DoorlockKeyPad_appl : ZCB_CommandClassSupervisionGetReceived
 * DoorlockKeyPad_appl --> Framework_protocol : CC_SUPERVISION_STATUS_WORKING & duration = 2
 * Framework_protocol -> controller_node : SupervisionReport
 * deactivate Framework_protocol
 * ...2 sec later...
 * DoorlockKeyPad_appl -> Framework_protocol : CmdClassSupervisionReportSend CC_SUPERVISION_STATUS_SUCCESS & duration = 0
 * Framework_protocol -> controller_node : SupervisionReport
 * @enduml
 */





#ifndef _COMMAND_CLASS_SUPERVISION_H_
#define _COMMAND_CLASS_SUPERVISION_H_


/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <stdint.h>
#include <ZW_classcmd.h>
#include <CC_Common.h>
#include <ZW_TransportEndpoint.h>
#include "ZAF_CC_Invoker.h"


#define CC_SUPERVISION_EXTRACT_SESSION_ID(properties)  (properties & 0x3F) /**< extract session id from properties bits [0..5] */
#define CC_SUPERVISION_ADD_SESSION_ID(properties)  (properties & 0x3F)     /**< add session id from properties bits [0..5] */
#define CC_SUPERVISION_EXTRACT_MORE_STATUS_UPDATE(status) (status >> 7)    /**< extract more status update field bit 7, CC SUPERVISION_REPORT */
#define CC_SUPERVISION_ADD_MORE_STATUS_UPDATE(status)  (status << 7)       /**< status values: {0,1}. Set bit 7, CC SUPERVISION_REPORT */
#define CC_SUPERVISION_EXTRACT_STATUS_UPDATE(status) (status >> 7)    /**< extract more status update field bit 7, CC SUPERVISION_GET */
#define CC_SUPERVISION_ADD_STATUS_UPDATE(status)  (status << 7)       /**< status values: {0,1}. Set bit 7, CC SUPERVISION_GET */


/**
 * Enum type is used in CC SUPERVISION_REPORT to allow a receiving node to advertise
 * application status updates in future Supervision Report Commands
 */
typedef enum
{
  CC_SUPERVISION_MORE_STATUS_UPDATES_THIS_IS_LAST,
  CC_SUPERVISION_MORE_STATUS_UPDATES_REPORTS_TO_FOLLOW
} cc_supervision_more_status_updates_t;

/**
 * enum type used in CC SUPERVISION_GET to advertise if more Supervision Reports follow
 * for the actual Session ID.
 */
typedef enum
{
  CC_SUPERVISION_STATUS_UPDATES_NOT_SUPPORTED,
  CC_SUPERVISION_STATUS_UPDATES_SUPPORTED
}cc_supervision_status_updates_t;

/**
 * Status of Supervision.
 */
typedef enum _SUPERVISION_STATUS_
{
  CC_SUPERVISION_STATUS_NOT_SUPPORTED,/**< SUPERVISION_STATUS_NOT_SUPPORTED */
  CC_SUPERVISION_STATUS_WORKING,      /**< SUPERVISION_STATUS_WORKING */
  CC_SUPERVISION_STATUS_FAIL,         /**< SUPERVISION_STATUS_FAIL */
  CC_SUPERVISION_STATUS_CANCEL,       /**< CC Supervision shouldn't do anything with this frame,
                                           report will be sent from somewhere else*/
  CC_SUPERVISION_STATUS_NOT_FOUND,    /**< CC handler was not found in CC handler map */
  CC_SUPERVISION_STATUS_SUCCESS = 0xFF/**< SUPERVISION_STATUS_SUCCESS */
} cc_supervision_status_t;

/**
 * This struct is used for the arguments in pGetReceivedHandler function
 * pointer in CommandClassSupervisionInit.
 */
typedef struct
{
  uint8_t cmdClass; /**< Input */
  uint8_t cmd; /**< Input */
  uint8_t properties1; /**< Input: bit[0..5] session id, bit[7] status update of type cc_supervision_status_updates_t.
                         Output: bit[0..5] session id, bit[7] more status updates of type cc_supervision_more_status_updates_t */
  cc_supervision_status_t status; /**< Output */
  uint8_t duration; /**< Output */
  RECEIVE_OPTIONS_TYPE_EX * rxOpt; /**< Input */
}
SUPERVISION_GET_RECEIVED_HANDLER_ARGS;



/**
 * @brief Initialize the Supervision command class with suitable callback
 * functions.
 * @param status_updates parameter is used CC SUPERVISION_GET command. This flag is
 * used to allow a receiving node to advertise application status updates in future
 * Supervision Report Commands. Default value 'status_updates_not_supported'.
 * @param pGetReceivedHandler Pointer to function which can be called when a
 * Supervision get command is received.
 * @param pReportReceivedHandler Pointer to function which can be called when a
 * Supervision report command is received.
 */
void
CommandClassSupervisionInit( cc_supervision_status_updates_t status_updates,
        void (*pGetReceivedHandler)(SUPERVISION_GET_RECEIVED_HANDLER_ARGS * pArgs),
        void (*pReportReceivedHandler)(cc_supervision_status_t status, uint8_t duration));

/**
 * @brief CmdClassSupervisionReportSend
 * @param[in] pTxOptionsEx Transmit options of type TRANSMIT_OPTIONS_TYPE_EX
 * @param[in] properties includes:
 *  bit[0..5] Session ID
 *  bit[6] unused and
 *  bit[7] more status updates. This field is used to advertise if more Supervision Reports
 *  follow for the actual Session ID. [0] last report, [1] more report follow.
 * @param[in] status Supervision status.
 * @param[in] duration The duration must comply with the following:
 * 0x00 0 seconds. (Already at the Target Value.)
 * 0x01-0x7F  1 second (0x01) to 127 seconds (0x7F) in 1 second resolution.
 * 0x80-0xFD  1 minute (0x80) to 126 minutes (0xFD) in 1 minute resolution.
 * 0xFE Unknown duration
 * 0xFF Reserved
 * @return status of the job of type JOB_STATUS
 */
JOB_STATUS
CmdClassSupervisionReportSend(
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX* pTxOptionsEx,
  uint8_t properties,
  cc_supervision_status_t status,
  uint8_t duration);


/**
 * @brief Used by Framework and should not be used be application!
 * Add supervision Get payload
 * @param[in] pPayload pointer to supervision get payload
 */
void CommandClassSupervisionGetAdd(ZW_SUPERVISION_GET_FRAME* pPayload);


/**
 * @brief Used by Framework and should not be used be application!
 * Write supervision Get payload
 * @param[in] pbuf Pointer to supervision get payload
 */
void
CommandClassSupervisionGetWrite(ZW_SUPERVISION_GET_FRAME* pbuf);

/**
 * @brief Used by Framework and should not be used be application!
 * Sets the payload length in a Supervision Get command.
 * @param[in] pbuf Pointer to supervision get payload
 * @param[in] payLoadlen Payload length.
 */
void CommandClassSupervisionGetSetPayloadLength(ZW_SUPERVISION_GET_FRAME* pbuf, uint8_t payLoadlen);

/**
 * CommandClassSupervisionGetSetPayloadLength
 */
uint8_t CommandClassSupervisionGetGetPayloadLength(ZW_SUPERVISION_GET_FRAME* pbuf);

#endif /*_COMMAND_CLASS_SUPERVISION_H_*/

