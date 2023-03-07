/***************************************************************************//**
 * @file sl_wisun_util.c
 * @brief Wi-SUN utilities
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

#include <stddef.h>
#include <stdint.h>
#include "rail_config.h"
#include "sl_wisun_api.h"
#include "sl_wisun_util.h"
#include "sli_wisun_internal_api.h"
#include "SEGGER_RTT.h"

#define RTT_LogChannel ((unsigned int) 0)

#define VERSION_WISUN_FAN1_0       0

#define SLI_WISUN_ERROR_CHECK_SET_STATUS(__result, __value)\
do {\
  if (!(__result)){\
    status = __value;\
    goto error_handler;\
  }\
} while(0)

#define SLI_WISUN_ERROR_SET_STATUS(__value)\
do {\
  status = __value;\
  goto error_handler;\
} while(0)

/** Matches a RAIL configuration with Wi-SUN frequency band settings. */
typedef struct sli_wisun_rf_settings {
  uint32_t channel_0_center_frequency;  ///< Center frequency
  uint32_t channel_spacing;             ///< Channel spacing
  uint32_t datarate;                    ///< Data rate
  uint16_t number_of_channels;          ///< Number of channels
  uint8_t reg_domain;                   ///< Regulatory domain
  uint8_t op_class;                     ///< Operating class
  uint16_t op_mode;                     ///< Operating mode
} sli_wisun_rf_settings_t;

const uint8_t phy_mode_id_to_op_mode[9] = {
  0xFF, 0x1a, 0x1b, 0x2a, 0x2b, 0x3, 0x4a, 0x4b, 0x5
};

sl_status_t sl_wisun_util_get_rf_settings(uint8_t *reg_domain, uint8_t *op_class, uint16_t *op_mode)
{
  sl_status_t status = SL_STATUS_OK;
  RAIL_Handle_t rail_handle;
  const RAIL_ChannelConfig_t *channel_config;
  const uint8_t *stack_info;
  RAIL_Status_t ret;
  uint8_t phy_mode;
  SEGGER_RTT_printf(RTT_LogChannel, "sl_wisun_util_get_rf_settings()");

  status = sli_wisun_get_rail_handle(&rail_handle);
  SLI_WISUN_ERROR_CHECK_SET_STATUS(status == SL_STATUS_OK, SL_STATUS_FAIL);

  /* Take first RAIL configuration as reference. */
  channel_config = channelConfigs[0];
  ret = RAIL_ConfigChannels(rail_handle, channel_config, NULL);
  SLI_WISUN_ERROR_CHECK_SET_STATUS(ret == RAIL_STATUS_NO_ERROR, SL_STATUS_FAIL);

  stack_info = channel_config->configs[0].stackInfo;
  if (stack_info[0] != RAIL_PTI_PROTOCOL_WISUN) {
    // Not a Wi-SUN configuration
    SLI_WISUN_ERROR_SET_STATUS(SL_STATUS_FAIL);
  }
  if (stack_info[1] & 0xE0) {
    // Not a valid PHY Type
    SLI_WISUN_ERROR_SET_STATUS(SL_STATUS_FAIL);
  }
  if (stack_info[2] == VERSION_WISUN_FAN1_0) {
    *reg_domain = stack_info[4];
    *op_class = stack_info[3];
    phy_mode = stack_info[1] & 0x0F;
    if (phy_mode == 0 || phy_mode > 8) {
      // Not a valid PHY Mode
      SLI_WISUN_ERROR_SET_STATUS(SL_STATUS_FAIL);
    } else {
      *op_mode = phy_mode_id_to_op_mode[phy_mode];
    }
  } else {
    // Unknown configuration version
    SLI_WISUN_ERROR_SET_STATUS(SL_STATUS_FAIL);
  }
  SEGGER_RTT_printf(RTT_LogChannel, "Using reg_domain %u, op_class %u, op_mode %x", *reg_domain, *op_class, *op_mode);

error_handler:

  return status;
}

sl_status_t sl_wisun_util_connect(const uint8_t * network_name)
{
  sl_status_t status;
  uint8_t reg_domain;
  uint8_t op_class;
  uint16_t op_mode;

  SEGGER_RTT_printf(RTT_LogChannel, "sl_wisun_util_connect()");

  status = sl_wisun_util_get_rf_settings(&reg_domain, &op_class, &op_mode);
  SLI_WISUN_ERROR_CHECK_SET_STATUS(status == SL_STATUS_OK, SL_STATUS_FAIL);

  status = sl_wisun_connect(network_name, (sl_wisun_regulatory_domain_t)reg_domain, (sl_wisun_operating_class_t)op_class, (sl_wisun_operating_mode_t)op_mode);

error_handler:

  return status;
}
