/***************************************************************************//**
 * @brief Initialization APIs for btmesh stack
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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


#ifndef SL_BTMESH_STACK_INIT_H
#define SL_BTMESH_STACK_INIT_H

#include "sl_status.h"

/* Forward declarations of BGAPI class declaration structures */
struct sli_bgapi_class;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_node_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_node_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_prov_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_prov_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_proxy_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_proxy_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_vendor_model_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_vendor_model_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_health_client_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_health_client_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_health_server_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_health_server_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_generic_client_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_generic_client_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_generic_server_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_generic_server_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_test_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_test_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_lpn_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_lpn_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_friend_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_friend_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_proxy_client_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_proxy_client_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_proxy_server_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_proxy_server_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_config_client_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_config_client_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_sensor_server_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_sensor_server_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_sensor_setup_server_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_sensor_setup_server_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_sensor_client_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_sensor_client_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_lc_client_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_lc_client_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_lc_server_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_lc_server_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_lc_setup_server_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_lc_setup_server_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_scene_client_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_scene_client_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_scene_server_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_scene_server_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_scene_setup_server_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_scene_setup_server_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_scheduler_client_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_scheduler_client_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_scheduler_server_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_scheduler_server_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_time_server_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_time_server_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_time_client_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_time_client_full;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_migration_optimized;
extern const struct sli_bgapi_class sli_bgapi_class_btmesh_migration_full;


/* Macro to get a pointer to a BGAPI class declaration */
#ifdef SL_BT_API_FULL
#define SL_BTMESH_BGAPI_CLASS(class_name) &sli_bgapi_class_btmesh_ ## class_name ## _full
#else
#define SL_BTMESH_BGAPI_CLASS(class_name) &sli_bgapi_class_btmesh_ ## class_name ## _optimized
#endif

/**
 * Initialize the BGAPI classes for btmesh stack.
 *
 * This function is automatically called by code generated by the Universal
 * Configurator if the feature is included in the project. The application is
 * not expected to call this function directly.
 *
 * @param[in] classes NULL-terminated array of pointers to class declarations.
 *   The array must remain valid until the next system reboot.
 *
 * @return SL_STATUS_OK if the initialization was successful; Other error code
 *   indicates a failure.
 */
sl_status_t sl_btmesh_init_classes(const struct sli_bgapi_class * const * classes);

#endif