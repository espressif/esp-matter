/***************************************************************************//**
 * @file
 * @brief Header for EZSP Host user interface functions
 *
 * See @ref ezsp_util for documentation.
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

/** @addtogroup ezsp_util
 *
 * See ezsp-host-ui.h.
 *
 *@{
 */

#ifndef SILABS_EZSP_HOST_UI_H
#define SILABS_EZSP_HOST_UI_H

/** @brief Prints usage instructions to stderr.
 *
 * @param name  program name (usually argv[0])
 */
void ezspPrintUsage(char *name);

/** @brief Sets host configuration values from command line options.
 *
 * @param argc  number of command line tokens
 *
 * @param argv  array of pointer to command line tokens
 *
 * @return  true if no errors were detected in the command line
 */
bool ezspProcessCommandOptions(int argc, char *argv[]);

/** @brief Writes a debug trace message, if enabled.
 *
 * @param string  pointer to message string
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_NO_RX_DATA
 */
void ezspTraceEvent(const char *string);

/** @brief  Converts EZSP error code to a string.
 *
 * @param error  error or reset code (from hostError or ncpError)
 *
 * @return  pointer to the string
 */
const uint8_t* ezspErrorString(uint8_t error);

#ifdef EZSP_ASH
  #define BUMP_HOST_COUNTER(mbr) do { ashCount.mbr++; } while (0)
  #define ADD_HOST_COUNTER(op, mbr) do { ashCount.mbr += op; }  while (0)
  #include "app/ezsp-host/ash/ash-host.h"
  #include "app/ezsp-host/ash/ash-host-ui.h"
  #define readConfig(x) ashReadConfig(x)
#elif defined(EZSP_USB)
  #include "app/ezsp-host/usb/usb-host.h"
  #include "app/ezsp-host/usb/usb-host-ui.h"
  #define readConfig(x) usbReadConfig(x)
  #define BUMP_HOST_COUNTER(mbr)
  #define ADD_HOST_COUNTER(op, mbr)
#elif defined(EZSP_SPI)
  #include "app/ezsp-host/spi/spi-host.h"
  #include "app/ezsp-host/spi/spi-host-ui.h"
  #define readConfig(x) spiReadConfig(x)
  #define BUMP_HOST_COUNTER(mbr)
  #define ADD_HOST_COUNTER(op, mbr)
#elif defined(EZSP_CPC)
  #include "app/ezsp-host/cpc/cpc-host.h"
  #include "app/ezsp-host/cpc/cpc-host-ui.h"
  #define readConfig(x) 0
  #define BUMP_HOST_COUNTER(mbr)
  #define ADD_HOST_COUNTER(op, mbr)
#endif // EZSP_ASH || EZSP_USB || EZSP_SPI || EZSP_CPC

#endif //__EZSP_HOST_UI_H___

/** @} END addtogroup
 */
