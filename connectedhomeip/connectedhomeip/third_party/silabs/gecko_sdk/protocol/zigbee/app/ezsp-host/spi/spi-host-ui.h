/***************************************************************************//**
 * @file
 * @brief Additional header for EZSP SPI Host user interface functions
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
 * See spi-host-ui.h.
 *
 *@{
 */

#ifndef SILABS_SPI_HOST_UI_H
#define SILABS_SPI_HOST_UI_H

static const char usage[] =
  " {options}\n"
  "  options:\n"
  "    -h                display usage information\n"
  "    -t <trace flags>  trace B0=frames, B1=verbose frames, B2=events, B3=EZSP\n"
  "    -d <OTA dir>      sets the directory to search OTA files.\n"
  "                      NOTE: Option is useful only when zigbee OTA storage\n"
  "                      posix filesystem component is present.\n";

bool ezspInternalProcessCommandOptions(int argc, char *argv[], char *errStr);
#endif //__SPI_HOST_UI_H___

/** @} END addtogroup
 */
