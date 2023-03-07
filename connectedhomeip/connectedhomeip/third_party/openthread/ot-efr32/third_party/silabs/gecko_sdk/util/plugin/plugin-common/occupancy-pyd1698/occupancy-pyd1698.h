// *****************************************************************************
// * occupancy-pyd1698.h
// *
// * macros, typedefs, enums, and API to interface with a PYD-1698 occupancy
// * sensor.
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#ifndef __OCCUPANCY_PYD1698_H__
#define __OCCUPANCY_PYD1698_H__

#include "occupancy-pyd1698.h"

// Macros used to interface with raw register data read from the DIRECT LINK pin
#define PYD_CONFIG_RESERVED_BIT         0
#define PYD_CONFIG_RESERVED_MASK        0x1F
#define PYD_CONFIG_FILTER_SRC_BIT       5
#define PYD_CONFIG_FILTER_SRC_MASK      (3 << PYD_CONFIG_FILTER_SRC_BIT)
#define PYD_CONFIG_OPERATION_MODE_BIT   7
#define PYD_CONFIG_OPERATION_MODE_MASK  (3 << PYD_CONFIG_OPERATION_MODE_BIT)
#define PYD_CONFIG_WINDOW_TIME_BIT      9
#define PYD_CONFIG_WINDOW_TIME_MASK     (3 << PYD_CONFIG_WINDOW_TIME_BIT)
#define PYD_CONFIG_PULSE_COUNTER_BIT    11
#define PYD_CONFIG_PULSE_COUNTER_MASK   (3 << PYD_CONFIG_PULSE_COUNTER_BIT)
#define PYD_CONFIG_BLIND_TIME_BIT       13
#define PYD_CONFIG_BLIND_TIME_MASK      (0xF << PYD_CONFIG_BLIND_TIME_BIT)
#define PYD_CONFIG_SENSITIVITY_BIT      17
#define PYD_CONFIG_SENSITIVITY_MASK     (0xFF << PYD_CONFIG_SENSITIVITY_BIT)
#define PYD_ADC_VOLTAGE_BIT             25
#define PYD_ADC_VOLTAGE_MASK            ((uint64_t)0x00003FFF \
                                         << PYD_ADC_VOLTAGE_BIT)

// Macros used to generated timing for interface
#define DATA_SETUP_MIN_US               110

#define PYD_MESSAGE_WRITE_LENGTH_BITS   26
#define PYD_MESSAGE_WRITE_BIT_DELAY_US  72
#define PYD_MESSAGE_READ_LENGTH_BITS    40
#define PYD_MESSAGE_READ_BIT_DELAY_US   20

#define PYD_CONFIG_RESERVED_VALUE       16

// Struct used to store a configuration message, which is the data sent to the
// device on a write
typedef struct tPydCfg{
  uint8_t reserved;
  uint8_t filterSource;
  uint8_t operationMode;
  uint8_t windowTime;
  uint8_t pulseCounter;
  uint8_t blindTime;
  uint8_t sensitivity;
} HalPydCfg_t;

// Struct used to store a messag read from the DIRECT LINK pin, which consists
// of the device configuration and the raw ADC value read by the sensor.
typedef struct tPydInMsg{
  HalPydCfg_t *config;
  int16_t AdcVoltage;
} HalPydInMsg_t;

/** @brief Perform a read of the PYD1698
 *
 * This function can be used to generate a read of the ADC and configuration
 * state of the occupancy sensor
 *
 * @param readMsg The message read from the occupancy sensor
 */
void halOccupancyPyd1698Read(HalPydInMsg_t *readMsg);

/** @brief Write a new set of configuration values to the sensor
 *
 * This function can be used to write new configuration parameters to the
 * sensor.  Note that it takes in a structure of all configuration parameters,
 * so if a single value is to be written, it is recommended to first make a call
 * to halOccupancyPyd1698GetCurrentConfiguration to first determine what the
 * state of the occupancy sensor is, modify the parameters that are to be
 * changed, and use that structure as the input to this write function.
 *
 * @param cfgMsg The entire configuration message to be written to the sensor
 */
void halOccupancyPyd1698WriteConfiguration(HalPydCfg_t *cfgMsg);

/** @brief Get the current configuration of the sensor
 *
 * This function can be used to determine how the occupancy sensor is currently
 * configured.  This is based on the RAM stored configuration structure, and as
 * such does not trigger a HW read of the occupancy sensor.
 *
 * @param config The structure in which the current configuration should be
 * stored.
 */
void halOccupancyPyd1698GetCurrentConfiguration(HalPydCfg_t *config);

#endif // __OCCUPANCY_PYD1698_H__
