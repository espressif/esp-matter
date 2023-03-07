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

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#include <string.h>
#include "app_project_info.h"
#include "sl_component_catalog.h"

#if APP_PROJECT_INFO_CMSIS_VERSION_ENABLED
#include APP_PROJECT_INFO_CMSIS_VERSION_HEADER
#endif

#if APP_PROJECT_INFO_STDC_VERSION_ENABLED
#endif

#if APP_PROJECT_INFO_RAIL_VERSION_ENABLED
#include APP_PROJECT_INFO_RAIL_VERSION_HEADER0
#include APP_PROJECT_INFO_RAIL_VERSION_HEADER1
#endif

#if APP_PROJECT_INFO_RTOS_VERSION_ENABLED
#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
#include APP_PROJECT_INFO_MICRIUMOS_VERSION_HEADER
#endif
#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
#include APP_PROJECT_INFO_FREERTOS_VERSION_HEADER0
#include APP_PROJECT_INFO_FREERTOS_VERSION_HEADER1
#endif
#endif

#if APP_PROJECT_INFO_WISUN_VERSION_ENABLED
#if defined(SL_CATALOG_WISUN_EVENT_MGR_PRESENT)
#include APP_PROJECT_INFO_WISUN_VERSION_HEADER
#endif
#endif

#if APP_PROJECT_INFO_MBEDTLS_VERSION_ENABLED
#include APP_PROJECT_INFO_MBEDTLS_VERSION_HEADER
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Default Json indent
#define app_project_info_DEFAULT_JSON_INDENT_SIZE    2U

/// Print with indent macro function
#define __print_with_indent(__indent, __printer, __format, ...) \
  do {                                                          \
    _print_indent(__indent, __printer);                         \
    (__printer)(__format, ##__VA_ARGS__);                       \
  } while (0)

/// Reset version member
#define __reset_version_member \
  { .available = false, .val = 0UL }

/// Reset version
#define __reset_version              \
  .major   = __reset_version_member, \
  .minor   = __reset_version_member, \
  .patch   = __reset_version_member, \
  .version = __reset_version_member,

/// Calculate GCC full version
#define __calc_gcc_full_version(__major, __minor, __patch) \
  ((uint32_t)(__major) * 10000UL + (uint32_t)(__minor) * 100UL + (uint32_t)(__patch))

/// Calculate IAR major version
#define __calc_iar_major_version(__full) \
  ((uint32_t)(__full) / 1000000UL)

/// Calculate IAR minor version
#define __calc_iar_minor_version(__full) \
  (((uint32_t)(__full) - __calc_iar_major_version((uint32_t)(__full)) * 1000000UL) / 1000UL)

/// Calculate IAR patch version
#define __calc_iar_patch_version(__full) \
  ((uint32_t)(__full) % 1000UL)

/// Calculate STDC major version
#define __calc_stdc_major_version(__full) \
  ((uint32_t)(__full) / 100UL)

/// Calculate STDC minor version
#define __calc_stdc_minor_version(__full) \
  ((uint32_t)(__full) % 100UL)

/// Calculate RAIL full version
#define __calc_rail_full_version(__major, __minor, __patch) \
  ((uint32_t)(__major) * 10000UL + (uint32_t)(__minor) * 100UL + (uint32_t)(__patch))

/// Calculate Wi-SUN Full version
#define __calc_wisun_full_version(__major, __minor, __patch) \
  ((uint32_t)(__major) * 10000UL + (uint32_t)(__minor) * 100UL + (uint32_t)(__patch))

/// Calculate MicriumOS kernel major version
#define __calc_micriumos_kernel_major_version(__full) \
  ((uint32_t)(__full) / 10000UL)

/// Calculate MicriumOS kernel minor version
#define __calc_micriumos_kernel_minor_version(__full) \
  ((uint32_t)(((__full) - __calc_micriumos_kernel_major_version(__full) * 10000UL) / 100UL))

/// Calculate MicriumOS kernel patch version
#define __calc_micriumos_kernel_patch_version(__full) \
  ((uint32_t)((__full) % 100LU))

/// Calculate freertos full version
#define __calc_freertos_full_version(__major, __minor, __patch) \
  ((uint32_t)(__major) * 10000UL + (uint32_t)(__minor) * 100UL + (uint32_t)(__patch))

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Print indent
 * @details Helper function
 * @param[in] indent Indent
 * @param[in] printer Printer ptr
 *****************************************************************************/
static inline void _print_indent(const uint8_t indent, app_project_info_printer_t printer);

/**************************************************************************//**
 * @brief Bool to json format
 * @details helper function
 * @param[in] bval bool value
 * @return const char* json format
 *****************************************************************************/
static inline const char * _bool_to_json(const bool bval);

#if APP_PROJECT_INFO_RAIL_VERSION_ENABLED
/**************************************************************************//**
 * @brief RAIL version internal setter
 * @details Callback function
 * @param[out] version Version pointer to set
 *****************************************************************************/
static void _rail_version_internal_setter(app_project_info_version_t *version);
#endif

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Versions list
static app_project_info_version_t _versions[] = {
#if APP_PROJECT_INFO_COMPILER_VERSION_ENABLED
// GCC Version
#if defined(__GNUC__)
  {
    .id   = APP_PROJECT_INFO_VERSION_ID_COMPILER,
    .name = "Compiler (GCC)",
    .info = "Full version: major * 10000 + minor * 100 + patch",
  #if !defined(__GNUC_MINOR__) || !defined(__GNUC_PATCHLEVEL__)
    __reset_version
  #else
    .major   = { .available = true, .val = __GNUC__ },
    .minor   = { .available = true, .val = __GNUC_MINOR__ },
    .patch   = { .available = true, .val = __GNUC_PATCHLEVEL__ },
    .version = { .available = true, .val = __calc_gcc_full_version(__GNUC__,
                                                                   __GNUC_MINOR__,
                                                                   __GNUC_PATCHLEVEL__) },
  #endif
    .initializer = NULL,
    .next = NULL
  },
#elif defined(__ICCARM__)
  {
// IAR Version
    .id   = APP_PROJECT_INFO_VERSION_ID_COMPILER,
    .name = "Compiler (IAR)",
    .info = "Full version: major * 1000000 + minor * 1000 + patch",
  #if !defined(__VER__)
    __reset_version
  #else
    .major   = { .available = true, .val = __calc_iar_major_version(__VER__) },
    .minor   = { .available = true, .val = __calc_iar_minor_version(__VER__) },
    .patch   = { .available = true, .val = __calc_iar_patch_version(__VER__) },
    .version = { .available = true, .val = __VER__ },
  #endif
    .initializer = NULL,
    .next = NULL
  },
#endif
#endif

// CMSIS version
#if APP_PROJECT_INFO_CMSIS_VERSION_ENABLED
  {
    .id   = APP_PROJECT_INFO_VERSION_ID_CMSIS,
    .name = "CMSIS",
    .info = "Full version: (major << 16) | minor",
  #if !defined(__CM_CMSIS_VERSION_MAIN) || !defined(__CM_CMSIS_VERSION_SUB) || !defined(__CM_CMSIS_VERSION_SUB)
    __reset_version
  #else
    .major   = { .available = true, .val = __CM_CMSIS_VERSION_MAIN },
    .minor   = { .available = true, .val = __CM_CMSIS_VERSION_SUB },
    .patch   = { .available = false, .val = 0LU },
    .version = { .available = true, .val = __CM_CMSIS_VERSION },
  #endif
    .initializer = NULL,
    .next = NULL
  },
#endif

// STDC version
#if APP_PROJECT_INFO_STDC_VERSION_ENABLED
  {
    .id   = APP_PROJECT_INFO_VERSION_ID_STDC,
    .name = "STDC",
    .info = "Full version: major + minor",
  #if !defined(__STDC_VERSION__)
    __reset_version
  #else
    .major   = { .available = true, .val = __calc_stdc_major_version(__STDC_VERSION__) },
    .minor   = { .available = true, .val = __calc_stdc_minor_version(__STDC_VERSION__) },
    .patch   = { .available = false, .val = 0LU },
    .version = { .available = true, .val = __STDC_VERSION__ },
  #endif
    .initializer = NULL,
    .next = NULL
  },
#endif

// RAIL lib version (setted by callback)
#if APP_PROJECT_INFO_RAIL_VERSION_ENABLED
  {
    .id   = APP_PROJECT_INFO_VERSION_ID_RAIL,
    .name = "RAIL",
    .info = "",
    .initializer = _rail_version_internal_setter,
    .next = NULL
  },
#endif

// OS Versions
#if APP_PROJECT_INFO_RTOS_VERSION_ENABLED && defined(SL_CATALOG_KERNEL_PRESENT)

// Micrium OS
#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
  {
    .id   = APP_PROJECT_INFO_VERSION_ID_RTOS,
    .name = "Micrium OS kernel",
    .info = "Full version: major * 10000 + minor * 100 + patch",
  #if !defined(OS_VERSION)
    __reset_version
  #else
    .major   = { .available = true, .val = __calc_micriumos_kernel_major_version(OS_VERSION) },
    .minor   = { .available = true, .val = __calc_micriumos_kernel_minor_version(OS_VERSION) },
    .patch   = { .available = true, .val = __calc_micriumos_kernel_patch_version(OS_VERSION) },
    .version = { .available = true, .val = OS_VERSION },
  #endif
    .initializer = NULL,
    .next = NULL
  },
#endif
#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
  {
    .id   = APP_PROJECT_INFO_VERSION_ID_RTOS,
    .name = "FreeRTOS kernel",
  #if defined(tskKERNEL_VERSION_NUMBER)
    .info = tskKERNEL_VERSION_NUMBER,
  #else
    .info = "Full version: major * 10000 + minor * 100 + patch",
  #endif
  #if !defined(tskKERNEL_VERSION_MAJOR) || !defined(tskKERNEL_VERSION_MINOR) || !defined(tskKERNEL_VERSION_BUILD)
    __reset_version
  #else
    .major   = { .available = true, .val = tskKERNEL_VERSION_MAJOR },
    .minor   = { .available = true, .val = tskKERNEL_VERSION_MINOR },
    .patch   = { .available = true, .val = tskKERNEL_VERSION_BUILD },
    .version = { .available = true, .val = __calc_freertos_full_version(tskKERNEL_VERSION_MAJOR,
                                                                        tskKERNEL_VERSION_MINOR,
                                                                        tskKERNEL_VERSION_BUILD) },
  #endif
    .initializer = NULL,
    .next = NULL
  },
#endif
#endif

// Wi-SUN Version
#if APP_PROJECT_INFO_WISUN_VERSION_ENABLED
  {
    .id   = APP_PROJECT_INFO_VERSION_ID_WISUN,
    .name = "Wi-SUN",
    .info = "Full version: major * 10000 + minor * 100 + patch",
  #if !defined(SL_WISUN_VERSION_MAJOR) || !defined(SL_WISUN_VERSION_MINOR) || !defined(SL_WISUN_VERSION_PATCH)
    __reset_version
  #else
    .major   = { .available = true, .val = SL_WISUN_VERSION_MAJOR },
    .minor   = { .available = true, .val = SL_WISUN_VERSION_MINOR },
    .patch   = { .available = true, .val = SL_WISUN_VERSION_PATCH },
    .version = { .available = true, .val = __calc_wisun_full_version(SL_WISUN_VERSION_MAJOR,
                                                                     SL_WISUN_VERSION_MINOR,
                                                                     SL_WISUN_VERSION_PATCH) },
  #endif
    .initializer = NULL,
    .next = NULL
  },
#endif

// mbed TLS version
#if APP_PROJECT_INFO_MBEDTLS_VERSION_ENABLED
  {
    .id   = APP_PROJECT_INFO_VERSION_ID_MBEDTLS,
    .name = "mbed TLS",
  #if defined(MBEDTLS_VERSION_STRING_FULL)
    .info = MBEDTLS_VERSION_STRING_FULL,
  #else
    .info = "",
  #endif
  #if !defined(MBEDTLS_VERSION_MAJOR) || !defined(MBEDTLS_VERSION_MINOR) \
    || !defined(MBEDTLS_VERSION_PATCH) || !defined(MBEDTLS_VERSION_NUMBER)
    __reset_version
  #else
    .major   = { .available = true, .val = MBEDTLS_VERSION_MAJOR },
    .minor   = { .available = true, .val = MBEDTLS_VERSION_MINOR },
    .patch   = { .available = true, .val = MBEDTLS_VERSION_PATCH },
    .version = { .available = true, .val = MBEDTLS_VERSION_NUMBER },
  #endif
    .initializer = NULL,
    .next = NULL
  },
#endif
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void app_project_info_get(app_project_info_t * const dest)
{
  static bool initialized = false;
  dest->version_count = sizeof(_versions) / sizeof(app_project_info_version_t);
  if (dest->version_count <= 1U) {
    return;
  }

  // init if it is required
  if (!initialized) {
    for (size_t i = 0; i < dest->version_count; ++i) {
      // create linked list
      if (i < dest->version_count - 1) {
        _versions[i].next = &_versions[i + 1];
      }
      // call internal initializer if it is set
      if (_versions[i].initializer != NULL) {
        _versions[i].initializer(&_versions[i]);
      }
    }
    initialized = true;
  }

  // dest->versions->next = NULL;
  dest->versions = (const app_project_info_version_t *)_versions;
}

void app_project_info_print_pretty(const app_project_info_t * const info, app_project_info_printer_t printer)
{
  app_project_info_version_t * ver = NULL;

  if (info == NULL || printer == NULL) {
    return;
  }

  ver = (app_project_info_version_t * ) info->versions;
  printer("%s\n", info->project_name);

  printer("Versions:\n");
  while (ver != NULL) {
    printer("  * ");
    app_project_info_print_version_pretty((const app_project_info_version_t *)ver, printer);
    ver = ver->next;
  }
}

const app_project_info_version_t * app_project_info_get_version(app_project_info_version_id_t id, const app_project_info_t * const pinfo)
{
  app_project_info_version_t * ver = NULL;

  if (pinfo == NULL) {
    return NULL;
  }

  ver = (app_project_info_version_t *) pinfo->versions;
  while (ver != NULL) {
    if (ver->id == id) {
      return (const app_project_info_version_t *)ver;
    }
    ver = ver->next;
  }
  return NULL;
}

void app_project_info_print_version(const app_project_info_version_t * const version,
                                    app_project_info_printer_t printer,
                                    const char *change_name,
                                    bool pretty)
{
  if (version == NULL || printer == NULL || version->name == NULL) {
    return;
  }

  // if change name is set, print it instead of stored name
  printer("%s: ", change_name == NULL ? version->name : change_name);
  if (pretty) {
    if (version->major.available) {
      printer("%lu", version->major.val);
    }
    if (version->minor.available) {
      printer(".%lu", version->minor.val);
    }
    if (version->patch.available) {
      printer(".%lu", version->patch.val);
    }
    printer("\n");
  } else if (version->version.available) {
    printer("%lu\n", version->version.val);
  }
}

void app_project_info_print_version_json(const app_project_info_version_t * const version,
                                         app_project_info_printer_t printer,
                                         const uint8_t indent)
{
  if (printer == NULL || version == NULL) {
    return;
  }
  __print_with_indent(indent, printer, "{\n");
  __print_with_indent(indent + 1, printer, "\"name\"    : \"%s\",\n", version->name);
  __print_with_indent(indent + 1, printer, "\"info\"    : \"%s\",\n", version->info);
  __print_with_indent(indent + 1, printer, "\"major\"   : { \"available\": %s, \"val\": %lu },\n",
                      _bool_to_json(version->major.available), version->major.val);
  __print_with_indent(indent + 1, printer, "\"minor\"   : { \"available\": %s, \"val\": %lu },\n",
                      _bool_to_json(version->minor.available), version->minor.val);
  __print_with_indent(indent + 1, printer, "\"patch\"   : { \"available\": %s, \"val\": %lu },\n",
                      _bool_to_json(version->patch.available), version->patch.val);
  __print_with_indent(indent + 1, printer, "\"version\" : { \"available\": %s, \"val\": %lu }\n",
                      _bool_to_json(version->version.available), version->version.val);
  __print_with_indent(indent, printer, "}");
}

void app_project_info_print_json(const app_project_info_t * const info,
                                 app_project_info_printer_t printer)
{
  if (info == NULL || printer == NULL) {
    return;
  }

  __print_with_indent(0U, printer, "{\n");
  __print_with_indent(1U, printer, "\"project_name\" : \"%s\",\n",
                      info->project_name == NULL ?  "nil" : info->project_name);
  __print_with_indent(1U, printer, "\"version_count\": %lu,\n", info->version_count);
  __print_with_indent(1U, printer, "\"versions\":\n");
  __print_with_indent(2U, printer, "[\n");

  for (size_t i = 0; i < info->version_count; ++i) {
    app_project_info_print_version_json(&info->versions[i], printer, 3U);
    printer("%s", i == info->version_count - 1 ? "\n" : ",\n");
  }

  __print_with_indent(2U, printer, "]\n");
  __print_with_indent(0U, printer, "}\n");
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static inline const char * _bool_to_json(const bool bval)
{
  return bval ? "true" : "false";
}

static inline void _print_indent(const uint8_t indent, app_project_info_printer_t printer)
{
  for (uint8_t i = 0; i < indent * app_project_info_DEFAULT_JSON_INDENT_SIZE; ++i) {
    printer(" ");
  }
}

#if APP_PROJECT_INFO_RAIL_VERSION_ENABLED
static void _rail_version_internal_setter(app_project_info_version_t *version)
{
  RAIL_Version_t rail_version = { 0 };

  // Get RAIL version
  RAIL_GetVersion(&rail_version, true);

  version->id = APP_PROJECT_INFO_VERSION_ID_RAIL,
  version->major.available = true;
  version->major.val = rail_version.major;
  version->minor.available = true;
  version->minor.val = rail_version.minor;
  version->patch.available = true;
  version->patch.val = rail_version.rev;
  version->version.available = true;
  version->version.val = __calc_rail_full_version(rail_version.major, rail_version.minor, rail_version.rev);
  version->info = rail_version.multiprotocol ? "Multi protocol" : "Single protocol";
}
#endif
