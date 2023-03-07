/***************************************************************************//**
 * @file
 * @brief BOARD module source file
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
#include "thunderboard/board_4184.h"

/**************************************************************************//**
* @addtogroup BRD4184_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @defgroup BOARD_4184 BOARD Module for Thunderboard Blue
 * @{
 * @brief Board hardware control, configuraton and miscellaneous functions
 * @details This module contains functions releated to board features.
 *
 * The BOARD module uses the common I2CSPM driver to communicate with the
 * I<sup>2</sup>C sensors on the board. The following
 * board features can be enabled when needed using the BOARD Module:
 * - Environmental sensor group
 *   - RH/Temp (Si7021)
 *   - UV/ALS (Si1133)
 * - Hall effect sensor (Si7210)
 * - Inertial sensor (ICM-20648)
 * - Microphone (SPK0641HT4H)
 *
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define BOARD_ENABLED_NONE      0
#define BOARD_ENABLED_RHTEMP    (1 << 0)
#define BOARD_ENABLED_ALS       (1 << 1)
#define BOARD_ENABLED_HALL      (1 << 2)
#define BOARD_ENABLED_MIC       (1 << 3)
#define BOARD_ENABLED_IMU       (1 << 4)
#define BOARD_ENABLED_ENV_SENSOR (BOARD_ENABLED_RHTEMP | BOARD_ENABLED_ALS | BOARD_ENABLED_HALL)

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
static BOARD_SensorInfo_t sensorInfo;
static BOARD_IrqCallback imuIRQCallback;
static BOARD_IrqCallback pushButton0IRQCallback;

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
  I2CSPM_Init_TypeDef  i2cInit        = I2CSPM_INIT_DEFAULT;
  uint32_t status;

  i2cInit.port            = I2C1;
  i2cInit.sclPort         = gpioPortD;
  i2cInit.sclPin          = 3;
  i2cInit.sdaPort         = gpioPortD;
  i2cInit.sdaPin          = 2;

  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  I2CSPM_Init(&i2cInit);

  sensorInfo.enabled = 0x00;
  sensorInfo.busInUse = 0x00;

  status = BOARD_OK;

  /*********************************************************************/
  /** Push button config                                              **/
  /*********************************************************************/
  GPIO_PinModeSet(BOARD_BUTTON0_PORT, BOARD_BUTTON0_PIN, gpioModeInput, 0);

  /* Initialize LED */
  GPIO_PinModeSet(BOARD_LED0_PORT, BOARD_LED0_PIN, gpioModePushPull, 0);

  /* Configure the Interrupt pins */
  GPIO_PinModeSet(BOARD_IMU_INT_PORT, BOARD_IMU_INT_PIN, gpioModeInput, 0);

  /*********************************************************************/
  /** IMU pin config                                                  **/
  /*********************************************************************/
  GPIO_PinModeSet(BOARD_IMU_ENABLE_PORT, BOARD_IMU_ENABLE_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_IMU_SPI_PORT, BOARD_IMU_SPI_MOSI_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BOARD_IMU_SPI_PORT, BOARD_IMU_SPI_MISO_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BOARD_IMU_SPI_PORT, BOARD_IMU_SPI_SCLK_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BOARD_IMU_SPI_PORT, BOARD_IMU_SPI_CS_PIN, gpioModeDisabled, 0);

  /*********************************************************************/
  /** Sensor pin config                                               **/
  /*********************************************************************/
  GPIO_PinModeSet(BOARD_SENSOR_ENABLE_PORT, BOARD_SENSOR_ENABLE_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_SENSOR_I2C_PORT, BOARD_SENSOR_I2C_SDA_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BOARD_SENSOR_I2C_PORT, BOARD_SENSOR_I2C_SCL_PIN, gpioModeDisabled, 0);

  /*********************************************************************/
  /** Microphone pin config                                           **/
  /*********************************************************************/
  GPIO_PinModeSet(BOARD_MIC_ENABLE_PORT, BOARD_MIC_ENABLE_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BOARD_MIC_PDM_DATA_PORT, BOARD_MIC_PDM_DATA_PIN, gpioModeDisabled, 0);
  GPIO_PinModeSet(BOARD_MIC_PDM_CLK_PORT, BOARD_MIC_PDM_CLK_PIN, gpioModePushPull, 0);

  GPIOINT_Init();

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
  GPIO_ExtIntConfig(BOARD_BUTTON0_PORT, BOARD_BUTTON0_PIN, EXTI_BUTTON0, false, true, enable);
}

/***************************************************************************//**
 * @brief
 *    Turns on or off the red and/or green LED
 *
 * @param[in] leds
 *    The LSB bit determines the state of the LED.
 *    If the bit is 1 then the LED will be turned on.
 *
 * @return
 *    None
 ******************************************************************************/
void BOARD_ledSet(uint8_t leds)
{
  if ( (leds & 1) == 1 ) {
    GPIO_PinOutSet(BOARD_LED_PORT, BOARD_LED0_PIN);
  } else {
    GPIO_PinOutClear(BOARD_LED_PORT, BOARD_LED0_PIN);
  }
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
    GPIO_PinOutSet(BOARD_SENSOR_ENABLE_PORT, BOARD_SENSOR_ENABLE_PIN);

    /* Setup I2C pins */
    GPIO_PinModeSet(BOARD_SENSOR_I2C_PORT, BOARD_SENSOR_I2C_SDA_PIN, gpioModeWiredAnd, 1);
    GPIO_PinModeSet(BOARD_SENSOR_I2C_PORT, BOARD_SENSOR_I2C_SCL_PIN, gpioModeWiredAnd, 1);
  } else {
    /* Disable power */
    GPIO_PinOutClear(BOARD_SENSOR_ENABLE_PORT, BOARD_SENSOR_ENABLE_PIN);

    /* Disconnect I2C pins */
    GPIO_PinModeSet(BOARD_SENSOR_I2C_PORT, BOARD_SENSOR_I2C_SDA_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BOARD_SENSOR_I2C_PORT, BOARD_SENSOR_I2C_SCL_PIN, gpioModeDisabled, 0);
  }

  return BOARD_OK;
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
 *    Enables or disables the Hall sensor.
 *    The Hall sensor on BRD4184 is sharing power and I2C lines
 *    with the ALS and RHTemp-sensor
 *
 * @param[in] enable
 *    Set true to enable, false to disable
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t BOARD_hallSensorEnable(bool enable)
{
  uint32_t status;

  status = BOARD_OK;

  if ( enable ) {
    /* Enable the environmental sensor group */
    status = BOARD_envSensEnable(true);
    /* If it was succesful then set the enabled bit in the sensorInfo structure for this sensor*/
    if ( status == BOARD_OK ) {
      sensorInfo.enabled |= BOARD_ENABLED_HALL;
    }
  } else {
    /* Check if other sensors enabled */
    if ( (sensorInfo.enabled & ~BOARD_ENABLED_HALL) & BOARD_ENABLED_ENV_SENSOR ) {
      /* If yes then just clear the enabled bit but do not turn off the power */
      sensorInfo.enabled &= ~BOARD_ENABLED_HALL;
    } else {
      /* If no other sensors enabled then turn off the power of the sensor group */
      status = BOARD_envSensEnable(false);
      if ( status == BOARD_OK ) {
        sensorInfo.enabled &= ~BOARD_ENABLED_HALL;
      }
    }
  }

  return status;
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
    case EXTI_IMU_INT:
      callback = imuIRQCallback;
      break;
    case EXTI_BUTTON0:
      callback = pushButton0IRQCallback;
      break;
    default:
      callback = NULL;
  }

  if (callback != NULL) {
    callback();
  }

  return;
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

  (void) select;

  status = BOARD_OK;

  return status;
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
  return BOARD_OK;
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
  /* Just for backward compatibility reasons */
  return;
}

/** @endcond */

/** @} (end defgroup BOARD_4184) */
/** @} {end addtogroup BRD4184_BSP} */
