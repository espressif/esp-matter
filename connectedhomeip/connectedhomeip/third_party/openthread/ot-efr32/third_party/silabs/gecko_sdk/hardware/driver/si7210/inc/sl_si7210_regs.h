/***************************************************************************//**
 * @file
 * @brief Driver for the Silicon Labs Si7210 Hall Effect Sensor
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

#ifndef SL_SI7210_REGS_H
#define SL_SI7210_REGS_H

/***************************************************************************//**
 * @addtogroup si7210
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @addtogroup si7210_details
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @name Registers
 * @{
 ******************************************************************************/
#define SI7210_REG_ADDR_HREVID               0xC0  /**< Hardware revision ID register                                */
#define SI7210_REG_ADDR_DSPSIGM              0xC1  /**< The most significant byte of the last conversion result      */
#define SI7210_REG_ADDR_DSPSIGL              0xC2  /**< The least significant byte of the last conversion result     */
#define SI7210_REG_ADDR_DSPSIGSEL            0xC3  /**< Select the data after filtering                              */
#define SI7210_REG_ADDR_POWER_CTRL           0xC4  /**< Power control register                                       */
#define SI7210_REG_ADDR_ARAUTOINC            0xC5  /**< Enables auto increment of the I2C register address pointer   */
#define SI7210_REG_ADDR_CTRL1                0xC6  /**< Control register 1, output pin configuration                 */
#define SI7210_REG_ADDR_CTRL2                0xC7  /**< Control register 2, output pin configuration                 */
#define SI7210_REG_ADDR_SLTIME               0xC8  /**< Sleep time control register                                  */
#define SI7210_REG_ADDR_CTRL3                0xC9  /**< Control register 3, output pin and sleep time configuration  */
#define SI7210_REG_ADDR_A0                   0xCA  /**< Coefficient A0                                               */
#define SI7210_REG_ADDR_A1                   0xCB  /**< Coefficient A1                                               */
#define SI7210_REG_ADDR_A2                   0xCC  /**< Coefficient A2                                               */
#define SI7210_REG_ADDR_CTRL4                0xCD  /**< Control register 4, Digital filter configuration             */
#define SI7210_REG_ADDR_A3                   0xCE  /**< Coefficient A3                                               */
#define SI7210_REG_ADDR_A4                   0xCF  /**< Coefficient A4                                               */
#define SI7210_REG_ADDR_A5                   0xD0  /**< Coefficient A5                                               */
#define SI7210_REG_ADDR_OTP_ADDR             0xE1  /**< OTP address of the data to read                              */
#define SI7210_REG_ADDR_OTP_DATA             0xE2  /**< Data read from OTP                                           */
#define SI7210_REG_ADDR_OTP_CTRL             0xE3  /**< OTP read control register                                    */
#define SI7210_REG_ADDR_TM_FG                0xE4  /**< On-chip test coil control                                    */

#define SI7210_REG_HREVID_REVID_MASK         0x0F  /**< Revision ID mask, Hardware revision ID register                             */
#define SI7210_REG_HREVID_REVID_SHIFT        0     /**< Revision ID shift value, Hardware revision ID register                      */
#define SI7210_REG_HREVID_CHIPID_MASK        0xF0  /**< Chip ID mask, Hardware revision ID register                                 */
#define SI7210_REG_HREVID_CHIPID_SHIFT       4     /**< Revision ID shift value, Hardware revision ID register                      */

#define SI7210_REG_DSPSIGM_FRESH_MASK        0x80  /**< New data available mask, MSB of last conversion result register             */
#define SI7210_REG_DSPSIGM_FRESH_SHIFT       7     /**< New data available shift value, MSB of last conversion result register      */
#define SI7210_REG_DSPSIGM_DSPSIGM_MASK      0x7F  /**< MSB bits of conversion mask, MSB of last conversion result register         */
#define SI7210_REG_DSPSIGM_DSPSIGM_SHIFT     0     /**< MSB bits of conversion shift value, MSB of last conversion result register  */

#define SI7210_REG_POWER_CTRL_SLEEP_MASK     0x01  /**< Sleep mode bit mask, Power control register                                 */
#define SI7210_REG_POWER_CTRL_SLEEP_SHIFT    0     /**< Sleep mode bit shift value, Power control register                          */
#define SI7210_REG_POWER_CTRL_STOP_MASK      0x02  /**< Stop bit mask, Power control register                                       */
#define SI7210_REG_POWER_CTRL_STOP_SHIFT     1     /**< Stop bit shift value, Power control register                                */
#define SI7210_REG_POWER_CTRL_ONEBURST_MASK  0x04  /**< One burst mesurement bit mask, Power control register                       */
#define SI7210_REG_POWER_CTRL_ONEBURST_SHIFT 2     /**< One burst mesurement bit shift value, Power control register                */
#define SI7210_REG_POWER_CTRL_USESTORE_MASK  0x08  /**< Save the current state of OTP regs bit mask, Power control register         */
#define SI7210_REG_POWER_CTRL_USESTORE_SHIFT 3     /**< Save the current state of OTP regs bit shift value, Power control register  */
#define SI7210_REG_POWER_CTRL_MEAS_MASK      0x80  /**< Measurement in progress bit mask, Power control register                    */
#define SI7210_REG_POWER_CTRL_MEAS_SHIFT     7     /**< Measurement in progress bit shift value, Power control register             */

#define SI7210_REG_CTRL1_SW_OP_MASK          0x7F  /**< Switching point mask, Control register 1                                    */
#define SI7210_REG_CTRL1_SW_OP_SHIFT         0     /**< Switching point shift value, Control register 1                             */
#define SI7210_REG_CTRL1_SW_LOW4FIELD_MASK   0x80  /**< Output polarity setting bit mask, Control register 1                        */
#define SI7210_REG_CTRL1_SW_LOW4FIELD_SHIFT  7     /**< Output polarity setting bit shift value, Control register 1                 */

#define SI7210_REG_CTRL2_SW_HYST_MASK        0x3F  /**< Switch hysteresis mask, Control register 2                                  */
#define SI7210_REG_CTRL2_SW_HYST_SHIFT       0     /**< Switch hyteresis shift value, Control register 2                            */
#define SI7210_REG_CTRL2_SW_FIELDPOLSEL_MASK 0xC0  /**< Switch polarity mask, Control register 2                                    */
#define SI7210_REG_CTRL2_SW_FIELDPOLSEL_SHIFT 6    /**< Switchi polarity shift value, Control register 2                            */

#define SI7210_REG_CTRL3_SLTIMEENA_MASK      0x01  /**< Sleep timer enable bit mask, Control register 3                             */
#define SI7210_REG_CTRL3_SLTIMEENA_SHIFT     0     /**< Sleep timer enable bit shift value, Control register 3                      */
#define SI7210_REG_CTRL3_SLFAST_MASK         0x02  /**< Sleep time reduction enable bit mask, Control register 3                    */
#define SI7210_REG_CTRL3_SLFAST_SHIFT        1     /**< Sleep time reduction bit shift value, Control register 3                    */
#define SI7210_REG_CTRL3_SW_TAMPER_MASK      0xFC  /**< Tamper switch threshold mask, Control register 3                            */
#define SI7210_REG_CTRL3_SW_TAMPER_SHIFT     2     /**< Tamper switch threshold shift value, Control register 3                     */

#define SI7210_REG_CTRL4_IIR_MASK            0x01  /**< IIR filter enable bit mask, Control register 4                              */
#define SI7210_REG_CTRL4_IIR_SHIFT           0     /**< IIR filter enable bit shift value, Control register 4                       */
#define SI7210_REG_CTRL4_DF_BW_MASK          0x1E  /**< Number of samples to average mask, Control register 4                       */
#define SI7210_REG_CTRL4_DF_BW_SHIFT         1     /**< Number of samples to average shift value, Control register 4                */
#define SI7210_REG_CTRL4_DF_BURSTSIZE_MASK   0x0E  /**< Measurement burst size mask, Control register 4                             */
#define SI7210_REG_CTRL4_DF_BURSTSIZE_SHIFT  5     /**< Measurement burst size shift value, Control register 4                      */

#define SI7210_REG_OTP_CTRL_BUSY_MASK        0x01  /**< OTP busy indicator bit mask, OTP read control register                      */
#define SI7210_REG_OTP_CTRL_BUSY_SHIFT       0     /**< OTP busy indicator bit shift value, OTP read control register               */
#define SI7210_REG_OTP_CTRL_READ_EN_MASK     0x02  /**< OTP read enable bit mask, OTP read control register                         */
#define SI7210_REG_OTP_CTRL_READ_EN_SHIFT    1     /**< OTP read enable bit shift value, OTP read control register                  */

#define SI7210_OTP_ADDR_CTRL1                0x04  /**< OTP Control register 1, output pin configuration                   */
#define SI7210_OTP_ADDR_CTRL2                0x05  /**< OTP Control register 2, output pin configuration                   */
#define SI7210_OTP_ADDR_SLTIME               0x06  /**< OTP Sleep time control register                                    */
#define SI7210_OTP_ADDR_CTRL3                0x08  /**< OTP Control register 3, output pin and sleep time configuration    */
#define SI7210_OTP_ADDR_POWER_UP_A0          0x09  /**< OTP Coefficient A0                                                 */
#define SI7210_OTP_ADDR_POWER_UP_A1          0x0A  /**< OTP Coefficient A1                                                 */
#define SI7210_OTP_ADDR_POWER_UP_A2          0x0B  /**< OTP Coefficient A2                                                 */
#define SI7210_OTP_ADDR_CTRL4                0x0C  /**< OTP Control register 4, Digital filter configuration               */
#define SI7210_OTP_ADDR_POWER_UP_A3          0x0D  /**< OTP Coefficient A3                                                 */
#define SI7210_OTP_ADDR_POWER_UP_A4          0x0E  /**< OTP Coefficient A4                                                 */
#define SI7210_OTP_ADDR_POWER_UP_A5          0x0F  /**< OTP Coefficient A5                                                 */
#define SI7210_OTP_ADDR_BASE_PART_NUMBER     0x14  /**< OTP Base part number                                               */
#define SI7210_OTP_ADDR_VARIANT              0x15  /**< OTP Variant number                                                 */
#define SI7210_OTP_ADDR_SERIAL_NUMBER        0x18  /**< OTP Serial number                                                  */
#define SI7210_OTP_ADDR_BPERVCAL             0x20  /**< OTP On-chip field generator calibration value                      */

#define SI7210_OTP_ADDR_COEFFS_20MT              0x21 /**< OTP 20mT scale no magnet temperature compensation value         */
#define SI7210_OTP_ADDR_COEFFS_200MT             0x27 /**< OTP 200mT scale no magnet temperature compensation value        */
#define SI7210_OTP_ADDR_COEFFS_20MT_NEODYMIUM    0x2D /**< OTP 20mT scale neodymium magnet temperature compensation value  */
#define SI7210_OTP_ADDR_COEFFS_200MT_NEODYMIUM   0x33 /**< OTP 200mT scale neodymium temperature compensation value        */
#define SI7210_OTP_ADDR_COEFFS_20MT_CERAMIC      0x39 /**< OTP 20mT scale ceramic magnet temperature compensation value    */
#define SI7210_OTP_ADDR_COEFFS_200MT_CERAMIC     0x3F /**< OTP 200mT scale ceramic magnet temperature compensation value   */
/** @} */
/** @} end addtogroup Si7210_low_level */
/** @} end addtogroup Si7210 */

#endif // SL_SI7210_REGS_H
