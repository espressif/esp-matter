/*******************************************************************************
* @file  rsi_spi_frame_rd_wr.c
* @brief
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
/**
 * @file     rsi_spi_frame_rd_wr.c
 * @version  0.1
 * @date     15 Aug 2015
 *
 *
 *
 * @brief SPI Control: Control SPI frame read/write functions
 *
 * Description
 * SPI function to read/write management descriptors frames to/from the Wi-Fi module
 *
 * Improvements
 * Function header is modified for frameDscRd() function
 */

/*
 * Includes
 */
#include "rsi_driver.h"

#if ((defined DEBUG_PACKET_EXCHANGE) && (defined RSI_DEBUG_PRINTS))
#include <stdio.h>
#define MAX_PRINT_PAYLOAD_LEN 8
#define DEBUG_OUTPUT_SZ       (80 + MAX_PRINT_PAYLOAD_LEN * 2)
char debug_output[DEBUG_OUTPUT_SZ];
#endif

#ifdef RSI_SPI_INTERFACE
/*
  Global Variables
 */

/** @} */
/** @addtogroup DRIVER2
* @{
*/

/*====================================================*/
/**
 * @brief       Reads response for every command and data from the module. 
 * @param[in]   pkt_buffer  - pointer to buffer to which packet has to read 
 * @return       0 - SUCCESS \n
 *             < 0 - FAILURE \n
 *              -1 - SPI busy / Timeout in case of SPI \n
 *              -2 - SPI Failure in case of SPI
 * @note       Enable DEBUG_PACKET_EXCHANGE macro for spi level packet exchange debug prints and \n
 *             MAX_PRINT_PAYLOAD_LEN for configuring no.bytes of payload to print, by default it will print 8 bytes of payload. 
 *             
 */
int16_t rsi_frame_read(uint8_t *pkt_buffer)
{

  int16_t retval;
  uint8_t local_buffer[8];
  // Read first 4 bytes
  retval = rsi_pre_dsc_rd(&local_buffer[0]);
  if (retval != RSI_SUCCESS) {
    return retval;
  }
  // Read complete RX packet
  retval = rsi_pkt_rd(pkt_buffer,
                      ((rsi_bytes2R_to_uint16(&local_buffer[2])) - 4),
                      ((rsi_bytes2R_to_uint16(&local_buffer[0])) - 4));
  if (retval != RSI_SUCCESS) {
    return retval;
  }

#ifdef DEBUG_PACKET_EXCHANGE
  uint16_t size = rsi_bytes2R_to_uint16(pkt_buffer) & 0x0FFF;
  memset(debug_output, '\0', sizeof(debug_output));
  sprintf(debug_output, "[RD(%d)]", rsi_hal_gettickcount());
  for (uint32_t i = 0; i < 16; i++) {
    sprintf(debug_output, "%s%.2x", debug_output, pkt_buffer[i]);
  }
  if (size) {
    sprintf(debug_output, "%s | ", debug_output);
    // Print max of 8 bytes of payload
    for (uint32_t i = 0; i < size && i < MAX_PRINT_PAYLOAD_LEN; i++) {
      sprintf(debug_output, "%s%.2x", debug_output, pkt_buffer[16 + i]);
    }
  }
  LOG_PRINT("%s\r\n", debug_output);
#endif
  return retval;
}

/*====================================================*/
/**
 * @brief       Process a command to the wlan module.
 * @param[in]   uFrameDscFrame  -  Frame descriptor
 * @param[in]   payloadparam    -  Pointer to the command payload parameter structure
 * @param[in]   size_param      -  Size of the payload for the command
 * @return       0              - SUCCESS \n
 *             < 0              - FAILURE \n
 *              -1              - SPI busy / Timeout in case of SPI \n
 *              -2              - SPI Failure in case of SPI
 * @note       Enable DEBUG_PACKET_EXCHANGE macro for spi level packet exchange debug prints and \n
 *             MAX_PRINT_PAYLOAD_LEN for configuring no.bytes of payload to print, by default it will print 8 bytes of payload..             
 *
 */
int16_t rsi_frame_write(rsi_frame_desc_t *uFrameDscFrame, uint8_t *payloadparam, uint16_t size_param)
{
  int16_t retval;

#ifdef DEBUG_PACKET_EXCHANGE
  memset(debug_output, '\0', sizeof(debug_output));
  sprintf(debug_output, "[WR(%d)]", rsi_hal_gettickcount());
  for (uint32_t i = 0; i < 16; i++) {
    sprintf(debug_output, "%s%.2x", debug_output, ((char *)(uFrameDscFrame))[i]);
  }
  if (size_param) {
    sprintf(debug_output, "%s | ", debug_output);
    // Print max of 8 bytes of payload
    for (uint32_t i = 0; i < size_param && i < MAX_PRINT_PAYLOAD_LEN; i++) {
      sprintf(debug_output, "%s%.2x", debug_output, payloadparam[i]);
    }
  }
  LOG_PRINT("%s\r\n", debug_output);
#endif
  // Write host descriptor
  retval = rsi_spi_frame_dsc_wr(uFrameDscFrame);
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  // Write payload if present
  if (size_param) {
    // 4 byte align for payload size
    size_param = (size_param + 3) & ~3;
    retval     = rsi_spi_frame_data_wr(size_param, payloadparam, 0, NULL);
    if (retval != RSI_SUCCESS) {
      return retval;
    }
  }
  return retval;
}

/*===========================================================================*/
/**
 * @brief       Perform a pre-frame decriptor read.
 * @param[in]   dbuf - Pointer to the buffer into which pre-decriptor has to be read
 * @return       0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
 *
 */
int16_t rsi_pre_dsc_rd(uint8_t *dbuf)
{
  int16_t retval;
  uint8_t c1;
  uint8_t c2;
  uint8_t c3;
  uint8_t c4;

  c1 = RSI_C1FRMRD16BIT4BYTE;
#ifdef RSI_BIT_32_SUPPORT
  c2 = RSI_C2SPIADDR4BYTE;
#else
  c2 = RSI_C2MEMRDWRNOCARE;
#endif
  // Command frame response descriptor
  c3 = 0x04;

  // Upper byte of transfer length
  c4 = 0x00;

  // Send C1/C2
  retval = rsi_send_c1c2(c1, c2);
  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for the SPI to get ready
    return retval;
  }

  // Send C3/C4
  retval = rsi_send_c3c4(c3, c4);
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  // Wait for start token
  retval = rsi_spi_wait_start_token(RSI_START_TOKEN_TIMEOUT, RSI_MODE_32BIT);
  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for the SPI to get ready
    return retval;
  }

  // SPI read after start token
  retval = rsi_spi_transfer(NULL, (uint8_t *)dbuf, 0x4, RSI_MODE_32BIT);
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  return retval;
}

/*===========================================================================*/
/**
 * @brief       Perform frame decriptor and payload read.
 * @param[in]   buf       - Pointer to the buffer into which  decriptor and payload has to be read
 * @param[in]   dummy_len - Number of dummy bytes which can be discarded
 * @param[in]   total_len - Number of bytes to be read
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \nrsi_spi_frame_data_wr
 *              -2 - SPI Failure
 *
 */

int16_t rsi_pkt_rd(uint8_t *buf, uint16_t dummy_len, uint16_t total_len)
{
  int16_t retval;
  uint8_t c1;
  uint8_t c2;
  uint8_t c3;
  uint8_t c4;
#ifdef SAPIS_BT_STACK_ON_HOST
  uint8_t dummy_buf[150];
#else
  uint8_t dummy_buf[8];
#endif
  uint32_t aligned_len = 0;

  aligned_len = ((total_len) + 3) & ~3;

  c1 = RSI_C1FRMRD16BIT1BYTE;
#ifdef RSI_BIT_32_SUPPORT
  c2 = RSI_C2SPIADDR1BYTE;
#else
  c2 = RSI_C2MEMRDWRNOCARE;
#endif
  // Command frame response descriptor
  c3 = aligned_len & 0xff;

  // Upper byte of transfer length
  c4 = (aligned_len & 0xff00) >> 8;

  // Send C1/C2
  retval = rsi_send_c1c2(c1, c2);
  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for the SPI to get ready
    return retval;
  }

  // Send C3/C4
  retval = rsi_send_c3c4(c3, c4);
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  // Wait for start token
  retval = rsi_spi_wait_start_token(RSI_START_TOKEN_TIMEOUT, RSI_MODE_32BIT);
  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for the SPI to get ready
    return retval;
  }

  if (dummy_len) {
    retval = rsi_spi_transfer(NULL, (uint8_t *)&dummy_buf[0], dummy_len, RSI_MODE_8BIT);
    if (retval != RSI_SUCCESS) {
      return retval;
    }
  }

  // Actual spi read for descriptor and payload
  if (buf) {
    retval = rsi_spi_transfer(NULL, buf, (aligned_len - dummy_len), RSI_MODE_32BIT);
    if (retval != RSI_SUCCESS) {
      return retval;
    }
  }

  return retval;
}

/*===========================================================================*/
/**
 * @brief       Write a Frame descriptor.
 * @param[in]   uFrameDscFrame - Frame descriptor
 * @return      0  - SUCCESS \n
 *              -1  - SPI busy / Timeout \n
 *              -2  - SPI Failure
 *
 */
int16_t rsi_spi_frame_dsc_wr(rsi_frame_desc_t *uFrameDscFrame)
{
  int16_t retval;
  uint8_t localBuf[16];
  uint8_t c1;
  uint8_t c2;
  uint8_t c3;
  uint8_t c4;

  c1 = RSI_C1FRMWR16BIT4BYTE;
#ifdef RSI_BIT_32_SUPPORT
  c2 = RSI_C2RDWR4BYTE;
#else
  c2 = RSI_C2RDWR1BYTE;
#endif
  // Frame descriptor is 16 bytes long
  c3 = RSI_FRAME_DESC_LEN;

  // Upper byte of transfer length
  c4 = 0x00;

  // Send C1/C2
  retval = rsi_send_c1c2(c1, c2);
  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for the SPI to get ready
    return retval;
  }

  // Send C3/C4
  retval = rsi_send_c3c4(c3, c4);
  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for the SPI to get ready
    return retval;
  }
  // SPI send
  retval = rsi_spi_transfer(uFrameDscFrame->frame_len_queue_no, localBuf, RSI_FRAME_DESC_LEN, RSI_MODE_32BIT);
  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for the SPI to get ready
    return retval;
  }

  return retval;
}

/*===========================================================================*/
/**
 * @brief       Perform Frame Data Write.
 * @param[in]   buflen       -   Length of the data buffer to write
 * @param[in]   dBuf         -   Pointer to the buffer of data to write
 * @param[in]   tbuflen      -   Length of the data fragment to write
 * @param[in]   tBuf         -   Pointer to the buffer of data fragment to write
 * @return      0 - SUCCESS \n
 *              -1 - SPI busy / Timeout \n
 *              -2 - SPI Failure
 *
 */
int16_t rsi_spi_frame_data_wr(uint16_t bufLen, uint8_t *dBuf, uint16_t tbufLen, uint8_t *tBuf)
{
  int16_t retval;
  uint8_t c1;
  uint8_t c2;
  uint8_t c3;
  uint8_t c4;
  uint16_t tempbufLen;
  tempbufLen = bufLen + tbufLen;

  c1 = RSI_C1FRMWR16BIT4BYTE;
#ifdef RSI_BIT_32_SUPPORT
  c2 = RSI_C2RDWR4BYTE;
#else
  c2 = RSI_C2RDWR1BYTE;
#endif
  // Lower byte of transfer length
  c3 = (uint8_t)(tempbufLen & 0x00ff);

  // Upper byte of transfer length
  c4 = (uint8_t)((tempbufLen >> 8) & 0x00FF);

  // Send C1/C2
  retval = rsi_send_c1c2(c1, c2);
  if (retval != RSI_SUCCESS) {
    // Exit with error if timed out waiting for the SPI to get ready
    return retval;
  }

  // Send C3/C4
  retval = rsi_send_c3c4(c3, c4);
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  // SPI send
  retval = rsi_spi_transfer(dBuf, NULL, bufLen, RSI_MODE_32BIT);
  if (retval != RSI_SUCCESS) {
    return retval;
  }
  if (tbufLen) {
    retval = rsi_spi_transfer(tBuf, NULL, tbufLen, RSI_MODE_32BIT);
    if (retval != RSI_SUCCESS) {
      return retval;
    }
  }

  return retval;
}

#endif
/** @} */
