/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef __SL_WISUN_NETWORK_MEASUREMENT_LCD__
#define __SL_WISUN_NETWORK_MEASUREMENT_LCD__

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
typedef struct sl_wisun_nwm_logo_form_args {
  const char *network_name;
  const char *join_state_str;
  uint32_t join_state;
} sl_wisun_nwm_logo_form_args_t;
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Network Measurement Main form
 * @details Renderer Main form
 * @param[in] args Arguments
 *****************************************************************************/
void sl_wisun_nwm_main_form(void *args);

/**************************************************************************//**
 * @brief Network Measurement Logo form
 * @details Renderer Logo form during connection
 * @param[in] args Arguments
 *****************************************************************************/
void sl_wisun_nwm_logo_form(void *args);

/**************************************************************************//**
 * @brief Network Measurement Test result form
 * @details Renderer Test result form
 * @param[in] args Arguments
 *****************************************************************************/
void sl_wisun_nwm_test_result_form(void *args);
#endif
