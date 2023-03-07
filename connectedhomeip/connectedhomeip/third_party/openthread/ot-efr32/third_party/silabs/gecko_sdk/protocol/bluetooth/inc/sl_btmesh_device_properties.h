/******************************************************************************/
/**
 * @file   sl_btmesh_device_properties.h
 * @brief  Silicon Labs Bluetooth Mesh Generic Model API
 *
 *******************************************************************************
 * <b> (C) Copyright 2018 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 ******************************************************************************/

#ifndef SL_BTMESH_DEVICE_PROPERTIES_H
#define SL_BTMESH_DEVICE_PROPERTIES_H

#include "stdint.h"

/**
 * @brief Mesh Characteristics
 *
 * Mesh Characteristics
 * The GATT characteristics below are used by device properties that are required by the Mesh Model specification,
 * the Mesh Provisioning Service and the Mesh Proxy Service, excluding characteristics defined in the Bluetooth Core Specification
 * or in the Bluetooth Service specifications.
 */

/**
 * Boolean
 * 0: False
 * 1: False
 * 2-0xff: Prohibited
 */
typedef uint8_t boolean_t;

/**
 * Electric current
 * Unit is ampere with a resolution of 0.01. Range is from 0 to 655.34.
 * A value of 0xFFFF represents 'value is not known'.
 */
typedef uint16_t electric_current_t;

/**
 * Voltage
 *  The Voltage characteristic is used to represent a measure of positive electric potential difference in units of volts.
 *  Unit is volt with a resolution of 1/64V.
 *  Range: 0.0 to 1022.0
 *  A value of 0xFFFF represents 'value is not known'.
 *  The minimum representable value represents the minimum value or lower, the maximum representable value represents the maximum value or higher.
 */
typedef uint16_t voltage_t;

/**
 * High Voltage
 * The High Voltage characteristic is used to represent a measure of positive electric potential difference
 * in units of volt.
 * Unit is volt with a resolution of 1/64V represented as a 24-bit number.
 * Range: 0.0 to 262143.97
 * A value of 0xFFFFFF represents “Value is not known”.
 */
typedef uint32_t high_voltage_t;

/** Presence detected */
/**
 * Time Exponential 8
 * The Time Exponential 8 characteristic is used to represent a measure of period of time in seconds.
 * The time duration is given by the value 1.1(N-64) in seconds, with N being the raw 8-bit value.
 * Range is form 0.0 to 73216705.
 * A raw value of 0x00 represents 0 seconds, and a raw value of 0xFF represents the total life of the device.
 */
typedef uint8_t time_exponential_8_t;

/**
 * Average Current
 * This characteristic aggregates the Electric Current characteristic and instance of the Time Exponential 8 characteristic.
 */
typedef struct average_current {
  /** Current */
  electric_current_t current;
  /** Duration */
  time_exponential_8_t duration;
} average_current_t;
//typedef average_current_t average_current_t;

/**
 * Average Voltage
 * This characteristic aggregates the Voltage characteristic and instance of the Time Exponential 8 characteristic.
 */
typedef struct average_voltage {
  /** Voltage */
  voltage_t voltage;
  /** Duration */
  time_exponential_8_t duration;
} average_voltage_t;
//typedef average_voltage_t average_voltage_t;

/**
 * Chromatic Distance From Planckian
 * Unit is unitless with a resolution of 0.00001 with a range of -0.05 to 0.05.
 * A value of 0xFFFF represents 'value is not known'. A value of 0xFFFE represents 'value is not valid'.
 */
typedef int16_t chromatic_distance_from_planckian_t;

/**
 * Chromaticity Coordinate
 * This characteristic represents a chromaticity coordinate in a color diagram such as the CIE1931 diagram. It can represent an x or y coordinate.
 */
typedef uint16_t chromaticity_coordinate_t;

/**
 * Chromaticity Coordinates
 * This characteristic represents a chromaticity coordinate as a tuple with an x and y coordinate.
 * Unit is unitless with a resolution of 1/65535 in the range of 0 to 1.0
 */
typedef struct chromaticity_coordinates {
  /** Chromaticity x-coordinate */
  chromaticity_coordinate_t x;
  /** Chromaticity y-coordinate */
  chromaticity_coordinate_t y;
} chromaticity_coordinates_t;

/*
 *
 * Chromaticity Tolerance
 * Cie 13.3-1995 Color Rendering Index
 */

/** Coefficient
 * The Coefficient characteristic is used to represent a general coefficient value. */
typedef float coefficient_t;

/**
 * Correlated Color Temperature
 * Unit is Kelvin with a resolution of 1, ranging from 800 to 65534.
 * A value of 0xFFFF represents 'value is not known'.
 */
typedef uint16_t correlated_color_temperature_t;

/** Count 16 */
typedef uint16_t count16_t;

/** Count 24 */
typedef uint32_t count24_t;

/**
 * Country Code
 * This characteristic represents a country or dependent areas in accordance with the ISO 3166-1 Numeric standard.\
 * Unitless, a value of 0xFFFF represents 'value is not known'.
 *
 */
typedef uint16_t country_code_t;

/**
 * Date UTC
 * Date as days elapsed since the Epoch (Jan 1, 1970) in the Coordinated Universal Time (UTC) time zone.
 * Unit is a day with a resolution of 1. Range is from 1 to 16777214.
 * A value of 0x000000 represents 'value is not known'.
 * NOTE: It is a 24-bit field;
 */
typedef uint32_t date_utc_t;

/**
 * The Temperature 8 characteristic is used to represent a measure of temperature with a unit of 0.5 degree Celsius.
 * Unit is degree Celsius with a resolution of 0.5
 * A value of 0xFF represents 'value is not known'.
 */
typedef int8_t temperature_8_t;

/**
 * The Time Decihour 8 characteristic is used to represent a period of time in tenths of an hour.
 * Unit is hour with a resolution of 0.1.
 * A value of 0xFF represents 'value is not known'. All other values are Prohibited.
 */
typedef uint8_t time_decihour_8_t;

/**
 * Electric Current Range
 * This characteristic aggregates two instances of the Electric Current characteristic to represent a range of Electric Current values.
 */
typedef struct electric_current_range {
  /** Minimum current */
  electric_current_t minimum;
  /** Maximum current */
  electric_current_t maximum;
} electric_current_range_t;

/**
 * Electric Current Specification
 * This characteristic aggregates three instances of the Electric Current characteristic to represent a specification of electric current values.
 */
typedef struct electric_current_specification {
  /** Minimum */
  electric_current_t minimum;
  /** Current */
  electric_current_t typical;
  /** Maximum */
  electric_current_t maximum;
} electric_current_specification_t;

/**
 * Electric Current Statistics
 * This characteristic aggregates four instances of the Electric Current characteristic with a Sensing Duration to represent a set of statistical electric current values.
 */
typedef struct electric_current_statistics {
  /** Current */
  electric_current_t current;
  /** Standard deviation */
  electric_current_t std_deviation;
  /** Minimum */
  electric_current_t minimum;
  /** Maximum */
  electric_current_t maximum;
  /** Sensing duration */
  time_exponential_8_t sensing_duration;
} electric_current_statistics_t;

/**
 * Energy
 * The Energy characteristic is used to represent a measure of energy in units of kilowatt hours.
 * Unit is kilowatt-hour with a resolution of 1.
 * Format UINT24, ranging from 0 to 16777214.
 * A value of 0xFFFFFF represents ‘value is not known’.
 */
typedef uint32_t energy_t;

/**
 * Energy32
 * The Energy32 characteristic is used to represent a measure of energy in units of kilowatt-hours,
 * with a precision of 1 Watt-hour.
 * Format UINT32, ranging from 0.000 to 4294967.293.
 * A value of 0xFFFFFFFE represents ’value is not valid’.
   A value of 0xFFFFFFFF represents ‘value is not known’.
 */
typedef uint32_t energy32_t;

/**
 * Apparent Energy32
 * The integral of Apparent Power over a time interval, represented in units of kVAh (kilo-volt-ampere-hour),
 * with a resolution of 1 volt-ampere-hour.
 * Format UINT32, ranging from 0.000 to 4294967.293.
 * A value of 0xFFFFFFFE represents ’value is not valid’.
   A value of 0xFFFFFFFF represents ‘value is not known’.
 */
typedef uint32_t apparent_energy32_t;

/**
 * Energy In A Period Of Day
 * This characteristic aggregates the Energy characteristic, and two instances of the Time Decihour 8 characteristic,
 * to represent energy use in a period of day.
 */
typedef struct energy_in_a_period_of_day {
  /** Energy Value */
  energy_t energy;
  /** Start time */
  time_decihour_8_t start_time;
  /** End time */
  time_decihour_8_t end_time;
} energy_in_a_period_of_day_t;

/**
 * Fixed String 8 *
 * The Fixed String 8 characteristic represents a 8-octet UTF-8 string.
 */
typedef uint8_t fixed_string8[8];

/**
 * Fixed String 16 *
 * The Fixed String 16 characteristic represents a 16-octet UTF-8 string.
 */
typedef uint8_t fixed_string16[16];

/**
 * Fixed String 24 *
 * The Fixed String 24 characteristic represents a 24-octet UTF-8 string.
 */
typedef uint8_t fixed_string24[24];

/**
 * Fixed String 36 *
 * The Fixed String 16 characteristic represents a 36-octet UTF-8 string.
 */
typedef uint8_t fixed_string36[36];

/**
 * Temperature 8 in a period of a day
 * This characteristic aggregates the Temperature 8 characteristic, and two instances of
 * the Time Decihour 8 characteristic, to represent a temperature value in a period of day.
 */
typedef struct temperature_8_in_a_period_of_day {
  /** Temperature */
  temperature_8_t temperature;
  /** Start time */
  time_decihour_8_t start_time;
  /** End time */
  time_decihour_8_t end_time;
} temperature_8_in_a_period_of_day_t;

/**
 * Temperature 8 Statistics
 * This characteristic aggregates four instances of the Temperature 8 characteristic,
 * and one instance of the Time Exponential 8 characteristic.
 *
 */
typedef struct temperature_8_statistics {
  /** Average */
  temperature_8_t average;
  /** Standard Deviation value */
  temperature_8_t standard_deviation_value;
  /** Minimum Value */
  temperature_8_t minimum_value;
  /** Maximum Value */
  temperature_8_t maximum_value;
  /** Sensing Duration */
  time_exponential_8_t sensing_duration;
} temperature_8_statistics_t;

/**
 * Present Input Current
 */
typedef electric_current_t present_input_current_t;

/**
 * Present Output Current
 */
typedef electric_current_t present_output_current_t;

/**
 * Average Input Current
 * This property represents the most recently measured average current for the element.
 * It consists of the average current value and the averaging period.
 */
typedef average_current_t average_input_current_t;
/**
 * Average Output Current
 * This property represents the average output current for the device.
 * This property consists of two fields:
 *  the first value (Electric Current Value) is the average current of a set of current values,
 *  and the second value (Sensing Duration) the period over which the set of values were measured.
 */
typedef average_current_t average_output_current_t;

/**
 * Temperature
 * Unit is in degrees Celsius with a resolution of 0.01 degrees Celsius
 * Range: 327.68 to 327.67.
 * NOTE: there is no 'UNKNOWN' value defined in the specification
 */
typedef int16_t temperature_t;

/**
 * High Temperature
 * Unit is in degrees Celsius with a resolution of 0.5 degrees Celsius
 * Range: -273 to 16383.5
 * Values 0x8002 to 0xFDDE are prohibited.
 * A value of 0x8001 represents “Value is not valid”.
 * A value of 0x8000 represents “Value is not known”.
 */
typedef int16_t high_temperature_t;

/**
 * Temperature Statistics
 * This characteristic aggregates four instances of the Temperature characteristic,
 * and one instance of the Time Exponential 8 characteristic.
 */
typedef struct temperature_statistics {
  /** Average temperature */
  temperature_t average;
  /** Standard deviation */
  temperature_t standard_deviation;
  /** Minimum */
  temperature_t minimum;
  /** Minimum */
  temperature_t maximum;
  /** Duration */
  time_exponential_8_t duration;
} temperature_statistics_t;

/**
 * Temperature Range
 * This characteristic aggregates two instances of the Temperature characteristic to represent a temperature range.
 */
typedef struct temperature_range {
  /** Minimum Temperature */
  temperature_t minimum;
  /** Maximum temperature */
  temperature_t maximum;
} temperature_range_t;

/**
 * Time Hour 24
 * The Time Hour 24 characteristic is used to represent a period of time in hours.
 * Unit is hour with a resolution of 1.
 * Range: 0 to 16777214
 * A value of 0xFFFFFF represents 'value is not known'.
 * Should be 24 bits, only if uint24 existed. Packed structure should define the field length;
 */
typedef uint32_t time_hour_24_t;

/**
 * Device Runtime Since Turn On
 * This property represents the total time the element has been operating in the
 * On-state since it was turned on the last time. A value of 0 represents that the run time is not known.
 */
typedef time_hour_24_t device_runtime_since_turn_on_t;
/**
 * Device Runtime Warranty
 * This property represents the total operating time covered by the warranty.
 */
typedef time_hour_24_t device_runtime_warranty_t;
/**
 * Total Device Power On Time
 * This property represents the total of the recorded duration the device has been powered on.
 * A value of 0 represents that this time is not known.
 */
typedef time_hour_24_t total_device_power_on_time_t;
/**
 * Total Device Runtime
 * This property represents the total time that the element has been operating (has been in an On-state).
 */
typedef time_hour_24_t total_device_runtime_t;
/**
 * Total Light Exposure Time
 * This property represents the total recorded sensing duration of a light sensor (typically a photodetector).
 * This property typically can, for example, be used in combination with the Relative Exposure Time In An Illuminance Range property
 * to calculate absolute exposure times of an illuminated work of art as presented in a histogram.
 */
typedef time_hour_24_t total_light_exposure_time_t;

/**
 * Time Millisecond 24
 * The Time Millisecond 24 characteristic is used to represent a period of time with a resolution of 1 millisecond.
 * Unit is second with a resolution of 0.001.
 * Range: 0 to 16777.214
 * A value of 0xFFFFFF represents 'value is not known'.
 * Should be 24 bits, if uint24 existed... Packed structures should define the length for the fields.
 *
 */
typedef uint32_t time_millisecond_24_t;

/**
 * Illumincance
 * The Illuminance characteristic is used to represent a measure of illuminance in units of lux.
 * Unit is lux with a resolution of 0.01, in the range of 0 to 167772.14, represented on uint24.
 * 0xFFFFFF represents unknown value
 */
typedef uint32_t illuminance_t;

/**
 * Light Control Time Fade
 * This property represents the time a light takes to transition from a run state to a prolong state.
 * The run state is the state when the light is running at normal light level,
 * the prolong state is an intermediate state of a light between the run state and the standby state.
 */
typedef time_millisecond_24_t light_control_time_fade;

/**
 * Light Control Time Fade On
 * This property represents the time lights take to transition from a standby state to a run state.
 */
typedef time_millisecond_24_t light_control_time_fade_on;

/**
 * Light Control Time Fade Standby Auto
 * This property represents the time lights transition from a prolong state to a standby state when
 * the transition is automatic (such as when triggered by an occupancy or light sensor).
 */
typedef time_millisecond_24_t light_control_time_standby_auto;

/**
 * Light Control Time Fade Standby Auto
 * This property represents the time lights transition from a prolong state to a standby state when
 * the transition is automatic (such as when triggered by an occupancy or light sensor).
 */
typedef time_millisecond_24_t light_control_time_standby_manual;

/**
 * Light Control Time Fade Standby Manual
 * This property represents the time lights take to transition from a prolong state to an standby state
 * when the transition is triggered by a manual operation (for example by a user operating a light switch).
 */
typedef time_millisecond_24_t light_control_time_occupancy_delay;

/**
 *  Light Control Time Prolong
 *  This property represents the duration of the prolong state which is the state of a device between its run state and its standby state.
 */
typedef time_millisecond_24_t light_control_time_prolong;

/**
 * Light Control Time Run On
 * This property represents the duration of the run state after last occupancy was detected.
 * This duration can be used to prevent lights from entering a standby state, for example,
 * when people are still in a room and occupancy detectors fail to detect occupancy (which could occur,
 * for example, with passive infrared motion sensors and people in the room not moving for a period of time).
 */
typedef time_millisecond_24_t light_control_time_run_on;

/**
 * Percentage 8
 * The Percentage 8 characteristic is used to represent a measure of percentage.
 * Unit is a percentage with a resolution of 0.5.
 * Range 0 to 100, a value of 0xFF represents 'value is not known'. All other values are Prohibited.
 */
typedef uint8_t percentage_8_t;

/**
 * Power
 * The Power characteristic is used to represent a measure of power in units of watts.
 * Unit is watt with a resolution of 0.1.
 * Range is from 0 to 0 1677721.4 represented by a 24-bit unsiged integer.
 * A value of 0xFFFFFF represents 'value is not known'. All other values are Prohibited.
 */
typedef uint32_t power_t;

/**
 * Apparent Power
 * Apparent power is expressed in volt-amperes (VA) since it is the product of quadratic mean
 * voltage and quadratic mean current.
 * Range is from 0 to 0 1677721.3 represented by a 24-bit unsiged integer.
 * A value of 0xFFFFFE represents 'value is not valid'.
 * A value of 0xFFFFFF represents 'value is not known'.
 */
typedef uint32_t apparent_power_t;

/**
 * Power specification
 * This characteristic aggregates three instances of the Power characteristic to represent a specification of Power values.
 */
typedef struct power_specification {
  /** Minimum power value */
  power_t minimum_power_value;
  /** Typical power value */
  power_t typical_power_value;
  /** Maximum power value */
  power_t maximum_power_value;
} power_specification_t;

/**
 * Time Second 16
 * The Time Second 16 characteristic is used to represent a period of time with a unit of 1 second.
 * Unit is second with a resolution of 1.
 * Range: 0 to 65534.
 * A value of 0xFFFF represents 'value is not known'.
 */
typedef uint16_t time_second_16_t;

/**
 * Time Second 32
 * The Time Second 32 characteristic is used to represent a period of time with a unit of 1 second.
 * Unit is second with a resolution of 1.
 * Range: 0 to 4294967294.
 * A value of 0xFFFFFFFF represents 'value is not known'.
 */
typedef uint32_t time_second_32_t;

/**
 * Event Statistics
 * This characteristic aggregates the Count 16 characteristic, two instances of the
 * Time Decihour 8 characteristic and an instance of the Sensing Duration characteristic,
 * to represent statistical values of events.
 */
typedef struct event_statistics {
  /** Number of events */
  count16_t number_of_events;
  /** Average Event Duration */
  time_second_16_t average_event_duration;
  /** Time Elapsed Since Last Event */
  time_exponential_8_t time_since_last_event;
  /** Sensing Duration */
  time_exponential_8_t sensing_duration;
} event_statistics_t;

/**
 * Time Since Motion Sensed
 */
typedef time_second_16_t time_since_motion_sensed_t;

/** Time Since Presence Detected */
typedef time_second_16_t time_since_presence_detected_t;

/**
 * Present Input Voltage
 * This property represents the most recently measured input voltage for the device.
 */
typedef voltage_t present_input_voltage_t;

/**
 * Present Output voltage
 * This property represents the present output voltage of the power supply.
 * Unit is volt with a resolution of 1/64V represented on uint16, ranging from 0.0 to 1022.0.
 * A value of 0xFFFF represents 'value is not known'.
 * The minimum representable value represents the minimum value or lower,
 * the maximum representable value represents the maximum value or higher.
 */
typedef voltage_t present_output_voltage_t;

/**
 * Voltage Specification
 * This characteristic aggregates three instances of the Voltage characteristic to represent a specification of voltage values.
 */
typedef struct voltage_specification {
  /** Minimum */
  voltage_t minimum;
  /** Typical */
  voltage_t typical;
  /** Maximum */
  voltage_t maximum;
} voltage_specification_t;

/**
 * Input Voltage Range specification
 * This property represents the minimum, typical, and maximum input voltage range
 * as specified for a device.
 * These values can be used together with the measured input voltages to check if
 * the device is operating within specification.
 */
typedef voltage_specification_t input_voltage_range_specification_t;

/**
 * Output Voltage Range
 * This property represents the minimum and maximum output voltage of the power supply.
 */
typedef voltage_specification_t output_voltage_range_t;

/**
 * Voltage statistics
 * This characteristic aggregates four instances of the Voltage characteristic and
 * an instance of the Time Exponential 8 characteristic to represent a set of
 * statistical voltage values over a period of time.
 */
typedef struct voltage_statistics {
  /** Average voltage */
  voltage_t average;
  /** Standard deviation */
  voltage_t standard_deviation;
  /** Minimum */
  voltage_t minimum;
  /** Maximum */
  voltage_t maximum;
  /** Duration */
  time_exponential_8_t duration;
} voltage_statistics_t;

/** Output Voltage Statistics */
typedef voltage_statistics_t output_voltage_statistics_t;

/** Input Voltage Statistics */
typedef voltage_statistics_t input_voltage_statistics_t;

/**
 * Average Input Voltage
 */
typedef average_voltage_t average_input_voltage_t;
/**
 * Average Output Voltage
 */
typedef average_voltage_t average_output_voltage;

/**
 * Voltage Frequency
 * Power supply voltage frequency measured in hertz
 * Unit is hertz with resolution of 1 represented as a 16-bit number.
 * Range is from 1 to 65533.
 * A value of 0 represents DC power supply.
 * A value of 0xFFFE represents “Value is not valid”.
 * A value of 0xFFFF represents “Value is not known”.
 */
typedef uint16_t voltage_frequency_t;

/**
 * Cosine Of The Angle
 * The Cosine Of The Angle characteristic represents a value of cosine of the angle.
 * This is unitless value, expressed as Cos(o)/100, with a resolution of 1.
 * Unit: org.bluetooth.unit.unitless
 * Allowed range is -100 to 100.
 * A raw value of 0x7F represents ’value is not known’.
 */
typedef uint8_t cosine_of_the_angle_t;

/**
 * Humidity
 * The Humidity characteristic is used to represent the humi
 * Unit is in percent with a resolution of 0.01 percent.
 * Allowed range is 0.00 to 100.00
 * A value of 0xFFFF represents "value is not known"
 */
typedef uint16_t humidity_t;

/**
 * CO2 Concentration
 * The CO2 Concentration characteristic is used to represent a measure of carbon dioxide
 * concentration in units of parts per million.
 * Unit is parts per million (ppm) with a resolution of 1.
 * Allowed range is: 0 to 65533.
 * A value of 0xFFFE represents ‘value is 65534 or greater’.
 * A value of 0xFFFF represents ‘value is not known.
 */
typedef uint16_t co2_concentration_t;

/**
 * VOC Concentration
 * The VOC Concentration characteristic is used to represent a measure of volatile organic
 * compounds concentration in units of parts per billion
 * Unit is parts per billion (ppb) with a resolution of 1.
 * Allowed range is: 0 to 65533.
 * A value of 0xFFFE represents ‘value is 65534 or greater’.
 * A value of 0xFFFF represents ‘value is not known.
 */
typedef uint16_t voc_concentration_t;

/**
 * Pollen Concentration
 * The Pollen Concentration characteristic is used to represent the pollen count.
 * Unit is pollen count per cubic metre, represented as uint24
 */
typedef uint32_t pollen_concentration_t;

/**
 * Noise
 * The Noise characteristic is used to represent a measure of sound pressure level in units of
 * decibel.
 * Unit is decibel with a resolution of 1.
 * Allowed range is: 0 to 253.
 * A value of 0xFE represents ‘value is 254 or greater’.
 * A value of 0xFF represents ‘value is not known’.
 */
typedef uint8_t noise_t;

/**
 * Pressure
 * Yhe Pressure characteristic is used to represent pressure
 * Unit is Pascals with a resolution of 0.1 Pa
 */
typedef uint32_t pressure_t;

/**
 * Apparent Wind Direction
 * The Apparent Wind Direction characteristic is used to represent the apparent wind direction
 * Unit is degrees with a resolution of 0.01 degrees
 * Range is from 0 to 359.99 represented as a 16-bit number.
 */
typedef uint16_t apparent_wind_direction_t;

/**
 * True Wind Direction
 * The True Wind Direction characteristic is used to represent the true wind direction
 * Unit is degrees with a resolution of 0.01 degrees
 * Range is from 0 to 359.99 represented as a 16-bit number.
 */
typedef uint16_t true_wind_direction_t;

/**
 * Apparent Wind Speed
 * The Apparent Wind Speed characteristic is used to represent the apparent wind speed
 * Unit is in metres per second with a resolution of 0,01 m/s
 * Range is from 0 to 655.35 represented as a 16-bit number.
 */
typedef uint16_t apparent_wind_speed_t;

/**
 * True Wind Speed
 * The True Wind Speed characteristic is used to represent the true wind speed
 * Unit is in metres per second with a resolution of 0,01 m/s
 * Range is from 0 to 655.35 represented as a 16-bit number.
 */
typedef uint16_t true_wind_speed_t;

/**
 * Dew Point
 * The Dew Point characteristic is used to represent the dew point in degrees Celsius.
 * Unit is in degrees Celsius with a resolution of 1 degree Celsius.
 */
typedef int8_t dew_point_t;

/**
 * Gust Factor
 * The Gust Factor characteristic is used to represent the gust factor
 * Unitless value is represented as an 8-bit number.
 * The factor has a fixed-point representation, where the actual factor is (attribute value * 0.1).
 */
typedef uint8_t gust_factor_t;

/**
 * Heat Index
 * The Heat Index characteristic is used to represent the heat index
 * The value is represented as a signed 8-bit number.
 * Unit is in degrees Celsius.
 */
typedef int8_t heat_index_t;

/**
 * Light Distribution
 * Light distribution is the projected pattern of outdoor light that a fixture disperses onto a surface.
 * The value is represented as an unsigned 8-bit number and is an enumeration with the following values:
 * 0: Type not specified
 * 1: Type I
 * 2: Type II
 * 3: Type III
 * 4: Type IV
 * 5: Type V
 * All other values are reserved for future use.
 */
typedef uint8_t light_distribution_t;

/**
 * Light Output
 * Measure of the total quantity of visible light emitted by a source per unit of time.
 * Unit is Lumen with resolution of 1, represented as 24 bit number.
 * Range is from 0 to 16777213,
 * A value of 0xFFFFFE represents “Value is not valid”.
 * A value of 0xFFFFFF represents “Value is not known"
 */
typedef uint32_t light_output_t;

/**
 * Global Trade Item Number
 * The Global Trade Item Number characteristic represents an identifier as issued by GS1 [6], which may
 * consist up to 14 digits, and is here represented as a 48-bit unsigned integer.
 */
typedef uint8_t global_trade_item_number_t[6];

/**
 * Magnetic Declination
 * The Magnetic Declination characteristic is used to represent the magnetic declination.
 * Unit is degrees with a resolution of 0.01 degrees
 * Range is from 0 to 359.99 represented as a 16-bit number.
 */
typedef uint16_t magnetic_declination_t;

/**
 * Magnetic Flux Density - 2D
 * The Magnetic Flux Density - 2D characteristic is used to represent measurements of magnetic flux
 * density for two orthogonal axes: X and Y.
 * Unit is 10^-7 Tesla.
 */
typedef struct {
  uint16_t x; /**< X axis density */
  uint16_t y; /**< Y axis density */
} magnetic_flux_density_2d_t;

/**
 * Magnetic Flux Density - 3D
 * The Magnetic Flux Density - 3D characteristic is used to represent measurements of magnetic flux
 * density for two orthogonal axes: X, Y, and Z.
 * Unit is 10^-7 Tesla.
 */
typedef struct {
  uint16_t x; /**< X axis density */
  uint16_t y; /**< Y axis density */
  uint16_t z; /**< Z axis density */
} magnetic_flux_density_3d_t;

/**
 * Rainfall
 * The Rainfall characteristic is used to represent the amount of rain that has fallen.
 * Unit is meters with a resolution of 1mm, represented with a 16 bit number.
 */
typedef uint16_t rainfall_t;

/**
 * UV Index
 * The UV Index characteristic is used to represent the UV Index.
 * The unitless value is represented as an 8-bit number.
 */
typedef uint8_t uv_index_t;

/**
 * Wind Chill
 * The Wind Chill characteristic is used to represent the wind chill factor.
 * Unit is degrees Celsius, and the value is represented as a signed 8-bit number
 */
typedef int8_t wind_chill_t;

/**
 * Mesh Device Property union
 */
typedef struct mesh_device_property {
  union {
    /** int8 base type */
    int8_t int8;
    /** uint8 base type */
    uint8_t uint8;
    /** int16 base type */
    int16_t int16;
    /** uint16 base type */
    uint16_t uint16;
    /** uint32 base type */
    uint32_t uint32;
    /** Apparent Energy32 */
    apparent_energy32_t apparent_energy;
    /** Apparent Power */
    apparent_power_t apparent_power;
    /** Apparent Wind Direction */
    apparent_wind_direction_t apparent_wind_direction;
    /** Apparent Wind Speed */
    apparent_wind_speed_t apparent_wind_speed;
    /** Average current */
    average_current_t average_current;
    /** Average Voltage */
    average_voltage_t average_voltage;
    /** Boolean */
    boolean_t boolean;
    /** Chromatic distance from planckian */
    chromatic_distance_from_planckian_t chromatic_distance_from_planckian;
    /** Chromaticitiy coordinates */
    chromaticity_coordinates_t chromaticity_coordinates;
    /** Chromaticity tolerance */
    //chromaticity_tolerance_t chromaticity_tolerance_t;
    // ** Color rendering index */
    //cie_color_rendering_index_t color_rendering_index;
    /** CO2 Concentration */
    co2_concentration_t co2_concentration;
    /** Coefficient */
    coefficient_t coefficient;
    /** Correlated color temperature */
    correlated_color_temperature_t correlated_color_temperature;
    /** Cosine of the angle */
    cosine_of_the_angle_t cosine_of_the_angle;
    /** Count16 */
    count16_t count16;
    /** Count24 */
    count24_t count24;
    /** Country Code */
    country_code_t country_code;
    /** Date UTC */
    date_utc_t date_utc;
    /** Dew Point */
    dew_point_t dew_point;
    /** Electric current */
    electric_current_t electric_current;
    /** Electric current range */
    electric_current_range_t electric_current_range;
    /** Electric current specification */
    electric_current_specification_t electric_current_specification;
    /** Electric current statistics */
    electric_current_statistics_t electric_current_statistics;
    /** Energy */
    energy_t energy;
    /** Energy32 */
    energy32_t energy32;
    /** Energy in a period of day */
    energy_in_a_period_of_day_t energy_in_a_period_of_day;
    /** Event statistics */
    event_statistics_t event_statistics;
    /** Global Trade Item Number */
    global_trade_item_number_t global_trade_item_number;
    /** Gust Factor */
    gust_factor_t gust_factor;
    /** Heat Index */
    heat_index_t heat_index;
    /** High Temperature */
    high_temperature_t high_temperature;
    /** High Voltage */
    high_voltage_t high_voltage;
    /** Humidity */
    humidity_t humidity;
    /** Illuminance */
    illuminance_t illuminance;
    /** Light distribution */
    light_distribution_t light_distribution;
    /** Light output */
    light_output_t light_output;
    //luminous_efficacy_t luminous_efficacy;
    //luminous_energy_t luminous_energy;
    //luminous_exposure_t lumimous_exposure;
    //luminous_flux_t luminous_flux;
    //luminous_flux_range_t luminous_flux_range;
    //luminois_intensity_t luminous_intensity;
    //perceived_lightness_t perceived_lightness;
    /** Magnetic Declination */
    magnetic_declination_t magnetic_declination;
    /** Magnetic Flux Density - 2D */
    magnetic_flux_density_2d_t magnetic_flux_density_2d;
    /** Magnetic Flux Density - 3D */
    magnetic_flux_density_3d_t magnetic_flux_density_3d;
    /** Noise */
    noise_t noise;
    /** Percentage */
    percentage_8_t percentage;
    /** Pollen Concentration */
    pollen_concentration_t pollen_concentration;
    /** Power */
    power_t power;
    /** Power specification */
    power_specification_t power_specification;
    /** Pressure */
    pressure_t pressure;
    /** Rainfall */
    rainfall_t rainfall;
    //relative_runtime_in_a_current_range_t relative_runtime_in_a_current_range;
    //relative_runtime_in_a_relative_level_range_t relative_runtime_in_a_relative_level_range;
    //relative_runtime_in_a_temperature_range_t relative_runtime_in_a_temperature_range;
    //relative_value_in_a_temperature_range_t relative_value_in_a_temperature_range;
    //relative_value_in_a_voltage_range_t relative_value_in_a_voltage_range;
    //relative_value_in_a_an_illuminance_rang;
    /** Temperature */
    temperature_t temperature;
    /** Temperature 8 */
    temperature_8_t temperature_8;
    /** Temperature 8 in a period of day */
    temperature_8_in_a_period_of_day_t temperature_8_in_a_period_of_day;
    /** Temperature 8 statistics */
    temperature_8_statistics_t temperature_8_statistics;
    /** Temperature range */
    temperature_range_t temperature_range;
    /** Temperature statistics */
    temperature_statistics_t temperature_statistics;
    /** Time hour 24 */
    time_hour_24_t time_hour_24;
    /** Time millisecond 24 */
    time_millisecond_24_t time_millisecond_24;
    /** Time second 16 */
    time_second_16_t time_second_16;
    /** Time second 32 */
    time_second_32_t time_second_32;
    /** True Wind Direction */
    true_wind_direction_t true_wind_direction;
    /** True Wind Speed */
    true_wind_speed_t true_wind_speed;
    /** UV Index */
    uv_index_t uv_index;
    /** VOC Concentration */
    voc_concentration_t voc_concentration;
    /** Voltage */
    voltage_t voltage;
    /** Voltage Frequency */
    voltage_frequency_t voltage_frequency;
    /** Voltage specification */
    voltage_specification_t voltage_specification;
    /** Voltage statistics */
    voltage_statistics_t voltage_statistics;
    wind_chill_t wind_chill;
  };
} mesh_device_property_t;

/**
 * @brief Mesh Device Properties
 *
 * Mesh Devices Properties
 *
 * A device property is a collection of one or more format descriptors that interpret data contained by a server state.
 * The property is identified by an assigned Property ID, which references Generic Attributes (GATT) characteristics,
 * and has a state called the Property Value.
 * The device properties below are required by the Mesh Model specification, the Mesh Provisioning Service, and/or the Mesh Proxy Service.
 */

typedef enum mesh_device_properties_e {
  DEVICE_PROPERTY_INVALID                         = 0x0000,
  /** Average Ambient Temperature In A Period Of Day
   * Type: Temperature 8 In A Period Of Day */
  AVERAGE_AMBIENT_TEMPERATURE_IN_A_PERIOD_OF_DAY  = 0x0001,
  /** Average Input Current
   * Type: Average Current */
  AVERAGE_INPUT_CURRENT                           = 0x0002,
  /** Average Input Voltage
   * Type: Average Voltage */
  AVERAGE_INPUT_VOLTAGE                           = 0x0003,
  /** Average Output Current
  * Type: Average Current */
  AVERAGE_OUTPUT_CURRENT                          = 0x0004,
  /** Average Output Voltage
  * Type: Average Voltage */
  AVERAGE_OUTPUT_VOLTAGE                          = 0x0005,
  /** Center Beam Intensity At Full Power
   * Type: Luminous Intensity */
  CENTER_BEAM_INTENSITY_AT_FULL_POWER             = 0x0006,
  /** Chromaticity Tolerance
   * Type: Chromaticity Tolerance */
  CHROMATICITY_TOLERANCE                          = 0x0007,
  /** Color Rendering Index R9
   * Type: Cie 13.3-1995 Color Rendering Index */
  COLOR_RENDERING_INDEX_R9                        = 0x0008,
  /** Color Rendering Index Ra
   * Type: Cie 13.3-1995 Color Rendering Index */
  COLOR_RENDERING_INDEX_RA                        = 0x0009,
  /** Device Appearance
   * Type: Gap.Appearance */
  DEVICE_APPEARANCE                               = 0x000A,
  /** Device Country Of Origin
   * Type: Country Code */
  DEVICE_COUNTRY_OF_ORIGIN                        = 0x000B,
  /** Device Date Of Manufacture
   * Type: Date Utc */
  DEVICE_DATE_OF_MANUFACTURE                      = 0x000C,
  /** Device Energy Use Since Turn On
   * Type: Energy */
  DEVICE_ENERGY_USE_SINCE_TURN_ON                 = 0x000D,
  /** Device Firmware Revision
   * Type: Fixed String 8 */
  DEVICE_FIRMWARE_REVISION                        = 0x000E,
  /** Device Global Trade Item Number
  * Type: Global Trade Item Number */
  DEVICE_GLOBAL_TRADE_ITEM_NUMBER                 = 0x000F,
  /** Device Hardware Revision
   * Type: Fixed String 16 */
  DEVICE_HARDWARE_REVISION                        = 0x0010,
  /** Device Manufacturer Name
   * Type: Fixed String 36 */
  DEVICE_MANUFACTURER_NAME                        = 0x0011,
  /** Device Model Number
   * Type: Fixed String 24 */
  DEVICE_MODEL_NUMBER                             = 0x0012,
  /** Device Operating Temperature Range Specification
   * Type: Temperature Range */
  DEVICE_OPERATING_TEMPERATURE_RANGE_SPECIFICATION = 0x0013,
  /** Device Operating Temperature Statistical Values
   * Type: Temperature Statistics */
  DEVICE_OPERATING_TEMPERATURE_STATISTICAL_VALUES = 0x0014,
  /** Device Over Temperature Event Statistics
   * Type: Event Statistics */
  DEVICE_OVER_TEMPERATURE_EVENT_STATISTICS        = 0x0015,
  /** Device Power Range Specification
   * Type: Power Specification */
  DEVICE_POWER_RANGE_SPECIFICATION                = 0x0016,
  /** Device Runtime Since Turn On
   * Type: Time Hour 24 */
  DEVICE_RUNTIME_SINCE_TURN_ON                    = 0x0017,
  /** Device Runtime Warranty
   * Type: Time Hour 24 */
  DEVICE_RUNTIME_WARRANTY                         = 0x0018,
  /** Device Serial Number
   * Type: Fixed String 16 */
  DEVICE_SERIAL_NUMBER                            = 0x0019,
  /** Device Software Revision
   * Type: Fixed String 8*/
  DEVICE_SOFTWARE_REVISION                        = 0x001A,
  /** Device Under Temperature Event Statistics
   * Type: Event Statistics */
  DEVICE_UNDER_TEMPERATURE_EVENT_STATISTICS       = 0x001B,
  /** Indoor Ambient Temperature Statistical Values
   * Type: Temperature 8 Statistics */
  INDOOR_AMBIENT_TEMPERATURE_STATISTICAL_VALUES   = 0x001C,
  /** Initial CIE 1931 Chromaticity Coordinates
   * Type: Chromaticity Coordinates */
  INITIAL_CIE_1931_CHROMATICITY_COORDINATES       = 0x001D,
  /** Initial Correlated Color Temperature
   * Type: Correlated Color Temperature */
  INITIAL_CORRELATED_COLOR_TEMPERATURE            = 0x001E,
  /** Initial Luminous Flux
   * Type: Luminous Flux */
  INITIAL_LUMINOUS_FLUX                           = 0x001F,
  /** Initial Planckian Distance
   * Type: Chromatic Distance From Planckian */
  INITIAL_PLANCKIAN_DISTANCE                      = 0x0020,
  /** Input Current Range Specification
   * Type: Electric Current Specification */
  INPUT_CURRENT_RANGE_SPECIFICATION               = 0x0021,
  /** Input Current Statistics
   * Type: Electric Current Statistics */
  INPUT_CURRENT_STATISTICS                        = 0x0022,
  /** Input Over Current Event Statistics
   * Type: Event Statistics */
  INPUT_OVER_CURRENT_EVENT_STATISTICS             = 0x0023,
  /** Input Over Ripple Voltage Event Statistics
   * Type: Event Statistics */
  INPUT_OVER_RIPPLE_VOLTAGE_EVENT_STATISTICS      = 0x0024,
  /** Input Over Voltage Event Statistics
   * Type: Event Statistics */
  INPUT_OVER_VOLTAGE_EVENT_STATISTICS             = 0x0025,
  /** Input Under Current Event Statistics
   * Type: Event Statistics */
  INPUT_UNDER_CURRENT_EVENT_STATISTICS            = 0x0026,
  /** Input Under Voltage Event Statistics
   * Type: Event Statistics */
  INPUT_UNDER_VOLTAGE_EVENT_STATISTICS            = 0x0027,
  /** Input Voltage Range Specification
   * Type: Voltage Specification */
  INPUT_VOLTAGE_RANGE_SPECIFICATION               = 0x0028,
  /** Input Voltage Ripple Specification
   * Type: Percentage 8 */
  INPUT_VOLTAGE_RIPPLE_SPECIFICATION              = 0x0029,
  /** Input Voltage Statistics
   * Type: Voltage  */
  INPUT_VOLTAGE_STATISTICS                        = 0x002A,
  /** Light Control Ambient LuxLevel On
   * Type: Illuminance*/
  LIGHT_CONTROL_AMBIENT_LUXLEVEL_ON               = 0x002B,
  /** Light Control Ambient LuxLevel Prolong
   * Type: Illuminance */
  LIGHT_CONTROL_AMBIENT_LUXLEVEL_PROLONG          = 0x002C,
  /** Light Control Ambient LuxLevel Standby
   * Type: Illuminance */
  LIGHT_CONTROL_AMBIENT_LUXLEVEL_STANDBY          = 0x002D,
  /** Light Control Lightness On
  * Type: Perceived Lightness */
  LIGHT_CONTROL_LIGHTNESS_ON                      = 0x002E,
  /** Light Control Lightness Prolong
   * Type: Perceived Lightness */
  LIGHT_CONTROL_LIGHTNESS_PROLONG                 = 0x002F,
  /** Light Control Lightness Standby
   * Type: Perceived Lightness */
  LIGHT_CONTROL_LIGHTNESS_STANDBY                 = 0x0030,
  /** Light Control Regulator Accuracy
   * Type: Percentage 8 */
  LIGHT_CONTROL_REGULATOR_ACCURACY                = 0x0031,
  /** Light Control Regulator Kid
   * Type: Coefficient */
  LIGHT_CONTROL_REGULATOR_KID                     = 0x0032,
  /** Light Control Regulator Kiu
   * Type: Coefficient */
  LIGHT_CONTROL_REGULATOR_KIU                     = 0x0033,
  /** Light Control Regulator Kpd
   * Type: Coefficient */
  LIGHT_CONTROL_REGULATOR_KPD                     = 0x0034,
  /** Light Control Regulator Kpu
   * Type: Coefficient */
  LIGHT_CONTROL_REGULATOR_KPU                     = 0x0035,
  /** Light Control Time Fade
   * Type: Time Millisecond 24 */
  LIGHT_CONTROL_TIME_FADE                         = 0x0036,
  /** Light Control Time Fade On
  * Type: Time Millisecond 24 */
  LIGHT_CONTROL_TIME_FADE_ON                      = 0x0037,
  /** Light Control Time Fade Standby Auto
   * Type: Time Millisecond 24 */
  LIGHT_CONTROL_TIME_FADE_STANDBY_AUTO            = 0x0038,
  /** Light Control Time Fade Standby Manual
   * Type: Time Millisecond 24 */
  LIGHT_CONTROL_TIME_FADE_STANDBY_MANUAL          = 0x0039,
  /** Light Control Time Occupancy Delay
   * Type: Time Millisecond 24 */
  LIGHT_CONTROL_TIME_OCCUPANCY_DELAY              = 0x003A,
  /** Light Control Time Prolong
  * Type: Time Millisecond 24 */
  LIGHT_CONTROL_TIME_PROLONG                      = 0x003B,
  /** Light Control Time Run On
   * Type: Time Millisecond 24 */
  LIGHT_CONTROL_TIME_RUN_ON                       = 0x003C,
  /** Lumen Maintenance Factor
   * Type: Percentage 8 */
  LUMEN_MAINTENANCE_FACTOR                        = 0x003D,
  /** Luminous Efficacy
   * Type: Luminous Efficacy */
  LUMINOUS_EFFICACY                               = 0x003E,
  /** Luminous Energy Since Turn On
   * Type: Luminous Energy */
  LUMINOUS_ENERGY_SINCE_TURN_ON                   = 0x003F,
  /** Luminous Exposure
   * Type: Luminous Exposure */
  LUMINOUS_EXPOSURE                               = 0x0040,
  /** Luminous Flux Range
   * Type: Luminous Flux Range */
  LUMINOUS_FLUX_RANGE                             = 0x0041,
  /** Motion Sensed
   * Type: Percentage 8 */
  MOTION_SENSED                                   = 0x0042,
  /** Motion Threshold
   * Type: Percentage 8 */
  MOTION_THRESHOLD                                = 0x0043,
  /** Open Circuit Event Statistics
   * Type: Event Statistics */
  OPEN_CIRCUIT_EVENT_STATISTICS                   = 0x0044,
  /** Outdoor Statistical Values
   * Type: Temperature 8 Statistics */
  OUTDOOR_STATISTICAL_VALUES                      = 0x0045,
  /** Output Current Range
   * Type: Electric Current Range */
  OUTPUT_CURRENT_RANGE                            = 0x0046,
  /** Output Current Statistics
   * Type: Electric Current Statistics */
  OUTPUT_CURRENT_STATISTICS                       = 0x0047,
  /** Output Ripple Voltage Specification
   * Type: Percentage 8 */
  OUTPUT_RIPPLE_VOLTAGE_SPECIFICATION             = 0x0048,
  /** Output Voltage Range
   * Type: Voltage Specification */
  OUTPUT_VOLTAGE_RANGE                            = 0x0049,
  /** Output Voltage Statistics
  * Type: Voltage Statistics */
  OUTPUT_VOLTAGE_STATISTICS                       = 0x004A,
  /** Over Output Ripple Voltage Event Statistics
   * Type: Event Statistics */
  OVER_OUTPUT_RIPPLE_VOLTAGE_EVENT_STATISTICS     = 0x004B,
  /** People Count
   * Type: Count 16 */
  PEOPLE_COUNT                                    = 0x004C,
  /** Presence Detected
   * Type: Boolean */
  PRESENCE_DETECTED                               = 0x004D,
  /** Present Ambient Light Level
   * Type: Illuminance */
  PRESENT_AMBIENT_LIGHT_LEVEL                     = 0x004E,
  /** Present Ambient Temperature
   * Type: Temperature 8 */
  PRESENT_AMBIENT_TEMPERATURE                     = 0x004F,
  /** Present CIE 1931 Chromaticity Coordinates
   * Type: Chromaticity Coordinates */
  PRESENT_CIE_1931_CHROMATICITY_COORDINATES       = 0x0050,
  /** Present Correlated Color Temperature
   * Type: Correlated Color Temperature */
  PRESENT_CORRELATED_COLOR_TEMPERATURE            = 0x0051,
  /** Present Device Input Power
   * Type: Power */
  PRESENT_DEVICE_INPUT_POWER                      = 0x0052,
  /** Present Device Operating Efficiency
   * Type: Percentage 8 */
  PRESENT_DEVICE_OPERATING_EFFICIENCY             = 0x0053,
  /** Present Device Operating Temperature
   * Type: Temperature */
  PRESENT_DEVICE_OPERATING_TEMPERATURE            = 0x0054,
  /** Present Illuminance
   * Type: Illuminance */
  PRESENT_ILLUMINANCE                             = 0x0055,
  /** Present Indoor Ambient Temperature
   * Type: Temperature 8*/
  PRESENT_INDOOR_AMBIENT_TEMPERATURE              = 0x0056,
  /** Present Input Current
   * Type: Electric Current */
  PRESENT_INPUT_CURRENT                           = 0x0057,
  /** Present Input Ripple Voltage
   * Type: Percentage 8 */
  PRESENT_INPUT_RIPPLE_VOLTAGE                    = 0x0058,
  /** Present Input Voltage
   * Type: Voltage */
  PRESENT_INPUT_VOLTAGE                           = 0x0059,
  /** Present Luminous Flux
   * Type: Luminous Flux */
  PRESENT_LUMINOUS_FLUX                           = 0x005A,
  /** Present Outdoor Ambient Temperature
   * Type: Temperature 8 */
  PRESENT_OUTDOOR_AMBIENT_TEMPERATURE             = 0x005B,
  /** Present Output Current
   * Type: Electric Current */
  PRESENT_OUTPUT_CURRENT                          = 0x005C,
  /** Present Output Voltage
   * Type: Voltage */
  PRESENT_OUTPUT_VOLTAGE                          = 0x005D,
  /** Present Planckian Distance
   * Type: Chromatic Distance From Planckian */
  PRESENT_PLANCKIAN_DISTANCE                      = 0x005E,
  /** Present Relative Output Ripple Voltage
   * Type: Percentage 8 */
  PRESENT_RELATIVE_OUTPUT_RIPPLE_VOLTAGE          = 0x005F,
  /** Relative Device Energy Use In A Period Of Day
   * Type: Energy In A Period Of Day */
  RELATIVE_DEVICE_ENERGY_USE_IN_A_PERIOD_OF_DAY   = 0x0060,
  /** Relative Device Runtime In A Generic Level Range
  * Type: Relative Runtime In A Generic Level Range */
  RELATIVE_DEVICE_RUNTIME_IN_A_GENERIC_LEVEL_RANGE = 0x0061,
  /** Relative Exposure Time In An Illuminance Range
   * Type: Relative Value In An Illuminance Range */
  RELATIVE_EXPOSURE_TIME_IN_AN_ILLUMINANCE_RANGE  = 0x0062,
  /** Relative Runtime In A Correlated Color Temperature Range
   * Type: Luminous Energy */
  RELATIVE_RUNTIME_IN_A_CORRELATED_COLOR_TEMPERATURE_RANGE   = 0x0063,
  /** Relative Runtime In A Device Operating Temperature Range
   * Type: Relative Value In A Temperature Range */
  RELATIVE_RUNTIME_IN_A_DEVICE_OPERATING_TEMPERATURE_RANGE   = 0x0064,
  /** Relative Runtime In An Input Current Range
  * Type: Relative Runtime In A Current Range */
  RELATIVE_RUNTIME_IN_AN_INPUT_CURRENT_RANGE      = 0x0065,
  /** Relative Runtime In An Input Voltage Range
   * Type: Relative Value In A Voltage Range */
  RELATIVE_RUNTIME_IN_AN_INPUT_VOLTAGE_RANGE      = 0x0066,
  /** Short Circuit Event Statistics
   * Type: Event Statistics */
  SHORT_CIRCUIT_EVENT_STATISTICS                  = 0x0067,
  /** Time Since Motion Sensed
   * Type: Time Second 16 */
  TIME_SINCE_MOTION_SENSED                        = 0x0068,
  /** Time Since Presence Detected
   * Type: Time Second 16 */
  TIME_SINCE_PRESENCE_DETECTED                    = 0x0069,
  /** Total Device Energy Use
   * Type: Energy */
  TOTAL_DEVICE_ENERGY_USE                         = 0x006A,
  /** Total Device Off On Cycles
   * Type: Count 24 */
  TOTAL_DEVICE_OFF_ON_CYCLES                      = 0x006B,
  /** Total Device Power On Cycles
   * Type: Count 24 */
  TOTAL_DEVICE_POWER_ON_CYCLES                    = 0x006C,
  /** Total Device Power On Time
   * Type: Time Hour 24 */
  TOTAL_DEVICE_POWER_ON_TIME                      = 0x006D,
  /** Total Device Runtime
   * Type: Time Hour 24 */
  TOTAL_DEVICE_RUNTIME                            = 0x006E,
  /** Total Light Exposure Time
   * Type: Time Hour 24 */
  TOTAL_LIGHT_EXPOSURE_TIME                       = 0x006F,
  /** Total Luminous Energy
   * Type: Luminous Energy */
  TOTAL_LUMINOUS_ENERGY                           = 0x0070,
  /** Desired Ambient Temperature
   * Type: Temperature 8 */
  DESIRED_AMBIENT_TEMPERATURE                     = 0x0071,
  /** Precise Total Device Energy Use
   * Type: Energy32 */
  PRECISE_TOTAL_DEVICE_ENERGY_USE                 = 0x0072,
  /** Power Factor
   * Type: Cosine Of The Angle */
  POWER_FACTOR                                    = 0x0073,
  /** Sensor Gain
   * Type: Coefficient */
  SENSOR_GAIN                                     = 0x0074,
  /** Precise Present Ambient Temperature
   * Type: Temperature */
  PRECISE_PRESENT_AMBIENT_TEMPERATURE             = 0x0075,
  /** Present Ambient Relative Humidity
   * Type: Humidity */
  PRESENT_AMBIENT_RELATIVE_HUMIDITY               = 0x0076,
  /** Present Ambient Carbon Dioxide Concentration
   * Type: CO2 Concentration */
  PRESENT_AMBIENT_CO2_CONCENTRATION               = 0x0077,
  /** Present Ambient Volatile Organic Compounds Concentration
   * Type: VOC Concentration */
  PRESENT_AMBIENT_VOC_CONCENTRATION               = 0x0078,
  /** Present Ambient Noise
   * Type: Noise */
  PRESENT_AMBIENT_NOISE                           = 0x0079,

  /* -- No entries for 0x007A .. 0x007F -- */

  /** Active Energy Loadside
   * Energy32 */
  ACTIVE_ENERGY_LOADSIDE                          = 0x0080,
  /** Active Power Loadside
   * Type: Power */
  ACTIVE_POWER_LOADSIDE                           = 0x0081,
  /** Air Pressure
   * Type: Pressure */
  AIR_PRESSURE                                    = 0x0082,
  /** Apparent Energy
   * Type: Apparent Energy32 */
  APPARENT_ENERGY                                 = 0x0083,
  /** Apparent Power
   * Type: Apparent Power */
  APPARENT_POWER                                  = 0x0084,
  /** Apparent Wind Direction
   * Type: Apparent Wind Direction */
  APPARENT_WIND_DIRECTION                         = 0x0085,
  /** Apparent Wind Speed
   * Type: Apparent Wind Speed */
  APPARENT_WIND_SPEED                             = 0x0086,
  /** Dew Point
   * Type: Dew Point */
  DEW_POINT                                       = 0x0087,
  /** External Supply Voltage
   * Type: High Voltage */
  EXTERNAL_SUPPLY_VOLTAGE                         = 0x0088,
  /** External Supply Voltage Frequency
   * Type: Voltage Frequency */
  EXTERNAL_SUPPLY_VOLTAGE_FREQUENCY               = 0x0089,
  /** Gust Factor
   * Type: Gust Factor */
  GUST_FACTOR                                     = 0x008A,
  /** Heat Index
   * Type: Heat Index */
  HEAT_INDEX                                      = 0x008B,
  /** Light Distribution
   * Type: Light Distribution */
  LIGHT_DISTRIBUTION                              = 0x008C,
  /** Light Source Current
   * Type: Average Current */
  LIGHT_SOURCE_CURRENT                            = 0x008D,
  /** Light Source On Time Not Resettable
   * Type: Time Second 32 */
  LIGHT_SOURCE_ON_TIME_NOT_RESETTABLE             = 0x008E,
  /** Light Source On Time Resettable
   * Type: Time Second 32 */
  LIGHT_SOURCE_ON_TIME_RESETTABLE                 = 0x008F,
  /** Light Source Open Circuit Statistics
   * Type: Event Statistics */
  LIGHT_SOURCE_OPEN_CIRCUIT_STATISTICS            = 0x0090,
  /** Light Source Overall Failures Statistics
   * Type: Event Statistics */
  LIGHT_SOURCE_OVERALL_FAILURES_STATISTICS        = 0x0091,
  /** Light Source Short Circuit Statistics
   * Type: Event Statistics */
  LIGHT_SOURCE_SHORT_CIRCUIT_STATISTICS           = 0x0092,
  /** Light Source Start Counter Resettable
   * Type: Count 24 */
  LIGHT_SOURCE_START_COUNTER_RESETTABLE           = 0x0093,
  /** Light Source Temperature
   * Type: High Temperature */
  LIGHT_SOURCE_TEMPERATURE                        = 0x0094,
  /** Light Source Thermal Derating Statistics
   * Type: Event Statistics */
  LIGHT_SOURCE_THERMAL_DERATING_STATISTICS        = 0x0095,
  /** Light Source Thermal Shutdown Statistics
   * Type: Event Statistics */
  LIGHT_SOURCE_THERMAL_SHUTDOWN_STATISTICS        = 0x0096,
  /** Light Source Power On Cycles
   * Type: Count 24 */
  LIGHT_SOURCE_TOTAL_POWER_ON_CYCLES              = 0x0097,
  /** Light Source Voltage
   * Type: Average Voltage */
  LIGHT_SOURCE_VOLTAGE                            = 0x0098,
  /** Luminaire Color
   * Type: Fixed String 24 */
  LUMINAIRE_COLOR                                 = 0x0099,
  /** Luminaire Identification Number
   * Type: Fixed String 24 */
  LUMINAIRE_IDENTIFICATION_NUMBER                 = 0x009A,
  /** Luminaire Manufacturer GTIN
   * Type: Global Trade Item Number */
  LUMINAIRE_MANUFACTURER_GTIN                     = 0x009B,
  /** Luminaire Nominal Input Power
   * Type: Power */
  LUMINAIRE_NOMINAL_INPUT_POWER                   = 0x009C,
  /** Luminaire Nominal Maximum AC Mains Voltage
   * Type: Voltage */
  LUMINAIRE_NOMINAL_MAXIMUM_AC_MAINS_VOLTAGE      = 0x009D,
  /** Luminaire Nominal Minimum AC Mains Voltage
   * Type: Voltage */
  LUMINAIRE_NOMINAL_MINIMUM_AC_MAINS_VOLTAGE      = 0x009E,
  /** Luminaire Power At Minimum Dim Level
   * Type: Power */
  LUMINAIRE_POWER_AT_MINIMUM_DIM_LEVEL            = 0x009F,
  /** Luminaire Time Of Manufacture
   * Type: Date UTC */
  LUMINAIRE_TIME_OF_MANUFACTURE                   = 0x00A0,
  /** Magnetic Declination
   * Type: Magnetic Declination */
  MAGNETIC_DECLINATION                            = 0x00A1,
  /** Magnetic Flux Density - 2D
   * Type: Magnetic Flux Density 2D  */
  MAGNETIC_FLUX_DENSITY_2D                        = 0x00A2,
  /** Magnetic Flux Density - 3D
   * Type: Magnetic Flux Density 3D  */
  MAGNETIC_FLUX_DENSITY_3D                        = 0x00A3,
  /** Nominal Light Output
   * Type: Light Output */
  NOMINAL_LIGHT_OUTPUT                            = 0x00A4,
  /** Overall Failure Condition
   * Type: Event Statistics */
  OVERALL_FAILURE_CONDITION                       = 0x00A5,
  /** Pollen Concentration
   * Type: Pollen Concentration */
  POLLEN_CONCENTRATION                            = 0x00A6,
  /** Present Indoor Relative Humidity
   * Type; Humidity */
  PRESENT_INDOOR_RELATIVE_HUMIDITY                = 0x00A7,
  /** Present Outdoor Relative Humidity
   * Type; Humidity */
  PRESENT_OUTDOOR_RELATIVE_HUMIDITY               = 0x00A8,
  /** Pressure
   * Type: Pressure */
  PRESSURE                                        = 0x00A9,
  /** Rainfall
   * Type: Rainfall */
  RAINFALL                                        = 0x00AA,
  /** Rated Median Useful Life of Luminaire
   * Type: Time Hour 24 */
  RATED_MEDIAN_USEFUL_LIFE_OF_LUMINAIRE           = 0x00AB,
  /** Rated Median Useful Light Source Starts
   * Type: Count 24 */
  RATED_MEDIAN_USEFUL_LIGHT_SOURCE_STARTS         = 0x00AC,
  /** Reference Temperature
   * Type: High Temperature */
  REFERENCE_TEMPERATURE                           = 0x00AD,
  /** Total Device Starts
   * Type: Count 24 */
  TOTAL_DEVICE_STARTS                             = 0x00AE,
  /** True Wind Direction
   * Type: True Wind Direction */
  TRUE_WIND_DIRECTION                             = 0x00AF,
  /** True Wind Speed
   * Type: True Wind Speed */
  TRUE_WIND_SPEED                                 = 0x00B0,
  /** UV Index
   * Type: UV index */
  UV_INDEX                                        = 0x00B1,
  /** Wind Chill
   * Type: Wind Chill */
  WIND_CHILL                                      = 0x00B2,
  /**
   * */
  LIGHT_SOURCE_TYPE                               = 0x00B3,
  /** Luminaire Identification String
   * Type: Fixed String 24 */
  LUMINAIRE_IDENTIFICATION_STRING                 = 0x00B4,
  /** Output Power Limitation
  * Type: Event Statistics */
  OUTPUT_POWER_LIMITATION                         = 0x00B5,
  /** Thermal Derating
   * Type: Event Statistics */
  THERMAL_DERATING                                = 0x00B6,
  /** Output Current Percent
   * Type: Percentage 8 */
  OUTPUT_CURRENT_PERCENT                          = 0x00B7,
} mesh_device_properties_t;

#if 0
typedef enum mesh_characteristics_e {
  /** Temperature - Listed in GATT Characteristics */
  TEMPERATURE                               = 0x2A6E,
  /** Average Current */
  AVERAGE_CURRENT                           = 0x2AE0,
  /** Average Voltage */
  AVERAGE_VOLTAGE                           = 0x2AE1,
  /** Boolean */
  BOOLEAN                                   = 0x2AE2,
  /** Chromatic Distance From Planckian */
  CHROMATIC_DISTANCE_FROM_PLANCKIAN         = 0x2AE3,
  /** Chromaticity Coordinate */
  CHROMATICITY_COORDINATE                   = 0x2B1C,
  /** Chromaticity Coordinates */
  CHROMATICITY_COORDINATES                  = 0x2AE4,
  /** Chromaticity In CCT And Duv Values */
  CHROMATICITY_IN_CCT_AND_DUV_VALUES        = 0x2AE5,
  /** Chromaticity Tolerance */
  CHROMATICITY_TOLERANCE                    = 0x2AE6,
  /** CIE 13.3-1995 Color Rendering Index */
  COLOR_RENDERING_INDEX                     = 0x2AE7,
  /** Coefficient */
  COEFFICIENT                               = 0x2AE8,
  /** Correlated Color Temperature */
  CORRELATED_COLOR_TEMPERATURE              = 0x2AE9,
  /** Count 16 */
  COUNT_16                                  = 0x2AEA,
  /** Count 24 */
  COUNT_24                                  = 0x2AEB,
  /** Country Code */
  COUNTRY_CODE                              = 0x2AEC,
  /** Date UTC */
  DATE_UTC                                  = 0x2AED,
  /** Electric Current */
  ELECTRIC_CURRENT                          = 0x2AEE,
  /** Electric Current Range */
  ELECTRIC_CURRENT_RANGE                    = 0x2AEF,
  /** Electric Current Specification */
  ELECTRIC_CURRENT_SPECIFICATION            = 0x2AF0,
  /** Electric Current Statistics */
  ELECTRIC_CURRENT_STATISTICS               = 0x2AF1,
  /** Energy */
  ENERGY                                    = 0x2AF2,
  /** Energy In A Period Of Day */
  ENERGY_IN_A_PERIOD_OF_DAY                 = 0x2AF3,
  /** Event Statistics */
  EVENT_STATISTICS                          = 0x2AF4,
  /** Fixed String 16 */
  FIXED_STRING_16                           = 0x2AF5,
  /** Fixed String 24 */
  FIXED_STRING_24                           = 0x2AF6,
  /** Fixed String 36 */
  FIXED_STRING_36                           = 0x2AF7,
  /** Fixed String 8 */
  FIXED_STRING_8                            = 0x2AF8,
  /** Generic Level */
  GENERIC_LEVEL                             = 0x2AF9,
  /** Global Trade Item Number */
  GLOBAL_TRADE_ITEM_NUMBER                  = 0x2AFA,
  /** Illuminance */
  ILLUMINANCE                               = 0x2AFB,
  /** Luminous Efficacy */
  LUMINOUS_EFFICACY                         = 0x2AFC,
  /** Luminous Energy */
  LUMINOUS_ENERGY                           = 0x2AFD,
  /** Luminous Exposure */
  LUMINOUS_EXPOSURE                         = 0x2AFE,
  /** Luminous Flux */
  LUMINOUS_FLUX                             = 0x2AFF,
  /** Luminous Flux Range */
  LUMINOUS_FLUX_RANGE                       = 0x2B00,
  /** Luminous Intensity */
  LUMINOUS_INTENSITY                        = 0x2B01,
  /** Mass Flow */
  MASS_FLOW                                 = 0x2B02,
  /** Mesh Provisioning Data In */
  MESH_PROVISIONING_DATA_IN                 = 0x2ADB,
  /** Mesh Provisioning Data Out */
  MESH_PROVISIONING_DATA_OUT                = 0x2ADC,
  /** Mesh Proxy Data In */
  MESH_PROXY_DATA_IN                        = 0x2ADD,
  /** Mesh Proxy Data Out */
  MESH_PROXY_DATA_OUT                       = 0x2ADE,
  /** Perceived Lightness */
  PERCEIVED_LIGHTNESS                       = 0x2B03,
  /** Percentage 8 */
  PERCENTAGE_8                              = 0x2B04,
  /** Power */
  POWER                                     = 0x2B05,
  /** Power Specification */
  POWER_SPECIFICATION                       = 0x2B06,
  /** Relative Runtime In A Current Range */
  RELATIVE_RUNTIME_IN_A_CURRENT_RANGE       = 0x2B07,
  /** Relative Runtime In A Generic Level Range */
  RELATIVE_RUNTIME_IN_A_GENERIC_LEVEL_RANGE = 0x2B08,
  /** Relative Value In A Period of Day */
  RELATIVE_VALUE_IN_A_PERIOD_OF_DAY         = 0x2B0B,
  /** Relative Value In A Temperature Range */
  RELATIVE_VALUE_IN_A_TEMPERATURE_RANGE     = 0x2B0C,
  /** Relative Value In A Voltage Range */
  RELATIVE_VALUE_IN_A_VOLTAGE_RANGE         = 0x2B09,
  /** Relative Value In An Illuminance Range */
  RELATIVE_VALUE_IN_AN_ILLUMINANCE_RANGE    = 0x2B0A,
  /** Temperature 8 */
  TEMPERATURE_8                             = 0x2B0D,
  /** Temperature 8 In A Period Of Day */
  TEMPERATURE_8_IN_A_PERIOD_OF_DAY          = 0x2B0E,
  /** Temperature 8 Statistics */
  TEMPERATURE_8_STATISTICS                  = 0x2B0F,
  /** Temperature Range */
  TEMPERATURE_RANGE                         = 0x2B10,
  /** Temperature Statistics */
  TEMPERATURE_STATISTICS                    = 0x2B11,
  /** Time Decihour 8 */
  TIME_DECIHOUR_8                           = 0x2B12,
  /** Time Exponential 8 */
  TIME_EXPONENTIAL_8                        = 0x2B13,
  /** Time Hour 24 */
  TIME_HOUR_24                              = 0x2B14,
  /** Time Millisecond 24 */
  TIME_MILLISECOND_24                       = 0x2B15,
  /** Time Second 16 */
  TIME_SECOND_16                            = 0x2B16,
  /** Time Second 8 */
  TIME_SECOND_8                             = 0x2B17,
  /** Voltage */
  VOLTAGE                                   = 0x2B18,
  /** Voltage Specification */
  VOLTAGE_SPECIFICATION                     = 0x2B19,
  /** Voltage Statistics */
  VOLTAGE_STATISTICS                        = 0x2B1A,
  /** Volume Flow */
  VOLUME_FLOW                               = 0x2B1B,
} mesh_characteristics_t;
#endif
#endif
