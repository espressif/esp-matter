/***************************************************************************//**
 * @file
 * @brief Driver for the Cambridge CMOS Sensors CCS811 gas and indoor air
 * quality sensor
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

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "i2cspm.h"

#include "thunderboard/board.h"
#include "thunderboard/ccs811.h"
#include "thunderboard/util.h"

#include "thunderboard/rfs/rfs.h"

/**************************************************************************//**
* @defgroup CCS811 CCS811 - Indoor Air Quality Sensor
* @{
* @brief Driver for the Cambridge CMOS Sensors CCS811 gas and indoor air
* quality sensor
******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#if (CCS811_FIRMWARE_UPDATE > 0)
  #pragma message("CCS811 firmware update included")
  #include "ccs811_firmware.h"
static uint32_t CCS811_firmwareVerificationAndUpdate(void);

static uint32_t CCS811_eraseApplication   (void);
static uint32_t CCS811_verifyApplication  (bool *valid);

static void     debug_printf(const char *format, ...);
#endif

static uint32_t CCS811_setAppStart        (void);

/** @endcond */

/***************************************************************************//**
 * @brief
 *    Initializes the chip and performs firmware upgrade if required
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t CCS811_init(void)
{
  uint8_t id;
  uint32_t status;

  /* Enable the sensor and wake it up */
  BOARD_gasSensorEnable(true);
  BOARD_gasSensorWake(true);

  /* About 80 ms required to reliably start the device, wait a bit more */
  UTIL_delay(100);

  /* Check if the chip present and working by reading the hardware ID */
  status = CCS811_getHardwareID(&id);
  if ( (status != CCS811_OK) && (id != CCS811_HW_ID) ) {
    return CCS811_ERROR_INIT_FAILED;
  }

  /* If enabled check the current firmware version and update it */
  /* if there is a newer one available                           */
#if (CCS811_FIRMWARE_UPDATE > 0)
  status = CCS811_firmwareVerificationAndUpdate();
#endif

  /* Go back to sleep */
  BOARD_gasSensorWake(false);

  return CCS811_OK;
}

/***************************************************************************//**
 * @brief
 *    De-initializes the chip
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t CCS811_deInit(void)
{
  /* Disable the sensor  */
  BOARD_gasSensorWake(false);
  BOARD_gasSensorEnable(false);

  return CCS811_OK;
}

/***************************************************************************//**
 * @brief
 *    Reads Hardware ID from the CSS811 sensor
 *
 * @param[out] hardwareID
 *    The Hardware ID of the chip (should be 0x81)
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t CCS811_getHardwareID(uint8_t *hardwareID)
{
  uint32_t result;

  result = CCS811_readMailbox(CCS811_ADDR_HW_ID, 1, hardwareID);

  return result;
}

/**************************************************************************//**
* @brief
*    Reads the status of the CSS811 sensor
*
* @param[out] status
*    The content of the CSS811 Status Register
*
* @return
*    Returns zero on OK, non-zero otherwise.
******************************************************************************/
uint32_t CCS811_getStatus(uint8_t *status)
{
  uint32_t result;

  result = CCS811_readMailbox(CCS811_ADDR_STATUS, 1, status);

  return result;
}

/***************************************************************************//**
 * @brief
 *    Checks if new measurement data available
 *
 * @return
 *    True if new data available, otherwise false
 ******************************************************************************/
bool CCS811_isDataAvailable(void)
{
  bool state;
  uint32_t status;
  uint8_t reg;

  state = false;
  /* Read the status register */
  status = CCS811_getStatus(&reg);

  /* Check if the DATA_READY bit is set */
  if ( (status == CCS811_OK) && ( (reg & 0x08) == 0x08) ) {
    state = true;
  }

  return state;
}

/***************************************************************************//**
 * @brief
 *    Dumps the registers of the CSS811
 *
 * @return
 *    None
 ******************************************************************************/
void CCS811_dumpRegisters(void)
{
  uint8_t buffer[8];

  CCS811_readMailbox(CCS811_ADDR_STATUS, 1, buffer);
  printf("STATUS       : %02X\r\n", buffer[0]);

  CCS811_readMailbox(CCS811_ADDR_MEASURE_MODE, 1, buffer);
  printf("MEASURE_MODE : %02X\r\n", buffer[0]);

  CCS811_readMailbox(CCS811_ADDR_ALG_RESULT_DATA, 4, buffer);
  printf("ALG_DATA     : %02X%02X  %02X%02X\r\n", buffer[0], buffer[1], buffer[2], buffer[3]);

  CCS811_readMailbox(CCS811_ADDR_RAW_DATA, 2, buffer);
  printf("RAW_DATA     : %02X%02X\r\n", buffer[0], buffer[1]);

  CCS811_readMailbox(CCS811_ADDR_ENV_DATA, 4, buffer);
  printf("ENV_DATA     : %02X%02X  %02X%02X\r\n", buffer[0], buffer[1], buffer[2], buffer[3]);

  CCS811_readMailbox(CCS811_ADDR_NTC, 4, buffer);
  printf("NTC          : %02X%02X  %02X%02X\r\n", buffer[0], buffer[1], buffer[2], buffer[3]);

  CCS811_readMailbox(CCS811_ADDR_THRESHOLDS, 4, buffer);
  printf("THRESHOLDS   : %02X%02X  %02X%02X\r\n", buffer[0], buffer[1], buffer[2], buffer[3]);

  CCS811_readMailbox(CCS811_ADDR_HW_ID, 1, buffer);
  printf("HW_ID        : %02X\r\n", buffer[0]);

  CCS811_readMailbox(CCS811_ADDR_HW_VERSION, 1, buffer);
  printf("HW_VERSION   : %02X\r\n", buffer[0]);

  CCS811_readMailbox(CCS811_ADDR_FW_BOOT_VERSION, 2, buffer);
  printf("BOOT_VERSION : %d.%d.%d\r\n", (buffer[0] >> 4) & 0xF, buffer[0] & 0xF, buffer[1]);

  CCS811_readMailbox(CCS811_ADDR_FW_APP_VERSION, 2, buffer);
  printf("APP_VERSION  : %d.%d.%d\r\n", (buffer[0] >> 4) & 0xF, buffer[0] & 0xF, buffer[1]);

  CCS811_readMailbox(CCS811_ADDR_ERR_ID, 1, buffer);
  printf("ERR_ID       : %02X\r\n", buffer[0]);

  return;
}

/***************************************************************************//**
 * @brief
 *    Switches the CSS811 chip from boot to application mode
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t CCS811_startApplication(void)
{
  uint32_t result;
  uint8_t status;

  /* Read status */
  result = CCS811_readMailbox(CCS811_ADDR_STATUS, 1, &status);

  /* If no application firmware present in the CCS811 then return an error message */
  if ( (status & 0x10) != 0x10 ) {
    return CCS811_ERROR_APPLICATION_NOT_PRESENT;
  }

  /* Issue APP_START */
  result += CCS811_setAppStart();

  /* Check status again */
  result = CCS811_readMailbox(CCS811_ADDR_STATUS, 1, &status);

  /* If the chip firmware did not switch to application mode then return with error */
  if ( (status & 0x90) != 0x90 ) {
    return CCS811_ERROR_NOT_IN_APPLICATION_MODE;
  }

  return result;
}

/***************************************************************************//**
 * @brief
 *    Reads data from a specific Mailbox address
 *
 * @param[in] id
 *    The address of the Mailbox register
 *
 * @param[in] length
 *    The number of bytes to read
 *
 * @param[out] data
 *    The data read from the sensor
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t CCS811_readMailbox(uint8_t id, uint8_t length, uint8_t *data)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_write_data[1];
  uint32_t retval;

  retval = CCS811_OK;

  BOARD_gasSensorWake(true);

  /* Write data */
  i2c_write_data[0] = id;

  /* Configure I2C bus transaction */
  seq.addr        = CCS811_BUS_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE_READ;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;

  /* Select length of data to be read */
  seq.buf[1].data = data;
  seq.buf[1].len = length;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_GAS);

  ret = I2CSPM_Transfer(CCS811_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    retval = CCS811_ERROR_I2C_TRANSACTION_FAILED;
  }

  BOARD_gasSensorWake(false);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Reads measurement data (eCO2 and TVOC) from the CSS811 sensor
 *
 * @param[out] eco2
 *    The eCO2 level read from the sensor
 *
 * @param[out] tvoc
 *    The TVOC level read from the sensor
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t CCS811_getMeasurement(uint16_t *eco2, uint16_t *tvoc)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[4];
  uint8_t i2c_write_data[1];
  uint32_t retval;

  retval = CCS811_OK;

  *eco2 = 0;
  *tvoc = 0;

  BOARD_gasSensorWake(true);

  /* Read four bytes from the ALG_RESULT_DATA mailbox register */
  i2c_write_data[0] = CCS811_ADDR_ALG_RESULT_DATA;

  seq.addr        = CCS811_BUS_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE_READ;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 4;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_GAS);

  ret = I2CSPM_Transfer(CCS811_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    retval = CCS811_ERROR_I2C_TRANSACTION_FAILED;
  } else {
    /* Convert the read bytes to 16 bit values */
    *eco2 = ( (uint16_t) i2c_read_data[0] << 8) + (uint16_t) i2c_read_data[1];
    *tvoc = ( (uint16_t) i2c_read_data[2] << 8) + (uint16_t) i2c_read_data[3];
  }

  BOARD_gasSensorWake(false);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Gets the latest readings from the sense resistor of the CSS811 sensor
 *
 * @param[out] current
 *    The value of current through the sensor
 *
 * @param[out] rawData
 *    The raw ADC reading of the voltage across the sensor with the selected
 *    current
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
uint32_t CCS811_getRawData(uint16_t *current, uint16_t *rawData)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[2];
  uint8_t i2c_write_data[1];
  uint32_t retval;

  retval = CCS811_OK;

  *current = 0;
  *rawData = 0;

  BOARD_gasSensorWake(true);

  /* Read four bytes from the CCS811_ADDR_RAW_DATA mailbox register */
  i2c_write_data[0] = CCS811_ADDR_RAW_DATA;

  seq.addr        = CCS811_BUS_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE_READ;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 2;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_GAS);

  ret = I2CSPM_Transfer(CCS811_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    retval = CCS811_ERROR_I2C_TRANSACTION_FAILED;
  } else {
    /* current: the upper six bits of Byte0 */
    *current = (uint16_t) ( (i2c_read_data[0] >> 2) & 0x3F);
    /* raw ADC reading: the lower two bits of Byte0 is the MSB, Byte1 is the LSB */
    *rawData = (uint16_t) ( (i2c_read_data[0] & 0x03) << 8) + (uint16_t) i2c_read_data[1];
  }

  BOARD_gasSensorWake(false);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Performs software reset on the CCS811
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t CCS811_softwareReset(void)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[4];
  uint8_t i2c_write_data[5];
  uint32_t retval;

  retval = CCS811_OK;

  BOARD_gasSensorWake(true);

  /* Write the 0x11 0xE5 0x72 0x8A key sequence to software reset register */
  /* The key sequence is used to prevent accidental reset                  */
  i2c_write_data[0] = CCS811_ADDR_SW_RESET;
  i2c_write_data[1] = 0x11;
  i2c_write_data[2] = 0xE5;
  i2c_write_data[3] = 0x72;
  i2c_write_data[4] = 0x8A;

  seq.addr        = CCS811_BUS_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 5;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_GAS);

  ret = I2CSPM_Transfer(CCS811_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    retval = CCS811_ERROR_I2C_TRANSACTION_FAILED;
  }

  BOARD_gasSensorWake(false);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Sets the measurement mode of the CSS811 sensor
 *
 * @param[in] measMode
 *    The desired measurement mode
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t CCS811_setMeasureMode(uint8_t measMode)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[1];
  uint8_t i2c_write_data[2];
  uint32_t retval;

  retval = CCS811_OK;

  BOARD_gasSensorWake(true);

  /* Bits 7,6,2,1 and 0 are reserved, clear them */
  measMode = (measMode & 0x38);

  /* Write to the measurement mode register      */
  i2c_write_data[0] = CCS811_ADDR_MEASURE_MODE;
  i2c_write_data[1] = measMode;

  seq.addr        = CCS811_BUS_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 2;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_GAS);

  ret = I2CSPM_Transfer(CCS811_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    retval = CCS811_ERROR_I2C_TRANSACTION_FAILED;
  }

  BOARD_gasSensorWake(false);

  return retval;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Changes the mode of the CCS811 from Boot mode to running the application
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
static uint32_t CCS811_setAppStart(void)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[2];
  uint8_t i2c_write_data[1];
  uint32_t retval;

  retval = CCS811_OK;

  BOARD_gasSensorWake(true);

  /* Perform a write with no data to the APP_START register to change the */
  /* state from boot mode to application mode                             */
  i2c_write_data[0] = CCS811_ADDR_APP_START;

  seq.addr        = CCS811_BUS_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_GAS);

  ret = I2CSPM_Transfer(CCS811_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    retval = CCS811_ERROR_I2C_TRANSACTION_FAILED;
  }

  BOARD_gasSensorWake(false);

  return retval;
}

#if (CCS811_FIRMWARE_UPDATE > 0)

/***************************************************************************//**
 * @brief
 *    Erases the application firmware on the CCS811
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
static uint32_t CCS811_eraseApplication(void)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[4];
  uint8_t i2c_write_data[5];
  uint32_t retval;

  retval = CCS811_OK;

  BOARD_gasSensorWake(true);

  /* Write the 0xE7 0xA7 0xE6 0x09 key sequence to firmware erase register */
  /* The key sequence is used to prevent accidental erase                  */
  i2c_write_data[0] = CCS811_ADDR_FW_ERASE;
  i2c_write_data[1] = 0xE7;
  i2c_write_data[2] = 0xA7;
  i2c_write_data[3] = 0xE6;
  i2c_write_data[4] = 0x09;

  seq.addr        = CCS811_BUS_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 5;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_GAS);

  ret = I2CSPM_Transfer(CCS811_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    retval = CCS811_ERROR_I2C_TRANSACTION_FAILED;
  }

  BOARD_gasSensorWake(false);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Writes 8 bytes of data to the firmware memory of the CCS811
 *
 * @param[in] buffer
 *    An 8-byte lenght buffer containig the data to write
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
static uint32_t CCS811_programFirmware(uint8_t buffer[])
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[2];
  uint8_t i2c_write_data[9];
  uint32_t retval;

  retval = CCS811_OK;

  BOARD_gasSensorWake(true);

  /* Send the Write Data to FLASH command and 8 bytes of binary program code */
  i2c_write_data[0] = CCS811_ADDR_FW_PROGRAM;
  i2c_write_data[1] = buffer[0];
  i2c_write_data[2] = buffer[1];
  i2c_write_data[3] = buffer[2];
  i2c_write_data[4] = buffer[3];
  i2c_write_data[5] = buffer[4];
  i2c_write_data[6] = buffer[5];
  i2c_write_data[7] = buffer[6];
  i2c_write_data[8] = buffer[7];

  seq.addr        = CCS811_BUS_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 9;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_GAS);

  ret = I2CSPM_Transfer(CCS811_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    retval = CCS811_ERROR_I2C_TRANSACTION_FAILED;
  }

  BOARD_gasSensorWake(false);

  return retval;
}

/***************************************************************************//**
 * @brief
 *    Verifies if the application firmware downloaded to the CCS811
 *    was received error free
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 *****************************************************************************/
static uint32_t CCS811_verifyApplication(bool *appValid)
{
  uint32_t status;
  uint8_t reg;
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[2];
  uint8_t i2c_write_data[2];

  *appValid = false;

  BOARD_gasSensorWake(true);

  /* Write (with no data) to the verify register to check if the firmware */
  /* programmed to the CCS811 was received error free                     */
  i2c_write_data[0] = CCS811_ADDR_FW_VERIFY;

  seq.addr        = CCS811_BUS_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 1;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_GAS);

  status = CCS811_OK;
  ret = I2CSPM_Transfer(CCS811_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    status = CCS811_ERROR_I2C_TRANSACTION_FAILED;
  } else {
    /* Wait until the Verify command finishes */
    UTIL_delay(100);
    /* Check the status register to see if there were no errors */
    status = CCS811_getStatus(&reg);
    if ( (status == CCS811_OK) && ((reg & 0x10) == 0x10) ) {
      *appValid = true;
    }
  }

  BOARD_gasSensorWake(false);

  return status;
}

/***************************************************************************//**
 * @brief
 *    Checks the current firmware version, compares to the available update
 *    file version and peforms the update and verification if the file is
 *    newer than the current one.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
static uint32_t CCS811_firmwareVerificationAndUpdate(void)
{
  uint8_t *fileName;
  RFS_FileHandle fh;

  uint32_t status;
  uint8_t reg;
  uint8_t buffer[8];
  uint8_t major, minor, patch;
  int fmajor, fminor, fpatch;
  int fileLength;
  char vStr[3 + 1 + 3 + 1 + 3];
  int i, n;
  int pi, pus;
  bool upgrade;
  bool valid;

  /*************************************************************************/
  /** Read CCS811 firmware versions and validate RFS file                 **/
  /*************************************************************************/
  CCS811_readMailbox(CCS811_ADDR_FW_BOOT_VERSION, 2, buffer);
  major = (buffer[0] >> 4) & 0xF;
  minor = buffer[0] & 0xF;
  patch = buffer[1];
  debug_printf("BOOT_VERSION  : %d.%d.%d\r\n", major, minor, patch);

  CCS811_readMailbox(CCS811_ADDR_FW_APP_VERSION, 2, buffer);
  major = (buffer[0] >> 4) & 0xF;
  minor = buffer[0] & 0xF;
  patch = buffer[1];
  debug_printf("APP_VERSION   : %d.%d.%d\r\n", major, minor, patch);

  fileName = RFS_getFileNameByIndex(0);
  if ( !RFS_fileOpen(&fh, fileName) ) {
    debug_printf("RFS file open failed!\r\n");
    return CCS811_ERROR_FIRMWARE_UPDATE_FAILED;
  }
  fileLength = RFS_getFileLength(&fh);

  debug_printf("RFS file : %s [%d]\r\n", (char *) fileName, fileLength);
  if ( (fileLength % 8) != 0 ) {
    debug_printf("RFS file not 8 byte multiple!\r\n");
    return CCS811_ERROR_FIRMWARE_UPDATE_FAILED;
  }

  /*************************************************************************/
  /** Check file version vs. CCS811 version                               **/
  /*************************************************************************/
  for ( i = 0; i < strlen( (char const *) fileName); i++ ) {
    if ( fileName[i] == '_' ) {
      pus = i + 1;
    }
    if ( fileName[i] == '.' ) {
      pi = i - 1;
      break;
    }
  }

  /* Copy the version */
  memset(vStr, 0, sizeof(vStr) );
  for ( i = pus; i <= pi; i++ ) {
    vStr[i - pus] = fileName[i];
  }

  n = sscanf(vStr, "%dv%dp%d", &fmajor, &fminor, &fpatch);
  if ( n != 3 ) {
    return CCS811_ERROR_FIRMWARE_UPDATE_FAILED;
  }

  /* Check versioning */
  upgrade = (fmajor > major)
            || ((fmajor == major) && (fminor > minor))
            || ((fmajor == major) && (fminor == minor) && (fpatch > patch));

  /* Check for invalid version */
  if ( (major == 0xf) && (minor == 0xf) && (patch == 0xff) ) {
    upgrade = true;
  }

  if ( !upgrade ) {
    return CCS811_OK;
  }

  /*************************************************************************/
  /** Put CCS811 in BOOT mode                                             **/
  /*************************************************************************/
  status = CCS811_getStatus(&reg);
  if ( (reg & 0x80) == 0x80 ) {
    /* In APP mode - Go to BOOT mode */

    status = CCS811_softwareReset();
    if ( (status & 0x80) != 0x00 ) {
      return CCS811_ERROR_FIRMWARE_UPDATE_FAILED;
    }

    UTIL_delay(100);

    /* Check for BOOT mode */
    status = CCS811_getStatus(&reg);
    if ( (reg & 0x80) != 0x00 ) {
      return CCS811_ERROR_FIRMWARE_UPDATE_FAILED;
    }
  }

  /*************************************************************************/
  /** Erase                                                               **/
  /*************************************************************************/
  status = CCS811_getStatus(&reg);
  if ( (reg & 0x10) == 0x10 ) {
    /* Valid application - Erase this */

    status = CCS811_eraseApplication();
    UTIL_delay(500);
    if ( (status & 0x10) != 0x00 ) {
      return CCS811_ERROR_FIRMWARE_UPDATE_FAILED;
    }

    UTIL_delay(100);

    /* Check APP_VALID flag again */
    status = CCS811_getStatus(&reg);
    if ( (reg & 0x10) != 0x00 ) {
      return CCS811_ERROR_FIRMWARE_UPDATE_FAILED;
    }
  }

  /*************************************************************************/
  /** Program APP                                                         **/
  /*************************************************************************/
  for ( i = 0; i < fileLength; i += 8 ) {
    /* Read the next 8 bytes from the binary firmware file */
    n = RFS_fileRead(buffer, 1, 8, &fh);
    debug_printf("FW prog [%d] : %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X ",
                 i / 8,
                 buffer[0], buffer[1], buffer[2], buffer[3],
                 buffer[4], buffer[5], buffer[6], buffer[7]
                 );
    if ( n != 1 ) {
      return CCS811_ERROR_FIRMWARE_UPDATE_FAILED;
    }

    /* Write 8 bytes to the device's flash memory */
    status = CCS811_programFirmware(buffer);
    if ( status != CCS811_OK ) {
      debug_printf(" FAIL! \r\n");
      return CCS811_ERROR_FIRMWARE_UPDATE_FAILED;
    } else {
      debug_printf(" OK \r\n");
    }
  }

  /*************************************************************************/
  /** Verify APP                                                          **/
  /*************************************************************************/
  status = CCS811_verifyApplication(&valid);
  if ( (status != CCS811_OK) || (valid == false) ) {
    return false;
  }

  /*************************************************************************/
  /** Power cycle sensor                                                  **/
  /*************************************************************************/
  debug_printf("Cycling CCS811 power domain...");
  BOARD_gasSensorEnable(false);
  UTIL_delay(200);
  BOARD_gasSensorEnable(true);
  UTIL_delay(200);
  debug_printf("OK\r\n");

  return true;
}

static void debug_printf(const char *format, ...)
{
#ifdef DEBUG
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
#endif
}

#endif

/** @endcond */

/***************************************************************************//**
 * @brief
 *    Writes temperature and humidity values to the environmental data regs
 *
 * @param[in] tempData
 *    The environmental temperature in milli-Celsius
 *
 * @param[in] rhData
 *    The relative humidity in milli-percent
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t CCS811_setEnvData(int32_t tempData, uint32_t rhData)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t i2c_read_data[4];
  uint8_t i2c_write_data[5];
  uint8_t humidityRegValue;
  uint8_t temperatureRegValue;
  uint32_t retval;

  BOARD_gasSensorWake(true);

  /* The CCS811 currently supports only 0.5% resolution        */
  /* If the fraction greater than 0.7 then round up the value  */
  /* Shift to the left by one to meet the required data format */
  if ( ( (rhData % 1000) / 100) > 7 ) {
    humidityRegValue = (rhData / 1000 + 1) << 1;
  } else {
    humidityRegValue = (rhData / 1000) << 1;
  }

  /* If the fraction is greater than 0.2 or less than 0.8 set the            */
  /* LSB bit, which is the most significant bit of the fraction 2^(-1) = 0.5 */
  if ( ( (rhData % 1000) / 100) > 2 && ( ( (rhData % 1000) / 100) < 8) ) {
    humidityRegValue |= 0x01;
  }

  /* Add +25 C to the temperature value                        */
  /* The CCS811 currently supports only 0.5C resolution        */
  /* If the fraction greater than 0.7 then round up the value  */
  /* Shift to the left by one to meet the required data format */
  tempData += 25000;
  if ( ( (tempData % 1000) / 100) > 7 ) {
    temperatureRegValue = (tempData / 1000 + 1) << 1;
  } else {
    temperatureRegValue = (tempData / 1000) << 1;
  }

  /* If the fraction is greater than 0.2 or less than 0.8 set the            */
  /* LSB bit, which is the most significant bit of the fraction 2^(-1) = 0.5 */
  if ( ( (tempData % 1000) / 100) > 2 && ( ( (tempData % 1000) / 100) < 8) ) {
    temperatureRegValue |= 0x01;
  }

  /* Write the correctly formatted values to the environmental data register */
  /* The LSB bytes of the humidity and temperature data are 0x00 because     */
  /* the current CCS811 supports only 0.5% and 0.5C resolution               */
  i2c_write_data[0] = CCS811_ADDR_ENV_DATA;
  i2c_write_data[1] = humidityRegValue;
  i2c_write_data[2] = 0x00;
  i2c_write_data[3] = temperatureRegValue;
  i2c_write_data[4] = 0x00;

  seq.addr        = CCS811_BUS_ADDRESS;
  seq.flags       = I2C_FLAG_WRITE;
  seq.buf[0].data = i2c_write_data;
  seq.buf[0].len  = 5;
  seq.buf[1].data = i2c_read_data;
  seq.buf[1].len  = 0;

  BOARD_i2cBusSelect(BOARD_I2C_BUS_SELECT_GAS);

  retval = CCS811_OK;

  ret = I2CSPM_Transfer(CCS811_I2C_DEVICE, &seq);
  if ( ret != i2cTransferDone ) {
    retval = CCS811_ERROR_I2C_TRANSACTION_FAILED;
  }

  BOARD_gasSensorWake(false);

  return retval;
}
/** @} (end defgroup CCS811) */
/** @} {end addtogroup TBSense_BSP} */
