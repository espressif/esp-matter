/***************************************************************************//**
 * @file
 * @brief Dot matrix display interface using EBI
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdint.h>
#include "dmd/dmd.h"
#include "dmd_ssd2119_registers.h"
#include "dmd_ssd2119.h"
#include "dmdif_ssd2119_ebi.h"
#include "bsp.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/* Local function prototypes */
static EMSTATUS setNextReg(uint8_t reg);

static volatile uint16_t *command_register;
static volatile uint16_t *data_register;

/**************************************************************************//**
*  @brief
*  Initializes the data interface to the LCD controller SSD2119
*
*
*  @param cmdRegAddr
*  The address in memory where data to the command register in the display
*  controller are written
*  @param dataRegAddr
*  The address in memory where data to the data register in the display
*  controller are written
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMDIF_init(uint32_t cmdRegAddr, uint32_t dataRegAddr)
{
  command_register = (volatile uint16_t*) cmdRegAddr;
  data_register    = (volatile uint16_t*) dataRegAddr;

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Writes a value to a control register in the LCD controller
*
*  @param reg
*  The register that will be written to
*  @param data
*  The value to write to the register
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMDIF_writeReg(uint8_t reg, uint16_t data)
{
  setNextReg(reg);

  *data_register = data;

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Reads the device code of the LCD controller
*  DOESN'T WORK
*
*  @return
*  The device code of the LCD controller
******************************************************************************/
uint16_t DMDIF_readDeviceCode(void)
{
  uint16_t deviceCode;

  /* Reading from the oscillation control register gives the device code */
  setNextReg(DMD_SSD2119_DEVICE_CODE_READ);

  deviceCode    = *data_register;

  return deviceCode;
}

/**************************************************************************//**
*  @brief
*  Sends the data access command to the LCD controller to prepare for one or more
*  writes or reads using the DMDIF_writeData() and DMDIF_readData()
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMDIF_prepareDataAccess(void)
{
  setNextReg(DMD_SSD2119_ACCESS_DATA);

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Writes one pixel to the LCD controller. DMDIF_prepareDataAccess() needs to be
*  called before writing data using this function.
*
*  @param data
*  The color value of the pixel to be written in 18bpp format
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMDIF_writeData(uint32_t data)
{
  *data_register = (data & 0x0000FFFF);

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Writes one pixel to the LCD controller. DMDIF_prepareDataAccess() needs to be
*  called before writing data using this function.
*
*  @param data
*  The color value of the pixel to be written in 18bpp format
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMDIF_writeDataRepeated(uint32_t data, int len)
{
  uint16_t pixelData;
  int i;

  /* Write bits [8:0] of the pixel data to bits [8:0] on the output lines */
  pixelData = data & 0x0000FFFF;

  for (i = 0; i < len; i++) {
    *data_register = pixelData;
  }

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Writes one pixel to the LCD controller. DMDIF_prepareDataAccess() needs to be
*  called before writing data using this function.
*
*  @param a
*  The upper 9 bits of color value of the pixel to be written in 18bpp format
*
*  @param b
*  The low 9 bits of color value of the pixel to be written in 18bpp format
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMDIF_writeDataConverted(uint16_t a, uint16_t b)
{
  uint16_t pixel;

  pixel  = b >> 1;
  pixel |= (a << 8) & 0xFF00;

  *data_register = pixel;

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Reads a byte of data from the memory of the LCD controller.
*  DMDIF_prepareDataAccess() needs to be called before using this function.
*  DOESN'T WORK
*
*  @return
*  18bpp value of pixel
******************************************************************************/
uint32_t DMDIF_readData(void)
{
  uint32_t data;

  data = *data_register;

  return data;
}

/**************************************************************************//**
*  \brief
*  Sets the register in the LCD controller to write commands to
*
*  \param reg
*  The next register in the LCD controller to write to
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
static EMSTATUS setNextReg(uint8_t reg)
{
  uint16_t data;

  data = reg & 0xff;
  /* Write the register address to bits [8:1] in the index register */
  *command_register = data;

  return DMD_OK;
}

/** @endcond */
