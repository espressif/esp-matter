/**
 * @file
 * Handler for Command Class Binary Switch.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _CC_BINARY_SWITCH_H_
#define _CC_BINARY_SWITCH_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_classcmd.h>
#include <CC_Common.h>
#include <ZAF_types.h>
#include <ZW_TransportEndpoint.h>
#include <ZAF_TSE.h>
#include "ZAF_CC_Invoker.h"

/****************************************************************************/
/*                       PUBLIC TYPES and DEFINITIONS                       */
/****************************************************************************/

#define BIN_SWITCH_DURATION_MIN_VAL_SECONDS 0x00
#define BIN_SWITCH_DURATION_MAX_VAL_SECONDS 0x7F
#define BIN_SWITCH_DURATION_MIN_VAL_MINUTES 0x80
#define BIN_SWITCH_DURATION_MAX_VAL_MINUTES 0xFE
#define BIN_SWITCH_DURATION_FACTORY_DEFAULT 0xFF
#define BIN_SWITCH_DURATION_MAX_MINUTES     (BIN_SWITCH_DURATION_MAX_VAL_MINUTES - BIN_SWITCH_DURATION_MIN_VAL_MINUTES)

/**
 * The value can be either 0x00 (off/disable), 0xFF (on/enable) or
 * 0xFE (unknown, only used for reporting). The values from 1 to 99 (0x01 to 0x63)
 * SHALL be mapped to 0xFF upon receipt of the Command in the device. All other
 * values are reserved and SHALL be ignored by the receiving device.
 */
typedef enum
{
  CMD_CLASS_BIN_OFF     = 0x00, /**< off/disable */
  CMD_CLASS_BIN_UNKNOWN = 0xFE, /**< unknown */
  CMD_CLASS_BIN_ON      = 0xFF  /**< on/enable */
}
CMD_CLASS_BIN_SW_VAL;

/**
 * Struct used to pass operational data to TSE module
 */
typedef struct s_CC_binarySwitch_data_t_
{
  RECEIVE_OPTIONS_TYPE_EX rxOptions; /**< rxOptions */
} s_CC_binarySwitch_data_t;

/**
 * Check value and duration are correct for current class and call
 * application handleApplBinarySwitchSet function
 * @param[in] target_value binary switch target value. 0 is mapped to OFF,
 *            while 1-99 and 255 are mapped to ON. Other values are invalid
 *            and are ignored.
 * @param[in] duration binary switch transition duration (encoded according
 *            to Binary Switch CC specification)
 * @param[in] endpoint the destination endpoint
 * @return command handler return code
 */
e_cmd_handler_return_code_t CommandClassBinarySwitchSupportSet(
  uint8_t target_value,
  uint8_t duration,
  uint8_t endpoint);

/**
 * Send a unsolicited command class Binary Switch report.
 * @param[in] pProfile pointer to AGI profile
 * @param[in] sourceEndpoint source endpoint
 * @param[in] current_value current value of switch to include in report
 * @param[in] target_value target value of switch to include in report
 * @param[in] duration remaining transition duration of switch to include
 *            in report (encoded according to Binary Switch CC specification)
 * @param[out] pCbFunc callback function returning status destination node
 *             receive job.
 * @return status on the job.
 */
JOB_STATUS CmdClassBinarySwitchReportSendUnsolicited(
  AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  CMD_CLASS_BIN_SW_VAL current_value,
  CMD_CLASS_BIN_SW_VAL target_value,
  uint8_t duration,
  VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult));

/****************************************************************************/
/*                      APPLICATION SPECIFIC FUNCTIONS                      */
/****************************************************************************/

/**
 * Get the factory default transition duration time for a binary switch.
 *
 * This function must be implemented in the application code.
 * Called by binary switch command class handler.
 *
 * @param[in] endpoint binary switch endpoint
 * @return the endpoint's factory default switch transition duration
 *         (encoded according to Binary Switch CC specification).
 */
extern uint8_t appBinarySwitchGetFactoryDefaultDuration(uint8_t endpoint);

/**
 * Get current value of binary switch from application.
 *
 * This function must be implemented in the application code.
 * Called by binary switch command class handler.
 *
 * @param[in] endpoint binary switch endpoint
 * @return current value of switch
 */
extern CMD_CLASS_BIN_SW_VAL appBinarySwitchGetCurrentValue(uint8_t endpoint);

/**
 * Get target value of binary switch.
 *
 * This function must be implemented in the application code.
 * Called by binary switch command class handler.
 *
 * @param[in] endpoint binary switch endpoint
 * @return target value of switch
 */
extern CMD_CLASS_BIN_SW_VAL appBinarySwitchGetTargetValue(uint8_t endpoint);

/**
 * Get current duration of binary switch.
 *
 * This function must be implemented in the application code.
 * Called by binary switch command class handler.
 *
 * @param[in] endpoint binary switch endpoint
 * @return remaining duration before target value is reached
 *         (encoded according to Binary Switch CC specification)
 */
extern uint8_t appBinarySwitchGetDuration(uint8_t endpoint);

/**
 * Set target value and duration for binary switch
 *
 * This function must be implemented in the application code.
 * Called by binary switch command class handler on incoming set command.
 *
 * @param[in] target_value requested switch value
 * @param[in] duration switch transition duration (encoded according
 *            to Binary Switch CC specification)
 * @param[in] endpoint is the destination endpoint
 * @return command handler return code
 */
extern e_cmd_handler_return_code_t appBinarySwitchSet(
  CMD_CLASS_BIN_SW_VAL target_value,
  uint8_t duration,
  uint8_t endpoint);

/**
 * Send report when change happen via lifeLine.
 *
 * Callback used by TSE module. Refer to @ref ZAF_TSE.h for more details.
 *
 * @param txOptions txOptions
 * @param pData Command payload for the report
 */
void CC_BinarySwitch_report_stx(
    TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions,
    void* pData);

/**
 * Prepare the data input for the TSE for any Binary Switch CC command based on the pRxOption pointer.
 * @param pRxOpt pointer used to indicate how the frame was received (if any) and what endpoints are affected
*/
extern void* CC_BinarySwitch_prepare_zaf_tse_data(RECEIVE_OPTIONS_TYPE_EX* pRxOpt);

#endif /* _CC_BINARY_SWITCH_H_ */
