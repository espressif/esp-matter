/***************************************************************************//**
 * @file
 * @brief BOARD module source file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "i2cspm.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_timer.h"
#include "em_usart.h"
#include "bspconfig.h"
#include "mx25flash_config.h"

#include "thunderboard/board.h"
#include "thunderboard/util.h"
#include "thunderboard/board_rgbled_profiles.h"

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @defgroup BOARD_4166 BOARD Module for Thunderboard Sense 2
 * @{
 * @brief Board hardware control, configuraton and miscellaneous functions
 * @details This module contains functions releated to board features. It allows
 *  control over power management features, interrupt controller and RGB LEDs.
 *
 * The BOARD module uses the common I2CSPM driver to communicate with the
 * I<sup>2</sup>C sensors on the board. The following
 * board features can be enabled when needed using the BOARD Module:
 * - Environmental sensor group
 *   - RH/Temp (Si7021)
 *   - UV/ALS (Si1133)
 *   - Barometric Pressure (BMP280)
 * - Hall effect sensor (Si7210)
 * - Indoor air quality sensor (CCS811)
 * - Inertial sensor (ICM-20948)
 * - Microphone (ICS-43434)
 * - RGB LED boost converter
 *
 * The BOARD Module also contains functions for controlling the RGB LEDs. The
 * implementation uses one of the TIMER peripherals in PWM mode, with three
 * compare capture channels, one each for red, green and blue.
 *
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define BOARD_ENABLED_NONE      0
#define BOARD_ENABLED_BAP       (1 << 0)
#define BOARD_ENABLED_RHTEMP    (1 << 1)
#define BOARD_ENABLED_ALS       (1 << 2)
#define BOARD_ENABLED_HALL      (1 << 3)
#define BOARD_ENABLED_GAS       (1 << 4)
#define BOARD_ENABLED_MIC       (1 << 5)
#define BOARD_ENABLED_IMU       (1 << 6)
#define BOARD_ENABLED_ENV_SENSOR (BOARD_ENABLED_BAP | BOARD_ENABLED_RHTEMP | BOARD_ENABLED_ALS)

/*******************************************************************************
 *******************************   TYPEDEFS   **********************************
 ******************************************************************************/

typedef struct {
  uint16_t enabled;
  uint16_t busInUse;
} BOARD_SensorInfo_t;

/****************************************************************************/
/* Local variables                                                          */
/****************************************************************************/
static TIMER_TypeDef     *pwmTimer;                                   /*  Timer used to generate PWM for the RGB LEDs                 */
static const uint8_t     lightLevels[] = BOARD_RGBLED_PROFILE_EXP;    /*  Array to linearize the light level of the RGB LEDs          */
static bool              timerEnable;                                 /*  The status of the PWM timer, enabled or disabled            */
static BOARD_SensorInfo_t sensorInfo;
static BOARD_IrqCallback gasSensorIRQCallback;
static BOARD_IrqCallback hallSensorIRQCallback;
static BOARD_IrqCallback alsIRQCallback;
static BOARD_IrqCallback imuIRQCallback;
static BOARD_IrqCallback pushButton0IRQCallback;
static BOARD_IrqCallback pushButton1IRQCallback;

/****************************************************************************/
/* Local function prototypes                                                */
/****************************************************************************/
static void flashSPIInit    (void);
static void gpioInterruptHandler(uint8_t pin);

/** @endcond */

/***************************************************************************//**
 * @brief
 *    Initializes the Thunderboard Sense board
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_init(void)
{
  TIMER_Init_TypeDef   timerInit      = TIMER_INIT_DEFAULT;
  TIMER_InitCC_TypeDef ccInit         = TIMER_INITCC_DEFAULT;
  I2CSPM_Init_TypeDef  i2cInit        = I2CSPM_INIT_DEFAULT;
  uint32_t status;

  timerEnable     = false;

  i2cInit.port            = I2C1;
  i2cInit.sclPort         = gpioPortC;
  i2cInit.sclPin          = 4;
  i2cInit.sdaPort         = gpioPortC;
  i2cInit.sdaPin          = 5;
  i2cInit.portLocationScl = 17;
  i2cInit.portLocationSda = 17;
  I2CSPM_Init(&i2cInit);

  sensorInfo.enabled = 0x00;
  sensorInfo.busInUse = 0x00;

  status = BOARD_OK;

  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /*********************************************************************/
  /** Push button config                                              **/
  /*********************************************************************/
  GPIO_PinModeSet(BOARD_BUTTON_LEFT_PORT, BOARD_BUTTON_LEFT_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(BOARD_BUTTON_RIGHT_PORT, BOARD_BUTTON_RIGHT_PIN, gpioModeInput, 0);

  /* Initialize LEDs */
  GPIO_PinModeSet(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, gpioModePushPull, 0);

  /* Configure the Interrupt pins */
  GPIO_PinModeSet(BOARD_IMU_INT_PORT, BOARD_IMU_INT_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(BOARD_UV_ALS_INT_PORT, BOARD_UV_ALS_INT_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(BOARD_CCS811_CTRL_PORT, BOARD_CCS811_INT_PIN, gpioModeInput, 0);

  /*********************************************************************/
  /** HALL effect sensor pin config                                   **/
  /*********************************************************************/
  GPIO_PinModeSet(BOARD_HALL_PORT, BOARD_HALL_ENABLE_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_HALL_PORT, BOARD_HALL_I2C_SDA_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BOARD_HALL_PORT, BOARD_HALL_I2C_SCL_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BOARD_HALL_PORT, BOARD_HALL_OUTPUT_PIN, gpioModeDisabled, 0);

  /*********************************************************************/
  /** IMU pin config                                                  **/
  /*********************************************************************/
  GPIO_PinModeSet(BOARD_IMU_ENABLE_PORT, BOARD_IMU_ENABLE_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_IMU_SPI_PORT, BOARD_IMU_SPI_MOSI_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BOARD_IMU_SPI_PORT, BOARD_IMU_SPI_MISO_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BOARD_IMU_SPI_PORT, BOARD_IMU_SPI_SCLK_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BOARD_IMU_SPI_PORT, BOARD_IMU_SPI_CS_PIN, gpioModeDisabled, 0);

  /*********************************************************************/
  /** ENV Sensor pin config                                           **/
  /*********************************************************************/
  GPIO_PinModeSet(BOARD_ENV_ENABLE_PORT, BOARD_ENV_ENABLE_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_ENV_I2C_PORT, BOARD_ENV_I2C_SDA_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BOARD_ENV_I2C_PORT, BOARD_ENV_I2C_SCL_PIN, gpioModeDisabled, 0);

  /*********************************************************************/
  /** Microphone pin config                                           **/
  /*********************************************************************/
  GPIO_PinModeSet(BOARD_MIC_ENABLE_PORT, BOARD_MIC_ENABLE_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_MIC_INPUT_PORT, BOARD_MIC_INPUT_PIN, gpioModeDisabled, 0);

  /*********************************************************************/
  /** Gas sensor pin config                                           **/
  /*********************************************************************/
  GPIO_PinModeSet(BOARD_CCS811_CTRL_PORT, BOARD_CCS811_ENABLE_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_CCS811_CTRL_PORT, BOARD_CCS811_WAKE_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BOARD_CCS811_I2C_PORT, BOARD_CCS811_I2C_SDA_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BOARD_CCS811_I2C_PORT, BOARD_CCS811_I2C_SCL_PIN, gpioModeDisabled, 0);

  GPIOINT_Init();

  /*********************************************************************/
  /** RGBLED / LED config                                             **/
  /*********************************************************************/
  CMU_ClockEnable(BOARD_RGBLED_CMU_CLK, true);
  GPIO_PinModeSet(BOARD_RGBLED_RED_PORT, BOARD_RGBLED_RED_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_RGBLED_GREEN_PORT, BOARD_RGBLED_GREEN_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_RGBLED_BLUE_PORT, BOARD_RGBLED_BLUE_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_RGBLED_PWR_EN_PORT, BOARD_RGBLED_PWR_EN_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_RGBLED_COM0_PORT, BOARD_RGBLED_COM0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_RGBLED_COM1_PORT, BOARD_RGBLED_COM1_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_RGBLED_COM2_PORT, BOARD_RGBLED_COM2_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_RGBLED_COM3_PORT, BOARD_RGBLED_COM3_PIN, gpioModePushPull, 0);

  pwmTimer = BOARD_RGBLED_TIMER;

  timerInit.debugRun = true;
  timerInit.prescale = timerPrescale2;
  TIMER_Init(pwmTimer, &timerInit);

  /* 65536 counts at 38.4 MHz / 2 =  293 Hz. */
  TIMER_TopSet(pwmTimer, 0xFFFF);

  ccInit.mode  = timerCCModePWM;
  TIMER_InitCC(pwmTimer, 0, &ccInit);
  TIMER_InitCC(pwmTimer, 1, &ccInit);
  TIMER_InitCC(pwmTimer, 2, &ccInit);

  pwmTimer->ROUTEPEN  = 0;
  pwmTimer->ROUTELOC0 = (BOARD_RGBLED_RED_CCLOC   << _TIMER_ROUTELOC0_CC0LOC_SHIFT)
                        | (BOARD_RGBLED_GREEN_CCLOC << _TIMER_ROUTELOC0_CC1LOC_SHIFT)
                        | (BOARD_RGBLED_BLUE_CCLOC  << _TIMER_ROUTELOC0_CC2LOC_SHIFT);

  return status;
}

/***************************************************************************//**
 * @brief
 *    Puts the Flash chip in deep power down mode
 *
 * @return
 *    Returns none
 ******************************************************************************/
void BOARD_flashDeepPowerDown(void)
{
  /* Initialize USART for SPI transactions to the flash */
  flashSPIInit();

  /* Wake up device first if it was in DP already */
  GPIO_PinOutClear(MX25_PORT_CS, MX25_PIN_CS);
  UTIL_delay(10);
  GPIO_PinOutSet(MX25_PORT_CS, MX25_PIN_CS);

  /* Send DP command ( 0xB9 ) */
  GPIO_PinOutClear(MX25_PORT_CS, MX25_PIN_CS);
  USART_SpiTransfer(MX25_USART, 0xB9);
  GPIO_PinOutSet(MX25_PORT_CS, MX25_PIN_CS);

  /* We must disable SPI communication */
  USART_Reset(MX25_USART);
  GPIO_PinModeSet(MX25_PORT_MOSI, MX25_PIN_MOSI, gpioModeDisabled, 0);
  GPIO_PinModeSet(MX25_PORT_MISO, MX25_PIN_MISO, gpioModeDisabled, 0);
  GPIO_PinModeSet(MX25_PORT_SCLK, MX25_PIN_SCLK, gpioModeDisabled, 0);

  return;
}

/***************************************************************************//**
 * @brief
 *    Gets the state of the pushbuttons
 *
 * @return
 *    Returns the state of the buttons
 ******************************************************************************/
uint8_t BOARD_pushButtonGetState(void)
{
  uint32_t portState;
  uint8_t  buttonState;

  portState   = GPIO_PortInGet(BOARD_BUTTON_PORT);
  buttonState = (uint8_t) ( (portState >> BOARD_BUTTON_SHIFT) & BOARD_BUTTON_MASK);

  buttonState = ~buttonState & BOARD_BUTTON_MASK;

  return buttonState;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the pushbutton GPIO interrupt
 *
 * @param[in] enable
 *    Set true to enable, false to disable the pushbutton interrupts
 *
 * @return
 *    None
 ******************************************************************************/
void BOARD_pushButtonEnableIRQ(bool enable)
{
  GPIO_ExtIntConfig(BOARD_BUTTON_PORT, BOARD_BUTTON_LEFT_PIN, EXTI_BUTTON0, false, true, enable);
  GPIO_ExtIntConfig(BOARD_BUTTON_PORT, BOARD_BUTTON_RIGHT_PIN, EXTI_BUTTON1, false, true, enable);
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the RGB LED power supply line
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    None
 ******************************************************************************/
void BOARD_rgbledPowerEnable(bool enable)
{
  if ( enable ) {
    GPIO_PinOutSet(BOARD_RGBLED_PWR_EN_PORT, BOARD_RGBLED_PWR_EN_PIN);
  } else {
    GPIO_PinOutClear(BOARD_RGBLED_PWR_EN_PORT, BOARD_RGBLED_PWR_EN_PIN);
  }

  return;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the RGB LED power supply line
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @param[in] mask
 *    Mask indicating which bits to modify
 *
 * @return
 *    None
 ******************************************************************************/
void BOARD_rgbledEnable(bool enable, uint8_t mask)
{
  if ( ( (mask != 0) && (enable == true) ) || ( ( (mask & 0xf) == 0xf) && (enable == false) ) ) {
    BOARD_rgbledPowerEnable(enable);
  }

  int i;
  uint8_t pins[4] = { BOARD_RGBLED_COM0_PIN,
                      BOARD_RGBLED_COM1_PIN,
                      BOARD_RGBLED_COM2_PIN,
                      BOARD_RGBLED_COM3_PIN };

  for ( i = 0; i < 4; i++ ) {
    if ( ( (mask >> i) & 1) == 1 ) {
      if ( enable ) {
        GPIO_PinOutSet(BOARD_RGBLED_COM_PORT, pins[3 - i]);
      } else {
        GPIO_PinOutClear(BOARD_RGBLED_COM_PORT, pins[3 - i]);
      }
    }
  }

  return;
}

/***************************************************************************//**
 * @brief
 *    Turns on or off the red and/or green LED
 *
 * @param[in] leds
 *    The two LSB bits determine the state of the green and red LED. If the bit
 *    is 1 then the LED will be turned on.
 *
 * @return
 *    None
 ******************************************************************************/
void BOARD_ledSet(uint8_t leds)
{
  uint8_t pins[2] = { BOARD_LED_GREEN_PIN, BOARD_LED_RED_PIN };
  for ( int i = 0; i < 2; i++ ) {
    if ( ( (leds >> i) & 1) == 1 ) {
      GPIO_PinOutSet(BOARD_LED_PORT, pins[i]);
    } else {
      GPIO_PinOutClear(BOARD_LED_PORT, pins[i]);
    }
  }
  return;
}

/***************************************************************************//**
 * @brief
 *    Sets the raw color of the RGB LEDs. The brightness is non-linear function
 *    of the raw color value.
 *
 * @param[in] red
 *    A 16-bit value, which determines the PWM value of the red channel
 *
 * @param[in] green
 *    A 16-bit value, which determines the PWM value of the green channel
 *
 * @param[in] blue
 *    A 16-bit value, which determines the PWM value of the blue channel
 *
 * @return
 *    None
 ******************************************************************************/
void BOARD_rgbledSetRawColor(uint16_t red, uint16_t green, uint16_t blue)
{
  if ( (red == 0) && (green == 0) && (blue == 0) ) {
    timerEnable = false;
    TIMER_Enable(pwmTimer, timerEnable);
    TIMER_CompareBufSet(pwmTimer, 0, 0);
    TIMER_CompareBufSet(pwmTimer, 1, 0);
    TIMER_CompareBufSet(pwmTimer, 2, 0);
    pwmTimer->ROUTEPEN  = 0;
  } else {
    timerEnable = true;
    TIMER_Enable(pwmTimer, timerEnable);
    TIMER_CompareBufSet(pwmTimer, 0, red);
    TIMER_CompareBufSet(pwmTimer, 1, green);
    TIMER_CompareBufSet(pwmTimer, 2, blue);
    pwmTimer->ROUTEPEN  = TIMER_ROUTEPEN_CC0PEN | TIMER_ROUTEPEN_CC1PEN | TIMER_ROUTEPEN_CC2PEN;
  }

  return;
}

/***************************************************************************//**
 * @brief
 *    Sets the color of the RGB LEDs. The brightness of the LEDs is almost
 *    linear to the color value.
 *
 * @param[in] red
 *    An 8-bit value, which determines the brightness of the red channel
 *
 * @param[in] green
 *    An 8-bit value, which determines the brightness of the green channel
 *
 * @param[in] blue
 *    An 8-bit value, which determines the brightness of the blue channel
 *
 * @return
 *    None
 ******************************************************************************/
void BOARD_rgbledSetColor(uint8_t red, uint8_t green, uint8_t blue)
{
  BOARD_rgbledSetRawColor(256 * (uint16_t)lightLevels[red],
                          256 * (uint16_t)lightLevels[green],
                          256 * (uint16_t)lightLevels[blue]);

  return;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the accelerometer and gyroscope sensor
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_imuEnable(bool enable)
{
  if ( enable ) {
    GPIO_PinOutSet(BOARD_IMU_ENABLE_PORT, BOARD_IMU_ENABLE_PIN);
    sensorInfo.enabled |= BOARD_ENABLED_IMU;
  } else {
    GPIO_PinOutClear(BOARD_IMU_ENABLE_PORT, BOARD_IMU_ENABLE_PIN);
    sensorInfo.enabled &= ~BOARD_ENABLED_IMU;
  }

  return BOARD_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the accelerometer and gyroscope GPIO interrupt
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_imuEnableIRQ(bool enable)
{
  GPIO_ExtIntConfig(BOARD_IMU_INT_PORT, BOARD_IMU_INT_PIN, EXTI_IMU_INT, false, true, enable);

  return BOARD_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the environmental sensor group (Pressure, RH/Temp,
 *    UV/Ambient light and Hall sensors)
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_envSensEnable(bool enable)
{
  if ( enable ) {
    /* Enable power */
    GPIO_PinOutSet(BOARD_ENV_ENABLE_PORT, BOARD_ENV_ENABLE_PIN);

    /* Setup I2C pins */
    GPIO_PinModeSet(BOARD_ENV_I2C_PORT, BOARD_ENV_I2C_SDA_PIN, gpioModeWiredAnd, 1);
    GPIO_PinModeSet(BOARD_ENV_I2C_PORT, BOARD_ENV_I2C_SCL_PIN, gpioModeWiredAnd, 1);
  } else {
    /* Disable power */
    GPIO_PinOutClear(BOARD_ENV_ENABLE_PORT, BOARD_ENV_ENABLE_PIN);

    /* Disconnect I2C pins */
    GPIO_PinModeSet(BOARD_ENV_I2C_PORT, BOARD_ENV_I2C_SDA_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BOARD_ENV_I2C_PORT, BOARD_ENV_I2C_SCL_PIN, gpioModeDisabled, 0);
  }

  return BOARD_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the Barometric Pressure sensor
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_bapEnable(bool enable)
{
  uint32_t status;

  status = BOARD_OK;

  if ( enable ) {
    /* Enable the environmental sensor group */
    status = BOARD_envSensEnable(true);
    /* If it was succesful then set the enabled bit in the sensorInfo structure for this sensor*/
    if ( status == BOARD_OK ) {
      sensorInfo.enabled |= BOARD_ENABLED_BAP;
    }
  } else {
    /* Check if other sensors enabled */
    if ( (sensorInfo.enabled & ~BOARD_ENABLED_BAP) & BOARD_ENABLED_ENV_SENSOR ) {
      /* If yes then just clear the enabled bit but do not turn off the power */
      sensorInfo.enabled &= ~BOARD_ENABLED_BAP;
    } else {
      /* If no other sensors enabled then turn off the power of the sensor group */
      status = BOARD_envSensEnable(false);
      if ( status == BOARD_OK ) {
        sensorInfo.enabled &= ~BOARD_ENABLED_BAP;
      }
    }
  }

  return status;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the RH/Temp sensor
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_rhtempEnable(bool enable)
{
  uint32_t status;

  status = BOARD_OK;

  if ( enable ) {
    /* Enable the environmental sensor group */
    status = BOARD_envSensEnable(true);
    /* If it was succesful then set the enabled bit in the sensorInfo structure for this sensor*/
    if ( status == BOARD_OK ) {
      sensorInfo.enabled |= BOARD_ENABLED_RHTEMP;
    }
  } else {
    /* Check if other sensors enabled */
    if ( (sensorInfo.enabled & ~BOARD_ENABLED_RHTEMP) & BOARD_ENABLED_ENV_SENSOR ) {
      /* If yes then just clear the enabled bit but do not turn off the power */
      sensorInfo.enabled &= ~BOARD_ENABLED_RHTEMP;
    } else {
      /* If no other sensors enabled then turn off the power of the sensor group */
      status = BOARD_envSensEnable(false);
      if ( status == BOARD_OK ) {
        sensorInfo.enabled &= ~BOARD_ENABLED_RHTEMP;
      }
    }
  }

  return status;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the UV/Ambient light sensor
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_alsEnable(bool enable)
{
  uint32_t status;

  status = BOARD_OK;

  if ( enable ) {
    /* Enable the environmental sensor group */
    status = BOARD_envSensEnable(true);
    /* If it was succesful then set the enabled bit in the sensorInfo structure for this sensor*/
    if ( status == BOARD_OK ) {
      sensorInfo.enabled |= BOARD_ENABLED_ALS;
    }
  } else {
    /* Check if other sensors enabled */
    if ( (sensorInfo.enabled & ~BOARD_ENABLED_ALS) & BOARD_ENABLED_ENV_SENSOR ) {
      /* If yes then just clear the enabled bit but do not turn off the power */
      sensorInfo.enabled &= ~BOARD_ENABLED_ALS;
    } else {
      /* If no other sensors enabled then turn off the power of the sensor group */
      status = BOARD_envSensEnable(false);
      if ( status == BOARD_OK ) {
        sensorInfo.enabled &= ~BOARD_ENABLED_ALS;
      }
    }
  }

  return status;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the UV/Ambient light sensor GPIO interrupts
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_alsEnableIRQ(bool enable)
{
  GPIO_ExtIntConfig(BOARD_UV_ALS_INT_PORT, BOARD_UV_ALS_INT_PIN, EXTI_UV_ALS_INT, false, true, enable);

  return BOARD_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the Hall sensor
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_hallSensorEnable(bool enable)
{
  /* The Hall sensor on TBS2 has separate power switch */
  if ( enable ) {
    /* Enable pin */
    GPIO_PinOutSet(BOARD_HALL_PORT, BOARD_HALL_ENABLE_PIN);

    /* Setup I2C pins */
    GPIO_PinModeSet(BOARD_HALL_PORT, BOARD_HALL_I2C_SDA_PIN, gpioModeWiredAnd, 1);
    GPIO_PinModeSet(BOARD_HALL_PORT, BOARD_HALL_I2C_SCL_PIN, gpioModeWiredAnd, 1);

    /* OUT1 pin */
    GPIO_PinModeSet(BOARD_HALL_PORT, BOARD_HALL_OUTPUT_PIN, gpioModeInput, 0);

    sensorInfo.enabled |= BOARD_ENABLED_HALL;
  } else {
    /* Enable pin */
    GPIO_PinOutClear(BOARD_HALL_PORT, BOARD_HALL_ENABLE_PIN);

    /* Setup I2C pins */
    GPIO_PinModeSet(BOARD_HALL_PORT, BOARD_HALL_I2C_SDA_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BOARD_HALL_PORT, BOARD_HALL_I2C_SCL_PIN, gpioModeDisabled, 0);

    /* OUT1 pin */
    GPIO_PinModeSet(BOARD_HALL_PORT, BOARD_HALL_OUTPUT_PIN, gpioModeDisabled, 0);

    sensorInfo.enabled &= ~BOARD_ENABLED_HALL;
  }

  return BOARD_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the Hall sensor GPIO interrupt
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_hallSensorEnableIRQ(bool enable)
{
  GPIO_ExtIntConfig(BOARD_HALL_PORT, BOARD_HALL_OUTPUT_PIN, EXTI_HALL_OUT1, true, true, enable);

  return BOARD_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the environmental sensor group (Pressure, RH/Temp,
 *    UV/Ambient light and Hall sensors) GPIO interrupts
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_envSensEnableIRQ(bool enable)
{
  /* Just for backward compatibility reasons */
  GPIO_ExtIntConfig(BOARD_UV_ALS_INT_PORT, BOARD_UV_ALS_INT_PIN, EXTI_UV_ALS_INT, false, true, enable);

  return BOARD_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the MEMS microphone
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_micEnable(bool enable)
{
  if ( enable ) {
    GPIO_PinOutSet(BOARD_MIC_ENABLE_PORT, BOARD_MIC_ENABLE_PIN);
  } else {
    GPIO_PinOutClear(BOARD_MIC_ENABLE_PORT, BOARD_MIC_ENABLE_PIN);
  }

  return BOARD_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the Air Quality / Gas Sensor
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_gasSensorEnable(bool enable)
{
  if ( enable ) {
    sensorInfo.enabled |= BOARD_ENABLED_GAS;
  } else {
    sensorInfo.enabled &= ~BOARD_ENABLED_GAS;
  }

  if ( enable ) {
    /* Enable pin */
    GPIO_PinOutSet(BOARD_CCS811_CTRL_PORT, BOARD_CCS811_ENABLE_PIN);

    /* Setup I2C pins */
    GPIO_PinModeSet(BOARD_CCS811_I2C_PORT, BOARD_CCS811_I2C_SDA_PIN, gpioModeWiredAnd, 1);
    GPIO_PinModeSet(BOARD_CCS811_I2C_PORT, BOARD_CCS811_I2C_SCL_PIN, gpioModeWiredAnd, 1);

    /* Wake pin */
    GPIO_PinModeSet(BOARD_CCS811_CTRL_PORT, BOARD_CCS811_WAKE_PIN, gpioModePushPull, 1);
  } else {
    /* Wake pin */
    GPIO_PinModeSet(BOARD_CCS811_CTRL_PORT, BOARD_CCS811_WAKE_PIN, gpioModeDisabled, 0);

    /* Setup I2C pins */
    GPIO_PinModeSet(BOARD_CCS811_I2C_PORT, BOARD_CCS811_I2C_SDA_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BOARD_CCS811_I2C_PORT, BOARD_CCS811_I2C_SCL_PIN, gpioModeDisabled, 0);

    /* Enable pin */
    GPIO_PinOutClear(BOARD_CCS811_CTRL_PORT, BOARD_CCS811_ENABLE_PIN);
  }

  return BOARD_OK;
}

/***************************************************************************//**
 * @brief
 *    Wakes up the Air Quality / Gas Sensor
 *
 * @param[in] wake
 *    Set true to wake up, false otherwise
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_gasSensorWake(bool wake)
{
  if ( wake ) {
    GPIO_PinOutClear(BOARD_CCS811_CTRL_PORT, BOARD_CCS811_WAKE_PIN);
    UTIL_delay(10);
  } else {
    GPIO_PinOutSet(BOARD_CCS811_CTRL_PORT, BOARD_CCS811_WAKE_PIN);
    UTIL_delay(10);
  }

  return BOARD_OK;
}

/***************************************************************************//**
 * @brief
 *    Enables or disables the Air Quality / Gas Sensor GPIO interrupt
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_gasSensorEnableIRQ(bool enable)
{
  GPIO_ExtIntConfig(BOARD_CCS811_CTRL_PORT, BOARD_CCS811_INT_PIN, EXTI_CCS811_INT, false, true, enable);

  return BOARD_OK;
}

/***************************************************************************//**
 * @brief
 *    Sets up the route register of the I2C device to use the correct
 *    set of pins
 *
 * * @param[in] select
 *    The I2C bus route to use (None, Environmental sensors, Gas sensor, Hall
 *    sensor)
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_i2cBusSelect(uint8_t select)
{
  uint32_t status;

  status = BOARD_OK;

  switch ( select ) {
    case BOARD_I2C_BUS_SELECT_NONE:
      I2C1->ROUTEPEN  = 0;
      I2C1->ROUTELOC0 = 0;
      sensorInfo.busInUse = BOARD_I2C_BUS_SELECT_NONE;
      break;

    case BOARD_I2C_BUS_SELECT_ENV_SENSOR:
      I2C1->ROUTELOC0 = BOARD_ENV_I2C_ROUTELOC0;
      I2C1->ROUTEPEN = (I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN);
      sensorInfo.busInUse = BOARD_I2C_BUS_SELECT_ENV_SENSOR;
      break;

    case BOARD_I2C_BUS_SELECT_GAS:
      I2C1->ROUTELOC0 = BOARD_CCS811_I2C_ROUTELOC0;
      I2C1->ROUTEPEN = (I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN);
      sensorInfo.busInUse = BOARD_I2C_BUS_SELECT_GAS;
      break;

    case BOARD_I2C_BUS_SELECT_HALL:
      I2C1->ROUTELOC0 = BOARD_HALL_I2C_ROUTELOC0;
      I2C1->ROUTEPEN = (I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN);
      sensorInfo.busInUse = BOARD_I2C_BUS_SELECT_HALL;
      break;

    default:
      status = BOARD_ERROR_I2C_BUS_SELECT_INVALID;
  }

  return status;
}

/***************************************************************************//**
 * @brief
 *    Function to register the gas sensor interrupt callback function
 *
 * @param[in] cb
 *    The callback function to be registered
 *
 * @return
 *    Returns none
 ******************************************************************************/
void BOARD_gasSensorSetIRQCallback(BOARD_IrqCallback cb)
{
  GPIOINT_CallbackRegister(EXTI_CCS811_INT, gpioInterruptHandler);
  gasSensorIRQCallback = cb;

  return;
}

/***************************************************************************//**
 * @brief
 *    Function to clear the gas sensor interrupt
 *
 * @return
 *    Returns none
 ******************************************************************************/
void BOARD_gasSensorClearIRQ(void)
{
  GPIO_IntClear(1 << EXTI_CCS811_INT);

  return;
}

/***************************************************************************//**
 * @brief
 *    Function to register the Hall sensor interrupt callback function
 *
 * @param[in] cb
 *    The callback function to be registered
 *
 * @return
 *    Returns none
 ******************************************************************************/
void BOARD_hallSensorSetIRQCallback(BOARD_IrqCallback cb)
{
  GPIOINT_CallbackRegister(EXTI_HALL_OUT1, gpioInterruptHandler);
  hallSensorIRQCallback = cb;

  return;
}

/***************************************************************************//**
 * @brief
 *    Function to clear the Hall sensor interrupt
 *
 * @return
 *    Returns none
 ******************************************************************************/
void BOARD_hallSensorClearIRQ(void)
{
  GPIO_IntClear(1 << EXTI_HALL_OUT1);
  return;
}

/***************************************************************************//**
 * @brief
 *    Function to register the ALS sensor interrupt callback function
 *
 * @param[in] cb
 *    The callback function to be registered
 *
 * @return
 *    Returns none
 ******************************************************************************/
void BOARD_alsSetIRQCallback(BOARD_IrqCallback cb)
{
  GPIOINT_CallbackRegister(EXTI_UV_ALS_INT, gpioInterruptHandler);
  alsIRQCallback = cb;

  return;
}

/***************************************************************************//**
 * @brief
 *    Function to clear the ALS sensor interrupt
 *
 * @return
 *    Returns none
 ******************************************************************************/
void BOARD_alsClearIRQ(void)
{
  GPIO_IntClear(1 << EXTI_UV_ALS_INT);
  return;
}

/***************************************************************************//**
 * @brief
 *    Function to register the IMU sensor interrupt callback function
 *
 * @param[in] cb
 *    The callback function to be registered
 *
 * @return
 *    Returns none
 *****************************************************************************/
void BOARD_imuSetIRQCallback(BOARD_IrqCallback cb)
{
  GPIOINT_CallbackRegister(EXTI_IMU_INT, gpioInterruptHandler);
  imuIRQCallback = cb;

  return;
}

/***************************************************************************//**
 * @brief
 *    Function to clear the IMU sensor interrupt
 *
 * @return
 *    Returns none
 ******************************************************************************/
void BOARD_imuClearIRQ(void)
{
  GPIO_IntClear(1 << EXTI_IMU_INT);

  return;
}

/***************************************************************************//**
 * @brief
 *    Function to register the pushbutton 0 interrupt callback function
 *
 * @param[in] cb
 *    The callback function to be registered
 *
 * @return
 *    Returns none
 ******************************************************************************/
void BOARD_pushButton0SetIRQCallback(BOARD_IrqCallback cb)
{
  GPIOINT_CallbackRegister(EXTI_BUTTON0, gpioInterruptHandler);
  pushButton0IRQCallback = cb;

  return;
}

/***************************************************************************//**
 * @brief
 *    Function to clear the pushbutton 0 interrupt
 *
 * @return
 *    Returns none
 ******************************************************************************/
void BOARD_pushButton0ClearIRQ(void)
{
  GPIO_IntClear(1 << EXTI_BUTTON0);

  return;
}

/***************************************************************************//**
 * @brief
 *    Function to register the pushbutton 1 interrupt callback function
 *
 * @param[in] cb
 *    The callback function to be registered
 *
 * @return
 *    Returns none
 ******************************************************************************/
void BOARD_pushButton1SetIRQCallback(BOARD_IrqCallback cb)
{
  GPIOINT_CallbackRegister(EXTI_BUTTON1, gpioInterruptHandler);
  pushButton1IRQCallback = cb;

  return;
}

/***************************************************************************//**
 * @brief
 *    Function to clear the pushbutton 1 interrupt
 *
 * @return
 *    Returns none
 ******************************************************************************/
void BOARD_pushButton1ClearIRQ(void)
{
  GPIO_IntClear(1 << EXTI_BUTTON1);

  return;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Initializes the SPI bus to communicate with the Flash chip
 *
 * @return
 *    Returns none
 ******************************************************************************/
static void flashSPIInit(void)
{
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  /* Enable clocks */
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(MX25_USART_CLK, true);

  /* Configure GPIO pins */
  GPIO_PinModeSet(MX25_PORT_MOSI, MX25_PIN_MOSI, gpioModePushPull, 1);
  GPIO_PinModeSet(MX25_PORT_MISO, MX25_PIN_MISO, gpioModeInput, 0);
  GPIO_PinModeSet(MX25_PORT_SCLK, MX25_PIN_SCLK, gpioModePushPull, 1);
  GPIO_PinModeSet(MX25_PORT_CS, MX25_PIN_CS, gpioModePushPull, 1);

  /* Configure USART */
  init.msbf         = true;
  init.baudrate     = 8000000;
  USART_InitSync(MX25_USART, &init);

  MX25_USART->ROUTELOC0 = (MX25_USART_LOC_MISO | MX25_USART_LOC_MOSI | MX25_USART_LOC_SCLK);
  MX25_USART->ROUTEPEN  = (USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_CLKPEN);

  /* Wait for flash to power up */
  UTIL_delay(10);

  /* Set EM4 pin retention so chip select stays high if we enter EM4 */
  EMU->EM4CTRL |= EMU_EM4CTRL_EM4IORETMODE_EM4EXIT;

  return;
}

/***************************************************************************//**
 * @brief
 *    Common callback from the GPIOINT driver
 *
 * @return
 *    Returns none
 ******************************************************************************/
static void gpioInterruptHandler(uint8_t pin)
{
  BOARD_IrqCallback callback;

  switch ( pin ) {
    case EXTI_CCS811_INT:
      callback = gasSensorIRQCallback;
      break;
    case EXTI_HALL_OUT1:
      callback = hallSensorIRQCallback;
      break;
    case EXTI_UV_ALS_INT:
      callback = alsIRQCallback;
      break;
    case EXTI_IMU_INT:
      callback = imuIRQCallback;
      break;
    case EXTI_BUTTON0:
      callback = pushButton0IRQCallback;
      break;
    case EXTI_BUTTON1:
      callback = pushButton1IRQCallback;
      break;
    default:
      callback = NULL;
  }

  if (callback != NULL) {
    callback();
  }

  return;
}
/** @endcond */

/** @} (end defgroup BOARD_4166) */
/** @} {end addtogroup TBSense_BSP} */
