/**
 * @file
 * Handler for Command Class Multilevel Switch.
 *
 * This file contains functions for supporting CC Multilevel Switch. It is intended for
 * applications that is to be controlled by other devices using CC Multilevel Switch.
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _CC_MULTILEVEL_SWITCH_H_
#define _CC_MULTILEVEL_SWITCH_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_classcmd.h>
#include <CC_Common.h>
#include <ZAF_types.h>
#include <ZW_TransportEndpoint.h>
#include <ZAF_Actuator.h>
#include "ZAF_CC_Invoker.h"

struct cc_multilevel_switch_t; // Forward declaration

typedef void (*cc_multilevel_switch_callback_t)(struct cc_multilevel_switch_t * p_switch);

typedef struct cc_multilevel_switch_t {
  RECEIVE_OPTIONS_TYPE_EX rxOpt;            // Contains information required for Supervision and
                                            // True Status. Must be the first element in this
                                            // struct because TSE assumes this location.
  s_Actuator actuator;                      // Defined in ZAF_Actuator.h
  uint8_t endpoint;                         // The switch must be tied to an endpoint. (0 if no endpoints)
  cc_multilevel_switch_callback_t callback; // That will be invoked on changes of the current value.
                                            // Will only be invoked if no callback is passed to
                                            // cc_multilevel_switch_init().
} cc_multilevel_switch_t;

/**
 * Initializes the Multilevel Switch Command Class.
 * @param p_switches Array of switches.
 * @param switch_count Length of the array of switches.
 * @param default_duration Factory default duration. The value must be encoded like described in
 *                         requirement CC:0000.00.00.11.015 in SDS13781.
 * @param callback Function that is invoked everytime a value changes.
 */
void cc_multilevel_switch_init(cc_multilevel_switch_t * p_switches,
                               uint8_t switch_count,
                               uint8_t default_duration,
                               cc_multilevel_switch_callback_t callback);

/****************************************************************************/
/*                          EXTERNAL FUNCTIONS                              */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                           CC BASIC MAPPING                               */
/****************************************************************************/

/**
 * Handler for mapping Basic Set to Multilevel Switch Set.
 *
 * This function MUST NOT be used for anything else than mapping.
 * @param[in] value Mapped value.
 * @param[in] duration Mapped duration.
 * @param[in] endpoint Mapped endpoint.
 * @return Result of the set command. @ref E_CMD_HANDLER_RETURN_CODE_HANDLED if the actual value
 *         was set to the given value right away. @ref E_CMD_HANDLER_RETURN_CODE_WORKING if the
 *         actual value is transitioning to the given value. @ref E_CMD_HANDLER_RETURN_CODE_FAIL
 *         if the parameters were incorrect.
 */
e_cmd_handler_return_code_t cc_multilevel_switch_set_handler(uint8_t value,
                                                             uint8_t duration,
                                                             uint8_t endpoint);

/**
 * Returns the current value of the multilevel switch assigned with the given endpoint.
 *
 * This function MUST NOT be used for anything else than mapping.
 * @param endpoint Endpoint of switch.
 * @return Returns the current value.
 */
uint8_t cc_multilevel_switch_get_current_value_handler(uint8_t endpoint);

/**
 * Returns the target value of the multilevel switch assigned with the given endpoint.
 *
 * This function MUST NOT be used for anything else than mapping.
 * @param endpoint Endpoint of switch.
 * @return Returns the target value.
 */
uint8_t cc_multilevel_switch_get_target_value_handler(uint8_t endpoint);

/**
 * Returns the remaining duration of the multilevel switch assigned with the given endpoint.
 *
 * This function MUST NOT be used for anything else than mapping.
 * @param endpoint Endpoint of switch.
 * @return Returns the remaining duration.
 */
uint8_t cc_multilevel_switch_get_duration_handler(uint8_t endpoint);

/****************************************************************************/
/*                            LOCAL ACTUATION                               */
/****************************************************************************/

/**
 * Sets the value of a given multilevel switch.
 *
 * This function can be used in an application that might have a button.
 *
 * Must be used for local actuation only.
 *
 * For mapping to CC Basic see cc_multilevel_switch_set_handler().
 * @param p_switch The switch of which the value is set.
 * @param value The value to set.
 * @param duration The time it takes to reach the value.
 */
void cc_multilevel_switch_set(cc_multilevel_switch_t * p_switch, uint8_t value, uint8_t duration);

/**
 * Starts changing the value of a given multilevel switch.
 *
 * Must be used for local actuation only.
 * @param p_switch The multilevel switch of which the value is changed.
 * @param up The direction of the change. If true the value will decrease. If false, the value will
 *           increase.
 * @param ignore_start_level Decides whether the start level is ignored or not. If set to true,
 *                           the change will start from the current value. If set to false, the
 *                           value will start it's change from the given start level.
 * @param start_level The value to start from if ignore_start_level is set to false.
 * @param duration The duration of the change.
 */
void cc_multilevel_switch_start_level_change(cc_multilevel_switch_t * p_switch,
                                             bool up,
                                             bool ignore_start_level,
                                             uint8_t start_level,
                                             uint8_t duration);

/**
 * Stops an ongoing change of a given multilevel switch.
 *
 * If no change is ongoing, nothing happens.
 *
 * Must be used for local actuation only.
 * @param p_switch The multilevel switch of which the change will be stopped.
 */
void cc_multilevel_switch_stop_level_change(cc_multilevel_switch_t * p_switch);

/**
 * Returns the current value of a given multilevel switch.
 *
 * Must be used for local actuation only.
 * @param p_switch The multilevel switch of which the value is returned.
 * @return Current value of the given multilevel switch.
 */
uint8_t cc_multilevel_switch_get_current_value(cc_multilevel_switch_t * p_switch);

/**
 * Returns the last on-value of a given multilevel switch.
 *
 * Must be used for local actuation only.
 * @param p_switch The multilevel switch of which the last on-value is returned.
 * @return Last on value of the given multilevel switch.
 */
uint8_t cc_multilevel_switch_get_last_on_value(cc_multilevel_switch_t * p_switch);

#endif
