/***************************************************************************//**
 * @file
 * @brief Hall Effect Wheel Demo for SLSTK3400A_EFM32HG
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

#ifndef WHEEL_H_
#define WHEEL_H_

#define SAMPLES_TO_AVG                   5

#define SI72XXEXP_PORT              gpioPortC
#define SI72XXEXP_U1_PIN            1
#define SI72XXEXP_U2_PIN            0

#define SI72XXPS_OUT_PORT           gpioPortD
#define SI72XXPS_OUT_PIN            4

typedef enum _demoNo{
  menuScreen,
  expAnglePosition,
  expRevolutionCounter,
  psI2cFieldData,
  psI2cTempData,
  psSwitchLatch,
  psAnalogOut,
  psPwmOut,
  psSentOut,
} DemoNo_t;

void gpioEnablePushButton1(void);
void gpioDisablePushButton1(void);

#endif /* WHEEL_H_ */
