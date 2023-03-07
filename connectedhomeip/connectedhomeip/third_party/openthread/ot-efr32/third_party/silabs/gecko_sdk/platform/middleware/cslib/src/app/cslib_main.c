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

#include "em_device.h"
#include "em_chip.h"
#if defined(_EMU_DCDCCTRL_MASK)
#include "em_emu.h"
#include "bspconfig.h"
#endif
#include "cslib.h"
#include "device_init.h"
#include "comm_routines.h"

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
#if defined(_EMU_DCDCCTRL_MASK)
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_WSTK_DEFAULT;
#endif

  CHIP_Init();

#if defined(_EMU_DCDCCTRL_MASK)
  /* Init DCDC regulator with kit specific parameters. */
  EMU_DCDCInit(&dcdcInit);
#endif

  systemInit();

  CSLIB_commInit();

  CSLIB_initHardware();
  CSLIB_initLibrary();

  // Infinite loop
  while (1) {
    CSLIB_update();
    CSLIB_commUpdate();
    CSLIB_lowPowerUpdate();
  }
}
