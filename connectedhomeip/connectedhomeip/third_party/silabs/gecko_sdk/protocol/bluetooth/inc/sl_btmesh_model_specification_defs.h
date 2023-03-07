/******************************************************************************/
/**
 * @file   sl_btmesh_model_specification_defs.h
 * @brief  Silicon Labs Bluetooth Mesh Model v1.0 Specification Definitions
 *
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
#ifndef SL_BTMESH_MODEL_SPECIFICATION_DEFS_H
#define SL_BTMESH_MODEL_SPECIFICATION_DEFS_H

#include "sl_btmesh_capi_types.h"

/** Generic on/off server */
#define MESH_GENERIC_ON_OFF_SERVER_MODEL_ID             0x1000
/** Generic on/off client */
#define MESH_GENERIC_ON_OFF_CLIENT_MODEL_ID             0x1001

/** Generic level server */
#define MESH_GENERIC_LEVEL_SERVER_MODEL_ID              0x1002
/** Generic level client */
#define MESH_GENERIC_LEVEL_CLIENT_MODEL_ID              0x1003

/** Generic transition time server */
#define MESH_GENERIC_TRANSITION_TIME_SERVER_MODEL_ID    0x1004
/** Generic transition time client */
#define MESH_GENERIC_TRANSITION_TIME_CLIENT_MODEL_ID    0x1005

/** Generic power on/off server */
#define MESH_GENERIC_POWER_ON_OFF_SERVER_MODEL_ID       0x1006
/** Generic power on/off setup server */
#define MESH_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL_ID 0x1007
/** Generic power on/off client */
#define MESH_GENERIC_POWER_ON_OFF_CLIENT_MODEL_ID       0x1008

/** Generic power level server */
#define MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_ID        0x1009
/** Generic power level setup server */
#define MESH_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL_ID  0x100a
/** Generic power level client */
#define MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_ID        0x100b

/** Generic battery server */
#define MESH_GENERIC_BATTERY_SERVER_MODEL_ID            0x100c
/** Generic battery client */
#define MESH_GENERIC_BATTERY_CLIENT_MODEL_ID            0x100d

/** Generic location server */
#define MESH_GENERIC_LOCATION_SERVER_MODEL_ID           0x100e
/** Generic location setup server */
#define MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_ID     0x100f
/** Generic location client */
#define MESH_GENERIC_LOCATION_CLIENT_MODEL_ID           0x1010

/** Generic admin property server */
#define MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_ID     0x1011
/** Generic manufacturer property server */
#define MESH_GENERIC_MANUF_PROPERTY_SERVER_MODEL_ID     0x1012
/** Generic user property server */
#define MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_ID      0x1013
/** Generic client property server */
#define MESH_GENERIC_CLIENT_PROPERTY_SERVER_MODEL_ID    0x1014
/** Generic property client */
#define MESH_GENERIC_PROPERTY_CLIENT_MODEL_ID           0x1015

/** Sensor Server model ID */
#define MESH_SENSOR_SERVER_MODEL_ID                     0x1100
/** Sensor Setup Server model ID*/
#define MESH_SENSOR_SETUP_SERVER_MODEL_ID               0x1101
/** Sensor Client model ID */
#define MESH_SENSOR_CLIENT_MODEL_ID                     0x1102

/** Time Server model ID */
#define MESH_TIME_SERVER_MODEL_ID                       0x1200
/** Time Setup Server model ID */
#define MESH_TIME_SETUP_SERVER_MODEL_ID                 0x1201
/** Time Client model ID */
#define MESH_TIME_CLIENT_MODEL_ID                       0x1202

/** Scheduler Server model ID */
#define MESH_SCHEDULER_SERVER_MODEL_ID                  0x1206
/** Scheduler Setup Server model ID */
#define MESH_SCHEDULER_SETUP_SERVER_MODEL_ID            0x1207
/** Scheduler Client model ID */
#define MESH_SCHEDULER_CLIENT_MODEL_ID                  0x1208

/** Light lightness server */
#define MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID         0x1300
/** Light lightness setup server */
#define MESH_LIGHTING_LIGHTNESS_SETUP_SERVER_MODEL_ID   0x1301
/** Light lightness client */
#define MESH_LIGHTING_LIGHTNESS_CLIENT_MODEL_ID         0x1302

/** Light CTL server */
#define MESH_LIGHTING_CTL_SERVER_MODEL_ID               0x1303
/** Light CTL setup server */
#define MESH_LIGHTING_CTL_SETUP_SERVER_MODEL_ID         0x1304
/** Light CTL client */
#define MESH_LIGHTING_CTL_CLIENT_MODEL_ID               0x1305
/** Light CTL temperature server */
#define MESH_LIGHTING_CTL_TEMPERATURE_SERVER_MODEL_ID   0x1306

/** Light HSL server */
#define MESH_LIGHTING_HSL_SERVER_MODEL_ID               0x1307
/** Light HSL setup server */
#define MESH_LIGHTING_HSL_SETUP_SERVER_MODEL_ID         0x1308
/** Light HSL client */
#define MESH_LIGHTING_HSL_CLIENT_MODEL_ID               0x1309
/** Light HSL hue server */
#define MESH_LIGHTING_HSL_HUE_SERVER_MODEL_ID           0x130a
/** Light HSL saturation server */
#define MESH_LIGHTING_HSL_SATURATION_SERVER_MODEL_ID    0x130b

/** Light xyl server */
#define MESH_LIGHTING_XYL_SERVER_MODEL_ID               0x130c
/** Light xyl setup server */
#define MESH_LIGHTING_XYL_SETUP_SERVER_MODEL_ID         0x130d
/** Light xyl client */
#define MESH_LIGHTING_XYL_CLIENT_MODEL_ID               0x130e

/*
 * Generic model opcodes
 */

/** Generic On/Off Get */
#define GENERIC_ON_OFF_GET      0x8201
/** Generic On/Off Set */
#define GENERIC_ON_OFF_SET      0x8202
/** Generic On/Off Set Unreliable*/
#define GENERIC_ON_OFF_SET_UNRL   0x8203
/** Generic On/Off Status */
#define GENERIC_ON_OFF_STATUS   0x8204

/** Generic Level Get */
#define GENERIC_LEVEL_GET     0x8205
/** Generic Level Set */
#define GENERIC_LEVEL_SET     0x8206
/** Generic Level Set Unreliable */
#define GENERIC_LEVEL_SET_UNRL    0x8207
/** Generic Level Status */
#define GENERIC_LEVEL_STATUS    0x8208
/** Generic Level Delta */
#define GENERIC_LEVEL_DELTA     0x8209
/** Generic Level Delta Unreliable */
#define GENERIC_LEVEL_DELTA_UNRL  0x820a
/** Generic Level Move */
#define GENERIC_LEVEL_MOVE      0x820b
/** Generic Level Move Unreliable */
#define GENERIC_LEVEL_MOVE_UNRL   0x820c

/** Generic Transition Time Get */
#define GENERIC_TRANSITION_TIME_GET     0x820d
/** Generic Transition Time Set */
#define GENERIC_TRANSITION_TIME_SET     0x820e
/** Generic Transition Time Set Unreliable */
#define GENERIC_TRANSITION_TIME_SET_UNRL  0x820f
/** Generic Transition Time Status */
#define GENERIC_TRANSITION_TIME_STATUS    0x8210

/** Generic On Power Up Get */
#define GENERIC_ON_POWER_UP_GET       0x8211
/** Generic On Power Up Status */
#define GENERIC_ON_POWER_UP_STATUS      0x8212
/** Generic On Power Up Set */
#define GENERIC_ON_POWER_UP_SET       0x8213
/** Generic On Power Up Set Unreliable */
#define GENERIC_ON_POWER_UP_SET_UNRL    0x8214

/** Generic Power Level Get */
#define GENERIC_POWER_LEVEL_GET       0x8215
/** Generic Power Level Set */
#define GENERIC_POWER_LEVEL_SET       0x8216
/** Generic Power Level Set Unreliable */
#define GENERIC_POWER_LEVEL_SET_UNRL    0x8217
/** Generic Power Level Status */
#define GENERIC_POWER_LEVEL_STATUS      0x8218
/** Generic Power Level Last Get */
#define GENERIC_POWER_LAST_GET        0x8219
/** Generic Power Level Last Status */
#define GENERIC_POWER_LAST_STATUS     0x821a
/** Generic Power Level Default Get */
#define GENERIC_POWER_DEFAULT_GET     0x821b
/** Generic Power Level Default Status */
#define GENERIC_POWER_DEFAULT_STATUS    0x821c
/** Generic Power Level Range Get */
#define GENERIC_POWER_RANGE_GET       0x821d
/** Generic Power Level Range Status */
#define GENERIC_POWER_RANGE_STATUS      0x821e
/** Generic Power Level Default Set */
#define GENERIC_POWER_DEFAULT_SET     0x821f
/** Generic Power Level Default Set Unreliable */
#define GENERIC_POWER_DEFAULT_SET_UNRL    0x8220
/** Generic Power Level Range Set */
#define GENERIC_POWER_RANGE_SET       0x8221
/** Generic Power Level Range Set Unreliable */
#define GENERIC_POWER_RANGE_SET_UNRL    0x8222

/** Generic Battery Get */
#define GENERIC_BATTERY_GET         0x8223
/** Generic Battery Status */
#define GENERIC_BATTERY_STATUS        0x8224

/** Generic Location Global Get */
#define GENERIC_LOCATION_GLOBAL_GET     0x8225
/** Generic Location Local Get */
#define GENERIC_LOCATION_LOCAL_GET      0x8226
/** Generic Location Local Status */
#define GENERIC_LOCATION_LOCAL_STATUS   0x8227
/** Generic Location Local Set */
#define GENERIC_LOCATION_LOCAL_SET      0x8228
/** Generic Location Local Set Unreliable */
#define GENERIC_LOCATION_LOCAL_SET_UNRL   0x8229
/** Generic Location Global Status */
#define GENERIC_LOCATION_GLOBAL_STATUS    0x40
/** Generic Location Global Set */
#define GENERIC_LOCATION_GLOBAL_SET     0x41
/** Generic Location Global Set Unreliable */
#define GENERIC_LOCATION_GLOBAL_SET_UNRL  0x42

/** Generic Manufacturer Properties Get */
#define GENERIC_MANUF_PROPERTIES_GET 0x822a
/** Generic Manufacturer Properties Status */
#define GENERIC_MANUF_PROPERTIES_STATUS 0x43
/** Generic Manufacturer Property Get */
#define GENERIC_MANUF_PROPERTY_GET 0x822b
/** Generic Manufacturer Property Set */
#define GENERIC_MANUF_PROPERTY_SET 0x44
/** Generic Manufacturer Property Set Unreliable */
#define GENERIC_MANUF_PROPERTY_SET_UNRL 0X45
/** Generic Manufacturer Property Status */
#define GENERIC_MANUF_PROPERTY_STATUS 0x46

/** Generic Admin Properties Get */
#define GENERIC_ADMIN_PROPERTIES_GET 0x822c
/** Generic Admin Properties Status */
#define GENERIC_ADMIN_PROPERTIES_STATUS 0x47
/** Generic Admin Property Get */
#define GENERIC_ADMIN_PROPERTY_GET 0x822D
/** Generic Admin Property Set */
#define GENERIC_ADMIN_PROPERTY_SET 0x48
/** Generic Admin Property Set Unreliable */
#define GENERIC_ADMIN_PROPERTY_SET_UNRL 0x49
/** Generic Admin Property Status */
#define GENERIC_ADMIN_PROPERTY_STATUS 0x4A

/** Generic User Properties Get */
#define GENERIC_USER_PROPERTIES_GET 0x822e
/** Generic User Properties Status */
#define GENERIC_USER_PROPERTIES_STATUS 0x4b
/** Generic User Property Get */
#define GENERIC_USER_PROPERTY_GET 0x822f
/** Generic User Property Set */
#define GENERIC_USER_PROPERTY_SET 0x4c
/** Generic User Property Set Unreliable */
#define GENERIC_USER_PROPERTY_SET_UNRL 0x4d
/** Generic User Property Status */
#define GENERIC_USER_PROPERTY_STATUS 0x4e
/** Generic Client Properties Get */
#define GENERIC_CLIENT_PROPERTIES_GET 0x4f
/** Generic Client Properties Status */
#define GENERIC_CLIENT_PROPERTIES_STATUS 0x50

/**
 * Sensor Server model opcodes
 */

/** Sensor Descriptor Get request */
#define SENSOR_DESCRIPTOR_GET       0x8230
/** Sensor Descriptor Status message */
#define SENSOR_DESCRIPTOR_STATUS    0x51

/** Sensor Get request */
#define SENSOR_GET                  0x8231
/** Sensor Status message */
#define SENSOR_STATUS               0x52

/** Sensor Column Get request */
#define SENSOR_COLUMN_GET           0x8232
/** Sensor Status message */
#define SENSOR_COLUMN_STATUS        0x53

/** Sensor Series Get request */
#define SENSOR_SERIES_GET           0x8233
/** Sensor Series Status message */
#define SENSOR_SERIES_STATUS        0x54

/**
 * Sensor Setup Server opcodes
 */
/** Sensor Cadence Get request */
#define SENSOR_CADENCE_GET          0x8234
/** Sensor Cadence Set request */
#define SENSOR_CADENCE_SET          0x55
/** Sensor Cadence Set request, no acknowledgment */
#define SENSOR_CADENCE_SET_UNACK    0x56
/** Sensor Cadence Status message */
#define SENSOR_CADENCE_STATUS       0x57

/** Sensor Settings Get request */
#define SENSOR_SETTINGS_GET         0x8235
/** Sensor Settings Status message */
#define SENSOR_SETTINGS_STATUS      0x58

/** Sensor Setting Get request */
#define SENSOR_SETTING_GET          0x8236
/** Sensor Setting Set request */
#define SENSOR_SETTING_SET          0x59
/** Sensor Setting Set request */
#define SENSOR_SETTING_SET_UNACK    0x5A

/** Sensor Setting Status message */
#define SENSOR_SETTING_STATUS       0x5B

/* Time model message opcodes */

/** Time Get */
#define TIME_GET                    0x8237
/** Time Set */
#define TIME_SET                    0x5C
/** Time Status */
#define TIME_STATUS                 0x5D
/** Time Role Get */
#define TIME_ROLE_GET               0x8238
/** Time Role Set */
#define TIME_ROLE_SET               0x8239
/** Time Role Status */
#define TIME_ROLE_STATUS            0x823A
/** Time Zone Get */
#define TIME_ZONE_GET               0x823B
/** Time Zone Set */
#define TIME_ZONE_SET               0x823C
/** Time Zone Status */
#define TIME_ZONE_STATUS            0x823D
/** Time TAI-UTC Delta Get */
#define TIME_TAI_UTC_DELTA_GET      0x823E
/** Time TAI-UTC Delta Set */
#define TIME_TAI_UTC_DELTA_SET      0x823F
/** Time TAI-UTC Delta Status */
#define TIME_TAI_UTC_DELTA_STATUS   0x8240

/** Scene Get request */
#define SCENE_GET                0x8241
/** Scene Recall request */
#define SCENE_RECALL             0x8242
/** Scene Recall Unacknowledged request */
#define SCENE_RECALL_UNACK       0x8243
/** Scene Register Get request */
#define SCENE_REGISTER_GET       0x8244
/** Scene Register Status request */
#define SCENE_REGISTER_STATUS    0x8245
/** Scene Store request */
#define SCENE_STORE              0x8246
/** Scene Store Unacknowledged request */
#define SCENE_STORE_UNACK        0x8247
/** Scene Delete request */
#define SCENE_DELETE              0x829E
/** Scene Delete Unacknowledged request */
#define SCENE_DELETE_UNACK        0x829F
/** Scene Status */
#define SCENE_STATUS             0x5E

/* Scheduler model message opcodes */
/** Scheduler Action Get */
#define SCHEDULER_ACTION_GET          0x8248
/** Scheduler Action Status */
#define SCHEDULER_ACTION_STATUS       0x5F
/** Scheduler Get */
#define SCHEDULER_GET                 0x8249
/** Scheduler Status */
#define SCHEDULER_STATUS              0x824A
/** Scheduler Action Set */
#define SCHEDULER_ACTION_SET          0x60
/** Scheduler Action Set Unreliable */
#define SCHEDULER_ACTION_SET_NOACK    0x61

/**
 * LC Server model opcodes
 */

/** LC Mode Get request */
#define LC_MODE_GET       0x8291
/** LC Mode Set request */
#define LC_MODE_SET       0x8292
/** LC Mode Set request */
#define LC_MODE_SET_UNACK       0x8293
/** LC Mode Status message */
#define LC_MODE_STATUS    0x8294

/** LC OM Get request */
#define LC_OM_GET       0x8295
/** LC OM Set request */
#define LC_OM_SET       0x8296
/** LC OM Set Unacknowledged request */
#define LC_OM_SET_UNACK       0x8297
/** LC OM Status message */
#define LC_OM_STATUS    0x8298

/** LC Light OnOff Get request */
#define LC_LIGHT_ONOFF_GET       0x8299
/** LC Light OnOff Set request */
#define LC_LIGHT_ONOFF_SET       0x829A
/** LC Light OnOff Set Unacknowledged request */
#define LC_LIGHT_ONOFF_SET_UNACK       0x829B
/** LC Light OnOff Status message */
#define LC_LIGHT_ONOFF_STATUS    0x829C

/** LC Property Get request */
#define LC_PROPERTY_GET       0x829D
/** LC Property Set request */
#define LC_PROPERTY_SET       0x62
/** LC Property Set Unacknowledged request */
#define LC_PROPERTY_SET_UNACK       0x63
/** LC Property Status message */
#define LC_PROPERTY_STATUS    0x64

/*
 * Lighting model opcodes
 */

/** Lighting Lightness Actual Get */
#define LIGHTING_LIGHTNESS_ACTUAL_GET 0x824b
/** Lighting Lightness Actual Set */
#define LIGHTING_LIGHTNESS_ACTUAL_SET 0x824c
/** Lighting Lightness Actual Set Unreliable */
#define LIGHTING_LIGHTNESS_ACTUAL_SET_UNRL 0x824d
/** Lighting Lightness Actual Status */
#define LIGHTING_LIGHTNESS_ACTUAL_STATUS 0x824e
/** Lighting Lightness Linear Get */
#define LIGHTING_LIGHTNESS_LINEAR_GET 0x824f
/** Lighting Lightness Linear Set */
#define LIGHTING_LIGHTNESS_LINEAR_SET 0x8250
/** Lighting Lightness Linear Set Unreliable */
#define LIGHTING_LIGHTNESS_LINEAR_SET_UNRL 0x8251
/** Lighting Lightness Linear Status */
#define LIGHTING_LIGHTNESS_LINEAR_STATUS 0x8252
/** Lighting Lightness Last Get */
#define LIGHTING_LIGHTNESS_LAST_GET 0x8253
/** Lighting Lightness Last Status */
#define LIGHTING_LIGHTNESS_LAST_STATUS 0x8254
/** Lighting Lightness Default Get */
#define LIGHTING_LIGHTNESS_DEFAULT_GET 0x8255
/** Lighting Lightness Default Set */
#define LIGHTING_LIGHTNESS_DEFAULT_SET 0x8259
/** Lighting Lightness Default Set Unreliable */
#define LIGHTING_LIGHTNESS_DEFAULT_SET_UNRL 0x825a
/** Lighting Lightness Default Status */
#define LIGHTING_LIGHTNESS_DEFAULT_STATUS 0x8256
/** Lighting Lightness Range Get */
#define LIGHTING_LIGHTNESS_RANGE_GET 0x8257
/** Lighting Lightness Range Set */
#define LIGHTING_LIGHTNESS_RANGE_SET 0x825b
/** Lighting Lightness Range Set Unreliable */
#define LIGHTING_LIGHTNESS_RANGE_SET_UNRL 0x825c
/** Lighting Lightness Range Status */
#define LIGHTING_LIGHTNESS_RANGE_STATUS 0x8258

/** Lighting CTL Get */
#define LIGHTING_CTL_GET 0x825d
/** Lighting CTL Set */
#define LIGHTING_CTL_SET 0x825e
/** Lighting CTL Set Unreliable */
#define LIGHTING_CTL_SET_UNRL 0x825f
/** Lighting CTL Status */
#define LIGHTING_CTL_STATUS 0x8260
/** Lighting CTL Temperature Get */
#define LIGHTING_CTL_TEMP_GET 0x8261
/** Lighting CTL Temperature Range Get */
#define LIGHTING_CTL_TEMP_RANGE_GET 0x8262
/** Lighting CTL Temperature Range Status */
#define LIGHTING_CTL_TEMP_RANGE_STATUS 0x8263
/** Lighting CTL Temperature Set */
#define LIGHTING_CTL_TEMP_SET 0x8264
/** Lighting CTL Temperature Set Unreliable */
#define LIGHTING_CTL_TEMP_SET_UNRL 0x8265
/** Lighting CTL Temperature Status */
#define LIGHTING_CTL_TEMP_STATUS 0x8266
/** Lighting CTL Temperature Default Get */
#define LIGHTING_CTL_DEFAULT_GET 0x8267
/** Lighting CTL Temperature Default Status */
#define LIGHTING_CTL_DEFAULT_STATUS 0x8268
/** Lighting CTL Temperature Default Set */
#define LIGHTING_CTL_DEFAULT_SET 0x8269
/** Lighting CTL Temperature Default Set Unreliable */
#define LIGHTING_CTL_DEFAULT_SET_UNRL 0x826a
/** Lighting CTL Temperature Range Set */
#define LIGHTING_CTL_TEMP_RANGE_SET 0x826b
/** Lighting CTL Temperature Range Set Unreliable */
#define LIGHTING_CTL_TEMP_RANGE_SET_UNRL 0x826c

/** Lighting HSL Get */
#define LIGHTING_HSL_GET 0x826d
/** Lighting HSL Set */
#define LIGHTING_HSL_SET 0x8276
/** Lighting HSL Set Unreliable */
#define LIGHTING_HSL_SET_UNRL 0x8277
/** Lighting HSL Status */
#define LIGHTING_HSL_STATUS 0x8278
/** Lighting HSL Hue Get */
#define LIGHTING_HSL_HUE_GET 0x826e
/** Lighting HSL Hue Set */
#define LIGHTING_HSL_HUE_SET 0x826f
/** Lighting HSL Hue Set Unreliable */
#define LIGHTING_HSL_HUE_SET_UNRL 0x8270
/** Lighting HSL Hue Status */
#define LIGHTING_HSL_HUE_STATUS 0x8271
/** Lighting HSL Saturation Get */
#define LIGHTING_HSL_SATURATION_GET 0x8272
/** Lighting HSL Saturation Set */
#define LIGHTING_HSL_SATURATION_SET 0x8273
/** Lighting HSL Saturation Set Unreliable */
#define LIGHTING_HSL_SATURATION_SET_UNRL 0x8274
/** Lighting HSL Saturation Status */
#define LIGHTING_HSL_SATURATION_STATUS 0x8275
/** Lighting HSL Target Get */
#define LIGHTING_HSL_TARGET_GET 0x8279
/** Lighting HSL Target Status */
#define LIGHTING_HSL_TARGET_STATUS 0x827a
/** Lighting HSL Default Get */
#define LIGHTING_HSL_DEFAULT_GET 0x827b
/** Lighting HSL Default Set */
#define LIGHTING_HSL_DEFAULT_SET 0x827f
/** Lighting HSL Default Set Unreliable */
#define LIGHTING_HSL_DEFAULT_SET_UNRL 0x8280
/** Lighting HSL Default Status */
#define LIGHTING_HSL_DEFAULT_STATUS 0x827c
/** Lighting HSL Range Get */
#define LIGHTING_HSL_RANGE_GET 0x827d
/** Lighting HSL Range Set */
#define LIGHTING_HSL_RANGE_SET 0x8281
/** Lighting HSL Range Set Unreliable */
#define LIGHTING_HSL_RANGE_SET_UNRL 0x8282
/** Lighting HSL Range Status */
#define LIGHTING_HSL_RANGE_STATUS 0x827e

/**
 * Enumeration describing attributes a model supports.
 * Multiple bits can be supported.
 */
typedef enum {
  sl_btmesh_model_attributes_none       =                      0, /**< No attribute flags */
  sl_btmesh_model_supports_subscription =               (1 << 0), /**< Subscription supported */
  sl_btmesh_model_supports_publication  =               (1 << 1), /**< Publication supported */
} sl_btmesh_model_attributes_bitmask_t;

/**
 * @brief Check supported attributes bitmask for Bluetooth SIG model
 *
 * @param model_id Model ID of the model to check
 * @param attributes If model is recognized, attributes are written here
 * @return SL_STATUS_OK if model is recognized, SL_STATUS_NOT_SUPPORTED otherwise
 */
sl_status_t mesh_stack_bt_sig_model_attributes(uint16_t model_id,
                                               sl_btmesh_model_attributes_bitmask_t *attributes);

#endif
