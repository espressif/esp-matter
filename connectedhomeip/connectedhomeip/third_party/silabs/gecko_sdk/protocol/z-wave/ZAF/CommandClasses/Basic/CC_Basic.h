/**
 * @file
 * Handler for Command Class Basic.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _CC_BASIC_H_
#define _CC_BASIC_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_classcmd.h>
#include <ZW_TransportEndpoint.h>
#include <agi.h>
#include <ZAF_TSE.h>

/**
 * Application code. Incoming command class Set call to set value in application endpoint
 * @param[in] val parmeter dependent of the application device class
 * @param[in] endpoint is the destination endpoint
 * @return command handler return code
 */
extern e_cmd_handler_return_code_t CC_Basic_Set_handler(
  uint8_t val,
  uint8_t endpoint
);


/**
 * Application code. Incoming command class Report call to set value in application endpoint
 * @param[in] endpoint is the destination endpoint
 * @return get application value to send in a report
 */
extern uint8_t CC_Basic_GetCurrentValue_handler( uint8_t endpoint );

/**
 * Return the value of an ongoing transition or the most recent transition.
 * @param[in] endpoint is the destination endpoint
 * @return target value.
 */
extern uint8_t CC_Basic_GetTargetValue_handler( uint8_t endpoint );

/**
 * Return the time needed to reach the Target Value at the actual transition rate.
 * @details Duration encoded as follow:
 * Duration      Description
 *  0x00           0 seconds. Already at the Target Value.
 *  0x01-0x7F      1 second (0x01) to 127 seconds (0x7F) in 1 second resolution.
 *  0x80-0xFD      1 minute (0x80) to 126 minutes (0xFD) in 1 minute resolution.
 *  0xFE           Unknown duration
 *  0xFF           Reserved
 * @param[in] endpoint is the destination endpoint
 * @return duration time.
 */
uint8_t CC_Basic_GetDuration_handler(uint8_t endpoint);

/**
 * Send unsolicited command class Basic report
 * @param[in] pProfile pointer to AGI profile
 * @param[in] sourceEndpoint source endpoint
 * @param[in] bValue Basic Report value
 * @param[out] pCbFunc callback function pointer returning status on job. Can be initialized to NULL.
 * @return status of the job of type JOB_STATUS
 */
JOB_STATUS CC_Basic_Report_tx(
  AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  uint8_t bValue,
  VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult));

/**
 * Sends an unsolicited Basic Set command.
 * @param[in] pProfile Pointer to an AGI profile defined in the config_app.h of the application.
 * @param[in] sourceEndpoint Source endpoint if the sending device use endpoints. Otherwise 0.
 * @param[in] bValue Value as defined in the specification of the command.
 * @param[out] pCbFunc Callback function pointer giving a result of the transmission.
 * @return Status on whether the transmission could be initiated.
 */
//@ [CC_Basic_Set_tx_ID]
JOB_STATUS CC_Basic_Set_tx(
  const AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  uint8_t bValue,
  void (*pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult));
//@ [CC_Basic_Set_tx_ID]


/**
 * Send report when change happen via lifeLine.
 *
 * Callback used by TSE module
 *
 * @param txOptions txOptions
 * @param pData Command payload for the report
 */
extern void CC_Basic_report_stx(TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions,void* pData);

/**
 * This function is used to return a pointer for input data for the TSE trigger.
 * @param pRxOpt pointer that will be included in the ZAF_TSE input data.
*/
extern void* CC_Basic_prepare_zaf_tse_data(RECEIVE_OPTIONS_TYPE_EX* pRxOpt);

/**
 * This function is used to notify the Application that the CC Basic Set
 * status is in a WORKING state. The application can subsequently make the TSE Trigger
 * using the information passed on from the rxOptions.
 * @param pRxOpt pointer used to when triggering the "working state"
*/
extern void CC_Basic_report_notifyWorking(RECEIVE_OPTIONS_TYPE_EX* pRxOpt);

#endif
