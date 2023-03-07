/**
 * @file
 * Actuator module. Manages all requests that transit from starting state to the end state during given time period.
 *
 * @copyright 2020 Silicon Laboratories Inc.
 *
 * ### Description
 * ZAF_Actuator module can be used by any actuator command class. In current implementation, it's been used by
 * Color Switch and Multilevel Switch command classes.
 * Its purpose is to simplify implementation of actuator command classes,
 * as well to provide single peace of code that will manage timed change on device.
 * ZAF_Actuator is responsible for triggering any timed change, dealing with timers, stopping the change if needed, etc.
 * In addition, this module knows data such as current value, target value, duration...
 *
 * #### Using the module
 *
 * Command class that uses ZAF_Actuator module must contain s_Actuator data type.
 * Before usage, ZAF_Actuator must be initialized by calling @ref ZAF_Actuator_Init.
 *
 * @startuml
 * title Initialization
 * skinparam handwritten true
 *
 * participant App #yellow
 * participant CommandClass #orange
 * participant Actuator #violet
 *
 * rnote over App #Lightgreen: Initialized array of switches\n driver functions available
 * App->CommandClass: CC_Switch_Init
 * rnote over CommandClass #Lightblue: Saving local data\nCalling Actuator_Init() with\n CC-specific parameters
 * CommandClass->Actuator: ZAF_Actuator_init
 * rnote over Actuator #white: all swithes saved\n and ready to use
 * @enduml
 *
 * Actuator takes care of timers used when there is a timed change in progress, so that Command Class
 * or Application don't have to.
 *
 * #### Invoking Actuator
 * @startuml
 * title Handling_Set_command
 * skinparam shadowing false
 *
 * skinparam SequenceGroupBorderColor brown
 * skinparam SequenceGroupBorderThickness 1
 *
 * participant App #gold
 * participant CommandClass #orange
 * participant Actuator #darkorange
 * participant Device #chocolate
 *
 * ->App: Incoming Switch SET (value, duration)
 * activate App
 * App->CommandClass: CC_Switch_Set(pSwitch, value, duration)
 * deactivate App
 * activate CommandClass
 * CommandClass->Actuator: ZAF_Actuator_Set(pActuator, value, duration)
 * deactivate CommandClass
 * activate Actuator
 * loop #snow If duration>0 Repeat this until target value is reached
 *   Actuator-->Actuator:Trigger timer
 *   Actuator->CommandClass: trigger CC_callback
 *   deactivate Actuator
 *   activate CommandClass
 *   CommandClass->App: Trigger App callback
 *   deactivate CommandClass
 *   activate App
 *   App->Device: trigger driver
 * end
 * deactivate App
 * @enduml
 *
 */


#ifndef ZAF_ACTUATOR_H
#define ZAF_ACTUATOR_H

#include <SwTimer.h>

struct actuator;
typedef void(*zaf_actuator_callback_t)(struct actuator *pActuator);

/**
 * The structure of Actuator module
 */
typedef struct actuator {
    uint32_t defaultRefreshRate; ///< Step/rate counted in milliseconds - how often app refresh its state
                                 ///< when transitions from current to target value. Used as timeout in timer
    uint32_t refreshRate;        ///< Adjusted refreshRate if needed, greater than defaultRefreshRate
    uint8_t min;                 ///< Minimum value that object can have
    uint8_t max;                 ///< Maximum value that object can have
    SSwTimer timer;              ///< Timer used while transitioning from one state to another
    uint16_t valueCurrent;       ///< Current value of object. Internally stored as 10 times higher to get
                                 ///< more accurate change.
                                 ///< @warning Always use @ref ZAF_Actuator_GetCurrentValue() to get current value
    uint16_t valueTarget;        ///< End value. Internally stored as 10 times higher to match current value format
                                 ///< @warning Always use @ref ZAF_Actuator_GetTargetValue() to get target value
    zaf_actuator_callback_t cc_cb;         ///< Pointer to function called when CC needs to take an action
    uint16_t singleStepValue;    ///< Internal value used to increase/decrease valueCurrent during timed changes
    bool directionUp;            ///< Internal value, determines should valueCurrent be increasing or decreasing
    uint8_t lastOnValue;         ///< Last ON value before setting to OFF (when applicable)
    uint8_t durationDefault;     ///< Factory default duration, used in timed changes, when no other duration was given
} s_Actuator;

/**
 * Return type for Actuator functions that can trigger changes that require some time to be done.
 */
typedef enum _eActuatorState{
  EACTUATOR_NOT_CHANGING,  ///< EACTUATOR_NOT_CHANGING - Already in final state
  EACTUATOR_CHANGING,      ///< EACTUATOR_CHANGING - Not in final state
  EACTUATOR_FAILED         ///< EACTUATOR_FAILED - Requested change rejected
}eActuatorState;

/**
 * Initializes Actuator module
 *
 * @param pActuator Pointer to Actuator to be initialized
 * @param minValue Minimum possible value
 * @param maxValue Maximum possible value
 * @param refreshRate Minimal refresh rate in milliseconds. Tells how often the application should refresh it's state
 *                    during timed changes. Actuator might adjust it to higher value, when number of steps
 *                    between current and target value is low.
 * @param durationDefault Factory default duration
 * @param cc_callback Pointer to function in 'user' command class
 */
void ZAF_Actuator_Init(s_Actuator *pActuator,
                       uint8_t minValue,
                       uint8_t maxValue,
                       uint16_t refreshRate,
                       uint8_t durationDefault,
                       zaf_actuator_callback_t cc_callback);
/**
 * Sets target value to @p value in actuator pointed by @p pActuator, during @p duration period
 *
 * @param[in,out] pActuator Pointer to Actuator that will be set to new value
 * @param[in] value         New target value
 * @param[in] duration      Time period that change should take. Actual time spent depends on number of steps between
 *                          current and target value and refreshRate. Requested and actual time may differ slightly.
 * @return                  EACTUATOR_NOT_CHANGING if already on target value
 *                          EACTUATOR_CHANGING if the change has successfully started
 *                          EACTUATOR_FAILED if request was invalid
 */
eActuatorState ZAF_Actuator_Set(s_Actuator *pActuator, uint8_t value, uint8_t duration);

/**
 * Starts the change on actuator pointed by  @p pActuator
 * @param[in,out] pActuator    Pointer to object whose value will be updated
 * @param[in] ignoreStartLevel Should startLevel be ignored. If yes, change starts from current level.
 * @param[in] upDown           Direction of change. FALSE for UP, TRUE for DOWN.
 * @param[in] startLevel       If not ignored, color will be first set to this level and then change will start
 * @param[in] duration         Time period that change should take. Actual time spent depends on number of steps between
 *                             current and target value and refreshRate. Requested and actual time may differ slightly.
 * @return                     EACTUATOR_NOT_CHANGING if already on target value
 *                             EACTUATOR_CHANGING if the change has successfully started
 */
eActuatorState ZAF_Actuator_StartChange(s_Actuator *pActuator,
                                        bool ignoreStartLevel,
                                        bool upDown,
                                        uint8_t startLevel,
                                        uint8_t duration);

/**
 * Stops ongoing change on actuator pointed by @p pActuator
 * @param pActuator Pointer to object whose change should be stopped
 * @return EACTUATOR_CHANGING if there was an ongoing change that was stopped, or else EACTUATOR_NOT_CHANGING
 */
eActuatorState ZAF_Actuator_StopChange(s_Actuator *pActuator);

/**
 * Returns current value.
 * @param pActuator Pointer to Actuator struct for which current value was requested
 * @return Current value
 */
uint8_t ZAF_Actuator_GetCurrentValue(s_Actuator *pActuator);

/**
 * Returns target value.
 * @param pActuator Pointer to Actuator struct for which target value was requested
 * @return Target value
 */
uint8_t ZAF_Actuator_GetTargetValue(s_Actuator *pActuator);

/**
 * Returns estimated time until target value is reached
 * @param pActuator Pointer to Actuator struct for which remaining duration was requested
 * @return Remaining duration in format described by requirement CC:0000.00.00.11.015 or 0 if already at target value
 */
uint8_t ZAF_Actuator_GetDurationRemaining(s_Actuator *pActuator);

/**
 * Gets last ON value. If lastOnValue was never set, it returns zero.
 * Then the caller has to use default ON value.
 * @param pActuator Pointer to Actuator struct for which last On value was requested
 * @return lastOnValue
 */
uint8_t ZAF_Actuator_GetLastOnValue(s_Actuator *pActuator);

/**
 * Converts coded duration into milliseconds format, refer to CC:0000.00.00.11.015
 * @param duration Coded duration
 * @return duration in milliseconds
 */
uint32_t getDurationInMs(uint8_t duration);

/**
 * Returns value defined as maximum for @p pActuator
 * @param pActuator Pointer to actuator
 * @return Value of max
 */
uint8_t ZAF_Actuator_GetMax(s_Actuator *pActuator);

#endif /* ZAF_ACTUATOR_H */
