/***************************************************************************//**
 * @file
 * @brief Header for SPI Host functions
 *
 * See @ref spi_util for documentation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

/** @addtogroup spi_util
 *
 * See spi-host.h.
 *
 *@{
 */

#ifndef SILABS_SPI_HOST_H
#define SILABS_SPI_HOST_H
#include "../ezsp-host-common.h"

#define SPI_MAX_TIMEOUTS          6   /*!< timeouts before link is judged down */

#define SPI_PORT_LEN              40  /*!< length of serial port name string */

// Bits in traceFlags to enable various host trace outputs
#define TRACE_FRAMES_BASIC        1   /*!< frames sent and received */
#define TRACE_FRAMES_VERBOSE      2   /*!< basic frames + internal variables */
#define TRACE_EVENTS              4   /*!< events */
#define TRACE_EZSP                8   /*!< EZSP commands, responses and callbacks */
#define TRACE_EZSP_VERBOSE        16  /*!< additional EZSP information */

// resetMethod values
#define SPI_RESET_METHOD_RST      0   /*!< send RST frame */
#define SPI_RESET_METHOD_NONE     3   /*!< no reset - for testing */

#define spiReadConfig(member) \
  (spiHostConfig.member)

#define spiReadConfigOrDefault(member, defval) \
  (spiHostConfig.member)

#define spiWriteConfig(member, value) \
  do { spiHostConfig.member = value; } while (0)

extern EzspHostConfig spiHostConfig;
extern bool ncpSleepEnabled;

#endif //__SPI_HOST_H___

/** @} END addtogroup
 */
