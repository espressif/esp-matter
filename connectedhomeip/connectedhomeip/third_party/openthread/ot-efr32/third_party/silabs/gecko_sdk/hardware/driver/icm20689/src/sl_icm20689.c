/***************************************************************************//**
 * @file
 * @brief Driver for the Invensense ICM20689 6-axis motion sensor
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_icm20689.h"

#include <stdint.h>
#include <stdio.h>

#include "em_assert.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_eusart.h"

#include "sl_sleeptimer.h"
#include "sl_icm20689_config.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
static void sl_icm20689_chip_select_set(bool select);
static sl_status_t sl_icm20689_masked_write(uint8_t addr, uint8_t data, uint8_t mask);

/* Concatenate preprocessor tokens A and B. */
#define SL_CONCAT(A, B) A ## B

/* Generate the cmu clock symbol based on instance. */
#define ICM20689_SPI_CLK(N) SL_CONCAT(cmuClock_EUSART, N)
/** @endcond */

/***************************************************************************//**
 *    Initializes the ICM20689 sensor. Enables the power supply and SPI lines,
 *    sets up the host SPI controller, configures the chip control interface,
 *    clock generator and interrupt line.
 ******************************************************************************/
sl_status_t sl_icm20689_init(void)
{
  uint8_t data;

  /* Enable and setup the SPI bus */
  sl_icm20689_spi_init();

  /* Issue reset */
  sl_icm20689_reset();

  /* Read Who am I register, should get 0x98 */
  sl_icm20689_read_register(ICM20689_REG_WHO_AM_I, 1, &data);

  /* If not - return */
  if (data != ICM20689_DEVICE_ID) {
    return SL_STATUS_INITIALIZATION;
  }

  /* Disable I2C interface, use SPI */
  sl_icm20689_write_register(ICM20689_REG_USER_CTRL, ICM20689_USER_CTRL_BIT_I2C_IF_DIS | ICM20689_USER_CTRL_BIT_SIG_COND_RST);

  /* Set clock select to automatic clock source selection and clear the sleep bit */
  sl_icm20689_write_register(ICM20689_REG_PWR_MGMT_1, _ICM20689_PWR_MGMT_1_CLKSEL_AUTOSEL_0);

  /* PLL startup time - maybe it is too long but better be on the safe side, no spec in the datasheet */
  sl_sleeptimer_delay_millisecond(30);

  /* INT pin: active low, open drain, IT status read clears. It seems that latched mode does not work, the INT pin cannot be cleared if set */
  sl_icm20689_write_register(ICM20689_REG_INT_PIN_CFG, ICM20689_INT_PIN_CFG_BIT_INT_OPEN
                             | ICM20689_INT_PIN_CFG_BIT_INT_LEVEL);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    De-initializes the ICM20689 sensor.
 ******************************************************************************/
sl_status_t sl_icm20689_deinit(void)
{
  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Performs soft reset on the ICM20689 chip
 ******************************************************************************/
sl_status_t sl_icm20689_reset(void)
{
  /* Set H_RESET bit to initiate soft reset */
  sl_icm20689_write_register(ICM20689_REG_PWR_MGMT_1, ICM20689_PWR_MGMT_1_BIT_DEVICE_RESET);

  /* Wait 100ms to complete the reset sequence */
  sl_sleeptimer_delay_millisecond(100);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Reads register from the ICM20689 device
 ******************************************************************************/
sl_status_t sl_icm20689_read_register(uint8_t addr, int num_bytes, uint8_t *data)
{
  /* Enable chip select */
  sl_icm20689_chip_select_set(true);

  /* Set R/W bit to 1 - read */
  EUSART_Spi_TxRx(SL_ICM20689_SPI_EUSART_PERIPHERAL, (addr | 0x80));

  /* Transmit 0's to provide clock and read the data */
  while ( num_bytes-- ) {
    *data++ = EUSART_Spi_TxRx(SL_ICM20689_SPI_EUSART_PERIPHERAL, 0x00);
  }

  /* Disable chip select */
  sl_icm20689_chip_select_set(false);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Writes a register in the ICM20689 device
 ******************************************************************************/
sl_status_t sl_icm20689_write_register(uint8_t addr, uint8_t data)
{
  /* Enable chip select */
  sl_icm20689_chip_select_set(true);

  /* clear R/W bit - write, send the address */
  EUSART_Spi_TxRx(SL_ICM20689_SPI_EUSART_PERIPHERAL, (addr & 0x7F));

  /* Send the data */
  EUSART_Spi_TxRx(SL_ICM20689_SPI_EUSART_PERIPHERAL, data);

  /* Disable chip select */
  sl_icm20689_chip_select_set(false);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Initializes the SPI bus in order to communicate with the ICM20689
 ******************************************************************************/
sl_status_t sl_icm20689_spi_init(void)
{
  EUSART_TypeDef *eusart = SL_ICM20689_SPI_EUSART_PERIPHERAL;
  EUSART_SpiInit_TypeDef init = EUSART_SPI_MASTER_INIT_DEFAULT_HF;
  EUSART_SpiAdvancedInit_TypeDef advancedInit = EUSART_SPI_ADVANCED_INIT_DEFAULT;

  init.bitRate = 3300000UL;
  init.advancedSettings = &advancedInit;

  advancedInit.autoCsEnable = false;
  advancedInit.msbFirst = true;

  /* Enabling clock to EUSART */
  CMU_ClockEnable(ICM20689_SPI_CLK(SL_ICM20689_SPI_EUSART_PERIPHERAL_NO), true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* IO configuration */
  GPIO_PinModeSet(SL_ICM20689_SPI_EUSART_TX_PORT, SL_ICM20689_SPI_EUSART_TX_PIN, gpioModePushPull, 0);      /* TX - MOSI */
  GPIO_PinModeSet(SL_ICM20689_SPI_EUSART_RX_PORT, SL_ICM20689_SPI_EUSART_RX_PIN, gpioModeInput, 0);         /* RX - MISO */
  GPIO_PinModeSet(SL_ICM20689_SPI_EUSART_SCLK_PORT, SL_ICM20689_SPI_EUSART_SCLK_PIN, gpioModePushPull, 0);  /* Clock */
  GPIO_PinModeSet(SL_ICM20689_SPI_EUSART_CS_PORT, SL_ICM20689_SPI_EUSART_CS_PIN, gpioModePushPull, 1);      /* CS */

  /* Initialize EUSART, in SPI master mode. */
  EUSART_SpiInit(eusart, &init);

  /* Enable pins at correct EUSART location. */
  GPIO->EUSARTROUTE[SL_ICM20689_SPI_EUSART_PERIPHERAL_NO].TXROUTE = ((SL_ICM20689_SPI_EUSART_TX_PORT << _GPIO_EUSART_TXROUTE_PORT_SHIFT) | (SL_ICM20689_SPI_EUSART_TX_PIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT));
  GPIO->EUSARTROUTE[SL_ICM20689_SPI_EUSART_PERIPHERAL_NO].RXROUTE = ((SL_ICM20689_SPI_EUSART_RX_PORT << _GPIO_EUSART_RXROUTE_PORT_SHIFT) | (SL_ICM20689_SPI_EUSART_RX_PIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT));
  GPIO->EUSARTROUTE[SL_ICM20689_SPI_EUSART_PERIPHERAL_NO].SCLKROUTE = ((SL_ICM20689_SPI_EUSART_SCLK_PORT << _GPIO_EUSART_SCLKROUTE_PORT_SHIFT) | (SL_ICM20689_SPI_EUSART_SCLK_PIN << _GPIO_EUSART_SCLKROUTE_PIN_SHIFT));
  GPIO->EUSARTROUTE[SL_ICM20689_SPI_EUSART_PERIPHERAL_NO].ROUTEEN = GPIO_EUSART_ROUTEEN_RXPEN | GPIO_EUSART_ROUTEEN_TXPEN | GPIO_EUSART_ROUTEEN_SCLKPEN;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Sets the sample rate both of the accelerometer and the gyroscope.
 ******************************************************************************/
float sl_icm20689_set_sample_rate(float sample_rate)
{
#define INTERNAL_SAMPLE_RATE (1000.0)

  uint8_t div;

  /* Calculate the sample rate divider */
  sample_rate = (INTERNAL_SAMPLE_RATE / sample_rate) - 1.0;

  /* Check if it fits in the divider registers */
  if ( sample_rate > 255.0 ) {
    sample_rate = 255.0;
  }

  if ( sample_rate < 0.0 ) {
    sample_rate = 0.0;
  }

  /* Write the value to the registers */
  div = (uint8_t) sample_rate;
  sl_icm20689_write_register(ICM20689_REG_SMPLRT_DIV, div);

  /* Calculate the actual sample rate from the divider value */
  sample_rate = INTERNAL_SAMPLE_RATE / (div + 1);

  return sample_rate;
}

/***************************************************************************//**
 *    Sets the bandwidth of the gyroscope
 ******************************************************************************/
sl_status_t sl_icm20689_gyro_set_bandwidth(sl_gyro_bandwidth_t gyro_bandwidth)
{
  uint8_t f_choice, dlpf_config;

  f_choice = (gyro_bandwidth >> GYRO_BANDWITDH_ENUM_FCHOICE_SHIFT) & ICM20689_GYRO_CONFIG_MASK_FCHOICE_B;
  dlpf_config = gyro_bandwidth & ICM20689_CONFIG_MASK_DLPF_CFG;

  /* Read the GYRO_CONFIG register */
  sl_icm20689_masked_write(ICM20689_REG_GYRO_CONFIG, f_choice << ICM20689_GYRO_CONFIG_SHIFT_FCHOICE_B, ICM20689_GYRO_CONFIG_MASK_FCHOICE_B);

  /* Read the CONFIG register */
  sl_icm20689_masked_write(ICM20689_REG_CONFIG, dlpf_config << ICM20689_CONFIG_SHIFT_DLPF_CFG, ICM20689_CONFIG_MASK_DLPF_CFG);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Sets the bandwidth of the accelerometer
 ******************************************************************************/
sl_status_t sl_icm20689_accel_set_bandwidth(sl_accel_bandwidth_t accel_bandwidth)
{
  return sl_icm20689_masked_write(ICM20689_REG_ACCEL_CONFIG2, accel_bandwidth, (ICM20689_ACCEL_CONFIG2_MASK_A_DLPF_CFG
                                                                                | ICM20689_ACCEL_CONFIG2_MASK_ACCEL_FCHOICE_B));
}

/***************************************************************************//**
 *    Reads the raw acceleration value and converts to g value based on
 *    the actual resolution
 ******************************************************************************/
sl_status_t sl_icm20689_accel_read_data(float accel[3])
{
  uint8_t raw_data[6];
  float accel_res;
  int16_t temp;

  /* Retrieve the current resolution */
  sl_icm20689_accel_get_resolution(&accel_res);

  /* Read the six raw data registers into data array */
  sl_icm20689_read_register(ICM20689_REG_ACCEL_XOUT_H, 6, &raw_data[0]);

  /* Convert the MSB and LSB into a signed 16-bit value and multiply by the resolution to get the dps value */
  temp = ( (int16_t) raw_data[0] << 8) | raw_data[1];
  accel[0] = (float) temp * accel_res;

  temp = ( (int16_t) raw_data[2] << 8) | raw_data[3];
  accel[1] = (float) temp * accel_res;

  temp = ( (int16_t) raw_data[4] << 8) | raw_data[5];
  accel[2] = (float) temp * accel_res;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Reads the raw gyroscope value and converts to deg/sec value based on
 *    the actual resolution
 ******************************************************************************/
sl_status_t sl_icm20689_gyro_read_data(float gyro[3])
{
  uint8_t raw_data[6];
  float gyro_res;
  int16_t temp;

  /* Read the six raw data registers into data array */
  sl_icm20689_read_register(ICM20689_REG_GYRO_XOUT_H, 6, &raw_data[0]);

  /* Retrieve the current resolution */
  sl_icm20689_gyro_get_resolution(&gyro_res);

  /* Convert the MSB and LSB into a signed 16-bit value and multiply by the resolution to get the dps value */
  temp = ( (int16_t) raw_data[0] << 8) | raw_data[1];
  gyro[0] = (float) temp * gyro_res;

  temp = ( (int16_t) raw_data[2] << 8) | raw_data[3];
  gyro[1] = (float) temp * gyro_res;

  temp = ( (int16_t) raw_data[4] << 8) | raw_data[5];
  gyro[2] = (float) temp * gyro_res;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Gets the actual resolution of the accelerometer
 ******************************************************************************/
sl_status_t sl_icm20689_accel_get_resolution(float *accel_res)
{
  uint8_t reg;

  /* Read the actual acceleration full scale setting */
  sl_icm20689_read_register(ICM20689_REG_ACCEL_CONFIG, 1, &reg);
  reg &= ICM20689_ACCEL_CONFIG_MASK_ACCEL_FS_SEL;

  /* Calculate the resolution */
  switch ( reg ) {
    case ICM20689_ACCEL_CONFIG_ACCEL_FS_SEL_2G:
      *accel_res = 2.0 / 32768.0;
      break;

    case ICM20689_ACCEL_CONFIG_ACCEL_FS_SEL_4G:
      *accel_res = 4.0 / 32768.0;
      break;

    case ICM20689_ACCEL_CONFIG_ACCEL_FS_SEL_8G:
      *accel_res = 8.0 / 32768.0;
      break;

    case ICM20689_ACCEL_CONFIG_ACCEL_FS_SEL_16G:
      *accel_res = 16.0 / 32768.0;
      break;

    default:
      EFM_ASSERT(false);
      return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Gets the actual resolution of the gyroscope
 ******************************************************************************/
sl_status_t sl_icm20689_gyro_get_resolution(float *gyro_res)
{
  uint8_t reg;

  /* Read the actual gyroscope full scale setting */
  sl_icm20689_read_register(ICM20689_REG_GYRO_CONFIG, 1, &reg);
  reg &= ICM20689_GYRO_CONFIG_MASK_FS_SEL;

  /* Calculate the resolution */
  switch ( reg ) {
    case ICM20689_GYRO_CONFIG_FS_SEL_250_DPS:
      *gyro_res = 250.0 / 32768.0;
      break;

    case ICM20689_GYRO_CONFIG_FS_SEL_500_DPS:
      *gyro_res = 500.0 / 32768.0;
      break;

    case ICM20689_GYRO_CONFIG_FS_SEL_1000_DPS:
      *gyro_res = 1000.0 / 32768.0;
      break;

    case ICM20689_GYRO_CONFIG_FS_SEL_2000_DPS:
      *gyro_res = 2000.0 / 32768.0;
      break;

    default:
      EFM_ASSERT(false);
      return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Sets the full scale value of the accelerometer
 ******************************************************************************/
sl_status_t sl_icm20689_accel_set_full_scale(sl_accel_full_scale_t accel_fs)
{
  uint8_t reg;

  sl_icm20689_read_register(ICM20689_REG_ACCEL_CONFIG, 1, &reg);

  reg &= ~(ICM20689_ACCEL_CONFIG_MASK_ACCEL_FS_SEL);
  reg |= accel_fs;

  sl_icm20689_write_register(ICM20689_REG_ACCEL_CONFIG, reg);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Sets the full scale value of the gyroscope
 ******************************************************************************/
sl_status_t sl_icm20689_gyro_set_full_scale(sl_gyro_full_scale_t gyro_fs)
{
  uint8_t reg;

  sl_icm20689_read_register(ICM20689_REG_GYRO_CONFIG, 1, &reg);

  reg &= ~(ICM20689_GYRO_CONFIG_MASK_FS_SEL);
  reg |= gyro_fs;

  sl_icm20689_write_register(ICM20689_REG_GYRO_CONFIG, reg);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Enables or disables the sleep mode of the device
 ******************************************************************************/
sl_status_t sl_icm20689_enable_sleep_mode(bool enable)
{
  uint8_t data;

  if ( enable ) {
    /* Sleep: set the SLEEP bit */
    data = ICM20689_PWR_MGMT_1_BIT_SLEEP;
  } else {
    /* Wake up: clear the SLEEP bit */
    data = 0;
  }

  return sl_icm20689_masked_write(ICM20689_REG_PWR_MGMT_1, data, ICM20689_PWR_MGMT_1_MASK_SLEEP);
}

/***************************************************************************//**
 *    Enables or disables the sensors in the ICM20689 chip
 ******************************************************************************/
sl_status_t sl_icm20689_enable_sensor(bool accel, bool gyro, bool temp)
{
  uint8_t pwrManagement1;
  uint8_t pwrManagement2;

  pwrManagement1 = 0U;
  pwrManagement2 = 0U;

  /* To enable the temperature sensor clear the TEMP_DIS bit in PWR_MGMT_1 */
  if (!temp) {
    pwrManagement1 = ICM20689_PWR_MGMT_1_BIT_TEMP_DIS;
  }

  /* To enable the accelerometer clear the DISABLE_ACCEL bits in PWR_MGMT_2 */
  if (!accel) {
    pwrManagement2 |= ICM20689_PWR_MGMT_2_BIT_STBY_ZA
                      | ICM20689_PWR_MGMT_2_BIT_STBY_YA
                      | ICM20689_PWR_MGMT_2_BIT_STBY_XA;
  }

  /* To enable gyro clear the DISABLE_GYRO bits in PWR_MGMT_2 */
  if (!gyro) {
    pwrManagement2 |= ICM20689_PWR_MGMT_2_BIT_STBY_ZG
                      | ICM20689_PWR_MGMT_2_BIT_STBY_YG
                      | ICM20689_PWR_MGMT_2_BIT_STBY_XG;
  }

  /* Write back the modified values */
  sl_icm20689_masked_write(ICM20689_REG_PWR_MGMT_1, pwrManagement1, ICM20689_PWR_MGMT_1_MASK_TEMP_DIS);

  sl_icm20689_masked_write(ICM20689_REG_PWR_MGMT_2, pwrManagement2, ICM20689_PWR_MGMT_2_MASK_STBY_ZG
                           | ICM20689_PWR_MGMT_2_MASK_STBY_YG
                           | ICM20689_PWR_MGMT_2_MASK_STBY_XG
                           | ICM20689_PWR_MGMT_2_MASK_STBY_ZA
                           | ICM20689_PWR_MGMT_2_MASK_STBY_YA
                           | ICM20689_PWR_MGMT_2_MASK_STBY_XA);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Enables or disables the interrupts in the ICM20689 chip
 ******************************************************************************/
sl_status_t sl_icm20689_enable_interrupt(bool data_ready_enable, bool wom_enable)
{
  uint8_t int_enable;

  /* All interrupts disabled by default */
  int_enable = 0;

  /* Enable one or both of the interrupt sources if required */
  if ( wom_enable ) {
    int_enable |= ICM20689_INT_ENABLE_MASK_WOM_INT_EN;
  }

  if ( data_ready_enable ) {
    int_enable |= ICM20689_INT_ENABLE_MASK_DATA_RDY_INT_EN;
  }

  /* Write value to register */
  sl_icm20689_masked_write(ICM20689_REG_INT_ENABLE, int_enable, ICM20689_INT_ENABLE_MASK_DATA_RDY_INT_EN | ICM20689_INT_ENABLE_MASK_WOM_INT_EN);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Reads the interrupt status registers of the ICM20689 chip
 ******************************************************************************/
sl_status_t sl_icm20689_read_interrupt_status(uint32_t *int_status)
{
  uint8_t reg[4];

  sl_icm20689_read_register(ICM20689_REG_INT_STATUS, 4, reg);
  *int_status = (uint32_t) reg[0];
  *int_status |= ( ( (uint32_t) reg[1]) << 8);
  *int_status |= ( ( (uint32_t) reg[2]) << 16);
  *int_status |= ( ( (uint32_t) reg[3]) << 24);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Checks if new data is available for read
 ******************************************************************************/
bool sl_icm20689_is_data_ready(void)
{
  uint8_t status;
  bool ret;

  ret = false;
  sl_icm20689_read_register(ICM20689_REG_INT_STATUS, 1, &status);

  if ( status & ICM20689_INT_STATUS_BIT_DATA_RDY_INT ) {
    ret = true;
  }

  return ret;
}

/***************************************************************************//**
 *    Accelerometer and gyroscope calibration function. Reads the gyroscope
 *    and accelerometer values while the device is at rest and in level. The
 *    resulting values are loaded to the accel and gyro bias registers to cancel
 *    the static offset error.
 ******************************************************************************/
sl_status_t sl_icm20689_calibrate_accel_and_gyro(float *accel_bias_scaled, float *gyro_bias_scaled)
{
#define FIFO_SAMPLE_COUNT (12 * 42)

  uint8_t data[12];
  uint16_t i, packet_count, fifo_count;
  int32_t gyro_bias[3] = { 0, 0, 0 };
  int32_t accel_bias[3] = { 0, 0, 0 };
  int32_t accel_temp[3];
  int32_t gyro_temp[3];
  int32_t accel_bias_factory[3];
  int32_t gyro_bias_stored[3];
  float gyro_res, accel_res;

  sl_icm20689_enable_sensor(false, false, false);

  /* Set 1kHz sample rate */
  sl_icm20689_set_sample_rate(1000.0);

  /* 246Hz BW for the accelerometer and 200Hz for the gyroscope */
  sl_icm20689_accel_set_bandwidth(sl_accelBandwidthDlpf218_1_0Hz);
  sl_icm20689_gyro_set_bandwidth(sl_gyroBandwidthDlpf176Hz);

  /* Set the most sensitive range: 2G full scale and 250dps full scale */
  sl_icm20689_accel_set_full_scale(sl_accelFullscale2G);
  sl_icm20689_gyro_set_full_scale(sl_gyroFullscale250Dps);

  /* Retrieve the resolution per bit */
  sl_icm20689_accel_get_resolution(&accel_res);
  sl_icm20689_gyro_get_resolution(&gyro_res);

  /* Disable the FIFO */
  sl_icm20689_write_register(ICM20689_REG_CONFIG, ICM20689_CONFIG_BIT_FIFO_MODE);

  /* Enable the accelerometer and the gyro */
  sl_icm20689_enable_sensor(true, true, false);

  /* The accel sensor needs max 30ms, the gyro max 35ms to fully start */
  /* Experiments show that the gyro needs more time to get reliable results */
  sl_sleeptimer_delay_millisecond(50);

  /* Reset the FIFO */
  sl_icm20689_write_register(ICM20689_REG_USER_CTRL, ICM20689_USER_CTRL_BIT_FIFO_EN | ICM20689_USER_CTRL_BIT_FIFO_RST);

  // Enable the different sensors to write to the FIFO
  sl_icm20689_write_register(ICM20689_REG_FIFO_EN, ICM20689_FIFO_EN_BIT_ACCEL_FIFO_EN
                             | ICM20689_FIFO_EN_BIT_ZG_FIFO_EN
                             | ICM20689_FIFO_EN_BIT_YG_FIFO_EN
                             | ICM20689_FIFO_EN_BIT_XG_FIFO_EN);

  /* The max FIFO size is 4096 bytes, but is limited to 512. */
  fifo_count = 0;
  while ( fifo_count < FIFO_SAMPLE_COUNT ) {
    sl_sleeptimer_delay_millisecond(5);
    /* Read FIFO sample count */
    sl_icm20689_read_register(ICM20689_REG_FIFO_COUNTH, 2, &data[0]);
    /* Convert to a 16 bit value */
    fifo_count = ( (uint16_t) (data[0] << 8) | data[1]);
  }

  /* Disable accelerometer and gyro to store the data in FIFO */
  sl_icm20689_masked_write(ICM20689_REG_FIFO_EN, 0U, ICM20689_FIFO_EN_MASK_ACCEL_FIFO_EN
                           | ICM20689_FIFO_EN_MASK_ZG_FIFO_EN
                           | ICM20689_FIFO_EN_MASK_YG_FIFO_EN
                           | ICM20689_FIFO_EN_MASK_XG_FIFO_EN);

  /* Read FIFO sample count */
  sl_icm20689_read_register(ICM20689_REG_FIFO_COUNTH, 2, &data[0]);

  /* Convert to a 16 bit value */
  fifo_count = ( (uint16_t) (data[0] << 8) | data[1]);

  /* Calculate the number of data sets (3 axis of accel an gyro, two bytes each = 12 bytes) */
  packet_count = fifo_count / 12;
  /* Retrieve the data from the FIFO */
  for ( i = 0; i < packet_count; i++ ) {
    sl_icm20689_read_register(ICM20689_REG_FIFO_R_W, sizeof(data), &data[0]);
    /* Convert to 16 bit signed accel and gyro x,y and z values */
    accel_temp[0] = ( (int16_t) (data[0] << 8) | data[1]);
    accel_temp[1] = ( (int16_t) (data[2] << 8) | data[3]);
    accel_temp[2] = ( (int16_t) (data[4] << 8) | data[5]);
    gyro_temp[0] = ( (int16_t) (data[6] << 8) | data[7]);
    gyro_temp[1] = ( (int16_t) (data[8] << 8) | data[9]);
    gyro_temp[2] = ( (int16_t) (data[10] << 8) | data[11]);

    /* Sum the values */
    accel_bias[0] += accel_temp[0];
    accel_bias[1] += accel_temp[1];
    accel_bias[2] += accel_temp[2];
    gyro_bias[0] += gyro_temp[0];
    gyro_bias[1] += gyro_temp[1];
    gyro_bias[2] += gyro_temp[2];
  }

  /* Divide by packet count to get the average */
  accel_bias[0] /= packet_count;
  accel_bias[1] /= packet_count;
  accel_bias[2] /= packet_count;
  gyro_bias[0] /= packet_count;
  gyro_bias[1] /= packet_count;
  gyro_bias[2] /= packet_count;

  /* Acceleormeter: add or remove (depending on the orientation of the chip) 1G (gravity) from the Z axis value */
  if ( accel_bias[2] > 0L ) {
    accel_bias[2] -= (int32_t) (1.0 / accel_res);
  } else {
    accel_bias[2] += (int32_t) (1.0 / accel_res);
  }

  /* Convert the values to degrees per sec for displaying */
  gyro_bias_scaled[0] = (float) gyro_bias[0] * gyro_res;
  gyro_bias_scaled[1] = (float) gyro_bias[1] * gyro_res;
  gyro_bias_scaled[2] = (float) gyro_bias[2] * gyro_res;

  /* Read stored gyro trim values. After reset these values are all 0 */
  sl_icm20689_read_register(ICM20689_REG_XG_OFFS_USRH, 2, &data[0]);
  gyro_bias_stored[0] = ( (int16_t) (data[0] << 8) | data[1]);
  sl_icm20689_read_register(ICM20689_REG_YG_OFFS_USRH, 2, &data[0]);
  gyro_bias_stored[1] = ( (int16_t) (data[0] << 8) | data[1]);
  sl_icm20689_read_register(ICM20689_REG_ZG_OFFS_USRH, 2, &data[0]);
  gyro_bias_stored[2] = ( (int16_t) (data[0] << 8) | data[1]);

  /* The gyro bias should be stored in 1000dps full scaled format. We measured in 250dps to get */
  /* the best sensitivity, so need to divide by 4 */
  /* Substract from the stored calibration value */
  gyro_bias_stored[0] -= gyro_bias[0] / 4;
  gyro_bias_stored[1] -= gyro_bias[1] / 4;
  gyro_bias_stored[2] -= gyro_bias[2] / 4;

  /* Split the values into two bytes */
  data[0] = (gyro_bias_stored[0] >> 8) & 0xFF;
  data[1] = (gyro_bias_stored[0]) & 0xFF;
  data[2] = (gyro_bias_stored[1] >> 8) & 0xFF;
  data[3] = (gyro_bias_stored[1]) & 0xFF;
  data[4] = (gyro_bias_stored[2] >> 8) & 0xFF;
  data[5] = (gyro_bias_stored[2]) & 0xFF;

  /* Write the  gyro bias values to the chip */
  sl_icm20689_write_register(ICM20689_REG_XG_OFFS_USRH, data[0]);
  sl_icm20689_write_register(ICM20689_REG_XG_OFFS_USRL, data[1]);
  sl_icm20689_write_register(ICM20689_REG_YG_OFFS_USRH, data[2]);
  sl_icm20689_write_register(ICM20689_REG_YG_OFFS_USRL, data[3]);
  sl_icm20689_write_register(ICM20689_REG_ZG_OFFS_USRH, data[4]);
  sl_icm20689_write_register(ICM20689_REG_ZG_OFFS_USRL, data[5]);

  /* Calculate the accelerometer bias values to store in the hardware accelerometer bias registers. These registers contain */
  /* factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold */
  /* non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature */
  /* compensation calculations(? the datasheet is not clear). Accelerometer bias registers expect bias input */
  /* as 2048 LSB per g, so that the accelerometer biases calculated above must be divided by 8. */

  /* Read factory accelerometer trim values */
  sl_icm20689_read_register(ICM20689_REG_XA_OFFSET_H, 2, &data[0]);
  accel_bias_factory[0] = ( (int16_t) (data[0] << 8) | data[1]);
  sl_icm20689_read_register(ICM20689_REG_YA_OFFSET_H, 2, &data[0]);
  accel_bias_factory[1] = ( (int16_t) (data[0] << 8) | data[1]);
  sl_icm20689_read_register(ICM20689_REG_ZA_OFFSET_H, 2, &data[0]);
  accel_bias_factory[2] = ( (int16_t) (data[0] << 8) | data[1]);

  /* Construct total accelerometer bias, including calculated average accelerometer bias from above */
  /* Scale the 2g full scale (most sensitive range) results to 16g full scale - divide by 8 */
  /* Clear the last bit (temperature compensation? - the datasheet is not clear) */
  /* Substract from the factory calibration value */

  accel_bias_factory[0] -= ( (accel_bias[0] / 8) & ~1);
  accel_bias_factory[1] -= ( (accel_bias[1] / 8) & ~1);
  accel_bias_factory[2] -= ( (accel_bias[2] / 8) & ~1);

  /* Split the values into two bytes */
  data[0] = (accel_bias_factory[0] >> 8) & 0xFF;
  data[1] = (accel_bias_factory[0]) & 0xFF;
  data[2] = (accel_bias_factory[1] >> 8) & 0xFF;
  data[3] = (accel_bias_factory[1]) & 0xFF;
  data[4] = (accel_bias_factory[2] >> 8) & 0xFF;
  data[5] = (accel_bias_factory[2]) & 0xFF;

  /* Store them in the accelerometer offset registers */
  sl_icm20689_write_register(ICM20689_REG_XA_OFFSET_H, data[0]);
  sl_icm20689_write_register(ICM20689_REG_XA_OFFSET_L, data[1]);
  sl_icm20689_write_register(ICM20689_REG_YA_OFFSET_H, data[2]);
  sl_icm20689_write_register(ICM20689_REG_YA_OFFSET_L, data[3]);
  sl_icm20689_write_register(ICM20689_REG_ZA_OFFSET_H, data[4]);
  sl_icm20689_write_register(ICM20689_REG_ZA_OFFSET_L, data[5]);

  /* Convert the values to G for displaying */
  accel_bias_scaled[0] = (float) accel_bias[0] * accel_res;
  accel_bias_scaled[1] = (float) accel_bias[1] * accel_res;
  accel_bias_scaled[2] = (float) accel_bias[2] * accel_res;

  /* Turn off FIFO */
  sl_icm20689_masked_write(ICM20689_REG_USER_CTRL, 0x00, ICM20689_USER_CTRL_MASK_FIFO_EN);

  /* Disable all sensors */
  sl_icm20689_enable_sensor(false, false, false);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Gyroscope calibration function. Reads the gyroscope
 *    values while the device is at rest and in level. The
 *    resulting values are loaded to the gyro bias registers to cancel
 *    the static offset error.
 ******************************************************************************/
sl_status_t sl_icm20689_calibrate_gyro(float *gyro_bias_scaled)
{
#define FIFO_SAMPLE_COUNT (12 * 42)

  uint8_t data[12];
  uint16_t i, packet_count, fifo_count;
  int32_t gyro_bias[3] = { 0, 0, 0 };
  int32_t gyro_temp[3];
  int32_t gyro_bias_stored[3];
  float gyro_res;

  /* Enable the accelerometer and the gyro */
  sl_icm20689_enable_sensor(true, true, false);

  /* Set 1kHz sample rate */
  sl_icm20689_set_sample_rate(1000.0);

  /* Configure bandwidth for gyroscope to 10Hz */
  sl_icm20689_gyro_set_bandwidth(sl_gyroBandwidthDlpf10Hz);

  /* Configure sensitivity to 250dps full scale */
  sl_icm20689_gyro_set_full_scale(sl_gyroFullscale250Dps);

  /* Retrieve the resolution per bit */
  sl_icm20689_gyro_get_resolution(&gyro_res);

  /* The accel sensor needs max 30ms, the gyro max 35ms to fully start */
  /* Experiments show that the gyro needs more time to get reliable results */
  sl_sleeptimer_delay_millisecond(50);

  sl_icm20689_write_register(ICM20689_REG_CONFIG, ICM20689_CONFIG_BIT_FIFO_MODE);

  /* Disable the FIFO and reset the FIFO */
  sl_icm20689_write_register(ICM20689_REG_USER_CTRL, ICM20689_USER_CTRL_BIT_FIFO_EN | ICM20689_USER_CTRL_BIT_FIFO_RST);

  /* Enable accelerometer and gyro to store the data in FIFO */
  sl_icm20689_write_register(ICM20689_REG_FIFO_EN, ICM20689_FIFO_EN_BIT_ACCEL_FIFO_EN
                             | ICM20689_FIFO_EN_BIT_ZG_FIFO_EN
                             | ICM20689_FIFO_EN_BIT_YG_FIFO_EN
                             | ICM20689_FIFO_EN_BIT_XG_FIFO_EN);

  /* The max FIFO size is 4096 bytes, but is limited to 512. */
  fifo_count = 0;
  while ( fifo_count < FIFO_SAMPLE_COUNT ) {
    sl_sleeptimer_delay_millisecond(5);

    /* Read FIFO sample count */
    sl_icm20689_read_register(ICM20689_REG_FIFO_COUNTH, 2, &data[0]);

    /* Convert to a 16 bit value */
    fifo_count = ( (uint16_t) (data[0] << 8) | data[1]);
  }

  /* Disable accelerometer and gyro to store the data in FIFO */
  sl_icm20689_masked_write(ICM20689_REG_FIFO_EN, 0U, ICM20689_FIFO_EN_MASK_ACCEL_FIFO_EN
                           | ICM20689_FIFO_EN_MASK_ZG_FIFO_EN
                           | ICM20689_FIFO_EN_MASK_YG_FIFO_EN
                           | ICM20689_FIFO_EN_MASK_XG_FIFO_EN);

  /* Read FIFO sample count */
  sl_icm20689_read_register(ICM20689_REG_FIFO_COUNTH, 2, &data[0]);

  /* Convert to a 16 bit value */
  fifo_count = ( (uint16_t) (data[0] << 8) | data[1]);

  /* Calculate the number of data sets (3 axis of accel an gyro, two bytes each = 12 bytes) */
  packet_count = fifo_count / 12;

  /* Retrieve the data from the FIFO */
  for ( i = 0; i < packet_count; i++ ) {
    sl_icm20689_read_register(ICM20689_REG_FIFO_R_W, 12, &data[0]);
    /* Convert to 16 bit signed accel and gyro x,y and z values */
    gyro_temp[0] = ( (int16_t) (data[6] << 8) | data[7]);
    gyro_temp[1] = ( (int16_t) (data[8] << 8) | data[9]);
    gyro_temp[2] = ( (int16_t) (data[10] << 8) | data[11]);

    /* Sum the values */
    gyro_bias[0] += gyro_temp[0];
    gyro_bias[1] += gyro_temp[1];
    gyro_bias[2] += gyro_temp[2];
  }

  /* Divide by packet count to get the average */
  gyro_bias[0] /= packet_count;
  gyro_bias[1] /= packet_count;
  gyro_bias[2] /= packet_count;

  /* Convert the values to degrees per sec for displaying */
  gyro_bias_scaled[0] = (float) gyro_bias[0] * gyro_res;
  gyro_bias_scaled[1] = (float) gyro_bias[1] * gyro_res;
  gyro_bias_scaled[2] = (float) gyro_bias[2] * gyro_res;

  /* Read stored gyro trim values. After reset these values are all 0 */
  sl_icm20689_read_register(ICM20689_REG_XG_OFFS_USRH, 2, &data[0]);
  gyro_bias_stored[0] = ( (int16_t) (data[0] << 8) | data[1]);

  sl_icm20689_read_register(ICM20689_REG_YG_OFFS_USRH, 2, &data[0]);
  gyro_bias_stored[1] = ( (int16_t) (data[0] << 8) | data[1]);

  sl_icm20689_read_register(ICM20689_REG_ZG_OFFS_USRH, 2, &data[0]);
  gyro_bias_stored[2] = ( (int16_t) (data[0] << 8) | data[1]);

  /* The gyro bias should be stored in 1000dps full scaled format. We measured in 250dps to get */
  /* the best sensitivity, so need to divide by 4 */
  /* Substract from the stored calibration value */
  gyro_bias_stored[0] -= gyro_bias[0] / 4;
  gyro_bias_stored[1] -= gyro_bias[1] / 4;
  gyro_bias_stored[2] -= gyro_bias[2] / 4;

  /* Split the values into two bytes */
  data[0] = (gyro_bias_stored[0] >> 8) & 0xFF;
  data[1] = (gyro_bias_stored[0]) & 0xFF;
  data[2] = (gyro_bias_stored[1] >> 8) & 0xFF;
  data[3] = (gyro_bias_stored[1]) & 0xFF;
  data[4] = (gyro_bias_stored[2] >> 8) & 0xFF;
  data[5] = (gyro_bias_stored[2]) & 0xFF;

  /* Write the  gyro bias values to the chip */
  sl_icm20689_write_register(ICM20689_REG_XG_OFFS_USRH, data[0]);
  sl_icm20689_write_register(ICM20689_REG_XG_OFFS_USRL, data[1]);
  sl_icm20689_write_register(ICM20689_REG_YG_OFFS_USRH, data[2]);
  sl_icm20689_write_register(ICM20689_REG_YG_OFFS_USRL, data[3]);
  sl_icm20689_write_register(ICM20689_REG_ZG_OFFS_USRH, data[4]);
  sl_icm20689_write_register(ICM20689_REG_ZG_OFFS_USRL, data[5]);

  /* Turn off FIFO */
  sl_icm20689_masked_write(ICM20689_REG_USER_CTRL, 0x00, ICM20689_USER_CTRL_MASK_FIFO_EN);

  /* Disable all sensors */
  sl_icm20689_enable_sensor(false, false, false);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Reads the temperature sensor raw value and converts to Celsius.
 ******************************************************************************/
sl_status_t sl_icm20689_read_temperature_data(float *temperature)
{
  uint8_t data[2];
  int16_t raw_temp;

  /* Read temperature registers */
  sl_icm20689_read_register(ICM20689_REG_TEMP_OUT_H, 2, data);

  /* Convert to int16 */
  raw_temp = (int16_t) ( (data[0] << 8) + data[1]);

  /* Calculate the Celsius value from the raw reading */
  *temperature = ( (float) raw_temp / 333.87) + 21.0;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *    Reads the device ID of the ICM20689
 ******************************************************************************/
sl_status_t sl_icm20689_get_device_id(uint8_t *dev_id)
{
  sl_icm20689_read_register(ICM20689_REG_WHO_AM_I, 1, dev_id);

  return SL_STATUS_OK;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Write data in a register while keeping the non masked bits.
 *
 * @param[in] addr
 *    The register address to write
 *    Bit[7:0] - register address
 *
 * @param[in] data
 *    The data to write to the register
 *
 * @param[in] mask
 *    The mask where data will be written
 *    Bit[7:0] - mask
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
static sl_status_t sl_icm20689_masked_write(uint8_t addr, uint8_t data, uint8_t mask)
{
  uint8_t reg;
  sl_status_t status;

  status = sl_icm20689_read_register(addr, 1, &reg);

  if (status != SL_STATUS_OK) {
    return status;
  }

  // Clear masked bit(s)
  reg &= ~(mask);

  // Write data in the masked space
  reg |= data;

  status = sl_icm20689_write_register(addr, reg);

  return status;
}

/***************************************************************************//**
 * @brief
 *    Controls the state of the ICM20689 SPI chip select pin
 *
 * @param[in] select
 *    The desired state of the select pin. True: asserted (logic L)
 *
 * @return
 *    None
 ******************************************************************************/
static void sl_icm20689_chip_select_set(bool select)
{
  if ( select ) {
    GPIO_PinOutClear(SL_ICM20689_SPI_EUSART_CS_PORT, SL_ICM20689_SPI_EUSART_CS_PIN);
  } else {
    GPIO_PinOutSet(SL_ICM20689_SPI_EUSART_CS_PORT, SL_ICM20689_SPI_EUSART_CS_PIN);
  }
}

/** @endcond */
