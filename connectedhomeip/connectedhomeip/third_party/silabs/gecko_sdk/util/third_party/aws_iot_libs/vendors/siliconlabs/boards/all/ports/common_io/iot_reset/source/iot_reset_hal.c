/***************************************************************************//**
 * @file    iot_reset_hal.c
 * @brief   Silicon Labs implementation of Common I/O Reset API.
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

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

/* SDK emlib layer */
#include "em_core.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_rmu.h"
#include "em_emu.h"

/* FreeRTOS kernel layer */
#include "FreeRTOS.h"
#include "semphr.h"
#include "string.h"

/* Common I/O interface layer */
#include "iot_reset.h"

/*******************************************************************************
 *                            STATIC VARIABLES
 ******************************************************************************/

uint32_t ulResetCauseReg = 0;

/*******************************************************************************
 *                            iot_reset_reboot()
 ******************************************************************************/

/**
 * @brief    iot_reset_reboot is used to reboot the device.
 *
 * @param[in]   xResetBootFlag  flag to determine either to do cold-reset or warm-reset.
 *                              cold-reset means the device is restarted and does not keep
 *                              any blocks of the SOC powered on i.e. device is shutdown and rebooted),
 *                              and warm-reset means the device is restarted while keeping some of the SoC blocks
 *                              powered on through the reboot process.
 *                              For example warm-boot may keep the RAM contents valid after reset by keeping the power
 *                              on for RAM banks, while cold-boot will wipe off the contents.
 *                              One of the IotResetBootFlag_t value.
 */
void iot_reset_reboot(IotResetBootFlag_t xResetBootFlag)
{
  /* hard reset is not supported at the moment */
  (void) xResetBootFlag;

  /* trigger reset signal */
#ifndef AWS_TESTS
  NVIC_SystemReset();
#endif
}

/*******************************************************************************
 *                            iot_reset_shutdown()
 ******************************************************************************/

/**
 * @brief   iot_reset_shutdown is used to shutdown the device.
 *          If the target does not support shutdown of the device, IOT_RESET_FUNCTION_NOT_SUPPORTED
 *          is returned to the user.
 *
 * @return
 *   - does not return and device shutdown on success
 *   - IOT_RESET_FUNCTION_NOT_SUPPORTED if shutdown not supported.
 */
int32_t iot_reset_shutdown(void)
{
#ifndef AWS_TESTS
#if defined(_EMU_EM4CTRL_MASK)
  /* EM4 shutdown mode */
  EMU_EnterEM4S();
#else
  /* go to lowest energy mode */
  EMU_EnterEM4();
#endif
#endif

  /* if code execution reaches this line then we cannot enter EM4 */
  return IOT_RESET_FUNCTION_NOT_SUPPORTED;
}

/*******************************************************************************
 *                            iot_get_reset_reason()
 ******************************************************************************/

/**
 * @brief   iot_get_reset_reason is used to get the last reset reason.
 *          If the underlying HW does not support the feature of persisting the
 *          reset reason, then this API will return IOT_RESET_FUNCTION_NOT_SUPPORTED
 *          error with the value in out as don't care.
 *
 * @param[out]   xResetReason  One of the reset reasons specified in IotResetReason_t types
 *
 * @return
 *   - IOT_RESET_SUCCESS on success.
 *   - IOT_RESET_FUNCTION_NOT_SUPPORTED if not supported.
 *   - IOT_RESET_INVALID_VALUE if xREsetReason == NULL
 */
int32_t iot_get_reset_reason(IotResetReason_t * xResetReason)
{
  /* validate xResetReason */
  if (xResetReason == NULL) {
    return IOT_RESET_INVALID_VALUE;
  }

  /* read reset cause reg if not read before */
  if (ulResetCauseReg == 0) {
    ulResetCauseReg = RMU_ResetCauseGet();
    RMU_ResetCauseClear();
  }

  /* interpret reset cause */
  if (ulResetCauseReg == 0) {
    /* it is not possible to query reset cause */
    return IOT_RESET_FUNCTION_NOT_SUPPORTED;

#if defined(RMU_RSTCAUSE_PORST)
  } else if (ulResetCauseReg & RMU_RSTCAUSE_PORST) {
    /* RMU POWER-ON RESET */
    *xResetReason = eResetPowerOnBoot;
#endif
#if defined(EMU_RSTCAUSE_POR)
  } else if (ulResetCauseReg & EMU_RSTCAUSE_POR) {
    /* EMU POWER-ON RESET */
    *xResetReason = eResetPowerOnBoot;
#endif

#if defined(RMU_RSTCAUSE_SYSREQRST)
  } else if (ulResetCauseReg & RMU_RSTCAUSE_SYSREQRST) {
    /* RMU SYSREQRESET SIGNAL */
    *xResetReason = eResetWarmBoot;
#endif
#if defined(EMU_RSTCAUSE_SYSREQ)
  } else if (ulResetCauseReg & EMU_RSTCAUSE_SYSREQ) {
    /* EMU SYSREQRESET SIGNAL */
    *xResetReason = eResetWarmBoot;
#endif

#if defined(RMU_RSTCAUSE_EXTRST)
  } else if (ulResetCauseReg & RMU_RSTCAUSE_EXTRST) {
    /* RMU nRESET signal */
    *xResetReason = eResetColdBoot;
#endif
#if defined(EMU_RSTCAUSE_PIN)
  } else if (ulResetCauseReg & EMU_RSTCAUSE_PIN) {
    /* EMU nRESET signal */
    *xResetReason = eResetColdBoot;
#endif

#if defined(RMU_RSTCAUSE_WDOGRST)
  } else if (ulResetCauseReg & RMU_RSTCAUSE_WDOGRST) {
    /* RMU WDOG reset */
    *xResetReason = eResetWatchdog;
#endif
#if defined(EMU_RSTCAUSE_WDOG0)
  } else if (ulResetCauseReg & EMU_RSTCAUSE_WDOG0) {
    /* EMU WDOG0 reset */
    *xResetReason = eResetWatchdog;
#endif
#if defined(EMU_RSTCAUSE_WDOG1)
  } else if (ulResetCauseReg & EMU_RSTCAUSE_WDOG1) {
    /* EMU WDOG1 reset */
    *xResetReason = eResetWatchdog;
#endif

#if defined(RMU_RSTCAUSE_EM4RST)
  } else if (ulResetCauseReg & RMU_RSTCAUSE_EM4RST) {
    /* RMU EM4 reset */
    *xResetReason = eResetPmic;
#endif
#if defined(EMU_RSTCAUSE_EM4)
  } else if (ulResetCauseReg & EMU_RSTCAUSE_EM4) {
    /* EMU EM4 reset */
    *xResetReason = eResetPmic;
#endif

#if defined(RMU_RSTCAUSE_LOCKUPRST)
  } else if (ulResetCauseReg & RMU_RSTCAUSE_LOCKUPRST) {
    /* RMU lock up reset */
    *xResetReason = eResetBusTimeout;
#endif
#if defined(EMU_RSTCAUSE_LOCKUP)
  } else if (ulResetCauseReg & EMU_RSTCAUSE_LOCKUP) {
    /* EMU lock up reset */
    *xResetReason = eResetBusTimeout;
#endif

#if defined(RMU_RSTCAUSE_AVDDBOD)
  } else if (ulResetCauseReg & RMU_RSTCAUSE_AVDDBOD) {
    /* EMU AVDD brown-out reset */
    *xResetReason = eResetBrownOut;
#endif
#if defined(EMU_RSTCAUSE_AVDDBOD)
  } else if (ulResetCauseReg & EMU_RSTCAUSE_AVDDBOD) {
    /* EMU AVDD brown-out reset */
    *xResetReason = eResetBrownOut;
#endif
#if defined(EMU_RSTCAUSE_IOVDD0BOD)
  } else if (ulResetCauseReg & EMU_RSTCAUSE_IOVDD0BOD) {
    /* EMU IOVDD0 brown-out reset */
    *xResetReason = eResetBrownOut;
#endif
#if defined(RMU_RSTCAUSE_DVDDBOD)
  } else if (ulResetCauseReg & RMU_RSTCAUSE_DVDDBOD) {
    /* RMU DVDD brown-out reset */
    *xResetReason = eResetBrownOut;
#endif
#if defined(EMU_RSTCAUSE_DVDDBOD)
  } else if (ulResetCauseReg & EMU_RSTCAUSE_DVDDBOD) {
    /* EMU DVDD brown-out reset */
    *xResetReason = eResetBrownOut;
#endif
#if defined(EMU_RSTCAUSE_DVDDLEBOD)
  } else if (ulResetCauseReg & EMU_RSTCAUSE_DVDDLEBOD) {
    /* EMU DVDDLE brown-out reset */
    *xResetReason = eResetBrownOut;
#endif
#if defined(RMU_RSTCAUSE_DECBOD)
  } else if (ulResetCauseReg & RMU_RSTCAUSE_DECBOD) {
    /* RMU Decouple brown-out reset */
    *xResetReason = eResetBrownOut;
#endif
#if defined(EMU_RSTCAUSE_DECBOD)
  } else if (ulResetCauseReg & EMU_RSTCAUSE_DECBOD) {
    /* EMU Decouple brown-out reset */
    *xResetReason = eResetBrownOut;
#endif

  } else {
    /* unknown reason */
    *xResetReason = eResetOther;
  }

  /* done */
  return IOT_RESET_SUCCESS;
}
