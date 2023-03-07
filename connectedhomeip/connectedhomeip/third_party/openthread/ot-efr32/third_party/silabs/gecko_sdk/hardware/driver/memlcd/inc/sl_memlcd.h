/***************************************************************************//**
 * @file
 * @brief Memory LCD interface
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_MEMLCD_H
#define SL_MEMLCD_H

#include "sl_status.h"

#include <stdbool.h>
#include <stdint.h>

#include "sl_memlcd_spi.h"
#if defined(SL_MEMLCD_USE_EUSART)
#include "sl_memlcd_eusart_config.h"
#elif defined(SL_MEMLCD_USE_USART)
#include "sl_memlcd_usart_config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SL_MEMLCD_COLOR_MODE_MONOCHROME          1
#define SL_MEMLCD_COLOR_MODE_RGB_3BIT            2

/***************************************************************************//**
 * @addtogroup memlcd MEMLCD - Memory LCD
 * @brief Memory LCD interface.
 *
 * @note When using the EUSART as the communication engine, care must be taken
 *       to ensure the EUSART is re-enabled when coming back from EM2/3.
 *       This is automatically handle by the Power Manager when added to a project.
 *       See EMLIB Eusart - EM2 guideline for non EM2-Capable instance
 * @{
 ******************************************************************************/

/**
 * General memory LCD data structure.
 */
typedef struct sl_memlcd_t {
  unsigned short width;       ///< Display pixel width
  unsigned short height;      ///< Display pixel height
  uint8_t bpp;                ///< Bits per pixel
  uint8_t color_mode;         ///< Color mode
  int spi_freq;               ///< SPI clock frequency
  uint8_t extcomin_freq;      ///< Extcomin pin toggle frequency
  uint8_t setup_us;           ///< SPI CS setup time
  uint8_t hold_us;            ///< SPI CS hold time
} sl_memlcd_t;

/**************************************************************************//**
 * @brief
 *   Configure the memory LCD device.
 *
 * @details
 *   This function must be called to configure the memory LCD device
 *   before starting to use the memory LCD.
 *
 * @param[in] device
 *   Display device pointer.
 *
 * @return
 *   status code of the operation.
 *****************************************************************************/
sl_status_t sl_memlcd_configure(struct sl_memlcd_t *device);

/**************************************************************************//**
 * @brief
 *   Enable or disable the display.
 *
 * @details
 *   This function enables or disables the display. Disabling the display does
 *   not make it lose its data. Note that this function will not control the
 *   DISP pin on the display. This pin is usually controlled by board specific
 *   code.
 *
 * @param[in] device
 *   Display device pointer.
 *
 * @param[in] on
 *   Set this parameter to 'true' to enable the display. Set to 'false' in
 *   order to disable the display.
 *
 * @return
 *   status code of the operation.
 *****************************************************************************/
sl_status_t sl_memlcd_power_on(const struct sl_memlcd_t *device, bool on);

/**************************************************************************//**
 * @brief
 *   Clear the display.
 *
 * @param[in] device
 *   Memory LCD display device.
 *
 * @return
 *   SL_STATUS_OK if there are no errors.
 *****************************************************************************/
sl_status_t sl_memlcd_clear(const struct sl_memlcd_t *device);

/**************************************************************************//**
 * @brief
 *   Draw a number of rows to the memory LCD display.
 *
 * @param[in] device
 *   Memory LCD display device.
 *
 * @param[in] data
 *   Pointer to the pixel matrix buffer to draw. The format of the buffer
 *   depends on the color mode of the memory LCD display.
 *
 * @param[in] row_start
 *   Start row on the display where to start drawing. First row is 0.
 *
 * @param[in] row_count
 *   Number of rows to draw.
 *
 * @return
 *   SL_STATUS_OK if there are no errors.
 *****************************************************************************/
sl_status_t sl_memlcd_draw(const struct sl_memlcd_t *device,
                           const void *data,
                           unsigned int row_start,
                           unsigned int row_count);

/**************************************************************************//**
 * @brief
 *   Refresh the display device.
 *
 * @details
 *   This function can be used when, for instance, the clock configuration was
 *   changed to re-initialize the SPI clock rate.
 *
 * @param[in] device
 *   Display device pointer.
 *
 * @return
 *   status code of the operation.
 *****************************************************************************/
sl_status_t sl_memlcd_refresh(const struct sl_memlcd_t *device);

/**************************************************************************//**
 * @brief
 *   Get a handle to the memory LCD.
 *
 * @return
 *   Pointer to a memory LCD structure or NULL if no memory LCD is initialized
 *   yet.
 *****************************************************************************/
const sl_memlcd_t *sl_memlcd_get(void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
