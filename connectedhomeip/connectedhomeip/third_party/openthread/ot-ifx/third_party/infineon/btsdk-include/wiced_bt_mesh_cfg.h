/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
/**************************************************************************//**
* \file <wiced_bt_mesh_cfg.h>
*
* Definitions for Bluetooth Mesh Configuration
*
******************************************************************************/

#ifndef __WICED_BT_MESH_CFG_H__
#define __WICED_BT_MESH_CFG_H__

#include "wiced_bt_mesh_model_defs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup  wiced_bt_mesh_core          Mesh Core Library API
 * @ingroup     wiced_bt_mesh
 *
 * Mesh Core library of the AIROC BTSDK provide a simple method for an application to integrate
 * Bluetooth Mesh functionality.
 *
 * @{
 */

/**
 * @anchor BT_MESH_CORE_FEATURE
 * @name Supported features bitmap
 * \details The following is the list of optional features that a mesh node can support.
 * @{
 */
#define WICED_BT_MESH_CORE_FEATURE_BIT_RELAY                0x0001   /**< Relay feature support: 0 = False, 1 = True */
#define WICED_BT_MESH_CORE_FEATURE_BIT_FRIEND               0x0002   /**< Friend feature support: 0 = False, 1 = True */
#define WICED_BT_MESH_CORE_FEATURE_BIT_LOW_POWER            0x0004   /**< Low Power feature support: 0 = False, 1 = True */
#define WICED_BT_MESH_CORE_FEATURE_BIT_GATT_PROXY_SERVER    0x0008   /**< GATT Proxy support: 0 = False, 1 = True */
#define WICED_BT_MESH_CORE_FEATURE_BIT_PB_GATT              0x0010   /**< PB_GATT support: 0 = False, 1 = True */
#define WICED_BT_MESH_CORE_FEATURE_BIT_NO_ADV_BEARER        0x0020   /**< For GATT client mode: advert scanning but no advert sending and receiving: 0 = False, 1 = True */
#define WICED_BT_MESH_CORE_FEATURE_BIT_DISABLE_NET_BEACON   0x0040   /**< Makes net beacon disabled by default: 0 = False, 1 = True  */
/** @} BT_MESH_CORE_FEATURE */

 /**
 * @anchor BT_MESH_CORE_OOB
 * @name OOB bitmap
 * \details The OOB field is used to help drive the provisioning process by indicating the availability of OOB data, such as a public key of the device.
 * @{
 */
#define WICED_BT_MESH_CORE_OOB_BIT_OTHER                0x0001  /**< Other */
#define WICED_BT_MESH_CORE_OOB_BIT_ELECTRONIC_URI       0x0002  /**< Electronic / URI */
#define WICED_BT_MESH_CORE_OOB_BIT_2D_CODE              0x0004  /**< 2D machine-readable code */
#define WICED_BT_MESH_CORE_OOB_BIT_BAR_CODE             0x0008  /**< Bar code */
#define WICED_BT_MESH_CORE_OOB_BIT_NFC                  0x0010  /**< Near Field Communication (NFC) */
#define WICED_BT_MESH_CORE_OOB_BIT_NUMBER               0x0020  /**< Number */
#define WICED_BT_MESH_CORE_OOB_BIT_STRING               0x0040  /**< String */
#define WICED_BT_MESH_CORE_OOB_BIT_CERTIFICATE          0x0080  /**< Certificate-based provisioning */
#define WICED_BT_MESH_CORE_OOB_BIT_RECORD               0x0100  /**< Provisioning records */
#define WICED_BT_MESH_CORE_OOB_BIT_RFU3                 0x0200  /**< Reserved for Future Use */
#define WICED_BT_MESH_CORE_OOB_BIT_RFU4                 0x0400  /**< Reserved for Future Use */
#define WICED_BT_MESH_CORE_OOB_BIT_ON_BOX               0x0800  /**< On box */
#define WICED_BT_MESH_CORE_OOB_BIT_INSIDE_BOX           0x1000  /**< Inside box */
#define WICED_BT_MESH_CORE_OOB_BIT_ON_PIECE_OF_PAPER    0x2000  /**< On piece of paper */
#define WICED_BT_MESH_CORE_OOB_BIT_INSIDE_MANUAL        0x4000  /**< Inside manual */
#define WICED_BT_MESH_CORE_OOB_BIT_ON_DEVICE            0x8000  /**< On device */
 /** @} BT_MESH_CORE_OOB */

#pragma pack(1)
#ifndef PACKED
#define PACKED
#endif

/**
 * Sensor descriptor structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t positive_tolerance;             /**< value representing the magnitude of a possible positive tolerance with sensor measurement*/
    uint16_t negative_tolerance;             /**< value representing the magnitude of a possible negative tolerance with sensor measurement*/
    uint8_t  sampling_function;              /**< averaging operation or type of sampling function applied to the measured value*/
    uint8_t  measurement_period;             /**< uint8 value n that represents the averaging time span, accumulation time, or measurement period in seconds*/
    uint8_t  update_interval;                /**< measurement reported by a sensor is internally refreshed at the frequency indicated in the Sensor Update Interval field*/
} wiced_bt_mesh_sensor_config_descriptor_t;

/**
 * Sensor setting configuration exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t setting_property_id;                 /**<  Setting ID identifying a setting within a sensor */
    uint8_t  access;                              /**<  Read / Write access rights for the setting */
    uint8_t  value_len;                           /**<  Length of the raw_value corresponding to property_id*/
    uint8_t  *val;                                /**<  Raw value for the setting */
} wiced_bt_mesh_sensor_config_setting_t;


/**
 * Sensor column data configuration exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint8_t  *raw_valuex;                       /**< Raw value representing the left corner of the column on the X axis*/
    uint8_t  *column_width;                     /**< Raw value representing the width of the column*/
    uint8_t  *raw_valuey;                       /**< Raw value representing the height of the column on the Y axis*/
} wiced_bt_mesh_sensor_config_column_data_t;


/**
 * Defines Configuration of Sensor Cadence in the structure  between the application and the Sensor Model
 */
typedef struct
{
    uint16_t      fast_cadence_period_divisor;                              /**< Divisor for the Publish Period */
    wiced_bool_t  trigger_type_percentage;                                  /**< If true, the delta down/up are percentage, otherwise native values */
    uint32_t      trigger_delta_down;                                       /**< Delta down value that triggers a status message */
    uint32_t      trigger_delta_up;                                         /**< Delta up value that triggers a status message */
    uint32_t      min_interval;                                             /**< Minimum interval between two consecutive Status messages */
    uint32_t      fast_cadence_low;                                         /**< Low value for the fast cadence range */
    uint32_t      fast_cadence_high;                                        /**< High value for the fast cadence range */
} wiced_bt_mesh_sensor_config_cadence_t;

 /**
  * Defines configuration data of the one model in the array models of the structure wiced_bt_mesh_core_config_element_t
  */
typedef PACKED struct
{
    uint16_t  company_id;               /**< Company ID of the model. It is 0 for SIG models */
    uint16_t  model_id;                 /**< Model ID */
    void      *p_message_handler;       /**< message handler for this model */
    void      *p_scene_store_handler;   /**< function to call to store the scene contents */
    void      *p_scene_recall_handler;  /**< function to call to recall the scene contents */
} wiced_bt_mesh_core_config_model_t;


/**
 * Defines configuration data of the one sensor in the array of structure wiced_bt_mesh_core_config_element_t
 */
typedef struct
{
    uint16_t                                            property_id;                                            /**< Property ID identifying a sensor */
    uint8_t                                             prop_value_len;                                         /**< Length of the raw_value corresponding to property_id*/
    wiced_bt_mesh_sensor_config_descriptor_t            descriptor;                                             /**< Sensor Descriptor state*/
    uint8_t                                             *data;                                                  /**< Sensor Data state*/
    wiced_bt_mesh_sensor_config_cadence_t               cadence;                                                /**< Sensor Cadence state*/
    uint8_t                                             num_series;                                             /**< Number of series data present in sensor*/
    wiced_bt_mesh_sensor_config_column_data_t           *series_columns;                                        /**< List of series column values of sensor*/
    uint8_t                                             num_settings;                                           /**< Number of settings present in sensor*/
    wiced_bt_mesh_sensor_config_setting_t               *settings;                                              /**< List of setting data present in sensor*/
} wiced_bt_mesh_core_config_sensor_t;

/**
 * Defines configuration data of the one property in the array properties of the structure wiced_bt_mesh_core_config_element_t
 */
typedef PACKED struct
{
   uint16_t  id;                /**< Property ID of the property. */
#define WICED_BT_MESH_PROPERTY_ID_READABLE      0x01    /**< Property is readable */
#define WICED_BT_MESH_PROPERTY_ID_WRITABLE      0x02    /**< Property is writable */
   uint8_t   type;              /**< Property type (client, admin, manufacturer or user */
   uint8_t   user_access;       /**< User Access state of the property */
   uint8_t   max_len;           /**< len for the property value */
   uint8_t  *value;             /**< pointer to the property value */
} wiced_bt_mesh_core_config_property_t;

/**
 * Defines configuration data of the one element in the array elements of the structure wiced_bt_mesh_core_config_t
 */
typedef PACKED struct
{
    uint16_t    location;                               /**< A location description as defined in the GATT Bluetooth Namespace Descriptors section of the Bluetooth SIG Assigned Numbers */
    uint32_t    default_transition_time;                /**< Default transition time for models of the element in milliseconds */
    uint8_t     onpowerup_state;                        /**< Default element behavior on power up */
    uint16_t    default_level;                          /**< Default value of the variable controlled on this element (for example power, lightness, temperature, hue...) */
    uint16_t    range_min;                              /**< Minimum value of the variable controlled on this element (for example power, lightness, temperature, hue...) */
    uint16_t    range_max;                              /**< Maximum value of the variable controlled on this element (for example power, lightness, temperature, hue...) */
    uint8_t     move_rollover;                          /**< If true when level gets to range_max during move operation, it switches to min, otherwise move stops. */
    uint8_t     properties_num;                         /**< Number of properties in the properties array */
    wiced_bt_mesh_core_config_property_t *properties;   /**< Array of properties of that element. Properties data is defined by structure wiced_bt_mesh_core_config_property_t */
    uint8_t     sensors_num;                            /**< Number of sensors in the sensor array */
    wiced_bt_mesh_core_config_sensor_t    *sensors;     /**< Array of sensors of that element */
    uint8_t     models_num;                             /**< Number of models in the array models */
    wiced_bt_mesh_core_config_model_t  *models;         /**< Array of models located in that element. Model data is defined by structure wiced_bt_mesh_core_config_model_t */
} wiced_bt_mesh_core_config_element_t;

/**
* Defines configuration data for Low Power Feature in the wiced_bt_mesh_core_config_t
*/
typedef PACKED struct
{
    uint8_t     rssi_factor;            /**< The contribution of the RSSI measured by the Friend node used in Friend Offer Delay calculations.Must be <= 3. RSSIFactor = 1 + (rssi_factor * 0.5) */
    uint8_t     receive_window_factor;  /**< The contribution of the supported Receive Window used in Friend Offer Delay calculations.Must be <= 3. ReceiveWindowFactor = 1 + (receive_window_factor * 0.5) */
    uint8_t     min_cache_size_log;     /**< The minimum number of messages that the Friend node can store in its Friend Cache.Must be >= 1 and <= 7. MinCacheSizeLog = 2 * * min_cache_size_log */
    uint8_t     receive_delay;          /**< Receive delay in 1 ms units to be requested by the Low Power node. It should be >= 0x0a */
    uint32_t    poll_timeout;           /**< Poll timeout in 100ms units to be requested by the Low Power node. It should be between 0x00000a and 0x34bbff */
    uint8_t     startup_unprovisioned_adv_to;   /**< Interval in seconds of the advertisments of unprovisioned beacon and service on startup. Value 0 means default 30 seconds. */
} wiced_bt_mesh_core_config_low_power_t;

/**
* Defines configuration data for Friend Feature in the wiced_bt_mesh_core_config_t
*/
typedef PACKED struct
{
    uint8_t     receive_window;         /**< Receive Window value in milliseconds supported by the Friend node. It must be > 0. */
    uint16_t    cache_buf_len;          /**< Length of the buffer for the cache */
    uint16_t    max_lpn_num;            /**< Max number of Low Power Nodes with established friendship. Must be > 0 if Friend feature is supported. See note below for calculating max_lpn_num */
} wiced_bt_mesh_core_config_friend_t;

/** To calculate max_lpn_num:
#define MSG_OFFSET 20 // offset in friend structure defined internally in mesh core library is 20 bytes
#define MSG_QUE_SIZE 33 // Each message in the queue takes 33 bytes
#define FRIEND_LPN_STATE 140 // sizeof friend LPN structure defined internally in mesh core library is 140 bytes
uint16_t max_lpn_msg_num; // number of messages user application needs in the queue
uint64_t app_available_memory; // available user application memory

max_lpn_num = (app_available_memory - MSG_OFFSET) / (FRIEND_LPN_STATE  + max_lpn_msg_num* MSG_QUE_SIZE);
// For example, if max_lpn_msg_num = 4, and app_available_memory = 6K, then max_lpn_num =  (6k - MSG_OFFSET) / (FRIEND_LPN_STATE + max_lpn_msg_num * MSG_QUE_SIZE) = 22
*/

/**
 * Defines mesh device configuration data
 */
typedef PACKED struct
{
    uint16_t                                company_id;         /**< Company identifier assigned by the Bluetooth SIG */
    uint16_t                                product_id;         /**< Vendor-assigned product identifier */
    uint16_t                                vendor_id;          /**< Vendor-assigned product version identifier */
    uint16_t                                replay_cache_size;  /**< Number of replay protection entries, i.e. maximum number of mesh devices that can send application messages to this device.*/
    uint16_t                                features;           /**< A bit field indicating the device features (see @ref BT_MESH_CORE_FEATURE "Supported features bitmap") */
    uint16_t                                oob;                /**< The OOB field (see @ref BT_MESH_CORE_OOB "OOB bitmap"). It indicates the availability of OOB data.*/
    const char                              *uri;               /**< Optional URI (can be NULL) for advertising along with the Unprovisioned Device beacon. Can start from "HTTP://" or "HTTPS://"*/
    wiced_bt_mesh_core_config_friend_t      friend_cfg;         /**< Friend feature configuration. It is ignored if Friend feature isn't supported (no FRIEND bit in the features).*/
    wiced_bt_mesh_core_config_low_power_t   low_power;          /**< Low Power configuration. It is ignored if Low Power feature isn't supported (no LOW_POWER bit in the features).*/
    wiced_bool_t                            gatt_client_only;   /**< If TRUE, the device only connects to mesh over GATT proxy. */
    uint8_t                                 elements_num;       /**< Number of elements in the following array elements */
    wiced_bt_mesh_core_config_element_t     *elements;          /**< Array of elements defined by tFND_ELEMENT with size elements_num */
} wiced_bt_mesh_core_config_t;

extern wiced_bt_mesh_core_config_t  mesh_config;

#pragma pack()

// Max size of the NVRAM chunk
#if defined(CYW20819A1)
#define WICED_BT_MESH_CORE_NVRAM_CHUNK_MAX_SIZE 492
#else
#define WICED_BT_MESH_CORE_NVRAM_CHUNK_MAX_SIZE 255
#endif

/* @} wiced_bt_mesh_core */

#ifdef __cplusplus
}
#endif

#endif /* __WICED_BT_MESH_CFG_H__ */
