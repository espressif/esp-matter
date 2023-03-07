/***************************************************************************//**
 * @file
 * @brief Driver for the Si72xx Hall Effect Sensor
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

#ifndef SL_SI72XX_H
#define SL_SI72XX_H

#include "em_device.h"
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup si72xx Si72xx - Magnetic Hall Effect Sensor
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** I2C device address for Si72xx */
#define SI7200_ADDR_0      (0x30 << 1)
#define SI7200_ADDR_1      (0x31 << 1)
#define SI7200_ADDR_2      (0x32 << 1)
#define SI7200_ADDR_3      (0x33 << 1)

/** I2C registers for Si72xx */
#define SI72XX_HREVID             0xC0
#define SI72XX_DSPSIGM            0xC1
#define SI72XX_DSPSIGL            0xC2
#define SI72XX_DSPSIGSEL          0xC3
#define SI72XX_POWER_CTRL         0xC4
#define SI72XX_ARAUTOINC          0xC5
#define SI72XX_CTRL1              0xC6
#define SI72XX_CTRL2              0xC7
#define SI72XX_SLTIME             0xC8
#define SI72XX_CTRL3              0xC9
#define SI72XX_A0                 0xCA
#define SI72XX_A1                 0xCB
#define SI72XX_A2                 0xCC
#define SI72XX_CTRL4              0xCD
#define SI72XX_A3                 0xCE
#define SI72XX_A4                 0xCF
#define SI72XX_A5                 0xD0
#define SI72XX_OTP_ADDR           0xE1
#define SI72XX_OTP_DATA           0xE2
#define SI72XX_OTP_CTRL           0xE3
#define SI72XX_TM_FG              0xE4

#define SI72XX_OTP_20MT_ADDR      0x21
#define SI72XX_OTP_200MT_ADDR     0x27

#define SI72XX_ERROR_BUSY         0xfe
#define SI72XX_ERROR_NODATA       0xfd

/** @endcond */

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
/** Si72xx magnetic field full-scales */
typedef enum {
  SI7210_20MT,
  SI7210_200MT
} Si72xxFieldScale_t;

/** Si72xx sleep modes */
typedef enum {
  SI72XX_SLEEP_MODE,
  SI72XX_SLTIMEENA_MODE
} Si72xxSleepMode_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***********************************************************************//**
 * @brief
 *  Reads register from the Si72xx sensor.
 *  Command can only be issued if Si72xx in idle mode.
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @param[in] reg
 *   The register address.
 *
 * @param[out] data
 *   The data read from the sensor.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t sl_si72xx_read_register(I2C_TypeDef *i2c,
                                 uint8_t addr,
                                 uint8_t reg,
                                 uint8_t *data);

/***********************************************************************//**
 * @brief
 *   Writes register in the Si72xx sensor.
 *   Command can only be issued if Si72xx in idle mode.
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @param[in] reg
 *   The register address.
 *
 * @param[in] data
 *   The data to write to the sensor.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t sl_si72xx_write_register(I2C_TypeDef *i2c,
                                  uint8_t addr,
                                  uint8_t reg,
                                  uint8_t data);

/***********************************************************************//**
 * @brief
 *   Read out Si72xx Magnetic Field Conversion Data
 *   Command can only be issued if Si72xx in idle mode.
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @param[out] magData
 *   Mag-field conversion reading, signed 16-bit integer.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t sl_si72xx_read_magfield_data(I2C_TypeDef *i2c,
                                      uint8_t addr,
                                      int16_t *magData);

/***********************************************************************//**
 * @brief
 *   Puts Si72xx into Sleep mode (lowest power).
 *   Command can only be issued if Si72xx in idle mode.
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t sl_si72xx_from_idle_go_to_sleep(I2C_TypeDef *i2c, uint8_t addr);

/***********************************************************************//**
 * @brief
 *   Puts Si72xx into Sleep-Timer-Enabled mode.
 *   Si72xx periodically wakes-up, samples the magnetic field, updates the
 *   output, and goes back to sleep-timer-enabled mode.
 *   Command can only be issued if Si72xx in idle mode.
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t sl_si72xx_from_idle_go_to_sltimeena(I2C_TypeDef *i2c, uint8_t addr);

/***********************************************************************//**
 * @brief
 *   Wake-up Si72xx and places sensor in idle-mode.
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t sl_si72xx_wake_up_and_idle(I2C_TypeDef *i2c, uint8_t addr);

/***********************************************************************//**
 * @brief
 *   Wake-up SI72xx, performs a magnetic-field conversion with FIR,
 *   and places Si72xx back to sleep-mode.
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @param[in] mTScale
 *   mTScale= Si7210_20MT: 20mT full-scale magnetic-field range.
 *   mTScale= Si7210_200MT: 200mT full-scale magnetic-field range.
 *
 * @param[in] sleepMode
 *   SI72XX_SLEEP: Sleep mode. Lowest power & doesn't update output.
 *   SI72XX_SLTIMEENA: Sleep-Timer-Enabled mode. Updates output periodically.
 *
 * @param[out] magFieldData
 *   Magnetic-field conversion reading, signed 16-bit integer.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t sl_si72xx_read_magfield_data_and_sleep(I2C_TypeDef *i2c,
                                                uint8_t addr,
                                                Si72xxFieldScale_t mTScale,
                                                Si72xxSleepMode_t sleepMode,
                                                int16_t *magFieldData);

/**************************************************************************//**
 * @brief
 *   Convert Si7210 I2C Data Readings to Magnetic Field in microTeslas.
 *
 * @param[in] fieldScale
 *   20mT or 200mT full-scale magnetic field range.
 *
 * @param[in] dataCode
 *   signed 15-bit value read from hall sensor after magnetic field conversion.
 *
 * @return
 *   microTeslas.
 *****************************************************************************/
int32_t sl_si72xx_convert_data_codes_to_magnetic_field(Si72xxFieldScale_t fieldScale,
                                                       int16_t dataCode);

/***********************************************************************//**
 * @brief
 *   Wake-up Si72xx, and set sleep-mode option.
 *   If Si72xx is in a sleep-mode, it requires a wake-up command first.
 *   Useful for placing Si72xx in SLTIMEENA mode from SLEEP mode,
 *   or vice-versa.
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @param[in] sleepMode
 *   SI72XX_SLEEP: Puts Si72xx into sleep mode. Lowest power & doesn't update.
 *   SI72XX_SLTIMEENA: Si72xx into sltimeena mode. Updates output periodically.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t sl_si72xx_enter_sleep_mode(I2C_TypeDef *i2c,
                                    uint8_t addr,
                                    Si72xxSleepMode_t sleepMode);

/***********************************************************************//**
 * @brief
 *   Wake-up Si72xx, and configures output for Latch mode.
 *   Switch point = 0mT w/ 0.2mT hysteresis
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t sl_si72xx_enter_latch_mode (I2C_TypeDef *i2c, uint8_t addr);

/***********************************************************************//**
 * @brief
 *   Wakes up SI72xx, performs temperature conversion and places Si72xx
 *   into SI72XX_SLEEP sleep-mode.
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @param[out] rawTemp
 *   Temperature measurement in millidegree Celsius.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t sl_si72xx_read_temperature_and_sleep(I2C_TypeDef *i2c,
                                              uint8_t addr,
                                              int32_t *rawTemp);

/***********************************************************************//**
 * @brief
 *   Wakes up SI72xx, performs temperature conversion and places Si72xx
 *   into SI72XX_SLEEP sleep-mode.
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @param[out] offsetValue
 *   Temperature offset correction.
 *
 * @param[out] gainValue
 *   Temperature gain correction.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t sl_si72xx_read_temp_correction_data_and_sleep(I2C_TypeDef *i2c,
                                                       uint8_t addr,
                                                       int16_t *offsetValue,
                                                       int16_t *gainValue);

/**************************************************************************//**
 * @brief
 *   Wakes up SI72xx, performs a temperature conversion, and places sensor
 *   back to sleep. Temperature calculation is performed using compensation
 *   data.
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @param[out] correctedTemp
 *   Temperature measurement in millidegree Celsius.
 *
 * @param[in] offsetData
 *   Offset correction data.
 *
 * @param[in] gainData
 *   Gain correction data.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 *****************************************************************************/
uint32_t sl_si72xx_read_corrected_temp_and_sleep(I2C_TypeDef *i2c,
                                                 uint8_t addr,
                                                 int16_t offsetData,
                                                 int16_t gainData,
                                                 int32_t *correctedTemp);

/**************************************************************************
 * @brief
 *   Wake-up Si72xx, read out part Revision and ID, and place Si72xx
 *   back to SLEEP sleep-mode.
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @param[out] partId
 *   Si72xx part ID.
 *
 * @param[out] partRev
 *   Si72xx part Revision.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t sl_si72xx_identify_and_sleep(I2C_TypeDef *i2c,
                                      uint8_t addr,
                                      uint8_t *partId,
                                      uint8_t *partRev);

/**************************************************************************
 * @brief
 *   Wake-up Si72xx, read out Si72xx base part-number and variant, and
 *   place sensor back to SLEEP sleep-mode.
 *
 * @param[in] i2c
 *   Pointer to the I2C peripheral register block.
 *
 * @param[in] addr
 *   The I2C address of the sensor.
 *
 * @param[out] basePn
 *   Si72xx part ID.
 *
 * @param[out] pnVariant
 *   Si72xx part Revision.
 *
 * @return
 *   Returns zero on success. Otherwise returns error codes
 *   based on the I2CSPM.
 **************************************************************************/
uint32_t sl_si72xx_read_variant_and_sleep(I2C_TypeDef *i2c,
                                          uint8_t addr,
                                          uint8_t *basePn,
                                          uint8_t *pnVariant);

/** @} (end group si72xx) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI72xx_H */
