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
 * \file <wiced_bt_mesh_models.h>
 *
 * Definitions for interface between Bluetooth Mesh Models and Mesh Core
 *
 */

#ifndef __WICED_BT_MESH_MODELS_H__
#define __WICED_BT_MESH_MODELS_H__

#include "wiced_bt_mesh_event.h"
#include "wiced_bt_mesh_core.h"
#include "wiced_bt_mesh_model_defs.h"

extern uint64_t wiced_bt_mesh_core_get_tick_count(void);

/**
 * NVRAM sections used by different models.  Each model uses IDs from START_ID to (START_ID + number of elements on the device)
 * The values should be initialized by the application.
 */
extern uint16_t wiced_bt_mesh_scene_register_nvram_id;
extern uint16_t wiced_bt_mesh_scene_nvram_id_start;
extern uint16_t wiced_bt_mesh_scene_nvram_id_end;
extern uint16_t wiced_bt_mesh_scheduler_nvram_id_start;
extern uint16_t wiced_bt_mesh_default_trans_time_nvram_id_start;
extern uint16_t wiced_bt_mesh_power_level_nvram_id_start;
extern uint16_t wiced_bt_mesh_power_onoff_nvram_id_start;
extern uint16_t wiced_bt_mesh_light_lightness_nvram_id_start;
extern uint16_t wiced_bt_mesh_light_xyl_nvram_id_start;
extern uint16_t wiced_bt_mesh_light_ctl_nvram_id_start;
extern uint16_t wiced_bt_mesh_light_hsl_nvram_id_start;
extern uint16_t wiced_bt_mesh_light_lc_run_nvram_id_start;
extern uint16_t wiced_bt_mesh_light_lc_nvram_id_start;

extern uint8_t  wiced_bt_mesh_scene_max_num;
extern uint8_t  wiced_bt_mesh_scheduler_events_max_num;

/**
 * @anchor BATTERY_EVENT
 * @name Definition for messages exchanged between an app and Generic Battery Model
 * @{ */
#define WICED_BT_MESH_BATTERY_GET                           0   /**< Get battery state */
#define WICED_BT_MESH_BATTERY_STATUS                        1   /**< Reply to get battery or battery changed notification */
/** @} BATTERY_EVENT */

/**
 * @anchor LOCATION_EVENT
 * @name Definition for messages exchanged between an app and Generic Location Model
 * @{ */
#define WICED_BT_MESH_LOCATION_GLOBAL_GET                   2   /**< Get global location */
#define WICED_BT_MESH_LOCATION_GLOBAL_SET                   3   /**< Set global location */
#define WICED_BT_MESH_LOCATION_GLOBAL_STATUS                4   /**< Reply to get/set global location or global location changed notification */
#define WICED_BT_MESH_LOCATION_LOCAL_GET                    5   /**< Get local location */
#define WICED_BT_MESH_LOCATION_LOCAL_SET                    6   /**< Set local location */
#define WICED_BT_MESH_LOCATION_LOCAL_STATUS                 7   /**< Reply to get/set local location or local location changed notification */
/** @} LOCATION_EVENT */

#define WICED_BT_MESH_VENDOR_DATA                           8   /**< Vendor data received from peer device */
/**
 * @anchor ONOFF_EVENT
 * @name Definition for messages exchanged between an app and Generic OnOff Model
 * @{ */
#define WICED_BT_MESH_ONOFF_SET                             9   /**< Set on off state */
#define WICED_BT_MESH_ONOFF_STATUS                          10  /**< Reply to get/set on off message or on off state changed notification */
/** @} ONOFF_EVENT */

/**
 * @anchor LEVEL_EVENT
 * @name Definition for messages exchanged between an app and Generic Level Model
 * @{ */
#define WICED_BT_MESH_LEVEL_SET                             13  /**< Set level state */
#define WICED_BT_MESH_LEVEL_STATUS                          16  /**< Reply to  level message or level state changed notification */
/** @} LEVEL_EVENT */

/**
 * @anchor DEFAULT_TRANSITION_TIME_EVENT
 * @name Definition for messages exchanged between an app and Generic Default Transition Time Service Model
 * @{ */
#define WICED_BT_MESH_DEFAULT_TRANSITION_TIME_STATUS        20  /**< Reply to get/set default transition time message */
/** @} DEFAULT_TRANSITION_TIME_EVENT */

/**
 * @anchor POWER_ONOFF_EVENT
 * @name Definition for messages exchanged between an app and Generic Power OnOff Model and Generic Power OnOff Setup Model
 * @{ */
#define WICED_BT_MESH_POWER_ONOFF_ONPOWERUP_STATUS           23  /**< Reply to get power on off message or power on off state changed notification */
/** @} POWER_ONOFF_EVENT */

/**
 * @anchor POWER_LEVEL_EVENT
 * @name Definition for messages exchanged between an app and Generic Power Level Model and Generic Power Level Setup Models
 * @{ */
#define WICED_BT_MESH_POWER_LEVEL_SET                        25  /**< Set power level state */
#define WICED_BT_MESH_POWER_LEVEL_STATUS                     28  /**< Reply to power level message or level state changed notification */
#define WICED_BT_MESH_POWER_LEVEL_LAST_STATUS                33  /**< Reply to Get Last Power Level message */
#define WICED_BT_MESH_POWER_LEVEL_DEFAULT_STATUS             34  /**< Reply to Get default Power :evel message or default changed notification */
#define WICED_BT_MESH_POWER_LEVEL_RANGE_STATUS               35  /**< Reply to Get range power level message or range changed notification */
/** @} POWER_LEVEL_EVENT */

/**
 * @anchor LIGHT_LIGHTNESS_EVENT
 * @name Definition for messages exchanged between an app and Light Lightness Model and Light Lightness Setup Models
 * @{ */
#define WICED_BT_MESH_LIGHT_LIGHTNESS_SET                   37  /**< Set Light Lightness Actual state */
#define WICED_BT_MESH_LIGHT_LIGHTNESS_CONTROL               38  /**< Set command received from the external source */
#define WICED_BT_MESH_LIGHT_LIGHTNESS_STATUS                40  /**< Reply to Light Lightness Get/Set message or Light Lightness Actual changed notification */
#define WICED_BT_MESH_LIGHT_LIGHTNESS_LINEAR_SET            42  /**< Set Light Lightness Linear state */
#define WICED_BT_MESH_LIGHT_LIGHTNESS_LINEAR_STATUS         43  /**< Reply to Light Lightness Get/Set message or Light Lightness Status changed notification */
#define WICED_BT_MESH_LIGHT_LIGHTNESS_SET_RANGE             44  /**< Set Light Lightness Actual min/max values for the element */
#define WICED_BT_MESH_LIGHT_LIGHTNESS_LAST_STATUS           45  /**< Reply to Get last Light Lightness Actual message */
#define WICED_BT_MESH_LIGHT_LIGHTNESS_DEFAULT_STATUS        46  /**< Reply to Get Light Lightness Actual default message or default level state changed notification */
#define WICED_BT_MESH_LIGHT_LIGHTNESS_RANGE_STATUS          47  /**< Reply to Get Light Lightness Actual range message or range changed notification */
/** @} LIGHT_LIGHTNESS_EVENT */

/**
 * @anchor LIGHT_CTL_EVENT
 * @name Definition for messages exchanged between an app and Light CTL Model and Light CTL Setup Models
 * @{ */
#define WICED_BT_MESH_LIGHT_CTL_SET                         51  /**< Set Light CTL state */
#define WICED_BT_MESH_LIGHT_CTL_STATUS                      54  /**< Reply to Light CTL Get/Set message or Light CTL changed notification */
#define WICED_BT_MESH_LIGHT_CTL_TEMPERATURE_STATUS          59  /**< Reply to Get last Light CTL message */
#define WICED_BT_MESH_LIGHT_CTL_SET_TEMPERATURE_RANGE       60  /**< Set Light CTL min/max temperature range values for the element */
#define WICED_BT_MESH_LIGHT_CTL_DEFAULT_STATUS              61  /**< Reply to Get Light CTL default message or default level state changed notification */
#define WICED_BT_MESH_LIGHT_CTL_SET_DEFAULT                 62  /**< Set Light CTL default temperature value for the element */
#define WICED_BT_MESH_LIGHT_CTL_TEMPERATURE_RANGE_STATUS    63  /**< Reply to Get Light CTL range message or range changed notification */
/** @} LIGHT_CTL_EVENT */

/**
 * @anchor LIGHT_HSL_EVENT
 * @name Definition for messages exchanged between an app and Light CTL Model and Light CTL Setup Models
 * @{ */
#define WICED_BT_MESH_LIGHT_HSL_SET                         65  /**< Set Light HSL state */
#define WICED_BT_MESH_LIGHT_HSL_STATUS                      66  /**< Reply to Light HSL Get/Set message or Light HSL changed notification */
#define WICED_BT_MESH_LIGHT_HSL_TARGET_STATUS               68  /**< Light HSL Target status */
#define WICED_BT_MESH_LIGHT_HSL_DEFAULT_STATUS              69  /**< Reply to Get Light HSL default message or default level state changed notification */
#define WICED_BT_MESH_LIGHT_HSL_RANGE_SET                   70  /**< Set Light HSL min/max hue/saturation range values for the element */
#define WICED_BT_MESH_LIGHT_HSL_RANGE_STATUS                71  /**< Reply to Get Light HSL range message or range changed notification */
#define WICED_BT_MESH_LIGHT_HSL_HUE_SET                     72  /**< Set Light HSL Temperature state */
#define WICED_BT_MESH_LIGHT_HSL_HUE_STATUS                  73  /**< Reply to Get/Set Light HSL Hue message */
#define WICED_BT_MESH_LIGHT_HSL_SATURATION_SET              74  /**< Set Light HSL Saturation state */
#define WICED_BT_MESH_LIGHT_HSL_SATURATION_STATUS           75  /**< Light HSL Saturation changed or reply message */
/** @} LIGHT_HSL_EVENT */

/**
* @anchor LIGHT_XYL_EVENT
* @name Definition for messages exchanged between an app and Light CTL Model and Light CTL Setup Models
* @{ */
#define WICED_BT_MESH_LIGHT_XYL_SET                         77  /**< Set Light xyL state */
#define WICED_BT_MESH_LIGHT_XYL_STATUS                      78  /**< Reply to Light xyL Get/Set message or Light xyL changed notification */
#define WICED_BT_MESH_LIGHT_XYL_TARGET_STATUS               80  /**< Light xyL Target values */
#define WICED_BT_MESH_LIGHT_XYL_DEFAULT_STATUS              81  /**< Reply to Get Light xyL default message or default level state changed notification */
#define WICED_BT_MESH_LIGHT_XYL_RANGE_SET                   82  /**< Set Light xyL min/max x/y range values for the element */
#define WICED_BT_MESH_LIGHT_XYL_RANGE_STATUS                83  /**< Reply to Get Light xyL range message or range changed notification */
#define WICED_BT_MESH_LIGHT_XYL_X_SET                       84  /**< Set Light xyL Temperature state */
#define WICED_BT_MESH_LIGHT_XYL_X_STATUS                    85  /**< Reply to Get last Light xyL message */
#define WICED_BT_MESH_LIGHT_XYL_Y_SET                       86  /**< Set Light xyL Temperature state */
#define WICED_BT_MESH_LIGHT_XYL_Y_STATUS                    87  /**< Reply to Get last Light xyL message */
/** @} LIGHT_XYL_EVENT */

/**
 * @anchor LIGHT_LC_EVENT
 * @name Definition for messages exchanged between an app and Light CTL Model and Light CTL Setup Models
 * @{ */
#define WICED_BT_MESH_LIGHT_LC_MODE_SET                     89  /**< Set Light LC Mode Set state */
#define WICED_BT_MESH_LIGHT_LC_MODE_STATUS                  90  /**< Reply to Light LC Mode Get/Set */
#define WICED_BT_MESH_LIGHT_LC_OCCUPANCY_MODE_SET           92  /**< Set Light LC Occupancy Mode Set state */
#define WICED_BT_MESH_LIGHT_LC_OCCUPANCY_MODE_STATUS        93  /**< Reply to Light LC Occupancy Mode Get/Set */
#define WICED_BT_MESH_LIGHT_LC_LIGHT_ONOFF_SET              95  /**< Set Light LC Light OnOff Set state */
#define WICED_BT_MESH_LIGHT_LC_LIGHT_ONOFF_STATUS           96  /**< Reply to Light LC Light OnOff Get/Set */
#define WICED_BT_MESH_LIGHT_LC_PROPERTY_GET                 97  /**< Get Light LC Property Get state */
#define WICED_BT_MESH_LIGHT_LC_PROPERTY_SET                 98  /**< Set Light LC Property Set state */
#define WICED_BT_MESH_LIGHT_LC_PROPERTY_STATUS              99  /**< Reply to Light LC Property Get/Set */
 /** @} LIGHT_LC_EVENT */

/**
 * @anchor PROPERTY_EVENT
 * @name Definition for messages exchanged between an app and Generic User Property Model
 * @{ */
#define WICED_BT_MESH_USER_PROPERTY_GET                     100 /**< Get Property state */
#define WICED_BT_MESH_USER_PROPERTY_SET                     101 /**< Set Property state */
#define WICED_BT_MESH_ADMIN_PROPERTIES_STATUS               102 /**< A list of the Admin Properties reported by the peer */
#define WICED_BT_MESH_ADMIN_PROPERTY_STATUS                 103 /**< Reply to get/set Property message or Property state changed notification */
#define WICED_BT_MESH_MANUF_PROPERTIES_STATUS               104 /**< A list of the Manufacturer Properties reported by the peer */
#define WICED_BT_MESH_MANUF_PROPERTY_STATUS                 105 /**< Reply to get/set Property message or Property state changed notification */
#define WICED_BT_MESH_USER_PROPERTIES_STATUS                106 /**< A list of the User Properties reported by the peer */
#define WICED_BT_MESH_USER_PROPERTY_STATUS                  107 /**< Reply to get/set Property message or Property state changed notification */
#define WICED_BT_MESH_CLIENT_PROPERTIES_STATUS              108 /**< A list of the Client Properties reported by the peer */
/** @} PROPERTY_EVENT */

/**
 * @anchor SENSOR_EVENT
 * @name Definition for messages exchanged between an app and Sensor Model
 * @{ */
#define WICED_BT_MESH_SENSOR_DESCRIPTOR_STATUS               111 /**< A list of the Descriptors reported by the peer */
#define WICED_BT_MESH_SENSOR_GET                             112 /**< Library asks application to report sensor data for a property or all properties of an element. */
#define WICED_BT_MESH_SENSOR_STATUS                          113 /**< A list of the sensor values reported by the peer */
#define WICED_BT_MESH_SENSOR_COLUMN_GET                      114 /**< Get Sensor Column values from the peer */
#define WICED_BT_MESH_SENSOR_COLUMN_STATUS                   115 /**< A list of the sensor Column values from the peer */
#define WICED_BT_MESH_SENSOR_SERIES_GET                      116 /**< Get sensor Series values from the peer */
#define WICED_BT_MESH_SENSOR_SERIES_STATUS                   117 /**< A list of the Series values reported by the peer */

/* Sensor Setup */
#define WICED_BT_MESH_SENSOR_CADENCE_GET                     120 /**< Get Sensor Cadence state */
#define WICED_BT_MESH_SENSOR_CADENCE_SET                     121 /**< Set Sensor Cadence state */
#define WICED_BT_MESH_SENSOR_CADENCE_SET_UNACKED             122 /**< Set Sensor Cadence state UnAcknowledged */
#define WICED_BT_MESH_SENSOR_CADENCE_STATUS                  123 /**< Sensor Cadence state values reported by peer */
#define WICED_BT_MESH_SENSOR_SETTINGS_GET                    124 /**< Get Sensor Settings list from the peer */
#define WICED_BT_MESH_SENSOR_SETTINGS_STATUS                 125 /**< A list of the sensor setting values reported by the peer */
#define WICED_BT_MESH_SENSOR_SETTING_GET                     126 /**< Get Sensor Setting state */
#define WICED_BT_MESH_SENSOR_SETTING_SET                     127 /**< Set Sensor Setting state */
#define WICED_BT_MESH_SENSOR_SETTING_SET_UNACKED             128 /**< Set Sensor Setting state UnAcknowledged */
#define WICED_BT_MESH_SENSOR_SETTING_STATUS                  129 /**< Sensor Setting state values reported by peer */
/** @} SENSOR_EVENT */

/**
 * @anchor SCENE_EVENT
 * @name Definition for messages exchanged between an app and Scene Model
 * @{ */
#define WICED_BT_MESH_SCENE_GET                              130 /**< Get the scene */
#define WICED_BT_MESH_SCENE_RECALL                           131 /**< Recall the scene */
#define WICED_BT_MESH_SCENE_STORE                            132 /**< Store the scene */
#define WICED_BT_MESH_SCENE_DELETE                           133 /**< Delete the scene */
#define WICED_BT_MESH_SCENE_STATUS                           134 /**< Scene Status reply */
#define WICED_BT_MESH_SCENE_REGISTER_STATUS                  135 /**< Scene Register Status reply */
/** @} SCENE_EVENT */

/**
 * @anchor SCHEDULER_EVENT
 * @name Definition for messages exchanged between an app and Scheduler Model
 * @{ */
#define WICED_BT_MESH_SCHEDULER_GET                          135 /**< Get the scheduler register */
#define WICED_BT_MESH_SCHEDULER_STATUS                       136 /**< Scheduler register status reported by the peer */
#define WICED_BT_MESH_SCHEDULER_ACTION_GET                   137 /**< Get the scheduler entry */
#define WICED_BT_MESH_SCHEDULER_ACTION_SET                   138 /**< Set the scheduler entry */
#define WICED_BT_MESH_SCHEDULER_ACTION_STATUS                139 /**< Scheduler entry status reply */
/** @} SCHEDULER_EVENT */

/*
 * @anchor TIME_EVENT
 * @name Definition for messages exchanged between an app and Time Model
 * @{ */
#define WICED_BT_MESH_TIME_GET                               140 /**< Get Time state */
#define WICED_BT_MESH_TIME_SET                               141 /**< Set Time state */
#define WICED_BT_MESH_TIME_STATUS                            142 /**< Get Time status */
#define WICED_BT_MESH_TIME_ROLE_GET                          143 /**< Get Time role state */
#define WICED_BT_MESH_TIME_ROLE_SET                          144 /**< Set Time role state */
#define WICED_BT_MESH_TIME_ROLE_STATUS                       145 /**< Time role state value reported by time setup model */
#define WICED_BT_MESH_TIME_ZONE_GET                          146 /**< Get Time zone state */
#define WICED_BT_MESH_TIME_ZONE_SET                          147 /**< Set Time zone state */
#define WICED_BT_MESH_TIME_ZONE_STATUS                       148 /**< Time zone status values reported by time model */
#define WICED_BT_MESH_TAI_UTC_DELTA_GET                      149 /**< Get Time delta state */
#define WICED_BT_MESH_TAI_UTC_DELTA_SET                      150 /**< Set Time delta state */
#define WICED_BT_MESH_TAI_UTC_DELTA_STATUS                   151 /**< Time Delta state values reported by time model */
#define WICED_BT_MESH_TIME_CHANGED                           152 /**< Time changed event state */
/** @} TIME_EVENT */

#define WICED_BT_MESH_TX_COMPLETE                           255  /**< Transmission completed, or timeout waiting for peer ack. */

/* TODO : Move this defines into sensor structures */
#define WICED_BT_MESH_MAX_PROP_VAL_LEN                      36 // Manufactrure name is Fixed String 36
#define WICED_BT_MESH_SENSOR_MAX_SERIES_COLUMN              5
#define WICED_BT_MESH_SENSOR_MAX_SETTINGS                   5
#define WICED_BT_MESH_MAX_SENSOR_PAYLOAD_LEN                380

/**
 * Sensor property id structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t property_id;                    /**<  Property ID identifying a sensor */
} wiced_bt_mesh_sensor_get_t;


/**
 * Sensor Descriptor structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t property_id;                    /**< property that describes the meaning and format of data reported by sensor*/
    uint16_t positive_tolerance;             /**< 12-bit value representing the magnitude of a possible positive tolerance with sensor measurement*/
    uint16_t negative_tolerance;             /**< 12-bit value representing the magnitude of a possible negative tolerance with sensor measurement*/
    uint8_t  sampling_function;              /**< averaging operation or type of sampling function applied to the measured value*/
    uint8_t  measurement_period;             /**< uint8 value n that represents the averaging time span, accumulation time, or measurement period in seconds*/
    uint8_t  update_interval;                /**< measurement reported by a sensor is internally refreshed at the frequency indicated in the Sensor Update Interval field*/
} wiced_bt_mesh_sensor_descriptor_data_t;

/**
 * Sensor Descriptor status structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint8_t                                 num_descriptors;      /**< Total number of descriptors present in status structure*/
    wiced_bt_mesh_sensor_descriptor_data_t  descriptor_list[1];   /**< Start of the descriptor data list */
} wiced_bt_mesh_sensor_descriptor_status_data_t;


/**
 * Sensor status data structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t property_id;                                   /**< Property identifying a sensor */
    uint8_t  prop_value_len;                                /**< Length of the raw_value corresponding to property_id*/
    uint8_t  raw_value[WICED_BT_MESH_MAX_PROP_VAL_LEN];     /**< Value of the Property */
} wiced_bt_mesh_sensor_status_data_t;

/**
 * Sensor Column get structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t property_id;                                   /**< Property identifying a sensor*/
    uint8_t  prop_value_len;                                /**< Length of the raw_value corresponding to property_id*/
    uint8_t  raw_valuex[WICED_BT_MESH_MAX_PROP_VAL_LEN];    /**< Raw value identifying a column*/
} wiced_bt_mesh_sensor_column_get_data_t;

/**
 * Sensor column data structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint8_t         raw_valuex[WICED_BT_MESH_MAX_PROP_VAL_LEN];    /**< Raw value representing the left corner of the column on the X axis*/
    uint8_t         column_width[WICED_BT_MESH_MAX_PROP_VAL_LEN];  /**< Raw value representing the width of the column*/
    uint8_t         raw_valuey[WICED_BT_MESH_MAX_PROP_VAL_LEN];    /**< Raw value representing the height of the column on the Y axis*/
} wiced_bt_mesh_sensor_column_data_t;

/**
 * Sensor Column status structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t                            property_id;        /**< Property identifying a sensor and the Y axis*/
    uint8_t                             prop_value_len;     /**< Length of the raw_value corresponding to property_id*/
    wiced_bool_t                        is_column_present;  /**< boolean value indicating if the column width is present in the message*/
    wiced_bt_mesh_sensor_column_data_t  column_data;        /**< Data field of column status */
} wiced_bt_mesh_sensor_column_status_data_t;


/**
 * Sensor Series get structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t property_id;                                   /**< Property identifying a sensor*/
    uint8_t  prop_value_len;                                /**< Length of the raw_value corresponding to property_id*/
    uint8_t  start_index;                                   /**< Start index of the series to be copied */
    uint8_t  end_index;                                     /**< End index of the series to be copied */
    uint8_t  raw_valuex1[WICED_BT_MESH_MAX_PROP_VAL_LEN];   /**< Raw value identifying a starting column*/
    uint8_t  raw_valuex2[WICED_BT_MESH_MAX_PROP_VAL_LEN];   /**< Raw value identifying an ending column*/
} wiced_bt_mesh_sensor_series_get_data_t;

/**
 * Sensor series status data structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t                             property_id;                                           /**< Property identifying a sensor*/
    uint8_t                              prop_value_len;                                        /**< Length of the raw_value corresponding to property_id*/
    uint8_t                              no_of_columns;                                         /**< Total number of columns*/
    wiced_bt_mesh_sensor_column_data_t   column_list[WICED_BT_MESH_SENSOR_MAX_SERIES_COLUMN];   /**< Series of column states*/
} wiced_bt_mesh_sensor_series_status_data_t;

/**
 * Sensor setting exchanged between the application and the Sensor Model
 */
typedef struct
{
#define WICED_BT_MESH_SENSOR_SETTING_READABLE                   0x01
#define WICED_BT_MESH_SENSOR_SETTING_READABLE_AND_WRITABLE      0x03
    uint16_t setting_property_id;                 /**<  Setting ID identifying a setting within a sensor */
    uint8_t  access;                              /**<  Read / Write access rights for the setting */
    uint8_t  value_len;                           /**< Length of the raw_value corresponding to property_id*/
    uint8_t  val[WICED_BT_MESH_MAX_PROP_VAL_LEN]; /**<  Raw value for the setting */
} wiced_bt_mesh_sensor_setting_t;

/**
 * Sensor Setting status data structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t                       property_id;      /**< Property ID identifying a sensor */
    wiced_bt_mesh_sensor_setting_t setting;          /**< Sensor setting data */
} wiced_bt_mesh_sensor_setting_status_data_t;

/**
 * Sensor Setting get structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t property_id;                       /**<  Property ID identifying a sensor */
    uint16_t setting_property_id;               /**<  Setting ID identifying a setting within a sensor */
} wiced_bt_mesh_sensor_setting_get_data_t;

/**
 * Sensor Setting set structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t property_id;                                      /**<  Property ID identifying a sensor */
    uint16_t setting_property_id;                              /**<  Setting ID identifying a setting within a sensor */
    uint8_t  prop_value_len;                                   /**< Length of the raw_value corresponding to property_id*/
    uint8_t  setting_raw_val[WICED_BT_MESH_MAX_PROP_VAL_LEN];  /**<  Raw value for the setting */
} wiced_bt_mesh_sensor_setting_set_data_t;


/**
 * Sensor Settings status structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t property_id;                                                 /**<  Property ID identifying a sensor */
    uint16_t num_setting_property_id;                                     /**<  Total Setting IDs identifying a setting within a sensor */
    uint16_t setting_property_id_list[WICED_BT_MESH_SENSOR_MAX_SETTINGS]; /**<  List of setting ID identifying a setting within a sensor */
} wiced_bt_mesh_sensor_settings_status_data_t;

/**
 * Sensor Cadence structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t      fast_cadence_period_divisor;          /**< Divisor for the Publish Period */
    wiced_bool_t  trigger_type;                         /**< Defines the unit and format of the Status Trigger Delta fields*/
    uint32_t      trigger_delta_down;                   /**< Delta down value that triggers a status message */
    uint32_t      trigger_delta_up;                     /**< Delta up value that triggers a status message */
    uint32_t      min_interval;                         /**< Minimum interval between two consecutive Status messages */
    uint32_t      fast_cadence_low;                     /**< Low value for the fast cadence range */
    uint32_t      fast_cadence_high;                    /**< High value for the fast cadence range */
} wiced_bt_mesh_sensor_cadence_t;


/**
 * Sensor Cadence set structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t                             property_id;    /**<  Property ID identifying a sensor */
    uint8_t                              prop_value_len; /**< Length of the raw_value corresponding to property_id*/
    wiced_bt_mesh_sensor_cadence_t       cadence_data;   /**<  Cadence data field of set message */
} wiced_bt_mesh_sensor_cadence_set_data_t;

/**
 * Sensor Cadence status structure exchanged between the application and the Sensor Model
 */
typedef struct
{
    uint16_t                             property_id;    /**<  Property ID identifying a sensor */
    wiced_bool_t                         is_data_present;/**<  Cadence data is received from peer */
    wiced_bt_mesh_sensor_cadence_t       cadence_data;   /**<  Optional cadence data field of status message */
} wiced_bt_mesh_sensor_cadence_status_data_t;


/**
 * Sensor status data structure exchanged between the sensor model and Application
 */
typedef union
{
    wiced_bt_mesh_sensor_descriptor_status_data_t      desc_status;      /**< Descriptor Status data received from Server*/
    wiced_bt_mesh_sensor_status_data_t                 sensor_status;    /**< Sensor Status data received from Server*/
    wiced_bt_mesh_sensor_column_status_data_t          column_status;    /**< Column Status data received from Server*/
    wiced_bt_mesh_sensor_series_status_data_t          series_status;    /**< Series Status data received from Server*/
    wiced_bt_mesh_sensor_setting_status_data_t         setting_status;   /**< Setting Status data received from Server*/
    wiced_bt_mesh_sensor_settings_status_data_t        settings_status;  /**< Settings Status data received from Server*/
    wiced_bt_mesh_sensor_cadence_status_data_t         cadence_status;   /**< Cadence Status data received from Server*/
} wiced_bt_mesh_sensor_status_message_data_t;

/**
 * Scene Data structure exchanged between the application and the Scene Models
 */
typedef struct
{
#define WICED_BT_MESH_SCENE_REQUEST_TYPE_STORE  0
#define WICED_BT_MESH_SCENE_REQUEST_TYPE_DELETE 1
#define WICED_BT_MESH_SCENE_REQUEST_TYPE_GET    2
    uint8_t  type;                                  /**< Scene Request Type */
    uint16_t scene_number;                          /**< The number of the scene to be recalled */
} wiced_bt_mesh_scene_request_t;

/**
 * Scene Recall Data structure exchanged between the application and the Scene Models
 */
typedef struct
{
    uint16_t scene_number;                          /**< The number of the scene to be recalled */
    uint32_t transition_time;                       /**< Transition time to the target level */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_scene_recall_t;

/**
 * Scene Status Data structure exchanged between the application Scene Models
 */
typedef struct
{
    uint8_t  status_code;
    uint16_t current_scene;                         /**< Scene Number of a current scene */
    uint16_t target_scene;                          /**< Scene Number of a target scene */
    uint32_t remaining_time;                        /**< Remaining time for transition */
} wiced_bt_mesh_scene_status_data_t;

#define WICED_BT_MESH_MODELS_MAX_SCENES                 100

/**
 * Scene Register Status Data structure exchanged between the application Scene Models
 */
typedef struct
{
    uint8_t  status_code;                           /**< Scene operation status code (see @ref WICED_BT_MESH_SCENE_STATUS_CODE */
    uint16_t current_scene;                         /**< Scene Number of a current scene */
    uint8_t  scene_num;                             /**< Number of scenese reported */
    uint32_t scene[WICED_BT_MESH_MODELS_MAX_SCENES];/**< Remaining time for transition */
} wiced_bt_mesh_scene_register_status_data_t;

/**
 * Time State structure exchanged between the Time Client and Time Server
 */
typedef struct
{
    uint64_t tai_seconds;                           /**< Current TAI time in seconds since the epoch */
    uint8_t  subsecond;                             /**< The sub-second time in units of 1/256s */
    uint8_t  uncertainty;                           /**< Estimated uncertainty in 10-millisecond steps */
    uint8_t  time_authority;                        /**< Element has a reliable source of TAI or not */
    uint16_t tai_utc_delta_current;                 /**< Current difference between TAI and UTC in seconds */
    uint8_t  time_zone_offset_current;              /**< Current zone offset in 15-minute increments */
} wiced_bt_mesh_time_state_msg_t;

/**
 * Time Zone State structure exchanged between the Application and Time model
 */
typedef struct
{
    uint8_t  time_zone_offset_new;                  /**< Upcoming local time zone offset */
    uint64_t tai_of_zone_change;                    /**< Absolute TAI time when the Time Zone Offset will change from Current to New */
} wiced_bt_mesh_time_zone_set_t;

/**
 * Time Zone Status structure exchanged between the Application and Time model
 */
typedef struct
{
    uint8_t  time_zone_offset_current;              /**< Current local time zone offset */
    uint8_t  time_zone_offset_new;                  /**< Upcoming local time zone offset */
    uint64_t tai_of_zone_change;                    /**< Absolute TAI time when the Time Zone Offset will change from Current to New */
} wiced_bt_mesh_time_zone_status_t;

/**
 * Time TAI UTC delta set structure exchanged between the Application and Time model
 */
typedef struct
{
    uint16_t tai_utc_delta_new;                     /**< Upcoming difference between TAI and UTC in seconds */
    uint64_t tai_of_delta_change;                   /**< Absolute TAI time when the TAI-UTC Delta will change from Current to New */
} wiced_bt_mesh_time_tai_utc_delta_set_t;

/**
 * Time TAI UTC delta status structure exchanged between the Application and Time model
 */
typedef struct
{
    uint16_t tai_utc_delta_current;                 /**< Current difference between TAI and UTC in seconds */
    uint16_t tai_utc_delta_new;                     /**< Upcoming difference between TAI and UTC in seconds */
    uint64_t tai_of_delta_change;                   /**< Absolute TAI time when the TAI-UTC Delta will change from Current to New */
} wiced_bt_mesh_time_tai_utc_delta_status_t;

/**
 * Time role structure exchanged between the Application and Time model
 */
typedef struct
{
   uint8_t role;                                    /**< The Time Role for the element */
} wiced_bt_mesh_time_role_msg_t;

/**
 * Scheduler Status Data structure exchanged between the application Scheduler Models
 */
typedef struct
{
    uint16_t actions;                               /**< Bit field indicating defined Actions in the Schedule Register */
} wiced_bt_mesh_scheduler_status_t;

/**
 * Scheduler Status Data structure exchanged between the application Scheduler Models
 */
typedef struct
{
    uint8_t action_number;                          /**< Index of the Schedule Register entry to get */
} wiced_bt_mesh_scheduler_action_get_t;

/**
 * @anchor SCHEDULER_ACTIONS
 * @name Scheduler Actions
 * @{ */
#define WICED_BT_MESH_SCHEDULER_ACTION_TURN_OFF     0       /**< Scheduler action Turn Off */
#define WICED_BT_MESH_SCHEDULER_ACTION_TURN_ON      1       /**< Scheduler action Turn On */
#define WICED_BT_MESH_SCHEDULER_ACTION_SCENE_RECALL 2       /**< Scheduler action Scene Recall */
#define WICED_BT_MESH_SCHEDULER_ACTION_NONE         0x0f    /**< Scheduler action None */
 /* @} */

#define WICED_BT_MESH_SCHEDULER_EVERY_YEAR          0x00    /**< Scheduled event should happen every year */
#define WICED_BT_MESH_SCHEDULER_EVERY_DAY           0x00    /**< Scheduled event should happen every day */

 /**
 * @anchor SCHEDULER_HOUR
 * @name Scheduler Hours
 * @{ */
#define WICED_BT_MESH_SCHEDULER_EVERY_HOUR          0x18    /**< Scheduled event should happen every hour */
#define WICED_BT_MESH_SCHEDULER_RANDOM_HOUR         0x19    /**< Scheduled event should happen on a random hour */
 /* @} */

/**
 * @anchor SCHEDULER_MINUTE
 * @name Scheduler Minutes
 * @{ */
#define WICED_BT_MESH_SCHEDULER_EVERY_MINUTE        0x3C    /**< Scheduled event should happen every minute */
#define WICED_BT_MESH_SCHEDULER_EVERY_15_MINUTES    0x3D    /**< Scheduled event should happen every 15 minutes */
#define WICED_BT_MESH_SCHEDULER_EVERY_20_MINUTES    0x3E    /**< Scheduled event should happen every 20 minutes */
#define WICED_BT_MESH_SCHEDULER_RANDOM_MINUTE       0x3F    /**< Scheduled event should happen on a random minute */
/* @} */

/**
 * @anchor SCHEDULER_SECONDS
 * @name Scheduler Seconds
 * @{ */
#define WICED_BT_MESH_SCHEDULER_EVERY_SECOND        0x3C    /**< Scheduled event should happen every second */
#define WICED_BT_MESH_SCHEDULER_EVERY_15_SECONDS    0x3D    /**< Scheduled event should happen every 15 seconds */
#define WICED_BT_MESH_SCHEDULER_EVERY_20_SECONDS    0x3E    /**< Scheduled event should happen every 20 seconds */
#define WICED_BT_MESH_SCHEDULER_RANDOM_SECOND       0x3F    /**< Scheduled event should happen on a random second */
/* @} */

/**
 * Scheduler Actions Data structure exchanged between the application Scheduler Models
 */
typedef struct
{
    uint8_t action_number;                                  /**< zero based entry number */
    uint8_t year;                                           /**< scheduled year for the action, or 0 if action needs to happen every year */
    uint16_t month;                                         /**< Bit field of the months for the action */
    uint8_t  day;                                           /**< Scheduled day of the month, or 0 to repeat every day */
    uint8_t  hour;                                          /**< Scheduled hour for the action, values 0-23 indicate the hour, or use one of the (@ref SCHEDULER_HOUR "special values") */
    uint8_t  minute;                                        /**< Scheduled hour for the action, values 0-59 indicate the minute, or use one of the (@ref SCHEDULER_MINUTE "special values")  */
    uint8_t  second;                                        /**< Scheduled second for the action, values 0-59 indicate the second, or use one of the (@ref SCHEDULER_SECONDS "special values")  */
    uint8_t  day_of_week;                                   /**< Bit field of the days of week when the action should happen */
    uint8_t  action;                                        /**< Action to be executed (see @ref SCHEDULER_ACTIONS "possible scheduler actions") */
    uint32_t transition_time;                               /**< Transition time to turn on/off or to transition to a scene */
    uint16_t scene_number;                                  /**< Scene number to transition to */
} wiced_bt_mesh_scheduler_action_data_t;

/**
 * Firmware Update Start Data structure exchanged between the application and Firmware Update Models
 */
#define WICED_BT_MESH_BLOB_TRANSFER_BLOB_ID_LEN         8

#ifdef __cplusplus
extern "C"
{
#endif

#define WICED_BT_MESH_MODELS_MAX_UNIQUE_MODELS_ON_NODE    20

/**
 * @ingroup     wiced_bt_mesh
 * @{
 * @page wiced_bt_mesh_application_structure Mesh Application Structure
 * Each AIROC Mesh Application shall contain a @ref wiced_bt_mesh_core_config_t "device configuration" and
 * application callbacks. The device configuration provides to the mesh stack the information about
 * the device which is supported. For example it can be a color bulb, or a dimmer, or a dimmable light
 * with additional vendor specific functionality.  The callbacks are used by the mesh stack to notify
 * the application when certain actions shall be performed. For example, the application for the color
 * bulb would probably be interested to process notification when the brightness shall be set to 50%,
 * or when the color shall be set to red.
 * @}
 */

/**
 * @addtogroup  wiced_bt_mesh_models        Mesh Models Library API
 * @ingroup     wiced_bt_mesh
 *
 * Mesh Models library of the AIROC BTSDK provide a simple method for an application to use various Mesh models
 * defined in the Bluetooth Mesh Model specification.
 *
 * @{
 */

/**
 * @anchor WICED_BT_MESH_MODEL_DEFINITIONS
 * @name Following macros can be used instantiate the mesh models
 * \details Following macros instantiate the mesh models on a given element.
 *
 * Based on the device definition the mesh core creates a device composition data
 * that can be read by the the provisioner.
 *
 * There certain rules that application shall follow when building device composition data.
 * 1. The first element of the device shall always contain WICED_BT_MESH_DEVICE which
 * instantiates models that shall be present on every device, including the Configuration
 * Server Model and the Health Server Model.
 *
 * 2. Each model consists of the model definition itself (company ID and model ID) and callback
 * function which is executed by the Mesh Core library on receiving a mesh message. Application can
 * use Mesh Library's callback.  In that case all validation of the messages, sending responses, and
 * other protocol requirements are executed by the model. Alternatively application can implement
 * callback itself. In that case, customer will be required to perform certification.
 *
 * 3. A model cannot be repeated twice in the same element.
 *
 * @{
 */

extern wiced_bool_t wiced_bt_mesh_remote_provisioning_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * Mesh Device includes Configuration Server and Health Server models. This need to be included
 * in the first element of the device.
 */
#define WICED_BT_MESH_DEVICE \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_CONFIG_SRV, NULL, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_HEALTH_SRV, NULL, NULL, NULL }

/**
 * Mesh Configuration Client Model
 */
 #define WICED_BT_MESH_MODEL_CONFIG_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_CONFIG_CLNT, wiced_bt_mesh_config_client_message_handler, NULL, NULL }

/**
 * Mesh Health Client Model
 */
#define WICED_BT_MESH_MODEL_HEALTH_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_HEALTH_CLNT, wiced_bt_mesh_model_health_client_message_handler, NULL, NULL }

/**
 * Mesh Remote Provisioning Model
 */
#define WICED_BT_MESH_MODEL_REMOTE_PROVISION_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_REMOTE_PROVISION_CLNT, wiced_bt_mesh_config_client_message_handler, NULL, NULL }

/**
 * Mesh Remote Provisioning Server Model.
 * The model is used by a Provisioner to provision new devices not in the immediate radio range, i.e. provision
 * over mesh.  This is an optional feature, but will provide additional flexibility for a mesh network.
 * It is recommended to include in the first element space permitted.
 */
#define WICED_BT_MESH_MODEL_REMOTE_PROVISION_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_REMOTE_PROVISION_SRV, wiced_bt_mesh_remote_provisioning_server_message_handler, NULL, NULL }

/**
 * Mesh Location Server Device.
 * The device provides global and/or local location.  The device can exist standalone and can be combined with other devices.
 */
#define WICED_BT_MESH_MODEL_LOCATION_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LOCATION_SRV, wiced_bt_mesh_model_location_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LOCATION_SETUP_SRV, wiced_bt_mesh_model_location_setup_server_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_USER_PROPERTY_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_USER_PROPERTY_SRV, wiced_bt_mesh_model_user_property_server_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_ADMIN_PROPERTY_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ADMIN_PROPERTY_SRV, wiced_bt_mesh_model_admin_property_server_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_MANUFACTURER_PROPERTY_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_MANUFACT_PROPERTY_SRV, wiced_bt_mesh_model_manufacturer_property_server_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_CLIENT_PROPERTY_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_CLIENT_PROPERTY_SRV, wiced_bt_mesh_model_client_property_server_message_handler, NULL, NULL }

/**
 * Mesh Light HSL/CTL Server Device.
 * The device implements all required models for the main element of a color bulb that supports
 * brightness, hue and saturation but also a color temperature controls.
 * Note that for the full color bulb implementation this device needs to be combined with
 * hue server (WICED_BT_MESH_MODEL_LIGHT_HSL_HUE_SERVER), saturation server
 * (WICED_BT_MESH_MODEL_LIGHT_HSL_SATURATION_SERVER) and color temperature
 * server (WICED_BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_SERVER) on the consecutive elements.
 * The time and scheduler models are optional and can be commented out to reduce memory footprint.
 */
#ifdef TIME_AND_SCHEDULER_SUPPORT
#define WICED_BT_MESH_MODEL_LIGHT_HSL_CTL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SRV, wiced_bt_mesh_model_scheduler_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SETUP_SRV, wiced_bt_mesh_model_scheduler_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SRV, wiced_bt_mesh_model_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SETUP_SRV, wiced_bt_mesh_model_time_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SRV, wiced_bt_mesh_model_light_lightness_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV,  wiced_bt_mesh_model_light_lightness_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SRV, wiced_bt_mesh_model_light_hsl_server_message_handler, wiced_bt_mesh_model_light_hsl_server_scene_store_handler, wiced_bt_mesh_model_light_hsl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SETUP_SRV, wiced_bt_mesh_model_light_hsl_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SRV, wiced_bt_mesh_model_light_ctl_server_message_handler, wiced_bt_mesh_model_light_ctl_server_scene_store_handler, wiced_bt_mesh_model_light_ctl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SETUP_SRV, wiced_bt_mesh_model_light_ctl_setup_server_message_handler, NULL, NULL  }
#else
#define WICED_BT_MESH_MODEL_LIGHT_HSL_CTL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SRV, wiced_bt_mesh_model_light_lightness_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV,  wiced_bt_mesh_model_light_lightness_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SRV, wiced_bt_mesh_model_light_hsl_server_message_handler, wiced_bt_mesh_model_light_hsl_server_scene_store_handler, wiced_bt_mesh_model_light_hsl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SETUP_SRV, wiced_bt_mesh_model_light_hsl_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SRV, wiced_bt_mesh_model_light_ctl_server_message_handler, wiced_bt_mesh_model_light_ctl_server_scene_store_handler, wiced_bt_mesh_model_light_ctl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SETUP_SRV, wiced_bt_mesh_model_light_ctl_setup_server_message_handler, NULL, NULL  }
#endif

/**
 * Mesh Light HSL/CTL/XYL Server Device.
 * The device implements all required models for the main element of a color bulb that supports
 * brightness, hue and saturation but also a color temperature controls and xyL controls.
 * Note that for the full color bulb implementation this device needs to be combined with
 * hue server (WICED_BT_MESH_MODEL_LIGHT_HSL_HUE_SERVER), saturation server
 * (WICED_BT_MESH_MODEL_LIGHT_HSL_SATURATION_SERVER) and color temperature
 * server (WICED_BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_SERVER) on the consecutive elements.
 * The time and scheduler models are optional and can be commented out to reduce memory footprint.
 */
#ifdef TIME_AND_SCHEDULER_SUPPORT
#define WICED_BT_MESH_MODEL_LIGHT_HSL_CTL_XYL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SRV, wiced_bt_mesh_model_scheduler_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SETUP_SRV, wiced_bt_mesh_model_scheduler_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SRV, wiced_bt_mesh_model_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SETUP_SRV, wiced_bt_mesh_model_time_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SRV, wiced_bt_mesh_model_light_lightness_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV,  wiced_bt_mesh_model_light_lightness_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SRV, wiced_bt_mesh_model_light_hsl_server_message_handler, wiced_bt_mesh_model_light_hsl_server_scene_store_handler, wiced_bt_mesh_model_light_hsl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SETUP_SRV, wiced_bt_mesh_model_light_hsl_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SRV, wiced_bt_mesh_model_light_ctl_server_message_handler, wiced_bt_mesh_model_light_ctl_server_scene_store_handler, wiced_bt_mesh_model_light_ctl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SETUP_SRV, wiced_bt_mesh_model_light_ctl_setup_server_message_handler, NULL, NULL  }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_XYL_SRV, wiced_bt_mesh_model_light_xyl_server_message_handler, wiced_bt_mesh_model_light_xyl_server_scene_store_handler, wiced_bt_mesh_model_light_xyl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_XYL_SETUP_SRV, wiced_bt_mesh_model_light_xyl_setup_server_message_handler, NULL, NULL  }
#else
#define WICED_BT_MESH_MODEL_LIGHT_HSL_CTL_XYL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SRV, wiced_bt_mesh_model_light_lightness_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV,  wiced_bt_mesh_model_light_lightness_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SRV, wiced_bt_mesh_model_light_hsl_server_message_handler, wiced_bt_mesh_model_light_hsl_server_scene_store_handler, wiced_bt_mesh_model_light_hsl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SETUP_SRV, wiced_bt_mesh_model_light_hsl_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SRV, wiced_bt_mesh_model_light_ctl_server_message_handler, wiced_bt_mesh_model_light_ctl_server_scene_store_handler, wiced_bt_mesh_model_light_ctl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SETUP_SRV, wiced_bt_mesh_model_light_ctl_setup_server_message_handler, NULL, NULL  }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_XYL_SRV, wiced_bt_mesh_model_light_xyl_server_message_handler, wiced_bt_mesh_model_light_xyl_server_scene_store_handler, wiced_bt_mesh_model_light_xyl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_XYL_SETUP_SRV, wiced_bt_mesh_model_light_xyl_setup_server_message_handler, NULL, NULL  }
#endif

/**
 * Mesh Light Controller Device.
 * This is an additional part of the light controller.  The main part will contain one or
 * more elements required to implement a dimmable or a colored bulb.
 * The time and scheduler models are optional and can be commented out to reduce memory footprint.
 */
#define WICED_BT_MESH_MODEL_LIGHT_LC_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LC_SRV, wiced_bt_mesh_model_light_lc_server_message_handler, wiced_bt_mesh_model_light_lc_server_scene_store_handler, wiced_bt_mesh_model_light_lc_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LC_SETUP_SRV, wiced_bt_mesh_model_light_lc_setup_server_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_SENSOR_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SENSOR_SRV, wiced_bt_mesh_model_sensor_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SENSOR_SETUP_SRV, wiced_bt_mesh_model_sensor_setup_server_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_TIME_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SRV, wiced_bt_mesh_model_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SETUP_SRV, wiced_bt_mesh_model_time_setup_server_message_handler, NULL, NULL }

/**
 * Mesh Level Client Device.
 * The device implements a Generic Level device.
 * The device can be configured to control any device that supports Generic Level Server model (WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV).
 * For example, @ref WICED_BT_MESH_MODEL_LIGHT_LIGHTNESS_SERVER, @ref WICED_BT_MESH_MODEL_LIGHT_CTL_SERVER, @ref WICED_BT_MESH_MODEL_LIGHT_HSL_SERVER,...
 */
#define WICED_BT_MESH_MODEL_LEVEL_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_CLNT, wiced_bt_mesh_model_level_client_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_DEFAULT_TRANSITION_TIME_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_CLNT, wiced_bt_mesh_model_default_transition_time_client_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_POWER_ONOFF_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_CLNT, wiced_bt_mesh_model_power_onoff_client_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_BATTERY_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_BATTERY_CLNT, wiced_bt_mesh_model_battery_client_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_LOCATION_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LOCATION_CLNT, wiced_bt_mesh_model_location_client_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_PROPERTY_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_PROPERTY_CLNT, wiced_bt_mesh_model_property_client_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_SENSOR_CLIENT \
    { MESH_COMPANY_ID_BT_SIG,  WICED_BT_MESH_CORE_MODEL_ID_SENSOR_CLNT, wiced_bt_mesh_model_sensor_client_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_SCENE_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_CLNT, wiced_bt_mesh_model_scene_client_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_SCHEDULER_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_CLNT, wiced_bt_mesh_model_scheduler_client_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_TIME_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_CLNT, wiced_bt_mesh_model_time_client_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_LIGHT_LC_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LC_CLNT, wiced_bt_mesh_model_light_lc_client_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_TIME_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_CLNT, wiced_bt_mesh_model_time_client_message_handler, NULL, NULL }


/** @} WICED_BT_MESH_MODEL_DEFINITIONS */

/**
 * @anchor BATTERY_PRESENCE_FLAGS
 * @name Battery Presence Flags
 * @{ */
#define WICED_BT_MESH_BATTERY_FLAG_NOT_PRESENT              0x00    /**< The battery is not present */
#define WICED_BT_MESH_BATTERY_FLAG_PRESENT_REMOVABLE        0x01    /**< The battery is present and is removable */
#define WICED_BT_MESH_BATTERY_FLAG_PRESENT_NON_REMOVABLE    0x02    /**< The battery is present and is non-removable */
#define WICED_BT_MESH_BATTERY_FLAG_PRESENCE_UNKNOWN         0x03    /**< The battery presence is unknown */
/* @} */

typedef uint8_t wiced_bt_mesh_battery_presence_t;                   /**< Generic Battery Flags Presence State (see @ref BATTERY_PRESENCE_FLAGS "Battery Presence Flags") */

/**
 * @anchor BATTERY_INDICATOR_FLAGS
 * @name Battery Indicator Flags
 * @{ */
#define WICED_BT_MESH_BATTERY_FLAG_LEVEL_CRITICALLY_LOW     0x00    /**< The battery charge is Critically Low Level. */
#define WICED_BT_MESH_BATTERY_FLAG_LEVEL_LOW                0x01    /**< The battery charge is Low Level. */
#define WICED_BT_MESH_BATTERY_FLAG_LEVEL_GOOD               0x02    /**< The battery charge is Good Level. */
#define WICED_BT_MESH_BATTERY_FLAG_LEVEL_UNKNOWN            0x03    /**< The battery charge is unknown. */
/* @} */

typedef uint8_t wiced_bt_mesh_battery_indicator_t;                  /**< Generic Battery Flags Indicator States (see @ref BATTERY_INDICATOR_FLAGS "Battery Indicator Flags") */

/**
 * @anchor BATTERY_CHARGING_FLAGS
 * @name Battery Charging State Flags
 * @{ */
#define WICED_BT_MESH_BATTERY_FLAG_NOT_CHARGABLE            0x00    /**< The battery is not chargeable. */
#define WICED_BT_MESH_BATTERY_FLAG_NOT_CHARGING             0x01    /**< The battery is chargeable and is not charging. */
#define WICED_BT_MESH_BATTERY_FLAG_CHARGING                 0x02    /**< The battery is chargeable and is charging. */
#define WICED_BT_MESH_BATTERY_FLAG_CHARGING_UNKNOWN         0x03    /**< The battery charging state is unknown. */
/* @} */

typedef uint8_t wiced_bt_mesh_battery_charging_t;                   /**< Generic Battery Flags Charging States (see @ref BATTERY_CHARGING_FLAGS "Battery Charging Flags") */

/**
 * @anchor BATTERY_SERVICABLITY_FLAGS
 * @name Battery Service Required State Flags
 * @{ */
#define WICED_BT_MESH_BATTERY_FLAG_SERVICE_NOT_REQUIRED     0x01    /**< The battery does not require service. */
#define WICED_BT_MESH_BATTERY_FLAG_SERVICE_REQUIRED         0x02    /**< The battery requires service. */
#define WICED_BT_MESH_BATTERY_FLAG_SERVICABILITY_UNKNOWN    0x03    /**< The battery serviceability is unknown. */
/* @} */

typedef uint8_t wiced_bt_mesh_battery_servicibility_t;              /**< Generic Battery Flags Serviceability States (see @ref BATTERY_SERVICABLITY_FLAGS "Battery Service Required Flags") */

#define WICED_BT_BATTERY_LEVEL_UNKNOWN                      0xff        /**< Current battery level unknown */
#define WICED_BT_BATTERY_TIME_TO_DISCHARGE_UNKNOWN          0xffffff    /**< Time to full discharge unknown */
#define WICED_BT_BATTERY_TIME_TO_CHARGE_UNKNOWN             0xffffff    /**< Time to full charge unknown */

#define CONVERT_TOLERANCE_PERCENTAGE_TO_MESH(A) ((4095 * A)/100)

/**
 * Mesh Battery Status Event used to report battery state to the client device
 */
typedef struct
{
    uint8_t                                 battery_level;          /**< The Generic Battery Level state is a value ranging from 0 percent through 100 percent */
    uint32_t                                time_to_discharge;      /**< The remaining time (in minutes) of the discharging process */
    uint32_t                                time_to_charge;         /**< The remaining time of the charging process is not known. */
    wiced_bt_mesh_battery_presence_t        presence;               /**< Generic Battery Flags Presence */
    wiced_bt_mesh_battery_indicator_t       level_inidicator;       /**< Generic Battery Flags Indicator */
    wiced_bt_mesh_battery_charging_t        charging;               /**< Generic Battery Flags Charging */
    wiced_bt_mesh_battery_servicibility_t   servicability;          /**< Generic Battery Flags Serviceability */
} mesh_battery_event_t;

/**
 * Global Location Data structure exchanged between the application Generic Location Model
 */
typedef struct
{
    uint32_t global_latitude;                       /**< Global latitude */
    uint32_t global_longitude;                      /**< Global longitude */
    uint16_t global_altitude;                       /**< Global altitude */
} wiced_bt_mesh_location_global_data_t;

/**
 * Local Location Data structure exchanged between the application Generic Location Model
 */
typedef struct
{
    uint16_t local_north;                           /**< North coordinate of the device using a local coordinate system */
    uint16_t local_east;                            /**< East coordinate of the device using a local coordinate system */
    uint16_t local_altitude;                        /**< Altitude of the device relative to the Generic Location Global Altitude */
    uint8_t  floor_number;                          /**< Floor number where the element is installed */
    uint8_t  is_mobile;                             /**< 0 if device is stationary, 1 if mobile */
    uint8_t  update_time;                           /**< time in seconds elapsed since the last update of the device's position (t = 2 ^ (update_time - 3)) */
    uint8_t  precision;                             /**< location precision in meters position (m = 2 ^ (precision - 3)) */
} wiced_bt_mesh_location_local_data_t;

#define WICED_BT_MESH_ONOFF_MAX_RETRANSMIT_TIME 6   /**< Max retransmit duration for on/off set command in seconds */

#define WICED_BT_MESH_TRANSITION_TIME_DEFAULT   0xFFFFFFFF  /**< Application or parent model should use default setting for transition time */

/**
 * OnOff Set Data structure exchanged between the application Generic OnOff Model
 */
typedef struct
{
    uint8_t  onoff;                                 /**< The target value of the Generic OnOff state */
    uint32_t transition_time;                       /**< Transition time to the target state */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_onoff_set_data_t;

/**
 * OnOff Status Data structure exchanged between the application and Generic OnOff Model
 */
typedef struct
{
    uint8_t  present_onoff;                         /**< The present value of the Generic OnOff state */
    uint8_t  target_onoff;                          /**< The target value of the Generic OnOff state */
    uint32_t remaining_time;                        /**< Remaining time for transition */
} wiced_bt_mesh_onoff_status_data_t;

#define WICED_BT_MESH_LEVEL_MAX_RETRANSMIT_TIME         6       /**< Max retransmit duration for level set command in seconds. This time is also indicates max duration of the delta command.  */
#define WICED_BT_MESH_LEVEL_MOVE_MAX_TRANSITION_TIME    0x3E    /**< Max transition time for Move command */

/**
 * Level Set Data structure exchanged between the application and the Generic Level Model
 */
typedef struct
{
    int16_t  level;                                 /**< The target value of the Generic Level state */
    uint32_t transition_time;                       /**< Transition time to the target level */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_level_set_level_t;

/**
 * Delta Set Data structure exchanged between the application and the Generic Level Model
 */
typedef struct
{
    int32_t     delta;                             /**< The Delta change of the Generic Level state */
    uint32_t    transition_time;                   /**< Transition time to the target level */
    uint16_t    delay;                             /**< Delay before starting the transition */
#define WICED_BT_MESH_LEVEL_DELTA_START             0   /**< Delta transaction start */
#define WICED_BT_MESH_LEVEL_DELTA_CONTINUATION      1   /**< Delta transaction continuation */
    uint8_t     continuation;                      /**< If false, a new delta transaction is started */
} wiced_bt_mesh_level_set_delta_t;

/**
 * Move Set Data structure exchanged between the application and the Generic Level Model
 */
typedef struct
{
    int16_t  delta;                                 /**< The Delta Level step to calculate Move speed for the Generic Level state. */
    uint32_t transition_time;                       /**< Transition time to the target level */
    uint16_t delay;                                 /**< Delay before starting the transition */
#define WICED_BT_MESH_LEVEL_MOVE_START              0   /**< Move transaction start */
#define WICED_BT_MESH_LEVEL_MOVE_STOP               1   /**< Move transaction stop */
    uint8_t  continuation;                      /**< If false, a new delta transaction is started */
} wiced_bt_mesh_level_set_move_t;

/**
 * Level Status Data structure exchanged between the application and Generic Level Model
 */
typedef struct
{
    int16_t  present_level;                         /**< The present value of the Generic Level state */
    int16_t  target_level;                          /**< The target value of the Generic Level state */
    uint32_t remaining_time;                        /**< Remaining time for transition */
} wiced_bt_mesh_level_status_data_t;

/**
 * Default Transition Time Data structure exchanged between an application and the Generic Default Transition Time Model
 */
typedef struct
{
    uint32_t time;                                  /**< Transition time in milliseconds */
} wiced_bt_mesh_default_transition_time_data_t;


/**
 * Power OnOff Data structure exchanged between an application and the Power OnOff Model
 */
typedef struct
{
    uint8_t on_power_up;                                /**< Value on power up */
} wiced_bt_mesh_power_onoff_data_t;

/**
 * Power Level Status Data structure exchanged between the application and Generic Power Level Model
 */
typedef struct
{
    uint16_t present_power;                         /**< The present value of the Generic Power Actual state */
    uint16_t target_power;                          /**< The target value of the Generic Power Actual state  */
    uint32_t remaining_time;                        /**< Remaining time for transition */
} wiced_bt_mesh_power_level_status_data_t;

/**
 * Power Level Last Status Data structure exchanged between the application and Generic Power Level Model
 */
typedef struct
{
    uint16_t power;                                 /**< The value of the Generic Power Last state. */
} wiced_bt_mesh_power_level_last_data_t;

/**
 * Power Level Default Status Data structure exchanged between the application and Generic Power Level Model
 */
typedef struct
{
    uint16_t power;                                 /**< The value of the Generic Power Default state. */
} wiced_bt_mesh_power_default_data_t;

/**
 * Power Level Set Range Data structure exchanged between the application and Generic Power Level Model
 */
typedef struct
{
    uint16_t power_min;                             /**< The value of the Generic Power Min field of the Generic Power Range state. */
    uint16_t power_max;                             /**< The value of the Generic Power Range Max field of the Generic Power Range state. */
} wiced_bt_mesh_power_level_range_set_data_t;

/**
 * Power Range Status Data structure exchanged between the application and Generic Power Level Model
 */
typedef struct
{
    uint8_t  status;                                /**< status of set range */
    uint16_t power_min;                             /**< The value of the Generic Power Min field of the Generic Power Range state. */
    uint16_t power_max;                             /**< The value of the Generic Power Range Max field of the Generic Power Range state. */
} wiced_bt_mesh_power_range_status_data_t;

/**
 * Level Set Data structure exchanged between the application and the Generic Power Level Model
 */
typedef struct
{
    uint16_t level;                                 /**< The target value of the Generic Power Actual state */
    uint32_t transition_time;                       /**< Transition time to the target level */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_power_level_set_level_t;

/**
 * Light Lightness Status Data structure exchanged between the application and Light Lightness Model
 */
typedef struct
{
    uint16_t present;                               /**< The present value of the Light Lightness Actual state */
    uint16_t target;                                /**< The target value of the Light Lightness Actual state  */
    uint32_t remaining_time;                        /**< Remaining time for transition */
} wiced_bt_mesh_light_lightness_status_data_t;

typedef struct
{
    uint16_t lightness_actual_present;              /**< The present value of the Light Lightness Actual state */
    uint16_t lightness_linear_present;              /**< The present value of the Light Lightness Linear state */
    uint32_t lux_level_present;                     /**< The present value of the Lux Level to be produce by the device, only valid if used in LC */
    uint16_t lightness_actual_target;               /**< The target value of the Light Lightness Actual state */
    uint16_t lightness_linear_target;               /**< The target value of the Light Lightness Linear state */
    uint32_t lux_level_target;                      /**< The target value of the Lux Level to be produce by the device, only valid if used in LC */
    uint32_t remaining_time;                        /**< Remaining time for transition */
} wiced_bt_mesh_light_lightness_status_t;

/**
 * Light Lightness Set Data structure exchanged between the application and the models Light Lightness Server library
 */
typedef struct
{
    uint16_t lightness_actual;                      /**< The target value of the Light Lightness Actual */
    uint16_t lightness_linear;                      /**< The target value of the Light Lightness Linear */
    uint32_t transition_time;                       /**< Transition time to the target level */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_light_lightness_set_t;

/**
 * Light Lightness Actual Set Data structure exchanged between the application and the models Light Lightness Client library
 */
typedef struct
{
    uint16_t lightness_actual;                      /**< The target value of the Light Lightness Actual */
    uint32_t transition_time;                       /**< Transition time to the target level */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_light_lightness_actual_set_t;

/**
 * Light Lightness Linear Set Data structure exchanged between the application and the models Light Lightness Client library
 */
typedef struct
{
    uint16_t lightness_linear;                      /**< The target value of the Light Lightness Linear */
    uint32_t transition_time;                       /**< Transition time to the target level */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_light_lightness_linear_set_t;

/**
 * Light Lightness Last Status Data structure exchanged between the application and Light Lightness Model
 */
typedef struct
{
    uint16_t last_level;                           /**< The value of the Light Lightness Last state. */
} wiced_bt_mesh_light_lightness_last_data_t;

/**
 * Lightness Default Status Data structure exchanged between the application and Light Lightness Model
 */
typedef struct
{
    uint16_t default_level;                        /**< The default lightness level for the Light Lightness Actual state. */
} wiced_bt_mesh_light_lightness_default_data_t;

/**
* Lightness Range Status Data structure exchanged between the application and Generic Power Level Model
*/
typedef struct
{
    uint16_t min_level;                             /**< The value of the Light Lightness Min field of the Light Lightness Range state. */
    uint16_t max_level;                             /**< The value of the Light Lightness Max field of the Light Lightness Range state. */
} wiced_bt_mesh_light_lightness_range_set_data_t;

/**
 * Lightness Range Status Data structure exchanged between the application and Light Lightness Model
 */
typedef struct
{
    uint8_t  status;                                /**< status of set range */
    uint16_t min_level;                             /**< The value of the Light Lightness Min field of the Light Lightness Range state. */
    uint16_t max_level;                             /**< The value of the Light Lightness Max field of the Light Lightness Range state. */
} wiced_bt_mesh_light_lightness_range_status_data_t;

/**
 * Property types defined in the Mesh Models specifications
 * wiced_bt_mesh_property_type enumeration
 */
#define WICED_BT_MESH_PROPERTY_TYPE_CLIENT          0           /**< Property type client */
#define WICED_BT_MESH_PROPERTY_TYPE_ADMIN           1           /**< Property type admin */
#define WICED_BT_MESH_PROPERTY_TYPE_MANUFACTURER    2           /**< Property type manufacturer */
#define WICED_BT_MESH_PROPERTY_TYPE_USER            3           /**< Property type user */

/**
 * Properties Get Data for Generic Property Models
 */
typedef struct
{
    uint8_t  type;                                  /**< One of the property types defined in the wiced_bt_mesh_property_type enumeration */
    uint16_t starting_id;                           /**< The property ID to be reported in the property status message */
} wiced_bt_mesh_properties_get_data_t;

/**
 * Property Get Data for Generic Property Models
 */
typedef struct
{
    uint8_t  type;                                  /**< One of the property types defined in the wiced_bt_mesh_property_type enumeration */
    uint16_t id;                                    /**< The property ID to be reported in the property status message */
} wiced_bt_mesh_property_get_data_t;

/**
 * Properties Status Data for Generic Property Models
 */
typedef struct
{
    uint8_t  type;                                  /**< One of the property types defined in the wiced_bt_mesh_property_type enumeration */
    uint8_t  properties_num;                        /**< Number of property IDs */
    uint16_t id[1];                                 /**< Start of the list of Property IDs. */
} wiced_bt_mesh_properties_status_data_t;

/**
 * Property Set Data structure exchanged between the application Generic User Property Models
 */
typedef struct
{
    uint8_t  type;                                  /**< One of the property types defined in the wiced_bt_mesh_property_type enumeration */
    uint16_t id;                                    /**< The Property ID received in the set message */
    uint8_t  access;                                /**< Access level to be set for the property.  This is a bit map when 0x01 indicates readable and 0x02 writable */
    uint16_t len;                                   /**< Size of the Property Value */
#define MESH_MAX_PROPERTY_VALUE_LEN 32  /* ToDo */
    uint8_t  value[MESH_MAX_PROPERTY_VALUE_LEN];    /**< Property value */
} wiced_bt_mesh_property_set_data_t;

/**
 * Property Status Data structure exchanged between the application Generic User Property Models
 */
typedef struct
{
    uint8_t  type;                                  /**< One of the property types defined in the wiced_bt_mesh_property_type enumeration */
    uint16_t id;                                    /**< The Property ID received in the set message */
    uint8_t  access;                                /**< Access level allowed for the property.  This is a bit map when 0x01 indicates readable and 0x02 writable */
    uint16_t len;                                   /**< Size of the Property Value */
    uint8_t  value[1];                              /**< First byte of property value */
} wiced_bt_mesh_property_status_data_t;

/**
 * Data structure representing the state of Light Color Temperature device
 */
typedef struct
{
    uint16_t lightness;                             /**< Value of the Light CTL Lightness state */
    uint16_t temperature;                           /**< Value of the Light CTL Temperature state */
    uint16_t delta_uv;                              /**< Value of the Light CTL Delta UV state*/
} wiced_bt_mesh_light_ctl_data_t;

/**
* Light CTL Status Data structure exchanged between the application and the models library
*/
typedef struct
{
    wiced_bt_mesh_light_ctl_data_t present;         /**< Present value of the Light CTL Lightness, Temperature and Delta UV */
    wiced_bt_mesh_light_ctl_data_t target;          /**< Target value of the Light CTL Lightness, Temperature and Delta UV */
    uint32_t remaining_time;                        /**< Remaining time for transition */
} wiced_bt_mesh_light_ctl_status_data_t;

/**
 * Light CTL Set Data structure exchanged between the application and the models library
 */
typedef struct
{
    wiced_bt_mesh_light_ctl_data_t target;          /**< Target value of the Light CTL state */
    uint32_t transition_time;                       /**< Transition time to the target level */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_light_ctl_set_t;

/**
 * Light Lightness Last Status Data structure exchanged between the application and Light Lightness Model
 */
typedef struct
{
    uint16_t target_temperature;                    /**< Target value of the Light CTL Temperature state */
    uint16_t target_delta_uv;                       /**< Target value of the Light CTL Delta UV state */
    uint32_t transition_time;                       /**< Transition time to the target level */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_light_ctl_temperature_set_t;

/**
 * Lightness Default Status Data structure exchanged between the application and Light Lightness Model
 */
typedef struct
{
    wiced_bt_mesh_light_ctl_data_t default_status;          /**< The default lightness, temperature, delta uv for the Light CTL state. */
} wiced_bt_mesh_light_ctl_default_data_t;

/**
 * Temperature Range Status Data structure exchanged between the application and Light CTL Temperature Model
 */
typedef struct
{
    uint8_t  status;                                /**< Status of the Set Range operation */
    uint16_t min_level;                             /**< The value of the Light Lightness Min field of the Light Lightness Range state. */
    uint16_t max_level;                             /**< The value of the Light Lightness Max field of the Light Lightness Range state. */
} wiced_bt_mesh_light_ctl_temperature_range_status_data_t;

/**
 * Temperature Range Set Data structure exchanged between the application and Light CTL Temperature Model
 */
typedef struct
{
    uint16_t min_level;                             /**< The value of the Light Temperature Min field of the Light CTL Temperature Range state. */
    uint16_t max_level;                             /**< The value of the Light Lightness Max field of the Light Lightness Range state. */
} wiced_bt_mesh_light_ctl_temperature_range_data_t;

/**
 * Light Lightness Set Data structure exchanged between the application and the models library
 */
typedef struct
{
    uint16_t level;                                 /**< The target value of the Light Lightness Actual state */
    uint32_t transition_time;                       /**< Transition time to the target level */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_light_delta_uv_set_level_t;


/**
 * Light Lightness Last Status Data structure exchanged between the application and Light Lightness Model
 */
typedef struct
{
    uint16_t last_level;                           /**< The value of the Light Lightness Last state. */
} wiced_bt_mesh_light_delta_uv_last_data_t;

/**
 * Lightness Default Status Data structure exchanged between the application and Light Lightness Model
 */
typedef struct
{
    uint16_t default_level;                        /**< The default lightness level for the Light Lightness Actual state. */
} wiced_bt_mesh_light_delta_uv_default_data_t;

/**
 * Lightness Range Status Data structure exchanged between the application and Generic Power Level Model
 */
typedef struct
{
    uint16_t min_level;                             /**< The value of the Light Lightness Min field of the Light Lightness Range state. */
    uint16_t max_level;                             /**< The value of the Light Lightness Max field of the Light Lightness Range state. */
} wiced_bt_mesh_light_delta_uv_range_data_t;

/**
 * Data structure representing the state of Light HSL device
 */
typedef struct
{
    uint16_t lightness;                             /**< Value of the Light HSL Lightness state */
    uint16_t hue;                                   /**< Value of the Light HSL Hue state */
    uint16_t saturation;                            /**< Value of the Light HSL Saturation UV state*/
} wiced_bt_mesh_light_hsl_data_t;

/**
* Light HSL Status Data structure exchanged between the application and the models library
*/
typedef struct
{
    wiced_bt_mesh_light_hsl_data_t present;         /**< Present value of HSL state */
    uint32_t remaining_time;                        /**< Remaining time for transition */
} wiced_bt_mesh_light_hsl_status_data_t;

/**
* Light HSL Target Status Data structure exchanged between the application and the models library
*/
typedef struct
{
    wiced_bt_mesh_light_hsl_data_t target;          /**< Target value of HSL state */
    uint32_t remaining_time;                        /**< Remaining time for transition */
} wiced_bt_mesh_light_hsl_target_status_data_t;

/**
 * Light HSL Set Data structure exchanged between the application and the models library
 */
typedef struct
{
    wiced_bt_mesh_light_hsl_data_t target;          /**< Target value of the Light HSL state */
    uint32_t transition_time;                       /**< Transition time to the target level */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_light_hsl_set_t;

/**
* Light HSL Hue Set Data structure exchanged between the application and the models library
*/
typedef struct
{
    uint16_t level;                                 /**< The target value of the Light HSL Hue state */
    uint32_t transition_time;                       /**< Transition time to the target level */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_light_hsl_hue_set_t;

/**
* Light HSL Saturation Set Data structure exchanged between the application and the models library
*/
typedef struct
{
    uint16_t level;                                 /**< The target value of the Light HSL Saturation state */
    uint32_t transition_time;                       /**< Transition time to the target level */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_light_hsl_saturation_set_t;

/**
* Light HSL Set Default Data structure exchanged between the application and the models library
*/
typedef struct
{
    wiced_bt_mesh_light_hsl_data_t default_status;   /**< Target value of the Light HSL state */
} wiced_bt_mesh_light_hsl_default_data_t;

/**
 * Light HSL Hue status Data structure exchanged between the application and Light HSL Model
 */
typedef struct
{
    uint16_t present_hue;                           /**< The present value of the Light HSL Hue state */
    uint16_t target_hue;                            /**< The target value of the Light HSL Hue state */
    uint32_t remaining_time;                        /**< Remaining time for transition */
} wiced_bt_mesh_light_hsl_hue_status_data_t;

/**
 * Light HSL Saturation Status Data structure exchanged between the application and Light HSL Model
 */
typedef struct
{
    uint16_t present_saturation;                    /**< The present value of the Light HSL Saturation state */
    uint16_t target_saturation;                     /**< The target value of the Light HSL Saturation state */
    uint32_t remaining_time;                        /**< Remaining time for transition */
} wiced_bt_mesh_light_hsl_saturation_status_data_t;

/**
 * Light HSL Range Set Data structure exchanged between the application and Light HSL Level Model
 */
typedef struct
{
    uint16_t hue_min;                               /**< The value of the Hue Min  */
    uint16_t hue_max;                               /**< The value of the Hue Max  */
    uint16_t saturation_min;                        /**< The value of the Saturation Min  */
    uint16_t saturation_max;                        /**< The value of the Saturation Max  */
} wiced_bt_mesh_light_hsl_range_set_data_t;

/**
 * Light HSL Range Set Data structure exchanged between the application and Light HSL Level Model
 */
typedef struct
{
    uint8_t  status;                                /**< Status Code for the requesting message */
    uint16_t hue_min;                               /**< The value of the Hue Min  */
    uint16_t hue_max;                               /**< The value of the Hue Max  */
    uint16_t saturation_min;                        /**< The value of the Saturation Min  */
    uint16_t saturation_max;                        /**< The value of the Saturation Max  */
} wiced_bt_mesh_light_hsl_range_status_data_t;

/**
 * Light HSL Range Default Status structure exchanged between the application and Light HSL Level Model
 */
typedef struct
{
    wiced_bt_mesh_light_hsl_data_t default_status;  /**< HSL status data */
} wiced_bt_mesh_light_hsl_default_status_data_t;

/**
 * Data structure reppresenting the state of the Light xyL device
 */
typedef struct
{
    uint16_t lightness;                             /**< Value of the Light xyL Lightness */
    uint16_t x;                                     /**< x coordinate on the CIE1931 color space chart of a color light emitted by an element */
    uint16_t y;                                     /**< y coordinate on the CIE1931 color space chart of a color light emitted by an element */
} wiced_bt_mesh_light_xyl_data_t;

/**
 * Light xyL x_y_default and range settings
 */
typedef struct
{
    uint16_t x_default;                             /**< default value of x */
    uint16_t x_min;                                 /**< minimum value of x */
    uint16_t x_max;                                 /**< maximum value of x */
    uint16_t y_default;                             /**< default value of x */
    uint16_t y_min;                                 /**< minimum value of y */
    uint16_t y_max;                                 /**< maximum value of y */
} wiced_bt_mesh_light_xyl_xy_settings_t;


/**
* Light xyL Status Data structure exchanged between the application and the models library
*/
typedef struct
{
    wiced_bt_mesh_light_xyl_data_t present;         /**< Present value of xyL state */
    uint32_t remaining_time;                        /**< Remaining time for transition */
} wiced_bt_mesh_light_xyl_status_data_t;

/**
* Light xyL Target Value Data structure exchanged between the application and the models library
*/
typedef struct
{
    wiced_bt_mesh_light_xyl_data_t target;          /**< Target value of xyL state */
    uint32_t remaining_time;                        /**< Remaining time for transition */
} wiced_bt_mesh_light_xyl_target_status_data_t;

/**
* Light xyL Set Data structure exchanged between the application and the models library
*/
typedef struct
{
    wiced_bt_mesh_light_xyl_data_t target;          /**< Target value of the Light xyL state */
    uint32_t transition_time;                       /**< Transition time to the target levels */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_light_xyl_set_t;

/**
* Light xyL Set Default Data structure exchanged between the application and the models library
*/
typedef struct
{
    wiced_bt_mesh_light_xyl_data_t default_status;   /**< Default value of the Light xyL state */
} wiced_bt_mesh_light_xyl_default_data_t;

/**
* Light xyL Range Set Data structure exchanged between the application and Light xyL Level Model
*/
typedef struct
{
    uint16_t x_min;                                 /**< Light xyL x Range Min field of the Light xyL x Range state  */
    uint16_t x_max;                                 /**< Light xyL x Range Max field of the Light xyL y Range state  */
    uint16_t y_min;                                 /**< Light xyL y Range Min field of the Light xyL y Range state  */
    uint16_t y_max;                                 /**< Light xyL y Range Max field of the Light xyL y Range state  */
} wiced_bt_mesh_light_xyl_range_set_data_t;

/**
* Light xyL Range Set Data structure exchanged between the application and Light xyL Level Model
*/
typedef struct
{
    uint8_t  status;                                /**< Status Code for the requesting message */
    uint16_t x_min;                                 /**< Light xyL x Range Min field of the Light xyL x Range state  */
    uint16_t x_max;                                 /**< Light xyL x Range Max field of the Light xyL y Range state  */
    uint16_t y_min;                                 /**< Light xyL y Range Min field of the Light xyL y Range state  */
    uint16_t y_max;                                 /**< Light xyL y Range Max field of the Light xyL y Range state  */
} wiced_bt_mesh_light_xyl_range_status_data_t;

/**
* Light xyL Range Default Status structure exchanged between the application and Light xyL Level Model
*/
typedef struct
{
    wiced_bt_mesh_light_xyl_data_t default_status;  /**< Light xyL x Default value */
} wiced_bt_mesh_light_xyl_default_status_data_t;

/**
 * Light LC Mode Set data structure exchanged between the application and Light LC library
 */
typedef struct
{
    uint8_t mode;                                   /**< The target value of the Light LC Mode state */
} wiced_bt_mesh_light_lc_mode_set_data_t;

/**
 * Light LC Occupancy Mode Set data structure exchanged between the application and Light LC library
 */
typedef struct
{
    uint8_t mode;                                   /**< The target value of the Light LC Mode state */
} wiced_bt_mesh_light_lc_occupancy_mode_set_data_t;

/**
 * Light LC Light OnOff Set data structure exchanged between the application and Light LC library
 */
typedef struct
{
    uint8_t light_onoff;                            /**< The target value of the Light LC Light OnOff state */
    uint32_t transition_time;                       /**< Transition time to the target state */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_light_lc_light_onoff_set_data_t;

/**
 * Light LC Light OnOff Status data structure exchanged between the application and Light LC library
 */
typedef struct
{
    uint8_t present_onoff;                          /**< Present value of the Light LC Light OnOff state */
    uint8_t target_onoff;                           /**< Target value of the Light LC Light OnOff state */
    uint32_t remaining_time;                        /**< Remaining time */
} wiced_bt_mesh_light_lc_light_onoff_status_data_t;

/**
 * Light LC Light OnOff Set data structure exchanged between the application and Light LC library
 */
typedef struct
{
    uint16_t linear_out;                            /**< The target value of the Light LC Linear Output state */
    uint32_t transition_time;                       /**< Transition time to the target state */
    uint16_t delay;                                 /**< Delay before starting the transition */
} wiced_bt_mesh_light_lc_linear_out_set_data_t;

/**
 * Light LC Property Get Data structure exchanged between the application and Light LC library
 */
typedef struct
{
    uint16_t id;                                   /**< The Property ID identifying a Light LC Property */
} wiced_bt_mesh_light_lc_property_get_data_t;

/**
 * Light LC Property Set Data structure exchanged between the application and Light LC library
 */
typedef struct
{
    uint16_t id;                                   /**< The Property ID identifying a Light LC Property */
    uint16_t len;                                  /**< Size of the Property Value */
#define MESH_MAX_LIGHT_LC_PROPERTY_VALUE_LEN    4  /**< Light LC maximum property value len */
    uint8_t  value[MESH_MAX_LIGHT_LC_PROPERTY_VALUE_LEN];   /**< Property value */
} wiced_bt_mesh_light_lc_property_set_data_t;

/**
 * Light LC Property Status Data structure exchanged between the application and Light LC library
 */
typedef struct
{
    uint16_t id;                                   /**< The Property ID received in the set message */
    uint16_t len;                                  /**< Size of the Property Value */
    uint8_t  value[MESH_MAX_LIGHT_LC_PROPERTY_VALUE_LEN];   /**< Property value */
} wiced_bt_mesh_light_lc_property_status_data_t;

/**
 * \brief Scene store message handler
 * \details Each model may define a message handler if it needs to store data per scene in the NVRAM.  When Scene Server
 * receives Scene Store message it calls each model's handler.
 *
 * @param       element_idx Element index for which scene information is being stored
 * @param       p_buffer Pointer to a buffer where data should be stored
 * @param       buf_size Maximum amount of data a model can store
 *
 * @return      Number of bytes the model wrote into the provided buffer
 */
typedef uint16_t (*wiced_model_scene_store_handler_t)(uint8_t element_idx, uint8_t *p_buffer, uint16_t buf_size);

/**
 * \brief Scene retrieve message handler
 * \details Each model may define a message handler if it stores data per scene in the NVRAM.  When Scene Server
 * receives Scene Recall message it calls each model's handler.
 *
 * @param       element_idx Element index for which scene information is being recalled
 * @param       p_buffer Pointer to a buffer where model's data is located
 * @param       buf_size Maximum amount of data a model can get from the buffer
 *
 * @return      Number of bytes the model read from the provided buffer
 */
typedef uint16_t (*wiced_model_scene_recall_handler_t)(uint8_t element_idx, uint8_t *p_buffer, uint16_t buf_size, uint32_t transition_time, uint32_t delay);

/**
 * @addtogroup  wiced_bt_mesh_battery_server        Mesh Battery Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Battery Server module provides a way for a client to retrieve the Battery status of this device. The Server Model
 * can also be configured by a Provisioner to allow sending the Status message to a specific node or group of nodes.
 *
 * On startup the application should call wiced_bt_mesh_model_battery_server_init function providing the callback that
 * will be called when a request for the battery status is received. The application should call the wiced_bt_mesh_battery_server_send_status
 * function in response to a get request, or when application logic requires battery status to be sent out.  In the latter
 * case the message will be sent out only if device is configured remotely with the destination address where the battery status
 * should be sent to and the application key to be used to secure the message.
 *
 * @{
 */

/**
 * Mesh Battery Server Device.
 * The device can exist standalone, but usually is combined with other battery powered devices.
 */
#define WICED_BT_MESH_MODEL_BATTERY_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_BATTERY_SRV, wiced_bt_mesh_model_battery_server_message_handler, NULL, NULL }

/**
 * \brief Battery Server callback
 * \details The Battery Server callback is called by the Mesh Models library on receiving a get message from the peer
 *
 * @param       event The event that the application should process (see @ref BATTERY_EVENT "Mesh Battery Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_battery_server_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event);

/**
 * \brief Battery Server Module initialization
 * \details Application should call this function during startup to register a callback which will be executed when received message requires application processing.
 *
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      WICED_TRUE if initialization was successful
 */
void wiced_bt_mesh_model_battery_server_init(wiced_bt_mesh_battery_server_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Battery Server Message Handler
 * \details Application can call this models library function to process a message received from the Battery Client.
 * The function parses the message and if appropriate calls the application back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_battery_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief The application can call this function to send status message to the Battery Client. The mesh event should
 * contain information if this is a reply or an unsolicited message.
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_data Pointer to the Battery data
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_battery_server_send_status(wiced_bt_mesh_event_t *p_event, mesh_battery_event_t *p_data);

/* @} wiced_bt_mesh_battery_server */

/**
 * @addtogroup  wiced_bt_mesh_battery_client        Mesh Battery Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Battery Client module provides a way for an application to retrieve the battery status of a mesh node. The module
 * can also process unsolicited status messages from mesh nodes.
 *
 * On startup the application should call wiced_bt_mesh_model_battery_client_init function providing the callback that
 * will be called when battery status message is received. The application can call the wiced_bt_mesh_battery_client_send_get
 * function to retrieve the battery status of a mesh device.
 *
 * @{
 */

/**
 * \brief Battery Client callback
 * \details The Battery Client callback is called by the Mesh Models library on receiving a Battery Status message from the peer
 *
 * @param       event Event that the application should process (see @ref BATTERY_EVENT "Mesh Battery Events")
 * @param       p_event The information about the message received.  The same pointer should be used in the reply if required.
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_battery_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, mesh_battery_event_t *p_data);

/**
 * \brief Battery Client Module initialization
 * \details Application should call this function during startup to register a callback which will be executed when received message requires application processing.
 *
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_battery_client_init(wiced_bt_mesh_battery_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Battery Client Message Handler
 * \details The application should call this models library function to process a message received from the Battery Server.
 *  The function parses and validates the message and if appropriate calls the application to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_battery_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Battery Client Send
 * \details The application can call this function to send get request to the Battery Server. The mesh event should
 * contain the destination address and the application key index to be used to secure the message. In some cases the
 * model can be configured by the provisioner to send the request to a specific address and use specific key. In that
 * case application can use zeroes in the destination address and app key index fields.
 *
 * @param       p_event Mesh event created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_battery_client_send_get(wiced_bt_mesh_event_t *p_event);

/* @} wiced_bt_mesh_battery_client */

/**
 * @addtogroup  wiced_bt_mesh_location_server        Mesh Location Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Location Server module provides a way for a client to set the Global and Local location for a mesh device.
 * Setup location can be retrieved by the same or different clients. The Location Server may also be configured to
 * publish unsolicited messages with the location information to a specific node or to a group of nodes.
 *
 * Two types of location information can be exchanged between the client and the server.  The Global Location Data (see
 * @ref wiced_bt_mesh_location_global_data_t) and the Local Location Data (see @ref wiced_bt_mesh_location_local_data_t).
 *
 * On startup the application should call the wiced_bt_mesh_model_location_server_init function providing the callback
 * to be executed when a Get or Set request is received from a mesh device. The application should call the
 * wiced_bt_mesh_model_location_server_send function in response to a get request, a set request when reply is required
 * or when application logic requires location information to be sent out.  In the latter case the message will be sent
 * out only if device is configured remotely with the destination address where the location status should be set to and
 * and the application key to be used to secure the message.
 *
 * @{
 */

/**
 * \brief Location Server callback.
 * \details The Location Server callback is executed by the Mesh Models library on receiving a Get or a Set message from
 * the peer. In case of the Set the application should update the location information. In case of the Get or if the
 * reply variable is set to TRUE in the p_event structure, the application should call the wiced_bt_mesh_model_location_server_send
 * function returning p_event back to the library. Otherwise p_event should be released.
 *
 * @param       element_idx Element Index of the model
 * @param       event The event that the application should process (see @ref LOCATION_EVENT "Location Events")
 * @param       p_data Pointer to the local/global location data
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_location_server_callback_t)(uint8_t element_idx, uint16_t event, void *p_data);

/**
 * \brief Location Server initialization
 *
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_location_server_init(wiced_bt_mesh_location_server_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Location Server Message Handler
 * \details Application can call this models library function to process a message received from the Location Client
 * and targeted to the Generic Location Server model.
 * The function parses the message and if appropriate calls the application back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_location_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Location Setup Server Message Handler
 * \details Application can call this models library function to process a message received from the Location Client
 * and targeted to the Generic Location Setup Server model.
 * The function parses the message and if appropriate calls the application back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_location_setup_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief The application can call this function to send status message to the Location Client. The mesh event should
 * contain information if this is a reply or an unsolicited message.
 *
 * @param       type Type of the message to be send to the Client (see @ref LOCATION_EVENT "Location Events")
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_data Depending on the type of the message this should point to the Global Location Data (@ref wiced_bt_mesh_location_global_data_t) or the Local Location Data (see @ref wiced_bt_mesh_location_local_data_t)
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_location_server_send(uint16_t type, wiced_bt_mesh_event_t *p_event, void *p_data);

/* @} wiced_bt_mesh_location_server */

/**
 * @addtogroup  wiced_bt_mesh_location_client        Mesh Location Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Location Client module provides a way for an application to configure the device with a Global or a Local Location
 * data or to retrieve this data. The module can also process unsolicited status messages from mesh nodes.
 *
 * On startup the application should call wiced_bt_mesh_model_location_client_init function providing the callback that
 * will be called when location status message is received. The application can call the appropriate Get or Set
 * function to retrieve or setup the Global or Local Location data on a specific mesh node.
 *
 * @{
 */

/**
 * \brief Location Client callback
 * \details The Location Client callback is called by the Mesh Models library on receiving a message from the peer.
 * Depending on the event, the p_data can contain he Global Location Data (see @ref wiced_bt_mesh_location_global_data_t) or
 * the Local Location Data (see @ref wiced_bt_mesh_location_local_data_t). The application is responsible for releasing the
 * p_event when it completes processing of the data.
 *
 * @param       event The event that the application should process (see @ref LOCATION_EVENT "Mesh Location Events")
 * @param       p_event information about the message received including the message source address.
 * @param       p_data Pointer to the data to send
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_location_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Location Client Module initialization
 *
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_location_client_init(wiced_bt_mesh_location_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Location Client Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_location_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);


/**
 * \brief The application can call this function to send Global Location Get client message to the server.
 *
 * @param       p_event Mesh event created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_location_client_send_global_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Set Global Location client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_location_client_send_global_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_location_global_data_t* p_location_data);

/**
 * \brief The application can call this function to send Local Location Get client message to the server.
 *
 * @param       p_event Mesh event created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_location_client_send_global_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Get Location client message to the server.
 *
 * @param       p_event Mesh event created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_location_client_send_local_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Set Location client message to the server.
 *
 * @param       p_event Mesh event created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_location_client_send_local_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_location_local_data_t* p_location_data);

/* @} wiced_bt_mesh_location_client */

/**
 * @addtogroup  wiced_bt_mesh_onoff_server        Mesh OnOff Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh OnOff Server module of the Mesh Models library provides a way to implement functionality for the
 * devices that can be controlled using a simple On/Off switch, aka On/Off client.
 *
 * The OnOff Server module is typically not used by a target applications. It is used by other modules that
 * extend OnOff functionality.  A simple example is a Power Plug application.  Instead of using OnOff module, the
 * application uses Power On/Off, which in addition to OnOff provides On Power Up behavior, i.e. the library
 * saves in the NVRAM configuration of the state required when device is powered up and controls application accordingly.
 * Similarly, an application  for dimmable light uses Light Lightness module.  If peer sends OnOff command, the application receives a
 * callback from the Light Lightness module that the lightness is set to zero.
 *
 * To use the OnOff Server, on startup an application or a module that extends the OnOff functionality should call
 * wiced_bt_mesh_model_onoff_server_init function providing the callback that will be executed when an OnOff request is received
 * to turn the device on or off.
 *
 * The module which extends OnOff server should call the wiced_bt_mesh_model_onoff_changed function when the OnOff state
 * of the device is changed locally.
 *
 * @{
 */

/**
 * Mesh OnOff Server Device.
 * The model is used as a base for multiple other models.  This device is typically not used standalone.  The closest
 * is Power OnOff Device which adds on power up behavior to the device and then can be used standalone.
 */
#ifdef TIME_AND_SCHEDULER_SUPPORT
#define WICED_BT_MESH_MODEL_ONOFF_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SRV, wiced_bt_mesh_model_scheduler_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SETUP_SRV, wiced_bt_mesh_model_scheduler_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SRV, wiced_bt_mesh_model_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SETUP_SRV, wiced_bt_mesh_model_time_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, wiced_bt_mesh_model_onoff_server_scene_store_handler, wiced_bt_mesh_model_onoff_server_scene_recall_handler }
#else
#define WICED_BT_MESH_MODEL_ONOFF_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, wiced_bt_mesh_model_onoff_server_scene_store_handler, wiced_bt_mesh_model_onoff_server_scene_recall_handler }
#endif

/**
 * \brief Generic On/Off Server callback
 * \details The Generic On/Off Server callback is executed by the Mesh Models library on receiving
 * a SET message from the peer. The callback is provided by the application or by the mesh model
 * which extends the Generic On/Off server during the initialization.
 *
 * @param       element_idx Element Index of the model
 * @param       event The event that the application should process (see @ref ONOFF_EVENT "On/Off Events")
 * @param       p_data Pointer to the on/off state data
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_onoff_server_callback_t)(uint8_t element_idx, uint16_t event, void *p_data);

/**
 * \brief On/Off Server Module initialization
 * \details The function shall be called to initialize the On/Off Server Module of the Mesh Models library.
 * For multi element applications, the initialization shall be done for each element that supports on/off get, on/off set
 * functionality.
 *
 * @param       element_idx Device element where OnOff model is used
 * @param       p_callback Application or Parent model callback that will be executed on receiving a message
 * @param       transition_interval Interval in milliseconds to update application during transition change with STATUS messages.  If 0, library will send a single SET message when the transition should start.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE, the device is unprovisioned.
 *
 * @return      None
 */
void wiced_bt_mesh_model_onoff_server_init(uint8_t element_idx, wiced_bt_mesh_onoff_server_callback_t *p_callback, uint32_t transition_interval, wiced_bool_t is_provisioned);

/**
 * \brief On/Off Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  This function is implemented by Mesh Models library and does not need to be replicated
 * by the application or any other module.  The function returns WICED_TRUE if the message is destined for this specific model,
 * and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_onoff_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief On/Off Server Scene Store Handler
 * \details The mesh Scene Server calls this function so that the module can store required data for the scene. The function is
 * implemented in the Mesh Models library and does not need to be replicated by the application.  To extend library functionality,
 * the application can replace the function in the @ref WICED_BT_MESH_MODEL_ONOFF_SERVER, performs its functionality and then call
 * this function directly.
 *
 * @param       element_idx Element index for which scene information is being stored
 * @param       p_buffer Pointer to a buffer where data should be stored
 * @param       buf_size Maximum amount of data a model can store
 *
 * @return      Number of bytes the model wrote into the provided buffer
 */
uint16_t wiced_bt_mesh_model_onoff_server_scene_store_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len);

/**
 * \brief On/Off Server Scene Recall Handler
 * \details When the Scene Server receives the Scene Recall message it calls this function with data previously stored in the NVRAM.
 * The function is implemented in the Mesh Models library and does not need to be replicated by the application.  To extend
 * library functionality, the application can replace the function in the @ref WICED_BT_MESH_MODEL_ONOFF_SERVER, performs
 * its functionality and then call this function directly.
 *
 * @param       element_idx Element index for which scene information is being recalled
 * @param       p_buffer Pointer to a buffer where model's data is located
 * @param       buf_size Maximum amount of data a model can get from the buffer
 * @param       transition_time Specified time in milliseconds to transition to the target state
 * @param       delay Specifies time in milliseconds before starting to transition to the target state
 *
 * @return      Number of bytes the model read from the provided buffer
 */
uint16_t wiced_bt_mesh_model_onoff_server_scene_recall_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len, uint32_t transition_time, uint32_t delay);

/**
 * \brief If On/Off state of the device can be changed locally, the application shall call this function to notify
 * OnOff library that the value has been changed.
 *
 * @param       element_idx Element index for which onoff state has changed
 * @param       onoff New value of onoff
 *
 * @return      None
 */
void wiced_bt_mesh_model_onoff_changed(uint8_t element_idx, uint8_t onoff);

/* @} wiced_bt_mesh_onoff_server */

/**
 * @addtogroup  wiced_bt_mesh_onoff_client        Mesh OnOff Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh On/Off Client module of the Mesh Models library provides a way for an application to send OnOff
 * commands to the server and optionally receive status information.
 *
 * To include On/Off functionality, the device description shall contain @ref WICED_BT_MESH_MODEL_ONOFF_CLIENT. To
 * implement a device which supports multiple switches, the @ref WICED_BT_MESH_MODEL_ONOFF_CLIENT shall be included
 * in multiple elements. Based on this information, the device that performs configuration will be able to
 * specify different destination for every switch.
 *
 * In a simplest form, an application needs to initialize using the @ref wiced_bt_mesh_model_onoff_client_init function
 * and then call @ref wiced_bt_mesh_model_onoff_client_set to tell the Mesh Library to send a command. The configuration
 * of the destination address and security credential is done during the provisioning/configuration process and
 * the application does not need to worry about it.
 *
 * If an application needs more control of how messages are sent out, it can use @ref wiced_bt_mesh_model_onoff_client_send_get
 * and @ref wiced_bt_mesh_model_onoff_client_send_set functions. These functions require to use the p_event parameter which
 * include among others address of the desgination, application key index, retrnansmission parameters, TTL, and various others.
 * After creating the event, the application can adjust the paramters as needed.
 *
 * To receive On/Off status information, an application during initialization specifies the callback that will
 * be executed when OnOff status message is received. The status information can be received as a result of sending
 * On/Off Get and Set commands.  In addition to that, the On/Off server can be configured to send the status information
 * to the device that implements the OnOff Client module, or to a group of the devices to which the OnOff Client belongs.
 *
 * @{
 */

/**
 * Mesh On/Off Client Device.
 * The device implements a Generic On/Off switch.
 * The device can be configured to control any device that supports Generic On/Off Server model (WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV).
 * For example, a @ref WICED_BT_MESH_MODEL_LIGHT_CTL_SERVER, @ref WICED_BT_MESH_MODEL_LIGHT_HSL_SERVER,...
 */
#define WICED_BT_MESH_MODEL_ONOFF_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_CLNT, wiced_bt_mesh_model_onoff_client_message_handler, NULL, NULL }

/**
 * \brief On/Off Client callback is called by the Mesh Models library on receiving a On/Off status message from the peer.
 *
 * @param       event The event that the application should process (see @ref ONOFF_EVENT "Mesh On/Off Events")
 * @param       p_event information about the message received.  It is application responsibility to release the event using @ref wiced_bt_mesh_release_event.
 * @param       p_data Pointer to the data to send
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_onoff_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief On/Off Client Module initialization
 * \details The function shall be called to initialize the On/Off Module of the Mesh Models library.  For multi element
 * applications, the initialization shall be done for each element that supports On/Off get/set functionality.
 * The p_callback parameter may be set to NULL if no On/Off Status processing is required.
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss.
 *
 * @return      None
 */
void wiced_bt_mesh_model_onoff_client_init(uint8_t element_idx, wiced_bt_mesh_onoff_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief On/Off Client Message Handler
 * \details This is an internal Mesh Models library function and does not need to be implemented or called by the application,
 * unless application wants to replace the library functionality.  The Mesh Core library calls this function for each message received.
 * The function shall return WICED_TRUE if the message is destined for this specific model, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_onoff_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief The application can call this function to send get On/Off state client message to the server.
 * Model library will send the message out only if the OnOff Client model is configured by the provisioner
 * for publication.
 *
 * If the application has registerd the callback during the initialization, the callback will be executed
 * twice, first with @ref WICED_BT_MESH_TX_COMPLETE event indicating that the event has been sent out
 * and then with @ref WICED_BT_MESH_ONOFF_STATUS event with the status information.
 *
 * @param       element_idx Element Index of the model
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_onoff_client_get(uint8_t element_idx);

/**
 * \brief The application can call this function to send get On/Off state client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_onoff_client_send_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send On/Off command.  The application
 * does not need to know the destination of the command or application key that will be used to
 * secure the packet. The Mesh Models library will send the message out only if the OnOff Client
 * model of the referenced element is configured by the provisioner for publication.
 *
 * If the application has registered the callback during the initialization, the callback will be executed
 * with @ref WICED_BT_MESH_TX_COMPLETE event indicating that the event has been sent out. The OnOff
 * command is sent not requiring acknowledgment.  The @ref WICED_BT_MESH_ONOFF_STATUS event
 * will only be executed only if the provisioner configured the recipient of the command to publish
 * the status data to this element, or to a group to which this element belongs to.
 *
 * @param       element_idx Element Index of the model
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_onoff_client_set(uint8_t element_idx, wiced_bt_mesh_onoff_set_data_t* p_data);

/**
 * \brief The application can call this function to send Set OnOff client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_onoff_client_send_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_onoff_set_data_t* p_data);

/* @} wiced_bt_mesh_onoff_client */

/**
 * @addtogroup  wiced_bt_mesh_level_server        Mesh Level Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Level Server module of the Mesh Models library provides a way to implement functionality for the
 * devices that can be controlled using a simple level slider, aka Level client.
 *
 * The Level Server module is typically not used by a target applications. It is used by other modules that
 * extend Level functionality. For example a power adapter with controllable power level uses a Power Level
 * module which in addition to Level provides Power On/Off and On Power Up control.  Similarly,
 * an application for a dimmable light uses Light Lightness module.  While the client sends Level command,
 * the application receives callback from the Light Lightness module that the lightness is changed appropriately.
 *
 * To use the Level Server, on startup an application or a module that extends the Level functionality should call
 * @ref wiced_bt_mesh_model_level_server_init function providing the callback that will be executed when the change
 * of the level is requested.
 *
 * Note, that the Mesh Models library implements the transition state machine.  For example, when the change in
 * the level is requested over some transition time, the application will be called periodically while transition
 * is in progress.
 *
 * If the application initializes the Level server directly it should call @ref the wiced_bt_mesh_model_level_changed
 * function when the Level state of the device is changed locally.
 *
 * @{
 */

/**
 * Mesh Level Server Device.
 * The model is used as a base for multiple other models.  It is typically not used standalone.  The closest
 * is Power Level Device which adds on power up behavior to the device and then can be used standalone.
 */
#ifdef TIME_AND_SCHEDULER_SUPPORT
#define WICED_BT_MESH_MODEL_LEVEL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SRV, wiced_bt_mesh_model_scheduler_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SETUP_SRV, wiced_bt_mesh_model_scheduler_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SRV, wiced_bt_mesh_model_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SETUP_SRV, wiced_bt_mesh_model_time_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, wiced_bt_mesh_model_level_server_scene_store_handler, wiced_bt_mesh_model_level_server_scene_recall_handler }
#else
#define WICED_BT_MESH_MODEL_LEVEL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, wiced_bt_mesh_model_level_server_scene_store_handler, wiced_bt_mesh_model_level_server_scene_recall_handler }
#endif

/**
 * \brief Generic Level Server callback is called by the Mesh Models library when the level needs
 * to be changed. If peer device sends Set Level, Delta Level or Move Level command specifying
 * not zero transition interval, the callback will be called periodically.  The only event that
 * the application needs to process is @ref WICED_BT_MESH_LEVEL_SET.
 *
 * @param       element_idx Element Index of the model
 * @param       event The event that the application should process (see @ref LEVEL_EVENT "Level Events")
 * @param       p_data Pointer to the level state data
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_level_server_callback_t)(uint8_t element_idx, uint16_t event, void *p_data);

/**
 * \brief Level Server Module initialization
 * \details The function shall be called to initialize the Level Module of the Mesh Models library.  For multi element
 * applications, the initialization shall be done for each element that supports level get, level set, level delta,
 * or level move functionality.
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback Application or Parent model callback that will be executed on receiving a message
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_level_server_init(uint8_t element_idx, wiced_bt_mesh_level_server_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Level Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_level_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Level Server Scene Store Handler
 * \details The mesh Scene Server calls this function so that the module can store required data for the scene.
 *
 * @param       element_idx Element index for which scene information is being stored
 * @param       p_buffer Pointer to a buffer where data should be stored
 * @param       buf_size Maximum amount of data a model can store
 *
 * @return      Number of bytes the model wrote into the provided buffer
 */
uint16_t wiced_bt_mesh_model_level_server_scene_store_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len);

/**
 * \brief Level Server Scene Store Handler
 * \details When Scene Server receives Scene Recall message it calls this function with data previously stored in the NVRAM.
 *
 * @param       element_idx Element index for which scene information is being recalled
 * @param       p_buffer Pointer to a buffer where model's data is located
 * @param       buf_size Maximum amount of data a model can get from the buffer
 * @param       transition_time Specified time in milliseconds to transition to the target state
 * @param       delay Specifies time in milliseconds before starting to transition to the target state
 *
 * @return      Number of bytes the model read from the provided buffer
 */
uint16_t wiced_bt_mesh_model_level_server_scene_recall_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len, uint32_t transition_time, uint32_t delay);

/**
 * \brief An application can call this function to notify Generic Level library that the value has been changed locally.
 *
 * @param       element_idx Element index for which level state has changed
 * @param       value New value of level
 *
 * @return      None
*/
void wiced_bt_mesh_model_level_changed(uint8_t element_idx, int16_t value);

/* @} wiced_bt_mesh_level_server */

/**
 * @addtogroup  wiced_bt_mesh_level_client        Mesh Level Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Level Client module of the Mesh Models library provides a way for an application to send Level
 * commands (set, delta and move) to the server and optionally receive status information. The module can be
 * used in an implementation of a simple slider device.
 *
 * To include Level functionality, the device description shall contain @ref WICED_BT_MESH_MODEL_LEVEL_CLIENT. To
 * implement a device which supports multiple sliders, the @ref WICED_BT_MESH_MODEL_LEVEL_CLIENT shall be included
 * in multiple elements. Based on this information, the device that performs configuration will be able to
 * specify different destination for different sliders supported by the device.
 *
 * There are three controlling methods which can be used depending on the application. The device which remembers its
 * state (for example, the current level), can use Level Set command.  The device which does not remember the state
 * but knows how much the level is being changed can use Delta Set command.  The device which can detect speed of the change
 * can use Move Set command.
 *
 * In a simplest form, an application needs to initialize using the @ref wiced_bt_mesh_model_level_client_init function
 * and then call @ref wiced_bt_mesh_model_level_client_set to tell the Mesh Library to send a command. The configuration
 * of the destination address and security credential is done during the provisioning/configuration process and
 * the application does not need to worry about it.
 *
 * If an application needs more control of how messages are sent out, it can use @ref wiced_bt_mesh_model_level_client_send_get,
 * @ref wiced_bt_mesh_model_level_client_send_set, @ref wiced_bt_mesh_model_level_client_send_delta_set, or
 * @ref wiced_bt_mesh_model_level_client_send_move_set functions. These functions require to use the p_event parameter which
 * include among others address of the desgination, application key index, retrnansmission parameters, TTL, and various others.
 * After creating the event, the application can adjust the paramters as needed.
 *
 * To receive Level Status information, an application during initialization specifies the callback that will
 * be executed when Level status message is received. The status information can be received as a result of sending
 * Level Get and Set commands.  In addition to that, the Level server can be configured to send the status information
 * to the device that implements the Level Client module, or to a group of the devices to which the OnOff Client belongs.
 *
 * @{
 */

/**
 * \brief Generic Level Client callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref LEVEL_EVENT "Mesh Level Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the level data
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_level_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Generic Level Client Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise. Generic Level Server device.
 * The function parses the message and if appropriate calls the parent back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_level_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Level Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_level_client_init(uint8_t element_idx, void *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief The application can call this function to send Set Level client message. The function parameters doe not contain any information on where and how the
 * message has not be sent because configuration has been done by the Confoguration client.
 *
 * This function will likely be used in the controlling application
 * where application remembers the target Level state. If application does not remember the target state, the send_delta is more appropriate.
 *
 * @param       element_idx Device element to where model is used
 * @param       is_final WICED_TRUE, if this is a final set command.  WICED_FALSE if application expects more commands to come.
 * @param       p_data Pointer to the data with current level and transition time
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_level_client_set(uint8_t element_idx, wiced_bool_t is_final, wiced_bt_mesh_level_set_level_t *p_data);

/**
 * \brief The application can call this function to send Generic Level Get message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_level_client_send_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Set Level client message to the server. This function will likely be used in the controlling application
 * where application remembers the target Level state. If application does not remember the target state, the send_delta is more appropriate.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_level_client_send_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_level_set_level_t* p_data);

/**
 * \brief The application can call this function to send Set Delta Level client message to the server. This command will likely be used in the controlling application
 * where application does not remember the target Level state. The instant command is just to adjust level.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_level_client_send_delta_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_level_set_delta_t* p_data);

/**
 * \brief The application can call this function to send Set Move client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_level_client_send_move_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_level_set_move_t* p_data);

/* @} wiced_bt_mesh_level_client */

/**
 * @addtogroup  wiced_bt_mesh_default_transition_time_server        Mesh Default Transition Time Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Default Transition Time Server module of the Mesh Models library provides implementation of the Default Transition
 * Time model. The module cannot be used as a standalone device.  But it is required to be included in the definition of various
 * other devices.
 *
 * All functions of the module are implemented internally in the Mesh Models library and do not need to be replicated in the
 * application, unless application needs to replace the functionality of the server.
 *
 * @{
 */

/**
 * Mesh Default Transition Time Server Module.
 * The model is used as a base for multiple other models.  It is typically not used standalone.
 */
#define WICED_BT_MESH_MODEL_DEFAULT_TRANSITION_TIME_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }

/**
 * \brief Generic Default Transition Time Server callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref DEFAULT_TRANSITION_TIME_EVENT "Default Transition Time Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the on/off state data
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_default_transition_time_server_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *wiced_bt_mesh_default_transition_time_data_t);

/**
 * \brief Generic Default Transition Time Module initialization
 * \details An application should initialize default transition time model for each element which supports any of the server models that have transitions.
 * The callback that is passed as a parameter will be executed if the provisioner changes the value of the default transition time.
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_default_transition_time_server_init(uint8_t element_idx, wiced_bt_mesh_default_transition_time_server_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Default Transition Time Server Message Handler
 * \details The Mesh Core library calls this function for each message received.
 * The function returns WICED_TRUE if the message is destined for this specific
 * model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_default_transition_time_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/* @} wiced_bt_mesh_default_transition_time_server */

/**
 * @addtogroup  wiced_bt_mesh_default_transition_time_client        Mesh Default Transition Time Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Default Transition Time Client module of the Mesh Models library provides a way for a controlling device
 * to set default transition time for the devices in the mesh network.
 *
 * @{
 */

/**
 * \brief Default Transition Time Client callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref DEFAULT_TRANSITION_TIME_EVENT "Mesh Default Transition Time Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the data to send
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_default_transition_time_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Default Transition Time Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_default_transition_time_client_init(uint8_t element_idx, wiced_bt_mesh_default_transition_time_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
* \brief Default Transition Time Client Message Handler
* \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise. Default Transition Time Server device.
* The function parses the message and if appropriate calls the parent back to perform functionality.
*
* @param       p_event Mesh event with information about received message.
* @param       p_data Pointer to the data portion of the message
* @param       data_len Length of the data in the message
*
* @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_default_transition_time_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief The application can call this function to send get Default Transition Time client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_default_transition_time_client_send_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Set Default Transition Time client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the application for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_default_transition_time_client_send_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_default_transition_time_data_t* p_data);

/* @} wiced_bt_mesh_default_transition_time_client */

/**
 * @addtogroup  wiced_bt_mesh_power_onoff_server        Mesh Power OnOff Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Power OnOff Server module of the Mesh Models library provides a way to implement functionality for the
 * devices that can be controlled using a simple On/Off switch, aka On/Off client.  In addition to that
 * the device would support a default transition time server functionality and application can specify On Power Up behavior.
 * The default transition time and behavior of the device on power up is managed by the Mesh Models library
 * and is not exposed to the application.  The library stores in the persistent storage the configured values and
 * controls application based on the configuration.
 *
 * To instantiate the Power OnOff Server module, that application includes the
 * @ref WICED_BT_MESH_MODEL_POWER_ONOFF_SERVER in the device definition.  In addition to that, the application shall call
 * @ref wiced_bt_mesh_model_power_onoff_server_init function providing the callback that will be executed when an OnOff request
 * is received to turn the device on or off.
 *
 * The Mesh Power OnOff Server module maintains the On/Off state and notifies the application when the
 * state changes. The application shall call the @ref wiced_bt_mesh_model_power_onoff_server_onoff_changed function
 * when the OnOff state of the device is changed locally.
 *
 * @{
 */

/**
 * Mesh Power OnOff Server Device.
 * The device combines onoff server functionality with default on power up behavior.
 * The model can be used standalone, for example, to implement a power plug.
 * The time and scheduler models are optional and can be commented out to reduce memory footprint.
 */
#ifdef TIME_AND_SCHEDULER_SUPPORT
#define WICED_BT_MESH_MODEL_POWER_ONOFF_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SRV, wiced_bt_mesh_model_scheduler_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SETUP_SRV, wiced_bt_mesh_model_scheduler_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SRV, wiced_bt_mesh_model_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SETUP_SRV, wiced_bt_mesh_model_time_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, wiced_bt_mesh_model_onoff_server_scene_store_handler, wiced_bt_mesh_model_onoff_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }
#else
#define WICED_BT_MESH_MODEL_POWER_ONOFF_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, wiced_bt_mesh_model_onoff_server_scene_store_handler, wiced_bt_mesh_model_onoff_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }
#endif

/**
 * \brief Generic Power On/Off Server callback
 * \details The Generic Power On/Off Server callback is executed by the Mesh Models library on receiving
 * a On/Off Set message from the peer. The callback is provided by the application during the initialization.
 *
 * @param       element_idx Element Index of the model
 * @param       event The event that the application should process (see @ref ONOFF_EVENT "On/Off Events")
 * @param       p_data Pointer to the on/off state data
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_power_onoff_server_callback_t)(uint8_t element_idx, uint16_t event, void *p_data);

/**
 * \brief Power On/Off Server Module initialization
 * \details The function shall be called to initialize the Power On/Off Server Module of the Mesh Models library.
 * For multi element applications, the initialization shall be done for each element that supports
 * Power On/Off functionality.
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback Application or Parent model callback that will be executed when application action is required, for example, to adjust the hardware.
 * @param       transition_interval Interval in milliseconds to update application during transition change with STATUS messages.  If 0, library will send a single SET message when the transition should start.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM.
 *
 * @return      None
 */
void wiced_bt_mesh_model_power_onoff_server_init(uint8_t element_idx, wiced_bt_mesh_power_onoff_server_callback_t *p_callback, uint32_t transition_interval, wiced_bool_t is_provisioned);

/**
 * \brief Power On/Off Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  This function is implemented by Mesh
 * Models library and does not need to be replicated by the application or any other module.  The function returns
 * WICED_TRUE if the message is destined for this specific model, and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_power_onoff_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Power On/Off Setup Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  This function is implemented by Mesh
 * Models library and does not need to be replicated by the application or any other module.  The function returns
 * WICED_TRUE if the message is destined for this specific model, and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_power_onoff_setup_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \details The application shall call this function if the onoff status has changed locally.
 * The Power On/Off module stores new state information and publishes the status change if appropriate.
 *
 * @param       element_idx Element index for which onoff state has changed
 * @param       onoff New value of onoff
 *
 * @return      None
 *
 */
void wiced_bt_mesh_model_power_onoff_server_onoff_changed(uint8_t element_idx, uint8_t onoff);

/* @} wiced_bt_mesh_power_onoff_server */

/**
 * @addtogroup  wiced_bt_mesh_power_onoff_client        Mesh Power OnOff Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Power On/Off Client module of the Mesh Models library provides a way for an application to
 * manage the On Power Up behavior of other mesh devices. It is typicallly used by the applications running on
 * devices which performs mesh network configuration.
 *
 * To include Power On/Off functionality, the device description shall contain @ref WICED_BT_MESH_MODEL_POWER_ONOFF_CLIENT.
 * The application shall also initialize the Power On/Off Client calling the @ref wiced_bt_mesh_model_power_onoff_client_init
 * registering a callback to receive Power OnOff status messages
 *
 * To find out the current On Power Up state set on a particular device and to modify the behavior the application
 * shall use @ref wiced_bt_mesh_model_power_onoff_client_send_onpowerup_get. and @ref wiced_bt_mesh_model_power_onoff_client_send_onpowerup_set
 * methods respectively. These functions require to use the p_event parameter which include among others the address of
 * the desgination device, the application key index, the retrnansmission parameters, the TTL, and various others.
 *
 * @{
 */

/**
 * \brief Power On/Off Client callback is called by the Mesh Models library on receiving @ref WICED_BT_MESH_POWER_ONOFF_ONPOWERUP_STATUS
 * message from the peer. The function is also called when transmission of the request has been completed @ref WICED_BT_MESH_TX_COMPLETE
 * with tx_failed field of the p_event parameter indicating if the request has been sent out successfully.
 *
 * @param       event The event that the application should process (see @ref POWER_ONOFF_EVENT "Mesh Power On/Off Events")
 * @param       p_event information about the message received.
 * @param       p_data Pointer to the data with the status information
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_power_onoff_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Power On/Off Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback The application callback function that will be executed by the Mesh Models library when a request has been sent out, or failed, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_power_onoff_client_init(uint8_t element_idx, wiced_bt_mesh_power_onoff_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Generic Power OnOff Client Message Handler
 * \details This is an internal Mesh Models library function and does not need to be implemented or called by the application,
 * unless application wants to replace the library functionality.  The Mesh Core library calls this function for each message received.
 * The function shall return WICED_TRUE if the message is destined for this specific model, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_power_onoff_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief The application can call this function to send the Get Power On/Off state client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the application.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_power_onoff_client_send_onpowerup_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Set OnOff client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the application.
 *              The p_event->reply specifies if application wants to receive Status from the peer.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_power_onoff_client_send_onpowerup_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_power_onoff_data_t* p_data);

/* @} wiced_bt_mesh_power_onoff_client */

/**
 * @addtogroup  wiced_bt_mesh_power_level_server        Mesh Power Level Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Power Level Server module of the Mesh Models library provides a way to implement functionality of
 * a power plug type of a device that can be controlled using a Level slider (Generic Level client) and On/Off
 * switch (Generic On/Off client). In addition to that the device supports a default transition time server
 * functionality and the application can specify on power up behavior.
 *
 * To instantiate the Power Level Server module, that application includes the
 * @ref WICED_BT_MESH_MODEL_POWER_LEVEL_SERVER in the device definition. In addition to that, the application shall call
 * @ref wiced_bt_mesh_model_power_level_server_init function providing the @ref wiced_bt_mesh_power_level_server_callback_t
 * callback that will be executed when power level of the device needs to be changed.
 *
 * If the device supports several elements with the Power Level Server, for example a power strip with several power plugs, the @ref
 * WICED_BT_MESH_MODEL_POWER_LEVEL_SERVER shall be included for each element.

 *
 * The Mesh Power Level Server module in the Mesh Models library maintains the On/Off and Level states and notifies the
 * application on the state changes. The application shall call the @ref wiced_bt_mesh_model_power_level_server_level_changed function
 * when the power level of the device is changed locally.
 *
 * Most of the functionality of the Mesh Power Level server device is implemented in the Mesh Models Library. The
 * application only needs to set hardware appropriately on receiving @ref WICED_BT_MESH_POWER_LEVEL_SET event.
 * If client requests to change power to the target value over a period of time, the library will maintain the timer
 * and will periodically call the application with appropriate power level.
 *
 * The default transition time is managed by the Mesh Models library. The application provides factory default
 * in the default_transition_time element of the @ref wiced_bt_mesh_core_config_t configuration structure.
 * The default value is used unless overwritten by the default transition time client.
 *
 * The on power up behavior is managed by the Mesh Models library. The application provides factory default
 * in the onpowerup_state element of the @ref wiced_bt_mesh_core_config_t configuration structure.
 * The default value is used unless overwritten by the default transition time client.
 *
 * @{
 */

/**
 * \brief Mesh Power Level Server Device Composition Data.
 * \details The Mesh Power Level Server device combines power level and power onoff server functionality with default transition time and on power up behavior.
 * The model can be used standalone, for example, to implement a power plug with adjustible level.
 * The time and scheduler models are optional and can be commented out to reduce memory footprint.
 */
#ifdef TIME_AND_SCHEDULER_SUPPORT
#define WICED_BT_MESH_MODEL_POWER_LEVEL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SRV, wiced_bt_mesh_model_scheduler_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SETUP_SRV, wiced_bt_mesh_model_scheduler_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SRV, wiced_bt_mesh_model_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SETUP_SRV, wiced_bt_mesh_model_time_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_LEVEL_SRV, wiced_bt_mesh_model_power_level_server_message_handler, wiced_bt_mesh_model_power_level_server_scene_store_handler, wiced_bt_mesh_model_power_level_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_LEVEL_SETUP_SRV, wiced_bt_mesh_model_power_level_setup_server_message_handler, NULL, NULL }
#else
#define WICED_BT_MESH_MODEL_POWER_LEVEL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_LEVEL_SRV, wiced_bt_mesh_model_power_level_server_message_handler, wiced_bt_mesh_model_power_level_server_scene_store_handler, wiced_bt_mesh_model_power_level_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_LEVEL_SETUP_SRV, wiced_bt_mesh_model_power_level_setup_server_message_handler, NULL, NULL }
#endif

/**
 * \brief Generic Power Level Server callback.
 * \details Generic Power Level Server callback is executed by the Mesh Models library to change the power level of the device.
 * The p_data parameter points to the @ref wiced_bt_mesh_power_level_status_data_t structure. Typically the application
 * will only need to use the value of the present_power field which specify the power level of the
 * device which shall be set at this particular moment. If the remaining_time field is not zero, the Mesh Library
 * will execute the callback at least once more before the transition is completed.
 *
 * If peer device requested to transition to the power level over a period of time, the callback will be
 * executed repeatedly.  Mesh Library calculates the present power level and submits to the application to
 * update the hardware.
 *
 * @param       element_idx Element Index of the model
 * @param       event The event that the application should process (see @ref POWER_LEVEL_EVENT "Power Level Events")
 * @param       p_data Pointer to the power level data
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_power_level_server_callback_t)(uint8_t element_idx, uint16_t event, void *p_data);

/**
 * \brief Power Level Module initialization
 * \details A Mesh application which contains Power Level Server should call this function for each element where the Power Level Server is present.
 * If application needs to support multiple Power Level servers, the initialization shall be called multiple times, once for each element of the device.
 *
 * The application callback will be executed by the Model Library when it is required to set hardware to the desired power level
 * to a certain level.
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback Application callback that will be executed to set the hardware to the specified level.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the Mesh Models Library deletes this module's configuration data stored in the NVRAM.
 *
 * @return      WICED_TRUE if initialization was successful
 */
void wiced_bt_mesh_model_power_level_server_init(uint8_t element_idx, wiced_bt_mesh_power_level_server_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
* \brief Power Level Changed
* \details The application shall call this function if the power level of the device has been changed locally.
 * The Power Level module stores new state information and publishes the status change if appropriate.
 *
 * @param       element_idx Element index for which onoff state has changed
 * @param       level New value of the power level
 *
 * @return      None
 */
void wiced_bt_mesh_model_power_level_server_level_changed(uint8_t element_idx, uint16_t level);

/**
 * \brief Power Level Server Message Handler
 * \details The Mesh Core library calls this function for each message received. This function is implemented by Mesh
 * Models library and does not need to be replicated by the application or any other module.  The function returns
 * WICED_TRUE if the message is destined for this specific model, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_power_level_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Power Level Setup Server Message Handler
 * \details The Mesh Core library calls this function for each message received. This function is implemented by Mesh
 * Models library and does not need to be replicated by the application or any other module.  The function returns
 * WICED_TRUE if the message is destined for this specific model, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_power_level_setup_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Power Level Server Scene Store Handler
 * \details The mesh Scene Server calls this function so that the module can store required data for the scene. This function is implemented by Mesh
 * Models library and does not need to be replicated by the application or any other module.
 *
 * @param       element_idx Element index for which scene information is being stored
 * @param       p_buffer Pointer to a buffer where data should be stored
 * @param       buf_size Maximum amount of data a model can store
 *
 * @return      Number of bytes the model wrote into the provided buffer
 */
uint16_t wiced_bt_mesh_model_power_level_server_scene_store_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len);

/**
 * \brief Power Level Server Scene Store Handler
 * \details When Scene Server receives Scene Recall message it calls this function with data previously stored in the NVRAM. This function is implemented by Mesh
 * Models library and does not need to be replicated by the application or any other module.
 *
 * @param       element_idx Element index for which scene information is being recalled
 * @param       p_buffer Pointer to a buffer where model's data is located
 * @param       buf_size Maximum amount of data a model can get from the buffer
 * @param       transition_time Specified time in milliseconds to transition to the target state
 * @param       delay Specifies time in milliseconds before starting to transition to the target state
 *
 * @return      Number of bytes the model read from the provided buffer
 */
uint16_t wiced_bt_mesh_model_power_level_server_scene_recall_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len, uint32_t transition_time, uint32_t delay);

/* @} wiced_bt_mesh_power_level_server */

/**
 * @addtogroup  wiced_bt_mesh_power_level_client        Mesh Power Level Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Power Level Client module of the Mesh Models library provides a way for an application to control
 * power level of the Power Level Servers in the mesh network optionally receive status information.
 *
 * To include Power Level Client functionality, the device description shall contain @ref WICED_BT_MESH_MODEL_POWER_LEVEL_CLIENT.
 *
 * @{
 */

#define WICED_BT_MESH_MODEL_POWER_LEVEL_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_LEVEL_CLNT, wiced_bt_mesh_model_power_level_client_message_handler, NULL, NULL }

/**
 * \brief Power Level Client callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref POWER_LEVEL_EVENT "Power Level Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the level data
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_power_level_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
* \brief Generic Power Level Client Message Handler
* \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise. Generic Power Level Server device.
* The function parses the message and if appropriate calls the parent back to perform functionality.
*
* @param       p_event Mesh event with information about received message.
* @param       p_data Pointer to the data portion of the message
* @param       data_len Length of the data in the message
*
* @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_power_level_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Power Level Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_power_level_client_init(uint8_t element_idx, wiced_bt_mesh_power_level_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief The application can call this function to send Generic Power Level Get message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_power_level_client_send_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Set Level client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_power_level_client_send_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_power_level_set_level_t* p_data);

/**
 * \brief The application can call this function to send Get Last Level client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_power_level_client_send_last_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Get Default Level client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_power_level_client_send_default_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Set Default Level client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_power_level_client_send_default_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_power_default_data_t* p_data);

/**
 * \brief The application can call this function to send Get Range Level client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_power_level_client_send_range_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Set Range Level client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_power_level_client_send_range_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_power_level_range_set_data_t* p_data);

/* @} wiced_bt_mesh_power_level_client */

/**
 * \brief Generic User Property Server callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       element_idx Element Index of the model
 * @param       event The event that the application should process (see @ref USER_PROPERTY_EVENT "User Property Events")
 * @param       p_data Pointer to the property data
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_property_server_callback_t)(uint8_t element_idx, uint16_t event, void *p_data);

/**
 * \brief Property Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback Application or Parent model callback that will be executed on receiving a message
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_property_server_init(uint8_t element_idx, wiced_bt_mesh_property_server_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief User Property Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_user_property_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Admin Property Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_admin_property_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Manufacturer Property Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_manufacturer_property_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Client Property Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_client_property_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/*
 * Application can call this function to send the Properties Status message to the Client. The mesh event should
 * contain information if this is a reply or an unsolicited message.
 *
 * @param       p_event information about the message to be set.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_property_server_send_properties_status(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_properties_status_data_t *p_data);

/*
 * Application can call this function to send the Property Status message to the Client. The mesh event should
 * contain information if this is a reply or an unsolicited message.  The wiced_bt_mesh_property_status_data_t contains
 * the type, ID and the value of the property
 *
 * @param       p_event information about the message to be set.
 * @param       type of the property requested
 * @param       p_data Pointer to the property configuration
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_property_server_send_property_status(wiced_bt_mesh_event_t *p_event, uint8_t type, wiced_bt_mesh_core_config_property_t *p_property);

/**
 * \brief Property Client callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref PROPERTY_EVENT "Mesh Property Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the data to send
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_property_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Property Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_property_client_init(uint8_t element_idx, wiced_bt_mesh_property_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Property Client Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_property_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief The application can call this function to send Properties Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_property_client_send_properties_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_properties_get_data_t *p_data);

/**
 * \brief The application can call this function to send Property Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_property_client_send_property_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_property_get_data_t *p_get);

/**
 * \brief The application can call this function to send Property Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_property_client_send_property_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_property_set_data_t* p_sata);

/**
 * @addtogroup  wiced_bt_mesh_light_lightness_server   Mesh Light Lightness Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Light Lightness Server module provides a way for an application to implement a dimmable light device which
 * can be controlled specifically by setting the brightness, but also can be can be controlled using a Level slider (Generic
 * Level client) and On/Off switch (Generic On/Off client). In addition to that the device supports a default transition time
 * server functionality and the application can specify on power up behavior.
 *
 * To include the Mesh Models support for the Light Lightness Server model the application includes @ref
 * WICED_BT_MESH_MODEL_LIGHT_LIGHTNESS_SERVER in the device definition. If the device supports several
 * elements with Light Lightness Server, for example, a light fixture or a chandelier with several dimmable
 * lights, the @ref WICED_BT_MESH_MODEL_LIGHT_LIGHTNESS_SERVER shall be included for each element.
 *
 * In addition to the definition, the application shall initialize the Light Lightness module by calling
 * @ref wiced_bt_mesh_model_light_lightness_server_init function for each element where the light dimming
 * functionality is required providing the @ref wiced_bt_mesh_light_lightness_server_callback_t
 * callback that will be executed when the brightness level of the device needs to be changed.
 *
 * Note: The Light Lightness module is used internally by the mesh models library in color light scenarios where
 * dimming is one of the methods controlling the light. In that case, the application shall not implicitely include
 * @ref WICED_BT_MESH_MODEL_LIGHT_LIGHTNESS_SERVER and shall not perform module initialization.
 *
 * While performing initialization the application provides the callback that will be executed when the library needs
 * to change the brightness of a light due to a request received over the mesh network, or due to the internal schedule.
 *
 * If brightness of the dimmable light is changed locally the application should call the
 * @ref wiced_bt_mesh_model_light_lightness_status_changed so that the library can maintain correct state and
 * send an appropriate status message if it is configured to do so.
 *
 * Most of the functionality of the Light Lightness server device is implemented in the Mesh Models Library. The
 * application only needs to set hardware appropriately on receiving @ref WICED_BT_MESH_LIGHT_LIGHTNESS_SET event.
 * If client requests to change the brightness to the target value over a period of time, the library will maintain
 * the timer and will periodically call the application with appropriate power level.
 *
 * The default transition time is managed by the Mesh Models library. The application provides factory default
 * in the default_transition_time element of the @ref wiced_bt_mesh_core_config_t configuration structure.
 * The default value is used unless overwritten by the default transition time client.
 *
 * The on power up behavior is managed by the Mesh Models library. The application provides factory default
 * in the onpowerup_state element of the @ref wiced_bt_mesh_core_config_t configuration structure.
 * The default value is used unless overwritten by the default transition time client.
 *
 * @{
 */

/**
 * \brief Mesh Light Lightness Server Device.
 * \details The Mesh Light Lightness Server Device device implements all required models for a dimmable bulb.
 * The time and scheduler models are optional and can be commented out to reduce memory footprint.
 */
#ifdef TIME_AND_SCHEDULER_SUPPORT
#define WICED_BT_MESH_MODEL_LIGHT_LIGHTNESS_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SRV, wiced_bt_mesh_model_scheduler_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SETUP_SRV, wiced_bt_mesh_model_scheduler_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SRV, wiced_bt_mesh_model_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SETUP_SRV, wiced_bt_mesh_model_time_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SRV, wiced_bt_mesh_model_light_lightness_server_message_handler, wiced_bt_mesh_model_light_lightness_server_scene_store_handler, wiced_bt_mesh_model_light_lightness_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV, wiced_bt_mesh_model_light_lightness_setup_server_message_handler, NULL, NULL }
#else
#define WICED_BT_MESH_MODEL_LIGHT_LIGHTNESS_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SRV, wiced_bt_mesh_model_light_lightness_server_message_handler, wiced_bt_mesh_model_light_lightness_server_scene_store_handler, wiced_bt_mesh_model_light_lightness_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV, wiced_bt_mesh_model_light_lightness_setup_server_message_handler, NULL, NULL }
#endif

/**
 * \brief Light Lightness Server callback
 * \details Light Lightness Server callback is executed by the Mesh Models library when the application is required to
 * adjust the amount of light emitted by the device. The application is responsible for setting the hardware
 * (for example, by controlling PWM) to comply with the value requested by the library.
 *
 * If peer device requested to transition to the new brightness level over a period of time, the callback will be
 * executed repeatedly.  Mesh Library calculates the present lightness level and submits to the application to
 * update the hardware.
 *
 * The p_data parameter points to the @ref wiced_bt_mesh_light_lightness_status_t structure. Typically the application
 * will only need to use the value of the lightness_linear_present field which specify the lightness of the
 * device which shall be set at this particular moment. If the remaining_time field is not zero,
 * the transition to the new target level is in progress and the Mesh Library will execute the callback at
 * least once more before the transition is completed.
 *
 * @param       element_idx Element index of the primary element
 * @param       event The only event that the application should process is @ref WICED_BT_MESH_LIGHT_LIGHTNESS_SET
 * @param       p_data Pointer to the @ref wiced_bt_mesh_light_lightness_status_t structure
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_light_lightness_server_callback_t)(uint8_t element_idx, uint16_t event, void *p_data);

/**
 * \brief Light Lightness Server Module initialization
 * \details A Mesh application which contains Light Lightness Server (for example, an application for a dimmable light)
 * should call this function during application initialization passing element index of the element. If application
 * needs to support multiple dimmable lights, the initialization shall be called multiple times, once per each
 * element.
 *
 * The application shall not call this function if the Light Lightness is not the top level function of the device, for example,
 * when the Light Lightness is included as a part of the HSL Light.
 *
 * The application callback @ref wiced_bt_mesh_light_lightness_server_callback_t will be executed by the Model Library
 * when it is required to set amount of light emitted by the light to a certain level.
 *
 * @param       element_idx Element index where the server is located.
 * @param       p_callback Application or Parent model callback that will be executed to control the device hardware.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the Mesh Models Library deletes this module's configuration data stored in the NVRAM.
 *
 * @return      WICED_TRUE if initialization was successful
 */
void wiced_bt_mesh_model_light_lightness_server_init(uint8_t element_idx, wiced_bt_mesh_light_lightness_server_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Light Lightness Actual changed
 * \details An application shall call this function to notify the Models Library that the value of Lightness has
 * been changed locally.
 *
 * @param       element_idx Element index for which lightness value has changed
 * @param       value New value of lightness
 *
 * @return      None
*/
void wiced_bt_mesh_model_light_lightness_actual_changed(uint8_t element_idx, uint16_t value);

/**
 * \brief Light Lightness Linear changed
 * \details An application shall call this function to notify the Models Library that the value of Lightness has
 * been changed locally.
 *
 * @param       element_idx Element index for which lightness value has changed
 * @param       value New value of lightness
 *
 * @return      None
*/
void wiced_bt_mesh_model_light_lightness_linear_changed(uint8_t element_idx, uint16_t value);

/**
 * \brief Light Lightness Server Message Handler
 * \details The Light Lightness Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The Mesh Core library calls this function for each message received.  The function returns
 * WICED_TRUE if the message is destined for this model, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_light_lightness_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Light Lightness Setup Server Message Handler
 * \details The Light Lightness Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The Mesh Core library calls this function for each message received.  The function returns
 * WICED_TRUE if the message is destined for this model, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_light_lightness_setup_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Light Lightness Server Set Parent
 * \details This is an internal Mesh Models library function and does not need to be implemented or called by the application,
 * unless application wants to replace the library functionality.  In multi server applications, for example a color light which supports
 * both HSL and CTL controls, only one model should receive Get/Set for lightness, level and on/onff messages.
 * This function should be called when the startup controller changes.
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback Application or Parent model callback that will be executed on receiving a message
 *
 * @return      WICED_TRUE if initialization was successful
 */
void wiced_bt_mesh_model_light_lightness_set_parent(uint8_t element_idx, wiced_bt_mesh_light_lightness_server_callback_t *p_callback);

/**
 * \brief Light Lightness Server Scene Store Handler
 * \details This is an internal Mesh Models library function and does not need to be implemented or called by the application,
 * unless application wants to replace the library functionality.  The mesh Scene Server calls this function so that the module
 * can store required data for the scene.
 *
 * @param       element_idx Element index for which scene information is being stored
 * @param       p_buffer Pointer to a buffer where data should be stored
 * @param       buf_size Maximum amount of data a model can store
 *
 * @return      Number of bytes the model wrote into the provided buffer
 */
uint16_t wiced_bt_mesh_model_light_lightness_server_scene_store_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len);

/**
 * \brief Light Lightness Server Scene Restore Handler
 * \details This is an internal Mesh Models library function and does not need to be implemented or called by the application,
 * unless application wants to replace the library functionality.  When Scene Server receives Scene Recall message it calls
 * this function with data previously stored in the NVRAM.
 *
 * @param       element_idx Element index for which scene information is being recalled
 * @param       p_buffer Pointer to a buffer where model's data is located
 * @param       buf_size Maximum amount of data a model can get from the buffer
 * @param       transition_time Specified time in milliseconds to transition to the target state
 * @param       delay Specifies time in milliseconds before starting to transition to the target state
 *
 * @return      Number of bytes the model read from the provided buffer
 */
uint16_t wiced_bt_mesh_model_light_lightness_server_scene_recall_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len, uint32_t transition_time, uint32_t delay);

/* @} wiced_bt_mesh_light_lightness_server */

/**
 * @addtogroup  wiced_bt_mesh_light_lightness_client        Mesh Light Lightness Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Light Lightness Client module of the Mesh Models library provides a way for an application to control
 * brightness level of the Light Lightness Servers in the mesh network optionally receive status information.
 *
 * To include Light Lightness Client functionality, the device description shall contain @ref WICED_BT_MESH_MODEL_LIGHT_LIGHTNESS_CLIENT.
 *
 * @{
 */

#define WICED_BT_MESH_MODEL_LIGHT_LIGHTNESS_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_CLNT, wiced_bt_mesh_model_light_lightness_client_message_handler, NULL, NULL }

/**
 * \brief Light Lightness Client callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref LIGHT_LIGHTNESS_EVENT "Mesh Light Lightness Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the data to send
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_light_lightness_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Light Lightness Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_light_lightness_client_init(uint8_t element_idx, wiced_bt_mesh_light_lightness_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Light Lightness Client Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_light_lightness_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief The application can call this function to send the Light Lightness Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lightness_client_send_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light Lightness Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lightness_client_send_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_lightness_actual_set_t* p_data);

/**
 * \brief The application can call this function to send the Light Lightness Linear Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lightness_client_send_linear_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light Lightness Linear Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lightness_client_send_linear_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_lightness_linear_set_t* p_data);

/**
 * \brief The application can call this function to send the Light Lightness Last Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lightness_client_send_last_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send the Light Lightness Default Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lightness_client_send_default_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light Lightness Default Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lightness_client_send_default_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_lightness_default_data_t* p_data);

/**
 * \brief The application can call this function to send the Light Lightness Range Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lightness_client_send_range_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light Lightness Default Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lightness_client_send_range_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_lightness_range_set_data_t* p_data);

/* @} wiced_bt_mesh_light_lightness_client */

/**
 * @addtogroup  wiced_bt_mesh_light_ctl_server   Mesh Light CTL Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Color Temperature Lightness (CTL) Server module provides a way for an application to implement a dimmable light
 * device with color temperature support. The device can be controlled by setting the brightness, color temperature and delta UV,
 * but also can be can be controlled using a Level slider (Generic Level client) applied to the brightness or color
 * temperature and by a On/Off switch (Generic On/Off client). The device also supports a default transition time
 * server functionality and the application can specify on power up behavior.
 *
 * The definition of the CTL Server spans two elements. To include the Mesh Models support for the CTL Server model the
 * application includes @ref WICED_BT_MESH_MODEL_LIGHT_CTL_SERVER and @ref WICED_BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_SERVER
 * in two consecutive elements in the device definition. If the device supports several
 * lights with CTL Server, for example, a light fixture or a chandelier with several color
 * lights, the pair of @ref WICED_BT_MESH_MODEL_LIGHT_CTL_SERVER and @ref WICED_BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_SERVER
 * shall be repeated for each physical device.
 *
 * In addition to the definition, the application shall initialize the CTL Server module by calling
 * @ref wiced_bt_mesh_model_light_ctl_server_init function for each primary element where the CTL
 * functionality is required providing the @ref wiced_bt_mesh_light_ctl_server_callback_t
 * callback that will be executed when the brightness level or the color temperature of the device needs to be changed.
 *
 * If the brightness or color temperature of the device is changed locally the application should call the
 * @ref wiced_bt_mesh_model_light_ctl_status_changed so that the library can maintain correct state and
 * send an appropriate status message if it is configured to do so.
 *
 * Most of the functionality of the Light CTL server device is implemented in the Mesh Models Library. The
 * application only needs to set hardware appropriately on receiving @ref WICED_BT_MESH_LIGHT_CTL_SET event.
 * If client requests to change the brightness or the color temperature to the target value over a period of time,
 * the library will maintain the timer and will periodically call the application with appropriate values.
 *
 * The default transition time is managed by the Mesh Models library. The application provides factory default
 * in the default_transition_time element of the @ref wiced_bt_mesh_core_config_t configuration structure.
 * The default value is used unless overwritten by the default transition time client.
 *
 * The on power up behavior is managed by the Mesh Models library. The application provides factory default
 * in the onpowerup_state element of the @ref wiced_bt_mesh_core_config_t configuration structure.
 * The default value is used unless overwritten by the default transition time client.
 *
 * @{
 */

/**
 * Mesh Light CTL Server Device.
 * The device implements all required models for the two elements of a color bulb that supports
 * brightness and color temperature control. The WICED_BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_SERVER
 * should be defined in the element with the higher index than one with WICED_BT_MESH_MODEL_LIGHT_CTL_SERVER,
 * but before the next primary element of the device.  For example, if application supports two CTL
 * lights, the consecutive elements should be WICED_BT_MESH_MODEL_LIGHT_CTL_SERVER,
 * WICED_BT_MESH_MODEL_LIGHT_CTL_SERVER, WICED_BT_MESH_MODEL_LIGHT_CTL_SERVER, WICED_BT_MESH_MODEL_LIGHT_CTL_SERVER.
 * The time and scheduler models are optional and can be commented out to reduce memory footprint.
 */
#ifdef TIME_AND_SCHEDULER_SUPPORT
#define WICED_BT_MESH_MODEL_LIGHT_CTL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SRV, wiced_bt_mesh_model_scheduler_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SETUP_SRV, wiced_bt_mesh_model_scheduler_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SRV, wiced_bt_mesh_model_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SETUP_SRV, wiced_bt_mesh_model_time_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SRV, wiced_bt_mesh_model_light_lightness_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV,  wiced_bt_mesh_model_light_lightness_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SRV, wiced_bt_mesh_model_light_ctl_server_message_handler, wiced_bt_mesh_model_light_ctl_server_scene_store_handler, wiced_bt_mesh_model_light_ctl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SETUP_SRV, wiced_bt_mesh_model_light_ctl_setup_server_message_handler, NULL, NULL  }
#else
#define WICED_BT_MESH_MODEL_LIGHT_CTL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SRV, wiced_bt_mesh_model_light_lightness_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV,  wiced_bt_mesh_model_light_lightness_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SRV, wiced_bt_mesh_model_light_ctl_server_message_handler, wiced_bt_mesh_model_light_ctl_server_scene_store_handler, wiced_bt_mesh_model_light_ctl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_SETUP_SRV, wiced_bt_mesh_model_light_ctl_setup_server_message_handler, NULL, NULL  }
#endif

#define WICED_BT_MESH_MODEL_LIGHT_CTL_TEMPERATURE_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_TEMPERATURE_SRV, wiced_bt_mesh_model_light_ctl_temperature_server_message_handler, wiced_bt_mesh_model_light_ctl_temperature_server_scene_store_handler, wiced_bt_mesh_model_light_ctl_temperature_server_scene_recall_handler }

 /**
 * \brief Light CTL Server callback
 * \details Light CTL Server callback is executed by the Mesh Models library when the application is required to
 * adjust the amount of light emitted by the device or its color. The application is responsible for setting the hardware
 * (for example, by controlling PWM) to comply with the values requested by the library.
 *
 * If peer device requested to transition to the new brightness level over a period of time, the callback will be
 * executed repeatedly.  The Mesh Models Library calculates the present lightness level, color temperature and
 * delta UV and submits to the application to setup the hardware.
 *
 * The p_data parameter points to the @ref wiced_bt_mesh_light_ctl_status_data_t structure. Typically the application
 * will only need to use the value of the present lightness, temperature and delta_uv fields which specify the target
 * values that the device shall be set at present time. If the remaining_time field is not zero,
 * the transition to the new target level is in progress and the Mesh Models Library will execute the callback at
 * least once more before the transition is completed.
 *
 * @param       element_idx Element Index of the model
 * @param       event The only event that the application should process is @ref WICED_BT_MESH_LIGHT_CTL_SET
 * @param       p_data Pointer to the @ref wiced_bt_mesh_light_ctl_status_data_t structure
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_light_ctl_server_callback_t)(uint8_t element_idx, uint16_t event, void *p_data);

/**
 * \brief Light CTL Server Module initialization
 * \details A Mesh application which contains Light CTL Server (for example, a dimmable bulb with color temperature control)
 * should call this function during application initialization. The CTL functionality is split into two elements, the
 * lightness and the color temperature.  The initialization function should use the index of the primary - lightness element.
 * If the application needs to support multiple CTL lights (for example, a light fixture or a chandelier),
 * the function shall be called multiple times, once per each primary element.
 *
 * @param       lightness_element_idx Element index where lightness part of the CTL is located
 * @param       p_callback Application callback that will be executed to set device hardware to required brightness, color temperature, delta UV values.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      WICED_TRUE if initialization was successful
 */
void wiced_bt_mesh_model_light_ctl_server_init(uint8_t lightness_element_idx, wiced_bt_mesh_light_ctl_server_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Light CTL state changed
 * \details The application shall call the Light CTL state changed function to notify the Mesh Models
 * library when values brightness, color temperature or delta UV are changed internally.
 *
 * @param       element_idx Element index for which scene information is being recalled
 * @param       p_data Pointer to the @ref wiced_bt_mesh_light_ctl_status_data_t status data
 *
 * @return      None
 */
void wiced_bt_mesh_model_light_ctl_server_state_changed(uint8_t element_idx, wiced_bt_mesh_light_ctl_status_data_t *p_data);

/**
 * \brief Light CTL Server Message Handler
 * \details The Light CTL Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The Mesh Core library calls this function for each message received.  The function shall return WICED_TRUE if the message is
 * destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_light_ctl_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Light CTL Setup Server Message Handler
 * \details The Light CTL Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The Mesh Core library calls this function for each message received.  The function shall return WICED_TRUE if the message is
 * destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_light_ctl_setup_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Light CTL Temperature Server Message Handler
 * \details The Light CTL Temperature Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The Mesh Core library calls this function for each message received.  The function shall return WICED_TRUE if the message is
 * destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_light_ctl_temperature_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);


/**
 * \brief Light CTL Server Scene Store Handler
 * \details The Light CTL Scene Store Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The mesh Scene Server calls this function so that the module can store required data for the scene.
 *
 * @param       element_idx Element index for which scene information is being stored
 * @param       p_buffer Pointer to a buffer where data should be stored
 * @param       buf_size Maximum amount of data a model can store
 *
 * @return      Number of bytes the model wrote into the provided buffer
 */
uint16_t wiced_bt_mesh_model_light_ctl_server_scene_store_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len);

/**
 * \brief Light CTL Temperature Server Scene Store Handler
 * \details The Light CTL Scene Store Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The mesh Scene Server calls this function so that the module can store required data for the scene.
 * Note that if Scene Server is not present on CTL Temperature Server, this function will be called by the internally by the
 * CTL Server
 *
 * @param       element_idx Element index for which scene information is being stored
 * @param       p_buffer Pointer to a buffer where data should be stored
 * @param       buf_size Maximum amount of data a model can store
 *
 * @return      Number of bytes the model wrote into the provided buffer
 */
uint16_t wiced_bt_mesh_model_light_ctl_temperature_server_scene_store_handler(uint8_t element_idx, uint8_t* p_buffer, uint16_t buffer_len);

/**
 * \brief Light CTL Server Scene Restore Handler
 * \details The Light CTL Scene Restore Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * When Scene Server receives Scene Recall message it calls this function with data previously stored in the NVRAM.
 *
 * @param       element_idx Element index for which scene information is being recalled
 * @param       p_buffer Pointer to a buffer where model's data is located
 * @param       buf_size Maximum amount of data a model can get from the buffer
 * @param       transition_time Specified time in milliseconds to transition to the target state
 * @param       delay Specifies time in milliseconds before starting to transition to the target state
 *
 * @return      Number of bytes the model read from the provided buffer
 */
uint16_t wiced_bt_mesh_model_light_ctl_server_scene_recall_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len, uint32_t transition_time, uint32_t delay);

/**
 * \brief Light Temperature Server Scene Restore Handler
 * \details The Light Temperature Scene Restore Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * When Scene Server receives Scene Recall message it calls this function with data previously stored in the NVRAM.
 * Note that if SCENE_SERVER is not implemented on the Light Temperature element, this function will be call internally by the
 * parent Light Ctl Server
 *
 * @param       element_idx Element index for which scene information is being recalled
 * @param       p_buffer Pointer to a buffer where model's data is located
 * @param       buf_size Maximum amount of data a model can get from the buffer
 * @param       transition_time Specified time in milliseconds to transition to the target state
 * @param       delay Specifies time in milliseconds before starting to transition to the target state
 *
 * @return      Number of bytes the model read from the provided buffer
 */
uint16_t wiced_bt_mesh_model_light_ctl_temperature_server_scene_recall_handler(uint8_t element_idx, uint8_t* p_buffer, uint16_t buffer_len, uint32_t transition_time, uint32_t delay);

/**
 * \brief Set Light CTL Model as a startup controller
 * \details The Light CTL Scene Restore Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * For multi-model applications, for example, when application supports both HSL and CTL controls, only one can be setup as a startup controller.
 * If the controller mode is set, the library will execute the power on requirements, for example it can set the temperature, lightness
 * based on the last set state.  If the model is not set as a controller, it will not execute on power up state.
 *
 * @param       element_idx The element index of the top level model
 * @param       startup_controller If set to WICED_TRUE, the model will execute On Power Up operation
 *
 * @return      None
 */
void wiced_bt_mesh_model_light_ctl_server_set_startup_controller(uint8_t element_idx, wiced_bool_t startup_controller);

/* @} wiced_bt_mesh_light_ctl_server */

/**
 * @addtogroup  wiced_bt_mesh_light_ctl_client        Mesh Light CTL Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Light CTL Client module of the Mesh Models library provides a way for an application to configure and control
 * color lights that support Color Temperature adjustments.
 *
 * To include Light CTL Client functionality, the device description shall contain @ref WICED_BT_MESH_MODEL_LIGHT_CTL_CLIENT.
 *
 * @{
 */
#define WICED_BT_MESH_MODEL_LIGHT_CTL_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_CTL_CLNT, wiced_bt_mesh_model_light_ctl_client_message_handler, NULL, NULL }

/**
 * \brief Light CTL Client callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref LIGHT_CTL_EVENT "Light CTL Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the level data
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_light_ctl_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Light CTL Client Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_light_ctl_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Light CTL Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_light_ctl_client_init(uint8_t element_idx, wiced_bt_mesh_light_ctl_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief The application can call this function to send Light CTL Get message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_ctl_client_send_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light CTL Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_ctl_client_send_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_ctl_set_t* p_data);

/**
 * \brief The application can call this function to send Light CTL Temperature Get message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_ctl_client_send_temperature_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light CTL Temperature Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_ctl_client_send_temperature_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_ctl_temperature_set_t* p_data);

/**
 * \brief The application can call this function to send Light CTL Get Default client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_ctl_client_send_default_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light CTL Set Default  client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_ctl_client_send_default_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_ctl_default_data_t* p_data);

/**
 * \brief The application can call this function to send Light CTL Temperature Range Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_ctl_client_send_temperature_range_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light CTL Temperature Range Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_ctl_client_send_temperature_range_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_ctl_temperature_range_data_t* p_data);

/* @} wiced_bt_mesh_light_ctl_client */

/**
 * @addtogroup  wiced_bt_mesh_light_hsl_server   Mesh Light HSL Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Hue Saturation Lightness (HSL) Server module provides a way for an application to implement a dimmable light
 * device with hue/saturation support. The device can be controlled by a device with rich UI which can control the brightness,
 * hue and saturation, but also the device can be can be controlled using a Level sliders (Generic Level client) applied to the
 * brightness, hue or saturation and by a On/Off switch (Generic On/Off client). The device also supports a default transition time
 * server functionality and the application can specify on power up behavior.
 *
 * The definition of the HSL Server spans three elements. To include the Mesh Models support for the HSL Server module the
 * application includes the @ref WICED_BT_MESH_MODEL_LIGHT_HSL_SERVER, the @ref WICED_BT_MESH_MODEL_LIGHT_HSL_HUE_SERVER, and
 * the @ref WICED_BT_MESH_MODEL_LIGHT_HSL_SATURATION_SERVER in three consecutive elements in the device definition.
 * If the device supports several lights with HSL Server support, for example, a light fixture or a chandelier with several color
 * lights, the triplet of @ref WICED_BT_MESH_MODEL_LIGHT_HSL_SERVER, @ref WICED_BT_MESH_MODEL_LIGHT_HSL_HUE_SERVER and
 * @ref WICED_BT_MESH_MODEL_LIGHT_HSL_SATURATION_SERVER shall be repeated for each physical device.
 *
 * In addition to the definition, the application shall initialize the HSL Server module by calling
 * @ref wiced_bt_mesh_model_light_hsl_server_init function for each primary element where the HSL
 * functionality is required providing the @ref wiced_bt_mesh_light_hsl_server_callback_t
 * callback that will be executed when the brightness level, the hue or the saturation of the device needs to be changed.
 *
 * If device supports local control, for example, when the brightness of the device can be changed locally,
 * the application should call the @ref wiced_bt_mesh_model_light_hsl_status_changed function so that the library can
 * maintain correct state and send an appropriate status message if it is configured to do so.
 *
 * Most of the functionality of the Light HSL server device is implemented in the Mesh Models Library. The
 * application only needs to set hardware appropriately on receiving @ref WICED_BT_MESH_LIGHT_HSL_SET event.
 * If a client requests to change the brightness or the color to the target values over a period of time,
 * the library will maintain the timer and will periodically call the application with appropriate values.
 *
 * The default transition time is managed by the Mesh Models library. The application provides factory default
 * in the default_transition_time element of the @ref wiced_bt_mesh_core_config_t configuration structure.
 * The default value is used unless overwritten by the default transition time client.
 *
 * The on power up behavior is managed by the Mesh Models library. The application provides factory default
 * in the onpowerup_state element of the @ref wiced_bt_mesh_core_config_t configuration structure.
 * The default value is used unless overwritten by the default transition time client.
 *
 * @{
 */

/**
 * \brief Mesh Light HSL Server device definition.
 * \details The Mesh Light HSL Server device implements all required models for the primary, hue and saturation
 * elements of a color bulb that supports brightness, hue and saturation control.  The WICED_BT_MESH_MODEL_LIGHT_HSL_HUE_SERVER
 * and WICED_BT_MESH_MODEL_LIGHT_HSL_SATURATION_SERVER should be defined in the element with the higher index than one with
 * WICED_BT_MESH_MODEL_LIGHT_HSL_SERVER, but before the next primary element of the device.  For example, if application
 * supports one HSL and one dimmable light, the consecutive elements should be WICED_BT_MESH_MODEL_LIGHT_HSL_SERVER,
 * WICED_BT_MESH_MODEL_LIGHT_HSL_HUE_SERVER, WICED_BT_MESH_MODEL_LIGHT_HSL_SATURATION_SERVER,
 * WICED_BT_MESH_MODEL_LIGHT_LIGHTNESS_SERVER.
 * The time and scheduler models are optional and can be commented out to reduce memory footprint.
 */
#ifdef TIME_AND_SCHEDULER_SUPPORT
#define WICED_BT_MESH_MODEL_LIGHT_HSL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SRV, wiced_bt_mesh_model_scheduler_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SETUP_SRV, wiced_bt_mesh_model_scheduler_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SRV, wiced_bt_mesh_model_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SETUP_SRV, wiced_bt_mesh_model_time_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SRV, wiced_bt_mesh_model_light_lightness_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV,  wiced_bt_mesh_model_light_lightness_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SRV, wiced_bt_mesh_model_light_hsl_server_message_handler, wiced_bt_mesh_model_light_hsl_server_scene_store_handler, wiced_bt_mesh_model_light_hsl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SETUP_SRV, wiced_bt_mesh_model_light_hsl_setup_server_message_handler, NULL, NULL }
#else
#define WICED_BT_MESH_MODEL_LIGHT_HSL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SRV, wiced_bt_mesh_model_light_lightness_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV,  wiced_bt_mesh_model_light_lightness_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SRV, wiced_bt_mesh_model_light_hsl_server_message_handler, wiced_bt_mesh_model_light_hsl_server_scene_store_handler, wiced_bt_mesh_model_light_hsl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SETUP_SRV, wiced_bt_mesh_model_light_hsl_setup_server_message_handler, NULL, NULL }
#endif

#define WICED_BT_MESH_MODEL_LIGHT_HSL_HUE_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_HUE_SRV, wiced_bt_mesh_model_light_hsl_hue_server_message_handler, NULL, NULL }

#define WICED_BT_MESH_MODEL_LIGHT_HSL_SATURATION_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_SATURATION_SRV, wiced_bt_mesh_model_light_hsl_saturation_server_message_handler, NULL, NULL }

/**
 * \brief Light HSL Server callback
 * \details Light HSL Server callback is executed by the Mesh Models library when the application is required to
 * adjust the amount of light emitted by the device or its color. The application is responsible for setting the hardware
 * (for example, by controlling PWM) to comply with the values requested by the library.
 *
 * If peer device requested to transition to the new brightness, hue or saturation level over a period of time, the
 * callback will be executed repeatedly.  The Mesh Models Library calculates the present levels and submits to the
 * application to setup the hardware.
 *
 * The p_data parameter points to the @ref wiced_bt_mesh_light_hsl_status_data_t structure. Typically the application
 * will only need to use the value of the present lightness, hue and saturation fields which specify the target
 * values that the device shall be set at present time. If the remaining_time field is not zero,
 * the transition to the new target levels is in progress and the Mesh Models Library will execute the callback at
 * least once more before the transition is completed.
 *
 * @param       element_idx Element index of the primary element
 * @param       event The only event that the application should process is @ref WICED_BT_MESH_LIGHT_HSL_SET
 * @param       p_data Pointer to the @ref wiced_bt_mesh_light_hsl_status_data_t structure
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_light_hsl_server_callback_t)(uint8_t element_idx, uint16_t event, void *p_data);

/**
 * \brief Light HSL Server Module initialization
 * \details A Mesh application which contains Light HSL Server (for example, a dimmable color bulb)
 * should call this function during application initialization passing element index of the primary element.
 * The functionality of the HSL Server is split into three elements, the lightness, the hue and the
 * saturation.  The initialization function should use the index of the primary - lightness element.
 * If the application needs to support multiple HSL lights (for example, a light fixture or a chandelier),
 * the function shall be called multiple times, once per each primary element.
 *
 * @param       lightness_element_idx Element index where lightness part of the HSL is located.
 * @param       p_callback Application callback that will be executed to set device hardware to required brightness, hue, saturation values.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      WICED_TRUE if initialization was successful
 */
void wiced_bt_mesh_model_light_hsl_server_init(uint8_t lightness_element_idx, wiced_bt_mesh_light_hsl_server_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Light HSL state changed
 * \details The application shall call the Light HSL state changed function to notify the Mesh Models
 * library when values brightness, hue or saturation are changed internally.
 *
 * @param       element_idx Element index for the lightness part of the HSL device
 * @param       p_data Pointer to the @ref wiced_bt_mesh_light_hsl_status_data_t status data
 *
 * @return      None
 */
void wiced_bt_mesh_model_light_hsl_server_state_changed(uint8_t element_idx, wiced_bt_mesh_light_hsl_status_data_t *p_data);

/**
 * \brief Light HSL Server Message Handler
 * \details The Light HSL Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The Mesh Core library calls this function for each message received.  The function shall return WICED_TRUE if the message is
 * destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_light_hsl_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Light HSL Setup Server Message Handler
 * \details The Light HSL Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The Mesh Core library calls this function for each message received.  The function shall return WICED_TRUE if the message is
 * destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_light_hsl_setup_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Light HSL Hue Server Message Handler
 * \details The Light HSL Hue Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined
 * for this specific model, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_light_hsl_hue_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Light HSL Saturation Server Message Handler
 * \details The Light HSL Saturation Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined
 * for this specific model, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_light_hsl_saturation_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Light HSL Server Scene Store Handler
 * \details The Light HSL Server Store Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The mesh Scene Server calls this function so that the module can store required data for the scene.
 *
 * @param       element_idx Element index for which scene information is being stored
 * @param       p_buffer Pointer to a buffer where data should be stored
 * @param       buf_size Maximum amount of data a model can store
 *
 * @return      Number of bytes the model wrote into the provided buffer
 */
uint16_t wiced_bt_mesh_model_light_hsl_server_scene_store_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len);

/**
 * \brief Light HSL Server Scene Recall Handler
 * \details The Light HSL Server Recall Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * When Scene Server receives Scene Recall message it calls this function with data previously stored in the NVRAM.
 *
 * @param       element_idx Element index for which scene information is being recalled
 * @param       p_buffer Pointer to a buffer where model's data is located
 * @param       buf_size Maximum amount of data a model can get from the buffer
 * @param       transition_time Specified time in milliseconds to transition to the target state
 * @param       delay Specifies time in milliseconds before starting to transition to the target state
 *
 * @return      Number of bytes the model read from the provided buffer
 */
uint16_t wiced_bt_mesh_model_light_hsl_server_scene_recall_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len, uint32_t transition_time, uint32_t delay);

/**
 * \brief Set Light HSL Model as a startup controller
 * \details This is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * For multi-model applications, for example the ones that support both HSL and CTL controls, only one can be setup as a startup controller.
 * If the controller mode is set, the library will execute the power on requirements, for example it can set the lightness/hue/saturation
 * based on the last state before the power off.  If the model is not set as a controller, it will not execute on power up state.
 *
 * @param       element_idx The element index of the top level model
 * @param       startup_controller If set to WICED_TRUE, the model will execute on power up operation
 *
 * @return      None
 */
void wiced_bt_mesh_model_light_hsl_server_set_startup_controller(uint8_t element_idx, wiced_bool_t startup_controller);

/* @} wiced_bt_mesh_light_hsl_server */

/**
 * @addtogroup  wiced_bt_mesh_light_hsl_client        Mesh Light HSL Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Light HSL Client module of the Mesh Models library provides a way for an application to configure and control
 * color lights that support Hue/Saturation control.
 *
 * To include Light HSL Client functionality, the device description shall contain @ref WICED_BT_MESH_MODEL_LIGHT_HSL_CLIENT.
 *
 * @{
 */

#define WICED_BT_MESH_MODEL_LIGHT_HSL_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_HSL_CLNT, wiced_bt_mesh_model_light_hsl_client_message_handler, NULL, NULL }

/**
 * \brief Light HSL Client callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref LIGHT_HSL_EVENT "Light HSL Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the level data
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_light_hsl_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

 /**
 * \brief Light HSL Client Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_light_hsl_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

 /**
 * \brief Light HSL Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_light_hsl_client_init(uint8_t element_idx, wiced_bt_mesh_light_hsl_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief The application can call this function to send Light HSL Get message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_hsl_client_send_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light HSL Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_hsl_client_send_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_hsl_set_t* p_data);

/**
 * \brief The application can call this function to send Light HSL Hue Get message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_hsl_client_send_hue_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light HSL Hue Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_hsl_client_send_hue_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_hsl_hue_set_t* p_data);

/**
 * \brief The application can call this function to send Light HSL Saturation Get message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_hsl_client_send_saturation_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light HSL Saturation Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_hsl_client_send_saturation_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_hsl_saturation_set_t* p_data);

/**
 * \brief The application can call this function to send Light HSL Target Get message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_hsl_client_send_target_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light HSL Get Default client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_hsl_client_send_default_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light HSL Set Default client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_hsl_client_send_default_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_hsl_default_data_t* p_data);

 /**
 * \brief The application can call this function to send Light HSL Range Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_hsl_client_send_range_get(wiced_bt_mesh_event_t *p_event);

 /**
 * \brief The application can call this function to send Light HSL Range Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_hsl_client_send_range_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_hsl_range_set_data_t* p_data);

/* @} wiced_bt_mesh_light_hsl_client */

/**
 * @addtogroup  wiced_bt_mesh_light_xyl_server   Mesh Light xyL Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Light xyL Server module provides a way for an application to implement a dimmable light
 * device with x and y chromaticity coordinates support. The device can be controlled by a device with rich UI which can control the brightness,
 * x and y states, but also the device can be can be controlled using a Level sliders (Generic Level client) applied to the
 * brightness and by a On/Off switch (Generic On/Off client). The device also supports a default transition time
 * server functionality and the application can specify on power up behavior.
 *
 * To include the Mesh Models support for the XYL Server module the application includes the @ref WICED_BT_MESH_MODEL_LIGHT_XYL_SERVER
 * in the device definition. If the device supports several lights with XYL Server support, for example, a light fixture or a chandelier with several color
 * lights, the @ref WICED_BT_MESH_MODEL_LIGHT_XYL_SERVER, shall be repeated for each physical device.
 *
 * In addition to the definition, the application shall initialize the XYL Server module by calling
 * @ref wiced_bt_mesh_model_light_xyl_server_init function for each primary element where the XYL
 * functionality is required providing the @ref wiced_bt_mesh_light_xyl_server_callback_t
 * callback that will be executed when the brightness level, the hue or the saturation of the device needs to be changed.
 *
 * If device supports local control, for example, when the brightness of the device can be changed locally,
 * the application should call the @ref wiced_bt_mesh_model_light_xyl_status_changed function so that the library can
 * maintain correct state and send an appropriate status message if it is configured to do so.
 *
 * Most of the functionality of the Light XYL server device is implemented in the Mesh Models Library. The
 * application only needs to set hardware appropriately on receiving @ref WICED_BT_MESH_LIGHT_XYL_SET event.
 * If a client requests to change the brightness or the color to the target values over a period of time,
 * the library will maintain the timer and will periodically call the application with appropriate values.
 *
 * The default transition time is managed by the Mesh Models library. The application provides factory default
 * in the default_transition_time element of the @ref wiced_bt_mesh_core_config_t configuration structure.
 * The default value is used unless overwritten by the default transition time client.
 *
 * The on power up behavior is managed by the Mesh Models library. The application provides factory default
 * in the onpowerup_state element of the @ref wiced_bt_mesh_core_config_t configuration structure.
 * The default value is used unless overwritten by the default transition time client.
 *
 * @{
 */

/**
 * Mesh Light XYL Server Device.
 * The device implements all required models for the main element of a color bulb that supports
 * brightness and xyL color control.
 * The time and scheduler models are optional and can be commented out to reduce memory footprint.
 */
#ifdef TIME_AND_SCHEDULER_SUPPORT
#define WICED_BT_MESH_MODEL_LIGHT_XYL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SRV, wiced_bt_mesh_model_scheduler_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCHEDULER_SETUP_SRV, wiced_bt_mesh_model_scheduler_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SRV, wiced_bt_mesh_model_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_TIME_SETUP_SRV, wiced_bt_mesh_model_time_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SRV, wiced_bt_mesh_model_light_lightness_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV,  wiced_bt_mesh_model_light_lightness_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_XYL_SRV, wiced_bt_mesh_model_light_xyl_server_message_handler, wiced_bt_mesh_model_light_xyl_server_scene_store_handler, wiced_bt_mesh_model_light_xyl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_XYL_SETUP_SRV, wiced_bt_mesh_model_light_xyl_setup_server_message_handler, NULL, NULL }
#else
#define WICED_BT_MESH_MODEL_LIGHT_XYL_SERVER \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_DEFTT_SRV, wiced_bt_mesh_model_default_transition_time_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SRV, wiced_bt_mesh_model_scene_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_SCENE_SETUP_SRV, wiced_bt_mesh_model_scene_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_ONOFF_SRV, wiced_bt_mesh_model_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SRV, wiced_bt_mesh_model_power_onoff_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_POWER_ONOFF_SETUP_SRV, wiced_bt_mesh_model_power_onoff_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_GENERIC_LEVEL_SRV, wiced_bt_mesh_model_level_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SRV, wiced_bt_mesh_model_light_lightness_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV,  wiced_bt_mesh_model_light_lightness_setup_server_message_handler, NULL, NULL }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_XYL_SRV, wiced_bt_mesh_model_light_xyl_server_message_handler, wiced_bt_mesh_model_light_xyl_server_scene_store_handler, wiced_bt_mesh_model_light_xyl_server_scene_recall_handler }, \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_XYL_SETUP_SRV, wiced_bt_mesh_model_light_xyl_setup_server_message_handler, NULL, NULL }
#endif

/**
 * \brief Light XYL Server callback
 * \details Light XYL Server callback is executed by the Mesh Models library when the application is required to
 * adjust the amount of light emitted by the device or its color. The application is responsible for setting the hardware
 * (for example, by controlling PWM) to comply with the values requested by the library.
 *
 * If peer device requested to transition to the new brightness, x or y states over a period of time, the
 * callback will be executed repeatedly.  The Mesh Models Library calculates the present levels and submits to the
 * application to setup the hardware.
 *
 * The p_data parameter points to the @ref wiced_bt_mesh_light_xyl_status_data_t structure. Typically the application
 * will only need to use the value of the present lightness, x and y fields which specify the target
 * values that the device shall be set at present time. If the remaining_time field is not zero,
 * the transition to the new target levels is in progress and the Mesh Models Library will execute the callback at
 * least once more before the transition is completed.
 *
 * @param       element_idx Element index of the element
 * @param       event The only event that the application should process is @ref WICED_BT_MESH_LIGHT_XYL_SET
 * @param       p_data Pointer to the @ref wiced_bt_mesh_light_xyl_status_data_t structure
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_light_xyl_server_callback_t)(uint8_t element_idx, uint16_t event, void *p_data);

/**
 * \brief Light XYL Module initialization
 * \details A Mesh application which contains Light XYL Server (for example, a dimmable color bulb)
 * should call this function during application initialization passing element index of the element.
 * If the application needs to support multiple XYL lights (for example, a light fixture or a chandelier),
 * the function shall be called multiple times, once per each primary element.
 *
 * @param       element_idx Element index corresponding to XYL device
 * @param       p_callback Application callback that will be executed to set device hardware to required brightness, x and y values.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the device is not provision and NVRAM is erased.
 *
 * @return      WICED_TRUE if initialization was successful
 */
void wiced_bt_mesh_model_light_xyl_server_init(uint8_t element_idx, wiced_bt_mesh_light_xyl_server_callback_t *p_callback, wiced_bt_mesh_light_xyl_xy_settings_t *p_xy_settings, wiced_bool_t is_provisioned);

/**
 * \brief Light XYL state changed
 * \details The application shall call the Light XYL state changed function to notify the Mesh Models
 * library when values of brightness, x or y are changed internally.
 *
 * @param       element_idx Element index corresponding to the XYL device
 * @param       p_data Pointer to the @ref wiced_bt_mesh_light_xyl_status_data_t status data
 *
 * @return      None
 */
void wiced_bt_mesh_model_light_xyl_server_state_changed(uint8_t element_idx, wiced_bt_mesh_light_xyl_status_data_t *p_data);

/**
 * \brief Light XYL Server Message Handler
 * \details The Light XYL Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The Mesh Core library calls this function for each message received.  The function shall return WICED_TRUE if the message is
 * destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_light_xyl_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Light XYL Setup Server Message Handler
 * \details The Light XYL Server Setup Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The Mesh Core library calls this function for each message received.  The function shall return WICED_TRUE if the message is
 * destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_light_xyl_setup_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Light xyL Server Scene Store Handler
 * \details The Light HSL Saturation Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * The mesh Scene Server calls this function so that the module can store required data for the scene.
 *
 * @param       element_idx Element index for which scene information is being stored
 * @param       p_buffer Pointer to a buffer where data should be stored
 * @param       buf_size Maximum amount of data a model can store
 *
 * @return      Number of bytes the model wrote into the provided buffer
 */
uint16_t wiced_bt_mesh_model_light_xyl_server_scene_store_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len);

/**
 * \brief Light xyL Server Scene Store Handler
 * \details The Light HSL Saturation Server Message Handler is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * When Scene Server receives Scene Store message it calls this function with data previously stored in the NVRAM.
 *
 * @param       element_idx Element index for which scene information is being recalled
 * @param       p_buffer Pointer to a buffer where model's data is located
 * @param       buf_size Maximum amount of data a model can get from the buffer
 * @param       transition_time Specified time in milliseconds to transition to the target state
 * @param       delay Specifies time in milliseconds before starting to transition to the target state
 *
 * @return      Number of bytes the model read from the provided buffer
 */
uint16_t wiced_bt_mesh_model_light_xyl_server_scene_recall_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len, uint32_t transition_time, uint32_t delay);

/**
 * \brief Set Light XYL Model as a startup controller
 * \details This is an internal Mesh Models library function and
 * does not need to be implemented or called by the application, unless application wants to replace the library functionality.
 * For multi-model applications, for example the ones that support HSL, CTL and XYL controls, only one can be setup as a startup controller.
 * If the controller mode is set, the library will execute the power on requirements, for example it can set the lightness/hue/saturation
 * based on the last state before the power off.  If the model is not set as a controller, it will not execute on power up state.
 *
 * @param       element_idx The element index of the top level model
 * @param       startup_controller If set to WICED_TRUE, the model will execute on power up operation
 *
 * @return      None
 */
void wiced_bt_mesh_model_light_xyl_server_set_startup_controller(uint8_t element_idx, wiced_bool_t startup_controller);

/* @} wiced_bt_mesh_light_xyl_server */

/**
 * @addtogroup  wiced_bt_mesh_light_xyl_client        Mesh Light xyL Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Light XYL Client module of the Mesh Models library provides a way for an application to configure and control
 * color lights that support xyL control.
 *
 * To include Light XYL Client functionality, the device description shall contain @ref WICED_BT_MESH_MODEL_LIGHT_XYL_CLIENT.
 *
 * @{
 */

#define WICED_BT_MESH_MODEL_LIGHT_XYL_CLIENT \
    { MESH_COMPANY_ID_BT_SIG, WICED_BT_MESH_CORE_MODEL_ID_LIGHT_XYL_CLNT, wiced_bt_mesh_model_light_xyl_client_message_handler, NULL, NULL }

/**
 * \brief Light xyL Client callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref LIGHT_XYL_EVENT "Light xyL Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the level data
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_light_xyl_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Light xyL Client Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
 */
wiced_bool_t wiced_bt_mesh_model_light_xyl_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

 /**
 * \brief Light xyL Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_light_xyl_client_init(uint8_t element_idx, wiced_bt_mesh_light_xyl_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief The application can call this function to send Light xyL Get message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_xyl_client_send_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light xyL Set message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_xyl_client_send_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_xyl_set_t *p_data);

/**
 * \brief The application can call this function to send Light xyL Target Get message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_xyl_client_send_target_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light xyL Get Default client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_xyl_client_send_default_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light xyL Set Default client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_xyl_client_send_default_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_xyl_default_data_t* p_data);

/**
 * \brief The application can call this function to send Light xyL Range Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_xyl_client_send_range_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light xyL Range Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_xyl_client_send_range_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_xyl_range_set_data_t* p_data);

/* @} wiced_bt_mesh_light_xyl_client */

/**
 * @addtogroup  wiced_bt_mesh_model_light_lc_server Mesh Light LC Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Light LC Server module provides a way for an application to implement a Light Control Server
 * device which can be a client to a Sensor Server model and controls the Light Lightness Actual state via defined state bindings.
 *
 * To include the Mesh Models support for the LC Server module the application shall contain the @ref WICED_BT_MESH_MODEL_LIGHT_LC_SERVER
 *
 * TBD
 *
 @{
 */

/**
 * \brief Light LC Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_light_lc_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Light LC Setup Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_light_lc_setup_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief An application or a a parent model can call this function to send Light Lightness Status message to the Client. The mesh event should
 * contain information if this is a reply or an unsolicited message.
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_data Pointer to the status data
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lc_server_send_status(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_lightness_status_data_t *p_data);

/**
 * \brief Light LC Server Mode Changed
 * \details Application should call this function when LC Mode is changed locally
 *
 * @param       element_idx Element index where the LC Model is located
 * @param       p_status Pointer to the structure with the new mode
 *
 * @return      None
*/
void wiced_bt_mesh_model_light_lc_mode_changed(uint8_t element_idx, wiced_bt_mesh_light_lc_mode_set_data_t *p_status);

/**
 * \brief Light LC Server Occupancy Mode Changed
 * \details Application should call this function when LC Occupancy Mode is changed locally
 *
 * @param       element_idx Element index where the LC Model is located
 * @param       p_status Pointer to the structure with the new mode
 *
 * @return      None
*/
void wiced_bt_mesh_model_light_lc_occupancy_mode_changed(uint8_t element_idx, wiced_bt_mesh_light_lc_occupancy_mode_set_data_t *p_status);

/**
 * \brief Light LC Server Occupancy Detected
 * \details Application should call this function when LC Occupancy is detected locally
 *
 * @param       element_idx Element index where the LC Model is located
 *
 * @return      None
*/
void wiced_bt_mesh_model_light_lc_occupancy_detected(uint8_t element_idx);

/**
 * \brief Light LC Server OnOff Changed
 * \details Application should call this function when LC Light OnOff is changed locally
 *
 * @param       element_idx Element index where the LC Model is located
 * @param       p_status Pointer to the structure with the new Light OnOff State
 *
 * @return      None
*/
void wiced_bt_mesh_model_light_lc_onoff_changed(uint8_t element_idx, wiced_bt_mesh_onoff_set_data_t *p_status);

/**
 * \brief Light LC Property Changed
 * \details Application should call this function when an LC LC property is changed locally
 *
 * @param       element_idx Element index where the LC Model is located
 *
 * @return      None
*/
void wiced_bt_mesh_model_light_lc_property_changed(uint8_t element_idx);

/**
 * \brief Light LC Server Scene Store Handler
 * \details The mesh Scene Server calls this function so that the module can store required data for the scene.
 *
 * @param       element_idx Element index for which scene information is being stored
 * @param       p_buffer Pointer to a buffer where data should be stored
 * @param       buf_size Maximum amount of data a model can store
 *
 * @return      Number of bytes the model wrote into the provided buffer
 */
uint16_t wiced_bt_mesh_model_light_lc_server_scene_store_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len);

/**
 * \brief Light LC Server Scene Store Handler
 * \details When Scene Server receives Scene Recall message it calls this function with data previously stored in the NVRAM.
 *
 * @param       element_idx Element index for which scene information is being recalled
 * @param       p_buffer Pointer to a buffer where model's data is located
 * @param       buf_size Maximum amount of data a model can get from the buffer
 * @param       transition_time Specified time in milliseconds to transition to the target state
 * @param       delay Specifies time in milliseconds before starting to transition to the target state
 *
 * @return      Number of bytes the model read from the provided buffer
 */
uint16_t wiced_bt_mesh_model_light_lc_server_scene_recall_handler(uint8_t element_idx, uint8_t *p_buffer, uint16_t buffer_len, uint32_t transition_time, uint32_t delay);

/**
 * \brief Light Control Setup Server callback is called by the Mesh Models library on receiving a message from the peer
 * Application or higher level model should be able process messages for the Light LC Setup server model as well as
 * bound models including generic Light Lightness, Power On/Off, Generic Level and Generic On/Off.
 *
 * @param       event The event that the application should process (see @ref LIGHT_LIGHTNESS_EVENT Light Lightness Events, @ref ONOFF_EVENT "On/Off Events, @ref LEVEL_EVENT "Level Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the data structure specific to the event
 *
 * @return      None
 */
typedef void (wiced_bt_mesh_light_lc_server_callback_t)(uint8_t element_idx, uint16_t event, void *p_data);

/**
 * \brief Light LC Setup Server Module initialization
 * \details A Mesh application which contains Light LC Setup Server (for example a dimmable bulb) should call this function for each element where the Light LC Server is present (for example for each bulb).
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback Application or Parent model callback that will be executed on receiving a message
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      WICED_TRUE if initialization was successful
 */
void wiced_bt_mesh_model_light_lc_server_init(uint8_t element_idx, wiced_bt_mesh_light_lc_server_callback_t *p_callback, wiced_bool_t is_provisioned);
/* @} wiced_bt_mesh_model_light_lc_server */

/**
 * @addtogroup  wiced_bt_mesh_light_lc_client        Mesh Light LC Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Light LC Client module of the Mesh Models library provides a way for an application to configure and control
 * lights that support LC control.
 *
 * To include Light LC Client functionality, the device description shall contain @ref WICED_BT_MESH_MODEL_LIGHT_LC_CLIENT.
 *
 * TBD
 *
 * @{
 */
/**
 * \brief Light Control Client callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref LIGHT_LC_EVENT "Mesh Light LC Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the data to send
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_light_lc_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Light Control Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_light_lc_client_init(uint8_t element_idx, wiced_bt_mesh_power_onoff_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Light Control Client Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_light_lc_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief The application can call this function to send the Light LC Mode Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lc_client_send_mode_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light LC Mode Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lc_client_send_mode_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_lc_mode_set_data_t* p_data);

/**
 * \brief The application can call this function to send the Light LC Occupancy Mode Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lc_client_send_occupancy_mode_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light LC Occupancy Mode Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lc_client_send_occupancy_mode_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_lc_occupancy_mode_set_data_t* p_data);

/**
 * \brief The application can call this function to send the Light LC OnOff Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lc_client_send_light_onoff_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief The application can call this function to send Light LC Occupancy Mode Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
 * @param       p_data Pointer to the data to send
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lc_client_send_light_onoff_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_lc_light_onoff_set_data_t* p_data);

/**
* \brief The application can call this function to send Light LC Property Get client message to the server.
*
* @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
* @param       p_data Pointer to the data to send containing property ID to retrieve
*
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lc_client_send_property_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_lc_property_get_data_t *p_data);

/**
* \brief The application can call this function to send Light LC Property Set client message to the server.
*
* @param       p_event Mesh event with the information about the message that has been created by the app for unsolicited message.
* @param       p_data Pointer to the data to send containing property ID and the value.
*
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_light_lc_client_send_property_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_light_lc_property_set_data_t *p_data);
/* @} wiced_bt_mesh_model_light_lc_client */

/**
 * @addtogroup  wiced_bt_mesh_sensor_server   Mesh Sensor Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Sensor Server module provides a way for an application to implement a functionality for the devices that can be controlled using sensor.
 *
 * To include the Mesh Models support for the Sensor Server module the application shall contain the @ref WICED_BT_MESH_MODEL_SENSOR_SERVER
 *
 * TBD
 *
 @{
 */
/*
 * \brief Sensor server Config callback is called by the Mesh Models library to notify cadence and setting changes.
 *
 * @param       element_idx Device element to where model is used
 * @param       event The event that the application should process (see @ref SENSOR_EVENT "Sensor Events")
 * @param       property_id Property ID of the sensor for which Sensor Cadence change shall be reported
 * @param       setting_property_id Setting Property ID of the sensor for which Sensor setting change shall be reported
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_sensor_server_config_callback_t)(uint8_t element_idx, uint16_t event, uint16_t property_id, uint16_t setting_prop_id);


/*
 * \brief Sensor server report callback is called by the Mesh Models library on receiving a Get message from the sensor client.
 *
 * @param       event The event that the application should process (see @ref SENSOR_EVENT "Sensor Events")
 * @param       element_idx Index of the element which contains the sensor.
 * @param       p_get_params Get parameters spefic to the Get request being processed
 * @param       p_ref_data When application replies to the callback with sensor_server_data, it should pass back to the library the same pointer.
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_sensor_server_report_callback_t)(uint16_t event, uint8_t element_idx, void *p_get_params, void *p_ref_data);

/**
 * \brief Sensor Senosr Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_report_callback The application callback that will be executed on receiving a sensor get messages.
 * @param       p_config_change_callback The application callback that will be executed to notify cadenence and setting changes.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_sensor_server_init(uint8_t element_idx, wiced_bt_mesh_sensor_server_report_callback_t *p_report_callback, wiced_bt_mesh_sensor_server_config_callback_t *p_config_change_callback,  wiced_bool_t is_provisioned);

/**
 * \brief Sensor Server Message Handler
 * \details An application or a parent model can call this models library function to process a message received
 * from the Sensor Client device. The function parses the message and if appropriate calls the parent back
 * to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_sensor_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Sensor Setup Server Message Handler
 * \details An application or a parent model can call this models library function to process a message received
 * from the Sensor Client device. The function parses the message and if appropriate calls the parent back
 * to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_sensor_setup_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Sensor Series Status send status
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       series_status_data pointer to the series data
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_sensor_server_series_status_send(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_sensor_series_status_data_t *series_status_data);

/**
 * \brief Sensor Column Status send status
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       column_status_data pointer to the column data
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_sensor_server_column_status_send(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_sensor_column_status_data_t *column_status_data);

/**
 * \brief Format Sensor Data
 * \details An Application can call this function to format sensor data to be sent out. Function returns length of the data filled in the buffer.
 * Property_id 0 means that we need to send data for all sensors of the element.
 *
 * @param       element_idx Element Index where the sensor is located
 * @param       property_id Property ID of the sensor for which sensor data shall be used.  If Property ID is zero, the data will include all sensors of the element.
 * @param       p_buffer Buffer to be filled with information
 * @param       buffer_len size of the buffer
 *
 * @return      number of bytess filled in the buffer.
 */
uint16_t wiced_bt_mesh_model_format_sensor_server_data(uint8_t element_idx, uint16_t property_id, uint8_t* p_buffer, uint16_t buffer_len);

/**
 * \brief Sensor Status send
 *
 * @param       element_idx Element Index where the sensor is located
 * @param       property_id Property ID of the sensor for which sensor data shall be reported.  If Property ID is zero, the Status will include all sensors of the element.
 * @param       p_ref_data If application is sending data based on the Get request from the library, the parameter should contain the values passed in the GET request.  Should be zero otherwise.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_sensor_server_data(uint8_t element_idx, uint16_t property_id, void *p_ref_data);


/**
 * \brief Sensor Setting status
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       property_id Property ID of the sensor for which Sensor Setting status shall be reported
 * @param       setting_property_id Property ID of the Sensor Setting, that the Sensor Setting status shall be reported
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_sensor_server_setting_status_send(wiced_bt_mesh_event_t *p_event, uint16_t property_id, uint16_t setting_property_id);
/* @} wiced_bt_mesh_model_sensor_server */

/**
 * @addtogroup  wiced_bt_mesh_sensor_client   Mesh Sensor Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Sensor Client module provides a way for an application to control Sensor Servers in the mesh network
 *
 * To include the Mesh Models support for the Sensor Client module the application shall contain the @ref WICED_BT_MESH_MODEL_SENSOR_CLIENT
 *
 * TBD
 *
 * @{
 */
/**
 * \brief Sensor Client callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref LIGHT_LC_EVENT "Mesh Light LC Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the data to send
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_sensor_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t* p_event, void* p_data);

/**
 * \brief Sensor Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback The application callback function that will be executed by the mesh models library when application action is required, or when a reply for the application request has been received.
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_sensor_client_init(uint8_t element_idx, wiced_bt_mesh_sensor_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Sensor client Message Handler
 * \details An application or a parent model can call this models library function to process a message received
 * from the Sensor Server device. The function parses the message and if appropriate calls the parent back
 * to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_sensor_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Sensor Descriptor get
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       desc_get_data pointer to descriptor get values
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_sensor_client_descriptor_send_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_sensor_get_t *desc_get_data);

/**
 * \brief Sensor Data get
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       sensor_get pointer to the sensor data to be received
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_sensor_client_sensor_send_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_sensor_get_t *sensor_get);

/**
 * \brief Sensor Column get
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       column_data pointer to data portion of the sensor column get message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_sensor_client_sensor_column_send_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_sensor_column_get_data_t *column_data);

/**
 * \brief Sensor Series get
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       series_data pointer to data portion of the sensor series get message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_sensor_client_sensor_series_send_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_sensor_series_get_data_t *series_data);

/**
 * \brief Sensor Setting get
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       setting_data pointer to data portion of the sensor setting get message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_sensor_client_sensor_setting_send_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_sensor_setting_get_data_t *setting_data);

/**
 * \brief Sensor Settings get
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       settings_data pointer to data portion of the sensor settings get message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_sensor_client_sensor_settings_send_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_sensor_get_t *settings_data);

/**
 * \brief Sensor Setting set
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       setting_data pointer to data portion of the sensor settings get message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_sensor_client_sensor_setting_send_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_sensor_setting_set_data_t *setting_data);

/**
 * \brief Sensor Cadence get
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       cadence_data pointer to data portion of the cadence get message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_sensor_client_sensor_cadence_send_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_sensor_get_t *cadence_data);

/**
 * \brief Sensor Cadence set
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       cadence_set pointer to data portion of the cadence set message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_sensor_client_sensor_cadence_send_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_sensor_cadence_set_data_t *cadence_set);
/* @} wiced_bt_mesh_model_sensor_client */

/**
 * @addtogroup  wiced_bt_mesh_scene_server   Mesh Scene Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Scene Server module provides a way for an application to implement a functionality for the devices that support scene state.
 *
 * To include the Mesh Models support for the Scene Server module the application shall contain the @ref WICED_BT_MESH_MODEL_SCENE_SERVER
 *
 * TBD
 *
 @{
 */

/**
 * \brief Scene Server callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref SCENE_EVENT "Scene Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the data structure
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_scene_server_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Scene Server Module initialization
 *
 * @param       p_callback Application or Parent model callback that will be executed on receiving a message
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_scene_server_init(wiced_bt_mesh_scene_server_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Scene Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_scene_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Scene Setup Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_scene_setup_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Scene Data Changed
 * The Scene Data Changed function should be called when any data stored with a scene has changed
 */
void wiced_bt_mesh_model_scene_data_changed(uint8_t element_idx);
/* @} wiced_bt_mesh_model_scene_server */

/**
 * @addtogroup  wiced_bt_mesh_scene_client   Mesh Scene Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Scene Client module provides a way for an application to control Scene Servers in the mesh network
 *
 * To include the Mesh Models support for the Scene Client module the application shall contain the @ref WICED_BT_MESH_MODEL_SCENE_CLIENT
 *
 * TBD
 *
 * @{
 */
/**
 * \brief Scene Client callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref SCENE_EVENT "Scene Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the data structure
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_scene_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Scene Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback Application or Parent model callback that will be executed on receiving a message
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_scene_client_init(uint8_t element_idx, wiced_bt_mesh_scene_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Scene client Message Handler
 * \details An application or a parent model can call this models library function to process a message received
 * from the Scene Server device. The function parses the message and if appropriate calls the parent back
 * to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_scene_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Scene Client send request
 * \details Application can call Scene Client send request function to send Scene Store/Recall/Get/Delete client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_request pointer to a data structure containing request type and scene number
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_scene_client_send_request(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_scene_request_t *p_request);

/**
 * \brief Scene Client Send Recall
 * \details Application can call Scene Client Send Recall function to send Scene Recall client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_request pointer to a data structure containing request type and scene number
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_scene_client_send_recall(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_scene_recall_t *p_request);

 /**
 * \brief Scene Client Send Register Get
 * \details Application can call Scene Client Send Register Get function to send Scene Register Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_scene_client_send_register_get(wiced_bt_mesh_event_t *p_event);
/* @} wiced_bt_mesh_model_scene_client */

/**
 * @addtogroup  wiced_bt_mesh_time_server   Mesh Time Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Time Server module provides a way for an application to implement a functionality for the devices that support time state.
 *
 * To include the Mesh Models support for the Time Server module the application shall contain the @ref WICED_BT_MESH_MODEL_TIME_SERVER
 *
 * TBD
 *
 * @{
 */

/**
 * \brief Time Server callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref TIME_EVENT "Time Events")
 * @param       p_data Pointer to the RTC Time data structure
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_time_server_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Time Server Module initialization
 *
 * @param       p_callback Application or Parent model callback that will be executed on receiving a message
 *
 * @return      None
 */
void wiced_bt_mesh_model_time_server_init(wiced_bt_mesh_time_server_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Time Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_time_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Time Setup Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_time_setup_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Time server status send
 *
 * @param       p_event               Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_time_zone_status_data pointer to data portion of the time zone status message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_time_server_status_send(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_time_state_msg_t *p_time_status_data);

/**
 * \brief Time zone status send
 *
 * @param       p_event          Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_time_status_data pointer to data portion of the time status message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_time_server_zone_status_send(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_time_zone_status_t *p_time_zone_status_data);

/**
 * \brief Time TAI_UTC delta status send
 *
 * @param       p_event          Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_time_delta_status_data pointer to data portion of the Time TAI_UTC delta status message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_time_server_tai_utc_delta_status_send(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_time_tai_utc_delta_status_t *p_time_delta_status_data);
/* @} wiced_bt_mesh_model_time_server */

/**
 * @addtogroup  wiced_bt_mesh_time_client   Mesh Time Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Time Client module provides a way for an application to control Time Servers in the mesh network
 *
 * To include the Mesh Models support for the Time Client module the application shall contain the @ref WICED_BT_MESH_MODEL_TIME_CLIENT
 *
 * TBD
 *
 * @{
 */
/**
 * \brief Time Client callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref TIME_EVENT "Time Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the data structure
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_time_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Time Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback Application or Parent model callback that will be executed on receiving a message
 *
 * @return      None
 */
void wiced_bt_mesh_model_time_client_init(wiced_bt_mesh_time_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Time Client Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_time_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Time Get send
 *
 * @param       p_event          Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_time_client_time_get_send(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Time Set send
 *
 * @param       p_event          Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_time_set_data    pointer to data portion of the Time set message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_time_client_time_set_send(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_time_state_msg_t *p_time_set_data);

/**
 * \brief Time Zone Get send
 *
 * @param       p_event  Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_time_client_time_zone_get_send(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Time Zone Set send
 *
 * @param       p_event          Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_set_data         pointer to data portion of the Time zone set message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_time_client_time_zone_set_send(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_time_zone_set_t *p_set_data);

/**
 * \brief Time TAI UTC delta Get send
 *
 * @param       p_event  Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_time_client_tai_utc_delta_get_send(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Time TAI UTC delta Set send
 *
 * @param       p_event          Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_time_delta_set         pointer to data portion of the Time TAI UTC delta Set message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_time_client_tai_utc_delta_set_send(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_time_tai_utc_delta_set_t *p_time_delta_set);

/**
 * \brief Time Role get send
 *
 * @param       p_event  Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_time_client_time_role_get_send(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Time role Set send
 *
 * @param       p_event          Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_set_data         pointer to data portion of the Time role Set message
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_time_client_time_role_set_send(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_time_role_msg_t *p_set_data);
/* @} wiced_bt_mesh_time_client */


/**
 * @addtogroup  wiced_bt_mesh_scheduler_server   Mesh Scheduler Server
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Scheduler Server module provides a way for an application to implement a functionality for the devices that support Scheduler.
 *
 * TBD
 *
 * @{
 */
/**
 * \brief Scheduler Server callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref SCHEDULER_EVENT "Scheduler Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the data structure
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_scheduler_server_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Scheduler Server Module initialization
 *
 * @param       p_callback Application or Parent model callback that will be executed on receiving a message
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_scheduler_server_init(wiced_bt_mesh_scheduler_server_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Scheduler Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_scheduler_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Scheduler Setup Server Message Handler
 * \details The Mesh Core library calls this function for each message received.  The function returns WICED_TRUE if the message is destined for this specific model and successfully processed, and returns WICED_FALSE otherwise.
 * The function parses the message and if appropriate calls the parent back to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_scheduler_setup_server_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/* @} wiced_bt_mesh_scheduler_server */

/**
 * @addtogroup  wiced_bt_mesh_scheduler_client   Mesh Scheduler Client
 * @ingroup     wiced_bt_mesh_models
 *
 * The AIROC Mesh Scheduler Client module provides a way for an application to control Scheduler Servers in the mesh network
 *
 * TBD
 *
 * @{
 */
/**
 * \brief Scheduler Client callback is called by the Mesh Models library on receiving a message from the peer
 *
 * @param       event The event that the application should process (see @ref SCHEDULER_EVENT "Scheduler Events")
 * @param       p_event information about the message received.  The same pointer should be used in the reply if required.
 * @param       p_data Pointer to the data structure
 *
 * @return      None
 */
typedef void(wiced_bt_mesh_scheduler_client_callback_t)(uint16_t event, wiced_bt_mesh_event_t *p_event, void *p_data);

/**
 * \brief Scheduler Client Module initialization
 *
 * @param       element_idx Device element to where model is used
 * @param       p_callback Application or Parent model callback that will be executed on receiving a message
 * @param       is_provisioned If TRUE, the application is being restarted after being provisioned or after a power loss. If FALSE the model cleans up NVRAM on startup.
 *
 * @return      None
 */
void wiced_bt_mesh_model_scheduler_client_init(wiced_bt_mesh_scheduler_client_callback_t *p_callback, wiced_bool_t is_provisioned);

/**
 * \brief Scheduler client Message Handler
 * \details An application or a parent model can call this models library function to process a message received
 * from the Scheduler Server device. The function parses the message and if appropriate calls the parent back
 * to perform functionality.
 *
 * @param       p_event Mesh event with information about received message.
 * @param       p_data Pointer to the data portion of the message
 * @param       data_len Length of the data in the message
 *
 * @return      WICED_TRUE if the message is for this company ID/Model/Element Index combination, WICED_FALSE otherwise.
*/
wiced_bool_t wiced_bt_mesh_model_scheduler_client_message_handler(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

/**
 * \brief Scheduler Client send Get request
 * \details Application can call Scheduler Client Get function to send Scheduler Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_scheduler_client_send_get(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Scheduler Client Send Action Get request
 * \details Application can call Scheduler Client Action Get function to send Scheduler Action Get client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_data Pointer to the data structure containing the action number of interest
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_scheduler_client_send_action_get(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_scheduler_action_get_t *p_data);

/**
 * \brief Scheduler Client Send Action Set request
 * \details Application can call Scheduler Client Action Set function to send Scheduler Action Set client message to the server.
 *
 * @param       p_event Mesh event with the information about the message that has been received in the request, or created by the app for unsolicited message.
 * @param       p_data Pointer to the data structure containing the action number of interest, time stamp when action should accure and the scene number for the device to move to.
 *
 * @return      WICED_BT_SUCCESS if message has been queued for transmission.
 */
wiced_result_t wiced_bt_mesh_model_scheduler_client_send_action_set(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_scheduler_action_data_t *p_data);
/* @} wiced_bt_mesh_scheduler_client */

/*
 * Application can register with remote provisioning server to receive notification
 * when unprovisioned device is detected
 */
typedef void (*wiced_bt_mesh_unprovisioned_device_cb_t)(uint8_t* p_uuid, uint16_t oob, uint32_t uri_hash, int8_t rssi);

/**
 * \brief Remote Provisioning Server initialization
 *
 * @param       None
 *
 * @return      None
 */
void wiced_bt_mesh_remote_provisioning_server_init(void);

/**
 * \brief Process Scan Response Packets to check if the packet is for remote provisioning server.
 *
 * @param  p_adv_report Advertising report paremeters
 * @param  p_adv_data Advertising data
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_remote_provisioning_scan_rsp(wiced_bt_ble_scan_results_t* p_adv_report, uint8_t* p_adv_data);

/**
 * \brief Process Connectable Advertising Packets to check if the packet is for remote provisioning server.
 *
 * @param  p_adv_report Advertising report paremeters
 * @param  p_adv_data Advertising data
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_remote_provisioning_connectable_adv_packet(wiced_bt_ble_scan_results_t* p_adv_report, uint8_t* p_adv_data);

/**
 * \brief Process None Connectable Advertising Packets to check if the packet is for remote provisioning server.
 *
 * @param  p_adv_report Advertising report paremeters
 * @param  p_adv_data Advertising data
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_remote_provisioning_nonconnectable_adv_packet(wiced_bt_ble_scan_results_t* p_adv_report, uint8_t* p_adv_data);

/**
 * \brief Sets trace level for mesh_models_lib and mesh_provisioner_lib.
 * \details Application may call this function to set trace level for mesh_models_lib and mesh_provisioner_lib. By default trace level is 0(no trace).
 *
 * @param[in]   level       Trace level to set. It can be any of WICED_BT_MESH_CORE_TRACE_XXX.
 */
void wiced_bt_mesh_models_set_trace_level(uint8_t level);

/**
 * \brief Find model in the node configuration starting with specified element index
 *
 * @param[in]   company_id   Company ID of the model
 * @param[in]   model_id     Model ID of the model
 * @param[in]   start_idx    Starting element index
 */
int wiced_bt_mesh_model_find_element_idx(uint16_t company_id, uint16_t model_id, uint8_t start_idx);

/* @} wiced_bt_mesh_models */

#define MESH_NODE_ID_INVALID          0x0000

#ifdef __cplusplus
}
#endif

#endif /* __WICED_BT_MESH_MODELS_H__ */
