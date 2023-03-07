/***************************************************************************//**
 * @file
 * @brief Driver for the Si1133 Ambient Light and UV sensor
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

#ifndef SL_SI1133_H
#define SL_SI1133_H

#include "sl_i2cspm.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup si1133 Si1133 - Light and UV Sensor
 * @brief   Driver for the Silicon Labs Si1133 ambient light and UV sensor

 @n @section si1133_example Si1133 usage example code

   Basic example for measuring light and UV-index: @n @n
   @verbatim

 #include "sl_i2cspm_instances.h"
 #include "sl_si1133.h"

 int main( void )
 {

   ...

   // Initialize sensor using i2cspm instance for sensor
   sl_si1133_init(sl_i2cspm_sensor);

   // Measure
   float lux;
   float uvi;

   sl_si1133_measure_lux_uvi(sl_i2cspm_sensor, &lux, &uvi);

   ...

 } @endverbatim
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Structure to store the data measured by the Si1133
 ******************************************************************************/
typedef struct {
  uint8_t     irq_status;      /**< Interrupt status of the device    */
  int32_t     ch0;             /**< Channel 0 measurement data        */
  int32_t     ch1;             /**< Channel 1 measurement data        */
  int32_t     ch2;             /**< Channel 2 measurement data        */
  int32_t     ch3;             /**< Channel 3 measurement data        */
} sl_si1133_samples_t;


/***************************************************************************//**
 * @addtogroup si1133_details Si1133 Details
 * @brief   Register interface and implementation details
 * @{
 ******************************************************************************/
/**************************************************************************//**
* @addtogroup si1133_registers Registers
* @brief Si1133 register definitions
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
/** @} (end addtogroup si1133_registers) */

/**************************************************************************//**
* @addtogroup si1133_parameters Parameters
* @brief Si1133 parameters
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
/** @} (end addtogroup si1133_parameters) */

/**************************************************************************//**
* @addtogroup si1133_commands Commands
* @brief Si1133 commands
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
/** @} (end addtogroup si1133_commands) */

/**************************************************************************//**
* @addtogroup si1133_responses Responses
* @brief Si1133 responses
* @{
******************************************************************************/
#define SI1133_RSP0_CHIPSTAT_MASK   0xE0  /**< Chip state mask in Response0 register                           */
#define SI1133_RSP0_COUNTER_MASK    0x1F  /**< Command counter and error indicator mask in Response0 register  */
#define SI1133_RSP0_SLEEP           0x20  /**< Sleep state indicator bit mask in Response0 register            */
/** @} (end addtogroup si1133_responses) */
/** @} (end addtogroup si1133_details) */

/**************************************************************************//**
 * @brief
 *    Initialize the Si1133 chip.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_INITIALIZATION Initialization Failure
 *****************************************************************************/
sl_status_t sl_si1133_init(sl_i2cspm_t *i2cspm);

/***************************************************************************//**
 * @brief
 *    Stop the measurements on all channel and waits until the chip
 *    goes to sleep state.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_deinit(sl_i2cspm_t *i2cspm);

/***************************************************************************//**
 * @brief
 *    Measure lux and UV index using the Si1133 sensor.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @param[out] lux
 *    The measured ambient light illuminance in lux
 *
 * @param[out] uvi
 *    The measured UV index
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_measure_lux_uvi(sl_i2cspm_t *i2cspm, float *lux, float *uvi);

/***************************************************************************//**
 * @brief
 *    Read Hardware ID from the SI1133 sensor.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @param[out] hardwareID
 *    The Hardware ID of the chip (should be 0x33)
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_get_hardware_id(sl_i2cspm_t *i2cspm, uint8_t *hardwareID);

/***************************************************************************//**
 * @addtogroup si1133_details
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Read register from the Si1133 sensor.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @param[in] reg
 *    The register address to read from in the sensor.
 *
 * @param[out] data
 *    The data read from the sensor
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_read_register(sl_i2cspm_t *i2cspm, uint8_t reg, uint8_t *data);

/***************************************************************************//**
 * @brief
 *    Write register in the Si1133 sensor.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @param[in] reg
 *    The register address to write to in the sensor
 *
 * @param[in] data
 *    The data to write to the sensor
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_write_register(sl_i2cspm_t *i2cspm, uint8_t reg, uint8_t  data);

/***************************************************************************//**
 * @brief
 *    Read a block of data from the Si1133 sensor.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @param[in] reg
 *    The first register to begin reading from
 *
 * @param[in] length
 *    The number of bytes to write to the sensor
 *
 * @param[out] data
 *    The data read from the sensor
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_read_register_block(sl_i2cspm_t *i2cspm, uint8_t reg, uint8_t length, uint8_t *data);

/***************************************************************************//**
 * @brief
 *    Write a block of data to the Si1133 sensor.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @param[in] reg
 *    The first register to begin writing to
 *
 * @param[in] length
 *    The number of bytes to write to the sensor
 *
 * @param[in] data
 *    The data to write to the sensor
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_write_register_block(sl_i2cspm_t *i2cspm, uint8_t reg, uint8_t length, const uint8_t *data);
/** @} {end addtogroup si1133_details} */

/***************************************************************************//**
 * @brief
 *    Reset the Si1133.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_reset(sl_i2cspm_t *i2cspm);

/***************************************************************************//**
 * @brief
 *    Send a RESET COMMAND COUNTER command to the Si1133.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_reset_command_counter(sl_i2cspm_t *i2cspm);

/*************************************************************************//**
 * @brief
 *    Send a FORCE command to the Si1133.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_force_measurement(sl_i2cspm_t *i2cspm);

/***************************************************************************//**
 * @brief
 *    Send a PAUSE command to the Si1133.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_pause_measurement(sl_i2cspm_t *i2cspm);

/***************************************************************************//**
 * @brief
 *    Send a START command to the Si1133.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_start_measurement(sl_i2cspm_t *i2cspm);

/***************************************************************************//**
 * @brief
 *    Write a byte to an Si1133 Parameter.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @param[in] address
 *    The parameter address
 *
 * @param[in] value
 *    The byte value to be written to the Si1133 parameter
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 *
 * @note
 *    This function ensures that the Si1133 is idle and ready to
 *    receive a command before writing the parameter. Furthermore,
 *    command completion is checked. If setting parameter is not done
 *    properly, no measurements will occur. This is the most common
 *    error. It is highly recommended that host code make use of this
 *    function.
 ******************************************************************************/
sl_status_t sl_si1133_set_parameter(sl_i2cspm_t *i2cspm, uint8_t address, uint8_t value);

/***************************************************************************//**
 * @brief
 *    Read a parameter from the Si1133.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @param[in] address
 *    The address of the parameter.
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_read_parameter(sl_i2cspm_t *i2cspm, uint8_t address);

/***************************************************************************//**
 * @brief
 *    Read samples from the Si1133 chip.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @param[out] samples
 *    Retrieves interrupt status and measurement data for channel 0..3 and
 *    converts the data to int32_t format
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_read_samples(sl_i2cspm_t *i2cspm, sl_si1133_samples_t *samples);

/***************************************************************************//**
 * @brief
 *    Retrieve the sample values from the chip and convert them
 *    to lux and UV index values
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @param[out] lux
 *    The measured ambient light illuminance in lux
 *
 * @param[out] uvi
 *    UV index
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_get_measurement(sl_i2cspm_t *i2cspm, float *lux, float *uvi);

/***************************************************************************//**
 * @brief
 *    Read the interrupt status register of the device.
 *
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 *
 * @param[out] irqStatus
 *    The content of the IRQ status register
 *
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmit failure
 ******************************************************************************/
sl_status_t sl_si1133_get_irq_status(sl_i2cspm_t *i2cspm, uint8_t *irqStatus);
/** @} (end addtogroup si1133) */

#ifdef __cplusplus
}
#endif
#endif // SL_SI1133_H
