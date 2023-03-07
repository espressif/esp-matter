/***************************************************************************//**
 * @file
 * @brief App BT Mesh Utility
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

#ifndef APP_BTMESH_UTIL_H
#define APP_BTMESH_UTIL_H

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#include "app_btmesh_util_config.h"

/***************************************************************************//**
 * @addtogroup App BT Mesh Utility
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @def app_btmesh_util_log_status_f
 * @brief Logs the btmesh API failures
 *
 * There are two special errors which are related to missing model configuration.
 * If the BT Mesh configurator (e.g. mobile app) does not bind application key
 * to the model or does not configure the model publication data then publish
 * or other API calls which leads to message sending fails.
 * From the BT Mesh node perspective it can't be determined if the configurator
 * missed these configuration steps on purpose or not so these errors are logged
 * at a less severe log level.
 ******************************************************************************/
#if defined(SL_CATALOG_APP_LOG_PRESENT)
#define app_btmesh_util_log_status_f(sc, ...)                               \
  do {                                                                      \
    switch (sc) {                                                           \
      case SL_STATUS_OK:                                                    \
        break;                                                              \
      case SL_STATUS_BT_MESH_APP_KEY_NOT_BOUND:                             \
        app_log_status_level_f(APP_BTMESH_UTIL_LOG_LEVEL_APP_KEY_NOT_BOUND, \
                               sc,                                          \
                               __VA_ARGS__);                                \
        break;                                                              \
      case SL_STATUS_BT_MESH_PUBLISH_NOT_CONFIGURED:                        \
        app_log_status_level_f(APP_BTMESH_UTIL_LOG_LEVEL_PUBLISH_NOT_CONF,  \
                               sc,                                          \
                               __VA_ARGS__);                                \
        break;                                                              \
      default:                                                              \
        app_log_status_level_f(APP_LOG_LEVEL_ERROR, sc, __VA_ARGS__);       \
        break;                                                              \
    }                                                                       \
  } while (0)
#else
#define app_btmesh_util_log_status_f(sc, ...) (void)sc
#endif

// This macro calculates the number of precompile logging enable request in the
// specific c file where the this header file is included from
#define APP_BTMESH_UTIL_COMPONENT_LOGGING             \
  (SL_BTMESH_CTL_CLIENT_LOGGING_CFG_VAL               \
   + SL_BTMESH_CTL_SERVER_LOGGING_CFG_VAL             \
   + SL_BTMESH_FRIEND_LOGGING_CFG_VAL                 \
   + SL_BTMESH_GENERIC_ONOFF_SERVER_LOGGING_CFG_VAL   \
   + SL_BTMESH_LC_SERVER_LOGGING_CFG_VAL              \
   + SL_BTMESH_LIGHTING_CLIENT_LOGGING_CFG_VAL        \
   + SL_BTMESH_LIGHTING_SERVER_LOGGING_CFG_VAL        \
   + SL_BTMESH_LPN_LOGGING_CFG_VAL                    \
   + SL_BTMESH_PROVISIONING_DECORATOR_LOGGING_CFG_VAL \
   + SL_BTMESH_SCENE_CLIENT_LOGGING_CFG_VAL           \
   + SL_BTMESH_SCHEDULER_SERVER_LOGGING_CFG_VAL       \
   + SL_BTMESH_SENSOR_CLIENT_LOGGING_CFG_VAL          \
   + SL_BTMESH_SENSOR_SERVER_LOGGING_CFG_VAL          \
   + SL_BTMESH_TIME_SERVER_LOGGING_CFG_VAL            \
   + SL_BTMESH_VENDOR_LOOPBACK_LOGGING_CFG_VAL)
// Component A shall not include the config file of another component B because
// <COMPONENT_B>_LOGGING macro of component B could turn on the logging in the
// component A unnecessarily. This is important in case of components with log.
// Warning is emitted to notify the developer to resolve the include problem.
#if (1 < APP_BTMESH_UTIL_COMPONENT_LOGGING)
#warning "Multiple BT Mesh component logging macros are defined in one file."
#endif

// Defines the local log macros for components based on the component configuration.
// The app_btmesh_util.h file shall be included after the component configuration
// header file include which defines the <COMPONENT>_LOGGING macro.
// Warning! The app_btmesh_util.h header file is context sensitive so it behaves
//          differently in different c files based on the file specific value of
//          the <COMPONENT>_LOGGING macros so this header shall not be included
//          from other header files.
#if defined(SL_CATALOG_APP_LOG_PRESENT) && (APP_BTMESH_UTIL_COMPONENT_LOGGING != 0)
#define log(...)                            app_log(__VA_ARGS__)
#define log_debug(...)                      app_log_debug(__VA_ARGS__)
#define log_info(...)                       app_log_info(__VA_ARGS__)
#define log_warning(...)                    app_log_warning(__VA_ARGS__)
#define log_error(...)                      app_log_error(__VA_ARGS__)
#define log_critical(...)                   app_log_critical(__VA_ARGS__)
#define log_status_debug(sc)                app_log_status_debug(sc)
#define log_status_info(sc)                 app_log_status_info(sc)
#define log_status_warning(sc)              app_log_status_warning(sc)
#define log_status_error(sc)                app_log_status_error(sc)
#define log_status_critical(sc)             app_log_status_critical(sc)
#define log_status_debug_f(sc, ...)         app_log_status_debug_f(sc, __VA_ARGS__)
#define log_status_info_f(sc, ...)          app_log_status_info_f(sc, __VA_ARGS__)
#define log_status_warning_f(sc, ...)       app_log_status_warning_f(sc, __VA_ARGS__)
#define log_status_error_f(sc, ...)         app_log_status_error_f(sc, __VA_ARGS__)
#define log_status_critical_f(sc, ...)      app_log_status_critical_f(sc, __VA_ARGS__)
#define log_status(sc)                      app_log_status(sc)
#define log_status_level(level, sc)         app_log_status_level(level, sc)
#define log_status_f(sc, ...)               app_log_status_f(sc, __VA_ARGS__)
#define log_status_level_f(level, sc, ...)  app_log_status_level_f(level, sc, __VA_ARGS__)
#define log_btmesh_status_f(sc, ...)        app_btmesh_util_log_status_f(sc, __VA_ARGS__)
#else
#define log(...)
#define log_debug(...)
#define log_info(...)
#define log_warning(...)
#define log_error(...)
#define log_critical(...)
#define log_status_debug(sc) (void)sc
#define log_status_info(sc) (void)sc
#define log_status_warning(sc) (void)sc
#define log_status_error(sc) (void)sc
#define log_status_critical(sc) (void)sc
#define log_status_debug_f(sc, ...) (void)sc
#define log_status_info_f(sc, ...) (void)sc
#define log_status_warning_f(sc, ...) (void)sc
#define log_status_error_f(sc, ...) (void)sc
#define log_status_critical_f(sc, ...) (void)sc
#define log_status(sc) (void)sc
#define log_status_level(level, sc) (void)sc
#define log_status_f(sc, ...) (void)sc
#define log_status_level_f(level, sc, ...) (void)sc
#define log_btmesh_status_f(sc, ...) (void)sc
#endif // defined(SL_CATALOG_APP_LOG_PRESENT) && (APP_BTMESH_UTIL_COMPONENT_LOGGING != 0)

/** @} (end addtogroup App BT Mesh Utility) */

#endif /* APP_BTMESH_UTIL_H */
