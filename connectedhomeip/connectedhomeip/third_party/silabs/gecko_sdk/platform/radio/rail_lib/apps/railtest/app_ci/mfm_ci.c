/***************************************************************************//**
 * @file
 * @brief This file implements commands for configuring MFM RAIL options
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

#include <stdio.h>
#include <string.h>
#include "em_core.h"
#include "response_print.h"
#include "buffer_pool_allocator.h"
#include "circular_queue.h"
#include "rail.h"
#include "rail_features.h"
#include "rail_mfm.h"
#include "app_common.h"

#if RAIL_SUPPORTS_MFM
#define MFM_RAW_BUF_SZ_BYTES 512

typedef struct RAIL_MFM_Config_App {
  RAIL_MFM_PingPongBufferConfig_t buffer;
  RAIL_StateTiming_t timings;
} RAIL_MFM_Config_App_t;

RAIL_MFM_Config_App_t mfmConfig = {
  .buffer = {
    .pBuffer0 = (&channelHoppingBufferSpace[0]),
    .pBuffer1 = (&channelHoppingBufferSpace[MFM_RAW_BUF_SZ_BYTES / 4]),
    .bufferSizeWords = (MFM_RAW_BUF_SZ_BYTES / 4)
  },
  .timings = {
    .idleToTx = 100,
    .idleToRx = 0,
    .rxToTx = 0,
    .txToRx = 0,
    .rxSearchTimeout = 0,
    .txToRxSearchTimeout = 0
  }
};

/******************* Local functions *******************/
bool RAIL_MFM_IsEnabled(RAIL_Handle_t railHandle)
{
  (void) railHandle;

  return (railDataConfig.txSource == TX_MFM_DATA);
}

RAIL_Status_t RAIL_MFM_Init(RAIL_Handle_t railHandle,
                            RAIL_MFM_Config_App_t *config)
{
  RAIL_Status_t status;

  railDataConfig.txSource = TX_MFM_DATA;
  status = RAIL_SetMfmPingPongFifo(railHandle,
                                   &(config->buffer));
  if (status != RAIL_STATUS_NO_ERROR) {
    return (status);
  }

  status = RAIL_ConfigData(railHandle, &railDataConfig);
  if (status != RAIL_STATUS_NO_ERROR) {
    return (status);
  }

  return (RAIL_SetStateTiming(railHandle, &(config->timings)));
}

RAIL_Status_t RAIL_MFM_Deinit(RAIL_Handle_t railHandle)
{
  if (!RAIL_MFM_IsEnabled(railHandle)) {
    return RAIL_STATUS_INVALID_STATE;
  }

  RAIL_Status_t status;
  status = RAIL_StopTx(railHandle, RAIL_STOP_MODES_ALL);
  if (status != RAIL_STATUS_NO_ERROR) {
    return (status);
  }

  railDataConfig.txSource = TX_PACKET_DATA;
  return (RAIL_ConfigData(railHandle, &railDataConfig));
}

void mfmStatus(sl_cli_command_arg_t *args)
{
  bool enabled = RAIL_MFM_IsEnabled(railHandle);

  // Report the current enabled status for MFM
  responsePrint(sl_cli_get_command_string(args, 0),
                "MFM:%s",
                enabled ? "Enabled" : "Disabled");
}

void mfmEnable(sl_cli_command_arg_t *args)
{
  if (sl_cli_get_argument_count(args) >= 1) {
    bool enable = !!sl_cli_get_argument_uint8(args, 0);

    // Turn MFM mode on or off as requested
    if (enable) {
      uint32_t idx;
      uint32_t *pDst0 = mfmConfig.buffer.pBuffer0;
      uint32_t *pDst1 = mfmConfig.buffer.pBuffer1;

      disableIncompatibleProtocols(RAIL_PTI_PROTOCOL_CUSTOM);
      RAIL_MFM_Init(railHandle, &mfmConfig);

      for (idx = 0; idx < (MFM_RAW_BUF_SZ_BYTES / 16); idx++) {
        pDst0[4 * idx + 0] = 0x755A3100;
        pDst1[4 * idx + 0] = 0x755A3100;
        pDst0[4 * idx + 1] = 0x315A757F;
        pDst1[4 * idx + 1] = 0x315A757F;
        pDst0[4 * idx + 2] = 0x8BA6CF00;
        pDst1[4 * idx + 2] = 0x8BA6CF00;
        pDst0[4 * idx + 3] = 0xCFA68B81;
        pDst1[4 * idx + 3] = 0xCFA68B81;
      }
    } else {
      RAIL_MFM_Deinit(railHandle);
    }
  }
  // Report the current status of MFM mode
  mfmStatus(args);
}

#else //!RAIL_SUPPORTS_MFM

void mfmNotSupported(int argc, char **argv)
{
  (void)argc;
  responsePrintError(argv[0], 0x56, "MFM is not suppported on this chip");
}

void mfmStatus(int argc, char **argv)
{
  mfmNotSupported(argc, argv);
}

void mfmEnable(int argc, char **argv)
{
  mfmNotSupported(argc, argv);
}

#endif //RAIL_SUPPORTS_MFM
