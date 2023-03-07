/***************************************************************************//**
 * @file
 * @brief Coexistence support for Bluetooth
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

#ifndef SL_BT_COEX_H
#define SL_BT_COEX_H

#include "sl_rail_util_coex_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Coexistence interface
 */

typedef struct {
  uint32_t options;
  uint16_t requestWindow;
  uint16_t requestWindowCalibration;
} sl_bt_coex_init_t;

//Compatibility defines for TX abort
#ifndef SL_RAIL_UTIL_COEX_BLE_TX_ABORT
#define SL_RAIL_UTIL_COEX_BLE_TX_ABORT SL_RAIL_UTIL_COEX_TX_ABORT
#endif

//Compatibility defines if Tx and Rx defined separately
#ifndef SL_BT_COEX_PRIORITY_DEFAULT
#if defined(HAL_COEX_TX_HIPRI) && defined(HAL_COEX_RX_HIPRI)
#define SL_BT_COEX_PRIORITY_DEFAULT (HAL_COEX_TX_HIPRI | HAL_COEX_RX_HIPRI)
#endif
#endif

// Request window calibration value takes into account the hardware and
// software delayes which may delay the request signal
#ifndef SL_BT_COEX_REQ_WINDOW_CALIBRATION
#if (HAL_COEX_REQ_SHARED)
#define SL_BT_COEX_REQ_WINDOW_CALIBRATION   89
#else
#define SL_BT_COEX_REQ_WINDOW_CALIBRATION   87
#endif
#endif

#define SL_BT_COEXINIT_DEFAULT {                        \
    0, /* configuration options */                      \
    SL_RAIL_UTIL_COEX_REQ_WINDOW,  /* request window */ \
    SL_BT_COEX_REQ_WINDOW_CALIBRATION,                  \
}

#define SL_BT_COEX_OPTION_REQUEST_BACKOFF_SHIFT 0
#define SL_BT_COEX_OPTION_REQUEST_BACKOFF_MASK  0xff
#define SL_BT_COEX_OPTION_REQUEST_BACKOFF_DEFAULT \
  SL_RAIL_UTIL_COEX_REQ_BACKOFF
#define SL_BT_COEX_OPTION_ENABLE_SHIFT          8
#define SL_BT_COEX_OPTION_TX_ABORT_SHIFT        10
#define SL_BT_COEX_OPTION_PRIORITY_ENABLE_SHIFT 11
#define SL_BT_COEX_OPTION_PWM_PRIORITY_SHIFT    12
#define SL_BT_COEX_OPTION_SCANPWM_ENABLE_SHIFT      13
#define SL_BT_COEX_OPTION_PWM_ENABLE_SHIFT      14

/* Coex is enabled*/
#define SL_BT_COEX_OPTION_ENABLE                (1 << SL_BT_COEX_OPTION_ENABLE_SHIFT)
/* Abort transmission if grant is lost*/
#define SL_BT_COEX_OPTION_TX_ABORT              (1 << SL_BT_COEX_OPTION_TX_ABORT_SHIFT)
/* Priority signal toggling is enabled */
#define SL_BT_COEX_OPTION_PRIORITY_ENABLE       (1 << SL_BT_COEX_OPTION_PRIORITY_ENABLE_SHIFT)
/* Priority signal is toggled with Pwm */
#define SL_BT_COEX_OPTION_PWM_PRIORITY          (1 << SL_BT_COEX_OPTION_PWM_PRIORITY_SHIFT)
/* SCANPWM toggling is enabled */
#define SL_BT_COEX_OPTION_SCANPWM_ENABLE            (1 << SL_BT_COEX_OPTION_SCANPWM_ENABLE_SHIFT)
/* PWM toggling is enabled */
#define SL_BT_COEX_OPTION_PWM_ENABLE            (1 << SL_BT_COEX_OPTION_PWM_ENABLE_SHIFT)

#define SL_BT_COEX_OPTION_MASK                  0x0000ff00
#define SL_BT_COEX_OPTION_REQUEST_WINDOW_SHIFT  16
#define SL_BT_COEX_OPTION_REQUEST_WINDOW_MASK   0xffff0000

void sl_bt_init_coex(const sl_bt_coex_init_t *coexInit);
void sl_bt_class_coex_init(void);
static inline void sl_bt_init_coex_hal(void)
{
//#if (HAL_COEX_ENABLE)
// Initialise coexistence interface
  sl_bt_coex_init_t coexInit = {
    .options = SL_BT_COEX_OPTION_ENABLE | SL_BT_COEX_OPTION_REQUEST_BACKOFF_DEFAULT,
    .requestWindow = SL_RAIL_UTIL_COEX_REQ_WINDOW,
    .requestWindowCalibration = SL_BT_COEX_REQ_WINDOW_CALIBRATION,
  };

  // COEX options
  coexInit.options |= SL_RAIL_UTIL_COEX_BLE_TX_ABORT ? SL_BT_COEX_OPTION_TX_ABORT : 0;
  coexInit.options |= SL_RAIL_UTIL_COEX_PRIORITY_DEFAULT ? SL_BT_COEX_OPTION_PRIORITY_ENABLE : 0;
#if SL_RAIL_UTIL_COEX_PWM_REQ_ENABLED
  coexInit.options |= SL_RAIL_UTIL_COEX_PWM_PRIORITY ? SL_BT_COEX_OPTION_PWM_PRIORITY : 0;
  coexInit.options |= SL_RAIL_UTIL_COEX_SCANPWM_DEFAULT_ENABLED ? SL_BT_COEX_OPTION_SCANPWM_ENABLE : 0;
  coexInit.options |= SL_RAIL_UTIL_COEX_PWM_DEFAULT_ENABLED ? SL_BT_COEX_OPTION_PWM_ENABLE : 0;
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_ENABLED

  sl_bt_init_coex(&coexInit);

  sl_bt_class_coex_init();

//#endif // HAL_COEX_ENABLE
}

/**
 * Set coexistence options
 *
 * @param mask options to configure
 * @param options enable/disable features
 *
 * @return true Parameters was set
 * @return false Operation failed, this can happen if command is used through BGAPI and coex is not linked in
 */
bool sl_bt_set_coex_options(uint32_t mask, uint32_t options);

typedef struct {
  uint8_t threshold_coex_pri; /** Priority line is toggled if priority is below this*/
  uint8_t threshold_coex_req; /** Coex request is toggled if priority is below this*/
  uint8_t coex_pwm_period;    /** PWM Period in ms, if 0 Pwm is disabled*/
  uint8_t coex_pwm_dutycycle; /** PWM dutycycle percentage, if 0 pwm is disabled, if >= 100 scanPwm line is always enabled*/
} sl_bt_ll_coex_config_t;
#define SL_BT_COEX_DEFAULT_CONFIG { 175, 255, (SL_RAIL_UTIL_COEX_PWM_REQ_PERIOD + 1) / 2, SL_RAIL_UTIL_COEX_PWM_REQ_DUTYCYCLE }

/**
 * Update coex configuration
 *
 */
void sl_bt_coex_set_config(sl_bt_ll_coex_config_t * config);

// Interval functions for debugging purposes
void sl_bt_enable_coex_pull_resistor(bool enable);

/* Coex counters interface */
/**
 * Get coex counter data structure
 *
 * @param ptr returns pointer to coex structure
 * @param size returns size of coex structure
 * if either ptr or size is NULL, coex data structure is cleared
 *
 * @return true Parameters was set
 * @return false Operation failed, this can happen if command is used through BGAPI and coex is not linked in
 */

bool sl_bt_get_coex_counters(const void **ptr, uint8_t *size);

/**
 * Called internally by coex to update data structure
 *
 */
void sli_bt_coex_counter_request(bool request, bool priority);
void sli_bt_coex_counter_grant_update(bool state);
void sli_bt_coex_counter_increment_denied(void);

#ifdef __cplusplus
}
#endif

#endif
