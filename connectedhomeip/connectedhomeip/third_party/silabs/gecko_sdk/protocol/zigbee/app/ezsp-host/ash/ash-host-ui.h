/***************************************************************************//**
 * @file
 * @brief Additional header for EZSP ASH Host user interface functions
 *
 * See @ref ash_util for documentation.
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

/** @addtogroup ash_util
 *
 * See ash-host-ui.h.
 *
 *@{
 */

#ifndef SILABS_ASH_HOST_UI_H
#define SILABS_ASH_HOST_UI_H

/** @brief  Prints host counter data.
 *
 * @param counters  pointer to counters structure
 *
 * @param clear     if true clears counters
 */
void ashPrintCounters(AshCount *counters, bool clear);

/** @brief  Clears host counter data.
 *
 * @param counters  pointer to counters structure
 */
void ashClearCounters(AshCount *counters);

static const char usage[] =
  " {ncp type} {options}\n"
  "  ncp defaults:\n"
  "    -n 0,1            0=NCP defaults for 115200 bps, RTS/CTS\n"
  "                      1=NCP defaults for 57600 bps, XON/XOFF\n"
  "                      (if present must be the first option)\n"
  "  options:\n"
  "    -b <baud rate>    9600, 19200, 38400, 57600, 115200, etc.\n"
  "    -f r,x            flow control: r=RST/CTS, x=XON/XOFF\n"
  "    -h                display usage information\n"
  "    -i 0,1            enable/disable input buffering\n"
  "    -o 0,1            enable/disable output buffering\n"
  "    -p <port>         serial port name or number (eg, COM1, ttyS0, or 1)\n"
  "    -r d,r,c          ncp reset method: d=DTR, r=RST frame, c=custom\n"
  "    -s 1,2            stop bits\n"
  "    -t <trace flags>  trace B0=frames, B1=verbose frames, B2=events, B3=EZSP\n"
  "    -v[base-port]     enables virtual ISA support.  The [base-port] argument\n"
  "                      is optional.  Both serial ports are available via telnet\n"
  "                      instead of local console.  RAW serial port is available\n"
  "                      on the first port (offset 0 from base port), and CLI is\n"
  "                      available on the second port (offset 1 from base port).\n"
  "                      By default, 4900 is the base-port, therefore RAW access\n"
  "                      is available from port 4900, and CLI access is available\n"
  "                      on port 4901.\n"
  "                      NOTE: No space is allowed between '-v' and [base-port].\n"
  "    -x 0,1            enable/disable data randomization\n"
  "    -d <OTA dir>      sets the directory to search OTA files.\n"
  "                      NOTE: Option is useful only when zigbee OTA storage\n"
  "                      posix filesystem component is present.\n";

static const AshCount zeroAshCount = { 0 };

bool ezspInternalProcessCommandOptions(int argc, char *argv[], char *errStr);
#endif //__ASH_HOST_UI_H___

/** @} END addtogroup
 */
