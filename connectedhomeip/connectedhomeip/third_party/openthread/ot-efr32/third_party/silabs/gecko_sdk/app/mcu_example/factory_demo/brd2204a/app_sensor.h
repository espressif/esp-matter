/***************************************************************************//**
 * @file
 * @brief Helper functions for using LESENSE and I2C sensors
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

#ifndef APP_SENSOR_H
#define APP_SENSOR_H

/* DAC */
#define DAC_FREQ               500000
#define DAC_CHANNEL            1
#define DAC_DATA               800

/* LESENSE */
#define LCSENSE_CH             3
#define LCSENSE_SCAN_FREQ      20
#define LESENSE_MAX_CHANNELS  16
#define BUFFER_INDEX_LAST   15
#define LCSENSE_CH_PORT        gpioPortC
#define LCSENSE_CH_PIN         3

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "em_lesense.h"
#include "em_vdac.h"

#include "app_rgbled.h"

// Function prototypes
void sensorSetup(void);
void sensorReadHumTemp(uint32_t *RhData, int32_t *TempData);
void sensorReadHallEffect(int16_t * HallField);

void setupVDAC(void);
void writeDataDAC(VDAC_TypeDef *dac, unsigned int value, unsigned int ch);

void setupACMP(void);

void setupLESENSE(void);
void lesenseCalibrateLC(uint8_t chIdx);

void setupTRNG(void);
void randomColor(uint32_t * rngResult, RGBLED_Settings_t * ledSettings);
uint32_t littleToBigEndian(uint32_t input);

#endif /* APP_SENSOR_H */
