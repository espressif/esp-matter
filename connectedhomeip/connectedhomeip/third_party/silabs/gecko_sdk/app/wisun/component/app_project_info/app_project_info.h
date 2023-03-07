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

#ifndef __APP_PROJECT_INFO_H__
#define __APP_PROJECT_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "app_project_info_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Include cmsis version
#define APP_PROJECT_INFO_CMSIS_VERSION_HEADER           "cmsis_version.h"

/// Include wisun version
#define APP_PROJECT_INFO_WISUN_VERSION_HEADER           "sl_wisun_version.h"

/// Include RAIL lib version (rail.h)
#define APP_PROJECT_INFO_RAIL_VERSION_HEADER0           "rail.h"

/// Include RAIL lib version (rail_types.h)
#define APP_PROJECT_INFO_RAIL_VERSION_HEADER1           "rail_types.h"

/// Include MicriumOS Kernel version
#define APP_PROJECT_INFO_MICRIUMOS_VERSION_HEADER       "os.h"

/// Include FreeRTOS Kernel version0 (FreeRTOS.h required before include task.h)
#define APP_PROJECT_INFO_FREERTOS_VERSION_HEADER0       "FreeRTOS.h"

/// Include FreeRTOS Kernel version1
#define APP_PROJECT_INFO_FREERTOS_VERSION_HEADER1       "task.h"

/// Include mbedtls version
#define APP_PROJECT_INFO_MBEDTLS_VERSION_HEADER         "mbedtls/build_info.h"

/// Version ID enumeration
typedef enum app_project_info_version_id {
  /// Compiler version
  APP_PROJECT_INFO_VERSION_ID_COMPILER,
  /// CMSIS version
  APP_PROJECT_INFO_VERSION_ID_CMSIS,
  /// STDC version
  APP_PROJECT_INFO_VERSION_ID_STDC,
  /// RAIL version
  APP_PROJECT_INFO_VERSION_ID_RAIL,
  /// RTOS version
  APP_PROJECT_INFO_VERSION_ID_RTOS,
  /// Wi-SUN version
  APP_PROJECT_INFO_VERSION_ID_WISUN,
  /// MbedTLS version
  APP_PROJECT_INFO_VERSION_ID_MBEDTLS
} app_project_info_version_id_t;

/// Version number structure definition
typedef struct app_project_info_version_num {
  /// Available
  bool available;
  /// Version number
  uint32_t val;
} app_project_info_version_num_t;

/// Version structure definition
typedef struct app_project_info_version {
  /// ID for getters
  app_project_info_version_id_t id;
  /// Major version number
  app_project_info_version_num_t major;
  /// Minor version number
  app_project_info_version_num_t minor;
  /// Patch version number
  app_project_info_version_num_t patch;
  /// Full version number
  app_project_info_version_num_t version;
  /// Version name string
  const char *name;
  /// Additional info
  const char *info;
  /// Internal setter callback
  void (*initializer)(struct app_project_info_version * dst_version);
  /// Next version ptr
  struct app_project_info_version *next;
} app_project_info_version_t;

typedef struct app_project_info {
  /// Project name
  const char *project_name;
  /// Available version count
  size_t version_count;
  /// Versions linked list
  const app_project_info_version_t *versions;
} app_project_info_t;

/// Printer function type definition
typedef int (* app_project_info_printer_t) (const char *format, ...);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Get project info
 * @details Get project info instance with constant list of versions
 * @param[out] dest Destination
 *****************************************************************************/
void app_project_info_get(app_project_info_t * const dest);

/**************************************************************************//**
 * @brief Print project info version
 * @details Print version, optionally in pretty format
 * @param[in] version Version instance
 * @param[in] printer Printer function pointer
 * @param[in] change_name Change version name (optional)
 * @param[in] pretty Is pretty indicator
 *****************************************************************************/
void app_project_info_print_version(const app_project_info_version_t * const version,
                                    app_project_info_printer_t printer,
                                    const char *change_name,
                                    bool pretty);

/**************************************************************************//**
 * @brief Print project info version pretty format
 * @details Prepared call of app_project_info_print_version
 * @param[in] version Version instance
 * @param[in] printer Printer function pointer
 *****************************************************************************/
static inline void app_project_info_print_version_pretty(const app_project_info_version_t * const version,
                                                         app_project_info_printer_t printer)
{
  app_project_info_print_version(version, printer, NULL, true);
}

/**************************************************************************//**
 * @brief Print project info version in json format
 * @details Json formatted string
 * @param[in] version Version instance
 * @param[in] printer Printer function pointer
 * @param[in] indent Indent count
 *****************************************************************************/
void app_project_info_print_version_json(const app_project_info_version_t * const version,
                                         app_project_info_printer_t printer,
                                         const uint8_t indent);

/**************************************************************************//**
 * @brief Get project info version
 * @details Get version by ID
 * @param[in] id ID
 * @param[in] pinfo Project info instance
 * @return const app_project_info_version_t* Read only version on success,
 *         NULL ptr on failure
 *****************************************************************************/
const app_project_info_version_t * app_project_info_get_version(const app_project_info_version_id_t id,
                                                                const app_project_info_t * const pinfo);

/**************************************************************************//**
 * @brief Print project info pretty
 * @details Pretty formatted project info
 * @param[in] info Project info
 * @param[in] printer Printer function pointer
 *****************************************************************************/
void app_project_info_print_pretty(const app_project_info_t * const info,
                                   app_project_info_printer_t printer);

/**************************************************************************//**
 * @brief Print project info json
 * @details Json formatted project info
 * @param[in] info Project info
 * @param[in] printer Printer function pointer
 *****************************************************************************/
void app_project_info_print_json(const app_project_info_t * const info,
                                 app_project_info_printer_t printer);
#ifdef __cplusplus
}
#endif
#endif
