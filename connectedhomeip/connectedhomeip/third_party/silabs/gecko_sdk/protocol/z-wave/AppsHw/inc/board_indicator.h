/**
 * @file
 *
 * Board Indicator handling definitions and functions.
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef BOARD_INDICATOR_H_
#define BOARD_INDICATOR_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Board status type
 */
typedef enum
{
  BOARD_STATUS_IDLE,
  BOARD_STATUS_POWER_DOWN,
  BOARD_STATUS_LEARNMODE_ACTIVE,
  BOARD_STATUS_LEARNMODE_INACTIVE,
} board_status_t;

/**
 * Indicates if board is in learn mode using LED_D0.
 * @param[in]    status    Status of board. BOARD_STATUS_LEARNMODE_ACTIVE => (LED_DO = ON)
 */
void Board_IndicateStatus(board_status_t status);

/**
 * Configure/initialize the indicator LED
 *
 * @note
 *   This function must *not* be called until *after* the queue to the
 *   protocol thread has been created (the indicator uses a power lock
 *   that is initialized by calling zpal_pm_register() which sends a
 *   message to the PM module in the protocol thread).
 */
void Board_IndicatorInit(void);

/**
 * Control the indicator LED.
 *
 * Even though on/off time parameters are given in milliseconds, the
 * resolution is 1/10'th of a second.
 *
 * @param on_time_ms  ON duration (in milliseconds) for a single blink cycle.
 *                    If on_time_ms is zero the indicator should be turned off.
 *
 * @param off_time_ms OFF duration (in milliseconds) for a single blink cycle.
 *
 * @param num_cycles  Number of blink cycles. If num_cycles is zero the indicator
 *                    LED should blink "forever" or until the next time this
 *                    function is called.
 *
 * @param called_from_indicator_cc Set to true if called from indicator command class.
 *                                 This flag is used to not have @ref Board_IsIndicatorActive()
 *                                 report to the indicator command class that the indicator
 *                                 is active while blinking for any other reason (e.g. the
 *                                 indicator led is also used to signal learn mode).
 *
 * @return false if parameters out of range or indicator not initialized.
 */
bool Board_IndicatorControl(uint32_t on_time_ms,
                            uint32_t off_time_ms,
                            uint32_t num_cycles,
                            bool called_from_indicator_cc);

/**
 * Reports if the indicator LED has been set active by the indicator
 * command class
 *
 * @return true   if indicator LED is currently blinking and has been
 *                told so by the indicator command class.
 *         false  if the indicator LED is active for any other reason
 *                or not active at all
 */
bool Board_IsIndicatorActive(void);




#endif /* BOARD_INDICATOR_H_ */
