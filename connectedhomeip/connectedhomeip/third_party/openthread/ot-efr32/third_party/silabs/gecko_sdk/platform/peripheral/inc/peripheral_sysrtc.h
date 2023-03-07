/***************************************************************************//**
 * @file
 * @brief System Real Time Counter (SYSRTC) peripheral API
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef PERIPHERAL_SYSRTC_H
#define PERIPHERAL_SYSRTC_H

#include "em_device.h"
#if defined(SYSRTC_COUNT) && (SYSRTC_COUNT > 0)

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup sysrtc
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/// Minimum compare channels for SYSRTC group.
#define SYSRTC_GROUP_MIN_CHANNEL_COMPARE  1u
/// Maximum compare channels for SYSRTC group.
#define SYSRTC_GROUP_MAX_CHANNEL_COMPARE  2u

/// Minimum capture channels for SYSRTC group.
#define SYSRTC_GROUP_MIN_CHANNEL_CAPTURE  0u
/// Maximum capture channels for SYSRTC group.
#define SYSRTC_GROUP_MAX_CHANNEL_CAPTURE  1u

/// Sysrtc group number.
#ifdef _SILICON_LABS_32B_SERIES_2
#if !defined(SYSRTC_GROUP_NUMBER)
#define SYSRTC_GROUP_NUMBER   1u
#endif
#endif

/// Validation of valid SYSRTC group for assert statements.
#define SYSRTC_GROUP_VALID(group)    ((unsigned)(group) < SYSRTC_GROUP_NUMBER)

/*******************************************************************************
 *********************************   ENUM   ************************************
 ******************************************************************************/

/// Capture input edge select.
typedef enum {
  SL_SYSRTC_CAPTURE_EDGE_RISING = 0,  ///< Rising edges detected.
  SL_SYSRTC_CAPTURE_EDGE_FALLING,     ///< Falling edges detected.
  SL_SYSRTC_CAPTURE_EDGE_BOTH         ///< Both edges detected.
} sl_sysrtc_capture_edge_t;

/// Compare match output action mode.
typedef enum {
  SL_SYSRTC_COMPARE_MATCH_OUT_ACTION_CLEAR = 0, ///< Clear output.
  SL_SYSRTC_COMPARE_MATCH_OUT_ACTION_SET,       ///< Set output.
  SL_SYSRTC_COMPARE_MATCH_OUT_ACTION_PULSE,     ///< Generate a pulse.
  SL_SYSRTC_COMPARE_MATCH_OUT_ACTION_TOGGLE,    ///< Toggle output.
  SL_SYSRTC_COMPARE_MATCH_OUT_ACTION_CMPIF      ///< Export CMPIF.
} sl_sysrtc_compare_match_out_action_t;

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/// SYSRTC configuration structure.
typedef struct {
  bool enable_debug_run;      ///< Counter shall keep running during debug halt.
} sl_sysrtc_config_t;

/// Suggested default values for SYSRTC configuration structure.
#define SYSRTC_CONFIG_DEFAULT                        \
  {                                                  \
    false, /* Disable updating during debug halt. */ \
  }

/// Compare channel configuration structure.
typedef struct {
  sl_sysrtc_compare_match_out_action_t  compare_match_out_action; ///< Compare mode channel match output action.
} sl_sysrtc_group_channel_compare_config_t;

/// Capture channel configuration structure.
typedef struct {
  sl_sysrtc_capture_edge_t  capture_input_edge; ///< Capture mode channel input edge.
} sl_sysrtc_group_channel_capture_config_t;

/// Group configuration structure.
typedef struct {
  bool compare_channel0_enable;                                             ///< Enable/Disable compare channel 0
  bool compare_channel1_enable;                                             ///< Enable/Disable compare channel 1
  bool capture_channel0_enable;                                             ///< Enable/Disable capture channel 0
  sl_sysrtc_group_channel_compare_config_t const *p_compare_channel0_config; ///< Pointer to compare channel 0 config
  sl_sysrtc_group_channel_compare_config_t const *p_compare_channel1_config; ///< Pointer to compare channel 1 config
  sl_sysrtc_group_channel_capture_config_t const *p_capture_channel0_config; ///< Pointer to capture channel 0 config
} sl_sysrtc_group_config_t;

/// Suggested default values for compare channel configuration structure.
#define SYSRTC_GROUP_CHANNEL_COMPARE_CONFIG_DEFAULT \
  {                                                 \
    SL_SYSRTC_COMPARE_MATCH_OUT_ACTION_PULSE        \
  }

/// Suggested default values for capture channel configuration structure.
#define SYSRTC_GROUP_CHANNEL_CAPTURE_CONFIG_DEFAULT \
  {                                                 \
    SL_SYSRTC_CAPTURE_EDGE_RISING                   \
  }

/// Suggested default values for SYSRTC group configuration structure.
#define SYSRTC_GROUP_CONFIG_DEFAULT                                          \
  {                                                                          \
    true, /* Enable compare channel 0. */                                    \
    false, /* Disable compare channel 1. */                                  \
    false, /* Disable capture channel 0. */                                  \
    NULL, /* NULL Pointer to configuration structure for compare channel 0*/ \
    NULL, /* NULL Pointer to configuration structure for compare channel 1*/ \
    NULL /* NULL Pointer to configuration structure for capture channel 0*/  \
  }

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * Initializes SYSRTC module.
 *
 * Note that the compare values must be set separately with
 * (sl_sysrtc_set_group_compare_channel_value()), which should probably be
 * done prior to the use of this function if configuring the SYSRTC to start
 * when initialization is completed.
 *
 * @param[in] p_config  A pointer to the SYSRTC initialization structure
 *                      variable.
 ******************************************************************************/
void sl_sysrtc_init(const sl_sysrtc_config_t *p_config);

/***************************************************************************//**
 * Enables SYSRTC counting.
 ******************************************************************************/
void sl_sysrtc_enable(void);

/***************************************************************************//**
 * Disables SYSRTC counting.
 ******************************************************************************/
void sl_sysrtc_disable(void);

/***************************************************************************//**
 * Waits for the SYSRTC to complete all synchronization of register changes
 * and commands.
 ******************************************************************************/
__STATIC_INLINE void sl_sysrtc_wait_sync(void)
{
  while ((SYSRTC0->EN & SYSRTC_EN_EN) && (SYSRTC0->SYNCBUSY != 0U)) {
    // Wait for all synchronizations to finish
  }
}

/***************************************************************************//**
 * Waits for the SYSRTC to complete reseting or disabling procedure.
 ******************************************************************************/
__STATIC_INLINE void sl_sysrtc_wait_ready(void)
{
  while ((SYSRTC0->SWRST & _SYSRTC_SWRST_RESETTING_MASK) || (SYSRTC0->EN & _SYSRTC_EN_DISABLING_MASK) || (SYSRTC0->SYNCBUSY != 0U)) {
    // Wait for all synchronizations to finish
  }
}

/***************************************************************************//**
 * Starts SYSRTC counter.
 *
 * This function will send a start command to the SYSRTC peripheral. The SYSRTC
 * peripheral will use some LF clock ticks before the command is executed.
 * The sl_sysrtc_wait_sync() function can be used to wait for the start
 * command to be executed.
 *
 * @note  This function requires the SYSRTC to be enabled.
 ******************************************************************************/
__STATIC_INLINE void sl_sysrtc_start(void)
{
  sl_sysrtc_wait_sync();
  SYSRTC0->CMD = SYSRTC_CMD_START;
}

/***************************************************************************//**
 * Stops the SYSRTC counter.
 *
 * This function will send a stop command to the SYSRTC peripheral. The SYSRTC
 * peripheral will use some LF clock ticks before the command is executed.
 * The sl_sysrtc_wait_sync() function can be used to wait for the stop
 * command to be executed.
 *
 * @note  This function requires the SYSRTC to be enabled.
 ******************************************************************************/
__STATIC_INLINE void sl_sysrtc_stop(void)
{
  sl_sysrtc_wait_sync();
  SYSRTC0->CMD = SYSRTC_CMD_STOP;
}

/***************************************************************************//**
 * Restores SYSRTC to its reset state.
 ******************************************************************************/
void sl_sysrtc_reset(void);

/***************************************************************************//**
 * Gets SYSRTC STATUS register value.
 *
 * @return  Current STATUS register value.
 ******************************************************************************/
__STATIC_INLINE uint32_t sl_sysrtc_get_status(void)
{
  return SYSRTC0->STATUS;
}

/***************************************************************************//**
 * Locks SYSRTC registers.
 *
 * @note  When SYSRTC registers are locked SYSRTC_EN, SYSRTC_CFG, SYSRTC_CMD,
 *        SYSRTC_SWRST, SYSRTC_CNT and SYSRTC_TOPCNT registers cannot be written
 *        to.
 ******************************************************************************/
__STATIC_INLINE void sl_sysrtc_lock(void)
{
  SYSRTC0->LOCK = ~SYSRTC_LOCK_LOCKKEY_UNLOCK;
}

/***************************************************************************//**
 * Unlocks SYSRTC registers.
 *
 * @note  When SYSRTC registers are locked SYSRTC_EN, SYSRTC_CFG, SYSRTC_CMD,
 *        SYSRTC_SWRST, SYSRTC_CNT and SYSRTC_TOPCNT registers cannot be written
 *        to.
 ******************************************************************************/
__STATIC_INLINE void sl_sysrtc_unlock(void)
{
  SYSRTC0->LOCK = SYSRTC_LOCK_LOCKKEY_UNLOCK;
}

/***************************************************************************//**
 * Gets SYSRTC counter value.
 *
 * @return  Current SYSRTC counter value.
 ******************************************************************************/
__STATIC_INLINE uint32_t sl_sysrtc_get_counter(void)
{
  // Wait for Counter to synchronize before getting value
  sl_sysrtc_wait_sync();

  return SYSRTC0->CNT;
}

/***************************************************************************//**
 * Sets the SYSRTC counter value.
 *
 * @param[in] value   The new SYSRTC counter value.
 ******************************************************************************/
__STATIC_INLINE void sl_sysrtc_set_counter(uint32_t value)
{
  // Wait for Counter to synchronize before getting value
  sl_sysrtc_wait_sync();

  SYSRTC0->CNT = value;
}

/***************************************************************************//**
 * Initializes the selected SYSRTC group.
 *
 * @param[in] group_number  SYSRTC group number to use.
 *
 * @param[in] p_group_config  Pointer to group configuration structure
 *                            variable.
 ******************************************************************************/
void sl_sysrtc_init_group(uint8_t group_number,
                          sl_sysrtc_group_config_t const *p_group_config);

/***************************************************************************//**
 * Enables one or more SYSRTC interrupts for the given group.
 *
 * @note  Depending on the use, a pending interrupt may already be set prior to
 *        enabling the interrupt. To ignore a pending interrupt, consider using
 *        sl_sysrtc_clear_group_interrupts() prior to enabling the interrupt.
 *
 * @param[in] group_number  SYSRTC group number to use.
 *
 * @param[in] flags   SYSRTC interrupt sources to enable.
 *                    Use a set of interrupt flags OR-ed together to set
 *                    multiple interrupt sources for the given SYSRTC group.
 ******************************************************************************/
void sl_sysrtc_enable_group_interrupts(uint8_t group_number,
                                       uint32_t flags);

/***************************************************************************//**
 * Disables one or more SYSRTC interrupts for the given group.
 *
 * @param[in] group_number  SYSRTC group number to use.
 *
 * @param[in] flags   SYSRTC interrupt sources to disable.
 *                    Use a set of interrupt flags OR-ed together to disable
 *                    multiple interrupt sources for the given SYSRTC group.
 ******************************************************************************/
void sl_sysrtc_disable_group_interrupts(uint8_t group_number,
                                        uint32_t flags);

/***************************************************************************//**
 * Clears one or more pending SYSRTC interrupts for the given group.
 *
 * @param[in] group_number  SYSRTC group number to use.
 *
 * @param[in] flags   SYSRTC interrupt sources to clear.
 *                    Use a set of interrupt flags OR-ed together to clear
 *                    multiple interrupt sources for the given SYSRTC group.
 ******************************************************************************/
void sl_sysrtc_clear_group_interrupts(uint8_t group_number,
                                      uint32_t flags);

/***************************************************************************//**
 * Gets pending SYSRTC interrupt flags for the given group.
 *
 * @note  Event bits are not cleared by using this function.
 *
 * @param[in] group_number  SYSRTC group number to use.
 *
 * @return  Pending SYSRTC interrupt sources.
 *          Returns a set of interrupt flags OR-ed together for multiple
 *          interrupt sources in the SYSRTC group.
 ******************************************************************************/
uint32_t sl_sysrtc_get_group_interrupts(uint8_t group_number);

/***************************************************************************//**
 * Gets enabled and pending SYSRTC interrupt flags.
 * Useful for handling more interrupt sources in the same interrupt handler.
 *
 * @note  Interrupt flags are not cleared by using this function.
 *
 * @param[in] group_number  SYSRTC group number to use.
 *
 * @return  Pending and enabled SYSRTC interrupt sources.
 *          The return value is the bitwise AND of
 *          - the enabled interrupt sources in SYSRTC_GRPx_IEN and
 *          - the pending interrupt flags SYSRTC_GRPx_IF.
 ******************************************************************************/
uint32_t sl_sysrtc_get_group_enabled_interrupts(uint8_t group_number);

/***************************************************************************//**
 * Sets one or more pending SYSRTC interrupts for the given group from Software.
 *
 * @param[in] group_number  SYSRTC group number to use.
 *
 * @param[in] flags   SYSRTC interrupt sources to set to pending.
 *                    Use a set of interrupt flags OR-ed together to set
 *                    multiple interrupt sources for the SYSRTC group.
 ******************************************************************************/
void sl_sysrtc_set_group_interrupts(uint8_t group_number,
                                    uint32_t flags);

/***************************************************************************//**
 * Gets SYSRTC compare register value for selected channel of given group.
 *
 * @param[in] group_number  SYSRTC group number to use.
 *
 * @param[in] channel   Channel selector.
 *
 * @return  Compare register value.
 ******************************************************************************/
uint32_t sl_sysrtc_get_group_compare_channel_value(uint8_t group_number,
                                                   uint8_t channel);

/***************************************************************************//**
 * Sets SYSRTC compare register value for selected channel of given group.
 *
 * @param[in] group_number  SYSRTC group number to use.
 *
 * @param[in] channel   Channel selector.
 *
 * @param[in] value   Compare register value.
 ******************************************************************************/
void sl_sysrtc_set_group_compare_channel_value(uint8_t group_number,
                                               uint8_t channel,
                                               uint32_t value);

/***************************************************************************//**
 * Gets SYSRTC input capture register value for capture channel of given group.
 *
 * @param[in] group_number  SYSRTC group number to use.
 *
 * @return  Capture register value.
 ******************************************************************************/
uint32_t sl_sysrtc_get_group_capture_channel_value(uint8_t group_number);

/** @} (end addtogroup sysrtc) */

#ifdef __cplusplus
}
#endif

#endif /* defined(SYSRTC_COUNT) && (SYSRTC_COUNT > 0) */
#endif /* PERIPHERAL_SYSRTC_H */
