/***************************************************************************//**
 * @file
 * @brief System Real Time Counter (SYSRTC) Peripheral API
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

#include "peripheral_sysrtc.h"
#if defined(SYSRTC_COUNT) && (SYSRTC_COUNT > 0)
#include "sl_assert.h"
#include "em_bus.h"
#include "stddef.h"

/***************************************************************************//**
 * @addtogroup sysrtc SYSRTC - System Real Time Counter
 * @brief System Real Time Counter (SYSRTC) Peripheral API
 * @details
 *  This module contains functions to control the SYSRTC peripheral of Silicon
 *  Labs 32-bit MCUs and SoCs. The SYSRTC ensures timekeeping in low energy modes.
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Initializes SYSRTC module.
 ******************************************************************************/
void sl_sysrtc_init(const sl_sysrtc_config_t *p_config)
{
  // Wait to be ready
  sl_sysrtc_wait_ready();

  if (SYSRTC0->EN == SYSRTC_EN_EN) {
    // Disable the module
    sl_sysrtc_disable();
    // Wait to be ready
    sl_sysrtc_wait_ready();
  }

  // Set configuration
  SYSRTC0->CFG = (p_config->enable_debug_run ? 1UL : 0UL) << _SYSRTC_CFG_DEBUGRUN_SHIFT;
}

/***************************************************************************//**
 * Enables SYSRTC counting.
 ******************************************************************************/
void sl_sysrtc_enable(void)
{
  // Wait if disabling
  sl_sysrtc_wait_ready();

  // Enable SYSRTC module
  SYSRTC0->EN_SET = SYSRTC_EN_EN;

  // Start counter
  SYSRTC0->CMD = SYSRTC_CMD_START;
}

/***************************************************************************//**
 * Disables SYSRTC counting.
 ******************************************************************************/
void sl_sysrtc_disable(void)
{
  if (SYSRTC0->EN != SYSRTC_EN_EN) {
    return;
  }

  // Stop counter
  sl_sysrtc_stop();

  // Disable module
  SYSRTC0->EN_CLR = SYSRTC_EN_EN;
}

/***************************************************************************//**
 * Restores SYSRTC to its reset state.
 ******************************************************************************/
void sl_sysrtc_reset(void)
{
  // Reset timer
  SYSRTC0->SWRST = SYSRTC_SWRST_SWRST;
}

/***************************************************************************//**
 * Initializes the selected SYSRTC group.
 ******************************************************************************/
void sl_sysrtc_init_group(uint8_t group_number,
                          sl_sysrtc_group_config_t const *p_group_config)
{
  uint32_t temp = 0;

  EFM_ASSERT(SYSRTC_GROUP_VALID(group_number));

  switch (group_number) {
    case 0:
      temp = ((p_group_config->compare_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP0_CTRL_CMP0EN_SHIFT);
      if (p_group_config->p_compare_channel0_config != NULL) {
        temp |= ((uint32_t)p_group_config->p_compare_channel0_config->compare_match_out_action << _SYSRTC_GRP0_CTRL_CMP0CMOA_SHIFT);
      }

#ifdef SYSRTC_GRP0_CTRL_CMP1EN
      temp |= ((p_group_config->compare_channel1_enable ? 1UL : 0UL) << _SYSRTC_GRP0_CTRL_CMP1EN_SHIFT);
      if (p_group_config->p_compare_channel1_config != NULL) {
        temp |= ((uint32_t)p_group_config->p_compare_channel1_config->compare_match_out_action << _SYSRTC_GRP0_CTRL_CMP1CMOA_SHIFT);
      }
#endif
#ifdef SYSRTC_GRP0_CTRL_CAP0EN
      temp |= ((p_group_config->capture_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP0_CTRL_CAP0EN_SHIFT);
      if (p_group_config->p_capture_channel0_config != NULL) {
        temp |= ((uint32_t)p_group_config->p_capture_channel0_config->capture_input_edge << _SYSRTC_GRP0_CTRL_CAP0EDGE_SHIFT);
      }
#endif
      SYSRTC0->GRP0_CTRL = temp;
      break;

#if SYSRTC_GROUP_NUMBER > 1
    case 1:
      temp = ((p_group_config->compare_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP1_CTRL_CMP0EN_SHIFT);
      if (p_group_config->p_compare_channel0_config != NULL) {
        temp |= ((uint32_t)p_group_config->p_compare_channel0_config->compare_match_out_action << _SYSRTC_GRP1_CTRL_CMP0CMOA_SHIFT);
      }
#ifdef SYSRTC_GRP1_CTRL_CMP1EN
      temp |= ((p_group_config->compare_channel1_enable ? 1UL : 0UL) << _SYSRTC_GRP1_CTRL_CMP1EN_SHIFT);
      if (p_group_config->p_compare_channel1_config != NULL) {
        temp |= ((uint32_t)p_group_config->p_compare_channel1_config->compare_match_out_action << _SYSRTC_GRP1_CTRL_CMP1CMOA_SHIFT);
      }
#endif
#ifdef SYSRTC_GRP1_CTRL_CAP0EN
      temp |= ((p_group_config->capture_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP1_CTRL_CAP0EN_SHIFT);
      if (p_group_config->p_capture_channel0_config != NULL) {
        temp |=  ((uint32_t)p_group_config->p_capture_channel0_config->capture_input_edge << _SYSRTC_GRP1_CTRL_CAP0EDGE_SHIFT);
      }
#endif
      SYSRTC0->GRP1_CTRL = temp;
      break;

#if SYSRTC_GROUP_NUMBER > 2
    case 2:
//      SYSRTC0->GRP2_CTRL = ((p_group_config->compare_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP2_CTRL_CMP0EN_SHIFT)
//                           | ((uint32_t)p_group_config->p_compare_channel0_config->compare_match_out_action << _SYSRTC_GRP2_CTRL_CMP0CMOA_SHIFT);
#ifdef SYSRTC_GRP2_CTRL_CMP1EN
      SYSRTC0->GRP2_CTRL |= ((p_group_config->compare_channel1_enable ? 1UL : 0UL) << _SYSRTC_GRP2_CTRL_CMP1EN_SHIFT)
                            | ((uint32_t)p_group_config->p_compare_channel1_config->compare_match_out_action << _SYSRTC_GRP2_CTRL_CMP1CMOA_SHIFT);
#endif
#ifdef SYSRTC_GRP2_CTRL_CAP0EN
      SYSRTC0->GRP2_CTRL |= ((p_group_config->capture_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP2_CTRL_CAP0EN_SHIFT)
                            | ((uint32_t)p_group_config->p_capture_channel0_config->capture_input_edge << _SYSRTC_GRP2_CTRL_CAP0EDGE_SHIFT);
#endif
      break;

#if SYSRTC_GROUP_NUMBER > 3
    case 3:
      SYSRTC0->GRP3_CTRL = ((p_group_config->compare_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP3_CTRL_CMP0EN_SHIFT)
                           | ((uint32_t)p_group_config->p_compare_channel0_config->compare_match_out_action << _SYSRTC_GRP3_CTRL_CMP0CMOA_SHIFT);
#ifdef SYSRTC_GRP3_CTRL_CMP1EN
      SYSRTC0->GRP3_CTRL |= ((p_group_config->compare_channel1_enable ? 1UL : 0UL) << _SYSRTC_GRP3_CTRL_CMP1EN_SHIFT)
                            | ((uint32_t)p_group_config->p_compare_channel1_config->compare_match_out_action << _SYSRTC_GRP3_CTRL_CMP1CMOA_SHIFT);
#endif
#ifdef SYSRTC_GRP3_CTRL_CAP0EN
      SYSRTC0->GRP3_CTRL |= ((p_group_config->capture_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP3_CTRL_CAP0EN_SHIFT)
                            | ((uint32_t)p_group_config->p_capture_channel0_config->capture_input_edge << _SYSRTC_GRP3_CTRL_CAP0EDGE_SHIFT);
#endif
      break;

#if SYSRTC_GROUP_NUMBER > 4
    case 4:
      SYSRTC0->GRP4_CTRL = ((p_group_config->compare_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP4_CTRL_CMP0EN_SHIFT)
                           | ((uint32_t)p_group_config->p_compare_channel0_config->compare_match_out_action << _SYSRTC_GRP4_CTRL_CMP0CMOA_SHIFT);
#ifdef SYSRTC_GRP4_CTRL_CMP1EN
      SYSRTC0->GRP4_CTRL |= ((p_group_config->compare_channel1_enable ? 1UL : 0UL) << _SYSRTC_GRP4_CTRL_CMP1EN_SHIFT)
                            | ((uint32_t)p_group_config->p_compare_channel1_config->compare_match_out_action << _SYSRTC_GRP4_CTRL_CMP1CMOA_SHIFT);
#endif
#ifdef SYSRTC_GRP4_CTRL_CAP0EN
      SYSRTC0->GRP4_CTRL |= ((p_group_config->capture_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP4_CTRL_CAP0EN_SHIFT)
                            | ((uint32_t)p_group_config->p_capture_channel0_config->capture_input_edge << _SYSRTC_GRP4_CTRL_CAP0EDGE_SHIFT);
#endif
      break;

#if SYSRTC_GROUP_NUMBER > 5
    case 5:
      SYSRTC0->GRP5_CTRL = ((p_group_config->compare_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP5_CTRL_CMP0EN_SHIFT)
                           | ((uint32_t)p_group_config->p_compare_channel0_config->compare_match_out_action << _SYSRTC_GRP5_CTRL_CMP0CMOA_SHIFT);
#ifdef SYSRTC_GRP5_CTRL_CMP1EN
      SYSRTC0->GRP5_CTRL |= ((p_group_config->compare_channel1_enable ? 1UL : 0UL) << _SYSRTC_GRP5_CTRL_CMP1EN_SHIFT)
                            | ((uint32_t)p_group_config->p_compare_channel1_config->compare_match_out_action << _SYSRTC_GRP5_CTRL_CMP1CMOA_SHIFT);
#endif
#ifdef SYSRTC_GRP5_CTRL_CAP0EN
      SYSRTC0->GRP5_CTRL |= ((p_group_config->capture_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP5_CTRL_CAP0EN_SHIFT)
                            | ((uint32_t)p_group_config->p_capture_channel0_config->capture_input_edge << _SYSRTC_GRP5_CTRL_CAP0EDGE_SHIFT);
#endif
      break;

#if SYSRTC_GROUP_NUMBER > 6
    case 6:
      SYSRTC0->GRP6_CTRL = ((p_group_config->compare_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP6_CTRL_CMP0EN_SHIFT)
                           | ((uint32_t)p_group_config->p_compare_channel0_config->compare_match_out_action << _SYSRTC_GRP6_CTRL_CMP0CMOA_SHIFT);
#ifdef SYSRTC_GRP6_CTRL_CMP1EN
      SYSRTC0->GRP6_CTRL |= ((p_group_config->compare_channel1_enable ? 1UL : 0UL) << _SYSRTC_GRP6_CTRL_CMP1EN_SHIFT)
                            | ((uint32_t)p_group_config->p_compare_channel1_config->compare_match_out_action << _SYSRTC_GRP6_CTRL_CMP1CMOA_SHIFT);
#endif
#ifdef SYSRTC_GRP6_CTRL_CAP0EN
      SYSRTC0->GRP6_CTRL |= ((p_group_config->capture_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP6_CTRL_CAP0EN_SHIFT)
                            | ((uint32_t)p_group_config->p_capture_channel0_config->capture_input_edge << _SYSRTC_GRP6_CTRL_CAP0EDGE_SHIFT);
#endif
      break;

#if SYSRTC_GROUP_NUMBER > 7
    case 7:
      SYSRTC0->GRP7_CTRL = ((p_group_config->compare_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP7_CTRL_CMP0EN_SHIFT)
                           | ((uint32_t)p_group_config->p_compare_channel0_config->compare_match_out_action << _SYSRTC_GRP7_CTRL_CMP0CMOA_SHIFT);
#ifdef SYSRTC_GRP7_CTRL_CMP1EN
      SYSRTC0->GRP7_CTRL |= ((p_group_config->compare_channel1_enable ? 1UL : 0UL) << _SYSRTC_GRP7_CTRL_CMP1EN_SHIFT)
                            | ((uint32_t)p_group_config->p_compare_channel1_config->compare_match_out_action << _SYSRTC_GRP7_CTRL_CMP1CMOA_SHIFT);
#endif
#ifdef SYSRTC_GRP7_CTRL_CAP0EN
      SYSRTC0->GRP7_CTRL |= ((p_group_config->capture_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP7_CTRL_CAP0EN_SHIFT)
                            | ((uint32_t)p_group_config->p_capture_channel0_config->capture_input_edge << _SYSRTC_GRP7_CTRL_CAP0EDGE_SHIFT);
#endif
      break;
#endif
#endif
#endif
#endif
#endif
#endif
#endif

    default:
      EFM_ASSERT(1);
  }
}

/***************************************************************************//**
 * Enables one or more SYSRTC interrupts for the given group.
 ******************************************************************************/
void sl_sysrtc_enable_group_interrupts(uint8_t group_number,
                                       uint32_t flags)
{
  EFM_ASSERT(SYSRTC_GROUP_VALID(group_number));

  switch (group_number) {
    case 0:
      SYSRTC0->GRP0_IEN_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 1
    case 1:
      SYSRTC0->GRP1_IEN_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 2
    case 2:
      SYSRTC0->GRP2_IEN_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 3
    case 3:
      SYSRTC0->GRP3_IEN_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 4
    case 4:
      SYSRTC0->GRP4_IEN_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 5
    case 5:
      SYSRTC0->GRP5_IEN_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 6
    case 6:
      SYSRTC0->GRP6_IEN_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 7
    case 7:
      SYSRTC0->GRP7_IEN_SET = flags;
      break;
#endif
#endif
#endif
#endif
#endif
#endif
#endif

    default:
      EFM_ASSERT(1);
  }
}

/***************************************************************************//**
 * Disables one or more SYSRTC interrupts for the given group.
 ******************************************************************************/
void sl_sysrtc_disable_group_interrupts(uint8_t group_number,
                                        uint32_t flags)
{
  EFM_ASSERT(SYSRTC_GROUP_VALID(group_number));

  switch (group_number) {
    case 0:
      SYSRTC0->GRP0_IEN_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 1
    case 1:
      SYSRTC0->GRP1_IEN_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 2
    case 2:
      SYSRTC0->GRP2_IEN_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 3
    case 3:
      SYSRTC0->GRP3_IEN_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 4
    case 4:
      SYSRTC0->GRP4_IEN_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 5
    case 5:
      SYSRTC0->GRP5_IEN_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 6
    case 6:
      SYSRTC0->GRP6_IEN_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 7
    case 7:
      SYSRTC0->GRP7_IEN_CLR = flags;
      break;
#endif
#endif
#endif
#endif
#endif
#endif
#endif

    default:
      EFM_ASSERT(1);
  }
}

/***************************************************************************//**
 * Clears one or more pending SYSRTC interrupts for the given group.
 ******************************************************************************/
void sl_sysrtc_clear_group_interrupts(uint8_t group_number,
                                      uint32_t flags)
{
  EFM_ASSERT(SYSRTC_GROUP_VALID(group_number));

  switch (group_number) {
    case 0:
      SYSRTC0->GRP0_IF_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 1
    case 1:
      SYSRTC0->GRP1_IF_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 2
    case 2:
      SYSRTC0->GRP2_IF_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 3
    case 3:
      SYSRTC0->GRP3_IF_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 4
    case 4:
      SYSRTC0->GRP4_IF_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 5
    case 5:
      SYSRTC0->GRP5_IF_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 6
    case 6:
      SYSRTC0->GRP6_IF_CLR = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 7
    case 7:
      SYSRTC0->GRP7_IF_CLR = flags;
      break;
#endif
#endif
#endif
#endif
#endif
#endif
#endif

    default:
      EFM_ASSERT(1);
  }
}

/***************************************************************************//**
 * Gets pending SYSRTC interrupt flags for the given group.
 ******************************************************************************/
uint32_t sl_sysrtc_get_group_interrupts(uint8_t group_number)
{
  EFM_ASSERT(SYSRTC_GROUP_VALID(group_number));

  switch (group_number) {
    case 0:
      return SYSRTC0->GRP0_IF;

#if SYSRTC_GROUP_NUMBER > 1
    case 1:
      return SYSRTC0->GRP1_IF;

#if SYSRTC_GROUP_NUMBER > 2
    case 2:
      return SYSRTC0->GRP2_IF;

#if SYSRTC_GROUP_NUMBER > 3
    case 3:
      return SYSRTC0->GRP3_IF;

#if SYSRTC_GROUP_NUMBER > 4
    case 4:
      return SYSRTC0->GRP4_IF;

#if SYSRTC_GROUP_NUMBER > 5
    case 5:
      return SYSRTC0->GRP5_IF;

#if SYSRTC_GROUP_NUMBER > 6
    case 6:
      return SYSRTC0->GRP6_IF;

#if SYSRTC_GROUP_NUMBER > 7
    case 7:
      return SYSRTC0->GRP7_IF;
#endif
#endif
#endif
#endif
#endif
#endif
#endif

    default:
      EFM_ASSERT(1);
      return 0;
  }
}

/***************************************************************************//**
 * Gets enabled and pending SYSRTC interrupt flags.
 ******************************************************************************/
uint32_t sl_sysrtc_get_group_enabled_interrupts(uint8_t group_number)
{
  uint32_t ien = 0;

  EFM_ASSERT(SYSRTC_GROUP_VALID(group_number));

  switch (group_number) {
    case 0:
      ien = SYSRTC0->GRP0_IEN;
      return SYSRTC0->GRP0_IF & ien;

#if SYSRTC_GROUP_NUMBER > 1
    case 1:
      ien = SYSRTC0->GRP1_IEN;
      return SYSRTC0->GRP1_IF & ien;

#if SYSRTC_GROUP_NUMBER > 2
    case 2:
      ien = SYSRTC0->GRP2_IEN;
      return SYSRTC0->GRP2_IF & ien;

#if SYSRTC_GROUP_NUMBER > 3
    case 3:
      ien = SYSRTC0->GRP3_IEN;
      return SYSRTC0->GRP3_IF & ien;

#if SYSRTC_GROUP_NUMBER > 4
    case 4:
      ien = SYSRTC0->GRP4_IEN;
      return SYSRTC0->GRP4_IF & ien;

#if SYSRTC_GROUP_NUMBER > 5
    case 5:
      ien = SYSRTC0->GRP5_IEN;
      return SYSRTC0->GRP5_IF & ien;

#if SYSRTC_GROUP_NUMBER > 6
    case 6:
      ien = SYSRTC0->GRP6_IEN;
      return SYSRTC0->GRP6_IF & ien;

#if SYSRTC_GROUP_NUMBER > 7
    case 7:
      ien = SYSRTC0->GRP7_IEN;
      return SYSRTC0->GRP7_IF & ien;
#endif
#endif
#endif
#endif
#endif
#endif
#endif

    default:
      EFM_ASSERT(1);
      return 0;
  }
}

/***************************************************************************//**
 * Sets one or more pending SYSRTC interrupts for the given group from Software.
 ******************************************************************************/
void sl_sysrtc_set_group_interrupts(uint8_t group_number,
                                    uint32_t flags)
{
  EFM_ASSERT(SYSRTC_GROUP_VALID(group_number));

  switch (group_number) {
    case 0:
      SYSRTC0->GRP0_IF_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 1
    case 1:
      SYSRTC0->GRP1_IF_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 2
    case 2:
      SYSRTC0->GRP2_IF_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 3
    case 3:
      SYSRTC0->GRP3_IF_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 4
    case 4:
      SYSRTC0->GRP4_IF_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 5
    case 5:
      SYSRTC0->GRP5_IF_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 6
    case 6:
      SYSRTC0->GRP6_IF_SET = flags;
      break;

#if SYSRTC_GROUP_NUMBER > 7
    case 7:
      SYSRTC0->GRP7_IF_SET = flags;
      break;
#endif
#endif
#endif
#endif
#endif
#endif
#endif

    default:
      EFM_ASSERT(1);
  }
}

/***************************************************************************//**
 * Gets SYSRTC compare register value for selected channel of given group.
 ******************************************************************************/
uint32_t sl_sysrtc_get_group_compare_channel_value(uint8_t group_number,
                                                   uint8_t channel)
{
  EFM_ASSERT(SYSRTC_GROUP_VALID(group_number));

  switch (group_number) {
    case 0:
      switch (channel) {
        case 0:
          return SYSRTC0->GRP0_CMP0VALUE;

#ifdef SYSRTC_GRP0_CTRL_CMP1EN
        case 1:
          return SYSRTC0->GRP0_CMP1VALUE;
#endif

        default:
          EFM_ASSERT(1);
          return 0;
      }
      break;

#if SYSRTC_GROUP_NUMBER > 1
    case 1:
      switch (channel) {
        case 0:
          return SYSRTC0->GRP1_CMP0VALUE;

#ifdef SYSRTC_GRP1_CTRL_CMP1EN
        case 1:
          return SYSRTC0->GRP1_CMP1VALUE;
#endif

        default:
          EFM_ASSERT(1);
          return 0;
      }
      break;

#if SYSRTC_GROUP_NUMBER > 2
    case 2:
      switch (channel) {
        case 0:
          return SYSRTC0->GRP2_CMP0VALUE;

#ifdef SYSRTC_GRP2_CTRL_CMP1EN
        case 1:
          return SYSRTC0->GRP2_CMP1VALUE;
#endif

        default:
          EFM_ASSERT(1);
          return 0;
      }
      break;

#if SYSRTC_GROUP_NUMBER > 3
    case 3:
      switch (channel) {
        case 0:
          return SYSRTC0->GRP3_CMP0VALUE;

#ifdef SYSRTC_GRP3_CTRL_CMP1EN
        case 1:
          return SYSRTC0->GRP3_CMP1VALUE;
#endif

        default:
          EFM_ASSERT(1);
          return 0;
      }
      break;

#if SYSRTC_GROUP_NUMBER > 4
    case 4:
      switch (channel) {
        case 0:
          return SYSRTC0->GRP4_CMP0VALUE;

#ifdef SYSRTC_GRP4_CTRL_CMP1EN
        case 1:
          return SYSRTC0->GRP4_CMP1VALUE;
#endif

        default:
          EFM_ASSERT(1);
          return 0;
      }
      break;

#if SYSRTC_GROUP_NUMBER > 5
    case 5:
      switch (channel) {
        case 0:
          return SYSRTC0->GRP5_CMP0VALUE;

#ifdef SYSRTC_GRP5_CTRL_CMP1EN
        case 1:
          return SYSRTC0->GRP5_CMP1VALUE;
#endif

        default:
          EFM_ASSERT(1);
          return 0;
      }
      break;

#if SYSRTC_GROUP_NUMBER > 6
    case 6:
      switch (channel) {
        case 0:
          return SYSRTC0->GRP6_CMP0VALUE;

#ifdef SYSRTC_GRP6_CTRL_CMP1EN
        case 1:
          return SYSRTC0->GRP6_CMP1VALUE;
#endif

        default:
          EFM_ASSERT(1);
          return 0;
      }
      break;

#if SYSRTC_GROUP_NUMBER > 7
    case 7:
      switch (channel) {
        case 0:
          return SYSRTC0->GRP7_CMP0VALUE;

#ifdef SYSRTC_GRP7_CTRL_CMP1EN
        case 1:
          return SYSRTC0->GRP7_CMP1VALUE;
#endif

        default:
          EFM_ASSERT(1);
          return 0;
      }
      break;
#endif
#endif
#endif
#endif
#endif
#endif
#endif

    default:
      EFM_ASSERT(1);
      return 0;
  }
}

/***************************************************************************//**
 * Sets SYSRTC compare register value for selected channel of given group.
 ******************************************************************************/
void sl_sysrtc_set_group_compare_channel_value(uint8_t group_number,
                                               uint8_t channel,
                                               uint32_t value)
{
  EFM_ASSERT(SYSRTC_GROUP_VALID(group_number));

  switch (group_number) {
    case 0:
      switch (channel) {
        case 0:
          SYSRTC0->GRP0_CMP0VALUE = value;
          break;

#ifdef SYSRTC_GRP0_CTRL_CMP1EN
        case 1:
          SYSRTC0->GRP0_CMP1VALUE = value;
          break;
#endif

        default:
          EFM_ASSERT(1);
      }
      break;

#if SYSRTC_GROUP_NUMBER > 1
    case 1:
      switch (channel) {
        case 0:
          SYSRTC0->GRP1_CMP0VALUE = value;
          break;

#ifdef SYSRTC_GRP1_CTRL_CMP1EN
        case 1:
          SYSRTC0->GRP1_CMP1VALUE = value;
          break;
#endif

        default:
          EFM_ASSERT(1);
      }
      break;

#if SYSRTC_GROUP_NUMBER > 2
    case 2:
      switch (channel) {
        case 0:
          SYSRTC0->GRP2_CMP0VALUE = value;
          break;

#ifdef SYSRTC_GRP2_CTRL_CMP1EN
        case 1:
          SYSRTC0->GRP2_CMP1VALUE = value;
          break;
#endif

        default:
          EFM_ASSERT(1);
      }
      break;

#if SYSRTC_GROUP_NUMBER > 3
    case 3:
      switch (channel) {
        case 0:
          SYSRTC0->GRP3_CMP0VALUE = value;
          break;

#ifdef SYSRTC_GRP3_CTRL_CMP1EN
        case 1:
          SYSRTC0->GRP3_CMP1VALUE = value;
          break;
#endif

        default:
          EFM_ASSERT(1);
      }
      break;

#if SYSRTC_GROUP_NUMBER > 4
    case 4:
      switch (channel) {
        case 0:
          SYSRTC0->GRP4_CMP0VALUE = value;
          break;

#ifdef SYSRTC_GRP4_CTRL_CMP1EN
        case 1:
          SYSRTC0->GRP4_CMP1VALUE = value;
          break;
#endif

        default:
          EFM_ASSERT(1);
      }
      break;

#if SYSRTC_GROUP_NUMBER > 5
    case 5:
      switch (channel) {
        case 0:
          SYSRTC0->GRP5_CMP0VALUE = value;
          break;

#ifdef SYSRTC_GRP5_CTRL_CMP1EN
        case 1:
          SYSRTC0->GRP5_CMP1VALUE = value;
          break;
#endif

        default:
          EFM_ASSERT(1);
      }
      break;

#if SYSRTC_GROUP_NUMBER > 6
    case 6:
      switch (channel) {
        case 0:
          SYSRTC0->GRP6_CMP0VALUE = value;
          break;

#ifdef SYSRTC_GRP6_CTRL_CMP1EN
        case 1:
          SYSRTC0->GRP6_CMP1VALUE = value;
          break;
#endif

        default:
          EFM_ASSERT(1);
      }
      break;

#if SYSRTC_GROUP_NUMBER > 7
    case 7:
      switch (channel) {
        case 0:
          SYSRTC0->GRP7_CMP0VALUE = value;
          break;

#ifdef SYSRTC_GRP7_CTRL_CMP1EN
        case 1:
          SYSRTC0->GRP7_CMP1VALUE = value;
          break;
#endif

        default:
          EFM_ASSERT(1);
      }
      break;
#endif
#endif
#endif
#endif
#endif
#endif
#endif

    default:
      EFM_ASSERT(1);
  }
}

/***************************************************************************//**
 * Gets SYSRTC input capture register value for selected channel of given group.
 ******************************************************************************/
uint32_t sl_sysrtc_get_group_capture_channel_value(uint8_t group_number)
{
  EFM_ASSERT(SYSRTC_GROUP_VALID(group_number));

  switch (group_number) {
#ifdef SYSRTC_GRP0_CTRL_CAP0EN
    case 0:
      return SYSRTC0->GRP0_CAP0VALUE;
#endif

#if SYSRTC_GROUP_NUMBER > 1
#ifdef SYSRTC_GRP1_CTRL_CAP0EN
    case 1:
      return SYSRTC0->GRP1_CAP0VALUE;
#endif

#if SYSRTC_GROUP_NUMBER > 2
#ifdef SYSRTC_GRP2_CTRL_CAP0EN
    case 2:
      return SYSRTC0->GRP2_CAP0VALUE;
#endif

#if SYSRTC_GROUP_NUMBER > 3
#ifdef SYSRTC_GRP3_CTRL_CAP0EN
    case 3:
      return SYSRTC0->GRP3_CAP0VALUE;
#endif

#if SYSRTC_GROUP_NUMBER > 4
#ifdef SYSRTC_GRP4_CTRL_CAP0EN
    case 4:
      return SYSRTC0->GRP4_CAP0VALUE;
#endif

#if SYSRTC_GROUP_NUMBER > 5
#ifdef SYSRTC_GRP5_CTRL_CAP0EN
    case 5:
      return SYSRTC0->GRP5_CAP0VALUE;
#endif

#if SYSRTC_GROUP_NUMBER > 6
#ifdef SYSRTC_GRP6_CTRL_CAP0EN
    case 6:
      return SYSRTC0->GRP6_CAP0VALUE;
#endif

#if SYSRTC_GROUP_NUMBER > 7
#ifdef SYSRTC_GRP7_CTRL_CAP0EN
    case 7:
      return SYSRTC0->GRP7_CAP0VALUE;
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif

    default:
      EFM_ASSERT(1);
      return 0;
  }
}

/** @} (end addtogroup sysrtc) */
#endif /* defined(SYSRTC_COUNT) && (SYSRTC_COUNT > 0) */
