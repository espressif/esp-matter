/***************************************************************************//**
 * @file
 * @brief Hardware driver layer for simple SPI transactions.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef BTL_DRIVER_SPI_H
#define BTL_DRIVER_SPI_H

/**
 * @addtogroup Driver
 * @{
 * @addtogroup Spi SPI
 * @brief Basic Serial Peripheral Interface Driver
 * @details Simple, blocking SPI controller implementation for communication with
 *          external devices.
 * @{
 */

/**
 * Initialize a USART peripheral for SPI.
 */
void spi_init(void);

/**
 * De-initialize a USART peripheral for SPI.
 */
void spi_deinit(void);

/**
 * Write a single byte discarding the received byte.
 *
 * @param[in] data The byte to send
 */
void spi_writeByte(uint8_t data);

/**
 * Write two bytes discarding the received bytes.
 *
 * @param[in] data The bytes to send, most significant byte first
 */
void spi_writeHalfword(uint16_t data);

/**
 * Write three bytes discarding the received bytes.
 *
 * @param[in] data The bytes to send, most significant byte first
 */
void spi_write3Byte(uint32_t data);

/**
 * Read a byte by sending a 0xFF byte.
 *
 * @return The received byte
 */
uint8_t spi_readByte(void);

/**
 * Read two bytes by sending two 0xFF bytes.
 *
 * @return The received bytes, most significant byte first
 */
uint16_t spi_readHalfword(void);

/**
 * Assert the peripheral select line. Polarity is configured by @ref spi_init.
 */
void spi_setCsActive(void);

/**
 * Dessert the peripheral select line. Polarity is configured by @ref spi_init.
 */
void spi_setCsInactive(void);

/**
 * Get PPUSATD word of the (E)USART in use.
 *
 * @param[out] ppusatdNr PPUSATD register number
 *
 * @return The PPUSATD word of the (E)USART in use
 */
uint32_t spi_getUsartPPUSATD(uint32_t *ppusatdNr);

/**
 * @} // addtogroup Spi
 * @} // addtogroup Driver
 */

#endif // BTL_DRIVER_SPI_H
