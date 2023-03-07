/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "retargetserial.h"

#include "em_device.h"
#include "em_chip.h"
#include "bsp_trace.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "device_init.h"
#include "cslib.h"

void systemInit(void)
{
  CHIP_Init();

  // Ensure core frequency has been updated
  SystemCoreClockUpdate();
}
