/***************************************************************************//**
 * @file
 * @brief This file contains the radio coexistence interface.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef __COEXISTENCE_H__
#define __COEXISTENCE_H__

// Include standard type headers to help define structures
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#ifdef TEST_COEX_HAL_CONFIG
  #include "coexistence-hal-config.h"
#endif //TEST_COEX_HAL_CONFIG

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/// The COEX plugin does not use enums because the ARM EABI leaves their
/// size ambiguous. This ambiguity causes problems if the application is built
/// with different flags than the library. Instead, uint8_t typedefs
/// are used in compiled code for all enums. For documentation purposes this is
/// converted to an actual enum since it's much easier to read in Doxygen.
#define COEX_ENUM(name) enum name
#define COEX_ENUM_GENERIC(name, type) enum name
#else
/// Define used for the COEX plugin, which sets each enum to a uint8_t
/// typedef and creates a named enum structure for the enumeration values.
#define COEX_ENUM(name) typedef uint8_t name; enum name##_enum
#define COEX_ENUM_GENERIC(name, type) typedef type name; enum name##_enum
// For debugging use the following define to turn this back into a proper enum
// #define COEX_ENUM(name) typedef enum name##_enum name; enum name##_enum
#endif

// Radio Coexistence Structures
// -----------------------------------------------------------------------------
/**
 * @addtogroup Radio_Coexistence Radio Coexistence
 * @{
 */

/**
 * @typedef COEX_GpioHandle_t
 * @brief User provided reference to a GPIO.
 */
typedef const void *COEX_GpioHandle_t;

/**
 * @enum COEX_GpioOptions_t
 * @brief Configuration options for an individual GPIO.
 */
COEX_ENUM(COEX_GpioOptions_t) {
  /** Shift position of \ref COEX_GPIO_OPTION_DEFAULT_ASSERTED bit */
  COEX_GPIO_OPTION_DEFAULT_ASSERTED_SHIFT = 0,
  /** Shift position of \ref COEX_GPIO_OPTION_INT_ASSERTED bit */
  COEX_GPIO_OPTION_INT_ASSERTED_SHIFT,
  /** Shift position of \ref COEX_GPIO_OPTION_INT_DEASSERTED bit */
  COEX_GPIO_OPTION_INT_DEASSERTED_SHIFT,
  /** Shift position of \ref COEX_GPIO_OPTION_SHARED bit */
  COEX_GPIO_OPTION_SHARED_SHIFT,
  /** Shift position of \ref COEX_GPIO_OPTION_OUTPUT bit */
  COEX_GPIO_OPTION_OUTPUT_SHIFT,
};

/** Value representing no options enabled. */
#define COEX_GPIO_OPTION_NONE 0U
/** GPIO asserted by default */
#define COEX_GPIO_OPTION_DEFAULT_ASSERTED (1U << COEX_GPIO_OPTION_DEFAULT_ASSERTED_SHIFT)
/** Trigger interrupt on GPIO asserted */
#define COEX_GPIO_OPTION_INT_ASSERTED (1U << COEX_GPIO_OPTION_INT_ASSERTED_SHIFT)
/** Trigger interrupt on GPIO deasserted */
#define COEX_GPIO_OPTION_INT_DEASSERTED (1U << COEX_GPIO_OPTION_INT_DEASSERTED_SHIFT)
/** GPIO is shared **/
#define COEX_GPIO_OPTION_SHARED (1U << COEX_GPIO_OPTION_SHARED_SHIFT)
/** GPIO is an output **/
#define COEX_GPIO_OPTION_OUTPUT (1U << COEX_GPIO_OPTION_OUTPUT_SHIFT)

/**
 * @enum COEX_GpioIndex_t
 * @brief COEX GPIO index enumeration.
 */
COEX_ENUM(COEX_GpioIndex_t) {
  COEX_GPIO_INDEX_NONE = 0,
  COEX_GPIO_INDEX_RHO = 1,
  COEX_GPIO_INDEX_REQ = 2,
  COEX_GPIO_INDEX_GNT = 3,
  COEX_GPIO_INDEX_PHY_SELECT = 4,
  COEX_GPIO_INDEX_WIFI_TX = 5,
  COEX_GPIO_INDEX_INTERNAL_REQ = 6,
  COEX_GPIO_INDEX_COUNT
};

/**
 * @struct COEX_GpioConfig_t
 * @brief Configuration structure for an individual GPIO.
 */
typedef struct COEX_GpioConfig{
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  COEX_GpioIndex_t index;
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  /** Function called when GPIO is toggled **/
  void (*cb)(void);

  /** GPIO configuration options **/
  COEX_GpioOptions_t options;
} COEX_GpioConfig_t;

/**
 * @enum COEX_Events_t
 * @brief COEX events bit mask.
 */
COEX_ENUM_GENERIC(COEX_Events_t, uint32_t) {
  /** Shift position of \ref COEX_EVENT_COEX_CHANGED bit */
  COEX_EVENT_COEX_CHANGED_SHIFT = 0,
  /** Shift position of \ref COEX_EVENT_REQUEST_RELEASED bit */
  COEX_EVENT_REQUEST_RELEASED_SHIFT,
  /** Shift position of \ref COEX_EVENT_REQUEST_DENIED bit */
  COEX_EVENT_REQUEST_DENIED_SHIFT,
  /** Shift position of \ref COEX_EVENT_GRANT_RELEASED bit */
  COEX_EVENT_GRANT_RELEASED_SHIFT,
  /** Shift position of \ref COEX_EVENT_PRIORITY_ASSERTED bit */
  COEX_EVENT_PRIORITY_ASSERTED_SHIFT,
  /** Shift position of \ref COEX_EVENT_HOLDOFF_CHANGED bit */
  COEX_EVENT_HOLDOFF_CHANGED_SHIFT,
  /** Shift position of \ref COEX_EVENT_TX_ABORTED bit */
  COEX_EVENT_TX_ABORTED_SHIFT,
  /** Shift position of \ref COEX_EVENT_PHY_SELECT_CHANGED bit */
  COEX_EVENT_PHY_SELECT_CHANGED_SHIFT,
  /** Shift position of \ref COEX_EVENT_WIFI_TX_CHANGED bit */
  COEX_EVENT_WIFI_TX_CHANGED_SHIFT
};

/** Value representing no events */
#define COEX_EVENT_NONE 0U
/** Radio coexistence has been enabled or disabled */
#define COEX_EVENT_COEX_CHANGED (1U << COEX_EVENT_COEX_CHANGED_SHIFT)
/** Request GPIO has been released */
#define COEX_EVENT_REQUEST_RELEASED (1U << COEX_EVENT_REQUEST_RELEASED_SHIFT)
/** Request GPIO has been denied */
#define COEX_EVENT_REQUEST_DENIED (1U << COEX_EVENT_REQUEST_DENIED_SHIFT)
/** Grant GPIO has been released */
#define COEX_EVENT_GRANT_RELEASED (1U << COEX_EVENT_GRANT_RELEASED_SHIFT)
/** Priority GPIO is asserted */
#define COEX_EVENT_PRIORITY_ASSERTED (1U << COEX_EVENT_PRIORITY_ASSERTED_SHIFT)
/** Radio transmits should be enabled or disabled */
#define COEX_EVENT_HOLDOFF_CHANGED (1U << COEX_EVENT_HOLDOFF_CHANGED_SHIFT)
/** The last transmit was aborted due to GRANT loss */
#define COEX_EVENT_TX_ABORTED (1U << COEX_EVENT_TX_ABORTED_SHIFT)
/** The COEX optimized PHY should be enabled or disabled */
#define COEX_EVENT_PHY_SELECT_CHANGED (1U << COEX_EVENT_PHY_SELECT_CHANGED_SHIFT)
/** The Wifi Tx GPIO is asserted or deasserted */
#define COEX_EVENT_WIFI_TX_CHANGED (1U << COEX_EVENT_WIFI_TX_CHANGED_SHIFT)
/** Events only needed if there is an active request **/
#define COEX_EVENT_REQUEST_EVENTS (COEX_EVENT_NONE               \
                                   | COEX_EVENT_REQUEST_RELEASED \
                                   | COEX_EVENT_REQUEST_DENIED   \
                                   | COEX_EVENT_GRANT_RELEASED   \
                                   | COEX_EVENT_PRIORITY_ASSERTED)
/** All Coexistence events */
#define COEX_EVENT_ALL_EVENTS (~COEX_EVENT_NONE)

/**
 * @enum COEX_Options_t
 * @brief COEX configuration options.
 */
COEX_ENUM_GENERIC(COEX_Options_t, uint32_t) {
  /** Shift position of \ref COEX_OPTION_MAX_REQ_BACKOFF bit */
  COEX_OPTION_MAX_REQ_BACKOFF_SHIFT = 0U,
  /** Shift position of \ref COEX_OPTION_REQ_SHARED bit */
  COEX_OPTION_REQ_SHARED_SHIFT = 4U,
  /** Shift position of \ref COEX_OPTION_PULSE_REQ_ON_RHO_RELEASE bit */
  COEX_OPTION_PULSE_REQ_ON_RHO_RELEASE_SHIFT,
  /** Shift position of \ref COEX_OPTION_PULSE_REQ_ON_GNT_RELEASE bit */
  COEX_OPTION_PULSE_REQ_ON_GNT_RELEASE_SHIFT,
  /** Shift position of \ref COEX_OPTION_TX_ABORT bit */
  COEX_OPTION_TX_ABORT_SHIFT,
  /** Shift position of \ref COEX_OPTION_PRI_SHARED bit */
  COEX_OPTION_PRI_SHARED_SHIFT,
  /** Shift position of \ref COEX_OPTION_RHO_ENABLED bit */
  COEX_OPTION_RHO_ENABLED_SHIFT,
  /** Shift position of \ref COEX_OPTION_COEX_ENABLED bit */
  COEX_OPTION_COEX_ENABLED_SHIFT,
  /** Shift position of \ref COEX_OPTION_PHY_SELECT_SHIFT bit */
  COEX_OPTION_PHY_SELECT_SHIFT,
  /** Shift position of \ref COEX_OPTION_HOLDOFF_ACTIVE_SHIFT bit */
  COEX_OPTION_HOLDOFF_ACTIVE_SHIFT
};

/** No coexistence options selected */
#define COEX_OPTION_NONE 0U
/** Maximum random backoff time(0 to 15 microseconds) before asserting request */
#define COEX_OPTION_MAX_REQ_BACKOFF_MASK (0x0FU << COEX_OPTION_MAX_REQ_BACKOFF_SHIFT)
/** Request GPIO is shared */
#define COEX_OPTION_REQ_SHARED (1U << COEX_OPTION_REQ_SHARED_SHIFT)
/** Pulse request when RHO GPIO is released */
#define COEX_OPTION_PULSE_REQ_ON_RHO_RELEASE (1U << COEX_OPTION_PULSE_REQ_ON_RHO_RELEASE_SHIFT)
/** Pulse request when GNT GPIO is released */
#define COEX_OPTION_PULSE_REQ_ON_GNT_RELEASE (1U << COEX_OPTION_PULSE_REQ_ON_GNT_RELEASE_SHIFT)
/** Abort any TX in progress if GNT GPIO is released */
#define COEX_OPTION_TX_ABORT (1U << COEX_OPTION_TX_ABORT_SHIFT)
/** Priority GPIO is shared */
#define COEX_OPTION_PRI_SHARED (1U << COEX_OPTION_PRI_SHARED_SHIFT)
/** Radio Holdoff is enabled */
#define COEX_OPTION_RHO_ENABLED (1U << COEX_OPTION_RHO_ENABLED_SHIFT)
/** Coexistence is enabled */
#define COEX_OPTION_COEX_ENABLED (1U << COEX_OPTION_COEX_ENABLED_SHIFT)
/** Coexistence optimized PHY is enabled */
#define COEX_OPTION_PHY_SELECT (1U << COEX_OPTION_PHY_SELECT_SHIFT)
/** Radio should not transmit */
#define COEX_OPTION_HOLDOFF_ACTIVE (1U << COEX_OPTION_HOLDOFF_ACTIVE_SHIFT)

/**
 * @enum COEX_Req_t
 * @brief COEX request radio enumeration.
 */
COEX_ENUM(COEX_Req_t) {
  /** Shift position of \ref COEX_REQ_ON bit */
  COEX_REQ_ON_SHIFT = 0U,
  /** Shift position of \ref COEX_REQ_HIPRI bit */
  COEX_REQ_HIPRI_SHIFT,
  /** Shift position of \ref COEX_REQ_FORCE bit */
  COEX_REQ_FORCE_SHIFT,
  /** Shift position of \ref COEX_REQCB_REQUESTED bit */
  COEX_REQCB_REQUESTED_SHIFT,
  /** Shift position of \ref COEX_REQCB_GRANTED bit */
  COEX_REQCB_GRANTED_SHIFT,
  /** Shift position of \ref COEX_REQCB_NEGATED bit */
  COEX_REQCB_NEGATED_SHIFT,
  /** Shift position of \ref COEX_REQCB_OFF bit */
  COEX_REQCB_OFF_SHIFT,
  /** Shift position of \ref COEX_REQ_PWM bit */
  COEX_REQ_PWM_SHIFT,
};

/** Disable both request and PWM request */
#define COEX_REQ_OFF 0U
/** Assert both request and PWM request */
#define COEX_REQ_ON (1U << COEX_REQ_ON_SHIFT)
/** Request is hi-pri */
#define COEX_REQ_HIPRI (1U << COEX_REQ_HIPRI_SHIFT)
/** Force assertion immediately */
#define COEX_REQ_FORCE (1U << COEX_REQ_FORCE_SHIFT)
/** Callback when REQUEST asserted */
#define COEX_REQCB_REQUESTED (1U << COEX_REQCB_REQUESTED_SHIFT)
/** Callback when GRANT asserted */
#define COEX_REQCB_GRANTED (1U << COEX_REQCB_GRANTED_SHIFT)
/** Callback when GRANT negated */
#define COEX_REQCB_NEGATED (1U << COEX_REQCB_NEGATED_SHIFT)
/** Callback when REQUEST removed */
#define COEX_REQCB_OFF (1U << COEX_REQCB_OFF_SHIFT)
/** Assert only PWM request when !COEX_REQ_ON */
#define COEX_REQ_PWM (1U << COEX_REQ_PWM_SHIFT)

/**
 * @typedef COEX_ReqCb_t
 * @brief User provided callbacks for radio coexistence (COEX)
 *        REQUEST and/or GRANT events.
 */
typedef void (*COEX_ReqCb_t)(COEX_Req_t coexStatus);

/**
 * @struct COEX_ReqState_t
 * @brief User provided callbacks for radio coexistence (COEX)
 *
 * This structure must be allocated in application global read-write memory
 * that persists for the duration of the COEX request. It cannot be allocated
 * in read-only memory or on the call stack.
 */
typedef struct COEX_ReqState {
  struct COEX_ReqState *next;
  volatile COEX_Req_t coexReq;
  COEX_ReqCb_t cb;
} COEX_ReqState_t;

/**
 * @struct COEX_PwmArgs_t
 * @brief PWM Coexistence configuration
 */
typedef struct COEX_PwmArgs{
  COEX_Req_t req;
  uint8_t dutyCycle;
  uint8_t periodHalfMs;
} COEX_PwmArgs_t;

/**
 * User provided random wait callback.
 *
 * @param delayMaskUs Value to mask random delay with.
 *
 * Wait a random period of time(0-0xFFFF microseconds)
 */
typedef void (*COEX_RandomDelayCallback_t)(uint16_t randomDelayMaskUs);

/**
 * User provided radio callback for radio coexistence (COEX).
 *
 * @param[in] events A bit mask of COEX events.
 *
 * See the \ref COEX_Events_t documentation for the list of COEX events.
 */
typedef void (*COEX_RadioCallback_t)(COEX_Events_t events);

/**
 * @struct COEX_HalCallbacks_t
 * @brief User provided HAL callbacks for radio coexistence (COEX)
 */
typedef struct COEX_HalCallbacks {
  /**
   * Set/clear the logical output of a GPIO.
   *
   * @param[in] gpioHandle A GPIO instance handle.
   * @param[in] enabled
   *  true - set the GPIO if it is active high, clear otherwise
   *  false - clear the GPIO if it is active high, set otherwise
   *
   */
  void (*setGpio)(COEX_GpioHandle_t gpioHandle, bool enabled);
  /**
   * Set/clear GPIO interrupt flag.
   *
   * @param[in] gpioHandle A GPIO instance handle.
   * @param[in] enabled
   *  true - set the GPIO interrupt flag
   *  false - clear the GPIO interrupt flag
   * @note The return value should be inverted if the GPIO is active low.
   */
  void (*setGpioFlag)(COEX_GpioHandle_t gpioHandle, bool enabled);
  /**
   * Enable/disable a GPIO interrupt.
   *
   * @param[in] gpioHandle A GPIO instance handle.
   * @param[in] enabled
   *  true - enable the GPIO interrupt
   *  false - disable the GPIO interrupt
   * @param[out] wasAsserted pointer to asserted flag.
   *  This flag is cleared before an interrupt is enabled.
   *  This can be used to prevent missing GPIO toggles.
   */
  void (*enableGpioInt)(COEX_GpioHandle_t gpioHandle,
                        bool enabled,
                        bool *wasAsserted);
  /**
   * Configure a GPIO.
   *
   * @param[in] gpioHandle A GPIO instance handle.
   * @param[in] config A pointer to GPIO configuration struct
   */
  void (*configGpio)(COEX_GpioHandle_t gpioHandle, COEX_GpioConfig_t *config);
  /**
   * Get the current logical output state of a GPIO.
   *
   * @param[in] gpioHandle A GPIO instance handle.
   * @param[in] defaultValue Value returned if GPIO output state is undefined.
   *
   * @return The current output state of GPIO.
   *
   * @note The return value should be inverted if the GPIO is active low.
   */
  bool (*isGpioOutSet)(COEX_GpioHandle_t gpioHandle, bool defaultValue);
  /**
   * Get the current logical input state(asserted/deasserted) of a GPIO.
   *
   * @param[in] gpioHandle A GPIO instance handle.
   * @param[in] defaultValue Value returned if GPIO input state is undefined.
   *
   * @return The current logical input state(asserted/deasserted) of GPIO.
   *
   * @note The return value should be inverted if the GPIO is active low.
   */
  bool (*isGpioInSet)(COEX_GpioHandle_t gpioHandle, bool defaultValue);
} COEX_HalCallbacks_t;

/**
 * User provided atomic callback.
 *
 * @param[in] args A generic argument passed to the atomic callback.
 */
typedef void (*COEX_AtomicCallback_t)(void *arg);

/**
 * Call a user provided function with interrupts disabled.
 *
 * @param[in] cb User provided callback called with interrupts disabled.
 * @param[in] args A generic argument passed to the atomic callback.
 */
void COEX_HAL_CallAtomic(COEX_AtomicCallback_t cb, void *args);

/**
 * Request permission to transmit from COEX master.
 *
 * @param[in] reqState Pointer to \ref COEX_ReqState_t structure.
 *                     This structure should be zero initialized before its first use.
 * @param[in] coexReq This parameter is either ON, OFF, PRIORITY or FORCED. PRIORITY AND FORCED can be
 *                    combined with ON and OFF.
 * @param[in] cb Callback fired when REQUEST is asserted.
 *
 * @return This function returns true if request was set, false otherwise.
 */
bool COEX_SetRequest(COEX_ReqState_t *reqState,
                     COEX_Req_t coexReq,
                     COEX_ReqCb_t cb);

/**
 * Request permission to transmit from COEX master.
 *
 * @return This function returns the current COEX pwm configuration.
 */
const COEX_PwmArgs_t *COEX_GetPwmRequest(void);

/**
 * Configure PWM COEX request.
 *
 * @param[in] coexReq This parameter is either ON, OFF, PRIORITY or FORCED. PRIORITY AND FORCED can be
 *                    combined with ON and OFF.
 * @param[in] cb Callback fired when REQUEST is asserted.
 *
 * @param[in] dutyCycle PTA PWM duty cycle percentage(0-100)
 * @param[in] periodHalfMs PTA PWM request period in half milliseconds
 * @return This function returns true if the PWM request was successfully configured, false otherwise.
 */
bool COEX_SetPwmRequest(COEX_Req_t coexReq,
                        COEX_ReqCb_t cb,
                        uint8_t dutyCycle,
                        uint8_t periodHalfMs);

/**
 * Configure the COEX request GPIO.
 *
 * @param[in] gpioHandle A GPIO instance handle.
 * @return This function returns true if the request GPIO
 *  was successfully configured, false otherwise.
 *
 * The request GPIO is asserted when the radio
 * needs to transmit.  The COEX master will assert
 * the grant GPIO if the radio can transmit.
 *
 * @note Pass NULL to disable the request GPIO.
 */
bool COEX_ConfigRequest(COEX_GpioHandle_t gpioHandle);

/**
 * Configure the COEX external request GPIO.
 *
 * @param[in] gpioHandle A GPIO instance handle.
 * @return This function returns true if the request GPIO
 *  was successfully configured, false otherwise.
 *
 * The external request GPIO is used with
 * \ref COEX_GetGpioInputOverride to simulate
 * a request from an external coexistence device.
 * This GPIO output can be used to debug shared request.
 *
 * @note Pass NULL to disable the external request GPIO.
 */
bool COEX_ConfigExternalRequest(COEX_GpioHandle_t gpioHandle);

/**
 * Configure the COEX PWM request GPIO.
 *
 * @param[in] gpioHandle A GPIO instance handle.
 * @return This function returns true if the request GPIO
 *  was successfully configured, false otherwise.
 *
 * The PWM request GPIO is asserted when the radio
 * needs to transmit.  The COEX master will assert
 * the grant GPIO if the radio can transmit.
 * Requesting on the shared PWM request GPIO does not
 * prevent other radios from requesting.
 * Requesting on the standard shared request GPIO blocks
 * other radios from requesting.
 *
 * @note Pass NULL to disable the PWM request GPIO.
 */
bool COEX_ConfigPwmRequest(COEX_GpioHandle_t gpioHandle);

/**
 * Configure the COEX grant GPIO.
 *
 * @param[in] gpioHandle A GPIO instance handle.
 * @return This function returns true if the grant GPIO
 *  was successfully configured, false otherwise.
 *
 * The grant GPIO is asserted by the COEX master
 * when a request is granted.  The radio will not
 * transmit until unless the grant GPIO is asserted.
 *
 * @note Pass NULL to disable the grant GPIO.
 */
bool COEX_ConfigGrant(COEX_GpioHandle_t gpioHandle);

/**
 * Configure the COEX priority GPIO.
 *
 * @param[in] gpioHandle A GPIO instance handle.
 * @return This function returns true if the priority GPIO
 *  was successfully configured, false otherwise.
 *
 * This GPIO is asserted when a high priority
 * transmission is needed.
 *
 * @note Pass NULL to disable the priority GPIO.
 */
bool COEX_ConfigPriority(COEX_GpioHandle_t gpioHandle);

/**
 * Configure the radio hold off GPIO.
 *
 * @param[in] gpioHandle A GPIO instance handle.
 * @return This function returns true if the radio hold off GPIO
 *  was successfully configured, false otherwise.
 *
 * The radio will not transmit if the radio hold off GPIO
 * is asserted.
 *
 * @note Pass NULL to disable the radio hold off GPIO.
 */
bool COEX_ConfigRadioHoldOff(COEX_GpioHandle_t gpioHandle);

/**
 * Configure the COEX PHY select GPIO.
 *
 * @param[in] gpioHandle A GPIO instance handle.
 * @return This function returns true if the PHY select GPIO
 *  was successfully configured, false otherwise.
 *
 * If the COEX PHY select GPIO is asserted,
 * the COEX optimized PHY will be enabled.
 *
 * @note Pass NULL to disable the COEX PHY select GPIO.
 */
bool COEX_ConfigPhySelect(COEX_GpioHandle_t gpioHandle);

/**
 * Set the COEX configuration options
 *
 * @param[in] options New COEX configuration options.
 */
bool COEX_SetOptions(COEX_Options_t options);

/**
 * Get the current COEX configuration options
 *
 * @return This function returns current COEX configuration
 *         set by \ref COEX_SetOptions.
 */
COEX_Options_t COEX_GetOptions(void);

/**
 * Check if COEX is enabled.
 *
 * @return True if COEX is enabled, false otherwise.
 */
bool COEX_IsEnabled(void);

/**
 * Update COEX grant state.
 *
 * This function should be called to reassess the COEX grant GPIO
 * before transmitting.
 * This is needed if the COEX grant GPIO is preempted by radio interrupts.
 */
void COEX_UpdateGrant(void);

/**
 * Set COEX HAL callback function pointers.
 *
 * @param[in] callbacks
 *   Pointer to struct of HAL callback function pointers.
 *
 * @note This function does not create a local copy of callbacks.
 *   Callbacks should be allocated in persistent memory and not the call stack.
 */
void COEX_SetHalCallbacks(const COEX_HalCallbacks_t * callbacks);

/**
 * Set COEX radio callback function pointers.
 *
 * @param[in] callbacks
 *   Pointer to struct of radio callback function pointers.
 *
 * @note This function does not create a local copy of callbacks.
 *   Callbacks should be allocated in persistent memory and not the call stack.
 */
void COEX_SetRadioCallback(COEX_RadioCallback_t callback);

/**
 * Set COEX random delay callback function pointers.
 *
 * @param[in] callbacks
 *   Pointer to struct of radio callback function pointers.
 *
 * @note This function does not create a local copy of callbacks.
 *   Callbacks should be allocated in persistent memory and not the call stack.
 */
void COEX_SetRandomDelayCallback(COEX_RandomDelayCallback_t callback);

/**
 * Notify COEX of radio power state.
 *
 * @param[in] powerUp radio is powered up if true; radio is powered down if false.
 */
void setCoexPowerState(bool powerUp);

/**
 * Initialize COEX options from HAL-CONFIG settings.
 */
void COEX_InitHalConfigOptions(void);

/**
 * Configure directional priority pulse width.
 *
 * @param[in] pulseWidthUs directional priority in microseconds.
 *   Set the pulse width to 0 disable directional priority.
 * @return This function returns true if directional priority pulse width
 *  was successfully configured, false otherwise.
 */
bool COEX_SetDirectionalPriorityPulseWidth(uint8_t pulseWidthUs);

/**
 * Get directional priority pulse width.
 *
 * @return This function returns the directional priority
 *  pulse width in microseconds.
 */
uint8_t COEX_GetDirectionalPriorityPulseWidth(void);

/**
 * Get GPIO input override value.
 *
 * @param[in] gpioIndex \ref COEX_GpioIndex_t of COEX GPIO.
 * @return This function returns the GPIO input override
 *  value.  The return is inverted if the selected GPIO is active low.
 */
bool COEX_GetGpioInputOverride(COEX_GpioIndex_t gpioIndex);

/**
 * Set GPIO input override value.
 *
 * @param[in] gpioIndex \ref COEX_GpioIndex_t input value to override.
 * @param[in] enabled value to return when selected GPIO input is read.
 *   Pass an inverted value if the selected GPIO is active low.
 * @return Return true if the override value was successfully set.
 * @note Calling this function will trigger a GPIO interrupt.
 */
bool COEX_SetGpioInputOverride(COEX_GpioIndex_t gpioIndex, bool enabled);

/**
 * Enable/disable COEX PHY select interrupt.
 *
 * @param[in] enable if true, enable PHY select interrupt; else disable interrupt.
 */
void COEX_EnablePhySelectIsr(bool enable);

/**
 * Enable/disable COEX Wifi Tx interrupt.
 *
 * @param[in] enable if true, enable Wifi Tx interrupt; else disable interrupt.
 */
void COEX_EnableWifiTxIsr(bool enable);

/**
 * Coexistence event handler.
 *
 * @param[in] Coexistence event.
 */
void COEX_on_event(COEX_Events_t events);

/**
 * @}
 * end of COEX_API
 */

#ifdef __cplusplus
}
#endif

#endif  // __COEXISTENCE_H__
