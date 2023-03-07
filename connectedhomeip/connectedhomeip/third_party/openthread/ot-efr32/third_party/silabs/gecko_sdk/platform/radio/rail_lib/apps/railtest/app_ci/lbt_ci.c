/***************************************************************************//**
 * @file
 * @brief This file implements the LBT commands for RAILtest applications.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <string.h>
#if !defined(__ICCARM__)
// IAR doesn't have strings.h and puts those declarations in string.h
#include <strings.h>
#endif

#include "response_print.h"

#include "rail.h"
#include "app_common.h"

RailTxType_t txType = TX_TYPE_NORMAL;
RAIL_LbtConfig_t *lbtConfig = NULL;
RAIL_CsmaConfig_t *csmaConfig = NULL;

static RAIL_LbtConfig_t lbtParams = RAIL_CSMA_CONFIG_802_15_4_2003_2p4_GHz_OQPSK_CSMA;

void setLbtMode(sl_cli_command_arg_t *args)
{
  if (sl_cli_get_argument_count(args) >= 1) {
    if (strcasecmp(sl_cli_get_argument_string(args, 0), "off") == 0) {
      txType = TX_TYPE_NORMAL;
    } else if (strcasecmp(sl_cli_get_argument_string(args, 0), "csma") == 0) {
      txType = TX_TYPE_CSMA;
      csmaConfig = (RAIL_CsmaConfig_t*)&lbtParams;
    } else if (strcasecmp(sl_cli_get_argument_string(args, 0), "lbt") == 0) {
      txType = TX_TYPE_LBT;
      lbtConfig = &lbtParams; // Used for CSMA and LBT
    } else {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x70, "Unknown LBT mode specified.");
      return;
    }
  }
  if (txType == TX_TYPE_NORMAL) {
    responsePrint(sl_cli_get_command_string(args, 0), "LbtMode:off");
  } else if (txType == TX_TYPE_CSMA) {
    responsePrint(sl_cli_get_command_string(args, 0), "LbtMode:CSMA");
  } else if (txType == TX_TYPE_LBT) {
    responsePrint(sl_cli_get_command_string(args, 0), "LbtMode:LBT");
  }
}

void getLbtParams(sl_cli_command_arg_t *args)
{
  responsePrint(sl_cli_get_command_string(args, 0),
                "MinBo:%u,"
                "MaxBo:%u,"
                "Tries:%u,"
                "Threshold:%d,"
                "Backoff:%u,"
                "Duration:%u,"
                "Timeout:%u",
                lbtParams.lbtMinBoRand,
                lbtParams.lbtMaxBoRand,
                lbtParams.lbtTries,
                lbtParams.lbtThreshold,
                lbtParams.lbtBackoff,
                lbtParams.lbtDuration,
                lbtParams.lbtTimeout);
}

void setLbtParams(sl_cli_command_arg_t *args)
{
  lbtParams.lbtMinBoRand = sl_cli_get_argument_uint8(args, 0);
  lbtParams.lbtMaxBoRand = sl_cli_get_argument_uint8(args, 1);
  lbtParams.lbtTries     = sl_cli_get_argument_uint8(args, 2);
  lbtParams.lbtThreshold = sl_cli_get_argument_int8(args, 3);
  lbtParams.lbtBackoff   = sl_cli_get_argument_uint16(args, 4);
  lbtParams.lbtDuration  = sl_cli_get_argument_uint16(args, 5);
  lbtParams.lbtTimeout   = sl_cli_get_argument_uint32(args, 6);

  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getLbtParams(args);
}
