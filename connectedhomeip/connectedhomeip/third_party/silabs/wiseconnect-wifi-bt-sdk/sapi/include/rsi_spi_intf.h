/*******************************************************************************
* @file  rsi_spi_intf.h
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

#ifndef RSI_INTERFACE_H
#define RSI_INTERFACE_H

#ifndef EFM32_SDIO // This file is not needed for EFM32 board. In order to avoid compilation warnings, we excluded the below code for EFM32
#include "rsi_board_configuration.h"
#endif

/******************************************************
 * *                      Macros
 * ******************************************************/

// buffer full indication register value from module
#define RSI_BUFFER_FULL 0x01

// buffer empty indication register value from module
#define RSI_BUFFER_EMPTY 0x02

// RX packet pending register value from module
#define RSI_RX_PKT_PENDING 0x08

// Assertion Interrupt indication from module
#define RSI_ASSERT_INTR 0x80 // BIT(7)

#define RSI_POWER_SAVE 0x08

// offset of status in host descriptor
#define RSI_STATUS_OFFSET 12

// offset of status in host descriptor
#define RSI_TWOBYTE_STATUS_OFFSET 12

// offset of response type in host descriptor
#define RSI_RSP_TYPE_OFFSET 2

#ifndef RSI_HWTIMER
// need to define this macro if h/w timer is available
// and it should increment spiTimer2, spiTimer1
#define RSI_TICKS_PER_SECOND 50000
#else
#define RSI_TICKS_PER_SECOND 10
#endif

// SPI transfer type (8 BIT/32 BIT)
#define RSI_MODE_8BIT  0
#define RSI_MODE_32BIT 1

/*@ firmware upgradation timeout */
#define RSI_FWUPTIMEOUT 100 * RSI_TICKS_PER_SECOND
/*@ wireless firmware upgradation timeout */
#define RSI_WIRELESSFWUPTIMEOUT 500 * RSI_TICKS_PER_SECOND
/*@ bootloading timeout */
#define RSI_BLTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ band timeout */
#define RSI_BANDTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Init timeout */
#define RSI_INITTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Query firmware version timeout */
#define RSI_QFWVTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Set Mac address timeout */
#define RSI_SMATIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Scan timeout */
#define RSI_SCANTIMEOUT 12 * RSI_TICKS_PER_SECOND
/*@ Sleep timer timeout */
#define RSI_SLEEPTIMERTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Wepkeys timeout */
#define RSI_WEPKEYSTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Join timeout */
#define RSI_JOINTIMEOUT 12 * RSI_TICKS_PER_SECOND
/*@ Disconnect timeout */
#define RSI_DISCONTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Query connection status timeout */
#define RSI_QCSTIMEOUT 3 * RSI_TICKS_PER_SECOND
/*@ Query dhcp params timeout */
#define RSI_QDPTIMEOUT 3 * RSI_TICKS_PER_SECOND
/*@ Query network params timeout */
#define RSI_QNPTIMEOUT 3 * RSI_TICKS_PER_SECOND
/*@ Ip configuration timeout */
#define RSI_IPPTIMEOUT 6 * RSI_TICKS_PER_SECOND
/*@ Query RSSI Value timeout */
#define RSI_RSSITIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ recv timeout */
#define RSI_RECVTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Socket open timeout */
#define RSI_SOPTIMEOUT 6 * RSI_TICKS_PER_SECOND
/*@ Regread timeout */
#define RSI_REGREADTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Query DNS timeout */
#define RSI_QDNSTIMEOUT 6 * RSI_TICKS_PER_SECOND
/*@ Start token timeout */
#define RSI_START_TOKEN_TIMEOUT 10 * RSI_TICKS_PER_SECOND
/*@ Set Listen interval timeout */
#define RSI_SLITIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Config Enable timeout */
#define RSI_CETIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Config store timeout */
#define RSI_CSTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Config get timeout */
#define RSI_CGTIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Query BSSID/NW TYPE timeout */
#define RSI_QBSSIDNWTIMEOUT 6 * RSI_TICKS_PER_SECOND
#define RSI_QMACADDTIMEOUT  6 * RSI_TICKS_PER_SECOND
/*@ Query MAC ADDR timeout */
/*@ Get HTTP timeout */
#define RSI_GETHTTPTIMEOUT 40 * RSI_TICKS_PER_SECOND
/*@ Post HTTP timeout */
#define RSI_POSTHTTPTIMEOUT 6 * RSI_TICKS_PER_SECOND
/*@ Mode select timeout */
#define RSI_MODESEL_TIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ Feature select timeout */
#define RSI_FEATSEL_TIMEOUT 1 * RSI_TICKS_PER_SECOND
/*@ WPS RESPONSE timeout */
#define RSI_WPSRSPTIMEOUT 120 * RSI_TICKS_PER_SECOND
/*@ Power save timeout*/
#define RSI_PWSAVE_TIMEOUT 5 * RSI_TICKS_PER_SECOND
/*@ Command response timeout */
#define RSI_RESPONSE_TIMEOUT(A)             \
  RSI_RESET_TIMER3;                         \
  while (rsi_check_pkt_irq() != RSI_TRUE) { \
    if (RSI_INC_TIMER_3 > A) {              \
      retval = -1;                          \
      break;                                \
    }                                       \
  }

#define RSI_BYTES_3 3
#define RSI_BYTES_2 2

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
#ifdef RSI_SPI_INTERFACE
//! host descriptor structure
typedef struct rsi_frame_desc_s {
  // Data frame body length. Bits 14:12=queue, 000 for data, Bits 11:0 are the length
  uint8_t frame_len_queue_no[2];
  // Frame type
  uint8_t frame_type;
  // Unused , set to 0x00
  uint8_t reserved[9];
  // Management frame descriptor response status, 0x00=success, else error
  uint8_t status;

  uint8_t reserved1[3];
} rsi_frame_desc_t;
#endif
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
extern uint32_t rsi_spiTimer1, rsi_spiTimer2, rsi_spiTimer3;
/******************************************************
 * *               Function Declarations
 * ******************************************************/
extern int16_t rsi_module_power_cycle(void);
extern void rsi_build_frame_descriptor(rsi_frame_desc_t *uFrameDscFrame, uint8_t *cmd);
extern int16_t rsi_frame_write(rsi_frame_desc_t *uFrameDscFrame, uint8_t *payloadparam, uint16_t size_param);
extern int16_t rsi_frame_read(uint8_t *pkt_buffer);
extern int16_t rsi_spi_frame_dsc_wr(rsi_frame_desc_t *uFrameDscFrame);
extern int16_t rsi_spi_frame_data_wr(uint16_t bufLen, uint8_t *dBuf, uint16_t tbufLen, uint8_t *tBuf);
#endif
