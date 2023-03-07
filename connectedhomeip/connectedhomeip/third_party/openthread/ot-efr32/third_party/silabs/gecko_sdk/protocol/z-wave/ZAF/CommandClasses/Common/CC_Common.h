/**
 * @file
 * Common types and definitions for all command classes.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef PRODUCTPLUS_COMMANDCLASS_COMMON_H_
#define PRODUCTPLUS_COMMANDCLASS_COMMON_H_

#include <stdint.h>
#include <ZW_typedefs.h>
#include "ZAF_types.h"

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

JOB_STATUS cc_engine_multicast_request(const AGI_PROFILE* pProfile,
                                       uint8_t endpoint,
                                       CMD_CLASS_GRP *pcmdGrp,
                                       uint8_t* pPayload,
                                       uint8_t size,
                                       uint8_t fSupervisionEnable,
                                       void (*pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult));

#endif /* PRODUCTPLUS_COMMANDCLASS_COMMON_H_ */
