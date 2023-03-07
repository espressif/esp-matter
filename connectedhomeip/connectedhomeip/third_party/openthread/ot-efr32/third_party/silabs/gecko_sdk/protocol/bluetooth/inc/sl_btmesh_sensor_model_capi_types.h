/******************************************************************************/
/**
 * @file   sl_btmesh_sensor_model_capi_types.h
 * @brief  Silicon Labs Bluetooth Mesh Sensor Model API
 *
 *******************************************************************************
 * <b> (C) Copyright 2018 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 ******************************************************************************/

#ifndef SL_BTMESH_SENSOR_MODEL_CAPI_TYPES_H
#define SL_BTMESH_SENSOR_MODEL_CAPI_TYPES_H

#include "stdint.h"
#include "sl_btmesh_device_properties.h"
#include "sl_btmesh_model_specification_defs.h"

/** Sensor model events */
typedef enum mesh_sensor_event_type {
  /** Sensor Descriptor Get acknowledged client request to server. */
  MESH_SENSOR_EVENT_GET_DESCRIPTOR = 1,
  /** Sensor Descriptor Status server response to Descriptor Get request */
  MESH_SENSOR_EVENT_DESCRIPTOR_STATUS,

  /** Sensor Cadence Get acknowledged client request to setup server*/
  MESH_SENSOR_EVENT_GET_CADENCE,
  /** Sensor Cadence Set acknowledged client request to setup server
   * Unacknowledged request is done using message flags*/
  MESH_SENSOR_EVENT_SET_CADENCE,
  /** Sensor Cadence Set unacknowledged client request to setup server */
  MESH_SENSOR_EVENT_CADENCE_STATUS,

  /** Sensor Server Settings Get acknowledged client request to setup server */
  MESH_SENSOR_EVENT_GET_SETTINGS,
  /** Sensor Settings Status setup server response to Settings Get request or unsolicited message */
  MESH_SENSOR_EVENT_SETTINGS_STATUS,

  /** Sensor Setting Get acknowledged client request to setup server */
  MESH_SENSOR_EVENT_GET_SETTING,
  /** Sensor Setting Set acknowledged client request to setup server server
   * Unacknowledged request is done using message flags */
  MESH_SENSOR_EVENT_SET_SETTING,
  /** Sensor Setting Status setup server response to Setting Get/Set requests */
  MESH_SENSOR_EVENT_SETTING_STATUS,

  /** Sensor Get acknowledged client request to server */
  MESH_SENSOR_EVENT_GET,
  /** Sensor Status server response to Sensor Get requests or unsolicited message */
  MESH_SENSOR_EVENT_STATUS,

  /** Sensor Column Get acknowledged client request to server */
  MESH_SENSOR_EVENT_GET_COLUMN,
  /** Sensor Column Status server response to Get Column requests */
  MESH_SENSOR_EVENT_COLUMN_STATUS,

  /** Sensor Series Get acknowledged client request to server */
  MESH_SENSOR_EVENT_GET_SERIES,
  /** Sensor Series Status server response to Series Get requests */
  MESH_SENSOR_EVENT_SERIES_STATUS,

  /** Publishing period timer elapsed, state should be reported */
  MESH_SENSOR_EVENT_PUBLISH
} mesh_sensor_event_type_t;

/**
 * Sensor Server message flags to indicate that a response is required
 * Used for Set Setting and Set Cadence messages where there are
 * acknowledged and unacknowledged versions differentiated by opcode
 */
#define SENSOR_SERVER_MSG_RESPONSE_REQUIRED 0x02

/** Sensor Property ID, 16-bit ID referencing a device property */
typedef uint16_t sensor_property_id_t;
/** Sensor Property ID 0x0000 is prohibited */
#define SENSOR_PROPERY_ID_INVALID DEVICE_PROPERTY_INVALID

/*
 * Sensor parameters
 */

/** 12-bit Positive Tolerance value (0x001 - 0xFFF), the value is derived as Possible Positive Error % = 100 % * Positive Tolerance / 4095 */
typedef uint16_t sensor_positive_tolerance_t;

/** 12-bit Negative Tolerance value (0x001 - 0xFFF), the value is derived as Possible Negative Error % = 100 % * Negative Tolerance / 4095 */
typedef uint16_t sensor_negative_tolerance_t;
/** Sensor Tolerance not specified */
#define TOLERANCE_UNSPECIFIED 0

/** Sensor Sampling Period, 8-bit value */
typedef uint8_t sensor_sampling_function_t;
/** Sensor Sampling Function values */
enum sensor_sampling_function_e {
  /** Unspecified */
  SAMPLING_UNSPECIFIED     = 0x00,
  /** Instantaneous sampling */
  SAMPLING_INSTANTANEOUS   = 0x01,
  /** Arithmetic mean */
  SAMPLING_ARITHMETIC_MEAN = 0x02,
  /** Root mean square */
  SAMPLING_RMS             = 0x03,
  /** Maximum value */
  SAMPLING_MAXIMUM         = 0x04,
  /** Minimum value */
  SAMPLING_MINIMUM         = 0x05,
  /** Cumulative moving average updated with the frequency given by Sensor Update Interval */
  SAMPLING_ACCUMULATED     = 0x06,  //
  /** Number of "events" over the period of time defined by the Measurement Period */
  SAMPLING_COUNT           = 0x07,
  /** Values starting from 0x08 are Reserved for Future Use */
  SAMPLING_RFU             = 0x08
};

/**
 * Sensor Measurement Period
 * 8-bit value n representing the averaging time span, accumulation time or measurement period in seconds
 * Represented value = 1.1ˆ(n-64), ranging from approximately 2.5ms to about 932 days.
 * Value zero means undefined, definitive values start from 1
 */
typedef uint8_t sensor_measurement_period_t;

/** Sensor measurement period unspecified */
#define MEASUREMENT_PERIOD_UNDEFINED 0

/**
 * Sensor Update Interval
 * 8-bit value representing the frequency at which the measurement value is updated internally
 * Represented value = 1.1ˆ(n-64), ranging from approximately 2.5ms to about 932 days.
 * Value zero means undefined, definitive values start from 1
 */
typedef uint8_t sensor_update_interval_t;
/** Sensor update interval unspecified */
#define UPDATE_INTERVAL_UNDEFINED 0

/**
 *  @brief Sensor Descriptor
 *  Sensor Descriptor
 *
 *  The Sensor Descriptor state represents the attributes describing the sensor data. This state does not change throughout the lifetime of an element.
 *  One sensor model can have multiple Sensor Descriptor states in case of a multisensor setup.
 */
typedef struct sensor_descriptor {
  /** Sensor Property ID */
  sensor_property_id_t property_id;
  /** Sensor Positive Tolerance */
  sensor_positive_tolerance_t positive_tolerance;
  /** Sensor Negative Tolerance */
  sensor_negative_tolerance_t negative_tolerance;
  /** Sensor Sampling Function */
  sensor_sampling_function_t sampling_function;
  /** Sensor Measurement Period */
  sensor_measurement_period_t measurement_period;
  /** Sensor Update Interval */
  sensor_update_interval_t update_interval;
} sensor_descriptor_t;

/*
 * Sensor setting state controlling the sensor's parameters
 */

/** Property ID of a setting within a sensor */
typedef uint16_t sensor_setting_property_id_t;

/**
 * Setting Access control is essentially defined in the form of a two-bit bit-flag field,
 * Bit 0 (0x01) means read access, bit 1 (0x2) means write access
 * NOTE: only Read (01b) or Read&Write (11b) is permitted, all other values are prohibited.
 */
typedef uint8_t sensor_setting_access_t;
/** Sensor Setting access rights values */
enum sensor_setting_access_e {
  /** PROHIBITED                   = 0x00 */
  /** Setting Read Access */
  SETTING_ACCESS_READ            = 0x01,
  /** PROHIBITED "Write only flag" = 0x02, */
  /** Setting Read and Write Access */
  SETTING_ACCESS_READWRITE       = 0x03
                                   /** PROHIBITED                   = 0x04-0xFF */
};

/**
 * Sensor Setting state
 */
typedef struct sensor_setting {
  /** Device Property ID */
  sensor_property_id_t property_id;
  /** Sensor Setting Property ID */
  sensor_setting_property_id_t setting_property_id;
  /** Access level */
  sensor_setting_access_t setting_access;
  /** Setting raw value */
  uint8_t *raw_value;
} sensor_setting_t;

/*
 * Sensor Cadence definitions
 */

/**
 * Fast Cadence Period Divisor
 * 7-bit value on "n", defining a 2ˆn divisor for the normal publish period interval
 */
typedef uint8_t fast_cadence_period_divisor_t;

/**
 * Status Trigger Type
 * Specifies the value and unit for the trigger threshold given in Status Trigger Delta Up/Down fields
 * 1-bit value as follows:
 *  0b0 - the format is defined by Format Type specified for the characteristics of the Sensor Property ID
 *  0b1 - the format is UINT16, the trigger value represent a percentage change with the resolution of 0.01 percent;
 */
typedef uint8_t status_trigger_type_t;
/** Sensor Trigger Type constants */
enum status_trigger_type {
  STATUS_TRIGGER_TYPE_PREDEFINED = 0,
  STATUS_TRIGGER_TYPE_PERCENTAGE = 1
};

/**
 * Status Trigger Delta Down/Up
 * The Status Trigger Delta Down field shall control the negative/positive change of a measured quantity that triggers publication of a Sensor Status message.
 * The setting is calculated based on the value of the Status Trigger Type field:
 * If the value of the Status Trigger Type field is 0b0, the setting is calculated as defined by the Sensor Property ID state.
 * If the value of the Status Trigger Type field is 0b1, the setting is calculated using the following formula: represented value = value / 100
 */
typedef struct status_trigger_delta {
  /** Data length */
  uint16_t size;
  /** Trigger value */
  uint8_t *value;
} status_trigger_delta_t;

/**
 * The Status Min Interval field is a 1-octet value that shall control the minimum interval between publishing two consecutive Sensor Status messages.
 * The value is represented as 2n milliseconds. For example, the value 0x0A would represent an interval of 1024ms.
 * The valid range for the Status Min Interval is 0–26 and other values are Prohibited.
 */
typedef uint8_t min_interval_t;

/**
 * Fast Cadence Low/High
 * Defines the lower or upper boundary of the range of measured quantities when the publishing cadence is increased as defined by the Fast Cadence Period Divisor fiels.
 * The represented value is calculated as defined by the Sensor Property ID state.
 * NOTE: Lower bound can be set to a value higher than the higher bound. In such cases, the increased cadence occurs outside of the [low...high] range.
 */
typedef struct fast_cadence_bound_t {
  /** Number of bytes in the raw value */
  uint16_t size;
  /** Lower/Upper bound raw value */
  uint8_t *value;
} fast_cadence_bound_t;

/** Sensor Cadence state */
struct sensort_cadence_state {
  /** Sensor Property ID */
  sensor_property_id_t property_id;               // 16-bit value property id
  /** Fast Cadence Period Divisor */
  fast_cadence_period_divisor_t period_divisor;   // 7-bit value "n", giving 2ˆn divisor of the publish period interval
  /** Status Trigger Type */
  status_trigger_type_t status_trigger_type;      // 1-bit
  /** Status Trigger Delta Down */
  status_trigger_delta_t status_trigger_delta_down;
  /** Status Trigger Delta Up */
  status_trigger_delta_t status_trigger_delta_up;
  /** Status Min Interval */
  min_interval_t min_interval;
  /** Fast Cadence Low */
  fast_cadence_bound_t fast_cadence_low;
  /** Fast Cadence High */
  fast_cadence_bound_t fast_cadence_high;
};
/** Sensor Cadence state */
typedef struct sensor_cadence sensor_cadence_t;

/** Sensor measurement data */
struct sensor_data {
  /** Data length */
  uint16_t size;
  /** Raw data value */
  uint8_t value;
};
/** Sensor measurement data */
typedef struct sensor_data sensor_data_t;

/**
 * Sensor Descriptor Get
 * Message content definition for Get Descriptor request
 */
typedef struct sensor_descriptor_get {
  /** Sensor Property ID */
  uint16_t property_id_t;
} sensor_descriptor_get_t;

/**
 * Sensor Descriptor Status
 * Definition for Descriptor Status message sent by sensor server as a reply to Get Descriptor
 */
typedef struct sensor_descriptor_status_t {
  /** Sensor Descriptor array */
  sensor_descriptor_t *descriptor;
} sensor_descriptor_status_t;

/**
 * Sensor Settings Get
 * Message definition for Get Settings request
 */
typedef struct {
  /** Sensor Property ID */
  uint16_t property_id;
} sensor_get_settings_msg_t;

/**
 * Sensor Settings Status
 * Definition for Settings Status message sent by sensor server as a reply to Get Settings request
 */
typedef struct {
  /** Sensor Property ID */
  sensor_property_id_t property_id;
  /** Sensor Setting Property ID array */
  sensor_setting_property_id_t *setting_property;
} sensor_settings_status_t;

/**
 * Sensor Setting Get
 * Definition of Get Setting message content
 */
typedef struct {
  /** Sensor Property Id */
  uint16_t property_id;
  /** Sensor Setting Property ID */
  uint16_t setting_property_id;
} sensor_get_setting_msg_t;

/**
 * Sensor Setting Set
 * Definition of Set Setting message content
 */
typedef struct {
  /** Sensor Property ID */
  uint16_t property_id;
  /** Sensor Setting Property ID */
  uint16_t setting_property_id;
  /** Data length */
  uint16_t setting_raw_len;
  /** Raw setting value */
  uint8_t *setting_raw;
} sensor_set_setting_msg_t;

/**
 * Sensor Setting Status
 * Definition of Setting State message content sent by sensor server as a reply to Get Setting
 */
struct sensor_setting_status {
  /** Device Property Id */
  sensor_property_id_t property_id;
  /** Sensor Settings Property ID */
  sensor_setting_property_id_t settings_property_id;
  /** Access level */
  sensor_setting_access_t access;
  /** Setting raw value */
  uint8_t *setting_raw;
};
/**
 * Sensor Setting Status
 */
typedef struct sensor_setting_status sensor_setting_status_t;

/**
 * Sensor Cadence Get
 * Message content definition for Get Cadence request
 */
struct sensor_get_cadence_msg {
  /** Sensor Property ID */
  uint16_t property_id;
};
/**
 * Sensor Cadence Get
 */
typedef struct sensor_get_cadence_msg sensor_get_cadence_msg_t;

/**
 * Sensor Cadence Set request
 */
struct sensor_set_cadence_msg {
  /** Sensor Property ID */
  uint16_t sensor_property_id;
  /** Fast Cadence Period Divisor */
  uint8_t fast_cadence_period_divisor;
  /** Status Trigger Type */
  uint8_t status_trigger_type;
  /** Length of raw data */
  uint16_t cadence_setting_raw_len;
  /** Raw value of multiple variable length fields combined:
   *  - Status Trigger Delta Down
   *  - Status Trigger Delta Up
   *  - Status Min Interval
   *  - Fast Cadence Low
   *  - Fast Cadence High
   */
  uint8_t *cadence_setting_raw;
};
/**
 * Sensor Cadence Set request
 */
typedef struct sensor_set_cadence_msg sensor_set_cadence_msg_t;

/**
 * Sensor Cadence Status message
 */
typedef struct {
  /** Sensor Property ID */
  uint16_t sensor_property_id;
  /** Fast Cadence Period Divisor */
  uint8_t fast_cadence_period_divisor;
  /** Status Trigger Type */
  uint8_t status_trigger_type;
  /** Length of raw data */
  uint16_t cadence_setting_raw_len;
  /** Raw value of multiple variable length fields combined:
   *  - Status Trigger Delta Down
   *  - Status Trigger Delta Up
   *  - Status Min Interval
   *  - Fast Cadence Low
   *  - Fast Cadence High
   */
  uint8_t *cadence_setting_raw;
} sensor_cadence_status_t;

/**
 * Setup Setup Server messages
 */
typedef union sensor_setup_server_msg {
  /** Get Settings event */
  sensor_get_settings_msg_t settings_get_msg;
  /** Get Setting event */
  sensor_get_setting_msg_t setting_get_msg;
  /** Set Setting event */
  sensor_set_setting_msg_t setting_set_msg;
  /** Get Cadence event */
  sensor_get_cadence_msg_t cadence_get_msg;
  /** Set Cadence event */
  sensor_set_cadence_msg_t cadence_set_msg;
  /** Publishing period in milliseconds for publish event to indicate the next approximate event time */
  uint32_t publish_period;
} sensor_setup_server_msg_t;

/**
 * Sensor Client messages
 */
typedef union {
  /** Sensor Descriptor Status */
  sensor_descriptor_status_t descriptor_status;
  /** Sensor Settings Status */
  sensor_settings_status_t settings_status;
  /** Sensor Setting Status */
  sensor_setting_status_t setting_status;
  /** Sensor Cadence Status */
  sensor_cadence_status_t cadence_status;
  //sensor_status_t status;
  //sensor_column_status_t column_status;
  //sensor_series_status_t series_status;
  /** Publishing period in milliseconds for publish event to indicate the next approximate event time */
  uint32_t publish_period;
} sensor_client_msg_t;

#endif /* SL_BTMESH_SENSOR_MODEL_CAPI_TYPES_H */
