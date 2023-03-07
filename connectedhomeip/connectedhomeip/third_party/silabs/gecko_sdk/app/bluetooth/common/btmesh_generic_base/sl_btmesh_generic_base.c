/***************************************************************************//**
 * @file
 * @brief Mesh Generic Base library
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

#include "app_assert.h"

#include "sl_bt_api.h"
#include "sl_btmesh_api.h"

#include "sl_btmesh_generic_model_capi_types.h"
#include "sl_btmesh_lib.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#include "sl_btmesh_generic_base.h"
#include "sl_btmesh_generic_base_config.h"

sl_status_t sl_btmesh_generic_base_init(void)
{
  return mesh_lib_init(SL_BTMESH_GENERIC_BASE_REGISTRY_INIT_SIZE,
                       SL_BTMESH_GENERIC_BASE_INCREMENT_CFG_VAL);
}

void sl_btmesh_generic_base_on_event(sl_btmesh_msg_t *evt)
{
  sl_status_t sc = SL_STATUS_OK;
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
#if SL_BTMESH_GENERIC_BASE_SERVER_CFG_VAL || defined(SL_CATALOG_BTMESH_LIGHTING_SERVER_PRESENT) \
      || defined(SL_CATALOG_BTMESH_CTL_SERVER_PRESENT)                                          \
      || defined(SL_CATALOG_BTMESH_HSL_SERVER_PRESENT)                                          \
      || defined(SL_CATALOG_BTMESH_GENERIC_ONOFF_SERVER_PRESENT)

#if SL_BTMESH_GENERIC_CTL_SERVER_INIT_CFG_VAL || defined(SL_CATALOG_BTMESH_CTL_SERVER_PRESENT)      \
      || SL_BTMESH_GENERIC_HSL_SERVER_INIT_CFG_VAL || defined(SL_CATALOG_BTMESH_HSL_SERVER_PRESENT) \
      || SL_BTMESH_GENERIC_POWER_LEVEL_SERVER_INIT_CFG_VAL

  #if SL_BTMESH_GENERIC_CTL_SERVER_INIT_CFG_VAL || defined(SL_CATALOG_BTMESH_CTL_SERVER_PRESENT)
      sc = sl_btmesh_generic_server_init_ctl();
      app_assert_status_f(sc, "Failed to init ctl server\n");
  #endif // SL_BTMESH_GENERIC_CTL_SERVER_INIT_CFG_VAL
  #if SL_BTMESH_GENERIC_HSL_SERVER_INIT_CFG_VAL || defined(SL_CATALOG_BTMESH_HSL_SERVER_PRESENT)
      sc = sl_btmesh_generic_server_init_hsl();
      app_assert_status_f(sc, "Failed to init hsl server\n");
  #endif // SL_BTMESH_GENERIC_HSL_SERVER_INIT_CFG_VAL
  #if SL_BTMESH_GENERIC_POWER_LEVEL_SERVER_INIT_CFG_VAL
      sc = sl_btmesh_generic_server_init_power_level();
      app_assert_status_f(sc, "Failed to init power level server\n");
  #endif // SL_BTMESH_GENERIC_POWER_LEVEL_SERVER_INIT_CFG_VAL
#elif SL_BTMESH_GENERIC_LIGHTNESS_SERVER_INIT_CFG_VAL || defined(SL_CATALOG_BTMESH_LIGHTING_SERVER_PRESENT)
      sc = sl_btmesh_generic_server_init_lightness();
      app_assert_status_f(sc, "Failed to init lightness server\n");
#else // SL_BTMESH_GENERIC_CTL_SERVER_INIT_CFG_VAL || SL_BTMESH_GENERIC_HSL_SERVER_INIT_CFG_VAL || SL_BTMESH_GENERIC_POWER_LEVEL_SERVER_INIT_CFG_VAL
  #if SL_BTMESH_GENERIC_LEVEL_SERVER_INIT_CFG_VAL
      sc = sl_btmesh_generic_server_init_level();
      app_assert_status_f(sc, "Failed to init level server\n");
  #endif // SL_BTMESH_GENERIC_LEVEL_SERVER_INIT_CFG_VAL
  #if SL_BTMESH_GENERIC_POWER_ON_OFF_SERVER_INIT_CFG_VAL
      sc = sl_btmesh_generic_server_init_power_on_off();
      app_assert_status_f(sc, "Failed to init power on/off server\n");
  #else //SL_BTMESH_GENERIC_POWER_ON_OFF_SERVER_INIT_CFG_VAL
    #if SL_BTMESH_GENERIC_ON_OFF_SERVER_INIT_CFG_VAL || defined(SL_CATALOG_BTMESH_GENERIC_ONOFF_SERVER_PRESENT)
      sc = sl_btmesh_generic_server_init_on_off();
      app_assert_status_f(sc, "Failed to init on/off server\n");
    #endif // SL_BTMESH_GENERIC_ON_OFF_SERVER_INIT_CFG_VAL
    #if SL_BTMESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_INIT_CFG_VAL
      sc = sl_btmesh_generic_server_init_default_transition_time();
      app_assert_status_f(sc, "Failed to init default transition time server\n");
    #endif // SL_BTMESH_GENERIC_DEFAULT_TRANSITION_TIME_SERVER_INIT_CFG_VAL
  #endif //SL_BTMESH_GENERIC_POWER_ON_OFF_SERVER_INIT_CFG_VAL
#endif // SL_BTMESH_GENERIC_CTL_SERVER_INIT_CFG_VAL || SL_BTMESH_GENERIC_HSL_SERVER_INIT_CFG_VAL || SL_BTMESH_GENERIC_POWER_LEVEL_SERVER_INIT_CFG_VAL
#if SL_BTMESH_GENERIC_BATTERY_SERVER_INIT_CFG_VAL
      sc = sl_btmesh_generic_server_init_battery();
      app_assert_status_f(sc, "Failed to init battery server\n");
#endif // SL_BTMESH_GENERIC_BATTERY_SERVER_INIT_CFG_VAL
#if SL_BTMESH_GENERIC_LOCATION_SERVER_INIT_CFG_VAL
      sc = sl_btmesh_generic_server_init_location();
      app_assert_status_f(sc, "Failed to init location server\n");
#endif // SL_BTMESH_GENERIC_LOCATION_SERVER_INIT_CFG_VAL
#if SL_BTMESH_GENERIC_PROPERTY_SERVER_INIT_CFG_VAL
      sc = sl_btmesh_generic_server_init_property();
      app_assert_status_f(sc, "Failed to init property server\n");
#endif // SL_BTMESH_GENERIC_PROPERTY_SERVER_INIT_CFG_VAL
      sc = sl_btmesh_generic_server_init_common();
      app_assert_status_f(sc, "Failed to common init Generic Server\n");
#endif // SL_BTMESH_GENERIC_BASE_SERVER_CFG_VAL

#if SL_BTMESH_GENERIC_BASE_CLIENT_CFG_VAL || defined(SL_CATALOG_BTMESH_LIGHTING_CLIENT_PRESENT) \
      || defined(SL_CATALOG_BTMESH_CTL_CLIENT_PRESENT)

#if SL_BTMESH_GENERIC_ON_OFF_CLIENT_INIT_CFG_VAL || defined(SL_CATALOG_BTMESH_LIGHTING_CLIENT_PRESENT)
      sc = sl_btmesh_generic_client_init_on_off();
      app_assert_status_f(sc, "Failed to init on/off client\n");
#endif // SL_BTMESH_GENERIC_ON_OFF_CLIENT_INIT_CFG_VAL || defined(SL_CATALOG_BTMESH_LIGHTING_CLIENT_PRESENT)
#if SL_BTMESH_GENERIC_LEVEL_SERVER_INIT_CFG_VAL
      sc = sl_btmesh_generic_client_init_level();
      app_assert_status_f(sc, "Failed to init level client\n");
#endif // SL_BTMESH_GENERIC_LEVEL_SERVER_INIT_CFG_VAL
#if SL_BTMESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_INIT_CFG_VAL
      sc = sl_btmesh_generic_client_init_default_transition_time();
      app_assert_status_f(sc, "Failed to init default transition time client\n");
#endif // SL_BTMESH_GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_INIT_CFG_VAL
#if SL_BTMESH_GENERIC_POWER_ON_OFF_CLIENT_INIT_CFG_VAL
      sc = sl_btmesh_generic_client_init_power_on_off();
      app_assert_status_f(sc, "Failed to init power on/off client\n");
#endif // SL_BTMESH_GENERIC_POWER_ON_OFF_CLIENT_INIT_CFG_VAL
#if SL_BTMESH_GENERIC_POWER_LEVEL_CLIENT_INIT_CFG_VAL
      sc = sl_btmesh_generic_client_init_power_level();
      app_assert_status_f(sc, "Failed to init power level client\n");
#endif // SL_BTMESH_GENERIC_POWER_LEVEL_CLIENT_INIT_CFG_VAL
#if SL_BTMESH_GENERIC_BATTERY_CLIENT_INIT_CFG_VAL
      sc = sl_btmesh_generic_client_init_battery();
      app_assert_status_f(sc, "Failed to init battery client\n");
#endif // SL_BTMESH_GENERIC_BATTERY_CLIENT_INIT_CFG_VAL
#if SL_BTMESH_GENERIC_LOCATION_CLIENT_INIT_CFG_VAL
      sc = sl_btmesh_generic_client_init_location();
      app_assert_status_f(sc, "Failed to init location client\n");
#endif // SL_BTMESH_GENERIC_LOCATION_CLIENT_INIT_CFG_VAL
#if SL_BTMESH_GENERIC_PROPERTY_CLIENT_INIT_CFG_VAL
      sc = sl_btmesh_generic_client_init_property();
      app_assert_status_f(sc, "Failed to init property client\n");
#endif // SL_BTMESH_GENERIC_PROPERTY_CLIENT_INIT_CFG_VAL
#if SL_BTMESH_GENERIC_LIGHTNESS_CLIENT_INIT_CFG_VAL || defined(SL_CATALOG_BTMESH_LIGHTING_CLIENT_PRESENT)
      sc = sl_btmesh_generic_client_init_lightness();
      app_assert_status_f(sc, "Failed to init lightness client\n");
#endif // SL_BTMESH_GENERIC_LIGHTNESS_CLIENT_INIT_CFG_VAL || defined(SL_CATALOG_BTMESH_LIGHTING_CLIENT_PRESENT)
#if SL_BTMESH_GENERIC_CTL_CLIENT_INIT_CFG_VAL || defined(SL_CATALOG_BTMESH_CTL_CLIENT_PRESENT)
      sc = sl_btmesh_generic_client_init_ctl();
      app_assert_status_f(sc, "Failed to init ctl client\n");
#endif // SL_BTMESH_GENERIC_CTL_CLIENT_INIT_CFG_VAL || defined(SL_CATALOG_BTMESH_CTL_CLIENT_PRESENT)
#if SL_BTMESH_GENERIC_HSL_CLIENT_INIT_CFG_VAL
      sc = sl_btmesh_generic_client_init_hsl();
      app_assert_status_f(sc, "Failed to init hsl client\n");
#endif // SL_BTMESH_GENERIC_HSL_CLIENT_INIT_CFG_VAL
      sc = sl_btmesh_generic_client_init_common();
      app_assert_status_f(sc, "Failed to common init Generic Client\n");

#endif // SL_BTMESH_GENERIC_BASE_CLIENT_CFG_VAL
      break;
#if SL_BTMESH_GENERIC_BASE_SERVER_CFG_VAL || defined(SL_CATALOG_BTMESH_LIGHTING_SERVER_PRESENT)
    case sl_btmesh_evt_generic_server_client_request_id:
    // intentional fall through
    case sl_btmesh_evt_generic_server_state_recall_id:
    // intentional fall through
    case sl_btmesh_evt_generic_server_state_changed_id:
      mesh_lib_generic_server_event_handler(evt);
      break;
#endif // SL_BTMESH_GENERIC_BASE_SERVER_CFG_VAL
#if SL_BTMESH_GENERIC_BASE_CLIENT_CFG_VAL
    case sl_btmesh_evt_generic_client_server_status_id:
      mesh_lib_generic_client_event_handler(evt);
      break;
#endif // SL_BTMESH_GENERIC_BASE_CLIENT_CFG_VAL
  }
}
