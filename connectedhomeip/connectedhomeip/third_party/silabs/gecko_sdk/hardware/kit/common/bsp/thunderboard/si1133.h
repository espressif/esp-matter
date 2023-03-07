/***************************************************************************//**
 * @file
 * @brief Driver for the Si1133 Ambient Light and UV sensor
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

#ifndef SI1133_H
#define SI1133_H

#include "si1133_config.h"

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @addtogroup Si1133
 * @{
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define X_ORDER_MASK            0x0070
#define Y_ORDER_MASK            0x0007
#define SIGN_MASK               0x0080
#define get_x_order(m)          ( (m & X_ORDER_MASK) >> 4)
#define get_y_order(m)          ( (m & Y_ORDER_MASK)      )
#define get_sign(m)             ( (m & SIGN_MASK) >> 7)

#define UV_INPUT_FRACTION       15
#define UV_OUTPUT_FRACTION      12
#define UV_NUMCOEFF             2

#define ADC_THRESHOLD           16000
#define INPUT_FRACTION_HIGH     7
#define INPUT_FRACTION_LOW      15
#define LUX_OUTPUT_FRACTION     12
#define NUMCOEFF_LOW            9
#define NUMCOEFF_HIGH           4

/** @endcond */

/**************************************************************************//**
* @name Error Codes
* @{
******************************************************************************/
#define SI1133_OK                            0x0000   /**< No errors                  */
#define SI1133_ERROR_I2C_TRANSACTION_FAILED  0x0001   /**< I2C transaction failed     */
#define SI1133_ERROR_SLEEP_FAILED            0x0002   /**< Entering sleep mode failed */
/**@}*/

/***************************************************************************//**
 * @brief
 *    Structure to store the data measured by the Si1133
 ******************************************************************************/
typedef struct {
  uint8_t     irq_status;     /**< Interrupt status of the device    */
  int32_t     ch0;            /**< Channel 0 measurement data        */
  int32_t     ch1;            /**< Channel 1 measurement data        */
  int32_t     ch2;            /**< Channel 2 measurement data        */
  int32_t     ch3;            /**< Channel 3 measurement data        */
} SI1133_Samples_TypeDef;

/***************************************************************************//**
 * @brief
 *    Structure to store the calculation coefficients
 ******************************************************************************/
typedef struct {
  int16_t     info;           /**< Info                              */
  uint16_t    mag;            /**< Magnitude                         */
} SI1133_Coeff_TypeDef;

/***************************************************************************//**
 * @brief
 *    Structure to store the coefficients used for Lux calculation
 ******************************************************************************/
typedef struct {
  SI1133_Coeff_TypeDef   coeff_high[4];   /**< High amplitude coeffs */
  SI1133_Coeff_TypeDef   coeff_low[9];    /**< Low amplitude coeffs  */
} SI1133_LuxCoeff_TypeDef;

/**************************************************************************//**
* @name Registers
* @{
******************************************************************************/
#define SI1133_REG_PART_ID          0x00  /**< Part ID                                                               */
#define SI1133_REG_HW_ID            0x01  /**< Hardware ID                                                           */
#define SI1133_REG_REV_ID           0x02  /**< Hardware revision                                                     */
#define SI1133_REG_HOSTIN0          0x0A  /**< Data for parameter table on PARAM_SET write to COMMAND register       */
#define SI1133_REG_COMMAND          0x0B  /**< Initiated action in Sensor when specific codes written here           */
#define SI1133_REG_IRQ_ENABLE       0x0F  /**< Interrupt enable                                                      */
#define SI1133_REG_RESPONSE1        0x10  /**< Contains the readback value from a param query or a param set command */
#define SI1133_REG_RESPONSE0        0x11  /**< Chip state and error status                                           */
#define SI1133_REG_IRQ_STATUS       0x12  /**< Interrupt status                                                      */
#define SI1133_REG_HOSTOUT0         0x13  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT1         0x14  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT2         0x15  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT3         0x16  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT4         0x17  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT5         0x18  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT6         0x19  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT7         0x1A  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT8         0x1B  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT9         0x1C  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT10        0x1D  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT11        0x1E  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT12        0x1F  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT13        0x20  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT14        0x21  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT15        0x22  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT16        0x23  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT17        0x24  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT18        0x25  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT19        0x26  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT20        0x27  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT21        0x28  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT22        0x29  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT23        0x2A  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT24        0x2B  /**< Captured Sensor Data                                                  */
#define SI1133_REG_HOSTOUT25        0x2C  /**< Captured Sensor Data                                                  */
/**@}*/

/**************************************************************************//**
* @name Parameters
* @{
******************************************************************************/
#define SI1133_PARAM_I2C_ADDR       0x00  /**< I2C address                                                  */
#define SI1133_PARAM_CH_LIST        0x01  /**< Channel list                                                 */
#define SI1133_PARAM_ADCCONFIG0     0x02  /**< ADC config for Channel 0                                     */
#define SI1133_PARAM_ADCSENS0       0x03  /**< ADC sensitivity setting for Channel 0                        */
#define SI1133_PARAM_ADCPOST0       0x04  /**< ADC resolution, shift and threshold settings for Channel 0   */
#define SI1133_PARAM_MEASCONFIG0    0x05  /**< ADC measurement counter selection for Channel 0              */
#define SI1133_PARAM_ADCCONFIG1     0x06  /**< ADC config for Channel 1                                     */
#define SI1133_PARAM_ADCSENS1       0x07  /**< ADC sensitivity setting for Channel 1                        */
#define SI1133_PARAM_ADCPOST1       0x08  /**< ADC resolution, shift and threshold settings for Channel 1   */
#define SI1133_PARAM_MEASCONFIG1    0x09  /**< ADC measurement counter selection for Channel 1              */
#define SI1133_PARAM_ADCCONFIG2     0x0A  /**< ADC config for Channel 2                                     */
#define SI1133_PARAM_ADCSENS2       0x0B  /**< ADC sensitivity setting for Channel 2                        */
#define SI1133_PARAM_ADCPOST2       0x0C  /**< ADC resolution, shift and threshold settings for Channel 2   */
#define SI1133_PARAM_MEASCONFIG2    0x0D  /**< ADC measurement counter selection for Channel 2              */
#define SI1133_PARAM_ADCCONFIG3     0x0E  /**< ADC config for Channel 3                                     */
#define SI1133_PARAM_ADCSENS3       0x0F  /**< ADC sensitivity setting for Channel 3                        */
#define SI1133_PARAM_ADCPOST3       0x10  /**< ADC resolution, shift and threshold settings for Channel 3   */
#define SI1133_PARAM_MEASCONFIG3    0x11  /**< ADC measurement counter selection for Channel 3              */
#define SI1133_PARAM_ADCCONFIG4     0x12  /**< ADC config for Channel 4                                     */
#define SI1133_PARAM_ADCSENS4       0x13  /**< ADC sensitivity setting for Channel 4                        */
#define SI1133_PARAM_ADCPOST4       0x14  /**< ADC resolution, shift and threshold settings for Channel 4   */
#define SI1133_PARAM_MEASCONFIG4    0x15  /**< ADC measurement counter selection for Channel 4              */
#define SI1133_PARAM_ADCCONFIG5     0x16  /**< ADC config for Channel 5                                     */
#define SI1133_PARAM_ADCSENS5       0x17  /**< ADC sensitivity setting for Channel 5                        */
#define SI1133_PARAM_ADCPOST5       0x18  /**< ADC resolution, shift and threshold settings for Channel 5   */
#define SI1133_PARAM_MEASCONFIG5    0x19  /**< ADC measurement counter selection for Channel 5              */
#define SI1133_PARAM_MEASRATE_H     0x1A  /**< Main measurement rate counter MSB                            */
#define SI1133_PARAM_MEASRATE_L     0x1B  /**< Main measurement rate counter LSB                            */
#define SI1133_PARAM_MEASCOUNT0     0x1C  /**< Measurement rate extension counter 0                         */
#define SI1133_PARAM_MEASCOUNT1     0x1D  /**< Measurement rate extension counter 1                         */
#define SI1133_PARAM_MEASCOUNT2     0x1E  /**< Measurement rate extension counter 2                         */
#define SI1133_PARAM_THRESHOLD0_H   0x25  /**< Threshold level 0 MSB                                        */
#define SI1133_PARAM_THRESHOLD0_L   0x26  /**< Threshold level 0 LSB                                        */
#define SI1133_PARAM_THRESHOLD1_H   0x27  /**< Threshold level 1 MSB                                        */
#define SI1133_PARAM_THRESHOLD1_L   0x28  /**< Threshold level 1 LSB                                        */
#define SI1133_PARAM_THRESHOLD2_H   0x29  /**< Threshold level 2 MSB                                        */
#define SI1133_PARAM_THRESHOLD2_L   0x2A  /**< Threshold level 2 LSB                                        */
#define SI1133_PARAM_BURST          0x2B  /**< Burst enable and burst count                                 */
/**@}*/

/**************************************************************************//**
* @name Commands
* @{
******************************************************************************/
#define SI1133_CMD_RESET_CMD_CTR    0x00  /**< Resets the command counter                                         */
#define SI1133_CMD_RESET            0x01  /**< Forces a Reset                                                     */
#define SI1133_CMD_NEW_ADDR         0x02  /**< Stores the new I2C address                                         */
#define SI1133_CMD_FORCE_CH         0x11  /**< Initiates a set of measurements specified in CHAN_LIST parameter   */
#define SI1133_CMD_PAUSE_CH         0x12  /**< Pauses autonomous measurements                                     */
#define SI1133_CMD_START            0x13  /**< Starts autonomous measurements                                     */
#define SI1133_CMD_PARAM_SET        0x80  /**< Sets a parameter                                                   */
#define SI1133_CMD_PARAM_QUERY      0x40  /**< Reads a parameter                                                  */
/**@}*/

/**************************************************************************//**
* @name Responses
* @{
******************************************************************************/
#define SI1133_RSP0_CHIPSTAT_MASK   0xE0  /**< Chip state mask in Response0 register                           */
#define SI1133_RSP0_COUNTER_MASK    0x1F  /**< Command counter and error indicator mask in Response0 register  */
#define SI1133_RSP0_SLEEP           0x20  /**< Sleep state indicator bit mask in Response0 register            */
/**@}*/

uint32_t  SI1133_registerRead       (uint8_t reg, uint8_t *data);
uint32_t  SI1133_registerWrite      (uint8_t reg, uint8_t  data);
uint32_t  SI1133_registerBlockRead  (uint8_t reg, uint8_t length, uint8_t *data);
uint32_t  SI1133_registerBlockWrite (uint8_t reg, uint8_t length, uint8_t *data);
uint32_t  SI1133_reset              (void);
uint32_t  SI1133_resetCmdCtr        (void);
uint32_t  SI1133_measurementForce   (void);
uint32_t  SI1133_measurementPause   (void);
uint32_t  SI1133_measurementStart   (void);
uint32_t  SI1133_paramSet           (uint8_t address, uint8_t value);
uint32_t  SI1133_paramRead          (uint8_t address);
uint32_t  SI1133_init               (void);
uint32_t  SI1133_deInit             (void);
uint32_t  SI1133_measurementGet     (SI1133_Samples_TypeDef *samples);
int32_t   SI1133_getUv              (int32_t uv, SI1133_Coeff_TypeDef *uk);
int32_t   SI1133_getLux             (int32_t vis_high, int32_t vis_low, int32_t ir, SI1133_LuxCoeff_TypeDef *lk);
uint32_t  SI1133_measureLuxUvi      (float *lux, float *uvi);
uint32_t  SI1133_getHardwareID      (uint8_t *hardwareID);
uint32_t  SI1133_getMeasurement     (float *lux, float *uvi);
uint32_t  SI1133_getIrqStatus       (uint8_t *irqStatus);

/** @} */
/** @} */

#endif // SI1133_H
