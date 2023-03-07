/******************************************************************************/
/**
 * @file   sl_btmesh_generic_model_capi_types.h
 * @brief  Silicon Labs Bluetooth Mesh Generic Model API
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

/*
 * C API for generic models
 */

#ifndef SL_BTMESH_GENERIC_MODEL_CAPI_TYPES_H
#define SL_BTMESH_GENERIC_MODEL_CAPI_TYPES_H

#include "sl_btmesh_capi_types.h"
#include "sl_btmesh_model_specification_defs.h"

/** Model status */
typedef enum {
  mesh_model_status_success = 0x00,
  mesh_model_status_cannot_set_range_min = 0x01,
  mesh_model_status_cannot_set_range_max = 0x02,
} mesh_model_status_t;

/*
 * Generic model states
 *
 * Generic models contain cached values representing the actual
 * implementation state. The implementation is responsible for
 * updating the cache when the implementation state changes --
 * including at initialization -- and is also responsible for
 * responding to requests for setting the state (requests for getting
 * the state are handled directly from the cache)
 */

/** Generic on/off state value off */
#define MESH_GENERIC_ON_OFF_STATE_OFF 0x00
/** Generic on/off state value on */
#define MESH_GENERIC_ON_OFF_STATE_ON 0x01
/** Generic on/off state */
struct mesh_generic_on_off_state {
  /** The state can be either #MESH_GENERIC_ON_OFF_STATE_OFF or
   #MESH_GENERIC_ON_OFF_STATE_ON */
  uint8_t on;
};

/** Generic on power up state value off */
#define MESH_GENERIC_ON_POWER_UP_STATE_OFF 0x00
/** Generic on power up state value on */
#define MESH_GENERIC_ON_POWER_UP_STATE_ON 0x01
/** Generic on power up state value restore previous state */
#define MESH_GENERIC_ON_POWER_UP_STATE_RESTORE 0x02
/** Generic on power up state */
struct mesh_generic_on_power_up_state {
  /** The state can be either #MESH_GENERIC_ON_POWER_UP_STATE_OFF,
   #MESH_GENERIC_ON_POWER_UP_STATE_ON, or
   #MESH_GENERIC_ON_POWER_UP_STATE_RESTORE */
  uint8_t on_power_up;
};

/** Generic level state */
struct mesh_generic_level_state {
  /** By default the level range is from -32768 to 32767,
      but this can be restricted for a model by calling
      mesh_generic_level_server_cap() or
      mesh_generic_level_server_wrap() */
  int16_t level;
};

/** Generic power level state */
struct mesh_generic_power_level_state {
  /** The power level range is from 0 (off) to 65535 (maximum power) */
  uint16_t level;
};

/** Generic power level last state */
struct mesh_generic_power_level_last_state {
  /** The power level last range is from 1 (minimum power) to
      65535 (maximum power); value of 0 is not allowed. */
  uint16_t level;
};

/** Generic power default state */
struct mesh_generic_power_level_default_state {
  /** The power default value can range from 1 (minimum power) to
      65535 (maximum power); value of 0 is allowed and has the
      special meaning of using the generic power level last state. */
  uint16_t level;
};

/** Generic power range status success */
#define MESH_GENERIC_POWER_RANGE_STATUS_SUCCESS 0x00
/** Generic power range status cannot set minimum */
#define MESH_GENERIC_POWER_RANGE_STATUS_CANNOT_SET_MIN 0x01
/** Generic power range status cannot set maximum */
#define MESH_GENERIC_POWER_RANGE_STATUS_CANNOT_SET_MAX 0x02

/** Generic power range state */
struct mesh_generic_power_level_range_state {
  /** The minimum power level ranges from 1 to
      65535; value of 0 is not allowed. */
  uint16_t min;
  /** The maximum power level ranges from 1 to
      65535; value of 0 is not allowed. */
  uint16_t max;
  /** Range set status */
  uint8_t status;
};

/** Generic default transition time state */
struct mesh_generic_transition_time_state {
  /** The transition time is divided into two fields. The high 2 bits
      indicate unit resolution (0b00 = 100ms, 0b01 = 1s, 0b10 = 10s, 0b11 =
      10min) while the low 6 bits are the number of units. For instance,
      a value of 0x85 would mean 5*10s = 50s. */
  uint8_t time;
};

/** Generic battery state */
struct mesh_generic_battery_state {
  /** The allowed battery level values are either in the range of
      0 to 100, indicating battery charge percentage, or 0xff for
      unknown charge level; other values are not allowed. */
  uint8_t level;
  /** Discharge time is a 24-bit unsigned value representing
      battery discharge time in minutes; the value of 0xffffff
      represents unknown discharging time */
  uint8_t discharge_time[3];
  /** Discharge time is a 24-bit unsigned value representing
      battery charge time in minutes; the value of 0xffffff
      represents unknown charging time */
  uint8_t charge_time[3];
  /** Battery flags are divided into four fields:

   * Bits 0 and 1
     represent battery presence (0b00 = not present, 0b01 =
     present and removable, 0b10 = present and nonremovable,
     0b11 = unknown);

   * Bits 2 and 3 represent a simple level
     indicator (0b00 = critically low, 0b01 = low level,
     0b10 = good level, 0b11 = unknown);

   * Bits 4 and 5
     represent charging indicator (0b00 = not chargeable,
     0b01 = chargeable, not charging, 0b10 = chargeable,
     charging, 0b11 = unknown);

   * Bits 6 and 7 represent
     battery serviceability (0b10 = service not required,
     0b10 = service required, 0b11 = unknown)
   */
  uint8_t flags;
};

/** Generic global location state */
struct mesh_generic_location_global_state {
  /** Latitude; see the Mesh model specification for encoding */
  int32_t lat;
  /** Longitude; see the Mesh model specification for encoding */
  int32_t lon;
  /** Altitude in metres; value of 0x7ffe means 32766 or higher,
     while a value of 0x7fff means altitude not configured. */
  int16_t alt;
};

/** Generic local location state */
struct mesh_generic_location_local_state {
  /** Local coordinate North-South axis offset from origin in decimetres */
  int16_t north;
  /** Local coordinate East-West axis offset from origin in decimetres */
  int16_t east;
  /** Local altitude in decimetres; value of 0x7ffe means 32766 or higher,
      while a value of 0x7fff means altitude not configured. */
  int16_t alt;
  /** Floor number + 20, with the following special cases
   *
   * Value of 0 indicates floor -20 or lower;
   *
   * Value of 252 indicates floor 232 or above.
   *
   * Value of 253 indicates ground floor (floor 0)
   *
   * Value of 254 indicates ground floor (floor 1)
   *
   * Value of 255 indicates floor not configured. */
  uint8_t  floor;
  /** Location uncertainty; see the Mesh model specification
      for encoding details */
  uint16_t uncertainty;
};

/** Generic property type */
typedef enum {
  /** User property */
  mesh_generic_property_type_user = 0x00,
  /** Admin property */
  mesh_generic_property_type_admin = 0x01,
  /** Manufacturer property */
  mesh_generic_property_type_manuf = 0x02,
  /** Client property */
  mesh_generic_property_type_client = 0x03,
} mesh_generic_property_type;

/** Invalid property ID */
#define MESH_GENERIC_PROPERTY_ID_INVALID 0x0000

/** Generic property list state */
struct mesh_generic_property_list_state {
  /** Length of property list data */
  uint16_t length;
  /** Offset of property list data */
  uint16_t offset;
  /** Handle to a buffer in which property list data is contained */
  const uint8_t *buffer;
};

/** Generic property state */
struct mesh_generic_property_state {
  /** Property ID */
  uint16_t id;
  /** Property access control */
  uint8_t access;
  /** Length of property data */
  uint16_t length;
  /** Offset of property data */
  uint16_t offset;
  /** Handle to a buffer in which property data is contained */
  const uint8_t *buffer;
};

/** Light lightness state */
struct mesh_lighting_lightness_state {
  uint16_t level; /**< Lightness level */
};

/** Light lightness range state */
struct mesh_lighting_lightness_range_state {
  uint16_t min; /**< Minimum lightness */
  uint16_t max; /**< Maximum lightness */
  uint8_t status; /**< Status of previous range operation */
};

/** Light CTL state */
struct mesh_lighting_ctl_state {
  uint16_t lightness; /**< Lightness level */
  uint16_t temperature; /**< Color temperature */
  int16_t deltauv; /**< Delta UV */
};

/** Light CTL temperature + deltauv state */
struct mesh_lighting_ctl_temperature_state {
  uint16_t temperature; /**< Color temperature */
  int16_t deltauv; /**< Delta UV */
};

/** Light CTL lightness + temperature state */
struct mesh_lighting_ctl_lightness_temperature_state {
  uint16_t lightness; /**< Lightness level */
  uint16_t temperature; /**< Color temperature */
};

/** Light CTL range state */
struct mesh_lighting_ctl_temperature_range_state {
  uint16_t min; /**< Minimum temperature */
  uint16_t max; /**< Maximum temperature */
  uint8_t status; /**< Status of previous range operation */
};

/** Light HSL, HSL target, HSL default state */
struct mesh_lighting_hsl_state {
  uint16_t lightness; /**< Lightness value */
  uint16_t hue; /**< Hue value */
  uint16_t saturation; /**< Saturation value */
};

/** Light HSL hue state */
struct mesh_lighting_hsl_hue_state {
  uint16_t hue;   /**< Hue value */
};

/** Light HSL saturation state */
struct mesh_lighting_hsl_saturation_state {
  uint16_t saturation; /**< Saturation value */
};

/** Light HSL range state */
struct mesh_lighting_hsl_range_state {
  uint16_t hue_min; /**< Minimum hue value */
  uint16_t hue_max; /**< Maximum hue value */
  uint16_t saturation_min; /**< Minimum saturation value */
  uint16_t saturation_max; /**< Maximum saturation value */
  uint8_t status; /**< Status of previous range operation */
};

/** Generic state type */
typedef enum {
  mesh_generic_state_on_off = 0x00,
  mesh_generic_state_on_power_up = 0x01,
  mesh_generic_state_level = 0x02,
  mesh_generic_state_power_level = 0x03,
  mesh_generic_state_power_level_last = 0x04,
  mesh_generic_state_power_level_default = 0x05,
  mesh_generic_state_power_level_range = 0x06,
  mesh_generic_state_transition_time = 0x07,
  mesh_generic_state_battery = 0x08,
  mesh_generic_state_location_global = 0x09,
  mesh_generic_state_location_local = 0x0a,
  mesh_generic_state_property_user = 0x0b,
  mesh_generic_state_property_admin = 0x0c,
  mesh_generic_state_property_manuf = 0x0d,
  mesh_generic_state_property_list_user = 0x0e,
  mesh_generic_state_property_list_admin = 0x0f,
  mesh_generic_state_property_list_manuf = 0x10,
  mesh_generic_state_property_list_client = 0x11,

  mesh_lighting_state_lightness_actual = 0x80,
  mesh_lighting_state_lightness_linear = 0x81,
  mesh_lighting_state_lightness_last = 0x82,
  mesh_lighting_state_lightness_default = 0x83,
  mesh_lighting_state_lightness_range = 0x84,

  mesh_lighting_state_ctl = 0x85,
  mesh_lighting_state_ctl_temperature = 0x86,
  mesh_lighting_state_ctl_default = 0x87,
  mesh_lighting_state_ctl_temperature_range = 0x88,
  mesh_lighting_state_ctl_lightness_temperature = 0x89,

  mesh_lighting_state_hsl = 0x8a,
  mesh_lighting_state_hsl_hue = 0x8b,
  mesh_lighting_state_hsl_saturation = 0x8c,
  mesh_lighting_state_hsl_default = 0x8d,
  mesh_lighting_state_hsl_range = 0x8e,
  mesh_lighting_state_hsl_target = 0x8f,

  mesh_lighting_state_lightness_range_with_status = 0x90,
  mesh_lighting_state_ctl_temperature_range_with_status = 0x91,
  mesh_lighting_state_hsl_range_with_status = 0x92,

  mesh_generic_state_last
} mesh_generic_state_t;

/** Generic state */
struct mesh_generic_state {
  /** Type of state */
  mesh_generic_state_t kind;
  /** State data */
  union {
    struct mesh_generic_on_off_state on_off;
    struct mesh_generic_on_power_up_state on_power_up;
    struct mesh_generic_level_state level;
    struct mesh_generic_power_level_state power_level;
    struct mesh_generic_power_level_last_state power_level_last;
    struct mesh_generic_power_level_default_state power_level_default;
    struct mesh_generic_power_level_range_state power_level_range;
    struct mesh_generic_transition_time_state transition_time;
    struct mesh_generic_battery_state battery;
    struct mesh_generic_location_global_state location_global;
    struct mesh_generic_location_local_state location_local;
    struct mesh_generic_property_state property;
    struct mesh_generic_property_list_state property_list;

    struct mesh_lighting_lightness_state lightness;
    struct mesh_lighting_lightness_range_state lightness_range;

    /** Light CTL state used for updating
        mesh_lighting_state_ctl, and
        mesh_lighting_state_ctl_default */
    struct mesh_lighting_ctl_state ctl;
    /** Light CTL state used for updating
        mesh_lighting_state_ctl_temperature */
    struct mesh_lighting_ctl_temperature_state ctl_temperature;
    /** Light CTL state used for updating
        mesh_lighting_state_ctl_temperature_range */
    struct mesh_lighting_ctl_temperature_range_state ctl_temperature_range;
    /** Light CTL state used for updating
        mesh_lighting_state_ctl_lightness_temperature */
    struct mesh_lighting_ctl_lightness_temperature_state ctl_lightness_temperature;

    /** Light HSL state used for updating/getting mesh_lighting_state_hsl,
        mesh_lighting_state_hsl_target and mesh_lighting_state_hsl_default */
    struct mesh_lighting_hsl_state hsl;

    /** Light HSL hue state used for updating mesh_lighting_state_hsl_hue */
    struct mesh_lighting_hsl_hue_state hsl_hue;

    /** Light HSL saturation state used for updating
        mesh_lighting_state_hsl_saturation */
    struct mesh_lighting_hsl_saturation_state hsl_saturation;

    /** Light HSL range state used for updating
        mesh_lighting_state_hsl_range */
    struct mesh_lighting_hsl_range_state hsl_range;
  };
};

/** Generic request type */
typedef enum {
  mesh_generic_request_on_off = 0x00,
  mesh_generic_request_on_power_up = 0x01,
  mesh_generic_request_level = 0x02,
  mesh_generic_request_level_delta = 0x03,
  mesh_generic_request_level_move = 0x04,
  mesh_generic_request_level_halt = 0x05,
  mesh_generic_request_power_level = 0x06,
  // last level cannot be set
  mesh_generic_request_power_level_default = 0x07,
  mesh_generic_request_power_level_range = 0x08,
  mesh_generic_request_transition_time = 0x09,
  // battery information cannot be set
  mesh_generic_request_location_global = 0x0a,
  mesh_generic_request_location_local = 0x0b,
  mesh_generic_request_property_user = 0x0c,
  mesh_generic_request_property_admin = 0x0d,
  mesh_generic_request_property_manuf = 0x0e,

  mesh_lighting_request_lightness_actual = 0x80,
  mesh_lighting_request_lightness_linear = 0x81,
  mesh_lighting_request_lightness_default = 0x82,
  mesh_lighting_request_lightness_range = 0x83,

  mesh_lighting_request_ctl = 0x84,
  mesh_lighting_request_ctl_temperature = 0x85,
  mesh_lighting_request_ctl_default = 0x86,
  mesh_lighting_request_ctl_temperature_range = 0x87,

  mesh_lighting_request_hsl = 0x88,
  mesh_lighting_request_hsl_hue = 0x89,
  mesh_lighting_request_hsl_saturation = 0x8a,
  mesh_lighting_request_hsl_default = 0x8b,
  mesh_lighting_request_hsl_range = 0x8c,
} mesh_generic_request_t;

/** Generic request */
struct mesh_generic_request {
  /** Request type */
  mesh_generic_request_t kind;
  /** Request data */
  union {
    /** On/off state change request data; either
     #MESH_GENERIC_ON_OFF_STATE_OFF or
     #MESH_GENERIC_ON_OFF_STATE_ON */
    uint8_t on_off;
    /** On power up state change request data; can be either
     #MESH_GENERIC_ON_POWER_UP_STATE_OFF,
     #MESH_GENERIC_ON_POWER_UP_STATE_ON, or
     #MESH_GENERIC_ON_POWER_UP_STATE_RESTORE */
    uint8_t on_power_up;
    /** Generic level state change request data for mesh_generic_request_level
        and mesh_generic_request_level_move types */
    int16_t level;
    /** Generic level state change request data for
        mesh_generic_request_level_delta type */
    int32_t delta;
    /** Generic power level state change request data for
        mesh_generic_request_power_level and
        mesh_generic_request_power_level_default */
    uint16_t power_level;
    /** Generic power range state change request data for
        mesh_generic_request_power_level_range */
    uint16_t power_range[2];
    /** Generic transition time state change request data for
        mesh_generic_request_transition_time */
    uint8_t transition_time;
    /** Generic global location state change request data for
        mesh_generic_request_location_global */
    struct {
      /** Latitude */
      int32_t lat;
      /** Longitude */
      int32_t lon;
      /** Altitude */
      int16_t alt;
    } location_global;
    /** Generic local location state change request data for
        mesh_generic_request_location_local */
    struct {
      /** Local north-south offset from origin */
      int16_t north;
      /** Local east-west offset from origin */
      int16_t east;
      /** Local altitude */
      int16_t alt;
      /** Floor */
      uint8_t floor;
      /** Location uncertainty */
      uint16_t uncertainty;
    } location_local;
    /** Generic property state change request data for
        mesh_generic_request_property_user,
        mesh_generic_request_property_admin, and
        mesh_generic_request_property_manuf */
    struct {
      /** Property ID */
      uint16_t id;
      /** Access control */
      uint8_t access;
      /** Property data length */
      uint16_t length;
      /** Property data offset */
      uint16_t offset;
      /** Handle to property data buffer */
      const uint8_t *buffer;
    } property;

    /** Light lightness state change request data for
        mesh_lighting_request_lightness_actual,
        mesh_lighting_request_lightness_linear,
        and mesh_lighting_request_lightness_default */
    uint16_t lightness;
    /** Light lightness range state change request data for
        mesh_lighting_request_lightness_range */
    struct {
      /** Minimum lightness */
      uint16_t min;
      /** Maximum lightness */
      uint16_t max;
    } lightness_range;

    /** Light CTL state change request data for
        mesh_lighting_request_ctl, and
        mesh_lighting_request_ctl_default */
    struct {
      uint16_t lightness;
      uint16_t temperature;
      int16_t deltauv;
    } ctl;
    /** Light CTL state change request data for
        mesh_lighting_request_ctl_temperature */
    struct {
      uint16_t temperature;
      int16_t deltauv;
    } ctl_temperature;
    /** Light CTL range state change request data for
        mesh_lighting_request_ctl_temperature_range */
    struct {
      /** Minimum temperature */
      uint16_t min;
      /** Maximum temperature */
      uint16_t max;
    } ctl_temperature_range;
    /** Light HSL state change request data for
        mesh_lighting_request_hsl and mesh_lighting_request_hsl_default */
    struct {
      uint16_t lightness;
      uint16_t hue;
      uint16_t saturation;
    } hsl;
    /** Light HSL hue state change request data for
        mesh_lighting_request_hsl_hue */
    uint16_t hsl_hue;
    /** Light HSL saturation state change request data for
        mesh_lighting_request_hsl_saturation */
    uint16_t hsl_saturation;
    /** Light HSL range state change request data for
        mesh_lighting_request_hsl_range */
    struct {
      /** Minimum hue */
      uint16_t hue_min;
      /** Maximum hue */
      uint16_t hue_max;
      /** Minimum saturation */
      uint16_t saturation_min;
      /** Maximum saturation */
      uint16_t saturation_max;
    } hsl_range;
  };
};

/** Use default transition time */
#define MESH_GENERIC_DEFAULT_TRANS_MS 0xffffffff

/**
 * Transaction based request history record
 */
PACKSTRUCT(struct mesh_generic_history_record {
  mesh_tick_t tstamp;   /**< When received */
  mesh_addr_t src; /**< Sent from */
  mesh_addr_t dst; /**< Sent to */
  uint8_t tid; /**< Transaction ID */
  unsigned aborted: 1; /**< Aborted transaction */
});

/** Per model transaction history length */
#define MESH_GENERIC_HISTORY_MAX 8

/**
 * Transaction based request history array
 */
PACKSTRUCT(struct mesh_generic_history {
  struct mesh_generic_history_record rec[MESH_GENERIC_HISTORY_MAX];  /**< Request history */
  uint8_t len; /**< Valid items in history */
});

/**
 * API for handling state binding between LC and Generic On/Off model
 */
typedef void (*mesh_lc_light_onoff_binding_fn) (mesh_model_t model,
                                                uint8_t onoff_value);

#endif /* SL_BTMESH_GENERIC_MODEL_CAPI_TYPES_H */
