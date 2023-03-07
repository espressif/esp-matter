/***************************************************************************//**
 * @file
 * @brief Helper functions for PWM control of RGB LEDs
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

#ifndef APP_RGBLED_H
#define APP_RGBLED_H

typedef struct {
  uint32_t red;
  uint32_t green;
  uint32_t blue;
} ColorMix;

typedef struct {
  int16_t led0Color;
  int16_t led1Color;
  int8_t led0Power;
  int8_t led1Power;
  uint8_t curSetting;
} RGBLED_Settings_t;

#define RGBLED_SETTINGS_DEFAULT \
  {                             \
    330,                        \
    90,                         \
    6,                          \
    6,                          \
    0,                          \
  }

void setupRGBLed(void);
void setLedSettings(RGBLED_Settings_t * ledSettings);
void getColorMix(int16_t wheelpos, ColorMix * inColor, uint32_t gain);

#endif /* APP_RGBLED_H */