/***************************************************************************//**
 * @file
 * @brief Header for EZSP over CPC functions
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SILABS_CPC_HOST_UI_H
#define SILABS_CPC_HOST_UI_H

static const char usage[] =
  "  options:\n"
  "    -h                       display usage information\n"
  "    -c <cpcd instance name>  use the specified instance name; e.g. -c cpcd_1\n"
  "                             default: cpcd_0\n"
  "    -t <trace flags>         trace B0=frames, B1=verbose frames, B2=events, B3=EZSP\n"
  "                             default: no tracing\n"
  "    -d <OTA dir>             sets the directory to search OTA files.\n"
  "                             NOTE: Option is useful only when zigbee OTA storage\n"
  "                             posix filesystem component is present.\n";
;

bool ezspInternalProcessCommandOptions(int argc, char *argv[], char *errStr);
#endif //SILABS_CPC_HOST_UI_H
