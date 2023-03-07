/***************************************************************************//**
 * @file
 * @brief Dot matrix display driver for LCD controller SSD2119
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
#include <stdlib.h>
#include <stdio.h>
#include "dmd/dmd.h"
#include "dmd_ssd2119.h"
#include "dmd_ssd2119_registers.h"
#include "dmdif_ssd2119_ebi.h"

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** Dimensions of the display */
DMD_DisplayGeometry dimensions;

/* Local variables */
static uint32_t initialized = 0;
static uint16_t rcDriverOutputControl;

/* Local function prototypes */
static uint32_t colorTransform24To18bpp(uint8_t red,
                                        uint8_t green, uint8_t blue);
static void colorTransform18To24bpp(uint32_t color, uint8_t *red,
                                    uint8_t *green, uint8_t *blue);
static EMSTATUS setPixelAddress(uint16_t x, uint16_t y);

/**************************************************************************//**
*  @brief
*  Initializes the LCD display
*
*  @param InitConfig
*     Not used in this DMD module.
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_init(DMD_InitConfig *initConfig)
{
  uint16_t data;
  (void)   initConfig;  /* Suppress compiler warning. */

  /* Initialize register cache variables */
  rcDriverOutputControl = 0;

  /* Initialization sequence, see UMSH-8252MD-T page 13 */

  /*  printf("R%x: 0x%x\n", DMD_SSD2119_VCOM_OTP_1, 0x0006); */
  DMDIF_writeReg(DMD_SSD2119_VCOM_OTP_1, 0x0006);

  /* Start oscillation */
  data = DMD_SSD2119_OSCILLATION_START_OSCEN;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_OSCILLATION_START, data); */
  DMDIF_writeReg(DMD_SSD2119_OSCILLATION_START, data);

  /* Exit sleep mode */
  data = 0;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_SLEEP_MODE_1, data); */
  DMDIF_writeReg(DMD_SSD2119_SLEEP_MODE_1, data);

  /* Display control */
  data  = DMD_SSD2119_DISPLAY_CONTROL_DTE;
  data |= DMD_SSD2119_DISPLAY_CONTROL_GON;
  data |= DMD_SSD2119_DISPLAY_CONTROL_D1;
  data |= DMD_SSD2119_DISPLAY_CONTROL_D0;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_DISPLAY_CONTROL, data); */
  DMDIF_writeReg(DMD_SSD2119_DISPLAY_CONTROL, data);

  /* Entry mode */
  data  = DMD_SSD2119_ENTRY_MODE_DFM_262K << DMD_SSD2119_ENTRY_MODE_DFM_SHIFT;
  data |= DMD_SSD2119_ENTRY_MODE_DENMODE;
  data |= DMD_SSD2119_ENTRY_MODE_WMODE;
  data |= DMD_SSD2119_ENTRY_MODE_NOSYNC;
  data |= DMD_SSD2119_ENTRY_MODE_TY_TYPE_B << DMD_SSD2119_ENTRY_MODE_TY_SHIFT;
  data |= DMD_SSD2119_ENTRY_MODE_ID1;
  data |= DMD_SSD2119_ENTRY_MODE_ID0;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_ENTRY_MODE, data); */
  DMDIF_writeReg(DMD_SSD2119_ENTRY_MODE, data);

  /* LCD AC control */
  data  = DMD_SSD2119_LCD_AC_CONTROL_BC;
  data |= DMD_SSD2119_LCD_AC_CONTROL_EOR;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_LCD_AC_CONTROL, data); */
  DMDIF_writeReg(DMD_SSD2119_LCD_AC_CONTROL, data);

  /* Power control */
  data  = 0x06 << DMD_SSD2119_POWER_CONTROL_1_DCT_SHIFT;
  data |= 0x05 << DMD_SSD2119_POWER_CONTROL_1_BT_SHIFT;
  data |= 0x03 << DMD_SSD2119_POWER_CONTROL_1_DC_SHIFT;
  data |= 0x04 << DMD_SSD2119_POWER_CONTROL_1_AP_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_POWER_CONTROL_1, data); */
  DMDIF_writeReg(DMD_SSD2119_POWER_CONTROL_1, data);

  /* Driver output control */
  data                  = DMD_SSD2119_DRIVER_OUTPUT_CONTROL_RL;
  data                 |= DMD_SSD2119_DRIVER_OUTPUT_CONTROL_REV;
  data                 |= DMD_SSD2119_DRIVER_OUTPUT_CONTROL_GD;
  data                 |= DMD_SSD2119_DRIVER_OUTPUT_CONTROL_TB;
  data                 |= (DMD_VERTICAL_SIZE - 1) << DMD_SSD2119_DRIVER_OUTPUT_CONTROL_MUX_SHIFT;
  rcDriverOutputControl = data;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_DRIVER_OUTPUT_CONTROL, data); */
  DMDIF_writeReg(DMD_SSD2119_DRIVER_OUTPUT_CONTROL, data);

  /* Power Control */
  data = 0x05 << DMD_SSD2119_POWER_CONTROL_2_VRC_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_POWER_CONTROL_2, data); */
  DMDIF_writeReg(DMD_SSD2119_POWER_CONTROL_2, data);

  data = 0x0D << DMD_SSD2119_POWER_CONTROL_3_VRH_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_POWER_CONTROL_3, data); */
  DMDIF_writeReg(DMD_SSD2119_POWER_CONTROL_3, data);

  data  = DMD_SSD2119_POWER_CONTROL_4_VCOMG;
  data |= 0x0D << DMD_SSD2119_POWER_CONTROL_4_VDV_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_POWER_CONTROL_4, data); */
  DMDIF_writeReg(DMD_SSD2119_POWER_CONTROL_4, data);

  data  = DMD_SSD2119_POWER_CONTROL_5_NOTP;
  data |= 0x3E << DMD_SSD2119_POWER_CONTROL_5_VCM_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_POWER_CONTROL_5, data); */
  DMDIF_writeReg(DMD_SSD2119_POWER_CONTROL_5, data);

  data = 0x0058;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_GENERIC_INTERFACE_CONTROL, data); */
  DMDIF_writeReg(DMD_SSD2119_GENERIC_INTERFACE_CONTROL, data);

  /* Gamma settings */
  data  = 0x00 << DMD_SSD2119_GAMMA_1_PKP1_SHIFT;
  data |= 0x00 << DMD_SSD2119_GAMMA_1_PKP0_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_GAMMA_1, data); */
  DMDIF_writeReg(DMD_SSD2119_GAMMA_1, data);

  data  = 0x01 << DMD_SSD2119_GAMMA_2_PKP3_SHIFT;
  data |= 0x01 << DMD_SSD2119_GAMMA_2_PKP2_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_GAMMA_2, data); */
  DMDIF_writeReg(DMD_SSD2119_GAMMA_2, data);

  data  = 0x01 << DMD_SSD2119_GAMMA_3_PKP5_SHIFT;
  data |= 0x00 << DMD_SSD2119_GAMMA_3_PKP4_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_GAMMA_3, data); */
  DMDIF_writeReg(DMD_SSD2119_GAMMA_3, data);

  data  = 0x03 << DMD_SSD2119_GAMMA_4_PRP1_SHIFT;
  data |= 0x05 << DMD_SSD2119_GAMMA_4_PRP0_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_GAMMA_4, data); */
  DMDIF_writeReg(DMD_SSD2119_GAMMA_4, data);

  data  = 0x07 << DMD_SSD2119_GAMMA_5_PKN1_SHIFT;
  data |= 0x07 << DMD_SSD2119_GAMMA_5_PKN0_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_GAMMA_5, data); */
  DMDIF_writeReg(DMD_SSD2119_GAMMA_5, data);

  data  = 0x03 << DMD_SSD2119_GAMMA_6_PKN3_SHIFT;
  data |= 0x05 << DMD_SSD2119_GAMMA_6_PKN2_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_GAMMA_6, data); */
  DMDIF_writeReg(DMD_SSD2119_GAMMA_6, data);

  data  = 0x07 << DMD_SSD2119_GAMMA_7_PKN5_SHIFT;
  data |= 0x07 << DMD_SSD2119_GAMMA_7_PKN4_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_GAMMA_7, data); */
  DMDIF_writeReg(DMD_SSD2119_GAMMA_7, data);

  data  = 0x02 << DMD_SSD2119_GAMMA_8_PRN1_SHIFT;
  data |= 0x01 << DMD_SSD2119_GAMMA_8_PRN0_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_GAMMA_8, data); */
  DMDIF_writeReg(DMD_SSD2119_GAMMA_8, data);

  data  = 0x12 << DMD_SSD2119_GAMMA_9_VRP1_SHIFT;
  data |= 0x00 << DMD_SSD2119_GAMMA_9_VRP0_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_GAMMA_9, data); */
  DMDIF_writeReg(DMD_SSD2119_GAMMA_9, data);

  data  = 0x09 << DMD_SSD2119_GAMMA_10_VRN1_SHIFT;
  data |= 0x00 << DMD_SSD2119_GAMMA_10_VRN0_SHIFT;
  /*  printf("R%x: 0x%x\n", DMD_SSD2119_GAMMA_10, data); */
  DMDIF_writeReg(DMD_SSD2119_GAMMA_10, data);

  /* Set up dimensions of the display */
  dimensions.xSize = DMD_HORIZONTAL_SIZE;
  dimensions.ySize = DMD_VERTICAL_SIZE;

  /* At initialization, the clip is the entire display */
  dimensions.xClipStart = 0;
  dimensions.yClipStart = 0;
  dimensions.clipWidth  = dimensions.xSize;
  dimensions.clipHeight = dimensions.ySize;

  initialized = 1;

  /* Fill the entire display with black color */
  DMD_writeColor(0, 0, 0x00, 0x00, 0x00, dimensions.xSize * dimensions.ySize);

  return DMD_OK;
}

/**************************************************************************//**
*  \brief
*  Get the dimensions of the display and of the current clipping area
*
*  \return
*  DMD_Dimensions structure containing the size of the display and the
*  clipping area
******************************************************************************/
EMSTATUS DMD_getDisplayGeometry(DMD_DisplayGeometry **geometry)
{
  if (!initialized) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }
  *geometry = &dimensions;

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Sets the clipping area. All coordinates given to writeData/writeColor/readData
*  are relative to this clipping area.
*
*  @param xStart
*  X coordinate of the upper left corner of the clipping area
*  @param yStart
*  Y coordinate of the upper left corner of the clipping area
*  @param width
*  Width of the clipping area
*  @param height
*  Height of the clipping area
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_setClippingArea(uint16_t xStart, uint16_t yStart,
                             uint16_t width, uint16_t height)
{
  uint16_t verticalPos;
  uint16_t xEnd;
  uint16_t yEnd;

  if (!initialized) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  /* Check parameters */
  if (xStart + width > dimensions.xSize
      || yStart + height > dimensions.ySize) {
    return DMD_ERROR_PIXEL_OUT_OF_BOUNDS;
  }

  if (width == 0 || height == 0) {
    return DMD_ERROR_EMPTY_CLIPPING_AREA;
  }

  xEnd = xStart + width - 1;
  yEnd = yStart + height - 1;

  /* Set the clipping region in the display */
  DMDIF_writeReg(DMD_SSD2119_HORIZONTAL_RAM_ADDRESS_START_POS, xStart);
  DMDIF_writeReg(DMD_SSD2119_HORIZONTAL_RAM_ADDRESS_END_POS, xEnd);

  verticalPos  = yEnd << DMD_SSD2119_VERTICAL_RAM_ADDRESS_POS_END_SHIFT;
  verticalPos |= yStart << DMD_SSD2119_VERTICAL_RAM_ADDRESS_POS_START_SHIFT;
  DMDIF_writeReg(DMD_SSD2119_VERTICAL_RAM_ADDRESS_POS, verticalPos);

  /* Update the dimensions structure */
  dimensions.xClipStart = xStart;
  dimensions.yClipStart = yStart;
  dimensions.clipWidth  = width;
  dimensions.clipHeight = height;

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Draws pixels to the display
*
*  @param x
*  X coordinate of the first pixel to be written, relative to the clipping area
*  @param y
*  Y coordinate of the first pixel to be written, relative to the clipping area
*  @param data
*  Array containing the pixel data. Each 8-bit element in the array are one color
*  component of the pixel, so that 3 bytes represent one pixel. The pixels are
*  ordered by increasing x coordinate, after the last pixel of a row, the next
*  pixel will be the first pixel on the next row.
*  @param numPixels
*  Number of pixels to be written
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_writeData(uint16_t x, uint16_t y, const uint8_t data[],
                       uint32_t numPixels)
{
  uint32_t statusCode;
  uint32_t clipRemaining;
  uint32_t color;
  uint32_t i;

  if (!initialized) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  /* Set the address of the first pixel */
  statusCode = setPixelAddress(x, y);
  if (statusCode != DMD_OK) {
    return statusCode;
  }

  /* Number of pixels from the first pixel (given by x and y) to the end
   * of the clipping area */
  clipRemaining = (dimensions.clipHeight - y) * dimensions.clipWidth - x;

  /* Check that the length of data isn't longer than the number of pixels
   * in the rest of the clipping area */
  if (numPixels > clipRemaining) {
    return DMD_ERROR_TOO_MUCH_DATA;
  }

  /* Write data */
  DMDIF_prepareDataAccess( );
  for (i = 0; i < numPixels; i++) {
    color = colorTransform24To18bpp(data[3 * i], data[3 * i + 1],
                                    data[3 * i + 2]);
    DMDIF_writeData(color);
  }

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Reads data from display memory
*  DOESN'T WORK yet - TIMING ISSUE?
*
*  @param x
*  X coordinate of the first pixel to be read, relative to the clipping area
*  @param y
*  Y coordinate of the first pixel to be read, relative to the clipping area
*  @param data
*  Pointer to where the pixel data will be stored
*  @param numPixels
*  Number of pixels to be read
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_readData(uint16_t x, uint16_t y,
                      uint8_t data[], uint32_t numPixels)
{
  uint32_t statusCode;
  uint32_t clipRemaining;
  uint32_t i;
  uint32_t color;
  uint8_t  red, green, blue;

  if (!initialized) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  /* Set the address of the first pixel */
  statusCode = setPixelAddress(x, y);
  if (statusCode != DMD_OK) {
    return statusCode;
  }

  /* Number of pixels from the first pixel (given by x and y) to the end
   * of the clipping area */
  clipRemaining = (dimensions.clipHeight - y) * dimensions.clipWidth - x;

  /* Check that the length of data isn't longer than the number of pixels
   * in the rest of the clipping area */
  if (numPixels > clipRemaining) {
    return DMD_ERROR_TOO_MUCH_DATA;
  }

  /* Read data */
  DMDIF_prepareDataAccess();
  for (i = 0; i < numPixels; i++) {
    /* Dummy read  */
    color = DMDIF_readData();
    /* Read the color value */
    color = DMDIF_readData();

    /* Transform into 24bpp */
    colorTransform18To24bpp(color, &red, &green, &blue);

    data[3 * i]     = red;
    data[3 * i + 1] = green;
    data[3 * i + 2] = blue;

    /* Update the position. This is not done automatically for reading, so
     * it must be set for every read. */
    x++;
    if (x > dimensions.clipWidth) {
      y++;
      x = 0;
      if (y > dimensions.clipHeight) {
        y = 0;
      }
    }

    statusCode = setPixelAddress(x, y);
    if (statusCode != DMD_OK) {
      return statusCode;
    }
  }

  return DMD_OK;
}

/**************************************************************************//**
*  \brief
*  Draws a number of pixels of the same color to the display
*
*  @param x
*  X coordinate of the first pixel to be written, relative to the clipping area
*  @param y
*  Y coordinate of the first pixel to be written, relative to the clipping area
*  @param red
*  Red component of the color
*  @param green
*  Green component of the color
*  @param blue
*  Blue component of the color
*  @param numPixels
*  Number of pixels to be written
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_writeColor(uint16_t x, uint16_t y, uint8_t red,
                        uint8_t green, uint8_t blue, uint32_t numPixels)
{
  uint32_t i;
  uint32_t clipRemaining;
  uint32_t statusCode;
  uint32_t color;

  if (!initialized) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  /* Set the address of the first pixel */
  statusCode = setPixelAddress(x, y);
  if (statusCode != DMD_OK) {
    return statusCode;
  }

  /* Number of pixels from the first pixel (given by x and y) to the end
   * of the clipping area */
  clipRemaining = (dimensions.clipHeight - y) * dimensions.clipWidth - x;

  /* Check that the length of data isn't longer than the number of pixels
   * in the rest of the clipping area */
  if (numPixels > clipRemaining) {
    return DMD_ERROR_TOO_MUCH_DATA;
  }

  /* Write data */
  DMDIF_prepareDataAccess( );
  color = colorTransform24To18bpp(red, green, blue);
  for (i = 0; i < numPixels; i++) {
    DMDIF_writeData(color);
  }

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Turns off the display and puts it into sleep mode
*  Does not turn off backlight
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_sleep(void)
{
  uint16_t data;

  if (!initialized) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  /* Put into sleep mode */
  data = DMD_SSD2119_SLEEP_MODE_1_SLP;
  DMDIF_writeReg(DMD_SSD2119_SLEEP_MODE_1, data);

  /* Turn off display */
  data = 0;
  DMDIF_writeReg(DMD_SSD2119_DISPLAY_CONTROL, 0x0000);

  /* Delay 1.5 frame */
  DMDIF_delay((1000 / DMD_FRAME_FREQUENCY) * 3 / 2);

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Wakes up the display from sleep mode
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMD_wakeUp(void)
{
  uint16_t data;

  if (!initialized) {
    return DMD_ERROR_DRIVER_NOT_INITIALIZED;
  }

  /* Get out of sleep mode */
  data = 0;
  DMDIF_writeReg(DMD_SSD2119_SLEEP_MODE_1, data);

  /* Turn on display */
  data  = DMD_SSD2119_DISPLAY_CONTROL_DTE;
  data |= DMD_SSD2119_DISPLAY_CONTROL_GON;
  data |= DMD_SSD2119_DISPLAY_CONTROL_D1;
  data |= DMD_SSD2119_DISPLAY_CONTROL_D0;
  DMDIF_writeReg(DMD_SSD2119_DISPLAY_CONTROL, 0x0033);

  /* Delay 10 frames */
  DMDIF_delay((1000 / DMD_FRAME_FREQUENCY) * 10);

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Transforms a 24bpp pixel data into an 18bpp pixel
*
*  @param red
*  8-bit red component of the pixel
*  @param green
*  8-bit green component of the pixel
*  @param blue
*  8-bit blue component of the pixel
*  @return
*  18bpp value of pixel
******************************************************************************/
static uint32_t colorTransform24To18bpp(uint8_t red, uint8_t green, uint8_t blue)
{
  /* Transform each color into 6 bits by dropping the 2 LSB */
  red   = red >> 2;
  green = green >> 2;
  blue  = blue >> 2;

  /* Put it together to one 18bpp color number */
  return (red << 12) | (green << 6) | blue;
}

/**************************************************************************//**
*  @brief
*  Transforms an 18 bpp pixel into a 24bpp pixel
*
*  @param color
*  18bpp color pixel
*  @param red
*  return value for red component of 24bpp pixel
*  @param green
*  return value for green component of 24bpp pixel
*  @param blue
*  return value for blue component of 24bpp pixel
******************************************************************************/
static void colorTransform18To24bpp(uint32_t color, uint8_t *red,
                                    uint8_t *green, uint8_t *blue)
{
  /* Get the individual colors out of the 18bpp number */
  uint8_t redValue   = (color & 0x0003F000) >> 12;
  uint8_t greenValue = (color & 0x00000FC0) >> 6;
  uint8_t blueValue  = (color & 0x0000003F);

  /* Convert each color to 8-bit */
  redValue   <<= 2;
  greenValue <<= 2;
  blueValue  <<= 2;

  *red   = redValue;
  *green = greenValue;
  *blue  = blueValue;

  return;
}

/**************************************************************************//**
*  @brief
*  Set the x and y coordinate of the next pixel to draw
*
*  @param x
*  X address of the pixel, relative to the current clipping area
*  @param y
*  Y address of the pixel, relative to the current clipping area
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS setPixelAddress(uint16_t x, uint16_t y)
{
  /* Check parameters */
  if (x > dimensions.clipWidth || y > dimensions.clipHeight) {
    return DMD_ERROR_PIXEL_OUT_OF_BOUNDS;
  }

  /* Set pixel position */
  DMDIF_writeReg(DMD_SSD2119_SET_X_ADDRESS_COUNTER,
                 x + dimensions.xClipStart);
  DMDIF_writeReg(DMD_SSD2119_SET_Y_ADDRESS_COUNTER,
                 y + dimensions.yClipStart);

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Set horizontal and vertical flip mode of display controller
*
*  @param hor
*  Set to flip display horizontally
*
*  @param ver
*  Set to flip display vertically
*
*  @return
*  Returns DMD_OK if successful, error otherwise.
******************************************************************************/
EMSTATUS DMD_flipDisplay(int horizontal, int vertical)
{
  uint16_t reg;

  reg = rcDriverOutputControl;

  if (horizontal) {
    reg &= ~DMD_SSD2119_DRIVER_OUTPUT_CONTROL_RL;
  } else {
    reg |= DMD_SSD2119_DRIVER_OUTPUT_CONTROL_RL;
  }

  if (vertical) {
    reg &= ~DMD_SSD2119_DRIVER_OUTPUT_CONTROL_TB;
  } else {
    reg |= DMD_SSD2119_DRIVER_OUTPUT_CONTROL_TB;
  }

  rcDriverOutputControl = reg;
  DMDIF_writeReg(DMD_SSD2119_DRIVER_OUTPUT_CONTROL, rcDriverOutputControl);

  return DMD_OK;
}

/***************************************************************************//**
 * @brief
 *    Get current framebuffer used by DMD for drawing (backbuffer).
 *
 * @param framebuffer
 *    Pointer to a framebuffer array.
 *    Gets set to DMD's current buffer.
 *
 * @return
 *    DMD_ERROR_NOT_SUPPORTED
 ******************************************************************************/
EMSTATUS DMD_getFrameBuffer(void **framebuffer)
{
  (void) framebuffer;
  return DMD_ERROR_NOT_SUPPORTED;
}

/** @endcond */
