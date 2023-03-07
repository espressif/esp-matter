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

#ifndef SL_BT_COEX_LL_H
#define SL_BT_COEX_LL_H

#include <stdbool.h>
#include <rail_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Set request window
 * @param request window in us
 */
void sl_bt_ll_coex_set_request_window(uint16_t requestWindow);

/**
 * Inform COEX code that grant signal state is changed.
 */
void sl_bt_ll_coex_update_grant(bool allowAbort);

/**
 * Set COEX request and priority signals immediately.
 * @param request state of request signal
 * @param pwmEnable Enable pwm toggling of request line
 * @param priority priority level of request
 */
void sl_bt_ll_coex_request(bool request, bool pwmEnable, uint8_t priority);

/**
 * Set COEX request and priority signals at given time. Signals are set
 * immediately if time is already past.
 * @param time setting time in RAIL time
 * @param request state of request signal
 * @param priority priority level of request
 */
void sl_bt_ll_coex_request_delayed(uint32_t time, bool request, bool pwmEnable, uint8_t priority);

typedef void(* sl_bt_coex_abort_tx_callback)(void);
typedef uint16_t(* sl_bt_coex_fast_random_callback)(void);
/**
 * Set RAIL handle and callbacks for coex to use
 *
 */
void sl_bt_ll_coex_set_context(RAIL_Handle_t railHandle, sl_bt_coex_abort_tx_callback abortTx, sl_bt_coex_fast_random_callback fastRandom);

/**
 * Get random number quickly
 *
 */
uint16_t sl_bt_ll_coex_fast_random(void);

bool sl_bt_coex_tx_allowed(void);
/**
 * Get coex events that need to be enabled by the link layer.
 */
RAIL_Events_t sl_bt_ll_coex_get_events(void);

/**
 * Handle the coex events passed down by the link layer.
 */
void sl_bt_ll_coex_handle_events(RAIL_Events_t events);

#ifdef __cplusplus
}
#endif

#endif
