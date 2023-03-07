/**
 * @file
 * Controlling functions for CC Multilevel Switch.
 *
 * These functions are intended for use by an application that would like to control another
 * device using CC Multilevel Switch. An example of such a device could be a Wall Controller / Wall
 * Switch.
 *
 * @copyright 2020 Silicon Laboratories Inc.
 */

#ifndef _CC_MULTILEVEL_SWITCH_H_
#define _CC_MULTILEVEL_SWITCH_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <CC_Common.h>
#include <ZAF_types.h>
#include <ZW_TransportEndpoint.h>

/**
 * Enumeration for "Start Level Change" command.
 */
typedef enum
{
  CCMLS_PRIMARY_SWITCH_UP,              //!< CCMLS_PRIMARY_SWITCH_UP
  CCMLS_PRIMARY_SWITCH_DOWN,            //!< CCMLS_PRIMARY_SWITCH_DOWN
  CCMLS_PRIMARY_SWITCH_RESERVED,        //!< CCMLS_PRIMARY_SWITCH_RESERVED
  CCMLS_PRIMARY_SWITCH_NO_UP_DOWN_MOTION//!< CCMLS_PRIMARY_SWITCH_NO_UP_DOWN_MOTION
}
CCMLS_PRIMARY_SWITCH_T;

/**
 * Enumeration for "Start Level Change" command.
 */
typedef enum
{
  CCMLS_IGNORE_START_LEVEL_FALSE,//!< CCMLS_IGNORE_START_LEVEL_FALSE
  CCMLS_IGNORE_START_LEVEL_TRUE  //!< CCMLS_IGNORE_START_LEVEL_TRUE
}
CCMLS_IGNORE_START_LEVEL_T;

/**
 * Enumeration for "Start Level Change" command.
 */
typedef enum
{
  CCMLS_SECONDARY_SWITCH_INCREMENT,//!< CCMLS_SECONDARY_SWITCH_INCREMENT
  CCMLS_SECONDARY_SWITCH_DECREMENT,//!< CCMLS_SECONDARY_SWITCH_DECREMENT
  CCMLS_SECONDARY_SWITCH_RESERVED, //!< CCMLS_SECONDARY_SWITCH_RESERVED
  CCMLS_SECONDARY_SWITCH_NO_INC_DEC//!< CCMLS_SECONDARY_SWITCH_NO_INC_DEC
}
CCMLS_SECONDARY_SWITCH_T;

/****************************************************************************/
/*                         CONTROLLING FUNCTIONS                            */
/****************************************************************************/

/**
 * @brief Initiates the transmission of a "Multilevel Switch Start Level Change"
 * command.
 * @param[in] pProfile pointer to AGI profile
 * @param[in] sourceEndpoint source endpoint
 * @param[out] pCbFunc Callback function to be called when transmission is done/failed.
 * @param[in] primarySwitch Controls the primary device functionality.
 * @param[in] fIgnoreStartLevel Ignore start level.
 * @param[in] secondarySwitch Controls the secondary device functionality.
 * @param[in] primarySwitchStartLevel Start level for the primary device functionality.
 * @param[in] duration The duration from lowest to highest value.
 * @param[in] secondarySwitchStepSize Step size for secondary device functionality.
 * @return Status of the job.
 */
JOB_STATUS
CmdClassMultilevelSwitchStartLevelChange(
  AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult),
  CCMLS_PRIMARY_SWITCH_T primarySwitch,
  CCMLS_IGNORE_START_LEVEL_T fIgnoreStartLevel,
  CCMLS_SECONDARY_SWITCH_T secondarySwitch,
  uint8_t primarySwitchStartLevel,
  uint8_t duration,
  uint8_t secondarySwitchStepSize);

/**
 * @brief Initiates the transmission of a "Multilevel Switch Stop Level Change"
 * command.
 * @param[in] pProfile pointer to AGI profile
 * @param[in] sourceEndpoint source endpoint
 * @param[out] pCbFunc Callback function to be called when transmission is done/failed.
 * @return Status of the job.
 */
JOB_STATUS
CmdClassMultilevelSwitchStopLevelChange(
  AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult));

/**
 * @brief Initiates the transmission of a "Multilevel Switch Set" command.
 * @param[in] pProfile pointer to AGI profile
 * @param[in] sourceEndpoint source endpoint
 * @param[out] pCbFunc Callback function to be called when transmission is done/failed.
 * @param[in] value Multilevel value.
 * @param[in] duration The duration from current value to the new given value.
 * @return Status of the job.
 */
JOB_STATUS
CmdClassMultilevelSwitchSetTransmit(
  AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult),
  uint8_t value,
  uint8_t duration);

#endif
