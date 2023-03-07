/***************************************************************************//**
 * @brief Internal types and defines for Bluetooth configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SLI_BT_CONFIG_DEFS_H
#define SLI_BT_CONFIG_DEFS_H

#include "sl_status.h"

/** @brief Forward declaration of the internal feature structure */
struct sli_bt_feature;

/** @brief Forward declaration of the internal BGAPI class structure */
struct sli_bgapi_class;

/**
 * @brief Forward declaration of internal feature config structure
 *
 * Note that this is only used as an opaque type that is never instantiated
 * directly. The actual configuration structure is defined and populated in
 * feature-specific files.
 */
struct sli_bt_feature_config;

/** @brief Structure to specify the use of a feature and its configuration */
struct sli_bt_feature_use {
  const struct sli_bt_feature        *feature; /**< Pointer to the feature definition */
  const struct sli_bt_feature_config *config;  /**< Pointer to the feature-specific configuration */
};

/**
 * @brief Macro that expands to the name of a BGAPI class structure
 *
 * @param[in] category_name Short category name string without quotes, for
 *   example bt, btmesh, or btctrl
 * @param[in] class_name Short class name string without quotes, for example
 *   advertiser or connection
 */
#define SLI_BT_BGAPI_CLASS_NAME(category_name, class_name, mode) \
  sli_bgapi_class_ ## category_name ## _ ## class_name ## _ ## mode

/**
 * @brief Macro to forward declare a BGAPI class structure
 */
#define SLI_BT_DECLARE_BGAPI_CLASS(category_name, class_name)                                   \
  extern const struct sli_bgapi_class SLI_BT_BGAPI_CLASS_NAME(category_name, class_name, full); \
  extern const struct sli_bgapi_class SLI_BT_BGAPI_CLASS_NAME(category_name, class_name, optimized)

#ifdef SL_BT_API_FULL
/**
 * @brief Macro to declare the use of a BGAPI class
 *
 * This macro is used by stack-specific initialization code to declare the use
 * of a BGAPI class in the array of used BGAPI classes.
 *
 * @param[in] category_name Short category name string without quotes, for
 *   example bt, btmesh, or btctrl
 * @param[in] class_name Short class name string without quotes, for example
 *   advertiser or connection
 */
#define SLI_BT_USE_BGAPI_CLASS(category_name, class_name) \
  & SLI_BT_BGAPI_CLASS_NAME(category_name, class_name, full)
#else
#define SLI_BT_USE_BGAPI_CLASS(category_name, class_name) \
  & SLI_BT_BGAPI_CLASS_NAME(category_name, class_name, optimized)
#endif

/**
 * @brief Macro that expands to the name of a feature definition structure
 *
 * @param[in] category_name Short category name string without quotes, for
 *   example bt, btmesh, or btctrl
 * @param[in] feature_name Short feature name string without quotes, for example
 *   advertiser or connection
 * @param[in] init_mode Feature initialization mode without quotes: init_always
 *   or on_demand
 */
#define SLI_BT_FEATURE_DEFINITION_NAME(category_name, feature_name, init_mode) \
  sli_feature_ ## category_name ## _ ## feature_name ## _ ## init_mode

/**
 * @brief Macro that expands to the name of a feature configuration structure
 *
 * @param[in] category_name Short category name string without quotes, for
 *   example bt, btmesh, or btctrl
 * @param[in] feature_name Short feature name string without quotes, for example
 *   advertiser or connection
 */
#define SLI_BT_FEATURE_CONFIG_NAME(category_name, feature_name) \
  sli_feature_ ## category_name ## _ ## feature_name ## _config

/**
 * @brief Macro to forward declare a feature definition structure
 *
 * @param[in] category_name Short category name string without quotes, for
 *   example bt, btmesh, or btctrl
 * @param[in] feature_name Short feature name string without quotes, for example
 *   advertiser or connection
 */
#define SLI_BT_DECLARE_FEATURE(category_name, feature_name)                                                    \
  extern const struct sli_bt_feature SLI_BT_FEATURE_DEFINITION_NAME(category_name, feature_name, init_always); \
  extern const struct sli_bt_feature SLI_BT_FEATURE_DEFINITION_NAME(category_name, feature_name, on_demand)

/**
 * @brief Macro to forward declare an instance of a feature configuration structure
 *
 * This macro is used by stack-specific initialization code to declare relevant
 * configuration structures before populating the array of optional features.
 *
 * @param[in] category_name Short category name string without quotes, for
 *   example bt, btmesh, or btctrl
 * @param[in] feature_name Short feature name string without quotes, for example
 *   advertiser or connection
 */
#define SLI_BT_DECLARE_FEATURE_CONFIG(category_name, feature_name) \
  extern const struct sli_bt_feature_config SLI_BT_FEATURE_CONFIG_NAME(category_name, feature_name)

/**
 * @brief Macro to define an instance of a feature configuration structure
 *
 * This macro is used by the internal feature-specific configuration files
 * `sli_bt_*_config.c` to define an instance of the feature configuration
 * structure. The intended use is to assign the value directly. For example:
 *
 * @code
 * SLI_BT_DEFINE_FEATURE_CONFIG(bt, advertiser, sli_bt_advertiser_config_t) = {
 *   .max_advertisers = SL_BT_CONFIG_MAX_ADVERTISERS
 * };
 * @endcode
 *
 * @param[in] category_name Short category name string without quotes, for
 *   example bt, btmesh, or btctrl
 * @param[in] feature_name Short feature name string without quotes, for example
 *   advertiser or connection
 * @param[in] type The C language type of the configuration, for example
 *   sli_bt_advertiser_config_t
 */
#define SLI_BT_DEFINE_FEATURE_CONFIG(category_name, feature_name, type) \
  const type SLI_BT_FEATURE_CONFIG_NAME(category_name, feature_name)

/**
 * @brief Internal helper to implement @ref SLI_BT_USE_FEATURE
 *
 * This macro provides the extra layer of indirection that's needed to expand
 * `SLI_BT_INIT_MODE` before it's used as a parameter to @ref
 * SLI_BT_FEATURE_DEFINITION_NAME(). Files that define feature uses should
 * invoke @ref SLI_BT_USE_FEATURE() instead of this helper.
 */
#define SLI_BT_USE_FEATURE_MODE(category_name, feature_name, init_mode) \
  { &SLI_BT_FEATURE_DEFINITION_NAME(category_name, feature_name, init_mode), NULL }

/**
 * @brief Macro to declare the use of an optional feature that has no configuration
 *
 * This macro is used by stack-specific initialization code to declare the use
 * of a feature in the array of optional features.
 *
 * Files that invoke this macro must define `SLI_BT_INIT_MODE` to either
 * init_always or on_demand to decide the initialization mode of all features.
 * When the init_always mode is used, the de-init functions are omitted to
 * eliminate code that's not needed in the build.
 *
 * @param[in] category_name Short category name string without quotes, for
 *   example bt, btmesh, or btctrl
 * @param[in] feature_name Short feature name string without quotes, for example
 *   advertiser or connection
 */
#define SLI_BT_USE_FEATURE(category_name, feature_name) \
  SLI_BT_USE_FEATURE_MODE(category_name, feature_name, SLI_BT_INIT_MODE)

/**
 * @brief Internal helper to implement @ref SLI_BT_USE_FEATURE_WITH_CONFIG
 *
 * This macro provides the extra layer of indirection that's needed to expand
 * `SLI_BT_INIT_MODE` before it's used as a parameter to @ref
 * SLI_BT_FEATURE_DEFINITION_NAME(). Files that define feature uses should
 * invoke @ref SLI_BT_USE_FEATURE_WITH_CONFIG() instead of this helper.
 */
#define SLI_BT_USE_FEATURE_MODE_WITH_CONFIG(category_name, feature_name, init_mode, config_name) \
  { &SLI_BT_FEATURE_DEFINITION_NAME(category_name, feature_name, init_mode), &config_name }

/**
 * @brief Macro to declare the use of an optional feature with configuration
 *
 * This macro is used by stack-specific initialization code to declare the use
 * of a feature in the array of optional features.
 *
 * Files that invoke this macro must define `SLI_BT_INIT_MODE` to either
 * init_always or on_demand to decide the initialization mode of all features.
 * When the init_always mode is used, the de-init functions are omitted to
 * eliminate code that's not needed in the build.
 *
 * @param[in] category_name Short category name string without quotes, for
 *   example bt, btmesh, or btctrl
 * @param[in] feature_name Short feature name string without quotes, for example
 *   advertiser or connection
 * @param[in] config_name The name of the configuration structure. The caller
 *   should use @ref SLI_BT_FEATURE_CONFIG_NAME() to get the name of the
 *   configuration structure.
 */
#define SLI_BT_USE_FEATURE_WITH_CONFIG(category_name, feature_name, config_name) \
  SLI_BT_USE_FEATURE_MODE_WITH_CONFIG(category_name, feature_name, SLI_BT_INIT_MODE, config_name)

#endif // SLI_BT_CONFIG_DEFS_H
