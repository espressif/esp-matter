/**
 * @file
 * Handler for Command Class Device Reset Locally.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _CC_DEVICE_RESET_LOCALLY_H_
#define _CC_DEVICE_RESET_LOCALLY_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZAF_types.h>

/****************************************************************************/
/*                       PUBLIC TYPES and DEFINITIONS                       */
/****************************************************************************/

/****************************************************************************/
/*                            PUBLIC FUNCTIONS                              */
/****************************************************************************/

/**
 * @brief Transmission callback for CC_DeviceResetLocally_notification_tx().
 * 
 * @param pTransmissionResult Result of each transmission.
 */
extern void CC_DeviceResetLocally_done(TRANSMISSION_RESULT * pTransmissionResult);

/**
 * @brief Transmits a Device Reset Locally Notification.
 */
void CC_DeviceResetLocally_notification_tx();

#endif
