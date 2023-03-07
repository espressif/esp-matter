/***************************************************************************//**
 * @file
 * @brief Application Output LCD code
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdbool.h>
#include <stdio.h>
#include "em_common.h"
#include "sl_status.h"

#include "app.h"
#include "app_log.h"

#include "sl_btmesh_api.h"
#include "sl_btmesh_friend.h"
#include "sl_btmesh_hsl_server.h"
#include "sl_btmesh_lighting_server.h"
#include "sl_btmesh_provisioning_decorator.h"
#include "sl_btmesh_factory_reset.h"
#include "sl_btmesh_wstk_lcd.h"

#define UINT16_TO_PERCENTAGE(level) ((((uint32_t)(level) * 100) + 32767) / 65535)
#define UINT16_TO_DEGREE(level) ((((uint32_t)(level) * 360) + 32767) / 65535)

// -----------------------------------------------------------------------------
// BT mesh Friend Node Callbacks

/*******************************************************************************
 * Called when the Friend Node establishes friendship with another node.
 *
 * @param[in] netkey_index Index of the network key used in friendship
 * @param[in] lpn_address Low Power Node address
 ******************************************************************************/
void sl_btmesh_friend_on_friendship_established(uint16_t netkey_index,
                                                uint16_t lpn_address)
{
  app_log("BT mesh Friendship established with LPN (netkey idx: %d, lpn addr: 0x%04x)\r\n",
          "(netkey idx: %d, lpn addr: 0x%04x)\r\n",
          netkey_index,
          lpn_address);
  sl_status_t status = sl_btmesh_LCD_write("FRIEND",
                                           SL_BTMESH_WSTK_LCD_ROW_FRIEND_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
  (void)netkey_index;
  (void)lpn_address;
}

/*******************************************************************************
 * Called when the friendship that was successfully established with a Low Power
 * Node has been terminated.
 *
 * @param[in] netkey_index Index of the network key used in friendship
 * @param[in] lpn_address Low Power Node address
 * @param[in] reason Reason for friendship termination
 ******************************************************************************/
void sl_btmesh_friend_on_friendship_terminated(uint16_t netkey_index,
                                               uint16_t lpn_address,
                                               uint16_t reason)
{
  app_log("BT mesh Friendship terminated with LPN "
          "(netkey idx: %d, lpn addr: 0x%04x, reason: 0x%04x)\r\n",
          netkey_index,
          lpn_address,
          reason);
  sl_status_t status = sl_btmesh_LCD_write("NO LPN",
                                           SL_BTMESH_WSTK_LCD_ROW_FRIEND_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
  (void)netkey_index;
  (void)lpn_address;
  (void)reason;
}

// -----------------------------------------------------------------------------
// BT mesh HSL Callbacks

/*******************************************************************************
 * Called when the UI shall be updated with the changed HSL Model state during
 * a transition. The rate of this callback can be controlled by changing the
 * SL_BTMESH_HSL_SERVER_HUE_UI_UPDATE_PERIOD_CFG_VAL macro.
 *
 * @param[in] hue  Hue value.
 ******************************************************************************/
void sl_btmesh_hsl_hue_on_ui_update(uint16_t hue)
{
  // Temporary buffer to format the LCD output text
  char tmp_str[LCD_ROW_LEN];
  uint16_t hue_degree = UINT16_TO_DEGREE(hue);

  app_log("BT mesh HSL Hue:      %4udeg\r\n", hue_degree);
  snprintf(tmp_str, LCD_ROW_LEN, "Hue:      %4udeg", hue_degree);
  sl_status_t status = sl_btmesh_LCD_write(tmp_str,
                                           SL_BTMESH_WSTK_LCD_ROW_HUE_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
}

/*******************************************************************************
 * Called when the UI shall be updated with the changed HSL Model state during
 * a transition. The rate of this callback can be controlled by changing the
 * SL_BTMESH_HSL_SERVER_SATURATION_UI_UPDATE_PERIOD_CFG_VAL macro.
 *
 * @param[in] saturation  Saturation value.
 ******************************************************************************/
void sl_btmesh_hsl_saturation_on_ui_update(uint16_t saturation)
{
  // Temporary buffer to format the LCD output text
  char tmp_str[LCD_ROW_LEN];
  uint16_t saturation_percent = UINT16_TO_PERCENTAGE(saturation);

  app_log("BT mesh HSL Saturation: %4u%%\r\n", saturation_percent);
  snprintf(tmp_str, LCD_ROW_LEN, "Saturation: %4u%%", saturation_percent);
  sl_status_t status = sl_btmesh_LCD_write(tmp_str,
                                           SL_BTMESH_WSTK_LCD_ROW_SATURATION_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
}

// -----------------------------------------------------------------------------
// BT mesh Lightning Server Callbacks

/*******************************************************************************
 * Called when the UI shall be updated with the changed state of
 * lightning server during a transition. The rate of this callback can be
 * controlled by changing the SL_BTMESH_LIGHTING_SERVER_UI_UPDATE_PERIOD_CFG_VAL macro.
 *
 * @param[in] lightness_level lightness level (0x0001 - FFFE)
 ******************************************************************************/
void sl_btmesh_lighting_server_on_ui_update(uint16_t lightness_level)
{
  // Temporary buffer to format the LCD output text
  char tmp_str[LCD_ROW_LEN];
  uint16_t lightness_percent = UINT16_TO_PERCENTAGE(lightness_level);

  app_log("BT mesh Lightness:     %5u%%\r\n", lightness_percent);
  snprintf(tmp_str, LCD_ROW_LEN, "Lightness: %5u%%", lightness_percent);
  sl_status_t status = sl_btmesh_LCD_write(tmp_str,
                                           SL_BTMESH_WSTK_LCD_ROW_LIGHTNESS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
}

// -----------------------------------------------------------------------------
// Provisioning Decorator Callbacks

/*******************************************************************************
 * Called at node initialization time to provide provisioning information
 *
 * @param[in] provisioned  true: provisioned, false: unprovisioned
 * @param[in] address      Unicast address of the primary element of the node.
                           Ignored if unprovisioned.
 * @param[in] iv_index     IV index for the first network of the node
                           Ignored if unprovisioned.
 ******************************************************************************/
void sl_btmesh_on_provision_init_status(bool provisioned,
                                        uint16_t address,
                                        uint32_t iv_index)
{
  if (provisioned) {
    app_show_btmesh_node_provisioned(address, iv_index);
  } else {
    app_log("BT mesh node is unprovisioned, "
            "started unprovisioned beaconing...\r\n");
    sl_status_t status = sl_btmesh_LCD_write("unprovisioned",
                                             SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
    app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
  }
}

/*******************************************************************************
 * Called when the Provisioning starts
 *
 * @param[in] result  Result code. 0: success, non-zero: error
 ******************************************************************************/
void app_show_btmesh_node_provisioning_started(uint16_t result)
{
  app_log("BT mesh node provisioning is started (result: 0x%04x)\r\n",
          result);
  sl_status_t status = sl_btmesh_LCD_write("provisioning...",
                                           SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
  (void)result;
}

/*******************************************************************************
 * Called when the Provisioning finishes successfully
 *
 * @param[in] address      Unicast address of the primary element of the node.
                           Ignored if unprovisioned.
 * @param[in] iv_index     IV index for the first network of the node
                           Ignored if unprovisioned.
 ******************************************************************************/
void app_show_btmesh_node_provisioned(uint16_t address,
                                      uint32_t iv_index)
{
  app_log("BT mesh node is provisioned (address: 0x%04x, iv_index: 0x%x)\r\n",
          address,
          iv_index);
  sl_status_t status = sl_btmesh_LCD_write("provisioned",
                                           SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
  (void)address;
  (void)iv_index;
}

/*******************************************************************************
 * Called when the Provisioning fails
 *
 * @param[in] result  Result code. 0: success, non-zero: error
 ******************************************************************************/
void sl_btmesh_on_node_provisioning_failed(uint16_t result)
{
  app_log("BT mesh node provisioning failed (result: 0x%04x)\r\n", result);
  sl_status_t status = sl_btmesh_LCD_write("prov failed...",
                                           SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
  (void)result;
}

// -----------------------------------------------------------------------------
// Factory Reset Callback

/*******************************************************************************
 * Shows the node reset information
 ******************************************************************************/
void app_show_btmesh_node_reset(void)
{
  app_log("Node reset\r\n");
  sl_status_t status = sl_btmesh_LCD_write("Node reset",
                                           SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
}

/*******************************************************************************
 * Called when full reset is established, before system reset
 ******************************************************************************/
void sl_btmesh_factory_reset_on_full_reset(void)
{
  app_log("Factory reset\r\n");
  sl_status_t status = sl_btmesh_LCD_write("Factory reset",
                                           SL_BTMESH_WSTK_LCD_ROW_STATUS_CFG_VAL);
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, status, "LCD write failed");
}
