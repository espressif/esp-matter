/**
*
* @file
* Network management utility functions
*
* @copyright 2019 Silicon Laboratories Inc.
*
*/

#ifndef APPS_SERIALAPI_CMDS_NETWORK_MANAGEMENT_H_
#define APPS_SERIALAPI_CMDS_NETWORK_MANAGEMENT_H_

#include <stdint.h>
#include <ZW_application_transport_interface.h>

/**
 * Command to set the maximum inclusion request interval.
 *
 * A status message with type=EZWAVECOMMANDSTATUS_ZW_SET_MAX_INCL_REQ_INTERVALS
 * is returned to the application command status queue when the command has
 * completed.
 *
 * @param intervals The maximum number of seconds in units of 128sec/tick in between
 * SmartStart inclusion requests. Valid range 0 and 5-99.
 * 0 is default value and corresponds to 512 sec.
 * The range 5-99 corresponds to 640-12672sec.
 */
void ZAF_SetMaxInclusionRequestIntervals(uint32_t intervals);

/**
 * Transmits an Included Node Information Frame (INIF).
 * @param pCallback Will be invoked when transmission is successful or failed.
 *                  NOTICE: The callback is not supported yet.
 */
void ZAF_SendINIF(void (*pCallback)(uint8_t txStatus, TX_STATUS_TYPE* extendedTxStatus));

#endif /* APPS_SERIALAPI_CMDS_NETWORK_MANAGEMENT_H_ */
