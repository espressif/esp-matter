/***************************************************************************//**
 * @brief Mesh sensor model helpers
 * @version 0.0.1
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
#include "stdint.h"
#include "math.h"

/* Select BGAPI flavor */
#if defined(MESH_LIB_HOST)
#include "gecko_bglib.h"
#include "host_gecko.h"
#else /* MESH_LIB_NATIVE */
#include "sl_btmesh_api.h"
#endif /* MESH_LIB_NATIVE */

#include "sl_btmesh_sensor.h"
#include "sl_btmesh_device_properties.h"
#include "sl_btmesh_sensor_model_capi_types.h"

/* int 8 */
static int8_t int8_from_buf(const uint8_t *ptr)
{
  return (int8_t)ptr[0];
}

static void int8_to_buf(uint8_t *ptr, int8_t n)
{
  ptr[0] = n & 0xff;
}

/* uint 8 */
static uint8_t uint8_from_buf(const uint8_t *ptr)
{
  return (uint8_t)ptr[0];
}

static void uint8_to_buf(uint8_t *ptr, uint8_t n)
{
  ptr[0] = n & 0xff;
}

/* int16 */
static int16_t int16_from_buf(const uint8_t *ptr)
{
  return ((int16_t)ptr[0]) | ((int16_t)ptr[1] << 8);
}

static void int16_to_buf(uint8_t *ptr, int16_t n)
{
  ptr[0] = n & 0xff;
  ptr[1] = (n >> 8) & 0xff;
}

/* uint16 */
static int16_t uint16_from_buf(const uint8_t *ptr)
{
  return ((int16_t)ptr[0]) | ((int16_t)ptr[1] << 8);
}

static void uint16_to_buf(uint8_t *ptr, uint16_t n)
{
  ptr[0] = n & 0xff;
  ptr[1] = (n >> 8) & 0xff;
}

/* uint 24 */
static uint32_t uint24_from_buf(const uint8_t *ptr)
{
  return ((uint32_t)ptr[0]) | ((uint32_t)ptr[1] << 8) | ((uint32_t)ptr[2] << 16);
}

static void uint24_to_buf(uint8_t *ptr, int32_t n)
{
  ptr[0] = n & 0xff;
  ptr[1] = (n >> 8) & 0xff;
  ptr[2] = (n >> 16) & 0xff;
}

#if 0
/* int32 */
static int32_t int32_from_buf(const uint8_t *ptr)
{
  return
    ((int32_t)ptr[0])
    | ((int32_t)ptr[1] << 8)
    | ((int32_t)ptr[2] << 16)
    | ((int32_t)ptr[3] << 24)
  ;
}

static void int32_to_buf(uint8_t *ptr, int32_t n)
{
  ptr[0] = n & 0xff;
  ptr[1] = (n >> 8) & 0xff;
  ptr[2] = (n >> 16) & 0xff;
  ptr[3] = (n >> 24) & 0xff;
}
#endif
/* uint32 */
static int32_t uint32_from_buf(const uint8_t *ptr)
{
  return
    ((uint32_t)ptr[0])
    | ((uint32_t)ptr[1] << 8)
    | ((uint32_t)ptr[2] << 16)
    | ((uint32_t)ptr[3] << 24)
  ;
}

static void uint32_to_buf(uint8_t *ptr, uint32_t n)
{
  ptr[0] = n & 0xff;
  ptr[1] = (n >> 8) & 0xff;
  ptr[2] = (n >> 16) & 0xff;
  ptr[3] = (n >> 24) & 0xff;
}

#if 0
/* float 32 */
static float float_from_buf(const uint8_t *ptr)
{
  uint32_t n =
    ((uint32_t)ptr[0])
    | ((uint32_t)ptr[1] << 8)
    | ((uint32_t)ptr[2] << 16)
    | ((uint32_t)ptr[3] << 24);
  return (float)n;
}

static void float_to_buf(uint8_t *ptr, float f)
{
  uint32_t n = (uint32_t)f;
  ptr[0] = n & 0xff;
  ptr[1] = (n >> 8) & 0xff;
  ptr[2] = (n >> 16) & 0xff;
  ptr[3] = (n >> 24) & 0xff;
}
#endif

uint8_t mesh_sensor_data_to_buf(uint16_t property_id, uint8_t *ptr, uint8_t *value)
{
  int ret = 2; // Initial length is the length of the property ID
  uint16_to_buf(ptr, property_id);
  ptr += 2;

  switch (property_id) {
    case PRESENT_AMBIENT_TEMPERATURE:
    case PRESENT_INDOOR_AMBIENT_TEMPERATURE:
    case PRESENT_OUTDOOR_AMBIENT_TEMPERATURE:
    case COLOR_RENDERING_INDEX_R9:
    case COLOR_RENDERING_INDEX_RA:
    {
      uint8_to_buf(ptr++, 1);
      int8_to_buf(ptr, *(int8_t*)value);
      ret += 2;
    } break;
    case PRESENCE_DETECTED:
    case CHROMATICITY_TOLERANCE:
    case INPUT_VOLTAGE_RIPPLE_SPECIFICATION:
    case LIGHT_CONTROL_REGULATOR_ACCURACY:
    case LUMEN_MAINTENANCE_FACTOR:
    case MOTION_SENSED:
    case MOTION_THRESHOLD:
    case OUTPUT_RIPPLE_VOLTAGE_SPECIFICATION:
    case PRESENT_DEVICE_OPERATING_EFFICIENCY:
    case PRESENT_INPUT_RIPPLE_VOLTAGE:
    case PRESENT_RELATIVE_OUTPUT_RIPPLE_VOLTAGE:
    {
      uint8_to_buf(ptr++, 1);
      uint8_to_buf(ptr++, *(uint8_t*)value);
      ret += 2;
    } break;
    case INITIAL_PLANCKIAN_DISTANCE:
    case PRESENT_PLANCKIAN_DISTANCE:
    {
      uint8_to_buf(ptr++, 2);
      int16_to_buf(ptr, *(int16_t*)value);
      ret += 3;
    } break;
    case INITIAL_CORRELATED_COLOR_TEMPERATURE:
    case PRESENT_CORRELATED_COLOR_TEMPERATURE:
    case PEOPLE_COUNT:
    case DEVICE_COUNTRY_OF_ORIGIN:
    case PRESENT_INPUT_CURRENT:
    case PRESENT_OUTPUT_CURRENT:
    case LUMINOUS_EFFICACY:
    case INITIAL_LUMINOUS_FLUX:
    case PRESENT_LUMINOUS_FLUX:
    case CENTER_BEAM_INTENSITY_AT_FULL_POWER:
    case LIGHT_CONTROL_LIGHTNESS_ON:
    case LIGHT_CONTROL_LIGHTNESS_PROLONG:
    case LIGHT_CONTROL_LIGHTNESS_STANDBY:
    case PRESENT_DEVICE_OPERATING_TEMPERATURE:
    case TIME_SINCE_MOTION_SENSED:
    case TIME_SINCE_PRESENCE_DETECTED:
    case PRESENT_INPUT_VOLTAGE:
    case PRESENT_OUTPUT_VOLTAGE:
    {
      uint8_to_buf(ptr++, 2);
      uint16_to_buf(ptr, *(uint16_t*)value);
      ret += 3;
    } break;
    /** 24-bit values */
    case TOTAL_DEVICE_OFF_ON_CYCLES:
    case TOTAL_DEVICE_POWER_ON_CYCLES:
    case DEVICE_DATE_OF_MANUFACTURE:
    case DEVICE_ENERGY_USE_SINCE_TURN_ON:
    case TOTAL_DEVICE_ENERGY_USE:
    case DEVICE_RUNTIME_SINCE_TURN_ON:
    case DEVICE_RUNTIME_WARRANTY:
    case TOTAL_DEVICE_POWER_ON_TIME:
    case TOTAL_DEVICE_RUNTIME:
    case TOTAL_LIGHT_EXPOSURE_TIME:
    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_ON:
    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_PROLONG:
    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_STANDBY:
    case LUMINOUS_ENERGY_SINCE_TURN_ON:
    case RELATIVE_RUNTIME_IN_A_CORRELATED_COLOR_TEMPERATURE_RANGE:
    case TOTAL_LUMINOUS_ENERGY:
    case LUMINOUS_EXPOSURE:
    case PRESENT_DEVICE_INPUT_POWER:
    case LIGHT_CONTROL_TIME_FADE:
    case LIGHT_CONTROL_TIME_FADE_ON:
    case LIGHT_CONTROL_TIME_FADE_STANDBY_AUTO:
    case LIGHT_CONTROL_TIME_FADE_STANDBY_MANUAL:
    case LIGHT_CONTROL_TIME_OCCUPANCY_DELAY: \
    case LIGHT_CONTROL_TIME_PROLONG:
    case LIGHT_CONTROL_TIME_RUN_ON:
    case PRESENT_AMBIENT_LIGHT_LEVEL:
    case PRESENT_ILLUMINANCE:
    {
      uint8_to_buf(ptr++, 3);
      uint24_to_buf(ptr, *(uint32_t*)value);
      ret += 4;
    } break;
    /* 32-bit*/
    case LIGHT_CONTROL_REGULATOR_KID:
    case LIGHT_CONTROL_REGULATOR_KIU:
    case LIGHT_CONTROL_REGULATOR_KPD:
    case LIGHT_CONTROL_REGULATOR_KPU:
    {
      uint8_to_buf(ptr++, 4);
      uint32_to_buf(ptr, *(uint32_t*)value);
      ret += 5;
    } break;
    /* Average Current */
    case AVERAGE_INPUT_CURRENT:
    case AVERAGE_OUTPUT_CURRENT:
    {
      uint8_to_buf(ptr++, 3);
      average_current_t *ac = (average_current_t*)value;
      uint16_to_buf(ptr, ac->current);
      uint8_to_buf(ptr + 2, ac->duration);
      ret += 4;
    } break;
    /* Average Voltage */
    case AVERAGE_INPUT_VOLTAGE:
    case AVERAGE_OUTPUT_VOLTAGE:
    {
      uint8_to_buf(ptr++, 3);
      average_voltage_t *av = (average_voltage_t*)value;
      uint16_to_buf(ptr, av->voltage);
      uint8_to_buf(ptr + 2, av->duration);
      ret += 4;
    } break;
    /* Chromaticity Coordinates */
    case INITIAL_CIE_1931_CHROMATICITY_COORDINATES:
    case PRESENT_CIE_1931_CHROMATICITY_COORDINATES:
    {
      uint8_to_buf(ptr++, 4);
      chromaticity_coordinates_t *cc = (chromaticity_coordinates_t*)value;
      uint16_to_buf(ptr, cc->x);
      uint16_to_buf(ptr + 2, cc->y);
      ret += 5;
    } break;
    /* Electric Current Range */
    case OUTPUT_CURRENT_RANGE:
    {
      uint8_to_buf(ptr++, 4);
      electric_current_range_t *range = (electric_current_range_t*)value;
      uint16_to_buf(ptr, range->minimum);
      uint16_to_buf(ptr + 2, range->maximum);
      ret += 5;
    } break;
    case INPUT_CURRENT_RANGE_SPECIFICATION:
    {
      uint8_to_buf(ptr++, 6);
      electric_current_specification_t *spec = (electric_current_specification_t*)value;
      uint16_to_buf(ptr, spec->minimum);
      uint16_to_buf(ptr + 2, spec->typical);
      uint16_to_buf(ptr + 4, spec->maximum);
      ret += 7;
    } break;
    case INPUT_CURRENT_STATISTICS:
    case OUTPUT_CURRENT_STATISTICS:
    {
      uint8_to_buf(ptr++, 9);
      electric_current_statistics_t *stat = (electric_current_statistics_t*)value;
      uint16_to_buf(ptr, stat->current);
      uint16_to_buf(ptr + 2, stat->std_deviation);
      uint16_to_buf(ptr + 4, stat->minimum);
      uint16_to_buf(ptr + 6, stat->maximum);
      uint8_to_buf(ptr + 8, stat->sensing_duration);
      ret += 10;
    } break;
    case RELATIVE_DEVICE_ENERGY_USE_IN_A_PERIOD_OF_DAY:
    {
      uint8_to_buf(ptr++, 4);
      energy_in_a_period_of_day_t *e = (energy_in_a_period_of_day_t*)value;
      uint16_to_buf(ptr, e->energy);
      uint8_to_buf(ptr + 2, e->start_time);
      uint8_to_buf(ptr + 3, e->end_time);
      ret += 5;
    } break;
    case DEVICE_OVER_TEMPERATURE_EVENT_STATISTICS:
    case DEVICE_UNDER_TEMPERATURE_EVENT_STATISTICS:
    case INPUT_OVER_CURRENT_EVENT_STATISTICS:
    case INPUT_OVER_RIPPLE_VOLTAGE_EVENT_STATISTICS:
    case INPUT_OVER_VOLTAGE_EVENT_STATISTICS:
    case INPUT_UNDER_CURRENT_EVENT_STATISTICS:
    case INPUT_UNDER_VOLTAGE_EVENT_STATISTICS:
    case OPEN_CIRCUIT_EVENT_STATISTICS:
    case OVER_OUTPUT_RIPPLE_VOLTAGE_EVENT_STATISTICS:
    case SHORT_CIRCUIT_EVENT_STATISTICS:
    {
      uint8_to_buf(ptr++, 6);
      event_statistics_t *stat = (event_statistics_t*)value;
      uint16_to_buf(ptr, stat->number_of_events);
      uint16_to_buf(ptr + 2, stat->average_event_duration);
      uint8_to_buf(ptr + 4, stat->time_since_last_event);
      uint8_to_buf(ptr + 5, stat->sensing_duration);
      ret += 7;
    } break;
    case DEVICE_POWER_RANGE_SPECIFICATION:
    {
      uint8_to_buf(ptr++, 9);
      power_specification_t *spec = (power_specification_t*)value;
      uint24_to_buf(ptr, spec->minimum_power_value);
      uint24_to_buf(ptr + 3, spec->typical_power_value);
      uint24_to_buf(ptr + 6, spec->maximum_power_value);
      ret += 10;
    } break;
    case AVERAGE_AMBIENT_TEMPERATURE_IN_A_PERIOD_OF_DAY:
    {
      uint8_to_buf(ptr++, 3);
      temperature_8_in_a_period_of_day_t *t = (temperature_8_in_a_period_of_day_t*)value;
      uint8_to_buf(ptr, t->temperature);
      uint8_to_buf(ptr + 1, t->start_time);
      uint8_to_buf(ptr + 2, t->end_time);
      ret += 4;
    } break;
    case INDOOR_AMBIENT_TEMPERATURE_STATISTICAL_VALUES:
    case OUTDOOR_STATISTICAL_VALUES:
    {
      uint8_to_buf(ptr++, 5);
      temperature_8_statistics_t *stat = (temperature_8_statistics_t*)value;
      uint8_to_buf(ptr, stat->average);
      uint8_to_buf(ptr + 1, stat->standard_deviation_value);
      uint8_to_buf(ptr + 2, stat->minimum_value);
      uint8_to_buf(ptr + 3, stat->minimum_value);
      uint8_to_buf(ptr + 4, stat->sensing_duration);
      ret += 6;
    } break;
    case DEVICE_OPERATING_TEMPERATURE_RANGE_SPECIFICATION:
    {
      uint8_to_buf(ptr++, 4);
      temperature_range_t *r = (temperature_range_t*)value;
      uint16_to_buf(ptr, r->minimum);
      uint16_to_buf(ptr, r->maximum);
      ret += 5;
    } break;
    case DEVICE_OPERATING_TEMPERATURE_STATISTICAL_VALUES:
    {
      uint8_to_buf(ptr++, 9);
      temperature_statistics_t *stat = (temperature_statistics_t*)value;
      uint16_to_buf(ptr, stat->average);
      uint16_to_buf(ptr + 2, stat->standard_deviation);
      uint16_to_buf(ptr + 4, stat->minimum);
      uint16_to_buf(ptr + 6, stat->maximum);
      uint8_to_buf(ptr + 8, stat->duration);
      ret += 10;
    } break;
    case INPUT_VOLTAGE_RANGE_SPECIFICATION:
    case OUTPUT_VOLTAGE_RANGE:
    {
      uint8_to_buf(ptr++, 6);
      voltage_specification_t *v = (voltage_specification_t*)value;
      uint16_to_buf(ptr, v->minimum);
      uint16_to_buf(ptr + 2, v->typical);
      uint16_to_buf(ptr + 4, v->maximum);
      ret += 7;
    } break;
    case INPUT_VOLTAGE_STATISTICS:
    case OUTPUT_VOLTAGE_STATISTICS:
    {
      uint8_to_buf(ptr++, 9);
      voltage_statistics_t *stat = (voltage_statistics_t*)value;
      uint16_to_buf(ptr, stat->average);
      uint16_to_buf(ptr + 2, stat->standard_deviation);
      uint16_to_buf(ptr + 4, stat->minimum);
      uint16_to_buf(ptr + 6, stat->maximum);
      uint8_to_buf(ptr + 8, stat->duration);
      ret += 10;
    } break;
    default:
      // If Property ID has not been recognized, roll back the length and the pointer
      ret -= 2;
      ptr -= 2;
      break;
  }
  return ret;
}

mesh_device_property_t mesh_sensor_data_from_buf(uint16_t property_id, const uint8_t *ptr)
{
  mesh_device_property_t property = { 0 };
  switch (property_id) {
    case PRESENT_AMBIENT_TEMPERATURE:
    case PRESENT_INDOOR_AMBIENT_TEMPERATURE:
    case PRESENT_OUTDOOR_AMBIENT_TEMPERATURE:
    case COLOR_RENDERING_INDEX_R9:
    case COLOR_RENDERING_INDEX_RA:
    {
      property.int8 = int8_from_buf(ptr);
    } break;
    case PRESENCE_DETECTED:
    case CHROMATICITY_TOLERANCE:
    case INPUT_VOLTAGE_RIPPLE_SPECIFICATION:
    case LIGHT_CONTROL_REGULATOR_ACCURACY:
    case LUMEN_MAINTENANCE_FACTOR:
    case MOTION_SENSED:
    case MOTION_THRESHOLD:
    case OUTPUT_RIPPLE_VOLTAGE_SPECIFICATION:
    case PRESENT_DEVICE_OPERATING_EFFICIENCY:
    case PRESENT_INPUT_RIPPLE_VOLTAGE:
    case PRESENT_RELATIVE_OUTPUT_RIPPLE_VOLTAGE:
    {
      property.uint8 = uint8_from_buf(ptr);
    } break;
    case INITIAL_PLANCKIAN_DISTANCE:
    case PRESENT_PLANCKIAN_DISTANCE:
    {
      property.int16 = int16_from_buf(ptr);
    } break;
    case INITIAL_CORRELATED_COLOR_TEMPERATURE:
    case PRESENT_CORRELATED_COLOR_TEMPERATURE:
    case PEOPLE_COUNT:
    case DEVICE_COUNTRY_OF_ORIGIN:
    case PRESENT_INPUT_CURRENT:
    case PRESENT_OUTPUT_CURRENT:
    case LUMINOUS_EFFICACY:
    case INITIAL_LUMINOUS_FLUX:
    case PRESENT_LUMINOUS_FLUX:
    case CENTER_BEAM_INTENSITY_AT_FULL_POWER:
    case LIGHT_CONTROL_LIGHTNESS_ON:
    case LIGHT_CONTROL_LIGHTNESS_PROLONG:
    case LIGHT_CONTROL_LIGHTNESS_STANDBY:
    case PRESENT_DEVICE_OPERATING_TEMPERATURE:
    case TIME_SINCE_MOTION_SENSED:
    case TIME_SINCE_PRESENCE_DETECTED:
    case PRESENT_INPUT_VOLTAGE:
    case PRESENT_OUTPUT_VOLTAGE:
    {
      property.uint16 = uint16_from_buf(ptr);
    } break;
    /** 24-bit values */
    case TOTAL_DEVICE_OFF_ON_CYCLES:
    case TOTAL_DEVICE_POWER_ON_CYCLES:
    case DEVICE_DATE_OF_MANUFACTURE:
    case DEVICE_ENERGY_USE_SINCE_TURN_ON:
    case TOTAL_DEVICE_ENERGY_USE:
    case DEVICE_RUNTIME_SINCE_TURN_ON:
    case DEVICE_RUNTIME_WARRANTY:
    case TOTAL_DEVICE_POWER_ON_TIME:
    case TOTAL_DEVICE_RUNTIME:
    case TOTAL_LIGHT_EXPOSURE_TIME:
    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_ON:
    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_PROLONG:
    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_STANDBY:
    case LUMINOUS_ENERGY_SINCE_TURN_ON:
    case RELATIVE_RUNTIME_IN_A_CORRELATED_COLOR_TEMPERATURE_RANGE:
    case TOTAL_LUMINOUS_ENERGY:
    case LUMINOUS_EXPOSURE:
    case PRESENT_DEVICE_INPUT_POWER:
    case LIGHT_CONTROL_TIME_FADE:
    case LIGHT_CONTROL_TIME_FADE_ON:
    case LIGHT_CONTROL_TIME_FADE_STANDBY_AUTO:
    case LIGHT_CONTROL_TIME_FADE_STANDBY_MANUAL:
    case LIGHT_CONTROL_TIME_OCCUPANCY_DELAY: \
    case LIGHT_CONTROL_TIME_PROLONG:
    case LIGHT_CONTROL_TIME_RUN_ON:
    case PRESENT_AMBIENT_LIGHT_LEVEL:
    case PRESENT_ILLUMINANCE:
    {
      property.uint32 = uint24_from_buf(ptr);
    } break;
    /* 32-bit*/
    case LIGHT_CONTROL_REGULATOR_KID:
    case LIGHT_CONTROL_REGULATOR_KIU:
    case LIGHT_CONTROL_REGULATOR_KPD:
    case LIGHT_CONTROL_REGULATOR_KPU:
    {
      property.uint32 = uint32_from_buf(ptr);
    } break;
    /* Average Current */
    case AVERAGE_INPUT_CURRENT:
    case AVERAGE_OUTPUT_CURRENT:
    {
      property.average_current.current = uint16_from_buf(ptr);
      property.average_current.duration = uint8_from_buf(ptr + 2);
    } break;
    /* Average Voltage */
    case AVERAGE_INPUT_VOLTAGE:
    case AVERAGE_OUTPUT_VOLTAGE:
    {
      property.average_voltage.voltage = uint16_from_buf(ptr);
      property.average_voltage.duration = uint8_from_buf(ptr + 2);
    } break;
    /* Chromaticity Coordinates */
    case INITIAL_CIE_1931_CHROMATICITY_COORDINATES:
    case PRESENT_CIE_1931_CHROMATICITY_COORDINATES:
    {
      property.chromaticity_coordinates.x = uint16_from_buf(ptr);
      property.chromaticity_coordinates.y = uint16_from_buf(ptr + 2);
    } break;
    /* Electric Current Range */
    case OUTPUT_CURRENT_RANGE:
    {
      property.electric_current_range.minimum = uint16_from_buf(ptr);
      property.electric_current_range.maximum = uint16_from_buf(ptr + 2);
    } break;
    case INPUT_CURRENT_RANGE_SPECIFICATION:
    {
      property.electric_current_specification.minimum = uint16_from_buf(ptr);
      property.electric_current_specification.typical = uint16_from_buf(ptr + 2);
      property.electric_current_specification.maximum = uint16_from_buf(ptr + 4);
    } break;
    case INPUT_CURRENT_STATISTICS:
    case OUTPUT_CURRENT_STATISTICS:
    {
      property.electric_current_statistics.current = uint16_from_buf(ptr);
      property.electric_current_statistics.std_deviation = uint16_from_buf(ptr + 2);
      property.electric_current_statistics.minimum = uint16_from_buf(ptr + 4);
      property.electric_current_statistics.maximum = uint16_from_buf(ptr + 6);
      property.electric_current_statistics.sensing_duration = uint8_from_buf(ptr + 8);
    } break;
    case RELATIVE_DEVICE_ENERGY_USE_IN_A_PERIOD_OF_DAY:
    {
      property.energy_in_a_period_of_day.energy = uint16_from_buf(ptr);
      property.energy_in_a_period_of_day.start_time = uint8_from_buf(ptr + 2);
      property.energy_in_a_period_of_day.end_time = uint8_from_buf(ptr + 3);
    } break;
    case DEVICE_OVER_TEMPERATURE_EVENT_STATISTICS:
    case DEVICE_UNDER_TEMPERATURE_EVENT_STATISTICS:
    case INPUT_OVER_CURRENT_EVENT_STATISTICS:
    case INPUT_OVER_RIPPLE_VOLTAGE_EVENT_STATISTICS:
    case INPUT_OVER_VOLTAGE_EVENT_STATISTICS:
    case INPUT_UNDER_CURRENT_EVENT_STATISTICS:
    case INPUT_UNDER_VOLTAGE_EVENT_STATISTICS:
    case OPEN_CIRCUIT_EVENT_STATISTICS:
    case OVER_OUTPUT_RIPPLE_VOLTAGE_EVENT_STATISTICS:
    case SHORT_CIRCUIT_EVENT_STATISTICS:
    {
      property.event_statistics.number_of_events = uint16_from_buf(ptr);
      property.event_statistics.average_event_duration = uint16_from_buf(ptr + 2);
      property.event_statistics.time_since_last_event = uint8_from_buf(ptr + 4);
      property.event_statistics.sensing_duration = uint8_from_buf(ptr + 5);
    } break;
    case DEVICE_POWER_RANGE_SPECIFICATION:
    {
      property.power_specification.minimum_power_value = uint24_from_buf(ptr);
      property.power_specification.typical_power_value = uint24_from_buf(ptr + 3);
      property.power_specification.maximum_power_value = uint24_from_buf(ptr + 6);
    } break;
    case AVERAGE_AMBIENT_TEMPERATURE_IN_A_PERIOD_OF_DAY:
    {
      property.temperature_8_in_a_period_of_day.temperature = uint8_from_buf(ptr);
      property.temperature_8_in_a_period_of_day.start_time = uint8_from_buf(ptr + 1);
      property.temperature_8_in_a_period_of_day.end_time = uint8_from_buf(ptr + 2);
    } break;
    case INDOOR_AMBIENT_TEMPERATURE_STATISTICAL_VALUES:
    case OUTDOOR_STATISTICAL_VALUES:
    {
      property.temperature_8_statistics.average = uint8_from_buf(ptr);
      property.temperature_8_statistics.standard_deviation_value = uint8_from_buf(ptr + 1);
      property.temperature_8_statistics.minimum_value = uint8_from_buf(ptr + 2);
      property.temperature_8_statistics.maximum_value = uint8_from_buf(ptr + 3);
      property.temperature_8_statistics.sensing_duration = uint8_from_buf(ptr + 4);
    } break;
    case DEVICE_OPERATING_TEMPERATURE_RANGE_SPECIFICATION:
    {
      property.temperature_range.minimum = uint16_from_buf(ptr);
      property.temperature_range.maximum = uint16_from_buf(ptr + 2);
    } break;
    case DEVICE_OPERATING_TEMPERATURE_STATISTICAL_VALUES:
    {
      property.temperature_statistics.average = uint16_from_buf(ptr);
      property.temperature_statistics.standard_deviation = uint16_from_buf(ptr + 2);
      property.temperature_statistics.minimum = uint16_from_buf(ptr + 4);
      property.temperature_statistics.maximum = uint16_from_buf(ptr + 6);
      property.temperature_statistics.duration = uint8_from_buf(ptr + 8);
    } break;
    case INPUT_VOLTAGE_RANGE_SPECIFICATION:
    case OUTPUT_VOLTAGE_RANGE:
    {
      property.voltage_specification.minimum = uint16_from_buf(ptr);
      property.voltage_specification.typical = uint16_from_buf(ptr + 2);
      property.voltage_specification.maximum = uint16_from_buf(ptr + 2);
    } break;
    case INPUT_VOLTAGE_STATISTICS:
    case OUTPUT_VOLTAGE_STATISTICS:
    {
      property.voltage_statistics.average = uint16_from_buf(ptr);
      property.voltage_statistics.standard_deviation = uint16_from_buf(ptr + 2);
      property.voltage_statistics.minimum = uint16_from_buf(ptr + 4);
      property.voltage_statistics.maximum = uint16_from_buf(ptr + 6);
      property.voltage_statistics.duration = uint8_from_buf(ptr + 8);
    } break;
    default:
      break;
  }
  return property;
}

sl_status_t mesh_lib_sensor_server_init(uint16_t elem_idx, uint8_t number_of_sensors, const sensor_descriptor_t *descriptors)
{
  uint8_t buf[128]; // temporary buffer allowing maximum 16 sensors for one element
  for (int i = 0; i < number_of_sensors; i++) {
    sensor_descriptor_t d = descriptors[i];
    uint8_t *ptr = &buf[i * 8];

    uint16_to_buf(ptr, d.property_id);
    uint32_t tolerance = (d.positive_tolerance & 0xfff) | (d.negative_tolerance << 12);
    uint24_to_buf(ptr + 2, tolerance);
    uint8_to_buf(ptr + 5, d.sampling_function);
    uint8_to_buf(ptr + 6, d.measurement_period);
    uint8_to_buf(ptr + 7, d.update_interval);
  }
  sl_btmesh_sensor_server_init(elem_idx, number_of_sensors * 8, buf);
  return SL_STATUS_OK;
}

sl_status_t mesh_lib_sensor_descriptors_from_buf(sensor_descriptor_t *descriptor, uint8_t *buf, int16_t input_len)
{
  // Descriptor array length should be a multiply of 8
  if (input_len & 0x07) {
    return SL_STATUS_INVALID_PARAMETER;
  } else {
    int pos = 0;
    int idx = 0;
    while (pos < input_len) {
      descriptor[idx].property_id = buf[pos] | (buf[pos + 1] << 8);
      descriptor[idx].positive_tolerance = buf[pos + 2] | ((buf[pos + 3] & 0x0F) << 8);
      descriptor[idx].negative_tolerance = (buf[pos + 3] >> 4) | (buf[pos + 4] << 4);
      descriptor[idx].sampling_function = buf[pos + 5];
      descriptor[idx].measurement_period = buf[pos + 6];
      descriptor[idx].measurement_period = buf[pos + 7];
      pos += 8;
      idx++;
    }
  }
  return SL_STATUS_OK;
}

/**
 * The following functions are to convert fields for the Sensor Descriptor
 * These are highly inefficient due to the fact that they require floating point calculations
 * and therefore probable better avoided by just pre-calculating the values since they are constants.
 */

/**
 * Converting Positive and Negative Tolerance
 * The formula is:
 *  Possible Error percentage = 100 * Tolerance value / 4095
 *  and
 *  Tolerance value = Possible Error percentage / 100 * 4095;
 */
uint16_t error_percentage2tolerance_value(float error_percentage)
{
  return (uint16_t)(error_percentage / 100 * 4095);
}

double tolerance_value2error_percentage(uint16_t tolerance)
{
  return (double)tolerance / 4095 * 100;
}

/**
 * Converting Measurement Period or Update Interval values between seconds and the representation if the standard
 * The formula is:
 *  time period = 1.1 ^ (value - 64)
 *  and
 *  descriptor fields' value = (base 1.1 logarithm of time_period_in_seconds ) + 64
 */
uint32_t time_exp_to_seconds(uint32_t mp)
{
  return (uint32_t)pow((double)1.1, mp - 64);
}

uint32_t seconds_to_time_exp(uint32_t seconds)
{
  return (uint32_t)(log((double)seconds) / log((double)1.1) + 64);
}
