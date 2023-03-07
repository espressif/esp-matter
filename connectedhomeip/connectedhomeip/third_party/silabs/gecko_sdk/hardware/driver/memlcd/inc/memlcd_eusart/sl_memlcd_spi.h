/***************************************************************************//**
 * @file
 * @brief SPI abstraction used by memory lcd display
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
#ifndef __SL_MEMLCD_SPI_H_
#define __SL_MEMLCD_SPI_H_

#include "sl_status.h"
#include "em_eusart.h"
#include "em_cmu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SL_MEMLCD_USE_EUSART        1

#define SL_MEMLCD_SPI_VALUE_NONE 0xFF

/***************************************************************************//**
 * @addtogroup memlcd
 * @{
 ******************************************************************************/
typedef struct {
  EUSART_TypeDef *eusart;
  CMU_Clock_TypeDef clock;
  uint8_t mosi_port;
  uint8_t mosi_pin;
  uint8_t miso_port;
  uint8_t miso_pin;
  uint8_t sclk_port;
  uint8_t sclk_pin;
  uint8_t mosi_loc;
  uint8_t miso_loc;
  uint8_t clk_loc;
} sli_memlcd_spi_handle_t;

/***************************************************************************//**
 * @brief
 *   Initialize the SPI interface.
 *
 * @param[in] handle
 *   Handle to the SPI interface.
 *
 * @param[in] bitrate
 *   Requested frequency of the SPI interface. This will determine the speed
 *   of transfers.
 *
 * @param[in] mode
 *   This parameter determines the clock phase and clock polarity.
 *
 * @return
 *   If all operations completed sucessfully SL_STATUS_OK is returned. On
 *   failure a different status code is returned specifying the error.
 *****************************************************************************/
sl_status_t sli_memlcd_spi_init(sli_memlcd_spi_handle_t *handle, int bitrate, EUSART_ClockMode_TypeDef mode);

/***************************************************************************//**
 * @brief
 *   Shutdown the SPI interface.
 *
 * @detail
 *   Note that this function will also disable the clock to the SPI inteface.
 *   So this function should not be called if there are multiple users of the
 *   same SPI interface.
 *
 * @param[in] handle
 *   Handle to the SPI interface.
 *
 * @return
 *   If all operations completed sucessfully SL_STATUS_OK is returned. On
 *   failure a different status code is returned specifying the error.
 *****************************************************************************/
sl_status_t sli_memlcd_spi_shutdown(sli_memlcd_spi_handle_t *handle);

/***************************************************************************//**
 * @brief
 *   Transmit data on the SPI interface connected to a display.
 *
 * @param[in] handle
 *   Handle to the SPI interface.
 *
 * @param[in] data
 *   Pointer to the data to be transmitted.
 *
 * @param[in] len
 *   Length of data to transmit.
 *
 * @return
 *   If all operations completed sucessfully SL_STATUS_OK is returned. On
 *   failure a different status code is returned specifying the error.
 *****************************************************************************/
sl_status_t sli_memlcd_spi_tx(sli_memlcd_spi_handle_t *handle, const void *data, unsigned len);

/***************************************************************************//**
 * @brief
 *   Wait for the SPI interface to complete all the pending transfers.
 *
 * @param[in] handle
 *   Handle to the SPI interface.
 *****************************************************************************/
void sli_memlcd_spi_wait(sli_memlcd_spi_handle_t *handle);

/***************************************************************************//**
 * @brief
 *   Empty RX FIFO.
 *
 * @param[in] handle
 *   Handle to the SPI interface.
 *****************************************************************************/
void sli_memlcd_spi_rx_flush(sli_memlcd_spi_handle_t *handle);

/***************************************************************************//**
 * @brief
 *  Prepare SPI Interface after wakeup from EM23.
 *
 * @param[in] handle
 *   Handle to the SPI interface.
 *****************************************************************************/
sl_status_t sli_memlcd_spi_exit_em23(sli_memlcd_spi_handle_t *handle);

/***************************************************************************//**
 * @brief
 *  Prepare SPI Interface before transition to EM23.
 *
 * @param[in] handle
 *   Handle to the SPI interface.
 *****************************************************************************/
sl_status_t sli_memlcd_spi_enter_em23(sli_memlcd_spi_handle_t *handle);

/** @} */
#ifdef __cplusplus
}
#endif

#endif // __SL_MEMLCD_SPI_H_
